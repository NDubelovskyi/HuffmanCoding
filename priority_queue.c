#include "priority_queue.h"
#include <stdlib.h>

// Helper methods prototypes
static void sink(PriorityQueue *queue, size_t root_node);
static void swim(PriorityQueue *queue, size_t leaf_node);
static bool priority(const Node *const first_node, const Node *const second_node);
static void exchange(Node **first_node, Node **second_node);
static int resize(PriorityQueue *queue, const size_t new_capacity);

// PQ Constructor & Destroyer
// Creates an empty minimum priority queue by allocating necessary memories for appropriate structures
PriorityQueue *create_priority_queue(void) {
    // Allocate memory for the queue. Handle errors by freeing memory and returning nullptr
    PriorityQueue *queue = malloc(sizeof(PriorityQueue));
    if (queue == nullptr) {
        puts("Memory allocation failure for 'queue' at 'create_priority_queue()'");
        return nullptr;
    }

    // Allocate memory for the array of Node elements. Handle errors by freeing memory and returning nullptr
    queue->nodes = malloc(INITIAL_CAPACITY * sizeof(Node *));
    if (queue->nodes == nullptr) {
        puts("Memory allocation failure for 'nodes' at 'create_priority_queue()'");
        free(queue);
        queue = nullptr;
        return nullptr;
    }

    // Set initial size and capacity
    queue->size = 0;
    queue->capacity = INITIAL_CAPACITY;

    // Return the pointer to the priority queue structure
    return queue;
}

// Frees a PQ. The caller is responsible for freeing the individual nodes
void free_priority_queue(PriorityQueue **queue) {
    if (queue == nullptr || *queue == nullptr) return;

    // Free the allocated array of nodes
    free((*queue)->nodes);
    (*queue)->nodes = nullptr;

    // Free the allocated priority queue
    free(*queue);
    *queue = nullptr;
}


// Public PQ Node-related methods
// Pushes a Node into a PQ
int push(PriorityQueue *queue, Node *node) {
    // Error with invalid arguments
    if (queue == nullptr || node == nullptr) {
        puts("Invalid arguments: nullptr pointers incoming to 'push()'");
        return -1;
    }

    // Allocate more memory if needed. If allocation fails, print the error
    if (queue->size == queue->capacity) {
        if (resize(queue, 2 * queue->capacity) != 0) {
            puts("Memory reallocation (enlargement) failure at 'push()'");
            return -1;
        }
    }

    // Add a new Node, maintain the heap property, and update the size
    queue->nodes[queue->size] = node;
    swim(queue, queue->size);
    queue->size++;
    return 0;
}

// Pops a Node with the highest priority (the smallest frequency one) from a PQ
Node *pop(PriorityQueue *queue) {
    // Error with invalid arguments
    if (queue == nullptr) {
        puts("Invalid arguments: nullptr pointer incoming to 'pop()'");
        return nullptr;
    }

    // Empty queue exception
    if (queue->size == 0)  {
        puts("Element Does Not Exist: queue is empty at 'pop()'");
        return nullptr;
    }

    // Preserve the node to be returned
    Node *node = queue->nodes[0];

    // Move the last node to become the first
    queue->nodes[0] = queue->nodes[queue->size - 1];

    // Sink the first node to preserve the heap order
    sink(queue, 0);

    // Remove a dangling pointer / Remove the node from the array of nodes
    queue->nodes[queue->size - 1] = nullptr;
    queue->size--;

    // Shrink the array if needed. If allocation fails, print the error, but keep using the current PQ
    if (queue -> size > 0 && queue->size < queue->capacity / 4 && queue->capacity > 8) {
        if (resize(queue, queue->capacity / 2) != 0) {
            puts("Memory reallocation (shrinkage) failure at 'pop()'. Continue using the provided PQ");
        }
    }

    // Return a pointer of a popped node
    return node;
}

