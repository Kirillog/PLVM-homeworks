package ru.mkn.lama.runtime;

import com.oracle.truffle.api.interop.TruffleObject;

import java.util.Arrays;


public final class LamaArrayObject implements TruffleObject {
    private Object[] arrayElements;


//    public LamaArrayObject(Object[] arrayElements) {
//        this.arrayElements = arrayElements;
//    }

    public LamaArrayObject(Object... arrayElements) {
        this.arrayElements = arrayElements;
    }

    public Object get(int index) {
        return arrayElements[index];
    }

    public void set(int index, Object value) {
        arrayElements[index] = value;
    }

    public int length() {
        return arrayElements.length;
    }

    @Override
    public String toString() {
        return Arrays.toString(arrayElements);
    }
}
