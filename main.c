#include "huffman.h"

/*
                                                * DEVELOPMENT NOTES *
    // TODO: CLI - use getopt() for the interface
             Check the return codes for successful execution

    // Useful Terminal Commands
    xdd -b "FILE_NAME.huff" examines the binary code of the file
    xdd "FILE_NAME.huff" examines the hexadecimal (converted from binary) code of the file
    diff "FILE_NAME_1.txt" "FILE_NAME_2.txt" checks whether two are identical
*/


int main(void) {
    encode("../sources/a.txt");
    decode("../sources/a.huff");

    encode("../sources/b.txt");
    decode("../sources/b.huff");

    encode("../sources/Hamlet by William Shakespeare.txt");
    decode("../sources/Hamlet by William Shakespeare.huff");

    // Successful execution
    return 0;
}