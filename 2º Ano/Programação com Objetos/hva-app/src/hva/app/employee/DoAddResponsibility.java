package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.NoResponsibilityException;
import hva.app.exceptions.UnknownAnimalKeyException;
import hva.app.exceptions.UnknownEmployeeKeyException;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.exceptions.NoSuchResponsibilityException;
import hva.exceptions.NoSuchAnimalException;
import hva.exceptions.NoSuchEmployeeException;
import hva.exceptions.NoSuchHabitatException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoAddResponsibility extends Command<Hotel> {

    DoAddResponsibility(Hotel receiver) {
        super(Label.ADD_RESPONSABILITY, receiver);
        addStringField("employeeId", Prompt.employeeKey());
        addStringField("responsibilityId", Prompt.responsibilityKey());    
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String employeeId = stringField("employeeId");
            String responsibilityId = stringField("responsibilityId");
            _receiver.attributeResponsibility(employeeId, responsibilityId);
        } catch (NoSuchEmployeeException e) {
            throw new UnknownEmployeeKeyException(e.getID());
        } catch (NoSuchResponsibilityException e) {
            throw new NoResponsibilityException(e.getEmployeeID(), e.getResponsibilityID());
        }
    }
}
