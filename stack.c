#include "stack.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FREE_VALUE TRUE

Stack *stack_empty(void) {
  Stack *stack;
  if (!(stack = malloc(sizeof(Stack)))) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  stack->top = NULL;
  return stack;
}

Node *create_node(void *value) {
  Node *node;
  if (!(node = malloc(sizeof(Node)))) {
    perror("malloc");
    return NULL;
  }
  node->value = value;
  return node;
}

_Bool stack_is_empty(Stack *stack) {
  return stack->top == NULL;
}

void *stack_top(Stack *stack)  {
  if (!stack_is_empty(stack)) {
    return stack->top->value;
  }else {
    perror("Stack empty");
  }
  return NULL;
}

Stack *stack_push(Stack *stack, void *value) {
  Node *node = NULL;
  if((node = create_node(value)) != NULL)  {
    node->next = stack->top;
    stack->top = node;
    return stack;
  }else {
    perror("Error pushing to stack");
  }
  return stack;
}

Stack *stack_pop(Stack *stack)  {
  if(stack->top != NULL) {
    Node *temp = stack->top;
    stack->top = stack->top->next;
    if(FREE_VALUE == 0 && temp->value != NULL)  {
      free(temp->value);
    }
    free(temp);
  }
  return stack;
}

void stack_free(Stack *stack)  {
  while (stack->top != NULL) {
    stack_pop(stack);
  }
  free(stack);
}

void stack_print(Stack *stack)  {
  if (stack->top != NULL) {
    Node *node = stack->top;
    while (node != NULL) {
      if (node->value != NULL) {
        printf("%s\n", (char*)node->value);
      }else {
        perror("Stack value: NULL");
      }
      node = node->next;
    }
  }
}
