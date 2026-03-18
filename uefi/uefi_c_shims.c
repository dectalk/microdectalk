//
// uefi_c_shims.c — out-of-line C stdlib shims that must survive LTO
//

#include <uefi_shim.h>

// Define GUIDs here so they have a single linkable address
EFI_GUID myEfiPciIoProtocolGuid = EFI_PCI_IO_PROTOCOL_GUID;

int strcasecmp(const char *a, const char *b) {
    while (*a && *b) {
        unsigned char ca = (unsigned char)*a, cb = (unsigned char)*b;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return (int)ca - (int)cb;
        a++; b++;
    }
    return (int)(unsigned char)*a - (unsigned char)*b;
}

int strncasecmp(const char *a, const char *b, size_t n) {
    while (n && *a && *b) {
        unsigned char ca = (unsigned char)*a, cb = (unsigned char)*b;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return (int)ca - (int)cb;
        a++; b++; n--;
    }
    return n ? ((int)(unsigned char)*a - (unsigned char)*b) : 0;
}

int sprintf (char *b, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vsprintf(b, fmt, ap);
    va_end(ap);
    return r;
}

int snprintf(char *b, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(b, n, fmt, ap);
    va_end(ap);
    return r;
}
