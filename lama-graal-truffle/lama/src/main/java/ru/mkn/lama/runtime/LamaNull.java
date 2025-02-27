package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.TruffleObject;

@SuppressWarnings("static-method")
public final class LamaNull implements TruffleObject {

    public static final LamaNull SINGLETON = new LamaNull();
    private static final int IDENTITY_HASH = System.identityHashCode(SINGLETON);

    private LamaNull() {
    }

}