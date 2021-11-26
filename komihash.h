/**
 * komihash.h version 2.4
 *
 * The inclusion file for the "komihash" hash function.
 *
 * Description is available at https://github.com/avaneev/komihash
 *
 * License
 *
 * Copyright (c) 2021 Aleksey Vaneev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef KOMIHASH_INCLUDED
#define KOMIHASH_INCLUDED

#include <stdint.h>
#include <string.h>

// Macros that apply byte-swapping.

#if defined( __GNUC__ ) || defined( __clang__ ) || ( defined( __GNUC__ ) && defined( __ICL ))

#define KOMIHASH_BYTESW32( v ) __builtin_bswap32( v )
#define KOMIHASH_BYTESW64( v ) __builtin_bswap64( v )

#elif defined( _MSC_VER )

#define KOMIHASH_BYTESW32( v ) _byteswap_ulong( v )
#define KOMIHASH_BYTESW64( v ) _byteswap_uint64( v )

#else // defined( _MSC_VER )

#define KOMIHASH_BYTESW32( v ) ( \
	( v & 0xFF000000 ) >> 24 | \
	( v & 0x00FF0000 ) >> 8 | \
	( v & 0x0000FF00 ) << 8 | \
	( v & 0x000000FF ) << 24 )

#define KOMIHASH_BYTESW64( v ) ( \
	( v & 0xFF00000000000000 ) >> 56 | \
	( v & 0x00FF000000000000 ) >> 40 | \
	( v & 0x0000FF0000000000 ) >> 24 | \
	( v & 0x000000FF00000000 ) >> 8 | \
	( v & 0x00000000FF000000 ) << 8 | \
	( v & 0x0000000000FF0000 ) << 24 | \
	( v & 0x000000000000FF00 ) << 40 | \
	( v & 0x00000000000000FF ) << 56 )

#endif // defined( _MSC_VER )

// Macros that apply byte-swapping, used for endianness-correction.

#if defined( _WIN32 ) || defined( __LITTLE_ENDIAN__ ) || ( defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ )

	#define KOMIHASH_EC32( v ) ( v )
	#define KOMIHASH_EC64( v ) ( v )

#elif defined( __BIG_ENDIAN__ ) || ( defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ )

	#define KOMIHASH_EC32( v ) KOMIHASH_BYTESW32( v )
	#define KOMIHASH_EC64( v ) KOMIHASH_BYTESW64( v )

#else // endianness check

	#warning KOMIHASH: cannot determine endianness, assuming little-endian.
	#define KOMIHASH_EC32( v ) ( v )
	#define KOMIHASH_EC64( v ) ( v )

#endif // endianness check

// Likelihood macros that are used for manually-guided optimization
// (inefficient in clang).

#if defined( __GNUC__ )
	#define KOMIHASH_LIKELY( x )  __builtin_expect( x, 1 )
	#define KOMIHASH_UNLIKELY( x )  __builtin_expect( x, 0 )
#else // likelihood macros
	#define KOMIHASH_LIKELY( x ) ( x )
	#define KOMIHASH_UNLIKELY( x ) ( x )
#endif // likelihood macros

/**
 * An auxiliary function that returns an unsigned 32-bit value created out of
 * individual bytes in a buffer. This function is used to convert endianness
 * of supplied 32-bit unsigned values, and to avoid unaligned memory accesses.
 *
 * @param p 4-byte buffer. Alignment is unimportant.
 */

static inline uint32_t kh_lu32ec( const uint8_t* const p )
{
	uint32_t v;
	memcpy( &v, p, 4 );

	return( KOMIHASH_EC32( v ));
}

/**
 * An auxiliary function that returns an unsigned 64-bit value created out of
 * individual bytes in a buffer. This function is used to convert endianness
 * of supplied 64-bit unsigned values, and to avoid unaligned memory accesses.
 *
 * @param p 8-byte buffer. Alignment is unimportant.
 */

static inline uint64_t kh_lu64ec( const uint8_t* const p )
{
	uint64_t v;
	memcpy( &v, p, 8 );

	return( KOMIHASH_EC64( v ));
}

/**
 * Function loads 64-bit message word and pads it with the "final byte". This
 * function should only be called if there is less than 8 bytes left to read.
 *
 * @param Msg Message pointer, alignment is unimportant. Should be below or
 * equal to MsgEnd.
 * @param MsgEnd Message's end pointer.
 * @param fb Final byte used for padding.
 */

