/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

#include <stdio.h>

#define max(a,b)	((a) > (b) ? (a) : (b))
#define min(a,b)	((a) < (b) ? (a) : (b))

static char *sleeves_name[]={
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves",
    "Sleeves"
};

static char *armor_name[]={
    "Armor",
    "Armor",
    "Armor",
    "Armor",
    "Armor",
    "Armor",
    "Armor",
    "Armor",
    "Armor",
    "Armor"
};

static char *helmet_name[]={
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet",
    "Helmet"
};

static char *leggings_name[]={
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings",
    "Leggings"
};

static char *desc[]={
    "Light chain mail %s made of %s.",
    "Chain mail %s made of %s.",
    "Heavy chain mail %s made of %s.",
    "Light scale mail %s made of %s.",
    "Scale mail %s made of %s.",
    "Heavy scale mail %s made of %s.",
    "Light plate mail %s made of %s.",
    "Plate mail %s made of %s.",
    "Heavy plate mail %s made of %s.",
    "Full plate mail %s made of %s."
};

static char *material[3]={
    "iron",
    "steel",
    "mithril"
};

static char *name[4]={
    "sleeves",
    "helmet",
    "armor",
    "leggings"
};

/*# light iron chainmail
armor:
name="Armor"
description="A light chain mail shirt made of iron."
value=50
max_damage=10000
sprite=501
mod_index=V_ARMOR
mod_value=40
req_index=V_ARMORSKILL
req_value=1
flag=IF_TAKE
flag=IF_WNBODY
;*/

void create_armor(char *xname,char *textname[],char *xdesc[],char *descname,char *xmaterial,int costbase,int armorbase,char *placement,int quality,int spritebase,double mod) {
    int n;

    for (n=0; n<10; n++) {
        printf("%s%dq%d:\n",xname,n+1,quality+1);
        printf("name=\"%s\"\n",textname[n]);
        printf("description=\""); printf(xdesc[n],descname,xmaterial); printf("\"\n");
        if (costbase>20) printf("value=%d\n",costbase*(1<<n));
        else printf("value=%d\n",costbase*n*n+5);

        printf("sprite=%d\n",spritebase+n);
        printf("mod_index=V_ARMOR\n");
        printf("mod_value=%d\n",(int)((armorbase+(n+1)*10)*mod));
        printf("req_index=V_ARMORSKILL\n");
        printf("req_value=%d\n",max(1,n*10));
        printf("flag=IF_TAKE\n");
        printf("flag=%s\n",placement);
        printf(";\n");
    }
}

int main(void) {
    int n;
    static int basetab[3]={-2,0,4};
    static int costtab[3]={1,5,25};

    for (n=0; n<3; n++) {
        create_armor(name[0],sleeves_name,desc,name[0],material[n],5*costtab[n],basetab[n],"IF_WNARMS",n,10020+n*50,20.0/100.0*15.0);
    }
    for (n=0; n<3; n++) {
        create_armor(name[1],helmet_name,desc,name[1],material[n],7*costtab[n],basetab[n],"IF_WNHEAD",n,10030+n*50,20.0/100.0*20.0);
    }
    for (n=0; n<3; n++) {
        create_armor(name[2],armor_name,desc,name[2],material[n],10*costtab[n],basetab[n],"IF_WNBODY",n,10040+n*50,20.0/100.0*50.0);
    }
    for (n=0; n<3; n++) {
        create_armor(name[3],leggings_name,desc,name[3],material[n],5*costtab[n],basetab[n],"IF_WNLEGS",n,10050+n*50,20.0/100.0*15.0);
    }

    return 0;
}
