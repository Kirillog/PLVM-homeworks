#include "stack_frame.h"

stack_frame *init_call_stack() {
  stack_frame *frame = calloc(1, sizeof(stack_frame));
  frame->base_ptr = vstack_after_top();
  return frame;
}

stack_frame *stack_frame_call(struct stack_frame *prev_frame, int *base_ptr, char* ret_addr, bool cls, int arg_count) {
  stack_frame *frame  = calloc(1, sizeof(stack_frame));
  frame->base_ptr     = base_ptr;
  frame->ret_addr     = ret_addr;
  frame->cls          = cls;
  frame->arg_count    = arg_count;
  frame->prev         = prev_frame;
  return frame;
}