extern "C" {
#include "bytefile.h"
#include "code.h"
#include "../runtime/runtime.h"
#include <stdbool.h>
#include <stdio.h>
}

#include <algorithm>
#include <vector>
#include <deque>
#include <array>

const char *end;

static inline int next_int(const char *&ip) {
    if (ip + sizeof(int) > end) {
        failure("end of file reached\n");
    }
    ip += sizeof(int);
    return *reinterpret_cast<const int *>(ip - sizeof(int));
}

static inline char next_byte(const char *&ip) {
    if (ip + sizeof(char) > end) {
        failure("end of file reached\n");
    }
    return *ip++;
}

struct ByteCode {
    unsigned char x, h, l;
};

static inline ByteCode get_bytecode(const char *&ip) {
    unsigned char x = next_byte(ip);
    unsigned char h = (x & 0xF0) >> 4;
    unsigned char l = x & 0x0F;
    return {x, h, l};
}

typedef struct Instruction {
    Code code;
    std::vector<int> args;
    int depth_change;
    ssize_t min_change;
    ssize_t max_change;
} Instruction;

static inline Instruction decode_instruction(const bytefile *file, const char *&iter, ssize_t depth) {
    auto [x, h, l] = get_bytecode(iter);
    switch (h) {
        case CSTOP:
            return Instruction{CSTOP, {}, 0, 0, 0};

        case CBINOP: {
            return Instruction{CBINOP, {}, -1, -2, 0};
        }

        case CLD: {
            int ind = next_int(iter);
            return Instruction{(Code)h, {ind}, 1, 0, 1};
        }
        case CLDA: {
            int ind = next_int(iter);
            return Instruction{(Code)h, {ind}, 2, 0, 2};
        }
        case CST: {
            int ind = next_int(iter);
            return Instruction{(Code)h, {ind}, 0, -1, 0};
        }

        case CPATT: {
            switch (l) {
                case STRING_PATT: {
                    return Instruction{CPATT, {}, -1, -2, 0};
                }
            }
            return Instruction{CPATT, {}, 0, -1, 0};
        }

        default:
            switch (x) {
                case CCONST: {
                    int val = next_int(iter);
                    return Instruction{CCONST, {val}, 1, 0, 1};
                }

                case CSTRING: {
                    int pos = next_int(iter);
                    const char *str = get_string(file, pos);
                    return Instruction{CSTRING, {pos}, 1, 0, 1};
                }

                case CSEXP: {
                    int pos = next_int(iter);
                    const char *str = get_string(file, pos);
                    int count = next_int(iter);
                    return Instruction{CSEXP,{pos, count}, -count + 1, -count, std::max(-count + 1, 0)};
                }

                case CSTI: {
                    return Instruction{CSTI, {}, -1, -2, -1};
                }

                case CSTA: {
                    return Instruction{CSTA, {}, -2, -3, -2};
                }

                case CJMP: {
                    int offset = next_int(iter);
                    return Instruction{CJMP, {offset}, 0, 0, 0};
                }

                case CEND:
                case CRET: {
                    return Instruction{static_cast<Code>(x), {}, 0, 0, 0};
                }

                case CDROP: {
                    return Instruction{CDROP, {}, -1, -1, 0};
                }

                case CDUP: {
                    return Instruction{CDUP, {}, 1, 0, 1};
                }

                case CSWAP: {
                    return Instruction{CSWAP, {}, 0, -2, 0};
                }

                case CELEM: {
                    return Instruction{CELEM, {}, -1, -2, 0};
                }

                case CCJMPZ:
                case CCJMPNZ: {
                    int offset = next_int(iter);
                    return Instruction{static_cast<Code>(x),{offset}, -1, -1, 0};
                }

                case CBEGIN:
                case CCBEGIN: {
                    int arg_count = next_int(iter), local_count = next_int(iter);
                    return Instruction{static_cast<Code>(x), {arg_count, local_count}, local_count, 0, local_count};
                }

                case CCLOSURE: {
                    int offset = next_int(iter), n = next_int(iter);
                    std::vector<int> args = {offset, n};
                    for (int i = 0; i < n; i++) {
                        int typ = next_byte(iter), ind = next_int(iter);
                        args.push_back(typ);
                        args.push_back(ind);
                    };
                    return Instruction{CCLOSURE, args, 1, 0, n};
                }

                case CCALLC: {
                    int arg_count = next_int(iter);
                    return Instruction{CCALLC, {arg_count}, -arg_count - 1 + 1, -arg_count - 1, 0};
                }

                case CCALL: {
                    int offset = next_int(iter);
                    int arg_count = next_int(iter);
                    return Instruction{CCALL,{offset, arg_count}, -arg_count + 1, -arg_count, std::max(0, -arg_count + 1)};
                }

                case CTAG: {
                    int pos = next_int(iter);
                    const char *name = get_string(file, pos);
                    int arg_count = next_int(iter);
                    return Instruction{CTAG, {pos, arg_count}, 0, -1, 0};
                }

                case CARRAY: {
                    int size = next_int(iter);
                    return Instruction{CARRAY, {size}, 0, -1, 0};
                }

                case CFAIL: {
                    int ln = next_int(iter), col = next_int(iter);
                    return Instruction{CFAIL, {ln, col}, -1, -1, 0};
                }

                case CLINE: {
                    int ln = next_int(iter);
                    return Instruction{CLINE, {ln}, 0, 0, 0};
                }

                case CLREAD: {
                    return Instruction{CLREAD, {}, 1, 0, 1};
                }

                case CLWRITE: {
                    return Instruction{CLWRITE, {}, 0, -1, 0};
                }

                case CLLENGTH: {
                    return Instruction{CLLENGTH, {}, 0, -1, 0};
                }

                case CLSTRING: {
                    return Instruction{CLSTRING, {}, 0, -1, 0};
                }

                case CBARRAY: {
                    int size = next_int(iter);
                    return Instruction{CBARRAY, {size}, -size + 1, -size, std::max(0, -size + 1)};
                }

                default:
                    failure("incorrect bytecode");
            }
    }
}

