#ifndef _PORT_H
#define _PORT_H

#include <stdio.h>
#include <stdint.h>

#define __far
#define _far
#define far
#define huge
#define _huge

typedef uint16_t DT_PIPE_T;
typedef int32_t S32;
typedef uint32_t U32;
typedef int16_t S16;
typedef uint16_t U16;
typedef int8_t S8;
typedef uint8_t U8;

typedef unsigned long long QWORD;

typedef uint32_t DWORD;
typedef uint16_t USHORT;
typedef void * PVOID;
typedef uint32_t UINT;

#define IsBadWritePtr(ptr, size) ((ptr == NULL) ? 1 : 0)
#define _stricmp strcasecmp
#define PRINTFDEBUG

#endif /* _PORT_H */


