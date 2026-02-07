package hva.app.vaccine;

import hva.Hotel;
import hva.app.exceptions.UnknownAnimalKeyException;
import hva.app.exceptions.UnknownVaccineKeyException;
import hva.app.exceptions.UnknownVeterinarianKeyException;
import hva.app.exceptions.VeterinarianNotAuthorizedException;
import hva.exceptions.NoSuchAnimalException;
import hva.exceptions.NoSuchVaccineException;
import hva.exceptions.NoSuchEmployeeException;
import hva.exceptions.NoSuchVeterinarianException;
import hva.exceptions.HarmfulVaccinationException;
import hva.exceptions.UnableToVaccinateAnimalException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoVaccinateAnimal extends Command<Hotel> {

    DoVaccinateAnimal(Hotel receiver) {
        super(Label.VACCINATE_ANIMAL, receiver);
        addStringField("vaccineId", Prompt.vaccineKey());
        addStringField("vetId", Prompt.veterinarianKey());
        addStringField("animalId", hva.app.animal.Prompt.animalKey());
    }

    @Override
    protected final void execute() throws CommandException {
        try {
            String vaccineId = stringField("vaccineId");
            String vetId = stringField("vetId");
            String animalId = stringField("animalId");

            _receiver.vaccinateAnimal(vetId, animalId, vaccineId);
        } catch (NoSuchVeterinarianException e) {
            throw new UnknownVeterinarianKeyException(e.getID());

        } catch (NoSuchAnimalException e){
            throw new UnknownAnimalKeyException(e.getID());

        } catch (NoSuchVaccineException e) {
            throw new UnknownVaccineKeyException(e.getID());

        } catch (UnableToVaccinateAnimalException e) {
            throw new VeterinarianNotAuthorizedException(e.getVetID(), e.getSpeciesID());

        } catch (HarmfulVaccinationException e) {
            _display.popup(Message.wrongVaccine(e.getVaccineID(), e.getAnimalID()));
        }
    }

}
