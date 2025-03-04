package ru.mkn.lama.nodes.vars;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;

public final class LamaReadGlobalVariableNode extends LamaNode {

    private final TruffleString name;

    public LamaReadGlobalVariableNode(TruffleString name) {
        this.name = name;
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return currentLanguageContext().globalScope.get(name);
    }

    public TruffleString getName() {
        return name;
    }
}
