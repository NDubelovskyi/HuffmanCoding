#include "huffman.h"
#include "priority_queue.h"
#include <stdio.h>



/*
                                                * DEVELOPMENT NOTES *
    // TODO: Generate a whole Huffman Tree based on picking the Nodes from the Priority Queue
    // Generate the codes for each Node in a Huffman Tree
    // Generate the encoded file and keys separately

    // Make a Decoder
    // Unite the encoded and keys files
    // Let users interact - shift away from hardcoded files
*/




int main(void) {
    // Initialize the frequency array to 0's
    unsigned int frequencies[CHAR_AMOUNT] = {0};


    // Open for encoding
    FILE *encoding_input = fopen("../sources/a.txt", "rb");
    if (encoding_input == NULL) { printf("Cannot open the file to be encoded"); return 1; }

    build_frequency_table(frequencies, encoding_input);
    // for (int i = 0; i < 256; i++)
    //     printf("%d ", frequencies[i]);

    // Close the encoding file
    fclose(encoding_input);

    priority_queue_testing();


    // Successful execution
    return 0;

}