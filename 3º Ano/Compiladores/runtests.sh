#!/bin/bash
# Corre todos os testes: compila, linka, executa e compara com o expected
cd "$(dirname "$0")"
TESTS=../co26-master/co26-master/auto-tests
RTS=$HOME/compiladores/root/usr/lib
pass=0; fail=0; failed=""

for src in $TESTS/*.p6; do
  name=$(basename "$src" .p6)
  ./p6 --target asm "$src" -o /tmp/$name.asm 2>/dev/null || { fail=$((fail+1)); failed="$failed $name(p6)"; continue; }
  yasm -felf32 /tmp/$name.asm -o /tmp/$name.o 2>/dev/null || { fail=$((fail+1)); failed="$failed $name(yasm)"; continue; }
  ld -melf_i386 -o /tmp/$name /tmp/$name.o -L$RTS -lrts 2>/dev/null || { fail=$((fail+1)); failed="$failed $name(ld)"; continue; }
  /tmp/$name > /tmp/$name.got 2>/dev/null
  if diff -iw /tmp/$name.got $TESTS/expected/$name.out >/dev/null 2>&1; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failed="$failed $name(out)"
  fi
done

echo "=== PASSOU: $pass | FALHOU: $fail ==="
[ -n "$failed" ] && echo "Falhas:$failed"