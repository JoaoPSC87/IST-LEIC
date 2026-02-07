package hva;

import java.io.Serializable;
import java.util.Map;
import java.util.TreeMap;

public class Species implements Serializable {

    private String _id;
    private String _name;
    private Map<String, Animal> _animals = new TreeMap<String, Animal>();
    private int _availableVets = 0;

    public Species(String id, String name) {
        _id = id;
        _name = name;
    }

    public void addAnimal(Animal a) {
        _animals.put(a.getID(), a);
    }

    public int getPopulation() {
        return _animals.size();
    }

    public void incrementAvailableVets() {
        _availableVets = _availableVets + 1;
    }

    public void decrementAvailableVets() {
        _availableVets = _availableVets -1;
    }

    public int getAvailableVets() {
        return _availableVets;
    }

    public String getID() {
        return _id;
    }

    public String getName() {
        return _name;
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof Species s) {
            return _id == s._id;
        }
        return false;
    }
}