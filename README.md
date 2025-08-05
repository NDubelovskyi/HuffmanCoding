# HuffmanCoding Project

This project implements Huffman coding for lossless compression and decompression of ASCII text files. It provides command-line tools to compress `.txt` files into `.huff` files and decompress `.huff` files back into text files. The `.huff` file is typically smaller than the original for sufficiently large inputs, due to the efficiency of Huffman coding in reducing redundancy.

## Getting Started

To get started, clone the repository:

```bash
git clone https://github.com/NDubelovskyi/HuffmanCoding.git
```


Then, build the program as described in the "Building the Program" section.

## Building the Program

To build the project, use a C compiler like GCC. Run the following command:

```bash
gcc -o HuffmanCoding main.c file_io.c huffman.c priority_queue.c
```

This will generate an executable named `HuffmanCoding`.

**Note for Older Compilers**: This project is written in the C23 standard. If using a compiler that does not support C23 (e.g., lacking `nullptr` or `bool` without explicit inclusion), add the following lines to `huffman.h` right after `#include <stdio.h>`, which ensure compatibility with older C standards:
```c
#include <stdbool.h>
#define nullptr NULL
```

## Usage

- **Compress a file:**

  ```bash
  ./HuffmanCoding -c <file_path.txt>
  ```

- **Decompress a file:**

  ```bash
  ./HuffmanCoding -d <file_path.huff>
  ```

The decompressed file will be saved with the extension ` (decompressed).txt`. For example, decompressing `example.huff` will produce `example (decompressed).txt`.

**Note:** The program checks file extensions and will print an error if they do not match the expected extensions (`.txt` for compression, `.huff` for decompression).

## File Extensions

- **Original files:** `.txt` (ASCII text files)
- **Compressed files:** `.huff`
- **Decompressed files:** `<original_name> (decompressed).txt`

## Supported Files and Limitations

- The program is designed for ASCII text files (`.txt`).
- Files must be encoded in ASCII; non-ASCII characters will cause an error.
- The original file size must be between 1B and 4GiB.

## Project Structure

- `main.c`: Entry point, handles command-line arguments.
- `file_io.c`: File input/output operations, encoding, and decoding.
- `huffman.c`: Huffman tree construction and code generation.
- `priority_queue.c`: Priority queue implementation for building the Huffman tree.
- `huffman.h`: Header file with structure definitions and function declarations.
- `priority_queue.h`: Header file for the priority queue.
- `priority_queue_test.c`: Test file for the priority queue (not part of the main program, ensures correctness).

## Compression Process

1. Read the input `.txt` file and build a frequency table of ASCII characters.
2. Construct a Huffman tree based on the frequency table using a priority queue.
3. Generate Huffman codes for each character.
4. Serialize the Huffman tree and write it to the `.huff` file.
5. Compress the input data using the Huffman codes and write the compressed data to the `.huff` file.

## Decompression Process

1. Read the header from the `.huff` file.
2. Deserialize the Huffman tree from the `.huff` file.
3. Read the compressed data and decode it using the Huffman tree, writing the decompressed data to a new text file.

## Compressed File Format

The `.huff` file consists of:

- **Header:** Contains metadata such as file extension (`HUFF`), version (`1`), Huffman tree size, last bit count, and original file size.
- **Serialized Huffman Tree:** Stored using a pre-order traversal, where internal nodes are represented by `0` and leaf nodes by `1` followed by the character.
- **Compressed Data:** The encoded binary data of the original file.

`[MAGIC WORD: 4 bytes][VERSION: 1 byte][HUFFMAN TREE SIZE: 4 bytes][LAST BIT COUNT: 1 byte][ORIGINAL FILE SIZE: 4 bytes][SERIALIZED HUFFMAN TREE: HUFFMAN TREE SIZE bytes][COMPRESSED DATA: variable # bytes]`


## Basic Example

To illustrate how the Huffman Tree is constructed, consider the simplified input text "AABBBCCCCDDDEEEEE".

**Frequency Table**:
- A: 2
- B: 3
- C: 4
- D: 3
- E: 5

Based on the debugging output, the Huffman Tree is built as follows:

1. Combine A (2) and B (3) → Node (5)
2. Combine C (4) and D (3) → Node (7)
3. Combine E (5) and Node (5) → Node (10)
4. Combine Node (10) and Node (7) → Root (17)

**Huffman Tree**:
```
Root (17)
├── 0: Node (10)
│   ├── 0: E (code: 11)
│   └── 1: Node (5)
│       ├── 0: A (code: 100)
│       └── 1: B (code: 101)
└── 1: Node (7)
    ├── 0: C (code: 01)
    └── 1: D (code: 00)
```

**Huffman Codes**:
- A: 100
- B: 101
- C: 01
- D: 00
- E: 11

This tree matches the provided codes and demonstrates the Huffman coding process clearly. The structure reflects one possible valid Huffman Tree, where the assignment of 0 and 1 may vary depending on implementation choices, but the prefix-free property and code lengths remain optimal. The compressed binary file view (.huff) in HEX is given below.

```
xxd a.huff
00000000: 4855 4646 010e 0000 0007 1100 0000 0000  HUFF............
00000010: 0144 0143 0000 0141 0142 0145 92da aa07  .D.C...A.B.E....
00000020: fe                                       .
```

## Compressing Hamlet

To demonstrate the program’s effectiveness, let's compress the full text of "Hamlet" by William Shakespeare, sourced from [Project Gutenberg](https://www.gutenberg.org/) and converted to ASCII from UTF-8.

**Commands**:
```bash
./HuffmanCoding -c "Hamlet by William Shakespeare.txt"
./HuffmanCoding -d "Hamlet by William Shakespeare.huff"
```

**Results**:
- Original file size: 197 KB
- Compressed file size: 119 KB
- Compression ratio: 60.4%

**Sample Huffman Codes** (top 5 characters by frequency):
- `e` (16,258 occurrences): `1110`
- `t` (12,400 occurrences): `1000`
- `o` (11,827 occurrences): `0111`
- `a` (9,755 occurrences): `0100`
- `n` (8,905 occurrences): `0010`


**Verification**:
Run `diff "Hamlet by William Shakespeare.txt" "Hamlet by William Shakespeare (decompressed).txt"` to confirm lossless compression. The decompressed file is identical to the original.

The full frequency table, Huffman tree, and codes are available in `sources/hamlet_debugging.txt` in the repository.

## Development Notes

- Examine the binary code of a `.huff` file:
  ```bash
  xdd -b <file.huff>
  ```
- Examine the hexadecimal code:
  ```bash
  xdd <file.huff>
  ```
- Check if two files are identical:
  ```bash
  diff <file1.txt> <file2.txt>
  ```

## License

This program is released under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author

Program created by Nazarii Dubelovskyi. Visit my [GitHub](https://github.com/NDubelovskyi) or contact me at [LinkedIn](https://www.linkedin.com/in/NDubelovskyi/) for more information.

**Credit**: The "Hamlet" text used for testing is sourced from [Project Gutenberg](https://www.gutenberg.org/), a repository of public-domain books, converted to ASCII for compatibility with this program.