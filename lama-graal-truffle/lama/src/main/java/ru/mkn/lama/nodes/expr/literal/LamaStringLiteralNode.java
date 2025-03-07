package ru.mkn.lama.nodes.expr.literal;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaArrayObject;
import ru.mkn.lama.runtime.LamaStringObject;

import java.util.Arrays;

@NodeInfo(shortName = "string literal")
public class LamaStringLiteralNode extends LamaNode {
    private final LamaStringObject value;

    public LamaStringLiteralNode(String value) {
        this.value = new LamaStringObject(value);
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        return value;
    }
}
