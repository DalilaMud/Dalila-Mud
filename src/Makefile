# Generated automatically from Makefile.in by configure.
# CircleMUD 3.0 Makefile.in - Makefile template used by 'configure'
#

# C compiler to use
CC = gcc

# Any special flags you want to pass to the compiler
MYFLAGS = -Wall 

#flags for profiling (see hacker.doc for more information)
PROFILE =

##############################################################################
# Do Not Modify Anything Below This Line (unless you know what you're doing) #
##############################################################################

CFLAGS = -g -O $(MYFLAGS) $(PROFILE)

OBJFILES = comm.o act.comm.o act.informative.o act.movement.o act.item.o \
    act.offensive.o act.other.o act.social.o act.wizard.o ban.o boards.o \
	castle.o class.o clan.o config.o constants.o db.o fight.o graph.o \
        handler.o act.create.o alias.o quest.o auction.o trains.o arena.o\
	house.o interpreter.o limits.o magic.o mail.o mobact.o modify.o \
	objsave.o olc.o shop.o spec_assign.o spec_procs.o spell_parser.o \
	spells.o utils.o weather.o redit.o oedit.o zedit.o medit.o sedit.o \
        dg_comm.o dg_db_scripts.o dg_event.o dg_handler.o dg_mobcmd.o \
        dg_objcmd.o dg_scripts.o dg_triggers.o dg_wldcmd.o dg_olc.o act.intermud.o \
        queue.o events.o regen.o wilderness.o wedit.o mestieri.o abilita.o go-patrol.o \
		clan2.o dg_misc.o eserciti.o

default: .accepted
	$(MAKE) ../bin/circle

.accepted:
	@./licheck more

#utils: .accepted
#	$(MAKE) ../bin/autowiz
#	$(MAKE) ../bin/delobjs
#	$(MAKE) ../bin/listrent
#	$(MAKE) ../bin/mudpasswd
#	$(MAKE) ../bin/play2to3
#	$(MAKE) ../bin/purgeplay
#	$(MAKE) ../bin/shopconv
#	$(MAKE) ../bin/showplay
#	$(MAKE) ../bin/sign
#	$(MAKE) ../bin/split
#	$(MAKE) ../bin/wld2html
#
all: .accepted
	$(MAKE) ../bin/circle
#	$(MAKE) utils

circle:
	$(MAKE) ../bin/circle
autowiz:
	$(MAKE) ../bin/autowiz
#delobjs:
#	$(MAKE) ../bin/delobjs
#listrent:
#	$(MAKE) ../bin/listrent
mudpasswd:
	$(MAKE) ../bin/mudpasswd
#play2to3:
#	$(MAKE) ../bin/play2to3
#purgeplay:
#	$(MAKE) ../bin/purgeplay
#shopconv:
#	$(MAKE) ../bin/shopconv
#showplay:
#	$(MAKE) ../bin/showplay
#sign:
#	$(MAKE) ../bin/sign
#split:
#	$(MAKE) ../bin/split
#wld2html:
#	$(MAKE) ../bin/wld2html


../bin/autowiz: util/autowiz.c conf.h sysdep.h structs.h utils.h db.h
	$(CC) $(CFLAGS) -o ../bin/autowiz util/autowiz.c
#../bin/delobjs: util/delobjs.c conf.h sysdep.h structs.h
#	$(CC) $(CFLAGS) -o ../bin/delobjs util/delobjs.c
#../bin/listrent: util/listrent.c conf.h sysdep.h structs.h
#	$(CC) $(CFLAGS) -o ../bin/listrent util/listrent.c
../bin/mudpasswd: util/mudpasswd.c conf.h sysdep.h structs.h utils.h
	$(CC) $(CFLAGS) -o ../bin/mudpasswd util/mudpasswd.c -lcrypt
#../bin/play2to3: util/play2to3.c
#	$(CC) $(CFLAGS) -o ../bin/play2to3 util/play2to3.c
#../bin/purgeplay: util/purgeplay.c conf.h sysdep.h structs.h utils.h
#	$(CC) $(CFLAGS) -o ../bin/purgeplay util/purgeplay.c
#../bin/shopconv: util/shopconv.c conf.h sysdep.h structs.h db.h utils.h shop.h
#	$(CC) $(CFLAGS) -o ../bin/shopconv util/shopconv.c
#../bin/showplay: util/showplay.c conf.h sysdep.h structs.h
#	$(CC) $(CFLAGS) -o ../bin/showplay util/showplay.c
#../bin/sign: util/sign.c conf.h sysdep.h
#	$(CC) $(CFLAGS) -o ../bin/sign util/sign.c
#../bin/split: util/split.c
#	$(CC) $(CFLAGS) -o ../bin/split util/split.c
#../bin/wld2html: util/wld2html.c
#	$(CC) $(CFLAGS) -o ../bin/wld2html util/wld2html.c

