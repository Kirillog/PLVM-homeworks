#include "byteiter.h"

ByteCodeIter::ByteCodeIter(const char *ip, const char *end) : begin(ip), ip(ip), end(end) {
}

int ByteCodeIter::next_int() {
    if (ip + sizeof(int) > end) {
        failure("end of file reached\n");
    }
    ip += sizeof(int);
    return *reinterpret_cast<const int *>(ip - sizeof(int));
}

char ByteCodeIter::next_byte() {
    if (ip + sizeof(char) > end) {
        failure("end of file reached\n");
    }
    return *ip++;
}

int ByteCodeIter::get_offset() {
    return ip - begin;
}

ByteCodeIter::ByteCode ByteCodeIter::get_bytecode() {
    unsigned char x = next_byte();
    unsigned char h = (x & 0xF0) >> 4;
    unsigned char l = x & 0x0F;
    return {x, h, l};
}

ByteCodeIter Bytefile::from_offset(int offset) const {
    return ByteCodeIter(f_->code_ptr + offset, f_->end);
}