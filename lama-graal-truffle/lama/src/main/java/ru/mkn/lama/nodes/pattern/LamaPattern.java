package ru.mkn.lama.nodes.pattern;

import com.oracle.truffle.api.strings.TruffleString;

public interface LamaPattern {
    record Decimal(Integer number) implements LamaPattern {

    }

    record Wildcard() implements LamaPattern {

    }

    record SExpPattern(TruffleString name, LamaPattern[] elems) implements LamaPattern {

    }

    record NamedPattern(TruffleString name, LamaPattern pattern) implements LamaPattern {

    }

    record ListPattern() implements LamaPattern {

    }

    record ConsPattern(LamaPattern h, LamaPattern t) implements LamaPattern {

    }

    record FuncTypePattern() implements LamaPattern {

    }
}


