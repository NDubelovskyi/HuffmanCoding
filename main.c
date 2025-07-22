#include "huffman.h"
#include <stdio.h>

/*
                                                * DEVELOPMENT NOTES *
    // TODO: Make an Encoder and Decoder
    // Generate the encoded file and keys separately
    // Unite the encoded and keys files
    // Let users interact - shift away from hardcoded files
*/


int main(void) {
    // Declare the Huffman Tree
    HuffmanTree tree;

    // Initialize the frequency array to 0's
    size_t frequencies[CHAR_AMOUNT] = {0};

    // Open for encoding
    FILE *encoding_input = fopen("../sources/a.txt", "rb");
    if (encoding_input == NULL) { printf("Cannot open the file to be encoded"); return 1; }

    // Check return codes for successful execution later on
    build_frequency_table(frequencies, encoding_input);
    build_huffman_tree(&tree, frequencies);
    free_huffman_tree(&tree);

    // Close the encoding file
    fclose(encoding_input);



    // Testing the priority queue methods
    // Include '#include "priority_queue.h"' before testing
    // priority_queue_testing();

    // Successful execution
    return 0;
}