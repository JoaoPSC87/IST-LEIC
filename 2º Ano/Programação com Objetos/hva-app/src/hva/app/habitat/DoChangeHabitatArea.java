package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.exceptions.NoSuchHabitatException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoChangeHabitatArea extends Command<Hotel> {

    DoChangeHabitatArea(Hotel receiver) {
        super(Label.CHANGE_HABITAT_AREA, receiver);
        addStringField("habitatId", Prompt.habitatKey());
        addIntegerField("area", Prompt.habitatArea());
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String habitatId = stringField("habitatId");
            int area = integerField("area");
            _receiver.changeHabitatArea(habitatId, area);
        } catch (NoSuchHabitatException e) {
            throw new UnknownHabitatKeyException(e.getID());
        }
    }

}
