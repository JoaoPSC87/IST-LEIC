package pt.tecnico.blockchainist.node;

import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.contract.*;
import pt.tecnico.blockchainist.contract.NodeServiceGrpc.NodeServiceImplBase;
import pt.tecnico.blockchainist.node.domain.NodeState;
import pt.tecnico.blockchainist.utils.Debug;
import pt.tecnico.blockchainist.wallet.domain.Wallet;
import io.grpc.Status;
import io.grpc.ManagedChannel;
import io.grpc.Context;

import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.locks.ReentrantLock;

import java.util.concurrent.ConcurrentHashMap;

import java.util.List;

public class NodeServiceImpl extends NodeServiceImplBase {
    private NodeState state;
    private final SequencerServiceGrpc.SequencerServiceBlockingStub sequencerStub;
    private final SequencerServiceGrpc.SequencerServiceStub sequencerAsyncStub;
    private final Map<String, CompletableFuture<Void>> pendingRequests;
    private final String orgName;
    private final NodeKeyManager keyManager;
    private final Map<String, ReentrantLock> walletLocks = new ConcurrentHashMap<>();
    
    private static final Map<String, String> USER_ORG_MAP = Map.of(
        "BC", "OrgA",
        "Alice", "OrgA",
        "Bob", "OrgA",
        "Charlie", "OrgA",
        "David", "OrgB",
        "Emma", "OrgB",
        "Fred", "OrgB",
        "Ginger", "OrgC",
        "Henry", "OrgC",
        "Iris", "OrgC"
    );

    public NodeServiceImpl(NodeState state, NodeKeyManager keyManager, ManagedChannel sequencerChannel, Map<String, CompletableFuture<Void>> pendingRequests, String orgName) throws RuntimeException {
        this.walletLocks.put("bc", new ReentrantLock());
        
        this.state = state;
        this.keyManager = keyManager;
        // Create a blocking stub for the sequencer to be used in broadcast and deliverBlock
        this.sequencerStub = SequencerServiceGrpc.newBlockingStub(sequencerChannel);
        // Create an asynchronous stub for the sequencer to be used in streamBlocks
        this.sequencerAsyncStub = SequencerServiceGrpc.newStub(sequencerChannel);
        this.pendingRequests = pendingRequests;
        
        // Launch a thread to continuously stream blocks from the sequencer and update the node state
        streamBlocks();
        this.orgName = orgName;
    }
    
    private boolean checkUser(String userId) {
        String userOrg = USER_ORG_MAP.get(userId);
        return userOrg != null && userOrg.equals(orgName);
    }

    private void validateUser(String userId) {
        if (!checkUser(userId)){
            throw new IllegalArgumentException("User " + userId + " does not belong to organization " + orgName);
        }
    }

    private void streamBlocks() {
        StreamBlocksRequest request = StreamBlocksRequest.getDefaultInstance(); 
        //Send a empty request to start streaming blocks
        sequencerAsyncStub.streamBlocks(request, new NodeObserver(state, pendingRequests, this::streamBlocks, keyManager)); 
    }

    private void submitAndWaitTransaction(Transaction transaction) throws Exception {
        submitTransaction(transaction);
        waitTransaction(transaction);
    }

    private void submitTransaction(Transaction transaction) throws Exception {
        // Now the node submits the transaction to the sequencer and waits for the response. 
        // The node uses the transaction key to track pending requests and respond to the correct client once the transaction is processed.
        String key = transaction.getKey();
        CompletableFuture<Void> future = new CompletableFuture<>();
        pendingRequests.put(key, future);
        Debug.log("[TRANSACTION] Sending to sequencer");
        sequencerStub.broadcast(BroadcastRequest.newBuilder().setTransaction(transaction).build());

    }
    
    private void waitTransaction(Transaction transaction) throws Exception {
        CompletableFuture<Void> future = pendingRequests.get(transaction.getKey());
        try{
            //Wait for the transaction to be processed and the future to be completed 
            // The NodeObserver will complete the future when the block containing the transaction is processed.
            future.get(); 
        } catch (ExecutionException e){
            Throwable cause = e.getCause();
            if (cause instanceof IllegalArgumentException) throw (IllegalArgumentException) cause;
            throw new RuntimeException(cause);
        }
    }

