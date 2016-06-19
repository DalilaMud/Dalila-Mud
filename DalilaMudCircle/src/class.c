/**************************************************************************
*   File: class.c              35                       Part of CircleMUD *
*  Usage: Source file for class-specific code                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * This file attempts to concentrate most of the code which must be changed
 * in order for new classes to be added.  If you're adding a new class,
 * you should go through this entire file from beginning to end and add
 * the appropriate new special cases for your new class.
 */



#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "db.h"
#include "utils.h"
#include "comm.h"
#include "spells.h"
#include "interpreter.h"
#include "handler.h"

extern char *proficienze[];
struct nomi_titolanti *pg_titolanti;
int num_titolanti;

/* Names first */

const char *class_abbrevs[] = {
	"Pandion",
	"Cyrinic",
	"Alcione",
	"Genidian",
	"Peloi",
	"Daresiano",
	"\n"
};

const char *pc_class_types[] = {
	"Pandion",
	"Cyrinic",
	"Alcione",
	"Genidian",
	"Peloi",
	"Daresiano",
	"\n"
};

const char *hometown[] = {
	"Deira",
	"Jiroch",
	"Cimmura",
	"Chyrellos",
	"Lamorka",
	"Aldeeran",
	"\n"
};

const char *nomi_religioni[] = {
	"Nessuna",
	"Shaarr",
	"Xhyphys",
	"Silue",
	"Therion",
	"\n",
};

/* The menu for choosing a class in interpreter.c: */
const char *class_menu =
	"\r\n\r\n"
	"In Dalila 2, l'ordine combattente non e' piu' scelto all'inizio del gioco\r\n"
	"ma il personaggio sceglie durante il gioco, quando si sente pronto, in\r\n"
	"quale ordine combattente entrare.\r\n"
	"Tutti iniziano come DARESIANI, cioe' come cittadini normali, non iniziati\r\n"
	"in maniera particolare al combattimento o alla magia, e fintanto che restano\r\n"
	"tali non possono imparare spells o skills, ne' abilita'.\r\n"
	"\r\nA titolo informativo, gli ordini combattenti di Daresia sono i seguenti:\r\n"
	"\r\n"
	"   Ordine             Caratteristiche                     Attributi Innati\r\n"
	"________________________________________________________________________________\r\n"
	"  Cyrinic    Sinceri ed affidabili, legati alla        Intelligenza Saggezza\r\n"
	"             Madre Chiesa\r\n"
	"\r\n"
	"  Alcioni    Veloci e forti, conoscono i segreti       Forza        Destrezza\r\n"
	"             della magia ma ne fanno scarso uso\r\n"
	"\r\n"
	"  Genidian   Alti, forti e robusti.                    Forza        Costituzione\r\n"
	"\r\n"
	"  Pandion    Portati per le arti magiche.              Intelligenza Saggezza\r\n"
	"\r\n"
	"  Peloi      Coraggiosi, intelligenti e astuti.        Costituzione Destrezza\r\n"
	"\r\n"
	"Premi un tasto per continuare la creazione del PG\r\n";


/*
 * The code to interpret a class letter -- used in interpreter.c when a
 * new character is selecting a class and by 'set class' in act.wizard.c.
 */

int parse_class(char arg)
{
	arg = LOWER(arg);
	
	switch (arg) {
		case 'p':
			return CLASS_MAGIC_USER;
			break;
		case 'c':
			return CLASS_CLERIC;
			break;
		case 'g':
			return CLASS_WARRIOR;
			break;
		case 'a':
			return CLASS_THIEF;
			break;
		case 'e':
			return CLASS_PELOI;
			break;
		case 'd':
			return CLASS_DARESIANO;
			break;
		default:
			return CLASS_UNDEFINED;
			break;
	}
}
const char *hometown_menu =
	"\r\n"
	"Ora puoi scegliere la tua citta' natale. Pensaci bene perche'\r\n"
	"ogni Regno ha leggi e usanze diverse che guideranno il tuo\r\n"
	"destino finche' non sarai forte abbastanza.\r\n"
	"\r\n"
	"[D]eira ----(sito in http://dalila.shopitaly.net/Aldaria/welcome.htm):\r\n"
	"            Capitale del Regno di Aldaria, essa e' situata nel mezzo della \r\n"
	"            intera Daresia. Una struttura gestionale molto organizzata, sotto il \r\n"
	"            controllo di Re Rodric, rende questa citta' molto adatta a coloro \r\n"
	"            che intendano iniziare ogni genere di fruttuosa attivita' economica o\r\n"
	"            militare,  mentre il notevole apparato d'informazione permette ai \r\n"
	"            nuovi cittadini di orientarsi rapidamente nel loro nuovo Regno.\r\n"
	"\r\n"
	"[C]immura - Capitale del giovane regno di Tolnedra, ne rappresenta\r\n"
	"            l'anima indipendente e libertaria. Territorialmente\r\n"
	"            collocato nella zona nord-ovest della Daresia, il regno\r\n"
	"            tolnedriano fonda la sua politica interna ed estera sui\r\n"
	"            diritti civili e le liberta' individuali e collettive.\r\n"
	"            Questo lo rende meta ambita non solo dai perseguitati,\r\n"
	"            politici e non, degli altri regni, ma da tutti coloro\r\n"
	"            che vogliono costruirsi con le proprie mani il proprio\r\n"
	"            futuro senza soggiacere a poteri oppressivi e repressivi.\r\n"
	"            Un giovane regno in espansione popolato da eroi,\r\n"
	"            avventurieri, coraggiosi, combattenti...\r\n"
	"            cioe' da uomini liberi.\r\n"
	"\r\n"
	"[L]amorka - Capitale del fiorente regno di Sendaria, si affaccia\r\n"
	"            sul mare a sud ed est. Il suo porto e la vastita' di\r\n"
	"            risorse naturali ne fanno una Citta' ricca e popolosa.\r\n"
	"            Commercio e difesa Militare sono le principali forze del\r\n"
	"            Regno sendariano. Qui i giovani avventurieri trovano\r\n"
	"            aiuto e comodita' per acquisire l'esperienza necessaria\r\n"
	"            a crearsi un futuro militare ed economico.\r\n"
	"\r\n"
	"Ch[y]rellos La citta' santa della Daresia, sede della famosa\r\n" 
	"            Cattedrale e centro di riferimento per i quattro ordini\r\n" 
	"            militari. La linea politica di Re Dolmant e dei suoi\r\n" 
	"            funzionari e', come tradizione dei sovrani eleni, volta\r\n" 
	"            a fare di questo Regno il garante della Pace in Daresia.\r\n" 
	"            La pace e la sicurezza sono i punti di forza di questa\r\n" 
	"            nazione, dove ogni nuovo arrivato e' messo in grado di\r\n" 
	"            lavorare in tranquillita' così da arricchire se stesso\r\n" 
	"            e l'Elenia tutta. La non numerosa popolazione garantisce\r\n" 
	"            inoltre un rapporto diretto tra cittadini e funzionari,\r\n" 
	"            nonche' ampie possibilita' per i meritevoli di avere\r\n" 
	"            parte attiva nelle gestione del Regno.\r\n"	
	"\r\n"
	"Maggiori informazioni sui Regni puoi trovarle sul sito di Dalila.\r\n"
	"  http://dalila.shopitaly.net\r\n"
	"\r\n"
	"Che citta scegli?\r\n";


int membri_religione(int num_religione)
{
	extern FILE *player_fl;

	struct char_file_u st;
	int i = 0;

	rewind(player_fl);

	while (!feof(player_fl)){
		fread (&st, sizeof(struct char_file_u), 1, player_fl);
		if (feof(player_fl))
			break;

		if (*st.name < 'A' || *st.name > 'Z')
			continue;
		if (st.level >= LVL_QUEST_MASTER)
			continue;
		if (IS_SET(st.char_specials_saved.act, PLR_DELETED))
			continue;

		if (st.player_specials_saved.culto == num_religione)
			i++;
	}
	return i;
}


int parse_hometown(char arg)
{
	arg = LOWER(arg);
	
	switch (arg) {
		case 'd':
			return 0;
			break;
		case 'j': // JIROCH - Non e' valida alla nascita !!!!
			return 1;
			break;
		case 'c':
			return 2;
			break;
		case 'y':
			return 3;
			break;
		case 'l':
			return 4;
			break;
		default:
			return 0;
			break;
	}
}


const char *roll_stat_menu =
	"\r\n\r\n\r\n"
	"In Dalila 2 gli attributi di forza, intelligenza, saggezza, destrezza\r\n"
	"costituzione e carisma vengono attribuiti casualmente al personaggio\r\n"
	"dal programma di creazione.\r\n"
	"A differenza della prima versione di Dalila, i punti aggiuntivi, in\r\n"
	"numero di dieci, saranno distribuiti tra gli attributi non in fase di\r\n"
	"creazione del personaggio, ma all'iniziazione in un ordine combattente,\r\n"
	"quindi a gioco iniziato.\r\n"
	"\r\nEcco i tuoi attributi:\r\n\r\n";


int parse_abils(char arg)
{
	arg = LOWER(arg);
	
	switch (arg) {
		case 'f':
			return 0;
			break;
		case 'i':
			return 1;
			break;
		case 's':
			return 2;
			break;
		case 'd':
			return 3;
			break;
		case 'c':
			return 4;
			break;
		case 'a':
			return 5;
			break;
		default:
			return -1;
			break;
	}
}



/*
 * bitvectors (i.e., powers of two) for each class, mainly for use in
 * do_who and do_users.  Add new classes at the end so that all classes
 * use sequential powers of two (1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4,
 * 1 << 5, etc.
 */

long find_class_bitvector(char arg)
{
	arg = LOWER(arg);
	
	switch (arg) {
		case 'p':
			return (1 << 0);
			break;
		case 'c':
			return (1 << 1);
			break;
		case 'a':
			return (1 << 2);
			break;
		case 'g':
			return (1 << 3);
			break;
		case 'e':
			return (1 << 4);
			break;
		case 'd':
			return (1 << 5);
			break;
		default:
			return 0;
			break;
	}
}


/*
 * These are definitions which control the guildmasters for each class.
 *
 * The first field (top line) controls the highest percentage skill level
 * a character of the class is allowed to attain in any skill.  (After
 * this level, attempts to practice will say "You are already learned in
 * this area."
 *
 * The second line controls the maximum percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out higher than this number, the gain will only be
 * this number instead.
 *
 * The third line controls the minimu percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out below this number, the gain will be set up to
 * this number.
 *
 * The fourth line simply sets whether the character knows 'spells'
 * or 'skills'.  This does not affect anything except the message given
 * to the character when trying to practice (i.e. "You know of the
 * following spells" vs. "You know of the following skills"
 */

#define SPELL	0
#define SKILL	1

/* #define LEARNED_LEVEL	0  % known which is considered "learned" */
/* #define MAX_PER_PRAC		1  max percent gain in skill per practice */
/* #define MIN_PER_PRAC		2  min percent gain in skill per practice */
/* #define PRAC_TYPE		3  should it say 'spell' or 'skill'?	*/

