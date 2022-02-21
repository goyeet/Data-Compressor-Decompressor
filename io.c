#include "io.h"

#include "code.h"
#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

static uint8_t buffer[BLOCK] = { 0 }; // buffer shared by write_code and flush_codes
static uint32_t index = 0; // tracks position in buffer
uint64_t bytes_written = 0; // stat tracker for verbose
uint64_t bytes_read = 0; // stat tracker for verbose

// reads bytes into a buffer until no more can be read
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    uint64_t total = 0;
    uint64_t num_bytes_read = 0;
    if (nbytes == 0) {
        return 0;
    }
    while ((num_bytes_read = read(infile, buf + total, nbytes - total)) > 0) {
        total += num_bytes_read;
        if (bytes_read == (uint64_t) nbytes) {
            break;
        }
    }
    bytes_read += total;
    return total;
}

// writes a number of bytes into a buffer until no more can be written
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    uint64_t total = 0;
    uint64_t num_bytes_written = -2;
    if (nbytes == 0) {
        return 0;
    }
    while ((num_bytes_written = write(outfile, buf + total, nbytes - total)) > 0) {
        total += num_bytes_written;
        if (bytes_read == (uint64_t) nbytes) {
            break;
        }
    }
    bytes_written += total;
    return total;
}

// CITE: Christian/Eugene
// Reads a bit from a file
bool read_bit(int infile, uint8_t *bit) {
    static uint8_t read_bit_buffer[BLOCK] = { 0 };
    static uint32_t read_bit_top = 0; // tracks position in buffer
    static uint32_t read_bit_buffer_size = 0;
    if (read_bit_top == 0) { // Fill if buffer is empty
        read_bit_buffer_size = read_bytes(infile, read_bit_buffer, BLOCK);
    }
    if (read_bit_buffer_size == 0) { // set the end
        return false;
    }
    *bit = (1 & (read_bit_buffer[read_bit_top / 8] >> (read_bit_top % 8)));
    read_bit_top++;
    if (read_bit_top == read_bit_buffer_size * 8) {
        read_bit_top = 0; // reset to 0 since done reading
    }
    return true;
}

// CITE: Eugene
// Buffers a code
// Buffer is written when filled
// Buffer each bit into the buffer
void write_code(int outfile, Code *c) {
    uint32_t buffer_size = 0;
    for (uint32_t i = 0; i < code_size(c); i++) {
        uint8_t bit = code_get_bit(c, i);
        if (bit == 1) { // set bit at index
            buffer[index / 8] |= (1 << (index % 8));
        } else { // clear bit
            buffer[index / 8] &= ~(1 << (index % 8));
        }
        index++;
        if (index == BLOCK * 8) { // buffer is full
            write_bytes(outfile, buffer, BLOCK);
            buffer_size += BLOCK;
            index = 0;
        }
    }
}

// writes out any leftover, buffered bits and zeroes out extra bits and last byte
void flush_codes(int outfile) {
    uint32_t buffer_size = 0;
    if (index % 8 == 0) {
        write_bytes(outfile, buffer, index / 8);
        buffer_size += index / 8;
    } else {
        buffer[index / 8] &= ((1 << (index % 8)) - 1); // zeroes out extra bits
        write_bytes(outfile, buffer, index / 8 + 1);
    }
    index = 0;
}
