package hva.exceptions;

public class UnableToVaccinateAnimalException extends Exception {
    private final String _vetId;
    private final String _speciesId;

    public UnableToVaccinateAnimalException(String vetId, String speciesId) {
        _vetId = vetId;
        _speciesId = speciesId;
    }

    public String getVetID() {
        return _vetId;
    } 

    public String getSpeciesID() {
        return _speciesId;
    }
}
