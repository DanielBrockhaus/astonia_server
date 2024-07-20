
create table rodar_team (
    ID int not null auto_increment,
    name char(80) not null,
    founderID int,
    founded timestamp not null default now(),
    status enum ('active','banned','retired') not null default 'active',
    wins int not null default 0,
    losses int not null default 0,
    kills int not null default 0,
    killed int not null default 0,
    score int not null default 0,
    primary key(ID),
    unique key(name),
    key(score),
    foreign key(founderID) references chars(ID) on delete set null
);

create table rodar_member (
    teamID int not null,
    charID int not null,
    type enum ('member','admin','owner') not null,
    primary key(teamID,charID),
    key(charID),
    foreign key(charID) references chars(ID) on delete cascade
);

create table rodar_event (
    ID int not null auto_increment,
    t timestamp not null default 0,
    type enum ('2','3','5','7','12','any') not null default 'any',
    option set ('nomagic','nofreeze','nowarcry') not null default (''),
    room int not null default 1,
    level int not null default 200,
    winnerID int default null,
    primary key(ID),
    key(t)
);

insert into area values (38,1,'Rodneys Arena',0,0,0,0,0,0,0,0);