struct TraverseInfo {
    size_t offset;
    ssize_t st_depth;
};

struct Function {
    size_t begin_offset;
    ssize_t stack_depth;
    bool closed;

    bool operator<(size_t offset) const {
        return begin_offset < offset;
    }
};

#define ASSERT(expr) if (!(expr)) return false;
static constexpr const int semiword_mask = (1 << 16) - 1;
static constexpr const int semiword_bytes = 2;

extern "C" bool verify(const bytefile *f) {
    const char *begin = f->code_ptr;
    end = f->end;
    std::vector<ssize_t> depths(end - begin, -1);
    std::vector<Function> functions;
    std::deque<TraverseInfo> q;

    auto visit_offset = [&](size_t offset, ssize_t depth) {
        if (depths[offset] == -1) {
            depths[offset] = depth;
            // fprintf(stderr, "set depth at %08x, depth: %d\n", offset, depth);
            q.push_back({offset, depth});
            return true;
        } else {
            auto flag = depths[offset] == depth; 
            // fprintf(stderr, "checking flag at %08x, got: %d, expected: %d\n", offset, depth, depths[offset]);
            return flag;
        }
    };

    visit_offset(0, 0);

    while (!q.empty()) {
        auto [offset, depth] = q.front();
        q.pop_front();
        auto iter = begin + offset;
        auto instr = decode_instruction(f, iter, depth);
        auto code = instr.code;
        auto func = std::lower_bound(functions.begin(), functions.end(), offset);
        // fprintf(stderr, "offset: 0x%08x, depth: %d\n", offset, depth);
        if (code == CBEGIN || code == CCBEGIN) {
            auto ins = functions.insert(func,{offset, instr.depth_change, false});
            ins->stack_depth = instr.max_change;
            ASSERT(visit_offset(iter - begin, instr.depth_change))
            continue;
        }
        --func;
        func->stack_depth = std::max(func->stack_depth, depth + instr.max_change);
        // fprintf(stderr, "depth: %d\n", depth + instr.min_change);
        ASSERT(depth + instr.min_change >= 0)
        ASSERT(func != functions.end())
        if (code == CEND) {
            func->closed = true;
        } else if (code == CJMP) {
            ASSERT(visit_offset(instr.args[0], depth + instr.depth_change))
        } else if (code == CCJMPZ || code == CCJMPNZ) {
            std::array<size_t, 2> p = {(size_t)instr.args[0], (size_t)(iter - begin)};
            for (auto to : p) {
                ASSERT(visit_offset(to, depth + instr.depth_change))
            }
        } else if (code == CCALL) {
            auto call_iter = begin + instr.args[0];
            auto begin_instr = decode_instruction(f, call_iter, 0);
            ASSERT(begin_instr.args[0] == instr.args[1])
            ASSERT(visit_offset(iter - begin, depth + instr.depth_change))
            ASSERT(visit_offset(instr.args[0], 0))
        } else if (code != CSTOP && code != CFAIL) {
            ASSERT(visit_offset(iter - begin, depth + instr.depth_change))
        }
    }
    for (const auto &func : functions) {
        // fprintf(stderr, "offset: 0x%08x, closed:%d\n", func.begin_offset, func.closed);
        ASSERT(func.closed)
        unsigned short *t = (unsigned short *)(begin + func.begin_offset + sizeof(char) + semiword_bytes);
        *t = func.stack_depth;
    }
    return true;
}