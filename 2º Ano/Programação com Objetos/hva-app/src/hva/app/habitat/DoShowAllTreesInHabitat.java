package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.exceptions.NoSuchHabitatException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoShowAllTreesInHabitat extends Command<Hotel> {

    DoShowAllTreesInHabitat(Hotel receiver) {
        super(Label.SHOW_TREES_IN_HABITAT, receiver);
        addStringField("habitatId", Prompt.habitatKey());
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String habitatId = stringField("habitatId");
            _display.popup(_receiver.getTreesInHabitat(habitatId));
        } catch (NoSuchHabitatException e) {
            throw new UnknownHabitatKeyException(e.getID());
        }
    }

}
