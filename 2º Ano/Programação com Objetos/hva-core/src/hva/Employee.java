package hva;

import java.io.Serializable;

public abstract class Employee implements Serializable {
    private String _id;
    private String _name;

    public Employee(String id, String name) {
        _id = id;
        _name = name;
    }

    public String getID() {
        return _id;
    }

    public String getName() {
        return _name;
    }

    public abstract double calculateSatisfaction();
}