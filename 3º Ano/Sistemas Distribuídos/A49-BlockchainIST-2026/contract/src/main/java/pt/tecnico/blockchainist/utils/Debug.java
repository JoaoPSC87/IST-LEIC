package pt.tecnico.blockchainist.utils;

public class Debug {
    private static boolean enabled = false;

    public static void enable() {
        enabled = true;
    }

    public static void log(String message){
        if (enabled)
            System.err.println("DEBUG: " + message);
    }
}