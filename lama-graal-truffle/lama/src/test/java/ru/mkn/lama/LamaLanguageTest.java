package ru.mkn.lama;

import org.graalvm.polyglot.Context;
import org.graalvm.polyglot.Engine;
import org.graalvm.polyglot.Value;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;

import java.io.*;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class LamaLanguageTest {
    private Context context;

    @BeforeEach
    public void setUp() {
        this.context = Context.create();
    }

    @AfterEach
    public void tearDown() {
        this.context.close();
    }

    @Test
    public void simpleArithmeticExpression() {
        Value result = context.eval("lama",
                "100 / 10 - 5 * 2 + 2");
        assertEquals(2, result.asInt());
    }

    @Test
    void simpleVariableDefinitions() {
        Value result1 = context.eval("lama",
 """
            var a;
                1
        """);
        assertEquals(1, result1.asInt());

        Value result2 = context.eval("lama",
 """
            var a, b = 2;
            b
        """);
        assertEquals(2, result2.asInt());

        Value result3 = context.eval("lama",
 """
            var a = 1, b = 2;
            b := a;
            b
        """);
        assertEquals(1, result3.asInt());

        Value result4 = context.eval("lama",
 """
            var a = 1, b = 2;
            var c = a + b;
            c
        """);
        assertEquals(3, result4.asInt());
    }

    @Test
    void simpleScopes() {
        Value result = context.eval("lama",
 """
            var a = 1;
            (
                var a = 2;
            );
            a
        """);
        assertEquals(1, result.asInt());
    }

    @Test
    void nestedScopes() {
        Value result = context.eval("lama",
 """
            var a = 1;
            var b = 1;
            (
                var a = 2;
                b := 3;
                (
                    var b = 42;
                    a := 20
                )
            );
            a + b
        """);
        assertEquals(4, result.asInt());
    }

    @Test
    void logicalCompare() {
        var expected = new byte[]{'1','\n','1','\n','0','\n','1','\n','0','\n','0','\n'};

        var outputStream = new ByteArrayOutputStream();
        Engine engine = Engine.newBuilder().out(outputStream).build();
        Context context = Context.newBuilder().engine(engine).build();
        context.eval("lama",
 """
            var x = -6, y = 7, z;
            z := x < y;
            write (z);
            z := x <= y;
            write (z);
            z := x == y;
            write (z);
            z := x != y;
            write (z);
            z := x >= y;
            write (z);
            z := x > y;
            write (z)
        """);
        assertArrayEquals(expected, outputStream.toByteArray());
    }

    @Test
    void readWriteBuiltin() {
        var array = new byte[]{'1', '\n'};
        var expected = new byte[]{'>', ' ', '1', '\n'};
        var inputStream = new ByteArrayInputStream(array);
        var outputStream = new ByteArrayOutputStream();
        Engine engine = Engine.newBuilder().in(inputStream).out(outputStream).build();
        Context context = Context.newBuilder().engine(engine).build();
        context.eval("lama",
 """
        var a = read();
        write(a)
        """);
        assertArrayEquals(expected, outputStream.toByteArray());
    }


}