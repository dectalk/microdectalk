//
// uefi_shim.h -- C stdlib compatibility shim for DECtalkMini in UEFI
//
// Provides: malloc/free, mem*/str* functions, printf stubs, math, ctype,
//           setjmp/longjmp (via gnu-efi), and header-guard macros so
//           the uefi_compat/ stub headers suppress system #includes.
//

#ifndef UEFI_SHIM_H
#define UEFI_SHIM_H

// Force 16-bit wchar_t for UEFI strings
#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short wchar_t;
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef __SIZE_TYPE__    size_t;
#endif
#ifndef _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#endif

#include <stddef.h>

// ---------------------------------------------------------------------------
// Pull in gnu-efi base types and libraries
// ---------------------------------------------------------------------------
#include <efi.h>
#include <efilib.h>
#include <efisetjmp.h>

// ---------------------------------------------------------------------------
// Fix gnu-efi vs DECtalkMini conflicts
// ---------------------------------------------------------------------------
#ifdef WCHAR
#undef WCHAR
#endif

// ---------------------------------------------------------------------------
// Fix gnu-efi vs EDK2 style calls
// ---------------------------------------------------------------------------
#undef LocateHandleBuffer
#undef HandleProtocol
#undef OpenProtocol
#undef CloseProtocol
#undef LocateProtocol
#undef FreePool

// ---------------------------------------------------------------------------
// stdarg - must come after efi headers to avoid va_start conflicts
// ---------------------------------------------------------------------------
#undef va_start
#undef va_end
#undef va_arg
#undef va_copy
#include <stdarg.h>

// ---------------------------------------------------------------------------
// EDK2 to gnu-efi compatibility macros
// ---------------------------------------------------------------------------

#define gBS BS
#define gST ST

extern EFI_GUID myEfiPciIoProtocolGuid;
#define gEfiPciIoProtocolGuid  myEfiPciIoProtocolGuid

#ifdef EFIAPI
#undef EFIAPI
#endif
#define EFIAPI

#ifndef VOID
#define VOID                void
#endif
#ifndef CONST
#define CONST               const
#endif
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef EFI_ERROR
#define EFI_ERROR(Status)   ((INTN)(Status) < 0)
#endif

// BIT macros
#ifndef BIT0
#define BIT0  0x00000001
#define BIT1  0x00000002
#define BIT2  0x00000004
#define BIT3  0x00000008
#define BIT4  0x00000010
#define BIT5  0x00000020
#define BIT6  0x00000040
#define BIT7  0x00000080
#define BIT8  0x00000100
#define BIT9  0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000
#endif

// String functions
#define AsciiStrCmp(s1, s2)       strcmp(s1, s2)

// Missing helpers
#ifndef MemoryFence
#define MemoryFence()             __sync_synchronize()
#endif
#ifndef OFFSET_OF
#define OFFSET_OF(TYPE, Field)    ((UINTN) &(((TYPE *)0)->Field))
#endif

// PCI Commands
#ifndef EFI_PCI_COMMAND_IO_SPACE
#define EFI_PCI_COMMAND_IO_SPACE             0x0001
#define EFI_PCI_COMMAND_MEMORY_SPACE         0x0002
#define EFI_PCI_COMMAND_BUS_MASTER           0x0004
#endif

// ---------------------------------------------------------------------------
// Header guards
// ---------------------------------------------------------------------------
#define _STRING_H        1
#define _STDLIB_H        1
#define _STDIO_H         1
#define _MALLOC_H        1
#define _MATH_H          1
#define _CTYPE_H         1
#define _ASSERT_H        1
#define _LIMITS_H        1
#define _STDDEF_H        1
#define __STRING_H       1
#define __STDLIB_H       1
#define __STDIO_H        1
#define _SYS_TYPES_H     1

#ifndef NULL
#define NULL ((void *)0)
#endif

// ---------------------------------------------------------------------------
// Memory functions
// ---------------------------------------------------------------------------
static inline void *memcpy (void *d, const void *s, size_t n)
  { uefi_call_wrapper(BS->CopyMem, 3, d, (void*)s, (UINTN)n); return d; }
static inline void *memmove(void *d, const void *s, size_t n)
  { uefi_call_wrapper(BS->CopyMem, 3, d, (void*)s, (UINTN)n); return d; }
static inline void *memset (void *s, int c, size_t n)
  { uefi_call_wrapper(BS->SetMem, 3, s, (UINTN)n, (UINT8)c); return s; }
