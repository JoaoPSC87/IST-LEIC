package pt.tecnico.blockchainist.client;

import java.util.ArrayList;

import pt.tecnico.blockchainist.client.grpc.ClientNodeService;
import pt.tecnico.blockchainist.utils.Debug;

public class ClientMain {

    public static void main(String[] args) {
        
        if (System.getProperty("debug") != null) Debug.enable();
        
        System.out.println(ClientMain.class.getSimpleName());

        // check arguments
        if (args.length < 1) {
            System.err.println("Argument(s) missing!");
            printUsage();
            return;
        }

        ClientKeyManager keyManager = new ClientKeyManager();

        // parse arguments
        ArrayList<ClientNodeService> nodes = new ArrayList<>(args.length);
        for (String arg : args) {
            String[] split = arg.split(":");
            if (split.length != 3) {
                System.err.println("Invalid argument: " + arg);
                printUsage();
                return;
            }
            String host = split[0];
            int port = -1;
            try {
                port = Integer.parseInt(split[1]);
            } catch (NumberFormatException e) {
                System.err.println("Invalid port (" + split[1] + ") in argument: " + arg);
                printUsage();
                return;
            }
            if (port > 65535 || port < 0) {
                System.err.println("Port number out of range (0-65535): " + port);
                printUsage();
                return;
            }
            String organization = split[2];
            nodes.add(new ClientNodeService(host, port, organization, keyManager));
        }
       
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            for (var node: nodes) {
                try {
                    node.shutdown();
                } catch (InterruptedException e) { 
                    // do nothing
                }
            }
        }));

        CommandProcessor processor = new CommandProcessor(nodes);
        processor.userInputLoop();
    }

    private static void printUsage() {
        System.err.println("Usage: mvn exec:java -Dexec.args=\"<host>:<port>:<organization> [<host>:<port>:<organization> ...]\"");
    }
}
