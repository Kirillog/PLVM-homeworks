#ifndef OP_STACK_H
#define OP_STACK_H
#include "../runtime/virt_stack.h"
#include "stack_frame.h"
/*
  +--------+
  | cls    | // optional
  +--------+
  | arg  n |
  | ...... |
  | arg  0 | 
  +--------+ 
  | loc  0 | <- base_ptr
  | .....  |
  | loc  m |
  |        |
  
*/

virt_stack *op_stack_init(int global_area_size);

typedef enum location {
  GLOBAL    = 0,
  LOCAL     = 1,
  ARGUMENT  = 2,
  CAPTURED  = 3,
} location;

void *op_stack_load_addr(virt_stack *op_stack, stack_frame * frame, location loc, size_t ind);

void op_stack_cleanup(virt_stack *op_stack);

#endif