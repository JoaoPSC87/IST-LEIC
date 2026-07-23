package pt.tecnico.blockchainist.node;

import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.contract.Block;
import pt.tecnico.blockchainist.contract.StreamBlocksResponse;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.node.domain.NodeState;
import pt.tecnico.blockchainist.utils.Debug;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CompletableFuture;

public class NodeObserver implements StreamObserver<StreamBlocksResponse>{
    private final NodeState state;
    private final Map<String, CompletableFuture<Void>> pendingRequests;
    private final Runnable reconnect;
    private final Set<String> processedKeys = new HashSet<>();
    private int expectedSequenceNumber = 0;
    private final NodeKeyManager keyManager;

    public NodeObserver(NodeState state, Map<String, CompletableFuture<Void>> pendingRequests, Runnable reconnect, NodeKeyManager keyManager) throws RuntimeException {
        this.state = state;
        this.pendingRequests = pendingRequests;
        this.reconnect = reconnect;
        this.keyManager = keyManager;
    }

    @Override
    public void onNext(StreamBlocksResponse response) {
        // The node received a block from the sequencer.
        Block block = response.getBlock();
        Block blockWithoutSig = block.toBuilder().clearSignature().build();

        try {
            keyManager.verifyBlockSignature(blockWithoutSig.toByteArray(), block.getSignature().toByteArray());
        } catch (SecurityException e) {
            Debug.log("[NODE OBSERVER] Invalid block signature at expected seq "
             + expectedSequenceNumber + ", halting chain application: " + e.getMessage());
            return;
        } catch (Exception e) {
            Debug.log("[NODE OBSERVER] Error verifying block signature: " + e.getMessage());
            return;
        }

        // Check if it's the flag block indicating the end of existing blocks
        if(block.getSequenceNumber() == -1){ 
            Debug.log("[NODE OBSERVER] Received flag block, state is now updated");
            state.setUpdated(true);
            return; // Ignore the flag block
        }

        Debug.log("[NODE OBSERVER] Received block " + block.getSequenceNumber());
        
        // A cadeia tem de ser aplicada em ordem e sem saltos. Se o número de
        // sequência não for o esperado, não se pode aplicar este bloco - o estado ficaria incoerente
        if (block.getSequenceNumber() != expectedSequenceNumber) {
            Debug.log("[NODE OBSERVER] Out-of-order block: expected " 
                + expectedSequenceNumber + ", got " + block.getSequenceNumber());
            return;
        }
        expectedSequenceNumber++;

        // Process the block and update the node state
        for (Transaction tx : block.getTransactionsList()) {
            String key = tx.getKey();
            try{
                // Check for duplicate transaction keys to prevent processing the same transaction multiple times
                if (!key.isEmpty() && processedKeys.contains(key)){
                    Debug.log("[NODE OBSERVER] Duplicate key =" + key);
                    CompletableFuture<Void> future = pendingRequests.remove(key);
                    if (future != null) future.completeExceptionally(new IllegalArgumentException("Duplicate transaction"));
                } else {
                    // Process the transaction and update the node state
                    state.processTransaction(tx);
                    if (!key.isEmpty()) processedKeys.add(key);
                    // Complete the future associated with this transaction key to unblock the client waiting for the response
                    CompletableFuture<Void> future = pendingRequests.remove(key);
                    if (future != null) future.complete(null);
                }
            } catch (Exception e){
                Debug.log("[NODE OBSERVER] Error processing tx: " + e.getMessage());
                CompletableFuture<Void> future = pendingRequests.remove(key);
                if (future != null) future.completeExceptionally(e);
            }
        }
    }

    @Override
    public void onError(Throwable t) {
        // Hopefully never called.
        Debug.log("[NODE OBSERVER] Error: " + t.getMessage());
        reconnect.run();
    }

    @Override
    public void onCompleted() {
        Debug.log("[NODE OBSERVER] Stream completed");
        reconnect.run();
    }
}
