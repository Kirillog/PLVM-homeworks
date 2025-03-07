package ru.mkn.lama.nodes.expr.literal;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaArrayObject;

import java.util.List;

public final class LamaArrayLiteralNode extends LamaNode {
    @Children
    private final LamaNode[] arrayElementExprs;

    public LamaArrayLiteralNode(List<LamaNode> arrayElementExprs) {
        this.arrayElementExprs = arrayElementExprs.toArray(new LamaNode[]{});
    }

    @Override
    @ExplodeLoop
    public Object executeGeneric(VirtualFrame frame) {
        Object[] arrayElements = new Object[this.arrayElementExprs.length];
        for (var i = 0; i < this.arrayElementExprs.length; i++) {
            arrayElements[i] = this.arrayElementExprs[i].executeGeneric(frame);
        }
        return new LamaArrayObject(arrayElements);
    }
}
