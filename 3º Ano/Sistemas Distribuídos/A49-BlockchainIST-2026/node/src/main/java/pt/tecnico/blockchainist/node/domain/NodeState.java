package pt.tecnico.blockchainist.node.domain;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.node.NodeKeyManager;
import pt.tecnico.blockchainist.utils.Debug;
import pt.tecnico.blockchainist.wallet.domain.Wallet;

public class NodeState {
    // walletId → userId (dono da carteira)
    private final Map<String, Wallet> wallets = new HashMap<>();
    private final Set<String> optimisticallyExecutedKeys = ConcurrentHashMap.newKeySet();
    private final ReadWriteLock lock = new ReentrantReadWriteLock();
    // walletId → saldo
    private final List<Transaction> ledger = new ArrayList<>();
    private final NodeKeyManager keyManager;

    // Flag to indicate if the node state is updated with the initial blocks from the sequencer.
    // This flag is changed when the last block with sequence number -1 is received.
    private volatile boolean isUpdated = false;

    public NodeState(NodeKeyManager keyManager) {
        // Carteira especial do banco central: pertence ao utilizador "BC", saldo inicial 1000
        wallets.put("bc", new Wallet("BC", "bc", 1000L));
        wallets.get("bc").setCurrentState(Wallet.State.VALID);
        wallets.get("bc").setFutureState(Wallet.State.VALID);
        this.keyManager = keyManager;
    }

    public void executeTransferOptimistic(Transaction tx) {
        lock.writeLock().lock();
        try{
        var transfer = tx.getTransfer();
        transfer(transfer.getSrcUserId(), transfer.getSrcWalletId(),
                transfer.getDstWalletId(), transfer.getValue());
        optimisticallyExecutedKeys.add(tx.getKey());
        } finally {
            lock.writeLock().unlock();
        }
    }

    public void processTransaction(Transaction transaction) {
        lock.writeLock().lock();
        String key = transaction.getKey();
        if (!key.isEmpty() && optimisticallyExecutedKeys.contains(key)) {
            Debug.log("[NODE STATE] Transaction with key " + key + " was already optimistically executed, skipping execution");
            optimisticallyExecutedKeys.remove(key);
            ledger.add(transaction);
            lock.writeLock().unlock();
            return;
        }
        try {
            // Verify signature of the transaction before processing it
            byte[] requestBytes;

            switch (transaction.getOperationCase()) {
                case CREATE_WALLET:
                    var createWallet = transaction.getCreateWallet();
                    requestBytes = createWallet.toBuilder().clearSignature().build().toByteArray();

                    keyManager.verifyUserSignature(createWallet.getUserId(), requestBytes, createWallet.getSignature().toByteArray());
                    checkKeyMatches(transaction.getKey(), createWallet.getKey());
                    createWallet(createWallet.getUserId(), createWallet.getWalletId());

                    break;
                case DELETE_WALLET:
                    var deleteWallet = transaction.getDeleteWallet();
                    requestBytes = deleteWallet.toBuilder().clearSignature().build().toByteArray();
                    
                    keyManager.verifyUserSignature(deleteWallet.getUserId(), requestBytes, deleteWallet.getSignature().toByteArray());
                    checkKeyMatches(transaction.getKey(), deleteWallet.getKey());
                    deleteWallet(deleteWallet.getUserId(), deleteWallet.getWalletId());

                    break;
                case TRANSFER:
                    var transfer = transaction.getTransfer();
                    requestBytes = transfer.toBuilder().clearSignature().build().toByteArray();

                    keyManager.verifyUserSignature(transfer.getSrcUserId(), requestBytes, transfer.getSignature().toByteArray());
                    checkKeyMatches(transaction.getKey(), transfer.getKey());
                    transfer(transfer.getSrcUserId(), transfer.getSrcWalletId(), transfer.getDstWalletId(), transfer.getValue());

                    break;
                default:
                    throw new IllegalArgumentException("Unknown transaction type: " + transaction.getOperationCase());
            }
        } catch(Exception e) {
            throw new IllegalArgumentException("Error processing transaction: " + e.toString());
        } finally {
            ledger.add(transaction);
            lock.writeLock().unlock();
        }
    }

