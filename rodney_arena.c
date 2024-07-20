/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

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
#include "see.h"
#include "map.h"
#include "sector.h"
#include "tool.h"
#include "player.h"
#include "player_driver.h"
#include "act.h"
#include "item_id.h"
#include "clan.h"
#include "chat.h"


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

static struct rodar_event_room room[]={
    {0,16,{{0,0}}},          // 0
    {1,16,{{10,10}}},        // 1
    {8,4,{{81,17},{95,17},{109,17},{109,31},{109,45},{95,45},{81,45},{81,31}}},        // 2
};

struct master_data {
    int woneventID;
};

void announce(int cn,char *format,...) __attribute__ ((format(printf,2,3)));

void announce(int cn,char *format,...) {
    unsigned char buf[1024];
    va_list args;
    int len,co;

    va_start(args,format);
    len=vsnprintf(buf,1020,format,args);
    va_end(args);

    if (len==1020) return;

    if (strchr(buf,'"')) return;

    for (co=getfirst_char(); co; co=getnext_char(co)) {
        if (!(ch[co].flags&CF_PLAYER)) continue;
        log_char(co,LOG_SYSTEM,0,"%s announces: \"%s\"",ch[cn].name,buf);
    }
}

static void clear_arena(void) {
    int co;

    for (co=getfirst_char(); co; co=getnext_char(co)) {
        if (!(ch[co].flags&CF_PLAYER)) continue;

        if (ch[co].x<240 && ch[co].y<240) {
            remove_char(co);

            ch[co].action=ch[co].step=ch[co].duration=0;
            if (ch[co].player) player_driver_stop(ch[co].player,0);

            if (!drop_char_extended(co,249,234,7)) {
                exit_char(co);
                if (ch[co].player) kick_player(ch[co].player,"No space to drop character");
            }
        }
    }
}

void rodarmaster(int cn,int ret,int lastact) {
    struct msg *msg,*next;
    struct master_data *dat;
    struct rodar_event ev;
    int n;

    dat=set_data(cn,DRD_RODARMASTER,sizeof(struct master_data));
    if (!dat) return;   // oops...

    // loop through our messages
    for (msg=ch[cn].msg; msg; msg=next) {
        next=msg->next;

        if (msg->type==NT_CREATE) {
            rodar_read_event();
        }

        remove_message(cn,msg);
    }

    if (rodar_event_loaded()) {
        // get newest event
        n=rodar_get_event_cnt();
        if (!n) {
            rodar_create_event(time_now-60*29);
        } else {
            rodar_get_event(n-1,&ev);

            // less than one week in the future?
            if (ev.t<time_now+60*60*24*7) {
                rodar_create_event(ev.t);
            }
        }
    }

    // current event
    if (rodar_event_loaded() && rodar_get_event_cnt()>0) {


        rodar_get_event(0,&ev);
        if (!rodar_data.ev.ID) {
            rodar_data.ev=ev;
        }

        if (time_now-ev.t>60*30) { // event is over, load new schedule
            rodar_cleanup_event();
            rodar_read_event();
        } else {
            if (ev.t-time_now<60*30) { // event will start in less than 30 minutes
                if (rodar_data.ev.ID!=ev.ID) {
                    rodar_data.ev=ev;

                    announce(cn,"Event %d will start in 30 minutes!",rodar_data.ev.ID);

                    clear_arena();
                }
            }
        }
    }

    if (rodar_data.ev.winnerID && dat->woneventID!=rodar_data.ev.ID) {
        xlog("%d %d %d %d",rodar_data.ev.winnerID,dat->woneventID,rodar_data.ev.ID,rodar_data.win_team.ID);
        if (rodar_data.win_team.ID==rodar_data.ev.winnerID) {
            dat->woneventID=rodar_data.ev.ID;
            announce(cn,"Team %s (%d) has won event %d!",rodar_data.win_team.name,rodar_data.ev.winnerID,rodar_data.ev.ID);
        } else {
            rodar_load_winner();
        }
    }

    if (spell_self_driver(cn)) return;

    if (secure_move_driver(cn,ch[cn].tmpx,ch[cn].tmpy,DX_DOWN,ret,lastact)) return;

    do_idle(cn,TICKS);
}

