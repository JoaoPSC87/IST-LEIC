package hva.exceptions;

public class NoSuchAnimalException extends Exception {
    private final String _id;

    public NoSuchAnimalException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}
