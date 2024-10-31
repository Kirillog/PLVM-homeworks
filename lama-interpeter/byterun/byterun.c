/* Lama SM Bytecode interpreter */

#include "../runtime/runtime.h"
#include "../runtime/gc.h"
#include "bytefile.h"
#include "byterun.h"
#include <stdio.h>

#define RUNTIME_VSTACK_SIZE 100000

// (__gc_stack_top, __gc_stack_bottom)
extern size_t *__gc_stack_top, *__gc_stack_bottom;

size_t mem[RUNTIME_VSTACK_SIZE];

void vstack_init () {
  __gc_stack_bottom = mem + RUNTIME_VSTACK_SIZE;
  __gc_stack_top = __gc_stack_bottom - 1;
}

static inline void vstack_push (size_t value) {
  if (__gc_stack_top == mem) { failure("ERROR: stack overflow\n"); }
  *(__gc_stack_top--) = value;
}

static inline void vstack_alloc_count (size_t count) {
  if (__gc_stack_top - count <= mem) { failure("ERROR: stack overflow\n"); }
  __gc_stack_top -= count;
}

static inline void vstack_reverse_count (size_t count) {
  if (__gc_stack_bottom - __gc_stack_top <= count)  { failure("ERROR: stack underflow in reverse\n"); }
  for (size_t *bot = __gc_stack_top + count, *top = __gc_stack_top + 1; bot > top; --bot, ++top) {
    size_t tmp = *bot;
    *bot = *top;
    *top = tmp;
  }
}

static inline size_t vstack_pop () {
  if (__gc_stack_bottom - __gc_stack_top == 1) { failure("ERROR: stack underflow in pop\n"); }
  return *(++__gc_stack_top);
}

static inline void vstack_pop_count (size_t count) {
  if (__gc_stack_bottom - __gc_stack_top <= count)  { failure("ERROR: stack underflow in pop count\n"); }
  __gc_stack_top += count;
}

static inline void *vstack_top () { return __gc_stack_top + 1; }

static inline void *vstack_after_top () { return __gc_stack_top; }

static inline void *vstack_kth_ptr_from_start (size_t k) {
  if (__gc_stack_bottom - __gc_stack_top <= k) { failure("ERROR: stack underflow in kth from start\n"); }
  return __gc_stack_bottom - (k + 1);
}

static inline size_t vstack_kth_from_top (size_t k) {
  if (__gc_stack_bottom - __gc_stack_top <= k) { failure("ERROR: stack underflow in kth from top\n"); }
  return *(__gc_stack_top + k + 1);
}

void vstack_debug(FILE* f) {
  fprintf(f, "\n+------+");
  for (size_t *cur = __gc_stack_top + 1; cur < __gc_stack_bottom; cur++) {
    fprintf(f, "\n|%lu", *cur);
  }
  fprintf(f, "\n+______+");
}

/* 
  +--------------+
  | Frame n      |
  +--------------+
  | base_ptr     |
  | ret_offset   |
  | cls          |
  | arg_count    |
  | local_count  | // lazy set after call
  | prev         | --+
  +--------------+   |
                     |
                     |
  +--------------+ <-+
  | Frame (n - 1)|
  +--------------+
*/

typedef struct stack_frame {
  int *base_ptr;
  char *ret_addr;
  bool cls;
  int arg_count;
  int local_count;
  struct stack_frame *prev;
} stack_frame;

stack_frame *init_call_stack() {
  stack_frame *frame = calloc(1, sizeof(stack_frame));
  frame->base_ptr = vstack_after_top();
  return frame;
}

static inline stack_frame *stack_frame_call(struct stack_frame *prev_frame, int *base_ptr, char* ret_addr, bool cls, int arg_count) {
  stack_frame *frame  = calloc(1, sizeof(stack_frame));
  frame->base_ptr     = base_ptr;
  frame->ret_addr     = ret_addr;
  frame->cls          = cls;
  frame->arg_count    = arg_count;
  frame->prev         = prev_frame;
  return frame;
}

void op_stack_init(int global_area_size) {
  vstack_init();
  vstack_alloc_count(global_area_size);
}

static inline void *op_stack_load_addr(const stack_frame * frame, location loc, size_t ind) {
  switch (loc) {
    case GLOBAL: {
      return (void *)vstack_kth_ptr_from_start(ind);
    }
    case LOCAL: {
      return frame->base_ptr - ind;
    }
    case ARGUMENT: {
      return frame->base_ptr + (1 + ind);
    }
    case CAPTURED: {
      int *closure = *((int **)frame->base_ptr + frame->arg_count + 1);
      return closure + 1 + ind;
    }
    default: {
      failure("ERROR: unexpected location %d\n", loc);
    }
  }
}