static int cmd_rodar(int cn) {

    log_char(cn,LOG_SYSTEM,0,"Rodar Help");
    log_char(cn,LOG_SYSTEM,0,"%s","");
    log_char(cn,LOG_SYSTEM,0,"/show [offset] - show current event, or event [offset]");
    log_char(cn,LOG_SYSTEM,0,"/enter [team name] - enter event [on behalf of team] or active team");
    log_char(cn,LOG_SYSTEM,0,"/join <team name> - join team");
    log_char(cn,LOG_SYSTEM,0,"/leave <team name> - leave team");
    log_char(cn,LOG_SYSTEM,0,"/activate <team name> - activate team membership");
    log_char(cn,LOG_SYSTEM,0,"/hire <char name> - hire player to active team");
    log_char(cn,LOG_SYSTEM,0,"/fire <char name> - fire player from active team");
    log_char(cn,LOG_SYSTEM,0,"/promote <char name> - promote player to admin in the active team");
    log_char(cn,LOG_SYSTEM,0,"/demote <char name> - demote player from admin to member");
    log_char(cn,LOG_SYSTEM,0,"/found <team name> - found a new team (1000G)");
    log_char(cn,LOG_SYSTEM,0,"/members <team name> - list team members");
    log_char(cn,LOG_SYSTEM,0,"/offer [name] - list offer, details about offer");
    log_char(cn,LOG_SYSTEM,0,"/buy <name> - buy offer");

    if (ch[cn].flags&CF_GOD) {
        log_char(cn,LOG_SYSTEM,0,"/setteam <team name> - temporarily join team");
        log_char(cn,LOG_SYSTEM,0,"/rename <new team name> - rename active team");
        log_char(cn,LOG_SYSTEM,0,"/ban <team name> - ban team");
        log_char(cn,LOG_SYSTEM,0,"/unban <team name> - unban team");
    }

    return 2;
}

static int cmd_show(int cn,char *ptr) {
    int offset,d;
    struct rodar_event ev;

    offset=atoi(ptr);
    if (offset<0) {
        log_char(cn,LOG_SYSTEM,0,"Offset must not be negative.");
        return 2;
    }

    if (!offset) {
        xlog("%d %d %d",rodar_data.ev.ID,rodar_data.ev.winnerID,rodar_data.win_team.ID);
        if (rodar_data.ev.ID && rodar_data.ev.winnerID) {
            if (rodar_data.ev.winnerID==rodar_data.win_team.ID) {
                log_char(cn,LOG_SYSTEM,0,"The event %d is already over. Team %s (%d) has won.",
                    rodar_data.ev.ID,rodar_data.win_team.name,rodar_data.ev.winnerID);
                return 2;
            } else {
                rodar_load_winner();
                return 3;
            }

        }
        ev=rodar_data.ev;
    } else {
        if (!rodar_get_event(offset,&ev)) {
            log_char(cn,LOG_SYSTEM,0,"No data about event at offset %d.",offset);
            return 2;
        }
    }

    if (!ev.ID) {
        log_char(cn,LOG_SYSTEM,0,"There is no current event.");
        return 2;
    }

    if (ev.t>time_now) {
        d=ev.t-time_now;
        log_char(cn,LOG_SYSTEM,0,"The event %d will start in %02d:%02d:%02d.",ev.ID,d/60/60,(d/60)%60,d%60);
    } else {
        d=time_now-ev.t;
        log_char(cn,LOG_SYSTEM,0,"The event %d has started %02d:%02d:%02d ago.",ev.ID,d/60/60,(d/60)%60,d%60);
    }

    log_char(cn,LOG_SYSTEM,0,"It is for level %d, with %s participants per team in room %d. Options: %s.",
        ev.level,rodar_eventtype2(ev.type),ev.room,ev.opt?rodar_eventopt2(ev.opt):"none");

    return 2;
}

