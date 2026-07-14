package hva;

import java.io.Serializable;

public class CaretakerSatisfactionCalculator implements EmployeeSatisfactionCalculator, Serializable {
    /**
     * satisfação(t) = 300 - trabalho(t)
        trabalho(t) = Σ (trabalho_no_habitat(h) / número_de_tratadores_podem_cuidar_habitat(h)), h ∈ habitats(t)
        trabalho_no_habitat(h) = área(h) + 3 * população(h) + Σ esforço_limpeza(a), a ∈ árvores(h)
     */
    
    public double compute(Employee e) {
        return 300 - workload(e);
    }

    private double habitatWorkload(Habitat h) {
        double habitatWorkload = h.getArea() + 3 * h.getPopulation();

        for (Tree tree : h.getTrees()) {
            habitatWorkload += tree.getMaintenanceDifficulty() * tree.getSeasonalMaintenanceDifficulty() * Math.log(tree.getAge() + 1);
        }

        return habitatWorkload;
    }

    private double workload(Employee e) {
        double workload = 0;
        Caretaker caretaker = (Caretaker) e;
        for (Habitat habitat : caretaker.getAssignedHabitats()) {
            workload += habitatWorkload(habitat) / habitat.getAvailableCaretakers();
        }

        return workload;
    }
}
