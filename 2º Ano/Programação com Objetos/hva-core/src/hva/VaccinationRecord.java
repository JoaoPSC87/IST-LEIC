package hva;

import java.io.Serializable;

public class VaccinationRecord implements Serializable {
    private Vaccine _vaccine;
    private Veterinarian _vet;
    private Species _species;
    private int _damage;

    public VaccinationRecord(Vaccine v, Veterinarian vet, Species s, int damage) {
        _vaccine = v;
        _vet = vet;
        _species = s;
        _damage = damage;
    }

    public Vaccine getVaccine() {
        return _vaccine;
    }

    public Veterinarian getVeterinarian() {
        return _vet;
    }

    public Species getSpecies() {
        return _species;
    }

    public int getDamage() {
        return _damage;
    }

    public boolean wasHarmful() {
        return !_vaccine.isAppropriateForSpecies(_species);
    }

    public String damageReport() {
        if(_damage == 0) {
            return _vaccine.isAppropriateForSpecies(_species) ? "NORMAL" : "CONFUSÃO";
        }
        if(_damage > 0 && _damage < 4) {
            return "ACIDENTE";
        }
        return "ERRO";
    }

    @Override
    public String toString() {
        return "REGISTO-VACINA|" + _vaccine.getID() + "|" + _vet.getID() + "|" + _species.getID();
    }
}