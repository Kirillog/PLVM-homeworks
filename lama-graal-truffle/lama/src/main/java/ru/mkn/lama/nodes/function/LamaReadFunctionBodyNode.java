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


    @Specialization
    public int read() {
        return doRead(LamaContext.get(this).getInput(), LamaContext.get(this).getOutput());
    }

    @CompilerDirectives.TruffleBoundary
    private int doRead(BufferedReader in, PrintWriter out) {
        try {
            out.print("> ");
            out.flush();
            var value = Integer.parseInt(in.readLine());
            return value;
        } catch (IOException ex) {
            throw new LamaException(this, ex.getMessage());
        }
    }
}
