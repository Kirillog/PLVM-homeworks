package ru.mkn.lama.nodes.expr.array;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.InvalidArrayIndexException;
import com.oracle.truffle.api.interop.UnsupportedMessageException;
import com.oracle.truffle.api.library.CachedLibrary;
import ru.mkn.lama.LamaException;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaSExpObject;
import ru.mkn.lama.runtime.LamaStringObject;

@NodeChild("obj")
@NodeChild("index")
public abstract class LamaReadIndexExpressionNode extends LamaNode {

    public abstract LamaNode getObj();
    public abstract LamaNode getIndex();

    @Specialization(guards = "arrayInteropLibrary.isArrayElementReadable(obj, index)", limit = "2")
    protected Object readIntIndexOfArray(Object obj, int index,
                                         @CachedLibrary("obj") InteropLibrary arrayInteropLibrary) {
        try {
            return arrayInteropLibrary.readArrayElement(obj, index);
        } catch (UnsupportedMessageException | InvalidArrayIndexException e) {
            throw new LamaException(this, e.getMessage());
        }
    }

    @Specialization
    protected int readIntIndexOfString(LamaStringObject obj, int index) {
        return obj.get(index);
    }

    @Specialization
    protected Object readIntIndexOfSExp(LamaSExpObject obj, int index) {
        return obj.get(index);
    }
}
