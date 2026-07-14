package hva;

import java.io.Serializable;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public class Animal implements Serializable {
    private String _id;
    private String _name;
    private Habitat _habitat;
    private Species _species;
    private List<VaccinationRecord> _vaccinationRecords = new LinkedList<VaccinationRecord>();

    public Animal(String id, String name, Species species, Habitat habitat) {
        _id = id;
        _name = name;
        _species = species;
        _habitat = habitat;
    }

    public String getID() {
        return _id;
    }

    public String getName() {
        return _name;
    }

    public Species getSpecies() {
        return _species;
    }

    public Habitat getHabitat() {
        return _habitat;
    }

    public Collection<VaccinationRecord> getVaccinationRecords() {
        return Collections.unmodifiableCollection(_vaccinationRecords);
    }

    void changeHabitat(Habitat h) {
        _habitat.removeAnimal(this);
        _habitat = h;
        h.addAnimal(this);
    }

    public double calculateSatisfaction() {
        int sameSpeciesPopulation = _habitat.getSpeciesPopulation(_species);
        int habitatPopulation = _habitat.getPopulation();
        int differentSpeciesPopulation = habitatPopulation - sameSpeciesPopulation;
        int habitatArea = _habitat.getArea();
        int adequacy = _habitat.getInfluenceOverSpecies(_species.getID()).influence();

        return 20 + 3 * (sameSpeciesPopulation - 1) - 2 * differentSpeciesPopulation + (double) habitatArea / habitatPopulation
                + adequacy;
    }

    void updateVaccinationRecord(VaccinationRecord v) {
        _vaccinationRecords.add(v);
    }

    @Override
    public String toString() {
        String animalStr = "ANIMAL|" + _id + "|" + _name + "|" + _species.getID() + "|";
        if (_vaccinationRecords.isEmpty()) {
            return animalStr + "VOID|" + _habitat.getID();
        }
        String healthHistory = "";
        for (VaccinationRecord vaccination : _vaccinationRecords) {
            healthHistory += (healthHistory.isEmpty() ? "" : ",") + vaccination.damageReport();
        }
        return animalStr + healthHistory + "|" + _habitat.getID();
    }

    public boolean hasBeenVaccinated() {
        return !_vaccinationRecords.isEmpty();
       
    }

}