../bin/circle : $(OBJFILES)
	$(CC) -lm -o ../bin/circle $(PROFILE) $(OBJFILES) -lcrypt
clean:
	rm -f *.o

# Dependencies for the object files (automagically generated with
# gcc -MM)

act.comm.o: act.comm.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h \
  handler.h db.h screen.h clan.h dg_scripts.h clan2.h
	$(CC) -c $(CFLAGS) act.comm.c
act.create.o: act.create.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h \
  handler.h db.h screen.h clan.h
	$(CC) -c $(CFLAGS) act.create.c
act.informative.o: act.informative.c conf.h sysdep.h structs.h utils.h comm.h \
  interpreter.h handler.h db.h spells.h screen.h clan.h wilderness.h olc.h \
  dg_scripts.h eserciti.h
	$(CC) -c $(CFLAGS) act.informative.c
act.item.o: act.item.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h \
  handler.h db.h spells.h dg_scripts.h
	$(CC) -c $(CFLAGS) act.item.c
act.movement.o: act.movement.c conf.h sysdep.h structs.h utils.h comm.h \
  interpreter.h handler.h db.h spells.h house.h dg_scripts.h olc.h
	$(CC) -c $(CFLAGS) act.movement.c
act.offensive.o: act.offensive.c conf.h sysdep.h structs.h utils.h comm.h \
  interpreter.h handler.h db.h spells.h
	$(CC) -c $(CFLAGS) act.offensive.c
act.other.o: act.other.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h \
  handler.h db.h spells.h screen.h house.h dg_scripts.h
	$(CC) -c $(CFLAGS) act.other.c
act.social.o: act.social.c conf.h sysdep.h structs.h utils.h comm.h \
  interpreter.h handler.h db.h spells.h
	$(CC) -c $(CFLAGS) act.social.c
act.wizard.o: act.wizard.c conf.h sysdep.h structs.h utils.h comm.h \
  interpreter.h handler.h db.h spells.h house.h screen.h dg_scripts.h arena.h clan.h shop.h
	$(CC) -c $(CFLAGS) act.wizard.c
ban.o: ban.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h handler.h db.h
	$(CC) -c $(CFLAGS) ban.c
boards.o: boards.c conf.h sysdep.h structs.h utils.h comm.h db.h boards.h \
  interpreter.h handler.h
	$(CC) -c $(CFLAGS) boards.c
castle.o: castle.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h \
  handler.h db.h spells.h
	$(CC) -c $(CFLAGS) castle.c
clan.o: clan.c conf.h sysdep.h structs.h utils.h comm.h spells.h handler.h \
  db.h clan.h shop.h
	$(CC) -c $(CFLAGS) clan.c
class.o: class.c conf.h sysdep.h structs.h db.h utils.h spells.h interpreter.h comm.h
	$(CC) -c $(CFLAGS) class.c
comm.o: comm.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h handler.h \
  db.h house.h dg_scripts.h arena.h clan.h
	$(CC) -c $(CFLAGS) comm.c
config.o: config.c conf.h sysdep.h structs.h
	$(CC) -c $(CFLAGS) config.c
constants.o: constants.c conf.h sysdep.h structs.h
	$(CC) -c $(CFLAGS) constants.c
auction.o: auction.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h \
  handler.h db.h screen.h auction.h
	$(CC) -c $(CFLAGS) auction.c
db.o: db.c conf.h sysdep.h structs.h utils.h db.h comm.h handler.h \
 spells.h mail.h auction.h interpreter.h house.h dg_scripts.h \
 wilderness.h shop.h eserciti.h
	$(CC) -c $(CFLAGS) db.c
fight.o: fight.c conf.h sysdep.h structs.h utils.h comm.h handler.h interpreter.h \
  db.h spells.h screen.h dg_scripts.h arena.h
	$(CC) -c $(CFLAGS) fight.c
graph.o: graph.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h handler.h \
  db.h spells.h
	$(CC) -c $(CFLAGS) graph.c
arena.o: arena.c conf.h sysdep.h structs.h utils.h comm.h screen.h spells.h \
  handler.h interpreter.h db.h arena.h
	$(CC) -c $(CFLAGS) arena.c
