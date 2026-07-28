# colfinder 1.0

**Colfinder** is a high-performance collision analysis tool for 64-bit pseudo-random number generators (PRNGs).

Unlike traditional statistical test suites such as TestU01 and PractRand, which evaluate the statistical properties of generated sequences through a large collection of tests, colfinder performs a direct and exact collision count on the first **12 billion generated values**.

The tool was designed to verify whether a PRNG produces the expected number of collisions predicted by probability theory while requiring only about **3 GB of RAM** and completing in approximately **20 to 30 minutes** on modern hardware.

## Motivation

During the evaluation of several 64-bit PRNGs, it was observed that some generators successfully pass extensive test suites such as TestU01 and PractRand while still exhibiting an unexpected number of duplicate values when very large numbers of samples are generated.

Although collision behavior is theoretically related to the birthday paradox and should follow well-understood probabilistic expectations, few publicly available tools are capable of performing an exact collision count on billions of 64-bit outputs without requiring excessive memory.

colfinder was created to provide:

- Exact collision counting
- Large-scale empirical validation
- Low memory usage
- Fast execution
- A complementary test to traditional statistical batteries

It should not be considered a replacement for TestU01 or PractRand, but rather an additional validation step capable of detecting anomalies that may not be revealed by conventional tests.

## Theoretical Background

The collision behavior of a 64-bit PRNG can be estimated using the classical birthday paradox.

A perfect 64-bit generator produces values uniformly distributed over the space:

```text
N = 2^64
  = 18,446,744,073,709,551,616
```

When generating:

```text
n = 12,000,000,000
```

values, the expected number of pairwise collisions is approximately:

```text
λ = n(n - 1) / (2N)
```

Substituting the values:

```text
λ ≈ 3.903
```

Therefore, a perfectly uniform random source is expected to produce about four collisions among the first 12 billion generated values.

### Collision Distribution

For such a large output space and relatively small collision probability, the number of observed collisions is well approximated by a Poisson distribution:

```text
X ~ Poisson(λ = 3.903)
```

Approximate probabilities are:

| Collisions | Probability |
|-----------:|------------:|
| 0 | 2.0% |
| 1 | 7.9% |
| 2 | 15.4% |
| 3 | 20.1% |
| 4 | 19.6% |
| 5 | 15.3% |
| 6 | 10.0% |
| 7 | 5.6% |
| 8 | 2.7% |

The most likely outcomes are therefore between **2 and 5 collisions** which are centered around the theoretical expectation.

### Interpreting Results

The purpose of colfinder is not merely to detect the existence of collisions.

Collisions are expected.

In fact, finding no collisions at all may be as suspicious as finding too many.

For a truly random 64-bit source generating 12 billion values: **P(0 collisions) ≈ 2%**  

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

The probability of observing a value three times among 12 billion uniformly random 64-bit outputs is extraordinarily small.

The expected number of triple collisions is approximately: **n³ / (6N²)**  

which yields: **≈ 8.5 × 10⁻¹⁰**  

This corresponds to less than one expected event in a billion independent executions of the test.

For this reason, the appearance of a triple collision is treated by colfinder as an immediate failure condition.

### Accepted Range

Although the theoretical expectation is approximately: **3.9 collisions**  random variation naturally occurs.  

colfinder therefore accepts any final result in the interval: **1 to 8 collisions** provided that no other failure condition is triggered.

This range comfortably encompasses the vast majority of outcomes expected from a statistically sound 64-bit PRNG generating the first 12 billion values of its sequence.

## Overview

The program analyzes the first **12,000,000,000** generated 64-bit values.

The final result provides a direct measurement of the actual number of duplicated values observed.

## Algorithm

A straightforward collision analysis of 12 billion 64-bit values would require an impractical amount of memory.

To overcome this limitation, colfinder divides the analysis into 32 independent passes.

### Block Partitioning

For pass **0, 1, 2, ... 31** the generator is restarted from the original seed and all 12 billion values are regenerated.

Only values whose lower 5 bits match the current pass number are retained:

```c
(value & 31) == block_id
```

Assuming a uniform distribution, each block contains approximately **12,000,000,000 / 32 ≈ 375,000,000 values** which fits into roughly 3 GB of memory.

### Sorting

Each block is sorted entirely in memory using an optimized sorting implementation based on:

- Quicksort
- Insertion sort

After sorting, duplicate values become adjacent and can be detected efficiently through a linear scan.

### Collision Detection

Every duplicate value encountered after sorting is:

- Printed
- Counted
- Included in the final statistics

The process is repeated for all 32 blocks.

No temporary files are used.

All operations are performed entirely in RAM.

## Memory Usage

Typical memory consumption:

```text
~3 GB RAM
```

No disk storage is required.

No temporary files are created.

## Performance

Typical execution time: **20 to 30 minutes**

depending on:

- CPU performance
- Memory bandwidth
- PRNG generation speed

## Pass/Fail Criteria

### Immediate Failure Conditions

The test terminates immediately when one of the following conditions occurs.

#### Non-Uniform Block Size

Expected block size: **375,000,000**

Failure occurs if: **block_size <= 370,000,000 or block_size >= 380,000,000**  

This indicates a significant deviation from the expected uniform distribution.

#### Triple-or-Higher Collision

Failure occurs if a value appears three or more times.
Such events are considered extraordinarily unlikely for a correctly behaving 64-bit PRNG over 12 billion outputs.

#### Excessive Number of Collisions

Failure occurs if: **collision_count > 8**

#### No Collisions Detected

Failure occurs if the final **collision_count == 0**

A complete absence of collisions is statistically possible but sufficiently unlikely to be considered suspicious.

### Successful Completion

The test is considered successful when:

- All 32 blocks are processed
- No failure condition occurs
- The final collision count is between 1 and 8 inclusive

The theoretical expectation is: **3.903 collisions**  

Values in the range: **3 to 5 collisions** can be regarded as particularly close to the theoretical prediction.

## Project Structure

```text
colfinder/
├── colfinder.c
├── prng.c
└── prng.h
```

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
gcc -O3 -mcmodel=medium colfinder.c prng.c -o colfinder
```

POSIX-compatible C compiler should work.

The project has no external dependencies.

## Usage

```bash
colfinder [start]
```

Running the program without arguments display the built-in help screen.

The optional `start` parameter allows execution to begin from a specific block.

## Design Goals

colfinder was designed with the following objectives:

- Exact collision counting
- No statistical approximations
- Low memory footprint
- No temporary files
- Cross-platform portability
- Compatibility with any 64-bit PRNG- Practical execution time on consumer hardware

## Hash Function Analysis

Although primarily designed for PRNG evaluation, colfinder can also be adapted to analyze collisions produced by 64-bit hash functions.

With minor modifications, the same collision-counting engine can be used to verify whether a hashing algorithm exhibits collision behavior consistent with theoretical expectations.

## License

Licensed under the Apache License 2.0.

See the `LICENSE` file for details.
