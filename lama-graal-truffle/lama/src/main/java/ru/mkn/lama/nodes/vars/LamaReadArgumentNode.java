package ru.mkn.lama.nodes.vars;

import com.oracle.truffle.api.frame.VirtualFrame;
import ru.mkn.lama.nodes.LamaNode;

public class LamaReadArgumentNode extends LamaNode {
    private final int index;

    public LamaReadArgumentNode(int index) {
        this.index = index;
    }
    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return frame.getArguments()[index];
    }
}
