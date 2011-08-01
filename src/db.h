/* ************************************************************************
*   File: db.h                                          Part of CircleMUD *
*  Usage: header file for database handling                               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* arbitrary constants used by index_boot() (must be unique) */
#define DB_BOOT_WLD	0
#define DB_BOOT_MOB	1
#define DB_BOOT_OBJ	2
#define DB_BOOT_ZON	3
#define DB_BOOT_SHP	4
#define DB_BOOT_HLP	5
#define DB_BOOT_TRG	6
#define MAX_SPEC        48
/* names of various files and directories */
#define INDEX_FILE	"index"		/* index of world files		*/
#define MINDEX_FILE	"index.mini"	/* ... and for mini-mud-mode	*/
#define WLD_PREFIX	"world/wld"	/* room definitions		*/
#define MOB_PREFIX	"world/mob"	/* monster prototypes		*/
#define OBJ_PREFIX	"world/obj"	/* object prototypes		*/
#define ZON_PREFIX	"world/zon"	/* zon defs & command tables	*/
#define SHP_PREFIX	"world/shp"	/* shop definitions		*/
#define TRG_PREFIX	"world/trg"	/* for script triggers          */
#define HLP_PREFIX	"text/help"	/* for HELP <keyword>		*/

#define CREDITS_FILE	"text/credits"	/* for the 'credits' command	*/
#define NEWS_FILE	"text/news"	/* for the 'news' command	*/
#define MOTD_FILE	"text/motd"	/* messages of the day / mortal	*/
#define IMOTD_FILE	"text/imotd"	/* messages of the day / immort	*/
#define HELP_PAGE_FILE	"text/help/screen" /* for HELP <CR>		*/
#define INFO_FILE	"text/info"	/* for INFO			*/
#define WIZLIST_FILE	"text/wizlist"	/* for WIZLIST			*/
#define IMMLIST_FILE	"text/immlist"	/* for IMMLIST			*/
#define BACKGROUND_FILE	"text/background" /* for the background story	*/
#define POLICIES_FILE	"text/policies"	/* player policies/rules	*/
#define HANDBOOK_FILE	"text/handbook"	/* handbook for new immorts	*/
#define REGNI_FILE	"text/regni"	/* handbook for new immorts	*/


#define IDEA_FILE	"misc/ideas"	/* for the 'idea'-command	*/
#define TYPO_FILE	"misc/typos"	/*         'typo'		*/
#define BUG_FILE	"misc/bugs"	/*         'bug'		*/
#define MESS_FILE	"misc/messages"	/* damage messages		*/
#define SOCMESS_FILE	"misc/socials"	/* messgs for social acts	*/
#define XNAME_FILE	"misc/xnames"	/* invalid name substrings	*/
#define PRAY_FILE       "misc/pray"
#define PLAYER_FILE	"etc/players"	   /* the player database	   */
#define OLD_PLAYER_FILE "etc/old_players"  // Orione - Old Players Database
#define REN_PLAYER_FILE "etc/ren_players"  // Orione - Player da Reinserire
#define MOBS_FILE	"etc/mobs"	   /* the mobs database		   */
#define MAIL_FILE	"etc/plrmail"	   /* for the mudmail system	   */
#define BAN_FILE	"etc/badsites"	   /* for the siteban system	   */
#define UNLOCKED_IP_FILE "etc/unlockedip"  /* gli ip uiversitari sblocati  */
#define HCONTROL_FILE	"etc/hcontrol"     /* for the house system	   */
#define CLAN_FILE       "etc/clans"        /*  Regni e Clan                */
#define TIME_FILE       "etc/time_passed" /*MUD Time since last Crash/reboot*/
#define NEW_CLAN_FILE       "etc/new_clans"        /*  Gilde   */
#define REWARD_FILE     "reward/"

/* public procedures in db.c */
void	boot_db(void);
int	create_entry(char *name);
void	zone_update(void);
room_rnum real_room(room_vnum virtual);
char	*fread_string(FILE *fl, char *error);
long	get_id_by_name(char *name);
char	*get_name_by_id(long id);
int     count_hash_records(FILE * fl);

void	char_to_store(struct char_data *ch, struct char_file_u *st);
void	store_to_char(struct char_file_u *st, struct char_data *ch);
int	load_char(char *name, struct char_file_u *char_element);
void	save_char(struct char_data *ch, room_rnum load_room);
void	init_char(struct char_data *ch);
struct char_data* create_char(void);
struct char_data *read_mobile(int nr, int type);
int	real_mobile(int virtual);
int	vnum_mobile(char *searchname, struct char_data *ch);
void	clear_char(struct char_data *ch);
void	reset_char(struct char_data *ch);
void	free_char(struct char_data *ch);

