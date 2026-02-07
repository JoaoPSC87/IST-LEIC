#include "tlb.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "constants.h"
#include "log.h"
#include "memory.h"
#include "page_table.h"

typedef struct {
  bool valid;
  bool dirty;
  uint64_t last_access;
  va_t virtual_page_number;
  pa_dram_t physical_page_number;
} tlb_entry_t;

tlb_entry_t tlb_l1[TLB_L1_SIZE];
tlb_entry_t tlb_l2[TLB_L2_SIZE];

uint64_t tlb_l1_hits = 0;
uint64_t tlb_l1_misses = 0;
uint64_t tlb_l1_invalidations = 0;

uint64_t tlb_l2_hits = 0;
uint64_t tlb_l2_misses = 0;
uint64_t tlb_l2_invalidations = 0;

uint64_t get_total_tlb_l1_hits() { return tlb_l1_hits; }
uint64_t get_total_tlb_l1_misses() { return tlb_l1_misses; }
uint64_t get_total_tlb_l1_invalidations() { return tlb_l1_invalidations; }

uint64_t get_total_tlb_l2_hits() { return tlb_l2_hits; }
uint64_t get_total_tlb_l2_misses() { return tlb_l2_misses; }
uint64_t get_total_tlb_l2_invalidations() { return tlb_l2_invalidations; }

void tlb_init() {
  memset(tlb_l1, 0, sizeof(tlb_l1));
  memset(tlb_l2, 0, sizeof(tlb_l2));
  tlb_l1_hits = 0;
  tlb_l1_misses = 0;
  tlb_l1_invalidations = 0;
  tlb_l2_hits = 0;
  tlb_l2_misses = 0;
  tlb_l2_invalidations = 0;
}

// Helper function to find LRU entry in a cache
static int find_lru_entry(tlb_entry_t *cache, int size) {
  int lru_idx = 0;
  uint64_t oldest_time = cache[0].last_access;
  
  for (int i = 1; i < size; i++) {
    if (!cache[i].valid) {
      return i; // Return first invalid entry
    }
    if (cache[i].last_access < oldest_time) {
      oldest_time = cache[i].last_access;
      lru_idx = i;
    }
  }
  
  return lru_idx;
}

// Helper function to search for an entry in a cache
static int find_entry(tlb_entry_t *cache, int size, va_t vpn) {
  for (int i = 0; i < size; i++) {
    if (cache[i].valid && cache[i].virtual_page_number == vpn) {
      return i;
    }
  }
  return -1; // Not found
}

void tlb_invalidate(va_t virtual_page_number) {
  // Invalidate in L1
  int l1_idx = find_entry(tlb_l1, TLB_L1_SIZE, virtual_page_number);
  if (l1_idx != -1) {
    // Write back if dirty (write-back policy)
    if (tlb_l1[l1_idx].dirty) {
      write_back_tlb_entry(virtual_page_number);
    }
    tlb_l1[l1_idx].valid = false;
    tlb_l1_invalidations++;
    
  }
  increment_time(TLB_L1_LATENCY_NS);
  // Invalidate in L2
  int l2_idx = find_entry(tlb_l2, TLB_L2_SIZE, virtual_page_number);
  if (l2_idx != -1) {
    // Write back if dirty (write-back policy)
    if (tlb_l2[l2_idx].dirty) {
      write_back_tlb_entry(virtual_page_number);
    }
    tlb_l2[l2_idx].valid = false;
    tlb_l2_invalidations++;
    
  }
  increment_time(TLB_L2_LATENCY_NS);
}