int prac_params[4][NUM_CLASSES] = {
  /* MAG	CLE	THE	WAR   PEL   DAR*/
	{100,		100,	100,	100,  100,  100},	/* learned level */
	{10,		10,	10,	5,    6,      5},	/* max per prac */
	{ 6,		 6,	4,	2,    2,      2},	/* min per pac */
	{SPELL, SPELL, SPELL,	SPELL,  SPELL, SKILL}		/* prac name */
};


/*
 * ...And the appropriate rooms for each guildmaster/guildguard; controls
 * which types of people the various guildguards let through.  i.e., the
 * first line shows that from room 3017, only MAGIC_USERS are allowed
 * to go south.
 */
int guild_info[][3] = {
	
/* Deira*/
  /* {CLASS_MAGIC_USER,	3017,	SCMD_SOUTH},
  {CLASS_CLERIC,	3004,	SCMD_NORTH},
  {CLASS_THIEF,		3027,	SCMD_EAST},
  {CLASS_WARRIOR,	3021,	SCMD_EAST},
  {CLASS_PELOI,	        3068,	SCMD_EAST},*/
  /*Deira2*/
	{CLASS_MAGIC_USER,	12547,	SCMD_UP},
	{CLASS_CLERIC,	12554,	SCMD_WEST},
	{CLASS_THIEF,		12667,	SCMD_SOUTH},
	{CLASS_WARRIOR,	12605,	SCMD_EAST},
	{CLASS_PELOI,	        12696,	SCMD_NORTH},
/* Brass Dragon */
	{-999 /* all */ ,	5065,	SCMD_WEST},
	
/* Jiroch */
	{CLASS_MAGIC_USER,	5525,	SCMD_NORTH},
	{CLASS_CLERIC,	5512,	SCMD_SOUTH},
	{CLASS_THIEF,		5532,	SCMD_SOUTH},
	{CLASS_WARRIOR,	5526,	SCMD_SOUTH},
	{CLASS_PELOI,	        5680,	SCMD_NORTH},
	
/* Cimmura */
	{CLASS_MAGIC_USER,	9514,	SCMD_UP},
	{CLASS_CLERIC,	9027,	SCMD_UP},
	{CLASS_THIEF,		9029,	SCMD_UP},
	{CLASS_WARRIOR,	9023,	SCMD_UP},
	{CLASS_PELOI,	        9010,	SCMD_UP},
	
//PEPPEGILDE
/* Cimmura 2*/
	{CLASS_MAGIC_USER,	8685,	SCMD_UP},
	{CLASS_CLERIC,	8674,	SCMD_UP},
	{CLASS_THIEF,		8680,	SCMD_UP},
	{CLASS_WARRIOR,	8683,	SCMD_UP},
	{CLASS_PELOI,	        8677,	SCMD_UP},
	
/* Chyrellos*/
	
	{CLASS_MAGIC_USER,	21312,	SCMD_UP},
	{CLASS_CLERIC,	21241,	SCMD_WEST},
	{CLASS_THIEF,		21320,	SCMD_DOWN},
	{CLASS_WARRIOR,	21309,	SCMD_EAST},
	{CLASS_PELOI,	    21317,	SCMD_WEST},
	
/* Lamorka */
	{CLASS_MAGIC_USER,	6616,	SCMD_UP},
	{CLASS_CLERIC,	6661,	SCMD_WEST},
	{CLASS_THIEF,		6740,	SCMD_EAST},
	{CLASS_THIEF,         6742,   SCMD_WEST},
	{CLASS_WARRIOR,	6652,	SCMD_WEST},
	{CLASS_PELOI,	        6672,	SCMD_SOUTH},
	
/* Aldeeran */
	{CLASS_MAGIC_USER,	50076,	SCMD_UP},
	{CLASS_CLERIC,	50070,	SCMD_UP},
	{CLASS_THIEF,		50171,	SCMD_NORTH},
	{CLASS_WARRIOR,	50057,	SCMD_UP},
	{CLASS_PELOI,	        50073,	SCMD_UP},
	
/* this must go last -- add new guards above! */
	{-1, -1, -1}};



/* THAC0 for classes and levels.  (To Hit Armor Class 0) */

