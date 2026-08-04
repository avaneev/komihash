/**
 * @file komihash.h
 *
 * @version 5.34
 *
 * @brief The header file for the `komihash` 64-bit hash function,
 * the `komirand` 64-bit PRNG, and the streamed `komihash` implementation.
 *
 * The source code is written in ISO C99 and automatically provides full C++
 * compatibility when compiled with a C++ compiler.
 *
 * The `komihash` function is named in honor of the Komi Republic (located in
 * Russia), the author's native region.
 *
 * The description is available at https://github.com/avaneev/komihash
 *
 * Email: aleksey.vaneev@gmail.com or info@voxengo.com
 *
 * LICENSE:
 *
 * Copyright (c) 2021-2026 Aleksey Vaneev
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

#define KOMIHASH_VER_STR "5.34" ///< KOMIHASH source code version string.

/**
 * @def KOMIHASH_NS_CUSTOM
 * @brief If this macro is defined externally, all symbols will be placed
 * in the namespace specified by the macro, and they will not be placed in the
 * global namespace. WARNING: If the value defined by the macro is empty, the
 * symbols will be placed in the global namespace anyway.
 */

/**
 * @def KOMIHASH_U64_C( x )
 * @brief Macro that defines a numeric constant as an unsigned 64-bit value.
 *
 * @param x Value.
 */

/**
 * @def KOMIHASH_NOEXC
 * @brief Macro that defines the `noexcept` function specifier in a C++
 * environment.
 */

/**
 * @def KOMIHASH_NS
 * @brief Macro that defines the actual implementation namespace in a C++
 * environment. Relevant symbols are also placed in the global namespace
 * (if @ref KOMIHASH_NS_CUSTOM is undefined).
 */

#if defined( __cplusplus )

	#include <cstring> // This header defines std::size_t.

	#if __cplusplus >= 201103L

		#include <cstdint>

		#define KOMIHASH_U64_C( x ) UINT64_C( x )
		#define KOMIHASH_NOEXC noexcept

	#else // __cplusplus >= 201103L

		#include <stdint.h> // A C99 fallback, as C++98 has no cstdint header.

		#define KOMIHASH_U64_C( x ) (uint64_t) x
		#define KOMIHASH_NOEXC throw()

	#endif // __cplusplus >= 201103L

	#if defined( KOMIHASH_NS_CUSTOM )
		#define KOMIHASH_NS KOMIHASH_NS_CUSTOM
	#else // defined( KOMIHASH_NS_CUSTOM )
		#define KOMIHASH_NS komihash_impl
	#endif // defined( KOMIHASH_NS_CUSTOM )

#else // defined( __cplusplus )

	#include <string.h> // This header defines size_t.
	#include <stdint.h>

	#define KOMIHASH_U64_C( x ) (uint64_t) x
	#define KOMIHASH_NOEXC

#endif // defined( __cplusplus )

/**
 * @{
 * @brief Unsigned 64-bit constant that defines the initial state of the
 * hash function (the first few digits of the fractional part of pi).
 */

#define KOMIHASH_IVAL1 KOMIHASH_U64_C( 0x243F6A8885A308D3 )
#define KOMIHASH_IVAL2 KOMIHASH_U64_C( 0x13198A2E03707344 )
#define KOMIHASH_IVAL3 KOMIHASH_U64_C( 0xA4093822299F31D0 )
#define KOMIHASH_IVAL4 KOMIHASH_U64_C( 0x082EFA98EC4E6C89 )
#define KOMIHASH_IVAL5 KOMIHASH_U64_C( 0x452821E638D01377 )
#define KOMIHASH_IVAL6 KOMIHASH_U64_C( 0xBE5466CF34E90C6C )
#define KOMIHASH_IVAL7 KOMIHASH_U64_C( 0xC0AC29B7C97C50DD )
#define KOMIHASH_IVAL8 KOMIHASH_U64_C( 0x3F84D5B5B5470917 )

/** @} */

/**
 * @def KOMIHASH_VAL01
 * @brief Unsigned 64-bit constant formed by repeating the `01` bit pair.
 */

#define KOMIHASH_VAL01 KOMIHASH_U64_C( 0x5555555555555555 )

/**
 * @def KOMIHASH_VAL10
 * @brief Unsigned 64-bit constant formed by repeating the `10` bit pair.
 */

#define KOMIHASH_VAL10 KOMIHASH_U64_C( 0xAAAAAAAAAAAAAAAA )

/**
 * @def KOMIHASH_DEFSEED1
 * @brief Initial `Seed1` value for the default (0) seed.
 */

#define KOMIHASH_DEFSEED1 KOMIHASH_U64_C( 0x01D2EE0AE40A48DC )

/**
 * @def KOMIHASH_DEFSEED5
 * @brief Initial `Seed5` value for the default (0) seed.
 */

#define KOMIHASH_DEFSEED5 KOMIHASH_U64_C( 0x4EF2E8526FEA8BC9 )

/**
 * @def KOMIHASH_ENDIAN_DEFS
 * @brief This macro is defined if the KOMIHASH_LITTLE_ENDIAN macro is not
 * defined externally.
 */

/**
 * @def KOMIHASH_LITTLE_ENDIAN
 * @brief Endianness definition macro that can be used as a logical constant.
 *
 * When C++20 is available, this macro is defined as 0, and the actual
 * endianness is determined at compile time via std::endian::native.
 * This means that a value of 0 for this macro indicates "big-endian" or
 * "unknown".
 *
 * Note that for exotic platforms, you may need to include
 * a compiler-dependent `endian.h` header before including `komihash.h` to
 * avoid using a potentially slower fallback.
 *
 * This macro can be externally defined as 1 to reduce overhead if endianness
 * correction and hash value portability are unnecessary.
 */

/**
 * @def KOMIHASH_COND_EC( vl, vb )
 * @brief Macro that emits either `vl` or `vb`, depending on the platform's
 * endianness.
 */

