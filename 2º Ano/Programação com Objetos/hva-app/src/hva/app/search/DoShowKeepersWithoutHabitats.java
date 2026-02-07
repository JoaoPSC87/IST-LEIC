package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

public class DoShowKeepersWithoutHabitats extends Command<Hotel> {

    public DoShowKeepersWithoutHabitats(Hotel receiver) {
        super(Label.KEEPERS_WITHOUT_HABITATS, receiver);
    }

    @Override
    protected void execute() throws CommandException {
        _display.popup(_receiver.getKeepersWithoutHabitats());
    }

    
}