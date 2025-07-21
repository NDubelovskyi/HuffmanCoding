#ifndef priority_queue_h
#define priority_queue_h

// Be careful of circular dependencies

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

// Helper methods
static void sink(PriorityQueue *queue, size_t root_node);
static void swim(PriorityQueue *queue, size_t leaf_node);
static bool priority(const Node *const first_node, const Node *const second_node);
static void exchange(Node **first_node, Node **second_node);
static int resize(PriorityQueue *queue, const size_t new_capacity);

// Testing
void priority_queue_testing(void);

#endif // priority_queue_h