extern void *Bstring(void *p);
extern int LtagHash (char *s);
extern void *Bsta (void *v, int i, void *x);
extern void *Belem (void *p, int i);
extern int Btag (void *d, int t, int n);
extern int Barray_patt (void *d, int n);

extern void Bmatch_failure (void *v, char *fname, int line, int col);

extern int Bstring_patt (void *x, void *y);
extern int Bclosure_tag_patt (void *x);
extern int Bboxed_patt (void *x);
extern int Bunboxed_patt (void *x);
extern int Barray_tag_patt (void *x);
extern int Bstring_tag_patt (void *x);
extern int Bsexp_tag_patt (void *x);

extern int Lread ();
extern int Lwrite (int n);
extern int Llength (void *p);
extern void *Lstring (void *p);

static void *Bsexp (int n, int tag) {
  int     i;
  int     ai;
  data   *r;

  int fields_cnt   = n;
  r                = (data *)alloc_sexp(fields_cnt);
  ((sexp *)r)->tag = 0;

  for (i = n; i > 0; --i) {
    ai                      = vstack_pop();
    ((int *)r->contents)[i] = ai;
  }

  ((sexp *)r)->tag = tag;

  return (int *)r->contents;
}
static inline void *Barray (int n) {
  int     i, ai;
  data   *r;

  r = (data *)alloc_array(n);

  for (i = n - 1; i >= 0; --i) {
    ai                      = vstack_pop();
    ((int *)r->contents)[i] = ai;
  }

  return r->contents;
}
static void *Bclosure (int n, void *entry) {
  int           i, ai;
  data         *r;

  r = (data *)alloc_closure(n + 1);
  ((void **)r->contents)[0] = entry;

  for (i = n; i >= 1; --i) {
    ai                      = vstack_pop();
    ((int *)r->contents)[i] = ai;
  }

  return r->contents;
}
#ifdef DEBUG
void debug_ld(FILE *f, char h, char l, int ind) {
  char *lds [] = {"LD", "LDA", "ST"};
  debug (f, "%s\t", lds[h-2]);
  switch (l) {
  case 0: debug (f, "G(%d)", ind); break;
  case 1: debug (f, "L(%d)", ind); break;
  case 2: debug (f, "A(%d)", ind); break;
  case 3: debug (f, "C(%d)", ind); break;
  }
}
#endif

typedef enum BinopCode {
  ADD = 0,
  SUB = 1, 
  MUL = 2, 
  DIV = 3,
  MOD = 4,
  LT  = 5,
  LEQ = 6,
  GT  = 7,
  GEQ = 8,
  EQ  = 9,
  NEQ = 10,
  AND = 11,
  OR = 12 
} BinopCode;

static inline int binop(BinopCode opcode, int a, int b) {
  switch (opcode) {
  case ADD:
    return a + b;
  case SUB:
    return a - b;
  case MUL:
    return a * b;
  case DIV:
    return a / b;
  case MOD:
    return a % b;
  case LT:
    return a < b;
  case LEQ:
    return a <= b;
  case GT:
    return a > b;
  case GEQ:
    return a >= b;
  case EQ:
    return a == b;
  case NEQ:
    return a != b;
  case AND:
    return a && b;
  case OR:
    return a || b;
  default:
    failure("ERROR: invalid binary opcode %d\n", opcode);
  }
}

enum HCode {
  CBINOP  = 0,
  CLD     = 2,
  CLDA    = 3,
  CST     = 4,
  CPATT   = 6,
  CSTOP   = 15,
};

enum Patt {
  STRING_PATT     = 0,
  STRING_TAG_PATT = 1,
  ARRAY_TAG_PATT  = 2,
  SEXP_TAG_PATT   = 3,
  BOXED_PATT      = 4,
  UNBOXED_PATT    = 5,
  CLOSURE_PATT    = 6,
};

enum Code {
  CCONST  = 16,
  CSTRING = 17,
  CSEXP   = 18,
  CSTI    = 19,
  CSTA    = 20,
  CJMP    = 21,
  CEND    = 22,
  CRET    = 23,
  CDROP   = 24,
  CDUP    = 25,
  CSWAP   = 26,
  CELEM   = 27,

  CCJMPZ  = 80,
  CCJMPNZ = 81,
  CBEGIN  = 82,
  CCBEGIN = 83,
  CCLOSURE= 84,
  CCALLC  = 85,
  CCALL   = 86,
  CTAG    = 87,
  CARRAY  = 88,
  CFAIL   = 89,
  CLINE   = 90,

  CLREAD  = 112,
  CLWRITE = 113,
  CLLENGTH= 114,
  CLSTRING= 115,
  CBARRAY = 116
};