pa_dram_t tlb_translate(va_t virtual_address, op_t op) {
    va_t vpn = virtual_address >> PAGE_SIZE_BITS;
    pa_dram_t offset = virtual_address & PAGE_OFFSET_MASK;

    // === L1 lookup ===
    int l1_idx = find_entry(tlb_l1, TLB_L1_SIZE, vpn);
    increment_time(TLB_L1_LATENCY_NS);  // L1 sempre custa 1ns

    if (l1_idx != -1) {
        tlb_l1_hits++;
        tlb_l1[l1_idx].last_access = get_time();
        if (op == OP_WRITE) {
            tlb_l1[l1_idx].dirty = true;
        }
        return (tlb_l1[l1_idx].physical_page_number << PAGE_SIZE_BITS) | offset;
    }

    // === L2 lookup ===
    tlb_l1_misses++;
    int l2_idx = find_entry(tlb_l2, TLB_L2_SIZE, vpn);

    pa_dram_t ppn;

    if (l2_idx != -1) {
        // L2 HIT
        tlb_l2_hits++;
        increment_time(TLB_L2_LATENCY_NS);
        tlb_l2[l2_idx].last_access = get_time();

        if (op == OP_WRITE) {
            tlb_l2[l2_idx].dirty = true;
        }

        ppn = tlb_l2[l2_idx].physical_page_number;

        // === PROMOVER para L1 ===
        int l1_victim_idx = find_lru_entry(tlb_l1, TLB_L1_SIZE);

        if (tlb_l1[l1_victim_idx].valid) {
            va_t victim_vpn = tlb_l1[l1_victim_idx].virtual_page_number;
            int existing_l2_idx = find_entry(tlb_l2, TLB_L2_SIZE, victim_vpn);

            if (existing_l2_idx != -1) {
                if (tlb_l1[l1_victim_idx].dirty) {
                    tlb_l2[existing_l2_idx].dirty = true;
                }
            } else {
                int l2_victim_idx = find_lru_entry(tlb_l2, TLB_L2_SIZE);
                if (tlb_l2[l2_victim_idx].valid && tlb_l2[l2_victim_idx].dirty) {
                    write_back_tlb_entry(tlb_l2[l2_victim_idx].physical_page_number << PAGE_SIZE_BITS);
                }

                tlb_l2[l2_victim_idx] = (tlb_entry_t){
                    .valid = true,
                    .dirty = tlb_l1[l1_victim_idx].dirty,
                    .virtual_page_number = victim_vpn,
                    .physical_page_number = tlb_l1[l1_victim_idx].physical_page_number,
                    .last_access = get_time()
                };
            }
        }

        tlb_l1[l1_victim_idx] = (tlb_entry_t){
            .valid = true,
            .dirty = tlb_l2[l2_idx].dirty,
            .virtual_page_number = vpn,
            .physical_page_number = ppn,
            .last_access = get_time()
        };

        return (ppn << PAGE_SIZE_BITS) | offset;
    }

    // === L2 MISS ===
    tlb_l2_misses++;
    increment_time(TLB_L2_LATENCY_NS);  

    pa_dram_t physical_address = page_table_translate(virtual_address, op);
    ppn = physical_address >> PAGE_SIZE_BITS;

    // Inserir no L2
    int l2_insert_idx = find_lru_entry(tlb_l2, TLB_L2_SIZE);

    if (tlb_l2[l2_insert_idx].valid && tlb_l2[l2_insert_idx].dirty) {
        write_back_tlb_entry(tlb_l2[l2_insert_idx].physical_page_number << PAGE_SIZE_BITS);
    }

    tlb_l2[l2_insert_idx] = (tlb_entry_t){
        .valid = true,
        .dirty = (op == OP_WRITE),
        .virtual_page_number = vpn,
        .physical_page_number = ppn,
        .last_access = get_time()
    };

    // Inserir no L1
    int l1_insert_idx = find_lru_entry(tlb_l1, TLB_L1_SIZE);

    if (tlb_l1[l1_insert_idx].valid) {
        va_t victim_vpn = tlb_l1[l1_insert_idx].virtual_page_number;
        int existing_l2_idx = find_entry(tlb_l2, TLB_L2_SIZE, victim_vpn);

        if (existing_l2_idx != -1) {
            if (tlb_l1[l1_insert_idx].dirty) {
                tlb_l2[existing_l2_idx].dirty = true;
            }
        } else {
            int l2_victim_idx = find_lru_entry(tlb_l2, TLB_L2_SIZE);
            if (tlb_l2[l2_victim_idx].valid && tlb_l2[l2_victim_idx].dirty) {
                write_back_tlb_entry(tlb_l2[l2_victim_idx].physical_page_number << PAGE_SIZE_BITS);
            }

            tlb_l2[l2_victim_idx] = (tlb_entry_t){
                .valid = true,
                .dirty = tlb_l1[l1_insert_idx].dirty,
                .virtual_page_number = victim_vpn,
                .physical_page_number = tlb_l1[l1_insert_idx].physical_page_number,
                .last_access = get_time()
            };
        }
    }

    tlb_l1[l1_insert_idx] = (tlb_entry_t){
        .valid = true,
        .dirty = (op == OP_WRITE),
        .virtual_page_number = vpn,
        .physical_page_number = ppn,
        .last_access = get_time()
    };

    return (ppn << PAGE_SIZE_BITS) | offset;
}
