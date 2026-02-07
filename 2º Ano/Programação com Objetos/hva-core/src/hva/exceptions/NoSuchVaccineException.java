package hva.exceptions;

public class NoSuchVaccineException extends Exception {
    private final String _id;

    public NoSuchVaccineException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}