package hva.app.search;

public class Menu extends pt.tecnico.uilib.menus.Menu {

    public Menu(hva.Hotel receiver) {
        super(Label.TITLE, //
                new DoShowAnimalsInHabitat(receiver),
                new DoShowMedicalActsOnAnimal(receiver),
                new DoShowMedicalActsByVeterinarian(receiver),
                new DoShowWrongVaccinations(receiver),
                new DoShowAnimalsWithoutVaccines(receiver),
                new DoShowHabitatsWithoutTrees(receiver),
                new DoShowHabitatsWithoutAnimals(receiver),
                new DoShowVeterinariansWithoutAnimals(receiver),
                new DoShowKeepersWithoutHabitats(receiver)
        );
    }

}