/* [class], [level] (all) */
const int thaco[NUM_CLASSES][LVL_IMPL + 1] = {
	
/* PANDION */
  /* 0                   5                  10                  15		    20			25		    30			35	      40	     45		    50		   55		   60		 65		 70*/
	{100, 20, 20, 20, 20, 20, 19, 19, 19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	
/* CYRINIC */
  /* 0                   5                  10                  15		    20			25		    30			35	      40             45		    50		   55		   60		 65		 70*/
	{100, 20, 20, 20, 20, 19, 19, 19, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	
/* ALCIONI */
  /* 0                   5                  10                  15		    20			25		    30			35		   40			45		   50		      55		  60		      65		  70*/
	{100, 20, 20, 20, 19, 19, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 10, 10,  9,  9,  9,  9, 8,  8,  7,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5, 5,  5,  4,  4,  4,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1},
	
/* GENIDIAN */
  /* 0                   5                  10                  15		    20			25		    30			35		   40		       45		  50		      55		  60		      65		  70*/
	{100, 20, 20, 19, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  9,  8,  8,  7,  7, 7,  6,  6,  6,  5,  5,  5,  5,  4,  4,  4,  4,  3,  3,  3, 3,  2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1},
	
/* PELOI */
  /* 0                   5                  10                  15		    20			25		    30			35		   40			45		   50		      55		  60		      65		  70*/
	{100, 20, 20, 20, 19, 19, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 10, 10,  9,  9,  9,  9, 8,  8,  7,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5, 5,  5,  4,  4,  4,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1},
	
/* DARESIANO = PANDION */
  /* 0                   5                  10                  15		    20			25		    30			35	      40	     45		    50		   55		   60		 65		 70*/
	{100, 20, 20, 20, 20, 20, 19, 19, 19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void boot_titolanti(void)
{
	FILE *titolanti_f;
	struct nomi_titolanti pg;

	num_titolanti = 0;

	if (!(titolanti_f = fopen ("etc/titolanti", "rb"))){
		log ("Nessun file di titolanti trovato. Ne creo uno nuovo...");
		titolanti_f = fopen ("etc/titolanti", "wb");
		pg_titolanti = (struct nomi_titolanti *) malloc (sizeof (struct nomi_titolanti));
	}
	else
		while (!feof (titolanti_f)){
			fread (&pg, sizeof (struct nomi_titolanti), 1, titolanti_f);
			if (feof (titolanti_f))
				break;
	
			num_titolanti++;
			pg_titolanti = (struct nomi_titolanti *) realloc (pg_titolanti, sizeof (struct nomi_titolanti) * num_titolanti);
			pg_titolanti[num_titolanti-1] = pg;
			sprintf (buf, "%do pg caricato.", num_titolanti);
			log (buf);
		}

	fclose (titolanti_f);
}

void roll_simple_abils(struct char_data * ch)
{
	ch->real_abils.str = 0;
	ch->real_abils.intel = 0;
	ch->real_abils.wis = 0;
	ch->real_abils.dex = 0;
	ch->real_abils.con = 0;
	ch->real_abils.cha = 0;
	ch->real_abils.str_add = 0;
	ch->aff_abils.str = 7 + number(1, 5);
	ch->aff_abils.intel = 7 + number (1, 5);
	ch->aff_abils.wis = 7 + number (1, 5);
	ch->aff_abils.dex = 7 + number (1, 5);
	ch->aff_abils.con = 7 + number (1, 5);
	ch->aff_abils.cha = 7 + number (1, 5);
  // NON SETTARE QUI STR_ADD PER AFF_ABILS
}

void accetta_roll(struct char_data * ch)
{
	ch->real_abils.str    = ch->aff_abils.str;
	ch->real_abils.intel  = ch->aff_abils.intel;
	ch->real_abils.wis    = ch->aff_abils.wis;
	ch->real_abils.dex    = ch->aff_abils.dex;
	ch->real_abils.con    = ch->aff_abils.con;
	ch->real_abils.cha    = ch->aff_abils.cha;
	ch->real_abils.str_add = 0;
	ch->aff_abils.str_add = 0;
}

/*
 * Roll the 6 stats for a character... each stat is made of the sum of
 * the best 3 out of 4 rolls of a 6-sided die.  Each class then decides
 * which priority will be given for the best to worst stats.
 */
void roll_real_abils(struct char_data * ch)
{
	int i, k, temp;
	ubyte table[6];
	
	for (i = 0; i < 6; i++)
		table[i] = 0;
	
	for (i = 0; i < 6; i++) {
		
		temp = 7 + number(1, 5);
		
		for (k = 0; k < 6; k++)
			if (table[k] < temp) {
				temp ^= table[k];
				table[k] ^= temp;
				temp ^= table[k];
			}
	}
	
	ch->real_abils.str_add = 0;
	
	switch (GET_CLASS(ch)) {
		case CLASS_MAGIC_USER:
			ch->real_abils.intel = table[0];
			ch->real_abils.wis = table[1];
			ch->real_abils.dex = table[2];
			ch->real_abils.con = table[3];
			ch->real_abils.str = table[4];
			ch->real_abils.cha = table[5];
			break;
		case CLASS_CLERIC:
			ch->real_abils.intel = table[0];
			ch->real_abils.wis = table[1];
			ch->real_abils.dex = table[2];
			ch->real_abils.con = table[3];
			ch->real_abils.str = table[4];
			ch->real_abils.cha = table[5];
			break;
		case CLASS_THIEF:
			ch->real_abils.dex = table[0];
			ch->real_abils.con = table[1];
			ch->real_abils.str = table[2];
			ch->real_abils.intel = table[3];
			ch->real_abils.wis = table[4];
			ch->real_abils.cha = table[5];
			break;
		case CLASS_WARRIOR:
			ch->real_abils.str = table[0];
			ch->real_abils.dex = table[2];
			ch->real_abils.con = table[1];
			ch->real_abils.wis = table[3];
			ch->real_abils.intel = table[4];
			ch->real_abils.cha = table[5];
			if (ch->real_abils.str == 18)
				ch->real_abils.str_add = number(0, 100);
			break;
		case CLASS_PELOI:
			ch->real_abils.dex = table[0];
			ch->real_abils.wis = table[1];
			ch->real_abils.str = table[2];
			ch->real_abils.con = table[3];
			ch->real_abils.intel = table[4];
			ch->real_abils.cha = table[5];
			if (ch->real_abils.str == 18)
				ch->real_abils.str_add = number(0, 100);
			break;
		case CLASS_DARESIANO:
			ch->real_abils.str   = table[2];
			ch->real_abils.dex   = table[1];
			ch->real_abils.con   = table[0];
			ch->real_abils.wis   = table[5];
			ch->real_abils.intel = table[4];
			ch->real_abils.cha   = table[3];
			break;
	}
	ch->aff_abils = ch->real_abils;
}


/* Some initializations for characters, including initial skills */
void do_start(struct char_data * ch)
{
	void advance_level(struct char_data * ch);
	
	GET_LEVEL(ch) = 1;
	GET_EXP(ch) = 1;
	GET_CLASS_TITLE(ch)=0;
	set_title(ch, NULL);
	
	switch (GET_CLASS(ch)) {
		
		case CLASS_MAGIC_USER:
			ch->points.max_hit = 11;
			break;
			
		case CLASS_CLERIC:
			ch->points.max_hit = 12;
			break;
			
		case CLASS_THIEF:
			ch->points.max_hit = 13;
			break;
			
		case CLASS_WARRIOR:
			ch->points.max_hit = 15;
			break;
			
		case CLASS_PELOI:
			ch->points.max_hit = 13;
			break;
			
		case CLASS_DARESIANO:
			ch->points.max_hit = 11;
			break;
	}
	
	advance_level(ch);
	
	GET_HIT(ch) = GET_MAX_HIT(ch);
	GET_MANA(ch) = GET_MAX_MANA(ch);
	GET_MOVE(ch) = GET_MAX_MOVE(ch);
	GET_FIATO(ch)=(GET_CON(ch)+GET_CON(ch)+GET_DEX(ch))/4 + GET_MOVE(ch)/15;
	//PEPPE_AMANU
	if(AFF_FLAGGED(ch,AFF_ENDURANCE))
		GET_FIATO(ch)=GET_FIATO(ch)*2;
	
	GET_COND(ch, THIRST) = 96;/* 4 GIORNI*/
	GET_COND(ch, FULL) = 96;
	GET_COND(ch, DRUNK) = 0;
	
	SET_BIT(PRF_FLAGS(ch), PRF_NOKILL);
	SET_BIT(PRF_FLAGS(ch), PRF_COLOR_2);
	SET_BIT(PRF_FLAGS(ch), PRF_AUTOEXIT);
	SET_BIT(PRF_FLAGS(ch), PRF_AUTODIR);
	
SET_BIT(PRF_FLAGS(ch), PRF_RPG);
	ch->player.time.played = 0;
	ch->player.time.logon = time(0);
}


/*
 * This function controls the change to maxmove, maxmana, and maxhp for
 * each class every time they gain a level.
 */
void advance_level(struct char_data * ch)
{
	int add_hp = 0, add_mana = 0, add_move = 0, i;
	
	extern struct wis_app_type wis_app[];
	extern struct con_app_type con_app[];
	
	add_hp = con_app[GET_CON(ch)].hitp;
	
	switch (GET_CLASS(ch)) {
		
		case CLASS_MAGIC_USER:
			add_hp += number(5, 7);
			add_mana = number(7, 9);
			add_move = number(2, 4);
			GET_PRACTICES(ch) += 3;
			break;
			
		case CLASS_CLERIC:
			add_hp += number(5, 7);
			add_mana = number(6, 8);
			add_move = number(2, 4);
			GET_PRACTICES(ch) += 3;
			break;
			
		case CLASS_PELOI:
			add_hp += number(6, 8);
			add_mana = number(3, 5);
			add_move = number(3, 5);
			GET_PRACTICES(ch) += 2;
			break;
			
		case CLASS_THIEF:
			add_hp += number(6, 8);
			add_mana = number(4, 6);
			add_move = number(2, 4);
			GET_PRACTICES(ch) += 2;
			break;
			
		case CLASS_WARRIOR:
			add_hp += number(8, 9);
			add_mana = number(1, 2);
			add_move = number(2, 4);
			GET_PRACTICES(ch) += 1;
			break;
			
		case CLASS_DARESIANO:
			add_hp   += number(5, 7);
			add_mana  = number(1, 2);
			add_move  = number(2, 4);
			GET_PRACTICES(ch) += 1;
			break;
	}
	
	ch->points.max_hit += MAX(1, add_hp)+GET_CLASS_TITLE(ch);
	ch->points.max_move += MAX(1, add_move);
	
	if (GET_LEVEL(ch) > 1)
		ch->points.max_mana += add_mana+GET_CLASS_TITLE(ch);
	
	GET_PRACTICES(ch) += wis_app[GET_WIS(ch)].bonus;
	GET_ABIL_PRACS(ch)+=2;
	GET_ABIL_PRACS(ch)+=GET_CHA(ch)/4;
	if( GET_CHA(ch) ==18)
		GET_ABIL_PRACS(ch)+=1;

	if (GET_LEVEL(ch) >= LVL_QUEST_MASTER) {
		for (i = 0; i < 3; i++)
			GET_COND(ch, i) = (char) -1;
		SET_BIT(PRF_FLAGS(ch), PRF_HOLYLIGHT);
	}
	
	ch->player_specials->saved.morti_per_livello=0;
	
	check_regen_rates(ch);	/* start regening new points */
	
	save_char(ch, NOWHERE);
	
	sprintf(buf, "%s avanza al livello %d", GET_NAME(ch), GET_LEVEL(ch));
	mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
}


/*
 * This simply calculates the backstab multiplier based on a character's
 * level.  This used to be an array, but was changed to be a function so
 * that it would be easier to add more levels to your MUD.  This doesn't
 * really create a big performance hit because it's not used very often.
 */
int backstab_mult(int level)
{
	if (level <= 0)
		return 1;	  /* level 0 */
	else if (level <= 13)
		return 2;	  /* level 1 - 7 */
	else if (level <= 20)
		return 3;	  /* level 8 - 13 */
	else if (level <= 30)
		return 4;	  /* level 14 - 20 */
	else if (level <= 40)
		return 5;
	else if (level < LVL_IMMORT)
		return 8;	  /* all remaining mortal levels */
	else
		return 14;	  /* immortals */
}


/*
 * invalid_class is used by handler.c to determine if a piece of equipment is
 * usable by a particular class, based on the ITEM_ANTI_{class} bitvectors.
 */

int invalid_class(struct char_data *ch, struct obj_data *obj)
{
	if (IS_DARESIANO(ch) && (IS_OBJ_STAT(obj, ITEM_ANTI_MAGIC_USER) ||
			IS_OBJ_STAT(obj, ITEM_ANTI_CLERIC    ) ||
			IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR   ) ||
			IS_OBJ_STAT(obj, ITEM_ANTI_THIEF     ) ||
			IS_OBJ_STAT(obj, ITEM_ANTI_PELOI    )))
		return 1;
	
	if ((IS_OBJ_STAT(obj, ITEM_ANTI_MAGIC_USER) && IS_MAGIC_USER(ch)) ||
		(IS_OBJ_STAT(obj, ITEM_ANTI_CLERIC) && IS_CLERIC(ch)) ||
		(IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR) && IS_WARRIOR(ch)) ||
		(IS_OBJ_STAT(obj, ITEM_ANTI_THIEF) && IS_THIEF(ch)) ||
		(IS_OBJ_STAT(obj, ITEM_ANTI_PELOI) && IS_PELOI(ch)))
		return 1;
	return 0;
}

int invalid_align(struct char_data *ch, struct obj_data *obj)
{
	if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch)) ||
		(IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch)) ||
		(IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)))
		return 1;
	return 0;
}

/*
 * SPELLS AND SKILLS.  This area defines which spells are assigned to
 * which classes, and the minimum level the character must be to use
 * the spell or skill.
 */
