all: server runtime/generic/base.dll runtime/generic/lostcon.dll \
runtime/generic/merchant.dll runtime/generic/simple_baddy.dll \
zones/generic/weapons.itm runtime/generic/bank.dll \
zones/generic/armor.itm chatserver runtime/1/gwendylon.dll \
runtime/2/area2.dll runtime/3/area3.dll runtime/generic/book.dll \
runtime/generic/transport.dll runtime/generic/clanmaster.dll \
runtime/generic/pents.dll \
runtime/generic/alchemy.dll \
runtime/3/gatekeeper.dll runtime/6/edemon.dll \
runtime/3/military.dll runtime/8/fdemon.dll \
update_server runtime/10/ice.dll runtime/11/palace.dll \
runtime/generic/mine.dll runtime/3/arena.dll \
runtime/13/dungeon.dll runtime/14/random.dll \
runtime/15/swamp.dll runtime/generic/professor.dll \
runtime/16/forest.dll runtime/17/two.dll \
runtime/18/bones.dll runtime/19/nomad.dll \
runtime/20/lq.dll runtime/22/lab1.dll \
runtime/22/lab2.dll runtime/23/strategy.dll runtime/22/lab3.dll \
runtime/22/lab4.dll runtime/22/lab5.dll \
runtime/19/saltmine.dll runtime/5/sewers.dll \
runtime/generic/sidestory.dll runtime/25/warped.dll \
runtime/1/shrike.dll runtime/26/staffer.dll \
runtime/29/staffer2.dll runtime/28/staffer3.dll \
runtime/31/warrmines.dll runtime/36/caligar.dll \
runtime/32/missions.dll runtime/generic/clubmaster.dll \
runtime/33/tunnel.dll runtime/34/teufel.dll \
runtime/37/arkhata.dll create_account create_character

CC=gcc
DEBUG=-g
CFLAGS=-Wall -Wshadow -Wno-format-truncation -Wno-pointer-sign -O3 $(DEBUG) -fno-strict-aliasing -m32 -DSTAFF
LDFLAGS=-O $(DEBUG) -m32 -L/usr/lib/mysql
LDRFLAGS=-O $(DEBUG) -m32 -rdynamic -L/usr/lib/mysql
DDFLAGS=-O $(DEBUG) -m32 -fPIC -shared
DFLAGS=$(CFLAGS) -m32 -fPIC
DATE=`date +%y%m%d%H`

OBJS=.obj/server.o .obj/io.o .obj/libload.o .obj/tool.o .obj/sleep.o \
.obj/log.o .obj/create.o .obj/notify.o .obj/skill.o .obj/do.o \
.obj/act.o .obj/player.o .obj/rdtsc.o .obj/los.o .obj/light.o \
.obj/map.o .obj/path.o .obj/error.o .obj/talk.o .obj/drdata.o \
.obj/death.o .obj/database.o .obj/see.o .obj/drvlib.o .obj/timer.o \
.obj/expire.o .obj/effect.o .obj/command.o .obj/date.o \
.obj/container.o .obj/store.o .obj/mem.o .obj/sector.o .obj/chat.o \
.obj/statistics.o .obj/mail.o .obj/player_driver.o .obj/clan.o \
.obj/lookup.o .obj/area.o .obj/task.o .obj/punish.o .obj/depot.o \
.obj/prof.o .obj/motd.o .obj/ignore.o .obj/tell.o .obj/clanlog.o \
.obj/respawn.o .obj/poison.o .obj/swear.o .obj/lab.o \
.obj/consistency.o .obj/btrace.o .obj/club.o .obj/teufel_pk.o \
.obj/questlog.o .obj/badip.o


# ------- Server -----

server:	$(OBJS)
	$(CC) $(LDRFLAGS) -o server $(OBJS) -lmysqlclient -lm -lz -ldl -lpthread

.obj/server.o:		src/server.c src/server.h src/client.h src/player.h src/io.h src/notify.h src/libload.h src/tool.h src/sleep.h src/log.h src/create.h src/direction.h src/act.h src/los.h src/path.h src/timer.h src/effect.h src/database.h src/map.h src/date.h src/container.h src/store.h src/mem.h src/sector.h src/chat.h
	$(CC) $(CFLAGS) -o .obj/server.o -c src/server.c

.obj/io.o:		src/io.c src/server.h src/client.h src/player.h src/log.h src/mem.h src/io.h
	$(CC) $(CFLAGS) -o $*.o -c $<

