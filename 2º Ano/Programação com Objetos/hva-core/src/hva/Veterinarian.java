package hva;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import hva.exceptions.NoSuchResponsibilityException;
import hva.exceptions.UnableToVaccinateAnimalException;

public class Veterinarian extends Employee {
    private Map<String, Species> _assignedSpecies = new TreeMap<String, Species>();
    private EmployeeSatisfactionCalculator _satisfactionCalculator = new VeterinarianSatisfactionCalculator();
    private List<VaccinationRecord> _vaccinations = new ArrayList<VaccinationRecord>();

    public Veterinarian(String id, String name) {
        super(id, name);
    }

    public void addResponsibility(Species s) {
        if(!_assignedSpecies.containsKey(s.getID())) {
            _assignedSpecies.put(s.getID(), s);
            s.incrementAvailableVets();
        }
    }

    public void revokeResponsibility(Species s) throws NoSuchResponsibilityException {
        if(!_assignedSpecies.containsKey(s.getID())) {
            throw new NoSuchResponsibilityException(getID(), s.getID());
        }
        _assignedSpecies.remove(s.getID());
        s.decrementAvailableVets();
    }

    public Collection<Species> getAssignedSpecies() {
        return Collections.unmodifiableCollection(_assignedSpecies.values());
    }

    public void vaccinateAnimal(Animal animal, Vaccine vaccine) throws UnableToVaccinateAnimalException {
        String speciesId = animal.getSpecies().getID();
        if(!_assignedSpecies.containsKey(speciesId)) {
            throw new UnableToVaccinateAnimalException(speciesId, getID());
        }

        Species species = _assignedSpecies.get(speciesId);

        int damage = vaccine.damageUponApplication(species);

        VaccinationRecord vaccination = new VaccinationRecord(vaccine, this, species, damage);

        vaccine.registerApplication(vaccination);
        animal.updateVaccinationRecord(vaccination);
        _vaccinations.add(vaccination);
    }

    public double calculateSatisfaction() {
        return _satisfactionCalculator.compute(this);
    }

    @Override
    public String toString() {
        return "VET|" + getID() + "|" + getName() + (_assignedSpecies.isEmpty() ? "" : "|")
                + String.join(",", _assignedSpecies.keySet());
    }

    public boolean doesNotHaveAnimals() {
        return _assignedSpecies.isEmpty();
    }
}