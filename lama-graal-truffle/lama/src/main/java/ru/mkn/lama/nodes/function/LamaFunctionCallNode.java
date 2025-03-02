package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.nodes.LamaNode;

import java.util.List;

public final class LamaFunctionCallNode extends LamaNode {
    private String name;

    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaNode targetFunction;

    @Children
    private final LamaNode[] callArguments;

    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaFunctionDispatchNode dispatchNode;

    public LamaFunctionCallNode(String name, LamaNode targetFunction, List<LamaNode> callArguments) {
        this.name = name;
        this.targetFunction = targetFunction;
        this.callArguments = callArguments.toArray(new LamaNode[]{});
        this.dispatchNode = LamaFunctionDispatchNodeGen.create();
    }

    @Override
    @ExplodeLoop
    public Object executeGeneric(VirtualFrame frame) {
        Object function = this.targetFunction.executeGeneric(frame);

        Object[] argumentValues = new Object[this.callArguments.length];
        for (int i = 0; i < this.callArguments.length; i++) {
            argumentValues[i] = this.callArguments[i].executeGeneric(frame);
        }

        return this.dispatchNode.executeDispatch(function, argumentValues);
    }
}