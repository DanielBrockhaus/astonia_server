/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

/*

insert into area values (38,1,'Rodneys Arena',0,0,0,0,0,0,0,0);
*/

#include <stdlib.h>

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

static void cmd_rodar(int cn) {
    log_char(cn,LOG_SYSTEM,0,"Rodar Help");
}

int rodar_parser(int cn,char *ptr) {
    static struct rodar_team team;

    if (*ptr=='#' || *ptr=='/') {
        ptr++;

        if (cmdcmp(ptr,"rodar",4)) { cmd_rodar(cn); return 2; }
        if (cmdcmp(ptr,"found",4)) { db_create_team("Godlike",ch[cn].ID); return 2; }
        if (cmdcmp(ptr,"read",4)) { db_read_team("Godlike"); return 2; }
        if (cmdcmp(ptr,"show",4)) { rodar_team_byname("Godlike",&team); log_char(cn,LOG_SYSTEM,0,"ID=%d, name=%s, founder=%d",team.ID,team.name,team.founderID); return 2; }
        if (cmdcmp(ptr,"write",4)) { team.type=TEAM2; db_write_team(&team); return 2; }
        if (cmdcmp(ptr,"add",3)) { db_add_team_member(1,3,MEMBER); return 2; }
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