void interpret(const bytefile *bf, char *end_file, const char *fname) {
  #ifdef DEBUG
  FILE *f = fopen("debug.log", "w");
  #endif
  __init();
  op_stack_init(bf->global_area_size);
  stack_frame *frame  = init_call_stack();
  char *ip     = bf->code_ptr;
  #define FAIL           failure ("ERROR: invalid opcode %d-%d\n", h, l)
  #define INT            (ip + sizeof(int) < end_file) ? (ip += sizeof (int), *(int*)(ip - sizeof (int))) : (failure("ERROR: end of file reached\n"), 0)
  #define BYTE           (ip < end_file) ? *ip++ : (failure("ERROR: end of file reached\n"), 0)
  // get_string panics with incorrect pos argument
  #define STRING         get_string (bf, INT)
  #ifdef DEBUG
  static const char * const ops [] = {"+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"};
  static const char * const pats[] = {"=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun"};
#endif
  do {
    unsigned char x = BYTE,
                  h = (x & 0xF0) >> 4,
                  l = x & 0x0F;

    debug (f, "0x%.8u:\t", ip - bf->code_ptr-1);
    
    switch (h) {
    case CSTOP:
      goto stop;
      
    case CBINOP: {
      debug (f, "BINOP\t%s", ops[l-1]);
      int right = UNBOX(vstack_pop());
      int left = UNBOX(vstack_pop());
      vstack_push(BOX(binop(l - 1, left, right)));
      break;
    }

    case CLD: {
      int ind = INT;
      int *addr = op_stack_load_addr(frame, l, ind);
      vstack_push(*addr);
      #ifdef DEBUG
      debug_ld(f, h, l, ind);
      #endif
      break;
    }

    case CLDA: {
      int ind = INT; 
      int *addr = op_stack_load_addr(frame, l, ind);
      vstack_push((size_t)addr);
      vstack_push((size_t)addr);
      #ifdef DEBUG
      debug_ld(f, h, l, ind);
      #endif
      break;
    }

    case CST: {
      int ind = INT;
      int value = *(size_t *)vstack_top();
      int* addr = op_stack_load_addr(frame, l, ind);
      *addr = value;
      #ifdef DEBUG
      debug_ld(f, h, l, ind);
      #endif
      break;
    }

    case CPATT: {
      void *z = (void *)vstack_pop(); 
      switch (l) {
        case STRING_PATT: {
          void *y = (void *)vstack_pop();
          vstack_push(Bstring_patt(z, y));
          break;
        }
        case STRING_TAG_PATT: {
          vstack_push(Bstring_tag_patt(z));
          break;
        }
        case ARRAY_TAG_PATT: {
          vstack_push(Barray_tag_patt(z));
          break;
        }
        case SEXP_TAG_PATT: {
          vstack_push(Bsexp_tag_patt(z));
          break;
        }
        case BOXED_PATT: {
          vstack_push(Bboxed_patt(z));
          break;
        }
        case UNBOXED_PATT: {
          vstack_push(Bunboxed_patt(z));
          break;
        }
        case CLOSURE_PATT: {
          vstack_push(Bclosure_tag_patt(z));
          break;
        }
        default: {
          FAIL;
        }
      }
      debug (f, "PATT\t%s", pats[l]);
      break;
    }

    default:
      switch (x) {
        case CCONST: {
          int val = INT;
          vstack_push(BOX(val));
          debug (f, "CONST\t%d", val);
          break;
        }
        
        case CSTRING: {
          const char *str = STRING; 
          int value = (size_t)Bstring((void *)str);
          vstack_push(value);
          debug (f, "STRING\t%s %x", str, value);
          break;
        }
          
        case  CSEXP: {
          const char *name    = STRING;
          int count     = INT;
          size_t value = (size_t)Bsexp(count, UNBOX(LtagHash((void *)name)));
          vstack_push(value);
          debug (f, "SEXP\t%s %d", name, count);
          break;
        }
        
        case  CSTI: {
          size_t value  = vstack_pop();
          size_t *ptr   = (size_t *)vstack_pop();
          *ptr = value;
          vstack_push(value);
          debug (f, "STI");
          break;
        }
        
        case  CSTA: {
          size_t *v = (size_t *)vstack_pop();
          size_t i  = vstack_pop();
          size_t *x = (size_t *)vstack_pop();
          size_t *ptr = Bsta(v, i, x);
          vstack_push((size_t)ptr);
          debug (f, "STA");
          break;
        }
        
        case  CJMP: {
          int offset = INT;
          ip = bf->code_ptr + offset;
          debug (f, "JMP\t0x%.8x", offset);
          break;
        }
        
        case  CEND: 
        case  CRET: {
          int rv = vstack_pop();
          vstack_pop_count(frame->local_count);
          vstack_pop_count(frame->arg_count);
          if (frame->cls) { vstack_pop(); }
          vstack_push(rv);
          ip = frame->ret_addr;
          stack_frame* prev = frame->prev;
          free(frame);
          frame = prev;
          debug (f, (l == 6) ? "END" : "RET");
          if (frame == NULL) {
            goto stop;  
          }
          break;
        }
        
        case  CDROP: {
          vstack_pop();
          debug (f, "DROP");
          break;
        }
        
        case  CDUP: {
          vstack_push(*(size_t *)vstack_top());
          debug (f, "DUP");
          break;
        }
        
        case CSWAP: {
          size_t val1 = vstack_pop();
          size_t val2 = vstack_pop();
          vstack_push(val1);
          vstack_push(val2);
          debug (f, "SWAP");
          break;
        }

        case CELEM: {
          int i     = vstack_pop();
          int *arr  = (int *)vstack_pop();
          vstack_push((size_t)Belem(arr, i));
          debug (f, "ELEM");
          break;
        }
        
        case  CCJMPZ:
        case  CCJMPNZ: {
          int offset = INT;
          size_t value = UNBOX(vstack_pop());
          if (l == 0 && value == 0 || l != 0 && value != 0) {
            ip = bf->code_ptr + offset;
          }
          debug (f, (l == 0) ? "CJMPz\t0x%.8x" : "CJMPnz\t0x%.8x", offset);
          break;
        }

        case  CBEGIN:
        case  CCBEGIN: {
          int arg_count = INT, 
              local_count = INT;
          vstack_alloc_count(local_count);
          frame->local_count = local_count;
          debug (f, (l == 2) ? "BEGIN\t%d " : "CBEGIN\t%d ", arg_count);
          debug (f, "%d", local_count);
          break;
        }
          
        case  CCLOSURE: {
          size_t offset = INT,
                 n = INT;
          for (int i = 0; i < n; i++) {
            int typ = BYTE,
                ind = INT;
            int *addr = op_stack_load_addr(frame, typ, ind);
            vstack_push(*addr);
          };
          size_t cls = (size_t)Bclosure(n, (void *)offset);
          vstack_push(cls);
          debug (f, "CLOSURE\t0x%.8x", offset);
          break;
        }
          
        case  CCALLC: {
          int arg_count = INT;
          int * closure = (int *)vstack_kth_from_top(arg_count);

          vstack_reverse_count(arg_count);
          frame = stack_frame_call(frame, vstack_after_top(), ip, true, arg_count);
          ip = bf->code_ptr + closure[0];

          debug (f, "CALLC\t%d", arg_count);
          break;
        }
        
        case  CCALL: {
          int offset    = INT;
          int arg_count = INT;
          vstack_reverse_count(arg_count);
          int *base_ptr = vstack_after_top();
          frame = stack_frame_call(frame, base_ptr, ip, false, arg_count);
          ip = bf->code_ptr + offset;
          debug (f, "CALL\t0x%.8x %d", offset, arg_count);
          break;
        }
          
        case  CTAG: {
          const char *name    = STRING;
          int arg_count = INT;
          size_t tag = Btag((void *)vstack_pop(), LtagHash((void *)name), BOX(arg_count));
          vstack_push(tag);
          debug (f, "TAG\t%s %d", name, arg_count);
          break;
        }
            
        case  CARRAY: {
          int size = INT;
          size_t arr = (size_t)Barray_patt((void *)vstack_pop(), BOX(size));
          vstack_push(arr);
          debug (f, "ARRAY\t%d", size);
          break;
        }
        
        case  CFAIL: {
          int ln      = INT,
              col     = INT;
          Bmatch_failure((void *)vstack_pop(), (char *)fname, ln, col);
          debug (f, "FAIL\t%d %d", ln, col);
          break;
        }

        case CLINE: {
          int ln = INT;
          debug (f, "LINE\t%d", ln);
          break;
        }

        case CLREAD: {
          vstack_push(Lread());
          debug (f, "CALL\tLread");
          break;
        }
        
        case CLWRITE: {
          vstack_push(Lwrite(vstack_pop()));
          debug (f, "CALL\tLwrite");
          break;
        }

        case CLLENGTH: {
          vstack_push(Llength((void *)vstack_pop()));
          debug (f, "CALL\tLlength");
          break;
        }

        case CLSTRING: {
          vstack_push((size_t)Lstring((void *)vstack_pop()));
          debug (f, "CALL\tLstring");
          break;
        }

        case CBARRAY: {
          int size = INT;
          size_t arr = (size_t)Barray(size);
          vstack_push(arr);
          debug (f, "CALL\tBarray\t%d", size);
          break;
        }

      default:
        FAIL;
      }
      
  }

#ifdef DEBUG
    vstack_debug(f);
#endif
    debug (f, "\n");
  }
  while (1);
  stop:
    __shutdown();
}

int main (int argc, char* argv[]) {
  char *ef = NULL;
  bytefile *f = read_file (argv[1], &ef);
  interpret(f, ef, argv[1]);
  close_file(f);
  return 0;
}
