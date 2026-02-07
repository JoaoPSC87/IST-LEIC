package hva;

import hva.exceptions.*;
import java.io.*;

/**
 * Class that represents the hotel application.
 */
public class HotelManager {

    /** This is the current hotel. */
    private Hotel _hotel = new Hotel();
    private String _filename = "";

    // FIXME maybe add more fields if needed

    /**
     * Saves the serialized application's state into the file associated to the current network.
     *
     * @throws FileNotFoundException if for some reason the file cannot be created or opened.
     * @throws MissingFileAssociationException if the current network does not have a file.
     * @throws IOException if there is some error while serializing the state of the network to disk.
     */
    public void save() throws FileNotFoundException, MissingFileAssociationException, IOException {
       if (_filename == null | _filename.isEmpty()) {
        throw new MissingFileAssociationException();
       }

       try (ObjectOutputStream out = new ObjectOutputStream(
                new BufferedOutputStream(new FileOutputStream(_filename)))) {
            out.writeObject(_hotel);
        }
    }

    /**
     * Saves the serialized application's state into the file associated to the current network.
     *
     * @throws FileNotFoundException if for some reason the file cannot be created or opened.
     * @throws MissingFileAssociationException if the current network does not have a file.
     * @throws IOException if there is some error while serializing the state of the network to disk.
     */
    public void saveAs(String filename) throws FileNotFoundException, MissingFileAssociationException, IOException {
       _filename = filename;
       save();
    }

    /**
     * @param filename name of the file containing the serialized application's state
     *        to load.
     * @throws UnavailableFileException if the specified file does not exist or there is
     *         an error while processing this file.
     */
    public void load(String filename) throws UnavailableFileException {
        try (ObjectInputStream in = new ObjectInputStream(new BufferedInputStream(new FileInputStream(filename)))) {
            _hotel = (Hotel) in.readObject();
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            throw new UnavailableFileException(filename);
        }
    }

    /**
     * Read text input file.
     *
     * @param filename name of the text input file
     * @throws ImportFileException
     */
    public void importFile(String filename) throws ImportFileException {
        _hotel.importFile(filename);
    }

    public Hotel getHotel() {
        return _hotel;
    }

    public boolean hasChanged() {
        return _hotel.hasChanged();
    }

    public void resetChangedStatus() {
        _hotel.setChanged(false);
    }

    public void reset() {
        _hotel = new Hotel();
        _filename = "";
    }

    public String advanceSeason() {
        return _hotel.advanceSeason();
    }

    public double calculateGlobalSatisfaction() {
        return _hotel.calculateGlobalSatisfaction();
    }

}
