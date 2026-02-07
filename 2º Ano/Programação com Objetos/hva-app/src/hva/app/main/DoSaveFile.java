package hva.app.main;

import hva.HotelManager;
import hva.exceptions.MissingFileAssociationException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import java.io.FileNotFoundException;
import java.io.IOException;


class DoSaveFile extends Command<HotelManager> {
    DoSaveFile(HotelManager receiver) {
        super(Label.SAVE_FILE, receiver, r -> r.getHotel() != null);
    }

    @Override
    protected final void execute() {
    	try {
            if(_receiver.hasChanged()) {
                _receiver.save();
                _receiver.resetChangedStatus();
            }
        } catch (MissingFileAssociationException e) {
            try {
                _receiver.saveAs(Form.requestString(Prompt.newSaveAs()));
                _receiver.resetChangedStatus();
            } catch (IOException | MissingFileAssociationException e1) {
                e1.printStackTrace(); // Unreachable!
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
