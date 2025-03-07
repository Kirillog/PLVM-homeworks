package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.runtime.LamaArrayObject;
import ru.mkn.lama.runtime.LamaSExpObject;
import ru.mkn.lama.runtime.LamaStringObject;


public abstract class LamaLengthFunctionBodyNode extends LamaBuiltinFunctionBodyNode {

    @Specialization
    public int length(LamaArrayObject obj) {
        return obj.getArraySize();
    }

    @Specialization
    public int length(LamaStringObject obj) {
        return obj.length();
    }

    @Specialization
    public int length(LamaSExpObject obj) { return obj.length(); }
}
