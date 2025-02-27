package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaNull;

import java.util.List;

@NodeInfo(shortName = "expression list")
public class LamaExpressionListNode extends LamaNode {
    @Children
    private final LamaNode[] expressions;

    public LamaExpressionListNode(List<LamaNode> exprs) {
        this.expressions = exprs.toArray(new LamaNode[0]);
    }


    @Override
    @ExplodeLoop
    public Object executeGeneric(VirtualFrame frame) {
        if (expressions.length == 0) {
            return LamaNull.SINGLETON;
        }
        for (int i = 0; i < expressions.length - 1; ++i) {
            expressions[i].executeGeneric(frame);
        }
        Object ret = LamaNull.SINGLETON;
        for (LamaNode expression : expressions) {
            ret = expression.executeGeneric(frame);
        }
        return ret;
    }
}
