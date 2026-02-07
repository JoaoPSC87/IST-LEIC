package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.NoResponsibilityException;
import hva.app.exceptions.UnknownEmployeeKeyException;
import hva.exceptions.NoSuchResponsibilityException;
import hva.exceptions.NoSuchEmployeeException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoRemoveResponsibility extends Command<Hotel> {

    DoRemoveResponsibility(Hotel receiver) {
        super(Label.REMOVE_RESPONSABILITY, receiver);
        addStringField("employeeId", Prompt.employeeKey()):
        addStringField("responsibilityId", Prompt.responsibilityKey());
    }

    @Override
    protected void execute() throws CommandException {
        try {

            String employeeId = stringField("employeeId");
            String responsibilityId = stringField("responsibilityId");
            _receiver.revokeResponsibility(employeeId, responsibilityId);
        }catch (NoSuchEmployeeException e){
            throw new UnknownEmployeeKeyException(e.getID());
        }catch (NoSuchResponsibilityException e) {
            throw new NoResponsibilityException(e.getEmployeeID(), e.getResponsibilityID());
        }
    }

}
