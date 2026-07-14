package hva;

public class ConiferousTree extends Tree {

    public ConiferousTree(String id, String name, int age, int maintenanceDifficulty) {
        super(id, name, age, maintenanceDifficulty);
    }

    void updateBioCycleStage(Season season) {
        setBioCycleStage(season.biologicalCycle(this));
    }

    void updateSeasonalMaintenanceDifficulty(Season season) {
        setSeasonalMaintenanceDifficulty(season.seasonalMaintenanceDifficulty(this));
    }


    @Override
    public String toString() {
        return "ÁRVORE|" + getID() + "|" + getName() + "|" + getAge() + "|" + getMaintenanceDifficulty() +
               "|" + "PERENE" + "|" + getBioCycleStage();
    }
}
