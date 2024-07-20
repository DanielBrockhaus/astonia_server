/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "server.h"
#include "mem.h"
#include "rodar_helper.h"
#include "database.h"
#include "log.h"

static pthread_mutex_t cache_mutex=PTHREAD_MUTEX_INITIALIZER;

struct rodar_data rodar_data;

enum teamstatus rodar_teamstatus(char *val) {
    switch (val[0]) {
        case 'a':   return TEAM_ACTIVE;
        case 'b':   return TEAM_BANNED;
        default:    return TEAM_RETIRED;
    }
}

char *rodar_teamstatus2(enum teamstatus status) {
    switch (status) {
        case TEAM_ACTIVE:   return "active";
        case TEAM_BANNED:   return "banned";
        default:            return "retired";
    }
}

enum membertype rodar_membertype(char *val) {
    switch (val[0]) {
        case 'o':   return MEMBER_OWNER;
        case 'a':   return MEMBER_ADMIN;
        default:    return MEMBER;
    }
}

char *rodar_membertype2(enum membertype type) {
    switch (type) {
        case MEMBER_OWNER:  return "owner";
        case MEMBER_ADMIN:  return "admin";
        default:            return "member";
    }
}

enum eventtype rodar_eventtype(char *val) {
    switch (val[0]) {
        case '2':   return EVENT2;
        case '3':   return EVENT3;
        case '5':   return EVENT5;
        case '7':   return EVENT7;
        case '1':   return EVENT12;
        default:    return EVENT_ANY;
    }
}

char *rodar_eventtype2(enum eventtype type) {
    switch (type) {
        case EVENT2:    return "2";
        case EVENT3:    return "3";
        case EVENT5:    return "5";
        case EVENT7:    return "7";
        case EVENT12:   return "12";
        default:        return "any";
    }
}

enum eventopt rodar_eventopt(char *val) {
    enum eventopt opt=EVENT_NONE;

    if (strstr(val,"nomagic")) opt|=EVENT_NOMAGIC;
    if (strstr(val,"nofreeze")) opt|=EVENT_NOFREEZE;
    if (strstr(val,"nowarcry")) opt|=EVENT_NOWARCRY;

    return opt;
}

char *rodar_eventopt2(enum eventopt opt) {
    static char opts[256];
    char *ptr=opts;
    int comma=0;

    *ptr=0;
    if (opt&EVENT_NOMAGIC) ptr+=sprintf(ptr,"%s%s",comma++?",":"","nomagic");
    if (opt&EVENT_NOFREEZE) ptr+=sprintf(ptr,"%s%s",comma++?",":"","nofreeze");
    if (opt&EVENT_NOWARCRY) ptr+=sprintf(ptr,"%s%s",comma++?",":"","nowarcry");

    return opts;
}


#define MAXTEAM     64

static struct rodar_team team_cache[MAXTEAM]={0};
static int team_idx=0;

void rodar_cache_team(char *name_or_ID,struct rodar_team *team) {
    int n;
    int tID=atoi(name_or_ID);

    pthread_mutex_lock(&cache_mutex);

    for (n=0; n<MAXTEAM; n++) {
        if (tID) {
            if (team_cache[n].ID==tID) break;
        } else {
            if (!strcasecmp(team_cache[n].name,name_or_ID)) break;
        }
    }
    if (n==MAXTEAM) n=(team_idx+1)%MAXTEAM;

    if (team) {
        team_cache[n]=*team;
    } else { // negative cache
        bzero(&team_cache[n],sizeof(team_cache[n]));
        strcpy(team_cache[n].name,name_or_ID);
    }

    pthread_mutex_unlock(&cache_mutex);
}

int rodar_team_byname(char *name,struct rodar_team *team) {
    int n;
    int teamID;

    pthread_mutex_lock(&cache_mutex);

    for (n=0; n<MAXTEAM; n++)
        if (!strcasecmp(team_cache[n].name,name)) break;

    if (n==MAXTEAM) {
        pthread_mutex_unlock(&cache_mutex);

        if (team) bzero(team,sizeof(struct rodar_team));
        return -1;
    }

    teamID=team_cache[n].ID;

    if (team) *team=team_cache[n];

    pthread_mutex_unlock(&cache_mutex);

    return teamID;
}

