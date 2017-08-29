/*
 *  This is a simple, non-optimized stack.
**/

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

#define STACK_TYPE	long int

typedef struct stack {
	STACK_TYPE *data;
	size_t size;
	size_t max_size;
} Stack;

int stack_init(Stack *s, size_t size);
void stack_destroy(Stack *s);
int stack_top(Stack *s, STACK_TYPE *c);
int stack_pop(Stack *s, STACK_TYPE *c);
int stack_push(Stack *s, STACK_TYPE c);
int stack_empty(Stack *s);

#endif
