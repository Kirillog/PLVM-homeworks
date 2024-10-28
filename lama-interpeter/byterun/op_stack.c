#include "op_stack.h"

extern size_t __gc_stack_bottom;

virt_stack *op_stack_init(int global_area_size) {
  virt_stack *st = vstack_create();
  vstack_init(st);
  __gc_stack_bottom = (size_t)vstack_top(st);
  vstack_alloc_count(st, global_area_size);
  return st;
}

void *op_stack_load_addr(virt_stack *op_stack, stack_frame * frame, location loc, size_t ind) {
  switch (loc) {
    case GLOBAL: {
      return (void *)vstack_kth_ptr_from_start(op_stack, ind);
    }
    case LOCAL: {
      return frame->base_ptr + ind;
    }
    case ARGUMENT: {
      return frame->base_ptr - (1 + ind);
    }
    case CAPTURED: {
      int *closure = frame->base_ptr - frame->arg_count - 1;
      return closure + 1 + ind;
    }
    default: {
      failure("unexpected location");
    }
  }
}

void op_stack_cleanup(virt_stack *op_stack) {
  vstack_destruct(op_stack);
}