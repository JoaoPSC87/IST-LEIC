package hva.app.animal;

import hva.Hotel;
import hva.app.exceptions.UnknownAnimalKeyException;
import hva.exceptions.NoSuchAnimalException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoShowSatisfactionOfAnimal extends Command<Hotel> {

    DoShowSatisfactionOfAnimal(Hotel receiver) {
        super(Label.SHOW_SATISFACTION_OF_ANIMAL, receiver);
        addStringField("animalId", Prompt.animalKey());
    }

    @Override
    protected final void execute() throws CommandException {
        try {
            String animalId = stringField("animalId");
            _display.popup(Math.round(_receiver.calculateAnimalSatisfaction(animalId)));
        } catch (NoSuchAnimalException e) {
            throw new UnknownAnimalKeyException(e.getID());
        }
    }

}
