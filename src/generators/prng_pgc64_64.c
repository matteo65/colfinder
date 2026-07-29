/*
 * prng_pgc64_64.c
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
 * Copyright(C) The PCG algorithm family is suggested by M.E. O'Neill
 * (https://pcg-random.org).
 *
 * PCG64 PRNG implementation with RXS-M-XS64 function.
 *
 */

#include <stdint.h> // uint64_t

typedef struct {
    uint64_t state; ///< LCG state
    uint64_t inc;   ///< LCG increment, must be odd
} pcg64_state;

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "PGC64_64 (state=0x1234567890abcdef, inc=1)";

// Internal state
static pcg64_state state;

// Set state to initial seed
void set_seed(void)
{
	state.state = 0x1234567890abcdefLLU;
	state.inc = 1;
}

// Return next() value of PRNG 
uint64_t next(void) 
{
	const uint64_t word = ((state.state >> ((state.state >> 59) + 5)) ^ state.state) * 12605985483714917081ull;
	state.state = state.state * 6364136223846793005ull + state.inc;
	return (word >> 43) ^ word;
}
