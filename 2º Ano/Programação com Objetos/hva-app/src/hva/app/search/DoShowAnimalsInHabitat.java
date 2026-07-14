package hva.app.search;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.exceptions.NoSuchHabitatException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoShowAnimalsInHabitat extends Command<Hotel> {

    DoShowAnimalsInHabitat(Hotel receiver) {
        super(Label.ANIMALS_IN_HABITAT, receiver);
        addStringField("habitatId", hva.app.habitat.Prompt.habitatKey());
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String habitatId = stringField("habitatId");
            _display.popup((_receiver.getAnimalsInHabitat(habitatId)));
        } catch (NoSuchHabitatException e) {
            throw new UnknownHabitatKeyException(e.getID());
        }
    }

}
