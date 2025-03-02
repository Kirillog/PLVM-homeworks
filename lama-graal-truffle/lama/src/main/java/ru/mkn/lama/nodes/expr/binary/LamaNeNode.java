package ru.mkn.lama.nodes.expr.binary;

import com.oracle.truffle.api.dsl.Fallback;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.LamaException;
import ru.mkn.lama.nodes.expr.LamaBinaryExpression;

@NodeInfo(shortName = "!=")
public abstract class LamaNeNode extends LamaBinaryExpression {
    @Specialization
    protected boolean ne(int left, int right) {
        return left != right;
    }

    @Fallback
    protected Object typeError(Object left, Object right) {
        throw LamaException.typeError(this, left, right);
    }
}
