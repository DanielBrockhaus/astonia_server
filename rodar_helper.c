/*
 * Part of Astonia Server (c) Daniel Brockhaus. Please read license.txt.
 */

/*
Tables

Teams:
founderID = player ID of the founder. might come in handy.

create table rodar_team (
    ID int not null auto_increment,
    name char(80) not null,
    founderID int not null,
    type enum ('2','3','5','7','12','any') not null default 'any',
    wins int not null default 0,
    losses int not null default 0,
    kills int not null default 0,
    killed int not null default 0,
    score int not null default 0,
    primary key(ID),
    unique key(name),
    key(score)
);

Team Members:
tID = Team ID, ID from rodar_teams
pID = player ID

create table rodar_member (
    tID int not null,
    pID int not null,
    type enum ('member','admin','owner'),
    primary key(tID,pID),
    key(pID)
);

Events Schedule:

t = date/time the event starts
winnerID = winning team ID from rodar_team

Events will be created one week in advance.

create table rodar_schedule (
    ID int not null auto_increment,
    t timestamp not null default 0,
    type enum ('2','3','5','7','12','any') not null default 'any',
    option set ('open','clan','nomagic','nofreeze','nowarcry') not null default (''),
    winnerID int default null,
    primary key(ID),
    key(t)
);

We might want to add more logging eventually, like individual kills.

 */
