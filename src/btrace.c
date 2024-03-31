/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

#include <stdlib.h>
#include <execinfo.h>

#include "log.h"

void btrace(char *msg) {
    int i,n;
    void *ba[128];
    char **names;

    if ((n=backtrace(ba,sizeof(ba)/sizeof(ba[0])))!=0) {
        names=backtrace_symbols(ba,n);
        for (i=n-3; i>0; i--) {
            elog("%s: %2d: %s",msg,i,names[i]);
        }
        free(names);
    }
}


