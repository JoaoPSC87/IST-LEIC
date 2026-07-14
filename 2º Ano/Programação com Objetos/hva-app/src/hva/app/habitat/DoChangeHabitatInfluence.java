package hva.app.habitat;

import java.util.Arrays;
import java.util.List;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.app.exceptions.UnknownSpeciesKeyException;
import hva.exceptions.NoSuchHabitatException;
import hva.exceptions.NoSuchSpeciesException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoChangeHabitatInfluence extends Command<Hotel> {

    DoChangeHabitatInfluence(Hotel receiver) {
        super(Label.CHANGE_HABITAT_INFLUENCE, receiver);
        addStringField("habitatId", Prompt.habitatKey());
        addStringField("speciesId", hva.app.animal.Prompt.speciesKey());
        addOptionField("influence", Prompt.habitatInfluence(), new String[] {"POS", "NEU", "NEG"});
    }

    @Override
    protected void execute() throws CommandException {
        try {
            String habitatId = stringField("habitatId");
            String speciesId = stringField("speciesId");
            String influence = optionField("influence");

            _receiver.changeHabitatInfluenceOverSpecies(habitatId, speciesId, influence);
        } catch (NoSuchHabitatException e) {
            throw new UnknownHabitatKeyException(e.getID());
        } catch (NoSuchSpeciesException e) {
            throw new UnknownSpeciesKeyException(e.getID());
        }
    }

}
