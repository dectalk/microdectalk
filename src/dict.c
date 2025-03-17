#include "lsdef.h"
#include "lsconst.h"
#include "kernel.h"
#include <stdint.h>
#include <stdio.h>

// defines for main and user dictionary pointers
unsigned char *mdict;
unsigned char *udict;

// ls1 externs
extern int   lbphone;
extern int   rbphone;

#define get_long_int(ptr) ((uint32_t)\
                       ((((uint8_t *)(ptr))[3] << 24)  | \
                        (((uint8_t *)(ptr))[2] << 16)  | \
                        (((uint8_t *)(ptr))[1] << 8)  | \
                        (((uint8_t *)(ptr))[0])))

extern unsigned long   fc_struct[256];
extern int     fc_index;

char *d_lookup(LETTER llp[], char table[]) {
    // bytes 0-3
    int entries = get_long_int((char *)mdict);
    // bytes 4-7
    int bytes = get_long_int((char *) &mdict[4]);
    char *indices = &mdict[8];
    char *data = &mdict[(entries * 4) + 8];

    for (int j = 0; j < entries; j++) {
        int test = 1;
        int ind = get_long_int(&indices[j * 4]);
        char *entry = &data[ind + 4];
        int i; // finds beginning of phoneme data
        
        // Match the word part
        for (i = 0; entry[i] != 0; i++) {
            // Using ls_lower[] mapping from second function for case insensitivity
            if (entry[i] != ls_lower[llp[i].l_ch]) {
                test = 0;
                break;
            }
        }

        if (ls_lower[ llp[i].l_ch ] != 0) {
            continue;
        }
        if (!test) {
            continue;
        }

        

        // If we found a match, return pointer to the phoneme data
        i++; // Skip the null terminator between word and phonemes
        
        #ifdef FULL_LTS
        // Preserve the form class logic from second function
        if (ls_lower[llp[0].l_ch] == 't') fc_struct[fc_index] = FC_TO | FC_PREP | FC_FUNC;
        if (ls_lower[llp[0].l_ch] == 'a') fc_struct[fc_index] = FC_CONJ | FC_VERB | FC_FUNC;
        if (ls_lower[llp[0].l_ch] == 'f') fc_struct[fc_index] = FC_ADV | FC_PREP | FC_NEG;
        #endif
        
        return &entry[i]; // Return pointer to phoneme data
    }
    
    return NULL; // No match found
}

// check wlookup for the desired word in both the user and main dictionary
int lookup(LETTER *llp, LETTER *rlp, int context) {
    char *cp;


    // check if main dictionary is valid, then perform lookup
    if (mdict && (cp = (char *)d_lookup(llp,&mdict[0])) != NULL) {
        sendlist(cp);
        lbphone=WBOUND;
        rbphone=WBOUND;
        return (HIT);
    }

    return(MISS);
}