int rodar_team_byID(int teamID,struct rodar_team *team) {
    int n;

    pthread_mutex_lock(&cache_mutex);

    for (n=0; n<MAXTEAM; n++)
        if (team_cache[n].ID==teamID) break;

    if (n==MAXTEAM) {
        pthread_mutex_unlock(&cache_mutex);
        if (team) bzero(team,sizeof(struct rodar_team));
        return -1;
    }

    if (team) *team=team_cache[n];

    pthread_mutex_unlock(&cache_mutex);

    return 0;
}

#define MAXMEMBER   256

static struct rodar_member member_cache[MAXMEMBER]={0};
static int member_idx=0;

void rodar_cache_member(int teamID,int charID,int type) {
    int n;

    pthread_mutex_lock(&cache_mutex);

    for (n=0; n<MAXMEMBER; n++) {
        if (member_cache[n].teamID==teamID && member_cache[n].charID==charID) break;
    }
    if (n==MAXMEMBER) n=(member_idx+1)%MAXMEMBER;

    member_cache[n].teamID=teamID;
    member_cache[n].charID=charID;
    member_cache[n].type=type;

    pthread_mutex_unlock(&cache_mutex);
}

int rodar_member(int teamID,int charID) {
    int n;
    enum membertype type;

    pthread_mutex_lock(&cache_mutex);

    for (n=0; n<MAXMEMBER; n++)
        if (member_cache[n].teamID==teamID && member_cache[n].charID==charID) break;

    if (n==MAXMEMBER) {
        pthread_mutex_unlock(&cache_mutex);
        return -1;
    }

    type=member_cache[n].type;

    pthread_mutex_unlock(&cache_mutex);

    return type;
}

#define MAXSCHED    672 // 7 days, 24 hours, 4 events per hour

static struct rodar_event events[MAXSCHED];
static int event_cnt,event_loaded=0;

void rodar_read_event(void) {

    event_loaded=0; // atomic, no mutex needed

    db_read_event();
}

void rodar_reset_event(void) {
    event_loaded=0;
    event_cnt=0;
}

void rodar_event_done(void) {
    event_loaded=1;
}

int rodar_event_loaded(void) {
    return event_loaded;
}

void rodar_add_event(int ID,int t,enum eventtype type,enum eventopt opt,int level,int room,int winnerID) {
    int n;

    pthread_mutex_lock(&cache_mutex);
    n=event_cnt++;

    if (n>=MAXSCHED) {
        pthread_mutex_unlock(&cache_mutex);
        return;
    }

    events[n].ID=ID;
    events[n].t=t;
    events[n].type=type;
    events[n].opt=opt;
    events[n].level=level;
    events[n].room=room;
    events[n].winnerID=winnerID;

    pthread_mutex_unlock(&cache_mutex);
}

int rodar_get_event(int idx,struct rodar_event *ev) {

    pthread_mutex_lock(&cache_mutex);

    if (idx>=event_cnt) {
        pthread_mutex_unlock(&cache_mutex);
        return 0;
    }

    if (ev) *ev=events[idx];

    pthread_mutex_unlock(&cache_mutex);
    return 1;
}

int rodar_get_event_cnt(void) {
    int cnt;

    pthread_mutex_lock(&cache_mutex);

    cnt=event_cnt;  // this is atomic, no need for mutex?

    pthread_mutex_unlock(&cache_mutex);

    return cnt;
}

static int rand_level(void) {
    int r;

    //r=RANDOM(4);
    r=0;

    switch (r) {
        case 0: return  28;
        case 1: return  44;
        case 2: return  60;
        case 3: return  76;
        default:    return 200;
    }
}

static int rand_room(void) {
    int r;

    r=RANDOM(2)+1;

    return r;
}

static enum eventtype rand_type(void) {
    int r;

    r=RANDOM(10);

    switch (r) {
        case 0: return EVENT2;
        case 1: return EVENT3;
        case 2: return EVENT5;
        case 3: return EVENT7;
        case 4: return EVENT7;
        case 5: return EVENT12;
        case 6: return EVENT12;
        case 7: return EVENT12;
        default: return EVENT_ANY;
    }
}

