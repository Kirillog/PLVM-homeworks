package ru.mkn.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.nodes.LamaRootNode;
import ru.mkn.lama.parser.LamaLanguageParser;
import ru.mkn.lama.runtime.LamaContext;

@TruffleLanguage.Registration(
        id = LamaLanguage.ID,
        name = LamaLanguage.NAME,
        defaultMimeType = LamaLanguage.MIME_TYPE,
        characterMimeTypes = {LamaLanguage.MIME_TYPE}
)
public class LamaLanguage extends TruffleLanguage<LamaContext> {

    private static final LanguageReference<LamaLanguage> REFERENCE =
            LanguageReference.create(LamaLanguage.class);
    public static final String ID = "lama";
    public static final String NAME = "Lama";
    public static final String MIME_TYPE = "application/x-lama";

    public static LamaLanguage get(Node node) {
        return REFERENCE.get(node);
    }


    @Override
    protected CallTarget parse(ParsingRequest request) throws Exception {
        var source = request.getSource();
        var root = LamaLanguageParser.parseLama(this, source);
        return root.getCallTarget();
    }

    @Override
    protected LamaContext createContext(Env env) {
        return new LamaContext(this);
    }

    public static NodeInfo lookupNodeInfo(Class<?> clazz) {
        if (clazz == null) {
            return null;
        }
        NodeInfo info = clazz.getAnnotation(NodeInfo.class);
        if (info != null) {
            return info;
        } else {
            return lookupNodeInfo(clazz.getSuperclass());
        }
    }
}
