#ifndef _SHA1_Message_Digest_Algorithm_
#define _SHA1_Message_Digest_Algorithm_

//void sha1encode_wcs(WCHAR *szSrc, unsigned char *digest);
void sha1encode_str(char *src, unsigned char *digest);
void sha1encode_strn(unsigned char *src, int len, unsigned char *digest);

#endif		// _SHA1_Message_Digest_Algorithm_