void init_spell_levels(void)
{
  /* PANDION */
	spell_level(SKILL_READ_MAGIC, CLASS_MAGIC_USER, 1);
	spell_level(SPELL_MAGIC_MISSILE, CLASS_MAGIC_USER, 1);
	spell_level(SPELL_SHIELD, CLASS_MAGIC_USER, 2);
	spell_level(SPELL_DETECT_POISON, CLASS_MAGIC_USER, 3);
	spell_level(SPELL_BURNING_HANDS, CLASS_MAGIC_USER, 4);
	spell_level(SPELL_DETECT_ALIGN, CLASS_MAGIC_USER, 5);
	spell_level(SPELL_MONSUM_I, CLASS_MAGIC_USER, 6);
	spell_level(SPELL_CURE_LIGHT, CLASS_MAGIC_USER, 7);
	spell_level(SPELL_INFRAVISION, CLASS_MAGIC_USER, 8);
	spell_level(SPELL_SLEEP, CLASS_MAGIC_USER, 9);
	spell_level(SPELL_CREATE_WATER, CLASS_MAGIC_USER, 10);
	spell_level(SPELL_STRENGTH, CLASS_MAGIC_USER, 10);
	spell_level(SPELL_CONGIUNZIONE, CLASS_MAGIC_USER, 10);
	spell_level(SPELL_ACID_ARROW, CLASS_MAGIC_USER, 11);
	spell_level(SPELL_DETECT_MAGIC, CLASS_MAGIC_USER, 12);
	spell_level(SPELL_STONESKIN, CLASS_MAGIC_USER, 13);
	spell_level(SPELL_WIZEYE, CLASS_MAGIC_USER, 14);
	spell_level(SKILL_WRITE_MAGIC, CLASS_MAGIC_USER, 15);
	spell_level(SPELL_LIGHTNING_BOLT, CLASS_MAGIC_USER, 15);
	spell_level(SKILL_SECOND_ATTACK, CLASS_MAGIC_USER, 16);
	spell_level(SPELL_INVISIBLE, CLASS_MAGIC_USER, 16);
	spell_level(SPELL_PEACE, CLASS_MAGIC_USER, 17);
	spell_level(SPELL_CONE_OF_COLD, CLASS_MAGIC_USER, 18);
	spell_level(SPELL_MONSUM_II, CLASS_MAGIC_USER, 19);
	spell_level(SPELL_BLINDNESS, CLASS_MAGIC_USER, 20);
	spell_level(SPELL_CHARM, CLASS_MAGIC_USER, 21);
	spell_level(SPELL_PROTECT_LIGHT, CLASS_MAGIC_USER, 22);
	spell_level(SPELL_BLINK, CLASS_MAGIC_USER, 23);
	spell_level(SPELL_PROT_FIRE, CLASS_MAGIC_USER, 24);
	spell_level(SPELL_SHOCKING_GRASP, CLASS_MAGIC_USER, 25);
	spell_level(SPELL_SILENCE, CLASS_MAGIC_USER, 26);
	spell_level(SPELL_PIDENTIFY, CLASS_MAGIC_USER, 27);
	spell_level(SPELL_PEACE, CLASS_MAGIC_USER,28);
	spell_level(SPELL_FLY, CLASS_MAGIC_USER, 29);
	spell_level(SPELL_COLOR_SPRAY, CLASS_MAGIC_USER, 30);
	spell_level(SPELL_CONTROL_WEATHER, CLASS_MAGIC_USER, 31);
	spell_level(SPELL_MONSUM_III, CLASS_MAGIC_USER, 32);
	spell_level(SPELL_WATERBREATH, CLASS_MAGIC_USER, 34);
	spell_level(SPELL_CREATE_LIGHT, CLASS_MAGIC_USER, 35);
	spell_level(SPELL_WATERWALK, CLASS_MAGIC_USER, 37);
	spell_level(SPELL_TIME_DISTORTION, CLASS_MAGIC_USER, 38);
	spell_level(SPELL_FLAME_ARROW, CLASS_MAGIC_USER, 39);
	spell_level(SPELL_DETECT_INVIS, CLASS_MAGIC_USER, 40);
	spell_level(SPELL_LEVITATE, CLASS_MAGIC_USER, 41);
	spell_level(SPELL_PARALIZE, CLASS_MAGIC_USER, 42);
	spell_level(SPELL_CONTINUAL_LIGHT, CLASS_MAGIC_USER, 43);
	spell_level(SPELL_BLADEBARRIER, CLASS_MAGIC_USER, 44);
	spell_level(SPELL_MONSUM_IV, CLASS_MAGIC_USER, 45);
	spell_level(SPELL_GROUP_FLY, CLASS_MAGIC_USER, 46);
	spell_level(SPELL_FIREBALL, CLASS_MAGIC_USER, 47);
	spell_level(SPELL_GROUP_INVIS, CLASS_MAGIC_USER, 48);
	spell_level(SPELL_WORD_OF_RECALL, CLASS_MAGIC_USER, 49);
	spell_level(SPELL_BARKSKIN, CLASS_MAGIC_USER, 50);
	spell_level(SPELL_ENERGY_DRAIN, CLASS_MAGIC_USER, 51);
	spell_level(SPELL_MINUTE_METEOR, CLASS_MAGIC_USER,52);
	spell_level(SPELL_GROUP_WATBREATH, CLASS_MAGIC_USER, 53);
	spell_level(SPELL_FIRESHD, CLASS_MAGIC_USER, 54);
	spell_level(SPELL_AREA_LIGHTNING, CLASS_MAGIC_USER, 55);
	spell_level(SPELL_ANTIMAGIC, CLASS_MAGIC_USER, 56);
	spell_level(SKILL_THIRD_ATTACK, CLASS_MAGIC_USER, 57);
	spell_level(SPELL_ENCHANT_WEAPON, CLASS_CLERIC, 58);
	spell_level(SPELL_MIRROR_IMAGE, CLASS_MAGIC_USER, 59);
	spell_level(SPELL_REFRESH, CLASS_MAGIC_USER, 60);
	spell_level(SPELL_SUMMON, CLASS_MAGIC_USER, 61);
	spell_level(SPELL_DET_SNEAK, CLASS_MAGIC_USER, 62);
	spell_level(SPELL_DISINTEGRATE, CLASS_MAGIC_USER, 63);
	spell_level(SPELL_ENCHANT_WOOD, CLASS_MAGIC_USER, 64);
	spell_level(SPELL_DEATHDANCE, CLASS_MAGIC_USER, 66);
	spell_level(SPELL_DISPEL_MAGIC, CLASS_MAGIC_USER, 68);
	spell_level(SPELL_DISGUISE, CLASS_MAGIC_USER, 70);
	
  /*  CARLO CARLO CARLO
  spell_level(SPELL_CALL_DRAGON, CLASS_MAGIC_USER, 70);   // MODIFICARE
  CARLO CARLO CARLO */
	
 /* CYRINIC */
	spell_level(SKILL_READ_MAGIC, CLASS_CLERIC, 1);
	spell_level(SPELL_CURE_LIGHT, CLASS_CLERIC, 1);
	spell_level(SPELL_CHILL_TOUCH, CLASS_CLERIC, 2);
	spell_level(SPELL_AID, CLASS_CLERIC, 2);
	spell_level(SPELL_BLESS, CLASS_CLERIC, 3);
	spell_level(SPELL_DETECT_MAGIC, CLASS_CLERIC, 4);
	spell_level(SPELL_ARMOR, CLASS_CLERIC, 5);
	spell_level(SPELL_DETECT_POISON, CLASS_CLERIC, 6);
	spell_level(SPELL_DETECT_ALIGN, CLASS_CLERIC, 7);
	spell_level(SPELL_LEVITATE, CLASS_CLERIC, 8);
	spell_level(SPELL_CREATE_LIGHT, CLASS_CLERIC, 9);
	spell_level(SPELL_REFRESH, CLASS_CLERIC, 10);
	spell_level(SPELL_CREATE_WATER, CLASS_CLERIC, 11);
	spell_level(SPELL_CURE_BLIND, CLASS_CLERIC, 12);
	spell_level(SPELL_REMOVE_POISON, CLASS_CLERIC, 13);
	spell_level(SPELL_CURE_SERIOUS, CLASS_CLERIC, 14);
	spell_level(SKILL_SCAN, CLASS_CLERIC, 15);
	spell_level(SPELL_REMOVE_CURSE, CLASS_CLERIC, 16);
	spell_level(SPELL_DISPEL_EVIL, CLASS_CLERIC, 18);
	spell_level(SPELL_SLEEP, CLASS_CLERIC, 19);
	spell_level(SKILL_SECOND_ATTACK, CLASS_CLERIC, 20);
	spell_level(SPELL_DISPEL_GOOD, CLASS_CLERIC, 21);
	spell_level(SPELL_BLINDNESS, CLASS_CLERIC, 22);
	spell_level(SKILL_PARRY, CLASS_CLERIC, 23);
	spell_level(SPELL_LOCATE_OBJECT, CLASS_CLERIC, 24);
	spell_level(SPELL_PEACE, CLASS_CLERIC, 25);
	spell_level(SPELL_POISON, CLASS_CLERIC, 27);
	spell_level(SPELL_SANCTUARY, CLASS_CLERIC, 28);
	spell_level(SPELL_HARM, CLASS_CLERIC, 29);
	spell_level(SPELL_PROT_FROM_EVIL, CLASS_CLERIC, 30);
	spell_level(SPELL_CURSE, CLASS_CLERIC, 32);
	spell_level(SPELL_FLY, CLASS_CLERIC, 33);
	spell_level(SPELL_WATERWALK, CLASS_CLERIC, 34);
	spell_level(SPELL_PROT_FIRE, CLASS_CLERIC, 35);
	spell_level(SPELL_CURE_CRITIC, CLASS_CLERIC, 36);
	spell_level(SKILL_THIRD_ATTACK, CLASS_CLERIC, 38);
	spell_level(SPELL_EARTHQUAKE, CLASS_CLERIC, 40);
	spell_level(SPELL_WATERBREATH, CLASS_CLERIC, 41);
	spell_level(SPELL_ENCHANT_WEAPON, CLASS_CLERIC, 42);
	spell_level(SPELL_GROUP_ARMOR, CLASS_CLERIC, 43);
	spell_level(SPELL_DETECT_INVIS, CLASS_CLERIC, 45);
	spell_level(SPELL_HEAL, CLASS_CLERIC, 46);
	spell_level(SPELL_CONTINUAL_LIGHT, CLASS_CLERIC, 48);
	spell_level(SPELL_AREA_LIGHTNING, CLASS_CLERIC, 50);
	spell_level(SKILL_WRITE_MAGIC, CLASS_CLERIC, 50);
	spell_level(SPELL_CHARM, CLASS_CLERIC, 51);
	spell_level(SPELL_ENERGY_DRAIN, CLASS_CLERIC, 52);
	spell_level(SPELL_GROUP_PROT_EVIL, CLASS_CLERIC, 54);
	spell_level(SPELL_GROUP_HEAL, CLASS_CLERIC, 55);
	spell_level(SPELL_GROUP_FLY, CLASS_CLERIC, 57);
	spell_level(SPELL_BARKSKIN, CLASS_CLERIC, 59);  
	spell_level(SPELL_WORD_OF_RECALL, CLASS_CLERIC, 60);
	spell_level(SPELL_PIDENTIFY, CLASS_CLERIC, 61);
	spell_level(SPELL_SUMMON, CLASS_CLERIC, 62);
	spell_level(SPELL_BLADEBARRIER, CLASS_CLERIC, 63);
	spell_level(SPELL_GROUP_WATBREATH, CLASS_CLERIC, 64);
	spell_level(SPELL_GROUP_ARMOR, CLASS_CLERIC, 66);
	spell_level(SPELL_DISPEL_MAGIC, CLASS_CLERIC, 70);
	
	
  /* CARLO CARLO CARLO
  CARLO CARLO CARLO */
	
  /*  ALCIONI */
	spell_level(SKILL_BACKSTAB, CLASS_THIEF, 1);
	spell_level(SKILL_PEEP, CLASS_THIEF, 1);
	spell_level(SKILL_KICK, CLASS_THIEF, 2);
	spell_level(SPELL_SHIELD, CLASS_THIEF, 3);
	spell_level(SKILL_HIDE, CLASS_THIEF, 4);
	spell_level(SKILL_SNEAK, CLASS_THIEF, 5);
	spell_level(SKILL_STEAL, CLASS_THIEF, 6);
	spell_level(SPELL_FEAR, CLASS_THIEF, 8);
	spell_level(SKILL_DODGE, CLASS_THIEF, 9);
	spell_level(SKILL_PICK_LOCK, CLASS_THIEF, 10);
	spell_level(SKILL_BOW, CLASS_THIEF, 11);
	spell_level(SPELL_CHILL_TOUCH, CLASS_THIEF, 12);
	spell_level(SKILL_SECOND_ATTACK, CLASS_THIEF, 13);
	spell_level(SKILL_TRACK, CLASS_THIEF, 14);
	spell_level(SKILL_RESCUE, CLASS_THIEF, 15);
	spell_level(SKILL_READ_MAGIC, CLASS_THIEF, 15);
	spell_level(SPELL_BARKSKIN, CLASS_THIEF, 16);
	spell_level(SPELL_DET_SNEAK, CLASS_THIEF, 17);
	spell_level(SKILL_SPY, CLASS_THIEF, 18);
	spell_level(SPELL_CURE_LIGHT, CLASS_THIEF, 19);
	spell_level(SPELL_SENSE_LIFE, CLASS_THIEF, 20);
	spell_level(SPELL_ACID_ARROW, CLASS_THIEF, 21);
	spell_level(SPELL_LOCATE_OBJECT, CLASS_THIEF, 22);
	spell_level(SPELL_LOCATE_TARGET, CLASS_THIEF, 23);
	spell_level(SPELL_INFRAVISION, CLASS_THIEF, 24);
	spell_level(SPELL_POISON, CLASS_THIEF, 26);
	spell_level(SPELL_LEVITATE, CLASS_THIEF, 28);
	spell_level(SPELL_BURNING_HANDS, CLASS_THIEF, 30);
	spell_level(SPELL_WATERBREATH, CLASS_THIEF, 33);
	spell_level(SKILL_THIRD_ATTACK, CLASS_THIEF, 35);
	spell_level(SPELL_WATERWALK, CLASS_THIEF, 37);
	spell_level(SPELL_LIGHTNING_BOLT, CLASS_THIEF, 39);
	spell_level(SPELL_PARALIZE, CLASS_THIEF, 40);
	spell_level(SPELL_SILENCE, CLASS_THIEF, 41);
	spell_level(SPELL_ARMOR, CLASS_THIEF, 43);
	spell_level(SPELL_BLADEBARRIER, CLASS_THIEF, 44);
	spell_level(SPELL_PROT_FIRE, CLASS_THIEF, 46);
	spell_level(SPELL_DISPEL_EVIL, CLASS_THIEF, 48);
	spell_level(SPELL_CURSE, CLASS_THIEF, 55);
	spell_level(SPELL_DEATHDANCE, CLASS_THIEF, 60);
	spell_level(SPELL_DISPEL_GOOD, CLASS_THIEF, 65);
	spell_level(SPELL_DISGUISE, CLASS_THIEF, 68);
	
  /* CARLO CARLO CARLO
  CARLO CARLO CARLO */
	
/* GENIDIAN */
	spell_level(SKILL_BASH, CLASS_WARRIOR, 1);
	spell_level(SKILL_KICK, CLASS_WARRIOR, 1);
	spell_level(SKILL_PUNCH, CLASS_WARRIOR, 1);
	spell_level(SKILL_BOW, CLASS_WARRIOR, 2);
	spell_level(SKILL_DISARM, CLASS_WARRIOR, 3);
	spell_level(SPELL_ARMOR, CLASS_WARRIOR, 5);
	spell_level(SKILL_DODGE, CLASS_WARRIOR, 7);
	spell_level(SKILL_TRACK, CLASS_WARRIOR, 9);
	spell_level(SKILL_BASH_LOCK, CLASS_WARRIOR, 10);
	spell_level(SKILL_SECOND_ATTACK, CLASS_WARRIOR, 11);
	spell_level(SKILL_SCAN, CLASS_WARRIOR, 13);
	spell_level(SKILL_RESCUE, CLASS_WARRIOR, 15);
	spell_level(SPELL_DETECT_POISON, CLASS_WARRIOR, 17);
	spell_level(SPELL_CURE_LIGHT, CLASS_WARRIOR, 20);
	spell_level(SKILL_ENH_DAMAGE, CLASS_WARRIOR, 23);
	spell_level(SPELL_BARKSKIN, CLASS_WARRIOR, 26);
	spell_level(SKILL_THIRD_ATTACK, CLASS_WARRIOR, 28);
	spell_level(SPELL_WATERWALK, CLASS_WARRIOR, 32);
	spell_level(SPELL_STONESKIN, CLASS_WARRIOR, 35);
	spell_level(SKILL_RETREAT, CLASS_WARRIOR, 37);
	spell_level(SPELL_WATERBREATH, CLASS_WARRIOR, 39);
	spell_level(SPELL_AID, CLASS_WARRIOR, 40);
	spell_level(SKILL_COLPO_MORTALE, CLASS_WARRIOR, 42);
	spell_level(SKILL_FOURTH_ATTACK, CLASS_WARRIOR, 44);
	spell_level(SPELL_DETECT_ALIGN, CLASS_WARRIOR, 47);
	spell_level(SPELL_GROUP_WATBREATH, CLASS_WARRIOR, 50);
	spell_level(SKILL_FIFTH_ATTACK, CLASS_WARRIOR, 53);
	spell_level(SPELL_STRENGTH, CLASS_WARRIOR, 60);
	spell_level(SPELL_GROUP_ARMOR, CLASS_WARRIOR, 65);
	
	
  /* CARLO CARLO CARLO
  CARLO CARLO CARLO */
	
  /* PELOI */
	spell_level(SKILL_KICK, CLASS_PELOI, 1);
	spell_level(SKILL_BACKSTAB, CLASS_PELOI, 2);
	spell_level(SPELL_STRENGTH, CLASS_PELOI, 3);
	spell_level(SKILL_BOW, CLASS_PELOI, 3);
	spell_level(SKILL_READ_MAGIC, CLASS_PELOI, 5);
	spell_level(SPELL_ARMOR, CLASS_PELOI, 6);
	spell_level(SPELL_ACID_ARROW, CLASS_PELOI, 7);
	spell_level(SPELL_CREATE_WATER, CLASS_PELOI, 8);
	spell_level(SKILL_TRACK, CLASS_PELOI, 9);
	spell_level(SKILL_SNEAK, CLASS_PELOI, 10);
	spell_level(SPELL_CURE_LIGHT, CLASS_PELOI, 11);
	spell_level(SPELL_DETECT_POISON, CLASS_PELOI, 12);
	spell_level(SKILL_SCAN, CLASS_PELOI, 13);
	spell_level(SKILL_SECOND_ATTACK, CLASS_PELOI, 14);
	spell_level(SPELL_INFRAVISION, CLASS_PELOI, 15);
	spell_level(SKILL_SPOT, CLASS_PELOI, 17);
	spell_level(SKILL_HIDE, CLASS_PELOI, 19);
	spell_level(SPELL_STONESKIN, CLASS_PELOI, 20);
	spell_level(SKILL_DISARM, CLASS_PELOI, 21);
	spell_level(SKILL_RESCUE, CLASS_PELOI, 22);
	//PEPPE_AMANU
	spell_level(SPELL_ENDURANCE, CLASS_PELOI, 23);
	spell_level(SPELL_SENSE_LIFE, CLASS_PELOI, 24);
	spell_level(SPELL_REMOVE_POISON, CLASS_PELOI, 26);
	spell_level(SPELL_CURE_SERIOUS, CLASS_PELOI, 28);
	spell_level(SKILL_THIRD_ATTACK, CLASS_PELOI, 30);
	spell_level(SPELL_WATERWALK, CLASS_PELOI, 33);
	spell_level(SKILL_DODGE, CLASS_PELOI, 34);
	spell_level(SPELL_LEVITATE, CLASS_PELOI, 35);
	spell_level(SPELL_LOCATE_TARGET, CLASS_PELOI, 36);
	spell_level(SKILL_ENH_DAMAGE, CLASS_PELOI, 38);
	spell_level(SPELL_CALL_LIGHTNING, CLASS_PELOI, 40);
	spell_level(SPELL_DET_SNEAK, CLASS_PELOI, 42);
	spell_level(SPELL_WRAITHFORM, CLASS_PELOI, 43);
	spell_level(SPELL_FLAME_ARROW, CLASS_PELOI, 45);
	spell_level(SPELL_REFRESH, CLASS_PELOI, 50);
	spell_level(SKILL_FOURTH_ATTACK, CLASS_PELOI, 52);
	spell_level(SPELL_ENCHANT_WOOD, CLASS_PELOI, 53);
	spell_level(SPELL_ENCHANT_WEAPON, CLASS_PELOI, 55);
	spell_level(SPELL_DETECT_INVIS, CLASS_PELOI, 57);
	spell_level(SPELL_GROUP_WATBREATH, CLASS_PELOI, 61);
	spell_level(SPELL_DEATHDANCE, CLASS_PELOI, 63);
	spell_level(SPELL_GROUP_ARMOR, CLASS_PELOI, 67);
	
	
  /* DARESIANI */
  // SEMPLICEMENTE NON NE HANNO !!!
}

