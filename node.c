#include "node.h"

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

// creates a node with a symbol and frequency
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->left = n->right = NULL;
        n->symbol = symbol;
        n->frequency = frequency;
    }
    return n;
}

// frees memory allocated to a node
void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
}

// Joins a left child node and right child node, returning a pointer to a created parent node
Node *node_join(Node *left, Node *right) {
    Node *n = node_create('$', left->frequency + right->frequency);
    n->left = left;
    n->right = right;
    return n;
}

// print to verify nodes are joined correctly
void node_print(Node *n) {
    printf("%" PRIu8, n->symbol);
    printf("%" PRIu64, n->frequency);
}
