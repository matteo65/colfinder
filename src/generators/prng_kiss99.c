/*
 * prng_kiss99.c
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
 */

#include <stdint.h> // uint64_t

typedef struct {
    uint32_t z;     ///< MWC state 1: c - upper half, x - lower half
    uint32_t w;     ///< MWC state 2: c - upper half, x - lower half
    uint32_t jsr;   ///< SHR3 state
    uint32_t jcong; ///< LCG state
} KISS99State;

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "Kiss99 (z=0x1234 w=0x5678 jsr=0x90ab jcong=0xcdef)";

// Internal state
static KISS99State state;

// Set state to initial seed
void set_seed(void)
{
	state.z = 0x1234;
	state.w = 0x5678;
	state.jsr = 0x90ab;
	state.jcong = 0xcdef;
}

#include <stdint.h>

static uint32_t kiss99(void)
{
    /* MWC */
    state.z = 36969 * (state.z & 65535u) + (state.z >> 16);
    state.w = 18000 * (state.w & 65535u) + (state.w >> 16);
    uint32_t mwc = (state.z << 16) + state.w;

    /* XORSHIFT */
    state.jsr ^= state.jsr << 17;
    state.jsr ^= state.jsr >> 13;
    state.jsr ^= state.jsr << 5;

    /* CONG */
    state.jcong = 69069 * state.jcong + 1234567;

    return (mwc ^ state.jcong) + state.jsr;
}

// Return next() value of PRNG 
uint64_t next(void) 
{
	uint64_t hi = kiss99();
	uint64_t lo = kiss99();

	return (hi << 32) | lo;
}