/*Enrico :ABILITA_OFFAR:da assegnare alle classi come fa init_spell_level*/
/*                      usando give_abilita(int abilita,int class,bool at_start)       */

void init_abilita(void){
  //void give_abilita(int abilita,int class,bool at_start);
  /*PANDION*/
	give_abilita(ABIL_SPADA,CLASS_MAGIC_USER,TRUE);
	give_abilita(ABIL_LANCIA,CLASS_MAGIC_USER,TRUE);
	give_abilita(ABIL_SCUDO,CLASS_MAGIC_USER,TRUE);
	give_abilita(ABIL_ARM_BASE,CLASS_MAGIC_USER,TRUE);
	give_abilita(ABIL_MEDITAZIONE,CLASS_MAGIC_USER,TRUE);
	give_abilita(ABIL_TRATTATIVA,CLASS_MAGIC_USER,FALSE);
  //give_abilita(ABIL_DIPLOMAZIA,CLASS_MAGIC_USER,FALSE);
	give_abilita(ABIL_STUDIO,CLASS_MAGIC_USER,FALSE);
  //give_abilita(ABIL_PERCEZIONE,CLASS_MAGIC_USER,FALSE);
	
  /*CYRINIC*/
	give_abilita(ABIL_SPADA,CLASS_CLERIC,FALSE);
	give_abilita(ABIL_MAZZA,CLASS_CLERIC,TRUE);
	give_abilita(ABIL_SCUDO,CLASS_CLERIC,TRUE);
	give_abilita(ABIL_ARM_BASE,CLASS_CLERIC,TRUE);
	give_abilita(ABIL_ARM_MEDIA,CLASS_CLERIC,FALSE);
	give_abilita(ABIL_TRATTATIVA,CLASS_CLERIC,TRUE);
   //give_abilita(ABIL_DIPLOMAZIA,CLASS_CLERIC,FALSE);
	give_abilita(ABIL_STUDIO,CLASS_CLERIC,FALSE);
   //give_abilita(ABIL_MEDITAZIONE,CLASS_CLERIC);
   //give_abilita(ABIL_PERCEZIONE,CLASS_CLERIC,FALSE);
	give_abilita(ABIL_REGENERATION,CLASS_CLERIC,TRUE);
   /*ALCIONI*/
	give_abilita(ABIL_SPADA,CLASS_THIEF,TRUE);
	give_abilita(ABIL_PUGNALE,CLASS_THIEF,TRUE);
	give_abilita(ABIL_MAZZA,CLASS_THIEF,FALSE);
	give_abilita(ABIL_LANCIA,CLASS_THIEF,FALSE);
	give_abilita(ABIL_SCUDO,CLASS_THIEF,TRUE);
	give_abilita(ABIL_ARM_BASE,CLASS_THIEF,FALSE);
	give_abilita(ABIL_ARM_MEDIA,CLASS_THIEF,TRUE);
   //give_abilita(ABIL_ARM_PESANTE,CLASS_THIEF,FALSE);
	give_abilita(ABIL_RIPARAZIONE,CLASS_THIEF,FALSE);
	give_abilita(ABIL_TRAPPOLE,CLASS_THIEF,FALSE);
   //give_abilita(ABIL_PERCEZIONE,CLASS_THIEF,FALSE);
	give_abilita(ABIL_AGGUATO,CLASS_THIEF,TRUE);
	
            /*GENIDIAN*/
	give_abilita(ABIL_SPADA,CLASS_WARRIOR,TRUE);
	give_abilita(ABIL_ASCIA,CLASS_WARRIOR,TRUE);
   //give_abilita(ABIL_PUGNALE,CLASS_WARRIOR);
	give_abilita(ABIL_MAZZA,CLASS_WARRIOR,FALSE);
	give_abilita(ABIL_LANCIA,CLASS_WARRIOR,FALSE);
	give_abilita(ABIL_SCUDO,CLASS_WARRIOR,TRUE);
	give_abilita(ABIL_ARM_MEDIA,CLASS_WARRIOR,TRUE);
	give_abilita(ABIL_ARM_PESANTE,CLASS_WARRIOR,FALSE);
	give_abilita(ABIL_BODYBUILDING,CLASS_WARRIOR,TRUE);
   //give_abilita(ABIL_PERCEZIONE,CLASS_WARRIOR,FALSE);
	give_abilita(ABIL_RIPARAZIONE,CLASS_WARRIOR,FALSE);
             /*PELOI*/
	give_abilita(ABIL_SPADA,CLASS_PELOI,TRUE);
   //give_abilita(ABIL_MAZZA,CLASS_PELOI,);
	give_abilita(ABIL_PUGNALE,CLASS_PELOI,TRUE);
	give_abilita(ABIL_ASCIA,CLASS_PELOI,FALSE);
   //give_abilita(ABIL_LANCIA,CLASS_PELOI);
	give_abilita(ABIL_SCUDO,CLASS_PELOI,TRUE);
	give_abilita(ABIL_ARM_BASE,CLASS_PELOI,TRUE);
	give_abilita(ABIL_ARM_MEDIA,CLASS_PELOI,FALSE);
	give_abilita(ABIL_RIPARAZIONE,CLASS_PELOI,TRUE);
	give_abilita(ABIL_TRATTATIVA,CLASS_PELOI,TRUE);
   //give_abilita(ABIL_PERCEZIONE,CLASS_PELOI,TRUE);
}


