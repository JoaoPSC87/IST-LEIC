package hva.exceptions;

public class HarmfulVaccinationException extends Exception {

    private final String _vaccineId;
    private final String _animalId;

    public HarmfulVaccinationException(String vaccineId, String animalId) {
        _vaccineId = vaccineId;
        _animalId = animalId;
    }

    public String getVaccineID() {
        return _vaccineId;
    }

    public String getAnimalID() {
        return _animalId;
    }
    
}