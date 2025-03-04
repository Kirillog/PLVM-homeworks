package ru.mkn.lama.nodes.expr.literal;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;

@NodeInfo(shortName = "string literal")
public class LamaStringLiteralNode extends LamaNode {
    private final TruffleString value;

    public LamaStringLiteralNode(TruffleString value) {
        this.value = value;
    }

    @Override
    public TruffleString executeGeneric(VirtualFrame frame) {
        return value;
    }
}
