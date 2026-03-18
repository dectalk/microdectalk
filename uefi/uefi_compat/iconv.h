/* iconv not available in UEFI -- stub */
#ifndef _ICONV_H
#define _ICONV_H
typedef void *iconv_t;
static inline iconv_t iconv_open(const char *t, const char *f) { (void)t;(void)f; return (iconv_t)-1; }
static inline int     iconv_close(iconv_t cd) { (void)cd; return -1; }
#endif
