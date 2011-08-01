
/* ************************************************************************
*   File: db.c                                          Part of CircleMUD *
*  Usage: Loading/saving chars, booting/resetting world, internal funcs   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __DB_C__

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "handler.h"
#include "spells.h"
#include "mail.h"
#include "auction.h"
#include "interpreter.h"
#include "house.h"
#include "dg_scripts.h"
#include "wilderness.h"
#include "shop.h"
#include "mestieri.h"
#include "clan.h"
#include "eserciti.h"

/**************************************************************************
*  declarations of most of the 'global' variables                         *
************************************************************************ */

struct room_data *world = NULL;	/* array of rooms		*/
int top_of_world = 0;		/* ref to top element of world	*/

struct char_data *character_list = NULL; /* global linked list of chars */

struct index_data **trig_index; /* index table for triggers      */
int top_of_trigt = 0;           /* top of trigger index table    */
long max_id = 100000;           /* for unique mob/obj id's       */

struct index_data *mob_index;	/* index table for mobile file	*/
struct char_data *mob_proto;	/* prototypes for mobs		*/
int top_of_mobt = 0;		/* top of mobile index table	*/

struct obj_data *object_list = NULL;	/* global linked list of objs	*/
struct index_data *obj_index;	/* index table for object file	*/
struct obj_data *obj_proto;	/* prototypes for objs		*/
int top_of_objt = 0;		/* top of object index table	*/

struct zone_data *zone_table;	/* zone table			*/
int top_of_zone_table = 0;	/* top element of zone tab	*/
struct message_list fight_messages[MAX_MESSAGES];	/* fighting messages	*/

struct player_index_element *player_table = NULL;	/* index to plr file	*/
FILE *player_fl = NULL;		/* file desc of player file	*/
int top_of_p_table = 0;		/* ref to top of table		*/
int top_of_p_file = 0;		/* ref of size of p file	*/
long top_idnum = 0;		/* highest idnum in use		*/

int no_mail = 0;		/* mail disabled?		*/
int mini_mud = 0;		/* mini-mud mode?		*/
int no_rent_check = 0;		/* skip rent check on boot?	*/
time_t boot_time = 0;		/* time of mud boot		*/
time_t tempo_totale = 0;	/* tempo totale trascorso dal 21 settembre 2004	*/
int restrict = 0;		/* level of game restriction	*/
room_rnum r_mortal_start_room[NUM_STARTROOMS +1];	/* rnum of mortal start room	*/
room_rnum r_immort_start_room;	/* rnum of immort start room	*/
room_rnum r_frozen_start_room;	/* rnum of frozen start room	*/

char *credits = NULL;		/* game credits			*/
char *news = NULL;		/* mud news			*/
char *motd = NULL;		/* message of the day - mortals */
char *imotd = NULL;		/* message of the day - immorts */
char *help = NULL;		/* help screen			*/
char *info = NULL;		/* info page			*/
char *wizlist = NULL;		/* list of higher gods		*/
char *immlist = NULL;		/* list of peon gods		*/
char *background = NULL;	/* background story		*/
char *handbook = NULL;		/* handbook for new immortals	*/
char *policies = NULL;		/* policies page		*/
char *pray = NULL;
char *regni = NULL;

struct help_index_element *help_table = 0;	/* the help table	*/
int top_of_helpt = 0;		/* top of help index table	*/

struct time_info_data time_info;/* the infomation about the time    */
/*struct weather_data weather_info;  */
struct player_special_data dummy_mob;	/* dummy spec area for mobs	*/
struct reset_q_type reset_q;	/* queue of zones to be reset	*/

/*Adriano save mobs*/
struct mob_index_element *mobs_table = NULL;	/* index to mobs file	*/
FILE *mobs_fl = NULL;		/* file desc of mob file	*/
int top_of_m_table = 0;		/* ref to top of table		*/
int top_of_m_file = 0;		/* ref of size of mob file	*/
long top_idmobnum = 0;		/* highest idmobnum in use		*/
void Load_Savemob(void);

/*Criminali*/
FILE *reward_fl = NULL;
void loadreward(struct char_data *vict);
void savereward(struct char_data *vict);
void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia);
int del_crime(struct char_data *vict, int num);
int del_crime_flag(struct char_data *ch, struct char_data *vict, int regno, int Dflag);

/* local functions */
void setup_dir(FILE * fl, int room, int dir);
void index_boot(int mode);
void discrete_load(FILE * fl, int mode);
void parse_room(FILE * fl, int virtual_nr);
void parse_mobile(FILE * mob_f, int nr);
char *parse_object(FILE * obj_f, int nr);
void load_zones(FILE * fl, char *zonename);
void load_help(FILE *fl);
void assign_mobiles(void);
void assign_objects(void);
void assign_rooms(void);
void assign_the_shopkeepers(void);
void build_player_index(void);
void char_to_store(struct char_data * ch, struct char_file_u * st);
void store_to_char(struct char_file_u * st, struct char_data * ch);
int is_empty(int zone_nr);
void reset_zone(int zone);
int file_to_string(char *name, char *buf);
int file_to_string_alloc(char *name, char **buf);
void check_start_rooms(void);
void renum_world(void);
void renum_zone_table(void);
void log_zone_error(int zone, int cmd_no, char *message);
void reset_time(void);
void clear_char(struct char_data * ch);
int num_obj_in_zone (obj_rnum obj_num, int zone_num);
int num_obj_in_room (obj_rnum obj_num, room_rnum room);
int num_obj_in_obj (obj_rnum obj_num, struct obj_data * object);
void build_mob_index(void);
void mob_to_store(struct char_data * ch, struct smob_file_u * st);
void store_to_mob(struct smob_file_u * st, struct char_data * ch);
void random_mobiles(void);
void Pack_mob(void);

/* external functions */
extern struct descriptor_data *descriptor_list;
extern struct auction_data auction;
extern void interpret_sproc(int rnum,int nspec);
void load_messages(void);
void weather_and_time(int mode);
void mag_assign_spells(void);
void boot_social_messages(void);
void update_obj_file(void);	/* In objsave.c */
void sort_commands(void);
void sort_spells(void);
void load_banned(void);
void load_unlockedip_list(void); //Orione
void Read_Invalid_List(void);
void boot_the_shops(FILE * shop_f, char *filename, int rec_count);
void quest_update(void);
int hsort(const void *a, const void *b);
void add_var(struct trig_var_data **var_list, char *name, char *value);
int Crash_MobEqLoad(struct char_data * ch, room_vnum save_room);
void Crash_MobEqSave(struct char_data * ch, room_vnum save_room);
extern void load_corpse(void);
/* external vars */
extern int no_specials;
extern void load_shop();
extern void reset_risorse(int what);
extern void Elimina_Cantieri();
void clear_noplayer_pg();
void old_player_restore(); //Orione
extern void new_init_clans();
extern void lettura_teste();
void boot_hiscore (void);
void boot_tempeste(void);
void boot_titolanti(void);

#define READ_SIZE 256

/*Adriano*/
void times_events_ini (struct char_data *ch);

/*************************************************************************
*  routines for booting the system                                       *
*********************************************************************** */

/* this is necessary for the autowiz system */
void reboot_wizlists(void)
{
	file_to_string_alloc(WIZLIST_FILE, &wizlist);
	file_to_string_alloc(IMMLIST_FILE, &immlist);
}


ACMD(do_reboot)
{
	int i;
	
	one_argument(argument, arg);
	
	if (!str_cmp(arg, "all") || *arg == '*') {
		file_to_string_alloc(WIZLIST_FILE, &wizlist);
		file_to_string_alloc(IMMLIST_FILE, &immlist);
		file_to_string_alloc(NEWS_FILE, &news);
		file_to_string_alloc(CREDITS_FILE, &credits);
		file_to_string_alloc(MOTD_FILE, &motd);
		file_to_string_alloc(PRAY_FILE, &pray);
		file_to_string_alloc(IMOTD_FILE, &imotd);
		file_to_string_alloc(HELP_PAGE_FILE, &help);
		file_to_string_alloc(INFO_FILE, &info);
		file_to_string_alloc(POLICIES_FILE, &policies);
		file_to_string_alloc(HANDBOOK_FILE, &handbook);
		file_to_string_alloc(REGNI_FILE, &regni);
		
		file_to_string_alloc(BACKGROUND_FILE, &background);
	} else if (!str_cmp(arg, "wizlist"))
		file_to_string_alloc(WIZLIST_FILE, &wizlist);
	else if (!str_cmp(arg, "immlist"))
		file_to_string_alloc(IMMLIST_FILE, &immlist);
	else if (!str_cmp(arg, "news"))
		file_to_string_alloc(NEWS_FILE, &news);
	else if (!str_cmp(arg, "credits"))
		file_to_string_alloc(CREDITS_FILE, &credits);
	else if (!str_cmp(arg, "motd"))
		file_to_string_alloc(MOTD_FILE, &motd);
	else if (!str_cmp(arg, "imotd"))
		file_to_string_alloc(IMOTD_FILE, &imotd);
	else if (!str_cmp(arg, "help"))
		file_to_string_alloc(HELP_PAGE_FILE, &help);
	else if (!str_cmp(arg, "info"))
		file_to_string_alloc(INFO_FILE, &info);
	else if (!str_cmp(arg, "policy"))
		file_to_string_alloc(POLICIES_FILE, &policies);
	else if (!str_cmp(arg, "handbook"))
		file_to_string_alloc(HANDBOOK_FILE, &handbook);
	else if (!str_cmp(arg, "regni"))
		file_to_string_alloc(REGNI_FILE, &regni);
	else if (!str_cmp(arg, "background"))
		file_to_string_alloc(BACKGROUND_FILE, &background);
	else if (!str_cmp(arg, "xhelp")) {
		if (help_table) {
			for (i = 0; i <= top_of_helpt; i++) {
				if (help_table[i].keyword)
					free(help_table[i].keyword);
				if (help_table[i].entry && !help_table[i].duplicate)
					free(help_table[i].entry);
			}
			free(help_table);
		}
		top_of_helpt = 0;
		index_boot(DB_BOOT_HLP);
	} else {
		send_to_char("Unknown reload option.\r\n", ch);
		return;
	}
	
	send_to_char(OK, ch);
}


void boot_world(void)
{
	log("Loading zone table.");
	index_boot(DB_BOOT_ZON);
	
	log("Loading triggers and generating index.");
	index_boot(DB_BOOT_TRG);
	
	
	log("Loading rooms.");
	index_boot(DB_BOOT_WLD);
	
	log("Loading wilderness.");
	wilderness_boot();
	
	log("Renumbering rooms.");
	renum_world();
	
	log("Checking start rooms.");
	check_start_rooms();
	
	log("Loading mobs and generating index.");
	index_boot(DB_BOOT_MOB);
	
	log("Loading objs and generating index.");
	index_boot(DB_BOOT_OBJ);
	
	log("Renumbering zone table.");
	renum_zone_table();
	
	if (!no_specials) {
		log("Loading shops.");
		index_boot(DB_BOOT_SHP);
		load_shop();
	}
}



/* body of the booting system */
void boot_db(void)
{
	int i;
	void auction_reset();
	
	log("Boot db -- BEGIN.");
	
	log("Resetting the game time:");
	reset_time();
	
	log("Reading news, credits, help, bground, info & motds.");
	file_to_string_alloc(NEWS_FILE, &news);
	file_to_string_alloc(CREDITS_FILE, &credits);
	file_to_string_alloc(MOTD_FILE, &motd);
	file_to_string_alloc(IMOTD_FILE, &imotd);
	file_to_string_alloc(PRAY_FILE, &pray);
	file_to_string_alloc(HELP_PAGE_FILE, &help);
	file_to_string_alloc(INFO_FILE, &info);
	file_to_string_alloc(WIZLIST_FILE, &wizlist);
	file_to_string_alloc(IMMLIST_FILE, &immlist);
	file_to_string_alloc(POLICIES_FILE, &policies);
	file_to_string_alloc(HANDBOOK_FILE, &handbook);
	file_to_string_alloc(REGNI_FILE, &regni);
	file_to_string_alloc(BACKGROUND_FILE, &background);
	
	log("Boot World");
	boot_world();
	
	log("Loading help entries.");
	index_boot(DB_BOOT_HLP);
	
	log("Generating player index.");
	build_player_index();
	
	log("Generating mob index.");
	build_mob_index();
	
	log("Loading fight messages.");
	load_messages();
	
	log("Loading social messages.");
	boot_social_messages();
	
	log("Assigning function pointers:");	
	if (!no_specials) {
		log("   Mobiles.");
		assign_mobiles();
		log("   Shopkeepers.");
		assign_the_shopkeepers();
		log("   Objects.");
		assign_objects();
		log("   Rooms.");
		assign_rooms();
	}
	log("   Spells.");
	mag_assign_spells();
	
	log("Assigning spell and skill levels.");
	init_spell_levels();
	
	log("Assigning abilita.");
	init_abilita();
	
	log("Auction system reset.");
	auction_reset();
	auction.auctioneer = NULL;
	
	log("Sorting command list and spells.");
	sort_commands();
	sort_spells();
	
	log("Booting mail system.");
	if (!scan_file()) {
		log("    Mail boot failed -- Mail system disabled");
		no_mail = 1;
	}
	
	log("Reading banned site and invalid-name list and unlocked ip.");
	load_banned();
	Read_Invalid_List();
	load_unlockedip_list(); //Orione

	if (!no_rent_check) {
		log("Deleting timed-out crash and rent files:");
		update_obj_file();
		log("Done.");
	}
	
	log("RESET ZONE:");
	for (i = 0; i <= top_of_zone_table; i++) {
		sprintf(buf2, "   Reset %s (rooms %d-%d)",
			zone_table[i].name, (i ? (zone_table[i - 1].top + 1) : 0),
			zone_table[i].top);
		log(buf2);
		reset_zone(i);
	}
	
	reset_q.head = reset_q.tail = NULL;
	
	if (!mini_mud) {
		log("Booting houses.");
		House_boot();
	}
	
	log("Booting clans.");
	init_clans();
	
	log("Booting Gilde.");
	new_init_clans();

	log("Reset Boot Time");
	boot_time = time(0);
	
	log("Booting oggetti dei negozi");
	shop_crash_load();
	
	log("Loading saved mobs.");
	Load_Savemob();
	
	log("Creazione cadaveri:");
	load_corpse();

	log("Booting hiscore:");
	boot_hiscore();

	log("Booting tempeste:");
	boot_tempeste();
	
	log("Boot eserciti:");
	boot_eserciti();

	log("Booting titolanti:");
	boot_titolanti();
	
	log("Booting random mobiles");
	random_mobiles();
	
	log("Reset risorse");
	reset_risorse(RES_PIETRE);
	reset_risorse(RES_ALBERI); 
	reset_risorse(RES_GEMME);	
	reset_risorse(RES_ROCCE);
	reset_risorse(RES_NATURA);	
	reset_risorse(RES_ERBE);
	reset_risorse(RES_SELVAGGINA);	
	reset_risorse(RES_AMMAESTRABILI);		
	
	log("Player Deleted:");
	clear_noplayer_pg();
	
	log("Riunione Player:");
	old_player_restore();
	
	log("elimina cantieri:");
	Elimina_Cantieri();

	log("Boot db -- DONE.");
}


