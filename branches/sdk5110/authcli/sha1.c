// SECURE HASH STANDARD - SHA-1 (fips 180-1)
//

// taken from http://aforge.dotnet.lv/alg/crc_hash/sha1.html

#include "sha1.h"
#include "globals.h"
#include <string.h>

// work structure
typedef struct
{
	ULONG		state[5];
	ULONG		count[2];
	unsigned char	buffer[64];
}
SHA1_CTX;

#define MASK	0xF

// sequence of constant words is used in the SHA-1
static ULONG	_K[4] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
#define K(t)	(_K[(t) / 20])

// define four auxiliary functions (SHA1 basic dunctions)
#define F0(b, c, d)	(((b) & (c)) | ((~b) & (d)))
#define F1(b, c, d)	((b) ^ (c) ^ (d))
#define F2(b, c, d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))
#define F3(b, c, d)	((b) ^ (c) ^ (d))

// local functions
void sha1init(SHA1_CTX *context);
void sha1update(SHA1_CTX *context, unsigned char *src, ULONG size);
void sha1final(SHA1_CTX *context, unsigned char *digest);
void sha1transform(ULONG state[5], unsigned char src[64]);


// ------------------------------------------------------------
// calculate the message-digest of the wide character string
//
/*
void sha1encode_wcs(WCHAR *szSrc, unsigned char *digest)
{
	int		len = wcslen(szSrc);
	char	*sz = new char[len + 1];

	// convert to multibyte characters
	wcstombs((char *) sz, szSrc, len + 1);
	// MD5 encode
	sha1encode_str(sz, digest);

	delete [] sz;
}

*/

// ------------------------------------------------------------
// calculate the digest of the multibyte characters string
//
void sha1encode_str(char *src, unsigned char *digest)
{
	SHA1_CTX	context;

	sha1init(&context);
	sha1update(&context, (unsigned char *) src, strlen(src));
	sha1final(&context, digest);
}


void sha1encode_strn(unsigned char *src, int len, unsigned char *digest)
{
	SHA1_CTX	context;

	sha1init(&context);
	sha1update(&context, src, len);
	sha1final(&context, digest);
}


// ------------------------------------------------------------
// initialize SHA-1 calculation
//
void sha1init(SHA1_CTX *context)
{
	context->count[0] = context->count[1] = 0;
	// SHA1 initialization magic words
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
}



// ------------------------------------------------------------
// calculate the array
//
void sha1update(SHA1_CTX *context, unsigned char *src, ULONG size)
{
	ULONG		i, index, partLen;

	// compute number of bytes mod 64
	index = (ULONG)((context->count[0] >> 3) & 0x3F);

	// update number of bits
	if ((context->count[0] += (size << 3)) < (size << 3))
		context->count[1]++;
	context->count[1] += ((ULONG) size >> 29);

	partLen = 64 - index;

	// transform as many times as possible
	if (size >= partLen)
	{
		memcpy(&context->buffer[index], src, partLen);
		sha1transform(context->state, context->buffer);

		for (i = partLen; i + 63 < size; i += 64)
			sha1transform(context->state, &src[i]);

		index = 0;
	}
	else
		i = 0;

	memcpy(&context->buffer[index], &src[i], size - i);
}



// ------------------------------------------------------------
// finalize SHA1 calculation
//
void sha1final(SHA1_CTX *context, unsigned char *digest)
{
	unsigned char	bits[8];
	ULONG			index, padLen;

	// save number of bits
	encode_rev(bits, &context->count[1], 4);
	encode_rev(bits + 4, &context->count[0], 4);

	// pad out to 56 mod 64
	index = (ULONG)((context->count[0] >> 3) & 0x3F);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	sha1update(context, PADDING, padLen);

	// appenf legth
	sha1update(context, bits, 8);

	// store state in digest
	encode_rev(digest, context->state, 20);

	memset(context, 0, sizeof(*context));
}



// ------------------------------------------------------------
// calculate the block
//
void sha1transform(ULONG state[5], unsigned char src[64])
{
	ULONG	a = state[0], b = state[1], c = state[2], d = state[3], e = state[4], w[16];
	ULONG	t, s, tmp;

	decode_rev(w, src, 64);

	// step 1
	for (t = 0; t < 20; t++)
	{
		s = t & MASK;

		if (t >= 16)
		{
			w[s] = ROTATE_LEFT(w[(s + 13) & MASK] ^ w[(s + 8) & MASK] ^ w[(s + 2) & MASK] ^ w[s], 1);
		}

		tmp = ROTATE_LEFT(a, 5) + F0(b, c, d) + e + w[s] + K(t);
		e = d; d = c; c = ROTATE_LEFT(b, 30); b = a; a = tmp;
	}
	// step 2
	for (t = 20; t < 40; t++)
	{
		s = t & MASK;

		w[s] = ROTATE_LEFT(w[(s + 13) & MASK] ^ w[(s + 8) & MASK] ^	w[(s + 2) & MASK] ^ w[s], 1);
		tmp = ROTATE_LEFT(a, 5) + F1(b, c, d) + e + w[s] + K(t);
		e = d; d = c; c = ROTATE_LEFT(b, 30); b = a; a = tmp;
	}
	// step 3
	for (t = 40; t < 60; t++)
	{
		s = t & MASK;

		w[s] = ROTATE_LEFT(w[(s + 13) & MASK] ^ w[(s + 8) & MASK] ^	w[(s + 2) & MASK] ^ w[s], 1);
		tmp = ROTATE_LEFT(a, 5) + F2(b, c, d) + e + w[s] + K(t);
		e = d; d = c; c = ROTATE_LEFT(b, 30); b = a; a = tmp;
	}
	// step 4
	for (t = 60; t < 80; t++)
	{
		s = t & MASK;

		w[s] = ROTATE_LEFT(w[(s + 13) & MASK] ^ w[(s + 8) & MASK] ^	w[(s + 2) & MASK] ^ w[s], 1);
		tmp = ROTATE_LEFT(a, 5) + F3(b, c, d) + e + w[s] + K(t);
		e = d; d = c; c = ROTATE_LEFT(b, 30); b = a; a = tmp;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	//
	memset((char *) w, 0, sizeof(w));
}


