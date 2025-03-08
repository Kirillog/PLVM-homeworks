package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.TruffleObject;

import java.util.Arrays;

public class LamaStringObject implements TruffleObject {

    char[] value;
    public LamaStringObject(String value) {
        this.value = value.toCharArray();
    }

    public void set(int index, int val) {
        value[index] = (char) val;
    }

    public int get(int index) {
        return value[index];
    }

    public int length() {
        return value.length;
    }

    @Override
    public String toString() {
        return '"' +
                String.valueOf(value) +
                '"';
    }

    @Override
    public boolean equals(Object oth) {
        return oth instanceof LamaStringObject && Arrays.equals(((LamaStringObject) oth).value, value);
    }
}