/* reset the time in the game from file */
void reset_time(void)
{
	FILE *fd;
  //long beginning_of_time = 650336715;
	long beginning_of_time,tm=time(0);
	int  starting_year=0;
	int year;
	struct time_info_data mud_time_passed(time_t t2, time_t t1,int since_year);
	if(!(fd=fopen(TIME_FILE,"r"))) {
		sprintf(buf,"File '%s' not exist,attempting to create it\n",TIME_FILE);
		log(buf);
		if (!(fd = fopen(TIME_FILE, "wb"))) {
			log("SYSERR: Unable to open Time_passed file");
			return;
		}
     //fd=fopen(TIME_FILE,"wb");
		fprintf(fd,"%d %d %d %d %d %ld %ld %s",0,0,0,0,STARTING_YEAR,tm,tm,ctime(&tm));
		fclose(fd);
	}
	fd=fopen(TIME_FILE,"r");
	fscanf(fd,"%d %d %d %d %d %ld %ld",&time_info.hours,&time_info.day,&time_info.month,&year,&starting_year,&beginning_of_time,&tm);
   /* fd=fopen(TIME_FILE,"w");
      fprintf(fd,"%d %d",beginning_of_time,(long) time(0) );*/
	fclose(fd);
	time_info.year=year+starting_year;
   //time_info = mud_time_passed(time(0), beginning_of_time);
   //time_info = mud_time_passed(0, beginning_of_time,starting_year);
	if (time_info.hours <= 4)
		sunlight = SUN_DARK;
	else if (time_info.hours == 5)
		sunlight = SUN_DARK; //SUN_RISE;
	else if (time_info.hours <= 20)
		sunlight = SUN_DARK; //SUN_LIGHT;
	else if (time_info.hours == 21)
		sunlight = SUN_DARK; //SUN_SET;
	else
		sunlight = SUN_DARK;
	
	sprintf(buf, "   Current Gametime: %dH %dD %dM %dY.", time_info.hours,
		time_info.day, time_info.month, time_info.year);
	log(buf);
	sprintf(buf,"   Last Savetime :%s",ctime(&tm));
	log(buf);
}


void save_time(){
	FILE *fd;
	int t,starting_year,real_year=0;
	long the_begin,tm;
	if (!(fd = fopen(TIME_FILE, "r"))) {
		log("SYSERR: Unable to open Time_passed file,creating it");
	}
	else{
		fscanf(fd,"%d %d %d %d %d %ld %ld",&t,&t,&t,&t,&starting_year,&the_begin,&tm);
		fclose(fd);
	}
	tm=time(0);
	if (!(fd = fopen(TIME_FILE, "wb"))) {
		log("SYSERR: Unable to open Time_passed file,creating it");
	} else {
		real_year=(int) time_info.year-STARTING_YEAR;
		fprintf(fd,"%d %d %d %d %d %ld %ld %s",time_info.hours,time_info.day,time_info.month,real_year,STARTING_YEAR,the_begin,tm,ctime(&tm));
		fclose(fd);
	}
}



/* generate index table for the player file */
void build_player_index(void)
{
	int nr = -1, i;
	long size, recs;
	struct char_file_u dummy;
	
	if (!(player_fl = fopen(PLAYER_FILE, "r+b"))) {
		if (errno != ENOENT) {
			perror("fatal error opening playerfile");
			exit(1);
		} 
		else {
			log("No playerfile.  Creating a new one.");
			touch(PLAYER_FILE);
			if (!(player_fl = fopen(PLAYER_FILE, "r+b"))) {
				perror("fatal error opening playerfile");
				exit(1);
			}
		}
	}
	
	fseek(player_fl, 0L, SEEK_END);
	size = ftell(player_fl);
	rewind(player_fl);

	sprintf(buf, "   Compatibilita' dim. in byte: \n\r"
                 "     int    %d  sh_int %d  ush_int       %d \n\r"
                 "     byte   %d  ubyte  %d  sbyte         %d  char  %d \n\r"
                 "     time_t %d  long   %d  long long int %d  \n\r",
                 sizeof(int), sizeof(sh_int), sizeof(ush_int),
                 sizeof(byte), sizeof(ubyte), sizeof(sbyte), sizeof(char),
                 sizeof(time_t), sizeof(long), sizeof(long long int) );
	log(buf);

	sprintf(buf, "   PLAYER File %ld RecordSize %d ", size, sizeof(struct char_file_u));
	log(buf);
	
	if (size % sizeof(struct char_file_u))
		fprintf(stderr, "\aWARNING:  PLAYERFILE IS PROBABLY CORRUPT!\n");
	
	recs = size / sizeof(struct char_file_u);
	
	if (recs) {
		sprintf(buf, "   %ld players in database.", recs);
		log(buf);
		CREATE(player_table, struct player_index_element, recs);
	}
	else {
		player_table = NULL;
		top_of_p_file = top_of_p_table = -1;
		return;
	}
	
	for (; !feof(player_fl);) {
		fread(&dummy, sizeof(struct char_file_u), 1, player_fl);
		if (!feof(player_fl)) {	/* new record */
			nr++;
			CREATE(player_table[nr].name, char, strlen(dummy.name) + 1);
			for (i = 0; (*(player_table[nr].name + i) = LOWER(*(dummy.name + i))); i++);
			player_table[nr].id = dummy.char_specials_saved.idnum;
			player_table[nr].level= dummy.level;
			top_idnum = MAX(top_idnum, dummy.char_specials_saved.idnum);
		}
	}
	
	top_of_p_file = top_of_p_table = nr;
}



/* function to count how many hash-mark delimited records exist in a file */
int count_hash_records(FILE * fl)
{
	char buf[128];
	int count = 0;
	
	while (fgets(buf, 128, fl))
		if (*buf == '#')
			count++;
		
	return count;
}



void index_boot(int mode)
{
	char *index_filename, *prefix;
	FILE *index, *db_file;
	int rec_count = 0;
	
	switch (mode) {
		case DB_BOOT_TRG:
			prefix = TRG_PREFIX;
			break;
		case DB_BOOT_WLD:
			prefix = WLD_PREFIX;
			break;
		case DB_BOOT_MOB:
			prefix = MOB_PREFIX;
			break;
		case DB_BOOT_OBJ:
			prefix = OBJ_PREFIX;
			break;
		case DB_BOOT_ZON:
			prefix = ZON_PREFIX;
			break;
		case DB_BOOT_SHP:
			prefix = SHP_PREFIX;
			break;
		case DB_BOOT_HLP:
			prefix = HLP_PREFIX;
			break;
		default:
			log("SYSERR: Unknown subcommand to index_boot!");
			exit(1);
			break;
	}
	
	if (mini_mud)
		index_filename = MINDEX_FILE;
	else
		index_filename = INDEX_FILE;
	
	sprintf(buf2, "%s/%s", prefix, index_filename);
	
	if (!(index = fopen(buf2, "r"))) {
		sprintf(buf1, "Error opening index file '%s'", buf2);
		perror(buf1);
		exit(1);
	}
	
  /* first, count the number of records in the file so we can malloc */
	fscanf(index, "%s\n", buf1);
	while (*buf1 != '$') {
		sprintf(buf2, "%s/%s", prefix, buf1);
		if (!(db_file = fopen(buf2, "r"))) {
			perror(buf2);
			log("file listed in index not found");
			exit(1);
		} else {
			if (mode == DB_BOOT_ZON)
				rec_count++;
			else
				rec_count += count_hash_records(db_file);
		}
		
		fclose(db_file);
		fscanf(index, "%s\n", buf1);
	}
	
  /* Exit if 0 records, unless this is shops */
	if (!rec_count) {
		if ((mode == DB_BOOT_SHP)||(mode==DB_BOOT_TRG))
			return;
		log("SYSERR: boot error - 0 records counted");
		exit(1);
	}
	
	rec_count++;
	
	switch (mode) {
		case DB_BOOT_TRG:
			CREATE(trig_index, struct index_data *, rec_count);
			break;
		case DB_BOOT_WLD:
			CREATE(world, struct room_data, rec_count);
			break;
		case DB_BOOT_MOB:
			CREATE(mob_proto, struct char_data, rec_count);
			CREATE(mob_index, struct index_data, rec_count);
			break;
		case DB_BOOT_OBJ:
			CREATE(obj_proto, struct obj_data, rec_count + 1);
			CREATE(obj_index, struct index_data, rec_count + 1);
			break;
		case DB_BOOT_ZON:
			CREATE(zone_table, struct zone_data, rec_count);
			break;
		case DB_BOOT_HLP:
			CREATE(help_table, struct help_index_element, rec_count * 2);
			break;
	}
	
	rewind(index);
	fscanf(index, "%s\n", buf1);
	while (*buf1 != '$') {
		sprintf(buf2, "%s/%s", prefix, buf1);
		if (!(db_file = fopen(buf2, "r"))) {
			perror(buf2);
			exit(1);
		}
		switch (mode) {
			case DB_BOOT_TRG:
			case DB_BOOT_WLD:
			case DB_BOOT_OBJ:
			case DB_BOOT_MOB:
				discrete_load(db_file, mode);
				break;
			case DB_BOOT_ZON:
				load_zones(db_file, buf2);
				break;
			case DB_BOOT_HLP:
				load_help(db_file);
				break;
			case DB_BOOT_SHP:
				boot_the_shops(db_file, buf2, rec_count);
				break;
		}
		
		fclose(db_file);
		fscanf(index, "%s\n", buf1);
	}
	
  /* sort the help index */
	if (mode == DB_BOOT_HLP) {
		qsort(help_table, top_of_helpt, sizeof(struct help_index_element), hsort);
		top_of_helpt--;
	}
	
}


void discrete_load(FILE * fl, int mode)
{
	int nr = -1, last = 0;
	char line[256];
	
	char *modes[] = {"world", "mob", "obj", "ZON", "SHP", "HLP", "trg"};
  /* modes positions correspond to DB_BOOT_xxx in db.h */
	
	for (;;) {
    /*
     * we have to do special processing with the obj files because they have
     * no end-of-record marker :(
     */
		if (mode != DB_BOOT_OBJ || nr < 0)
			if (!get_line(fl, line)) {
				fprintf(stderr, "Format error after %s #%d\n", modes[mode], nr);
				exit(1);
			}
		if (*line == '$')
			return;
		
		if (*line == '#') {
			last = nr;
			if (sscanf(line, "#%d", &nr) != 1) {
				fprintf(stderr, "Format error after %s #%d\n", modes[mode], last);
				exit(1);
			}
			if (nr >= 1999999)
				return;
			else
				switch (mode) {
					case DB_BOOT_TRG:
						parse_trigger(fl, nr);
						break;
					case DB_BOOT_WLD:
						parse_room(fl, nr);
						break;
					case DB_BOOT_MOB:
						parse_mobile(fl, nr);
						break;
					case DB_BOOT_OBJ:
						strcpy(line, parse_object(fl, nr));
						break;
				}
		} else {
			fprintf(stderr, "Format error in %s file near %s #%d\n",
				modes[mode], modes[mode], nr);
			fprintf(stderr, "Offending line: '%s'\n", line);
			exit(1);
		}
	}
}


long long int asciiflag_conv(char *flag)
{
	long long int flags = 0;
	int is_number = 1;
	register char *p;
	
	for (p = flag; *p; p++) {
		if (islower(*p))
			flags |= 1 << (*p - 'a');
		else if (isupper(*p))
			flags |= 1 << (26 + (*p - 'A'));
		
		if (!isdigit(*p))
			is_number = 0;
	}
	
	if (is_number)
		flags = atolonglong(flag);
	
	return flags;
}

char fread_letter(FILE *fp)
{
	char c;
	do {
		c = getc(fp);
	} while (isspace(c));
	return c;
}

/* load the rooms */
void parse_room(FILE * fl, int virtual_nr)
{
	static int room_nr = 0, zone = 0;
	int t[10], i;
	char line[256], flags[128];
	struct extra_descr_data *new_descr;
	char letter;
	sprintf(buf2, "room #%d", virtual_nr);
	
	if (virtual_nr <= (zone ? zone_table[zone - 1].top : -1)) {
		fprintf(stderr, "Room #%d is below zone %d.\n", virtual_nr, zone);
		exit(1);
	}
	while (virtual_nr > zone_table[zone].top)
		if (++zone > top_of_zone_table) {
			fprintf(stderr, "Room %d is outside of any zone.\n", virtual_nr);
			exit(1);
		}
	world[room_nr].zone = zone;
	world[room_nr].number = virtual_nr;
	world[room_nr].name = fread_string(fl, buf2);
	world[room_nr].description = fread_string(fl, buf2);
	
	if (!get_line(fl, line) || sscanf(line, " %d %s %d ", t, flags, t + 2) != 3) {
		fprintf(stderr, "Format error in room #%d\n", virtual_nr);
		exit(1);
	}
  /* t[0] is the zone number; ignored with the zone-file system */
	world[room_nr].room_flags = asciiflag_conv(flags);
	world[room_nr].sector_type = t[2];
	
	world[room_nr].func = NULL;
	world[room_nr].contents = NULL;
	world[room_nr].people = NULL;
	world[room_nr].light = 0;	/* Zero light sources */
	
	for (i = 0; i < NUM_OF_DIRS; i++)
		world[room_nr].dir_option[i] = NULL;
	
	world[room_nr].ex_description = NULL;
	
	world[room_nr].wild_modif = FALSE;
	world[room_nr].wild_rnum = -1;
	
	sprintf(buf, "Format error in room #%d (expecting D/E/S)", virtual_nr);
	
	for (;;) {
		if (!get_line(fl, line)) {
			fprintf(stderr, "%s\n", buf);
			exit(1);
		}
		switch (*line) {
			case 'D':
				setup_dir(fl, room_nr, atoi(line + 1));
				break;
			case 'E':
				CREATE(new_descr, struct extra_descr_data, 1);
				new_descr->keyword = fread_string(fl, buf2);
				new_descr->description = fread_string(fl, buf2);
				new_descr->next = world[room_nr].ex_description;
				world[room_nr].ex_description = new_descr;
				break;
			case 'S':			/* end of room */
      /* DG triggers -- script is defined after the end of the room */
				letter = fread_letter(fl);
				ungetc(letter, fl);
				while (letter=='T') {
					dg_read_trigger(fl, &world[room_nr], WLD_TRIGGER);
					letter = fread_letter(fl);
					ungetc(letter, fl);
				}
				top_of_world = room_nr++;
				return;
				break;
			default:
				fprintf(stderr, "%s\n", buf);
				exit(1);
				break;
		}
	}
}



/* read direction data */
void setup_dir(FILE * fl, int room, int dir)
{
	int t[5];
	char line[256];
	
	sprintf(buf2, "room #%d, direction D%d", world[room].number, dir);
	
	CREATE(world[room].dir_option[dir], struct room_direction_data, 1);
	world[room].dir_option[dir]->general_description = fread_string(fl, buf2);
	world[room].dir_option[dir]->keyword = fread_string(fl, buf2);
	
	if (!get_line(fl, line)) {
		fprintf(stderr, "Format error, %s\n", buf2);
		exit(1);
	}
	if (sscanf(line, " %d %d %d ", t, t + 1, t + 2) != 3) {
		fprintf(stderr, "Format error, %s\n", buf2);
		exit(1);
	}
	if (t[0] == 1)
		world[room].dir_option[dir]->exit_info = EX_ISDOOR;
	else if (t[0] == 2)
		world[room].dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF;
	else
		world[room].dir_option[dir]->exit_info = 0;
	
	world[room].dir_option[dir]->key = t[1];
	world[room].dir_option[dir]->to_room = t[2];
}

/* make sure the start rooms exist & resolve their vnums to rnums */
void check_start_rooms(void)
{
	int count;
	extern room_vnum mortal_start_room[NUM_STARTROOMS +1];
	extern room_vnum immort_start_room;
	extern room_vnum frozen_start_room;
	
	for (count = 1; count <= (NUM_STARTROOMS + 1); count++)
		if ((r_mortal_start_room[count] = real_room(mortal_start_room[count])) < 0) {
			if (count > 1)
				r_mortal_start_room[count] = real_room(mortal_start_room[1]);
			else {
				log("SYSERR:  Mortal start room does not exist.  Change in config.c.");
				exit(1);
			}
		}
		
	if ((r_immort_start_room = real_room(immort_start_room)) < 0) {
		if (!mini_mud)
			log("SYSERR:  Warning: Immort start room does not exist.  Change in config.c.");
		r_immort_start_room = r_mortal_start_room[1];
	}
	if ((r_frozen_start_room = real_room(frozen_start_room)) < 0) {
		if (!mini_mud)
			log("SYSERR:  Warning: Frozen start room does not exist.  Change in config.c.");
		r_frozen_start_room = r_mortal_start_room[1];
	}
}




/* resolve all vnums into rnums in the world */
void renum_world(void)
{
	register int room, door;
	
	for (room = 0; room <= top_of_world; room++)
		for (door = 0; door < NUM_OF_DIRS; door++)
			if (world[room].dir_option[door])
				if (world[room].dir_option[door]->to_room != NOWHERE)
					world[room].dir_option[door]->to_room =
						real_room(world[room].dir_option[door]->to_room);
}


#define ZCMD zone_table[zone].cmd[cmd_no]

