package hva;

import java.io.Serial;
import java.io.Serializable;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import java.util.Map;
import java.util.TreeMap;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.LinkedList;

import hva.exceptions.AnimalAlreadyExistsException;
import hva.exceptions.EmployeeAlreadyExistsException;
import hva.exceptions.HabitatAlreadyExistsException;
import hva.exceptions.SpeciesAlreadyExistsException;
import hva.exceptions.TreeAlreadyExistsException;
import hva.exceptions.VaccineAlreadyExistsException;
import hva.exceptions.NoSuchAnimalException;
import hva.exceptions.NoSuchEmployeeException;
import hva.exceptions.NoSuchHabitatException;
import hva.exceptions.NoSuchResponsibilityException;
import hva.exceptions.NoSuchVaccineException;
import hva.exceptions.NoSuchVeterinarianException;
import hva.exceptions.UnableToVaccinateAnimalException;
import hva.exceptions.HarmfulVaccinationException;
import hva.exceptions.ImportFileException;
import hva.exceptions.UnrecognizedEntryException;
import hva.exceptions.NoSuchSpeciesException;

public class Hotel implements Serializable {

    @Serial
    private static final long serialVersionUID = 202407081733L;
    private Map<String, Animal> _animals;
    private Map<String, Employee> _employees;
    private Map<String, Habitat> _habitats;
    private Map<String, Tree> _trees;
    private Map<String, Vaccine> _vaccines;
    private Map<String, Species> _species;
    private List<VaccinationRecord> _vaccinationRecords;
    private Season _season = new Spring(this);
    private boolean _changed = false;

    public Hotel() {
        CaseInsensitiveComparator cic = new CaseInsensitiveComparator();

        _animals = new TreeMap<String, Animal>(cic);
        _employees = new TreeMap<String, Employee>(cic);
        _habitats = new TreeMap<String, Habitat>(cic);
        _vaccines = new TreeMap<String, Vaccine>(cic);
        _species = new TreeMap<String, Species>(cic);
        _trees = new TreeMap<String, Tree>(cic);
        _vaccinationRecords = new LinkedList<VaccinationRecord>();
    }

