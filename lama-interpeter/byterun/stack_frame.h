#ifndef STACK_FRAME_H
#define STACK_FRAME_H
#include <stdbool.h>
#include "../runtime/virt_stack.h"

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

stack_frame *init_call_stack(virt_stack *op_stack);

stack_frame *stack_frame_call(struct stack_frame *prev_frame, int *base_ptr, char* ret_addr, bool cls, int arg_count);

#endif