package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

public class DoShowHabitatsWithoutAnimals extends Command<Hotel> {

    public DoShowHabitatsWithoutAnimals(Hotel receiver) {
        super(Label.HABITATS_WITHOUT_ANIMALS, receiver);
    }

    @Override
    protected void execute() throws CommandException {
        _display.popup(_receiver.getHabitatsWithoutAnimals());
    }

    
}