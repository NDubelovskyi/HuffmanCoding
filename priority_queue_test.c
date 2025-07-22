#include "priority_queue.h"
#include <assert.h>
#include <stdlib.h>

// Priority Queue Testing
// Prints a PQ for testing purposes
void print_priority_queue(const PriorityQueue *queue) {
    printf("%s", "\n[");
    for (size_t i = 0; i < queue->size; ++i) {
        printf("{Data: %c, Frequency: %zu, Left: %p, Right: %p }",
                queue->nodes[i]->data, queue->nodes[i]->frequency, queue->nodes[i]->left, queue->nodes[i]->right);
        if (i < queue->size - 1) printf("%s", ", \n");
    }
    printf("%s", "]\n");
}

// Public Method Tests
void static constructor_destroyer_test(void) {
    // Constructor Test
    PriorityQueue *queue = create_priority_queue();
    Node node1, node2, node3;
    node1.data = 'a'; node2.data = 'b'; node3.data = 'c';
    node1.frequency = 2; node2.frequency = 10; node3.frequency = 1;
    node1.left = nullptr; node1.right = nullptr; node2.left = nullptr; node2.right = nullptr; node3.left = nullptr; node3.right = nullptr;
    push(queue, &node1); push(queue, &node2); push(queue, &node3);    // [node3, node1, node2]

    // Freeing Memory Test
    printf("\n%p\n", queue);
    free_priority_queue(&queue);
    printf("%p\n", queue); // NULL

    PriorityQueue *emptyQueue = create_priority_queue();
    printf("%p\n", emptyQueue);
    free_priority_queue(&emptyQueue);
    printf("%p\n", emptyQueue); // NULL
}

void static push_test(void) {
    // Creating a Priority Queue Test
    PriorityQueue *queue = create_priority_queue();
    Node node1, node2, node3;
    node1.data = 'a'; node2.data = 'b'; node3.data = 'c';
    node1.frequency = 2; node2.frequency = 10; node3.frequency = 1;
    node1.left = nullptr; node1.right = nullptr; node2.left = nullptr; node2.right = nullptr; node3.left = nullptr; node3.right = nullptr;

    push(queue, &node1);    // [node1]
    push(queue, &node2);    // [node2, node1]
    push(queue, &node3);    // [node3, node1, node2]

    puts("\nTesting invalid queue address here on push -> "); assert(push(nullptr, &node1) == -1);
    puts("Testing pushing an invalid node here -> "); assert(push(queue, nullptr) == -1);

    free_priority_queue(&queue);

    // Comprehensive Push Test
    queue = create_priority_queue();
    Node nodeA = {'a', 15, nullptr, nullptr}; push(queue, &nodeA);     // [nodeA]
    // print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity);
    Node nodeB = {'b', 25, nullptr, nullptr}; push(queue, &nodeB);     // [nodeA, nodeB]
    // print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity);
    Node nodeC = {'c', 1, nullptr, nullptr}; push(queue, &nodeC);      // [nodeC, nodeB, nodeA]
    // print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity);
    Node nodeD = {'d', 100, nullptr, nullptr}; push(queue, &nodeD);    // [nodeC, nodeB, nodeA, nodeD]
    print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity); // Full Capacity Here
    assert(queue->size == 4); assert(queue->capacity == 4);


    Node nodeE = {'e', 101, nullptr, nullptr}; push(queue, &nodeE);    // [nodeC, nodeB, nodeA, nodeD, nodeE]
    print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity); // Resized Here
    assert(queue->size == 5); assert(queue->capacity == 8);
    Node nodeF = {'f', 75, nullptr, nullptr}; push(queue, &nodeF);
    Node nodeG = {'g', 0, nullptr, nullptr}; push(queue, &nodeG);
    Node nodeH = {'h', 310, nullptr, nullptr}; push(queue, &nodeH);
    // print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity); // Full Capacity Here
    assert(queue->size == 8); assert(queue->capacity == 8);
    Node nodeI = {'i', 20, nullptr, nullptr}; push(queue, &nodeI);
    // print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity); // Resized here
    assert(queue->size == 9); assert(queue->capacity == 16);
    Node nodeJ = {'j', 20, nullptr, nullptr}; push(queue, &nodeJ);
    Node nodeK = {'k', 1000, nullptr, nullptr}; push(queue, &nodeK);
    print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity);
    assert(queue->size == 11); assert(queue->capacity == 16);

    free_priority_queue(&queue);
}

