#include "bytefile.h"
#include "code.h"
#include <cstring>
#include <tuple>
#include <array>
#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdio>

struct StoredBlock {
    const char *begin;
    size_t size;
    bool operator==(const StoredBlock& oth) const {
        return size == oth.size && (std::memcmp(begin, oth.begin, size) == 0);
    }
};

struct Block {
    StoredBlock sb;
    std::deque<size_t> insns_sizes;

    void push_back(size_t isize) {
        sb.size += isize;
        insns_sizes.push_back(isize);
    }

    void pop_front() {
        auto isize = insns_sizes.front();
        insns_sizes.pop_front();
        sb.size -= isize;
        sb.begin += isize;
    }
};

static constexpr size_t MOD = 1e9 + 7;
static constexpr size_t HASH_BASE = 7919;

struct StoredBlockHash {
    std::size_t operator()(StoredBlock const& s) const {
        std::size_t res = 0;
        for (const char *it = s.begin; it < s.begin + s.size; ++it) {
            res += (*it * HASH_BASE) % MOD;
        }
        return res;
    }
};

const char *end;

int next_int(const char *&ip) {
    if (ip + sizeof(int) > end) {
        failure("end of file reached\n");
    }
    ip += sizeof(int);
    return *reinterpret_cast<const int *>(ip - sizeof(int));
}

char next_byte(const char *&ip) {
    if (ip + sizeof(char) > end) {
        failure("end of file reached\n");
    }
    return *ip++;
}

struct ByteCode {
    unsigned char x, h, l;
};

ByteCode get_bytecode(const char *&ip) {
    unsigned char x = next_byte(ip);
    unsigned char h = (x & 0xF0) >> 4;
    unsigned char l = x & 0x0F;
    return {x, h, l};
}

int consume(FILE *__restrict __stream, const char *__restrict __format, ...) {}

typedef int (*VariadicFuncPtr)(FILE *__restrict __stream, const char *__restrict __format, ...);

void print_ld(FILE *f, VariadicFuncPtr func, char h, char l, int ind) {
    static const char *const lds[] = {"LD", "LDA", "ST"};
    func(f, "%s\t", lds[h - 2]);
    switch (l) {
        case 0:
            func(f, "G(%d)", ind);
            break;
        case 1:
            func(f, "L(%d)", ind);
            break;
        case 2:
            func(f, "A(%d)", ind);
            break;
        case 3:
            func(f, "C(%d)", ind);
            break;
    }
}

struct Instruction {
    Code code;
    size_t size;
};

