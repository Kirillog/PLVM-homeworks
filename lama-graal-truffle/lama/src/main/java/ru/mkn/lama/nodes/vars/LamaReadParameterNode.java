package ru.mkn.lama.nodes.vars;

import com.oracle.truffle.api.frame.VirtualFrame;
import ru.mkn.lama.nodes.LamaNode;

public class LamaReadParameterNode extends LamaNode {
    private final int index;

    public LamaReadParameterNode(int index) {
        this.index = index;
    }
    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return frame.getArguments()[index];
    }

    public int getIndex() {
        return index;
    }
}
