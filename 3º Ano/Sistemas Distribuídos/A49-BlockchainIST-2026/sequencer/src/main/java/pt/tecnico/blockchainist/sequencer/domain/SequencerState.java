package pt.tecnico.blockchainist.sequencer.domain;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.contract.Block;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.sequencer.SequencerKeyManager;


public class SequencerState {

    private final List<Block> blocks = new ArrayList<>();
    private final List<Transaction> pendingTransactions = new ArrayList<>();
    private final int maxTransactions;

    private final ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
    private ScheduledFuture<?> closeBlockFuture;

    private final List<StreamBlocksObserver> streamBlocksObservers = new CopyOnWriteArrayList<>();

    private final SequencerKeyManager keyManager;

    public int blockTimeout;

    public SequencerState(int maxTransactions, int blockTimeout) throws RuntimeException {
        this.maxTransactions = maxTransactions;
        this.blockTimeout = blockTimeout;
        try {
            this.keyManager = new SequencerKeyManager("Seq.priv");
        } catch (Exception e) {
            throw new RuntimeException("ERROR: Failed to load sequencer private key " + e.getMessage());
        }

        scheduleClose();
    }

    private void scheduleClose() {
        // Cancel the previous scheduled task if it exists
        if (closeBlockFuture != null && !closeBlockFuture.isDone()) {
            closeBlockFuture.cancel(false);
        }

        closeBlockFuture = scheduler.schedule(this::closeBlockIfNotEmpty, blockTimeout, TimeUnit.SECONDS);
    }

    public synchronized int addTransaction(Transaction tx) {
        pendingTransactions.add(tx);
        if (pendingTransactions.size() >= maxTransactions) {
            closeBlockIfNotEmpty();
        }
        return blocks.size() - (pendingTransactions.isEmpty() ? 1 : 0); // Devolve o numero do bloco onde a transação foi adicionada (ou o proximo bloco se ainda nao tiver sido fechado)
    }


    public synchronized Block getBlock(int sequenceNumber) {
        if (sequenceNumber < 0 || sequenceNumber >= blocks.size()) {
            return null;
        }
        return blocks.get(sequenceNumber);
    }
    
    public synchronized void closeBlockIfNotEmpty() {
        scheduleClose();
        if (!pendingTransactions.isEmpty()) {
            Block unsignedBlock = Block.newBuilder()
                .setSequenceNumber(blocks.size())
                .addAllTransactions(pendingTransactions)
                .build();
            try{
                Block block = keyManager.signBlock(unsignedBlock);
                blocks.add(block);
                streamBlocks(block); // Stream the new block to all subscribed observers
            } catch (Exception e) {
                System.err.println("Error signing block: " + e.getMessage());
            }
            pendingTransactions.clear();
        }
    }

    public synchronized void addStreamBlocksObserver(StreamObserver<Block> observer) {
        // Send all existings blocks and then subscribe to new ones
        for (Block block : blocks) {
            try {
                observer.onNext(block);
            } catch (Exception e) {
                // Handle any exceptions that may occur while streaming to observers
                System.err.println("Error streaming block to observer: " + e.getMessage());
                return; // Don't add the observer if we can't send existing blocks
            }
        }

        Block unsignedFlagBlock = Block.newBuilder().setSequenceNumber(-1).build();

        try {
            Block flagBlock = keyManager.signBlock(unsignedFlagBlock);
            observer.onNext(flagBlock); // Send a flag block to indicate the end of existing blocks
        } catch (Exception e) {
            System.err.println("Error streaming flag block to observer: " + e.getMessage());
            return; // Don't add the observer if we can't send the flag block
        }
        streamBlocksObservers.add(new StreamBlocksObserver(observer));
    }

    public synchronized void removeStreamBlocksObserver(StreamObserver<Block> observer) {
        streamBlocksObservers.removeIf(o -> o.getObserver() == observer);
    }


    private synchronized void streamBlocks(Block block) {
        // Whenever a new block is created, stream it to all subscribed observers. 
        // If an observer throws an exception, remove it from the list of observers.
        for (StreamBlocksObserver observer : streamBlocksObservers) {
            try {
                observer.onNewBlock(block);
            } catch (Exception e) {
                // Handle any exceptions that may occur while streaming to observers
                System.err.println("Error streaming block to observer: " + e.getMessage());
                removeStreamBlocksObserver(observer.getObserver());
            }
        }
    }

    private static class StreamBlocksObserver {
        private final StreamObserver<Block> observer;

        public StreamBlocksObserver(StreamObserver<Block> observer) {
            this.observer = observer;
        }

        public StreamObserver<Block> getObserver() {
            return observer;
        }

        public void onNewBlock(Block block) {
            observer.onNext(block);
        }
    }
}