void static pop_test(void) {
    // Creating a Priority Queue Test
    PriorityQueue *queue = create_priority_queue();
    Node node1, node2, node3;
    node1.data = 'a'; node2.data = 'b'; node3.data = 'c';
    node1.frequency = 2; node2.frequency = 10; node3.frequency = 1;
    node1.left = nullptr; node1.right = nullptr; node2.left = nullptr; node2.right = nullptr; node3.left = nullptr; node3.right = nullptr;

    push(queue, &node1);    // [node1]
    push(queue, &node2);    // [node2, node1]
    push(queue, &node3);    // [node3, node1, node2]

    Node *popped1 = pop(queue); Node *popped2 = pop(queue); Node *popped3 = pop(queue);
    assert(popped1->frequency == node3.frequency);
    assert(popped2->frequency == node1.frequency);
    assert(popped3->frequency == node2.frequency);

    free_priority_queue(&queue);

    // Comprehensive Pop Test
    queue = create_priority_queue();
    Node nodeA = {'a', 15, nullptr, nullptr}; push(queue, &nodeA);     // [nodeA]
    Node nodeB = {'b', 25, nullptr, nullptr}; push(queue, &nodeB);     // [nodeA, nodeB]
    Node nodeC = {'c', 1, nullptr, nullptr}; push(queue, &nodeC);      // [nodeC, nodeB, nodeA]
    Node nodeD = {'d', 3, nullptr, nullptr}; push(queue, &nodeD);    // [nodeC, nodeB, nodeA, nodeD] - Full Capacity Here
    Node nodeE = {'e', 4, nullptr, nullptr}; push(queue, &nodeE);    // [nodeC, nodeB, nodeA, nodeD, nodeE] - Resized Here
    pop(queue); pop(queue); pop(queue); pop(queue); pop(queue);
    assert(queue->size == 0); assert(queue->capacity == 8);

    // Remains functional
    // For the first resizing to occur, size = 3 & capacity = 16
    push(queue, &nodeA); push(queue, &nodeB); push(queue, &nodeC); push(queue, &nodeD); push(queue, &nodeE);
    push(queue, &nodeA); push(queue, &nodeB); push(queue, &nodeC); push(queue, &nodeD); push(queue, &nodeE);
    print_priority_queue(queue); printf("Size: %zu, Capacity: %zu\n", queue->size, queue->capacity);
    assert(queue->size == 10); assert(queue->capacity == 16);

    pop(queue); pop(queue); pop(queue); pop(queue); pop(queue); pop(queue);
    assert(queue->size == 4); assert(queue->capacity == 16); print_priority_queue(queue);
    pop(queue); // Shrinkage happens here
    assert(queue->size == 3); assert(queue->capacity == 8); print_priority_queue(queue);

    puts("\nTesting invalid queue address here on pop -> "); assert(pop(nullptr) == nullptr);
    pop(queue); pop(queue); pop(queue);
    puts("Testing popping an queue here -> "); assert(pop(queue) == nullptr);

    // Remains functional
    push(queue, &node1); print_priority_queue(queue);
    assert(queue->size == 1); assert(queue->capacity == 8);

    free_priority_queue(&queue);
}

void static peek_test(void) {
    PriorityQueue *queue = create_priority_queue();

    puts("\nTesting invalid queue address here on pop -> "); assert(peek(nullptr) == nullptr);
    puts("Testing peeking an queue here -> "); assert(peek(queue) == nullptr);

    Node nodeA = {'a', 5, nullptr, nullptr}; push(queue, &nodeA); assert(peek(queue)->frequency == nodeA.frequency);
    Node nodeB = {'b', 3, nullptr, nullptr}; push(queue, &nodeB); assert(peek(queue)->frequency == nodeB.frequency);
    Node nodeC = {'c', 3, nullptr, nullptr}; push(queue, &nodeC); assert(peek(queue)->frequency == nodeB.frequency);
    Node nodeD = {'d', 1, nullptr, nullptr}; push(queue, &nodeD); assert(peek(queue)->frequency == nodeD.frequency);
    Node nodeE = {'e', 3, nullptr, nullptr}; push(queue, &nodeE); assert(peek(queue)->frequency == nodeD.frequency);
    Node nodeF = {'f', 5, nullptr, nullptr}; push(queue, &nodeF); assert(peek(queue)->frequency == nodeD.frequency);
    Node nodeG = {'g', 30, nullptr, nullptr}; push(queue, &nodeG); assert(peek(queue)->frequency == nodeD.frequency);

    free_priority_queue(&queue);
}

void static size_and_empty_test(void) {
    PriorityQueue *queue = create_priority_queue();

    puts("\nTesting invalid queue address here on size -> "); assert(size(nullptr) == 0);
    puts("\nTesting invalid queue address here on is_empty -> "); assert(is_empty(nullptr) == true);

    assert(size(queue) == 0); assert(is_empty(queue) == true);
    Node nodeA = {'a', 5, nullptr, nullptr}; push(queue, &nodeA); assert(peek(queue)->frequency == nodeA.frequency);
    assert(size(queue) == 1); assert(is_empty(queue) == false);
    Node nodeB = {'b', 3, nullptr, nullptr}; push(queue, &nodeB);
    Node nodeC = {'c', 3, nullptr, nullptr}; push(queue, &nodeC);
    assert(size(queue) == 3); pop(queue);
    assert(size(queue) == 2); pop(queue);
    assert(size(queue) == 1); pop(queue);
    assert(size(queue) == 0);

    free_priority_queue(&queue);
}

// Tests the heap property specifically
void static heap_property_test(void) {
    // Randomly generate and push 50 nodes
    PriorityQueue *queue = create_priority_queue();
    Node nodes[50];
    for (int i = 0; i < 50; i++) {
        nodes[i].data = (char)(rand() % 26 + 'a');          // Random letter
        nodes[i].frequency = (size_t)(rand() % 100);        // Random number
        nodes[i].left = nullptr;
        nodes[i].right = nullptr;
        assert(push(queue, &nodes[i]) == 0);                // Assert pushing correctly
    }

    print_priority_queue(queue);

    // Check if for every node, its children are less or equal to them
    for (int i = 0; i < queue->size; i++) {
        size_t left = LEFT_CHILD(i), right = RIGHT_CHILD(i);
        if (left < queue->size)     // Going left
            assert(queue->nodes[i]->frequency <= queue->nodes[left]->frequency);
        if (right < queue->size)    // Going right
            assert(queue->nodes[i]->frequency <= queue->nodes[right]->frequency);
    }

    free_priority_queue(&queue);
}

// Tests all the publicly available methods
void priority_queue_testing(void) {
    puts("\n\nEntering the Priority Queue Testing.\n");

    constructor_destroyer_test();
    push_test();
    pop_test();
    peek_test();
    size_and_empty_test();
    heap_property_test();

    puts("\n\nExiting the Priority Queue Testing.\n");
}