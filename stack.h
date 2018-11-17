/*Implementation of stack
* tfy17jfo
*/

#define TRUE 0
#define FALSE 1

typedef struct Node {
  void *value;
  struct Node *next;
}Node;

typedef struct Stack  {
	Node *top;
}Stack;

Stack *stack_empty(void);

Node *create_node(void *value);

_Bool stack_is_empty(Stack *stack);

void *stack_top(Stack *stack);

Stack *stack_push(Stack *stack, void *value);

Stack *stack_pop(Stack *stack);

void stack_free(Stack *stack);

void stack_print(Stack *stack);
