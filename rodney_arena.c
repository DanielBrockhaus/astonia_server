/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

/*

insert into area values (38,1,'Rodneys Arena',0,0,0,0,0,0,0,0);
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "server.h"
#include "libload.h"
#include "notify.h"
#include "drvlib.h"
#include "direction.h"
#include "do.h"
#include "log.h"
#include "talk.h"
#include "create.h"
#include "command.h"
#include "drdata.h"
#include "rodar_helper.h"
#include "database.h"
#include "lookup.h"

// library helper functions needed for init
int ch_driver(int nr,int cn,int ret,int lastact);           // character driver (decides next action)
int it_driver(int nr,int in,int cn);                    // item driver (special cases for use)
int ch_died_driver(int nr,int cn,int co);               // called when a character dies
int ch_respawn_driver(int nr,int cn);                   // called when an NPC is about to respawn
int special_driver(int nr,int obj,int ret,int lastact);

// EXPORTED - character/item driver
int driver(int type,int nr,int obj,int ret,int lastact) {
    switch (type) {
        case CDT_DRIVER:	return ch_driver(nr,obj,ret,lastact);
        case CDT_ITEM: 		return it_driver(nr,obj,ret);
        case CDT_DEAD:		return ch_died_driver(nr,obj,ret);
        case CDT_RESPAWN:	return ch_respawn_driver(nr,obj);
        case CDT_SPECIAL:	return special_driver(nr,obj,ret,lastact);
        default: 	return 0;
    }
}

void rodarmaster(int cn,int ret,int lastact) {
    struct msg *msg,*next;

    //dat=set_data(cn,DRD_RANDOMMASTER,sizeof(struct master_data));
    //if (!dat) return;   // oops...

    // loop through our messages
    for (msg=ch[cn].msg; msg; msg=next) {
        next=msg->next;
        remove_message(cn,msg);
    }

    if (spell_self_driver(cn)) return;

    if (secure_move_driver(cn,ch[cn].tmpx,ch[cn].tmpy,DX_RIGHT,ret,lastact)) return;

    do_idle(cn,TICKS);
}

static int cmd_rodar(int cn) {

    log_char(cn,LOG_SYSTEM,0,"Rodar Help");
    log_char(cn,LOG_SYSTEM,0,"%s","");
    log_char(cn,LOG_SYSTEM,0,"/join <team name> - join team");
    log_char(cn,LOG_SYSTEM,0,"/leave <team name> - leave team");
    log_char(cn,LOG_SYSTEM,0,"/activate <team name> - activate team membership");
    log_char(cn,LOG_SYSTEM,0,"/hire <char name> - hire player to active team");
    log_char(cn,LOG_SYSTEM,0,"/fire <char name> - fire player from active team");
    log_char(cn,LOG_SYSTEM,0,"/promote <char name> - promote player to admin in the active team");
    log_char(cn,LOG_SYSTEM,0,"/demote <char name> - demote player from admin to member");
    log_char(cn,LOG_SYSTEM,0,"/found <team name> - found a new team (1000G)");

    if (ch[cn].flags&CF_GOD) {
        log_char(cn,LOG_SYSTEM,0,"/setteam <team name> - temporarily join team");
        log_char(cn,LOG_SYSTEM,0,"/rename <new team name> - rename active team");
        log_char(cn,LOG_SYSTEM,0,"/ban <team name> - ban team");
        log_char(cn,LOG_SYSTEM,0,"/unban <team name> - unban team");
    }

    return 2;
}

static int cmd_found(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /found <name>. The name must be letters and spaces only.");
            return 2;
        }
    }

    // eat trailing spaces
    while (tmp[-1]==' ') tmp--;
    tmp[0]=0;

    if (tmp-ptr<3) {
        log_char(cn,LOG_SYSTEM,0,"Team name must be at least three letters.");
        return 2;
    }
    if (tmp-ptr>42) {
        log_char(cn,LOG_SYSTEM,0,"Team name must be at most 42 letters.");
        return 2;
    }

    if (ch[cn].gold<1000*100) {
        log_char(cn,LOG_SYSTEM,0,"You cannot afford to found a team. The fee is 1000G.");
        return 2;
    }

    // check if the team already exists. asynchronous database access is a pain.
    if (rodar_team_byname(ptr,&team)==0) {
        // team doesn't exist yet, create it
        db_create_team(ptr,ch[cn].ID);
        db_read_team(ptr);
        return 3;
    }

    // team was loaded and founder is cn
    if (team.ID && team.founderID==ch[cn].ID) {
        log_char(cn,LOG_SYSTEM,0,"Success. You've founded the team '%s'!",team.name);
        // TODO: we waive the fee if the team is older than 3 seconds. this is a hack and should be fixed eventually.
        if (time_now-team.founded<3) {
            ch[cn].gold-=1000*100;
            ch[cn].flags|=CF_ITEMS;
        }
        db_add_team_member(team.ID,ch[cn].ID,MEMBER_OWNER);
        return 2;
    } else if (team.ID) {   // team was loaded and founder is not cn
        log_char(cn,LOG_SYSTEM,0,"Failure. There is already a team called '%s'!",team.name);
        return 2;
    } else {    // team wasn't loaded
        db_read_team(ptr);
        return 3;
    }
}

static int cmd_join(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /join <name>. The name must be letters and spaces only.");
            return 2;
        }
    }

    // team not found
    if (rodar_team_byname(ptr,&team)==-1) {
        db_read_team(ptr);
        return 3;
    }

    // load team
    if (!team.ID) {
        log_char(cn,LOG_SYSTEM,0,"No team by that name.");
        return 2;
    }

    dat=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat) return 2;

    dat->joinID=team.ID;

    log_char(cn,LOG_SYSTEM,0,"Team %s membership requested. An admin or the owner must now hire you.",team.name);

    return 2;
}

static int cmd_leave(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;
    enum membertype type;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /leave <name>. The name must be letters and spaces only.");
            return 2;
        }
    }

    // team not found
    if (rodar_team_byname(ptr,&team)==-1) {
        db_read_team(ptr);
        return 3;
    }

    // load team
    if (!team.ID) {
        log_char(cn,LOG_SYSTEM,0,"No team by that name.");
        return 2;
    }

    if ((type=rodar_member(team.ID,ch[cn].ID))==-1) {
        db_read_member(team.ID,ch[cn].ID);
        return 3;
    }

    if (type==MEMBER_NONE) {
        log_char(cn,LOG_SYSTEM,0,"You are not member of team %s.",team.name);
        return 2;
    }

    db_del_team_member(team.ID,ch[cn].ID);
    db_read_member(team.ID,ch[cn].ID);

    dat=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (dat && dat->teamID==team.ID) {
        dat->teamID=0;
        dat->memtype=0;
    }

    log_char(cn,LOG_SYSTEM,0,"You left team %s.",team.name);

    return 2;
}

static int cmd_activate(int cn,char *ptr,int godmode) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;
    int type;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /activate <name>. The name must be letters and spaces only.");
            return 2;
        }
    }

    // load team
    if (rodar_team_byname(ptr,&team)==-1) {
        db_read_team(ptr);
        return 3;
    }

    if (!team.ID) {
        log_char(cn,LOG_SYSTEM,0,"No team by that name.");
        return 2;
    }

    if (team.status!=TEAM_ACTIVE) {
        log_char(cn,LOG_SYSTEM,0,"That team has been %s.",rodar_teamstatus2(team.status));
        return 2;
    }

    if (godmode) type=MEMBER_OWNER;
    else if ((type=rodar_member(team.ID,ch[cn].ID))==-1) {
        db_read_member(team.ID,ch[cn].ID);
        return 3;
    }

    if (type==MEMBER_NONE) {
        log_char(cn,LOG_SYSTEM,0,"You are not member of team %s.",team.name);
        return 2;
    }

    dat=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat) return 2;

    dat->teamID=team.ID;
    dat->memtype=type;

    log_char(cn,LOG_SYSTEM,0,"Team %s membership of rank %s activated.",team.name,rodar_membertype2(type));

    return 2;
}

// why isn't this in tool.c?
int find_char_byname(char *name) {
    int co;

    for (co=getfirst_char(); co; co=getnext_char(co)) {
        if (!(ch[co].flags&CF_PLAYER)) continue;
        if (!strcasecmp(name,ch[co].name)) break;
    }
    return co;
}

static int cmd_hire(int cn,char *ptr) {
    char *tmp;
    struct rodar_drd *dat1,*dat2;
    int co;
    enum membertype type;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp)) {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /hire <name>. The name must be letters only.");
            return 2;
        }
    }

    dat1=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat1) return 2;

    if (!dat1->teamID) {
        log_char(cn,LOG_SYSTEM,0,"You have not activated your team membership.");
        return 2;
    }
    if (dat1->memtype!=MEMBER_ADMIN && dat1->memtype!=MEMBER_OWNER) {
        log_char(cn,LOG_SYSTEM,0,"You lack the required rank in your team.");
        return 2;
    }

    co=find_char_byname(ptr);
    if (!co) {
        log_char(cn,LOG_SYSTEM,0,"No player by that name in this area.");
        return 2;
    }

    dat2=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat2) return 2;

    if (dat2->joinID!=dat1->teamID) {
        log_char(cn,LOG_SYSTEM,0,"%s has not asked to join your team.",ch[co].name);
        return 2;
    }

    if ((type=rodar_member(dat1->teamID,ch[co].ID))==-1) {
        db_read_member(dat1->teamID,ch[co].ID);
        return 3;
    }

    if (type!=MEMBER_NONE) {
        log_char(cn,LOG_SYSTEM,0,"%s is already a member of your team.",ch[co].name);
        return 2;
    }

    db_add_team_member(dat1->teamID,ch[co].ID,MEMBER);
    db_read_member(dat1->teamID,ch[co].ID);

    log_char(cn,LOG_SYSTEM,0,"%s is now a member of your team.",ch[co].name);
    log_char(co,LOG_SYSTEM,0,"Your team application has been accepted by %s.",ch[cn].name);

    return 2;
}


static int cmd_fire(int cn,char *ptr) {
    char *tmp,name[80];
    struct rodar_drd *dat1,*dat2;
    int co,coID;
    enum membertype type;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp)) {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /fire <name>. The name must be letters only.");
            return 2;
        }
    }

    dat1=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat1) return 2;

    if (!dat1->teamID) {
        log_char(cn,LOG_SYSTEM,0,"You have not activated your team membership.");
        return 2;
    }
    if (dat1->memtype!=MEMBER_ADMIN && dat1->memtype!=MEMBER_OWNER) {
        log_char(cn,LOG_SYSTEM,0,"You lack the required rank in your team.");
        return 2;
    }

    coID=lookup_name(ptr,name);
    if (!coID) return 3;    // waiting for database

    if (coID==-1) {
        log_char(cn,LOG_SYSTEM,0,"No player by that name.");
        return 2;
    }

    if ((type=rodar_member(dat1->teamID,coID))==-1) {
        db_read_member(dat1->teamID,coID);
        return 3;
    }

    if (type==MEMBER_NONE) {
        log_char(cn,LOG_SYSTEM,0,"%s is not a member of your team.",name);
        return 2;
    }

    if (type==MEMBER_OWNER) {
        log_char(cn,LOG_SYSTEM,0,"You cannot fire the owner of a team.");
        return 2;
    }

    if (type==MEMBER_ADMIN && dat1->memtype!=MEMBER_OWNER) {
        log_char(cn,LOG_SYSTEM,0,"Only the owner can fire admins.");
        return 2;
    }

    db_del_team_member(dat1->teamID,coID);
    db_read_member(dat1->teamID,coID);

    co=find_char_byname(ptr);
    if (co) {
        dat2=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
        if (dat2) {
            if (dat2->teamID==dat1->teamID) {
                dat1->teamID=0;
                dat1->memtype=0;
                log_char(co,LOG_SYSTEM,0,"You have been fired from your active team.");
            }
        }
    }

    log_char(cn,LOG_SYSTEM,0,"%s has been fired.",name);

    return 2;
}

static int cmd_promote(int cn,char *ptr,int promote) {
    char *tmp,name[80];
    struct rodar_drd *dat1,*dat2;
    int co,coID;
    enum membertype type;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp)) {
            log_char(cn,LOG_SYSTEM,0,"Syntax: %s <name>. The name must be letters only.",promote?"/promote":"/demote");
            return 2;
        }
    }

    dat1=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat1) return 2;

    if (!dat1->teamID) {
        log_char(cn,LOG_SYSTEM,0,"You have not activated your team membership.");
        return 2;
    }
    if (dat1->memtype!=MEMBER_OWNER) {
        log_char(cn,LOG_SYSTEM,0,"Only the owner can promote or demote.");
        return 2;
    }

    coID=lookup_name(ptr,name);
    if (!coID) return 3;    // waiting for database

    if (coID==-1) {
        log_char(cn,LOG_SYSTEM,0,"No player by that name.");
        return 2;
    }

    if ((type=rodar_member(dat1->teamID,coID))==-1) {
        db_read_member(dat1->teamID,coID);
        return 3;
    }

    if (type==MEMBER_NONE) {
        log_char(cn,LOG_SYSTEM,0,"%s is not a member of your team.",name);
        return 2;
    }

    if (type==MEMBER_OWNER) {
        log_char(cn,LOG_SYSTEM,0,"You cannot change the rank of the owner.");
        return 2;
    }

    if (promote && type==MEMBER_ADMIN) {
        log_char(cn,LOG_SYSTEM,0,"%s is already an admin.",name);
        return 2;
    }
    if (!promote && type==MEMBER) {
        log_char(cn,LOG_SYSTEM,0,"%s is already a normal member.",name);
        return 2;
    }

    db_upd_team_member(dat1->teamID,coID,promote?MEMBER_ADMIN:MEMBER);
    db_read_member(dat1->teamID,coID);

    co=find_char_byname(ptr);
    if (co) {
        dat2=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
        if (dat2) {
            if (dat2->teamID==dat1->teamID) {
                if (promote) {
                    dat2->memtype=MEMBER_ADMIN;
                    log_char(co,LOG_SYSTEM,0,"You have been promoted to admin.");
                } else {
                    dat2->memtype=MEMBER;
                    log_char(co,LOG_SYSTEM,0,"You have been demoted to member.");
                }
            }
        }
    }

    log_char(cn,LOG_SYSTEM,0,"%s has been %s.",name,promote?"promoted":"demoted");

    return 2;
}

static int cmd_rename(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /rename <name>. The name must be letters and spaces only.");
            return 2;
        }
    }

    // eat trailing spaces
    while (tmp[-1]==' ') tmp--;
    tmp[0]=0;

    if (tmp-ptr<3) {
        log_char(cn,LOG_SYSTEM,0,"Team name must be at least three letters.");
        return 2;
    }
    if (tmp-ptr>42) {
        log_char(cn,LOG_SYSTEM,0,"Team name must be at most 42 letters.");
        return 2;
    }

    dat=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat) return 2;

    if (!dat->teamID) {
        log_char(cn,LOG_SYSTEM,0,"You need to /setteam first.");
        return 2;
    }

    // team not found
    if (rodar_team_byname(ptr,&team)==-1) {
        db_read_team(ptr);
        return 3;
    }

    // load team
    if (team.ID) {
        log_char(cn,LOG_SYSTEM,0,"New name already exists.");
        return 2;
    }

    db_write_team_name(dat->teamID,ptr);
    db_read_team_byID(dat->teamID);

    log_char(cn,LOG_SYSTEM,0,"Rename scheduled.");

    return 2;
}

static int cmd_ban(int cn,char *ptr,int doban) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;
    int co;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /%s <name>. The name must be letters and spaces only.",doban?"ban":"unban");
            return 2;
        }
    }

    // team not found
    if (rodar_team_byname(ptr,&team)==-1) {
        db_read_team(ptr);
        return 3;
    }

    // load team
    if (!team.ID) {
        log_char(cn,LOG_SYSTEM,0,"No team by that name.");
        return 2;
    }

    if (doban) db_write_team_status(team.ID,TEAM_BANNED);
    else db_write_team_status(team.ID,TEAM_ACTIVE);
    db_read_team_byID(team.ID);

    if (doban) {
        for (co=getfirst_char(); co; co=getnext_char(co)) {
            if (!(ch[co].flags&CF_PLAYER)) continue;

            dat=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
            if (!dat) continue;

            if (dat->teamID==team.ID) {
                dat->teamID=0;
                dat->memtype=0;
            }
        }
    }

    log_char(cn,LOG_SYSTEM,0,"%s scheduled.",doban?"Ban":"Unban");

    return 2;
}

int rodar_parser(int cn,char *ptr) {
    int len;

    if (*ptr=='#' || *ptr=='/') {
        ptr++;

        if (cmdcmp(ptr,"rodar",4)) return cmd_rodar(cn);
        if ((len=cmdcmp(ptr,"found",4))) return cmd_found(cn,ptr+len);
        if ((len=cmdcmp(ptr,"join",4))) return cmd_join(cn,ptr+len);
        if ((len=cmdcmp(ptr,"activate",4))) return cmd_activate(cn,ptr+len,0);
        if ((len=cmdcmp(ptr,"leave",4))) return cmd_leave(cn,ptr+len);
        if ((len=cmdcmp(ptr,"hire",4))) return cmd_hire(cn,ptr+len);
        if ((len=cmdcmp(ptr,"fire",4))) return cmd_fire(cn,ptr+len);
        if ((len=cmdcmp(ptr,"promote",4))) return cmd_promote(cn,ptr+len,1);
        if ((len=cmdcmp(ptr,"demote",4))) return cmd_promote(cn,ptr+len,0);

        if (ch[cn].flags&CF_GOD) {
            if ((len=cmdcmp(ptr,"setteam",4))) return cmd_activate(cn,ptr+len,1);
            if ((len=cmdcmp(ptr,"rename",4))) return cmd_rename(cn,ptr+len);
            if ((len=cmdcmp(ptr,"ban",3))) return cmd_ban(cn,ptr+len,1);
            if ((len=cmdcmp(ptr,"unban",4))) return cmd_ban(cn,ptr+len,0);
        }
    }

    return 1;
}

int rodar_canattack(int cn,int co) {
    int m,mf1,mf2;
    struct rodar_drd *dat1,*dat2;

    dat1=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat1) return 0;

    dat2=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat2) return 0;

    // one is not a team member. no fighting.
    if (!dat1->teamID || !dat2->teamID) return 3;

    // team members don't attack each other
    if (dat1->teamID==dat2->teamID) return 3;

    m=ch[cn].x+ch[cn].y*MAXMAP;
    mf1=map[m].flags;

    m=ch[co].x+ch[co].y*MAXMAP;
    mf2=map[m].flags;

    // arena is fair game
    if ((mf1&MF_ARENA) && (mf2&MF_ARENA)) return 2;

    // everywhere else not
    return 3;   // 1 = use default, 2 = yes, 3 = no
}

int rodar_canhelp(int cn,int co) {
    struct rodar_drd *dat1,*dat2;

    dat1=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat1) return 0;

    dat2=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat2) return 0;

    // team members can help each other
    if (dat1->teamID && dat1->teamID==dat2->teamID) return 2;

    return 3;   // 1 = use default, 2 = yes, 3 = no
}

void immortal_dead(int cn,int co) {
    charlog(cn,"I JUST DIED! I'M SUPPOSED TO BE IMMORTAL!");
}

int ch_driver(int nr,int cn,int ret,int lastact) {
    switch (nr) {
        case CDR_RODAR_MASTER:		rodarmaster(cn,ret,lastact); return 1;

        default:		return 0;
    }
}

int it_driver(int nr,int in,int cn) {
    switch (nr) {
        default:		return 0;
    }
}

int ch_died_driver(int nr,int cn,int co) {
    switch (nr) {
        case CDR_RODAR_MASTER:		immortal_dead(cn,co); return 1;

        default:		return 0;
    }
}

int ch_respawn_driver(int nr,int cn) {
    switch (nr) {
        default:		return 0;
    }
}

int special_driver(int nr,int obj,int ret,int lastact) {
    switch (nr) {
        case CDR_RODAR_PARSER:	    return rodar_parser(obj,(void *)(ret));
        case CDR_RODAR_CANATTACK:   return rodar_canattack(obj,ret);
        case CDR_RODAR_CANHELP:     return rodar_canhelp(obj,ret);

        default:		return 0;
    }
}
