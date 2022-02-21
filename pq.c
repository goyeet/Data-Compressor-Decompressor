#include "pq.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// struct for the priority queue ADT
struct PriorityQueue {
    uint32_t head; // head of the queue
    uint32_t tail; // tail of the queue
    uint32_t size; // size of the queue
    uint32_t capacity; // capacity of the queue
    Node **items;
};

// constructor for a priority queue
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (pq) {
        pq->head = 0;
        pq->tail = 0;
        pq->size = 0;
        pq->capacity = capacity;
        pq->items = (Node **) malloc(capacity * sizeof(Node *));
        if (!pq->items) {
            free(pq);
            pq = NULL;
        }
    }
    return pq;
}

// destructor for a priority queue
void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->items) {
        free((*q)->items);
        free(*q);
        *q = NULL;
    }
}

// checks if the priority queue is empty
bool pq_empty(PriorityQueue *q) {
    if (q->size == 0) {
        return true;
    } else {
        return false;
    }
}

// checks if the priority queue is full
bool pq_full(PriorityQueue *q) {
    if (q->size == q->capacity) {
        return true;
    } else {
        return false;
    }
}

// checks the size of a priority queue
uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

// Returns the parent index
static inline int parent_index(uint32_t i) {
    return (int) (i - 1) / 2;
}

// Returns the left index
static inline uint32_t left_index(uint32_t i) {
    return 2 * i + 1;
}

// Returns the right index
static inline uint32_t right_index(uint32_t i) {
    return 2 * i + 2;
}

// swaps two elements in the priority queue
void pq_swap(PriorityQueue *q, uint32_t i, uint32_t j) {
    Node *temp = q->items[i];
    q->items[i] = q->items[j];
    q->items[j] = temp;
}

// fixes heap to be a min heap
void pq_fix_heap(PriorityQueue *q) {
    uint32_t i = q->size - 1;
    while (parent_index(i) >= 0 && q->items[parent_index(i)]->frequency > q->items[i]->frequency) {
        pq_swap(q, parent_index(i), i);
        i = parent_index(i);
    }
}

// creates heap
void pq_heapify(PriorityQueue *q) {
    int i = 0;
    while (left_index(i) < q->size) {
        int smallest_child_index = left_index(i);
        if (right_index(i) < q->size
            && q->items[right_index(i)]->frequency < q->items[left_index(i)]->frequency) {
            smallest_child_index = right_index(i);
        }
        if (q->items[i]->frequency > q->items[smallest_child_index]->frequency) {
            pq_swap(q, i, smallest_child_index);
        } else {
            break;
        }
        i = smallest_child_index;
    }
}

// enqueues a node into the priority queue
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q)) {
        return false;
    }
    q->items[q->size] = n;
    q->size++;
    pq_fix_heap(q);
    return true;
}

// Dequeues a node from the priority queue, passing it back through the double pointer n
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }
    *n = q->items[0];
    q->items[0] = q->items[q->size - 1];
    q->size--;
    pq_heapify(q);
    return true;
}

// A debug function to print a priority queue
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size; i++) {
        node_print(q->items[i]);
    }
}
