# KOMIHASH - Very Fast Hash Function ##

## Introduction ##

The `komihash()` function available in the `komihash.h` file implements a very
fast 64-bit hash function, mainly designed for hash-table uses; produces
identical hashes on both big- and little-endian systems. Function's code is
portable, scalar.

This function features both a high large-block hashing performance (27.7 GB/s
on Ryzen 3700X) and a high hashing throughput of small messages (about 12
cycles/hash for 0-15-byte messages). Performance on 32-bit systems is,
however, quite low. Also, large-block hashing performance on big-endian
systems may be lower due to the need of byte-swapping.

It passes all [SMHasher](https://github.com/rurban/smhasher) tests.
Performance estimates on that page may be unreliable.

Technically, `komihash` is close to the class of hash functions like `wyhash`
and `CircleHash`, that are, in turn, close to the `lehmer64` PRNG. However,
`komihash` is structurally different to them in that it accumulates the full
128-bit multiplication result without "compression" into a single 64-bit state
variable. Thus `komihash` does not lose differentiation between consecutive
states while others may. Another important difference in `komihash` is that it
parses the input message without overlaps. While overlaps allow a function to
have fewer code branches, they are considered "non-ideal", potentially causing
collisions and seed value flaws. Beside that, `komihash` features a superior
seed value handling and PerlinNoise hashing.

Note that this function is not cryptographically-secure: in open systems it
should only be used with a secret seed, to minimize the chance of a collision
attack.

## Comparisons ##

These are the performance comparisons made and used by the author during the
development of `komihash`.

### LLVM clang-cl 8.0.1 64-bit, Windows 10, Ryzen 3700X (Zen2), 4.2 GHz ###

Compiler options: `/Ox /arch:sse2`; overhead: `1.8 cycles`.

|Hash function  |0-15b, cycles/h|8-28b, cycles/h|bulk, GB/s     |
|----           |----           |----           |----           |
|komihash 2.8   |11.3           |17.4           |27.7           |
|wyhash_final3  |13.4           |17.8           |29.7           |
|XXH3_64 0.8.0  |17.5           |21.1           |29.0           |

### GCC 8.5.0 64-bit, CentOS 8, Xeon E-2176G (CoffeeLake), 3.7 GHz ###

Compiler options: `-O3 -msse2`; overhead: `4.7 cycles` (seems a bit high).

|Hash function  |0-15b, cycles/h|8-28b, cycles/h|bulk, GB/s     |
|----           |----           |----           |----           |
|komihash 2.8   |15.1           |18.5           |24.7           |
|wyhash_final3  |12.3           |15.9           |29.8           |
|XXH3_64 0.8.0  |13.9           |18.5           |26.6           |

### GCC 8.5.0 64-bit, CentOS 8, Xeon E-2176G (CoffeeLake), 3.7 GHz ###

Compiler options: `-O3 -mavx2`; overhead: `4.7 cycles` (seems a bit high).

|Hash function  |0-15b, cycles/h|8-28b, cycles/h|bulk, GB/s     |
|----           |----           |----           |----           |
|komihash 2.8   |13.6           |17.5           |24.7           |
|wyhash_final3  |12.3           |15.8           |30.1           |
|XXH3_64 0.8.0  |15.5           |19.3           |38.0           |

### Apple clang 12.0.0 64-bit, macOS 12.0.1, Apple M1, 3.5 GHz ###

Compiler options: `-O3`; overhead: `unestimatable`.

|Hash function  |0-15b, cycles/h|8-28b, cycles/h|bulk, GB/s     |
|----           |----           |----           |----           |
|komihash 2.8   |10.1           |11.4           |23.5           |
|wyhash_final3  |7.9            |8.1            |26.1           |
|XXH3_64 0.8.0  |8.2            |8.2            |30.5           |

Notes: `XXH3_64` is unseeded. `bulk` is 256000 bytes. `GB/s` should not be
misinterpreted as `GiB/s`. `cycles/h` means `processor clock ticks per hash
value`, includes overhead.

The following methodology was used to obtain the `cycles/h` values:

```
	const uint64_t rc = 1ULL << 26;
	const int minl = 8; const int maxl = 28;
	volatile uint64_t msg[ 8 ] = { 0 };
	uint64_t v = 0;

	const TClock t1( CSystem :: getClock() );

	for( int k = minl; k <= maxl; k++ )
	{
		volatile size_t msgl = k;
		volatile uint64_t sd = k + 1;

		for( uint64_t i = 0; i < rc; i++ )
		{
			v ^= komihash( (uint8_t*) &msg, msgl, sd );
//			v ^= wyhash( (uint8_t*) &msg, msgl, sd, _wyp );
//			v ^= XXH3_64bits( (uint8_t*) &msg, msgl );
//			v ^= msg[ 0 ]; // Used to estimate the overhead.
			msg[ 0 ]++;
		}
	}

	printf( "%016llx\n", v );
	printf( "%.5f\n", CSystem :: getClockDiffSec( t1 ) * 4.2e9 / // 3.7 on Xeon, 3.5 on M1
		( rc * ( maxl - minl + 1 )));
```

## Discussion ##

You may wonder, why `komihash` does not include a quite common `^MsgLen` XOR
instruction at some place in the code? The main reason is that due to the way
`komihash` parses the input message such instruction is not necessary. Another
reason is that for a non-cryptographic hash function such instruction provides
no additional security: while it may seem that such instruction protects from
simple "state XORing" collision attacks, in practice it offers no protection
if one considers how powerful [SAT solvers](https://github.com/pysathq/pysat)
are: in a matter of seconds they can "forge" a preimage that produces a
required hash value. It is also important to note that in such "fast" hash
functions like `komihash` the input message has a complete control over the
state variables.

Is 128-bit version of this hash function planned? Most probably, not. While
such version may be reasonable for data structure compatibility reasons, there
is no much practical sense to use 128-bit hashes at a local level: a reliable
64-bit hash allows one to have 4.2 billion diverse binary objects (e.g. files
in a file system, or entries in a hash-table) without collisions, on average.
On the other hand, on a worldwide scale, having 128-bit hashes is clearly not
enough considering the number of existing digital devices and the number of
diverse binary objects (e.g. files and records in databases) on each of them.

A similarly efficient streamed version of `komihash` is doable given a serious
interest in one is expressed.

An opinion on the "bulk" performance of "fast" hash functions: in most
practical situations, when processor's total memory bandwidth is limited to
e.g. 41 GB/s, a "bulk" single-threaded hashing performance on the order of
30 GB/s is excessive considering memory bandwidth has to be spread over
multiple cores. So, practically, such "fast" hash function, working on a
high-load 8-core server, rarely receives more than 8 GB/s of bandwidth.
Another factor worth mentioning is that a server rarely has more than 10 Gb/s
network connectivity, thus further reducing practical hashing performance of
incoming data. The same applies to disk system's throughput, if on-disk data
is not already in memory.

## Other ##

This function is named the way it is named is to honor Komi Republic
(located in Russia), native to the author.
