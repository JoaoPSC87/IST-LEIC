package hva.app.search;

import hva.Hotel;
import hva.app.exceptions.UnknownVeterinarianKeyException;
import hva.exceptions.NoSuchVeterinarianException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoShowMedicalActsByVeterinarian extends Command<Hotel> {

    DoShowMedicalActsByVeterinarian(Hotel receiver) {
        super(Label.MEDICAL_ACTS_BY_VET, receiver);
        addStringField("vetId", hva.app.employee.Prompt.employeeKey());
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String vetId = stringField("vetId");
            _display.popup(_receiver.getMedicalActsByVeterinarian(vetId));
        } catch (NoSuchVeterinarianException e) {
            throw new UnknownVeterinarianKeyException(e.getID());
        }
    }

}
