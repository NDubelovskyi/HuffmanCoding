#include "huffman.h"

// Build a frequency table from the encoding file
void build_frequency_table(unsigned int frequencies[CHAR_AMOUNT], FILE *encoding_input) {
    // Stores one character at a time
    int character;

    // Array stores the frequency of each character from the encoding file
    // Each character is associated numerically (by an ASCII standard) with its place in the array
    // char -> int, where int is the position in the array
    while ((character = fgetc(encoding_input)) != EOF)
        frequencies[character]++;

    // Reset the pointer to the beginning of the file
    rewind(encoding_input);
}