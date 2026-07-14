package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.DuplicateHabitatKeyException;
import hva.exceptions.HabitatAlreadyExistsException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoRegisterHabitat extends Command<Hotel> {

    DoRegisterHabitat(Hotel receiver) {
        super(Label.REGISTER_HABITAT, receiver);
        addStringField("id", Prompt.habitatKey());
        addStringField("name", Prompt.habitatName());
        addStringField("area", Prompt.habitatArea());
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String id = stringField("id");
            String name = stringField("name");
            String area = stringField("area");
        
            _receiver.registerHabitat(new String[] {"HABITAT", id, name, area});
        } catch (HabitatAlreadyExistsException e) {
            throw new DuplicateHabitatKeyException(e.getID());
        }

    }

}
