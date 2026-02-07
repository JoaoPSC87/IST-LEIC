package hva;

public enum HabitatInfluence {
    POSITIVE(20),
    NEGATIVE(-20),
    NEUTRAL(0);

    private int _influence;

    private HabitatInfluence(int influence) {
        _influence = influence;
    }

    public int influence() {
        return _influence;
    }
}