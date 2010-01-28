#ifndef _XT_WEBSTR_H
#define _XT_WEBSTR_H

#define BM_MAX_NLEN 256

#define BLK_JAVA		0x01
#define BLK_ACTIVE		0x02
#define BLK_COOKIE		0x04
#define BLK_PROXY		0x08

struct xt_webstr_info {
    char string[BM_MAX_NLEN];
    u_int16_t invert;
    u_int16_t len;
    u_int8_t type;
};

enum xt_webstr_type
{
    XT_WEBSTR_HOST,
    XT_WEBSTR_URL,
    XT_WEBSTR_CONTENT
};

#endif /* _XT_WEBSTR_H */
