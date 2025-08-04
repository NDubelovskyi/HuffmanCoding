#include "huffman.h"
#include <getopt.h>

/*
                                                * DEVELOPMENT NOTES *
    * Useful Terminal Commands:
        xdd -b "FILE_NAME.huff" examines the binary code of the file
        xdd "FILE_NAME.huff" examines the hexadecimal (converted from binary) code of the file
        diff "FILE_NAME_1.txt" "FILE_NAME_2.txt" checks whether the two are identical

    * Example calls:
        ./HuffmanCoding -c "../sources/a.txt"
        ./HuffmanCoding -d "../sources/a.huff"

        ./HuffmanCoding -c "../sources/b.txt"
        ./HuffmanCoding -d "../sources/b.huff"

        ./HuffmanCoding -c "../sources/Hamlet by William Shakespeare.txt"
        ./HuffmanCoding -d "../sources/Hamlet by William Shakespeare.huff"
*/


// Prints the usage of the program
static void print_usage(char *program_name) {
    printf("%s%s%s", "Usage: ", program_name, " -c <file_path> | -d <file_path>\n");
    puts("\t-c <file_path>: Compress the file");
    puts("\t-d <file_path>: Decompress the file");
}


// The main program
int main(int argc, char **argv) {
    // Arguments -c for compression, -d for decompression, each followed by a file path name
    int option;
    int compress = 0;
    int decompress = 0;
    char *file_path = nullptr;

    // Checks possible arguments in the stream after the program was called
    while ((option = getopt(argc, argv, "c:d:")) != -1) {
        // Saves the chosen option
        switch (option) {
            case 'c':
                compress = 1;
                file_path = optarg;
                break;
            case 'd':
                decompress = 1;
                file_path = optarg;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Checks for extra arguments
    if (optind < argc) {
        puts("Error: Unexpected arguments provided.");
        print_usage(argv[0]);
        return 1;
    }

    // Checks if exactly one argument was selected
    if ((compress + decompress) != 1) {
        puts("Error: Exactly one argument, -c or -d, followed by a file path is allowed.");
        print_usage(argv[0]);
        return 1;
    }

    // Checks whether the file path was specified
    if (file_path == nullptr) {
        puts("Error: Must specify the file path after an argument -c or -d.");
        print_usage(argv[0]);
        return 1;
    }

    // Compresses or Decompresses the files accordingly
    if (compress) {
        if (encode(file_path) != 0) {
            puts("Error: Compression failed at main() after encode().");
            return 1;
        }
        printf("%s%s%s", "Successfully compressed '", file_path, "'.");
    } else if (decompress) {
        if (decode(file_path) != 0) {
            puts("Error: Decompression failed at main() after decode().");
            return 1;
        }
        printf("%s%s%s", "Successfully decompressed '", file_path, "'.");
    }

    // Successful execution
    return 0;
}