static int cmd_found(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;

    if (ch[cn].x<240 && ch[cn].y<240) {
        log_char(cn,LOG_SYSTEM,0,"No founding teams while being in an arena.");
        return 2;
    }

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

        // TODO: we waive the fee if the team is older than 3 seconds. this is a hack and should be fixed eventually.
        if (time_now-team.founded<3) {
            log_char(cn,LOG_SYSTEM,0,"Success. You've founded the team '%s'!",team.name);
            ch[cn].gold-=1000*100;
            ch[cn].flags|=CF_ITEMS;
        } else {
            log_char(cn,LOG_SYSTEM,0,"That team seems to exist already.");
        }
        db_add_team_member(team.ID,ch[cn].ID,MEMBER_OWNER);
        db_read_member(team.ID,ch[cn].ID);
        return 2;
    } else if (team.ID) {   // team was loaded and founder is not cn
        log_char(cn,LOG_SYSTEM,0,"Failure. There is already a team called '%s'!",team.name);
        return 2;
    } else {    // team wasn't loaded
        db_read_team(ptr);
        return 3;
    }
}

static int cmd_members(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /members <name>. The name must be letters and spaces only.");
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

    db_read_members(team.ID,ch[cn].ID);
    return 2;
}

static int cmd_winners(int cn,char *ptr) {
    int offset;

    offset=atoi(ptr);

    db_read_winners(offset,ch[cn].ID);
    return 2;
}

static int cmd_join(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;

    if (ch[cn].x<240 && ch[cn].y<240) {
        log_char(cn,LOG_SYSTEM,0,"No changing teams while being in an arena.");
        return 2;
    }

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

    if (ch[cn].x<240 && ch[cn].y<240) {
        log_char(cn,LOG_SYSTEM,0,"No changing teams while being in an arena.");
        return 2;
    }

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

static int cmd_activate(int cn,char *ptr,int godmode,int submode) {
    char *tmp;
    struct rodar_team team;
    struct rodar_drd *dat;
    int type;

    if (ch[cn].x<240 && ch[cn].y<240) {
        log_char(cn,LOG_SYSTEM,0,"No changing teams while being in an arena.");
        if (submode) return 4;
        else return 2;
    }

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            if (submode) log_char(cn,LOG_SYSTEM,0,"Syntax: /enter <name>. The name must be letters and spaces only.");
            else log_char(cn,LOG_SYSTEM,0,"Syntax: /activate <name>. The name must be letters and spaces only.");
            if (submode) return 4;
            else return 2;
        }
    }

    // load team
    if (rodar_team_byname(ptr,&team)==-1) {
        db_read_team(ptr);
        return 3;
    }

    if (!team.ID) {
        log_char(cn,LOG_SYSTEM,0,"No team by that name.");
        if (submode) return 4;
        else return 2;
    }

    if (team.status!=TEAM_ACTIVE) {
        log_char(cn,LOG_SYSTEM,0,"That team has been %s.",rodar_teamstatus2(team.status));
        if (submode) return 4;
        else return 2;
    }

    if (godmode) type=MEMBER_OWNER;
    else if ((type=rodar_member(team.ID,ch[cn].ID))==-1) {
        db_read_member(team.ID,ch[cn].ID);
        return 3;
    }

    if (type==MEMBER_NONE) {
        log_char(cn,LOG_SYSTEM,0,"You are not member of team %s.",team.name);
        if (submode) return 4;
        else return 2;
    }

    dat=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat) {
        if (submode) return 4;
        else return 2;
    }

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
                dat2->teamID=0;
                dat2->memtype=0;
                log_char(co,LOG_SYSTEM,0,"You have been fired from your active team.");

                if (ch[co].x<240 && ch[co].y<240) {
                    remove_char(co);

                    ch[co].action=ch[co].step=ch[co].duration=0;
                    if (ch[co].player) player_driver_stop(ch[co].player,0);

                    if (!drop_char_extended(co,249,234,7)) {
                        exit_char(co);
                        if (ch[co].player) kick_player(ch[co].player,"No space to drop character");
                    }
                }
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

