package hva.app.search;

import hva.Hotel;
import hva.app.exceptions.UnknownAnimalKeyException;
import hva.exceptions.NoSuchAnimalException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoShowMedicalActsOnAnimal extends Command<Hotel> {

    DoShowMedicalActsOnAnimal(Hotel receiver) {
        super(Label.MEDICAL_ACTS_ON_ANIMAL, receiver);
        addStringField("animalId", hva.app.animal.Prompt.animalKey());
    }

    @Override
    protected void execute() throws CommandException {
        try{
            String animalId = stringField("animalId");
            _display.popup(_receiver.getMedicalActsOnAnimal(animalId));
        } catch (NoSuchAnimalException e) {
            throw new UnknownAnimalKeyException(e.getID());
        }
    }

}
