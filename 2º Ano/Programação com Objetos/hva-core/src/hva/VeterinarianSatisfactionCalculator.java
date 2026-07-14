package hva;

import java.io.Serializable;

public class VeterinarianSatisfactionCalculator implements EmployeeSatisfactionCalculator, Serializable {
    public double compute(Employee e) {
        return 20 - workload(e);
    }

    private double workload(Employee e) {
        double workload = 0;
        Veterinarian vet = (Veterinarian) e;
        for (Species species : vet.getAssignedSpecies()) {
            workload += (double) species.getPopulation() / (species.getAvailableVets());
        }
        return workload;
    }
}
