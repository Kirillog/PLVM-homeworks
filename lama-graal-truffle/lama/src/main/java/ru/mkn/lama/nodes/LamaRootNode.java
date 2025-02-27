package ru.mkn.lama.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;
import ru.mkn.lama.LamaLanguage;

public final class LamaRootNode extends RootNode {

    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaNode bodyNode;

    public LamaRootNode(LamaLanguage language, FrameDescriptor frameDescriptor, LamaNode bodyNode) {
        super(language, frameDescriptor);
        this.bodyNode = bodyNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return bodyNode.executeGeneric(frame);
    }
}