    /**
     * Read text input file and create domain entities.
     *
     * @param filename name of the text input file
     * @throws ImportFileException
     */
    void importFile(String filename) throws ImportFileException {
        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] fields = line.split("\\|");
                switch (fields[0]) {
                    case "ESPÉCIE" -> registerSpecies(fields);
                    case "ÁRVORE" -> registerTree(fields);
                    case "ANIMAL" -> registerAnimal(fields);
                    case "VETERINÁRIO" -> registerVeterinarian(fields);
                    case "TRATADOR" -> registerCaretaker(fields);
                    case "HABITAT" -> registerHabitat(fields);
                    case "VACINA" -> registerVaccine(fields);
                    default -> throw new UnrecognizedEntryException(fields[0]);
                }
            }
        } catch (IOException | UnrecognizedEntryException
                | SpeciesAlreadyExistsException
                | AnimalAlreadyExistsException
                | NoSuchSpeciesException
                | NoSuchHabitatException
                | HabitatAlreadyExistsException
                | EmployeeAlreadyExistsException
                | TreeAlreadyExistsException
                | VaccineAlreadyExistsException e) {
            throw new ImportFileException(filename, e);
        }
    }

    /**
     * Registers a new animal
     * 
     * @param fields an array of strings with the animal's id, name, its species' id
     *               and its habitat.
     * @throws AnimalAlreadyExistsException if animal id has been registered
     *                                      previously
     */
    public void registerAnimal(String... fields)
            throws NoSuchSpeciesException, NoSuchHabitatException, AnimalAlreadyExistsException {
        String id = fields[1];
        String name = fields[2];
        String speciesId = fields[3];
        String habitatId = fields[4];

        if (!_species.containsKey(speciesId)) {
            throw new NoSuchSpeciesException(speciesId);
        }
        if (!_habitats.containsKey(habitatId)) {
            throw new NoSuchHabitatException(habitatId);
        }
        if (_animals.containsKey(id)) {
            throw new AnimalAlreadyExistsException(id);
        }

        Species species = _species.get(speciesId);
        Habitat habitat = _habitats.get(habitatId);

        Animal animal = new Animal(id, name, species, habitat);

        habitat.addAnimal(animal);
        species.addAnimal(animal);
        _animals.put(id, animal);

        _changed = true;
    }

    /**
     * Registers a new species
     * 
     * @param fields an array of strings with the species' id and name
     * @throws SpeciesAlreadyExistsException if this species id has been registered
     *                                       previously
     */
    public void registerSpecies(String... fields) throws SpeciesAlreadyExistsException {
        String id = fields[1];
        String name = fields[2];

        if (_species.containsKey(id)) {
            throw new SpeciesAlreadyExistsException(id);
        }

        for (Species species : _species.values()) {
            if (species.getName().equals(name)) {
                throw new SpeciesAlreadyExistsException(name);
            }
        }

        _species.put(id, new Species(id, name));

        _changed = true;
    }

    /**
     * Registers a new tree
     * 
     * @param fields an array of strings with the tree's id, name, age, maintenance
     *               difficulty and its type
     * @throws TreeAlreadyExistsException if this tree id has been registered
     *                                    previously
     */
    public void registerTree(String... fields) throws TreeAlreadyExistsException {
        String id = fields[1];
        String name = fields[2];
        int age = Integer.parseInt(fields[3]);
        int maintenanceDifficulty = Integer.parseInt(fields[4]);
        String treeType = fields[5];

        if (_trees.containsKey(id)) {
            throw new TreeAlreadyExistsException(id);
        }

        switch (treeType) {
            case "PERENE": {
                Tree t = new ConiferousTree(id, name, age, maintenanceDifficulty);
                t.updateBioCycleStage(_season);
                t.updateSeasonalMaintenanceDifficulty(_season);
                _trees.put(id, t);
                break;
            }
            case "CADUCA": {
                Tree t = new DeciduousTree(id, name, age, maintenanceDifficulty);
                t.updateBioCycleStage(_season);
                t.updateSeasonalMaintenanceDifficulty(_season);
                _trees.put(id, t);
                break;
            }
        }
        _changed = true;
    }

    /**
     * Registers a new veterinarian
     * 
     * @param fields an array of strings with the vet's id, name and all the species
     *               he is responsible for
     * @throws EmployeeAlreadyExistsException if this vet's id has been registered
     *                                        previously
     */
    public void registerVeterinarian(String... fields) throws EmployeeAlreadyExistsException {
        String id = fields[1];
        String name = fields[2];

        if (_employees.containsKey(id)) {
            throw new EmployeeAlreadyExistsException(id);
        }

        Veterinarian v = new Veterinarian(id, name);

        if (fields.length == 4) {
            for (String speciesId : fields[3].split(",")) {
                v.addResponsibility(_species.get(speciesId));
            }
        }

        _employees.put(id, v);

        _changed = true;
    }

    /**
     * Registers a new habitat caretaker
     * 
     * @param fields an array of strings with the caretaker's id, name and all the
     *               species he is responsible for
     * @throws EmployeeAlreadyExistsException if this caretaker's id has been
     *                                        registered previously
     */
    public void registerCaretaker(String... fields) throws EmployeeAlreadyExistsException {
        String id = fields[1];
        String name = fields[2];

        if (_employees.containsKey(id)) {
            throw new EmployeeAlreadyExistsException(id);
        }

        Caretaker c = new Caretaker(id, name);

        if (fields.length == 4) {
            for (String habitatId : fields[3].split(",")) {
                c.addResponsibility(_habitats.get(habitatId));
            }
        }

        _employees.put(id, c);

        _changed = true;
    }

    /**
     * Registers a new habitat
     * 
     * @param fields an array of strings with the habitat's id, name, area and all
     *               the tree in it
     * @throws HabitatAlreadyExistsException if this habitat's id has been
     *                                       registered previously
     */
    public void registerHabitat(String... fields) throws HabitatAlreadyExistsException {
        String id = fields[1];
        String name = fields[2];
        int area = Integer.parseInt(fields[3]);

        if (_habitats.containsKey(id)) {
            throw new HabitatAlreadyExistsException(id);
        }

        Habitat h = new Habitat(id, name, area);
        if (fields.length == 5) {
            for (String treeId : fields[4].split(",")) {
                h.addTree(_trees.get(treeId));
            }
        }

        _habitats.put(id, h);

        _changed = true;
    }

    /**
     * Registers a new vaccine
     * 
     * @param fields an array of strings with the caretaker's id, name and all the
     *               species it is made for
     * @throws VaccineAlreadyExistsException if this vaccine's id has been
     *                                       registered previously
     */
    public void registerVaccine(String... fields) throws VaccineAlreadyExistsException, NoSuchSpeciesException {
        String id = fields[1];
        String name = fields[2];

        if (_vaccines.containsKey(id)) {
            throw new VaccineAlreadyExistsException(id);
        }

        Vaccine v = new Vaccine(id, name);

        if (fields.length == 4) {
            for (String speciesId : fields[3].split(",")) {
                if (!_species.containsKey(speciesId)) {
                    throw new NoSuchSpeciesException(speciesId);
                }
                v.addAppropriateSpecies(_species.get(speciesId));
            }
        }

        _vaccines.put(id, v);

        _changed = true;
    }

    /**
     * Getter to get a list with all animals registered in the hotel
     * 
     * @return an unmodifiable collection with all registered animals
     */
    public Collection<Animal> animals() {
        return Collections.unmodifiableCollection(_animals.values());
    }

    /**
     * Getter to get a list with all employees registered in the hotel
     * 
     * @return an unmodifiable collection with all registered employees
     */
    public Collection<Employee> employees() {
        return Collections.unmodifiableCollection(_employees.values());
    }

    /**
     * Getter to get a list with all habitats registered in the hotel
     * 
     * @return an unmodifiable collection with all registered habitats
     */
    public Collection<Habitat> habitats() {
        return Collections.unmodifiableCollection(_habitats.values());
    }

    /**
     * Getter to get a list with all vaccines registered in the hotel
     * 
     * @return an unmodifiable collection with all registered vaccines
     */
    public Collection<Vaccine> vaccines() {
        return Collections.unmodifiableCollection(_vaccines.values());
    }

    /**
     * Getter to get a list with all vacccinations
     * 
     * @return an unmodifiable collection with all registered vaccines
     */
    public Collection<Tree> trees() {
        return Collections.unmodifiableCollection(_trees.values());
    }

    public Tree getTree(String treeId) {
        return _trees.get(treeId);
    }

    /**
     * Getter to get a list with all vacccinations
     * 
     * @return an unmodifiable collection with all registered vaccines
     */
    public Collection<VaccinationRecord> vaccinations() {
        return Collections.unmodifiableCollection(_vaccinationRecords);
    }

    public double calculateAnimalSatisfaction(String animalId) throws NoSuchAnimalException {
        if (!_animals.containsKey(animalId)) {
            throw new NoSuchAnimalException(animalId);
        }

        Animal animal = _animals.get(animalId);
        return animal.calculateSatisfaction();
    }

    /***
     * Allows for the transfer of an animal from its current habitat to a different
     * one
     * 
     * @param animalId  animal's unique identificator
     * @param habitatId habitat's unique identificator
     */
    public void transferAnimalToHabitat(String animalId, String habitatId)
            throws NoSuchAnimalException, NoSuchHabitatException {
        if (!_animals.containsKey(animalId)) {
            throw new NoSuchAnimalException(animalId);
        }
        if (!_habitats.containsKey(habitatId)) {
            throw new NoSuchHabitatException(habitatId);
        }

        Animal animal = _animals.get(animalId);
        Habitat newHabitat = _habitats.get(habitatId);

        animal.changeHabitat(newHabitat); // FIXME is it better to handle logic here or in func?

        _changed = true;
    }

    public void changeHabitatArea(String id, int area) throws NoSuchHabitatException {
        Habitat habitat = _habitats.get(id);
        if (habitat == null) {
            throw new NoSuchHabitatException(id);
        }

        habitat.changeArea(area);

        _changed = true;
    }

    public Tree plantTreeOnHabitat(String habitatId, String treeId)
            throws NoSuchHabitatException {
        Habitat habitat = _habitats.get(habitatId);
        if (habitat == null) {
            throw new NoSuchHabitatException(habitatId);
        }

        Tree tree = _trees.get(treeId);

        habitat.addTree(tree);

        _changed = true;

        return tree;
    }

    /**
     * Returns all trees from a given habitat.
     * 
     * @param habitatId habitat's unique identificator
     * @return collection with all trees from habitat
     * @throws NoSuchHabitatException if the given habitat ID does not map to an
     *                                existing habitat
     */
    public Collection<Tree> getTreesInHabitat(String habitatId) throws NoSuchHabitatException {
        Habitat habitat = _habitats.get(habitatId);

        if (habitat == null) {
            throw new NoSuchHabitatException(habitatId);
        }

        return habitat.getTrees();
    }

    public void changeHabitatInfluenceOverSpecies(String habitatId, String speciesId, String influence)
            throws NoSuchHabitatException, NoSuchSpeciesException {
        if (!_habitats.containsKey(habitatId)) {
            throw new NoSuchHabitatException(habitatId);
        }
        if (!_species.containsKey(speciesId)) {
            throw new NoSuchSpeciesException(speciesId);
        }

        Habitat habitat = _habitats.get(habitatId);
        switch (influence) {
            case "POS":
                habitat.changeInfluenceOverSpecies(speciesId, HabitatInfluence.POSITIVE);
                break;
            case "NEG":
                habitat.changeInfluenceOverSpecies(speciesId, HabitatInfluence.NEGATIVE);
                break;
            case "NEU":
                habitat.changeInfluenceOverSpecies(speciesId, HabitatInfluence.NEUTRAL);
                break;
        }

        _changed = true;
    }

    public void attributeResponsibility(String employeeId, String responsibilityId)
            throws NoSuchEmployeeException, NoSuchResponsibilityException {
        if (!_employees.containsKey(employeeId)) {
            throw new NoSuchEmployeeException(employeeId);
        }

        Employee employee = _employees.get(employeeId); // FIXME instanceof

        if (employee instanceof Veterinarian) {
            if (!_species.containsKey(responsibilityId)) {
                throw new NoSuchResponsibilityException(employeeId, responsibilityId);
            }
            Species species = _species.get(responsibilityId);
            Veterinarian vet = (Veterinarian) employee;

            vet.addResponsibility(species);
        } else if (employee instanceof Caretaker) {
            if (!_habitats.containsKey(responsibilityId)) {
                throw new NoSuchResponsibilityException(employeeId, responsibilityId);
            }
            Habitat habitat = _habitats.get(responsibilityId);
            Caretaker caretaker = (Caretaker) employee;

            caretaker.addResponsibility(habitat);
        }

        _changed = true;
    }

    public void revokeResponsibility(String employeeId, String responsibilityId)
            throws NoSuchEmployeeException, NoSuchResponsibilityException {
        if (!_employees.containsKey(employeeId)) {
            throw new NoSuchEmployeeException(employeeId);
        }

        Employee employee = _employees.get(employeeId); // FIXME how do I decide between employees?

        if (employee instanceof Veterinarian) {
            if (!_species.containsKey(responsibilityId)) {
                throw new NoSuchResponsibilityException(employeeId, responsibilityId);
            }
            Species species = _species.get(responsibilityId);
            Veterinarian vet = (Veterinarian) employee;

            vet.revokeResponsibility(species);
        } else if (employee instanceof Caretaker) {
            if (!_habitats.containsKey(responsibilityId)) {
                throw new NoSuchResponsibilityException(employeeId, responsibilityId);
            }
            Habitat habitat = _habitats.get(responsibilityId);
            Caretaker caretaker = (Caretaker) employee;

            caretaker.revokeResponsibility(habitat);
        }

        _changed = true;
    }

    public double calculateSatisfaction(String employeeId) throws NoSuchEmployeeException {
        if (!_employees.containsKey(employeeId)) {
            throw new NoSuchEmployeeException(employeeId);
        }

        Employee employee = _employees.get(employeeId);
        return employee.calculateSatisfaction();
    }

    public void vaccinateAnimal(String vetId, String animalId, String vaccineId) throws NoSuchVeterinarianException,
            NoSuchAnimalException, NoSuchVaccineException, UnableToVaccinateAnimalException,
            HarmfulVaccinationException {
        if (!_employees.containsKey(vetId) || !(_employees.get(vetId) instanceof Veterinarian)) { // FIXME instanceof
            throw new NoSuchVeterinarianException(vetId);
        }
        if (!_animals.containsKey(animalId)) {
            throw new NoSuchAnimalException(animalId);
        }
        if (!_vaccines.containsKey(vaccineId)) {
            throw new NoSuchVaccineException(vaccineId);
        }

        Veterinarian vet = (Veterinarian) _employees.get(vetId);
        Animal animal = _animals.get(animalId);
        Vaccine vaccine = _vaccines.get(vaccineId);

        vet.vaccinateAnimal(animal, vaccine);

        VaccinationRecord vaccination = vaccine.getLatestApplication();
        _vaccinationRecords.add(vaccination);

        if (vaccination.wasHarmful()) {
            throw new HarmfulVaccinationException(vaccineId, animalId);
        }

        _changed = true;
    }

    public Collection<Animal> getAnimalsInHabitat(String habitatId) throws NoSuchHabitatException {
        if (!_habitats.containsKey(habitatId)) {
            throw new NoSuchHabitatException(habitatId);
        }

        Habitat habitat = _habitats.get(habitatId);
        return Collections.unmodifiableCollection(habitat.getAnimals());
    }

    public Collection<VaccinationRecord> getMedicalActsByVeterinarian(String vetId) throws NoSuchVeterinarianException {
        if (!_employees.containsKey(vetId) || !(_employees.get(vetId) instanceof Veterinarian)) { // FIXME instanceof
            throw new NoSuchVeterinarianException(vetId);
        }

        List<VaccinationRecord> vaccinationsByVet = new LinkedList<VaccinationRecord>();
        for (VaccinationRecord vaccination : _vaccinationRecords) {
            if (vaccination.getVeterinarian().getID().equals(vetId)) {
                vaccinationsByVet.add(vaccination);
            }
        }

        return Collections.unmodifiableCollection(vaccinationsByVet);
    }

    public Collection<VaccinationRecord> getMedicalActsOnAnimal(String animalId) throws NoSuchAnimalException {
        if (!_animals.containsKey(animalId)) {
            throw new NoSuchAnimalException(animalId);
        }
        Animal animal = _animals.get(animalId);

        return animal.getVaccinationRecords();
    }

    public Collection<VaccinationRecord> getHarmfulMedicalActs() {
        List<VaccinationRecord> harmfulVaccinations = new LinkedList<VaccinationRecord>();
        for (VaccinationRecord vaccination : _vaccinationRecords) {
            if (vaccination.wasHarmful()) {
                harmfulVaccinations.add(vaccination);
            }
        }
        return Collections.unmodifiableCollection(harmfulVaccinations);
    }

    /**
     * Boolean value that indicates whether the program's state has been changed
     * since last save
     * 
     * @return true if the program has been changed since last save; false otherwise
     */
    public boolean hasChanged() {
        return _changed;
    }

    /**
     * Sets a boolean value to indicate whether the program's state has been changed
     * since last save
     * 
     * @param changed true or false, depending on whether the program's state has
     *                been changed since last save or not
     */
    public void setChanged(boolean changed) {
        _changed = changed;
    }

    /**
     * Sets the Hotel's season
     * 
     * @param s season to be set
     */
    public void setSeason(Season season) {
        _season = season;
    }

    /**
     * Advances the Hotel's season to the next one
     */
    public String advanceSeason() {
        _season.change();
        for (Tree t : _trees.values()) {
            t.age();
            t.updateBioCycleStage(_season);
            t.updateSeasonalMaintenanceDifficulty(_season);
        }

        _changed = true;

        return _season.toString();
    }

    public double calculateGlobalSatisfaction() {
        double globalSatisfaction = 0;
        for (Employee employee : _employees.values()) {
            globalSatisfaction += employee.calculateSatisfaction();
        }
        for (Animal animal : _animals.values()) {
            globalSatisfaction += animal.calculateSatisfaction();
        }

        return globalSatisfaction;
    }


}
