package hva;

public interface Season {
    public abstract void change();
    public abstract String biologicalCycle(ConiferousTree t);
    public abstract int seasonalMaintenanceDifficulty(ConiferousTree t);
    public abstract String biologicalCycle(DeciduousTree t);
    public abstract int seasonalMaintenanceDifficulty(DeciduousTree t);
}