static inline int   memcmp (const void *a, const void *b, size_t n)
  { 
    const unsigned char *_a = (const unsigned char *)a;
    const unsigned char *_b = (const unsigned char *)b;
    while(n--) { if (*_a != *_b) return (int)*_a - (int)*_b; _a++; _b++; }
    return 0;
  }
static inline void *memchr (const void *s, int c, size_t n) {
  const unsigned char *p = (const unsigned char *)s;
  while (n--) { if (*p == (unsigned char)c) return (void *)p; p++; }
  return NULL;
}

// ---------------------------------------------------------------------------
// String functions
// ---------------------------------------------------------------------------
static inline size_t strlen (const char *s)
  { const char *e = s; while (*e) e++; return (size_t)(e - s); }
static inline char *strcpy (char *d, const char *s)
  { char *r = d; while ((*d++ = *s++)); return r; }
static inline char *strncpy(char *d, const char *s, size_t n) {
  char *r = d;
  while (n && (*d++ = *s++)) n--;
  while (n--) *d++ = 0;
  return r;
}
static inline int strcmp (const char *a, const char *b) {
  while (*a && *a == *b) { a++; b++; }
  return (unsigned char)*a - (unsigned char)*b;
}
static inline int strncmp(const char *a, const char *b, size_t n) {
  while (n && *a && *a == *b) { a++; b++; n--; }
  return n ? ((unsigned char)*a - (unsigned char)*b) : 0;
}
static inline char *strcat (char *d, const char *s)
  { char *r = d; while (*d) d++; while ((*d++ = *s++)); return r; }
static inline char *strncat(char *d, const char *s, size_t n) {
  char *r = d; while (*d) d++;
  while (n-- && (*d++ = *s++));
  *d = 0; return r;
}
extern int strcasecmp (const char *a, const char *b);
extern int strncasecmp(const char *a, const char *b, size_t n);
static inline char *strchr (const char *s, int c) {
  while (*s) { if (*s == (char)c) return (char *)s; s++; }
  return c == 0 ? (char *)s : NULL;
}
static inline char *strrchr(const char *s, int c) {
  const char *p = NULL;
  while (*s) { if (*s == (char)c) p = s; s++; }
  return c == 0 ? (char *)s : (char *)p;
}
static inline char *strstr (const char *h, const char *n) {
  if (!*n) return (char *)h;
  for (; *h; h++) {
    const char *p = h, *q = n;
    while (*q && *p == *q) { p++; q++; }
    if (!*q) return (char *)h;
  }
  return NULL;
}
static inline size_t strspn (const char *s, const char *a) {
  size_t n = 0;
  while (*s) {
    const char *t = a; int ok = 0;
    while (*t) { if (*s == *t++) { ok = 1; break; } }
    if (!ok) break;
    s++; n++;
  }
  return n;
}
static inline size_t strcspn(const char *s, const char *r) {
  size_t n = 0;
  while (*s) { const char *t = r; while (*t) { if (*s == *t++) return n; } s++; n++; }
  return n;
}
static inline char *strpbrk(const char *s, const char *a) {
  while (*s) { const char *t = a; while (*t) { if (*s == *t++) return (char *)s; } s++; }
  return NULL;
}
static inline char *strtok(char *s, const char *delim) {
  static char *sv = NULL;
  if (s) sv = s; else s = sv;
  if (!s) return NULL;
  while (*s && strchr(delim, *s)) s++;
  if (!*s) { sv = NULL; return NULL; }
  char *start = s;
  while (*s && !strchr(delim, *s)) s++;
  if (*s) { *s = 0; sv = s + 1; } else { sv = NULL; }
  return start;
}

// ---------------------------------------------------------------------------
// Allocation
// ---------------------------------------------------------------------------
static inline void *malloc (size_t n)         { 
  void *p = NULL;
  uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, (UINTN)n, &p);
  return p;
}
static inline void *calloc (size_t n, size_t s){ 
  void *p = malloc(n * s);
  if (p) memset(p, 0, n * s);
  return p;
}
static inline void  free   (void *p)           { if (p) uefi_call_wrapper(BS->FreePool, 1, p); }
static inline void *realloc(void *p, size_t n) {
  void *q = malloc(n);
  if (q && p) memcpy(q, p, n);
  if (p) free(p);
  return q;
}

