package ru.mkn.lama.runtime;

import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.Node;
import ru.mkn.lama.LamaLanguage;

public final class LamaContext {

    private static final LamaLanguage.ContextReference<LamaContext> REF =
            TruffleLanguage.ContextReference.create(LamaLanguage.class);

    public static LamaContext get(Node node) {
        return REF.get(node);
    }
    private final LamaLanguage language;

    public LamaContext(LamaLanguage language) {
        this.language = language;
    }

}
