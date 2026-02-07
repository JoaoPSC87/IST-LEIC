package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

public class DoShowHabitatsWithoutTrees extends Command<Hotel> {

    public DoShowHabitatsWithoutTrees(Hotal receiver) {
        super(Label.HABITATS_WITHOUT_TREES, receiver);
    }

    @Override
    protected void execute() throws CommandException {
        _display.popup(_receiver._habitatsWithoutTrees());
    }
    

}