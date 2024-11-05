#include "bytefile.h"
#include "byteiter.h"
#include "code.h"
#include <tuple>
#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdio>

struct Instruction {
    size_t offset;
    Code code;
};

Code decode_instruction_code(ByteCodeIter &iter) {
    auto [x, h, l] = iter.get_bytecode();
    switch (h) {
        case CSTOP:
            return CSTOP;

        case CBINOP: {
            int arg = l;
            return CBINOP;
        }

        case CLD: {
            int arg = l;
            int ind = iter.next_int();
            return CLD;
        }

        case CLDA: {
            int arg = l;
            int ind = iter.next_int();
            return CLDA;
        }

        case CST: {
            int arg = l;
            int ind = iter.next_int();
            return CST;
        }

        case CPATT: {
            int arg = l;
            return CPATT;
        }

        default:
            switch (x) {
                case CCONST: {
                    int val = iter.next_int();
                    return CCONST;
                }

                case CSTRING: {
                    int pos = iter.next_int();
                    return CSTRING;
                }

                case CSEXP: {
                    int pos = iter.next_int();
                    int count = iter.next_int();
                    return CSEXP;
                }

                case CSTI: {
                    return CSTI;
                }

                case CSTA: {
                    return CSTA;
                }

                case CJMP: {
                    int offset = iter.next_int();
                    return CJMP;
                }

                case CEND: {
                    return CEND;
                }

                case CRET: {
                    return CRET;
                }

                case CDROP: {
                    return CDROP;
                }

                case CDUP: {
                    return CDUP;
                }

                case CSWAP: {
                    return CSWAP;
                }

                case CELEM: {
                    return CELEM;
                }

                case CCJMPZ: {
                    int offset = iter.next_int();
                    return CCJMPZ;
                }

                case CCJMPNZ: {
                    int offset = iter.next_int();
                    return CCJMPNZ;
                }

                case CBEGIN: {
                    int arg_count = iter.next_int(), local_count = iter.next_int();
                    return CBEGIN;
                }

                case CCBEGIN: {
                    int arg_count = iter.next_int(), local_count = iter.next_int();
                    return CCBEGIN;
                }

                case CCLOSURE: {
                    int offset = iter.next_int(), n = iter.next_int();
                    for (int i = 0; i < n; i++) {
                        int typ = iter.next_byte(), ind = iter.next_int();
                    };
                    return CCLOSURE;
                }

                case CCALLC: {
                    int arg_count = iter.next_int();
                    return CCALLC;
                }

                case CCALL: {
                    int offset = iter.next_int();
                    int arg_count = iter.next_int();
                    return CCALL;
                }

                case CTAG: {
                    int pos = iter.next_int();
                    int arg_count = iter.next_int();
                    return CTAG;
                }

                case CARRAY: {
                    int size = iter.next_int();
                    return CARRAY;
                }

                case CFAIL: {
                    int ln = iter.next_int(), col = iter.next_int();
                    return CFAIL;
                }

                case CLINE: {
                    int ln = iter.next_int();
                    return CLINE;
                }

                case CLREAD: {
                    return CLREAD;
                }

                case CLWRITE: {
                    return CLWRITE;
                }

                case CLLENGTH: {
                    return CLLENGTH;
                }

                case CLSTRING: {
                    return CLSTRING;
                }

                case CBARRAY: {
                    int size = iter.next_int();
                    return CBARRAY;
                }

                default:
                    failure("ERROR: invalid opcode %d-%d\n", h, l);
            }
    }
}

// Code < 256 required
int hash_block(const std::deque<Instruction> insns) {
    int hash = 0;
    for (auto i : insns) {
        hash <<= 8;
        hash |= i.code;
    }
    return hash;
}

bool equal_insn(ByteCodeIter &iter1, ByteCodeIter &iter2) {
    auto [x1, h1, l1] = iter1.get_bytecode();
    auto [x2, h2, l2] = iter2.get_bytecode();

    switch (h1) {
        case CSTOP:
            return true;

        case CPATT:
        case CBINOP: {
            return l1 == l2;
        }

        case CLD:
        case CLDA:
        case CST: {
            int eq = l1 == l2;
            eq &= iter1.next_int() == iter2.next_int();
            return eq;
        }

        default:
            switch (x1) {
                case CBARRAY:
                case CLINE:
                case CARRAY:
                case CCALLC:
                case CJMP:
                case CCJMPZ:
                case CCJMPNZ:
                case CSTRING:
                case CCONST: {
                    return iter1.next_int() == iter2.next_int();
                }
                case CTAG:
                case CCALL:
                case CFAIL:
                case CBEGIN:
                case CCBEGIN:
                case CSEXP: {
                    bool eq = true;
                    eq &= iter1.next_int() == iter2.next_int();
                    eq &= iter1.next_int() == iter2.next_int();
                    return eq;
                }

                case CSTI:
                case CSTA:
                case CEND:
                case CRET:
                case CDROP:
                case CDUP:
                case CSWAP:
                case CELEM:
                case CLREAD:
                case CLWRITE:
                case CLLENGTH:
                case CLSTRING: {
                    return true;
                }

                case CCLOSURE: {
                    bool eq = true;
                    eq &= iter1.next_int() == iter2.next_int();
                    int n1 = iter1.next_int(), n2 = iter2.next_int();
                    eq &= n1 == n2;
                    if (!eq) {
                        for (int i = 0; i < n1; i++) {
                            iter1.next_byte(), iter1.next_int();
                        };
                        for (int i = 0; i < n2; i++) {
                            iter2.next_byte(), iter2.next_int();
                        };
                    } else {
                        for (int i = 0; i < n1; i++) {
                            int typ1 = iter1.next_byte(), ind1 = iter1.next_int();
                            int typ2 = iter2.next_byte(), ind2 = iter2.next_int();
                            eq &= typ1 == typ2 && ind1 == ind2;
                        };
                    }
                    return eq;
                }
            }
    }
}