.obj/libload.o:		src/libload.c src/server.h src/tool.h src/log.h src/notify.h src/player.h src/mem.h src/libload.h
	$(CC) $(CFLAGS) -o .obj/libload.o -c src/libload.c

.obj/tool.o:		src/tool.c src/server.h src/log.h src/talk.h src/direction.h src/create.h src/skill.h src/player.h src/tool.h
	$(CC) $(CFLAGS) -o .obj/tool.o -c src/tool.c

.obj/questlog.o:	src/questlog.c src/server.h src/log.h src/talk.h src/direction.h src/create.h src/skill.h src/player.h src/tool.h
	$(CC) $(CFLAGS) -o .obj/questlog.o -c src/questlog.c

.obj/depot.o:		src/depot.c src/server.h src/log.h src/talk.h src/direction.h src/create.h src/skill.h src/player.h src/depot.h src/tool.h
	$(CC) $(CFLAGS) -o .obj/depot.o -c src/depot.c

.obj/sleep.o:		src/sleep.c src/server.h src/log.h src/sleep.h
	$(CC) $(CFLAGS) -o .obj/sleep.o -c src/sleep.c

.obj/log.o:		src/log.c src/log.h src/server.h
	$(CC) $(CFLAGS) -o .obj/log.o -c src/log.c

.obj/create.o:		src/create.c src/server.h src/tool.h src/log.h src/skill.h src/light.h src/player.h src/direction.h src/timer.h src/mem.h src/notify.h src/libload.h src/sector.h src/create.h src/balance.h
	$(CC) $(CFLAGS) -o .obj/create.o -c src/create.c

.obj/notify.o:		src/notify.c src/server.h src/log.h src/see.h src/mem.h src/sector.h src/notify.h src/create.h
	$(CC) $(CFLAGS) -o .obj/notify.o -c src/notify.c

.obj/teufel_pk.o:	src/teufel_pk.c src/server.h src/log.h src/see.h src/mem.h src/sector.h src/notify.h src/create.h
	$(CC) $(CFLAGS) -o .obj/teufel_pk.o -c src/teufel_pk.c

.obj/skill.o:		src/skill.c src/server.h src/create.h src/database.h src/log.h src/skill.h
	$(CC) $(CFLAGS) -o .obj/skill.o -c src/skill.c

.obj/do.o:		src/do.c src/server.h src/tool.h src/direction.h src/act.h src/error.h src/create.h src/drvlib.h src/talk.h src/see.h src/container.h src/log.h src/notify.h src/libload.h src/database.h src/spell.h src/effect.h src/map.h src/do.h
	$(CC) $(CFLAGS) -o .obj/do.o -c src/do.c

.obj/act.o:		src/act.c src/server.h src/log.h src/direction.h src/notify.h src/libload.h src/light.h src/tool.h src/map.h src/death.h src/create.h src/effect.h src/timer.h src/talk.h src/drvlib.h src/database.h src/drdata.h src/do.h src/see.h src/spell.h src/container.h src/path.h src/sector.h src/act.h src/balance.h
	$(CC) $(CFLAGS) -o .obj/act.o -c src/act.c

.obj/player.o:		src/player.c src/mail.h src/server.h src/do.h src/log.h src/io.h src/client.h src/map.h src/database.h src/create.h src/see.h src/notify.h src/player.h src/los.h src/effect.h src/talk.h src/drvlib.h src/direction.h src/drdata.h src/act.h src/command.h src/container.h src/date.h src/skill.h src/store.h src/libload.h src/death.h src/tool.h src/sector.h
	$(CC) $(CFLAGS) -o .obj/player.o -c src/player.c

.obj/player_driver.o:	src/player_driver.c src/server.h src/do.h src/log.h src/io.h src/client.h src/map.h src/database.h src/create.h src/see.h src/notify.h src/player_driver.h src/los.h src/effect.h src/talk.h src/drvlib.h src/direction.h src/drdata.h src/act.h src/command.h src/container.h src/date.h src/skill.h src/store.h src/libload.h src/death.h src/tool.h src/sector.h
	$(CC) $(CFLAGS) -o .obj/player_driver.o -c src/player_driver.c

.obj/rdtsc.o:		src/rdtsc.S
	$(CC) $(CFLAGS) -o .obj/rdtsc.o -c src/rdtsc.S

.obj/los.o:		src/los.c src/server.h src/log.h src/light.h src/mem.h src/sector.h src/los.h
	$(CC) $(CFLAGS) -o .obj/los.o -c src/los.c

