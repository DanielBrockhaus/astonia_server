/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

#include <stdlib.h>
#include <string.h>
#include "rodar_helper.h"

enum teamtype rodar_teamtype(char *val) {
    switch (val[0]) {
        case '2':   return TEAM2;
        case '3':   return TEAM3;
        case '5':   return TEAM5;
        case '7':   return TEAM7;
        case '1':   return TEAM12;
        default:    return TEAM_ANY;
    }
}

enum teamstatus rodar_teamstatus(char *val) {
    switch (val[0]) {
        case 'a':   return TEAM_ACTIVE;
        case 'b':   return TEAM_BANNED;
        default:    return TEAM_RETIRED;
    }
}

char *rodar_teamtype2(enum teamtype type) {
    switch (type) {
        case TEAM2:     return "2";
        case TEAM3:     return "3";
        case TEAM5:     return "5";
        case TEAM7:     return "7";
        case TEAM12:    return "12";
        default:        return "any";
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

#define MAXTEAM     64

static struct rodar_team team_cache[MAXTEAM]={0};
static int team_idx=0;

void rodar_cache_team(char *name_or_ID,struct rodar_team *team) {
    int n;
    int tID=atoi(name_or_ID);

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
}

int rodar_team_byname(char *name,struct rodar_team *team) {
    int n;

    for (n=0; n<MAXTEAM; n++)
        if (!strcasecmp(team_cache[n].name,name)) break;
    if (n==MAXTEAM) {
        if (team) bzero(team,sizeof(struct rodar_team));
        return -1;
    }

    if (team) *team=team_cache[n];

    return team_cache[n].ID;
}

#define MAXMEMBER   256

static struct rodar_member member_cache[MAXMEMBER]={0};
static int member_idx=0;

void rodar_cache_member(int teamID,int charID,int type) {
    int n;

    for (n=0; n<MAXMEMBER; n++) {
        if (member_cache[n].teamID==teamID && member_cache[n].charID==charID) break;
    }
    if (n==MAXMEMBER) n=(member_idx+1)%MAXMEMBER;

    member_cache[n].teamID=teamID;
    member_cache[n].charID=charID;
    member_cache[n].type=type;
}

int rodar_member(int teamID,int charID) {
    int n;

    for (n=0; n<MAXMEMBER; n++)
        if (member_cache[n].teamID==teamID && member_cache[n].charID==charID) break;
    if (n==MAXMEMBER) return -1;

    return member_cache[n].type;
}

/*
Tables

Teams:
founderID = player ID of the founder. might come in handy.

create table rodar_team (
    ID int not null auto_increment,
    name char(80) not null,
    founderID int,
    founded timestamp not null default now(),
    type enum ('2','3','5','7','12','any') not null default 'any',
    status enum ('active','banned','retired') not null default 'active',
    wins int not null default 0,
    losses int not null default 0,
    kills int not null default 0,
    killed int not null default 0,
    score int not null default 0,
    primary key(ID),
    unique key(name),
    key(score),
    foreign key(founderID) references chars(ID) on delete set null
);

Team Members:
teamID = Team ID, ID from rodar_teams
charID = character ID, ID from chars

create table rodar_member (
    teamID int not null,
    charID int not null,
    type enum ('member','admin','owner') not null,
    primary key(teamID,charID),
    key(charID),
    foreign key(charID) references chars(ID) on delete cascade
);

Events Schedule:

t = date/time the event starts
winnerID = winning team ID from rodar_team

Events will be created one week in advance.

winnerID = ID from rodar_teams

create table rodar_schedule (
    ID int not null auto_increment,
    t timestamp not null default 0,
    type enum ('2','3','5','7','12','any') not null default 'any',
    option set ('open','clan','nomagic','nofreeze','nowarcry') not null default (''),
    winnerID int default null,
    primary key(ID),
    key(t)
);

We might want to add more logging eventually, like individual kills.

 */
