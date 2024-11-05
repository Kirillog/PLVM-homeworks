#pragma once
#include <tuple>
#include "bytefile.h"

class ByteCodeIter {
private:
    const char *begin, *ip, *end;

public:
    ByteCodeIter(const char *ip, const char *end);

    int get_offset();
    int next_int();
    char next_byte();

public:
    struct ByteCode {
        unsigned char x, h, l;
    };

    ByteCode get_bytecode();
};