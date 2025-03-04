package ru.mkn.lama.nodes.vars;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.NodeField;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;

@NodeChild("valueNode")
@NodeField(name = "name", type = TruffleString.class)
public abstract class LamaWriteGlobalVariableNode extends LamaNode {
    protected abstract TruffleString getName();

    @Specialization
    protected Object write(VirtualFrame frame, Object value) {
        currentLanguageContext().globalScope.define(getName(), value);
        return value;
    }
}
