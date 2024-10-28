#include "virt_stack.h"
#include "runtime.h"

#include <malloc.h>

virt_stack *vstack_create () { return malloc(sizeof(virt_stack)); }

void vstack_destruct (virt_stack *st) { free(st); }

void vstack_init (virt_stack *st) {
  st->cur          = RUNTIME_VSTACK_SIZE;
  st->buf[st->cur] = 0;
}

void vstack_push (virt_stack *st, size_t value) {
  if (st->cur == 0) { failure("stack overflow"); }
  --st->cur;
  st->buf[st->cur] = value;
}

void vstack_alloc_count (virt_stack *st, size_t count) {
  if (st->cur < count) { failure("stack overflow"); }
  st->cur -= count;
}

void vstack_reverse_count (virt_stack *st, size_t count) {
  if (st->cur + count >= RUNTIME_VSTACK_SIZE)  { failure("stack underflow"); }
  for (int bot = st->cur + count - 1, top = st->cur; bot > top; --bot, ++top) {
    int tmp = st->buf[bot];
    st->buf[bot] = st->buf[top];
    st->buf[top] = tmp;
  }
}

size_t vstack_pop (virt_stack *st) {
  if (st->cur == RUNTIME_VSTACK_SIZE) { failure("stack underflow"); }
  size_t value = st->buf[st->cur];
  ++st->cur;
  return value;
}

void vstack_pop_count (virt_stack *st, size_t count) {
  if (st->cur + count >= RUNTIME_VSTACK_SIZE)  { failure("stack underflow"); }
  st->cur += count;
}

void *vstack_top (virt_stack *st) { return st->buf + st->cur; }
void *vstack_after_top (virt_stack *st) { return st->buf + st->cur + 1; }

size_t vstack_size (virt_stack *st) { return RUNTIME_VSTACK_SIZE - st->cur; }

size_t vstack_kth_from_start (virt_stack *st, size_t k) {
  assert(vstack_size(st) > k);
  return st->buf[RUNTIME_VSTACK_SIZE - 1 - k];
}

void *vstack_kth_ptr_from_start (virt_stack *st, size_t k) {
  assert(vstack_size(st) > k);
  return st->buf + RUNTIME_VSTACK_SIZE - 1 - k;
}

size_t vstack_kth_from_top (virt_stack *st, size_t k) {
  if (st->cur + k >= RUNTIME_VSTACK_SIZE) { failure("stack underflow"); }
  return st->buf[st->cur + k]; 
}
