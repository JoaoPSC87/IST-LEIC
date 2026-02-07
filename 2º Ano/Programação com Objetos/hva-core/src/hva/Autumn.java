package hva;

import java.io.Serializable;

public class Autumn implements Season, Serializable {
    private Hotel _hotel;

    public Autumn(Hotel h) {
        _hotel = h;
    }

    public void change() {
        _hotel.setSeason(new Winter(_hotel));
    }

    public String biologicalCycle(DeciduousTree t) {
        return "LARGARFOLHAS";
    }

    public int seasonMaintenanceDifficulty(DeciduousTree t) {
        return 5;
    }

    public String biologicalCycle(ConiferousTree t) {
        return "COMFOLHAS";
    }

    public int seasonMaintenanceDifficulty(ConiferousTree t) {
        return 1;
    }

    @Override
    public String toString() {
        return "2";
    }
}