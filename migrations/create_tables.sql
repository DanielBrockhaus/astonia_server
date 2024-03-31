-- MySQL dump 8.22
--
-- Host: localhost    Database: 
-- -------------------------------------------------------
-- Server version	3.23.56

--
-- Current Database: merc
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ merc;

USE merc;

--
-- Table structure for table 'area'
--

CREATE TABLE area (
  ID int(11) default NULL,
  mirror int(11) default NULL,
  name char(80) default NULL,
  players int(11) default NULL,
  alive_time int(11) default NULL,
  idle int(11) default NULL,
  server int(10) unsigned default NULL,
  port int(11) default NULL,
  bps int(11) NOT NULL default '0',
  mem_usage int(11) NOT NULL default '0',
  last_error int(11) NOT NULL default '0',
  UNIQUE KEY name (name),
  KEY ID (ID),
  KEY mirror (mirror)
);

--
-- Table structure for table 'badip'
--

CREATE TABLE badip (
  IP int(10) unsigned NOT NULL default '0',
  t int(11) NOT NULL default '0',
  KEY lookup (IP,t),
  KEY t (t)
);

--
-- Table structure for table 'badname'
--

CREATE TABLE badname (
  ID int(11) NOT NULL auto_increment,
  bad char(40) NOT NULL default '',
  PRIMARY KEY  (ID),
  UNIQUE KEY bad (bad)
);

--
-- Table structure for table 'blackcc'
--

CREATE TABLE blackcc (
  ID int(11) NOT NULL auto_increment,
  nr char(20) NOT NULL default '',
  PRIMARY KEY  (ID),
  UNIQUE KEY nr (nr)
);

--
-- Table structure for table 'blackip'
--

CREATE TABLE blackip (
  ID int(11) NOT NULL auto_increment,
  ip int(10) unsigned NOT NULL default '0',
  cnt int(11) NOT NULL default '0',
  created int(11) NOT NULL default '0',
  PRIMARY KEY  (ID),
  KEY ip (ip),
  KEY created (created)
);

--
-- Table structure for table 'charinfo'
--

CREATE TABLE charinfo (
  ID int(11) NOT NULL default '0',
  name char(40) NOT NULL default '',
  class int(11) NOT NULL default '0',
  karma int(11) NOT NULL default '0',
  clan int(11) NOT NULL default '0',
  clan_rank int(11) NOT NULL default '0',
  clan_serial int(11) NOT NULL default '0',
  experience int(11) NOT NULL default '0',
  current_area int(11) NOT NULL default '0',
  creation_time int(11) NOT NULL default '0',
  login_time int(11) NOT NULL default '0',
  logout_time int(11) NOT NULL default '0',
  locked enum('Y','N') NOT NULL default 'Y',
  sID int(11) NOT NULL default '0',
  PRIMARY KEY  (ID),
  UNIQUE KEY name (name),
  KEY experience (experience),
  KEY current_area (current_area),
  KEY sID (sID),
  KEY clan (clan)
);

--
-- Table structure for table 'chars'
--

CREATE TABLE chars (
  ID int(11) NOT NULL auto_increment,
  name varchar(40) NOT NULL default '',
  class int(11) default NULL,
  karma int(11) default NULL,
  clan int(11) default NULL,
  clan_rank int(11) default NULL,
  clan_serial int(11) default NULL,
  experience int(11) default NULL,
  current_area int(11) default NULL,
  allowed_area int(11) default NULL,
  creation_time int(11) default NULL,
  login_time int(11) default NULL,
  logout_time int(11) default NULL,
  locked enum('Y','N') default NULL,
  sID int(11) default NULL,
  chr blob,
  item blob,
  ppd blob,
  mirror int(11) default NULL,
  current_mirror int(11) default NULL,
  spacer bigint(20) default NULL,
  PRIMARY KEY  (ID),
  UNIQUE KEY name (name),
  KEY experience (experience),
  KEY current_area (current_area),
  KEY sID (sID),
  KEY clan (clan)
);

--
-- Table structure for table 'clanlog'
--