/* Names of class/levels and exp required for each level */

const struct title_type titles[NUM_CLASSES][LVL_IMPL + 1] = {
	{{"", "", 0},        /* Livello 0 */
		{"", "", 1},         /* Livello 1 */
		{"", "", 4000},
		{"", "", 10000},
		{"", "", 18000},
		{"", "", 28000},
		{"", "", 40000},
		{"", "", 54000},
		{"", "", 70000},
		{"", "", 90000},
		{"", "", 115000},    /* Livello 10 */
		{"", "", 150000},
		{"", "", 190000},
		{"", "", 238000},
		{"", "", 294000},
		{"", "", 358000},
		{"", "", 422000},
		{"", "", 524000},
		{"", "", 656000},
		{"", "", 818000},
		{"", "", 1010000},   /* Livello 20 */
		{"", "", 1230000},
		{"", "", 1530000},
		{"", "", 1820000},
		{"", "", 2190000},
		{"", "", 2610000},
		{"","",  3080000},
		{"", "", 3600000},
		{"", "", 4170000},
		{"", "", 4790000},
		{"", "", 5460000},  /* Livello 30 */
		{"", "", 6180000},
		{"", "", 7000000},
		{"", "", 7920000},
		{"", "", 8950000},
		{"", "", 10100000},
		{"", "", 11400000},
		{"", "", 12850000},
		{"", "", 14450000},
		{"", "", 16200000},
		{"", "", 18100000},  /* Livello 40 */
		{"", "", 20200000},
		{"", "", 22700000},
		{"", "", 25700000},
		{"", "", 29200000},
		{"", "", 33200000},
		{"", "", 38200000},
		{"", "", 44200000},
		{"", "", 51200000},
		{"", "", 59200000},
		{"", "", 68200000}, /* Livello 50 */
		{"", "", 78200000},
		{"", "", 89700000},
		{"", "", 102700000},
		{"", "", 117200000},
		{"", "", 133200000},
		{"", "", 150700000},
		{"", "", 170700000},
		{"", "", 193200000},
		{"", "", 218200000},
		{"", "", 246000000}, /* Livello 60 */
		{"", "", 276000000},
		{"", "", 310000000},
		{"", "", 350000000},
		{"", "", 400000000},
		{"", "", 460000000},
		{"", "", 530000000},
		{"", "", 610000000},
		{"", "", 700000000},
		{"", "", 800000000},
		{"", "", 900000000}, /* Livello 70 */
		{"", "", 1000000000}, /* Livello 71 */
		{"", "", 1050000000},
		{"", "", 1100000000},
		{"", "", 1150000000},
		{"", "", 1200000000},
		{"", "", 1250000000},
		{"", "", 1300000000},
		{"", "", 1350000000},
		{"", "", 1400000000},
		{"", "", 1450000000}, /* Livello 80 */
		{"", "", 1500000000},
		{"", "", 1550000000},
		{"", "", 1600000000},
		{"", "", 1650000000},
		{"", "", 1700000000},
		{"", "", 1750000000},
		{"", "", 1800000000},
		{"", "", 1850000000},
		{"", "", 1900000000}, /* Livello 89 */
		{"", "", 1950000000}, /* Livello 90 */
		{"", "", 2000000000}  /* Livello 91 */
	},
	{{"", "", 0},        /* Livello 0 */
		{"", "", 1},         /* Livello 1 */
		{"", "", 4000},
		{"", "", 10000},
		{"", "", 18000},
		{"", "", 28000},
		{"", "", 40000},
		{"", "", 54000},
		{"", "", 70000},
		{"", "", 90000},
		{"", "", 115000},    /* Livello 10 */
		{"", "", 150000},
		{"", "", 190000},
		{"", "", 238000},
		{"", "", 294000},
		{"", "", 358000},
		{"", "", 422000},
		{"", "", 524000},
		{"", "", 656000},
		{"", "", 818000},
		{"", "", 1010000},   /* Livello 20 */
		{"", "", 1230000},
		{"", "", 1530000},
		{"", "", 1820000},
		{"", "", 2190000},
		{"", "", 2610000},
		{"","",  3080000},
		{"", "", 3600000},
		{"", "", 4170000},
		{"", "", 4790000},
		{"", "", 5460000},  /* Livello 30 */
		{"", "", 6180000},
		{"", "", 7000000},
		{"", "", 7920000},
		{"", "", 8950000},
		{"", "", 10100000},
		{"", "", 11400000},
		{"", "", 12850000},
		{"", "", 14450000},
		{"", "", 16200000},
		{"", "", 18100000},  /* Livello 40 */
		{"", "", 20200000},
		{"", "", 22700000},
		{"", "", 25700000},
		{"", "", 29200000},
		{"", "", 33200000},
		{"", "", 38200000},
		{"", "", 44200000},
		{"", "", 51200000},
		{"", "", 59200000},
		{"", "", 68200000}, /* Livello 50 */
		{"", "", 78200000},
		{"", "", 89700000},
		{"", "", 102700000},
		{"", "", 117200000},
		{"", "", 133200000},
		{"", "", 150700000},
		{"", "", 170700000},
		{"", "", 193200000},
		{"", "", 218200000},
		{"", "", 246000000}, /* Livello 60 */
		{"", "", 276000000},
		{"", "", 310000000},
		{"", "", 350000000},
		{"", "", 400000000},
		{"", "", 460000000},
		{"", "", 530000000},
		{"", "", 610000000},
		{"", "", 700000000},
		{"", "", 800000000},
		{"", "", 900000000}, /* Livello 70 */
		{"", "", 1000000000}, /* Livello 71 */
		{"", "", 1050000000},
		{"", "", 1100000000},
		{"", "", 1150000000},
		{"", "", 1200000000},
		{"", "", 1250000000},
		{"", "", 1300000000},
		{"", "", 1350000000},
		{"", "", 1400000000},
		{"", "", 1450000000}, /* Livello 80 */
		{"", "", 1500000000},
		{"", "", 1550000000},
		{"", "", 1600000000},
		{"", "", 1650000000},
		{"", "", 1700000000},
		{"", "", 1750000000},
		{"", "", 1800000000},
		{"", "", 1850000000},
		{"", "", 1900000000}, /* Livello 89 */
		{"", "", 1950000000}, /* Livello 90 */
		{"", "", 2000000000}  /* Livello 91 */
	},
	{{"", "", 0},        /* Livello 0 */
		{"", "", 1},         /* Livello 1 */
		{"", "", 4000},
		{"", "", 10000},
		{"", "", 18000},
		{"", "", 28000},
		{"", "", 40000},
		{"", "", 54000},
		{"", "", 70000},
		{"", "", 90000},
		{"", "", 115000},    /* Livello 10 */
		{"", "", 150000},
		{"", "", 190000},
		{"", "", 238000},
		{"", "", 294000},
		{"", "", 358000},
		{"", "", 422000},
		{"", "", 524000},
		{"", "", 656000},
		{"", "", 818000},
		{"", "", 1010000},   /* Livello 20 */
		{"", "", 1230000},
		{"", "", 1530000},
		{"", "", 1820000},
		{"", "", 2190000},
		{"", "", 2610000},
		{"","",  3080000},
		{"", "", 3600000},
		{"", "", 4170000},
		{"", "", 4790000},
		{"", "", 5460000},  /* Livello 30 */
		{"", "", 6180000},
		{"", "", 7000000},
		{"", "", 7920000},
		{"", "", 8950000},
		{"", "", 10100000},
		{"", "", 11400000},
		{"", "", 12850000},
		{"", "", 14450000},
		{"", "", 16200000},
		{"", "", 18100000},  /* Livello 40 */
		{"", "", 20200000},
		{"", "", 22700000},
		{"", "", 25700000},
		{"", "", 29200000},
		{"", "", 33200000},
		{"", "", 38200000},
		{"", "", 44200000},
		{"", "", 51200000},
		{"", "", 59200000},
		{"", "", 68200000}, /* Livello 50 */
		{"", "", 78200000},
		{"", "", 89700000},
		{"", "", 102700000},
		{"", "", 117200000},
		{"", "", 133200000},
		{"", "", 150700000},
		{"", "", 170700000},
		{"", "", 193200000},
		{"", "", 218200000},
		{"", "", 246000000}, /* Livello 60 */
		{"", "", 276000000},
		{"", "", 310000000},
		{"", "", 350000000},
		{"", "", 400000000},
		{"", "", 460000000},
		{"", "", 530000000},
		{"", "", 610000000},
		{"", "", 700000000},
		{"", "", 800000000},
		{"", "", 900000000}, /* Livello 70 */
		{"", "", 1000000000}, /* Livello 71 */
		{"", "", 1050000000},
		{"", "", 1100000000},
		{"", "", 1150000000},
		{"", "", 1200000000},
		{"", "", 1250000000},
		{"", "", 1300000000},
		{"", "", 1350000000},
		{"", "", 1400000000},
		{"", "", 1450000000}, /* Livello 80 */
		{"", "", 1500000000},
		{"", "", 1550000000},
		{"", "", 1600000000},
		{"", "", 1650000000},
		{"", "", 1700000000},
		{"", "", 1750000000},
		{"", "", 1800000000},
		{"", "", 1850000000},
		{"", "", 1900000000}, /* Livello 89 */
		{"", "", 1950000000}, /* Livello 90 */
		{"", "", 2000000000}  /* Livello 91 */
	},
	{{"", "", 0},        /* Livello 0 */
		{"", "", 1},         /* Livello 1 */
		{"", "", 4000},
		{"", "", 10000},
		{"", "", 18000},
		{"", "", 28000},
		{"", "", 40000},
		{"", "", 54000},
		{"", "", 70000},
		{"", "", 90000},
		{"", "", 115000},    /* Livello 10 */
		{"", "", 150000},
		{"", "", 190000},
		{"", "", 238000},
		{"", "", 294000},
		{"", "", 358000},
		{"", "", 422000},
		{"", "", 524000},
		{"", "", 656000},
		{"", "", 818000},
		{"", "", 1010000},   /* Livello 20 */
		{"", "", 1230000},
		{"", "", 1530000},
		{"", "", 1820000},
		{"", "", 2190000},
		{"", "", 2610000},
		{"","",  3080000},
		{"", "", 3600000},
		{"", "", 4170000},
		{"", "", 4790000},
		{"", "", 5460000},  /* Livello 30 */
		{"", "", 6180000},
		{"", "", 7000000},
		{"", "", 7920000},
		{"", "", 8950000},
		{"", "", 10100000},
		{"", "", 11400000},
		{"", "", 12850000},
		{"", "", 14450000},
		{"", "", 16200000},
		{"", "", 18100000},  /* Livello 40 */
		{"", "", 20200000},
		{"", "", 22700000},
		{"", "", 25700000},
		{"", "", 29200000},
		{"", "", 33200000},
		{"", "", 38200000},
		{"", "", 44200000},
		{"", "", 51200000},
		{"", "", 59200000},
		{"", "", 68200000}, /* Livello 50 */
		{"", "", 78200000},
		{"", "", 89700000},
		{"", "", 102700000},
		{"", "", 117200000},
		{"", "", 133200000},
		{"", "", 150700000},
		{"", "", 170700000},
		{"", "", 193200000},
		{"", "", 218200000},
		{"", "", 246000000}, /* Livello 60 */
		{"", "", 276000000},
		{"", "", 310000000},
		{"", "", 350000000},
		{"", "", 400000000},
		{"", "", 460000000},
		{"", "", 530000000},
		{"", "", 610000000},
		{"", "", 700000000},
		{"", "", 800000000},
		{"", "", 900000000}, /* Livello 70 */
		{"", "", 1000000000}, /* Livello 71 */
		{"", "", 1050000000},
		{"", "", 1100000000},
		{"", "", 1150000000},
		{"", "", 1200000000},
		{"", "", 1250000000},
		{"", "", 1300000000},
		{"", "", 1350000000},
		{"", "", 1400000000},
		{"", "", 1450000000}, /* Livello 80 */
		{"", "", 1500000000},
		{"", "", 1550000000},
		{"", "", 1600000000},
		{"", "", 1650000000},
		{"", "", 1700000000},
		{"", "", 1750000000},
		{"", "", 1800000000},
		{"", "", 1850000000},
		{"", "", 1900000000}, /* Livello 89 */
		{"", "", 1950000000}, /* Livello 90 */
		{"", "", 2000000000}  /* Livello 91 */
	},
	{{"", "", 0},        /* Livello 0 */
		{"", "", 1},         /* Livello 1 */
		{"", "", 4000},
		{"", "", 10000},
		{"", "", 18000},
		{"", "", 28000},
		{"", "", 40000},
		{"", "", 54000},
		{"", "", 70000},
		{"", "", 90000},
		{"", "", 115000},    /* Livello 10 */
		{"", "", 150000},
		{"", "", 190000},
		{"", "", 238000},
		{"", "", 294000},
		{"", "", 358000},
		{"", "", 422000},
		{"", "", 524000},
		{"", "", 656000},
		{"", "", 818000},
		{"", "", 1010000},   /* Livello 20 */
		{"", "", 1230000},
		{"", "", 1530000},
		{"", "", 1820000},
		{"", "", 2190000},
		{"", "", 2610000},
		{"","",  3080000},
		{"", "", 3600000},
		{"", "", 4170000},
		{"", "", 4790000},
		{"", "", 5460000},  /* Livello 30 */
		{"", "", 6180000},
		{"", "", 7000000},
		{"", "", 7920000},
		{"", "", 8950000},
		{"", "", 10100000},
		{"", "", 11400000},
		{"", "", 12850000},
		{"", "", 14450000},
		{"", "", 16200000},
		{"", "", 18100000},  /* Livello 40 */
		{"", "", 20200000},
		{"", "", 22700000},
		{"", "", 25700000},
		{"", "", 29200000},
		{"", "", 33200000},
		{"", "", 38200000},
		{"", "", 44200000},
		{"", "", 51200000},
		{"", "", 59200000},
		{"", "", 68200000}, /* Livello 50 */
		{"", "", 78200000},
		{"", "", 89700000},
		{"", "", 102700000},
		{"", "", 117200000},
		{"", "", 133200000},
		{"", "", 150700000},
		{"", "", 170700000},
		{"", "", 193200000},
		{"", "", 218200000},
		{"", "", 246000000}, /* Livello 60 */
		{"", "", 276000000},
		{"", "", 310000000},
		{"", "", 350000000},
		{"", "", 400000000},
		{"", "", 460000000},
		{"", "", 530000000},
		{"", "", 610000000},
		{"", "", 700000000},
		{"", "", 800000000},
		{"", "", 900000000}, /* Livello 70 */
		{"", "", 1000000000}, /* Livello 71 */
		{"", "", 1050000000},
		{"", "", 1100000000},
		{"", "", 1150000000},
		{"", "", 1200000000},
		{"", "", 1250000000},
		{"", "", 1300000000},
		{"", "", 1350000000},
		{"", "", 1400000000},
		{"", "", 1450000000}, /* Livello 80 */
		{"", "", 1500000000},
		{"", "", 1550000000},
		{"", "", 1600000000},
		{"", "", 1650000000},
		{"", "", 1700000000},
		{"", "", 1750000000},
		{"", "", 1800000000},
		{"", "", 1850000000},
		{"", "", 1900000000}, /* Livello 89 */
		{"", "", 1950000000}, /* Livello 90 */
		{"", "", 2000000000}  /* Livello 91 */
	},
	{{"", "", 0},        /* Livello 0 */
		{"", "", 1},         /* Livello 1 */
		{"", "", 4000},
		{"", "", 10000},
		{"", "", 18000},
		{"", "", 28000},
		{"", "", 40000},
		{"", "", 54000},
		{"", "", 70000},
		{"", "", 90000},
		{"", "", 115000},    /* Livello 10 */
		{"", "", 150000},
		{"", "", 190000},
		{"", "", 238000},
		{"", "", 294000},
		{"", "", 358000},
		{"", "", 422000},
		{"", "", 524000},
		{"", "", 656000},
		{"", "", 818000},
		{"", "", 1010000},   /* Livello 20 */
		{"", "", 1230000},
		{"", "", 1530000},
		{"", "", 1820000},
		{"", "", 2190000},
		{"", "", 2610000},
		{"","",  3080000},
		{"", "", 3600000},
		{"", "", 4170000},
		{"", "", 4790000},
		{"", "", 5460000},  /* Livello 30 */
		{"", "", 6180000},
		{"", "", 7000000},
		{"", "", 7920000},
		{"", "", 8950000},
		{"", "", 10100000},
		{"", "", 11400000},
		{"", "", 12850000},
		{"", "", 14450000},
		{"", "", 16200000},
		{"", "", 18100000},  /* Livello 40 */
		{"", "", 20200000},
		{"", "", 22700000},
		{"", "", 25700000},
		{"", "", 29200000},
		{"", "", 33200000},
		{"", "", 38200000},
		{"", "", 44200000},
		{"", "", 51200000},
		{"", "", 59200000},
		{"", "", 68200000}, /* Livello 50 */
		{"", "", 78200000},
		{"", "", 89700000},
		{"", "", 102700000},
		{"", "", 117200000},
		{"", "", 133200000},
		{"", "", 150700000},
		{"", "", 170700000},
		{"", "", 193200000},
		{"", "", 218200000},
		{"", "", 246000000}, /* Livello 60 */
		{"", "", 276000000},
		{"", "", 310000000},
		{"", "", 350000000},
		{"", "", 400000000},
		{"", "", 460000000},
		{"", "", 530000000},
		{"", "", 610000000},
		{"", "", 700000000},
		{"", "", 800000000},
		{"", "", 900000000}, /* Livello 70 */
		{"", "", 1000000000}, /* Livello 71 */
		{"", "", 1050000000},
		{"", "", 1100000000},
		{"", "", 1150000000},
		{"", "", 1200000000},
		{"", "", 1250000000},
		{"", "", 1300000000},
		{"", "", 1350000000},
		{"", "", 1400000000},
		{"", "", 1450000000}, /* Livello 80 */
		{"", "", 1500000000},
		{"", "", 1550000000},
		{"", "", 1600000000},
		{"", "", 1650000000},
		{"", "", 1700000000},
		{"", "", 1750000000},
		{"", "", 1800000000},
		{"", "", 1850000000},
		{"", "", 1900000000}, /* Livello 89 */
		{"", "", 1950000000}, /* Livello 90 */
		{"", "", 2000000000}  /* Livello 91 */
	}
};