#if !defined( KOMIHASH_LITTLE_ENDIAN )

	#define KOMIHASH_ENDIAN_DEFS

	#if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && \
			__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || \
		( defined( __BYTE_ORDER ) && defined( __LITTLE_ENDIAN ) && \
			__BYTE_ORDER == __LITTLE_ENDIAN ) || \
		( defined( _BYTE_ORDER ) && defined( _LITTLE_ENDIAN ) && \
			_BYTE_ORDER == _LITTLE_ENDIAN ) || \
		defined( __LITTLE_ENDIAN__ ) || defined( __little_endian__ ) || \
		( !defined( __BYTE_ORDER ) && !defined( __BIG_ENDIAN ) && \
			defined( __LITTLE_ENDIAN )) || \
		( !defined( _BYTE_ORDER ) && !defined( _BIG_ENDIAN ) && \
			defined( _LITTLE_ENDIAN )) || \
		defined( _WIN32 ) || (( defined( i386 ) || defined( __i386 ) || \
		defined( __i386__ )) && !defined( __VOS__ )) || defined( _X86_ ) || \
		defined( _M_IX86 ) || defined( _M_AMD64 ) || defined( _M_ARM ) || \
		defined( __x86_64 ) || defined( __x86_64__ ) || \
		defined( __amd64 ) || defined( __amd64__ )

		#define KOMIHASH_LITTLE_ENDIAN 1

	#elif ( defined( __BYTE_ORDER__ ) && defined( __ORDER_BIG_ENDIAN__ ) && \
			__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ) || \
		( defined( __BYTE_ORDER ) && defined( __BIG_ENDIAN ) && \
			__BYTE_ORDER == __BIG_ENDIAN ) || \
		( defined( _BYTE_ORDER ) && defined( _BIG_ENDIAN ) && \
			_BYTE_ORDER == _BIG_ENDIAN ) || \
		defined( __BIG_ENDIAN__ ) || defined( __big_endian__ ) || \
		( !defined( __BYTE_ORDER ) && !defined( __LITTLE_ENDIAN ) && \
			defined( __BIG_ENDIAN )) || \
		( !defined( _BYTE_ORDER ) && !defined( _LITTLE_ENDIAN ) && \
			defined( _BIG_ENDIAN )) || \
		defined( __SYSC_ZARCH__ ) || defined( __zarch__ ) || \
		defined( __s390__ ) || defined( __s390x__ ) || \
		defined( __sparc ) || defined( __sparc__ ) || defined( __VOS__ )

		#define KOMIHASH_LITTLE_ENDIAN 0
		#define KOMIHASH_COND_EC( vl, vb ) ( vb )

	#elif defined( __cplusplus ) && __cplusplus >= 202002L

		#include <bit>

		#define KOMIHASH_LITTLE_ENDIAN 0
		#define KOMIHASH_COND_EC( vl, vb ) ( std::endian::native == \
			std::endian::little ? vl : vb )

	#else // defined( __cplusplus )

		#define KOMIHASH_LITTLE_ENDIAN 0
		#define KOMIHASH_COND_EC( vl, vb ) ( kh_is_little_endian() ? vl : vb )

	#endif // defined( __cplusplus )

#else // !defined( KOMIHASH_LITTLE_ENDIAN )

	#if !KOMIHASH_LITTLE_ENDIAN
		#error The KOMIHASH_LITTLE_ENDIAN macro must evaluate to true.
	#endif // !KOMIHASH_LITTLE_ENDIAN

#endif // !defined( KOMIHASH_LITTLE_ENDIAN )

/**
 * @def KOMIHASH_ICC_GCC
 * @brief Macro that denotes the use of the ICC classic compiler with
 * GCC-style built-in functions.
 */

#if defined( __INTEL_COMPILER ) && __INTEL_COMPILER >= 1300 && \
	!defined( _MSC_VER )

	#define KOMIHASH_ICC_GCC

#endif // ICC check

/**
 * @def KOMIHASH_GCC_BUILTINS
 * @brief Macro that denotes the availability of GCC-style built-in functions.
 */

#if defined( __GNUC__ ) || defined( __clang__ ) || \
	defined( __IBMC__ ) || defined( __IBMCPP__ ) || \
	defined( __COMPCERT__ ) || defined( KOMIHASH_ICC_GCC )

	#define KOMIHASH_GCC_BUILTINS

#endif // GCC built-ins check

/**
 * @def KOMIHASH_BMI2
 * @brief Macro that denotes the availability of the `mulx` intrinsic
 * (MSVC-compatible compilers only).
 */

#if defined( _MSC_VER )
	#if defined( __BMI2__ ) || ( !defined( KOMIHASH_GCC_BUILTINS ) && \
		defined( _M_AMD64 ) && defined( __AVX2__ ) && \
		( defined( __INTEL_COMPILER ) || _MSC_VER >= 1900 ))

		#include <immintrin.h>
		#define KOMIHASH_BMI2

	#else // BMI2

		#include <intrin.h>

	#endif // BMI2
#endif // defined( _MSC_VER )

/**
 * @def KOMIHASH_EC32( v )
 * @brief Macro that applies 32-bit byte-swapping for endianness correction.
 *
 * On big-endian platforms, this macro is left undefined when an unknown
 * compiler is used.
 *
 * @param v Value to byte-swap.
 */

/**
 * @def KOMIHASH_EC64( v )
 * @brief Macro that applies 64-bit byte-swapping for endianness correction.
 *
 * On big-endian platforms, this macro is left undefined when an unknown
 * compiler is used.
 *
 * @param v Value to byte-swap.
 */

#if KOMIHASH_LITTLE_ENDIAN

	#define KOMIHASH_EC32( v ) ( v )
	#define KOMIHASH_EC64( v ) ( v )

#else // KOMIHASH_LITTLE_ENDIAN

	#if defined( KOMIHASH_GCC_BUILTINS )

		#define KOMIHASH_EC32( v ) KOMIHASH_COND_EC( v, __builtin_bswap32( v ))
		#define KOMIHASH_EC64( v ) KOMIHASH_COND_EC( v, __builtin_bswap64( v ))

	#elif defined( _MSC_VER )

		#if defined( __cplusplus )
			#include <cstdlib>
		#else // defined( __cplusplus )
			#include <stdlib.h>
		#endif // defined( __cplusplus )

		#define KOMIHASH_EC32( v ) KOMIHASH_COND_EC( v, _byteswap_ulong( v ))
		#define KOMIHASH_EC64( v ) KOMIHASH_COND_EC( v, _byteswap_uint64( v ))

	#elif defined( __cplusplus ) && __cplusplus >= 202302L

		#include <bit>

		#define KOMIHASH_EC32( v ) KOMIHASH_COND_EC( v, std::byteswap( v ))
		#define KOMIHASH_EC64( v ) KOMIHASH_COND_EC( v, std::byteswap( v ))

	#endif // defined( __cplusplus )