/* resulve vnums into rnums in the zone reset tables */
void renum_zone_table(void)
{
	int zone, cmd_no, a, b;
	
	for (zone = 0; zone <= top_of_zone_table; zone++)
		for (cmd_no = 0; ZCMD.command != 'S'; cmd_no++) {
			a = b = 0;
			switch (ZCMD.command) {
				case 'M':
					a = ZCMD.arg1 = real_mobile(ZCMD.arg1);
					b = ZCMD.arg3 = real_room(ZCMD.arg3);
					break;
				case 'O':
					a = ZCMD.arg1 = real_object(ZCMD.arg1);
					if (ZCMD.arg3 != NOWHERE)
						b = ZCMD.arg3 = real_room(ZCMD.arg3);
					break;
				case 'G':
					a = ZCMD.arg1 = real_object(ZCMD.arg1);
					break;
				case 'E':
					a = ZCMD.arg1 = real_object(ZCMD.arg1);
					break;
				case 'P':
					a = ZCMD.arg1 = real_object(ZCMD.arg1);
					b = ZCMD.arg3 = real_object(ZCMD.arg3);
					break;
				case 'D':
					a = ZCMD.arg1 = real_room(ZCMD.arg1);
					break;
				case 'R': /* rem obj from room */
					a = ZCMD.arg1 = real_room(ZCMD.arg1);
					b = ZCMD.arg2 = real_object(ZCMD.arg2);
					break;
			}
			if (a < 0 || b < 0) {
				if (!mini_mud)
					log_zone_error(zone, cmd_no, "Invalid vnum, cmd disabled");
				ZCMD.command = '*';
			}
		}
}



void parse_simple_mob(FILE *mob_f, int i, int nr)
{
	int j, t[10];
	char line[256];
	extern int mount_max_move_add_per_level;
	
	mob_proto[i].real_abils.str = 11;
	mob_proto[i].real_abils.intel = 11;
	mob_proto[i].real_abils.wis = 11;
	mob_proto[i].real_abils.dex = 11;
	mob_proto[i].real_abils.con = 11;
	mob_proto[i].real_abils.cha = 11;
	
	get_line(mob_f, line);
	if (sscanf(line, " %d %d %d %dd%d+%d %dd%d+%d ",
			t, t + 1, t + 2, t + 3, t + 4, t + 5, t + 6, t + 7, t + 8) != 9) {
		fprintf(stderr, "Format error in mob #%d, first line after S flag\n"
			"...expecting line of form '# # # #d#+# #d#+#'\n", nr);
		exit(1);
	}
	GET_LEVEL(mob_proto + i) = t[0];
	mob_proto[i].points.hitroll = 20 - t[1];
	mob_proto[i].points.armor = 10 * t[2];
	
    /* max hit = 0 is a flag that H, M, V is xdy+z */
	mob_proto[i].points.max_hit = 0;
	mob_proto[i].points.hit = t[3];
	mob_proto[i].points.mana = t[4];
	mob_proto[i].points.move = t[5];
	
	mob_proto[i].points.max_mana = 100;
	mob_proto[i].points.max_move = 50;
	if (MOB_FLAGGED(mob_proto+i, MOB_MOUNTABLE))
		mob_proto[i].points.max_move += mount_max_move_add_per_level * GET_LEVEL(mob_proto + i);
	
	mob_proto[i].mob_specials.damnodice = t[6];
	mob_proto[i].mob_specials.damsizedice = t[7];
	mob_proto[i].points.damroll = t[8];
	
	get_line(mob_f, line);
	sscanf(line, " %d %d ", t, t + 1);
	/*if(GET_LEVEL(mob_proto + i)<=20)
      GET_GOLD(mob_proto + i) = GET_LEVEL(mob_proto + i);
    else
      GET_GOLD(mob_proto + i) = t[0];*/
	
	/*Lance */
	GET_GOLD(mob_proto + i) = (GET_LEVEL(mob_proto + i)/2) + number(1,GET_LEVEL(mob_proto + i)/2) ;
	
	GET_EXP(mob_proto + i) = t[1];
	
	
	
	get_line(mob_f, line);
	if (sscanf(line, " %d %d %d %d ", t, t + 1, t + 2, t + 3) != 3) {
		fprintf(stderr, "Format error in mob #%d, second line after S flag\n"
			"...expecting line of form '# # #'\n", nr);
	}
	
	mob_proto[i].char_specials.position = t[0];
	mob_proto[i].mob_specials.default_pos = t[1];
	mob_proto[i].player.sex = t[2];
	
	mob_proto[i].player.class = 0;
	mob_proto[i].player.weight = 200;
	mob_proto[i].player.height = 198;
	
	for (j = 0; j < 3; j++)
		GET_COND(mob_proto + i, j) = -1;
	
    /*
     * these are now save applies; base save numbers for MOBs are now from
     * the warrior save table.
     */
	for (j = 0; j < 5; j++)
		GET_SAVE(mob_proto + i, j) = 0;
}


/*
 * interpret_espec is the function that takes espec keywords and values
 * and assigns the correct value to the mob as appropriate.  Adding new
 * e-specs is absurdly easy -- just add a new CASE statement to this
 * function!  No other changes need to be made anywhere in the code.
 */

#define CASE(test) if (!matched && !str_cmp(keyword, test) && (matched = 1))
#define RANGE(low, high) (num_arg = MAX((low), MIN((high), (num_arg))))

void interpret_espec(char *keyword, char *value, int i, int nr)
{
	int num_arg, matched = 0;
	
	num_arg = atoi(value);
	
CASE("BareHandAttack") {
	RANGE(0, 99);
	mob_proto[i].mob_specials.attack_type = num_arg;
}

CASE("Str") {
	RANGE(3, 25);
	mob_proto[i].real_abils.str = num_arg;
}

CASE("StrAdd") {
	RANGE(0, 100);
	mob_proto[i].real_abils.str_add = num_arg;
}

CASE("Int") {
	RANGE(3, 25);
	mob_proto[i].real_abils.intel = num_arg;
}

CASE("Wis") {
	RANGE(3, 25);
	mob_proto[i].real_abils.wis = num_arg;
}

CASE("Dex") {
	RANGE(3, 25);
	mob_proto[i].real_abils.dex = num_arg;
}

CASE("Con") {
	RANGE(3, 25);
	mob_proto[i].real_abils.con = num_arg;
}

CASE("Cha") {
	RANGE(3, 25);
	mob_proto[i].real_abils.cha = num_arg;
}
CASE("Spec"){
	RANGE(0, MAX_SPEC);
	interpret_sproc(i,num_arg);
}
if (!matched) {
	fprintf(stderr, "Warning: unrecognized espec keyword %s in mob #%d\n",
		keyword, nr);
}
}

#undef CASE
#undef RANGE

void parse_espec(char *buf, int i, int nr)
{
	char *ptr;
	
	if ((ptr = strchr(buf, ':')) != NULL) {
		*(ptr++) = '\0';
		while (isspace(*ptr))
			ptr++;
	} else
		ptr = "";
	
	interpret_espec(buf, ptr, i, nr);
}


void parse_enhanced_mob(FILE *mob_f, int i, int nr)
{
	char line[256];
	
	parse_simple_mob(mob_f, i, nr);
	
	while (get_line(mob_f, line)) {
		if (!strcmp(line, "E"))	/* end of the ehanced section */
			return;
		else if (*line == '#') {	/* we've hit the next mob, maybe? */
			fprintf(stderr, "Unterminated E section in mob #%d\n", nr);
			exit(1);
		} else
			parse_espec(line, i, nr);
	}
	
	fprintf(stderr, "Unexpected end of file reached after mob #%d\n", nr);
	exit(1);
}


void parse_mobile(FILE * mob_f, int nr)
{
	static int i = 0;
	int j, t[10], z[10];
	long l[10];
	char line[256], *tmpptr, letter;
	char f1[128], f2[128];
	
	mob_index[i].virtual = nr;
	mob_index[i].number = 0;
	mob_index[i].func = NULL;
	
	clear_char(mob_proto + i);
	
	mob_proto[i].player_specials = &dummy_mob;
	sprintf(buf2, "mob vnum %d", nr);
	
  /***** String data *** */
	mob_proto[i].player.name = fread_string(mob_f, buf2);
	tmpptr = mob_proto[i].player.short_descr = fread_string(mob_f, buf2);
	if (tmpptr && *tmpptr)
		if (!str_cmp(fname(tmpptr), "a") || !str_cmp(fname(tmpptr), "an") ||
			!str_cmp(fname(tmpptr), "the"))
			*tmpptr = LOWER(*tmpptr);
	mob_proto[i].player.long_descr = fread_string(mob_f, buf2);
	mob_proto[i].player.description = fread_string(mob_f, buf2);
	mob_proto[i].player.title = NULL;
	
  /*non usate*/
	fread_string(mob_f, buf2);
	fread_string(mob_f, buf2);
	fread_string(mob_f, buf2);
	
  /* *** Numeric data *** */
	get_line(mob_f, line);
	sscanf(line, "%s %s %d %d %d %d %ld %d %c", f1, f2, t + 2, z, z + 1, z + 2, l , z + 3, &letter);
	CLAN_ID(mob_proto + i) = z[3];
	MASTER_ID(mob_proto + i) = l[0];
	
	MOB_FLAGS(mob_proto + i) = asciiflag_conv(f1);
	SET_BIT(MOB_FLAGS(mob_proto + i), MOB_ISNPC);
	AFF_FLAGS(mob_proto + i) = asciiflag_conv(f2);
	GET_ALIGNMENT(mob_proto + i) = t[2];
	
	switch (letter) {
		case 'S':	/* Simple monsters */
			parse_simple_mob(mob_f, i, nr);
			break;
		case 'E':	/* Circle3 Enhanced monsters */
			parse_enhanced_mob(mob_f, i, nr);
			break;
  /* add new mob types here.. */
		default:
			fprintf(stderr, "Unsupported mob type '%c' in mob #%d\n", letter, nr);
			exit(1);
			break;
	}
  /* DG triggers -- script info follows mob S/E section */
	letter = fread_letter(mob_f);
	ungetc(letter, mob_f);
	while (letter=='T') {
		dg_read_trigger(mob_f, &mob_proto[i], MOB_TRIGGER);
		letter = fread_letter(mob_f);
		ungetc(letter, mob_f);
	}
	
	mob_proto[i].aff_abils = mob_proto[i].real_abils;
	
	for (j = 0; j < NUM_WEARS; j++)
		mob_proto[i].equipment[j] = NULL;
	
	mob_proto[i].nr = i;
	mob_proto[i].desc = NULL;
	
	top_of_mobt = i++;
}




/* read all objects from obj file; generate index and prototypes */
char *parse_object(FILE * obj_f, int nr)
{
	static int i = 1, retval;
	static char line[256];
  /* int t[10], j;*/
	int t[6], j;
	long long int lungo[4];
	char *tmpptr;
	char f1[256], f2[256];
	struct extra_descr_data *new_descr;
	
	obj_index[i].virtual = nr;
	obj_index[i].number = 0;
	obj_index[i].func = NULL;
	
	clear_object(obj_proto + i);
	obj_proto[i].in_room = NOWHERE;
	obj_proto[i].item_number = i;
	
	sprintf(buf2, "object #%d", nr);
	
  /* *** string data *** */
	if ((obj_proto[i].name = fread_string(obj_f, buf2)) == NULL) {
		fprintf(stderr, "Null obj name or format error at or near %s\n", buf2);
		exit(1);
	}
	
	tmpptr = obj_proto[i].short_description = fread_string(obj_f, buf2);
	
	if (*tmpptr)
		if (!str_cmp(fname(tmpptr), "a") || !str_cmp(fname(tmpptr), "an") ||
			!str_cmp(fname(tmpptr), "the"))
			*tmpptr = LOWER(*tmpptr);
		
	tmpptr = obj_proto[i].description = fread_string(obj_f, buf2);
	
	if (tmpptr && *tmpptr)
		*tmpptr = UPPER(*tmpptr);
	obj_proto[i].action_description = fread_string(obj_f, buf2);
	
	
	
/* *** numeric data *** */
	if (!get_line(obj_f, line) ||
		(retval = sscanf(line, " %d %s %s", t, f1, f2)) != 3) {
		fprintf(stderr, "Format error in first numeric line (expecting 3 args, got %d), %s\n", retval, buf2); exit(1);
	}
	obj_proto[i].obj_flags.type_flag = t[0];
	obj_proto[i].obj_flags.extra_flags = asciiflag_conv(f1);
	obj_proto[i].obj_flags.wear_flags = asciiflag_conv(f2);
	
	if (!get_line(obj_f, line) ||
		(retval = sscanf(line, "%d %d %d %d %d %d", t, t + 1, t + 2, t + 3, t + 4, t + 5)) > 6) { // umm, still trying here!
		fprintf(stderr, "Format error in second numeric line (expecting 4 or 6 args, got %d), %s\n", retval, buf2); exit(1);
	}
	
	obj_proto[i].obj_flags.value[0] = t[0];
	obj_proto[i].obj_flags.value[1] = t[1];
	obj_proto[i].obj_flags.value[2] = t[2];
	obj_proto[i].obj_flags.value[3] = t[3];
	if (t[4] < 0 || t[4] > 100) {
		obj_proto[i].obj_flags.curr_slots = 0;
		obj_proto[i].obj_flags.total_slots = 0;
	}
	else {
		obj_proto[i].obj_flags.curr_slots = t[4];
		obj_proto[i].obj_flags.total_slots = t[5];
	}
	
	
	if (t[5] < 0 || t[5] > 100) {
		obj_proto[i].obj_flags.curr_slots = 0;
		obj_proto[i].obj_flags.total_slots = 0;
	}
	else {
		obj_proto[i].obj_flags.curr_slots = t[4];
		obj_proto[i].obj_flags.total_slots = t[5];
	}
	
	if (!get_line(obj_f, line) ||
		(retval = sscanf(line, "%d %d %d %d %d %d", t, t + 1, t + 2, t + 3, t + 4, t + 5)) < 3 || (retval>6) ) {
		fprintf(stderr, "Format error in third numeric line (expecting 3 or 4 args, got %d), %s\n", retval, buf2);
		exit(1);
	}
	obj_proto[i].obj_flags.weight = t[0];
	obj_proto[i].obj_flags.cost = t[1];
	obj_proto[i].obj_flags.cost_per_day = t[2];
	obj_proto[i].obj_flags.min_level = (retval == 3 ? -1 : t[3]);
	obj_proto[i].obj_flags.tipo_materiale = (retval <= 4 ? 0 : t[4]);
	obj_proto[i].obj_flags.num_materiale = (retval <= 4 ? 0 : t[5]);
	
  /* check to make sure that weight of containers exceeds curr. quantity */
	if (obj_proto[i].obj_flags.type_flag == ITEM_DRINKCON ||
		obj_proto[i].obj_flags.type_flag == ITEM_FOUNTAIN) {
		if (obj_proto[i].obj_flags.weight < obj_proto[i].obj_flags.value[1])
			obj_proto[i].obj_flags.weight = obj_proto[i].obj_flags.value[1] + 5;
	}
	
  /* *** extra descriptions and affect fields *** */
	
	for (j = 0; j < MAX_OBJ_AFFECT; j++) {
		obj_proto[i].affected[j].location = APPLY_NONE;
		obj_proto[i].affected[j].modifier = 0;
	}
	
	strcat(buf2, ", after numeric constants (expecting E/A/S/#xxx)");
	j = 0;
	
	
	for (;;) {
		if (!get_line(obj_f, line)) {
			fprintf(stderr, "Format error in %s\n", buf2);
			exit(1);
		}
		switch (*line) {
			case 'E':
				CREATE(new_descr, struct extra_descr_data, 1);
				new_descr->keyword = fread_string(obj_f, buf2);
				new_descr->description = fread_string(obj_f, buf2);
				new_descr->next = obj_proto[i].ex_description;
				obj_proto[i].ex_description = new_descr;
				break;
			case 'A':
				if (j >= MAX_OBJ_AFFECT) {
					fprintf(stderr, "Too many A fields (%d max), %s\n", MAX_OBJ_AFFECT, buf2);
					exit(1);
				}
				get_line(obj_f, line);
				sscanf(line, " %d %d ", t, t + 1);
				obj_proto[i].affected[j].location = t[0];
				obj_proto[i].affected[j].modifier = t[1];
				j++;
				break;
				
			case 'C':
				get_line(obj_f,line);
				sscanf(line, "%Ld %Ld %Ld %Ld", &lungo[0], &lungo[1], &lungo[2], &lungo[3]);
				obj_proto[i].obj_flags.bitvector[0] = lungo[0];
				obj_proto[i].obj_flags.bitvector[1] = lungo[1];
				obj_proto[i].obj_flags.bitvector[2] = lungo[2];
				obj_proto[i].obj_flags.bitvector[3] = lungo[3];
				break;
			case 'T':  /* DG triggers */
				dg_obj_trigger(line, &obj_proto[i]);
				break;
				
			case '$':
			case '#':
				top_of_objt = i++;
				return line;
				break;
			default:
				fprintf(stderr, "Format error in %s\n", buf2);
				exit(1);
				break;
		}
	}
}