Instruction print_instruction(FILE *f, VariadicFuncPtr func, const Bytefile &file, const char *&iter) {
    static const char *const ops[] = {
        "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"};
    static const char *const pats[] = {"=str", "#string", "#array", "#sexp",
                                    "#ref", "#val",    "#fun"};

    static constexpr int no_args = sizeof(char);
    static constexpr int one_arg = no_args + sizeof(int);
    static constexpr int two_args = one_arg + sizeof(int);
    auto [x, h, l] = get_bytecode(iter);
    switch (h) {
        case CSTOP:
            func(f, "STOP");
            return {CSTOP, no_args};

        case CBINOP: {
            func(f, "BINOP\t%s", ops[l - 1]);
            return {CBINOP, no_args};
        }

        case CLD:
        case CLDA:
        case CST: {
            int ind = next_int(iter);
            print_ld(f, func, h, l, ind);
            return {(Code)h, one_arg};
        }

        case CPATT: {
            func(f, "PATT\t%s", pats[l]);
            return {CPATT, no_args};
        }

        default:
            switch (x) {
                case CCONST: {
                    int val = next_int(iter);
                    func(f, "CONST\t%d", val);
                    return {CCONST, one_arg};
                }

                case CSTRING: {
                    const char *str = file.get_string(next_int(iter));
                    func(f, "STRING\t%s", str);
                    return {CSTRING, one_arg};
                }

                case CSEXP: {
                    const char *str = file.get_string(next_int(iter));
                    int count = next_int(iter);
                    func(f, "SEXP\t%s %d", str, count);
                    return {CSEXP,two_args};
                }

                case CSTI: {
                    func(f, "STI");
                    return {CSTI, no_args};
                }

                case CSTA: {
                    func(f, "STA");
                    return {CSTA, no_args};
                }

                case CJMP: {
                    int offset = next_int(iter);
                    func(f, "JMP\t0x%.8x", offset);
                    return {CJMP, one_arg};
                }

                case CEND:
                case CRET: {
                    func(f, (l == 6) ? "END" : "RET");
                    return {static_cast<Code>(x), no_args};
                }

                case CDROP: {
                    func(f, "DROP");
                    return {CDROP,no_args};
                }

                case CDUP: {
                    func(f, "DUP");
                    return {CDUP, no_args};
                }

                case CSWAP: {
                    func(f, "SWAP");
                    return {CSWAP, no_args};
                }

                case CELEM: {
                    func(f, "ELEM");
                    return {CELEM, no_args};
                }

                case CCJMPZ:
                case CCJMPNZ: {
                    int offset = next_int(iter);
                    func(f, (l == 0) ? "CJMPz\t0x%.8x" : "CJMPnz\t0x%.8x", offset);
                    return {static_cast<Code>(x),one_arg};
                }

                case CBEGIN:
                case CCBEGIN: {
                    int arg_count = next_int(iter), local_count = next_int(iter);
                    func(f, (l == 2) ? "BEGIN\t%d " : "CBEGIN\t%d ", arg_count);
                    func(f, "%d", local_count);
                    return {static_cast<Code>(x), two_args};
                }

                case CCLOSURE: {
                    size_t offset = next_int(iter), n = next_int(iter);
                    for (size_t i = 0; i < n; i++) {
                        int typ = next_byte(iter), ind = next_int(iter);
                        switch (typ) {
                            case 0: fprintf (f, "G(%d)", ind); break;
                            case 1: fprintf (f, "L(%d)", ind); break;
                            case 2: fprintf (f, "A(%d)", ind); break;
                            case 3: fprintf (f, "C(%d)", ind); break;
                            default: failure("incorrect closure argument");
                        }
                    };
                    func(f, "CLOSURE\t0x%.8x", offset);
                    return {CCLOSURE, two_args + n * one_arg};
                }

                case CCALLC: {
                    int arg_count = next_int(iter);
                    func(f, "CALLC\t%d", arg_count);
                    return {CCALLC, one_arg};
                }

                case CCALL: {
                    int offset = next_int(iter);
                    int arg_count = next_int(iter);
                    func(f, "CALL\t0x%.8x %d", offset, arg_count);
                    return {CCALL,two_args};
                }

                case CTAG: {
                    const char *name = file.get_string(next_int(iter));
                    int arg_count = next_int(iter);
                    func(f, "TAG\t%s %d", name, arg_count);
                    return {CTAG, two_args};
                }

                case CARRAY: {
                    int size = next_int(iter);
                    func(f, "ARRAY\t%d", size);
                    return {CARRAY, one_arg};
                }

                case CFAIL: {
                    int ln = next_int(iter), col = next_int(iter);
                    func(f, "FAIL\t%d %d", ln, col);
                    return {CFAIL, two_args};
                }

                case CLINE: {
                    int ln = next_int(iter);
                    func(f, "LINE\t%d", ln);
                    return {CLINE, one_arg};
                }

                case CLREAD: {
                    func(f, "CALL\tLread");
                    return {CLREAD, no_args};
                }

                case CLWRITE: {
                    func(f, "CALL\tLwrite");
                    return {CLWRITE, no_args};
                }

                case CLLENGTH: {
                    func(f, "CALL\tLlength");
                    return {CLLENGTH, no_args};
                }

                case CLSTRING: {
                    func(f, "CALL\tLstring");
                    return {CLSTRING, no_args};
                }

                case CBARRAY: {
                    int size = next_int(iter);
                    func(f, "CALL\tBarray\t%d", size);
                    return {CBARRAY, one_arg};
                }

                default:
                    failure("incorrect bytecode");
            }
    }
}

