#pragma once

enum Patt {
    STRING_PATT = 0,
    STRING_TAG_PATT = 1,
    ARRAY_TAG_PATT = 2,
    SEXP_TAG_PATT = 3,
    BOXED_PATT = 4,
    UNBOXED_PATT = 5,
    CLOSURE_PATT = 6,
};

enum Code {
    CBINOP = 0,
    CLD = 2,
    CLDA = 3,
    CST = 4,
    CPATT = 6,
    CSTOP = 15,
    CCONST = 16,
    CSTRING = 17,
    CSEXP = 18,
    CSTI = 19,
    CSTA = 20,
    CJMP = 21,
    CEND = 22,
    CRET = 23,
    CDROP = 24,
    CDUP = 25,
    CSWAP = 26,
    CELEM = 27,

    CCJMPZ = 80,
    CCJMPNZ = 81,
    CBEGIN = 82,
    CCBEGIN = 83,
    CCLOSURE = 84,
    CCALLC = 85,
    CCALL = 86,
    CTAG = 87,
    CARRAY = 88,
    CFAIL = 89,
    CLINE = 90,

    CLREAD = 112,
    CLWRITE = 113,
    CLLENGTH = 114,
    CLSTRING = 115,
    CBARRAY = 116
};
