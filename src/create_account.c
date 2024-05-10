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

int main(int argc,char **args)
{
	char buf[256];

	if (argc!=3) {
		fprintf(stderr,"Usage: %s <email> <password>\n",args[0]);
		return 1;
	}
	
	if (!init_database()) {
	        fprintf(stderr,"Cannot connect to database.\n");
	        return 3;
        }

	sprintf(buf,"insert subscriber (email,password,creation_time,locked,banned,vendor) values ("
                "'%s',"         // email
                "'%s',"         // password
                "%d,"           // creation time
                "'N',"          // locked
                "'I',"          // banned
                "%d)",          // vendor
                args[1],args[2],(int)time(NULL),0);
                
        if (mysql_query(&mysql,buf)) {
                fprintf(stderr,"Failed to create subscriber: Error: %s (%d)",mysql_error(&mysql),mysql_errno(&mysql));
                return 2;
        }

	printf("Success. Account ID is %d.\n",(int)mysql_insert_id(&mysql));
	
	exit_database();
	
	return 0;
}

