/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

enum teamtype {
    TEAM2=1,
    TEAM3,
    TEAM5,
    TEAM7,
    TEAM12,
    TEAM_ANY
};

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
    EVENT_CLAN       =1<< 0,
    EVENT_NOMAGIC    =1<< 1,
    EVENT_NOFREEZE   =1<< 2,
    EVENT_NOWARCRY   =1<< 3
};

struct rodar_team {
    int ID;
    char name[80];
    int founderID;
    int founded;
    enum teamtype type;
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
    int level;
    int winnerID;
};


struct rodar_drd {
    int teamID; // active team
    enum membertype memtype;

    int joinID; // team player wants to join
};

enum teamtype rodar_teamtype(char *val);
enum teamstatus rodar_teamstatus(char *val);
char *rodar_teamtype2(enum teamtype type);
char *rodar_teamstatus2(enum teamstatus status);
enum membertype rodar_membertype(char *val);
char *rodar_membertype2(enum membertype type);
enum eventtype rodar_eventtype(char *val);
char *rodar_eventtype2(enum eventtype type);
enum eventopt rodar_eventopt(char *val);
char *rodar_eventopt2(enum eventopt opt);

void rodar_cache_team(char *name_or_ID,struct rodar_team *team);
int rodar_team_byname(char *name,struct rodar_team *team);
void rodar_cache_member(int teamID,int charID,int type);
int rodar_member(int teamID,int charID);
void rodar_read_event(void);
void rodar_reset_event(void);
void rodar_event_done(void);
int rodar_event_loaded(void);
void rodar_add_event(int ID,int t,enum eventtype type,enum eventopt opt,int level,int winnerID);
int rodar_get_event(int idx,struct rodar_event *ev);
int rodar_get_event_cnt(void);
void rodar_create_event(int when);