CREATE TABLE clanlog (
  ID int(11) NOT NULL auto_increment,
  time int(11) NOT NULL default '0',
  clan int(11) NOT NULL default '0',
  serial int(11) NOT NULL default '0',
  cID int(11) NOT NULL default '0',
  prio int(11) NOT NULL default '0',
  content text NOT NULL,
  PRIMARY KEY  (ID),
  KEY time (time),
  KEY clan (clan),
  KEY cID (cID),
  KEY prio (prio)
);

--
-- Table structure for table 'clubs'
--

CREATE TABLE clubs (
  ID int(11) NOT NULL default '0',
  name char(80) NOT NULL default '',
  paid int(11) NOT NULL default '0',
  money int(11) NOT NULL default '0',
  serial int(11) NOT NULL default '0',
  PRIMARY KEY  (ID)
);

--
-- Table structure for table 'constants'
--

CREATE TABLE constants (
  name char(8) NOT NULL default '',
  val int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (name)
);

--
-- Table structure for table 'ipban'
--

CREATE TABLE ipban (
  ID int(11) NOT NULL auto_increment,
  ip int(10) unsigned NOT NULL default '0',
  created int(11) NOT NULL default '0',
  valid int(11) NOT NULL default '0',
  sID int(11) NOT NULL default '0',
  PRIMARY KEY  (ID),
  KEY ip (ip),
  KEY created (created),
  KEY valid (valid)
);

--
-- Table structure for table 'iplog'
--

CREATE TABLE iplog (
  ID int(11) NOT NULL auto_increment,
  ip int(10) unsigned default NULL,
  use_time int(11) default NULL,
  use_count int(11) default NULL,
  sID int(11) default NULL,
  PRIMARY KEY  (ID),
  KEY ip (ip),
  KEY use_time (use_time),
  KEY use_count (use_count),
  KEY sID (sID)
);

--
-- Table structure for table 'notes'
--

CREATE TABLE notes (
  ID int(11) NOT NULL auto_increment,
  uID int(11) NOT NULL default '0',
  kind int(11) default NULL,
  cID int(11) default NULL,
  date int(11) default NULL,
  content blob,
  PRIMARY KEY  (ID),
  KEY kind (kind),
  KEY cID (cID),
  KEY uID (uID),
  KEY date (date)
);

--
-- Table structure for table 'pin'
--

CREATE TABLE pin (
  pin char(16) NOT NULL default '',
  ctime int(11) NOT NULL default '0',
  utime int(11) NOT NULL default '0',
  sID int(11) NOT NULL default '0',
  IP int(10) unsigned NOT NULL default '0',
  status enum('new','used') NOT NULL default 'new',
  PRIMARY KEY  (pin),
  KEY status (status),
  KEY ctime (ctime),
  KEY utime (utime),
  KEY sID (sID),
  KEY IP (IP)
);

--
-- Table structure for table 'poll'
--

CREATE TABLE poll (
  name char(80) NOT NULL default '',
  description char(240) NOT NULL default '',
  choice1 char(80) default NULL,
  choice2 char(80) default NULL,
  choice3 char(80) default NULL,
  choice4 char(80) default NULL,
  choice5 char(80) default NULL,
  choice6 char(80) default NULL,
  choice7 char(80) default NULL,
  choice8 char(80) default NULL,
  votes1 int(11) NOT NULL default '0',
  votes2 int(11) NOT NULL default '0',
  votes3 int(11) NOT NULL default '0',
  votes4 int(11) NOT NULL default '0',
  votes5 int(11) NOT NULL default '0',
  votes6 int(11) NOT NULL default '0',
  votes7 int(11) NOT NULL default '0',
  votes8 int(11) NOT NULL default '0'
);

--
-- Table structure for table 'pvp'
--

CREATE TABLE pvp (
  ID bigint(20) NOT NULL default '0',
  cc char(16) NOT NULL default '',
  co char(16) NOT NULL default '',
  what enum('kill','assist','final') NOT NULL default 'kill',
  damage int(11) NOT NULL default '0',
  date int(11) NOT NULL default '0',
  KEY cc (cc),
  KEY co (co),
  KEY ID (ID),
  KEY spec1 (what,cc),
  KEY spec2 (what,co),
  KEY spec3 (cc,co,what,date)
);

--
-- Table structure for table 'pvplist'
--

CREATE TABLE pvplist (
  name char(16) NOT NULL default '',
  kills int(11) NOT NULL default '0',
  deaths int(11) NOT NULL default '0',
  PRIMARY KEY  (name),
  KEY kills (kills)
);