.obj/consistency.o:		src/consistency.c src/server.h src/log.h src/light.h src/mem.h src/sector.h src/consistency.h
	$(CC) $(CFLAGS) -o .obj/consistency.o -c src/consistency.c

.obj/light.o:		src/light.c src/server.h src/log.h src/los.h src/sector.h src/light.h
	$(CC) $(CFLAGS) -o .obj/light.o -c src/light.c

.obj/map.o:		src/map.c src/server.h src/light.h src/log.h src/create.h src/expire.h src/effect.h src/drdata.h src/notify.h src/libload.h src/container.h src/sector.h src/map.h
	$(CC) $(CFLAGS) -o .obj/map.o -c src/map.c

.obj/path.o:		src/path.c src/server.h src/direction.h src/log.h src/mem.h src/path.h
	$(CC) $(CFLAGS) -o .obj/path.o -c src/path.c

.obj/prof.o:		src/prof.c src/server.h src/talk.h src/tool.h src/log.h src/mem.h src/prof.h
	$(CC) $(CFLAGS) -o .obj/prof.o -c src/prof.c

.obj/motd.o:		src/motd.c src/server.h src/talk.h src/tool.h src/log.h src/mem.h src/motd.h
	$(CC) $(CFLAGS) -o .obj/motd.o -c src/motd.c

.obj/error.o:		src/error.c src/error.h
	$(CC) $(CFLAGS) -o .obj/error.o -c src/error.c

.obj/talk.o:		src/talk.c src/server.h src/notify.h src/log.h src/player.h src/see.h src/path.h src/mem.h src/sector.h src/talk.h
	$(CC) $(CFLAGS) -o .obj/talk.o -c src/talk.c

.obj/drdata.o:		src/drdata.c src/server.h src/log.h src/mem.h src/drdata.h
	$(CC) $(CFLAGS) -o .obj/drdata.o -c src/drdata.c

.obj/death.o:		src/death.c src/server.h src/log.h src/timer.h src/map.h src/notify.h src/create.h src/drdata.h src/libload.h src/direction.h src/error.h src/act.h src/talk.h src/expire.h src/effect.h src/database.h src/tool.h src/container.h src/player.h src/sector.h src/death.h
	$(CC) $(CFLAGS) -o .obj/death.o -c src/death.c

.obj/database.o:	src/database.c src/server.h src/log.h src/create.h src/player.h src/sleep.h src/tool.h src/drdata.h src/drvlib.h src/timer.h src/direction.h src/map.h src/mem.h src/database.h src/misc_ppd.h src/badip.h
	$(CC) $(CFLAGS) -o .obj/database.o -c src/database.c

.obj/lookup.o:	src/lookup.c src/server.h src/lookup.h src/log.h src/create.h src/player.h src/sleep.h src/tool.h src/drdata.h src/drvlib.h src/timer.h src/direction.h src/map.h src/mem.h src/database.h
	$(CC) $(CFLAGS) -o .obj/lookup.o -c src/lookup.c

.obj/see.o:		src/see.c src/server.h src/los.h src/log.h src/date.h src/see.h
	$(CC) $(CFLAGS) -o .obj/see.o -c src/see.c

.obj/drvlib.o:		src/drvlib.c src/server.h src/drdata.h src/direction.h src/error.h src/notify.h src/path.h src/do.h src/see.h src/talk.h src/map.h src/container.h src/timer.h src/libload.h src/spell.h src/tool.h src/effect.h src/create.h src/drvlib.h src/sector.h
	$(CC) $(CFLAGS) -o .obj/drvlib.o -c src/drvlib.c

.obj/timer.o:		src/timer.c src/server.h src/log.h src/mem.h src/timer.h
	$(CC) $(CFLAGS) -o .obj/timer.o -c src/timer.c

.obj/expire.o:		src/expire.c src/server.h src/log.h src/timer.h src/map.h src/create.h src/container.h src/expire.h
	$(CC) $(CFLAGS) -o .obj/expire.o -c src/expire.c

.obj/effect.o:		src/effect.c src/server.h src/log.h src/notify.h src/death.h src/light.h src/tool.h src/spell.h src/los.h src/mem.h src/sector.h src/effect.h
	$(CC) $(CFLAGS) -o .obj/effect.o -c src/effect.c

.obj/command.o:		src/command.c src/server.h src/talk.h src/log.h src/tool.h src/skill.h src/database.h src/date.h src/do.h src/map.h src/command.h src/chat.h src/misc_ppd.h
	$(CC) $(CFLAGS) -o .obj/command.o -c src/command.c