// ---------------------------------------------------------------------------
// I/O stubs
// ---------------------------------------------------------------------------
typedef void FILE;
#ifndef stderr
#define stdin  ((FILE*)0)
#define stdout ((FILE*)0)
#define stderr ((FILE*)0)
#endif
#ifndef EOF
#define EOF    (-1)
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

static inline FILE *fopen (const char *p, const char *m) { (void)p;(void)m; return NULL; }
static inline FILE *freopen(const char *p, const char *m, FILE *f) { (void)p;(void)m;(void)f; return NULL; }
static inline int   fclose (FILE *f) { (void)f; return EOF; }
static inline int   fread  (void *b, size_t s, size_t n, FILE *f) { (void)b;(void)s;(void)n;(void)f; return 0; }
static inline int   fwrite (const void *b, size_t s, size_t n, FILE *f) { (void)b;(void)s;(void)n;(void)f; return 0; }
static inline char *fgets  (char *b, int n, FILE *f) { (void)b;(void)n;(void)f; return NULL; }
static inline int   fgetc  (FILE *f) { (void)f; return EOF; }
static inline int   fscanf (FILE *f, const char *fmt, ...) { (void)f;(void)fmt; return EOF; }
static inline int   ungetc (int c, FILE *f) { (void)f; return EOF; }
static inline long  ftell  (FILE *f) { (void)f; return -1L; }
static inline int   fseek  (FILE *f, long o, int w) { (void)f;(void)o;(void)w; return -1; }
static inline void  rewind (FILE *f) { (void)f; }
static inline int   feof   (FILE *f) { (void)f; return 1; }
static inline int   ferror (FILE *f) { (void)f; return 1; }
static inline int   remove (const char *p) { (void)p; return -1; }
static inline int   rename (const char *o, const char *n) { (void)o;(void)n; return -1; }
static inline char *mktemp (char *t) { (void)t; return NULL; }

static inline int vfprintf(FILE *f, const char *fmt, va_list ap) { (void)f;(void)fmt;(void)ap; return 0; }
static inline int vprintf (const char *fmt, va_list ap)           { (void)fmt;(void)ap; return 0; }
static inline int vsprintf (char *b, const char *fmt, va_list ap) {
  return (int)AsciiVSPrint((CHAR8*)b, 0x7fffffff, (CHAR8*)fmt, ap);
}
static inline int vsnprintf(char *b, size_t n, const char *fmt, va_list ap) {
  return (int)AsciiVSPrint((CHAR8*)b, (UINTN)n, (CHAR8*)fmt, ap);
}

extern int sprintf (char *b, const char *fmt, ...);
extern int snprintf(char *b, size_t n, const char *fmt, ...);

#define printf(fmt, ...)           (0)
#define fprintf(fp, fmt, ...)      (0)
#define fputs(s, fp)               (0)
#define fputc(c, fp)               (c)
#define putchar(c)                 (c)
#define puts(s)                    (0)
#define fflush(fp)                 (0)
#define sscanf(buf, fmt, ...)      (0)

// ---------------------------------------------------------------------------
// Conversions / misc
// ---------------------------------------------------------------------------
static inline int atoi(const char *s) {
  int n = 0, neg = 0;
  while (*s == ' ') s++;
  if (*s == '-') { neg = 1; s++; } else if (*s == '+') s++;
  while (*s >= '0' && *s <= '9') { n = n * 10 + (*s - '0'); s++; }
  return neg ? -n : n;
}
static inline long atol (const char *s) { return (long)atoi (s); }
static inline long strtol(const char *s, char **end, int base) {
  long n = 0; int neg = 0;
  while (*s == ' ') s++;
  if (*s == '-') { neg = 1; s++; } else if (*s == '+') s++;
  if (base == 0 || base == 16) {
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { s += 2; base = 16; }
    else if (base == 0) { base = 10; }
  }
  while (1) {
    int d;
    if (*s >= '0' && *s <= '9') d = *s - '0';
    else if (base == 16 && *s >= 'a' && *s <= 'f') d = *s - 'a' + 10;
    else if (base == 16 && *s >= 'A' && *s <= 'F') d = *s - 'A' + 10;
    else break;
    if (d >= base) break;
    n = n * base + d; s++;
  }
  if (end) *end = (char *)s;
  return neg ? -n : n;
}
static inline unsigned long strtoul(const char *s, char **e, int b)
  { return (unsigned long)strtol (s, e, b); }

