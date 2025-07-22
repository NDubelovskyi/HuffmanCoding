#ifndef huffman_h
#define huffman_h

#include <stdio.h>

// Number of characters in an Extended ASCII Standard
#define CHAR_AMOUNT 256

// Basic Structures
// Node Structure: a single element of a tree
typedef struct Node {
    char data;                          // Leaf stores a character, Parent stores nothing
    size_t frequency;                   // Stores the frequencies of a character or combined characters
    struct Node *left, *right;          // Pointers to the children for parent nodes. For non-character nodes that combine characters or characters and another parent
} Node;

// Huffman Tree Data Structure
typedef struct {
    struct Node *root;                  // A pointer to the root of the Huffman Tree: a Huffman Tree consisting of Nodes
    char *codes[CHAR_AMOUNT];           // Stores the pointers to the Huffman Code strings for each character
} HuffmanTree;


// Function Prototypes
int build_frequency_table(size_t frequencies[CHAR_AMOUNT], FILE *encoding_input);
int build_huffman_tree(HuffmanTree *tree, const size_t frequencies[CHAR_AMOUNT]);
void free_huffman_tree(HuffmanTree *tree);

#endif