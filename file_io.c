#include "huffman.h"
#include <string.h>

// Helper Methods
static int serialize_huffman_tree(const HuffmanTree *tree, Header *header, FILE *encoding_output);
static int serialize_huffman_tree_recursively(const Node *node, Header *header, FILE *encoding_output);
static int compress(const HuffmanTree *tree, Header *header, FILE *encoding_input, FILE *encoding_output);

// TODO: Program encode() & decode() to take in file NAMES (!) and open them and execute write_h_f & reade_h_f


// Writes in the .huff file based on the input (encoding/compressing) and the Huffman Tree properties and Huffman algorithm
int write_huff_file(const HuffmanTree *tree, FILE *encoding_input, FILE *encoding_output) {
    if (tree == nullptr || encoding_input == nullptr || encoding_output == nullptr) {
        puts("NULL pointer exception at 'write_huff_file(). Some arguments are nullptr");
        return -1;
    }

    // Initializing the File Header
    Header header;
    const char file_extension[] = {'H', 'U', 'F', 'F'};
    if(sizeof(file_extension)/sizeof(file_extension[0]) != FILE_EXTENSION_SIZE) {
        puts("File extension sizes don't match at write_huff_file()");
        return -1;
    }
    memcpy(header.file_extension, file_extension, sizeof(file_extension)/sizeof(file_extension[0]));
    header.file_version = FILE_VERSION;
    header.huffman_tree_size = 0;
    header.last_bit_count = 0;
    header.original_size = 0;


    // Huffman Tree Serialization
    if (fseek(encoding_output, sizeof(Header), SEEK_SET) != 0) {
        puts("Error occurred while skipping the header size amount of bytes in fseek() before serialize_huffman_tree() at write_huff_file()");
        return -1;
    }
    if (serialize_huffman_tree(tree, &header, encoding_output) != 0) {
        puts("Error occurred while serializing the huffman tree in serialize_huffman_tree() called by write_huff_file()");
        return -1;
    }

    // Recording the Compression of Original File Data
    if (compress(tree, &header, encoding_input, encoding_output) != 0) {
        puts("Error occurred while compressing the original file in compress() called by write_huff_file()");
        return -1;
    }

    // Recording the File Header
    if (fseek(encoding_output, 0, SEEK_SET) != 0) {
        puts("Error occurred while returning to the header (the beginning of file) in fseek() after serialize_huffman_tree() and before recording the 'header' at write_huff_file()");
        return -1;
    }
    if (fwrite(&header, sizeof(Header), 1, encoding_output) != 1) {
        puts("Error occurred while writing a header at write_huff_file()");
        return -1;
    }

    return 0;
}

// Serializes the Huffman Tree by using a pre-order traversal recursively
// Writes into a binary file 0 for a root and 1 + character for a leaf
static int serialize_huffman_tree(const HuffmanTree *tree, Header *header, FILE *encoding_output) {
    if (tree == nullptr || tree->root == nullptr || header == nullptr || encoding_output == nullptr) {
        puts("NULL pointer exception at 'serialize_huffman_tree(). Some arguments are nullptr");
        return -1;
    }

    // Recursive Helper: Pre-order Huffman Tree Serialization
    serialize_huffman_tree_recursively(tree->root, header, encoding_output);

    // Successful execution
    return 0;
}

// Recursively serializes a Huffman Tree
// NOLINTNEXTLINE(misc-no-recursion)
static int serialize_huffman_tree_recursively(const Node *node, Header *header, FILE *encoding_output) {
    if (node == nullptr) {
        puts("NULL pointer exception at 'serialize_huffman_tree_recursively(). Node is nullptr");
        return -1;
    }

    // Pre-order traversal: writes into a binary file 0 for a root and 1 + character for a leaf and updates the header
    if (node->left == nullptr && node->right == nullptr) {  // The leaf node was reached
        // Flag the leaf
        const unsigned char flag = 1;
        if (fwrite(&flag, sizeof(unsigned char), 1, encoding_output) != 1 ||
            fwrite(&node->data, sizeof(char), 1, encoding_output) != 1) {
            puts("Error occurred while recording a leaf in serialize_huffman_tree_recursively()");
            return -1;
            }
        header->huffman_tree_size += 2; // Increment the Huffman Tree Size in the header
    } else {                                                // The root node was reached
        // Flag the root
        const unsigned char flag = 0;
        if (fwrite(&flag, sizeof(unsigned char), 1, encoding_output) != 1) {
            puts("Error occurred while recording a root in serialize_huffman_tree_recursively()");
            return -1;
        }
        header->huffman_tree_size += 1; // Increment the Huffman Tree Size in the header

        // Traverse the left and right subtrees
        if (serialize_huffman_tree_recursively(node->left, header, encoding_output) == -1) {
            puts("Error occurred while serializing a left subtree in serialize_huffman_tree_recursively()");
            return -1;
        }
        if (serialize_huffman_tree_recursively(node->right, header, encoding_output) == -1) {
            puts("Error occurred while serializing a right subtree in serialize_huffman_tree_recursively()");
            return -1;
        }
    }

    // Successful execution
    return 0;
}

