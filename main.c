#include "huffman.h"
#include <stdio.h>

/*
                                                * DEVELOPMENT NOTES *
    // TODO: Make an Encoder and Decoder
    // Let users interact - shift away from hardcoded files
*/


int main(void) {
    // Declare the Huffman Tree
    HuffmanTree tree;

    // Initialize the frequency array to 0's
    size_t frequencies[CHAR_AMOUNT] = {0};

    // Open for encoding
    // FILE *encoding_input = fopen("../sources/Hamlet by William Shakespeare (ASCII Version).txt", "rb");
    FILE *encoding_input = fopen("../sources/a.txt", "rb");
    if (encoding_input == NULL) { printf("Cannot open the file to be encoded"); return -1; }

    // TODO: Check return codes for successful execution later on
    build_frequency_table(frequencies, encoding_input);
    build_huffman_tree(&tree, frequencies);

    // FILE *encoding_output = fopen("../sources/Hamlet by William Shakespeare (ASCII Version).huff", "wb");
    FILE *encoding_output = fopen("../sources/a.huff", "wb");
    if (encoding_output == NULL) { printf("Cannot open the file to be encoded"); return -1; }

    // TODO: Check return codes for successful execution later on
    write_huff_file(&tree, encoding_input, encoding_output);


    free_huffman_tree(&tree);

    // Close the encoding files
    fclose(encoding_input);
    fclose(encoding_output);

    // Successful execution
    return 0;
}