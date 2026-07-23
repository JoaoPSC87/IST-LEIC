package pt.tecnico.blockchainist.node;

import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Map;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import pt.tecnico.blockchainist.node.domain.NodeState;
import pt.tecnico.blockchainist.utils.Debug;
import io.grpc.ServerInterceptors;
import pt.tecnico.blockchainist.node.NodeDelayInterceptor;
import java.util.concurrent.CompletableFuture;

public class NodeMain {
    public static void main(String[] args) {
        
        if (System.getProperty("debug") != null) Debug.enable();
        
        if (args.length != 3) {
            System.err.println("Argument(s) missing!");
            printUsage();
            return;
        }
        
        int nodePort = -1;
        
        try {
            nodePort = Integer.parseInt(args[0]);
        } catch (NumberFormatException e) {
            System.err.println("Invalid port (" + args[0] + ")");
            printUsage();
            return;
        }
        
        if (nodePort > 65535 || nodePort < 0) {
            System.err.println("Port number out of range (0-65535): " + nodePort);
            printUsage();
            return;
        }

        String orgName = args[1];

        String[] split = args[2].split(":");
        if (split.length != 2) {
            System.err.println("Invalid argument: " + args[2]);
            printUsage();
            return;
        }

        String sequencerHost = split[0];
        int sequencerPort = -1;
        try {
            sequencerPort = Integer.parseInt(split[1]);
        } catch (NumberFormatException e) {
            System.err.println("Invalid port (" + split[1] + ") in argument: " + args[2]);
            printUsage();
            return;
        }
        if (sequencerPort > 65535 || sequencerPort < 0) {
            System.err.println("Port number out of range (0-65535): " + sequencerPort);
            printUsage();
            return;
        }

        // Connect to Sequencer
        ManagedChannel sequencerChannel = ManagedChannelBuilder.forAddress(sequencerHost, sequencerPort)
                .usePlaintext()
                .build();

        NodeKeyManager keyManager;
        try {
            keyManager = new NodeKeyManager("seq/Seq.pub");
        } catch (Exception e) {
            throw new RuntimeException("ERROR: Failed to load sequencer key " + e.getMessage());
        }

        // Initialize node state and pending requests map        
        NodeState state = new NodeState(keyManager); 
        Map<String, CompletableFuture<Void>> pendingRequests = new ConcurrentHashMap<>(); // Map to track pending requests by their transaction ID, allowing the node to respond to the correct client once the transaction is processed.

        try {
            Server server = ServerBuilder.forPort(nodePort)
                    .addService(ServerInterceptors.intercept (new NodeServiceImpl(state, keyManager, sequencerChannel, pendingRequests, orgName), new NodeDelayInterceptor()))
                    .build()
                    .start();

            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                try {
                    server.shutdown().awaitTermination(5, TimeUnit.SECONDS);
                    sequencerChannel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
                } catch (InterruptedException e) { 
                    // ignore
                }
            }));

            System.out.println("Node started on port " + nodePort + " (org: " + orgName + ")");

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
        System.err.println("Usage: mvn exec:java -Dexec.args=\"<port> <organization> [<host>:<port>]\"");
    }
}
