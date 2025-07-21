#ifndef huffman_h
#define huffman_h

#include <stdio.h>

// Number of characters in an Extended ASCII Standard
#define CHAR_AMOUNT 256

// Basic Structures
// Node Structure: a single element of a tree
typedef struct {
    char data;
    size_t frequency;
    struct Node *left, *right;
} Node;

// Tree Structure: a Huffman Tree consisting of Nodes
typedef struct {
    struct Node *root;
    char *codes[CHAR_AMOUNT];
} Tree;



// Function Prototypes
void build_frequency_table(unsigned int frequencies[CHAR_AMOUNT], FILE *encoding_input);

#endif