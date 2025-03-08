#ifndef BYTERUN_H
#define BYTERUN_H
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

// #define DEBUG

#ifdef DEBUG
#define debug(...) fprintf(__VA_ARGS__)
#else
#define debug(...)
#endif


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

void op_stack_init(int global_area_size);

typedef enum location {
  GLOBAL    = 0,
  LOCAL     = 1,
  ARGUMENT  = 2,
  CAPTURED  = 3,
} location;

void op_stack_cleanup();

#endif   //LAMA_RUNTIME_VIRT_STACK_H
