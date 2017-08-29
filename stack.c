#include "stack.h"

int stack_init(Stack *s, size_t size)
{
	s->size = 0;
	s->max_size = size;
	s->data = malloc(sizeof(STACK_TYPE) * s->max_size);

	if (s->data == NULL)
		return 1;

	return 0;
}

void stack_destroy(Stack *s)
{
	if (s != NULL) {
		if (s->data != NULL)
			free(s->data);
	}

	s->size = 0;
}

int stack_top(Stack *s, STACK_TYPE *c)
{
	if (s == NULL)
		return 1;

	if ((s->size == 0) || (c == NULL))
		return 1;

	*c = s->data[s->size - 1];
	return 0;
}

int stack_pop(Stack *s, STACK_TYPE *c)
{
	if (s == NULL)
		return 1;

	if (s->size == 0)
		return 1;

	if (c != NULL)
		*c = s->data[s->size - 1];

	s->size--;
	return 0;
}

int stack_push(Stack *s, STACK_TYPE c)
{
	if (s == NULL)
		return 1;

	if (s->size >= s->max_size)
		return 1;

	s->data[s->size] = c;
	s->size++;
	return 0;
}

int stack_empty(Stack *s)
{
	return (!s->size) ? 0 : 1;
}