    private void applyDelay(){
        int delay = NodeDelayInterceptor.DELAY_CONTEXT_KEY.get(Context.current());
        if (delay > 0) {
            try {Thread.sleep(delay * 1000L);}
            catch (InterruptedException e) {Thread.currentThread().interrupt();}
        }
    }

    private String getKey() {
        return NodeDelayInterceptor.KEY_CONTEXT_KEY.get(Context.current());
    }

    @Override
    public void createWallet(CreateWalletRequest request, StreamObserver<CreateWalletResponse> responseObserver) {
        applyDelay();
        try {
            Debug.log("[NODE] Received request " + request.toString());
            CreateWalletRequest requestWithoutSig = request.toBuilder().clearSignature().build();
            Transaction tx = Transaction.newBuilder().setKey(request.getKey()).setCreateWallet(request).build();
            keyManager.verifyUserSignature(request.getUserId(), requestWithoutSig.toByteArray(), request.getSignature().toByteArray());
            // Submit the transaction to the sequencer
            validateUser(request.getUserId());
            submitTransaction(tx);
            
            walletLocks.computeIfAbsent(request.getWalletId(), k -> new ReentrantLock());
            lockWallet(request.getWalletId());
            try {
                state.createWallet(request.getUserId(), request.getWalletId());
                state.getWallet(request.getWalletId()).setCurrentState(Wallet.State.CREATING);
                state.getWallet(request.getWalletId()).setFutureState(Wallet.State.CREATING);
            } finally {
                unlockWallet(request.getWalletId());
            }

            waitTransaction(tx);
            // Wait for the transaction to be processed and completed. 
            // Then respond to the client.
            responseObserver.onNext(CreateWalletResponse.getDefaultInstance());
            responseObserver.onCompleted();
        } catch (SecurityException e) {
            responseObserver.onError(Status.UNAUTHENTICATED.withDescription(e.getMessage()).asRuntimeException()); 
        } catch (Exception e) {
            responseObserver.onError(Status.INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        }
    }

    @Override
    public void deleteWallet(DeleteWalletRequest request, StreamObserver<DeleteWalletResponse> responseObserver) {
        applyDelay();
        try {
            Debug.log("[NODE] Received request " + request.toString());
            DeleteWalletRequest requestWithoutSig = request.toBuilder().clearSignature().build();
            Transaction tx = Transaction.newBuilder().setKey(request.getKey()).setDeleteWallet(request).build();
            keyManager.verifyUserSignature(request.getUserId(), requestWithoutSig.toByteArray(), request.getSignature().toByteArray());
            // Submit the transaction to the sequencer
            validateUser(request.getUserId());
            
            lockWallet(request.getWalletId());
            try {
                Wallet w = state.getWallet(request.getWalletId());
                if(canDeleteWallet(request.getWalletId())){
                    w.setFutureState(Wallet.State.DELETING);
                }
                submitTransaction(tx);
            } finally {
                unlockWallet(request.getWalletId());
            }
            
            //Wait for the transaction to be processed and completed.
            waitTransaction(tx);
            // Then respond to the client
            responseObserver.onNext(DeleteWalletResponse.getDefaultInstance());
            responseObserver.onCompleted();
        } catch(SecurityException e){
            responseObserver.onError(Status.UNAUTHENTICATED.withDescription(e.getMessage()).asRuntimeException());
        } catch (Exception e) {
            responseObserver.onError(Status.INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        } 
    }

    @Override
    public void readBalance(ReadBalanceRequest request, StreamObserver<ReadBalanceResponse> responseObserver) {
        applyDelay();
        try {
            // Before reading the balance, check if the node state is updated. 
            // If not, respond with an error indicating that the node is not ready yet.
            if(!state.isUpdated()){
                throw new IllegalStateException("Node state is not updated yet");
            }
            Debug.log("[NODE] Received request " + request.toString());
            long balance = state.readBalance(request.getWalletId());
            ReadBalanceResponse response = ReadBalanceResponse
                    .newBuilder()
                    .setBalance(balance)
                    .build();
            responseObserver.onNext(response);
            responseObserver.onCompleted();
        } catch (Exception e) {
            responseObserver.onError(Status.INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        }
    }

    @Override
    public void transfer(TransferRequest request, StreamObserver<TransferResponse> responseObserver) {
        applyDelay(); 
        try {
            Debug.log("[NODE] Received request " + request.toString());
            TransferRequest requestWithoutSig = request.toBuilder().clearSignature().build();
            Transaction tx = Transaction.newBuilder().setKey(request.getKey()).setTransfer(request).build();
            keyManager.verifyUserSignature(request.getSrcUserId(), requestWithoutSig.toByteArray(), request.getSignature().toByteArray());
            validateUser(request.getSrcUserId());
            //Executa localmente 
            //String dstUserId = state.getWalletOwner(request.getDstWalletId());
            if (verifyConditionsForOptmisticTransfer(request)){
                // prevent deadlocks
                if (request.getSrcWalletId().compareTo(request.getDstWalletId()) < 0) {
                    lockWallet(request.getSrcWalletId());
                    lockWallet(request.getDstWalletId());
                } else {
                    lockWallet(request.getDstWalletId());
                    lockWallet(request.getSrcWalletId());
                }
                
                try {
                    state.executeTransferOptimistic(tx);
                    sequencerStub.broadcast(BroadcastRequest.newBuilder().setTransaction(tx).build());
                    responseObserver.onNext(TransferResponse.getDefaultInstance());
                    responseObserver.onCompleted();
                }finally{
                    unlockWallet(request.getSrcWalletId());
                    unlockWallet(request.getDstWalletId());
                }
            } else {
                submitTransaction(tx);
                responseObserver.onNext(TransferResponse.getDefaultInstance());
                responseObserver.onCompleted();
            }
        } catch (SecurityException e) {
            responseObserver.onError(Status.UNAUTHENTICATED.withDescription(e.getMessage()).asRuntimeException());
        } 
        catch (Exception e) {
            responseObserver.onError(Status.INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        }
    }

    @Override
    public void getBlockchainState(GetBlockchainStateRequest request, StreamObserver<GetBlockchainStateResponse> responseObserver) {
        // Before responding to the client, check if the node state is updated.
        // If not, respond with an error indicating that the node is not ready yet.
        if(!state.isUpdated()){
            responseObserver.onError(Status.FAILED_PRECONDITION.withDescription("Node state is not updated yet").asRuntimeException());
            return;
        }
        Debug.log("[NODE] Received request " + request.toString());
        List<Transaction> transactions = state.getBlockchainState();
        GetBlockchainStateResponse response = GetBlockchainStateResponse
                .newBuilder()
                .addAllTransactions(transactions)
                .build();
        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    public boolean verifyConditionsForOptmisticTransfer(TransferRequest request){
        String dstUserId = state.getWalletOwner(request.getDstWalletId());
        
        // Check if destination wallet is owned by a user in this organization
        if (dstUserId == null || !checkUser(dstUserId)) {
            return false;
        }
        
        // Check if either wallet has a delete pending (order causality: complete delete before transfer)
        Wallet srcWallet = state.getWallet(request.getSrcWalletId());
        Wallet dstWallet = state.getWallet(request.getDstWalletId());
        if (srcWallet.getFutureState() == Wallet.State.DELETING || dstWallet.getFutureState() == Wallet.State.DELETING) {
            return false;
        }
        
        return true;
    }

    public void lockWallet(String walletId) {
        ReentrantLock lock = walletLocks.get(walletId);
        if (lock == null) throw new IllegalArgumentException("Wallet does not exist: " + walletId);
        lock.lock();
    }

    public void unlockWallet(String walletId) {
        ReentrantLock lock = walletLocks.get(walletId);
        if (lock == null) throw new IllegalArgumentException("Wallet does not exist: " + walletId);
        lock.unlock();
    }

    public boolean canDeleteWallet(String walletId){
        // Check if wallet has pending transfer (order causality: complete transfer before delete)
        return (state.getWalletOwner(walletId) != null && state.readBalance(walletId) == 0);
    }
}