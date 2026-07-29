/*
 * prng.c
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

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "Sirius64 (seed = 0x1234567890abcdef)";

// Internal state
static uint64_t state;

// Set state to initial seed
void set_seed(void)
{
	state = 0x1234567890abcdefLLU;
}

// Return next() value of PRNG 
uint64_t next(void) 
{
	// Sirius64 PRNG
	uint64_t z = (state += 0x9e3779b97f4a7c15ull);
	z = 0x9e3779b97f4a7c15ull * (z ^ (z >> 17));
	z = (z << 32) | (z >> 32);
	return 0x9e3779b97f4a7c15ull * (state ^ z ^ (z >> 17));
}