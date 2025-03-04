package ru.mkn.lama.nodes.expr.unary;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.LamaException;
import ru.mkn.lama.nodes.expr.LamaUnaryExpression;

public abstract class LamaNegateNode extends LamaUnaryExpression {
    @Specialization
    protected int unaryMinus(int value) {
        return -value;
    }

    @Fallback
    protected Object typeError(Object value) {
        throw LamaException.typeError(this, value);
    }
}
