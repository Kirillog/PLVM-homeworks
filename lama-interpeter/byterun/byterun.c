/* Lama SM Bytecode interpreter */

#include "../runtime/runtime.h"
#include "../runtime/gc.h"
#include "virt_stack.h"
#include "bytefile.h"
#include "stack_frame.h"
#include "op_stack.h"
#include <stdio.h>


extern size_t __gc_stack_top, __gc_stack_bottom;

#define PRE_GC()                                                                                 \

#define POST_GC()                                                                                  \

#ifdef DEBUG
#define debug(...) fprintf(__VA_ARGS__)
#else
#define debug(...)
#endif


int binop(int opcode, int a, int b) {
  switch (opcode) {
  case 0:
    return a + b;
  case 1:
    return a - b;
  case 2:
    return a * b;
  case 3:
    return a / b;
  case 4:
    return a % b;
  case 5:
    return a < b;
  case 6:
    return a <= b;
  case 7:
    return a > b;
  case 8:
    return a >= b;
  case 9:
    return a == b;
  case 10:
    return a != b;
  case 11:
    return a && b;
  case 12:
    return a || b;
  default:
    failure("ERROR: invalid binary opcode %d", opcode);
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
static void *Bsexp (virt_stack *op_stack, int n, int tag) {
  int     i;
  int     ai;
  data   *r;

  PRE_GC();

  int fields_cnt   = n;
  r                = (data *)alloc_sexp(fields_cnt);
  ((sexp *)r)->tag = 0;

  for (i = n; i > 0; --i) {
    ai                      = vstack_pop(op_stack);
    ((int *)r->contents)[i] = ai;
  }

  ((sexp *)r)->tag = tag;

  POST_GC();
  return (int *)r->contents;
}
static void *Barray (virt_stack *op_stack, int n) {
  int     i, ai;
  data   *r;

  PRE_GC();

  r = (data *)alloc_array(n);

  for (i = n - 1; i >= 0; --i) {
    ai                      = vstack_pop(op_stack);
    ((int *)r->contents)[i] = ai;
  }

  POST_GC();
  return r->contents;
}
static void *Bclosure (virt_stack *op_stack, int n, void *entry) {
  int           i, ai;
  data         *r;

  PRE_GC();

  r = (data *)alloc_closure(n + 1);
  push_extra_root((void **)&r);
  ((void **)r->contents)[0] = entry;

  for (i = n; i >= 1; --i) {
    ai                      = vstack_pop(op_stack);
    ((int *)r->contents)[i] = ai;
  }

  POST_GC();
  pop_extra_root((void **)&r);

  return r->contents;
}

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

void interpret(bytefile *bf, char *fname) {
  #ifdef DEBUG
  FILE *f = fopen("debug.log", "w");
  #endif
  __init();
  virt_stack *st      = op_stack_init(bf->global_area_size);
  stack_frame *frame  = init_call_stack(st);
  char *ip     = bf->code_ptr;
# define INT            (ip += sizeof (int), *(int*)(ip - sizeof (int)))
# define BYTE           *ip++
# define STRING         get_string (bf, INT)
# define FAIL           failure ("ERROR: invalid opcode %d-%d\n", h, l)
  char *ops [] = {"+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"};
  char *pats[] = {"=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun"};
  do {
    char x = BYTE,
         h = (x & 0xF0) >> 4,
         l = x & 0x0F;

    // debug (f, "frame base ptr: %p\n", frame->base_ptr);
    debug (f, "0x%.8u:\t", ip - bf->code_ptr-1);
    
    switch (h) {
    case 15:
      goto stop;
      
    case 0: {
      debug (f, "BINOP\t%s", ops[l-1]);
      int right = UNBOX(vstack_pop(st));
      int left = UNBOX(vstack_pop(st));
      vstack_push(st, BOX(binop(l - 1, left, right)));
      break;
    }
      
    case 1: {
      switch (l) {
      case  0: {
        int val = INT;
        vstack_push(st, BOX(val));
        debug (f, "CONST\t%d", val);
        break;
      }
        
      case  1: {
        char *str = STRING; 
        int value = (size_t)Bstring(str);
        vstack_push(st, value);
        debug (f, "STRING\t%s %x", str, value);
        break;
      }
          
      case  2: {
        char *name    = STRING;
        int count     = INT;
        size_t value = (size_t)Bsexp(st, count, UNBOX(LtagHash(name)));
        vstack_push(st, value);
        debug (f, "SEXP\t%s %d", name, count);
        break;
      }
        
      case  3: {
        size_t value  = vstack_pop(st);
        size_t *ptr   = (size_t *)vstack_pop(st);
        *ptr = value;
        vstack_push(st, value);
        debug (f, "STI");
        break;
      }
        
      case  4: {
        size_t *v = (size_t *)vstack_pop(st);
        size_t i  = vstack_pop(st);
        size_t *x = (size_t *)vstack_pop(st);
        size_t *ptr = Bsta(v, i, x);
        vstack_push(st, (size_t)ptr);
        debug (f, "STA");
        break;
      }
        
      case  5: {
        int offset = INT;
        ip = bf->code_ptr + offset;
        debug (f, "JMP\t0x%.8x", offset);
        break;
      }
        
      case  6: 
      case  7: {
        int rv = vstack_pop(st);
        vstack_pop_count(st, frame->local_count);
        vstack_pop_count(st, frame->arg_count);
        if (frame->cls) { vstack_pop(st); }
        vstack_push(st, rv);
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
        
      case  8: {
        vstack_pop(st);
        debug (f, "DROP");
        break;
      }
        
      case  9: {
        vstack_push(st, *(size_t *)vstack_top(st));
        debug (f, "DUP");
        break;
      }
        
      case 10: {
        size_t val1 = vstack_pop(st);
        size_t val2 = vstack_pop(st);
        vstack_push(st, val1);
        vstack_push(st, val2);
        debug (f, "SWAP");
        break;
      }

      case 11: {
        int i     = vstack_pop(st);
        int *arr  = (int *)vstack_pop(st);
        vstack_push(st, (size_t)Belem(arr, i));
        debug (f, "ELEM");
        break;
      }
        
      default:
        FAIL;
      }
      break;
    }

    case 2: { // LD
      int ind = INT;
      int *addr = op_stack_load_addr(st, frame, l, ind);
      vstack_push(st, *addr);
      #ifdef DEBUG
      debug_ld(f, h, l, ind);
      #endif
      break;
    }

    case 3: { // LDA
      int ind = INT; 
      int *addr = op_stack_load_addr(st, frame, l, ind);
      vstack_push(st, (size_t)addr);
      vstack_push(st, (size_t)addr);
      #ifdef DEBUG
      debug_ld(f, h, l, ind);
      #endif
      break;
    }

    case 4: { // ST
      int ind = INT;
      int value = *(size_t *)vstack_top(st);
      int* addr = op_stack_load_addr(st, frame, l, ind);
      *addr = value;
      #ifdef DEBUG
      debug_ld(f, h, l, ind);
      #endif
      break;
    }

    case 5:
      switch (l) {
      case  0:
      case  1: {
        int offset = INT;
        size_t value = UNBOX(vstack_pop(st));
        if (l == 0 && value == 0 || l != 0 && value != 0) {
          ip = bf->code_ptr + offset;
        }
        debug (f, (l == 0) ? "CJMPz\t0x%.8x" : "CJMPnz\t0x%.8x", offset);
        break;
      }

      case  2:
      case  3: {
        int arg_count = INT, 
            local_count = INT;
        vstack_alloc_count(st, local_count);
        frame->local_count = local_count;
        debug (f, (l == 2) ? "BEGIN\t%d " : "CBEGIN\t%d ", arg_count);
        debug (f, "%d", local_count);
        break;
      }
        
      case  4: {
        int offset = INT;
        int n = INT;
        for (int i = 0; i < n; i++) {
          int typ = BYTE,
              ind = INT;
          int *addr = op_stack_load_addr(st, frame, typ, ind);
          vstack_push(st, *addr);
        };
        size_t cls = (size_t)Bclosure(st, n, (void *)offset);
        vstack_push(st, cls);
        debug (f, "CLOSURE\t0x%.8x", offset);
        break;
      }
          
      case  5: {
        int arg_count = INT;
        int * closure = (int *)vstack_kth_from_top(st, arg_count);

        vstack_reverse_count(st, arg_count);
        frame = stack_frame_call(frame, vstack_after_top(st), ip, true, arg_count);
        ip = bf->code_ptr + closure[0];

        debug (f, "CALLC\t%d", arg_count);
        break;
      }

        
      case  6: {
        int offset    = INT;
        int arg_count = INT;
        vstack_reverse_count(st, arg_count);
        int *base_ptr = vstack_after_top(st);
        frame = stack_frame_call(frame, base_ptr, ip, false, arg_count);
        ip = bf->code_ptr + offset;
        debug (f, "CALL\t0x%.8x %d", offset, arg_count);
        break;
      }
        
      case  7: {
        char *name    = STRING;
        int arg_count = INT;
        size_t tag = Btag((void *)vstack_pop(st), LtagHash(name), BOX(arg_count));
        vstack_push(st, tag);
        debug (f, "TAG\t%s %d", name, arg_count);
        break;
      }
        
      case  8: {
        int size = INT;
        size_t arr = (size_t)Barray_patt((void *)vstack_pop(st), BOX(size));
        vstack_push(st, arr);
        debug (f, "ARRAY\t%d", size);
        break;
      }
        
      case  9: {
        int ln      = INT,
            col     = INT;
        Bmatch_failure((void *)vstack_pop(st), fname, ln, col);
        debug (f, "FAIL\t%d %d", ln, col);
        break;
      }

      case 10: {
        int ln = INT;
        debug (f, "LINE\t%d", ln);
        break;
      }

      default:
        FAIL;
      }
      break;
      
    case 6: {
      void *x = (void *)vstack_pop(st); 
      switch (l) {
        case 0: {
          void *y = (void *)vstack_pop(st);
          vstack_push(st, Bstring_patt(x, y));
          break;
        }
        case 1: {
          vstack_push(st, Bstring_tag_patt(x));
          break;
        }
        case 2: {
          vstack_push(st, Barray_tag_patt(x));
          break;
        }
        case 3: {
          vstack_push(st, Bsexp_tag_patt(x));
          break;
        }
        case 4: {
          vstack_push(st, Bboxed_patt(x));
          break;
        }
        case 5: {
          vstack_push(st, Bunboxed_patt(x));
          break;
        }
        case 6: {
          vstack_push(st, Bclosure_tag_patt(x));
          break;
        }
        default: {
          FAIL;
        }
      }
      debug (f, "PATT\t%s", pats[l]);
      break;
    }

    case 7: {
      switch (l) {
      case 0: {
        vstack_push(st, Lread());
        debug (f, "CALL\tLread");
        break;
      }
        
      case 1: {
        vstack_push(st, Lwrite(vstack_pop(st)));
        debug (f, "CALL\tLwrite");
        break;
      }

      case 2: {
        vstack_push(st, Llength((void *)vstack_pop(st)));
        debug (f, "CALL\tLlength");
        break;
      }

      case 3: {
        vstack_push(st, (size_t)Lstring((void *)vstack_pop(st)));
        debug (f, "CALL\tLstring");
        break;
      }

      case 4: {
        int size = INT;
        size_t arr = (size_t)Barray(st, size);
        vstack_push(st, arr);
        debug (f, "CALL\tBarray\t%d", size);
        break;
      }

      default:
        FAIL;
      }
    }
    break;
      
    default:
      FAIL;
    }
    // vstack_debug(st, f);
    debug (f, "\n");
  }
  while (1);
  stop:
    op_stack_cleanup(st);
    __shutdown();
}

int main (int argc, char* argv[]) {
  bytefile *f = read_file (argv[1]);
  interpret(f, argv[1]);
  return 0;
}
