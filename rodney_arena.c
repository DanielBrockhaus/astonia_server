/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

/*

insert into area values (38,1,'Rodneys Arena',0,0,0,0,0,0,0,0);
*/

#include <stdlib.h>
#include <ctype.h>

#include "server.h"
#include "libload.h"
#include "notify.h"
#include "drvlib.h"
#include "direction.h"
#include "do.h"
#include "log.h"
#include "talk.h"
#include "command.h"
#include "rodar_helper.h"
#include "database.h"

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
    log_char(cn,LOG_SYSTEM,0,"/found <team name> - found a new team (1000G)");

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
    int type;

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

    if ((type=rodar_member(team.ID,ch[cn].ID))==-1) {
        db_read_member(team.ID,ch[cn].ID);
        return 3;
    }

    log_char(cn,LOG_SYSTEM,0,"Type %d found.",type);

    return 2;
}

static int cmd_activate(int cn,char *ptr) {
    char *tmp;
    struct rodar_team team;
    int type;

    while (*ptr==' ') ptr++;

    for (tmp=ptr; *tmp; tmp++) {
        if (!isalpha(*tmp) && *tmp!=' ') {
            log_char(cn,LOG_SYSTEM,0,"Syntax: /activate <name>. The name must be letters and spaces only.");
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

    log_char(cn,LOG_SYSTEM,0,"Team %s membership of rank %s activated.",team.name,rodar_membertype2(type));

    return 2;
}

int rodar_parser(int cn,char *ptr) {
    int len;

    if (*ptr=='#' || *ptr=='/') {
        ptr++;

        if (cmdcmp(ptr,"rodar",4)) return cmd_rodar(cn);
        if ((len=cmdcmp(ptr,"found",4))) return cmd_found(cn,ptr+len);
        if ((len=cmdcmp(ptr,"join",4))) return cmd_join(cn,ptr+len);
        if ((len=cmdcmp(ptr,"activate",4))) return cmd_activate(cn,ptr+len);
        //if ((len=cmdcmp(ptr,"hire",4))) return cmd_hire(cn,ptr+len);
        //if ((len=cmdcmp(ptr,"fire",4))) return cmd_fire(cn,ptr+len);
    }

    return 1;
}

int rodar_canattack(int cn,int co) {
    xlog("can attack %d %d",cn,co);
    return 1;   // 1 = use default, 2 = yes, 3 = no
}

int rodar_canhelp(int cn,int co) {
    xlog("can help %d %d",cn,co);
    return 1;   // 1 = use default, 2 = yes, 3 = no
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