#define Z	zone_table[zone]

/* load the zone table and command tables */
void load_zones(FILE * fl, char *zonename)
{
	static int zone = 0;
	int cmd_no = 0, num_of_cmds = 0, line_num = 0, tmp, error;
	char *ptr, buf[256], zname[256];
	int i;
	
	strcpy(zname, zonename);
	
	while (get_line(fl, buf))
		num_of_cmds++;		/* this should be correct within 3 or so */
	rewind(fl);
	
	if (num_of_cmds == 0) {
		fprintf(stderr, "%s is empty!\n", zname);
		exit(0);
	} else
		CREATE(Z.cmd, struct reset_com, num_of_cmds);
	
	line_num += get_line(fl, buf);
	
	if (sscanf(buf, "#%d", &Z.number) != 1) {
		fprintf(stderr, "Format error in %s, line %d\n", zname, line_num);
		exit(0);
	}
	sprintf(buf2, "beginning of zone #%d", Z.number);
	
	line_num += get_line(fl, buf);
	if ((ptr = strchr(buf, '~')) != NULL)	/* take off the '~' if it's there */
		*ptr = '\0';
	Z.name = str_dup(buf);
	
	line_num += get_line(fl, buf);
	Z.wilderness = ZONE_NORMAL_AREA; // In wilderness.h
	if (sscanf(buf, " %d %d %d %d ", &Z.top, &Z.lifespan, &Z.reset_mode, &Z.wilderness) < 3) {
		fprintf(stderr, "Format error in (3 or 4) - constant line of %s", zname);
		exit(0);
	}
	
	if (Z.wilderness == ZONE_MINIWILD)
	{
		line_num += get_line(fl, buf);
		if (sscanf(buf, " %d %d %d %d ", &Z.miniwild_exit[0], &Z.miniwild_exit[1], &Z.miniwild_exit[2], &Z.miniwild_exit[3]) != 4)
		{
			fprintf(stderr, "Errore in zedit, la zona %s e' una  miniwild ma non sono segnati i link al mondo", zname);
			exit(0);
		}
	}
	else for (i=0; i<4; i++) Z.miniwild_exit[i] = -1;
	
	Z.pressure = 960;
	if ((time_info.month >= 7) && (time_info.month <= 12))
		Z.pressure += dice(1, 50);
	else
		Z.pressure += dice(1, 80);
	Z.change = 0;
	if (Z.pressure <= 980)
		Z.sky = SKY_LIGHTNING;
	else if (Z.pressure <= 1000)
		Z.sky = SKY_RAINING;
	else if (Z.pressure <= 1020)
		Z.sky = SKY_CLOUDY;
	else
		Z.sky = SKY_CLOUDLESS;
	cmd_no = 0;
	
	for (;;) {
		if ((tmp = get_line(fl, buf)) == 0) {
			fprintf(stderr, "Format error in %s - premature end of file\n", zname);
			exit(0);
		}
		line_num += tmp;
		ptr = buf;
		skip_spaces(&ptr);
		
		if ((ZCMD.command = *ptr) == '*')
			continue;
		
		ptr++;
		
		if (ZCMD.command == 'S' || ZCMD.command == '$') {
			ZCMD.command = 'S';
			break;
		}
		error = 0;
		if (strchr("MOEPD", ZCMD.command) == NULL) {	/* a 3-arg command */
			if (sscanf(ptr, " %d %d %d ", &tmp, &ZCMD.arg1, &ZCMD.arg2) != 3)
				error = 1;
		} else {
			if (sscanf(ptr, " %d %d %d %d ", &tmp, &ZCMD.arg1, &ZCMD.arg2,
					&ZCMD.arg3) != 4)
				error = 1;
		}
		
		ZCMD.if_flag = tmp;
		
		if (error) {
			fprintf(stderr, "Format error in %s, line %d: '%s'\n", zname, line_num, buf);
			exit(0);
		}
		ZCMD.line = line_num;
		cmd_no++;
	}
	
	top_of_zone_table = zone++;
}

#undef Z


void get_one_line(FILE *fl, char *buf)
{
	if (fgets(buf, READ_SIZE, fl) == NULL) {
		log("error reading help file: not terminated with $?");
		exit(1);
	}
	
	buf[strlen(buf) - 1] = '\0'; /* take off the trailing \n */
}


void load_help(FILE *fl)
{
	char key[READ_SIZE+1], next_key[READ_SIZE+1], entry[32384];
	char line[READ_SIZE+1], *scan;
	struct help_index_element el;
	
  /* get the first keyword line */
	get_one_line(fl, key);
	while (*key != '$') {
    /* read in the corresponding help entry */
		strcpy(entry, strcat(key, "\r\n"));
		get_one_line(fl, line);
		while (*line != '#') {
			strcat(entry, strcat(line, "\r\n"));
			get_one_line(fl, line);
		}
		
    /* now, add the entry to the index with each keyword on the keyword line */
		el.duplicate = 0;
		el.entry = str_dup(entry);
		scan = one_word(key, next_key);
		while (*next_key) {
			el.keyword = str_dup(next_key);
			help_table[top_of_helpt++] = el;
			el.duplicate++;
			scan = one_word(scan, next_key);
		}
		
    /* get next keyword line (or $) */
		get_one_line(fl, key);
	}
}


int hsort(const void *a, const void *b)
{
	struct help_index_element *a1, *b1;
	
	a1 = (struct help_index_element *) a;
	b1 = (struct help_index_element *) b;
	
	return (str_cmp(a1->keyword, b1->keyword));
}


/*************************************************************************
*  procedures for resetting, both play-time and boot-time		*
*********************************************************************** */



int vnum_mobile(char *searchname, struct char_data * ch)
{
	int nr, found = 0;
	char buffer[MAX_STRING_LENGTH]; 
	
	sprintf(buffer," ");
	for (nr = 0; nr <= top_of_mobt && strlen(buffer) < 8000; nr++) {
		if (isname(searchname, mob_proto[nr].player.name)) {
			sprintf(buf, "%3d. [%5d] %s\r\n", ++found,
				mob_index[nr].virtual,
				mob_proto[nr].player.short_descr);
			strcat(buffer,buf);
		}
	}
	page_string(ch->desc,buffer,1);
	return (found);
}

int vnum_object(char *searchname, struct char_data *ch)
{
	int nr, found = 0;
	char buffer[MAX_STRING_LENGTH];
	
	sprintf(buffer," ");
	for (nr = 1; nr <= top_of_objt && strlen(buffer) < 8000; nr++) {
		if (obj_proto[nr].name != NULL) {
		  if (isname(searchname, obj_proto[nr].name)) {
			  sprintf(buf, "%3d. [%5d] %s\r\n", ++found,
				              obj_index[nr].virtual,
				              obj_proto[nr].short_description);
			  strcat(buffer, buf);
		  }
		}
		else {
		  sprintf(buf, "%3d. (It wanna CRASH)Phantom ci mette una pexxa!!!\r\n", 
		                  ++found);
			  strcat(buffer, buf);
		}
	}
	page_string(ch->desc,buffer,1);
	return (found);
}


/* create a character, and add it to the char list */
struct char_data *create_char(void)
{
	struct char_data *ch;
	
	CREATE(ch, struct char_data, 1);
	clear_char(ch);
	ch->next = character_list;
	character_list = ch;
	GET_ID(ch) = max_id++;
	return ch;
}


/* create a new mobile from a prototype */
struct char_data *read_mobile(int nr, int type)
{
	int i;
	int temp;
	struct char_data *mob;
	extern int proficienze_mob;
	
	if (type == VIRTUAL) {
		if ((i = real_mobile(nr)) < 0) {
			sprintf(buf, "Mobile (V) %d does not exist in database.", nr);
			return (0);
		}
	} else
		i = nr;
	
	CREATE(mob, struct char_data, 1);
	clear_char(mob);
	*mob = mob_proto[i];
	mob->next = character_list;
	character_list = mob;
	
	if (!mob->points.max_hit) {
		mob->points.max_hit = dice(mob->points.hit, mob->points.mana) +
			mob->points.move;
	} else
		mob->points.max_hit = number(mob->points.hit, mob->points.mana);
	
	mob->points.hit = mob->points.max_hit;
	mob->points.mana = mob->points.max_mana;
	mob->points.move = mob->points.max_move;
	
	//Chardhros - Setta la fame a 76 se cavalcatura
	if (MOB_FLAGGED(mob, MOB_MOUNTABLE))
		GET_COND(mob,FULL) = 76;
	
	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon = time(0);
	
	for (temp=0; temp<=MAX_PROFICIENZE; temp++)
		if (MOB_FLAGGED(mob,MOB_WORKER))
			GET_PROF(mob, temp) = proficienze_mob;
		else
			GET_PROF(mob, temp) = 0;
	
	mob_index[i].number++;
	GET_ID(mob) = max_id++;
	assign_triggers(mob, MOB_TRIGGER);
	times_events_ini(mob);
	
	/* Se il mob e' un esercito, creo la struttura dati_esercito */
	if(IS_ESERCITO(mob)) {
		CREATE(GET_ESERCITO(mob), struct dati_esercito, 1);
		clear_esercito(mob);
	}
	
	return mob;
}


/* create an object, and add it to the object list */
struct obj_data *create_obj(void)
{
	struct obj_data *obj;
	
	CREATE(obj, struct obj_data, 1);
	clear_object(obj);
	obj->next = object_list;
	object_list = obj;
	GET_ID(obj) = max_id++;
	assign_triggers(obj, OBJ_TRIGGER);
	
	return obj;
}


/* create a new object from a prototype */
struct obj_data *read_object(int nr, int type)
{
	struct obj_data *obj;
	int i;
	
	if (nr < 0) {
		log("SYSERR: trying to create obj with negative num!");
		return NULL;
	}
	if (type == VIRTUAL) {
		if ((i = real_object(nr)) < 0) {
			sprintf(buf, "Object (V) %d does not exist in database.", nr);
			mudlog (buf, BRF, LVL_IMMORT, TRUE);
			return NULL;
		}
	} else
		i = nr;
	
	CREATE(obj, struct obj_data, 1);
	clear_object(obj);
	*obj = obj_proto[i];
	obj->next = object_list;
	object_list = obj;
	
	obj_index[i].number++;
	GET_ID(obj) = max_id++;
	
	assign_triggers(obj, OBJ_TRIGGER);
	
	return obj;
}



#define ZO_DEAD  999

/* update zone ages, queue for reset if necessary, and dequeue when possible */
void zone_update(void)
{
	int i;
	struct reset_q_element *update_u, *temp;
	static int timer = 0;
	char buf[128];
	
  /* jelson 10/22/92 */
	if (((++timer * PULSE_ZONE) / PASSES_PER_SEC) >= 60) {
    /* one minute has passed */
    /*
     * NOT accurate unless PULSE_ZONE is a multiple of PASSES_PER_SEC or a
     * factor of 60
     */
		
		timer = 0;
		quest_update();
    /* since one minute has passed, increment zone ages */
		for (i = 0; i <= top_of_zone_table; i++) {
			if (zone_table[i].age < zone_table[i].lifespan &&
				zone_table[i].reset_mode)
				(zone_table[i].age)++;
			
			if (zone_table[i].age >= zone_table[i].lifespan &&
				zone_table[i].age < ZO_DEAD && zone_table[i].reset_mode) {
	/* enqueue zone */
				
				CREATE(update_u, struct reset_q_element, 1);
				
				update_u->zone_to_reset = i;
				update_u->next = 0;
				
				if (!reset_q.head)
					reset_q.head = reset_q.tail = update_u;
				else {
					reset_q.tail->next = update_u;
					reset_q.tail = update_u;
				}
				
				zone_table[i].age = ZO_DEAD;
			}
		}
	}	/* end - one minute has passed */
	
	
  /* dequeue zones (if possible) and reset */
  /* this code is executed every 10 seconds (i.e. PULSE_ZONE) */
	for (update_u = reset_q.head; update_u; update_u = update_u->next)
		if (zone_table[update_u->zone_to_reset].reset_mode == 2 ||
			is_empty(update_u->zone_to_reset)) {
			sprintf(buf, "Auto zone reset: %s",
				zone_table[update_u->zone_to_reset].name);
			mudlog(buf, NRM, LVL_GOD, TRUE);
			
			reset_zone(update_u->zone_to_reset);
      /* dequeue */
			if (update_u == reset_q.head)
				reset_q.head = reset_q.head->next;
			else {
				for (temp = reset_q.head; temp->next != update_u;
					temp = temp->next);
				
				if (!update_u->next)
					reset_q.tail = temp;
				
				temp->next = update_u->next;
			}
			
			free(update_u);
			break;
		}
}

void log_zone_error(int zone, int cmd_no, char *message)
{
	char buf[256];
	
	sprintf(buf, "SYSERR: error in zone file: %s", message);
	mudlog(buf, NRM, LVL_GOD, TRUE);
	
	sprintf(buf, "SYSERR: ...offending cmd: '%c' cmd in zone #%d, line %d",
		ZCMD.command, zone_table[zone].number, ZCMD.line);
	mudlog(buf, NRM, LVL_GOD, TRUE);
}

#define ZONE_ERROR(message) { log_zone_error(zone, cmd_no, message); last_cmd = 0; }

