package ru.mkn.lama.runtime;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.Node;
import ru.mkn.lama.LamaLanguage;

import java.io.PrintWriter;
import java.util.Scanner;

public final class LamaContext {
    private final LamaLanguage language;
    @CompilerDirectives.CompilationFinal
    private TruffleLanguage.Env env;
    private final Scanner input;
    private final PrintWriter output;

    public final GlobalScope globalScope = new GlobalScope();

    private static final LamaLanguage.ContextReference<LamaContext> REF =
            TruffleLanguage.ContextReference.create(LamaLanguage.class);

    public static LamaContext get(Node node) {
        return REF.get(node);
    }

    public LamaContext(LamaLanguage language, TruffleLanguage.Env env) {
        this.language = language;
        this.env = env;
        this.input = new Scanner(env.in());
        this.output = new PrintWriter(env.out(), true);
    }

    public PrintWriter getOutput() {
        return this.output;
    }

    public Scanner getInput() {
        return this.input;
    }

}
