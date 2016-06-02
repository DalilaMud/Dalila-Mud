/* ************************************************************************
*   File: comm.c        35                              Part of CircleMUD *
*  Usage: Communication, socket handling, main(), central game loop       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


#define __COMM_C__

#include "conf.h"
#include "sysdep.h"


#ifdef CIRCLE_WINDOWS		/* Includes for Win32 */

#include <direct.h>
#include <mmsystem.h>

#else				/* Includes for UNIX */

#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#endif



#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "house.h"
//#include "olc.h"
#include "screen.h"
#include "dg_scripts.h"
#include "arena.h"
//#include "intermud.hh"
#include "events.h"
#include "clan.h"
#include "mestieri.h"
#include "spells.h"

#ifdef HAVE_ARPA_TELNET_H

#include <arpa/telnet.h>

#else

#include "telnet.h"

#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

/* externs */
extern int restrict;
extern int mini_mud;
extern int no_rent_check;
extern FILE *player_fl;
extern FILE *mobs_fl;
extern int DFLT_PORT;
extern char *DFLT_DIR;
extern int MAX_PLAYERS;
extern int MAX_DESCRIPTORS_AVAILABLE;

extern struct room_data *world;	/* In db.c */
extern int top_of_world;	/* In db.c */
extern struct time_info_data time_info;		/* In db.c */
extern char help[];
//extern const char *save_info_msg[];	/* In olc.c */
extern int in_arena;
struct char_data *last_ch;
char last_command[MAX_STRING_LENGTH];
extern void save_shop();
extern void reset_risorse(int what);
extern int salva_eserciti();
extern void movimento_eserciti();

/* local globals */

void mud_sound(int stype);
struct descriptor_data *descriptor_list = NULL;		/* master desc list */
struct txt_block *bufpool = 0;	/* pool of large output buffers */
int buf_largecount = 0;		/* # of large buffers which exist */
int buf_overflows = 0;		/* # of overflows of output */
int buf_switches = 0;		/* # of switches from small to large buf */
int circle_shutdown = 0;	/* clean shutdown */
int circle_reboot = 0;		/* reboot the game after a shutdown */
int no_specials = 0;		/* Suppress ass. of special routines */
int max_players = 0;		/* max descriptors available */
int tics = 0;			/* for extern checkpointing */
int scheck = 0;			/* for syntax checking mode */
int dg_act_check;               /* toggle for act_trigger */
unsigned long dg_global_pulse = 0; /* number of pulses since game start */
extern int nameserver_is_slow;	/* see config.c */
extern int auto_save;		/* see config.c */
extern int autosave_time;	/* see config.c */
extern void approv_shop();
struct timeval null_time;	/* zero-valued time structure */
unsigned long pulse = 0;	/* number of pulses since game started */
int intermud_desc;
int connected_to_intermud;
extern struct clan_rec clan[MAX_CLANS];
extern int num_clan;
extern int num_of_clans;
void reboot_save_players(void);

const char *frasi_move_status[] =
{
	"ERRORE GET_MOVE_STATUS",
	"collassato",
	"esausto",
	"stanco",
	"ansimante",
	"respiro accelerato",
	"fiato regolare",
	"fresco",
	"indemoniato"
};

const char *frasi_lunghe_move_status[] =
{
	"ERRORE GET_MOVE_STATUS",
	"e' collassato",
	"e' esausto",
	"e' stanco",
	"e' ansimante",
	"ha il respiro accelerato",
	"ha il fiato regolare",
	"e' fresco",
	"e' indemoniato"
};

/* functions in this file */
int get_from_q(struct txt_q *queue, char *dest, int *aliased);
void init_game(int port);
void signal_setup(void);
void game_loop(int mother_desc);
int init_socket(int port);
int new_descriptor(int s);
int get_max_players(void);
int process_output(struct descriptor_data *t);
int process_input(struct descriptor_data *t);
void close_socket(struct descriptor_data *d);
struct timeval timediff(struct timeval a, struct timeval b);
struct timeval timeadd(struct timeval a, struct timeval b);
void flush_queues(struct descriptor_data *d);
void nonblock(socket_t s);
int perform_subst(struct descriptor_data *t, char *orig, char *subst);
int perform_alias(struct descriptor_data *d, char *orig);
void record_usage(void);
void make_prompt(struct descriptor_data *point);
void check_idle_passwords(void);
void train_upd(void); /* in trains.c */
void heartbeat(unsigned long pulse);
int count_chars(const char *txt, char character);  /*color*/
char *parse_color(const char *txt, struct descriptor_data *t); /*color*/
char *parse_sound(const char *txt); /* MSP sound remover */
void tick_grenade(void);
void init_intermud_socket(void);
void incoming_intermud_message(int intermud_desc);
int update_max_usage(void);
int get_move_status(struct char_data * ch);
void change_client(struct char_data *ch); //Orione

/* extern fcnts */
void boot_db(void);
void boot_world(void);
void zone_update(void);
void affect_update(void);	/* In spells.c */
void point_update(void);	/* In limits.c */
void damage_update(void);   /* ROOM_DAMAGE */
void ferite_update(void);   /* Ferite*/
void ferite_shaarr_update(void);
void check_raffreddore(void);
void check_peste(void);
void check_malattia_morfeo(void);
void check_male_antichi(void);
void chill_burn_update(void);
void tempeste_update(void);
void gain_fiato(void);
void mobile_activity(void);
void string_add(struct descriptor_data *d, char *str);
void perform_violence(void);
void perform_concentrato(void);
void show_string(struct descriptor_data *d, char *input);
int isbanned(char *hostname);
void weather_and_time(int mode);
void start_arena();
void beware_lightning(void);
void redit_save_to_disk(int zone_num);
void oedit_save_to_disk(int zone_num);
void medit_save_to_disk(int zone_num);
void sedit_save_to_disk(int zone_num);
void zedit_save_to_disk(int zone_num);
void update_diplo_timers();
void shop_crash_save();
void selvaggina_activity(void);
void go_patrol_activity(int pulse);
void Save_mob(void);
void save_time();
void stipendio_mob(void);
void check_catarsi(void);
void perform_distorsione(void);
void costo_fly(void);



/* *********************************************************************
*  main game loop and related stuff                                    *
********************************************************************* */

/* Windows doesn't have gettimeofday, so we'll simulate it. */
#ifdef CIRCLE_WINDOWS

void gettimeofday(struct timeval *t, struct timezone *dummy)
{
	DWORD millisec = GetTickCount();
	
	t->tv_sec = (int) (millisec / 1000);
	t->tv_usec = (millisec % 1000) * 1000;
}

#endif


int main(int argc, char **argv)
{
	int port;
	char buf[512];
	int pos = 1;
	char *dir;
	
	port = DFLT_PORT;
	dir = DFLT_DIR;
	
	while ((pos < argc) && (*(argv[pos]) == '-')) {
		switch (*(argv[pos] + 1)) {
			case 'd':
				if (*(argv[pos] + 2))
					dir = argv[pos] + 2;
				else if (++pos < argc)
					dir = argv[pos];
				else {
					log("Directory arg expected after option -d.");
					exit(1);
				}
				break;
			case 'm':
				mini_mud = 1;
				no_rent_check = 1;
				log("Running in minimized mode & with no rent check.");
				break;
			case 'c':
				scheck = 1;
				log("Syntax check mode enabled.");
				break;
			case 'q':
				no_rent_check = 1;
				log("Quick boot mode -- rent check supressed.");
				break;
			case 'r':
				restrict = 1;
				log("Restricting game -- no new players allowed.");
				break;
			case 's':
				no_specials = 1;
				log("Suppressing assignment of special routines.");
				break;
			default:
				sprintf(buf, "SYSERR: Unknown option -%c in argument string.", *(argv[pos] + 1));
				log(buf);
				break;
		}
		pos++;
	}
	
	if (pos < argc) {
		if (!isdigit(*argv[pos])) {
			fprintf(stderr, "Usage: %s [-c] [-m] [-q] [-r] [-s] [-d pathname] [port #]\n", argv[0]);
			exit(1);
		} 
		else if ((port = atoi(argv[pos])) <= 1024) {
			fprintf(stderr, "Illegal port number.\n");
			exit(1);
		}
	}

#ifdef CIRCLE_WINDOWS
	if (_chdir(dir) < 0) {
#else
	if (chdir(dir) < 0) {
#endif
		perror("Fatal error changing to data directory");
		exit(1);
	}

	sprintf(buf, "Using %s as data directory.", dir);
	log(buf);
		
	if (scheck) {
		boot_world();
		log("Done.");
		exit(0);
	} 
	else {
		sprintf(buf, "Running game on port %d.", port);
		log(buf);
		init_game(port);
	}
		
	return 0;
}
	
	
	
/* Init sockets, run game, and cleanup sockets */
void init_game(int port)
{
	int mother_desc;
	extern struct zone_data *zone_table;
	
	srandom(time(0));
	
	log("Finding player limit.");
	max_players = get_max_players();
	
	log("Opening mother connection.");
	mother_desc = init_socket(port);
	
  //log ("Opening intermud connection.");
  //init_intermud_socket();

	event_init();
	
	boot_db();
	
#ifndef CIRCLE_WINDOWS
	log("Signal trapping.");
	signal_setup();
#endif
	
	log("Entering game loop.");	
	game_loop(mother_desc);
	
	sprintf(buf,"(CRASH) : REBOOT!");
	mudlog(buf, CMP, LVL_IMMORT, TRUE);
	
	log("Start Game Save.");
	log("Saving players data.");
	reboot_save_players();
	log("Crash-saving keepers.");
	shop_crash_save();
	log("Crash-saving players.");
	Reboot_save_all();
	log("Crash-saving houses.");
	House_save_all();
	log("Saving Mobs");
	Save_mob();
	log("Closing all sockets.");
	while (descriptor_list)
		close_socket(descriptor_list);
	fclose(player_fl);
	
	CLOSE_SOCKET(mother_desc);
	//CLOSE_SOCKET(intermud_desc);
/* Rimozione OLC	
	if (circle_reboot != 2 && olc_save_list) { // Don't save zones.
		struct olc_save_info *entry, *next_entry;
		
		for (entry = olc_save_list; entry; entry = next_entry) {
			next_entry = entry->next;
			entry->zone = real_zone(entry->zone * 100); // CARLO - entry->zone deve essere rnum non vnum
			if (entry->type < 0 || entry->type > 4) {
				sprintf(buf, "OLC: Illegal save type %d!", entry->type);
				log(buf);
			}
			else if (entry->zone < 0) {
				sprintf(buf, "OLC: Illegal save zone %d!", entry->zone);
				log(buf);
			}
			else {
				sprintf(buf, "OLC: Reboot saving %s for zone %d.",
					save_info_msg[(int)entry->type], zone_table[entry->zone].number);
				log(buf);
				switch (entry->type)
				{
					case OLC_SAVE_ROOM: redit_save_to_disk(entry->zone); break;
					case OLC_SAVE_OBJ:  oedit_save_to_disk(entry->zone); break;
					case OLC_SAVE_MOB:  medit_save_to_disk(entry->zone); break;
					case OLC_SAVE_ZONE: zedit_save_to_disk(entry->zone); break;
					case OLC_SAVE_SHOP: sedit_save_to_disk(entry->zone); break;
					default:      log("Unexpected olc_save_list->type"); break;
				}
			}
		}
	}
*/	
	if (circle_reboot) {
		log("Rebooting.");
		exit(52);			/* what's so great about HHGTTG, anyhow? */
	}
	log("Normal termination of game.");
}



/*
 * init_socket sets up the mother descriptor - creates the socket, sets
 * its options up, binds it, and listens.
 */
int init_socket(int port)
{
	int s, opt;
	struct sockaddr_in sa;
	
  /*
   * Should the first argument to socket() be AF_INET or PF_INET?  I don't
   * know, take your pick.  PF_INET seems to be more widely adopted, and
   * Comer (_Internetworking with TCP/IP_) even makes a point to say that
   * people erroneously use AF_INET with socket() when they should be using
   * PF_INET.  However, the man pages of some systems indicate that AF_INET
   * is correct; some such as ConvexOS even say that you can use either one.
   * All implementations I've seen define AF_INET and PF_INET to be the same
   * number anyway, so ths point is (hopefully) moot.
   */
	
#ifdef CIRCLE_WINDOWS
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		
		wVersionRequested = MAKEWORD(1, 1);
		
		if (WSAStartup(wVersionRequested, &wsaData) != 0) {
			log("WinSock not available!\n");
			exit(1);
		}
		if ((wsaData.iMaxSockets - 4) < max_players) {
			max_players = wsaData.iMaxSockets - 4;
		}
		sprintf(buf, "Max players set to %d", max_players);
		log(buf);
		
		if ((s = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			fprintf(stderr, "Error opening network connection: Winsock err #%d\n", WSAGetLastError());
			exit(1);
		}
	}
#else
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error creating socket");
		exit(1);
	}
#endif				/* CIRCLE_WINDOWS */
	
#if defined(SO_SNDBUF)
	opt = LARGE_BUFSIZE + GARBAGE_SPACE;
	if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt SNDBUF");
		exit(1);
	}
