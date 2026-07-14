package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.UnknownEmployeeKeyException;
import hva.exceptions.NoSuchEmployeeException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoShowSatisfactionOfEmployee extends Command<Hotel> {

    DoShowSatisfactionOfEmployee(Hotel receiver) {
        super(Label.SHOW_SATISFACTION_OF_EMPLOYEE, receiver);
        addStringField("employeeId", Prompt.employeeKey());

    }

    @Override
    protected void execute() throws CommandException {        
        try {
            String employeeId = stringField("employeeId");
            double satisfaction = _receiver.calculateSatisfaction(employeeId);
            _display.popup(Math.round(satisfaction));
        } catch (NoSuchEmployeeException e) {
            throw new UnknownEmployeeKeyException(e.getID());
        }
    }

}
