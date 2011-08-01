/* ************************************************************************
*   File: config.c                                      Part of CircleMUD *
*  Usage: Configuration of various aspects of CircleMUD operation         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __CONFIG_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"

#define TRUE	1
#define YES	1
#define FALSE	0
#define NO	0

/*
 * Below are several constants which you can change to alter certain aspects
 * of the way CircleMUD acts.  Since this is a .c file, all you have to do
 * to change one of the constants (assuming you keep your object files around)
 * is change the constant in this file and type 'make'.  Make will recompile
 * this file and relink; you don't have to wait for the whole thing to
 * recompile as you do if you change a header file.
 *
 * I realize that it would be slightly more efficient to have lots of
 * #defines strewn about, so that, for example, the autowiz code isn't
 * compiled at all if you don't want to use autowiz.  However, the actual
 * code for the various options is quite small, as is the computational time
 * in checking the option you've selected at run-time, so I've decided the
 * convenience of having all your options in this one file outweighs the
 * efficency of doing it the other way.
 *
 */

/****************************************************************************/
/****************************************************************************/


/* GAME PLAY OPTIONS */

/*
 * pk_allowed sets the tone of the entire game.  If pk_allowed is set to
 * NO, then players will not be allowed to kill, summon, charm, or sleep
 * other players, as well as a variety of other "asshole player" protections.
 * However, if you decide you want to have an all-out knock-down drag-out
 * PK Mud, just set pk_allowed to YES - and anything goes.
 */
int pk_allowed = NO;

/* is playerthieving allowed? */
int pt_allowed = YES;

/* minimum level a player must be to shout/holler/gossip/auction */
int level_can_shout = 1;

/* number of movement points it costs to holler */
int holler_move_cost = 20;

/* i mob deridono i player che uccidono */
int brag_on = YES;

/* Costo iniziazione ad un ordine */
int costo_iniziazione = 40000;
int punti_aggiuntivi_iniziazione = 10;

/* exp change limits */
int max_exp_loss = 30000000;	/* max losable per death */
int max_exp_solo_gain = 250000;       /* Max xp da soli */
int max_exp_group_gain = 300000;

/* Numero di pratiche richieste per incrementare le caratteristiche */
int prac_per_hit = 5;
int prac_per_stat = 20;

/* Moltiplicatore danni da spell quando caster non e' Pandion */
float no_pandion_damage_modifier = 0.8;

/* Valore delle proficienze dei moob cosi' la piantano di farlo cambiare */
int proficienze_mob = 100;

/* number of tics (usually 75 seconds) before PC/NPC corpses decompose */
int max_npc_corpse_time = 5;
int max_pc_corpse_time = 10;

/* should items in death traps automatically be junked? */
int dts_are_dumps = YES;

/* "okay" etc. */
char *OK = "Okay.\r\n";
char *NOPERSON = "Nessuno con quel nome qui.\r\n";
char *NOEFFECT = "Non accade nulla.\r\n";

/****************************************************************************/
/* VARIE MECCANICHE DI GIOCO */
int mount_max_move_add_per_level = 20;

/****************************************************************************/


/* RENT/CRASHSAVE OPTIONS */

/*
 * Should the MUD allow you to 'rent' for free?  (i.e. if you just quit,
 * your objects are saved at no cost, as in Merc-type MUDs.)
 */
int free_rent = NO;

/* maximum number of items players are allowed to rent */
int max_obj_save = 50;

/* receptionist's surcharge on top of item costs */
int min_rent_cost = 10;

/*
 * Should the game automatically save people?  (i.e., save player data
 * every 4 kills (on average), and Crash-save as defined below.
 */
int auto_save = YES;

/*
 * if auto_save (above) is yes, how often (in minutes) should the MUD
 * Crash-save people's objects?   Also, this number indicates how often
 * the MUD will Crash-save players' houses.
 */
int autosave_time = 10;

/* Lifetime of crashfiles and forced-rent (idlesave) files in days */
int crash_file_timeout = 45;

