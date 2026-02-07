package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

public class DoShowVeterinariansWithoutAnimals extends Command<Hotel>{

    public DoShowVeterinariansWithoutAnimals(Hotel receiver) {
        super(Label.VETERINARIANS_WITHOUT_ANIMALS, receiver);
    }

    @Override
    protected void execute() extends CommandException {
        _display.popup(_receiver.getVeterinariansWithoutAnimals());
    }

    
}