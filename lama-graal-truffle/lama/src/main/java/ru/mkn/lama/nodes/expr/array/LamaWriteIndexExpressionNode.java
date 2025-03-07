package ru.mkn.lama.nodes.expr.array;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.InvalidArrayIndexException;
import com.oracle.truffle.api.interop.UnsupportedMessageException;
import com.oracle.truffle.api.interop.UnsupportedTypeException;
import com.oracle.truffle.api.library.CachedLibrary;
import com.oracle.truffle.api.strings.TruffleString;
import ru.mkn.lama.LamaException;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaNull;
import ru.mkn.lama.runtime.LamaSExpObject;
import ru.mkn.lama.runtime.LamaStringObject;


@NodeChild("obj")
@NodeChild("index")
@NodeChild("value")
public abstract class LamaWriteIndexExpressionNode extends LamaNode {

    @Specialization(guards = "arrayInteropLibrary.isArrayElementModifiable(obj, index)", limit = "2")
    protected Object writeIntIndexOfArray(Object obj, int index, Object value,
                                         @CachedLibrary("obj") InteropLibrary arrayInteropLibrary) {
        try {
            arrayInteropLibrary.writeArrayElement(obj, index, value);
            return LamaNull.INSTANCE;
        } catch (UnsupportedMessageException | UnsupportedTypeException | InvalidArrayIndexException e ) {
            throw new LamaException(this, e.getMessage());
        }
    }

    @Specialization
    protected int writeIntIndexOfString(LamaStringObject obj, int index, int value) {
        obj.set(index, value);
        return 0;
    }

    @Specialization
    protected int writeIntIndexOfSExp(LamaSExpObject obj, int index, Object value) {
        obj.set(index, value);
        return 0;
    }
}