#endif
	
#if defined(SO_REUSEADDR)
	opt = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt REUSEADDR");
		exit(1);
	}
#endif
	
#if defined(SO_LINGER)
	{
		struct linger ld;
		
		ld.l_onoff = 0;
		ld.l_linger = 0;
		if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0) {
			perror("setsockopt LINGER");
			exit(1);
		}
	}
#endif
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("bind");
		CLOSE_SOCKET(s);
		exit(1);
	}
	nonblock(s);
	listen(s, 5);
	return s;
}


int get_max_players(void)
{
#if defined(CIRCLE_OS2) || defined(CIRCLE_WINDOWS)
	return MAX_PLAYERS;
#else
	
	int max_descs = 0;
	char *method;
	
/*
 * First, we'll try using getrlimit/setrlimit.  This will probably work
 * on most systems.
 */
#if defined (RLIMIT_NOFILE) || defined (RLIMIT_OFILE)
#if !defined(RLIMIT_NOFILE)
#define RLIMIT_NOFILE RLIMIT_OFILE
#endif
	{
		struct rlimit limit;
		
    /* find the limit of file descs */
		method = "rlimit";
		if (getrlimit(RLIMIT_NOFILE, &limit) < 0) {
			perror("calling getrlimit");
			exit(1);
		}
    /* set the current to the maximum */
		limit.rlim_cur = limit.rlim_max;
		if (setrlimit(RLIMIT_NOFILE, &limit) < 0) {
			perror("calling setrlimit");
			exit(1);
		}
#ifdef RLIM_INFINITY
		if (limit.rlim_max == RLIM_INFINITY)
			max_descs = MAX_PLAYERS + NUM_RESERVED_DESCS;
		else
			max_descs = MIN(MAX_PLAYERS + NUM_RESERVED_DESCS, limit.rlim_max);
#else
		max_descs = MIN(MAX_PLAYERS + NUM_RESERVED_DESCS, limit.rlim_max);
#endif
	}
	
#elif defined (OPEN_MAX) || defined(FOPEN_MAX)
#if !defined(OPEN_MAX)
#define OPEN_MAX FOPEN_MAX
#endif
	method = "OPEN_MAX";
	max_descs = OPEN_MAX;		/* Uh oh.. rlimit didn't work, but we have
				 * OPEN_MAX */
#elif defined (POSIX)
  /*
   * Okay, you don't have getrlimit() and you don't have OPEN_MAX.  Time to
   * use the POSIX sysconf() function.  (See Stevens' _Advanced Programming
   * in the UNIX Environment_).
   */
	method = "POSIX sysconf";
	errno = 0;
	if ((max_descs = sysconf(_SC_OPEN_MAX)) < 0) {
		if (errno == 0)
			max_descs = MAX_PLAYERS + NUM_RESERVED_DESCS;
		else {
			perror("Error calling sysconf");
			exit(1);
		}
	}
#else
  /* if everything has failed, we'll just take a guess */
	max_descs = MAX_PLAYERS + NUM_RESERVED_DESCS;
#endif
	
  /* now calculate max _players_ based on max descs */
	max_descs = MIN(MAX_PLAYERS, max_descs - NUM_RESERVED_DESCS);
	
	if (max_descs <= 0) {
		sprintf(buf, "Non-positive max player limit!  (Set at %d using %s).",
			max_descs, method);
		log(buf);
		exit(1);
	}
	sprintf(buf, "Setting player limit to %d using %s.", max_descs, method);
	log(buf);
	return max_descs;
#endif				/* WINDOWS or OS2 */
}



/*
 * game_loop contains the main loop which drives the entire MUD.  It
 * cycles once every 0.10 seconds and is responsible for accepting new
 * new connections, polling existing connections for input, dequeueing
 * output and sending it out to players, and calling "heartbeat" functions
 * such as mobile_activity().
 */
void game_loop(int mother_desc)
{
	fd_set input_set, output_set, exc_set, null_set;
	struct timeval last_time, before_sleep, opt_time, process_time, now, timeout;
	char comm[MAX_INPUT_LENGTH];
	struct descriptor_data *d, *next_d;
	int missed_pulses, maxdesc, aliased;
	
  /* initialize various time values */
	null_time.tv_sec = 0;
	null_time.tv_usec = 0;
	opt_time.tv_usec = OPT_USEC;
	opt_time.tv_sec = 0;
	FD_ZERO(&null_set);
	
	gettimeofday(&last_time, (struct timezone *) 0);
	
  /* The Main Loop.  The Big Cheese.  The Top Dog.  The Head Honcho.  The.. */
	while (!circle_shutdown) {
		
    /* Sleep if we don't have any connections */
		if (descriptor_list == NULL) {
			log("No connections.  Going to sleep.");
			FD_ZERO(&input_set);
			FD_SET(mother_desc, &input_set);
			//if (connected_to_intermud == 1)
			//	FD_SET(intermud_desc, &input_set);
			if (select(mother_desc + 1, &input_set, (fd_set *) 0, (fd_set *) 0, NULL) < 0) {
				if (errno == EINTR)
					log("Waking up to process signal.");
				else
					perror("Select coma");
			} 
			else
				log("New connection.  Waking up.");
			gettimeofday(&last_time, (struct timezone *) 0);
		}
    /* Set up the input, output, and exception sets for select(). */
		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);
		FD_SET(mother_desc, &input_set);
		//if (connected_to_intermud == 1) {
		//	FD_SET(intermud_desc, &input_set);
		//	maxdesc = intermud_desc;
		//} else
			maxdesc = mother_desc;

		for (d = descriptor_list; d; d = d->next) {
#ifndef CIRCLE_WINDOWS
			if (d->descriptor > maxdesc)
				maxdesc = d->descriptor;
#endif
			FD_SET(d->descriptor, &input_set);
			FD_SET(d->descriptor, &output_set);
			FD_SET(d->descriptor, &exc_set);
		}
		
    /*
     * At this point, we have completed all input, output and heartbeat
     * activity from the previous iteration, so we have to put ourselves
     * to sleep until the next 0.1 second tick.  The first step is to
     * calculate how long we took processing the previous iteration.
     */
		
		gettimeofday(&before_sleep, (struct timezone *) 0); /* current time */
		process_time = timediff(before_sleep, last_time);
		
    /*
     * If we were asleep for more than one pass, count missed pulses and sleep
     * until we're resynchronized with the next upcoming pulse.
     */
		if (process_time.tv_sec == 0 && process_time.tv_usec < OPT_USEC) {
			missed_pulses = 0;
		} 
		else {
			missed_pulses = process_time.tv_sec * PASSES_PER_SEC;
			missed_pulses += process_time.tv_usec / OPT_USEC;
			process_time.tv_sec = 0;
			process_time.tv_usec = process_time.tv_usec % OPT_USEC;
		}
		
    /* Calculate the time we should wake up */
		last_time = timeadd(before_sleep, timediff(opt_time, process_time));
		
    /* Now keep sleeping until that time has come */
		gettimeofday(&now, (struct timezone *) 0);
		timeout = timediff(last_time, now);
		
    /* go to sleep */
		do {
#ifdef CIRCLE_WINDOWS
			Sleep(timeout.tv_sec * 1000 + timeout.tv_usec / 1000);
#else
			if (select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0) {
				if (errno != EINTR) {
					perror("Select sleep");
					exit(1);
				}
			}
#endif /* CIRCLE_WINDOWS */
			gettimeofday(&now, (struct timezone *) 0);
			timeout = timediff(last_time, now);
		} 
		while (timeout.tv_usec || timeout.tv_sec);
		
    /* poll (without blocking) for new input, output, and exceptions */
		if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time) < 0) {
			perror("Select poll");
			return;
		}
    /* If there are new connections waiting, accept them. */
		if (FD_ISSET(mother_desc, &input_set))
			new_descriptor(mother_desc);
		
		/*if (FD_ISSET(intermud_desc, &input_set) && connected_to_intermud == 1)
			incoming_intermud_message(intermud_desc);*/
		
    /* kick out the freaky folks in the exception set */
		for (d = descriptor_list; d; d = next_d) {
			next_d = d->next;
			if (FD_ISSET(d->descriptor, &exc_set)) {
				FD_CLR(d->descriptor, &input_set);
				FD_CLR(d->descriptor, &output_set);
				close_socket(d);
			}
		}
		
    /* process descriptors with input pending */
		for (d = descriptor_list; d; d = next_d) {
			next_d = d->next;
			if (FD_ISSET(d->descriptor, &input_set))
				if (process_input(d) < 0)
					close_socket(d);
		}
		
    /* process commands we just read from process_input */
		for (d = descriptor_list; d; d = next_d) {
			next_d = d->next;
			
			if ((--(d->wait) <= 0) && get_from_q(&d->input, comm, &aliased)) {
				if (d->character) {
	  /* reset the idle timer & pull char back from void if necessary */
					d->character->char_specials.timer = 0;
					if (!d->connected && GET_WAS_IN(d->character) != NOWHERE) {
						if (d->character->in_room != NOWHERE)
							char_from_room(d->character);
						char_to_room(d->character, GET_WAS_IN(d->character));
						GET_WAS_IN(d->character) = NOWHERE;
						act("$n has returned.", TRUE, d->character, 0, 0, TO_ROOM);
					}
				}
				d->wait = 1;
				d->prompt_mode = 1;
				
	 /* reversed these top 2 if checks so that you can use the page_string */
	 /* function in the editor */
				if (d->showstr_count)	/* reading something w/ pager     */
					show_string(d, comm);
				else if (d->str)		/* writing boards, mail, etc.     */
					string_add(d, comm);
				else if (d->connected != CON_PLAYING)	/* in menus, etc. */
					nanny(d, comm);
				else {			/* else: we're playing normally */
					if (aliased)		/* to prevent recursive aliases */
						d->prompt_mode = 0;
					else {
						if (perform_alias(d, comm))		/* run it through aliasing system */
							get_from_q(&d->input, comm, &aliased);
					}
					command_interpreter(d->character, comm);	/* send it to interpreter */
				}
			}
		}
		
    /* send queued output out to the operating system (ultimately to user) */
		for (d = descriptor_list; d; d = next_d) {
			next_d = d->next;
			
			if (d->character) {
			  if ((PRF_FLAGGED(d->character, PRF_AFK) || PLR_FLAGGED(d->character,PLR_CATARSI)) && (d->prompt_mode == 3)) {
			    d->bufspace = SMALL_BUFSIZE - 1;
          d->bufptr = 0;
          *(d->output) = '\0';
          continue;
        } 
      }
			
			if (FD_ISSET(d->descriptor, &output_set) && *(d->output)){
				if (process_output(d) < 0)
					close_socket(d);
				else
					d->prompt_mode = 1;
			}
			
			if (d->character) {
			  if ((PRF_FLAGGED(d->character, PRF_AFK) || PLR_FLAGGED(d->character,PLR_CATARSI)))
          d->prompt_mode = 3; 
      }
		}
		
    /* kick out folks in the CON_CLOSE state */
		for (d = descriptor_list; d; d = next_d) {
			next_d = d->next;
			if (STATE(d) == CON_CLOSE)
				close_socket(d);
		}
		
    /* give each descriptor an appropriate prompt */
		for (d = descriptor_list; d; d = d->next) {
		  
		  if (d->character) {
			  if (PRF_FLAGGED(d->character, PRF_AFK) || PLR_FLAGGED(d->character,PLR_CATARSI))
          continue;
      }
		  
			if (d->prompt_mode) {
				make_prompt(d);
				d->prompt_mode = 0;
			}
		}
		
    /*
     * Now, we execute as many pulses as necessary--just one if we haven't
     * missed any pulses, or make up for lost time if we missed a few
     * pulses by sleeping for too long.
     */
		missed_pulses++;
		
		if (missed_pulses <= 0) {
			log("SYSERR: MISSED_PULSES IS NONPOSITIVE!!!");
			missed_pulses = 1;
		}
		
    /* If we missed more than 30 seconds worth of pulses, forget it */
		if (missed_pulses > (30 * PASSES_PER_SEC)) {
			log("WARNING: Missed more than 30 seconds worth of pulses");
			missed_pulses = 30 * PASSES_PER_SEC;
		}
		
    /* Now execute the heartbeat functions */
		while (missed_pulses--)
			heartbeat(++pulse);
		
    /* Update tics for deadlock protection (UNIX only) */
		tics++;
	}
}