//new adriano
void	mob_to_store(struct char_data *ch, struct smob_file_u *st);
void	store_to_mob(struct smob_file_u *st, struct char_data *ch);
long	load_mob(long mob_i, struct smob_file_u *char_element);
void	save_mob(struct char_data *ch, room_rnum load_room);
long	get_idmob_by_name(char *name);
char	*get_name_by_idmob(long id);
int	create_entrymob(char *name);


struct obj_data *create_obj(void);
void	clear_object(struct obj_data *obj);
void	free_obj(struct obj_data *obj);
int	real_object(int virtual);
struct obj_data *read_object(int nr, int type);
int	vnum_object(char *searchname, struct char_data *ch);
int find_flag(struct char_data *vict, int regno, int FLAG);

int sunlight;	/* What state the sun is at */

#define REAL 0
#define VIRTUAL 1

/* structure for the reset commands */
struct reset_com {
   char	command;   /* current command                      */

   bool if_flag;	/* if TRUE: exe only if preceding exe'd */
   int	arg1;		/*                                      */
   int	arg2;		/* Arguments to the command             */
   int	arg3;		/*                                      */
   int line;		/* line number this command appears on  */

   /*
	*  Commands:              *
	*  'M': Read a mobile     *
	*  'O': Read an object    *
	*  'G': Give obj to mob   *
	*  'P': Put obj in obj    *
	*  'G': Obj to char       *
	*  'E': Obj to char equip *
	*  'D': Set state of door *
   */
};



/* zone definition structure. for the 'zone-table'   */
struct zone_data {
   char	*name;		    /* name of this zone                  */
   int	lifespan;           /* how long between resets (minutes)  */
   int	age;                /* current age of this zone (minutes) */
   int	top;                /* upper limit for rooms in this zone */

   int	reset_mode;         /* conditions for reset (see below)   */
   int	number;		    /* virtual number of this zone	  */
   struct reset_com *cmd;   /* command table for reset	          */
   int  pressure;	    /* How is the pressure ( Mb ) */
   int  change;		    /* How fast and what way does it change. */
   int  sky;		    /* How is the sky. */
   int  wilderness;         /* E' wilderness o miniwild ??? */
   room_vnum miniwild_exit[4]; /* Sono le uscite della miniwild n e s w */

   /*
	*  Reset mode:                              *
	*  0: Don't reset, and don't update age.    *
	*  1: Reset if no PC's are located in zone. *
	*  2: Just reset.                           *
   */
};



/* for queueing zones for update   */
struct reset_q_element {
   int	zone_to_reset;            /* ref to zone_data */
   struct reset_q_element *next;
};



/* structure for the update queue     */
struct reset_q_type {
   struct reset_q_element *head;
   struct reset_q_element *tail;
};



struct player_index_element {
   char	*name;
   int level;
   long id;
};

struct mob_index_element {
 	char	*name;
 	int level;
 	long idmob;
 	long vnum;
};

struct help_index_element {
   char	*keyword;
   char *entry;
   int duplicate;
};


/* don't change these */
#define BAN_NOT 	0
#define BAN_NEW 	1
#define BAN_SELECT	2
#define BAN_ALL		3

#define BANNED_SITE_LENGTH    50
struct ban_list_element {
   char	site[BANNED_SITE_LENGTH+1];
   int	type;
   time_t date;
   char	name[MAX_NAME_LENGTH+1];
   struct ban_list_element *next;
};

/* Orione Inizio */
#define UNLOCKED_SITE_LENGTH	50
struct unlockedip_list_element {
   char ip_address[UNLOCKED_SITE_LENGTH+1];	//Indirizzo IP che consente connessioni multiple
   int max_users;			//Massimo numero di utenti che possono connettersi con l'IP indicato
   time_t unlock_date;			//Data dell'ultima modifica di questo record
   char god_name[MAX_NAME_LENGTH+1];	//Nome del creatore che ha aggiunto questo record
   struct unlockedip_list_element *next;	//Puntatore all'elemento seguente della lista
};
/* Orione Fine */

/* global buffering system */

#ifdef __DB_C__
char	buf[MAX_STRING_LENGTH];
char	buf1[MAX_STRING_LENGTH];
char	buf2[MAX_STRING_LENGTH];
char	arg[MAX_STRING_LENGTH];
#else
extern char	buf[MAX_STRING_LENGTH];
extern char	buf1[MAX_STRING_LENGTH];
extern char	buf2[MAX_STRING_LENGTH];
extern char	arg[MAX_STRING_LENGTH];
#endif

#ifndef __CONFIG_C__
extern char	*OK;
extern char	*NOPERSON;
extern char	*NOEFFECT;
#endif