#ifndef abs
#define abs(x)  ((x) < 0   ? -(x)  : (x))
#endif
#define labs(x) ((x) < 0L  ? -(x)  : (x))
#define exit(x) do {} while (0)
#define abort() do {} while (0)

static inline void qsort(void *base, size_t nmemb, size_t size,
                          int (*cmp)(const void *, const void *)) {
  unsigned char *b = (unsigned char *)base;
  unsigned char *tmp = (unsigned char *)malloc(size);
  if (!tmp) return;
  for (size_t i = 1; i < nmemb; i++) {
    memcpy (tmp, b + i * size, size);
    size_t j = i;
    while (j > 0 && cmp (b + (j - 1) * size, tmp) > 0) {
      memcpy (b + j * size, b + (j - 1) * size, size); j--;
    }
    memcpy (b + j * size, tmp, size);
  }
  free (tmp);
}

static inline void *bsearch(const void *key, const void *base, size_t n,
                             size_t size, int (*cmp)(const void *, const void *)) {
  const unsigned char *b = (const unsigned char *)base;
  while (n) {
    size_t m = n / 2;
    int r = cmp (key, b + m * size);
    if (r == 0) return (void *)(b + m * size);
    if (r > 0) { b += (m + 1) * size; n -= m + 1; } else n = m;
  }
  return NULL;
}

