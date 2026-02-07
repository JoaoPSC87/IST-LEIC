package hva.exceptions;

public class AnimalAlreadyExistsException extends Exception {
    private final String _id;

    public AnimalAlreadyExistsException(String id){
        _id = id;
    }

    public String getID(){
        return _id;
    }
}