bool equal_block(ByteCodeIter &&iter1, ByteCodeIter &&iter2, size_t insn_count) {
    for (size_t i = 0; i < insn_count; ++i) {
        if (!equal_insn(iter1, iter2)) {
            return false;
        }
    }
    return true;
}

struct BlockCount {
    size_t count;
    size_t offset;
    size_t size;
    bool operator<(const BlockCount &oth) const {
        return std::make_tuple(count, size, offset) <
               std::make_tuple(oth.count, oth.size, oth.offset);
    }
};

// only insn_count from 1 to 4 supported
std::vector<BlockCount> count_occurencies(const Bytefile &file, size_t insn_count) {
    ByteCodeIter iter(file.get_ip(), file.get_end());
    std::deque<Instruction> block;
    int count = 1;
    for (size_t i = 0; i < insn_count; ++i) {
        size_t offset = iter.get_offset();
        Code instr = decode_instruction_code(iter);
        block.push_back(Instruction{offset, instr});
        ++count;
        if (instr == Code::CSTOP) {
            return {};
        }
    }
    size_t block_offset = 0;
    std::unordered_map<int, int> last_block_offset;
    std::vector<int> prev_block_offset(file.get_end() - file.get_ip(), -1);
    std::vector<BlockCount> offset_count(file.get_end() - file.get_ip());
    for (size_t i = 0; i < offset_count.size(); ++i) {
        offset_count[i] = {0, i, insn_count};
    }
    Code instr;
    size_t offset;
    do {
        offset = block.front().offset;
        int hash = hash_block(block);
        if (last_block_offset.count(hash) == 0) {
            last_block_offset[hash] = -1;
        }
        int lb_offset = last_block_offset[hash];
        while (lb_offset != -1 &&
               !equal_block(file.from_offset(offset), file.from_offset(lb_offset), insn_count)) {
            lb_offset = prev_block_offset[lb_offset];
        }
        if (lb_offset == -1) {
            // unique instruction
            lb_offset = offset;
            prev_block_offset[lb_offset] = last_block_offset[hash];
            last_block_offset[hash] = lb_offset;
        }
        ++offset_count[lb_offset].count;
        block.pop_front();
        offset = iter.get_offset();
        instr = decode_instruction_code(iter);
        ++count;
        // std::cout << instr << "\n";
        block.push_back(Instruction{offset, instr});
    } while (instr != Code::CSTOP);
    return offset_count;
}

void print_ld(FILE *f, char h, char l, int ind) {
    static const char *const lds[] = {"LD", "LDA", "ST"};
    fprintf(f, "%s\t", lds[h - 2]);
    switch (l) {
        case 0:
            fprintf(f, "G(%d)", ind);
            break;
        case 1:
            fprintf(f, "L(%d)", ind);
            break;
        case 2:
            fprintf(f, "A(%d)", ind);
            break;
        case 3:
            fprintf(f, "C(%d)", ind);
            break;
    }
}

