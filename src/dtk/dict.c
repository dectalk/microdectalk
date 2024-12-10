#include "lsdef.h"
#include "lsconst.h"
#include "kernel.h"

// defines for main and user dictionary pointers
unsigned char *mdict;
unsigned char *udict;

// ls1 externs
extern int   lbphone;
extern int   rbphone;

// check wlookup for the desired word in both the user and main dictionary
int lookup(LETTER *llp, LETTER *rlp, int context) {
    char *cp;

    // check if main dictionary is valid, then perform lookup
    if (mdict && (cp = (char *)wlookup(llp,&mdict[0])) != NULL) {
        sendlist(cp);
        lbphone=WBOUND;
        rbphone=WBOUND;
        return (HIT);
    }
    // check if user dictionary is valid, then perform lookup
    if (udict && (cp = (char *)wlookup(llp,&udict[0])) != NULL) {
        sendlist(cp);
        lbphone=WBOUND;
        rbphone=WBOUND;
        return (HIT);
    }

    return(MISS);
}
