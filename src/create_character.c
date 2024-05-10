#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#define EXTERNAL_PROGRAM
#include "server.h"
#undef EXTERNAL_PROGRAM
#include "mail.h"
#include "statistics.h"
#include "clan.h"
#include "drdata.h"
#include "skill.h"
#include "depot.h"
#include "club.h"
#include "bank.h"
#include "badip.h"

static MYSQL mysql;
static char mysqlpass[80];

static void makemysqlpass(void) {
    static char key1[]={117,127,98,38,118,115,100,104,0};
    static char key2[]={"fr5tgs23"};
    static char key3[]={"gj56ffe3"};
    int n;

    for (n=0; key1[n]; n++) {
        mysqlpass[n]=key1[n]^key2[n]^key3[n];
        //printf("%d, ",mysqlpass[n]);
    }
    mysqlpass[n]=0;
    //printf("\n%s\n",mysqlpass);
}

static void destroymysqlpass(void)
{
        bzero(mysqlpass,sizeof(mysqlpass));
}

int getDatabasePort()
{
   int databasePort = 0;
   if (getenv("ASTONIA_DATABASE_PORT")) {
        databasePort = atoi(getenv("ASTONIA_DATABASE_PORT"));
   } 
   return databasePort;
}

const char* getDatabaseHostname()
{
    const char *db_hostname = "localhost";
    if (getenv("ASTONIA_DATABASE_HOSTNAME")) {
        db_hostname = getenv("ASTONIA_DATABASE_HOSTNAME");
    }
    return db_hostname;
}

const char* getDatabaseUsername()
{
    const char *db_username = "root";
    if (getenv("ASTONIA_DATABASE_USERNAME") || getenv("ASTONIA_DATABASE_USERNAME_FILE")) {
            if (getenv("ASTONIA_DATABASE_USERNAME")) {
                db_username = (const char *)getenv("ASTONIA_DATABASE_USERNAME");
            } else {
                FILE *fileHandle;
                char buffer[1024];
                fileHandle = fopen(getenv("ASTONIA_DATABASE_USERNAME_FILE"), "r");
                if (fileHandle == NULL) {
                    char* errorLog;
                    fprintf(stderr, "Error opening file '%s', defaulting to %s", getenv("ASTONIA_DATABASE_USERNAME_FILE"), db_username);
                    perror(errorLog);
                } else {
                    size_t bytesRead = fread(buffer, sizeof(char), 1024, fileHandle);
                    buffer[bytesRead] = '\0';
                    fclose(fileHandle);
                    db_username = (const char *)buffer;
                }
            }
    }
    return db_username;
}


int init_database(void)
{
        // init database client
        if (!mysql_init(&mysql)) return 0;
        
        // try to login as root with our password
        makemysqlpass();
        if (!mysql_real_connect(
                &mysql,
                getDatabaseHostname(),
                getDatabaseUsername(),
                mysqlpass,
                "mysql",
                getDatabasePort(),
                NULL,
                0)
        ) {
                destroymysqlpass();
                fprintf(stderr,"MySQL error: %s (%d)\n",mysql_error(&mysql),mysql_errno(&mysql));
                return 0;
        }
        destroymysqlpass();

        if (mysql_query(&mysql,"use merc")) {
                fprintf(stderr,"MySQL error: %s (%d)\n",mysql_error(&mysql),mysql_errno(&mysql));
                return 0;
        }

        return 1;
}

void exit_database(void)
{
	mysql_close(&mysql);
}


#define WANTSIZE	512
#define CHARINFO

int create_char(int user_ID,char *new_user,char *class)
{
	char data[80];
	unsigned long long flag=0;
	int size,expandto,add,mirror;
	char buf[WANTSIZE*2+256],dbuf[WANTSIZE*2],ddata[WANTSIZE];


        if (class[1]=='W')  flag|=CF_WARRIOR;
	else flag|=CF_MAGE;

	if (class[0]=='M')  flag|=CF_MALE;
	else flag|=CF_FEMALE;

	mysql_real_escape_string(&mysql,data,(char*)&flag,sizeof(flag));

	size=8+8+strlen(new_user)+15*4+1+3*2+1 +6-20;
	expandto=((size+(WANTSIZE-1)+9)/WANTSIZE)*WANTSIZE;
	add=expandto-size;

        *(unsigned int*)(ddata+0)=DRD_JUNK_PPD;
	*(unsigned int*)(ddata+4)=add-8;
	memset(ddata+8,0,add-8);

        mysql_real_escape_string(&mysql,dbuf,ddata,add);

	mirror=RANDOM(26)+1;

        sprintf(buf,"insert chars values ("
		"0,"		// ID
		"'%s',"		// name
		"%u,"		// class (aka flags)
		"0,"		// karma
                "0,"		// clan
		"0,"		// clan rank
		"0,"		// clan serial
		"0,"		// experience
		"0,"		// current_area
		"1,"		// allowed_area
		"%d,"		// creation_time
		"1,"		// login_time
		"1,"		// logout_time
		"'N',"		// locked
		"%d,"		// sID
		"'%s',"		// chr
		"'%s',"		// item
		"'%s',"		// ppd
		"%d,"		// mirror
		"0,"            // current mirror
		"1)",		// spacer
		new_user,
		(unsigned int)(flag&0xffffffff),
		(int)time(NULL),
		user_ID,
		data,
		data,
		dbuf,
		mirror);

	if (mysql_query(&mysql,buf)) {
		if (mysql_errno(&mysql)==ER_DUP_ENTRY) {
			fprintf(stderr,"Sorry, the name %s is already taken.\n",new_user);
			return 1;
		} else {
			fprintf(stderr,"Failed to create account %s: Error: %s (%d)\n",new_user,mysql_error(&mysql),mysql_errno(&mysql));
			return 2;
		}
	} else {
#ifdef CHARINFO
		sprintf(buf,"insert charinfo values ("
		"%d,"		// ID
		"'%s',"		// name
		"%u,"		// class (aka flags)
		"0,"		// karma
                "0,"		// clan
		"0,"		// clan rank
		"0,"		// clan serial
		"0,"		// experience
		"0,"		// current_area
		"%d,"		// creation_time
		"1,"		// login_time
		"1,"		// logout_time
		"'N',"		// locked
		"%d)",		// sID
		(int)mysql_insert_id(&mysql),
		new_user,
		(unsigned int)(flag&0xffffffff),
		(int)time(NULL),
		user_ID);
		if (mysql_query(&mysql,buf)) {
			fprintf(stderr,"Failed to create account %s: Error: %s (%d)\n",new_user,mysql_error(&mysql),mysql_errno(&mysql));
		}
#endif
	}
	
	return 0;
}



int main(int argc,char **args)
{
	if (argc!=4) {
		fprintf(stderr,"Usage: %s <accountID> <name> <genderandclass>\ngenderandclass: MW = male warrior, FM = female mage\n",args[0]);
		return 1;
	}
	
	if (!init_database()) {
	        fprintf(stderr,"Cannot connect to database.\n");
	        return 3;
        }
        
        if (!create_char(atoi(args[1]),args[2],args[3])) {
                printf("Success.\n");
        }
	
	exit_database();
	
	return 0;
}

