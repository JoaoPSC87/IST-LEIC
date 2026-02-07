package hva.app.employee;

import java.util.Arrays;
import java.util.List;

import hva.Hotel;
import hva.app.exceptions.DuplicateEmployeeKeyException;
import hva.exceptions.EmployeeAlreadyExistsException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoRegisterEmployee extends Command<Hotel> {

    DoRegisterEmployee(Hotel receiver) {
        super(Label.REGISTER_EMPLOYEE, receiver);
        addStringField("employeeId", Prompt.employeeKey());
        addStringField("employeeName", Prompt.employeeName());
        addStringField("employeeType", Prompt.employeeType()); 
    }

    @Override
    protected void execute() throws CommandException {
        try{
            String employeeId = stringField("employeeId");
            String employeeName = stringField("employeeName");
            String employeeType = stringField("employeeType");
        
            switch(employeeType) {
                case "VET":
                    _receiver.registerVeterinarian(new String[] {"VETERINÁRIO", employeeId, employeeName});
                    break;
                case "TRT":
                    _receiver.registerCaretaker(new String[] {"TRATADOR", employeeId, employeeName});
            }
        } catch (EmployeeAlreadyExistsException e) {
            throw new DuplicateEmployeeKeyException(e.getID());
        }
  
    } 

}