/* execute the reset command table of a given zone */
void reset_zone(int zone)
{
	int cmd_no, last_cmd = 0;
	struct char_data *mob = NULL;
	struct obj_data *obj, *obj_to;
	
	for (cmd_no = 0; ZCMD.command != 'S'; cmd_no++) {
		
		if (ZCMD.if_flag && !last_cmd)
			continue;
		
		switch (ZCMD.command) {
			case '*':			/* ignore command */
				last_cmd = 0;
				break;
				
			case 'M':			/* read a mobile */
				if (mob_index[ZCMD.arg1].number < ZCMD.arg2) {
					mob = read_mobile(ZCMD.arg1, REAL);
					char_to_room(mob, ZCMD.arg3);
					load_mtrigger(mob); //Orione
					last_cmd = 1;
				} else
					last_cmd = 0;
				break;
				
			case 'O':			/* read an object */
				if (num_obj_in_zone(ZCMD.arg1,zone) < ZCMD.arg2)
				{
					if (ZCMD.arg3 >= 0)
					{
						obj = read_object(ZCMD.arg1, REAL);
						obj_to_room(obj, ZCMD.arg3);
						load_otrigger(obj); //Orione
						last_cmd = 1;
					}
					else
					{
						obj = read_object(ZCMD.arg1, REAL);
						obj->in_room = NOWHERE;
						last_cmd = 1;
					}
				}
				else last_cmd = 0;
				break;
				
/* CARLO CARLO - MODIFICA PER OGGETTI MAX IN MUD - PRIMA ERA COSI'
      if (obj_index[ZCMD.arg1].number < ZCMD.arg2)
        {
	if (ZCMD.arg3 >= 0)
          {
	  obj = read_object(ZCMD.arg1, REAL);
	  obj_to_room(obj, ZCMD.arg3);
	  last_cmd = 1;
	  }
        else
          {
	  obj = read_object(ZCMD.arg1, REAL);
	  obj->in_room = NOWHERE;
	  last_cmd = 1;
	  }
        }
      else last_cmd = 0;
      break;
*/
				
			case 'P':			/* object to object */
//      if (obj_index[ZCMD.arg1].number < ZCMD.arg2) {   VECCHIO !!!!
				if (num_obj_in_zone(ZCMD.arg1,zone) < ZCMD.arg2) {
					obj = read_object(ZCMD.arg1, REAL);
					if (!(obj_to = get_obj_num(ZCMD.arg3))) {
						ZONE_ERROR("target obj not found");
						break;
					}
					obj_to_obj(obj, obj_to);
					load_otrigger(obj); //Orione
					last_cmd = 1;
				} else
					last_cmd = 0;
				break;
				
			case 'G':			/* obj_to_char */
				if (!mob) {
					ZONE_ERROR("attempt to give obj to non-existant mob");
					break;
				}
//      if (obj_index[ZCMD.arg1].number < ZCMD.arg2) {  VECCHIO !!!!
				if (num_obj_in_zone(ZCMD.arg1,zone) < ZCMD.arg2) {
					obj = read_object(ZCMD.arg1, REAL);
					obj_to_char(obj, mob);
					load_otrigger(obj); //Orione
					last_cmd = 1;
				} else
					last_cmd = 0;
				break;
				
			case 'E':			/* object to equipment list */
				if (!mob) {
					ZONE_ERROR("trying to equip non-existant mob");
					break;
				}
//      if (obj_index[ZCMD.arg1].number < ZCMD.arg2) {    VECCHIO !!!!
				if (num_obj_in_zone(ZCMD.arg1,zone) < ZCMD.arg2) {
					if (ZCMD.arg3 < 0 || ZCMD.arg3 >= NUM_WEARS) {
						ZONE_ERROR("invalid equipment pos number");
					} else {
						obj = read_object(ZCMD.arg1, REAL);
						equip_char(mob, obj, ZCMD.arg3);
						load_otrigger(obj); //Orione
						last_cmd = 1;
					}
				} else
					last_cmd = 0;
				break;
				
			case 'R': /* rem obj from room */
				if ((obj = get_obj_in_list_num(ZCMD.arg2, world[ZCMD.arg1].contents)) != NULL) {
					obj_from_room(obj);
					extract_obj(obj);
				}
				last_cmd = 1;
				break;
				
				
			case 'D':			/* set state of door */
				if (ZCMD.arg2 < 0 || ZCMD.arg2 >= NUM_OF_DIRS ||
					(world[ZCMD.arg1].dir_option[ZCMD.arg2] == NULL)) {
					ZONE_ERROR("door does not exist");
				} else
					switch (ZCMD.arg3) {
						case 0:
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_LOCKED);
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_CLOSED);
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_HIDDEN);
							break;
						case 1:
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_CLOSED);
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_LOCKED);
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_HIDDEN);
							break;
						case 2:
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_LOCKED);
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_CLOSED);
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_HIDDEN);
							break;
							
						case 3:
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_CLOSED);
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_HIDDEN);
							REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_LOCKED);
							break;
						case 4:
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_CLOSED);
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_LOCKED);
							SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
								EX_HIDDEN);
							break;
					}
				last_cmd = 1;
				break;
				
			default:
				ZONE_ERROR("unknown cmd in reset table; cmd disabled");
				ZCMD.command = '*';
				break;
		}
	}
	
  //if (zone_table[zone].number == WILD_ZONE) reset_risorse();
	zone_table[zone].age = 0;
}



/* for use in reset_zone; return TRUE if zone 'nr' is free of PC's  */
int is_empty(int zone_nr)
{
	struct descriptor_data *i;
	
	for (i = descriptor_list; i; i = i->next)
		if (!i->connected)
			if (world[i->character->in_room].zone == zone_nr)
				return 0;
			
	return 1;
}





/*************************************************************************
*  stuff related to the save/load player system				*
*********************************************************************** */


long get_id_by_name(char *name)
{
	int i;
	
	for (i = 0; i <= top_of_p_table; i++) {
		if (!str_cmp((player_table + i)->name, name))
			return ((player_table + i)->id);
	}
	return -1;
}


char *get_name_by_id(long id)
{
	int i;
	
	for (i = 0; i <= top_of_p_table; i++)
		if ((player_table + i)->id == id)
			return ((player_table + i)->name);
		
	return NULL;
}

int get_clan_by_id(long id)
{
	int i;
	struct char_file_u tmp_store;
	struct char_data *victim = 0;
	
	CREATE(victim, struct char_data, 1);
	clear_char(victim);
	if (load_char(get_name_by_id(id), &tmp_store) > -1) {
		store_to_char(&tmp_store, victim);
		char_to_room(victim, 0);
		i = GET_CLAN(victim);
		extract_char(victim);
		return i;
	}
	
	return -1;
}

/* Load a char, TRUE if loaded, FALSE if not */
int load_char(char *name, struct char_file_u * char_element)
{
	int player_i;
	int find_name(char *name);
	
	if ((player_i = find_name(name)) >= 0) {
		fseek(player_fl, (long) (player_i * sizeof(struct char_file_u)), SEEK_SET);
		fread(char_element, sizeof(struct char_file_u), 1, player_fl);
		return (player_i);
	} 
	else
		return (-1);
}




/*
 * write the vital data of a player to the player file
 *
 * NOTE: load_room should be an *RNUM* now.  It is converted to a vnum here.
 */
void save_char(struct char_data * ch, room_rnum load_room)
{
	struct char_file_u st;
	void update_weight(struct char_data *ch);
	
	if (IS_NPC(ch) || GET_PFILEPOS(ch) < 0)
		return;
	
	update_weight(ch);
	char_to_store(ch, &st);
	
	if (ch->desc) {
		strncpy(st.host, ch->desc->host, HOST_LENGTH);
		st.host[HOST_LENGTH] = '\0';
	}
	
	if (load_room != NOWHERE) {
		st.player_specials_saved.load_room = world[load_room].number;
		GET_LOADROOM(ch)=world[load_room].number;
	}
	
	strcpy(st.pwd, GET_PASSWD(ch));
	
	fseek(player_fl, GET_PFILEPOS(ch) * sizeof(struct char_file_u), SEEK_SET);
	fwrite(&st, sizeof(struct char_file_u), 1, player_fl);
	savereward(ch);
}


void reboot_save_players(void)
{
	
}


/* copy data from the file structure to a char struct */
void store_to_char(struct char_file_u * st, struct char_data * ch)
{
	int i;
	
  /* to save memory, only PC's -- not MOB's -- have player_specials */
	if (ch->player_specials == NULL)
		CREATE(ch->player_specials, struct player_special_data, 1);
	
	GET_SEX(ch) = st->sex;
	GET_CLASS(ch) = st->class;
	GET_LEVEL(ch) = st->level;
	
	ch->player.short_descr = NULL;
	ch->player.long_descr = NULL;
	
	ch->player.description = str_dup(st->description);
	ch->player.title = str_dup(st->title);
	ch->player.poofin = str_dup(st->poofin);
	ch->player.poofout = str_dup(st->poofout);
	
	ch->player.hometown = st->hometown;
	ch->player.time.birth = st->birth;
	ch->player.time.played = st->played;
	ch->player.time.logon = time(0);
	
	ch->player.weight = st->weight;
	ch->player.height = st->height;
	
	ch->real_abils = st->abilities;
	ch->aff_abils = st->abilities;
	ch->points = st->points;
	ch->char_specials.saved = st->char_specials_saved;
	ch->player_specials->saved = st->player_specials_saved;
	
	if (ch->points.max_mana < 100)
		ch->points.max_mana = 100;
	
	ch->char_specials.carry_weight = 0;
	ch->char_specials.carry_items = 0;
	ch->char_specials.hitgain = 0;
	ch->char_specials.managain = 0;
	ch->points.armor = 100;
	ch->points.hitroll = 0;
	ch->points.damroll = 0;
	ch->resistenze.res_fuoco = 0;
	ch->resistenze.res_ghiaccio = 0;
	ch->resistenze.res_elettricita = 0;
	ch->resistenze.res_acido = 0;
	ch->resistenze.res_shaarr = 0;
	ch->resistenze.res_xhyphys = 0;
	ch->resistenze.res_therion = 0;
	ch->resistenze.res_silue = 0;
	ch->resistenze.res_fisico = 0;
	ch->char_specials.secondi_senza_dormire = 0;
	ch->char_specials.catarsi = 0;
	ch->char_specials.distorto = 0;
	
	if (ch->player.name == NULL)
		CREATE(ch->player.name, char, strlen(st->name) + 1);
	strcpy(ch->player.name, st->name);
	strcpy(ch->player.passwd, st->pwd);
	
  /* Add all spell effects */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			affect_to_char(ch, &st->affected[i]);
	}
	
  /*
   * If you're not poisioned and you've been away for more than an hour of
   * real time, we'll set your HMV back to full
   */
	
	if (!IS_AFFECTED(ch, AFF_POISON) && !IS_AFFECTED(ch, AFF_ACCAMPATO) &&
		(((long) (time(0) - st->last_logon)) >= SECS_PER_REAL_HOUR)) {
		GET_HIT(ch) = GET_MAX_HIT(ch);
		GET_MOVE(ch) = GET_MAX_MOVE(ch);
		GET_MANA(ch) = GET_MAX_MANA(ch);
	}
	
	REMOVE_BIT(AFF_FLAGS(ch), AFF_ACCAMPATO);
	
  //Temporaneo!!!!
 //	ch->points.fama = 0;
 //	ch->points.notorieta = 500;  
	
  /*Adriano*/
	times_events_ini(ch);
	loadreward(ch);
  /*Adriano*/ 
	
}				/* store_to_char */

/* copy vital data from a players char-structure to the file structure */
void char_to_store(struct char_data * ch, struct char_file_u * st)
{
	int i;
	struct affected_type *af;
	struct obj_data *char_eq[NUM_WEARS];
	sh_int hit,mana,move;
	
	hit = GET_HIT(ch);
	mana = GET_MANA(ch);
	move = GET_MOVE(ch);
	
  /* Unaffect everything a character can be affected by */
	
	for (i = 0; i < NUM_WEARS; i++) {
		if (GET_EQ(ch, i))
			char_eq[i] = unequip_char(ch, i);
		else
			char_eq[i] = NULL;
	}
	
	for (af = ch->affected, i = 0; i < MAX_AFFECT; i++) {
		if (af) {
			st->affected[i] = *af;
			st->affected[i].next = 0;
			af = af->next;
		} else {
			st->affected[i].type = 0;	/* Zero signifies not used */
			st->affected[i].duration = 0;
			st->affected[i].modifier = 0;
			st->affected[i].location = 0;
			st->affected[i].bitvector[0] = 0;
			st->affected[i].bitvector[1] = 0;
			st->affected[i].bitvector[2] = 0;
			st->affected[i].bitvector[3] = 0;
			st->affected[i].next = 0;
		}
	}
	
	
  /*
   * remove the affections so that the raw values are stored; otherwise the
   * effects are doubled when the char logs back in.
   */
	
	while (ch->affected)
		affect_remove(ch, ch->affected);
	
	if ((i >= MAX_AFFECT) && af && af->next)
		log("SYSERR: WARNING: OUT OF STORE ROOM FOR AFFECTED TYPES!!!");
	
	ch->aff_abils = ch->real_abils;
	
	st->birth = ch->player.time.birth;
	st->played = ch->player.time.played;
	st->played += (long) (time(0) - ch->player.time.logon);
	st->last_logon = time(0);
	
	ch->player.time.played = st->played;
	ch->player.time.logon = time(0);
	
	st->hometown = ch->player.hometown;
	st->weight = GET_WEIGHT(ch);
	st->height = GET_HEIGHT(ch);
	st->sex = GET_SEX(ch);
	st->class = GET_CLASS(ch);
	st->level = GET_LEVEL(ch);
	st->abilities = ch->real_abils;
	st->points = ch->points;
	st->char_specials_saved = ch->char_specials.saved;
	st->player_specials_saved = ch->player_specials->saved;
	
	st->points.armor = 100;
	st->points.hitroll = 0;
	st->points.damroll = 0;
	
	if (POOFIN(ch))
		strcpy(st->poofin, POOFIN(ch));
	else
		*st->poofin='\0';
	if (POOFOUT(ch))
		strcpy(st->poofout, POOFOUT(ch));
	else
		*st->poofout='\0';
	
	if (ch->player.description)
		strcpy(st->description, ch->player.description);
	else
		*st->description = '\0';
	
	if (GET_TITLE(ch))
		strcpy(st->title, GET_TITLE(ch));
	else
		*st->title = '\0';
	
	strcpy(st->name, GET_NAME(ch));
	
  /* add spell and eq affections back in now */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			affect_to_char(ch, &st->affected[i]);
	}
	
	for (i = 0; i < NUM_WEARS; i++) {
		if (char_eq[i])
			equip_char(ch, char_eq[i], i);
	}
/*   affect_total(ch); unnecessary, I think !?! */
/*   restore our points to previous level */
	GET_HIT(ch) = hit;
	GET_MANA(ch) = mana;
	GET_MOVE(ch) = move;
}				/* Char to store */



void save_etext(struct char_data * ch)
{
/* this will be really cool soon */
	
}


/* create a new entry in the in-memory index table for the player file */
int create_entry(char *name)
{
	int i;
	
	if (top_of_p_table == -1) {
		CREATE(player_table, struct player_index_element, 1);
		top_of_p_table = 0;
	} 
	else if (!(player_table = (struct player_index_element *)
				realloc(player_table, sizeof(struct player_index_element) *
					(++top_of_p_table + 1)))) {
		perror("create entry");
		exit(1);
	}
	CREATE(player_table[top_of_p_table].name, char, strlen(name) + 1);
	
  /* copy lowercase equivalent of name to table field */
	for (i = 0; (*(player_table[top_of_p_table].name + i) = LOWER(*(name + i)));
		i++);
	
	return (top_of_p_table);
}



/************************************************************************
*  funcs of a (more or less) general utility nature			*
********************************************************************** */


/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string(FILE * fl, char *error)
{
	char buf[MAX_STRING_LENGTH], tmp[512], *rslt;
	register char *point;
	int done = 0, length = 0, templength = 0;
	
	*buf = '\0';
	
	do {
		if (!fgets(tmp, 512, fl)) {
			fprintf(stderr, "SYSERR: fread_string: format error at or near %s\n",
				error);
			exit(1);
		}
    /* If there is a '~', end the string; else put an "\r\n" over the '\n'. */
		if ((point = strchr(tmp, '~')) != NULL) {
			*point = '\0';
			done = 1;
		} else {
			point = tmp + strlen(tmp) - 1;
			*(point++) = '\r';
			*(point++) = '\n';
			*point = '\0';
		}
		
		templength = strlen(tmp);
		
		if (length + templength >= MAX_STRING_LENGTH) {
			log("SYSERR: fread_string: string too large (db.c)");
			log(error);
			exit(1);
		} else {
			strcat(buf + length, tmp);
			length += templength;
		}
	} while (!done);
	
  /* allocate space for the new string and copy it */
	if (strlen(buf) > 0) {
		CREATE(rslt, char, length + 1);
		strcpy(rslt, buf);
	} else
		rslt = NULL;
	
	return rslt;
}


