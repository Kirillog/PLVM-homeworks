package ru.mkn.lama.nodes;

import com.oracle.truffle.api.dsl.TypeSystemReference;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.UnexpectedResultException;
import ru.mkn.lama.LamaTypes;
import ru.mkn.lama.LamaTypesGen;
import ru.mkn.lama.runtime.LamaContext;


@TypeSystemReference(LamaTypes.class)
public abstract class LamaNode extends Node {

    public abstract Object executeGeneric(VirtualFrame frame);

    public int executeInt(VirtualFrame frame) throws UnexpectedResultException {
        return LamaTypesGen.expectInteger(executeGeneric(frame));
    }

    public boolean executeBoolean(VirtualFrame frame) throws UnexpectedResultException {
        return LamaTypesGen.expectBoolean(executeGeneric(frame));
    }

    protected final LamaContext currentLanguageContext() {
        return LamaContext.get(this);
    }
}