#define DELAY 2  /*Blizzard :prima era 1*/
void heartbeat(unsigned long pulse)
{
	static int mins_since_crashsave = 0;
	static int sfasa =-1;
	void auction_update();
	void process_events(void);
	
	dg_global_pulse++;
	event_process();       /* dg events */
	process_events();
	
	if (!(pulse % PULSE_DG_SCRIPT))
		script_trigger_check();	
	
	if (!(pulse % PULSE_ZONE))
		zone_update();
	
	if (!(pulse % (30 * PASSES_PER_SEC)))		// 30 secondi
		check_idle_passwords();

	if (!(pulse % PASSES_PER_SEC))
		check_malattia_morfeo();
	
	if (!(pulse % (15 * PASSES_PER_SEC)))
		auction_update();
    //PEPPE DAMAGE
	if(!(pulse % (15 * PASSES_PER_SEC)))	
		damage_update();
    //PEPPE FERITE, Spini: burning hands e chill touch, ferite di Shaarr e maledizione di Xhyphys
	if(!(pulse % (15 * PASSES_PER_SEC))) {
		ferite_update();
		chill_burn_update();
		check_male_antichi();
	}

	if (!(pulse % PASSES_PER_SEC))
		tempeste_update();
	
	if (!(pulse % PULSE_MOBILE))
		mobile_activity();
	
	if (!(pulse % (6 * PASSES_PER_SEC)))
		selvaggina_activity();
	
	if (!(pulse % (5 * PASSES_PER_SEC))){
		gain_fiato();
		check_catarsi();
		check_peste();
		check_raffreddore();
		ferite_shaarr_update();
	}
		
	if (!(pulse % (1 * PASSES_PER_SEC)))
		go_patrol_activity(pulse);

	if (!(pulse % (3 * PASSES_PER_SEC)))
		costo_fly();

	if (!(pulse % (2 * PASSES_PER_SEC)))
		movimento_eserciti();
	
	if(!(pulse %( 60 * PASSES_PER_SEC)))
		update_diplo_timers();
		
	if(!(pulse %( 90 * PASSES_PER_SEC))) {
		salva_eserciti();
	}
	
	if(!(pulse %( SECS_PER_REAL_HOUR * PASSES_PER_SEC))) // Chardhros-> Prova Stipendi!
		stipendio_mob();
	
	if (!(pulse % PULSE_VIOLENCE)) {
		perform_concentrato();
		perform_violence();
		perform_distorsione();
    //tick_grenade();
		if (in_arena == ARENA_RUNNING)
			if(!(pulse % PULSE_ARENA))
				do_game();
		if (in_arena == ARENA_START)
			if(!(pulse % PULSE_ARENA))
				start_arena();
	}
  //Blizzard portato il Delay da 1 a 2:ora un ora/mud dura 150 sec
	if (!(pulse %  (DELAY * SECS_PER_MUD_HOUR * PASSES_PER_SEC))) {
		weather_and_time(1);
		affect_update();
		point_update();
		save_time();
		fflush(player_fl);
		fflush(mobs_fl);
	}
	
  /* Treni rimossi versione 3.9
  //if (!(pulse % (15 * PASSES_PER_SEC)))	  // 15 secondi
  //  train_upd();
  */
	
  /* if(!(pulse%(SECS_PER_MUD_HOUR/3))){
    beware_lightning();
  }*/
	
	if (auto_save && !(pulse % (60 * PASSES_PER_SEC))) {	/* 1 minute */
		if (++mins_since_crashsave >= autosave_time) {
			mins_since_crashsave = 0;
			Crash_save_all();	  
			House_save_all();	
			
			
			
			mudlog("- Autosaving -", CMP, LVL_GOD, TRUE);
		}
	}
	
	if (!(pulse % (13 * 60 * PASSES_PER_SEC)))	/* 13 minutes */
	{
		log("mobsave"); 
		Save_mob();	  
		log("mobsave fine");
		mudlog("- Autosaving mobs -", CMP, LVL_GOD, TRUE);
	}	   
	
	if (!(pulse % (5 * 60 * PASSES_PER_SEC)))	/* 5 minutes */
		record_usage();
	if (!(pulse % (6 * 60 * PASSES_PER_SEC)))  // 6 minuti
		update_max_usage();
	if (!(pulse % (81 * 30 * PASSES_PER_SEC)))	/* 40,5 minutes */
	{
		approv_shop(); //Approvigionamento negozi tolto.
		save_shop();
	}	
	if (!(pulse % (2 * 60 * PASSES_PER_SEC)))	/* 2 minutes */
		mud_sound(0);
	if (!(pulse % (5 * 60 * PASSES_PER_SEC)))	/* 5 minutes */
		mud_sound(1);
	
   //Reset Risore della Wild (Shade)
	if (!(pulse % (180 * 60 * PASSES_PER_SEC)))	/* 3 ore */
		sfasa = 0; 
	
	if (!(pulse % (10 * 60 * PASSES_PER_SEC))){	// 10 minuti
		if (sfasa > -1) {
			switch (sfasa) {
				case RES_PIETRE:
					reset_risorse(RES_PIETRE); 
					sfasa++;
					break;
				case RES_ALBERI:
					reset_risorse(RES_ALBERI);
					sfasa++;
					break;
				case RES_GEMME:
					reset_risorse(RES_GEMME);	 
					sfasa++;
					break;
				case RES_ROCCE: 
					reset_risorse(RES_ROCCE);
					sfasa++;
					break;
				case RES_NATURA:
					reset_risorse(RES_NATURA);	 
					sfasa++;
					break;
				case RES_ERBE:  
					reset_risorse(RES_ERBE);
					sfasa++;
					break;
				case RES_SELVAGGINA:
					reset_risorse(RES_SELVAGGINA);	   
					sfasa++;
					break;
				case RES_AMMAESTRABILI:
					reset_risorse(RES_AMMAESTRABILI);	 	 
					sfasa = -1; 
					break;
			}  
		} 
	}
	
}


/* ******************************************************************
*  general utility stuff (for local use)                            *
****************************************************************** */

/*
 *  new code to calculate time differences, which works on systems
 *  for which tv_usec is unsigned (and thus comparisons for something
 *  being < 0 fail).  Based on code submitted by ss@sirocco.cup.hp.com.
 */

/*
 * code to return the time difference between a and b (a-b).
 * always returns a nonnegative value (floors at 0).
 */
struct timeval timediff(struct timeval a, struct timeval b)
{
	struct timeval rslt;
	
	if (a.tv_sec < b.tv_sec)
		return null_time;
	else if (a.tv_sec == b.tv_sec) {
		if (a.tv_usec < b.tv_usec)
			return null_time;
		else {
			rslt.tv_sec = 0;
			rslt.tv_usec = a.tv_usec - b.tv_usec;
			return rslt;
		}
	} else {			/* a->tv_sec > b->tv_sec */
		rslt.tv_sec = a.tv_sec - b.tv_sec;
		if (a.tv_usec < b.tv_usec) {
			rslt.tv_usec = a.tv_usec + 1000000 - b.tv_usec;
			rslt.tv_sec--;
		} else
			rslt.tv_usec = a.tv_usec - b.tv_usec;
		return rslt;
	}
}

/* add 2 timevals */
struct timeval timeadd(struct timeval a, struct timeval b)
{
	struct timeval rslt;
	
	rslt.tv_sec = a.tv_sec + b.tv_sec;
	rslt.tv_usec = a.tv_usec + b.tv_usec;
	
	while (rslt.tv_usec >= 1000000) {
		rslt.tv_usec -= 1000000;
		rslt.tv_sec++;
	}
	
