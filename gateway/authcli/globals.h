#ifndef _Message_Digests_Globals_
#define _Message_Digests_Globals_

extern unsigned char	PADDING[64];
// long = 64 bit on AMD64, so using uint for compatibility
typedef unsigned int ULONG;

// ROTATE_LEFT rotates x left n bits - C style workaround for circularly
// shifting (rotating)
#define ROTATE_LEFT(x, n)	(((x) << (n)) | ((x) >> (32 - n)))

void encode(unsigned char *output, ULONG *input, int len);
void encode_rev(unsigned char *output, ULONG *input, int len);
void decode(ULONG *output, unsigned char *input, int len);
void decode_rev(ULONG *output, unsigned char *input, int len);


#endif		// _Message_Digests_Globals_