/* release memory allocated for a char struct */
void free_char(struct char_data * ch)
{
	int i;
	struct alias *a;
	void free_alias(struct alias * a);
	void event_cancel(struct event *event);
	
	
	if (ch->player_specials != NULL && ch->player_specials != &dummy_mob) {
		while ((a = GET_ALIASES(ch)) != NULL) {
			GET_ALIASES(ch) = (GET_ALIASES(ch))->next;
			free_alias(a);
		}
		for (i=0; i < TELLS_STORED; i++)
			if (ch->player_specials->previous_tells[i])
				free(ch->player_specials->previous_tells[i]);
		free(ch->player_specials);
		if (IS_NPC(ch))
			log("SYSERR: Mob had player_specials allocated!");
	}
	if (!IS_NPC(ch) || (IS_NPC(ch) && GET_MOB_RNUM(ch) == -1)) {
    /* if this is a player, or a non-prototyped non-player, free all */
		if (GET_NAME(ch))
			free(GET_NAME(ch));
		if (ch->player.title)
			free(ch->player.title);
		if (ch->player.poofin)
			free(ch->player.poofin);
		if (ch->player.poofout)
			free(ch->player.poofout);
		if (ch->player.short_descr)
			free(ch->player.short_descr);
		if (ch->player.long_descr)
			free(ch->player.long_descr);
		if (ch->player.description)
			free(ch->player.description);
	} else if ((i = GET_MOB_RNUM(ch)) > -1) {
    /* otherwise, free strings only if the string is not pointing at proto */
		if (ch->player.name && ch->player.name != mob_proto[i].player.name)
			free(ch->player.name);
		if (ch->player.title && ch->player.title != mob_proto[i].player.title)
			free(ch->player.title);
		if (ch->player.poofin && ch->player.poofin != mob_proto[i].player.poofin)
			free(ch->player.poofin);
		if (ch->player.poofout && ch->player.poofout != mob_proto[i].player.poofout)
			free(ch->player.poofout);
		if (ch->player.short_descr && ch->player.short_descr != mob_proto[i].player.short_descr)
			free(ch->player.short_descr);
		if (ch->player.long_descr && ch->player.long_descr != mob_proto[i].player.long_descr)
			free(ch->player.long_descr);
		if (ch->player.description && ch->player.description != mob_proto[i].player.description)
			free(ch->player.description);
	}
	
    /*free GO info*/
	if (ch->mob_specials.go_nametarget){
		free(ch->mob_specials.go_nametarget);
		ch->mob_specials.go_nametarget = NULL;
	}
	if (ch->mob_specials.go_path){
		free(ch->mob_specials.go_path);
		ch->mob_specials.go_path = NULL;
	}
	if (ch->mob_specials.searchcommand){
		free(ch->mob_specials.searchcommand);
		ch->mob_specials.searchcommand = NULL;
	}
	
	while (ch->affected)
		affect_remove(ch, ch->affected);
	
  /*By Lance (Shade) risolve i problemi del GET_COND e sgancia gli event*/
  /* cancel point update */
	for (i = 0; i < 3; i++)
		if (GET_POINTS_EVENT(ch, i))
		{
			event_cancel(GET_POINTS_EVENT(ch, i));
			GET_POINTS_EVENT(ch, i) = NULL;
		}
		
	for (i = 0; i < MAX_ACT_EVENTS; i++)
		if (GET_POINTS_ACT_EVENTS(ch, i))
		{
			event_cancel(GET_POINTS_ACT_EVENTS(ch, i));
			GET_POINTS_ACT_EVENTS(ch, i) = NULL;;
		}
		
	free(ch);
}




/* release memory allocated for an obj struct */
void free_obj(struct obj_data * obj)
{
	int nr;
	struct extra_descr_data *this, *next_one;
	
	if ((nr = GET_OBJ_RNUM(obj)) == -1) {
		if (obj->name)
			free(obj->name);
		if (obj->description)
			free(obj->description);
		if (obj->short_description)
			free(obj->short_description);
		if (obj->action_description)
			free(obj->action_description);
		if (obj->ex_description)
			for (this = obj->ex_description; this; this = next_one) {
				next_one = this->next;
				if (this->keyword)
					free(this->keyword);
				if (this->description)
					free(this->description);
				free(this);
			}
	} else {
		if (obj->name && obj->name != obj_proto[nr].name)
			free(obj->name);
		if (obj->description && obj->description != obj_proto[nr].description)
			free(obj->description);
		if (obj->short_description && obj->short_description != obj_proto[nr].short_description)
			free(obj->short_description);
		if (obj->action_description && obj->action_description != obj_proto[nr].action_description)
			free(obj->action_description);
		if (obj->ex_description && obj->ex_description != obj_proto[nr].ex_description)
			for (this = obj->ex_description; this; this = next_one) {
				next_one = this->next;
				if (this->keyword)
					free(this->keyword);
				if (this->description)
					free(this->description);
				free(this);
			}
	}
	
	free(obj);
}



/* read contets of a text file, alloc space, point buf to it */
int file_to_string_alloc(char *name, char **buf)
{
	char temp[MAX_STRING_LENGTH];
	
	if (*buf)
		free(*buf);
	
	if (file_to_string(name, temp) < 0) {
		*buf = "";
		return -1;
	} else {
		*buf = str_dup(temp);
		return 0;
	}
}


/* read contents of a text file, and place in buf */
int file_to_string(char *name, char *buf)
{
	FILE *fl;
	char tmp[READ_SIZE+3];
	
	*buf = '\0';
	
	if (!(fl = fopen(name, "r"))) {
		sprintf(tmp, "Error reading %s", name);
		perror(tmp);
		return (-1);
	}
	do {
		fgets(tmp, READ_SIZE, fl);
		tmp[strlen(tmp) - 1] = '\0'; /* take off the trailing \n */
		strcat(tmp, "\r\n");
		
		if (!feof(fl)) {
			if (strlen(buf) + strlen(tmp) + 1 > MAX_STRING_LENGTH) {
				sprintf(buf, "SYSERR: %s: string too big (%d max)", name,
					MAX_STRING_LENGTH);
				log(buf);
				*buf = '\0';
				return -1;
			}
			strcat(buf, tmp);
		}
	} while (!feof(fl));
	
	fclose(fl);
	
	return (0);
}



/* clear some of the the working variables of a char */
void reset_char(struct char_data * ch)
{
	int i;
	
	for (i = 0; i < NUM_WEARS; i++)
		GET_EQ(ch, i) = NULL;
	
	ch->followers = NULL;
	ch->master = NULL;
	ch->in_room = 1201;
	ch->carrying = NULL;
	ch->next = NULL;
	ch->next_fighting = NULL;
	ch->next_in_room = NULL;
	FIGHTING(ch) = NULL;
	ch->char_specials.position = POS_STANDING;
	ch->mob_specials.default_pos = POS_STANDING;
	ch->char_specials.carry_weight = 0;
	ch->char_specials.carry_items = 0;
	ASSASSIN(ch) = NULL;
	if (GET_HIT(ch) <= 0)
		GET_HIT(ch) = 1;
	if (GET_MOVE(ch) <= 0)
		GET_MOVE(ch) = 1;
	if (GET_MANA(ch) <= 0)
		GET_MANA(ch) = 1;
	
	check_regen_rates(ch);  /*starts regening points*/
	times_events_ini(ch);
	
	GET_LAST_TELL(ch) = NOBODY;
}



/* clear ALL the working variables of a char; do NOT free any space alloc'ed */
void clear_char(struct char_data * ch)
{
	memset((char *) ch, 0, sizeof(struct char_data));
	
	ch->in_room = NOWHERE;
	GET_PFILEPOS(ch) = -1;
	GET_WAS_IN(ch) = NOWHERE;
	GET_POS(ch) = POS_STANDING;
	ch->mob_specials.default_pos = POS_STANDING;
	
	GET_AC(ch) = 100;		/* Basic Armor */
	if (ch->points.max_mana < 100)
		ch->points.max_mana = 100;
}


void clear_object(struct obj_data * obj)
{
	memset((char *) obj, 0, sizeof(struct obj_data));
	
	obj->item_number = NOTHING;
	obj->in_room = NOWHERE;
}


/* initialize a new character only if class is set */
void init_char(struct char_data * ch)
{
	int i;
	
  /* create a player_special structure */
	if (ch->player_specials == NULL)
		CREATE(ch->player_specials, struct player_special_data, 1);
	
  /* *** if this is our first player --- he be God *** */
	
	if (top_of_p_table == 0) {
		GET_EXP(ch) =980000000;
		GET_LEVEL(ch) = LVL_IMPL;
		
		ch->points.max_hit = 1000;
		ch->points.max_mana =1000;
		ch->points.max_move =1000;
	}
	set_title(ch, NULL);
	
	ch->player.short_descr = NULL;
	ch->player.long_descr = NULL;
	ch->player.description = NULL;
	
	ch->player.time.birth = time(0);
	ch->player.time.played = 0;
	ch->player.time.logon = time(0);
	
	for (i = 0; i < MAX_TONGUE; i++)
		GET_TALK(ch, i) = 0;
	
  /* make favors for sex */
	if (ch->player.sex == SEX_MALE) {
		ch->player.weight = number(120, 180);
		ch->player.height = number(160, 200);
	} else {
		ch->player.weight = number(100, 160);
		ch->player.height = number(150, 180);
	}
	
	ch->points.max_mana = 100;
	ch->points.mana = GET_MAX_MANA(ch);
	ch->points.hit = GET_MAX_HIT(ch);
	ch->points.max_move = 150;
	ch->points.move = GET_MAX_MOVE(ch);
	ch->points.armor = 100;
	
	player_table[top_of_p_table].id = GET_IDNUM(ch) = ++top_idnum;
	GET_GOLD(ch)=100;
	for (i = 1; i <= MAX_SKILLS; i++) {
		if (GET_LEVEL(ch) < LVL_IMPL)
			SET_SKILL(ch, i, 0)
		else
			SET_SKILL(ch, i, 100);
	}
	
	ch->char_specials.saved.affected_by[0] = 0;
	ch->char_specials.saved.affected_by[1] = 0;
	ch->char_specials.saved.affected_by[2] = 0;
	ch->char_specials.saved.affected_by[3] = 0;
	
	for (i = 0; i < 5; i++)
		GET_SAVE(ch, i) = 0;
	
  /* ch->real_abils.intel = 25;
  ch->real_abils.wis = 25;
  ch->real_abils.dex = 25;
  ch->real_abils.str = 25;
  ch->real_abils.str_add = 100;
  ch->real_abils.con = 25;
  ch->real_abils.cha = 25;*/
	
  /* GO info*/
	ch->mob_specials.go_nametarget = NULL;
	ch->mob_specials.go_path = NULL;
	ch->mob_specials.searchcommand = NULL;
	
	for (i = 0; i < 3; i++)
		GET_COND(ch, i) = (GET_LEVEL(ch) == LVL_IMPL ? -1 : 90);
	
	GET_WILD_MAX_X_RANGE(ch) = 8;
	GET_WILD_MAX_Y_RANGE(ch) = 6;
	
	GET_NOTORIETA(ch)=500;
	GET_FAMA(ch)=0;
	GET_LOADROOM(ch) = NOWHERE;
	
}