// Returns the highest priority node in a PQ
Node *peek(const PriorityQueue *queue) {
    // Error with invalid arguments
    if (queue == nullptr) {
        puts("Invalid arguments: nullptr pointer incoming to 'peek()'");
        return nullptr;
    }

    // Empty queue exception
    if (queue->size == 0)  {
        puts("Element Does Not Exist: queue is empty at 'peek()'");
        return nullptr;
    }

    // Return a pointer to the highest priority node
    return queue->nodes[0];
}

// Returns the size of a PQ
size_t size(const PriorityQueue *queue) {
    if (queue == nullptr) {
        puts("Invalid arguments: nullptr pointer incoming to 'size()'");
        return 0;
    }
    return queue->size;
}

// Returns true if a PQ is empty, false otherwise
bool is_empty(const PriorityQueue *queue) {
    if (queue == nullptr) {
        puts("Invalid arguments: nullptr pointer incoming to 'is_empty()'");
        return true;
    }
    return queue->size == 0;
}


// Helper Methods. Assumes valid arguments.
// Sinks the root node if it has lower priority than any of its children, maintaining the heap property of its subtree
static void sink(PriorityQueue *queue, size_t root_node) {
    // Checks if the root node has at least one child
    while (LEFT_CHILD(root_node) < queue->size) {
        // Left-most child of the root node
        size_t child_node = LEFT_CHILD(root_node);

        // If there is a right child, and it has a larger priority than the left one
        if (child_node + 1 < queue->size &&
            !priority(queue->nodes[child_node], queue->nodes[child_node + 1])) {
                // Then, switch the child node from left to right child
                child_node++;
        }

        // Break the loop if the root has higher priority than any of its children
        if (priority(queue->nodes[root_node], queue->nodes[child_node])) break;

        // Exchange the data between the root and child nodes
        exchange(&queue->nodes[root_node], &queue->nodes[child_node]);

        // Maintain the heap property further down the tree after the exchange
        root_node = child_node;
    }
}

// Trickles up the leaf node on the tree to preserve the heap property
static void swim(PriorityQueue *queue, size_t leaf_node) {
    // Checks if the leaf node didn't swim up to the root of the whole tree and if the priority of the leaf is still higher than its parent
    while (leaf_node > 0 && priority( queue->nodes[leaf_node], queue->nodes[PARENT(leaf_node)])) {
        // Trickle up the leaf node by exchanging its data with its parent
        exchange(&queue->nodes[leaf_node], &queue->nodes[PARENT(leaf_node)]);

        // Update the address of the initial leaf to its previous parent
        leaf_node = PARENT(leaf_node);
    }
}

// Min-heap priority: Returns whether the first node has higher priority than the second one
// In other words, returns whether the first node has a smaller frequency than the second one
static bool priority(const Node *const first_node, const Node *const second_node) {
    return first_node->frequency < second_node->frequency;
}

// Exchange the data between the first and second nodes in the PQ
static void exchange(Node **first_node, Node **second_node) {
    Node *temp = *first_node;
    *first_node = *second_node;
    *second_node = temp;
}

// Increases or decreases the array size of the heap to breadth
// Doubles array size when full (on push).
// Halves array size when 1/4 full (on pop), if length > 8.
static int resize(PriorityQueue *queue, const size_t new_capacity) {
    // Handle incorrect arguments per accident
    if (new_capacity <= queue->size) {
        puts("The 'new_capacity' should be larger than the 'size'");
        return -1;
    }

    // Reallocate the memory given an updated capacity. Handle reallocation failure by preserving the original values
    Node **resized_nodes = realloc(queue->nodes, new_capacity * sizeof(Node *));
    if (resized_nodes == nullptr) {
        puts("Memory reallocation failure for 'resized_nodes' at 'resize()'");
        return -1;
    }

    queue->nodes = resized_nodes;
    queue->capacity = new_capacity;
    return 0;
}