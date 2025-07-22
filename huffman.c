#include "huffman.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <string.h>

// Helper methods
// Building the Tree
static int create_leaves(PriorityQueue *queue, const size_t frequencies[CHAR_AMOUNT]);
static int create_root_and_parents(PriorityQueue *queue);
// Building the Codes
static int assign_codes(HuffmanTree *tree);
static int generate_codes_recursively(const Node *node, char *code, size_t level, char *codes[CHAR_AMOUNT]);
// Freeing the DS
static void free_tree_nodes(Node **root);
static void free_tree_codes(char *codes[CHAR_AMOUNT]);
// Printing the Tree
static void print_huffman_tree(const HuffmanTree *tree);
static void print_huffman_tree_recursively(const Node *root, size_t level);


// Builds a frequency table from the encoding file
int build_frequency_table(size_t frequencies[CHAR_AMOUNT], FILE *encoding_input) {
    if (encoding_input == NULL) {
        puts("NULL pointer exception at 'build_frequency_table(). 'encoding_input' argument is nullptr");
        return -1;
    }

    // Stores one character at a time
    int character;

    // Array stores the frequency of each character from the encoding file
    // Each character is associated numerically (by an ASCII standard) with its place in the array
    // char -> int, where int is the position in the array
    while ((character = fgetc(encoding_input)) != EOF)
        frequencies[character]++;

    // Reset the pointer to the beginning of the file
    rewind(encoding_input);

    // Successful execution
    return 0;
}

// Builds an entire Huffman Tree based on frequencies and generates codes
// Nodes need to be freed separately as a whole tree
int build_huffman_tree(HuffmanTree *tree, const size_t frequencies[CHAR_AMOUNT]) {
    if (tree == nullptr) {
        puts("NULL pointer exception at 'build_huffman_tree(). 'tree' argument is nullptr");
        return -1;
    }

    // Initialize a tree and codes to null pointers
    tree->root = nullptr;
    for (size_t i = 0; i < CHAR_AMOUNT; ++i)
        tree->codes[i] = nullptr;

    PriorityQueue *queue = create_priority_queue();

    if (create_leaves(queue, frequencies) != 0) {
        puts("Memory allocation failed at 'build_huffman_tree()' after calling 'create_leaves()'.");
        return -1;
    }

    // For debugging purposes: prints all the leaves
    print_priority_queue(queue); puts("");

    if (create_root_and_parents(queue) != 0) {
        puts("Memory allocation failed at 'build_huffman_tree()' after calling 'create_root_and_parents()'.");
        return -1;
    }

    // Assign the only left root from a queue to a tree
    tree->root = pop(queue);
    free_priority_queue(&queue);

    // For debugging purposes: prints entire generated tree
    print_huffman_tree(tree); puts("");

    // Assign codes to all characters
    if (assign_codes(tree) != 0) {
        puts("Memory allocation failed while calling 'assign_codes()' at 'build_huffman_tree()'.");
        return -1;
    }

    // For debugging purposes: prints the codes
    for (size_t i = 0; i < CHAR_AMOUNT; i++) {
        if (tree->codes[i] != nullptr)
            printf("%c : %s\n", (char)i, tree->codes[i]);
    }

    // Successful execution
    return 0;
}

// Free the entire Huffman Tree
void free_huffman_tree(HuffmanTree *tree) {
    if (tree == nullptr) return;
    free_tree_nodes(&(tree->root));     // Free each node
    free_tree_codes(tree->codes);       // Free each string
}


// Helper methods. Arguments assumed to be valid.
// Building the Tree
// Creates leaf nodes for the Huffman Tree and pushes them into the priority queue
// The leaf nodes consist of the individual character and its frequency as well as nullptr's to children
static int create_leaves(PriorityQueue *queue, const size_t frequencies[CHAR_AMOUNT]) {
    for (size_t i = 0; i < CHAR_AMOUNT; i++) {
        if (frequencies[i] != 0) {
            // Allocate memory for each leaf-node. Handle memory allocation failure
            Node *node = malloc(sizeof(Node));
            if (node == NULL) {
                puts("Memory allocation failed for one of the 'node's at 'create_leaves()'");

                // Free all generated nodes so far and the priority queue
                for (size_t k = 0; k < size(queue); ++k) {
                    free(queue->nodes[k]);
                    queue->nodes[k] = nullptr;
                }
                free_priority_queue(&queue);

                return -1;
            }

            // Initialize each leaf-node
            node->data = (char)(i);
            node->frequency = frequencies[i];
            node->left = nullptr;
            node->right = nullptr;
            push(queue, node);
        }
    }

    // The leaf nodes must be freed separately as a whole Huffman Tree at free_huffman_tree()
    // Successful execution
    return 0;
}