#endif // KOMIHASH_LITTLE_ENDIAN

/**
 * @def KOMIHASH_LIKELY( x )
 * @brief Macro that indicates an expression is likely to be true and is used
 * for manual micro-optimization.
 *
 * @param x Expression that is likely to evaluate to `true`.
 */

/**
 * @def KOMIHASH_UNLIKELY( x )
 * @brief Macro that indicates an expression is unlikely to be true and is
 * used for manual micro-optimization.
 *
 * @param x Expression that is unlikely to evaluate to `true`.
 */

/**
 * @def KOMIHASH_LIKELY_DO
 * @brief Macro that applies the C++20 `[[likely]]` attribute to do-while
 * loops.
 */

/**
 * @def KOMIHASH_LIKELY_DO_EXPR( x )
 * @brief Macro that indicates a likely condition and is used for manual
 * micro-optimization of do-while loops.
 *
 * @param x Expression that is likely to evaluate to `true`.
 */

#if defined( KOMIHASH_GCC_BUILTINS )

	#define KOMIHASH_LIKELY( x ) ( __builtin_expect( x, 1 ))
	#define KOMIHASH_UNLIKELY( x ) ( __builtin_expect( x, 0 ))

#elif defined( __cplusplus ) && __cplusplus >= 202002L

	#define KOMIHASH_LIKELY( x ) ( x ) [[likely]]
	#define KOMIHASH_UNLIKELY( x ) ( x ) [[unlikely]]
	#define KOMIHASH_LIKELY_DO [[likely]]
	#define KOMIHASH_LIKELY_DO_EXPR( x ) ( x )

#else // defined( __cplusplus )

	#define KOMIHASH_LIKELY( x ) ( x )
	#define KOMIHASH_UNLIKELY( x ) ( x )

#endif // defined( __cplusplus )

#if !defined( KOMIHASH_LIKELY_DO )
	#define KOMIHASH_LIKELY_DO
	#define KOMIHASH_LIKELY_DO_EXPR( x ) KOMIHASH_LIKELY( x )
#endif // !defined( KOMIHASH_LIKELY_DO )

/**
 * @def KOMIHASH_PREFETCH( a )
 * @brief Macro that prefetches data from the given memory address into the
 * CPU cache.
 *
 * The level-3 temporal locality hint is used because the data may later be
 * used for collision resolution or for a subsequent disk write.
 *
 * @param a Prefetch address.
 */

#if defined( KOMIHASH_GCC_BUILTINS ) && !defined( __COMPCERT__ )

	#define KOMIHASH_PREFETCH( a ) __builtin_prefetch( a, 0, 3 )

#elif defined( _MSC_VER ) && defined( _M_AMD64 ) && \
	!defined( __INTEL_COMPILER )

	#include <intrin.h>

	#define KOMIHASH_PREFETCH( a ) _mm_prefetch( (const char*) ( a ), \
		_MM_HINT_T0 )

#else // defined( _MSC_VER )

	#define KOMIHASH_PREFETCH( a ) (void) 0

#endif // defined( _MSC_VER )

/**
 * @def KOMIHASH_STATIC
 * @brief Macro that defines a function as "static".
 */

#if defined( KOMIHASH_GCC_BUILTINS )

	#define KOMIHASH_STATIC static __attribute__((unused))

#elif ( defined( __cplusplus ) && __cplusplus >= 201703L ) || \
	( defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 202311L )

	#define KOMIHASH_STATIC [[maybe_unused]] static

#else // defined( __cplusplus )

	#define KOMIHASH_STATIC static

#endif // defined( __cplusplus )

/**
 * @def KOMIHASH_INLINE
 * @brief Macro that defines a function as an inline function, at the
 * compiler's discretion.
 */

#define KOMIHASH_INLINE KOMIHASH_STATIC inline

/**
 * @def KOMIHASH_INLINE_F
 * @brief Macro that forces function inlining.
 */

#if defined( KOMIHASH_GCC_BUILTINS )

	#define KOMIHASH_INLINE_F KOMIHASH_INLINE __attribute__((always_inline))

#elif defined( _MSC_VER )

	#define KOMIHASH_INLINE_F KOMIHASH_STATIC __forceinline

#else // defined( _MSC_VER )

	#define KOMIHASH_INLINE_F KOMIHASH_INLINE

#endif // defined( _MSC_VER )

#if defined( KOMIHASH_NS )

namespace KOMIHASH_NS {

using std::memcpy;
using std::size_t;

#if __cplusplus >= 201103L

	using uint8_t = unsigned char; ///< For C++ type aliasing compliance.
	using std::uint32_t;
	using std::uint64_t;

#endif // __cplusplus >= 201103L

#endif // defined( KOMIHASH_NS )

/**
 * @brief Determines the platform's endianness at runtime.
 *
 * Note that modern compilers evaluate this function at compile time,
 * resulting in branch elimination.
 *
 * @return 1 if the platform is little-endian, 0 otherwise.
 */

KOMIHASH_INLINE_F int kh_is_little_endian(void) KOMIHASH_NOEXC
{
	static const uint32_t val = 0x04030201;

	unsigned char lsb;
	memcpy( &lsb, &val, 1 );

	return( lsb == 1 );
}

/**
 * @{
 * @brief Loads an unsigned value of the corresponding bit size, with
 * endianness correction.
 *
 * This is an auxiliary function that returns an unsigned value created from a
 * sequence of bytes in memory. This function is used to correct the
 * endianness of in-memory unsigned values and to avoid unaligned memory
 * accesses.
 *
 * @param p Pointer to bytes in memory. Alignment is unimportant.
 * @return The endianness-corrected value from memory (as `uint64_t`).
 */

KOMIHASH_INLINE_F uint64_t kh_lu32ec( const uint8_t* const p ) KOMIHASH_NOEXC
{
#if defined( KOMIHASH_EC32 )

	uint32_t v;
	memcpy( &v, p, 4 );

	return( KOMIHASH_EC32( v ));

#else // defined( KOMIHASH_EC32 )

	return( (uint64_t) p[ 0 ] | (uint64_t) p[ 1 ] << 8 |
		(uint64_t) p[ 2 ] << 16 | (uint64_t) p[ 3 ] << 24 );

#endif // defined( KOMIHASH_EC32 )
}

KOMIHASH_INLINE_F uint64_t kh_lu64ec( const uint8_t* const p ) KOMIHASH_NOEXC
{
#if defined( KOMIHASH_EC64 )

	uint64_t v;
	memcpy( &v, p, 8 );

	return( KOMIHASH_EC64( v ));

#else // defined( KOMIHASH_EC64 )

	return( kh_lu32ec( p ) | kh_lu32ec( p + 4 ) << 32 );

#endif // defined( KOMIHASH_EC64 )
}

/** @} */

/**
 * @def KOMIHASH_M128_IMPL
 * @brief Auxiliary macro for the kh_m128() implementation.
 */

/**
 * @def KOMIHASH_EMULU( u, v )
 * @brief Auxiliary macro for the `__emulu()` intrinsic.
 *
 * @param u The first multiplier.
 * @param v The second multiplier.
 */

#if defined( KOMIHASH_BMI2 )

