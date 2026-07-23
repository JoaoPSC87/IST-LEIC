package pt.tecnico.blockchainist.client.grpc;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Metadata;
import io.grpc.Status;
import io.grpc.stub.MetadataUtils;
import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.client.ClientKeyManager;
import pt.tecnico.blockchainist.contract.*;

import java.io.InputStream;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.List;
import java.util.concurrent.TimeUnit;

import com.google.protobuf.ByteString;

public class ClientNodeService {

    private final ManagedChannel channel;
    private final NodeServiceGrpc.NodeServiceBlockingStub stub;
    private final NodeServiceGrpc.NodeServiceStub asyncStub;
    private static final Metadata.Key<String> DELAY_KEY = Metadata.Key.of("delay", Metadata.ASCII_STRING_MARSHALLER);
    private static final Metadata.Key<String> KEY_KEY =
            Metadata.Key.of("key", Metadata.ASCII_STRING_MARSHALLER);
    private final ClientKeyManager keyManager;

    public ClientNodeService(String host, int port, String organization, ClientKeyManager keyManager) {
        String target = host + ":" + port;
        channel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        stub = NodeServiceGrpc.newBlockingStub(channel);
        asyncStub = NodeServiceGrpc.newStub(channel);
        this.keyManager = keyManager;
    }
    
    private NodeServiceGrpc.NodeServiceBlockingStub createDelayedStub(int delay, String key) {
        Metadata headers = new Metadata();
        headers.put(DELAY_KEY, String.valueOf(delay));
        headers.put(KEY_KEY, key);
        return stub.withInterceptors(MetadataUtils.newAttachHeadersInterceptor(headers));
    }

    private NodeServiceGrpc.NodeServiceStub createDelayedAsyncStub(int delay, String key) {
        Metadata headers = new Metadata();
        headers.put(DELAY_KEY, String.valueOf(delay));
        headers.put(KEY_KEY, key);
        return asyncStub.withInterceptors(MetadataUtils.newAttachHeadersInterceptor(headers));
    }

    public void createWallet(String userId, String walletId, int delay, String key) throws Exception {
        NodeServiceGrpc.NodeServiceBlockingStub stubWithDelay = createDelayedStub(delay, key);

        CreateWalletRequest requestWithoutSignature = CreateWalletRequest.newBuilder()
                .setUserId(userId)
                .setWalletId(walletId)
                .setKey(key)
                .build();
        
        byte[] signature = keyManager.signMessage(userId, requestWithoutSignature.toByteArray());
        CreateWalletRequest request = requestWithoutSignature.toBuilder()
                .setSignature(com.google.protobuf.ByteString.copyFrom(signature))
                .build();
        
        stubWithDelay.createWallet(request);
    }

    public void createWalletAsync(String userId, String walletId, long commandNumber, int delay, String key) throws Exception{
        CreateWalletRequest requestWithoutSig = CreateWalletRequest.newBuilder()
            .setUserId(userId)
            .setWalletId(walletId)
            .setKey(key)
            .build();

        byte[] signature = keyManager.signMessage(userId, requestWithoutSig.toByteArray());
        CreateWalletRequest request = requestWithoutSig.toBuilder()
            .setSignature(ByteString.copyFrom(signature))
            .build();
        createDelayedAsyncStub(delay, key).createWallet(request, new StreamObserver<CreateWalletResponse>() {
           
            @Override
            public void onNext(CreateWalletResponse response){
                System.out.println(commandNumber + " OK");
            }
            @Override
            public void onError(Throwable t){
                System.err.println(commandNumber + " ERROR: " + Status.fromThrowable(t).getDescription());
            }
            @Override
            public void onCompleted(){}
        });
    }

    public void deleteWallet(String userId, String walletId, int delay, String key) throws Exception {
        NodeServiceGrpc.NodeServiceBlockingStub stubWithDelay = createDelayedStub(delay, key);

        DeleteWalletRequest requestWithoutSignature = DeleteWalletRequest.newBuilder()
                .setUserId(userId)
                .setWalletId(walletId)
                .setKey(key)
                .build();
        byte[] signature = keyManager.signMessage(userId, requestWithoutSignature.toByteArray());
        DeleteWalletRequest request = requestWithoutSignature.toBuilder()
                .setSignature(com.google.protobuf.ByteString.copyFrom(signature))
                .build();
                
        stubWithDelay.deleteWallet(request);
    }