static enum eventopt rand_opt(void) {
    enum eventopt opt=EVENT_NONE;

    /*
    if (RANDOM(10)==1) opt|=EVENT_NOMAGIC;
    else if (RANDOM(10)==1) opt|=EVENT_NOFREEZE;
    if (RANDOM(10)==1) opt|=EVENT_NOWARCRY;
    */

    return opt;
}

void rodar_create_event(int when) {

    when=(when/(60*60)+1)*60*60;

    db_add_event(when,rand_type(),rand_opt(),rand_level(),rand_room());
    event_loaded=0;
    db_read_event();
}

void rodar_cleanup_event(void) {
    int n;

    for (n=0; n<rodar_data.at_cnt; n++)
        if (rodar_data.at[n].charID)
            xfree(rodar_data.at[n].charID);
    if (rodar_data.at) xfree(rodar_data.at);

    rodar_data.at=NULL;
    rodar_data.at_max=rodar_data.at_cnt=0;
}

int rodar_is_in_event(int teamID,int charID) {
    int n,i;

    for (n=0; n<rodar_data.at_cnt; n++)
        for (i=0; i<rodar_data.at[n].char_cnt; i++)
            if (rodar_data.at[n].charID[i]==charID) {
                if (rodar_data.at[n].teamID==teamID) return 1;
                else return -1;
            }

    return 0;
}

int rodar_chars_in_event(int teamID) {
    int n;

    for (n=0; n<rodar_data.at_cnt; n++)
        if (rodar_data.at[n].teamID==teamID)
            return rodar_data.at[n].char_cnt;

    return 0;
}

int rodar_add_to_event(int teamID,int charID) {
    int n,i;

    for (n=0; n<rodar_data.at_cnt; n++)
        if (rodar_data.at[n].teamID==teamID)
            break;
    if (n==rodar_data.at_cnt) {
        if (rodar_data.at_cnt>=rodar_data.at_max) {
            rodar_data.at_max+=16;
            rodar_data.at=xrealloc(rodar_data.at,sizeof(struct rodar_active_team)*rodar_data.at_max,IM_TEMP);
        }
        bzero(&rodar_data.at[n],sizeof(struct rodar_active_team));
        rodar_data.at[n].teamID=teamID;
        rodar_data.at_cnt++;
    }

    for (i=0; i<rodar_data.at[n].char_cnt; i++)
        if (rodar_data.at[n].charID[i]==charID) break;
    if (i==rodar_data.at[n].char_cnt) {
        rodar_data.at[n].char_max+=16;
        rodar_data.at[n].charID=xrealloc(rodar_data.at[n].charID,sizeof(int)*rodar_data.at[n].char_max,IM_TEMP);
        rodar_data.at[n].charID[i]=charID;
        rodar_data.at[n].char_cnt++;
        return 1;
    }

    return 0;
}

int rodar_event_maxchars(enum eventtype type) {
    switch (type) {
        case EVENT2:    return 2;
        case EVENT3:    return 3;
        case EVENT5:    return 5;
        case EVENT7:    return 7;
        case EVENT12:   return 12;
        default:        return 255;
    }
}

int rodar_time_till_event(void) {
    if (!rodar_data.ev.ID) return 60*60;

    return rodar_data.ev.t-time_now;
}

void rodar_load_winner(void) {
    if (!rodar_data.ev.winnerID) return;

    if (rodar_team_byID(rodar_data.ev.winnerID,&rodar_data.win_team))
        db_read_team_byID(rodar_data.ev.winnerID);
}

void rodar_end_event(int winnerID) {
    int n;

    if (rodar_data.ev.ID && winnerID) {
        db_win_event(rodar_data.ev.ID,winnerID);
        rodar_data.ev.winnerID=winnerID;

        db_inc_team_value(winnerID,"wins",1);
        db_inc_team_value(winnerID,"score",10);

        rodar_load_winner();

        for (n=0; n<rodar_data.at_cnt; n++) {
            if (rodar_data.at[n].teamID==winnerID) continue;
            db_inc_team_value(rodar_data.at[n].teamID,"losses",1);
            db_inc_team_value(rodar_data.at[n].teamID,"score",-1);
        }
    }
}

