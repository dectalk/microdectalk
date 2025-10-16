#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define USE_ICONV
#define REALLOC_SIZE 4096

/* DO NOT CHECK FOR __unix__! that is used as solution to most macros */
#if !defined(_WIN32) && defined(USE_ICONV)
/* unix */
#include <langinfo.h>
#include <iconv.h>
#include <locale.h>
#include <wchar.h>
iconv_t cd;

void init_charset() { // i hate this but can't be bothered to make it better
    setlocale(LC_CTYPE, "");
    cd = iconv_open("Windows-1252//TRANSLIT//IGNORE", nl_langinfo(CODESET));
    if ((long)cd == -1) {
      cd = iconv_open("ISO-8859-15//TRANSLIT//IGNORE", nl_langinfo(CODESET));
      if ((long)cd == -1) {
        cd = iconv_open("ISO-8859-1//TRANSLIT//IGNORE", nl_langinfo(CODESET));
        if ((long)cd == -1) {
          cd = iconv_open("Windows-1252", nl_langinfo(CODESET));
          if ((long)cd == -1) {
            cd = iconv_open("ISO-8859-15", nl_langinfo(CODESET));
            if ((long)cd == -1) {
              cd = iconv_open("ISO-8859-1", nl_langinfo(CODESET));
              if ((long)cd == -1) {
                perror("iconv_open");
                exit(EXIT_FAILURE);
              }
            }
          }
        }
      }
    }
}

char *convert_string_for_dapi(char *in, size_t inlen) {
	char *out, *outp;
	size_t outsize = REALLOC_SIZE;
	size_t outleft = 0;
	size_t inleft = inlen;
	size_t r;
	size_t offset;

	out = malloc(outsize + 1);
	if (out == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	outleft = outsize;
	outp = out;

	do {
		memset(outp, 0, outleft + 1);
		errno = 0;
		r = iconv(cd, &in, &inleft, &outp, &outleft);
		if (r == -1 && errno == E2BIG) {
			offset = outp - out;
			outsize += REALLOC_SIZE;
			out = realloc(out, outsize + 1);
			if (out == NULL) {
				perror("realloc");
				exit(EXIT_FAILURE);
			}
			outleft += REALLOC_SIZE;
			outp = out + offset;
		} else if (r == -1) {
			if (inleft > 0) {
				/* Skip */
				in++;
				inleft--;
			} else {
				perror("iconv");
				exit(EXIT_FAILURE);
			}
		}
	} while (inleft > 0);

	iconv(cd, NULL, NULL, NULL, NULL);

	return out;
}

#else

#if defined(_WIN32)
/* win32 */
void init_charset() {

}
char *convert_string_for_dapi(char *in, size_t inlen) {
    return in; // TODO: windows implementation
}
#else
void init_charset() {

}
char *convert_string_for_dapi(char *in, size_t inlen) {
    return in; // TODO: non-windows non-iconv implementation
}

#endif

#endif
