/*
 * prng.h
 *
 * Copyright(C) 2026 Matteo Zapparoli - zapparoli.matteo@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef PRNG_H
#define PRNG_H

#include <stdint.h> // uint64_t

#ifdef __cplusplus
extern "C" {
#endif

extern char *prng_name;

void set_seed(void);
 
uint64_t next(void);

#ifdef __cplusplus
}
#endif

#endif /* PRNG_H */




