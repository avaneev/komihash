# KOMIHASH - Very Fast Hash Function ##

## Introduction ##

The `komihash()` function available in the `komihash.h` file implements a very
fast 64-bit hash function, mainly designed for hash-table uses; produces
identical hashes on both big- and little-endian systems. Function's code is
portable, scalar.

This function features both a high large-block hashing performance (28 GB/s on
Ryzen 3700X) and a high hashing throughput for small messages (about 13
cycles/hash for 0-15-byte messages if `UseSeed==0`, 15 cycles/hash otherwise).
Performance on 32-bit systems is, however, quite low. Also, large-block
hashing performance on big-endian systems may be lower due to the need of
byte-swapping.

It passes all [SMHasher](https://github.com/rurban/smhasher) tests.

Technically, `komihash` is close to the class of hash functions like `wyhash`
and `CircleHash`, that are, in turn, close to the `lehmer64` PRNG. However,
`komihash` is structurally different to them in that it accumulates the full
128-bit multiplication result without "compressing" it into a single 64-bit
state variable. Thus `komihash` does not lose differentiation between
consecutive states while others do not. Another important difference in
`komihash` is that it parses the input message without overlaps. While
overlaps allow a function to have fewer code branches, they are considered
"non-ideal", potentially causing collisions and seed value flaws.

Note that this function is not cryptographically-secure, and in open systems
it should only be used with a secret seed, to minimize the chance of a
collision attack.

## Other ##

This function is named the way it is named is to honor Komi Republic
(Russia), native to the author.
