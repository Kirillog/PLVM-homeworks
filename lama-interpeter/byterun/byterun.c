/* Lama SM Bytecode interpreter */

#include "../runtime/runtime.h"
#include "../runtime/gc.h"
#include "../runtime/virt_stack.h"
#include "bytefile.h"
#include "stack_frame.h"
#include "op_stack.h"


extern size_t __gc_stack_top, __gc_stack_bottom;

#define PRE_GC()                                                                                   \
  bool flag = false;                                                                               \
  flag      = __gc_stack_top == 0;                                                                 \
  if (flag) { __gc_stack_top = (size_t)__builtin_frame_address(0); }                               \
  assert(__gc_stack_top != 0);                                                                     \
  assert(__builtin_frame_address(0) <= (void *)__gc_stack_top);

#define POST_GC()                                                                                  \
  assert(__builtin_frame_address(0) <= (void *)__gc_stack_top);                                    \
  if (flag) { __gc_stack_top = 0; }


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

  return r->contents;
}

void interpret(bytefile *bf, FILE *f, char *fname) {
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
  char *lds [] = {"LD", "LDA", "ST"};
  do {
    char x = BYTE,
         h = (x & 0xF0) >> 4,
         l = x & 0x0F;

    fprintf (f, "0x%.8x:\t", ip - bf->code_ptr-1);
    
    switch (h) {
    case 15:
      goto stop;
      
    case 0: {
      fprintf (f, "BINOP\t%s", ops[l-1]);
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
        fprintf (f, "CONST\t%d", val);
        break;
      }
        
      case  1: {
        char *str = STRING; 
        vstack_push(st, (size_t)Bstring(str));
        fprintf (f, "STRING\t%s", str);
        break;
      }
          
      case  2: {
        char *name    = STRING;
        int count     = INT;
        Bsexp(st, count, UNBOX(LtagHash(name)));
        fprintf (f, "SEXP\t%s ", name);
        fprintf (f, "%d", count);
        break;
      }

        
      case  3: {
        size_t value  = vstack_pop(st);
        size_t *ptr   = (size_t *)vstack_pop(st);
        *ptr = value;
        vstack_push(st, value);
        fprintf (f, "STI");
        break;
      }
        
      case  4: {
        size_t *v = (size_t *)vstack_pop(st);
        size_t i  = vstack_pop(st);
        size_t *x = (size_t *)vstack_pop(st);
        size_t *ptr = Bsta(v, i, x);
        vstack_push(st, (size_t)ptr);
        fprintf (f, "STA");
        break;
      }
        
      case  5: {
        int offset = INT;
        ip = bf->code_ptr + offset;
        fprintf (f, "JMP\t0x%.8x", offset);
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
        fprintf (f, (l == 6) ? "END" : "RET");
        if (frame == NULL) {
          goto stop;  
        }
        break;
      }
        
      case  8: {
        vstack_pop(st);
        fprintf (f, "DROP");
        break;
      }
        
      case  9: {
        vstack_push(st, *(size_t *)vstack_top(st));
        fprintf (f, "DUP");
        break;
      }
        
      case 10: {
        size_t val1 = vstack_pop(st);
        size_t val2 = vstack_pop(st);
        vstack_push(st, val1);
        vstack_push(st, val2);
        fprintf (f, "SWAP");
        break;
      }

      case 11: {
        int i     = vstack_pop(st);
        int *arr  = (int *)vstack_pop(st);
        vstack_push(st, (size_t)Belem(arr, i));
        fprintf (f, "ELEM");
        break;
      }
        
      default:
        FAIL;
      }
      break;
    }

    case 2: { // LD
      int *addr = op_stack_load_addr(st, frame, l, INT);
      vstack_push(st, *addr);
      fprintf(f, "LD");
      break;
    }

    case 3: { // LDA
      int *addr = op_stack_load_addr(st, frame, l, INT);
      vstack_push(st, (size_t)addr);
      fprintf(f, "LDA");
      break;
    }

    case 4: { // ST
      int value = *(size_t *)vstack_top(st);
      int* addr = op_stack_load_addr(st, frame, l, INT);
      *addr = value;
      fprintf(f, "ST");
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
        fprintf (f, (l == 0) ? "CJMPz\t0x%.8x" : "CJMPnz\t0x%.8x", offset);
        break;
      }

      case  2:
      case  3: {
        int arg_count = INT, 
            local_count = INT;
        vstack_alloc_count(st, local_count);
        frame->local_count = local_count;
        fprintf (f, (l == 2) ? "BEGIN\t%d " : "CBEGIN\t%d ", arg_count);
        fprintf (f, "%d", local_count);
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
        vstack_pop_count(st, n);
        vstack_push(st, cls);
        fprintf (f, "CLOSURE\t0x%.8x", offset);
        break;
      }
          
      case  5: {
        int arg_count = INT;
        int * closure = (int *)vstack_kth_from_top(st, arg_count);

        vstack_reverse_count(st, arg_count);
        frame = stack_frame_call(frame, vstack_after_top(st), ip, true, arg_count);
        ip = bf->code_ptr + closure[0];

        fprintf (f, "CALLC\t%d", arg_count);
        break;
      }

        
      case  6: {
        int offset    = INT;
        int arg_count = INT;
        vstack_reverse_count(st, arg_count);
        frame = stack_frame_call(frame, vstack_after_top(st), ip, false, arg_count);
        ip = bf->code_ptr + offset;

        fprintf (f, "CALL\t0x%.8x ", offset);
        fprintf (f, "%d", arg_count);
        break;
      }
        
      case  7: {
        char *name    = STRING;
        int arg_count = INT;
        size_t tag = Btag((void *)vstack_pop(st), UNBOX(LtagHash(name)), BOX(arg_count));
        vstack_push(st, tag);
        fprintf (f, "TAG\t%s ", name);
        fprintf (f, "%d", arg_count);
        break;
      }
        
      case  8: {
        int size = INT;
        size_t arr = (size_t)Barray_patt((void *)vstack_pop(st), BOX(size));
        vstack_push(st, arr);
        fprintf (f, "ARRAY\t%d", size);
        break;
      }
        
      case  9: {
        int ln      = INT,
            col     = INT;
        Bmatch_failure((void *)vstack_pop(st), fname, ln, col);
        fprintf (f, "FAIL\t%d", ln);
        fprintf (f, "%d", col);
        break;
      }

      case 10: {
        int ln = INT;
        fprintf (f, "LINE\t%d", ln);
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
      fprintf (f, "PATT\t%s", pats[l]);
      break;
    }

    case 7: {
      switch (l) {
      case 0: {
        vstack_push(st, Lread());
        fprintf (f, "CALL\tLread");
        break;
      }
        
      case 1: {
        vstack_push(st, Lwrite(vstack_pop(st)));
        fprintf (f, "CALL\tLwrite");
        break;
      }

      case 2: {
        vstack_push(st, Llength((void *)vstack_pop(st)));
        fprintf (f, "CALL\tLlength");
        break;
      }

      case 3: {
        vstack_push(st, (size_t)Lstring((void *)vstack_pop(st)));
        fprintf (f, "CALL\tLstring");
        break;
      }

      case 4: {
        int size = INT;
        size_t arr = (size_t)Barray(st, size);
        vstack_pop_count(st, size);
        vstack_push(st, arr);
        fprintf (f, "CALL\tBarray\t%d", size);
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

    fprintf (f, "\n");
  }
  while (1);
  stop:
    op_stack_cleanup(st);
    __shutdown();
}

int main (int argc, char* argv[]) {
  bytefile *f = read_file (argv[1]);
  FILE *log = fopen("debug.log", "w");
  interpret(f, stderr, argv[1]);
  return 0;
}
