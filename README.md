# Colfinder 1.0

![Version](https://img.shields.io/badge/version-1.0-blue)
![License](https://img.shields.io/badge/license-Apache%202.0-green)
![Language](https://img.shields.io/badge/language-C-orange)

**Colfinder** is a high-performance collision analysis tool for 64-bit pseudo-random number generators (PRNGs).

Unlike traditional statistical test suites such as TestU01 and PractRand, which evaluate the statistical properties of generated sequences through a large collection of tests, colfinder performs a direct and exact collision count on the first **16 billion generated values**.

The tool was designed to verify whether a PRNG produces the expected number of collisions predicted by probability theory while requiring only about **4 GB of RAM** and completing in approximately **30 to 50 minutes** on modern hardware.

## Motivation

During the evaluation of several 64-bit PRNGs, it was observed that some generators successfully pass extensive test suites such as TestU01 and PractRand while still exhibiting an unexpected number of duplicate values when very large numbers of samples are generated.

Although collision behavior is theoretically related to the birthday paradox and should follow well-understood probabilistic expectations, few publicly available tools are capable of performing an exact collision count on billions of 64-bit outputs without requiring excessive memory.

colfinder was created to provide:

- Exact collision counting
- Large-scale empirical validation
- Low memory usage
- Fast execution
- A complementary test to traditional statistical batteries

## Theoretical Background

The collision behavior of a 64-bit PRNG can be estimated using the classical birthday paradox.

A perfect 64-bit generator produces values uniformly distributed over the space:

```text
M = 2^64
  = 18,446,744,073,709,551,616
```

When generating:

```text
N = 16,000,000,000
```

values, the expected number of pairwise collisions is approximately:

```text
λ = N(N - 1) / 2N
```

Substituting the values:

```text
λ ≈ 6.9388939
```

Therefore, a perfectly uniform random source is expected to produce about **seven collisions** among the first 16 billion generated values.

### Collision Distribution

For such a large output space and relatively small collision probability, the number of observed collisions is well approximated by a Poisson distribution:

```text
X ~ Poisson(λ = 6.9388939)
```

Approximate probabilities are:

| Collisions | Probability |
|-----------:|------------:|
|0|0.097%|
|1|0.673%|
|2|2.334%|
|3|5.398%|
|4|9.363%|
|5|12.994%|
|6|15.027%|
|7|14.896%|
|8|12.920%|
|9|9.962%|
|10|6.912%|
|11|4.360%|
|12|2.521%|
|13|1.346%|
|14|0.667%|
|15|0.309%|
|16|0.134%|

The most likely outcomes are therefore between **3 and 11 collisions** which are centered around the theoretical expectation.

### Interpreting Results

The purpose of colfinder is not merely to detect the existence of collisions.

Collisions are expected!

In fact, finding no collisions at all may be as suspicious as finding too many.

For a truly random 64-bit source generating 16 billion values: **P(0 collisions) ≈ 0.097%**  

Therefore, a complete absence of collisions is possible but relatively unlikely.

Likewise, observing a significantly larger number of collisions than predicted may indicate structural weaknesses, non-uniformity, or hidden correlations within the generator.

### Why Collision Counting Matters

Traditional statistical test suites such as TestU01 and PractRand evaluate many important properties of a random sequence, including:

- Uniformity
- Independence
- Linear complexity
- Correlation structure
- Pattern frequencies

However, these suites do not directly measure the exact number of duplicated 64-bit outputs produced over billions of generated values.

As a consequence, a generator may successfully pass conventional statistical batteries while still exhibiting collision behavior that deviates from theoretical expectations.

colfinder addresses this gap by performing a direct empirical measurement of collisions over a very large sample size.

For this reason, it should be considered a complementary validation tool rather than a replacement for established statistical test suites.

### Triple Collisions

The probability of observing a value three times among 16 billion uniformly random 64-bit outputs is extraordinarily small.

The expected number of triple collisions is approximately: **N³ / 6M²**  

which yields: **≈ 0.2 × 10⁻¹⁰**  

For this reason, the appearance of a triple collision is treated by colfinder as an immediate failure condition.

### Accepted Range

Although the theoretical expectation is approximately: **7 collisions**  random variation naturally occurs.  

colfinder therefore accepts any final result in the interval: **1 to 15 collisions** provided that no other failure condition is triggered.

If the number of collisions is 1 or 14 or 15 the test passes but is flagged as **SUSPICIOUS**; in this case it is recommended to rerun it with other seeds to see if it was an unfortunate case.  

## Algorithm

A straightforward collision analysis of 16 billion 64-bit values would require an impractical or very expensive amount of memory.

To overcome this limitation, colfinder divides the analysis into 32 independent passes.

### Block Partitioning

For pass **1, 1, 2, ... 32** the generator is restarted from the original seed and all 16 billion values are regenerated.

Only values whose lower 5 bits match the current pass number are retained:

```c
(value & 31) == block_id - 1
```

Assuming a uniform distribution, each block contains approximately **16,000,000,000 / 32 ≈ 500,000,000 values** which fits into roughly 4 GB of memory.

### Sorting

Each block is sorted entirely in memory; after sorting, duplicate values become adjacent and can be detected efficiently through a linear scan.

### Collision Detection

Every duplicate value encountered after sorting is:

- Printed
- Counted
- Included in the final statistics

The process is repeated for all 32 blocks; all operations are performed entirely in RAM.

## Memory Usage

Typical memory consumption: **~4 GB RAM**  

No disk storage is required.

No temporary files are created.

## Performance

Typical execution time: **30 to 50 minutes** depending on:

- CPU performance
- Memory bandwidth
- PRNG generation speed

## Pass/Fail Criteria

### Immediate Failure Conditions

The test terminates immediately (failure) when one of the following conditions occurs.

#### Non-Uniform Block Size

Expected block size: **500,000,000**

Failure occurs if: **block_size <= 495,000,000 or block_size >= 505,000,000**  

This indicates a significant deviation from the expected uniform distribution.

#### Triple-or-Higher Collision

Failure occurs if a value appears three or more times.
Such events are considered extraordinarily unlikely for a correctly behaving 64-bit PRNG over 16 billion outputs.

#### Excessive Number of Collisions

Failure occurs if: **collision_count > 15**

#### No Collisions Detected

Failure occurs if the final **collision_count == 0**

A complete absence of collisions is statistically possible but sufficiently unlikely to be considered suspicious.

### Successful Completion

The test is considered successful when:

- All 32 blocks are processed
- No failure condition occurs
- The final collision count is between 1 and 15 inclusive

## Project Structure

```text
|
|---reports
|       pgc128_64.txt
│       sirius64.txt
│       xoshiro256ss.txt
|       ...
│
\---src
    │   colfinder.c
    │   prng.c
    │   prng.h
    │
    \---generators
            prng_pgc128.c
            prng_pgc64_64.c
            prng_sirius64.c
            prng_xoshiro256ss.c
            ...
```
In the generator/ directory there are some prng.c files specific to some well-known 64-bit PRNG algorithms such as xoshiro, pgc, splitmix; direcory reports/ also contain the execution reports of these generators.

## Integrating a PRNG

To test a generator, edit `prng.c`.

The following functions must be implemented:

```c
uint64_t next(void);
void set_seed(void);
```

### next()

Returns the next generated 64-bit value.

```c
uint64_t next(void);
```

### set_seed()

Restores the generator to its initial state before each analysis pass.

```c
void set_seed(void);
```

### Generator Description

The global variable:

```c
char *prng_name;
```

must contain the generator name and optionally the seed information.

Example:
```c
char *prng_name = "MyPrng64 seed=123456789";
```

The string is displayed in the output header.

# Compilation

Example using GCC:
```bash
gcc -O3 colfinder.c prng.c -o colfinder
```

POSIX-compatible C compiler should work.

The project has no external dependencies.

You can compile colfinder for a generator in the tests directory with this command:

```bash
gcc -O3 colfinder.c ../tests/prng_xoshiro256ss.c -o ../colfinder_xoshiro256ss
```

## Usage

```bash
colfinder [start]
```
Running the program without arguments display the built-in help screen.

## Hash Function Analysis

Although primarily designed for PRNG evaluation, colfinder can also be adapted to analyze collisions produced by 64-bit hash functions.

With minor modifications, the same collision-counting engine can be used to verify whether a hashing algorithm exhibits collision behavior consistent with theoretical expectations.

## License

Licensed under the Apache License 2.0.

See the `LICENSE` file for details.
