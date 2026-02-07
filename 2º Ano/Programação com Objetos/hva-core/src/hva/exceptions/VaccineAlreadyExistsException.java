package hva.exceptions;

public class VaccineAlreadyExistsException extends Exception {
    private final String _id;

    public VaccineAlreadyExistsException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}