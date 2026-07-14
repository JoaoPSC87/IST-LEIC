package hva.exceptions;

public class NoSuchEmployeeException extends Exception {
    private final String _id;

    public NoSuchEmployeeException(String id) {
        _id = id;
    }
    
    public String getID() {
        return _id;
    }
}