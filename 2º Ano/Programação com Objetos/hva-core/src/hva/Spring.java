package hva;
import java.io.Serializable;;

public class Spring implements Season, Serializable {
    private Hotel _hotel;

    public Spring(Hotel h) {
        _hotel = h;
    }

    public void change() {
        _hotel.setSeason(new Summer(_hotel));
    }

    public String biologicalCycle(DeciduousTree t) {
        return "GERARFOLHAS";
    }

    public int seasonalMaintenanceDifficulty(DeciduousTree t) {
        return 1;
    }

    public String biologicalCycle(ConiferousTree t) {
        return "GERARFOLHAS";
    }

    public int seasonalMaintenanceDifficulty(ConiferousTree t) {
        return 1;
    }

    @Override
    public String toString() {
        return "0";
    }
}
