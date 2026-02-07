package hva.exceptions;

public class NoSuchVeterinarianException extends Exception {
    private final String _id;

    public NoSuchVeterinarianException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}