	#define KOMIHASH_M128_IMPL \
		unsigned long long rh; \
		*rl = _mulx_u64( u, v, &rh );

#elif defined( _MSC_VER ) && \
	( defined( _M_ARM64 ) || defined( _M_ARM64EC ) || \
	( defined( __INTEL_COMPILER ) && defined( _M_AMD64 )))

	#define KOMIHASH_M128_IMPL \
		const uint64_t rh = __umulh( u, v ); \
		*rl = u * v;

#elif defined( _MSC_VER ) && ( defined( _M_AMD64 ) || defined( _M_IA64 ))

	#pragma intrinsic(_umul128)

	#define KOMIHASH_M128_IMPL \
		uint64_t rh; \
		*rl = _umul128( u, v, &rh );

#elif defined( __SIZEOF_INT128__ ) || \
	( defined( KOMIHASH_ICC_GCC ) && defined( __x86_64__ ))

	#define KOMIHASH_M128_IMPL \
		__uint128_t r = u; \
		r *= v; \
		const uint64_t rh = (uint64_t) ( r >> 64 ); \
		*rl = (uint64_t) r;

#elif ( defined( __IBMC__ ) || defined( __IBMCPP__ )) && defined( __LP64__ )

	#define KOMIHASH_M128_IMPL \
		const uint64_t rh = __mulhdu( u, v ); \
		*rl = u * v;

#else // defined( __IBMC__ )

	#if defined( _MSC_VER ) && !defined( __INTEL_COMPILER ) && \
		!defined( _M_ARM )

		#pragma intrinsic(__emulu)

		#define KOMIHASH_EMULU( u, v ) __emulu( u, v )

	#else // __emulu

		#define KOMIHASH_EMULU( u, v ) ( (uint64_t) ( u ) * ( v ))