static int cmd_enter(int cn,char *ptr) {
    int oldx,oldy,tmp,ret,e;
    struct rodar_drd *dat2;

    if (ch[cn].x<240 && ch[cn].y<240) {
        log_char(cn,LOG_SYSTEM,0,"No entering an arena while being in an arena.");
        return 2;
    }

    if (ch[cn].level>rodar_data.ev.level) {
        log_char(cn,LOG_SYSTEM,0,"This event is for levels %d and below. You are level %d.",rodar_data.ev.level,ch[cn].level);
        return 2;
    }

    if (ch[cn].action!=AC_IDLE || ticker-ch[cn].regen_ticker<TICKS*5) {
        log_char(cn,LOG_SYSTEM,0,"You're still trying to catch your breath.");
        return 2;
    }

    while (*ptr==' ') ptr++;
    if (*ptr) {
        ret=cmd_activate(cn,ptr,0,1);
        if (ret==3) return 3;    // call activate to set team if needed
        if (ret==4) return 2;
    }

    dat2=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat2) return 2;

    if (!dat2->teamID) {
        log_char(cn,LOG_SYSTEM,0,"You need to activate your team first.");
        return 2;
    }

    tmp=rodar_is_in_event(dat2->teamID,ch[cn].ID);
    if (tmp==0) {
        if (rodar_chars_in_event(dat2->teamID)>=rodar_event_maxchars(rodar_data.ev.type)) {
            log_char(cn,LOG_SYSTEM,0,"There are already %d players in this event.",rodar_event_maxchars(rodar_data.ev.type));
            return 2;
        }
        rodar_add_to_event(dat2->teamID,ch[cn].ID);
    }

    if (tmp==-1) {
        log_char(cn,LOG_SYSTEM,0,"You cannot enter the same event for different teams.");
        return 2;
    }

    oldx=ch[cn].x; oldy=ch[cn].y;
    remove_char(cn);

    ch[cn].action=ch[cn].step=ch[cn].duration=0;
    if (ch[cn].player) player_driver_stop(ch[cn].player,0);

    e=RANDOM(room[rodar_data.ev.room].cnt);

    if (!drop_char_extended(cn,room[rodar_data.ev.room].entrance[e].x,room[rodar_data.ev.room].entrance[e].y,7)) {
        log_char(cn,LOG_SYSTEM,0,"There's no room in the event. Please try again.");
        drop_char(cn,oldx,oldy,0);
    }
    return 2;
}

enum offertype {
    OFFER_CLANJEWEL,
    OFFER_ITEM,
    OFFER_ORB
};

struct offer {
    char *name;
    int cost;
    char *desc;
    enum offertype type;
    char *item;
};

struct offer offer[]={
    {"ClanJ",6,"A single clan jewel, straight to your clan's vault.",OFFER_CLANJEWEL,NULL},
    {"GoldU",1,"1000 units of gold (metal).",OFFER_ITEM,"reward_gold1000"},
    {"OrbIm",3,"An orb of Immunity.",OFFER_ORB,(void*)(V_IMMUNITY)},
    {"OrbWis",2,"An orb of Wisdom.",OFFER_ORB,(void*)(V_WIS)},
    {"OrbInt",2,"An orb of Intuition.",OFFER_ORB,(void*)(V_INT)},
    {"OrbAgi",2,"An orb of Agility.",OFFER_ORB,(void*)(V_AGI)},
    {"OrbStr",2,"An orb of Strength.",OFFER_ORB,(void*)(V_STR)},
    {"OrbAt",2,"An orb of Attack.",OFFER_ORB,(void*)(V_ATTACK)},
    {"OrbPa",2,"An orb of Parry.",OFFER_ORB,(void*)(V_PARRY)},
    {"OrbMS",2,"An orb of Magic Shield.",OFFER_ORB,(void*)(V_MAGICSHIELD)},
    {"OrbLi",2,"An orb of Lightning.",OFFER_ORB,(void*)(V_FLASH)}
};

int cmd_offer(int cn,char *ptr) {
    int n;

    while (*ptr==' ') ptr++;
    for (n=0; n<sizeof(offer)/sizeof(offer[0]); n++) {
        if (!strncasecmp(ptr,offer[n].name,strlen(offer[n].name))) break;
    }
    if (n<sizeof(offer)/sizeof(offer[0])) {
        log_char(cn,LOG_SYSTEM,0,"%s for %d chip%s",offer[n].name,offer[n].cost,offer[n].cost>1?"s":"");
        log_char(cn,LOG_SYSTEM,0,"%s",offer[n].desc);
    } else {

        for (n=0; n<sizeof(offer)/sizeof(offer[0]); n++) {
            log_char(cn,LOG_SYSTEM,0,"%s for %d chip%s",offer[n].name,offer[n].cost,offer[n].cost>1?"s":"");
        }
        log_char(cn,LOG_SYSTEM,0,"Use /offer <name> for details, or /buy <name> to buy it.");
    }

    return 2;
}

