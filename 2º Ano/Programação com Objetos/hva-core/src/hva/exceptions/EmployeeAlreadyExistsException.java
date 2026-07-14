package hva.exceptions;

public class EmployeeAlreadyExistsException extends Exception {
    private final String _id;
    
    public EmployeeAlreadyExistsException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}