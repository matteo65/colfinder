/*
 * prng_xoshiro256ss.c
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
 * PRNG: xoshiro256** - Copyright(C) D.Blackman & S.Vigna (vigna@acm.org)
 *
 */

#include <stdint.h> // uint64_t

typedef struct {
	uint64_t s[4];
} xoshiro256ss_state;

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "Xoshiro256** (seed = {0x1234, 0x5678, 0x90ab, 0xcdef})";

// Internal state
static xoshiro256ss_state state;

// Set state to initial seed
void set_seed(void)
{
	state.s[0] = 0x1234;
	state.s[1] = 0x5678;
	state.s[2] = 0x90ab;
	state.s[3] = 0xcdef;
}

// Return next() value of PRNG 
uint64_t next(void) 
{
	// Xoshiro256** PRNG
	uint64_t result = state.s[1] * 5;
	result = (result << 7) | (result >> 57);
	result *= 9;

	uint64_t t = state.s[1] << 17;

	state.s[2] ^= state.s[0];
	state.s[3] ^= state.s[1];
	state.s[1] ^= state.s[2];
	state.s[0] ^= state.s[3];

	state.s[2] ^= t;
	state.s[3] = (state.s[3] << 45) | (state.s[3] >> 19);

	return result;
}