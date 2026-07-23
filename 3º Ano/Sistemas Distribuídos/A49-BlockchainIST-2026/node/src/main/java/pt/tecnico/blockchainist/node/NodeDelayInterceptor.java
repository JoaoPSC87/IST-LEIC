package pt.tecnico.blockchainist.node;

import io.grpc.Context;
import io.grpc.Contexts;
import io.grpc.Metadata;
import io.grpc.ServerCall;
import io.grpc.ServerCallHandler;
import io.grpc.ServerInterceptor;

public class NodeDelayInterceptor implements ServerInterceptor {
    // Now the client can send the delay and key as metadata, and the interceptor will extract them and put them in the context for the server call to use.

    public static final Metadata.Key<String> DELAY_KEY = 
            Metadata.Key.of("delay", Metadata.ASCII_STRING_MARSHALLER);
            
    public static final Context.Key<Integer> DELAY_CONTEXT_KEY = Context.key("delay");

    public static final Metadata.Key<String> KEY_KEY = 
            Metadata.Key.of("key", Metadata.ASCII_STRING_MARSHALLER);
    public static final Context.Key<String> KEY_CONTEXT_KEY = Context.key("key");

    @Override
    public <ReqT, RespT> ServerCall.Listener<ReqT> interceptCall(
        ServerCall<ReqT, RespT> call,
        Metadata headers,
        ServerCallHandler<ReqT, RespT> next) {
        String delayValue = headers.get(DELAY_KEY);
        int delay = 0;
        if (delayValue != null) {
            try {
                delay = Integer.parseInt(delayValue);
            } catch (NumberFormatException e) {
                // Invalid delay value, default to 0
            }
        }
        String keyValue = headers.get(KEY_KEY);
        String key = keyValue != null ? keyValue : "";
        Context context = Context.current()
            .withValue(DELAY_CONTEXT_KEY, delay)
            .withValue(KEY_CONTEXT_KEY, key);
        return Contexts.interceptCall(context, call, headers, next);
    }
}