.obj/date.o:		src/date.c src/server.h src/talk.h src/date.h
	$(CC) $(CFLAGS) -o .obj/date.o -c src/date.c

.obj/container.o:	src/container.c src/server.h src/log.h src/create.h src/mem.h src/container.h
	$(CC) $(CFLAGS) -o .obj/container.o -c src/container.c

.obj/store.o:	src/store.c src/server.h src/log.h src/error.h src/create.h src/tool.h src/talk.h src/mem.h src/store.h
	$(CC) $(CFLAGS) -o .obj/store.o -c src/store.c

.obj/mem.o:		src/mem.c src/log.h src/mem.h
	$(CC) $(CFLAGS) -DDEBUG -o .obj/mem.o -c src/mem.c

.obj/sector.o:		src/sector.c src/server.h src/mem.h src/log.h src/sector.h src/tool.h
	$(CC) $(CFLAGS) -o .obj/sector.o -c src/sector.c

.obj/chat.o:		src/chat.c src/chat.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/chat.o -c src/chat.c

.obj/statistics.o:	src/statistics.c src/statistics.h src/server.h src/mem.h src/drdata.h
	$(CC) $(CFLAGS) -o .obj/statistics.o -c src/statistics.c

.obj/mail.o:		src/mail.c src/mail.h
	$(CC) $(CFLAGS) -o .obj/mail.o -c src/mail.c

.obj/clan.o:		src/clan.c src/server.h
	$(CC) $(CFLAGS) -o .obj/clan.o -c src/clan.c

.obj/club.o:		src/club.c src/server.h
	$(CC) $(CFLAGS) -o .obj/club.o -c src/club.c

.obj/area.o:		src/area.c src/area.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/area.o -c src/area.c

.obj/task.o:		src/task.c src/task.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/task.o -c src/task.c

.obj/punish.o:		src/punish.c src/punish.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/punish.o -c src/punish.c

.obj/ignore.o:		src/ignore.c src/ignore.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/ignore.o -c src/ignore.c

.obj/tell.o:		src/tell.c src/tell.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/tell.o -c src/tell.c

.obj/clanlog.o:		src/clanlog.c src/clanlog.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/clanlog.o -c src/clanlog.c

.obj/respawn.o:		src/respawn.c src/respawn.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/respawn.o -c src/respawn.c

.obj/poison.o:		src/poison.c src/poison.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/poison.o -c src/poison.c

.obj/swear.o:		src/swear.c src/swear.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/swear.o -c src/swear.c

.obj/lab.o:		src/lab.c src/lab.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/lab.o -c src/lab.c

.obj/btrace.o:		src/btrace.c src/btrace.h
	$(CC) $(CFLAGS) -o .obj/btrace.o -c src/btrace.c

.obj/badip.o:		src/badip.c src/badip.h src/log.h src/talk.h src/server.h src/mem.h
	$(CC) $(CFLAGS) -o .obj/badip.o -c src/badip.c

# ------- DLLs -------

runtime/generic/base.dll:	.obj/base.o
	$(CC) $(DDFLAGS) -o base.tmp .obj/base.o
	@mkdir -p runtime/generic
	@mv base.tmp runtime/generic/base.dll

.obj/base.o:		src/base.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/base.o -c src/base.c

runtime/generic/sidestory.dll:	.obj/sidestory.o
	$(CC) $(DDFLAGS) -o sidestory.tmp .obj/sidestory.o
	@mkdir -p runtime/generic
	@mv sidestory.tmp runtime/generic/sidestory.dll

.obj/sidestory.o:		src/sidestory.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/sidestory.o -c src/sidestory.c

runtime/generic/pents.dll:	.obj/pents.o
	$(CC) $(DDFLAGS) -o pents.tmp .obj/pents.o
	@mkdir -p runtime/generic
	@mv pents.tmp runtime/generic/pents.dll

.obj/pents.o:		src/pents.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/pents.o -c src/pents.c

runtime/generic/professor.dll:	.obj/professor.o
	$(CC) $(DDFLAGS) -o professor.tmp .obj/professor.o
	@mkdir -p runtime/generic
	@mv professor.tmp runtime/generic/professor.dll

.obj/professor.o:		src/professor.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/professor.o -c src/professor.c

runtime/generic/bank.dll:	.obj/bank.o
	$(CC) $(DDFLAGS) -o bank.tmp .obj/bank.o
	@mkdir -p runtime/generic
	@mv bank.tmp runtime/generic/bank.dll

.obj/bank.o:		src/bank.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/bank.o -c src/bank.c

