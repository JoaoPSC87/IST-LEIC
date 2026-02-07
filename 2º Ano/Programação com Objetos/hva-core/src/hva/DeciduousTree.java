package hva;

public class DeciduousTree extends Tree {

    public DeciduousTree(String id, String name, int age, int maintenanceDifficulty) {
        super(id, name, age, maintenanceDifficulty);
    }

    public void updateBioCycleStage(Season s) {
        setBioCycleStage(s.biologicalCycle(this));
    }

    public void updateSeasonalMaintenanceDifficulty(Season season) {
        setSeasonalMaintenanceDifficulty(season.seasonalMaintenanceDifficulty(this));
    }

    @Override
    public String toString() {
        return "ÁRVORE|" + getID() + "|" + getName() + "|" + getAge() + "|" + getMaintenanceDifficulty() + 
                "|" + "CADUCA" + "|" + getBioCycleStage();
    }
}