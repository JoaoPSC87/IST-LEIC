package hva;
import java.io.Serializable;

public class Winter implements Season, Serializable {
    private Hotel _hotel;

    public Winter(Hotel h) {
        _hotel = h;
    }

    public void change() {
        _hotel.setSeason(new Spring(_hotel));
    }

    public String biologicalCycle(DeciduousTree t) {
        return "SEMFOLHAS";
    }

    public int seasonalMaintenanceDifficulty(DeciduousTree t) {
        return 0;
    }

    public String biologicalCycle(ConiferousTree t) {
        return "LARGARFOLHAS";
    }

    public int seasonalMaintenanceDifficulty(ConiferousTree t) {
        return 2;
    }

    @Override
    public String toString() {
        return "3";
    }
}