runtime/generic/alchemy.dll:	.obj/alchemy.o
	$(CC) $(DDFLAGS) -o alchemy.tmp .obj/alchemy.o
	@mkdir -p runtime/generic
	@mv alchemy.tmp runtime/generic/alchemy.dll

.obj/alchemy.o:		src/alchemy.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/alchemy.o -c src/alchemy.c

runtime/generic/book.dll:	.obj/book.o
	$(CC) $(DDFLAGS) -o book.tmp .obj/book.o
	@mkdir -p runtime/generic
	@mv book.tmp runtime/generic/book.dll

.obj/book.o:		src/book.c src/server.h src/book.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/book.o -c src/book.c

runtime/generic/transport.dll:	.obj/transport.o
	$(CC) $(DDFLAGS) -o transport.tmp .obj/transport.o
	@mkdir -p runtime/generic
	@mv transport.tmp runtime/generic/transport.dll

.obj/transport.o:		src/transport.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/transport.o -c src/transport.c

runtime/generic/clanmaster.dll:	.obj/clanmaster.o
	$(CC) $(DDFLAGS) -o clanmaster.tmp .obj/clanmaster.o
	@mkdir -p runtime/generic
	@mv clanmaster.tmp runtime/generic/clanmaster.dll

.obj/clanmaster.o:		src/clanmaster.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/clanmaster.o -c src/clanmaster.c

runtime/generic/clubmaster.dll:	.obj/clubmaster.o
	$(CC) $(DDFLAGS) -o clubmaster.tmp .obj/clubmaster.o
	@mkdir -p runtime/generic
	@mv clubmaster.tmp runtime/generic/clubmaster.dll

.obj/clubmaster.o:		src/clubmaster.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/map.h src/create.h src/container.h src/tool.h src/spell.h src/effect.h src/light.h src/los.h
	$(CC) $(DFLAGS) -o .obj/clubmaster.o -c src/clubmaster.c

runtime/generic/lostcon.dll:	.obj/lostcon.o
	$(CC) $(DDFLAGS) -o lostcon.tmp .obj/lostcon.o
	@mkdir -p runtime/generic
	@mv lostcon.tmp runtime/generic/lostcon.dll

.obj/lostcon.o:		src/lostcon.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/player.h
	$(CC) $(DFLAGS) -o .obj/lostcon.o -c src/lostcon.c

runtime/generic/merchant.dll:	.obj/merchant.o
	$(CC) $(DDFLAGS) -o merchant.tmp .obj/merchant.o
	@mkdir -p runtime/generic
	@mv merchant.tmp runtime/generic/merchant.dll

.obj/merchant.o:	src/merchant.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h
	$(CC) $(DFLAGS) -o .obj/merchant.o -c src/merchant.c

runtime/generic/simple_baddy.dll:	.obj/simple_baddy.o
	$(CC) $(DDFLAGS) -o simple_baddy.tmp .obj/simple_baddy.o
	@mkdir -p runtime/generic
	@mv simple_baddy.tmp runtime/generic/simple_baddy.dll

.obj/simple_baddy.o:		src/simple_baddy.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h
	$(CC) $(DFLAGS) -o .obj/simple_baddy.o -c src/simple_baddy.c

runtime/1/gwendylon.dll:	.obj/gwendylon.o
	$(CC) $(DDFLAGS) -o gwendylon.tmp .obj/gwendylon.o
	@mkdir -p runtime/1
	@mv gwendylon.tmp runtime/1/gwendylon.dll

.obj/gwendylon.o:	src/gwendylon.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/gwendylon.o -c src/gwendylon.c

runtime/1/shrike.dll:	.obj/shrike.o
	$(CC) $(DDFLAGS) -o shrike.tmp .obj/shrike.o
	@mkdir -p runtime/1
	@mv shrike.tmp runtime/1/shrike.dll

.obj/shrike.o:	src/shrike.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/shrike.o -c src/shrike.c

runtime/2/area2.dll:	.obj/area2.o
	$(CC) $(DDFLAGS) -o area2.tmp .obj/area2.o
	@mkdir -p runtime/2
	@mv area2.tmp runtime/2/area2.dll

.obj/area2.o:	src/area2.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/area2.o -c src/area2.c

runtime/3/area3.dll:	.obj/area3.o
	$(CC) $(DDFLAGS) -o area3.tmp .obj/area3.o
	@mkdir -p runtime/3
	@mv area3.tmp runtime/3/area3.dll

.obj/area3.o:	src/area3.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/area3.o -c src/area3.c