handler.o: handler.c conf.h sysdep.h structs.h utils.h comm.h db.h handler.h \
  interpreter.h spells.h dg_scripts.h
	$(CC) -c $(CFLAGS) handler.c
house.o: house.c conf.h sysdep.h structs.h comm.h handler.h db.h interpreter.h \
  utils.h house.h
	$(CC) -c $(CFLAGS) house.c
interpreter.o: interpreter.c conf.h sysdep.h structs.h comm.h interpreter.h db.h \
  utils.h spells.h handler.h mail.h screen.h dg_scripts.h
	$(CC) -c $(CFLAGS) interpreter.c
limits.o: limits.c conf.h sysdep.h structs.h utils.h spells.h comm.h db.h \
  handler.h dg_scripts.h
	$(CC) -c $(CFLAGS) limits.c
trains.o: trains.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h handler.h db.h
	$(CC) -c $(CFLAGS) trains.c
magic.o: magic.c conf.h sysdep.h structs.h utils.h comm.h spells.h handler.h db.h
	$(CC) -c $(CFLAGS) magic.c
mail.o: mail.c conf.h sysdep.h structs.h utils.h comm.h db.h interpreter.h \
  handler.h mail.h
	$(CC) -c $(CFLAGS) mail.c
mobact.o: mobact.c conf.h sysdep.h structs.h utils.h db.h comm.h interpreter.h \
  handler.h spells.h
	$(CC) -c $(CFLAGS) mobact.c
modify.o: modify.c conf.h sysdep.h structs.h utils.h interpreter.h handler.h db.h \
  comm.h spells.h mail.h boards.h
	$(CC) -c $(CFLAGS) modify.c
objsave.o: objsave.c conf.h sysdep.h structs.h comm.h handler.h db.h \
  interpreter.h utils.h spells.h
	$(CC) -c $(CFLAGS) objsave.c
olc.o: olc.c conf.h sysdep.h structs.h utils.h comm.h interpreter.h handler.h db.h \
  olc.h screen.h shop.h
	$(CC) -c $(CFLAGS) olc.c
random.o: random.c
	$(CC) -c $(CFLAGS) random.c
shop.o: shop.c conf.h sysdep.h structs.h comm.h handler.h db.h interpreter.h \
  utils.h shop.h olc.h clan.h
	$(CC) -c $(CFLAGS) shop.c
spec_assign.o: spec_assign.c conf.h sysdep.h structs.h db.h interpreter.h \
  utils.h
	$(CC) -c $(CFLAGS) spec_assign.c
spec_procs.o: spec_procs.c conf.h sysdep.h structs.h utils.h comm.h \
  interpreter.h handler.h db.h spells.h screen.h
	$(CC) -c $(CFLAGS) spec_procs.c
spell_parser.o: spell_parser.c conf.h sysdep.h structs.h utils.h interpreter.h \
  spells.h handler.h comm.h db.h dg_scripts.h
	$(CC) -c $(CFLAGS) spell_parser.c
spells.o: spells.c conf.h sysdep.h structs.h utils.h comm.h spells.h handler.h \
  db.h interpreter.h dg_scripts.h
	$(CC) -c $(CFLAGS) spells.c
utils.o: utils.c conf.h sysdep.h structs.h utils.h comm.h screen.h spells.h \
  handler.h
	$(CC) -c $(CFLAGS) utils.c
weather.o: weather.c conf.h sysdep.h structs.h utils.h comm.h handler.h \
  interpreter.h db.h
	$(CC) -c $(CFLAGS) weather.c
act.intermud.o: act.intermud.c sysdep.h structs.h utils.h comm.h db.h interpreter.h handler.h intermud.hh screen.h
	$(CC) -c $(CFLAGS) act.intermud.c
redit.o: redit.c conf.h sysdep.h structs.h utils.h comm.h handler.h interpreter.h boards.h \
  db.c olc.h dg_olc.h
	$(CC) -c $(CFLAGS) redit.c
oedit.o: oedit.c conf.h sysdep.h structs.h utils.h comm.h boards.h spells.h \
  db.h olc.h shop.h dg_olc.h mestieri.h
	$(CC) -c $(CFLAGS) oedit.c
zedit.o: zedit.c conf.h sysdep.h structs.h utils.h comm.h db.h olc.h wilderness.h
	$(CC) -c $(CFLAGS) zedit.c
