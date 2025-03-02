package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaNull;

@NodeInfo(shortName = "skip")
public final class LamaSkipNode extends LamaNode {
    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return LamaNull.INSTANCE;
    }
}