runtime/37/arkhata.dll:	.obj/arkhata.o
	$(CC) $(DDFLAGS) -o arkhata.tmp .obj/arkhata.o
	@mkdir -p runtime/37
	@mv arkhata.tmp runtime/37/arkhata.dll

.obj/arkhata.o:	src/arkhata.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/arkhata.o -c src/arkhata.c

runtime/22/lab2.dll:	.obj/lab2.o
	$(CC) $(DDFLAGS) -o lab2.tmp .obj/lab2.o
	@mkdir -p runtime/22
	@mv lab2.tmp runtime/22/lab2.dll

.obj/lab2.o:	src/lab2.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/lab2.o -c src/lab2.c

runtime/22/lab3.dll:	.obj/lab3.o
	$(CC) $(DDFLAGS) -o lab3.tmp .obj/lab3.o
	@mkdir -p runtime/22
	@mv lab3.tmp runtime/22/lab3.dll

.obj/lab3.o:	src/lab3.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/lab3.o -c src/lab3.c

runtime/22/lab4.dll:	.obj/lab4.o
	$(CC) $(DDFLAGS) -o lab4.tmp .obj/lab4.o
	@mkdir -p runtime/22
	@mv lab4.tmp runtime/22/lab4.dll

.obj/lab4.o:	src/lab4.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/lab4.o -c src/lab4.c

runtime/22/lab5.dll:	.obj/lab5.o
	$(CC) $(DDFLAGS) -o lab5.tmp .obj/lab5.o
	@mkdir -p runtime/22
	@mv lab5.tmp runtime/22/lab5.dll

.obj/lab5.o:	src/lab5.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/lab5.o -c src/lab5.c


runtime/3/arena.dll:	.obj/arena.o
	$(CC) $(DDFLAGS) -o arena.tmp .obj/arena.o
	@mkdir -p runtime/3
	@mv arena.tmp runtime/3/arena.dll

.obj/arena.o:	src/arena.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/arena.o -c src/arena.c

runtime/3/gatekeeper.dll:	.obj/gatekeeper.o
	$(CC) $(DDFLAGS) -o gatekeeper.tmp .obj/gatekeeper.o
	@mkdir -p runtime/3
	@mv gatekeeper.tmp runtime/3/gatekeeper.dll

.obj/gatekeeper.o:	src/gatekeeper.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/gatekeeper.o -c src/gatekeeper.c

runtime/3/military.dll:	.obj/military.o
	$(CC) $(DDFLAGS) -o military.tmp .obj/military.o
	@cp military.tmp military.tmp2
	@mkdir -p runtime/3
	@mv military.tmp runtime/3/military.dll
	@mkdir -p runtime/29
	@mv military.tmp2 runtime/29/military.dll

.obj/military.o:	src/military.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/military.o -c src/military.c

runtime/6/edemon.dll:	.obj/edemon.o
	$(CC) $(DDFLAGS) -o edemon.tmp .obj/edemon.o
	@mkdir -p runtime/6
	@mv edemon.tmp runtime/6/edemon.dll

.obj/edemon.o:	src/edemon.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/edemon.o -c src/edemon.c

runtime/5/sewers.dll:	.obj/sewers.o
	$(CC) $(DDFLAGS) -o sewers.tmp .obj/sewers.o
	@mkdir -p runtime/5
	@mv sewers.tmp runtime/5/sewers.dll

.obj/sewers.o:	src/sewers.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/sewers.o -c src/sewers.c

runtime/8/fdemon.dll:	.obj/fdemon.o
	$(CC) $(DDFLAGS) -o fdemon.tmp .obj/fdemon.o
	@mkdir -p runtime/8
	@mv fdemon.tmp runtime/8/fdemon.dll

.obj/fdemon.o:	src/fdemon.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/fdemon.o -c src/fdemon.c

runtime/10/ice.dll:	.obj/ice.o
	$(CC) $(DDFLAGS) -o ice.tmp .obj/ice.o
	@mkdir -p runtime/10
	@mv ice.tmp runtime/10/ice.dll

.obj/ice.o:	src/ice.c src/ice_shared.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/ice.o -c src/ice.c

runtime/11/palace.dll:	.obj/palace.o
	$(CC) $(DDFLAGS) -o palace.tmp .obj/palace.o
	@mkdir -p runtime/11
	@mv palace.tmp runtime/11/palace.dll

.obj/palace.o:	src/palace.c src/ice_shared.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/palace.o -c src/palace.c

