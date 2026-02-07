package hva.exceptions;

public class TreeAlreadyExistsException extends Exception {
    private final String _id;

    public TreeAlreadyExistsException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}