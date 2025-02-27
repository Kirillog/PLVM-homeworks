package ru.mkn.lama.runtime;

import com.oracle.truffle.api.strings.TruffleString;

import java.util.HashMap;
import java.util.Map;

public final class GlobalScope {

    private final Map<TruffleString, Object> variables = new HashMap<>();
    public boolean newVariable(TruffleString name, Object value) {
        Object existingValue = this.variables.put(name, value);
        return existingValue == null;
    }

    public boolean updateVariable(TruffleString name, Object value) {
        Object existingValue = this.variables.computeIfPresent(name, (k, v) -> value);
        return existingValue != null;
    }

    public Object getVariable(TruffleString name) {
        return this.variables.get(name);
    }
}

