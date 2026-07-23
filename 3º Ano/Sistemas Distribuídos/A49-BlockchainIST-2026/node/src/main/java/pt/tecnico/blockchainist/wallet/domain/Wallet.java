package pt.tecnico.blockchainist.wallet.domain;

public class Wallet {
    
    public static enum State {
        CREATING,
        VALID,
        DELETING
    }

    private String userId; 
    private String walletId;
    private long balance;
    private State futureState = State.CREATING;
    private State currentState = State.CREATING;

    public Wallet(String userId, String walletId){
        this.userId = userId;
        this.walletId = walletId;
    }

    public Wallet(String userId, String walletId, long balance){
        this.userId = userId;
        this.walletId = walletId;
        this.balance = balance;
    }


    public String getUserId() {
        return userId;
    }
    public void setUserId(String userId) {
        this.userId = userId;
    }
    public String getWalletId() {
        return walletId;
    }
    public void setWalletId(String walletId) {
        this.walletId = walletId;
    }
    public long getBalance() {
        return balance;
    }
    public void setBalance(long balance) {
        this.balance = balance;
    }
    public State getCurrentState() {
        return this.currentState;
    }
    public void setCurrentState(State state) {
        this.currentState = state;
    }
   public State getFutureState() {
        return this.futureState;
    }
    public void setFutureState(State state) {
        this.futureState = state;
    }

    public void addBalance(long amount){
        this.balance += amount;
    }
    public void withdrawBalance(long amount){
        this.balance -= amount;
    }    
}