	#endif // __emulu

#endif // defined( __IBMC__ )

/**
 * @brief 64-bit by 64-bit unsigned multiplication with result accumulation.
 *
 * @param u The first multiplier.
 * @param v The second multiplier.
 * @param[out] rl The lower half of the 128-bit result.
 * @param[in,out] rha The accumulator that receives the higher half of the
 * 128-bit result.
 */

#if defined( KOMIHASH_M128_IMPL )
KOMIHASH_INLINE_F
#else // defined( KOMIHASH_M128_IMPL )
KOMIHASH_INLINE
#endif // defined( KOMIHASH_M128_IMPL )

void kh_m128( const uint64_t u, const uint64_t v,
	uint64_t* const rl, uint64_t* const rha ) KOMIHASH_NOEXC
{
#if defined( KOMIHASH_M128_IMPL )

	KOMIHASH_M128_IMPL

#else // defined( KOMIHASH_M128_IMPL )

	// This is the `_umul128()` code for 32-bit systems, adapted from
	// Hacker's Delight by Henry S. Warren, Jr.

	*rl = u * v;

	const uint32_t u0 = (uint32_t) u;
	const uint32_t v0 = (uint32_t) v;
	const uint64_t w0 = KOMIHASH_EMULU( u0, v0 );
	const uint32_t u1 = (uint32_t) ( u >> 32 );
	const uint32_t v1 = (uint32_t) ( v >> 32 );
	const uint64_t t = KOMIHASH_EMULU( u1, v0 ) + (uint32_t) ( w0 >> 32 );
	const uint64_t w1 = KOMIHASH_EMULU( u0, v1 ) + (uint32_t) t;

	const uint64_t rh = KOMIHASH_EMULU( u1, v1 ) + (uint32_t) ( w1 >> 32 ) +
		(uint32_t) ( t >> 32 );

#endif // defined( KOMIHASH_M128_IMPL )

	*rha += rh;
}

/**
 * @def KOMIHASH_HASHROUND()
 * @brief Macro for a common hashing round without input data.
 */

#define KOMIHASH_HASHROUND() \
	kh_m128( Seed1, Seed5, &Seed1, &Seed5 ); \
	Seed1 ^= Seed5

/**
 * @def KOMIHASH_HASH16( m )
 * @brief Macro for a common hashing round with a 16-byte input.
 *
 * @param m Message pointer; alignment is unimportant.
 */

#define KOMIHASH_HASH16( m ) \
	kh_m128( kh_lu64ec( m ) ^ Seed1, \
		kh_lu64ec( m + 8 ) ^ Seed5, &Seed1, &Seed5 ); \
	Seed1 ^= Seed5

/**
 * @def KOMIHASH_HASHFIN()
 * @brief Macro for a common hashing finalization round.
 *
 * The final input to the hash function is expected to reside in the temporary
 * variables `r1h` and `r2h`. The macro includes the return statement.
 */

#define KOMIHASH_HASHFIN() \
	kh_m128( r1h, r2h, &Seed1, &Seed5 ); \
	Seed1 ^= Seed5; \
	KOMIHASH_HASHROUND(); \
	return( Seed1 )

/**
 * @def KOMIHASH_HASHLOOP64()
 * @brief Macro for a common 64-byte full-performance hashing loop.
 *
 * This macro expects `Msg` to point to the data and `MsgLen` to be greater
 * than 63, and requires `Seed1` through `Seed8` to be initialized.
 *
 * The "shifting" arrangement of the `Seed1` to `Seed4` XOR operations (below)
 * does not increase the PRNG period of individual `SeedN` values, but reduces
 * the chance of occasional synchronization between PRNG lanes. In practice,
 * `Seed1` to `Seed4` together become a single "fused" 256-bit PRNG value,
 * which gives the state a total PRNG period of 2^66.
 */

#define KOMIHASH_HASHLOOP64() \
	do KOMIHASH_LIKELY_DO \
	{ \
		kh_m128( kh_lu64ec( Msg ) ^ Seed1, \
			kh_lu64ec( Msg + 32 ) ^ Seed5, &Seed1, &Seed5 ); \
	\
		kh_m128( kh_lu64ec( Msg + 8 ) ^ Seed2, \
			kh_lu64ec( Msg + 40 ) ^ Seed6, &Seed2, &Seed6 ); \
	\
		kh_m128( kh_lu64ec( Msg + 16 ) ^ Seed3, \
			kh_lu64ec( Msg + 48 ) ^ Seed7, &Seed3, &Seed7 ); \
	\
		kh_m128( kh_lu64ec( Msg + 24 ) ^ Seed4, \
			kh_lu64ec( Msg + 56 ) ^ Seed8, &Seed4, &Seed8 ); \
	\
		Msg += 64; \
		MsgLen -= 64; \
	\
		KOMIHASH_PREFETCH( Msg ); \
	\
		Seed4 ^= Seed7; \
		Seed1 ^= Seed8; \
		Seed2 ^= Seed5; \
		Seed3 ^= Seed6; \
	\
	} while KOMIHASH_LIKELY_DO_EXPR( MsgLen > 63 )

/**
 * @brief The hashing epilogue function (for internal use).
 *
 * @param Msg Pointer to the remaining part of the message. It is assumed that
 * the original message is "long" so that `Msg + MsgLen - 8` does not point
 * beyond the original message.
 * @param MsgLen Length of the remaining part; can be zero.
 * @param Seed1 The latest `Seed1` value.
 * @param Seed5 The latest `Seed5` value.
 * @return The 64-bit hash value.
 */

KOMIHASH_INLINE_F uint64_t komihash_epi( const uint8_t* Msg, size_t MsgLen,
	uint64_t Seed1, uint64_t Seed5 ) KOMIHASH_NOEXC
{
	uint64_t r1h, r2h;

	if( MsgLen > 31 )
	{
		KOMIHASH_HASH16( Msg );
		KOMIHASH_HASH16( Msg + 16 );

		MsgLen -= 32;
		Msg += 32;
	}

	if( MsgLen > 15 )
	{
		KOMIHASH_HASH16( Msg );

		MsgLen -= 16;
		Msg += 16;
	}

	size_t ml8 = MsgLen * 8;

	if( MsgLen < 8 )
	{
		ml8 ^= 56;
		r1h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
		r2h = Seed5;
		r1h = ( r1h >> ml8 ) ^ Seed1;
	}
	else
	{
		r2h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
		ml8 ^= 120;
		r1h = kh_lu64ec( Msg ) ^ Seed1;
		r2h = ( r2h >> ml8 ) ^ Seed5;
	}

	KOMIHASH_HASHFIN();
}

/**
 * @brief Implementation of the KOMIHASH 64-bit hash function.
 *
 * This function produces and returns a 64-bit hash value of the specified
 * message, string, or binary data block. It is designed for hash tables and
 * hash maps and can also be used to generate checksums. It produces identical
 * hashes across big- and little-endian systems.
 *
 * @param Msg The message to hash.
 * @param MsgLen The message length, in bytes; can be zero.
 * @param Seed1 The initial `Seed1` value.
 * @param Seed5 The initial `Seed5` value.
 * @return The 64-bit hash of the input data.
 */

KOMIHASH_INLINE uint64_t komihash_inner( const uint8_t* Msg, size_t MsgLen,
	uint64_t Seed1, uint64_t Seed5 ) KOMIHASH_NOEXC
{
	uint64_t r1h, r2h;

	if KOMIHASH_LIKELY( MsgLen < 16 )
	{
		r1h = Seed1;
		r2h = Seed5;

		if( MsgLen > 7 )
		{
			// The following XOR operations are equivalent to mixing the
			// message with a cryptographic one-time pad (bitwise addition
			// modulo 2). The message's statistics and distribution are thus
			// unimportant.

			r1h ^= kh_lu64ec( Msg );

			size_t ml8 = MsgLen * 8;

			if( MsgLen < 12 )
			{
				ml8 ^= 88;
				const uint64_t m = (uint64_t) Msg[ MsgLen - 3 ] |
					(uint64_t) Msg[ MsgLen - 1 ] << 16 | (uint64_t) 1 << 24 |
					(uint64_t) Msg[ MsgLen - 2 ] << 8;

				r2h ^= m >> ml8;
			}
			else
			{
				const size_t mhs = 128 - ml8;
				const uint64_t mh = ( kh_lu32ec( Msg + MsgLen - 4 ) |
					(uint64_t) 1 << 32 ) >> mhs;

				const uint64_t ml = kh_lu32ec( Msg + 8 );

				r2h ^= mh << 32 | ml;
			}
		}
		else
		if KOMIHASH_LIKELY( MsgLen != 0 )
		{
			const size_t ml8 = MsgLen * 8;

			if( MsgLen < 4 )
			{
				r1h ^= (uint64_t) Msg[ 0 ];
				r1h ^= (uint64_t) 1 << ml8;

				if( MsgLen != 1 )
				{
					r1h ^= (uint64_t) Msg[ 1 ] << 8;

					if( MsgLen != 2 )
					{
						r1h ^= (uint64_t) Msg[ 2 ] << 16;
					}
				}
			}
			else
			{
				const size_t mhs = 64 - ml8;
				const uint64_t mh = ( kh_lu32ec( Msg + MsgLen - 4 ) |
					(uint64_t) 1 << 32 ) >> mhs;

				const uint64_t ml = kh_lu32ec( Msg );

				r1h ^= mh << 32 | ml;
			}
		}
	}
	else
	{
		if KOMIHASH_UNLIKELY( MsgLen > 31 )
		{
			goto longmsg;
		}

		KOMIHASH_HASH16( Msg );

		size_t ml8 = MsgLen * 8;

		if( MsgLen < 24 )
		{
			ml8 ^= 184;
			r1h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
			r2h = Seed5;
			r1h = ( r1h >> ml8 ) ^ Seed1;

			KOMIHASH_HASHFIN();
		}
		else
		{
			r2h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
			ml8 ^= 248;
			r1h = kh_lu64ec( Msg + 16 ) ^ Seed1;
			r2h = ( r2h >> ml8 ) ^ Seed5;
		}
	}

	KOMIHASH_HASHFIN();

longmsg:
	if KOMIHASH_LIKELY( MsgLen > 63 )
	{
		uint64_t Seed2 = KOMIHASH_IVAL2 ^ Seed1;
		uint64_t Seed3 = KOMIHASH_IVAL3 ^ Seed1;
		uint64_t Seed4 = KOMIHASH_IVAL4 ^ Seed1;
		uint64_t Seed6 = KOMIHASH_IVAL6 ^ Seed5;
		uint64_t Seed7 = KOMIHASH_IVAL7 ^ Seed5;
		uint64_t Seed8 = KOMIHASH_IVAL8 ^ Seed5;

		KOMIHASH_HASHLOOP64();

		Seed5 ^= Seed6 ^ Seed7 ^ Seed8;
		Seed1 ^= Seed2 ^ Seed3 ^ Seed4;
	}

	return( komihash_epi( Msg, MsgLen, Seed1, Seed5 ));
}

/**
 * @brief Context structure for hash function pre-seeding.
 *
 * The komihash_set_preseed() function should be called to initialize the
 * structure before hashing.
 */

typedef struct {
	uint64_t Seed1; ///< The initial Seed1 value.
	uint64_t Seed5; ///< The initial Seed5 value.
} komihash_preseed_t;

/**
 * @brief Implements pre-seeding of the hash function state.
 *
 * @param[out] outSeed1 The resulting `Seed1` value to be used by the hash
 * function.
 * @param[out] outSeed5 The resulting `Seed5` value to be used by the hash
 * function.
 * @param UseSeed Seed value.
 */

KOMIHASH_INLINE_F void komihash_set_preseed_inner( uint64_t* const outSeed1,
	uint64_t* const outSeed5, const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	uint64_t Seed1 = KOMIHASH_IVAL1 ^ ( UseSeed & KOMIHASH_VAL01 );
	uint64_t Seed5 = KOMIHASH_IVAL5 ^ ( UseSeed & KOMIHASH_VAL10 );

	KOMIHASH_HASHROUND(); // Required for Perlin noise hashing.

	*outSeed1 = Seed1;
	*outSeed5 = Seed5;
}

/**
 * @brief Performs hash function state pre-seeding.
 *
 * @param[out] ps The pre-seeding context structure.
 * @param UseSeed Seed value. To use the default seed, set this value to 0.
 * This value can have any number of significant bits and any statistical
 * quality. It may need endianness correction if shared between big- and
 * little-endian systems.
 */

KOMIHASH_INLINE_F void komihash_set_preseed( komihash_preseed_t* const ps,
	const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	komihash_set_preseed_inner( &ps -> Seed1, &ps -> Seed5, UseSeed );
}

/**
 * @brief The KOMIHASH 64-bit hash function.
 *
 * This function produces and returns a 64-bit hash value of the specified
 * message, string, or binary data block. It is designed for hash tables and
 * hash maps and can also be used to generate checksums. It produces identical
 * hashes across big- and little-endian systems.
 *
 * @param Msg0 The message to hash. The alignment of this pointer is
 * unimportant. It is valid to pass a null pointer when `MsgLen` equals 0
 * (assuming that the compiler's implementation of address prefetching is
 * non-faulting, according to the GCC specification).
 * @param MsgLen The message length, in bytes; can be zero.
 * @param UseSeed Optional value to use instead of the default seed. To use
 * the default seed, set this value to 0. This value can have any number of
 * significant bits and any statistical quality. It may need endianness
 * correction if shared between big- and little-endian systems.
 * @return The 64-bit hash of the input data. It should be corrected for
 * endianness when shared between big- and little-endian systems.
 */

KOMIHASH_INLINE_F uint64_t komihash( const void* const Msg0,
	size_t MsgLen, const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	KOMIHASH_PREFETCH( Msg );

	uint64_t Seed1, Seed5;
	komihash_set_preseed_inner( &Seed1, &Seed5, UseSeed );

	return( komihash_inner( Msg, MsgLen, Seed1, Seed5 ));
}

/**
 * @brief The KOMIHASH 64-bit hash function with pre-seeding.
 *
 * This is a faster, statically pre-seeded hash function. @see komihash() for
 * details.
 *
 * @param Msg0 The message to hash.
 * @param MsgLen The message length, in bytes; can be zero.
 * @param ps Pre-seeding context structure, which must be initialized by the
 * komihash_set_preseed() function.
 * @return The 64-bit hash of the input data.
 */

KOMIHASH_INLINE_F uint64_t komihash_with_preseed( const void* const Msg0,
	const size_t MsgLen, const komihash_preseed_t* const ps ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	KOMIHASH_PREFETCH( Msg );

	return( komihash_inner( Msg, MsgLen, ps -> Seed1, ps -> Seed5 ));
}

/**
 * @brief The KOMIHASH 64-bit hash function without a seed.
 *
 * This is a faster hash function without a seed argument (using the default
 * seed of 0). @see komihash() for details.
 *
 * @param Msg0 The message to hash.
 * @param MsgLen The message length, in bytes; can be zero.
 * @return The 64-bit hash of the input data.
 */

KOMIHASH_INLINE_F uint64_t komihash_seedless( const void* const Msg0,
	const size_t MsgLen ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	KOMIHASH_PREFETCH( Msg );

	return( komihash_inner( Msg, MsgLen,
		KOMIHASH_DEFSEED1, KOMIHASH_DEFSEED5 ));
}

/**
 * @brief The KOMIRAND 64-bit pseudorandom number generator.
 *
 * This is a simple, reliable, self-starting, and efficient PRNG with a period
 * of 2^64. Its performance is 0.62 cycles/byte. It self-starts within 4
 * iterations that constitute the suggested "warm-up" period (needed before
 * using the output if the seeds are initialized with an arbitrary value).
 *
 * If the seeds are initialized with a high-quality, uniformly random value
 * (e.g., from the operating system's entropy pool or the output of a hash
 * function), the PRNG produces valid output from the start.
 *
 * Note that although this PRNG is classified as "chaotic", one should not
 * assume that it can enter degenerate cycles. When properly initialized, it
 * does not exhibit degenerate cycles, regardless of the initial state.
 *
 * @param[in,out] Seed1 Seed value 1. Can be initialized to any value
 * (even 0). This is the usual "PRNG seed" value.
 * @param[in,out] Seed2 Seed value 2, a supporting variable. It must be
 * initialized to the same value as `Seed1`. It should not be used as the PRNG
 * output.
 * @return The next uniformly random 64-bit value.
 */

KOMIHASH_INLINE_F uint64_t komirand( uint64_t* const Seed1,
	uint64_t* const Seed2 ) KOMIHASH_NOEXC
{
	uint64_t s1 = *Seed1;
	uint64_t s2 = *Seed2;
	uint64_t rh = 0;

	// The three instructions performed by this function (multiplication,
	// addition, and XOR) represent the simplest constant-free PRNG that works
	// with state variables of any even bit width, where `Seed1` serves as the
	// PRNG output (the PRNG has a period of 2^64). It passes `PractRand`
	// tests with only rare, non-systematic "unusual" assessments.
	//
	// To make this PRNG reliable and self-starting, and to eliminate the risk
	// of stalling, the "register checkerboard" constants are added - a source
	// of raw entropy. These constants are not required for hashing (but work
	// in that context), since input entropy is abundantly available during
	// hashing. Besides that, to minimize the risk of stalling, the hashing
	// uses an adjusted construction of this PRNG.
	//
	// (The `0x5555` and `0xAAAA...` constants should match the width of the
	// register; essentially, they repeat the `01` and `10` bit pairs; they
	// are not arbitrary constants.)

	kh_m128( s1, s2, &s1, &rh );
	s2 += rh;
	s1 ^= rh;

	s2 += KOMIHASH_VAL10;
	s1 += KOMIHASH_VAL01;

	*Seed2 = s2;
	*Seed1 = s1;

	return( s1 );
}

/**
 * @def KOMIHASH_BUFSIZE
 * @brief Streamed hashing buffer size, in bytes.
 *
 * It must be a multiple of 64 and not less than 128. It can be defined
 * externally.
 */

#if !defined( KOMIHASH_BUFSIZE )
	#define KOMIHASH_BUFSIZE 768
#endif // !defined( KOMIHASH_BUFSIZE )

#if KOMIHASH_BUFSIZE < 128
	#error KOMIHASH_BUFSIZE must be at least 128.
#endif // KOMIHASH_BUFSIZE < 128

#if ( KOMIHASH_BUFSIZE % 64 ) != 0
	#error KOMIHASH_BUFSIZE must be a multiple of 64.
#endif // ( KOMIHASH_BUFSIZE % 64 ) != 0

/**
 * @brief Context structure for streamed `komihash` hashing.
 *
 * The komihash_stream_init() function should be called to initialize the
 * structure before hashing. Note that the default buffer size is modest,
 * which allows this structure to be placed on the stack. `Seed[ 0 ]` is used
 * to store the `UseSeed` value.
 */

typedef struct {
	uint8_t Buf[ 8 + KOMIHASH_BUFSIZE ]; ///< Buffer including padding bytes.
	uint64_t Seed[ 8 ]; ///< Hashing state variables.
	size_t BufFill; ///< Buffer fill count (position), in bytes.
	size_t IsHashing; ///< Flag (0 or 1); equals 1 if hashing has started.
} komihash_stream_t;

/**
 * @brief Initializes a streamed `komihash` hashing session.
 *
 * @param[out] ctx Pointer to the context structure.
 * @param UseSeed Optional value to use instead of the default seed. To use
 * the default seed, set this value to 0. This value can have any number of
 * significant bits and any statistical quality. It may need endianness
 * correction if shared between big- and little-endian systems.
 */

KOMIHASH_INLINE void komihash_stream_init( komihash_stream_t* const ctx,
	const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	ctx -> Seed[ 0 ] = UseSeed;
	ctx -> BufFill = 0;
	ctx -> IsHashing = 0;
}

/**
 * @brief Updates the streamed hashing state with new input data.
 *
 * @param[in,out] ctx Pointer to the context structure. The structure must be
 * initialized by calling the komihash_stream_init() function.
 * @param Msg0 The next part of the message being hashed. The alignment of
 * this pointer is unimportant. It is valid to pass a null pointer when
 * `MsgLen` equals 0.
 * @param MsgLen The message length, in bytes; can be zero.
 */

KOMIHASH_INLINE void komihash_stream_update( komihash_stream_t* const ctx,
	const void* const Msg0, size_t MsgLen ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	const uint8_t* SwMsg = Msg;
	size_t SwMsgLen = 0;
	size_t BufFill = ctx -> BufFill;

	if( MsgLen >= KOMIHASH_BUFSIZE - BufFill && BufFill != 0 )
	{
		const size_t CopyLen = KOMIHASH_BUFSIZE - BufFill;
		memcpy( ctx -> Buf + 8 + BufFill, Msg, CopyLen );
		BufFill = 0;

		SwMsg += CopyLen;
		SwMsgLen = MsgLen - CopyLen;

		Msg = ctx -> Buf + 8;
		MsgLen = KOMIHASH_BUFSIZE;
	}

	if( BufFill == 0 )
	{
		while( MsgLen > 127 )
		{
			uint64_t Seed1, Seed2, Seed3, Seed4;
			uint64_t Seed5, Seed6, Seed7, Seed8;

			KOMIHASH_PREFETCH( Msg );

			if( ctx -> IsHashing )
			{
				Seed1 = ctx -> Seed[ 0 ];
				Seed2 = ctx -> Seed[ 1 ];
				Seed3 = ctx -> Seed[ 2 ];
				Seed4 = ctx -> Seed[ 3 ];
				Seed5 = ctx -> Seed[ 4 ];
				Seed6 = ctx -> Seed[ 5 ];
				Seed7 = ctx -> Seed[ 6 ];
				Seed8 = ctx -> Seed[ 7 ];
			}
			else
			{
				ctx -> IsHashing = 1;

				komihash_set_preseed_inner( &Seed1, &Seed5, ctx -> Seed[ 0 ]);

				Seed2 = KOMIHASH_IVAL2 ^ Seed1;
				Seed3 = KOMIHASH_IVAL3 ^ Seed1;
				Seed4 = KOMIHASH_IVAL4 ^ Seed1;
				Seed6 = KOMIHASH_IVAL6 ^ Seed5;
				Seed7 = KOMIHASH_IVAL7 ^ Seed5;
				Seed8 = KOMIHASH_IVAL8 ^ Seed5;
			}

			KOMIHASH_HASHLOOP64();

			ctx -> Seed[ 0 ] = Seed1;
			ctx -> Seed[ 1 ] = Seed2;
			ctx -> Seed[ 2 ] = Seed3;
			ctx -> Seed[ 3 ] = Seed4;
			ctx -> Seed[ 4 ] = Seed5;
			ctx -> Seed[ 5 ] = Seed6;
			ctx -> Seed[ 6 ] = Seed7;
			ctx -> Seed[ 7 ] = Seed8;

			if( SwMsgLen == 0 )
			{
				if( MsgLen != 0 )
				{
					break;
				}

				ctx -> BufFill = 0;
				return;
			}

			Msg = SwMsg;
			MsgLen = SwMsgLen;
			SwMsgLen = 0;
		}
	}

	ctx -> BufFill = BufFill + MsgLen;
	uint8_t* op = ctx -> Buf + 8 + BufFill;

	while( MsgLen != 0 )
	{
		*op = *Msg;
		Msg++;
		op++;
		MsgLen--;
	}
}

/**
 * @brief Finalizes a streamed `komihash` hashing session.
 *
 * This function returns the hash value of the previously hashed data. This
 * value is equal to the value returned by the komihash() function for all of
 * the input data.
 *
 * Since this function does not destructively alter the context structure,
 * it can be used to obtain intermediate hashes of the data stream being
 * hashed, and hashing can then be resumed.
 *
 * @param[in] ctx Pointer to the context structure. The structure must be
 * initialized by calling the komihash_stream_init() function.
 * @return The 64-bit hash value. It should be corrected for endianness when
 * shared between big- and little-endian systems.
 */

KOMIHASH_INLINE uint64_t komihash_stream_final( komihash_stream_t* const ctx )
	KOMIHASH_NOEXC
{
	const uint8_t* Msg = ctx -> Buf + 8;
	size_t MsgLen = ctx -> BufFill;

	if( ctx -> IsHashing == 0 )
	{
		return( komihash( Msg, MsgLen, ctx -> Seed[ 0 ]));
	}

	const uint64_t zv = 0;
	memcpy( ctx -> Buf, &zv, 8 );

	uint64_t Seed1 = ctx -> Seed[ 0 ];
	uint64_t Seed2 = ctx -> Seed[ 1 ];
	uint64_t Seed3 = ctx -> Seed[ 2 ];
	uint64_t Seed4 = ctx -> Seed[ 3 ];
	uint64_t Seed5 = ctx -> Seed[ 4 ];
	uint64_t Seed6 = ctx -> Seed[ 5 ];
	uint64_t Seed7 = ctx -> Seed[ 6 ];
	uint64_t Seed8 = ctx -> Seed[ 7 ];

	if( MsgLen > 63 )
	{
		KOMIHASH_HASHLOOP64();
	}

	Seed5 ^= Seed6 ^ Seed7 ^ Seed8;
	Seed1 ^= Seed2 ^ Seed3 ^ Seed4;

	return( komihash_epi( Msg, MsgLen, Seed1, Seed5 ));
}

/**
 * @brief FOR TESTING PURPOSES ONLY: Use the komihash() function instead.
 *
 * @param Msg The message to hash.
 * @param MsgLen The message length, in bytes.
 * @param UseSeed The seed to use.
 * @return The 64-bit hash value.
 */

KOMIHASH_INLINE uint64_t komihash_stream_oneshot( const void* const Msg,
	const size_t MsgLen, const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	komihash_stream_t ctx;

	komihash_stream_init( &ctx, UseSeed );
	komihash_stream_update( &ctx, Msg, MsgLen );

	return( komihash_stream_final( &ctx ));
}

#if defined( KOMIHASH_NS )

} // namespace KOMIHASH_NS