	return rslt;
}


void record_usage(void)
{
	int sockets_connected = 0, sockets_playing = 0;
	struct descriptor_data *d;
	char buf[256];
		
	for (d = descriptor_list; d; d = d->next) {
		sockets_connected++;
		if (!d->connected)
			sockets_playing++;
	}
	
	sprintf(buf, "nusage: %-3d sockets connected, %-3d sockets playing",
		          sockets_connected, sockets_playing);
	log(buf);
	
#ifdef RUSAGE
	{
		struct rusage ru;
		
		getrusage(0, &ru);
		sprintf(buf, "rusage: user time: %ld sec, system time: %ld sec, max res size: %ld",
			ru.ru_utime.tv_sec, ru.ru_stime.tv_sec, ru.ru_maxrss);
		log(buf);
	}
#endif
	
}



/*
 * Turn off echoing (specific to telnet client)
 */
void echo_off(struct descriptor_data *d)
{
	char off_string[] =
	{
		(char) IAC,
		(char) WILL,
		(char) TELOPT_ECHO,
		(char) 0,
	};
	
	SEND_TO_Q(off_string, d);
}


/*
 * Turn on echoing (specific to telnet client)
 */
void echo_on(struct descriptor_data *d)
{
	char on_string[] =
	{
		(char) IAC,
		(char) WONT,
		(char) TELOPT_ECHO,
		(char) TELOPT_NAOFFD,
		(char) TELOPT_NAOCRD,
		(char) 0,
	};
	
	SEND_TO_Q(on_string, d);
}


void make_prompt(struct descriptor_data *d)
{
	
	extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
	int i;
	long exp_corrente,exp_totale,percentuale;
	char prompt[MAX_INPUT_LENGTH];
	
   /* reversed these top 2 if checks so that page_string() would work in */
   /* the editor */
	if (d->showstr_count) {
		sprintf(prompt, "\r%s[ Return to continue, (q)uit, (r)efresh, (b)ack, or page number (%d/%d) ]%s\r\n",
		                CCRED(d->character, C_NRM),
			              d->showstr_page, 
			              d->showstr_count,
			              CCNRM(d->character, C_NRM) );
		write_to_descriptor(d->descriptor, prompt);
	}
	else if (d->str)
		write_to_descriptor(d->descriptor, "] ");
	else if (!d->connected) {
		char prompt[MAX_INPUT_LENGTH];
		
		*prompt = '\0';
/*
 *sprintf(buf, "\x1B[30;%dr", ch->pagelen);
 *sprintf(buf, "\x1B[%d;%dB", 15, 10);
 *sprintf(buf, "\x1B[%d;%dH", 2, 1); 
 */		
    if (PRF_FLAGGED(d->character, PRF_DISPSTATIC))
      strcat(prompt,"\x1B[30;30r \x1B[30;31H");    
		
		if (IS_NPC(d->character))
			strcat(prompt,"-=<MOB>=- ");
		if (GET_INVIS_LEV(d->character))
			sprintf(prompt, "i%d ", GET_INVIS_LEV(d->character));
      
		if (!PRF_FLAGGED(d->character, PRF_RPG))
			strcat(prompt,"NOGDR ");
		if (AFF_FLAGGED(d->character, AFF_SANCTUARY))
			strcat(prompt,"[S] ");
		if (AFF_FLAGGED(d->character, AFF_FLYING))
			strcat(prompt,"[F] ");
		if (AFF_FLAGGED(d->character, AFF_INVISIBLE))
			strcat(prompt,"[I] ");
		if (AFF_FLAGGED(d->character, AFF_WATERWALK))
			strcat(prompt,"[W] ");
		if (AFF_FLAGGED(d->character, AFF_PASSDOOR))
			strcat(prompt,"[Wf] ");
		if (AFF_FLAGGED(d->character, AFF_MIRRORIMAGE))
			strcat(prompt,"[Mi] ");
		if (PRF_FLAGGED(d->character, PRF_NOKILL))
			strcat(prompt,"[NK] ");
		if (PLR_FLAGGED(d->character,PLR_FANTASMA))	
			strcat(prompt,"[FAN] ");
		
		if (PRF_FLAGGED(d->character, PRF_DISPHP))
		//	sprintf(prompt+strlen(prompt), "%s%d%s ", "HP ",
		//		GET_HIT(d->character),
		//		CCNRM(d->character, C_SPR));
			sprintf(prompt+strlen(prompt), "%s%d%s%%", "HP ",
				((GET_HIT(d->character)*100)/GET_MAX_HIT(d->character)),
				CCNRM(d->character, C_SPR));
		if (PRF_FLAGGED(d->character, PRF_DISPMANA))
			sprintf(prompt+strlen(prompt), "%s%d%s%%", " Ma ",
				((GET_MANA(d->character)*100)/GET_MAX_MANA(d->character)),
				CCNRM(d->character, C_SPR));
		
		if (PRF_FLAGGED(d->character, PRF_DISPMOVE))
			sprintf(prompt+strlen(prompt), "%s%d%s%%", " Mo ",
				((GET_MOVE(d->character)*100)/GET_MAX_MOVE(d->character)),
				CCNRM(d->character, C_SPR));
			//FIATO
		if(1)	
			sprintf(prompt+strlen(prompt), "%s%d%s%% ", " Fi " ,
				(GET_FIATO(d->character)*100)/((GET_CON(d->character)+GET_CON(d->character)+GET_DEX(d->character))/4 + (GET_MOVE(d->character)/15)),
				CCNRM(d->character, C_SPR));
		
		//AMANU PROVA NO XP
		
		exp_corrente = (titles[(int) GET_CLASS(d->character)][GET_LEVEL(d->character) + 1].exp) - GET_EXP(d->character);
		exp_totale = (titles[(int) GET_CLASS(d->character)][GET_LEVEL(d->character) + 1].exp) - (titles[(int) GET_CLASS(d->character)][(int)GET_LEVEL(d->character)].exp);
		if(exp_corrente!=exp_totale)
		    percentuale=((exp_corrente/(exp_totale/100))/10)+1;
		else
		    percentuale=10;    
		if (PRF_FLAGGED(d->character, PRF_DISPXP))
			if (GET_LEVEL(d->character) < (LVL_IMMORT - 1) && !IS_MOB(d->character))
				sprintf(prompt+strlen(prompt), "XP->Lv %d: %ld0%% ",
					GET_LEVEL(d->character) + 1,percentuale);
			
			
			
			
		if (RIDING(d->character) && PRF_FLAGGED(d->character, PRF_DISPCAV))
			sprintf(prompt+strlen(prompt), "%s%s %s%s ",
				      (MOB_FLAGGED(RIDING(d->character),MOB_HARD_TAME) ? CCRED(d->character, C_SPR) : CCCYN(d->character, C_SPR)),
				      (MOB_FLAGGED(RIDING(d->character),MOB_HARD_TAME) ? "Drago:" : "Cavallo:"),
				      frasi_move_status[get_move_status(RIDING(d->character))],
				      CCNRM(d->character, C_SPR));
		
		if (PRF_FLAGGED(d->character, PRF_DISPDAM)) {
			if (!FIGHTING(d->character))
				strcat(prompt, "[*] ");
			else {
				i = 100 * GET_HIT(FIGHTING(d->character)) / GET_MAX_HIT(FIGHTING(d->character));
				sprintf(prompt+strlen(prompt), "%s%d0%s%% ","MOB ", i / 10, CCNRM(d->character, C_SPR));
				if ((FIGHTING(FIGHTING(d->character))))
					if ((FIGHTING(FIGHTING(d->character)))!=(d->character)) {
						i = 100 * GET_HIT(FIGHTING(FIGHTING(d->character))) / GET_MAX_HIT(FIGHTING(FIGHTING(d->character)));
						sprintf(prompt+strlen(prompt), "%s%d0%s%% ","TANK ", i / 10, CCNRM(d->character, C_SPR));
					}
			}
		}	
		strcat(prompt, "> ");
		if (PRF_FLAGGED(d->character, PRF_DISPSTATIC))
      strcat(prompt,"\x1B[28,1 \r\n");
		write_to_descriptor(d->descriptor, prompt);
	}
	
}


void write_to_q(char *txt, struct txt_q *queue, int aliased)
{
	struct txt_block *newton;
	
	CREATE(newton, struct txt_block, 1);
	CREATE(newton->text, char, strlen(txt) + 1);
	strcpy(newton->text, txt);
	newton->aliased = aliased;
	
  /* queue empty? */
	if (!queue->head) {
		newton->next = NULL;
		queue->head = queue->tail = newton;
	} else {
		queue->tail->next = newton;
		queue->tail = newton;
		newton->next = NULL;
	}
}



int get_from_q(struct txt_q *queue, char *dest, int *aliased)
{
	struct txt_block *tmp;
	
  /* queue empty? */
	if (!queue->head)
		return 0;
	
	tmp = queue->head;
	strcpy(dest, queue->head->text);
	*aliased = queue->head->aliased;
	queue->head = queue->head->next;
	
	free(tmp->text);
	free(tmp);
	
	return 1;
}



/* Empty the queues before closing connection */
void flush_queues(struct descriptor_data *d)
{
	int dummy;
	
	if (d->large_outbuf) {
		d->large_outbuf->next = bufpool;
		bufpool = d->large_outbuf;
	}
	while (get_from_q(&d->input, buf2, &dummy));
}




// Add a new string to a player's output queue 
void write_to_output( const char *strPtr, struct descriptor_data *descPtr )
{
 char *textColorParser = NULL;
 char *textSoundParser = NULL;

 // Check for SOUND parsing.
 if  ( descPtr->character )
     if  ( ! PRF_FLAGGED( descPtr->character, PRF_SOUNDS ) )
         textSoundParser = parse_sound( strPtr );

 if  ( ! textSoundParser ) textSoundParser = strdup( strPtr );

 // Check for COLOR parsing.
 textColorParser = parse_color( textSoundParser, descPtr );
 free( textSoundParser );  // Free temporal SOUND parser buffer.

 // Send string to socket.
 write_bin_to_output( (BYTE *)textColorParser, strlen( textColorParser ), descPtr );

 free( textColorParser );  // Free temporal COLOR parser buffer.
};


