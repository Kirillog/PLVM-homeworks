package ru.mkn.lama.runtime;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.dsl.NodeFactory;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.LamaLanguage;
import ru.mkn.lama.nodes.FunctionRootNode;
import ru.mkn.lama.nodes.function.LamaBuiltinFunctionBodyNode;
import ru.mkn.lama.nodes.function.LamaReadFunctionBodyNodeFactory;
import ru.mkn.lama.nodes.function.LamaWriteFunctionBodyNodeFactory;
import ru.mkn.lama.nodes.vars.LamaReadArgumentNode;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.IntStream;

public final class LamaContext {
    private final LamaLanguage language;
    @CompilerDirectives.CompilationFinal
    private TruffleLanguage.Env env;
    private final BufferedReader input;
    private final PrintWriter output;


    private static final LamaLanguage.ContextReference<LamaContext> REF =
            TruffleLanguage.ContextReference.create(LamaLanguage.class);

    public static LamaContext get(Node node) {
        return REF.get(node);
    }

    public LamaContext(LamaLanguage language, TruffleLanguage.Env env) {
        this.language = language;
        this.env = env;
        this.input = new BufferedReader(new InputStreamReader(env.in()));
        this.output = new PrintWriter(env.out(), true);
    }

    public PrintWriter getOutput() {
        return this.output;
    }

    public BufferedReader getInput() {
        return this.input;
    }

}