    public void deleteWalletAsync(String userId, String walletId, long commandNumber, int delay, String key) throws Exception {
        NodeServiceGrpc.NodeServiceStub stubWithDelay = createDelayedAsyncStub(delay, key);

        DeleteWalletRequest requestWithoutSignature = DeleteWalletRequest.newBuilder()
                .setUserId(userId)
                .setWalletId(walletId)
                .setKey(key)
                .build();
        byte[] signature = keyManager.signMessage(userId, requestWithoutSignature.toByteArray());
       
        DeleteWalletRequest request = requestWithoutSignature.toBuilder()
                .setSignature(com.google.protobuf.ByteString.copyFrom(signature))
                .build();
        stubWithDelay.deleteWallet(request, new StreamObserver<DeleteWalletResponse>(){ //Asynchronous response handling
            @Override
            public void onNext(DeleteWalletResponse response){
                System.out.println(commandNumber + " OK");
            }
            @Override
            public void onError(Throwable t){
                System.err.println(commandNumber + " ERROR: " + Status.fromThrowable(t).getDescription());
            }
            @Override
            public void onCompleted(){}
        });
    }

    public void transfer(String srcUserId, String srcWalletId, String dstWalletId, Long amount, int delay, String key) throws Exception {
        NodeServiceGrpc.NodeServiceBlockingStub stubWithDelay = createDelayedStub(delay, key);        

        TransferRequest requestWithoutSignature = TransferRequest.newBuilder()
                    .setSrcUserId(srcUserId)
                    .setSrcWalletId(srcWalletId)
                    .setDstWalletId(dstWalletId)
                    .setValue(amount)
                    .setKey(key)
                    .build();

        byte[] signature = keyManager.signMessage(srcUserId, requestWithoutSignature.toByteArray());
        TransferRequest request = requestWithoutSignature.toBuilder()
                .setSignature(com.google.protobuf.ByteString.copyFrom(signature))
                .build();
        
        stubWithDelay.transfer(request);
    }

    public void transferAsync(String srcUserId, String srcWalletId, String dstWalletId, Long amount, long commandNumber, int delay, String key) throws Exception{
        TransferRequest requestWithoutSig = TransferRequest.newBuilder()
                    .setSrcUserId(srcUserId)
                    .setSrcWalletId(srcWalletId)
                    .setDstWalletId(dstWalletId)
                    .setValue(amount)
                    .setKey(key)
                    .build();

        byte[] signature = keyManager.signMessage(srcUserId, requestWithoutSig.toByteArray());
        TransferRequest request = requestWithoutSig.toBuilder()
            .setSignature(ByteString.copyFrom(signature))
            .build();
            
        createDelayedAsyncStub(delay, key).transfer(request, new StreamObserver<TransferResponse>(){ //Asynchronous response handling
            @Override
            public void onNext(TransferResponse response){
                System.out.println(commandNumber + " OK");
            }
            @Override
            public void onError(Throwable t){
                System.err.println(commandNumber + " ERROR: " + Status.fromThrowable(t).getDescription());
            }
            @Override
            public void onCompleted(){}
        });
    }

    public long readBalance(String walletId, int delay) {
        NodeServiceGrpc.NodeServiceBlockingStub stubWithDelay = createDelayedStub(delay, "");

        ReadBalanceRequest request = ReadBalanceRequest.newBuilder()
                    .setWalletId(walletId)
                    .build();
        ReadBalanceResponse response = stubWithDelay.readBalance(request);
        return response.getBalance();
    }

    public void readBalanceAsync(String walletId, long commandNumber, int delay) {
        NodeServiceGrpc.NodeServiceStub stubWithDelay = createDelayedAsyncStub(delay, "");

        ReadBalanceRequest request = ReadBalanceRequest.newBuilder()
                    .setWalletId(walletId)
                    .build();
        stubWithDelay.readBalance(request, new StreamObserver<ReadBalanceResponse>(){ //Asynchronous response handling
            @Override
            public void onNext(ReadBalanceResponse response){
                System.out.println(commandNumber + " OK: " + response.getBalance());
            }
            @Override
            public void onError(Throwable t){
                System.err.println(commandNumber + " ERROR: " + Status.fromThrowable(t).getDescription());
            }
            @Override
            public void onCompleted(){}
        });
    }

    public List<Transaction> getBlockchainState() {
        GetBlockchainStateRequest request = GetBlockchainStateRequest.newBuilder().build();
        GetBlockchainStateResponse response = stub.getBlockchainState(request);
        return response.getTransactionsList();
    }

    public void shutdown() throws InterruptedException {
        channel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }
}
