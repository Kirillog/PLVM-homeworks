package ru.mkn.lama.nodes.expr.literal;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaSExpObject;

import java.util.List;

public class LamaSExpLiteralNode extends LamaNode {

    TruffleString name;
    @Node.Children
    private final LamaNode[] sExpElements;

    public LamaSExpLiteralNode(TruffleString name, List<LamaNode> sExpElements) {
        this.name = name;
        this.sExpElements = sExpElements.toArray(new LamaNode[]{});
    }

    @Override
    @ExplodeLoop
    public Object executeGeneric(VirtualFrame frame) {
        Object[] sExpElements = new Object[this.sExpElements.length];
        for (var i = 0; i < this.sExpElements.length; i++) {
            sExpElements[i] = this.sExpElements[i].executeGeneric(frame);
        }
        return new LamaSExpObject(name, sExpElements);
    }
}
