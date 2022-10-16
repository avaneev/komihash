/**
 * testvec.c version 4.3.1
 *
 * The program that lists test vectors and their hash values, for the current
 * version of komihash. Also prints initial outputs of `komirand` PRNG.
 *
 * Description is available at https://github.com/avaneev/komihash
 *
 * License
 *
 * Copyright (c) 2021-2022 Aleksey Vaneev
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

#include <stdio.h>
#include "komihash.h"

int main()
{
	const int seedc = 3;
	const uint64_t seeds[ seedc ] = { 0, 0x0123456789ABCDEF, 256 };

	const int strc = 5;
	const char* const strs[ strc ] = {
		"This is a 32-byte tester string.",
		"The cat is out of the bag",
		"A 16-byte string",
		"The new string",
		"7 bytes"
	};

	const int bulkc = 15;
	const int bulks[ bulkc ] = { 6, 12, 20, 31, 32, 40, 47, 48, 56, 64,
		72, 80, 112, 132, 256 };

	uint8_t bulkbuf[ 256 ];
	int i;

	for( i = 0; i < 256; i++ )
	{
		bulkbuf[ i ] = (uint8_t) i;
	}

	int j;

	for( j = 0; j < seedc; j++ )
	{
		printf( "\tkomihash UseSeed = 0x%016llx:\n", seeds[ j ]);

		for( i = 0; i < strc; i++ )
		{
			const char* const s = strs[ i ];
			const size_t sl = strlen( s );

			printf( "\t\"%s\" = 0x%016llx\n", s,
				komihash( s, sl, seeds[ j ]));
		}

		for( i = 0; i < bulkc; i++ )
		{
			printf( "\tbulk(%i) = 0x%016llx\n", bulks[ i ],
				komihash( bulkbuf, bulks[ i ], seeds[ j ]));
		}

		printf( "\n" );
	}

	for( j = 0; j < seedc; j++ )
	{
		printf( "\tkomirand Seed1/Seed2 = 0x%016llx:\n", seeds[ j ]);

		uint64_t Seed1 = seeds[ j ];
		uint64_t Seed2 = seeds[ j ];

		for( i = 0; i < 12; i++ )
		{
			printf( "\t0x%016llx\n", komirand( &Seed1, &Seed2 ));
		}

		printf( "\n" );
	}
}
