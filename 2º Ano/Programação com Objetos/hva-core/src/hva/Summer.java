package hva;
import java.io.Serializable;

public class Summer implements Season, Serializable {
    private Hotel _hotel;

    public Summer(Hotel h) {
        _hotel = h;
    }

    public void change() {
        _hotel.setSeason(new Autumn(_hotel));
    }

    public String biologicalCycle(DeciduousTree t) {
        return "COMFOLHAS";
    }

    public int seasonalMaintenanceDifficulty(DeciduousTree t) {
        return 2;
    }

    public String biologicalCycle(ConiferousTree t) {
        return "COMFOLHAS";
    }

    public int seasonalMaintenanceDifficulty(ConiferousTree t) {
        return 1;
    }

    @Override
    public String toString() {
        return "1";
    }
}