/* returns the real number of the room with given virtual number */
room_rnum real_room(room_vnum virtual)
{
	int bot, top, mid;
	int wild,wildx,wildy;
	
	bot = 0;
	top = top_of_world;
	
	if (virtual>=MINIWILD_ORIGIN)
	{
		wild  = virtual-MINIWILD_ORIGIN;
		wild  = wild % (MINIWILD_XMEMSIZE * MINIWILD_YMEMSIZE);
		wildx = wild % MINIWILD_XMEMSIZE;
		wildy = wild / MINIWILD_YMEMSIZE;
		if (wildx<MINIWILD_RXOR || wildx>MINIWILD_RXEN || wildy<MINIWILD_RYOR || wildy > MINIWILD_RYEN) return -1;
	}
	
	else if (virtual>=WILD_ORIGIN)
	{
		wild=virtual-WILD_ORIGIN;
		wildx= wild % WILD_XMEMSIZE;
		wildy= wild / WILD_YMEMSIZE;
		if (wildx<WILD_RXOR || wildx>WILD_RXEN || wildy<WILD_RYOR || wildy > WILD_RYEN) return -1;
	}
	
  /* perform binary search on world-table */
	for (;;) {
		mid = (bot + top) / 2;
		
		if ((world + mid)->number == virtual)
			return mid;
		if (bot >= top)
			return NOWHERE;
		if ((world + mid)->number > virtual)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}


void save_char_file_u(struct char_file_u st)
{
	int player_i;
	int find_name(char *name);
	
	if((player_i = find_name(st.name)) >=0 )
	{
		fseek(player_fl, player_i * sizeof(struct char_file_u), SEEK_SET);
		fwrite(&st, sizeof(struct char_file_u), 1, player_fl);
	}
}


/* returns the real number of the monster with given virtual number */
int real_mobile(int virtual)
{
	int bot, top, mid;
	
	bot = 0;
	top = top_of_mobt;
	
  /* perform binary search on mob-table */
	for (;;) {
		mid = (bot + top) / 2;
		
		if ((mob_index + mid)->virtual == virtual)
			return (mid);
		if (bot >= top)
			return (-1);
		if ((mob_index + mid)->virtual > virtual)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}



/* returns the real number of the object with given virtual number */
int real_object(int virtual)
{
	int bot, top, mid;
	
	bot = 0;
	top = top_of_objt;
	
  /* perform binary search on obj-table */
	for (;;) {
		mid = (bot + top) / 2;
		
		if ((obj_index + mid)->virtual == virtual)
			return (mid);
		if (bot >= top)
			return (-1);
		if ((obj_index + mid)->virtual > virtual)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}

int num_obj_in_zone (obj_rnum obj_num, int zone_num)
{
	room_rnum room = 1;
	int count = 0;
	
	/* Se è una wild non contare il numero degli obj, area troppo grande! 
		conta solo il numero degli obj nella room*/
	if (zone_table[zone_num].wilderness==ZONE_WILDERNESS)
		return num_obj_in_room(obj_num, real_room(room));
	
	for (room=(zone_table[zone_num].number*100); room <= zone_table[zone_num].top; room++)
		count += num_obj_in_room(obj_num, real_room(room));
	
	return count;
}

int num_obj_in_room (obj_rnum obj_num, room_rnum room)
{
	int count = 0;
	struct obj_data *object;
	struct char_data *k;
	int i;
	
	if (room<0 || room>top_of_world) return 0;
	
	for (object=world[room].contents; object; object=object->next_content)
	{
		if (object->contains) count += num_obj_in_obj(obj_num, object);
		if (object->item_number==obj_num) count ++;
	}
	
	for (k=world[room].people; k; k=k->next_in_room)
	{
		for (i=0; i<NUM_WEARS; i++)
			if (GET_EQ(k,i))
			{
				if (GET_EQ(k,i)->contains) count += num_obj_in_obj(obj_num, GET_EQ(k,i));
				if (GET_OBJ_RNUM(GET_EQ(k,i))== obj_num) count ++;
			}
		for (object=k->carrying; object; object=object->next_content)
		{
			if (object->contains) count += num_obj_in_obj(obj_num, object);
			if (object->item_number==obj_num) count ++;
		}
	}
	return count;
}

int num_obj_in_obj (obj_rnum obj_num, struct obj_data * object)
{
	int count=0;
	struct obj_data *cont;
	
	for (cont=object->contains; cont; cont=cont->next_content)
	{
		if (cont->contains) count += num_obj_in_obj(obj_num, cont);
		if (cont->item_number==obj_num) count++;
	}
	return count;
}

/* Adriano - times_event_ini*/
void times_events_ini (struct char_data *ch)
{
	struct last_same_com *ev=ch->last_same_command;
	long int count;
	
	for (count = 0; count < NUM_EVENTS;count++)
	{
		if (ch->last_same_command == NULL)
		{
			CREATE(ch->last_same_command, struct last_same_com, 1);
			ev = ch->last_same_command;
		}
		else
		{
			CREATE(ev->next, struct last_same_com, 1);
			ev = ev->next;
		}
	// By Spini: impedisce il picklock nei primi 60 secondi di gioco, per rendere inutile il
	// trucchetto di campare o quittare tra un pick e l'altro
		ev->times = ((count == LAST_PICK) || (count == LAST_AFK)) ? time(NULL) : 0;
		ev->type_event = count;
	}
}


 /* Lance (Adriano micale) Save mob */

 /* generate index table for the mobs file */
void build_mob_index(void)
{
	int nr = -1, i;
	long size, recs;
	struct smob_file_u dummy;
	
	if (!(mobs_fl = fopen(MOBS_FILE, "r+b"))) {
		if (errno != ENOENT) {
			perror("fatal error opening mobfile");
			exit(1);
		} else {
			log("No mobfile.  Creating a new one.");
			touch(MOBS_FILE);
			if (!(mobs_fl = fopen(MOBS_FILE, "r+b"))) {
				perror("fatal error opening mobsfile");
				exit(1);
			}
		}
	}
	
	fseek(mobs_fl, 0L, SEEK_END);
	size = ftell(mobs_fl);
	rewind(mobs_fl);
	if (size % sizeof(struct smob_file_u))
		fprintf(stderr, "\nWARNING:  MOBSFILE IS PROBABLY CORRUPT!\n");
	recs = size / sizeof(struct smob_file_u);
	if (recs) {
		sprintf(buf, "   %ld mobs in database.", recs);
		log(buf);
		CREATE(mobs_table, struct mob_index_element, recs);
	} else {
		mobs_table = NULL;
		top_of_m_file = top_of_m_table = -1;
		return;
	}
	
	for (; !feof(mobs_fl);) {
		fread(&dummy, sizeof(struct smob_file_u), 1, mobs_fl);
		if (!feof(mobs_fl)) {	/* new record */
			nr++;
			CREATE(mobs_table[nr].name, char, strlen(dummy.name) + 1);
			for (i = 0;
				(*(mobs_table[nr].name + i) = LOWER(*(dummy.name + i))); i++);
			mobs_table[nr].idmob = dummy.idmob;
			mobs_table[nr].level = dummy.level;
			mobs_table[nr].vnum  = dummy.mob_vnum;
			top_idmobnum = MAX(top_idmobnum, dummy.idmob);
		}
	}
	
	top_of_m_file = top_of_m_table = nr;
	
}

long get_idmob_by_name(char *name)
{
	int i;
	
	for (i = 0; i <= top_of_m_table; i++)
		if (!str_cmp((mobs_table + i)->name, name))
			return ((mobs_table + i)->idmob);
		
	return -1;
}


char *get_name_by_idmob(long id)
{
	int i;
	
	for (i = 0; i <= top_of_m_table; i++)
		if ((mobs_table + i)->idmob == id)
			return ((mobs_table + i)->name);
		
	return NULL;
}



 /* locate entry in m_table with entry->name == name. -1 mrks failed search */
int find_mobname(char *name)
{
	int i;
	
	for (i = 0; i <= top_of_m_table; i++) {
		if (!str_cmp((mobs_table + i)->name, name))
			return i;
	}
	
	return -1;
}


 /* Load a mob, TRUE if loaded, FALSE if not */
long load_mob(long mob_i, struct smob_file_u * char_element)
{
	if (mob_i >= 0) {
		fseek(mobs_fl, (long) (mob_i * sizeof(struct smob_file_u)), SEEK_SET);
		fread(char_element, sizeof(struct smob_file_u), 1, mobs_fl);
		return (mob_i);
	} else
		return (-1);
}

void save_mob(struct char_data * ch, room_rnum load_room)
{
	struct smob_file_u st;
	
   //ottiene l' idmob se non lo ha
	if (GET_MOB_ID(ch) <= 0) {
		GET_MOB_ID(ch) = top_idmobnum + 1;
		// Con questa non mi salva il nome dei Manovali con nome Cambiato!
		//if ((ch->player.name) && (GET_MOB_RNUM(ch)> -1))
		//ch->player.name = str_dup(mob_proto[GET_MOB_RNUM(ch)].player.name);
		create_entrymob(ch->player.name);
		mobs_table[top_of_m_table].idmob = GET_MOB_ID(ch);
		mobs_table[top_of_m_table].level = GET_LEVEL(ch);
		mobs_table[top_of_m_table].vnum  = GET_MOB_VNUM(ch);
		top_idmobnum += 1;
	}
	
   //ottiene la posizione nel file se non lo ha
	if (GET_PFILEPOS(ch) < 0) {
		top_of_m_file += 1;
		GET_PFILEPOS(ch) = top_of_m_file;
	}
	
	mob_to_store(ch, &st);
	
	if (load_room == NOWHERE)
		st.player_specials_saved.load_room = NOWHERE;
	else
		st.player_specials_saved.load_room = world[load_room].number;
	
	fseek(mobs_fl,(long) (GET_PFILEPOS(ch) * sizeof(struct smob_file_u)), SEEK_SET);
	fwrite(&st, sizeof(struct smob_file_u), 1, mobs_fl);
}


 /* copy data from the file structure to a char struct */
void store_to_mob(struct smob_file_u * st, struct char_data * ch)
{
	int i;
	
   /* to save memory, only PC's -- not MOB's -- have player_specials */
	if (ch->player_specials == NULL)
		CREATE(ch->player_specials, struct player_special_data, 1);
	
	GET_MOB_ID(ch) = st->idmob;
	GET_SEX(ch) = st->sex;
	GET_CLASS(ch) = st->class;
	GET_LEVEL(ch) = st->level;
	
	GET_MOB_SAVED_MASTER(ch) = st->savedmaster;
	ch->player.name = str_dup(st->name);
	//ch->player.name[MAX_NAME_LENGTH+1] = '\0';
	ch->player.short_descr = str_dup(st->short_descr);
	//ch->player.short_descr[SHTDESC_LENGTH+1] = '\0';
	ch->player.long_descr = str_dup(st->long_descr);
	ch->player.title = str_dup(st->title);
	ch->player.description = str_dup(st->description);
	ch->player.weight = st->weight;
	ch->player.height = st->height;
	ch->real_abils = st->abilities;
	ch->aff_abils = st->abilities;
	ch->points = st->points;
	ch->char_specials.saved = st->char_specials_saved;
	ch->player_specials->saved = st->player_specials_saved;
	GET_POS(ch) = st->position;
	
   // go
	MASTER_ID(ch)= st->masterid;
	CLAN_ID(ch) = st->clanid;
	ch->mob_specials.pospath = st->pospath;
	ch->mob_specials.lastdir = st->lastdir;
	GO_PATH(ch) = str_dup(st->go_path);
	GO_PG_TARGET(ch) = st->go_chtarget;
	GO_NAME_TARGET(ch) = str_dup(st->go_nametarget);
	GO_ROOM_TARGET(ch)=st->go_room;
	SEARCH_MODE(ch)=st->search_mode;
	CMD_MODE(ch)=st->cmd_mode;
	SEARCH_COMMAND(ch) = str_dup(st->searchcommand);
   //fine go
	//Chardhros - Stipendio Mob
	GET_STIPENDIOMOB(ch) = st->stipendiomob;
	GET_PAGA(ch) = st->paga;
	GET_HOME(ch) = st->home;
	//Chardhros - Fame Mob
	GET_COND(ch,FULL) = (sbyte) st->condition;
	
	if (ch->points.max_mana < 100)
		ch->points.max_mana = 100;
	
	ch->char_specials.carry_weight = 0;
	ch->char_specials.carry_items = 0;
	ch->char_specials.hitgain = 0;
	ch->char_specials.managain = 0;
   //ch->points.armor = 100;
   //ch->points.hitroll = 0;
   //ch->points.damroll = 0;
	
   /* Add all spell effects */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			affect_to_char(ch, &st->affected[i]);
	}
	
   /*load global var trigger*/
	if (ch->script){
		for (i = 0; i < MAX_GBL_VAR_SAVED; i++) {
			if ((strcmp(st->global_var[i].name , "")!=0) && (strcmp(st->global_var[i].name , "#")!=0))
			{
				add_var(&((ch->script)->global_vars), str_dup(st->global_var[i].name),
					str_dup(st->global_var[i].value));
			}
		}
	}
	
   /*
 	* If you're not poisioned and you've been away for more than an hour of
 	* real time, we'll set your HMV back to full
 	*/
	
	
	GET_HIT(ch) = GET_MAX_HIT(ch);
	GET_MOVE(ch) = GET_MAX_MOVE(ch);
	GET_MANA(ch) = GET_MAX_MANA(ch);
	
	GET_LOADROOM(ch) = st->player_specials_saved.load_room;
	
}				/* store_to_char */

 /* copy vital data from a players char-structure to the file structure */
void mob_to_store(struct char_data * ch, struct smob_file_u * st)
{
	int i;
	struct affected_type *af;
	struct trig_var_data *tr;
	struct obj_data *char_eq[NUM_WEARS];
	sh_int hit,mana,move;
   //char emptystrn[MAX_NAME_VAR_LENGTH+1];
   //char emptystr[MAX_VAR_LENGTH+1];
	
	hit = GET_HIT(ch);
	mana = GET_MANA(ch);
	move = GET_MOVE(ch);
	
   /*Save global var trigger*/
	if (ch->script)
		for (tr = (ch->script)->global_vars, i = 0; i < MAX_GBL_VAR_SAVED; i++) {
			if (tr)
			{
				if ((tr->name) && (tr->name!="") && (tr->name!="done"))
					strcpy(st->global_var[i].name, tr->name);
				else
					strcpy(st->global_var[i].name,"#");
// 			*st->global_var[i].name='\0';
				
				if ((tr->value) && (tr->value!=""))
					strcpy(st->global_var[i].value, tr->value);
				else
// 			*st->global_var[i].value='\0';
					strcpy(st->global_var[i].value,"#");
				
				tr = tr->next;
			} else{
 		//*st->global_var[i].name ='\0';
 		//*st->global_var[i].value ='\0';
				strcpy(st->global_var[i].name,"#");
				strcpy(st->global_var[i].value,"#");
			}
		}
		
   /* Unaffect everything a character can be affected by */
		
	for (i = 0; i < NUM_WEARS; i++) {
		if (GET_EQ(ch, i))
			char_eq[i] = unequip_char(ch, i);
		else
			char_eq[i] = NULL;
	}
	
	for (af = ch->affected, i = 0; i < MAX_AFFECT; i++) {
		if (af) {
			st->affected[i] = *af;
			st->affected[i].next = 0;
			af = af->next;
		} else {
			st->affected[i].type = 0;	/* Zero signifies not used */
			st->affected[i].duration = 0;
			st->affected[i].modifier = 0;
			st->affected[i].location = 0;
			st->affected[i].bitvector[0] = 0;
			st->affected[i].bitvector[1] = 0;
			st->affected[i].bitvector[2] = 0;
			st->affected[i].bitvector[3] = 0;
			st->affected[i].next = 0;
		}
	}
	
	
   /*
 	* remove the affections so that the raw values are stored; otherwise the
 	* effects are doubled when the char logs back in.
 	*/
	
	while (ch->affected)
		affect_remove(ch, ch->affected);
	
	if ((i >= MAX_AFFECT) && af && af->next)
		log("SYSERR: WARNING: OUT OF STORE ROOM FOR AFFECTED TYPES!!!");
	
	ch->aff_abils = ch->real_abils;
	
	if ((ch->master) && !IS_NPC(ch->master))
		GET_MOB_SAVED_MASTER(ch) = GET_IDNUM(ch->master);
	else
		GET_MOB_SAVED_MASTER(ch) = -1;
	
	st->savedmaster = GET_MOB_SAVED_MASTER(ch);
	st->weight = GET_WEIGHT(ch);
	st->height = GET_HEIGHT(ch);
	st->sex = GET_SEX(ch);
	st->class = GET_CLASS(ch);
	st->level = GET_LEVEL(ch);
	st->abilities = ch->real_abils;
	st->points = ch->points;
	st->char_specials_saved = ch->char_specials.saved;
	st->player_specials_saved = ch->player_specials->saved;
	st->position = GET_POS(ch);
	
   // go
	st->mob_vnum = GET_MOB_VNUM(ch);
	st->idmob = GET_MOB_ID(ch);
	st->clanid = CLAN_ID(ch);
	st->masterid = MASTER_ID(ch);
	st->pospath = POS_PATH(ch);
	st->lastdir = ch->mob_specials.lastdir;
	
	//Chardhros - Stipendio Mob
	st->stipendiomob = GET_STIPENDIOMOB(ch);
	st->paga = GET_PAGA(ch);
	st->home = GET_HOME(ch);
	
	//Chardhros - Fame MOB
	st->condition = (int) GET_COND(ch,FULL);
	
	if (GO_PATH(ch))
		strcpy(st->go_path, GO_PATH(ch));
	else
		*st->go_path = '\0';
	
	st->go_chtarget = GO_PG_TARGET(ch);
	
	if (GO_NAME_TARGET(ch))
		strcpy(st->go_nametarget, GO_NAME_TARGET(ch));
	else
		*st->go_nametarget = '\0';
	
	st->go_room = GO_ROOM_TARGET(ch);
	st->search_mode = SEARCH_MODE(ch);
	st->cmd_mode=CMD_MODE(ch);
	
	if (SEARCH_COMMAND(ch))
		strcpy(st->searchcommand, SEARCH_COMMAND(ch));
	else
		*st->searchcommand = '\0';
   // fine go
	
   //st->points.armor = 100;
   //st->points.hitroll = 0;
   //st->points.damroll = 0;
	
	if (GET_TITLE(ch))
		strcpy(st->title, GET_TITLE(ch));
	else
		*st->title = '\0';
	
	if (ch->player.description)
		strcpy(st->description, ch->player.description);
	else
		*st->description = '\0';
	
	if (ch->player.name)
		strncpy(st->name, ch->player.name,MAX_NAME_LENGTH);
	else
		*st->name='\0';
	
	if (ch->player.short_descr)
		strncpy(st->short_descr, ch->player.short_descr,SHTDESC_LENGTH);
	else
		*st->short_descr='\0';
	
	if (ch->player.long_descr)
		strncpy(st->long_descr, ch->player.long_descr,LNGDESC_LENGTH);
	else
		*st->long_descr='\0';
	
   /* add spell and eq affections back in now */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			affect_to_char(ch, &st->affected[i]);
	}
	
	for (i = 0; i < NUM_WEARS; i++) {
		if (char_eq[i])
			equip_char(ch, char_eq[i], i);
	}
 /*   affect_total(ch); unnecessary, I think !?! */
 /*   restore our points to previous level */
	GET_HIT(ch) = hit;
	GET_MANA(ch) = mana;
	GET_MOVE(ch) = move;
}				/* Char to store */


 /* create a new entry in the in-memory index table for the mobs file */
int create_entrymob(char *name)
{
	int i;
	
	if (top_of_m_table == -1) {
		CREATE(mobs_table, struct mob_index_element, 1);
		top_of_m_table = 0;
	} else if (!(mobs_table = (struct mob_index_element *)
				realloc(mobs_table, sizeof(struct mob_index_element) *
					(++top_of_m_table + 1)))) {
		perror("create entry");
		exit(1);
	}
	CREATE(mobs_table[top_of_m_table].name, char, strlen(name) + 1);
	
   /* copy lowercase equivalent of name to table field */
	for (i = 0; (*(mobs_table[top_of_m_table].name + i) = LOWER(*(name + i)));
		i++);
	
	return (top_of_m_table);
}

void Load_Savemob(void)
{
	struct char_data *mob;
	struct smob_file_u *dummy;
	long i,nummob = 0;
	
	CREATE(dummy, struct smob_file_u ,1);
	for (i = 0; i <= top_of_m_file; i++) {
		if (load_mob(i, dummy)<0) {
			sprintf(buf,"Error load mob %s",(mobs_table + i)->name);
			mudlog(buf, CMP, LVL_GOD, FALSE);
			return;
		}
		
		mob = read_mobile((mobs_table + i)->vnum, VIRTUAL);
		if (mob) {
			GET_PFILEPOS(mob) = i;
			store_to_mob(dummy, mob);
			char_to_room(mob, real_room(GET_LOADROOM(mob)));
			REMOVE_BIT(MOB_FLAGS(mob), MOB_CRASH);
			if (!MOB_FLAGGED(mob, MOB_SAVE_DELETE)) {
				nummob++;
				if (MOB_FLAGGED(mob,MOB_SAVEINV)){
					Crash_MobEqLoad(mob, GET_MOB_ID(mob));
					REMOVE_BIT(MOB_FLAGS(mob), MOB_CRASH_EQ);
				}
			} //delete
			else {
				REMOVE_BIT(MOB_FLAGS(mob), MOB_SAVE);
				//extract_char(mob);
			}
			
		} else {
			sprintf(buf,"Error load mob file: mob %d",(int)(mobs_table + i)->vnum);
			log(buf);
		}
		
	} // for
	free(dummy);
	fclose(mobs_fl);
	
	if (nummob < i) Pack_mob();
	
	if (!(mobs_fl = fopen(MOBS_FILE, "r+b")))
	{
		mudlog("Error to reopen mobs file", CMP, LVL_GOD, FALSE);
		return;
	}
	
}


void Save_mob(void)
{
	long i=0, indb = 0 , e=0;
	register struct char_data *vict, *next_ch;
	
	mudlog("Save mobs...", CMP, LVL_GOD, FALSE);
	
	for (vict = character_list; vict; vict = next_ch) {
		next_ch = vict->next;
		
		if ((vict) && IS_MOB(vict) && MOB_FLAGGED(vict, MOB_SAVE)) {
			indb++;
			if (MOB_FLAGGED(vict, MOB_CRASH))
			{
				i++;
				REMOVE_BIT(MOB_FLAGS(vict), MOB_CRASH);
				save_mob(vict, vict->in_room);			
			}
			
			if (MOB_FLAGGED(vict,MOB_SAVEINV) && MOB_FLAGGED(vict,MOB_CRASH_EQ)){
				e++;
				Crash_MobEqSave(vict, GET_MOB_ID(vict));
				REMOVE_BIT(MOB_FLAGS(vict), MOB_CRASH_EQ);
			}
			
		}
	}
	
  //fflush(mobs_fl);
	sprintf(buf, "%ld Mobs in Db, %ld Mobs crash saved %ld Eq saved." ,indb, i, e);
	mudlog(buf, CMP, LVL_GOD, FALSE);
}


/*Shade Lance Adriano*/
void Pack_mob(void)
{
	long i=0, indb = 0, n=0;
	register struct char_data *vict, *next_ch;
	
	mudlog("Pack mobs...", CMP, LVL_GOD, FALSE);
	
	if (!(mobs_fl = fopen(MOBS_FILE, "wb")))
	{
		mudlog("Error to rewrite mobs file", CMP, LVL_GOD, FALSE);
		return; 
	}
	
	for (vict = character_list; vict; vict = next_ch) {
		next_ch = vict->next;
		
		if ((vict) && IS_MOB(vict) && MOB_FLAGGED(vict, MOB_SAVE)) {
			indb++;
			GET_PFILEPOS(vict) = i;
			top_of_m_file = i;
			REMOVE_BIT(MOB_FLAGS(vict), MOB_CRASH);
			save_mob(vict, vict->in_room);
			if (MOB_FLAGGED(vict,MOB_SAVEINV)){
				Crash_MobEqSave(vict, GET_MOB_ID(vict));			
			}        
			if (i <= top_of_m_table)
				free(mobs_table[i].name);
			else {
				if (!(mobs_table = (struct mob_index_element *)
							realloc(mobs_table, sizeof(struct mob_index_element) *
								(++top_of_m_table + 1)))) {
					perror("Pack create entry");
					exit(1);
				}
			}
			CREATE(mobs_table[i].name, char, strlen(vict->player.name) + 1);
			for (n = 0;
				(*(mobs_table[i].name + n) = LOWER(*(vict->player.name + n))); n++);
			mobs_table[i].idmob = GET_MOB_ID(vict);
			mobs_table[i].level = GET_LEVEL(vict);
			mobs_table[i].vnum  = GET_MOB_VNUM(vict);
			top_idmobnum = MAX(top_idmobnum, GET_MOB_ID(vict));
			i++;
		}
		top_of_m_table = top_of_m_file;
	}
	fclose(mobs_fl);
	sprintf(buf, "%ld Mobs in Db, %ld Mobs crash saved." ,indb, i);
	mudlog(buf, CMP, LVL_GOD, FALSE);
}


 //Blizzard: define per i mob random :sono i vnum dei mob prototipi
#define MAESTRO_ERR 1
#define CRIMINAL    2
#define VNUM_MAESTRO_ERR 11
#define VNUM_CRIMINAL    25
#define MAX_MAESTRI_ERRANTI 6
#define MAX_CRIMINALI 10

struct char_data *read_random_mobile(int vnum)
{
	extern const char * names[][2];
	int i,sex,level,abil1,abil2,name;
	char tmp_buf[300];
	struct char_data *mob;
	
	if(number(1,10)<=5)
		sex=0;
	else
		sex=1;
	level=number(5,35);
	name=number(0,30);
	bzero(tmp_buf,300);
	
	if ((i = real_mobile(vnum)) < 0) {
		sprintf(buf, "Mobile (V) %d does not exist in database.", vnum);
		return (0);
	}
	CREATE(mob, struct char_data, 1);
	clear_char(mob);
	*mob = mob_proto[i];
	mob->next = character_list;
	character_list = mob;
	mob->player.sex=sex+1;
	mob->real_abils.str = 13;
	mob->real_abils.intel = 16;
	mob->real_abils.wis = 16;
	mob->real_abils.dex = 13;
	mob->real_abils.con = 18;
	mob->real_abils.cha = 18;
	sprintf(buf,names[name][sex]);
	log(buf);
	if(vnum<=VNUM_MAESTRO_ERR+MAX_MAESTRI_ERRANTI){
		strcpy(tmp_buf,names[name][sex]);
		strcat(tmp_buf," maestro viandante");
		strcpy(mob->player.name,tmp_buf);
		strcpy(mob->player.short_descr,"Un maestro errante ");
		bzero(tmp_buf,300);
		strcpy(tmp_buf,names[name][sex]);
		strcat(tmp_buf," e qui vigile e attento.\n");
		strcpy(mob->player.long_descr,tmp_buf);
		bzero(tmp_buf,300);
		abil1=number(1,5);
		abil2=number(1,5);
		while(abil2==abil1)
			abil2=number(1,5);
		mob->char_specials.mob_abil[0]=abil1;
		mob->char_specials.mob_abil[1]=abil2;
		mob->char_specials.mob_abil[2]=number(6,9);
		abil1=number(10,19);
		abil2=number(10,19);
		while(abil2==abil1)
			abil2=number(10,19);
		mob->char_specials.mob_abil[3]=abil1;
		mob->char_specials.mob_abil[4]=abil2;
		level=69;
	}
	
	if(vnum>=VNUM_CRIMINAL&&vnum<VNUM_CRIMINAL+MAX_CRIMINALI){
		strcpy(tmp_buf,names[name][sex]);
		strcat(tmp_buf," criminale passante");
		strcpy(mob->player.name,tmp_buf);
		bzero(tmp_buf,300);
		strcpy(mob->player.short_descr,names[name][sex]);
		strcpy(tmp_buf,names[name][sex]);
		strcat(tmp_buf," e' qui vigile e attento.\n");
		strcpy(mob->player.long_descr,tmp_buf);
		bzero(tmp_buf,300);
		SET_BIT(MOB_FLAGS(mob),MOB_CRIMINAL);
		SET_BIT(MOB_FLAGS(mob), MOB_NOKILL);
		mob->player.level=level;
		mob->real_abils.dex=18;
		mob->real_abils.str=18;
		mob->points.armor=100-(level*4);
	}
	if(vnum>40)
		return NULL;
	mob->points.max_hit=level*18;
	mob->points.hitroll=MAX(7,level/4);
	mob->points.damroll=MIN(20,number(5,level/2));
	mob->mob_specials.damnodice = MAX(5,level/6);
	mob->mob_specials.damsizedice =MAX(2,number(1,level/6));
	mob->points.hit = mob->points.max_hit;
	mob->points.mana = mob->points.max_mana;
	mob->points.move = mob->points.max_move;
	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon = time(0);
	mob_index[i].number++;
	GET_ID(mob) = max_id++;
	return mob;
}

void random_mobiles(void){
	
	int i,top=0,bottom,intervallo=0,zone=0,ultima=0,stanza=0;
	char tmpbuf[100];
	int vector[15]={1,125,66,31,66,125,120,210,81,62,120,70,73,64,52};
	SPECIAL(maestro_errante);
	void ASSIGNMOB(int mob, SPECIAL(fname));
	bottom =  real_room(WILD_VNUM(WILD_RXOR, WILD_RYOR));
	ultima = real_room(WILD_VNUM(WILD_RXEN, WILD_RYEN));
	intervallo=ultima/MAX_MAESTRI_ERRANTI;
	top=bottom+intervallo;
	sprintf(tmpbuf,"Creating %d Maestri Erranti.",MAX_MAESTRI_ERRANTI);
	log(tmpbuf);
	for(i=VNUM_MAESTRO_ERR;i<(VNUM_MAESTRO_ERR+MAX_MAESTRI_ERRANTI);i++){
		while(SECT(stanza=number(bottom,top))>5&&SECT(stanza)!=10);
		char_to_room(read_random_mobile(i),stanza);
		
		ASSIGNMOB(i,maestro_errante);
		bottom+=intervallo;
		top+=intervallo;
		bottom=MIN(bottom,ultima-intervallo);
		top=MIN(top,ultima);
	}
	sprintf(tmpbuf,"Creating %d CRIMINALS.",MAX_CRIMINALI);
	log(tmpbuf);
	
	for(i=VNUM_CRIMINAL;i<(VNUM_CRIMINAL+MAX_CRIMINALI);i++){
		zone=vector[number(0,14)];
		while(real_room(stanza=number(zone*100,zone*100+40))<0 ||
			real_room(stanza) >top_of_world);
		char_to_room(read_random_mobile(i),real_room(stanza));
	}
}


#define DELETEPGTIMEOUT 90

void clear_noplayer_pg()
{
	struct char_file_u fch;
	int i;
	int j=0;
	extern int get_no_play(struct char_data *ch,struct char_file_u chdata,int time_out,int inplay);  
	extern struct char_data *is_playing(char *vict_name);
	
	for (i = 0; (i <= top_of_p_table); i++){
     //cerco tra i pg non giocanti
		if (!is_playing((player_table +i)->name)) {
			load_char((player_table + i)->name, &fch);	
			if (!IS_SET(fch.char_specials_saved.act, PLR_DELETED))
				if (!get_no_play(NULL,fch,DELETEPGTIMEOUT ,FALSE)) {	   
					if ((fch.level < 10)&&(fch.played < 86400)) {
						SET_BIT(fch.char_specials_saved.act, PLR_DELETED);
						save_char_file_u(fch);
						j++;
						sprintf(buf," (GC) Il pg %s e' stato cancellato (non gioca da 3 mesi).",(player_table + i)->name);
						log(buf);		   
					}	   
				} 	  
		}
	}		
	sprintf(buf,"Sono stati cancellati %d players", j);
	log(buf);
	
}

void old_player_restore() {
	struct char_file_u fch;
	FILE *ren_player_fl;

	ren_player_fl = fopen(REN_PLAYER_FILE, "r");
	for (; !feof(ren_player_fl); ) {
		fread(&fch, sizeof(struct char_file_u), 1, ren_player_fl);
		fseek(player_fl, 0L, SEEK_END);
		fwrite(&fch, sizeof(struct char_file_u), 1, player_fl);
	}

	fclose(ren_player_fl);
	ren_player_fl = fopen(REN_PLAYER_FILE, "w");
	fclose(ren_player_fl);
}

void loadreward(struct char_data *vict)
{
	struct reward *rew;
	long idpg,regno,gilda,taglia;
	int flag;
	int vivomorto;
	char descr[255];
	char filename[255];
	char line[256];
//   struct char_data *next_ch;
	
	    //mudlog("Rewards...", CMP, LVL_GOD, FALSE);
	
	
	    /*for (vict = character_list; vict; vict = next_ch) {
  	  next_ch = vict->next;*/
	
	strcpy(descr,"");
	strcpy(filename,"");
	if ((vict) && !IS_NPC(vict)) {
		
		strcat(filename,REWARD_FILE);
		strcat(filename,GET_NAME(vict));
		if (!(reward_fl = fopen(filename, "rb")))
		{
			mudlog("Error to rewrite reward file", CMP, LVL_GOD, FALSE);
			return;
		}
		
		rewind(reward_fl);
		vict->ch_reward = NULL;
		       //while (!feof(reward_fl))  {
		while (get_line(reward_fl,line)) {
			        // get_line(reward_fl,line);
			sscanf(line,"%ld %ld %ld %d %d %ld\n",&idpg,&regno,&gilda,&flag,&vivomorto,&taglia);		
			get_line(reward_fl,descr);
			CREATE(rew, struct reward, 1);
			rew->next = vict->ch_reward;
			vict->ch_reward = rew;
			rew->idpg = idpg;
			rew->regno = regno;
			rew->gilda = gilda;
			rew->flag =  flag;
			rew->taglia =  taglia;
			rew->vivo_morto =  vivomorto;
			if (descr)
				rew->descr = str_dup(descr);
			else
				rew->descr = str_dup("");
		}
	}
	    //}
	fflush(reward_fl);
	fclose(reward_fl);
}


void savereward(struct char_data *vict)
{
	struct reward *rew;
	char filename[255];
	
	    //mudlog("Save Rewards...", CMP, LVL_GOD, FALSE);
	
	strcpy(filename,"");
	
	if ((vict) && !IS_NPC(vict)) {
		strcat(filename,REWARD_FILE);
		strcat(filename,GET_NAME(vict));
		if (!(reward_fl = fopen(filename, "wb")))
		{
			mudlog("Error to rewrite reward file", CMP, LVL_GOD, FALSE);
			return;
		}
		
		rewind(reward_fl);
		for (rew = vict->ch_reward; rew; rew = rew->next) {
			fprintf(reward_fl,"%ld %ld %ld %d %d %ld\n",rew->idpg,rew->regno,rew->gilda,rew->flag,rew->vivo_morto,rew->taglia);
			fprintf(reward_fl,"%s\n",rew->descr);
		}
	}
	fflush(reward_fl);
	fclose(reward_fl);
	
}

int find_flag(struct char_data *vict, int regno, int FLAG)
{
	
	int clan_num;
	struct reward *rew;
	extern sh_int find_clan_by_id(int idnum);
	
	if (!vict) return -1;
	
	if ((clan_num = find_clan_by_id(regno))==-1) return -1;
	
	for (rew = vict->ch_reward; rew; rew = rew->next) {
		if ((rew->regno == clan_num) && (rew->flag == FLAG)) {
			return rew->vivo_morto;
		}
	}
	
	return -1;
}


int pg_flaggato_regno (struct char_data *ch, int regno)
{
	int clan_num;
	struct reward *rew;
	extern sh_int find_clan_by_id(int idnum);

	if (!ch)
		return -1;

	if ((clan_num = find_clan_by_id(regno)) == -1)
		return -1;

	for (rew = ch->ch_reward; rew; rew = rew->next)
		if (rew->regno == clan_num)
			return 1;

	return 0;
}


void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia)
{
	struct reward *rew;
	if (!IS_NPC(ch) && (clan_num>=0) && (GET_LEVEL(ch)<LVL_IMMORT)) {
		CREATE(rew, struct reward, 1);
		rew->next = ch->ch_reward;
		ch->ch_reward = rew;
		if (vict)
			rew->idpg = GET_IDNUM(vict);
		else	
			rew->idpg = -1; 
		rew->regno = clan_num;
		rew->gilda = 0;
		rew->flag = flag;
		rew->taglia = taglia;
		rew->vivo_morto = !strcmp(vivomorto,"vivo") ? 0 : 1;
		rew->descr = str_dup(descr);
		savereward(ch);
	}
}