/* Lifetime of normal rent files in days */
int rent_file_timeout = 90;


/****************************************************************************/
/****************************************************************************/


/* ROOM NUMBERS */
#define NUM_STARTROOMS 6
#define NUM_STARTZONES NUM_STARTROOMS
/* virtual number of room that mortals should enter at */
const room_vnum mortal_start_room[NUM_STARTROOMS +1] =  {
	0,     /* Newbie loadroom element */
	12500, /* Deira                   */
	5401,  /* Jiroch                  */
	2444951,  /* Cimmura                 */
	21208, /* Cyrellos                */
	6601,   /* Lamorka                 */
	50044    /* Alorn */
};
/* virtual number of room that immorts should enter at by default */
room_vnum immort_start_room = 1204;

/* virtual number of room that frozen players should enter at */
room_vnum frozen_start_room = 1202;

const int mortal_start_zone[NUM_STARTZONES+1]=
{
	0,
	125,      /*Deira*/
	54,       /*Jirock*/
	24400,       /*Cimmura*/
	210,      /*Cyrellos*/
	66,        /*Lamorka*/
 	500        /* Alorn */
};

/*
 * virtual numbers of donation rooms.  note: you must change code in
 * do_drop of act.item.c if you change the number of non-NOWHERE
 * donation rooms.
 */
/*room_vnum donation_room_1 = 3063;
room_vnum donation_room_2 = 9028;	
room_vnum donation_room_3 = 12500;
room_vnum donation_lamorka_1 = 6755;
room_vnum donation_lamorka_2 = 6663;
room_vnum donation_lamorka_3 = 6769;
room_vnum donation_lamorka_4 = 6743;
room_vnum donation_lamorka_5 = 6757;*/

room_vnum donation_room_2 = 21295; /*Elenia*/
room_vnum donation_room_3 = 8690; /*Tolnedra*/
room_vnum donation_room_5 = 12517; /*Aldaria*/
room_vnum donation_room_6 = 6602; /*Sendaria*/

/****************************************************************************/
/****************************************************************************/


/* GAME OPERATION OPTIONS */

/*
 * This is the default port the game should run on if no port is given on
 * the command-line.  NOTE WELL: If you're using the 'autorun' script, the
 * port number there will override this setting.  Change the PORT= line in
 * instead of (or in addition to) changing this.
 */
int DFLT_PORT = 6888;

/* default directory to use as data directory */
char *DFLT_DIR = "lib";

/* maximum number of players allowed before game starts to turn people away */
int MAX_PLAYERS = 300;

/* maximum size of bug, typo and idea files in bytes (to prevent bombing) */
int max_filesize = 50000;

/* maximum number of password attempts before disconnection */
int max_bad_pws = 3;

/*
 * Some nameservers are very slow and cause the game to lag terribly every
 * time someone logs in.  The lag is caused by the gethostbyaddr() function
 * which is responsible for resolving numeric IP addresses to alphabetic names.
 * Sometimes, nameservers can be so slow that the incredible lag caused by
 * gethostbyaddr() isn't worth the luxury of having names instead of numbers
 * for players' sitenames.
 *
 * If your nameserver is fast, set the variable below to NO.  If your
 * nameserver is slow, of it you would simply prefer to have numbers
 * instead of names for some other reason, set the variable to YES.
 *
 * You can experiment with the setting of nameserver_is_slow on-line using
 * the SLOWNS command from within the MUD.
 */

int nameserver_is_slow = YES;


