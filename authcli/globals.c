//#include "stdafx.h"
#include "globals.h"

// padding
unsigned char	PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ------------------------------------------------------------
// encode input (ULONG) into ouput (char)
//
void encode(unsigned char *output, ULONG *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j]		= (unsigned char) (input[i] & 0xFF);
		output[j + 1]	= (unsigned char) ((input[i] >> 8)  & 0xFF);
		output[j + 2]	= (unsigned char) ((input[i] >> 16)  & 0xFF);
		output[j + 3]	= (unsigned char) ((input[i] >> 24)  & 0xFF);
	}
}



// ------------------------------------------------------------
// encode input (ULONG) into ouput (char) reverse order
// the highest is the first byte
//
void encode_rev(unsigned char *output, ULONG *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j + 3]	= (unsigned char) (input[i] & 0xFF);
		output[j + 2]	= (unsigned char) ((input[i] >> 8)  & 0xFF);
		output[j + 1]	= (unsigned char) ((input[i] >> 16)  & 0xFF);
		output[j + 0]	= (unsigned char) ((input[i] >> 24)  & 0xFF);
	}
}


// ------------------------------------------------------------
// decode input (char) into ouput (ULONG)
//
void decode(ULONG *output, unsigned char *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((ULONG) input[j]) | (((ULONG) input[j + 1]) << 8) |
		(((ULONG) input[j + 2]) << 16) | (((ULONG) input[j + 3]) << 24);
}



// ------------------------------------------------------------
// decode input (char) into ouput (ULONG)
// the first byte is the highest
//
void decode_rev(ULONG *output, unsigned char *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((ULONG) input[j + 3]) | (((ULONG) input[j + 2]) << 8) |
		(((ULONG) input[j + 1]) << 16) | (((ULONG) input[j]) << 24);
}


