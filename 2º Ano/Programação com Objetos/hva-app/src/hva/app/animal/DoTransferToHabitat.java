package hva.app.animal;

import hva.Hotel;
import hva.app.exceptions.DuplicateAnimalKeyException;
import hva.app.exceptions.UnknownAnimalKeyException;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.exceptions.NoSuchAnimalException;
import hva.exceptions.NoSuchHabitatException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoTransferToHabitat extends Command<Hotel> {

    DoTransferToHabitat(Hotel hotel) {
        super(Label.TRANSFER_ANIMAL_TO_HABITAT, hotel);
        addStringField("animalId", Prompt.animalKey());
        addStringField("habitatId", hva.app.habitat.Prompt.habitatKey());
    }

    @Override
    protected final void execute() throws CommandException {
        try {
            String animalId = stringField("animalId");
            String habitatId = stringField("habitatId");
            _receiver.transferAnimalToHabitat(animalId, habitatId);
        } catch (NoSuchAnimalException e) {
            throw new UnknownAnimalKeyException(e.getID());
        } catch (NoSuchHabitatException e) {
            throw new UnknownHabitatKeyException(e.getID());
        }
    }

}
