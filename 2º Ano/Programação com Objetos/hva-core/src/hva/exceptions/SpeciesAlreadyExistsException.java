package hva.exceptions;

public class SpeciesAlreadyExistsException extends Exception {
    private final String _id;

    public SpeciesAlreadyExistsException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}