#ifndef NO_FILESYSTEM

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define USE_ICONV
#define REALLOC_SIZE 4096

#if defined(__unix__) && defined(USE_ICONV) && !defined(NO_CHARSET)
/* unix */
#include <langinfo.h>
#include <iconv.h>
#include <locale.h>
#include <wchar.h>
iconv_t cd;

void init_charset() {
	setlocale(LC_CTYPE, "");
	cd = iconv_open("Windows-1252//TRANSLIT//IGNORE", nl_langinfo(CODESET));
	if((long)cd == -1) cd = iconv_open("ISO-8859-15//TRANSLIT//IGNORE", nl_langinfo(CODESET));
	if((long)cd == -1) cd = iconv_open("ISO-8859-1//TRANSLIT//IGNORE", nl_langinfo(CODESET));
	if((long)cd == -1) cd = iconv_open("Windows-1252", nl_langinfo(CODESET));
	if((long)cd == -1) cd = iconv_open("ISO-8859-15", nl_langinfo(CODESET));
	if((long)cd == -1) cd = iconv_open("ISO-8859-1", nl_langinfo(CODESET));
	if((long)cd == -1) {
		perror("iconv_open");
		exit(EXIT_FAILURE);
	}
}

char* convert_string_for_dapi(char* in, size_t inlen) {
	char * out, *outp;
	size_t outsize = REALLOC_SIZE;
	size_t outleft = 0;
	size_t inleft  = inlen;
	size_t r;
	size_t offset;

	out = malloc(outsize + 1);
	if(out == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	outleft = outsize;
	outp	= out;

	do {
		memset(outp, 0, outleft + 1);
		errno = 0;
		r     = iconv(cd, &in, &inleft, &outp, &outleft);
		if(r == -1 && errno == E2BIG) {
			offset = outp - out;
			outsize += REALLOC_SIZE;
			out = realloc(out, outsize + 1);
			if(out == NULL) {
				perror("realloc");
				exit(EXIT_FAILURE);
			}
			outleft += REALLOC_SIZE;
			outp = out + offset;
		} else if(r == -1) {
			if(inleft > 0) {
				/* Skip */
				in++;
				inleft--;
			} else {
				perror("iconv");
				exit(EXIT_FAILURE);
			}
		}
	} while(inleft > 0);

	iconv(cd, NULL, NULL, NULL, NULL);

	return out;
}

#elif defined(_WIN32) && !defined(NO_CHARSET)
/* win32 */
#include <windows.h>
#include <wchar.h>

void init_charset() {
	/* do nothing */
}
char* convert_string_for_dapi(char* in, size_t inlen) {
	size_t mbbytes = (inlen + 1) * 4; /* wide enough??? i hope??? */
	size_t wbytes  = 0;
	size_t len;

	wchar_t* wout;
	char*	 mbout = malloc(mbbytes);

	wbytes = MultiByteToWideChar(CP_ACP, 0, in, inlen, NULL, 0) * sizeof(wchar_t);

	wout = malloc(wbytes + sizeof(wchar_t));
	len  = wbytes / sizeof(wchar_t);

	memset(wout, 0, wbytes);
	memset(mbout, 0, mbbytes);

	len	  = MultiByteToWideChar(CP_ACP, 0, in, inlen, wout, len);
	wout[len] = 0;

	len	   = WideCharToMultiByte(1252, 0, wout, len, mbout, mbbytes, 0, 0);
	mbout[len] = 0;

	free(wout);

	return mbout;
}
#else
void init_charset() {
}
char* convert_string_for_dapi(char* in, size_t inlen) {
	return in; // TODO: non-windows non-iconv implementation
}

#endif

#endif
