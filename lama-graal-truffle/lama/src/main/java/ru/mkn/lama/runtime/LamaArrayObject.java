package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.library.ExportMessage;

import java.util.Arrays;

@ExportLibrary(InteropLibrary.class)
public final class LamaArrayObject implements TruffleObject {
    private Object[] arrayElements;


    public LamaArrayObject(Object[] arrayElements) {
        this.arrayElements = arrayElements;
    }

    @ExportMessage
    public int getArraySize() {
        return this.arrayElements.length;
    }

    @ExportMessage
    boolean isArrayElementReadable(long index) {
        return index >= 0 && index < this.arrayElements.length;
    }

    @ExportMessage
    Object readArrayElement(long index) {
        return this.isArrayElementReadable(index)
                ? this.arrayElements[(int) index]
                : LamaNull.INSTANCE;
    }


    @ExportMessage boolean hasArrayElements() { return true; }
    @ExportMessage boolean isArrayElementInsertable(long index) { return false; }
    @ExportMessage
    boolean isArrayElementModifiable(long index) {
        return this.isArrayElementReadable(index);
    }

    @ExportMessage
    void writeArrayElement(long index, Object value) {
        if (this.isArrayElementModifiable(index)) {
            this.arrayElements[(int) index] = value;
        }
    }

    @Override
    public String toString() {
        return Arrays.toString(arrayElements);
    }
}