// Add a new buffer to a player's output queue
void write_bin_to_output( BYTE *bufPtr, int bufLen, struct descriptor_data *descPtr )
{
 // If we're in the overflow state already, ignore this new output
 if  ( descPtr->bufptr < 0 ) return;

 // If we have enough space, just write to buffer and that's it!
 if  ( descPtr->bufspace >= bufLen )
     {
      memcpy( descPtr->output + descPtr->bufptr, bufPtr, bufLen ); // Copy buffer into socket buffer.
      descPtr->bufptr   += bufLen;      // Increase used      buffer space.
      descPtr->bufspace -= bufLen;      // Decrease available buffer space.
      return;
     };

 // If we're already using the large buffer, or if even the large buffer
 // is too small to handle this new text, chuck the text and switch to the
 // overflow state.
 if  ( descPtr->large_outbuf || ( ( bufLen + descPtr->bufptr ) > LARGE_BUFSIZE ) )
     {
      descPtr->bufptr = -1;
      buf_overflows++;
      return;
     };

 buf_switches++;        // Increase number of buffer switches.

 // If the pool has a buffer in it, grab it
 if  ( bufpool != NULL )
     {
      descPtr->large_outbuf = bufpool;
      bufpool = bufpool->next;
     }
 else
     {                  // Else create a new one.
      CREATE( descPtr->large_outbuf,       struct txt_block, 1             );
      CREATE( descPtr->large_outbuf->text, char,             LARGE_BUFSIZE );
      buf_largecount++;
     };

 memcpy( descPtr->large_outbuf->text, descPtr->output, descPtr->bufptr ); // Copy to big buffer.
 descPtr->output = descPtr->large_outbuf->text;                 // Make big buffer primary.

 memcpy( descPtr->output + descPtr->bufptr, bufPtr, bufLen );   // Now add new buffer.
 descPtr->bufptr   += bufLen;                                   // Increase used      buffer space.
 descPtr->bufspace  = LARGE_BUFSIZE - 1 - descPtr->bufptr;      // Calculate how much space is left in the buffer.
};





/* ******************************************************************
*  socket handling                                                  *
****************************************************************** */


int new_descriptor(int s) {
	socket_t desc;
	int sockets_connected = 0;
	unsigned long addr;
	int i;
	static int last_desc = 0;	/* last descriptor number */
	struct descriptor_data *newd;
	struct sockaddr_in peer;
	struct hostent *from;
	extern char *GREETINGS;
	
  /* accept the new connection */
	i = sizeof(peer);
	if ((desc = accept(s, (struct sockaddr *) &peer, &i)) == INVALID_SOCKET) {
		perror("accept");
		return -1;
	}
  /* keep it from blocking */
	nonblock(desc);
	
  /* make sure we have room for it */
	for (newd = descriptor_list; newd; newd = newd->next)
		sockets_connected++;
	
	if (sockets_connected >= max_players) {
		write_to_descriptor(desc, "Sorry, CircleMUD is full right now... please try again later!\r\n");
		CLOSE_SOCKET(desc);
		return 0;
	}
  /* create a new descriptor */
	CREATE(newd, struct descriptor_data, 1);
	memset((char *) newd, 0, sizeof(struct descriptor_data));
	
  /* find the sitename */
	if (nameserver_is_slow || !(from = gethostbyaddr((char *) &peer.sin_addr,
					sizeof(peer.sin_addr), AF_INET))) {
		
    /* resolution failed */
		if (!nameserver_is_slow)
			perror("gethostbyaddr");
		
    /* find the numeric site address */
		addr = ntohl(peer.sin_addr.s_addr);
		sprintf(newd->host, "%03u.%03u.%03u.%03u", (int) ((addr & 0xFF000000) >> 24),
			(int) ((addr & 0x00FF0000) >> 16), (int) ((addr & 0x0000FF00) >> 8),
			(int) ((addr & 0x000000FF)));
	} else {
		strncpy(newd->host, from->h_name, HOST_LENGTH);
		*(newd->host + HOST_LENGTH) = '\0';
	}
	
  /* determine if the site is banned */
	if (isbanned(newd->host) == BAN_ALL) {
		CLOSE_SOCKET(desc);
		sprintf(buf2, "Connection attempt denied from [%s]", newd->host);
		mudlog(buf2, CMP, LVL_GOD, TRUE);
		free(newd);
		return 0;
	}
#if 0
  /* Log new connections - probably unnecessary, but you may want it */
	sprintf(buf2, "New connection from [%s]", newd->host);
	mudlog(buf2, CMP, LVL_GOD, FALSE);
#endif
	
  /* initialize descriptor data */
	newd->descriptor = desc;
	newd->connected = CON_GET_NAME;
	newd->idle_tics = 0;
	newd->wait = 1;
	newd->output = newd->small_outbuf;
	newd->bufspace = SMALL_BUFSIZE - 1;
	newd->next = descriptor_list;
	newd->login_time = time(0);
	CREATE(newd->history, char *, HISTORY_SIZE);
	if (++last_desc == 1000)
		last_desc = 1;
	newd->desc_num = last_desc;
	
  /* prepend to list */
	descriptor_list = newd;
	
	SEND_TO_Q(GREETINGS, newd);

	return 0;
}



int process_output( struct descriptor_data *descPtr )
{
 static BYTE bufTmp[ LARGE_BUFSIZE + GARBAGE_SPACE ];
 static int  bufLen;
 static int  result;

 // If we're in the overflow state, notify the user.
 if  ( descPtr->bufptr < 0 ) {
      strcpy( (char *)bufTmp, "\r\n**OVERFLOW**" );
      bufLen = strlen( (char *)bufTmp );
     }
 else
     {
      // We may need this "\r\n" for later -- see below.
      strcpy( (char *)bufTmp, "\r\n" );
      bufLen = strlen( (char *)bufTmp );

      // Now, append the 'real' output
      memcpy( bufTmp + bufLen, descPtr->output, descPtr->bufptr );
      bufLen += descPtr->bufptr;
     };

 // Add the extra CRLF if the person isn't in compact mode.
 if  ( ! descPtr->connected && descPtr->character && ! PRF_FLAGGED( descPtr->character, PRF_COMPACT ) )
     {
      memcpy( bufTmp + bufLen, "\r\n", 2 );
      bufLen += 2;
     }

 // Now, send the output.  If this is an 'interruption', use the prepended
 // CRLF, otherwise send the straight output sans CRLF.
 if  ( ! descPtr->prompt_mode )		/* && !t->connected) */
     result = write_bin_to_descriptor( descPtr->descriptor, bufTmp,     bufLen     );
 else
     result = write_bin_to_descriptor( descPtr->descriptor, bufTmp + 2, bufLen - 2 );

 // Handle snooping: prepend "% " and send to snooper.
 if ( descPtr->snoop_by ) {
      SEND_TO_Q( "% ", descPtr->snoop_by );
      //SEND_TO_Q( descPtr->output, descPtr->snoop_by );
      //SEND_TO_Q( " Snoop Rotto ;D ", descPtr->snoop_by );
      SEND_TO_Q( "%%", descPtr->snoop_by );
     }

 // If we were using a large buffer, put the large buffer on the buffer pool
 // and switch back to the small one.
 if  ( descPtr->large_outbuf )
     {
      // Remove LARGE buffer and put it in the queue.
      descPtr->large_outbuf->next = bufpool;
      bufpool                     = descPtr->large_outbuf;
      descPtr->large_outbuf       = NULL;

      // Reassign SMALL buffer.
      descPtr->output             = descPtr->small_outbuf;
     };

 // Reset total bufspace back to that of a small buffer.
 descPtr->bufspace  = SMALL_BUFSIZE - 1;
 descPtr->bufptr    = 0;
 *(descPtr->output) = '\0';

 return result;
};

// Function used to write a string into socket descriptor.
int write_to_descriptor( socket_t desc, char *strPtr )
{
 return write_bin_to_descriptor( desc, (BYTE *)strPtr, strlen( strPtr ) );
};