runtime/13/dungeon.dll:	.obj/dungeon.o
	$(CC) $(DDFLAGS) -o dungeon.tmp .obj/dungeon.o
	@mkdir -p runtime/13
	@mv dungeon.tmp runtime/13/dungeon.dll

.obj/dungeon.o:	src/dungeon.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/dungeon.o -c src/dungeon.c

runtime/14/random.dll:	.obj/random.o
	$(CC) $(DDFLAGS) -o random.tmp .obj/random.o
	@mkdir -p runtime/14
	@mv random.tmp runtime/14/random.dll

.obj/random.o:	src/random.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/random.o -c src/random.c

runtime/15/swamp.dll:	.obj/swamp.o
	$(CC) $(DDFLAGS) -o swamp.tmp .obj/swamp.o
	@mkdir -p runtime/15
	@mv swamp.tmp runtime/15/swamp.dll

.obj/swamp.o:	src/swamp.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/swamp.o -c src/swamp.c

runtime/16/forest.dll:	.obj/forest.o
	$(CC) $(DDFLAGS) -o forest.tmp .obj/forest.o
	@mkdir -p runtime/16
	@mv forest.tmp runtime/16/forest.dll

.obj/forest.o:	src/forest.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/forest.o -c src/forest.c

runtime/17/two.dll:	.obj/two.o
	$(CC) $(DDFLAGS) -o two.tmp .obj/two.o
	@mkdir -p runtime/17
	@mv two.tmp runtime/17/two.dll

.obj/two.o:	src/two.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/two.o -c src/two.c

runtime/18/bones.dll:	.obj/bones.o
	$(CC) $(DDFLAGS) -o bones.tmp .obj/bones.o
	@mkdir -p runtime/18
	@mv bones.tmp runtime/18/bones.dll

.obj/bones.o:	src/bones.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/bones.o -c src/bones.c

runtime/19/nomad.dll:	.obj/nomad.o
	$(CC) $(DDFLAGS) -o nomad.tmp .obj/nomad.o
	@mkdir -p runtime/19
	@mv nomad.tmp runtime/19/nomad.dll

.obj/nomad.o:	src/nomad.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/nomad.o -c src/nomad.c

runtime/19/saltmine.dll:	.obj/saltmine.o
	$(CC) $(DDFLAGS) -o saltmine.tmp .obj/saltmine.o
	@mkdir -p runtime/19
	@mv saltmine.tmp runtime/19/saltmine.dll

.obj/saltmine.o:	src/saltmine.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/saltmine.o -c src/saltmine.c

runtime/26/staffer.dll:	.obj/staffer.o
	$(CC) $(DDFLAGS) -o staffer.tmp .obj/staffer.o
	@mkdir -p runtime/26
	@mv staffer.tmp runtime/26/staffer.dll

.obj/staffer.o:	src/staffer.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/staffer.o -c src/staffer.c

runtime/29/staffer2.dll:	.obj/staffer2.o
	$(CC) $(DDFLAGS) -o staffer2.tmp .obj/staffer2.o
	@mkdir -p runtime/29
	@mv staffer2.tmp runtime/29/staffer2.dll

.obj/staffer2.o:	src/staffer2.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/staffer2.o -c src/staffer2.c

runtime/28/staffer3.dll:	.obj/staffer3.o
	$(CC) $(DDFLAGS) -o staffer3.tmp .obj/staffer3.o
	@mkdir -p runtime/28
	@mv staffer3.tmp runtime/28/staffer3.dll

.obj/staffer3.o:	src/staffer3.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/staffer3.o -c src/staffer3.c

runtime/25/warped.dll:	.obj/warped.o
	$(CC) $(DDFLAGS) -o warped.tmp .obj/warped.o
	@mkdir -p runtime/25
	@mv warped.tmp runtime/25/warped.dll

.obj/warped.o:	src/warped.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/warped.o -c src/warped.c

runtime/20/lq.dll:	.obj/lq.o
	$(CC) $(DDFLAGS) -o lq.tmp .obj/lq.o
	@cp lq.tmp lq.tmpx
	@mkdir -p runtime/35
	@mv lq.tmpx runtime/35/lq.dll
	@mkdir -p runtime/20
	@mv lq.tmp runtime/20/lq.dll

.obj/lq.o:	src/lq.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/lq.o -c src/lq.c

runtime/generic/mine.dll:	.obj/mine.o
	$(CC) $(DDFLAGS) -o mine.tmp .obj/mine.o
	@mkdir -p runtime/generic
	@mv mine.tmp runtime/generic/mine.dll

.obj/mine.o:	src/mine.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/mine.o -c src/mine.c