static inline int isalpha(int c) { return (c>='a'&&c<='z')||(c>='A'&&c<='Z'); }
static inline int isdigit(int c) { return c>='0'&&c<='9'; }
static inline int isalnum(int c) { return isalpha(c)||isdigit(c); }
static inline int isspace(int c) { return c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\f'||c=='\v'; }
static inline int isupper(int c) { return c>='A'&&c<='Z'; }
static inline int islower(int c) { return c>='a'&&c<='z'; }
static inline int isprint(int c) { return c>=' '&&c<='~'; }
static inline int ispunct(int c) { return isprint(c)&&!isalnum(c)&&c!=' '; }
static inline int iscntrl(int c) { return (unsigned)c<32||(unsigned)c==127; }
static inline int isxdigit(int c){ return isdigit(c)||(c>='a'&&c<='f')||(c>='A'&&c<='F'); }
static inline int toupper(int c) { return islower(c)?c-'a'+'A':c; }
static inline int tolower(int c) { return isupper(c)?c-'A'+'a':c; }

#ifndef assert
#define assert(x) do {} while (0)
#endif

#ifndef INT_MAX
#define INT_MAX    2147483647
#define INT_MIN    (-2147483648)
#define UINT_MAX   4294967295u
#define LONG_MAX   2147483647L
#define LONG_MIN   (-2147483648L)
#define ULONG_MAX  4294967295uL
#define SHRT_MAX   32767
#define SHRT_MIN   (-32768)
#define USHRT_MAX  65535u
#define CHAR_BIT   8
#define CHAR_MAX   127
#define CHAR_MIN   (-128)
#endif

#ifndef offsetof
#define offsetof(t, m) __builtin_offsetof(t, m)
#endif

#define fabs(x)   __builtin_fabs(x)
#define fabsf(x)  __builtin_fabsf(x)
#define sqrt(x)   __builtin_sqrt(x)
#define sqrtf(x)  __builtin_sqrtf(x)
#define floor(x)  __builtin_floor(x)
#define floorf(x) __builtin_floorf(x)
#define ceil(x)   __builtin_ceil(x)
#define ceilf(x)  __builtin_ceilf(x)

#ifndef M_PI
#define M_PI     3.14159265358979323846
#define M_PI_2   1.57079632679489661923
#define M_E      2.71828182845904523536
#define M_LN2    0.69314718055994530942
#define M_LN10   2.30258509299404568402
#define M_SQRT2  1.41421356237309504880
#endif
#define HUGE_VAL __builtin_huge_val()

static inline double log(double x) {
  if (x <= 0.0) return -HUGE_VAL;
  union { double d; unsigned long long i; } u;
  u.d = x;
  int e = (int)((u.i >> 52) & 0x7FFull) - 1023;
  u.i  = (u.i & 0x000FFFFFFFFFFFFFull) | 0x3FF0000000000000ull;
  double m = u.d;
  double r = (m - 1.0) / (m + 1.0);
  double r2 = r * r;
  double p = r * (2.0 + r2 * (2.0/3.0 + r2 * (2.0/5.0 + r2 * (2.0/7.0
                 + r2 * 2.0/9.0))));
  return p + e * M_LN2;
}
static inline float  logf (float x)  { return (float)log ((double)x); }
static inline double log2 (double x) { return log (x) * (1.0 / M_LN2); }
static inline double log10(double x) { return log (x) * (1.0 / M_LN10); }
static inline float  log10f(float x) { return (float)log10 ((double)x); }

static inline double exp(double x) {
  int n = (int)(x * (1.0 / M_LN2) + 0.5);
  double r = x - n * M_LN2;
  double p = 1.0 + r * (1.0 + r * (0.5 + r * (1.0/6.0
               + r * (1.0/24.0 + r * (1.0/120.0 + r * 1.0/720.0)))));
  union { double d; unsigned long long i; } u;
  u.i = (unsigned long long)(n + 1023) << 52;
  return p * u.d;
}
static inline float expf(float x) { return (float)exp ((double)x); }
static inline double pow (double x, double y) { return exp (y * log (x)); }
static inline float  powf(float x, float y)   { return (float)pow ((double)x, (double)y); }

static inline double sin(double x) {
  x -= (double)(long)(x / (2.0 * M_PI)) * (2.0 * M_PI);
  if (x < 0.0) x += 2.0 * M_PI;
  int q = (int)(x / M_PI_2) & 3;
  x -= q * M_PI_2;
  double x2 = x * x;
  double s  = x * (1.0 - x2 * (1.0/6.0 - x2 * (1.0/120.0 - x2 / 5040.0)));
  double c  = 1.0 - x2 * (0.5 - x2 * (1.0/24.0 - x2 / 720.0));
  switch (q) {
    case 0: return  s;
    case 1: return  c;
    case 2: return -s;
    default:return -c;
  }
}
static inline double cos(double x) { return sin (x + M_PI_2); }
static inline float  sinf(float x) { return (float)sin ((double)x); }
static inline float  cosf(float x) { return (float)cos ((double)x); }
static inline double tan(double x) { return sin(x)/cos(x); }
static inline double atan(double x) {
  int flip = 0;
  if (x < 0.0) { x = -x; flip = 1; }
  int comp = 0;
  if (x > 1.0) { x = 1.0 / x; comp = 1; }
  double x2 = x * x;
  double r = x * (1.0 - x2 * (1.0/3.0 - x2 * (1.0/5.0 - x2 * (1.0/7.0
                  - x2 * (1.0/9.0 - x2 / 11.0)))));
  if (comp) r = M_PI_2 - r;
  return flip ? -r : r;
}
static inline double atan2(double y, double x) {
  if (x > 0.0) return atan (y / x);
  if (x < 0.0) return y >= 0.0 ? atan (y/x) + M_PI : atan (y/x) - M_PI;
  return y > 0.0 ? M_PI_2 : -M_PI_2;
}
static inline double asin(double x) { return atan2 (x, sqrt (1.0 - x*x)); }
static inline double acos(double x) { return atan2 (sqrt (1.0 - x*x), x); }
static inline double fmod(double x, double y) { return x - (double)(long)(x/y)*y; }
static inline double fmodf(float x, float y)  { return fmod((double)x,(double)y); }
static inline double ldexp(double x, int e) {
  union { double d; unsigned long long i; } u;
  u.d = x;
  int ex = (int)((u.i >> 52) & 0x7FF);
  u.i = (u.i & 0x800FFFFFFFFFFFFFull) | ((unsigned long long)(ex+e) << 52);
  return u.d;
}
static inline double frexp(double x, int *e) {
  union { double d; unsigned long long i; } u;
  u.d = x;
  *e = (int)((u.i >> 52) & 0x7FF) - 1022;
  u.i = (u.i & 0x800FFFFFFFFFFFFFull) | 0x3FE0000000000000ull;
  return u.d;
}
static inline double modf(double x, double *iptr) {
  *iptr = (double)(long)x;
  return x - *iptr;
}

#ifdef HAVE_ICONV
#undef HAVE_ICONV
#endif
#define HAVE_ICONV 0

#ifdef VOID
#undef VOID
#endif
typedef void VOID;

#ifdef MAX
#undef MAX
#endif
#ifdef MIN
#undef MIN
#endif

#undef TRUE
#define TRUE  1
#undef FALSE
#define FALSE 0

#undef NULL
#define NULL  ((void *)0)

#endif // UEFI_SHIM_H
