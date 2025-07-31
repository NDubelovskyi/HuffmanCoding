#ifndef huffman_h
#define huffman_h

#include <stdint.h>
#include <stdio.h>

// Number of characters in an ASCII
#define CHAR_AMOUNT 128

// File Header Definitions
#define FILE_EXTENSION_SIZE 4
#define FILE_VERSION 1


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

// File Header Structure
// __attribute__((packed)) eliminates internal struct paddings,
// meaning no paddings when writing a struct into a binary file
typedef struct __attribute__((packed)) {
    char file_extension[FILE_EXTENSION_SIZE];
    uint8_t file_version;
    uint32_t huffman_tree_size;
    uint8_t last_bit_count;
    uint32_t original_size;
} Header;


// Huffman Tree Methods
int build_frequency_table(size_t frequencies[CHAR_AMOUNT], FILE *encoding_input);
int build_huffman_tree(HuffmanTree *tree, const size_t frequencies[CHAR_AMOUNT]);
void free_huffman_tree(HuffmanTree *tree);

// File Manipulations
int write_huff_file(const HuffmanTree *tree, FILE *encoding_input, FILE *encoding_output);
int read_huff_file();


#endif