// Compresses the original file via Huffman Codes and writes it into an encoding output
// Bitwise operators and buffer of a single byte are used to handle the writing data
static int compress(const HuffmanTree *tree, Header *header, FILE *encoding_input, FILE *encoding_output) {
    if (tree == nullptr || tree->codes == nullptr || header == nullptr) {
        puts("NULL pointer exception at 'compress(). Some of the custom structures arguments are nullptr");
        return -1;
    }

    if (encoding_input == nullptr || encoding_output == nullptr) {
        puts("NULL pointer exception at 'compress(). Some of the FILE arguments are nullptr");
        return -1;
    }

    // Symbol and buffer placeholders
    unsigned char symbol;
    unsigned char buffer = 0;
    int buffer_filled = 0;

    // Compress and record each symbol
    while (fread(&symbol, sizeof(unsigned char), 1, encoding_input) == 1) {
        // Find an appropriate Huffman Code for a symbol
        char *code = tree->codes[symbol];

        // Process each bit in the code
        for (size_t i = 0; code[i] != '\0'; ++i) {
            // Shift the buffer one place to the left; buffer is 0000 0000 by default
            // (adds 0 at the end and removes the first bit; 1111 1111 -> 1111 1110)
            buffer <<= 1;

            // By default, all bits in the buffer are set to 0
            // If 1 is encountered in the code, the leftmost bit flips to 1 via bitwise OR; no change otherwise
            if (code[i] == '1') buffer |= 1;
            buffer_filled++;

            // Buffer filled in
            if (buffer_filled == 8) {
                // Record the buffer into a file
                if (fwrite(&buffer, sizeof(unsigned char), 1, encoding_output) != 1) {
                    puts("Error occurred while recording a buffer at 'compress()'");
                    return -1;
                }

                // Empty the buffer
                buffer = 0;
                buffer_filled = 0;
            }
        }

        // Increment the original file size in the header
        header->original_size++;
    }

    // Record the leftover buffer with padding of 0's on the right to save a whole byte of information
    if (buffer_filled > 0) {
        buffer <<= (8 - buffer_filled); // The buffer shifts '8 - buffer_filled' bits to the left, 0's on the right
        if (fwrite(&buffer, sizeof(unsigned char), 1, encoding_output) != 1) {
            puts("Error occurred while recording a leftover buffer at 'compress()'");
            return -1;
        }
        // Record the number of last useful bits of compressed data in the last byte
        header->last_bit_count = buffer_filled;
    }

    // Successful execution
    return 0;
}


int read_huff_file() {
    // Header
    // Read in the file extension (4 bytes)
    // Read in the file version (1 byte)
    // Read in the size of a serialized Huffman Tree (4 bytes)
    // Read in the last bit count (1 or 4 bytes)
    // Read in the original size of the input

    // Serialized Tree (deserialize (rebuild the HF))

    // Compressed Data (decompress())

    return 0;
}


/*
 *  Logs:
    Final .huff example:
    00000000: 01001000 01010101 01000110 01000110 00000001 00001110  HUFF..
    00000006: 00000000 00000000 00000000 00000111 00000000 00000000  ......
    0000000c: 00000000 00000000 00000000 00000000 00000001 01000100  .....D
    00000012: 00000001 01000011 00000000 00000000 00000001 01000001  .C...A
    00000018: 00000001 01000010 00000001 01000101 10010010 11011010  .B.E..
    0000001e: 10101010 00000111 11111110                             ...

    00000000: 4855 4646 010e 0000 0007 0000 0000 0000  HUFF............
    00000010: 0144 0143 0000 0141 0142 0145 92da aa07  .D.C...A.B.E....
    00000020: fe                                       .
 *
 */