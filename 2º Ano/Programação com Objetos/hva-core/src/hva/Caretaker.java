package hva;

import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.TreeMap;

import hva.exceptions.NoSuchResponsibilityException;

public class Caretaker extends Employee {
    private Map<String, Habitat> _habitats = new TreeMap<String, Habitat>();
    private EmployeeSatisfactionCalculator _satisfactionCalculator = new CaretakerSatisfactionCalculator();
    
    public Caretaker(String id, String name) {
        super(id, name);
    }

    public void addResponsibility(Habitat h) {
        if (!_habitats.containsKey(h.getID())) {
            _habitats.put(h.getID(), h);
            h.incrementAvailableCaretakers();
        }
    }

    public void revokeResponsibility(Habitat h) throws NoSuchResponsibilityException  {
        if (!_habitats.containsKey(h.getID())) {
            throw new NoSuchResponsibilityException(getID(), h.getID());
        }
        _habitats.remove(h.getID());
        h.decrementAvailableCaretakers();
    }

    public double calculateSatisfaction() {
        return _satisfactionCalculator.compute(this);
    }

    public Collection<Habitat> getAssignedHabitats() {
        return Collections.unmodifiableCollection(_habitats.values());
    }

    @Override
    public String toString() {
        return "TRT|" + getID() + "|" + getName() + (_habitats.isEmpty() ? "" : "|") + String.join(",", _habitats.keySet());
    }
}
