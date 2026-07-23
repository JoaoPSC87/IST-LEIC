package pt.tecnico.blockchainist.node;

import java.io.InputStream;
import java.security.KeyFactory;
import java.security.PublicKey;
import java.security.spec.X509EncodedKeySpec;

public class NodeKeyManager {
    private PublicKey sequencerKey;

    public NodeKeyManager(String keyPath) throws Exception {
        this.sequencerKey = loadPublicKey(keyPath);
    }

    private byte[] readResource(String path) throws Exception {
        ClassLoader classloader = Thread.currentThread().getContextClassLoader();
        try (InputStream is = classloader.getResourceAsStream(path)) {
            if (is == null) throw new IllegalArgumentException("Key file not found: " + path);
            return is.readAllBytes();
        }
    }

    public PublicKey loadPublicKey(String resourcePath) throws Exception {
        byte[] keyBytes = readResource(resourcePath);
        X509EncodedKeySpec spec = new X509EncodedKeySpec(keyBytes);
        KeyFactory kf = KeyFactory.getInstance("RSA");
        return kf.generatePublic(spec);
    }

    public void verifyUserSignature(String userId, byte[] message, byte[] signature) throws Exception {
        PublicKey publicKey = loadPublicKey(userId + ".pub");
        java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
        sig.initVerify(publicKey);
        sig.update(message);
        if (!sig.verify(signature)) {
            throw new SecurityException("Invalid signature for user: " + userId);
        }
    }

    public void verifyBlockSignature(byte[] message, byte[] signature) throws Exception {
        PublicKey publicKey = this.sequencerKey;
        java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
        sig.initVerify(publicKey);
        sig.update(message);
        if (!sig.verify(signature)) {
            throw new SecurityException("Invalid block signature from sequencer");
        }
    }
}