static inline uint64_t kh_lpu64ec( const uint8_t* Msg,
	const uint8_t* const MsgEnd, const uint64_t fb )
{
	const int l = (int) ( MsgEnd - Msg );
	uint64_t r = fb << ( l << 3 );

	if( l > 3 )
	{
		r |= (uint64_t) kh_lu32ec( Msg );
		Msg += 4;

		if( Msg < MsgEnd )
		{
			r |= (uint64_t) *Msg << 32;

			if( ++Msg < MsgEnd )
			{
				r |= (uint64_t) *Msg << 40;

				if( ++Msg < MsgEnd )
				{
					r |= (uint64_t) *Msg << 48;
				}
			}
		}

		return( r );
	}

	if( l != 0 )
	{
		r |= *Msg;

		if( ++Msg < MsgEnd )
		{
			r |= (uint64_t) *Msg << 8;

			if( ++Msg < MsgEnd )
			{
				r |= (uint64_t) *Msg << 16;
			}
		}
	}

	return( r );
}

#if defined( __SIZEOF_INT128__ )

	static inline void kh_m128( const uint64_t m1, const uint64_t m2,
		uint64_t* const ra, uint64_t* const rb )
	{
		const __uint128_t r = (__uint128_t) m1 * m2;

		*ra = (uint64_t) r;
		*rb = (uint64_t) ( r >> 64 );
	}

#elif defined( _MSC_VER ) && defined( _M_X64 )

	#include <intrin.h>

	static inline void kh_m128( const uint64_t m1, const uint64_t m2,
		uint64_t* const ra, uint64_t* const rb )
	{
		*ra = _umul128( m1, m2, rb );
	}

#else // defined( _MSC_VER )

	// _umul128() code for 32-bit systems from
	// https://github.com/simdjson/simdjson, licensed under Apache-2.0
	// license.

	static inline uint64_t kh__emulu( const uint32_t x, const uint32_t y )
	{
		return( x * (uint64_t) y );
	}

	static inline void kh_m128( const uint64_t ab, const uint64_t cd,
		uint64_t* const ra, uint64_t* const rb )
	{
		uint64_t ad = kh__emulu( (uint32_t) ( ab >> 32 ), (uint32_t) cd );
		uint64_t bd = kh__emulu( (uint32_t) ab, (uint32_t) cd );
		uint64_t adbc = ad + kh__emulu( (uint32_t) ab,
			(uint32_t) ( cd >> 32 ));

		uint64_t adbc_carry = !!( adbc < ad );
		uint64_t lo = bd + ( adbc << 32 );

		*rb = kh__emulu( (uint32_t) ( ab >> 32 ), (uint32_t) ( cd >> 32 )) +
			( adbc >> 32 ) + ( adbc_carry << 32 ) + !!( lo < bd );

		*ra = lo;
	}

#endif // defined( _MSC_VER )

/**
 * KOMIHASH hash function. Produces and returns 64-bit hash of the specified
 * message. Designed for 64-bit hash-table uses. Produces identical hashes on
 * both big- and little-endian systems.
 *
 * @param Msg The message to produce hash from. The alignment of the message
 * is unimportant.
 * @param MsgLen Message's length, in bytes.
 * @param UseSeed Optional value, to use instead of the default seed. To use
 * the default seed, set to 0. The UseSeed value can have any bit length and
 * statistical quality, and is used only as an additional entropy source. May
 * need endianness-correction if this value is shared between big- and
 * little-endian systems.
 */

