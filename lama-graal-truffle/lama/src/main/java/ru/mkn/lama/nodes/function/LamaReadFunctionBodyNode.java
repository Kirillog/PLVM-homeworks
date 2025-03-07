package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.LamaException;
import ru.mkn.lama.runtime.LamaContext;
import ru.mkn.lama.runtime.LamaNull;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;

public abstract class LamaReadFunctionBodyNode extends LamaBuiltinFunctionBodyNode {

    @CompilerDirectives.TruffleBoundary
    @Specialization
    public int read() {
        var context = LamaContext.get(this);
        context.getOutput().print("> ");
        return context.getInput().nextInt();
    }
}
