#include "huffman.h"
#include <stdlib.h>
#include <string.h>

// Helper Methods
static int serialize_huffman_tree(const HuffmanTree *tree, Header *header, FILE *encoding_output);
static int serialize_huffman_tree_recursively(const Node *node, Header *header, FILE *encoding_output);
static int compress(const HuffmanTree *tree, Header *header, FILE *encoding_input, FILE *encoding_output);

static int deserialize_huffman_tree(HuffmanTree *tree, const Header *header, FILE *decoding_input);
static int deserialize_huffman_tree_recursively(Node *node, const Header *header, size_t *tree_size, FILE *decoding_input);
static int decompress(const HuffmanTree *tree, const Header *header, FILE *decoding_input, FILE *decoding_output);

// TODO: Create encode() & decode() to take in the file NAMES (!) and open them and execute write_h_f & reade_h_f
// Encode should handle building the frequency table and Huffman Tree as well as handle the file size checks
// Both should handle FILE openings, allocating/freeing the memory, and closing the FILES


// Writes in the .huff file based on the input (encoding/compressing) and the Huffman Tree properties and Huffman algorithm
int write_huff_file(const HuffmanTree *tree, FILE *encoding_input, FILE *encoding_output) {
    if (tree == nullptr || encoding_input == nullptr || encoding_output == nullptr) { puts("NULL pointer exception at 'write_huff_file(). Some arguments are nullptr"); return -1; }

    // Initializing the File Header
    Header header;
    memcpy(header.file_extension, "HUFF", FILE_EXTENSION_SIZE); // No '/0' terminator
    header.file_version = FILE_VERSION;
    header.huffman_tree_size = 0;
    header.last_bit_count = 0;
    header.original_size = 0;

    // Huffman Tree Serialization
    if (fseek(encoding_output, sizeof(Header), SEEK_SET) != 0) { puts("Error occurred while skipping the header size amount of bytes in fseek() before serialize_huffman_tree() at write_huff_file()"); return -1; }
    if (serialize_huffman_tree(tree, &header, encoding_output) != 0) { puts("Error occurred while serializing the huffman tree in serialize_huffman_tree() called by write_huff_file()"); return -1; }

    // Recording the Compression of Original File Data
    if (compress(tree, &header, encoding_input, encoding_output) != 0) { puts("Error occurred while compressing the original file in compress() called by write_huff_file()"); return -1; }

    // Recording the File Header
    if (fseek(encoding_output, 0, SEEK_SET) != 0) { puts("Error occurred while returning to the header (the beginning of file) in fseek() after serialize_huffman_tree() and before recording the 'header' at write_huff_file()"); return -1; }
    if (fwrite(&header, sizeof(Header), 1, encoding_output) != 1) { puts("Error occurred while writing a header at write_huff_file()"); return -1; }

    // Successful execution
    return 0;
}

// Serializes the Huffman Tree by using a pre-order traversal recursively
// Writes into a binary file 0 for a root and 1 + character for a leaf
static int serialize_huffman_tree(const HuffmanTree *tree, Header *header, FILE *encoding_output) {
    if (tree == nullptr || tree->root == nullptr || header == nullptr || encoding_output == nullptr) { puts("NULL pointer exception at 'serialize_huffman_tree(). Some arguments are nullptr"); return -1; }

    // Recursive Helper: Pre-order Huffman Tree Serialization
    if (serialize_huffman_tree_recursively(tree->root, header, encoding_output) != 0) { puts("Error occurred during recursive serialization by 'serialize_huffman_tree_recursively()' at 'serialize_huffman_tree()'"); return -1; }

    // Successful execution
    return 0;
}

// Recursively serializes a Huffman Tree
// NOLINTNEXTLINE(misc-no-recursion)
static int serialize_huffman_tree_recursively(const Node *node, Header *header, FILE *encoding_output) {
    if (node == nullptr) { puts("NULL pointer exception at 'serialize_huffman_tree_recursively(). Node is nullptr"); return -1; }

    // Pre-order traversal: writes into a binary file 0 for a root and 1 + character for a leaf and updates the header
    if (node->left == nullptr && node->right == nullptr) {  // The leaf node was reached
        // Flag the leaf
        const unsigned char flag = 1;
        if (fwrite(&flag, sizeof(unsigned char), 1, encoding_output) != 1 || fwrite(&node->data, sizeof(char), 1, encoding_output) != 1)
            { puts("Error occurred while recording a leaf in serialize_huffman_tree_recursively()"); return -1; }

        header->huffman_tree_size += 2; // Increment the Huffman Tree Size in the header
    } else {                                                // The root node was reached
        // Flag the root
        const unsigned char flag = 0;
        if (fwrite(&flag, sizeof(unsigned char), 1, encoding_output) != 1) { puts("Error occurred while recording a root in serialize_huffman_tree_recursively()"); return -1; }
        header->huffman_tree_size += 1; // Increment the Huffman Tree Size in the header

        // Traverse the left and right subtrees
        if (serialize_huffman_tree_recursively(node->left, header, encoding_output) == -1) { puts("Error occurred while serializing a left subtree in serialize_huffman_tree_recursively()"); return -1; }
        if (serialize_huffman_tree_recursively(node->right, header, encoding_output) == -1) { puts("Error occurred while serializing a right subtree in serialize_huffman_tree_recursively()"); return -1; }
    }

    // Successful execution
    return 0;
}

