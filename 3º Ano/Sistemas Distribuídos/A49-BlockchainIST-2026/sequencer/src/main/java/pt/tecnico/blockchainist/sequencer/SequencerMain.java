package pt.tecnico.blockchainist.sequencer;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import pt.tecnico.blockchainist.sequencer.domain.SequencerState;
import pt.tecnico.blockchainist.utils.Debug;

public class SequencerMain {
    public static void main(String[] args) {
        
        if (System.getProperty("debug") != null) Debug.enable();

        if (args.length < 1 || args.length > 3) {
            System.err.println("Argument(s) missing!");
            printUsage();
            return;
        }
        
        int sequencerPort = -1;
        
        try {
            sequencerPort = Integer.parseInt(args[0]);
        
        } catch (NumberFormatException e) {
            System.err.println("Invalid port (" + args[0] + ")");
            printUsage();
            return;
        }
        
        if (sequencerPort > 65535 || sequencerPort < 0) {
            System.err.println("Port number out of range (0-65535): " + sequencerPort);
            printUsage();
            return;
        }

        int maxTransactions = 4; // N default
        int blockTimeout = 5;   // T default (seconds)
        if(args.length >= 2) maxTransactions = Integer.parseInt(args[1]);
        if(args.length >= 3) blockTimeout = Integer.parseInt(args[2]);

		try {
            SequencerState state = new SequencerState(maxTransactions, blockTimeout);
            Server server = ServerBuilder.forPort(sequencerPort)
			        .addService(new SequencerServiceImpl(state))
			        .build()
			        .start();

            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                try {
                    server.shutdown().awaitTermination(5, TimeUnit.SECONDS);
                } catch (InterruptedException e) { 
                    // ignore
                }
            }));

            System.out.println("Sequencer started on port " + sequencerPort);
            server.awaitTermination();
		} catch (IOException e) {
            System.err.println("Failed to start server: " + e.getMessage());
		} catch (InterruptedException e) {
            System.err.println("Server interrupted: " + e.getMessage());
		} catch (RuntimeException e) {
            System.err.println(e.getMessage());
        }
    }

    private static void printUsage() {
        System.err.println("Usage: mvn exec:java -Dexec.args=\"<sequencerPort> [<maxTransactions>] [<blockTimeout>]\"");
    }
}