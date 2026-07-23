package pt.tecnico.blockchainist.sequencer;

import pt.tecnico.blockchainist.contract.*;
import pt.tecnico.blockchainist.sequencer.domain.SequencerState;
import pt.tecnico.blockchainist.utils.Debug;
import io.grpc.stub.StreamObserver;

public class SequencerServiceImpl extends SequencerServiceGrpc.SequencerServiceImplBase {

    private SequencerState state;

    public SequencerServiceImpl(SequencerState state) throws RuntimeException {
        this.state = state;
    }

    @Override
    public void broadcast(BroadcastRequest request, StreamObserver<BroadcastResponse> responseObserver) {
        // This method is called by the nodes to broadcast a transaction. It should return the sequence number of the block that will include the transaction.
        Debug.log("[SEQUENCER] Received request " + request.toString());
        Transaction tx = request.getTransaction();
        Debug.log("[TRANSACTION] Adding " + tx.toString());
        int seqNumber = state.addTransaction(tx);
        BroadcastResponse response = BroadcastResponse.newBuilder().setSequenceNumber(seqNumber).build();
        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void deliverBlock(DeliverBlockRequest request, StreamObserver<DeliverBlockResponse> responseObserver) {
        // This method is called by the nodes to request the block with the given sequence number. It should block until the block is available.
        Debug.log("[SEQUENCER] DeliverBlock requested for block " + request.getSequenceNumber());
        int seqNumber = request.getSequenceNumber();
        Block block = null;
        while(block == null){
            block = state.getBlock(seqNumber);
            if (block == null) {
                try { Thread.sleep(100); } catch (InterruptedException e) { break; }
            }
        }
        DeliverBlockResponse response = DeliverBlockResponse.newBuilder().setBlock(block).build();
        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void streamBlocks(StreamBlocksRequest request, StreamObserver<StreamBlocksResponse> responseObserver){
        // This method is called by the nodes to start streaming blocks. 
        // The sequencer should send all existing blocks and then continue sending new blocks as they are created.
        Debug.log("[SEQUENCER] StreamBlocks requested");
        state.addStreamBlocksObserver(adaptBlockObserver(responseObserver));
    }
    
    private StreamObserver<Block> adaptBlockObserver(StreamObserver<StreamBlocksResponse> responseObserver) {
        // Function to adapt a StreamObserver<Block> to a StreamObserver<StreamBlocksResponse> by wrapping each Block in a StreamBlocksResponse
        return new StreamObserver<Block>() {
            @Override
            public void onNext(Block block) {
                responseObserver.onNext(createBlockResponse(block));
            }

            @Override
            public void onError(Throwable t) {
                responseObserver.onError(t);
            }

            @Override
            public void onCompleted() {
                responseObserver.onCompleted();
            }
        };
    }
    
    private StreamBlocksResponse createBlockResponse(Block block) {
        return StreamBlocksResponse.newBuilder().setBlock(block).build();
    }
}
