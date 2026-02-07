package hva.app.vaccine;

import hva.Hotel;
import hva.app.exceptions.DuplicateVaccineKeyException;
import hva.app.exceptions.UnknownSpeciesKeyException;
import hva.exceptions.VaccineAlreadyExistsException;
import hva.exceptions.NoSuchSpeciesException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoRegisterVaccine extends Command<Hotel> {

    DoRegisterVaccine(Hotel receiver) {
        super(Label.REGISTER_VACCINE, receiver);
        addStringField("id", Prompt.vaccineKey());
        addStringField("name", Prompt.vaccineName());
        addStringField("targetSpeciesId", Prompt.listOfSpeciesKeys());
    }

    @Override
    protected final void execute() throws CommandException {
        try {
            String id = stringField("id");
            String name = stringField("name");
            String targetSpeciesIds = stringField("targetSpeciesId");

            _receiver.registerVaccine(new String[] {"VACINA", id, name, targetSpeciesIds});
        } catch (VaccineAlreadyExistsException e){
            throw new DuplicateVaccineKeyException(e.getID());
        } catch (NoSuchSpeciesException e) {
            throw new UnknownSpeciesKeyException(e.getID());
        }
    }

}