// Function used to write a buffer into socket descriptor.
int write_bin_to_descriptor( socket_t desc, BYTE *bufPtr, int bufLen )
{
 int bytes_written;

 do
   {
#ifdef CIRCLE_WINDOWS
    if ( ( bytes_written = send( desc, bufPtr, bufLen, 0 ) ) < 0 ) {
	    if  ( WSAGetLastError() == WSAEWOULDBLOCK )
#else
    if  ( ( bytes_written = write( desc, bufPtr, bufLen ) ) < 0 ) {

#ifdef EWOULDBLOCK
	    if  ( errno == EWOULDBLOCK ) errno = EAGAIN;
#endif /* EWOULDBLOCK */
      if  ( errno == EAGAIN )
#endif /* CIRCLE_WINDOWS */
	      log( "process_output: socket write would block, about to close" );
      else
	      perror( "Write to socket" );
      return -1;
	 }
   else {
	   bufPtr += bytes_written;
	   bufLen -= bytes_written;
   };
 }
 while ( bufLen > 0 );

 return 0;
};



/*
 * ASSUMPTION: There will be no newlines in the raw input buffer when this
 * function is called.  We must maintain that before returning.
 */
int process_input(struct descriptor_data *t)
{
	int buf_length, bytes_read, space_left, failed_subst;
	char *ptr, *read_point, *write_point, *nl_pos = NULL;
	char tmp[MAX_INPUT_LENGTH + 8];
	
  /* first, find the point where we left off reading data */
	buf_length = strlen(t->inbuf);
	read_point = t->inbuf + buf_length;
	space_left = MAX_RAW_INPUT_LENGTH - buf_length - 1;
	
	do {
		if (space_left <= 0) {
			log("process_input: about to close connection: input overflow");
			return -1;
		}
#ifdef CIRCLE_WINDOWS
		if ((bytes_read = recv(t->descriptor, read_point, space_left, 0)) < 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
				if ((bytes_read = read(t->descriptor, read_point, space_left)) < 0) {
#ifdef EWOULDBLOCK
					if (errno == EWOULDBLOCK)
						errno = EAGAIN;
#endif /* EWOULDBLOCK */
					if (errno != EAGAIN) {
#endif /* CIRCLE_WINDOWS */
						perror("process_input: about to lose connection");
						return -1;		// some error condition was encountered on read
					} else
						return 0;	//the read would have blocked: just means no data there but everything's okay
				} 
				else if (bytes_read == 0) {
					log("process_input: EOF on socket read (connection broken by peer)");
					return -1;
				}
    
    // at this point, we know we got some data from the read
				
				*(read_point + bytes_read) = '\0';	// terminate the string
				
    // search for a newline in the data we just read
				for (ptr = read_point; *ptr && !nl_pos; ptr++)
					if (ISNEWL(*ptr))
						nl_pos = ptr;
					
				read_point += bytes_read;
				space_left -= bytes_read;
				
/*
 * on some systems such as AIX, POSIX-standard nonblocking I/O is broken,
 * causing the MUD to hang when it encounters input not terminated by a
 * newline.  This was causing hangs at the Password: prompt, for example.
 * I attempt to compensate by always returning after the _first_ read, instead
 * of looping forever until a read returns -1.  This simulates non-blocking
 * I/O because the result is we never call read unless we know from select()
 * that data is ready (process_input is only called if select indicates that
 * this descriptor is in the read set).  JE 2/23/95.
 */
#if !defined(POSIX_NONBLOCK_BROKEN)
			} while (nl_pos == NULL);
#else
		} while (0);
		
		if (nl_pos == NULL)
			return 0;
#endif /* POSIX_NONBLOCK_BROKEN */
		
  /*
   * okay, at this point we have at least one newline in the string; now we
   * can copy the formatted data to a new array for further processing.
   */
		
		read_point = t->inbuf;
		
		while (nl_pos != NULL) {
			write_point = tmp;
			space_left = MAX_INPUT_LENGTH - 1;
			
			for (ptr = read_point; (space_left > 0) && (ptr < nl_pos); ptr++) {
				if (*ptr == '\b') {	/* handle backspacing */
					if (write_point > tmp) {
						if (*(--write_point) == '$') {
							write_point--;
							space_left += 2;
						} else
							space_left++;
					}
				} else if (isascii(*ptr) && isprint(*ptr)) {
					if ((*(write_point++) = *ptr) == '$') {		/* copy one character */
						*(write_point++) = '$';	/* if it's a $, double it */
						space_left -= 2;
					} else
						space_left--;
				}
			}
			
			*write_point = '\0';
			
			if ((space_left <= 0) && (ptr < nl_pos)) {
				char buffer[MAX_INPUT_LENGTH + 64];
				
				sprintf(buffer, "Line too long.  Truncated to:\r\n%s\r\n", tmp);
				if (write_to_descriptor(t->descriptor, buffer) < 0)
					return -1;
			}
			if (t->snoop_by) {
				SEND_TO_Q("% ", t->snoop_by);
				SEND_TO_Q(tmp, t->snoop_by);
				SEND_TO_Q("\r\n", t->snoop_by);
			}
			failed_subst = 0;
			
			
			if (*tmp == '!' && !(*(tmp + 1)))	/* Redo last command. */
				strcpy(tmp, t->last_input);
			else if (*tmp == '!' && *(tmp + 1)) {
				char *commandln = (tmp + 1);
				int starting_pos = t->history_pos,
				cnt = (t->history_pos == 0 ? HISTORY_SIZE - 1 : t->history_pos - 1);
				
				skip_spaces(&commandln);
				for (; cnt != starting_pos; cnt--) {
					if (t->history[cnt] && is_abbrev(commandln, t->history[cnt])) {
						strcpy(tmp, t->history[cnt]);
						break;
					}
					if (cnt == 0)	/* At top, loop to bottom. */
						cnt = HISTORY_SIZE;
				}
			} else if (*tmp == '^') {
				
				if (!(failed_subst = perform_subst(t, t->last_input, tmp)))
					strcpy(t->last_input, tmp);
			} else {
				
				strcpy(t->last_input, tmp);
				if (t->history[t->history_pos])
					free(t->history[t->history_pos]);	/* Clear the old line. */
				t->history[t->history_pos] = str_dup(tmp);	/* Save the new. */
				if (++t->history_pos >= HISTORY_SIZE)	/* Wrap to top. */
					t->history_pos = 0;
			}
			if (!failed_subst)
				write_to_q(tmp, &t->input, 0);
			
    /* find the end of this line */
			while (ISNEWL(*nl_pos))
				nl_pos++;
			
    /* see if there's another newline in the input buffer */
			read_point = ptr = nl_pos;
			for (nl_pos = NULL; *ptr && !nl_pos; ptr++)
				if (ISNEWL(*ptr))
					nl_pos = ptr;
		}
		
  /* now move the rest of the buffer up to the beginning for the next pass */
		write_point = t->inbuf;
		while (*read_point)
			*(write_point++) = *(read_point++);
		*write_point = '\0';
		
		return 1;
	}
	
	
	
/*
 * perform substitution for the '^..^' csh-esque syntax
 * orig is the orig string (i.e. the one being modified.
 * subst contains the substition string, i.e. "^telm^tell"
 */
	int perform_subst(struct descriptor_data *t, char *orig, char *subst)
{
	char new[MAX_INPUT_LENGTH + 5];
	
	char *first, *second, *strpos;
	
  /*
   * first is the position of the beginning of the first string (the one
   * to be replaced
   */
	first = subst + 1;
	
  /* now find the second '^' */
	if (!(second = strchr(first, '^'))) {
		SEND_TO_Q("Invalid substitution.\r\n", t);
		return 1;
	}
  /* terminate "first" at the position of the '^' and make 'second' point
   * to the beginning of the second string */
	*(second++) = '\0';
	
  /* now, see if the contents of the first string appear in the original */
	if (!(strpos = strstr(orig, first))) {
		SEND_TO_Q("Invalid substitution.\r\n", t);
		return 1;
	}
  /* now, we construct the new string for output. */
	
  /* first, everything in the original, up to the string to be replaced */
	strncpy(new, orig, (strpos - orig));
	new[(strpos - orig)] = '\0';
	
  /* now, the replacement string */
	strncat(new, second, (MAX_INPUT_LENGTH - strlen(new) - 1));
	
  /* now, if there's anything left in the original after the string to
   * replaced, copy that too. */
	if (((strpos - orig) + strlen(first)) < strlen(orig))
		strncat(new, strpos + strlen(first), (MAX_INPUT_LENGTH - strlen(new) - 1));
	
  /* terminate the string in case of an overflow from strncat */
	new[MAX_INPUT_LENGTH - 1] = '\0';
	strcpy(subst, new);
	
	return 0;
}



void close_socket(struct descriptor_data *d)
{
  char buf[128];
  struct descriptor_data *temp;
  long target_idnum = -1;

  CLOSE_SOCKET(d->descriptor);
  flush_queues(d);

  /* Forget snooping */
  if (d->snooping)
	  d->snooping->snoop_by = NULL;

  if (d->snoop_by) {
	  SEND_TO_Q("Stop Snooping.\r\n", d->snoop_by);
	  d->snoop_by->snooping = NULL;
  }

  /*. Kill any OLC stuff .*/
/*  switch(d->connected) { 
  	case CON_OEDIT:
	  case CON_REDIT:
	  case CON_ZEDIT:
	  case CON_MEDIT:
	  case CON_SEDIT:
		  cleanup_olc(d, CLEANUP_ALL);
	  default:
		  break;
  }
*/
/* In questo modo faccio scollegare anche il descrittore
dei pg nei menu di inizializzazione gilda - Chardhros 2/5/2001 */

  switch(d->connected) {
	  case CON_CONF_INIZIAZ:
	  case CON_AGPT_INIZIAZ:
	  case CON_PREMI_TASTO:
	  case CON_SCELTA_ABIL:
		  STATE(d) = CON_PLAYING;
		  sprintf(buf,"(DEBUG) -Iniziazione- Il PG %s si e' scollegato.", d->character->player.name);
		  log(buf);
	  default:
		  break;
  }

  if (d->character) {
	  target_idnum = GET_IDNUM(d->character);
	  
	  if (d->connected == CON_PLAYING) {
		  save_char(d->character, NOWHERE);
		  act("$n cade a terra e sviene.", TRUE, d->character, 0, 0, TO_ROOM);
		  sprintf(buf, "Closing link to: %s.", GET_NAME(d->character));
		  mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
		  d->character->desc = NULL;
	  } 
	  else {
		  sprintf(buf, "Losing player: %s.", GET_NAME(d->character) ? GET_NAME(d->character) : "<null>");
		  mudlog(buf, CMP, LVL_IMMORT, TRUE);
		  free_char(d->character);
	  }
  } 
  else
	  log("Losing descriptor without char.");

  /* JE 2/22/95 -- part of my unending quest to make switch stable */
  if (d->original && d->original->desc)
	  d->original->desc = NULL;

  REMOVE_FROM_LIST(d, descriptor_list, next);
 /* Clear the command history. */
  if (d->history) {
	  int cnt;
	  for (cnt = 0; cnt < HISTORY_SIZE; cnt++)
		if (d->history[cnt])
			free(d->history[cnt]);
	  
	  free(d->history);
  }

  if (d->showstr_head)
	  free(d->showstr_head);
  
  if (d->showstr_count)
	  free(d->showstr_vector);
  
  if (d->storage)
	  free(d->storage);

  free(d);
}



void check_idle_passwords(void)
{
struct descriptor_data *d, *next_d;

for (d = descriptor_list; d; d = next_d) {
next_d = d->next;
if (STATE(d) != CON_PASSWORD && STATE(d) != CON_GET_NAME)
	continue;
if (!d->idle_tics) {
	d->idle_tics++;
	continue;
} else {
	echo_on(d);
	SEND_TO_Q("\r\nTempo scaduto... arrivederci.\r\n", d);
	STATE(d) = CON_CLOSE;
}
}
}



/*
 * I tried to universally convert Circle over to POSIX compliance, but
 * alas, some systems are still straggling behind and don't have all the
 * appropriate defines.  In particular, NeXT 2.x defines O_NDELAY but not
 * O_NONBLOCK.  Krusty old NeXT machines!  (Thanks to Michael Jones for
 * this and various other NeXT fixes.)
 */

#ifdef CIRCLE_WINDOWS

void nonblock(socket_t s)
{
long val;

val = 1;
ioctlsocket(s, FIONBIO, &val);
}

#else

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

void nonblock(socket_t s)
{
int flags;

flags = fcntl(s, F_GETFL, 0);
flags |= O_NONBLOCK;
if (fcntl(s, F_SETFL, flags) < 0) {
perror("Fatal error executing nonblock (comm.c)");
exit(1);
}
}


/* ******************************************************************
*  signal-handling functions (formerly signals.c)                   *
****************************************************************** */


RETSIGTYPE checkpointing()
{
  if (!tics) {
    log("(CRASH) CHECKPOINT shutdown: tics not updated");
    mudlog(buf, CMP, LVL_IMMORT, TRUE);
    log("Start Game Save.");
    log("Crash-saving keepers.");
    shop_crash_save();
    log("Crash-saving players.");
    Crash_save_all();
    log("Crash-saving houses.");
    House_save_all();
    log("Saving Mobs");
    Save_mob();
    log("Salvataggio eserciti");
    salva_eserciti();
    log("Closing all sockets.");
    while (descriptor_list)
      close_socket(descriptor_list);
    fclose(player_fl);
    abort();
  } 
  else
    tics = 0;
}


RETSIGTYPE reread_wizlists()
{
void reboot_wizlists(void);

mudlog("Signal received - rereading wizlists.", CMP, LVL_IMMORT, TRUE);
reboot_wizlists();
}


RETSIGTYPE unrestrict_game()
{
extern struct ban_list_element *ban_list;
extern int num_invalid;

mudlog("Received SIGUSR2 - completely unrestricting game (emergent)",
BRF, LVL_IMMORT, TRUE);
ban_list = NULL;
restrict = 0;
num_invalid = 0;
}


RETSIGTYPE hupsig()
{
/* In  qeusto modo il reboot mattutino non e' piu' un crash
 ADDIO DUPLICAZIONE OBJ, BWABWABWABWA, Chardhros */
log("Received SIGHUP, SIGINT, or SIGTERM.  Shutting down...");
send_to_all("Connessione Chiusa per manutenzione.\r\n");
circle_shutdown = 1;	
}


/*
 * This is an implementation of signal() using sigaction() for portability.
 * (sigaction() is POSIX; signal() is not.)  Taken from Stevens' _Advanced
 * Programming in the UNIX Environment_.  We are specifying that all system
 * calls _not_ be automatically restarted for uniformity, because BSD systems
 * do not restart select(), even if SA_RESTART is used.
 *
 * Note that NeXT 2.x is not POSIX and does not have sigaction; therefore,
 * I just define it to be the old signal.  If your system doesn't have
 * sigaction either, you can use the same fix.
 *
 * SunOS Release 4.0.2 (sun386) needs this too, according to Tim Aldric.
 */

#ifndef POSIX
#define my_signal(signo, func) signal(signo, func)
#else
sigfunc *my_signal(int signo, sigfunc * func)
{
struct sigaction act, oact;

act.sa_handler = func;
sigemptyset(&act.sa_mask);
act.sa_flags = 0;
#ifdef SA_INTERRUPT
act.sa_flags |= SA_INTERRUPT;	/* SunOS */
#endif

if (sigaction(signo, &act, &oact) < 0)
return SIG_ERR;

return oact.sa_handler;
}
#endif				/* NeXT */


RETSIGTYPE segsig() { /* Chardhros SEGNALE CATTURA CRSH */
my_signal(SIGSEGV, NULL);
log("Crash");
sprintf(buf,"(CRASH) : Mamba-1 %s (§.§) GURU Guarding  ", mamba_uno);
log(buf);
sprintf(buf,"(CRASH) : Mamba-2 %s (x.x) GURU Sleeping  ", mamba_due);
log(buf);
sprintf(buf,"(CRASH) : Mamba-3 %s (^.^) GURU Dreaming  ", mamba_tre);
log(buf);
sprintf(buf,"(CRASH) : Mamba-4 %s (o.o) GURU Wake Up   ", mamba_qua);
log(buf);
sprintf(buf,"(CRASH) : Mamba-5 %s (@.@) GURU Stressssss", mamba_cin);
log(buf);
sprintf(buf,"(CRASH) : Mamba-6 %s (-.-) GURU Meditation", phantomima);
log(buf);

sprintf(buf,"(CRASH) : %s ha fatto crashare con il comando %s nella stanxa %5d",GET_NAME(last_ch), last_command, last_ch->in_room != NOWHERE ?
		world[last_ch->in_room].number : NOWHERE);
mudlog(buf, CMP, LVL_IMMORT, TRUE);
log("Start Game Save.");
log("Crash-saving keepers.");
shop_crash_save();
log("Crash-saving players.");
Crash_save_all();
log("Crash-saving houses.");
House_save_all();
log("Saving Mobs");
Save_mob();
log("Salvataggio eserciti");
salva_eserciti();
log("Closing all sockets.");
while (descriptor_list)
close_socket(descriptor_list);
fclose(player_fl);
log("- Crash -");

}

void signal_setup(void)
{
#ifndef CIRCLE_OS2
  struct itimerval itime;
#endif
struct timeval interval;

  /* user signal 1: reread wizlists.  Used by autowiz system. */
my_signal(SIGUSR1, reread_wizlists);

  /*
   * user signal 2: unrestrict game.  Used for emergencies if you lock
   * yourself out of the MUD somehow.  (Duh...)
   */
my_signal(SIGUSR2, unrestrict_game);

  /*
   * set up the deadlock-protection so that the MUD aborts itself if it gets
   * caught in an infinite loop for more than 3 minutes.  Doesn't work with
   * OS/2.
   */
#ifndef CIRCLE_OS2
  interval.tv_sec = 180;
  interval.tv_usec = 0;
  itime.it_interval = interval;
  itime.it_value = interval;
  setitimer(ITIMER_VIRTUAL, &itime, NULL);
  my_signal(SIGVTALRM, checkpointing);
#endif

  /* just to be on the safe side: */
my_signal(SIGHUP, hupsig);
my_signal(SIGINT, hupsig);
my_signal(SIGTERM, hupsig);
my_signal(SIGPIPE, SIG_IGN);
my_signal(SIGALRM, SIG_IGN);
my_signal(SIGSEGV, segsig);

#ifdef CIRCLE_OS2
#if defined(SIGABRT)
my_signal(SIGABRT, hupsig);
#endif
#if defined(SIGFPE)
my_signal(SIGFPE, hupsig);
#endif
#if defined(SIGILL)
my_signal(SIGILL, hupsig);
#endif
#if defined(SIGSEGV)
my_signal(SIGSEGV, hupsig);
#endif
#endif				/* CIRCLE_OS2 */

}

#endif				/* CIRCLE_WINDOWS */


/* ****************************************************************
*       Public routines for system-to-player-communication        *
**************************************************************** */

void send_to_char(char *messg, struct char_data *ch)
{
  if (ch->desc && messg)
    SEND_TO_Q(messg, ch->desc);
}

void send_to_arena(char *messg)
{
struct descriptor_data *d;

for (d = descriptor_list; d; d = d->next) {
if (d->descriptor && messg && d->character)
if (PRF_FLAGGED(d->character, PRF_ARENA))
SEND_TO_Q(messg, d);
}
}


void send_bin_to_char(char *messg ,int size, struct char_data *ch)
{
if (ch->desc && messg)
SEND_TO_BIN_Q(messg, size, ch->desc);
}



void send_to_all(char *messg)
{
struct descriptor_data *i;

if (messg)
for (i = descriptor_list; i; i = i->next)
if (!i->connected)
SEND_TO_Q(messg, i);
}


void send_to_outdoor(char *messg)
{
struct descriptor_data *i;

if (!messg || !*messg)
return;

for (i = descriptor_list; i; i = i->next)
if (!i->connected && i->character && AWAKE(i->character) &&
OUTSIDE(i->character))
SEND_TO_Q(messg, i);
}



void send_to_room(char *messg, int room)
{
struct char_data *i;

if (messg)
for (i = world[room].people; i; i = i->next_in_room)
if (i->desc)
SEND_TO_Q(messg, i->desc);
}



char *ACTNULL = "<NULL>";
	
#define CHECK_NULL(pointer, expression) \
  if ((pointer) == NULL) i = ACTNULL; else i = (expression);	
	
/* higher-level communication: the act() function */
	void perform_act(char *orig, struct char_data *ch, struct obj_data *obj,
		void *vict_obj, struct char_data *to)
{
	register char *i = NULL, *buf;
	static char lbuf[MAX_STRING_LENGTH];
	struct char_data *dg_victim = NULL;
	struct obj_data *dg_target = NULL;
	char *dg_arg = NULL;
	buf = lbuf;
	
	for (;;) {
		if (*orig == '$') {
			switch (*(++orig)) {
				case 'n':
					i = PERS(ch, to);
					break;
				case 'N':
					CHECK_NULL(vict_obj, PERS((struct char_data *) vict_obj, to));
					dg_victim = (struct char_data *) vict_obj;
					break;
				case 'm':
					i = HMHR(ch);
					break;
				case 'M':
					CHECK_NULL(vict_obj, HMHR((struct char_data *) vict_obj));
					dg_victim = (struct char_data *) vict_obj;
					break;
				case 's':
					i = HSHR(ch);
					break;
				case 'S':
					CHECK_NULL(vict_obj, HSHR((struct char_data *) vict_obj));
					dg_victim = (struct char_data *) vict_obj;
					break;
				case 'e':
					i = HSSH(ch);
					break;
				case 'E':
					CHECK_NULL(vict_obj, HSSH((struct char_data *) vict_obj));
					dg_victim = (struct char_data *) vict_obj;
					break;
				case 'o':
					CHECK_NULL(obj, OBJN(obj, to));
					break;
				case 'O':
					CHECK_NULL(vict_obj, OBJN((struct obj_data *) vict_obj, to));
					dg_target = (struct obj_data *) vict_obj;
					break;
				case 'p':
					CHECK_NULL(obj, OBJS(obj, to));
					break;
				case 'P':
					CHECK_NULL(vict_obj, OBJS((struct obj_data *) vict_obj, to));
					dg_target = (struct obj_data *) vict_obj;
					break;
				case 'a':
					CHECK_NULL(obj, SANA(obj));
					break;
				case 'A':
					CHECK_NULL(vict_obj, SANA((struct obj_data *) vict_obj));
					dg_target = (struct obj_data *) vict_obj;
					break;
				case 'T':
					CHECK_NULL(vict_obj, (char *) vict_obj);
					dg_arg = (char *) vict_obj;
					break;
				case 'F':
					CHECK_NULL(vict_obj, fname((char *) vict_obj));
					break;
				case '$':
					i = "$";
					break;
				default:
					log("SYSERR: Illegal $-code to act():");
					strcpy(buf1, "SYSERR: ");
					strcat(buf1, orig);
					log(buf1);
					break;
			}
			while ((*buf = *(i++)))
				buf++;
			orig++;
		} else if (!(*(buf++) = *(orig++)))
			break;
	}
	
	*(--buf) = '\r';
	*(++buf) = '\n';
	*(++buf) = '\0';
	
	if (to->desc)
		SEND_TO_Q(CAP(lbuf), to->desc);
	
	if (IS_NPC(to) && dg_act_check)
		act_mtrigger(to, lbuf, ch, dg_victim, obj, dg_target, dg_arg);
}

/* moved this to utils.h --- mah */
#ifndef SENDOK
#define SENDOK(ch) ((ch)->desc && (AWAKE(ch) || sleep) && \
		                 !PLR_FLAGGED((ch), PLR_WRITING) && !PRF_FLAGGED(ch, PRF_AFK) && !PLR_FLAGGED(i,PLR_CATARSI))
