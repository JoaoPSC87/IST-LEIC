package hva;

import java.io.Serializable;

// HACK This abstraction exists basically so that vets and caretakers may share the same map in hotel. May require further thought.
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
