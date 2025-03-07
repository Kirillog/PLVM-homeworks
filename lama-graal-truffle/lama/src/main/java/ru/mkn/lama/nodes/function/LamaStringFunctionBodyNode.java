package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.dsl.Specialization;
import ru.mkn.lama.runtime.LamaArrayObject;
import ru.mkn.lama.runtime.LamaSExpObject;
import ru.mkn.lama.runtime.LamaStringObject;


public abstract class LamaStringFunctionBodyNode extends LamaBuiltinFunctionBodyNode {

    @Specialization
    public LamaStringObject string(Integer a) {
        return new LamaStringObject(a.toString());
    }

    @Specialization
    public LamaStringObject string(LamaArrayObject obj) {
        return new LamaStringObject(obj.toString());
    }

    @Specialization
    public LamaStringObject string(LamaStringObject obj) {
        return new LamaStringObject(obj.toString());
    }

    @Specialization
    public LamaStringObject string(LamaSExpObject obj) {
        return new LamaStringObject(obj.toString());
    }
}