runtime/22/lab1.dll:		.obj/lab1.o
	$(CC) $(DDFLAGS) -o lab1.tmp .obj/lab1.o
	@mkdir -p runtime/22
	@mv lab1.tmp runtime/22/lab1.dll

.obj/lab1.o:	src/lab1.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/lab1.o -c src/lab1.c

runtime/23/strategy.dll: 	.obj/strategy.o
	$(CC) $(DDFLAGS) -o strategy.tmp .obj/strategy.o
	@cp strategy.tmp strategy2.tmp
	@mkdir -p runtime/23
	@mv strategy.tmp runtime/23/strategy.dll
	@mkdir -p runtime/24
	@mv strategy2.tmp runtime/24/strategy.dll

.obj/strategy.o:	src/strategy.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/strategy.o -c src/strategy.c

runtime/33/tunnel.dll: 	.obj/tunnel.o
	$(CC) $(DDFLAGS) -o tunnel.tmp .obj/tunnel.o
	@mkdir -p runtime/33
	@mv tunnel.tmp runtime/33/tunnel.dll

.obj/tunnel.o:	src/tunnel.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/tunnel.o -c src/tunnel.c

runtime/31/warrmines.dll: .obj/warrmines.o
	$(CC) $(DDFLAGS) -o warrmines.tmp .obj/warrmines.o
	@mkdir -p runtime/31
	@mv warrmines.tmp runtime/31/warrmines.dll

.obj/warrmines.o:	src/warrmines.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/warrmines.o -c src/warrmines.c

runtime/32/missions.dll: .obj/missions.o
	$(CC) $(DDFLAGS) -o missions.tmp .obj/missions.o
	@mkdir -p runtime/32
	@mv missions.tmp runtime/32/missions.dll

.obj/missions.o:	src/missions.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h src/mission_ppd.h
	$(CC) $(DFLAGS) -o .obj/missions.o -c src/missions.c

runtime/34/teufel.dll: .obj/teufel.o
	$(CC) $(DDFLAGS) -o teufel.tmp .obj/teufel.o
	@mkdir -p runtime/34
	@mv teufel.tmp runtime/34/teufel.dll

.obj/teufel.o:	src/teufel.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h src/mission_ppd.h
	$(CC) $(DFLAGS) -o .obj/teufel.o -c src/teufel.c

runtime/36/caligar.dll: .obj/caligar.o
	$(CC) $(DDFLAGS) -o caligar.tmp .obj/caligar.o
	@mkdir -p runtime/36
	@mv caligar.tmp runtime/36/caligar.dll

.obj/caligar.o:	src/caligar.c src/server.h src/log.h src/notify.h src/do.h src/direction.h src/path.h src/error.h src/drdata.h src/see.h src/drvlib.h src/death.h src/effect.h src/tool.h src/store.h src/area1.h
	$(CC) $(DFLAGS) -o .obj/caligar.o -c src/caligar.c

update_server:	.obj/update_server.o
	$(CC) $(LDFLAGS) -lm -o update_server .obj/update_server.o

.obj/update_server.o:	src/update_server.c
	$(CC) $(CFLAGS) -o .obj/update_server.o -c src/update_server.c

zones/generic/weapons.itm:	create_weapons
	@./create_weapons >zones/generic/weapons.itm

create_weapons:	.obj/create_weapons.o
	$(CC) $(LDFLAGS) -o create_weapons .obj/create_weapons.o

.obj/create_weapons.o:	src/create_weapons.c
	$(CC) $(CFLAGS) -o .obj/create_weapons.o -c src/create_weapons.c

zones/generic/armor.itm:	create_armor
	@./create_armor >zones/generic/armor.itm

create_armor:		.obj/create_armor.o
	$(CC) $(LDFLAGS) -o create_armor .obj/create_armor.o

.obj/create_armor.o:	src/create_armor.c
	$(CC) $(CFLAGS) -o .obj/create_armor.o -c src/create_armor.c

chatserver:		.obj/chatserver.o
	$(CC) $(LDFLAGS) -o chatserver .obj/chatserver.o

.obj/chatserver.o:	src/chatserver.c
	$(CC) $(CFLAGS) -o .obj/chatserver.o -c src/chatserver.c

create_account:		src/create_account.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o create_account src/create_account.c -lmysqlclient

create_character:	src/create_character.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o create_character src/create_character.c -lmysqlclient


# ------- Helper -----

clean:
	-rm server .obj/*.o *~ zones/*/*~ runtime/*/* chatserver update_server create_weapons create_armor create_account create_character

