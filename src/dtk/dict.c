#include "lsdef.h"
#include "lsconst.h"
#include "kernel.h"

extern const unsigned char user_dict[];

/*
 * Look up a word. The word is stored
 * in an array of LETTER structures, bracketed by
 * the "llp" and "rlp" pointers. The "context" is drawn
 * from the set in "lsdef.h". Return a standard lookup
 * status code. Output index marks on success.
 */

int lookup(LETTER *llp, LETTER *rlp, int context) {
    char *cp;
    if ( (cp = (char *)wlookup(llp,&user_dict[0])) != NULL) {       /*eab mini dictionary lookup */
        sendlist(cp);
//        lbphone=WBOUND;
//        rbphone=WBOUND;
        return (HIT);
    }
    return(MISS);
}
