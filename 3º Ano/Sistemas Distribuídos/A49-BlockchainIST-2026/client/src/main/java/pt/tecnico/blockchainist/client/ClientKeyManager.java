package pt.tecnico.blockchainist.client;

import java.io.InputStream;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.spec.PKCS8EncodedKeySpec;

public class ClientKeyManager {
    private byte[] readResource(String path) throws Exception {
        ClassLoader classloader = Thread.currentThread().getContextClassLoader();
        try (InputStream is = classloader.getResourceAsStream(path)) {
            if (is == null) throw new IllegalArgumentException("Key file not found: " + path);
            return is.readAllBytes();
        }
    }
    
    public PrivateKey loadPrivateKey(String resourcePath) throws Exception {
        byte[] keyBytes = readResource(resourcePath);
        PKCS8EncodedKeySpec spec = new PKCS8EncodedKeySpec(keyBytes);
        KeyFactory kf = KeyFactory.getInstance("RSA");
        return kf.generatePrivate(spec);
    }

    public byte[] signMessage(String userId, byte[] message) throws Exception {
        PrivateKey privateKey = loadPrivateKey(userId + ".priv");
        java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
        sig.initSign(privateKey);
        sig.update(message);

        return sig.sign();
    }
}