/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

enum teamstatus {
    TEAM_ACTIVE=1,
    TEAM_BANNED,
    TEAM_RETIRED
};

enum membertype {
    MEMBER_NONE=0,
    MEMBER_OWNER=1,
    MEMBER_ADMIN,
    MEMBER
};

enum eventtype {
    EVENT2=1,
    EVENT3,
    EVENT5,
    EVENT7,
    EVENT12,
    EVENT_ANY
};

enum eventopt {
    EVENT_NONE=0,
    EVENT_NOMAGIC    =1<< 0,
    EVENT_NOFREEZE   =1<< 1,
    EVENT_NOWARCRY   =1<< 2
};

struct rodar_team {
    int ID;
    char name[80];
    int founderID;
    int founded;
    enum teamstatus status;
    int wins,losses,kills,killed;
    int score;
};

struct rodar_member {
    int teamID;
    int charID;
    enum membertype type;
};

struct rodar_event {
    int ID;
    int t;
    enum eventtype type;
    enum eventopt opt;
    int room;
    int level;
    int winnerID;
};

struct rodar_drd {
    int teamID; // active team
    enum membertype memtype;

    int joinID; // team player wants to join
};

struct rodar_coords {
    int x,y;
};

#define MAXENTRANCE     16

struct rodar_event_room {
    int cnt;
    int delay;
    struct rodar_coords entrance[MAXENTRANCE];
};

struct rodar_active_team {
    int teamID;
    int *charID;
    int char_cnt,char_max;
};

struct rodar_data {
    struct rodar_event ev;
    struct rodar_active_team *at;
    int at_cnt,at_max;
    struct rodar_team win_team;
};

extern struct rodar_data rodar_data;

enum teamstatus rodar_teamstatus(char *val);
char *rodar_teamstatus2(enum teamstatus status);
enum membertype rodar_membertype(char *val);
char *rodar_membertype2(enum membertype type);
enum eventtype rodar_eventtype(char *val);
char *rodar_eventtype2(enum eventtype type);
enum eventopt rodar_eventopt(char *val);
char *rodar_eventopt2(enum eventopt opt);

void rodar_cache_team(char *name_or_ID,struct rodar_team *team);
int rodar_team_byname(char *name,struct rodar_team *team);
int rodar_team_byID(int teamID,struct rodar_team *team);
void rodar_cache_member(int teamID,int charID,int type);
int rodar_member(int teamID,int charID);
void rodar_read_event(void);
void rodar_reset_event(void);
void rodar_event_done(void);
int rodar_event_loaded(void);
void rodar_add_event(int ID,int t,enum eventtype type,enum eventopt opt,int level,int room,int winnerID);
int rodar_get_event(int idx,struct rodar_event *ev);
int rodar_get_event_cnt(void);
void rodar_create_event(int when);

void rodar_cleanup_event(void);
int rodar_is_in_event(int teamID,int charID);
int rodar_chars_in_event(int teamID);
int rodar_add_to_event(int teamID,int charID);
int rodar_event_maxchars(enum eventtype type);
int rodar_time_till_event(void);
void rodar_end_event(int winnerID);
void rodar_load_winner(void);
