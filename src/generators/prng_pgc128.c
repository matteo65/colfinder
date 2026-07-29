/*
 * prng_pgc128.c
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
 * PCG-XSH-RR 128/64 (permuted conguential generator) by Melissa O'Neill.
 *
 */

#include <stdint.h> // uint64_t

// Il moltiplicatore standard PCG a 128 bit
#define PCG_MULTIPLIER_128 (((__uint128_t)2549297995355413924ULL << 64) | 4865540595714422341ULL)

typedef struct {
    __uint128_t state;  // Stato interno a 128 bit
    __uint128_t inc;    // Incremento a 128 bit (definisce lo stream/sequenza)
} pcg128_64_t;

static pcg128_64_t state;

// Funzione di generazione del numero casuale a 64 bit
uint64_t pcg128_next(void) {
    // 1. Salva lo stato corrente
    __uint128_t old_state = state.state;

    // 2. Avanza lo stato interno usando il LCG a 128 bit
    // L'overflow a 128 bit avviene automaticamente via hardware/compilatore
    state.state = old_state * PCG_MULTIPLIER_128 + state.inc;

    // 3. Funzione di output: XSH-RR (Xorshift High, Random Rotate)
    // Applica xorshift ai bit alti dello stato per restringerli a 64 bit
    uint64_t xorshifted = (uint64_t)(((old_state >> 64) ^ old_state) >> 22);
    
    // Determina l'entità della rotazione usando i 5 bit più significativi
    unsigned int rot = (unsigned int)(old_state >> 122);
    
    // Esegue la rotazione circolare a destra (Right Rotate) a 64 bit
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 63));
}

// Funzione di inizializzazione (Seeding)
void pcg128_init(__uint128_t init_state, __uint128_t init_seq) {
    // L'incremento deve essere sempre dispari per garantire il periodo massimo (2^128)
    state.inc = (init_seq << 1) | 1;
    
    // Inizializzazione dello stato eseguendo un passo iniziale
    state.state = 0 + state.inc;
    (void)pcg128_next();
    state.state += init_state;
    (void)pcg128_next();
}

// PRNG header or name. It is written to stdout at the beginning of the test
char *prng_name = "PGC128_64 (state=0x123456789, inc=0x987654321)";

// Set state to initial seed
void set_seed(void)
{
	pcg128_init(0x123456789, 0x987654321);
}

// Return next() value of PRNG 
uint64_t next(void) 
{
	return pcg128_next();
}
