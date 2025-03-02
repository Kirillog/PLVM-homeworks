package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaFunctionObject;

@NodeInfo(shortName = "func")
public final class LamaFunctionWrapper extends LamaNode {
    private final LamaFunctionObject function;
    public LamaFunctionWrapper(LamaFunctionObject function) {
        this.function = function;
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return function;
    }
}

