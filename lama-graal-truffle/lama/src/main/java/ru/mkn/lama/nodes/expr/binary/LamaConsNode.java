package ru.mkn.lama.nodes.expr.binary;

import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.nodes.expr.LamaBinaryExpression;
import ru.mkn.lama.runtime.LamaArrayObject;

@NodeInfo(shortName = ":")
public abstract class LamaConsNode extends LamaBinaryExpression {
    @Specialization
    protected Object cons(Object left, Object right) {
        return new LamaArrayObject(left, right);
    }
}
