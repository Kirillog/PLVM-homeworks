package ru.mkn.lama.nodes.expr.literal;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaArrayObject;

import java.util.List;

public final class LamaListLiteralNode extends LamaNode {
    @Child
    private LamaNode h;
    @Child
    private LamaNode t;

    public LamaListLiteralNode() {
        this.h = null;
        this.t = null;
    }

    public LamaListLiteralNode(LamaNode h, LamaNode t) {
        this.h = h;
        this.t = t;
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        if (h == null || t == null) {
            return new LamaArrayObject();
        }
        return new LamaArrayObject(h.executeGeneric(frame), t.executeGeneric(frame));
    }
}
