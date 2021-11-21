# KOMIHASH - Very Fast Non-Cryptographic Hash Function ##

## Introduction ##

The `komihash()` function available in the `komihash.c` file implements a very
fast 64-bit hash function, mainly designed for hash table uses.

It features both a high large-block hashing performance (28 GB/s on Ryzen
3700X) and a high hashing throughput for small messages. Performance on 32-bit
systems is, however, quite low.

It passes all [SMHasher](https://github.com/rurban/smhasher) tests. However,
its uses in PerlinNoise-like applications may be limited due to its design.

Note that this function is not cryptographically-secure, and in open systems
it should only be used with a secret seed, to minimize a chance of collision
attack.

## Other ##

The function is called the way it is called is to honor Komi Republic
(Russia), native to author.
