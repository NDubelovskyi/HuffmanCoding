#ifndef priority_queue_h
#define priority_queue_h

#include "huffman.h"
#include <stdio.h>

#define INITIAL_CAPACITY 4
#define PARENT(i) (((i) - 1) / 2)
#define LEFT_CHILD(i) (2 * (i) + 1)
#define RIGHT_CHILD(i) ((2 * (i)) + 2)

// MinPQ: Priority Queue is made up of a collection of pointers to nodes
typedef struct {
    Node **nodes;               // Array of Node Pointers (Comparisons are made based on node's frequency)
    size_t size;                // The number of nodes in the array
    size_t capacity;            // The current maximum possible number of nodes in the array (used to allocate memory)
} PriorityQueue;

// PQ Constructor & Destroyer
PriorityQueue *create_priority_queue(void);
void free_priority_queue(PriorityQueue **queue);

// Public PQ Node-related methods
int push(PriorityQueue *queue, Node *node);
Node *pop(PriorityQueue *queue);
Node *peek(const PriorityQueue *queue);
size_t size(const PriorityQueue *queue);
bool is_empty(const PriorityQueue *queue);

// Testing
void print_priority_queue(const PriorityQueue *queue);
void priority_queue_testing(void);

#endif // priority_queue_h