#endif

void act(char *str, int hide_invisible, struct char_data *ch,
	struct obj_data *obj, void *vict_obj, int type)
{
	struct char_data *to;
	static int sleep;
	extern struct index_data *mob_index;
	extern struct index_data *obj_index;
	if (!str || !*str)
		return;
	
	if (!(dg_act_check = !(type & DG_NO_TRIG)))
		type &= ~DG_NO_TRIG;

	
  /*
   * Warning: the following TO_SLEEP code is a hack.
   *
   * I wanted to be able to tell act to deliver a message regardless of sleep
   * without adding an additional argument.  TO_SLEEP is 128 (a single bit
   * high up).  It's ONLY legal to combine TO_SLEEP with one other TO_x
   * command.  It's not legal to combine TO_x's with each other otherwise.
   */
	
  /* check if TO_SLEEP is there, and remove it if it is. */
	if ((sleep = (type & TO_SLEEP)))
		type &= ~TO_SLEEP;
	
	
	if (type == TO_CHAR) {
		if (ch && SENDOK(ch))
			perform_act(str, ch, obj, vict_obj, ch);
		return;
	}
	if (type == TO_VICT) {
		if ((to = (struct char_data *) vict_obj) && SENDOK(to))
			perform_act(str, ch, obj, vict_obj, to);
		return;
	}
  /* ASSUMPTION: at this point we know type must be TO_NOTVICT or TO_ROOM */
	
	if (ch && ch->in_room != NOWHERE)
		to = world[ch->in_room].people;
	else if (obj && obj->in_room != NOWHERE)
		to = world[obj->in_room].people;
	else {
		if(ch)
			sprintf(buf,"SYSERR: no valid target to act():%s # %d Testo = %s",GET_NAME(ch),GET_MOB_VNUM(ch), str);
		else{
			if(obj)
				sprintf(buf,"SYSERR: no valid target to act():Obj #%d Testo = %s",GET_OBJ_VNUM(obj), str);
			else
				sprintf(buf,"SYSERR: no valid target to act(): Testo = %s", str);
		}
		log(buf);
		return;
	}
	
