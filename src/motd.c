/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "server.h"
#include "mem.h"
#include "log.h"
#include "talk.h"
#include "player.h"
#include "motd.h"

static int motd_time;
static char *motd=NULL;

int read_motd(void) {
    struct stat st;
    int handle,len,tmp;

    if (stat("motd.txt",&st)) {
        elog("Error stat-ing motd");
        return 0;
    }
    if (st.st_mtime>motd_time) {
        handle=open("motd.txt",O_RDONLY);
        if (handle==-1) { elog("Error opening motd"); return 0; }

        len=lseek(handle,0,SEEK_END);
        lseek(handle,0,SEEK_SET);
        motd=xrealloc(motd,len+1,IM_BASE);
        if ((tmp=read(handle,motd,len))!=len) {
            elog("motd expected size %d, but got size %d.",len,tmp);
            motd[0]=0;
        } else motd[len]=0;

        close(handle);

        xlog("Read MotD");

        motd_time=st.st_mtime;
    }
    return 1;
}

void show_motd(int nr) {
    char buf[1024],*a,*b;

    for (a=motd,b=buf; *a; a++) {
        if (*a=='\n') {
            *b=0; b=buf;
            log_player(nr,LOG_SYSTEM,"%s",buf);
        } else *b++=*a;
    }
}

#ifdef STAFF
void check_staff_stop(void) {
    struct stat st;

    if (areaID==27 && !stat("./zones/27/stop",&st)) {
        if (!unlink("./zones/27/stop")) {
            quit=1;
        }
    }
    if (areaID==27 && !stat("./zones/27/stop.txt",&st)) {
        if (!unlink("./zones/27/stop.txt")) {
            quit=1;
        }
    }
}
int check_staff_start(void) {
    struct stat st;

    if (areaID!=27) return 1;

    if (!stat("./zones/27/start",&st)) {
        if (!unlink("./zones/27/start")) {
            return 1;
        }
    }
    if (!stat("./zones/27/start.txt",&st)) {
        if (!unlink("./zones/27/start.txt")) {
            return 1;
        }
    }
    return 0;
}

#endif