void set_chip_data(int in,int off) {
    if (*(unsigned int *)(it[in].drdata+0)>5) it[in].sprite=53012+off;
    else if (*(unsigned int *)(it[in].drdata+0)==5) it[in].sprite=53011+off;
    else if (*(unsigned int *)(it[in].drdata+0)==4) it[in].sprite=53010+off;
    else if (*(unsigned int *)(it[in].drdata+0)==3) it[in].sprite=53009+off;
    else if (*(unsigned int *)(it[in].drdata+0)==2) it[in].sprite=53008+off;
    else it[in].sprite=53007+off;

    if (*(unsigned int *)(it[in].drdata+0)>1) sprintf(it[in].description,"%d %ss.",*(unsigned int *)(it[in].drdata),it[in].name);
    else sprintf(it[in].description,"%d %s.",*(unsigned int *)(it[in].drdata),it[in].name);
}

static int pay_chip(int cn,int cost,int doit) {
    int i,have,in;

    for (i=30; i<INVENTORYSIZE; i++) {
        if ((in=ch[cn].item[i]) && it[in].ID==IID_RODAR_CHIP && (have=*(unsigned int *)(it[in].drdata+0))>=cost) {
            if (cost==have) {
                if (doit) {
                    destroy_item(in);
                    ch[cn].item[i]=0;
                    ch[cn].flags|=CF_ITEMS;
                }
                return 1;
            } else {
                if (doit) {
                    *(unsigned int *)(it[in].drdata+0)-=cost;
                    set_chip_data(in,12);
                    ch[cn].flags|=CF_ITEMS;
                }
                return 1;
            }
        }
    }
    return 0;
}

int cmd_buy(int cn,char *ptr) {
    int n,in;
    char buf[256];

    while (*ptr==' ') ptr++;

    for (n=0; n<sizeof(offer)/sizeof(offer[0]); n++) {
        if (!strncasecmp(ptr,offer[n].name,strlen(offer[n].name))) break;
    }

    if (n<sizeof(offer)/sizeof(offer[0])) {
        if (!pay_chip(cn,offer[n].cost,0)) {
            log_char(cn,LOG_SYSTEM,0,"You cannot afford %s.",offer[n].name);
            return 2;
        }
        switch (offer[n].type) {
            case OFFER_ITEM:
            case OFFER_ORB:
                if (offer[n].type==OFFER_ITEM) in=create_item(offer[n].item);
                else if (offer[n].type==OFFER_ORB) in=create_orb2((int)offer[n].item);
                else in=0;
                if (in) {
                    if (give_char_item(cn,in)) {
                        log_char(cn,LOG_SYSTEM,0,"You got a %s.",it[in].name);
                        pay_chip(cn,offer[n].cost,1);
                    } else {
                        log_char(cn,LOG_SYSTEM,0,"No room to put %s",it[in].name);
                        destroy_item(in);
                    }
                } else {
                    elog("no reward for team offer nr=%d",n);
                    log_char(cn,LOG_SYSTEM,0,"Bug #1779");
                }
                break;
            case OFFER_CLANJEWEL:
                if (ch[cn].clan==0 || ch[cn].clan>MAXCLAN) {
                    log_char(cn,LOG_SYSTEM,0,"You do not belong to a clan.");
                } else {
                    sprintf(buf,"%02d:Y:%10u:%s",ch[cn].clan,ch[cn].ID,ch[cn].name);
                    server_chat(1028,buf);
                    log_char(cn,LOG_SYSTEM,0,"You bought a clan jewel for your clan.");
                    pay_chip(cn,offer[n].cost,1);
                }
                break;

        }
    } else {
        log_char(cn,LOG_SYSTEM,0,"No offer by the name %s.",ptr);
    }

    return 2;
}

