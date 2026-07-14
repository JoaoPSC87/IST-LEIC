package hva.exceptions;

public class HabitatAlreadyExistsException extends Exception {
    private final String _id;
    
    public HabitatAlreadyExistsException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    }
}
