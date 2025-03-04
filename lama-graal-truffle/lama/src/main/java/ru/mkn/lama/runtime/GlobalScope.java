package ru.mkn.lama.runtime;

import com.oracle.truffle.api.strings.TruffleString;

import java.util.HashMap;
import java.util.Map;

public final class GlobalScope {
    private final Map<TruffleString, Object> vars = new HashMap<>();

    public void define(TruffleString name, Object value) {
        vars.put(name, value);
    }

    public Object get(TruffleString name) {
        return vars.get(name);
    }
}
