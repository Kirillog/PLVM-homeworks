package ru.mkn.lama.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;
import ru.mkn.lama.LamaLanguage;

public final class FunctionRootNode extends RootNode {
    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaNode functionBodyExpr;

    public FunctionRootNode(LamaLanguage lamaLanguage,
                            LamaNode functionBodyExpr) {
        super(lamaLanguage);
        this.functionBodyExpr = functionBodyExpr;
    }

    public FunctionRootNode(LamaLanguage lamaLanguage, LamaNode body, FrameDescriptor frameDescriptor) {
        super(lamaLanguage, frameDescriptor);
        this.functionBodyExpr = body;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return this.functionBodyExpr.executeGeneric(frame);
    }
}