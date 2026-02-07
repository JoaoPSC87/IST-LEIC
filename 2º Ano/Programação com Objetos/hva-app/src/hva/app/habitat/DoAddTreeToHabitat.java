package hva.app.habitat;

import java.util.Arrays;
import java.util.List;

import hva.Hotel;
import hva.Tree;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.app.exceptions.DuplicateTreeKeyException;
import hva.exceptions.NoSuchHabitatException;
import hva.exceptions.TreeAlreadyExistsException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

class DoAddTreeToHabitat extends Command<Hotel> {

    DoAddTreeToHabitat(Hotel receiver) {
        super(Label.ADD_TREE_TO_HABITAT, receiver);
        addStringField("habitatId", Prompt.habitatKey());
        addStringField("treeId", Prompt.treeKey());
        addStringField("treeName", Prompt.treeName());
        addStringField("treeAge", Prompt.treeAge());
        addStringField("treeMaintenanceDifficulty", Prompt.treeDifficulty());
        addOptionField("treeType", Prompt.treeType(), new String[] {"CADUCA", "PERENE"}); 
    }

    @Override
    protected void execute() throws CommandException {
        try{
            String habitatId = stringField("habitatId");
            String treeId = stringField("treeId");
            String treeName = stringField("treeName");
            String treeAge = stringField("treeAge");
            String treeMaintenanceDifficulty = stringField("treeMaintenanceDifficulty");
            String treeType = optionField("treeType");

            _receiver.registerTree(new String[] {"ÁRVORE", treeId, treeName, treeAge, treeMaintenanceDifficulty, treeType});
            _display.popup(_receiver.plantTreeOnHabitat(habitatId, treeId));
        } catch (TreeAlreadyExistsException e) {
            throw new DuplicateTreeKeyException(e.getID());
        } catch (NoSuchHabitatException e){
            throw new UnknownHabitatKeyException(e.getID());
        }
    }
}
