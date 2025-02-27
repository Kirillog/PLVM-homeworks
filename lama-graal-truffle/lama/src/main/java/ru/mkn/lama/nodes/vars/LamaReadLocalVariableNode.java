package ru.mkn.lama.nodes.vars;

import com.oracle.truffle.api.dsl.NodeField;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import ru.mkn.lama.nodes.LamaNode;

@NodeField(name = "slot", type = int.class)
public abstract class LamaReadLocalVariableNode extends LamaNode {

    public abstract int getSlot();

    @Specialization(guards = "frame.isInt(getSlot())")
    protected int readInt(VirtualFrame frame) {
        return frame.getInt(getSlot());
    }

    @Specialization(guards = "frame.isBoolean(getSlot())")
    protected boolean readBoolean(VirtualFrame frame) {
        return frame.getBoolean(getSlot());
    }

    @Specialization(replaces = {"readInt", "readBoolean"})
    protected Object readObject(VirtualFrame frame) {
        return frame.getObject(getSlot());
    }


}
