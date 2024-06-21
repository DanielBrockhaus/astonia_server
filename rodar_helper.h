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

enum teamtype rodar_teamtype(char *val);
enum teamstatus rodar_teamstatus(char *val);
char *rodar_teamtype2(enum teamtype type);
char *rodar_teamstatus2(enum teamstatus status);
enum membertype rodar_membertype(char *val);
char *rodar_membertype2(enum membertype type);

void rodar_cache_team(char *name_or_ID,struct rodar_team *team);
int rodar_team_byname(char *name,struct rodar_team *team);
void rodar_cache_member(int teamID,int charID,int type);
int rodar_member(int teamID,int charID);
