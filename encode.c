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
    fprintf(stderr, "  Compresses a file using the Huffman coding algorithm.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "  -h             Program usage and help.\n");
    fprintf(stderr, "  -v             Print compression statistics.\n");
    fprintf(stderr, "  -i infile      Input file to compress.\n");
    fprintf(stderr, "  -o outfile     Output of compressed data.\n");
}

// closes files after used by program
static void close_files(int infile, int outfile) {
    close(infile);
    close(outfile);
}

// read through file to construct histogram
static uint16_t construct_histogram(
    int infile, uint8_t *buf, uint64_t histogram[static ALPHABET], int bytes_read) {
    while ((bytes_read = read_bytes(infile, buf, BLOCK)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            histogram[buf[i]]++;
        }
    }
    uint16_t unique_symbols = 0;
    for (int i = 0; i < ALPHABET; i++) {
        if (histogram[i] > 0) {
            unique_symbols++;
        }
    }
    return unique_symbols;
}

// driver code of the program
int main(int argc, char **argv) {
    int opt = 0;
    bool verbose = false;
    int infile = 0;
    int outfile = 1;
    int tempfile = 0;

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

    int bytes_read = 0;
    uint8_t buf[BLOCK];

    // if infile is stdio, create/write to a temp file
    if (lseek(infile, 0, SEEK_SET) == -1) {
        tempfile = open("/tmp/encode.temp", O_CREAT | O_RDWR | O_TRUNC, 0600);
        while ((bytes_read = read_bytes(infile, buf, BLOCK)) > 0) {
            write_bytes(tempfile, buf, bytes_read);
        }
        infile = tempfile;
    }
    lseek(infile, 0, SEEK_SET);

    struct stat instatbuf;
    // get stats for infile and change mode
    fstat(infile, &instatbuf);
    fchmod(outfile, instatbuf.st_mode);

    // construct histogram
    uint64_t histogram[ALPHABET] = { 0 };
    histogram[0]++; // increment index 0
    histogram[255]++; // increment index 255
    uint16_t unique_symbols = construct_histogram(infile, buf, histogram, bytes_read);

    // construct huffman tree
    Node *huffman_tree = build_tree(histogram);

    // construct code table
    Code code_table[ALPHABET] = { 0 };
    build_codes(huffman_tree, code_table);

    // construct header
    Header header;
    header.magic = MAGIC;
    header.permissions = instatbuf.st_mode;
    header.tree_size = (3 * unique_symbols) - 1;
    header.file_size = instatbuf.st_size;

    // write constructed header to outfile
    write_bytes(outfile, (uint8_t *) &header, sizeof(Header));

    // write constructed huffman tree to outfile
    dump_tree(outfile, huffman_tree);

    // write the corresponding code for each symbol to outfile
    lseek(infile, 0, SEEK_SET);
    while ((bytes_read = read_bytes(infile, buf, BLOCK)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            write_code(outfile, &code_table[buf[i]]);
        }
    }
    flush_codes(outfile);

    // print verbose stats
    if (verbose) {
        fprintf(stderr, "Uncompressed file size: %" PRIu64 " bytes\n", instatbuf.st_size);
        fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %.2f%%\n",
            100 * (1 - ((double) bytes_written / instatbuf.st_size)));
    }

    // temp file is no longer needed
    if (tempfile) {
        unlink("/tmp/encode.temp");
    }

    // close infile and outfile
    close_files(infile, outfile);

    delete_tree(&huffman_tree);
    return 0;
}