const char *MENU =
	"\r\n \033[2J \r\n\n\n"
	"I gestori del mud avvisano i giocatori che, per motivi tecnici, quanto\r\n"
  "accade sul mud viene loggato.\r\n"
  "Tali log saranno utilizzati esclusivamente per la gestione del gioco e non\r\n"
  "saranno in alcun caso forniti a terzi estranei allo staff.\r\n"
  "Il giocatore che accede a Dalila e' consapevole di quanto sopra e lo accetta.\r\n"
  "Grazie a tutti e Buon divertimento!\r\n"
  "\r\n"
	"DALILA 2 MUD ha una propria web page con informazioni e manuali per\r\n"
	"facilitare il gioco, raggiungibile a http://www.dalilaworld.it.\r\n"
	"Nella stessa e' possibile (ma non obbligatorio) registrasi per \r\n"
	"accedere al FORUM di Dalila.\r\n"
	"\r\n"
	"\r\n"
	"&0\r\n"
	"*******-->Benvenuto nel Fantastico mondo di &6DALILA - 2 -&0<--*******\r\n"
	"\r\n"
	"       0) Esci da DALILA.          1) Entra nel gioco.\r\n"
	"       2) Descrivi il personaggio. 3) Leggi la storia.\r\n"
	"       4) Cambia Password.         5) Cancella il personaggio.\r\n"
	"\r\n"
	"******************************************************************\r\n"
	"\r\n"
	"   Scegli: ";



const char *GREETINGS =
	
	"!!MUSIC(evfalls.mid L=1 C=1) \r\n\r\n\r\n"
	"      /\\      .:  *       .                    _                              \r\n"
	"    .'  `.      .     .     *        .-.      / \\        _                    \r\n"
	"   /      \\  _ .________________    /   \\    /^./\\__   _/ \\                   \r\n"
	"  `.+-~~-+.'/.' `.^^^^^^^^\\~~~~~\\.-'\\/\\_ \\__/.      \\ /    \\  ^^  ___         \r\n"
	"    |u--.|  /     \\~~~~~~~|~~~~~|^      \\/  __  :'   /\\/\\  /\\  __/   \\        \r\n"
	"    |   u|.'       `.' '  |' ' '| .'   _/  /  \\   ^ /    \\/  \\/ .`'\\_/\\       \r\n"
	"    |.u-./ _..---.._ \\' ' | ' ' |__  ^/  ^/    `--./.'  ^  `-.\\ _    _:\\ _    \r\n"
	"*   |    ~-|U U U U|-~____L_____L_ \\-' __/.' ^ _   \\_   .'\\   _/>         \\  \r\n"
	".   |.-u.| |..---..|'//// ////// /\\  \\/ -.   _/ \\ -. `_/   \\ /    `._/  ^  \\  \r\n"
	"    |u   | |       |// /// // ///==\\.'    . /    `--./ .-'  `-.  `-. `.  -  `.\r\n"
	"    |.--u| |..---..|//////~\\////====\\    /~\\  .-'   / .   .'   \\    \\  \\  .- ^\r\n"
	"    | u  | |       |~~~~/\\u |~~|++++|~~`/~~~\\'.~-_~~-~~_~-~_~-~~~-_~~-~~_~-~-~\r\n"
	"-   |.-|~U~U~|---..|u u|u | |u ||||||~-~|  U|.~-_~~-~~_~-~_~-~~~-_~~-~~_~-~-~~\r\n"
	" /~~~~/-\\---.'     |===|  |u|==|++++|_~~|   |.~-_~~-~~_~-~_~-~~~-_~~-~~_~-~-~~\r\n"
	" |===| _ | ||.---..|u u|u | |u ||HH||U~U~U~U~|.~-_~~-~~_~-~_~-~~~-_~~-~~_~- ~-\r\n"
	" |===|||||_||      |===|_.|u|_.|+HH+|_/_/_/_/_-=_.~-_~~-~~_~-~_~-~~~-_~~-~~_~-\r\n"
	" |~~|~~~~\\---/~-.._|--.---------.~~~`.__ _.~~-_~~-~~_~-~_~-~~~-_~~-~~_~-~_~-~~\r\n"
	"~~-_~~-~~_~-~_~-~~~-_~~-~~_~-~_~-~~~~-_~~-~~_~-~_~-~~~-_~~-~~_~-~_~-~~~~-_~~-~\r\n"
	"******************************************************************************\r\n"
	"* ________    _____ .____   .___ ___      _____                        .___  *\r\n"
	"* \\______ \\  /  _  \\|    |  |___|   |    /  _  \\  DUE     ___ __ __  __| _/  *\r\n"
	"*  |    |  \\/  /_\\  \\    |  |   |   |   /  /_\\  \\      /     \\  |  \\/ __ |   *\r\n"
	"*  |       \\    |    \\   |__|   |   |___    |    \\    |  Y Y  \\ |  / /_/ |   *\r\n"
	"* /_______  /___|__  /______ \\__|______ \\___|__  /    |__|_|  /___/\\____ |   *\r\n"
	"*         \\/       \\/       \\/         \\/      \\/           \\/          \\/   *\r\n"
	"******************************************************************************\r\n"
	" Credits: J.Elson, H.H.Staerfeldt, K.Nyboe, T.Madsen,S.Hammer,M.Seifert       \r\n"
	"\r\n"
	"     D A L I L A - 2 - M U D...versione modificata di CircleMud              \r\n"
	"\r\n"
	"              ----Seconda era dell'epopea di Dalila----                     \r\n"
	"\r\n"
	" Hanno lavorato al progetto :   Chardhros, Ilweran, Jerold, Kizeridis, Kurtulmak, \r\n"
	"                                Khud, Ferus, Cyrgon, Kabir, Silk, Shade, Offar,  \r\n"
	"                                Orione, Fizban, Sophya \r\n"	
	"\r\n"
	" Lavorano al progetto :         Zarkon, Nimrhuin, Sheep, Rashan, Ardanos.\r\n"
	"\r\n"
	" Staff di sviluppo :            Alamir, Amanu, Dajikesh, Faymond, Phantom, Sephrenia. \r\n"
	"\r\n"
	" Hanno collaborato :            Belgarion, Martel, Nagal, Blizzard             \r\n"
	"\r\n"
  " Un grazie in particolare a Ferus che ospita il nostro server nel suo negozio e a \r\n"
	" tutta la comunita' dei giocatori. \r\n"
	"******************************************************************************\r\n"
	"Inserisci il tuo nome: ";


