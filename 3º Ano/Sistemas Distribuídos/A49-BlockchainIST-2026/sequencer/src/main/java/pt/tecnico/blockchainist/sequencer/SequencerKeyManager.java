package pt.tecnico.blockchainist.sequencer;

import java.io.InputStream;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;

import com.google.protobuf.ByteString;

import pt.tecnico.blockchainist.contract.Block;

public class SequencerKeyManager {
    private PrivateKey sequencerKey;

    public SequencerKeyManager(String keyPath) throws Exception {
        this.sequencerKey = loadPrivateKey(keyPath);
    }

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

    public Block signBlock(Block block) throws Exception {
        java.security.Signature signature = java.security.Signature.getInstance("SHA256withRSA");
        signature.initSign(sequencerKey);
        signature.update(block.toByteArray());

        return block
            .toBuilder()
            .setSignature(ByteString.copyFrom(signature.sign()))
            .build();
    }
}