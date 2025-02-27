package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;
import ru.mkn.lama.nodes.LamaNode;

@NodeInfo(shortName = "const")
public class LamaIntLiteralNode extends LamaNode {

    private final int value;

    public LamaIntLiteralNode(int value) {
        this.value = value;
    }

    @Override
    public int executeInt(VirtualFrame frame) {
        return value;
    }
    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return value;
    }
}