    /**
     * O campo key da Transaction viaja fora da assinatura do utilizador (o nó
     * preenche-o a partir do request). Como o canal nó-sequenciador não é
     * seguro, confirmamos que continua igual à key que o utilizador assinou.
     */
    private void checkKeyMatches(String transactionKey, String signedKey) {
        if (!transactionKey.equals(signedKey)) {
            throw new SecurityException("Transaction key does not match the signed key");
        }
    }

    public void createWallet(String userId, String walletId) {
        // write-lock reentrante: é seguro mesmo quando chamado a partir do
        // processTransaction, que já o detém
        lock.writeLock().lock();
        try {
            if (!wallets.containsKey(walletId)) {
                Wallet w = new Wallet(userId, walletId);
                w.setCurrentState(Wallet.State.VALID);
                w.setFutureState(Wallet.State.VALID);
                wallets.put(walletId, w);
                return;
            }

            Wallet w = wallets.get(walletId);
            if (w.getCurrentState() == Wallet.State.VALID) {
                throw new IllegalArgumentException("Wallet already exists: " + walletId);
            } else {
                w.setCurrentState(Wallet.State.VALID);
            }
        } finally {
            lock.writeLock().unlock();
        }
    }

    public void deleteWallet(String userId, String walletId) {
        lock.writeLock().lock();
        try {
            Wallet w = wallets.get(walletId);

            if (w == null){
                throw new IllegalArgumentException("Wallet does not exist: " + walletId);
            }

            if (!w.getUserId().equals(userId)){
                throw new IllegalArgumentException("Wallet does not belong to user: " + userId);
            }

            if (w.getBalance() != 0) {
                throw new IllegalArgumentException("Wallet balance is not zero: " + walletId);
            }

            wallets.remove(walletId);
        } finally {
            lock.writeLock().unlock();
        }
    }


    public void transfer(String srcUserId, String srcWalletId, String dstWalletId, Long amount) {
        lock.writeLock().lock();
        try{
            Wallet srcWallet = wallets.get(srcWalletId);
            Wallet dstWallet = wallets.get(dstWalletId);

            if (srcWallet == null) {
                throw new IllegalArgumentException("Source wallet does not exist: " + srcWalletId);
            }
            if (dstWallet == null) {
                throw new IllegalArgumentException("Destination wallet does not exist: " + dstWalletId);
            }
            if (!srcWallet.getUserId().equals(srcUserId)) {
                throw new IllegalArgumentException("Source wallet does not belong to user: " + srcUserId);
            }
            if (amount <= 0){
                throw new IllegalArgumentException("Amount must be positive");
            }
            if (srcWallet.getBalance() < amount) {
                throw new IllegalArgumentException("Insufficient balance in source wallet: " + srcWalletId);
            }
       

             // Perform transfer
            srcWallet.withdrawBalance(amount);
            dstWallet.addBalance(amount);
        } finally {
            lock.writeLock().unlock();
        }  

    }

    public long readBalance(String walletId) {
        lock.readLock().lock();
        try {
            Wallet w = wallets.get(walletId);
            if (w == null) {
                throw new IllegalArgumentException("Wallet does not exist: " + walletId);
            }
            return w.getBalance();
        } finally {
            lock.readLock().unlock();
        }
    }

    public List<Transaction> getBlockchainState() {
        lock.readLock().lock();
        try {
            return new ArrayList<>(ledger);
        } finally {
            lock.readLock().unlock();
        }
    }

    public boolean isUpdated() {
        return isUpdated;
    }

    public void setUpdated(boolean updated) {
        lock.writeLock().lock();
        try {
            isUpdated = updated;
        } finally {
            lock.writeLock().unlock();
        }
    }

    public String getWalletOwner(String walletId) {
        lock.readLock().lock();
        try {
            Wallet w = wallets.get(walletId);
            if (w == null) return null;
            return w.getUserId();
        } finally {
            lock.readLock().unlock();
        }
    }

    public Wallet getWallet(String walletId) {
        lock.readLock().lock();
        try {
            return wallets.get(walletId);
        } finally {
            lock.readLock().unlock();
        }
        
    }
}
