package hva;

import java.io.Serializable;

public class CaretakerSatisfactionCalculator implements EmployeeStatisfactionCalculator, Serializable {

    public double compute(Employee e) {
        return 300 - workload(e);
    }

    private double habitatWorkload(Habitat h) {
        double habitatWorkload = h.getArea() + 3 * h.getPopulation();

        for (Tree tree : h.getTrees()) {
            habitatWorkload += tree.getMaitenanceDifficulty() * tree.getSeasonalMaitenanceDifficulty() + Math.log(tree.getAge() + 1);
        }

        return habitatWorkload;
    }

    private double workload(Employee e) {
        double workload = 0;
        Caretaker caretaker = (Caretaker) e;
        for(Habitat habitat : caretaker.getAssignedHabitats()) {
            workload += habitatWorkload(habitat) / habitat.getAvailableCaretakers();
        }

        return workload;
    }
}