// Compresses the original file via Huffman Codes and writes it into an encoding output
// Bitwise operators and buffer of a single byte are used to handle the writing data
static int compress(const HuffmanTree *tree, Header *header, FILE *encoding_input, FILE *encoding_output) {
    if (tree == nullptr || header == nullptr) { puts("NULL pointer exception at 'compress(). Some of the custom structures arguments are nullptr"); return -1; }
    if (encoding_input == nullptr || encoding_output == nullptr) { puts("NULL pointer exception at 'compress(). Some of the FILE arguments are nullptr"); return -1; }

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
                if (fwrite(&buffer, sizeof(unsigned char), 1, encoding_output) != 1) { puts("Error occurred while recording a buffer at 'compress()'"); return -1; }

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
        // Shift the buffer
        buffer <<= (8 - buffer_filled); // The buffer shifts '8 - buffer_filled' bits to the left, 0's on the right

        // Record the buffer
        if (fwrite(&buffer, sizeof(unsigned char), 1, encoding_output) != 1) { puts("Error occurred while recording a leftover buffer at 'compress()'"); return -1; }

        // Record the number of last useful bits of compressed data in the last byte
        header->last_bit_count = buffer_filled;
    }

    // Successful execution
    return 0;
}


// Reads the .huff file and decompresses it into a decoding output file based on the serialized Huffman Tree and the encoded line of bits
int read_huff_file(HuffmanTree *tree, FILE *decoding_input, FILE *decoding_output) {
    // Declare the header
    Header header;

    // Record the header from the encoded file and handle basic file corruption
    if (fread(&header, sizeof(Header), 1, decoding_input) != 1) { puts("The encoded file might be corrupted: Couldn't read the 'header' at read_huff_file()"); return -1; }

    // Check the File Extension and other header fields for correctness and corruption
    if (memcmp(header.file_extension, "HUFF", FILE_EXTENSION_SIZE) != 0) { puts("The encoded file might be corrupted: File extensions do not match at read_huff_file()"); return -1; }
    if (header.file_version != FILE_VERSION) { puts("File Version do not match with the current one (FILE_EXTENSION_SIZE) at read_huff_file()"); return -1; }
    if (header.huffman_tree_size == 0 || header.huffman_tree_size > MAX_TREE_SIZE) { puts("The encoded file might be corrupted: 'huffman_tree_size' is 0 or exceeds 'MAX_TREE_SIZE' at read_huff_file()"); return -1; }
    if (header.last_bit_count > 7) { puts("The encoded file might be corrupted: 'last_bit_count' exceeds 7 possible bits at read_huff_file()"); return -1; }
    if (header.original_size == 0) { puts("The encoded file might be corrupted: 'original_size' is 0 at read_huff_file()"); return -1; }

    // Huffman Tree Deserialization
    if (deserialize_huffman_tree(tree, &header, decoding_input) != 0) { puts("Error occurred while deserializing the Huffman Tree at read_huff_file()"); return -1; }

    // Recording the Decompressed Data into an Output File
    if (decompress(tree, &header, decoding_input, decoding_output) != 0) { puts("Error occurred while decompressing the encoded line of bits at read_huff_file()"); return -1; }

    // Successful execution
    return 0;
}

// Kicks off the recursion to rebuild the Huffman Tree from the serialized data
static int deserialize_huffman_tree(HuffmanTree *tree, const Header *header, FILE *decoding_input) {
    if (tree == nullptr || header == nullptr || decoding_input == nullptr) { puts("NULL pointer exception at 'deserialize_huffman_tree(). Some arguments are nullptr"); return -1; }

    // Tracks the tree size to ensure file validity
    size_t tree_size = 0;

    // Declare and initialize the root of the tree
    tree->root = malloc(sizeof(Node));
    if (tree->root == NULL) { puts("Memory allocation failed for the 'root' at deserialize_huffman_tree()"); return -1; }
    tree->root->data = ' '; tree->root->frequency = 0; tree->root->left = nullptr; tree->root->right = nullptr;

    // Recursive Helper: Pre-order Huffman Tree Deserialization
    if (deserialize_huffman_tree_recursively(tree->root, header, &tree_size, decoding_input) != 0) { free_huffman_tree(tree); puts("Error occurred during recursive deserialization by deserialize_huffman_tree_recursively() at deserialize_huffman_tree()"); return -1; }

    // Check if the deserialization was successful
    if (tree_size != header->huffman_tree_size) { free_huffman_tree(tree); puts("Error occurred during deserialization at deserialize_huffman_tree(). 'tree_size' and 'header->huffman_tree_size' do not match."); return -1; }

    // Successful execution
    return 0;
}