struct reward *find_crime_by_num(struct char_data *vict, int num)
{
	int cont = 0;  
	struct reward *rew;  
	
	if (vict) {
		for (rew = vict->ch_reward; rew; rew = rew->next){
			cont++;
			if (cont == num) {
				return rew;
			}
		}
	}
	return NULL;
}

int del_crime(struct char_data *vict, int num)
{
	int cont = 0, flag = -1;
	struct reward *old;
	struct reward *rew;  
	
	if (vict) {
		old = vict->ch_reward;
		for (rew = vict->ch_reward; rew; rew = rew->next){
			cont++;
			if (cont == num) {
				flag = rew->flag;
				if (old == rew)
					if (rew->next == NULL)
						vict->ch_reward = NULL;
					else
						vict->ch_reward = rew->next;
				else
					old->next = rew->next;
				free(rew);
				savereward(vict);
				return flag;
			}
			old = rew;
		}
	}
	return -1;
}

int del_crime_flag(struct char_data *ch, struct char_data *vict, int regno, int Dflag)
{
	int cont = 0, flag = -1;
	struct reward *old;
	struct reward *rew;  
	
	if (ch) {
		old = ch->ch_reward;
		for (rew = ch->ch_reward; rew; rew = rew->next){
			cont++;
			if (((rew->idpg==-1) || (vict && (rew->idpg == GET_IDNUM(vict)))) &&
				(rew->regno == regno) && (rew->flag  == Dflag)) {
				flag = rew->flag;
				if (old == rew)
					if (rew->next == NULL)
						ch->ch_reward = NULL;
					else
						ch->ch_reward = rew->next;
				else
					old->next = rew->next;
				free(rew);
				savereward(ch);
				return flag;
			}
			old = rew;
		}
	}
	return -1;
}