int rodar_parser(int cn,char *ptr) {
    int len;

    if (*ptr=='#' || *ptr=='/') {
        ptr++;

        if (cmdcmp(ptr,"rodar",4)) return cmd_rodar(cn);
        if ((len=cmdcmp(ptr,"found",4))) return cmd_found(cn,ptr+len);
        if ((len=cmdcmp(ptr,"join",4))) return cmd_join(cn,ptr+len);
        if ((len=cmdcmp(ptr,"activate",4))) return cmd_activate(cn,ptr+len,0,0);
        if ((len=cmdcmp(ptr,"leave",4))) return cmd_leave(cn,ptr+len);
        if ((len=cmdcmp(ptr,"hire",4))) return cmd_hire(cn,ptr+len);
        if ((len=cmdcmp(ptr,"fire",4))) return cmd_fire(cn,ptr+len);
        if ((len=cmdcmp(ptr,"promote",4))) return cmd_promote(cn,ptr+len,1);
        if ((len=cmdcmp(ptr,"demote",4))) return cmd_promote(cn,ptr+len,0);
        if ((len=cmdcmp(ptr,"show",4))) return cmd_show(cn,ptr+len);
        if ((len=cmdcmp(ptr,"enter",4))) return cmd_enter(cn,ptr+len);
        if ((len=cmdcmp(ptr,"members",4))) return cmd_members(cn,ptr+len);
        if ((len=cmdcmp(ptr,"winners",4))) return cmd_winners(cn,ptr+len);
        if ((len=cmdcmp(ptr,"offer",4))) return cmd_offer(cn,ptr+len);
        if ((len=cmdcmp(ptr,"buy",3))) return cmd_buy(cn,ptr+len);

        if (ch[cn].flags&CF_GOD) {
            if ((len=cmdcmp(ptr,"setteam",4))) return cmd_activate(cn,ptr+len,1,0);
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

    dat2=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat2) return 0;

    // one is not a team member. no fighting.
    if (!dat1->teamID || !dat2->teamID) return 3;

    // team members don't attack each other
    if (dat1->teamID==dat2->teamID) return 3;

    m=ch[cn].x+ch[cn].y*MAXMAP;
    mf1=map[m].flags;

    m=ch[co].x+ch[co].y*MAXMAP;
    mf2=map[m].flags;

    // arena is fair game if an event is running
    if ((mf1&MF_ARENA) && (mf2&MF_ARENA) && rodar_time_till_event()<=0) return 2;

    // everywhere else not
    return 3;   // 1 = use default, 2 = yes, 3 = no
}

int rodar_canhelp(int cn,int co) {
    struct rodar_drd *dat1,*dat2;

    dat1=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat1) return 0;

    dat2=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
    if (!dat2) return 0;

    // no help while in the lobby
    if (ch[cn].x>240 || ch[cn].y>240) return 3;
    if (ch[co].x>240 || ch[co].y>240) return 3;

    // team members can help each other
    if (dat1->teamID && dat1->teamID==dat2->teamID) return 2;

    return 3;   // 1 = use default, 2 = yes, 3 = no
}

static int lightoff[]={
    -MAXMAP*14  -14,
    -MAXMAP*14  -12,
    -MAXMAP*14  -10,
    -MAXMAP*14  - 8,
    -MAXMAP*14  - 6,
    -MAXMAP*14  - 4,
    -MAXMAP*12  - 4,
    -MAXMAP*10  - 4,
    -MAXMAP* 8  - 4,
    -MAXMAP* 6  - 4,
    -MAXMAP* 4  - 4,
    -MAXMAP* 4  - 6,
    -MAXMAP* 4  - 8,
    -MAXMAP* 4  -10,
    -MAXMAP* 4  -12,
    -MAXMAP* 4  -14,
    -MAXMAP* 6  -14,
    -MAXMAP* 8  -14,
    -MAXMAP*10  -14,
    -MAXMAP*12  -14
};

static inline int mapx(int m) {
    return m%MAXMAP;
}

static inline int mapy(int m) {
    return m/MAXMAP;
}

static void reward_char(int cn) {
    int in;

    in=create_item("rodar_chip");
    if (!in) {
        elog("no reward for team arena");
        return;
    }
    if (!give_char_item(cn,in)) {
        log_char(cn,LOG_SYSTEM,0,"No space to put winnings. Too bad.");
        destroy_item(in);
    } else {
        log_char(cn,LOG_SYSTEM,0,"You won a %s.",it[in].name);
    }
}

