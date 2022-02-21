#include "stack.h"

#include "node.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Struct for a stack ADT
struct Stack {
    uint32_t top; // Index of the next empty slot
    uint32_t capacity; // Capacity of Stack
    Node **items;
};

// The constructor for a stack
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack)); // Allocates memory for stack
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = malloc(capacity * sizeof(Node *)); // dynamically allocated array
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

// The destructor for a stack
void stack_delete(Stack **s) {
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

// Returns true if the stack is empty and false otherwise.
bool stack_empty(Stack *s) {
    if (s->top == 0) {
        return true;
    } else {
        return false;
    }
}

// Returns true if the stack is full and false otherwise.
bool stack_full(Stack *s) {
    if (s->top == s->capacity) {
        return true;
    } else {
        return false;
    }
}

// Returns the number of items in a stack
uint32_t stack_size(Stack *s) {
    return s->top;
}

// pushes a Node n to the stack
bool stack_push(Stack *s, Node *n) {
    if (s == NULL || stack_full(s) == true) {
        return false;
    } else {
        s->items[s->top] = n;
        s->top += 1;
        return true;
    }
}

// pops a node from the stack and returns the popped value to **n
bool stack_pop(Stack *s, Node **n) {
    if (s == NULL || stack_empty(s) == true) {
        return false;
    } else {
        s->top -= 1;
        *n = s->items[s->top];
        return true;
    }
}

// prints stack for debugging
/*void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i += 1) {
        printf("1");
        if (i + 1 != s->top) {
            printf(" ");
        }
    }
}*/
