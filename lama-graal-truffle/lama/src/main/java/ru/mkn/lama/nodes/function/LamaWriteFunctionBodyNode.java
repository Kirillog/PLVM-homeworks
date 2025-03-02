package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.dsl.Specialization;
import ru.mkn.lama.runtime.LamaContext;
import ru.mkn.lama.runtime.LamaNull;

public abstract class LamaWriteFunctionBodyNode extends LamaBuiltinFunctionBodyNode {

    @Specialization
    @CompilerDirectives.TruffleBoundary
    public Object write(int argument) {
        LamaContext.get(this).getOutput().println(argument);
        return LamaNull.INSTANCE;
    }

}
