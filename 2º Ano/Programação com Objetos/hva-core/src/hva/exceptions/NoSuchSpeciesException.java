package hva.exceptions;

public class NoSuchSpeciesException extends Exception {
    private final String _id;

    public NoSuchSpeciesException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}