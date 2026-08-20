/*
 * prng_xoroshiro128pp.c
 *
 * Copyright(C) 2026 Matteo Zapparoli - zapparoli.matteo@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file contains the declaration of the PRNG that is to be tested.
 *
 * The functions set_seed(), next() and the global variable prng_name
 * must be modified in according to the PRNG to be tested
 *
 * PRNG:  The implementation is based on public domain code by D.Blackman
 *        and S.Vigna (vigna@acm.org)
 *
 */

#include <stdint.h> // uint64_t

static inline uint64_t rotl64(uint64_t x, int r)
{
    return (x << r) | (x >> ((-r) & 63));
}

typedef struct {
    uint64_t s[2];
} Xoroshiro128PPState;

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "xoroshiro128++ (seed = {0x123456789, 0x987654321})";

// Internal state
static Xoroshiro128PPState state;

// Set state to initial seed
void set_seed(void)
{
	state.s[0] = 0x123456789;
	state.s[1] = 0x987654321;
}

// Return next() value of PRNG 
uint64_t next(void) 
{
    const uint64_t s0 = state.s[0];
    uint64_t s1 = state.s[1];
    const uint64_t result = rotl64(s0 + s1, 17) + s0;
    s1 ^= s0;
    state.s[0] = rotl64(s0, 49) ^ s1 ^ (s1 << 21); // a, b
    state.s[1] = rotl64(s1, 28); // c
    return result;
}