/* Pandion, Cyrinic, Alcioni, Genidian, Peloi, (Daresiani) */
/* 0:Forza, 1:Intelligenza 2:Saggezza 3:Destrezza 4:Costituzione 5:Carisma */

const int requisiti_ordini[NUM_CLASSES][2] = {{1,2}, {1,2}, {0,3}, {0,4}, {3,4}, {3,5}};
const int max_incremento[NUM_CLASSES][6] = { {2, 10, 10, 2, 2, 5},
	{3, 10, 10, 2, 2, 4},
	{10, 2,  2,10, 3, 2},
	{10, 1,  1, 3,10, 1},
	{ 4, 2,  2,10,10, 4},
	{10,10,10,10,10,10}};

const char * nomi_attributi[] = {"Forza", "Intelligenza", "Saggezza", "Destrezza", "Costituzione", "Carisma"};
const char * nomi_attributi_hint[] = {"[F]orza", "[I]ntelligenza", "[S]aggezza", "[D]estrezza", "[C]ostituzione", "C[a]risma"};

int attributo (struct char_data *ch, int num)
{
	switch (num)
	{
		case 0: return ch->real_abils.str;
		case 1: return ch->real_abils.intel;
		case 2: return ch->real_abils.wis;
		case 3: return ch->real_abils.dex;
		case 4: return ch->real_abils.con;
		case 5: return ch->real_abils.cha;
			return -1;
	}
	return -1;
}

void ingresso_ordine (struct char_data *ch, int classe)
{
	extern int punti_aggiuntivi_iniziazione;
	int i;
	
	sprintf(buf, "\r\nE cosi' vorresti entrare nell'ordine combattente %s %s...\r\n", (classe==CLASS_THIEF ? "degli" : "dei"),
		pc_class_types[classe]);
	sprintf(buf+strlen(buf), "Il nostro ordine e' basato su %s e %s.\r\n\r\n", nomi_attributi[requisiti_ordini[classe][0]],
		nomi_attributi[requisiti_ordini[classe][1]]);
	if (attributo(ch, requisiti_ordini[classe][0]) + attributo(ch, requisiti_ordini[classe][1]) < 26 - punti_aggiuntivi_iniziazione)
	{
		strcat(buf, "Sinceramente, anche con tutta la buona volonta', non potresti mai raggiungere il livello minimo");
		strcat(buf, "richiesto per entrare nel nostro ordine. Dovrai ripiegare su qualche altra occupazione.\r\n");
		send_to_char(buf, ch);
		return;
	}
	
	strcat(buf, "Ad un primo esame, sembra che tu sia in possesso dei requisiti minimi per entrare\r\n");
	strcat(buf, "nel nostro ordine. Ora resta a te decidere, ricordati che questa scelta ti\r\n");
	strcat(buf, "accompagnera' per tutta la vita.\r\n");
	sprintf(buf+strlen(buf), "\r\nVuoi veramente entrare a far parte dell'ordine %s %s ? (Digita si/no): ",
		(classe==CLASS_THIEF ? "degli" : "dei"), pc_class_types[classe]);
	send_to_char(buf,ch);
	if (!ch->desc) send_to_char("Sei sconnesso o sei un mob ? In ogni caso non puoi!!!\r\n", ch);
	else
	{
		ch->iniz_classe=classe;
		for (i=0; i<6; i++) ch->iniz_ptagg[i]=0;
		ch->iniz_da_aggiungere = punti_aggiuntivi_iniziazione;
		STATE(ch->desc) = CON_CONF_INIZIAZ;
	}
	return;
}

