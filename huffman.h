#ifndef huffman_h
#define huffman_h

#include <stdint.h>
#include <stdio.h>

// Number of characters in an ASCII
#define CHAR_AMOUNT 128

// IO Extensions for FILE Name Checks and Generation
#define ORIGINAL_EXTENSION ".txt"
#define COMPRESSED_EXTENSION ".huff"
#define DECOMPRESSED_EXTENSION " (decompressed).txt"

// File Header Definitions
#define FILE_EXTENSION "HUFF"
#define FILE_EXTENSION_SIZE 4
#define FILE_VERSION 1
#define MAX_TREE_SIZE ((CHAR_AMOUNT * 3) - 1)


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
    char file_extension[FILE_EXTENSION_SIZE]; // Holds the extension of the encoding file
    uint8_t file_version;               // The version of the file
    uint32_t huffman_tree_size;         // The size of a serialized Huffman Tree (3*n - 1)
    uint8_t last_bit_count;             // Holds the number of the last written bits in the last byte of the compressed data
    uint32_t original_size;             // The size of an original file (number of characters)
} Header;


// Huffman Tree Methods
int build_frequency_table(size_t frequencies[CHAR_AMOUNT], FILE *encoding_input);
int build_huffman_tree(HuffmanTree *tree, const size_t frequencies[CHAR_AMOUNT]);
void free_huffman_tree(HuffmanTree *tree);

// File Manipulations
int encode(const char *file_path);
int write_huff_file(const HuffmanTree *tree, FILE *encoding_input, FILE *encoding_output);
int decode(const char *file_path);
int read_huff_file(HuffmanTree *tree, FILE *decoding_input, FILE *decoding_output);


#endif