	for (; to; to = to->next_in_room)
		if (SENDOK(to) && !(hide_invisible && ch && !CAN_SEE(to, ch)) &&
			(to != ch) && (type == TO_ROOM || (to != vict_obj)) &&
			!PLR_FLAGGED(to, PLR_FANTASMA))
			perform_act(str, ch, obj, vict_obj, to);
}

int count_chars(const char *txt, char character)
{
	int i, cnt = 0;
	
	for(i = 0; txt[i]; i++)
		if(txt[i] == character)
			cnt++;
		
	return cnt;
}

char *parse_sound(const char *txt)
{
	char *mspMus, *mspSou, *locIn, *locEnd;
	char *new_txt=NULL;
	char *toret=NULL;
	
	new_txt = strdup(txt);
	
	locIn = NULL;
	do
	{
		mspSou = strstr(new_txt,"!!SOUND(");
		mspMus = strstr(new_txt,"!!MUSIC(");
		if ((mspSou != NULL) && (mspMus == NULL))
		{ locIn = mspSou; }
		if ((mspSou == NULL) && (mspMus != NULL))
		{ locIn = mspMus; }
		if ((mspSou == NULL) && (mspMus == NULL))
		{ locIn = NULL; }
		if ((mspSou != NULL) && (mspMus != NULL))
		{
			if (mspSou < mspMus) { locIn = mspSou; }
			else { locIn = mspMus; }
		}
		if (locIn != NULL)
		{
			locEnd = strstr(locIn,")");
			if (locEnd == NULL)
			{
				locEnd = locIn + 7;
			}
			memmove(locIn,locEnd+1,strlen(locEnd));
		}
	}
	while (locIn != NULL);
	toret = strdup(new_txt);
	free (new_txt);
	
	return toret;
}

char *parse_color(const char *txt, struct descriptor_data *t)
{
	char *new_txt;
	char *toret;
	register int i, j = 0;
	
	
	i = count_chars(txt, '&'); /* count how many control-chars there
                                 are in the string */
	
	
	new_txt = malloc(i * 5 + strlen(txt) + 1); /* no ansi-escape code is larger
                                                 than 5 bytes so a 5 * times
                                                 the '&' appears + strlen(txt)
                                                 + 1 character big buffer
                                                 should be enough */
   /* the parser.. Huge but fast */
	for(i = 0; txt[i]; i++)
	{
		if(txt[i] == '&')
		{
			i++;
			
			switch(txt[i])
			{
				case '0' : strcpy(new_txt + j, CCNRM(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 4;
					break;
				case '1' : strcpy(new_txt + j, CCRED(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '2' : strcpy(new_txt + j, CCGRN(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '3' : strcpy(new_txt + j, CCYEL(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '4' : strcpy(new_txt + j, CCBLU(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '5' : strcpy(new_txt + j, CCMAG(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '6' : strcpy(new_txt + j, CCCYN(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '7' : strcpy(new_txt + j, CCWHT(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '8' : strcpy(new_txt + j, CCBLD(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 4;
					break;
				case '9' : strcpy(new_txt + j, CCBLK(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'u' : strcpy(new_txt + j, CCUND(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 4;
					break;
				case 'd' : strcpy(new_txt + j, CCDAR(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 4;
					break;
			  case 'f' : strcpy(new_txt + j, CCBLN(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 4;
					break;
				case 'b' : strcpy(new_txt + j, CCBLD(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 4;
					break;
					
				case 'R' : strcpy(new_txt + j, CCBRED(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'G' : strcpy(new_txt + j, CCBGRN(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'Y' : strcpy(new_txt + j, CCBYEL(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'B' : strcpy(new_txt + j, CCBBLU(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'M' : strcpy(new_txt + j, CCBMAG(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'C' : strcpy(new_txt + j, CCBCYN(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'W' : strcpy(new_txt + j, CCBWHT(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case 'S' : strcpy(new_txt + j, CCBBLK(t->character, C_NRM));
					if(clr(t->character, C_NRM))
						j += 5;
					break;
				case '&' : new_txt[j] = txt[i]; j++; break;
					
				default:  break;
			}
			
		}
		else
		{
			new_txt[j] = txt[i]; j++;
		}
	}
	
	new_txt[j] = '\0'; /* terminate the string */
	toret = strdup(new_txt); /* create a new string with no eventual memoryloss */
	free(new_txt); /* free the old buffer */
	
	return toret; /* the colorized buffer */
}

int update_max_usage(void)
{
	static int max_usage = 0;
	int sockets_playing = 0;
	struct descriptor_data *d;
	char buf[256];
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected) 
		  sockets_playing++;
		
	if (sockets_playing > max_usage) {
		max_usage = sockets_playing;
		sprintf(buf, "Nuovo massimo di giocatori dal reboot: %3d", max_usage);
		log(buf);
	}
	
	return max_usage;
}

void brag(struct char_data *ch, struct char_data *vict)
{
	
 /* Npc taunts slayed player characters.  Text goes out through gossip
    channel.  Muerte - Telnet://betterbox.net:4000                     */
	
	struct descriptor_data *i;
	char brag[256]={"       "};
	
	switch (number(0, 11)) {
		case 0:
			sprintf(brag, "$n urla, '%s e' stato troppo semplice da uccidere!'", GET_NAME(vict));
			break;
		case 1:
			sprintf(brag, "$n urla, '%s e' stato un buon pasto, ora chi c'e' per dessert? Muhaha!'", GET_NAME(vict));
			break;
		case 2:
			sprintf(brag, "$n urla, 'Bahaha! %s che il Mietitore vegli su di te!'",GET_NAME(vict));
			break;
		case 3:
			sprintf(brag, "$n urla, '%s penso che abbia bisogno di un po di exp ora...Muhaha!'", GET_NAME(vict));
			break;
		case 4:
			sprintf(brag, "$n urla, '%s necessita un ospedale ora. Muhaha!'",GET_NAME(vict));
			break;
		case 5:
			sprintf(brag, "$n urla, 'La madre di %s non ha fatto un gran lavoro!  Muhaha!'", GET_NAME(vict));
			break;
		case 6:
			sprintf(brag, "$n urla, '%s colpisce come una farfalla di palude. Bah.'", GET_NAME(vict));
			break;
		case 7:
			sprintf(brag, "$n brags, '%s, combattere contro una bambina sarebbe stato piu' impegnativo...Muahaha!'", GET_NAME(vict));
			break;
		case 8:
			sprintf(brag, "$n brags, 'Bah, %s torna a girare nella zona dei principianti!'",GET_NAME(vict));
			break;
		case 9:
			sprintf(brag, "$n brags, '%s, dammi il tuo indirizzo e magari rispedisco il tuo corpo a tua figlia.  Muhaha!'", GET_NAME(vict));
			break;
		case 10:
			sprintf(brag, "$n brags, 'Hey %s!  Torna qui, hai dimenticato il tuo corpo. Muahaha'", GET_NAME(vict));
			break;
		case 11:
			sprintf(brag, "$n brags, 'Penso che %s indossi mutandine rosa.  Combatte come una donnicciola!  Muhaha!'", GET_NAME(vict));
			break;
	}
	
	for (i = descriptor_list; i; i = i->next)
		if (!i->connected && i != ch->desc && i->character &&
			!PRF_FLAGGED(i->character, PRF_NOGOSS) &&
			!PLR_FLAGGED(i->character, PLR_WRITING) &&
			!ROOM_FLAGGED(i->character->in_room, ROOM_SOUNDPROOF))  {
			
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(CCRED(i->character, C_NRM), i->character);
			
			act(brag, FALSE, ch, 0, i->character, TO_VICT | TO_SLEEP);
			
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(CCNRM(i->character, C_NRM), i->character);
		}
}

// Riferirsi all'array frasi_move_status situato all'inizio di questo file
// Restituisce lo stato di forma della cavalcatura
int get_move_status(struct char_data * ch)
{
	int perc = 0;
	if (ch)
	{
		perc = (GET_MOVE(ch) * 100) / GET_MAX_MOVE(ch);
		if (perc<1)    return 1;
		if (perc<10)   return 2;
		if (perc<30)   return 3;
		if (perc<50)   return 4;
		if (perc<70)   return 5;
		if (perc<90)   return 6;
		if (perc<=100) return 7;
		return 8;
	}
	else return 0;
}


void mud_sound(int stype)
{
	struct descriptor_data *i;  
	extern char *itoa(int num,char *string);
	itoa(number(0,13),buf2);
	
	if (stype == 0){  
		for (i = descriptor_list; i; i = i->next)
			if (!i->connected && i->character &&
				IN_ROOM(i->character)!=NOWHERE)  {  
				switch (world[i->character->in_room].sector_type){
					case SECT_INSIDE:
					case SECT_CITY:
    //send_to_char("!!SOUND(gattino.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_FIELD:
						send_to_char("!!SOUND(grilli.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_FOREST:
						send_to_char("!!SOUND(foresta.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_HILLS:
						send_to_char("!!SOUND(cornacchia.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_MOUNTAIN:
						send_to_char("!!SOUND(lupo.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_WATER_SWIM:
						send_to_char("!!SOUND(mare1.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_WATER_NOSWIM:
						send_to_char("!!SOUND(mare1.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_UNDERWATER:
						send_to_char("!!SOUND(bolle.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_FLYING:
						send_to_char("!!SOUND(vento.wav L=1 P=1)\r\n",i->character);
						break;
					case SECT_ROAD:
						send_to_char("!!SOUND(foresta.wav L=1 P=1)\r\n",i->character);
						break;
					default:
						break;
				}
			}//for
	}else{
		for (i = descriptor_list; i; i = i->next)
			if (!i->connected && i->character &&
				IN_ROOM(i->character)!=NOWHERE)  { 
				switch (number(0,0)){ 
					case 0:
						sprintf(buf,"!!MUSIC(dag_%s.mid L=1 P=1)\r\n",buf2);
						send_to_char(buf,i->character);
						break;
					default:
						break;
				}
			} //for   
			
	}
}

/* Orione Inizio */
//Funzione per impostare il tipo di client usato dal pg
//prima versione: on/off tra client con mappa e normale
void change_client(struct char_data *ch) {
	if (str_cmp(GET_NAME(ch),"orione") && str_cmp(GET_NAME(ch),"amanu") && str_cmp(GET_NAME(ch), "wap") && \
	    str_cmp(GET_NAME(ch),"lucille") && str_cmp(GET_NAME(ch),"hiddukel") && str_cmp(GET_NAME(ch),"kizeridis") && 
		str_cmp(GET_NAME(ch),"faith")  && str_cmp(GET_NAME(ch),"phantom")) {
		send_to_char("Huh?!?\r\n", ch);
		return;
	}

	GET_CLIENT(ch) = (GET_CLIENT(ch)==0 ? 1 : 0);	
}
/* Orione Fine */
