#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"

#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hvi:o:" // Valid inputs

// prints help page
static void help() {
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "  A Huffman encoder.\n");
    fprintf(stderr, "  Decompresses a file using the Huffman coding algorithm.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "  -h             Program usage and help.\n");
    fprintf(stderr, "  -v             Print compression statistics.\n");
    fprintf(stderr, "  -i infile      Input file to decompress.\n");
    fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
}

// closes files after used by program
static void close_files(int infile, int outfile) {
    close(infile);
    close(outfile);
}

// driver code of program
int main(int argc, char **argv) {
    int opt = 0;
    bool verbose = false;
    int infile = 0;
    int outfile = 1;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help(); return 0; // print help page
        case 'v': verbose = true; break; // print verbose output
        case 'i':
            infile = open(optarg, O_RDONLY);
            if (infile == -1) {
                printf("Error opening file\n");
                return -1;
            };
            break;
        case 'o':
            outfile = open(optarg, O_WRONLY | O_CREAT);
            if (outfile == -1) {
                printf("Error opening file\n");
                return -1;
            };
            break;
        default: help(); return 1;
        }
    }

    Header header;
    // read in the header from infile and verify the magic number
    read_bytes(infile, (uint8_t *) &header, sizeof(Header));
    if (header.magic != MAGIC) {
        fprintf(stderr, "Error: Invalid header");
        return -1;
    }

    struct stat instatbuf;
    // set the permissions
    fstat(infile, &instatbuf);
    fchmod(outfile, header.permissions);

    // read the dumped tree from infile into an array
    uint8_t tree_dump[header.tree_size];
    read_bytes(infile, tree_dump, header.tree_size);

    // reconstruct the Huffman tree
    Node *root_node = NULL;
    root_node = rebuild_tree(header.tree_size, tree_dump);

    // Read infile one bit at a time
    Node *node = root_node;
    uint64_t bytes_writ = 0;
    uint32_t index = 0;
    uint8_t bit = 0;
    uint8_t buf[BLOCK];
    while (read_bit(infile, &bit) && bytes_writ < header.file_size) {
        if (bit) {
            node = node->right; // a bit of value 1 is read
        } else {
            node = node->left; // a bit of value 0 is read
        }
        if (node->left == NULL && node->right == NULL) { // leaf node
            buf[index++] = node->symbol; // add the symbol to the buffer
            bytes_writ++;
            node = root_node; // reset current node back to root
            if (index == BLOCK) { // write buffer whenever it is filled
                write_bytes(outfile, buf, BLOCK);
                index = 0;
            }
        }
    }

    // write leftover bytes to buffer if any remain
    write_bytes(outfile, buf, index);

    if (verbose) {
        fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", bytes_read);
        fprintf(stderr, "Decompressed file size: %" PRIu64 " bytes\n", bytes_written);
        fprintf(
            stderr, "Space saving: %.2f%%\n", 100 * (1 - ((double) bytes_read / bytes_written)));
    }

    // close infile and outfile
    close_files(infile, outfile);

    delete_tree(&root_node);
    return 0;
}
