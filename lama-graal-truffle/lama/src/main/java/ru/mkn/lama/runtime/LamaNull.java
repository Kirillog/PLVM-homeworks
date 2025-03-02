package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.library.ExportMessage;

@ExportLibrary(InteropLibrary.class)
@SuppressWarnings("static-method")
public final class LamaNull implements TruffleObject {

    public static final LamaNull INSTANCE = new LamaNull();
    private LamaNull() {
    }

    @ExportMessage
    boolean isNull() {
        return true;
    }
}