medit.o: medit.c conf.h sysdep.h structs.h utils.h comm.h spells.h shop.h \
  db.h olc.h handler.h dg_olc.h
	$(CC) -c $(CFLAGS) medit.c
sedit.o: sedit.c conf.h sysdep.h structs.h utils.h comm.h db.h olc.h shop.h clan.h handler.h
	$(CC) -c $(CFLAGS) sedit.c
alias.o : alias.c conf.h sysdep.h structs.h utils.h interpreter.h
	$(CC) -c $(CFLAGS) alias.c
quest.o : quest.c conf.h sysdep.h structs.h utils.h interpreter.h \
  handler.h comm.h db.h
	$(CC) -c $(CFLAGS) quest.c
dg_comm.o: dg_comm.c conf.h sysdep.h structs.h dg_scripts.h utils.h comm.h \
  handler.h db.h
	$(CC) -c $(CFLAGS) dg_comm.c
dg_db_scripts.o: dg_db_scripts.c conf.h sysdep.h structs.h dg_scripts.h \
  utils.h comm.h handler.h db.h dg_event.h
	$(CC) -c $(CFLAGS) dg_db_scripts.c
dg_event.o: dg_event.c conf.h sysdep.h dg_event.h utils.h structs.h
	$(CC) -c $(CFLAGS) dg_event.c
dg_handler.o: dg_handler.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h handler.h db.h dg_event.h
	$(CC) -c $(CFLAGS) dg_handler.c
dg_mobcmd.o: dg_mobcmd.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h handler.h db.h interpreter.h comm.h spells.h
	$(CC) -c $(CFLAGS) dg_mobcmd.c
dg_objcmd.o: dg_objcmd.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h handler.h db.h interpreter.h comm.h
	$(CC) -c $(CFLAGS) dg_objcmd.c
dg_scripts.o: dg_scripts.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h handler.h db.h comm.h screen.h
	$(CC) -c $(CFLAGS) dg_scripts.c
dg_triggers.o: dg_triggers.c conf.h sysdep.h structs.h dg_scripts.h \
  utils.h comm.h handler.h db.h interpreter.h
	$(CC) -c $(CFLAGS) dg_triggers.c
dg_wldcmd.o: dg_wldcmd.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h handler.h db.h comm.h interpreter.h
	$(CC) -c $(CFLAGS) dg_wldcmd.c
dg_olc.o: dg_olc.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h db.h olc.h dg_olc.h dg_event.h
	$(CC) -c $(CFLAGS) dg_olc.c
events.o: events.c conf.h sysdep.h structs.h utils.h queue.h events.h
	$(CC) -c $(CFLAGS) events.c
queue.o: queue.c conf.h sysdep.h structs.h utils.h queue.h
	$(CC) -c $(CFLAGS) queue.c
regen.o: regen.c conf.h sysdep.h structs.h utils.h spells.h handler.h events.h
	$(CC) -c $(CFLAGS) regen.c
wilderness.o: wilderness.c conf.h sysdep.h structs.h utils.h \
 wilderness.h db.h
	$(CC) -c $(CFLAGS) wilderness.c
wedit.o: wedit.c conf.h sysdep.h structs.h comm.h utils.h db.h olc.h \
 wilderness.h
	$(CC) -c $(CFLAGS) wedit.c
mestieri.o: mestieri.c conf.h sysdep.h structs.h comm.h utils.h handler.h spells.h \
 mestieri.h interpreter.h db.h wilderness.h
	$(CC) -c $(CFLAGS) mestieri.c
abilita.o: abilita.c conf.h sysdep.h structs.h comm.h utils.h handler.h spells.h \
interpreter.h db.h
	$(CC) -c $(CFLAGS) abilita.c
go-patrol.o: go-patrol.c conf.h sysdep.h structs.h comm.h utils.h handler.h clan.h \
 interpreter.h db.h wilderness.h
	$(CC) -c $(CFLAGS) go-patrol.c
clan2.o: clan2.c conf.h sysdep.h structs.h utils.h comm.h db.h \
 interpreter.h spells.h handler.h clan2.h shop.h
	$(CC) -c $(CFLAGS) clan2.c	
dg_misc.o: dg_misc.c conf.h sysdep.h structs.h dg_scripts.h utils.h \
  comm.h handler.h db.h comm.h screen.h
	$(CC) -c $(CFLAGS) dg_misc.c
eserciti.o: eserciti.c conf.h sysdep.h structs.h utils.h comm.h db.h \
  eserciti.h clan.h
	$(CC) -c $(CFLAGS) eserciti.c
