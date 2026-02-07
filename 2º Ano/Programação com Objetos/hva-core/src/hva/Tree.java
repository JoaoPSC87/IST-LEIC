package hva;

import java.io.Serializable;

public abstract class Tree implements Serializable {
    private String _id;
    private String _name;
    private double _age;
    private int _maintenanceDifficulty;
    private String _bioCycleStage;
    private int _seasonalMaintenanceDifficulty;

    public Tree(String id, String name, double age, int maintenanceDifficulty) {
        _id = id;
        _name = name;
        _age = age;
        _maintenanceDifficulty = maintenanceDifficulty;
    }

    public abstract void updateBioCycleStage(Season season);
    public abstract void updateSeasonalMaintenanceDifficulty(Season season);

    public void age() {
        _age = _age + 0.25;
    }

    public String getID() {
        return _id;
    }

    public String getName() {
        return _name;
    }

    public int getAge() {
        return (int) Math.floor(_age);
    }

    public int getMaintenanceDifficulty() {
        return _maintenanceDifficulty;
    }

    public String getBioCycleStage() {
        return _bioCycleStage;
    }

    public int getSeasonalMaintenanceDifficulty() {
        return _seasonalMaintenanceDifficulty;
    }

    public void setBioCycleStage(String bioCycleStage) {
        _bioCycleStage = bioCycleStage;
    }

    public void setSeasonalMaintenanceDifficulty(int seasonalMaintenanceDifficulty) {
        _seasonalMaintenanceDifficulty = seasonalMaintenanceDifficulty;
    }
}