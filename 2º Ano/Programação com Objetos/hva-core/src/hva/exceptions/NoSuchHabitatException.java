package hva.exceptions;

public class NoSuchHabitatException extends Exception {
    private final String _id;
    
    public NoSuchHabitatException(String id) {
        _id = id;
    }

    public String getID() {
        return _id;
    } 
}