// Recursively deserializes the Huffman Tree by allocating memory for nodes, correctly adding information and linking them
// NOLINTNEXTLINE(misc-no-recursion)
static int deserialize_huffman_tree_recursively(Node *node, const Header *header, size_t *tree_size, FILE *decoding_input) {
    if (*tree_size > header->huffman_tree_size) { puts("The file might be corrupted: Current tree_size exceeds the predefined size"); return -1; }

    // Read the first/next byte
    unsigned char flag = 0;
    if (fread(&flag, sizeof(unsigned char), 1, decoding_input) != 1) { puts("Error occurred while reading a 'flag' at deserialize_huffman_tree_recursively()"); return -1; }

    if (flag == 0) {
        // Root reached
        *tree_size += 1;    // A root has only one byte in a tree

        // Recursively deserialize the left subtree
        // Allocate, declare, initialize, and pass a (left subtree) node for further deserialization
        node->left = malloc(sizeof(Node));
        node->left->data = ' '; node->left->frequency = 0; node->left->left = nullptr; node->left->right = nullptr;
        if (node->left == NULL) { puts("Memory allocation failed for the left node at deserialize_huffman_tree_recursively()"); return -1; }
        if (deserialize_huffman_tree_recursively(node->left, header, tree_size, decoding_input) != 0) { puts("Failed to deserialize the left subtree at deserialize_huffman_tree_recursively()"); return -1; }

        // Recursively deserialize the left subtree
        // Allocate, declare, initialize, and pass a (right subtree) node for further deserialization
        node->right = malloc(sizeof(Node));
        node->right->data = ' '; node->right->frequency = 0; node->right->left = nullptr; node->right->right = nullptr;
        if (node->right == NULL) { puts("Memory allocation failed for the right node at deserialize_huffman_tree_recursively()"); return -1; }
        if (deserialize_huffman_tree_recursively(node->right, header, tree_size, decoding_input) != 0) { puts("Failed to deserialize the right subtree at deserialize_huffman_tree_recursively()"); return -1; }
    } else if (flag == 1) {
        // Leaf reached
        *tree_size += 2;    // A leaf corresponds to two bytes: leaf itself and its data character

        // Record the data into the tree data structure
        if (fread(&node->data, sizeof(unsigned char), 1, decoding_input) != 1) { puts("Error occurred while reading a leaf's data at deserialize_huffman_tree_recursively()"); return -1; }
    } else { puts("The file might be corrupted: Error occurred while deserializing the tree recursively at deserialize_huffman_tree_recursively()"); return -1; }

    // Successful execution
    return 0;
}

// Decompresses the encoded line of bits into an output based on the Huffman Tree
static int decompress(const HuffmanTree *tree, const Header *header, FILE *decoding_input, FILE *decoding_output) {
    if (tree == nullptr || tree->root == nullptr || decoding_input == nullptr || decoding_output == nullptr) {puts("NULL pointer exception at 'decompress(). Some arguments are nullptr"); return -1; }

    // Navigate to a root node
    Node *current_node = tree->root;

    // Byte buffer and control variables
    unsigned char buffer = 0;
    size_t bytes_read = 0;
    size_t chars_written = 0;

    // Read each byte of the compressed data into a buffer
    while (fread(&buffer, sizeof(unsigned char), 1, decoding_input)) {
        // Increment control variable
        bytes_read++;

        // Controls the inner for loop for a current byte
        size_t bits_to_read = 8;

        // If the end of the file was reached, only use the last remaining bits in the buffer
        if (feof(decoding_output) && header->last_bit_count > 0)
            bits_to_read = header->last_bit_count;

        // Process each remaining bit in the buffer
        for (size_t i = 0; i < bits_to_read; ++i) {
            // Get the next bit from a current byte buffer
            // Shifts 7-i characters to the right and performs a bitwise AND operation based on the rightmost bit
            // Such AND operation returns whether the bit was 1 or 0: 1 & 1 -> 1; 0 & 1 -> 0
            const int bit = (buffer >> (7 - i)) & 1;

            // If bit is 0, go into the left subtree; if bit is 1, go into the right subtree
            if (bit == 0) current_node = current_node->left;
            else if (bit == 1) current_node = current_node->right;

            // The file might be corrupted if the bit path doesn't lead to an existing node
            if (current_node == nullptr) { puts("The file might be corrupted: Error occurred while decompressing the encoded_input during accessing a NULL node at decompress()"); return -1; }

            // The leaf node was reached
            if (current_node->left == nullptr && current_node->right == nullptr) {
                // Record the leaf data into the decompressed file
                if (fwrite(&current_node->data, sizeof(unsigned char), 1, decoding_output) != 1) { puts("Error occurred while recording a decoded character into 'decoding_output' at decompress()"); return -1; }
                chars_written++; // Increment the control variable

                // Navigate back to the root of the tree
                current_node = tree->root;

                // Check if a supposed end of compressed data was reached based on an original size
                if (chars_written >= header->original_size) return 0;
            }
        }
    }

    // The original size doesn't match the written number of characters
    if (chars_written != header->original_size) { puts("The file might be corrupted: The 'original_size' doesn't match the written number of characters, 'chars_written', at decompress()"); return -1; }

    // Successful execution
    return 0;
}