#if !defined( KOMIHASH_NS_CUSTOM )

namespace {

using KOMIHASH_NS::komihash_preseed_t;
using KOMIHASH_NS::komihash_set_preseed;
using KOMIHASH_NS::komihash;
using KOMIHASH_NS::komihash_with_preseed;
using KOMIHASH_NS::komihash_seedless;
using KOMIHASH_NS::komirand;
using KOMIHASH_NS::komihash_stream_t;
using KOMIHASH_NS::komihash_stream_init;
using KOMIHASH_NS::komihash_stream_update;
using KOMIHASH_NS::komihash_stream_final;
using KOMIHASH_NS::komihash_stream_oneshot;

} // namespace

#endif // !defined( KOMIHASH_NS_CUSTOM )

#endif // defined( KOMIHASH_NS )

// Macro definitions for Doxygen.

#if !defined( KOMIHASH_NS_CUSTOM )
	#define KOMIHASH_NS_CUSTOM
	#undef KOMIHASH_NS_CUSTOM
#endif // !defined( KOMIHASH_NS_CUSTOM )

#if defined( KOMIHASH_ENDIAN_DEFS )
	#undef KOMIHASH_LITTLE_ENDIAN
	#undef KOMIHASH_COND_EC
	#undef KOMIHASH_ENDIAN_DEFS
