/*
 *  Author: Jergus Lysy
 *  Date:   3.8. 2016
 *  Desc:   Simple arithmetic expression parser
 *  Tested: gcc 4.9.2 (x86_64-linux-gnu)
**/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include "stack.h"

/* Uncomment for more information about processing an input expression */
//#define DEBUG

/* Stack size */
#define STACK_SIZE	200

/* More operators can be specified, but must be bijection and preserve indexes */
char op_table[] = {'+','-','*','/', 0};
int priority_table[] = {1,1,2,2};

/**
 *  Function get_op_priority()
 *  Input operator op
 *  Ouput mapped priority
 */
int get_op_priority(char op)
{
	char *ptr;

	ptr = strchr(op_table, op);
	if (ptr == NULL) {
		return 0;
	}

	assert((ptr - op_table) >= 0);
	return priority_table[ptr - op_table];
}

/**
 *  Function evaluate_nums()
 *  Input numbers num1, num2 and operator op
 *  Ouput num1 `op` num2
 */
long evaluate_nums(long num1, long num2, char op)
{
	long res;

	switch (op) {
	case '+':
		res = num1 + num2;
		break;
	case '-':
		res = num1 - num2;
		break;
	case '/':
		res = num1 / num2;
		break;
	case '*':
		res = num1 * num2;
		break;
	}

	return res;
}

/**
 *  Function evaluate_num_stack()
 *  Input pointer to stack s and operator op
 *  Ouput pop(s) `op` pop(s)
 */
int evaluate_num_stack(Stack *s, char op)
{
	long int n1, n2, n3;

	if (stack_pop(s, &n1) || stack_pop(s, &n2))
		return 1;

#ifdef DEBUG
	printf("DEBUG: Values %ld and %ld were removed from number stack.\n", n1, n2);
#endif
	n3 = evaluate_nums(n2, n1, op);
	stack_push(s, n3);
#ifdef DEBUG
	printf("DEBUG: Value %ld was added into number stack.\n", n3);
#endif
	return 0;
}

/**
 *  Function clear_stacks()
 *  Input num and op stacks
 *  Ouput clear both stacks
 */
void clear_stacks(Stack *s1, Stack *s2)
{
	while (!stack_pop(s1, NULL))
		;

	while (!stack_pop(s2, NULL))
		;
}

/* ---------------- MAIN ---------------- */

int main(void)
{
	char *line = NULL;
	char *exp = NULL;
	size_t len = 0;
	ssize_t read;
	Stack *op_stack = NULL;
	Stack *num_stack = NULL;
	char op;
	long int result;
	int error = 0;

	/* Initialization of stacks */
	op_stack = malloc(sizeof(Stack));
	num_stack = malloc(sizeof(Stack));

	/* Can lead to a memory leak if one of them was allocated properly (let OS take care of it) */
	if ((op_stack == NULL) || (num_stack == NULL)) {
		printf("Error: cannot reserve space for internal structures.\n");
		return EXIT_FAILURE;
	}

	/* Can lead to a memory leak if one of them was allocated properly (let OS take care of it) */
	if (stack_init(num_stack, STACK_SIZE) || stack_init(op_stack, STACK_SIZE)) {
		printf("Error: Cannot allocate required memory for stack. Please check memory restriction.");
		return EXIT_FAILURE;
	}

	printf("Type 'bye' to quit.\n\n");

	/* Loop until 'bye' is read */
	for (;;) {
		read = getline(&line, &len, stdin);
		if (read == -1) {
			perror("getline");
			break;
		}

		if (!strncmp(line, "bye", 3)) {
			free(line);
			break;
		}

		/* Prepare input. Simulate brackets on a given expression.
		 * This is risky, but getline manages safety in some way.
		*/
		exp = line;
		line[read - 1] = ')';
		stack_push(op_stack, '(');

		while (*exp && *exp != '\n') {
			/* Process brackets */
			if (*exp == '(') {
				if (stack_push(op_stack, (long) *exp)) {
					error = 2;
					break;
				}
#ifdef DEBUG
				printf("DEBUG: '%c' was added into operator stack.\n", *exp);
#endif
			} else if (*exp == ')') {
				int bracket = 0;

				while (!stack_pop(op_stack, (long *) &op)) {
					if (op == '(') {
						bracket = 1;
						break;
					}
#ifdef DEBUG
					printf("DEBUG: '%c' was removed from operator stack.\n", op);
#endif
					if (evaluate_num_stack(num_stack, op)) {
						error = 1;
						break;
					}
				}

				if (!bracket) {
					error = 1;
					break;
				}
			/* Process operators */
			} else if (strchr(op_table, *exp)) {
				while (!stack_top(op_stack, (long *) &op) && (get_op_priority(op) >= get_op_priority(*exp))) {
					if (evaluate_num_stack(num_stack, op)) {
						error = 1;
						break;
					}

					stack_pop(op_stack, NULL);
#ifdef DEBUG
					printf("DEBUG: '%c' was removed from operator stack.\n", op);
#endif
				}

				if (stack_push(op_stack, (long) *exp)) {
					error = 2;
					break;
				}
#ifdef DEBUG
				printf("DEBUG: '%c' was added into operator stack.\n", *exp);
#endif
			/* Process numbers */
			} else if (isdigit(*exp)) {
				long int num;
				char *end;

				errno = 0;
				num = strtol(exp, &end, 10);
				if ((errno && (num == 0)) || (errno == ERANGE && ((num == LONG_MAX) || (num == LONG_MIN)))) {
					error = 2;
					break;
				}

				if (stack_push(num_stack, num)) {
					error = 2;
					break;
				}
#ifdef DEBUG
				printf("DEBUG: '%ld' was added into number stack.\n", num);
#endif
				exp = end;
				continue;
			/* Ignore white spaces */
			} else if (*exp == ' ') {
				while (*exp == ' ')
					exp++;
				continue;
			} else {
				error = 1;
				break;
			}

			exp++;
		}

		if (!error && (num_stack->size == 1) && !op_stack->size) {
			stack_pop(num_stack, &result);
			printf("Result is %ld\n\n", result);
		} else {
			if (error == 2)
				printf("Memory error\n\n");
			else
				printf("Syntax error\n\n");
		}

		/*Clear stacks*/
		clear_stacks(num_stack, op_stack);

		/* Prepare for next input */
		free(line);
		line = NULL;
		exp = NULL;
		error = 0;
	}

	/* Destroy stacks */
	stack_destroy(op_stack);
	stack_destroy(num_stack);
	free(op_stack);
	free(num_stack);

	return EXIT_SUCCESS;
}
