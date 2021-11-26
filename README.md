# KOMIHASH - Very Fast Hash Function ##

## Introduction ##

The `komihash()` function available in the `komihash.h` file implements a very
fast 64-bit hash function, mainly designed for hash-table uses; produces
identical hashes on both big- and little-endian systems. Function's code is
portable, scalar.

This function features both a high large-block hashing performance (27.5 GB/s
on Ryzen 3700X) and a high hashing throughput for small messages (about 12
cycles/hash for 0-15-byte messages). Performance on 32-bit systems is,
however, quite low. Also, large-block hashing performance on big-endian
systems may be lower due to the need of byte-swapping.

It passes all [SMHasher](https://github.com/rurban/smhasher) tests.
Performance estimates on that page are, however, unreliable.

Technically, `komihash` is close to the class of hash functions like `wyhash`
and `CircleHash`, that are, in turn, close to the `lehmer64` PRNG. However,
`komihash` is structurally different to them in that it accumulates the full
128-bit multiplication result without "compressing" into a single 64-bit state
variable. Thus `komihash` does not lose differentiation between consecutive
states while others may. Another important difference in `komihash` is that it
parses the input message without overlaps. While overlaps allow a function to
have fewer code branches, they are considered "non-ideal", potentially causing
collisions and seed value flaws. Beside that, `komihash` features a superior
user seed handling and PerlinNoise hashing.

Note that this function is not cryptographically-secure, and in open systems
it should only be used with a secret seed, to minimize the chance of a
collision attack.

## Discussion ##

You may wonder why `komihash` does not include a quite common `^MsgLen` XOR
instruction at some place in the code. The reason is that for a
non-cryptographic hash function such instruction provides no additional
security benefit. While it may seem that such instruction protects from simple
"state XORing" collision attacks, in practice it offers no protection if one
considers how powerful SAT solvers are: in a matter of seconds, they can
"forge" a preimage of any length that produces a required hash value. It is
also important to note that in such "fast" hash functions like `komihash` the
input message has a complete control over the state variables.

Is 128-bit version of this hash function planned? Most probably, not. While
such version may be reasonable for data structure compatibility reasons, there
is no much practical sense to use 128-bit hashes at a local level: a reliable
64-bit hash allows one to have 4.2 billion diverse binary objects (e.g. files
in a file system, or entries in a hash-table) without collisions, on average.
On the other hand, on a worldwide scale, having 128-bit hashes is clearly not
enough considering the number of existing digital devices and the number of
diverse binary objects (e.g. files and records in databases) on each of them.

A similarly efficient streamed version of `komihash` is, however, doable given
a serious interest in one.

## Other ##

This function is named the way it is named is to honor Komi Republic
(in Russia), native to the author.