const char *WELC_MESSG =
	"\r\n"
	"\r\n"
	"BENVENUTO nel mondo di Dalila."
	"\r\n"
	"\r\n"
	"\r\n\r\n";

const char *START_MESSG =
	"\r\n";

const char *BANNER_1 =
  "\r\n&3"                                                                   
  "         ,--.      |    o|             . . .          |        | \r\n"
  "         |   |,---.|    .|    ,---.    | | |,---.,---.|    ,---| \r\n"
  "         |   |,---||    ||    ,---|    | | ||   ||    |    |   | \r\n"
  "         `--' `---^`---'``---'`---^    `-'-'`---'`    `---'`---' \r\n"                                                                      
  "\r\n&0";

const char *AFK_MESSG =
  "\033[2J \r\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n&4"                                                                   
  "         ,--.      |    o|             . . .          |        | \r\n"
  "         |   |,---.|    .|    ,---.    | | |,---.,---.|    ,---| \r\n"
  "         |   |,---||    ||    ,---|    | | ||   ||    |    |   | \r\n"
  "         `--' `---^`---'``---'`---^    `-'-'`---'`    `---'`---' \r\n"
  "&f                          Away From Keyboard                   \r\n"
  "&0&4\r\n"
  "                    [Premi Invio per rientrare in gioco]         \r\n"                                                                     
  "\r\n&0";

/****************************************************************************/
/****************************************************************************/


/* AUTOWIZ OPTIONS */

/* Should the game automatically create a new wizlist/immlist every time
   someone immorts, or is promoted to a higher (or lower) god level? */
int use_autowiz = YES;

/* If yes, what is the lowest level which should be on the wizlist?  (All
   immort levels below the level you specify will go on the immlist instead.) */
int min_wizlist_lev = LVL_GOD;


/****************************************************************************/
/****************************************************************************/

/* Wilderness Options */

int use_wilderness = YES;















