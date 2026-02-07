package hva.app.animal;

import hva.Hotel;
import hva.Species;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import hva.app.exceptions.DuplicateAnimalKeyException;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.exceptions.AnimalAlreadyExistsException;
import hva.exceptions.NoSuchHabitatException;
import hva.exceptions.NoSuchSpeciesException;
import hva.exceptions.SpeciesAlreadyExistsException;

class DoRegisterAnimal extends Command<Hotel> {

    DoRegisterAnimal(Hotel receiver) {
        super(Label.REGISTER_ANIMAL, receiver);
        addStringField("animalId", Prompt.animalKey());
        addStringField("animalName", Prompt.animalName());
        addStringField("speciesId", Prompt.speciesKey());
        addStringField("habitatId", hva.app.habitat.Prompt.habitatKey());
    }

    @Override
    protected final void execute() throws CommandException {
       String animalId = stringField("animalId");
       String animalName = stringField("animalName");
       String speciesId = stringField("speciesId");
       String habitatId = stringField("habitatId");

       try {
        _receiver.registerAnimal(new String[] {"ANIMAL", animalId, animalName, speciesId, habitatId});
       } catch (NoSuchSpeciesException e) {
        try {
            String speciesName = Form.requestString(Prompt.speciesName());
            _receiver.registerSpecies(nem String[] {"ESPÉCIE", speciesId, speciesName});
            _receiver.registerAnimal(new String[] {"ANIMAL", animalId, animalName, speciesId, habitatId});
        } catch (SpeciesAlreadyExistsException | NoSuchSpeciesException e1) {
            e1.printStackTrace(); // Unreachable!
        } catch (NoSuchHabitatException e1) {
            throw new UnknownHabitatKeyException(e1.getID());
        } catch (AnimalAlreadyExistsException e1) {
            throw new DuplicateAnimalKeyException(e1.getID();)
        }
       } catch (NoSuchHabitatException e) {
        throw new UnknownHabitatKeyException(e.getID());
       } catch (AnimalAlreadyExistsException e) {
        throw new DuplicateAnimalKeyException(e.getID());
       }
    }
}
