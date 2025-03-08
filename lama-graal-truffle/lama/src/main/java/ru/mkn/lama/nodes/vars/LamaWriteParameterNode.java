package ru.mkn.lama.nodes.vars;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.NodeField;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;

@NodeChild("valueNode")
@NodeField(name = "index", type = int.class)
public abstract class LamaWriteParameterNode extends LamaNode {
    protected abstract int getIndex();

    @Specialization
    protected Object write(VirtualFrame frame, Object value) {
        frame.getArguments()[getIndex()] = value;
        return value;
    }
}
