#include "huffman.h"

/*
                                                * DEVELOPMENT NOTES *
    // TODO: Let users interact - shift away from hardcoded files
             Check return codes for successful execution

    // Useful Terminal Commands
    xdd -b "FILE_NAME.huff" examines the binary code of the file
    xdd "FILE_NAME.huff" examines the hexadecimal (converted from binary) code of the file
    diff "FILE_NAME_1.txt" "FILE_NAME_2.txt" checks whether two are identical
*/


int main(void) {
    // Encoding
    // Declare the Huffman Tree
    HuffmanTree tree;

    // Initialize the frequency array to 0's
    size_t frequencies[CHAR_AMOUNT] = {0};

    // Open for encoding
    FILE *encoding_input = fopen("../sources/Hamlet by William Shakespeare (ASCII Version).txt", "rb");
    // FILE *encoding_input = fopen("../sources/a.txt", "rb");
    if (encoding_input == NULL) { printf("Cannot open the file to be encoded"); return -1; }

    build_frequency_table(frequencies, encoding_input);
    build_huffman_tree(&tree, frequencies);

    FILE *encoding_output = fopen("../sources/Hamlet by William Shakespeare (ASCII Version).huff", "wb");
    // FILE *encoding_output = fopen("../sources/a.huff", "wb");
    if (encoding_output == NULL) { printf("Cannot open the file to be encoded"); return -1; }

    write_huff_file(&tree, encoding_input, encoding_output);

    free_huffman_tree(&tree);

    // Close the encoding files
    fclose(encoding_input);
    fclose(encoding_output);




    // Decoding
    HuffmanTree decoding_tree;

    FILE *decoding_input = fopen("../sources/Hamlet by William Shakespeare (ASCII Version).huff", "rb");
    // FILE *decoding_input = fopen("../sources/a.huff", "rb");
    if (decoding_input == NULL) { printf("Cannot open the file to be decoded"); return -1; }

    FILE *decoding_output = fopen("../sources/DECODED Hamlet by William Shakespeare (ASCII Version).txt", "wb");
    // FILE *decoding_output = fopen("../sources/DECODED a.txt", "wb");
    if (decoding_output == NULL) { printf("Cannot open the file to be decoded"); return -1; }

    read_huff_file(&decoding_tree, decoding_input, decoding_output);

    free_huffman_tree(&decoding_tree);

    // Close the decoding files
    fclose(decoding_input);
    fclose(decoding_output);



    // Successful execution
    return 0;
}