struct BlockCount {
    const char *begin;
    size_t insn_count;
    size_t count;
    bool operator<(const BlockCount &oth) const {
        return std::make_tuple(count, insn_count, begin) <
               std::make_tuple(oth.count, oth.insn_count, oth.begin);
    }
};

enum Label {
    NOT_VISITED = 0,
    EP_VISITED = 1,
    C_VISITED = 2
};

void count_occurs_impl(const Bytefile &file, const std::vector<Label> &labels, size_t insn_count, std::unordered_map<StoredBlock, size_t, StoredBlockHash> &block_count, const char *&iter) {
    Block block{iter, 0};
    for (size_t i = 0; i < insn_count; ++i) {
        auto [code, size] = print_instruction(NULL, &consume, file, iter); 
        block.push_back(size);
        auto label = labels[iter - file.get_ip()];
        if (code == CSTOP || label != C_VISITED) {
            return;
        }
    }
    
    Instruction instr;
    Instruction prev;
    do {
        if (block_count[block.sb] == 0) {
            block_count[block.sb] = 1;
        } else {
            block_count[block.sb] += 1;
        }
        block.pop_front();
        prev = instr;
        instr = print_instruction(NULL, &consume, file, iter);
        block.push_back(instr.size);
    } while (labels[iter - file.get_ip()] == C_VISITED);
}

std::vector<BlockCount> count_occurencies(const Bytefile &file, const std::vector<Label> &labels, size_t insn_count, std::vector<BlockCount>& counts) {
    std::unordered_map<StoredBlock, size_t, StoredBlockHash> block_count;
    
    const char *begin = file.get_ip();
    const char *iter = begin;
    
    while (iter < file.get_end()) {
        count_occurs_impl(file, labels, insn_count, block_count, iter);
        while (iter < file.get_end() && labels[iter - begin] == NOT_VISITED) {
            ++iter;
        }
    }
    for (const auto& [block, count] : block_count) {
        counts.emplace_back(block.begin, insn_count, count);
    }
    return counts;
}

void print_sequencies(FILE *f, const Bytefile &file, std::vector<BlockCount> &&counts) {
    for (auto [begin, size, count] : counts) {
        if (count == 0) {
            break;
        }
        fprintf(f, "Count %lu of len %lu:\n", count, size);
        for (auto i = 0; i < size; ++i) {
            print_instruction(f, &fprintf, file, begin);
            fprintf(f, "\n");
        }
    }
}

std::vector<Label> label_code(const Bytefile &f) {
    std::vector<Label> labels(f.get_end() - f.get_ip(), NOT_VISITED);
    std::deque<size_t> q;

    auto visit_offset = [&](size_t offset, Label label) {
        if (labels[offset] == NOT_VISITED) {
            labels[offset] = label;
            q.push_back(offset);
        }
    };

    for (size_t i = 0; i < f.get_public_symbols_size(); ++i) {
        visit_offset(f.get_public_offset(i), EP_VISITED);
    }
    const char *begin = f.get_ip();
    while (!q.empty()) {
        auto offset = q.front();
        q.pop_front();
        auto iter = begin + offset;
        auto code = print_instruction(NULL, &consume, f, iter).code;
        if (code == CJMP) {
            const char *t = begin + offset + sizeof(char);
            size_t to = next_int(t);
            visit_offset(to, EP_VISITED);
        } else if (code == CCJMPZ || code == CCJMPNZ || code == CCALL) {
            const char *t = begin + offset + sizeof(char);
            std::array<size_t, 2> p = {(size_t)next_int(t), (size_t)(iter - begin)};
            for (auto to : p) {
                visit_offset(to, EP_VISITED);
            }
        } else if (code != CEND) {
            visit_offset(iter - begin, C_VISITED);
        }
    }
    return labels;
}

int main(int argc, char *argv[]) {
    const Bytefile f(argv[1]);
    end = f.get_end();
    auto labels = label_code(f);
    std::vector<BlockCount> counts;
    count_occurencies(f, labels, 1, counts);
    count_occurencies(f, labels, 2, counts);
    std::sort(counts.begin(), counts.end());
    std::reverse(counts.begin(), counts.end());
    print_sequencies(stdout, f, std::move(counts));
    return 0;
}