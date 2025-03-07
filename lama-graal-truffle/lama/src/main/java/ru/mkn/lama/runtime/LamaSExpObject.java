package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.TruffleObject;
import com.oracle.truffle.api.strings.TruffleString;

import java.util.Arrays;
import java.util.stream.Collectors;

public class LamaSExpObject implements TruffleObject {
    private Object[] sExpElements;

    TruffleString name;

    public LamaSExpObject(TruffleString name, Object[] sExpElements) {
        this.name = name;
        this.sExpElements = sExpElements;
    }

    public TruffleString getName() {
        return name;
    }
    public Object get(int index) {
        return sExpElements[index];
    }

    public void set(int index, Object value) {
        sExpElements[index] = value;
    }

    public int length() {
        return sExpElements.length;
    }

    @Override
    public String toString() {
        if (sExpElements.length == 0) {
            return name.toString();
        }
        return name.toString() + " (" + Arrays.stream(sExpElements).map(Object::toString)
                .collect(Collectors.joining(", ")) + ")";
    }
}
