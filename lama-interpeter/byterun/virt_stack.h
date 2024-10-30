#ifndef VIRT_STACK_H
#define VIRT_STACK_H
#include <stdio.h>

void vstack_init ();

void vstack_push (size_t value);

void vstack_alloc_count (size_t count);

void vstack_reverse_count (size_t count);

size_t vstack_pop ();

void vstack_pop_count (size_t count);

void *vstack_top ();

void *vstack_after_top ();

size_t vstack_size ();

size_t vstack_kth_from_start (size_t k);

void *vstack_kth_ptr_from_start (size_t k);

size_t vstack_kth_from_top (size_t k);

void vstack_debug(FILE* f);

#endif