#endif // defined( KOMIHASH_ENDIAN_DEFS )

#undef KOMIHASH_NS
#undef KOMIHASH_U64_C
#undef KOMIHASH_NOEXC
#undef KOMIHASH_IVAL1
#undef KOMIHASH_IVAL2
#undef KOMIHASH_IVAL3
#undef KOMIHASH_IVAL4
#undef KOMIHASH_IVAL5
#undef KOMIHASH_IVAL6
#undef KOMIHASH_IVAL7
#undef KOMIHASH_IVAL8
#undef KOMIHASH_VAL01
#undef KOMIHASH_VAL10
#undef KOMIHASH_DEFSEED1
#undef KOMIHASH_DEFSEED5
#undef KOMIHASH_ICC_GCC
#undef KOMIHASH_GCC_BUILTINS
#undef KOMIHASH_BMI2
#undef KOMIHASH_EC32
#undef KOMIHASH_EC64
#undef KOMIHASH_LIKELY
#undef KOMIHASH_UNLIKELY
#undef KOMIHASH_LIKELY_DO
#undef KOMIHASH_LIKELY_DO_EXPR
#undef KOMIHASH_PREFETCH
#undef KOMIHASH_STATIC
#undef KOMIHASH_INLINE
#undef KOMIHASH_INLINE_F
#undef KOMIHASH_M128_IMPL
#undef KOMIHASH_EMULU
#undef KOMIHASH_HASHROUND
#undef KOMIHASH_HASH16
#undef KOMIHASH_HASHFIN
#undef KOMIHASH_HASHLOOP64

#endif // KOMIHASH_INCLUDED
