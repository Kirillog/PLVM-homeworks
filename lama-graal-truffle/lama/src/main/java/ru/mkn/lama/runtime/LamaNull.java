package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.InteropLibrary;
import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.library.ExportLibrary;
import com.oracle.truffle.api.library.ExportMessage;

@ExportLibrary(InteropLibrary.class)
@SuppressWarnings("static-method")
public final class LamaNull implements TruffleObject {

    public static final LamaNull SINGLETON = new LamaNull();
    private static final int IDENTITY_HASH = System.identityHashCode(SINGLETON);

    private LamaNull() {
    }

    @ExportMessage
    boolean isNull() {
        return true;
    }
}