// Creates parent nodes for all children, leaving only a single root at the PQ in the end
// Frequencies are combined; parents have no data; memory should be freed separately
static int create_root_and_parents(PriorityQueue *queue) {
    // Build the Huffman Tree, leaving a root as the only element in the PQ
    while (queue->size > 1) {
        // Create a parent node
        Node *node = malloc(sizeof(Node));

        // Memory allocation failure
        if (node == nullptr) {
            puts("Memory allocation failed at 'create_root_and_parents()' while creating a parent node.");

            // Free all generated nodes so far and the priority queue
            for (size_t i = 0; i < size(queue); ++i) {
                free_tree_nodes(&queue->nodes[i]);
                queue->nodes[i] = nullptr;
            }
            free_priority_queue(&queue);

            return -1;
        }

        // Get children nodes with the highest priority
        Node *left_node = pop(queue);
        Node *right_node = pop(queue);

        // Combine the children nodes under a parent node
        node->data = ' ';                                                       // No parent node has data
        node->frequency = left_node->frequency + right_node->frequency;         // Parent nodes combine children frequencies
        // Assign children nodes
        node->left = left_node;
        node->right = right_node;
        push(queue, node);                                                      // Add the combined node back into the PQ
    }

    // The parent nodes must be freed separately as a whole Huffman Tree at free_huffman_tree()
    // Successful execution
    return 0;
}

// Building the Codes
// Assigns a code for each leaf node in a tree
static int assign_codes(HuffmanTree *tree) {
    char code[CHAR_AMOUNT] = {'\0'};
    if (generate_codes_recursively(tree->root, code, 0, tree->codes) !=  0) {
        // Memory allocation failure while generating each code
        puts("Failed to assign codes at 'assign_codes()' for each node due to a memory allocation failure at 'generate_codes_recursively()'.");

        // Clear up / free all allocated memory for the entire tree (all generated nodes and all generated codes so far)
        free_huffman_tree(tree);
        return -1;
    }

    // Successful execution
    return 0;
}

// Generates the codes recursively for each leaf by adding a corresponding
// 0 or 1 per tree level and storing the code in the Huffman Tree DS
// NOLINTNEXTLINE(misc-no-recursion)
static int generate_codes_recursively(const Node *node, char *code, size_t level, char *codes[CHAR_AMOUNT]) {
    // The leaf node was reached
    if (node->left == nullptr && node->right == nullptr) {
        // Allocate memory for a string code
        char *assigning_code = calloc(level + 1, sizeof(char));

        // Memory allocation failure
        if (assigning_code == nullptr) {
            puts("Memory allocation failure for an 'assigning_code' string for a leaf node at 'generate_codes_recursively()'.");
            // Freeing the generated Huffman Tree (nodes), and all generated codes so far happens at 'assign_codes()' if an accident occurs
            return -1;
        }

        // Copy the generated code recursively into an allocated string and store the string in the Huffman Tree DS's codes
        strncpy(assigning_code, code, level);   // the last char in a string must be '\0' per calloc
        codes[(unsigned char) node->data] = assigning_code;

        // Successful leaf code generation
        return 0;
    }

    // Reach the left subtree
    if (node->left != nullptr) {
        code[level] = '0';  // Add a corresponding code value
        if (generate_codes_recursively(node->left, code, level + 1, codes) != 0) {
            puts("Memory allocation failure in a left subtree while generating an 'assigning_code' string for a leaf node at 'generate_codes_recursively().'");
            return -1;
        }
    }

    // Reach the right subtree
    if (node->right != nullptr) {
        code[level] = '1';  // Add a corresponding code value
        if (generate_codes_recursively(node->right, code, level + 1, codes) != 0) {
            puts("Memory allocation failure in a right subtree while generating an 'assigning_code' string for a leaf node at 'generate_codes_recursively().'");
            return -1;
        }
    }

    // Successful execution
    return 0;
}

// Freeing the DS
// Free all the nodes in a given Node subtree recursively via the post-order traversal
// NOLINTNEXTLINE(misc-no-recursion)
static void free_tree_nodes(Node **root) {
    if (root == nullptr || *root == nullptr) return;
    free_tree_nodes(&((*root)->left));
    free_tree_nodes(&((*root)->right));

    free(*root);
    *root = nullptr;
}

// Free all the generated code strings in an array of pointers to chars
static void free_tree_codes(char *codes[CHAR_AMOUNT]) {
    if (codes == nullptr) return;
    for (size_t i = 0; i < CHAR_AMOUNT; ++i) {
        if (codes[i] != nullptr) {
            free(codes[i]);
            codes[i] = nullptr;
        }
    }
}

// Printing the Tree
// Prints the tree recursively sideways for debugging purposes
static void print_huffman_tree(const HuffmanTree *tree) {
    if (tree == nullptr || tree->root == nullptr) return;
    print_huffman_tree_recursively(tree->root, 0);
}

// Recursive helper to print the tree
// NOLINTNEXTLINE(misc-no-recursion)
static void print_huffman_tree_recursively(const Node *root, size_t level) {
    if (root != nullptr) {
        print_huffman_tree_recursively(root->right, level + 1);
        for (size_t i = 0; i < level; i++) printf("             ");
        printf("%c%s%zu%s", root->data, ": ", root->frequency, "\n");
        print_huffman_tree_recursively(root->left, level + 1);
    }
}