void rodar_solver(int in,int cn) {
    int arenaID,win_cn=0;
    int state,step,delay,d_cnt,newstate=0;
    int m,x,y,co,teamID=0;
    int len=sizeof(lightoff)/sizeof(lightoff[0]);
    struct rodar_drd *dat;

    if (cn) return;     // we handle ONLY timer calls

    arenaID=it[in].drdata[0];

    if (arenaID!=rodar_data.ev.room) {
        call_item(IDR_RODARSOLVER,in,0,ticker+TICKS);
        return;
    }

    state=it[in].drdata[1];
    step=it[in].drdata[2];
    delay=it[in].drdata[3];
    d_cnt=it[in].drdata[4];
    m=it[in].x+it[in].y*MAXMAP;

    if (rodar_time_till_event()<0 && !rodar_data.ev.winnerID) {
        for (x=it[in].x-13; x<it[in].x-4 && !newstate; x++) {
            for (y=it[in].y-13; y<it[in].y-4 && !newstate; y++) {
                if ((co=map[x+y*MAXMAP].ch) && (ch[co].flags&CF_PLAYER)) {
                    dat=set_data(co,DRD_RODAR,sizeof(struct rodar_drd));
                    if (dat) {
                        if (teamID && dat->teamID && teamID!=dat->teamID) newstate=2;  // two teams, go red
                        else if (dat->teamID) {
                            teamID=dat->teamID; // remember team ID
                            win_cn=co;          // and remember one of the winning characters
                        }
                    }
                }
            }
        }
        if (newstate) ;
        else if (teamID) newstate=1;
        else newstate=3;
    } else newstate=3;

    if (state!=newstate) {
        state=newstate;
        step=0;
        if (state==1) delay=room[arenaID].delay;
        else delay=0;
        d_cnt=0;
    }

    if (delay>d_cnt) {
        d_cnt++;
    } else {
        d_cnt=0;
        if (state==1) {         // going green
            if (step>len) {
                ;
            } else if (step==len) {
                it[in].sprite=14136;
                set_sector(it[in].x,it[in].y);
                rodar_end_event(teamID);
                clear_arena();
                if (win_cn) reward_char(win_cn);
                step++;
            } else {
                map[m+lightoff[step]].fsprite=14200;
                set_sector(mapx(m+lightoff[step]),mapy(m+lightoff[step]));
                step++;
            }
        } else if (state==2) {      // going red
            if (step>len) {
                ;
            } else if (step==len) {
                it[in].sprite=14139;
                set_sector(it[in].x,it[in].y);
                step++;
            } else {
                map[m+lightoff[step]].fsprite=14202;
                set_sector(mapx(m+lightoff[step]),mapy(m+lightoff[step]));
                step++;
            }
        } else if (state==3) {      // going orange
            if (step>len) {
                ;
            } else if (step==len) {
                it[in].sprite=14138;
                set_sector(it[in].x,it[in].y);
                step++;
            } else {
                map[m+lightoff[step]].fsprite=14201;
                set_sector(mapx(m+lightoff[step]),mapy(m+lightoff[step]));
                step++;
            }
        }
    }

    it[in].drdata[1]=state;
    it[in].drdata[2]=step;
    it[in].drdata[3]=delay;
    it[in].drdata[4]=d_cnt;

    call_item(IDR_RODARSOLVER,in,0,ticker+2);
}

int rodar_playerdeath(int cn,int cc) {
    struct rodar_drd *dat;

    dat=set_data(cn,DRD_RODAR,sizeof(struct rodar_drd));
    if (dat && dat->teamID) db_inc_team_value(dat->teamID,"killed",1);

    if (cc) {
        dat=set_data(cc,DRD_RODAR,sizeof(struct rodar_drd));
        if (dat && dat->teamID) db_inc_team_value(dat->teamID,"kills",1);
    }

    log_char(cn,LOG_SYSTEM,0,"Rodney's Journeyman saves thee from certain death.");

    return 2;
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
        case IDR_RODARSOLVER:       rodar_solver(in,cn); return 1;
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
        case CDR_RODAR_DEATH:       return rodar_playerdeath(obj,ret);

        default:		return 0;
    }
}
