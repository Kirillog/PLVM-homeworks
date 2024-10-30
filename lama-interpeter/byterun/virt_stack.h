#ifndef VIRT_STACK_H
#define VIRT_STACK_H
#include <string.h>
#include <stdio.h>
#define RUNTIME_VSTACK_SIZE 100000

#include <assert.h>
#include <stddef.h>

struct {
  size_t buf[RUNTIME_VSTACK_SIZE + 1];
  size_t cur;
} typedef virt_stack;

virt_stack *vstack_create ();

void vstack_destruct (virt_stack *st);

void vstack_init (virt_stack *st);

void vstack_push (virt_stack *st, size_t value);

void vstack_alloc_count (virt_stack *st, size_t count);

void vstack_reverse_count (virt_stack *st, size_t count);

size_t vstack_pop (virt_stack *st);
void vstack_pop_count (virt_stack *st, size_t count);

void *vstack_top (virt_stack *st);

void *vstack_after_top (virt_stack *st);

size_t vstack_size (virt_stack *st);

size_t vstack_kth_from_start (virt_stack *st, size_t k);

void *vstack_kth_ptr_from_start (virt_stack *st, size_t k);

size_t vstack_kth_from_top (virt_stack *st, size_t k);

void vstack_debug(virt_stack *st, FILE* f);

#endif   //LAMA_RUNTIME_VIRT_STACK_H
