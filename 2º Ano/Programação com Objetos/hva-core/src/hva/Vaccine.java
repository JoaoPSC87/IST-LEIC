package hva;

import java.io.Serializable;
import java.util.Collection;
import java.util.Collections;
import java.util.Set;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public class Vaccine implements Serializable {
    private String _id;
    private String _name;
    private Map<String, Species> _appropriateSpecies = new TreeMap<String, Species>();
    private List<VaccinationRecord> _applications = new LinkedList<VaccinationRecord>();

    public Vaccine(String id, String name) {
        _id = id;
        _name = name;
    }

    public String getID() {
        return _id;
    }

    public String getName() {
        return _name;
    }

    public int damageUponApplication(Species species) {
        int damage = 0;
        if (_appropriateSpecies.containsKey(species.getID())) {
            return damage;
        }

        for (Species appropriateSpecies : _appropriateSpecies.values()) {
            int maxNameSize = Math.max(species.getName().length(), appropriateSpecies.getName().length());
            
            Set<Character> characters = new HashSet<Character>();
            for (char c : appropriateSpecies.getName().toCharArray()) {
                characters.add(c);
            }
            int commonCharacters = 0;
            for (char c : species.getName().toCharArray()) {
                if (characters.contains(c)) {
                    commonCharacters++;
                }
            }

            damage = Math.max(damage, maxNameSize - commonCharacters);
        }
        
        return damage;
    }

    public boolean isAppropriateForSpecies(Species species) {
        return _appropriateSpecies.containsKey(species.getID()); 
    }

    public Collection<Species> getAppropriateSpecies() {
        return Collections.unmodifiableCollection(_appropriateSpecies.values());
    }

    void addAppropriateSpecies(Species s) {
        if (!_appropriateSpecies.containsKey(s.getID())) {
            _appropriateSpecies.put(s.getID(), s);
        }
    }

    public void registerApplication(VaccinationRecord r) {
        _applications.add(r);
    }

    public VaccinationRecord getLatestApplication() {
        return _applications.get(_applications.size() - 1);
    }

    @Override
    public String toString() {
        return "VACINA|" + _id + "|" + _name + "|" + _applications.size() + (_appropriateSpecies.isEmpty() ? "" : "|")
                + String.join(",", _appropriateSpecies.keySet());
    }
}