--
-- Table structure for table 'pvplist30'
--

CREATE TABLE pvplist30 (
  name char(16) NOT NULL default '',
  kills int(11) NOT NULL default '0',
  deaths int(11) NOT NULL default '0',
  PRIMARY KEY  (name),
  KEY kills (kills)
);

--
-- Table structure for table 'pvplist7'
--

CREATE TABLE pvplist7 (
  name char(16) NOT NULL default '',
  kills int(11) NOT NULL default '0',
  deaths int(11) NOT NULL default '0',
  PRIMARY KEY  (name),
  KEY kills (kills)
);

--
-- Table structure for table 'stats'
--

CREATE TABLE stats (
  name char(10) NOT NULL default '',
  value int(11) NOT NULL default '0',
  PRIMARY KEY  (name)
);

--
-- Table structure for table 'storage'
--

CREATE TABLE storage (
  ID int(11) NOT NULL default '0',
  description varchar(80) default NULL,
  version int(11) default NULL,
  content blob,
  PRIMARY KEY  (ID)
);

--
-- Table structure for table 'subscriber'
--

CREATE TABLE subscriber (
  ID int(11) NOT NULL auto_increment,
  email char(80) default NULL,
  first_name char(32) default NULL,
  last_name char(64) default NULL,
  address1 char(80) default NULL,
  address2 char(80) default NULL,
  address3 char(80) default NULL,
  country char(80) default NULL,
  birthdate int(11) default NULL,
  gender enum('M','F') default NULL,
  password char(16) default NULL,
  credit_nr char(32) default NULL,
  credit_date char(16) default NULL,
  credit_type char(1) default NULL,
  credit_hold char(80) default NULL,
  payment_type int(11) NOT NULL default '0',
  paid_till int(11) NOT NULL default '0',
  creation_time int(11) default NULL,
  locked enum('Y','N') default NULL,
  banned enum('Y','N','I') default NULL,
  admin enum('N','M','Y') default 'N',
  email_state int(11) default NULL,
  retry enum('Y','N') NOT NULL default 'N',
  vendor int(11) NOT NULL default '0',
  note char(80) default NULL,
  credit_cvs char(8) default NULL,
  PRIMARY KEY  (ID),
  KEY email (email),
  KEY birthdate (birthdate),
  KEY creation_time (creation_time),
  KEY paid_till (paid_till),
  KEY last_name (last_name),
  KEY first_name (first_name),
  KEY country (country),
  KEY credit_nr (credit_nr),
  KEY retry (retry),
  KEY vendor (vendor)
);

--
-- Table structure for table 'task'
--

CREATE TABLE task (
  ID int(11) NOT NULL auto_increment,
  content blob,
  PRIMARY KEY  (ID)
);

--
-- Table structure for table 'trans'
--

CREATE TABLE trans (
  ID int(11) NOT NULL auto_increment,
  type enum('A','C') NOT NULL default 'A',
  sID int(11) NOT NULL default '0',
  tax enum('E','N') NOT NULL default 'E',
  vendor int(11) NOT NULL default '0',
  creation_time int(11) NOT NULL default '0',
  amount int(11) NOT NULL default '0',
  nr char(32) NOT NULL default '',
  date char(16) default NULL,
  owner char(80) default NULL,
  rc int(11) default NULL,
  aid char(32) default NULL,
  ref char(32) default NULL,
  ip int(10) unsigned NOT NULL default '0',
  oID int(10) unsigned NOT NULL default '0',
  cvs char(8) default NULL,
  PRIMARY KEY  (ID),
  KEY type (type),
  KEY sID (sID),
  KEY parent (vendor),
  KEY creation_time (creation_time),
  KEY nr (nr),
  KEY rc (rc),
  KEY aid (aid),
  KEY ref (ref)
);

--
-- Table structure for table 'vote'
--

CREATE TABLE vote (
  uID int(11) NOT NULL default '0',
  voteID int(11) default NULL,
  PRIMARY KEY  (uID)
);

--
-- Table structure for table 'whitecc'
--

CREATE TABLE whitecc (
  ID int(11) NOT NULL auto_increment,
  nr char(20) NOT NULL default '',
  PRIMARY KEY  (ID),
  UNIQUE KEY nr (nr)
);
