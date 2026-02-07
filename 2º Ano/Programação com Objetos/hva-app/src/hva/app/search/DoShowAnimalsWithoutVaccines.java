package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

public class DoShowAnimalsWithoutVaccines extends Command<Hotel> {

    public DoShowAnimalsWithoutVaccines(Hotel receiver) {
        super(Label.ANIMALS_WITHOUT_VACCINES, receiver);
    }

    @Override
    protected void execute() throws CommandException {
        _display.popup(_receiver._animalsWithoutVaccines());
    }
    
}