void menu_iniziazione(struct descriptor_data * d)
{
	void menu_scelta_abilita(struct descriptor_data * d);
	extern int punti_aggiuntivi_iniziazione;
	int primo = (d->character->iniz_da_aggiungere == punti_aggiuntivi_iniziazione);
	int i;
	int libero=0;
	char numero[10];
	
	if (primo)
	{
		sprintf(buf, "\r\n\r\nCosi' sia allora, benvenuto nell'ordine %s %s !!!\r\n",
			(d->character->iniz_classe==CLASS_THIEF ? "degli" : "dei"), pc_class_types[d->character->iniz_classe]);
		sprintf(buf+strlen(buf), "L'effettuare il noviziato in un ordine combattente ti permette di distribuire %d\r\n"
			"punti ai tuoi attributi. I requisiti minimi devono pero' essere rispettati per gli\r\n"
			"attributi che caratterizzano l'ordine, e cio' avviene in automatico.\r\n"
			"I restanti punti, li puoi distribuire come vuoi, a condizione di non superare il valore\r\n"
			"di 16 in un singolo attributo, e altri vincoli imposti dall'ordine.\r\n\r\n",
			punti_aggiuntivi_iniziazione);
		d->character->iniz_ptagg[requisiti_ordini[d->character->iniz_classe][0]] =
			MAX (0, 13 - attributo(d->character, requisiti_ordini[d->character->iniz_classe][0]));
		d->character->iniz_ptagg[requisiti_ordini[d->character->iniz_classe][1]] =
			MAX (0, 13 - attributo(d->character, requisiti_ordini[d->character->iniz_classe][1]));
		for (i=0; i<6; i++)
			d->character->iniz_da_aggiungere -= d->character->iniz_ptagg[i];
		d->character->iniz_da_aggiungere = MAX(d->character->iniz_da_aggiungere, 0);
	}
	else
		strcpy(buf, "\r\n\r\nOkay, continuiamo nell'apprendistato...\r\n\r\n");
	SEND_TO_Q(buf, d);
	
	strcpy (buf, "Ecco i tuoi attributi:\r\n\r\n");
	strcat(buf, "                | Livello  | Livello Min |  Massimo   | Incremento ||  Livello\r\n");
	strcat(buf, "Attributo       | Iniziale |  Richiesto  | Incremento |   Attuale  ||  Attuale\r\n");
	strcat(buf, "--------------------------------------------------------------------------------\r\n");
	for (i=0; i<6; i++)
	{
		sprintf(numero, "%2d", max_incremento[d->character->iniz_classe][i]);
		sprintf(buf+strlen(buf), "%-15s |    %2d    |      %s    |     %s     |      %d      ||     %2d\r\n",
			nomi_attributi[i],
			attributo(d->character, i),
			((requisiti_ordini[d->character->iniz_classe][0] == i || requisiti_ordini[d->character->iniz_classe][1]==i) ? "13" : "  "),
			(max_incremento[d->character->iniz_classe][i] >= 10 ? "  " : numero),
			d->character->iniz_ptagg[i],
			(attributo(d->character, i)+d->character->iniz_ptagg[i]));
		strcat(buf, "--------------------------------------------------------------------------------\r\n");
	}
	
	if (d->character->iniz_da_aggiungere <= 0)
		strcat(buf, "\r\nNon hai piu' punti da distribuire ai tuoi attributi.\r\n"
			"Premi un tasto per continuare\r\n\r\n");
	else if (d->character->iniz_da_aggiungere == 1)
		strcat(buf, "\r\nTi resta un solo punto da distribuire ai tuoi attributi.\r\n");
	else
		sprintf(buf+strlen(buf), "Ti restano ancora da aggiungere %d punti ai tuoi attributi.\r\n", d->character->iniz_da_aggiungere);
	SEND_TO_Q(buf, d);
	
	strcpy(buf, "\r\nPuoi ancora incrementare i seguenti attributi:\r\n");
	for (i=0; i<6; i++)
		if ( (attributo(d->character, i)+d->character->iniz_ptagg[i]<16) && (d->character->iniz_ptagg[i]<max_incremento[d->character->iniz_classe][i]) )
		{
			sprintf(buf+strlen(buf), "%s  ", nomi_attributi_hint[i]);
			libero ++;
		}
	strcat(buf, "\r\nQuale attributo scegli ? ");
	if (libero && d->character->iniz_da_aggiungere>0)
	{
		SEND_TO_Q(buf, d);
		return;
	}
	if (!libero) strcpy(buf, "Tutti i tuoi attributi sono al massimo consentito, complimenti !\r\n");
	else         strcpy(buf, "");
	
	SEND_TO_Q(buf, d);
	GET_CLASS(d->character) = d->character->iniz_classe;
	d->character->real_abils.str += d->character->iniz_ptagg[0];
	d->character->real_abils.intel += d->character->iniz_ptagg[1];
	d->character->real_abils.wis += d->character->iniz_ptagg[2];
	d->character->real_abils.dex += d->character->iniz_ptagg[3];
	d->character->real_abils.con += d->character->iniz_ptagg[4];
	d->character->real_abils.cha += d->character->iniz_ptagg[5];
	affect_total(d->character);
	STATE(d) = CON_PREMI_TASTO;
	return;
}

void calcola_percentuali_mestieri(int * primo, int *secondo)
{
	extern int top_of_p_file;
	int i,t;
	int tot1=0, tot2=0;
	struct char_file_u temp;
	FILE *fp;
	
	for (i=0; i<=MAX_PROFICIENZE; i++)
	{
		primo[i]=0;
		secondo[i]=0;
	}
	
	if (!(fp = fopen(PLAYER_FILE, "r+")))
	{
		mudlog("Calcola_Percentuali_Mestieri: errore in apertura file player", NRM, LVL_GOD, TRUE);
		return;
	}
	
	for (i = 0; i < top_of_p_file; i++)
	{
		fread(&temp, sizeof(struct char_file_u), 1, fp);
		if (!IS_SET(temp.char_specials_saved.act, PLR_DELETED) &&
			!IS_SET(temp.player_specials_saved.pref, PRF_NO_EROE))
		{
			t=temp.player_specials_saved.prof_choice[0];
			if (t>=0 && t<=MAX_PROFICIENZE) primo[t]++;
			t=temp.player_specials_saved.prof_choice[1];
			if (t>=0 && t<=MAX_PROFICIENZE) secondo[t]++;
		}
	}
	for (i=1; i<=MAX_PROFICIENZE; i++)
	{
		tot1+=primo[i];
		tot2+=secondo[i];
	}
	if (tot1>0)
		for (i=1; i<=MAX_PROFICIENZE; i++)
			primo[i]=(100*primo[i])/tot1;
	if (tot2>0)
		for (i=1; i<=MAX_PROFICIENZE; i++)
			secondo[i]=(100*secondo[i])/tot2;
	
	fclose(fp);
}

void menu_mestieri(struct descriptor_data * d)
{
	int i;
	int percentuali1[MAX_PROFICIENZE+1];
	int percentuali2[MAX_PROFICIENZE+1];
	
	calcola_percentuali_mestieri(percentuali1, percentuali2);
	
	strcpy(buf,  "\r\n\r\n"
		"E' ora necessario che tu scelga i due mestieri che il tuo personaggio ha\r\n"
		"imparato nella sua giovinezza. I rapporti affianco a ogni mestiere indicano\r\n"
		"quante persone hanno scelto quel mestiere su Dalila, rispettivamente come\r\n"
		"mestiere principale o secondario.\r\n\r\n");
	for (i=4; i <= TOT_PROF_MESTIERI; i++)
	{
		if (d->character->player_specials->saved.prof_choice[0]!=i)
			sprintf(buf + strlen(buf), "  %2d) %c%-15s [%4d/ 100] [%4d/ 100]\r\n", (i-3),
				(proficienze[i][0]+'A'-'a'), proficienze[i]+1,
				percentuali1[i], percentuali2[i]);
	}
	if (d->character->player_specials->saved.prof_choice[0]==0)
		strcat(buf, "\r\nScegli il tuo mestiere principale: ");
	else
		strcat(buf, "\r\nScegli il tuo mestiere secondario: ");
	SEND_TO_Q(buf, d);
}

void menu_scelta_abilita(struct descriptor_data * d)
{
	int to_learn=0;
	int i;
	int conta;
	extern char *abilita[];
	extern struct abil_info_type abil_info[];
	
	for (i=0; i<3; i++)
		if (d->character->player_specials->saved.abil_choice[i]==0) to_learn++;
	SEND_TO_Q("\r\n\r\n"
		"Ora devi scegliere in quali delle seguenti abilita' vuoi specializzarti durante\r\n"
		"il tuo noviziato. Scegli con cura, perche' imparare in seguito si rivelera'\r\n"
		"molto piu' costoso... sia in termini di denaro che di fatica...\r\n\r\n", d);
	switch(to_learn)
	{
		case '3':
			SEND_TO_Q("Di tutte le abilita' qui elencate, ne puoi scegliere soltanto tre. Non\r\n"
				"una di piu', non una di meno...\r\n", d);
			break;
		case '2':
			SEND_TO_Q("Ora ti restano due abilita' ancora da scegliere, la scelta si fa piu' ardua...\r\n\r\n", d);
			break;
		case '1':
			SEND_TO_Q("E' la tua ultima chance per imparare qualcosa di veramente utile per il tuo futuro...\r\n"
				"Fai la tua ultima scelta, novizio.\r\n", d);
	}
	
	strcpy(buf, "\r\n\r\n");
	conta=1;
	for (i=1; i<TOP_ABILITA_DEFINE; i++)
		if (abil_info[i].start_abilita[(int)GET_CLASS(d->character)] &&
			d->character->player_specials->saved.abil_choice[0]!=i   &&
			d->character->player_specials->saved.abil_choice[1]!=i   &&
			d->character->player_specials->saved.abil_choice[2]!=i )
		{
			sprintf(buf+strlen(buf),"       %2d) %s\r\n", conta, abilita[i]);
			conta++;
		}
		
	strcat(buf, "\r\nDigita la tua scelta: ");
	SEND_TO_Q(buf, d);
}