void print_instruction(FILE *f, const Bytefile &file, ByteCodeIter &iter) {
    auto [x, h, l] = iter.get_bytecode();

    static const char *const ops[] = {
        "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"};
    static const char *const pats[] = {"=str", "#string", "#array", "#sexp",
                                       "#ref", "#val",    "#fun"};
    switch (h) {
        case CSTOP:
            fprintf(f, "STOP");
            break;

        case CBINOP: {
            fprintf(f, "BINOP\t%s", ops[l - 1]);
            break;
        }

        case CLD:
        case CLDA:
        case CST: {
            int ind = iter.next_int();
            print_ld(f, h, l, ind);
            break;
        }

        case CPATT: {
            fprintf(f, "PATT\t%s", pats[l]);
            break;
        }

        default:
            switch (x) {
                case CCONST: {
                    int val = iter.next_int();
                    fprintf(f, "CONST\t%d", val);
                    break;
                }

                case CSTRING: {
                    const char *str = file.get_string(iter.next_int());
                    fprintf(f, "STRING\t%s", str);
                    break;
                }

                case CSEXP: {
                    const char *str = file.get_string(iter.next_int());
                    int count = iter.next_int();
                    fprintf(f, "SEXP\t%s %d", str, count);
                    break;
                }

                case CSTI: {
                    fprintf(f, "STI");
                    break;
                }

                case CSTA: {
                    fprintf(f, "STA");
                    break;
                }

                case CJMP: {
                    int offset = iter.next_int();
                    fprintf(f, "JMP\t0x%.8x", offset);
                    break;
                }

                case CEND:
                case CRET: {
                    fprintf(f, (l == 6) ? "END" : "RET");
                    break;
                }

                case CDROP: {
                    fprintf(f, "DROP");
                    break;
                }

                case CDUP: {
                    fprintf(f, "DUP");
                    break;
                }

                case CSWAP: {
                    fprintf(f, "SWAP");
                    break;
                }

                case CELEM: {
                    fprintf(f, "ELEM");
                    break;
                }

                case CCJMPZ:
                case CCJMPNZ: {
                    int offset = iter.next_int();
                    fprintf(f, (l == 0) ? "CJMPz\t0x%.8x" : "CJMPnz\t0x%.8x", offset);
                    break;
                }

                case CBEGIN:
                case CCBEGIN: {
                    int arg_count = iter.next_int(), local_count = iter.next_int();
                    fprintf(f, (l == 2) ? "BEGIN\t%d " : "CBEGIN\t%d ", arg_count);
                    fprintf(f, "%d", local_count);
                    break;
                }

                case CCLOSURE: {
                    int offset = iter.next_int(), n = iter.next_int();
                    for (int i = 0; i < n; i++) {
                        int typ = iter.next_byte(), ind = iter.next_int();
                        switch (typ) {
                            case 0: fprintf (f, "G(%d)", ind); break;
                            case 1: fprintf (f, "L(%d)", ind); break;
                            case 2: fprintf (f, "A(%d)", ind); break;
                            case 3: fprintf (f, "C(%d)", ind); break;
                            default: failure("incorrect closure argument");
                            }
                    };
                    fprintf(f, "CLOSURE\t0x%.8x", offset);
                    break;
                }

                case CCALLC: {
                    int arg_count = iter.next_int();
                    fprintf(f, "CALLC\t%d", arg_count);
                    break;
                }

                case CCALL: {
                    int offset = iter.next_int();
                    int arg_count = iter.next_int();
                    fprintf(f, "CALL\t0x%.8x %d", offset, arg_count);
                    break;
                }

                case CTAG: {
                    const char *name = file.get_string(iter.next_int());
                    int arg_count = iter.next_int();
                    fprintf(f, "TAG\t%s %d", name, arg_count);
                    break;
                }

                case CARRAY: {
                    int size = iter.next_int();
                    fprintf(f, "ARRAY\t%d", size);
                    break;
                }

                case CFAIL: {
                    int ln = iter.next_int(), col = iter.next_int();
                    fprintf(f, "FAIL\t%d %d", ln, col);
                    break;
                }

                case CLINE: {
                    int ln = iter.next_int();
                    fprintf(f, "LINE\t%d", ln);
                    break;
                }

                case CLREAD: {
                    fprintf(f, "CALL\tLread");
                    break;
                }

                case CLWRITE: {
                    fprintf(f, "CALL\tLwrite");
                    break;
                }

                case CLLENGTH: {
                    fprintf(f, "CALL\tLlength");
                    break;
                }

                case CLSTRING: {
                    fprintf(f, "CALL\tLstring");
                    break;
                }

                case CBARRAY: {
                    int size = iter.next_int();
                    fprintf(f, "CALL\tBarray\t%d", size);
                    break;
                }

                default:
                    failure("incorrect bytecode");
            }
    }
    fprintf(f, "\n");

}

void print_sequencies(FILE *f, const Bytefile &file, std::vector<BlockCount> &&counts) {
    for (auto [count, offset, size] : counts) {
        if (count == 0) {
            break;
        }
        fprintf(f, "Count %lu of len %lu:\n", count, size);
        ByteCodeIter iter = file.from_offset(offset);
        for (auto i = 0; i < size; ++i) {
            print_instruction(f, file, iter);            
        }
    }
}

int main(int argc, char *argv[]) {
    const Bytefile f(argv[1]);
    auto counts = count_occurencies(f, 1);
    auto count2 = count_occurencies(f, 2);
    counts.insert(counts.end(), count2.begin(), count2.end());
    std::sort(counts.begin(), counts.end());
    std::reverse(counts.begin(), counts.end());
    print_sequencies(stdout, f, std::move(counts));
    return 0;
}