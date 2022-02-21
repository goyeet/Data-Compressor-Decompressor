#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <stdint.h>
#include <stdlib.h>

// constructs a huffman tree given a computed histogram
Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *pq = pq_create(ALPHABET);
    for (uint32_t i = 0; i < ALPHABET; i++) { // enqueue all leaf nodes
        uint64_t frequency = hist[i];
        if (frequency > 0) {
            Node *node = node_create(i, frequency);
            enqueue(pq, node);
        }
    }
    while (pq_size(pq) >= 2) { // join leaf nodes
        Node *left, *right;
        dequeue(pq, &left);
        dequeue(pq, &right);
        enqueue(pq, node_join(left, right));
    }
    Node *root_node = NULL;
    dequeue(pq, &root_node); // dequeue root
    pq_delete(&pq);
    return root_node;
}

// Populates a code table, building the code for each symbols in the Huffman tree
void build_codes(Node *root, Code table[static ALPHABET]) {
    static Code c = { 0 };
    if (!root) { // Huffman tree is empty
        return;
    }
    if (!root->left && !root->right) { // leaf
        table[root->symbol] = c;
    } else { // interior node
        uint8_t popped;
        code_push_bit(&c, 0);
        build_codes(root->left, table);
        code_pop_bit(&c, &popped);
        code_push_bit(&c, 1);
        build_codes(root->right, table);
        code_pop_bit(&c, &popped);
    }
}

// Conducts a post-order traversal of the Huffman tree rooted at root, writing it to outfile
void dump_tree(int outfile, Node *root) {
    if (!root) {
        return;
    }
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);
    if (!root->left && !root->right) { // leaf
        uint8_t symbol[2] = { 'L', root->symbol };
        write_bytes(outfile, symbol, 2);
    } else { // interior node
        uint8_t symbol = { 'I' };
        write_bytes(outfile, &symbol, 1);
    }
}

// Conducts a post-order traversal of the Huffman tree rooted at root, writing it to outfile
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *s = stack_create(nbytes);
    for (uint16_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') { // leaf
            stack_push(s, node_create(tree[i + 1], 1));
            i++; // Skip next element.
        } else if (tree[i] == 'I') { // interior node
            Node *left;
            Node *right;
            stack_pop(s, &right);
            stack_pop(s, &left);
            stack_push(s, node_join(left, right));
        }
    }
    Node *root = NULL;
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}

// destructor for a huffman tree
void delete_tree(Node **root) {
    if (!*root) {
        return;
    }
    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);
    node_delete(root);
}
