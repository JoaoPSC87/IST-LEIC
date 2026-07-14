package hva;

import java.io.Serializable;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.TreeMap;


public class Habitat implements Serializable {
    private String _id;
    private String _name;
    private int _area;
    private Map<String, Animal> _animals = new TreeMap<String, Animal>();
    private Map<String, Tree> _trees = new TreeMap<String, Tree>(new CaseInsensitiveComparator());
    private Map<String, HabitatInfluence> _influenceOverSpecies = new TreeMap<String, HabitatInfluence>();
    private int _availableCaretakers = 0;
    
    public Habitat(String id, String name, int area){
        _id = id;
        _name = name;
        _area = area;
    }

    public String getID(){
        return _id;
    }

    public String getName(){
        return _name;
    }
    
    public int getArea(){
        return _area;
    }

    public void changeArea(int area) {
        _area = area;
    }

    public Collection<Animal> getAnimals() {
        return Collections.unmodifiableCollection(_animals.values());
    }

    public Collection<Tree> getTrees() {
        return Collections.unmodifiableCollection(_trees.values());
    }

    public int getPopulation(){
        return _animals.size();
    }

    public int getSpeciesPopulation(Species s){
        int speciesPopulation = 0;
        
        for (Animal a : _animals.values()) {
            if (a.getSpecies().equals(s)) {
                speciesPopulation++;
            }
        }
        return speciesPopulation;
    }

    public HabitatInfluence getInfluenceOverSpecies(String speciesId) {
        if (!_influenceOverSpecies.containsKey(speciesId)) {
            return HabitatInfluence.NEUTRAL;
        }

        return _influenceOverSpecies.get(speciesId);
    }

    public void addTree(Tree t){
        _trees.put(t.getID(), t);
    }

    public void addAnimal(Animal a){
        _animals.put(a.getID(), a);
    }

    public void removeAnimal(Animal a){
        _animals.remove(a.getID());
    }
    
    public void changeInfluenceOverSpecies(String speciesId, HabitatInfluence influence) {
        _influenceOverSpecies.put(speciesId, influence);
    } 

    public int getAvailableCaretakers() {
        return _availableCaretakers;
    }

    public void incrementAvailableCaretakers() {
        _availableCaretakers++;
    }

    public void decrementAvailableCaretakers() {
        _availableCaretakers--;
    }

    @Override
    public String toString() {
        String habitatStr = "HABITAT|" + _id + "|" + _name + "|" + _area + "|" + _trees.size();
        for (Tree t : _trees.values()) {
            habitatStr = habitatStr + "\n" + t.toString();
        }

        return habitatStr;
    }

    public boolean hasTrees() {
        return _trees.isEmpty();
    }
}