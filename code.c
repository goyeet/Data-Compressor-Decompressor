#include "code.h"

#include "defines.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// constructor function for code
// CITE: Eugene
Code code_init(void) {
    Code c;
    c.top = 0;
    for (int i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

// returns size of the code
uint32_t code_size(Code *c) {
    return c->top;
}

// Checks if code is empty
bool code_empty(Code *c) {
    return c->top == 0;
}

// Checks if code is full
bool code_full(Code *c) {
    return c->top == ALPHABET;
}

// sets the bit at index i in the code, by setting it to 1
bool code_set_bit(Code *c, uint32_t i) {
    if (i > MAX_CODE_SIZE || i < 0) {
        return false;
    }
    c->bits[i / 8] |= (1 << (i % 8));
    return true;
}

// clears the bit at index i in the code, by setting it to 0
bool code_clr_bit(Code *c, uint32_t i) {
    if (i > MAX_CODE_SIZE || i < 0) {
        return false;
    }
    c->bits[i / 8] &= ~(1 << (i % 8));
    return true;
}

// gets bit at index i in the code
bool code_get_bit(Code *c, uint32_t i) {
    if ((i > MAX_CODE_SIZE) || i < 0) {
        return false;
    }
    return (c->bits[i / 8] >> i % 8) & 0x1;
}

// pushes a bit onto code
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    if (bit == 0) {
        c->bits[c->top / 8] &= ~(1 << (c->top % 8));
    } else {
        c->bits[c->top / 8] |= (1 << (c->top % 8));
    }
    c->top++;
    return true;
}

// pops a bit off of code
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top--;
    *bit = (1 & (c->bits[c->top / 8] >> (c->top % 8)));
    return true;
}

// print debugging function
void code_print(Code *c) {
    uint32_t n = code_size(c);
    for (uint32_t i = 0; i < n; i++) {
        if (code_get_bit(c, i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
}
