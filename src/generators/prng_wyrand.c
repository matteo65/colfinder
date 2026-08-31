/*
 * prng_wyrand.c
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
 * wyrand: author Wang Yi. wyhash project, public domain (Unlicense)
 *
 */

#include <stdint.h> // uint64_t

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "wyrand (seed = 12345123451234512345ull)";

// Internal state
static uint64_t state;

// Set state to initial seed
void set_seed(void)
{
	state = 12345123451234512345ull;
}

static inline void _wymum(uint64_t *A, uint64_t *B) 
{
	__uint128_t r = *A; 
	r *= *B; 
	*A = (uint64_t)r; 
	*B = (uint64_t)(r >> 64);
}

//multiply and xor mix function, aka MUM
static inline uint64_t _wymix(uint64_t A, uint64_t B) 
{ 
	_wymum(&A, &B); 
	return A ^ B; 
}

// Return next() value of PRNG 
uint64_t next(void) 
{
	state += 0xa0761d6478bd642fULL; 
	return _wymix(state, state ^ 0xe7037ed1a0b428dbULL);
}