static inline uint64_t komihash( const uint8_t* Msg, const size_t MsgLen,
	const uint64_t UseSeed )
{
	// Seeds are initialized to the first mantissa bits of PI.

	uint64_t Seed1 = 0x243F6A8885A308D3 ^ ( UseSeed & 0x5555555555555555 );
	uint64_t Seed5 = 0x452821E638D01377 ^ ( UseSeed & 0xAAAAAAAAAAAAAAAA );
	uint64_t r1a, r1b, r2a, r2b;

	kh_m128( Seed1, Seed5, &r2a, &r2b ); // Required for PerlinNoise.
	Seed5 += r2b;
	Seed1 = Seed5 ^ r2a;

	const uint8_t* const MsgEnd = Msg + MsgLen;

	if( KOMIHASH_LIKELY( MsgLen < 16 ))
	{
		r2a = Seed1;
		r2b = Seed5;

		if( KOMIHASH_LIKELY( MsgLen > 7 ))
		{
			r2a ^= kh_lu64ec( Msg );
			r2b ^= kh_lpu64ec( Msg + 8, MsgEnd, 1 << ( MsgEnd[ -1 ] >> 7 ));
		}
		else
		if( KOMIHASH_LIKELY( MsgLen != 0 ))
		{
			r2a ^= kh_lpu64ec( Msg, MsgEnd, 1 << ( MsgEnd[ -1 ] >> 7 ));
		}

		kh_m128( r2a, r2b, &r1a, &r1b );
		Seed5 += r1b;
		Seed1 = Seed5 ^ r1a;

		kh_m128( Seed1, Seed5, &r2a, &r2b );
		Seed5 += r2b;
		Seed1 = Seed5 ^ r2a;

		return( Seed1 );
	}

	const uint64_t fb = 1 << ( MsgEnd[ -1 ] >> 7 );
	uint64_t Seed2 = 0x13198A2E03707344 ^ Seed1;

	if( KOMIHASH_UNLIKELY( MsgLen > 63 ))
	{
		uint64_t Seed3 = 0xA4093822299F31D0 ^ Seed1;
		uint64_t Seed4 = 0x082EFA98EC4E6C89 ^ Seed1;
		uint64_t Seed6 = 0xBE5466CF34E90C6C ^ Seed5;
		uint64_t Seed7 = 0xC0AC29B7C97C50DD ^ Seed5;
		uint64_t Seed8 = 0x3F84D5B5B5470917 ^ Seed5;
		uint64_t r3a, r3b, r4a, r4b;

		do
		{
			kh_m128( Seed1 ^ kh_lu64ec( Msg ),
				Seed5 ^ kh_lu64ec( Msg + 8 ), &r1a, &r1b );

			kh_m128( Seed2 ^ kh_lu64ec( Msg + 16 ),
				Seed6 ^ kh_lu64ec( Msg + 24 ), &r2a, &r2b );

			kh_m128( Seed3 ^ kh_lu64ec( Msg + 32 ),
				Seed7 ^ kh_lu64ec( Msg + 40 ), &r3a, &r3b );

			kh_m128( Seed4 ^ kh_lu64ec( Msg + 48 ),
				Seed8 ^ kh_lu64ec( Msg + 56 ), &r4a, &r4b );

			Msg += 8 * 8;

			Seed5 += r1b;
			Seed6 += r2b;
			Seed7 += r3b;
			Seed8 += r4b;
			Seed2 = Seed5 ^ r2a;
			Seed3 = Seed6 ^ r3a;
			Seed4 = Seed7 ^ r4a;
			Seed1 = Seed8 ^ r1a;

		} while( KOMIHASH_LIKELY( Msg < MsgEnd - 63 ));

		kh_m128( Seed2, Seed6, &r2a, &r2b );
		kh_m128( Seed3, Seed7, &r3a, &r3b );
		kh_m128( Seed4, Seed8, &r4a, &r4b );

		Seed6 += r2b;
		Seed7 += r3b;
		Seed8 += r4b;
		Seed2 = Seed6 ^ r2a;
		Seed3 = Seed7 ^ r3a;
		Seed4 = Seed8 ^ r4a;

		Seed2 ^= Seed3 ^ Seed4;
	}

	while( KOMIHASH_LIKELY( Msg < MsgEnd - 15 ))
	{
		kh_m128( Seed1 ^ kh_lu64ec( Msg ),
			Seed5 ^ kh_lu64ec( Msg + 8 ), &r1a, &r1b );

		Msg += 8 * 2;

		Seed5 += r1b;
		Seed1 = Seed5 ^ r1a;
	}

	if( KOMIHASH_UNLIKELY( Msg < MsgEnd - 7 ))
	{
		r2a = Seed1 ^ kh_lu64ec( Msg );
		r2b = Seed5 ^ kh_lpu64ec( Msg + 8, MsgEnd, fb );
	}
	else
	{
		r2a = Seed1 ^ kh_lpu64ec( Msg, MsgEnd, fb );
		r2b = Seed5;
	}

	kh_m128( r2a, r2b, &r1a, &r1b );
	Seed5 += r1b;
	Seed1 = Seed5 ^ r1a;

	kh_m128( Seed1, Seed5, &r2a, &r2b );
	Seed5 += r2b;
	Seed1 = Seed5 ^ r2a;

	return( Seed1 ^ Seed2 );
}

#endif // KOMIHASH_INCLUDED
