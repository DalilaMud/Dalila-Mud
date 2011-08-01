/* ************************************************************************
*   File: constants.c                                   Part of CircleMUD *
*  Usage: Numeric and string contants used by the MUD                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"

const char circlemud_version[] = {
	"Dalila MUD - Un circleMUD bpl11 modificato - Versione attuale 6.5.10 della seconda era di Dalila\r\n"};


/* strings corresponding to ordinals/bitvectors in structs.h ***********/


/* (Note: strings for class definitions in class.c instead of here) */


/* cardinal directions */
const char *dirs[] =
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"\n"
};
/* cardinal directions per le persone */
const char *dirs1[] =
{
	"nord",
	"est",
	"sud",
	"ovest",
	"alto",
	"basso",
	"\n"
};


/* ROOM_x */
const char *room_bits[] = {
	"DARK",
	"DEATH",
	"!MOB",
	"INDOORS",
	"PEACEFUL",
	"SOUNDPROOF",
	"!TRACK",
	"!MAGIC",
	"TUNNEL",
	"PRIVATE",
	"GODROOM",
	"HOUSE",
	"HCRSH",
	"ATRIUM",
	"OLC",
	"BFS_MARK (Res)",
	"DAMAGE",
	"ARENA",
	"LANDMARK",
	"NO_DRAW",
	"WARROOM",
	"FORGIA",
	"FABBRO",
	"FALEGNAMERIA",
	"CARPENTERIA",
	"PIETRE_PREZIOSE",
	"GIOIELLERIA",
	"SQUADRATURA",
	"PRIGIONE",
	"ERBORISTERIA",
	"LAB_ALCHIMIA",
	"MULINO/RIPULIT",
	"CUCINA",
	"SCUOIATURA",
	"CONCERIA",
	"COSTRUZIONE",
	"STOP COSTRUZ.",
	"INGRESSO",
	"NO_BUY",
	"NO_SELL",
	"ZECCA",
	"TRADE",
	"MAGAZZINO",
	"EDIFICABILE",
	"TEMPIO",
	"!WRAITH",
//PEPPE DAMAGE
	"DAMAGE_FUOCO",
	"DAMAGE_ACQUA",
	"DAMAGE_ARIA",
	"DAMAGE_TERRA",
	"BIG_HOUSE",
	"\n"
};


/* EX_x */
const char *exit_bits[] = {
	"DOOR",
	"CLOSED",
	"LOCKED",
	"PICKPROOF",
	"\n"
};


/* SECT_ */
const char *sector_types[] = {
	"Interno",
	"Citta'",
	"Campo aperto",
	"Foresta",
	"Collina",
	"Montagna",
	"Acqua (Nuoto)",
	"Acqua (No Nuoto)",
	"Sottacqua",
	"In Volo",
	"Strada",
	"\n"
};

const char *colori[] = {
	"Colore non definito",
	"Nero - Invisibile",
	"Rosso",
	"Verde",
	"Giallo",
	"Blu",
	"Magenta",
	"Azzurro",
	"Grigio",
	"\n"
};

/* SEX_x */
const char *genders[] =
{
	"Neutro",
	"Maschio",
	"Femmina"
};


/* POS_x */
const char *position_types[] = {
	"Morto",
	"Colpito a morte",
	"Tramortito",
	"Stordito",
	"Dormendo",
	"Riposando",
	"Seduto",
	"Combattendo",
	"In piedi",
	"\n"
};


/* PLR_x */
const char *player_bits[] = {
	"PKILLER",
	"THIEF",
	"FROZEN",
	"DONTSET",
	"WRITING",
	"MAILING",
	"CSH",
	"SITEOK",
	"NOSHOUT",
	"NOTITLE",
	"DELETED",
	"LOADRM",
	"!WIZL",
	"!DEL",
	"INVST",
	"CRYO",
	"QUESTOR",
	"TKILLER",
	"WARKILLER",
	"WANTED",
	"MUTO",
	"MINISTRO COSTR.",
	"MLTPLAY",
	"TRUST",
	"FANTASMA",
	"MODIFICATO",
	"CONCENTRATO",
	"ALL_OLC",
	"CATARSI",
	"GO_GILDA",
	"PAPA",
	"\n"
};


/* MOB_x */
const char *action_bits[] = {
	"SPEC",
	"SENTINEL",
	"SCAVENGER",
	"ISNPC",
	"AWARE",
	"AGGR",
	"STAY-ZONE",
	"WIMPY",
	"AGGR_EVIL",
	"AGGR_GOOD",
	"AGGR_NEUTRAL",
	"MEMORY",
	"HELPER",
	"!CHARM",
	"!SUMMN",
	"!SLEEP",
	"!BASH",
	"!BLIND",
	"WILDHUNT",
	"QUEST",
	"QUESTMASTER",
	"ASSASSIN",
	"HUNTER",
	"CAVALCATURA",
	"DRAGO",
	"SOFFIODRAGO",
	"SALVA_INV",
	"NEGOZIANTE",
	"BOUNTY KILLER",
	"NOKILL",
	"CIVILIAN",
	"CRIMINAL",
	"CERCATORE",
	"SALVA_MOB",
	"FAST-GO",
	"SELVAGGINA",
	"RIS.CARNE",
	"RIS.PELLE",
	"RIS.STOFFA",
	"RIS.PELLICCIA",
	"RIS.SCAGLIE",
	"BODYGUARD",
	"NECRO",
	"!AGGR_PROPR",
	"!AGGR CLAN ALLEAT",
	"!AGGR CLAN NEMICO",
	"!AGGR CLAN PACE",
	"!AGGR CLAN",
	"!AGGR CLAN VASS",
	"AMMAESTRABILE",
	"LAVORATORE",
	"CRIM-HELPER",  
	"MOB_CRASH",  
	"MOB_DELETE",  
	"MOB_CRASH_EQ",  
	"!XP",
	"STIPENDIATO",
	"TASSA_EQUIP",
	"ESERCITO",
	"!DECAPITA",
	"\n"
};


/* PRF_x */
const char *preference_bits[] = {
	"BRIEF",
	"COMPACT",
	"DEAF",
	"!TELL",
	"D_HP",
	"D_MANA",
	"D_MOVE",
	"AUTOEX",
	"!HASS",
	"QUEST",
	"SUMN",
	"!REP",
	"LIGHT",
	"C1",
	"C2",
	"!WIZ",
	"L1",
	"L2",
	"!AUC",
	"!GOS",
	"!GTZ",
	"RMFLG",
	"AFK",
	"D_GOLD",
	"D_XP",
	"D_DAM",
	"DIR",
	"ARENA",
	"SOUND",
	"DET_HD_SK",
	"NO_EROE",
	"D_CAV",
	"GDR",
	"NOKILL",
	"!OOC",
	"!REGNO",
	"\n"
};


/* AFF_x */
const char *affected_bits[] =
{
	"BLIND",
	"INVIS",
	"DET-ALIGN",
	"DET-INVIS",
	"DET-MAGIC",
	"SENSE-LIFE",
	"WATWALK",
	"SANCT",
	"GROUP",
	"CURSE",
	"INFRA",
	"POISON",
	"PROT-EVIL",
	"PROT-GOOD",
	"SLEEP",
	"NOTRACK",
	"MARK",
	"PASSDOOR",
	"SNEAK",
	"HIDE",
	"FIRESHD",
	"CHARM",
	"FLYING",
	"WATERBREATH",
	"PROTFIRE",
	"SHIELD",
	"DEATHDANCE",
	"MIRRORIMAGE",
	"BLINK",
	"WIZEYE",
	"PARALIZED",
	"DOMATO",
	"ACCAMPATO (R)",
	"TRAMORTITO",
	"IMMOBILIZZATO",
	"CAPTURING",
	"TRASPORTATO",
	"PROTLIGHTN",
	"DET-SNEAK",
	"SILENCE",
	"DISGUISE",
	"ANTIMAGIA",
	"AID",
	"CONGIUNZIONE",
	"CRIT_TESTA_1",    //PEPPE CRITICAL
	"CRIT_BRACCIO_D_1",
	"CRIT_BRACCIO_S_1",
	"CRIT_TORSO_1",
	"CRIT_GAMBA_D_1",
	"CRIT_GAMBA_S_1",
	"CRIT_TESTA_2",
	"CRIT_BRACCIO_D_2",
	"CRIT_BRACCIO_S_2",
	"CRIT_TORSO_2",
	"CRIT_GAMBA_D_2",
	"CRIT_GAMBA_S_2",
	"CRIT_TESTA_3",
	"CRIT_BRACCIO_D_3",
	"CRIT_BRACCIO_S_3",
	"CRIT_TORSO_3",
	"CRIT_GAMBA_D_3",
	"CRIT_GAMBA_S_3",
	"FERITO",
	"ENDURANCE",
	"\n"
};

/* AFF_x del secondo bitvector (by Spini) */

const char *affected_bits_1[] = 
{
	"BURNED",
	"CHILLED",
	"LEVITATE",
	"RAFFREDDATO",
	"APPESTATO",
	"ABBACCATO",
	"MALEDETTO_DA_SHAARR",
	"PIAGATO_ALLE_MANI",
	"MALEDETTO_DA_XHYPHYS",
	"AMMORFEATO",
	"\n"
};

/* AFF_x del terzo bitvector (by Spini) */

const char *affected_bits_2[] = 
{
	"VUOTO_1",
	"\n"
};

/* AFF_x del quarto bitvector (by Spini) */

const char *affected_bits_3[] = 
{
	"VUOTO_2",
	"\n"
};


/* CON_x */
const char *connected_types[] = {
	"In gioco",
	"Disconnessione",
	"Nome",
	"Conferma il nome",
	"Password",
	"Nuova PW",
	"Conferma nuova PW",
	"Sesso",
	"Ordine",
	"Legge MOTD",
	"Menu Principale",
	"Descrizione.",
	"Cambio PW 1",
	"Cambio PW 2",
	"Cambio PW 3",
	"Auto-Cancella 1",
	"Auto-Cancella 2",
	"Object edit",
	"Room edit",
	"Zone edit",
	"Mobile edit",
	"Shop edit",
	"Accetta Rollaggio",
	"Scelta Citta'",
	"Editor Testo",
	"Scelta Mestiere",
	"Trigger Edit",
	"Wild type edit",
	"Conferma Iniz.",
	"AggPunti Iniz.",
	"Premi Tasto Iniz.",
	"Scelta Abilita'",
	"\n"
};


/* WEAR_x - for eq list */
const char *where[] = {
	"&1[&3&bLuce&0&1]----------[&7",
	"&1[&7&bDito&0&1]----------[&7",
	"&1[&7&bDito&0&1]----------[&7",
	"&1[&4Collo&1]---------[&7",
	"&1[&4Collo&1]---------[&7",
	"&1[&4&bCorpo&0&1]---------[&7",
	"&1[&4&bTesta&0&1]---------[&7",
	"&1[&4&bGambe&0&1]---------[&7",
	"&1[&4&bPiedi&0&1]---------[&7",
	"&1[&4&bMani&0&1]----------[&7",
	"&1[&4&bBraccia&0&1]-------[&7",
	"&1[&1&bScudo&0&1]---------[&7",
	"&1[&4&bAddosso&0&1]-------[&7",
	"&1[&2&bVita&0&1]----------[&7",
	"&1[&4Polso Des&1]-----[&7",
	"&1[&4Polso Sin&1]-----[&7",
	"&1[&1&bImpugn. Des.&0&1]--[&7", 
	"&1[&1&bTenuto&0&1]--------[&7",
	"&1[&6&bOrecchio Sin.&0&1]-[&7",
	"&1[&6&bOrecchio Des.&0&1]-[&7",
	"&1[&7&bSpalle&0&1]--------[&7",
	"&1[&1&bChiusa sui Polsi&0&1]-[&7",
	"&1[&5&bOcchi&0&1]---------[&7",
	"&1[&5&bBocca&0&1]---------[&7",
	"&1[&7&bIndefinita&0&1]----[&7",
	"&1[&1&bImpugn. Sin&0&1]---[&7",
	"&1[&1&bTrainato&0&1]------[&7",
	"&1[&6&bVestito&0&1]-------[&7",
	//RELIQUIE PEPPE
	"&1[&8&7Reliquia&0&1]------[&7",
	"\n",
	"\n"
};


/* WEAR_x - for stat */
const char *equipment_types[] = {
	"Usato come luce",
	"Infilato al dito destro",
	"Infilato al dito sinistro",
	"Indossato per primo intorno al collo",
	"Indossato per secondo intorno al collo",
	"Indossato sul corpo",
	"Indossato sulla testa",
	"Indossato sulle gambe",
	"Indossato sui piedi",
	"Indossato sulle mani",
	"Indossato sulle braccia",
	"Usato come scudo",
	"Addosso al corpo",
	"Indossato alla vita",
	"Portato intorno al polso destro",
	"Portato intorno al polso sinistro",
	"Impugnato con la destra",
	"Tenuto in mano",
	"Appeso all'orecchio sinistro",
	"Appeso all'orecchio destro",
	"Messo sulle spalle",
	"Legato ai polsi",
	"Legato sugli occhi",
	"Legato sulla bocca",
	"indefinita contatta un creatore",
	"Impugnato con la sinistra",
	"Trasportato (a due mani)",
	"Indossato attorno al corpo",
	"Indossato come reliquia",
	"\n",
	"\n"
};


/* ITEM_x (ordinal object types) */
const char *item_types[] = {
	"UNDEFINED",
	"LIGHT",
	"SCROLL",
	"WAND",
	"STAFF",
	"WEAPON",
	"FIRE WEAPON",
	"MISSILE",
	"TREASURE",
	"ARMOR",
	"POTION",
	"WORN",
	"OTHER",
	"TRASH",
	"TRAP",
	"CONTAINER",
	"NOTE",
	"LIQ CONTAINER",
	"KEY",
	"FOOD",
	"MONEY",
	"PEN",
	"BOAT",
	"FOUNTAIN",
	"FLY",
	"PORTAL",
	"THROW",
	"GRENADE",
	"BOW",
	"SLING",
	"CROSSBOW",
	"BOLT",
	"ARROW",
	"ROCK",
	"RICEVUTA_STALLA",
	"CHIAVE RENTABILE",
	"CIBO PER CAVALLI",
	"MARTELLO",
	"SEGA",
	"PICCA",
	"SCUOIATORE",
	"FALCE",
	"ASCIA",
	"MARTELLETTO",
	"PIETRA",
	"ALBERO",
	"MINERALE",
	"NATURALE",
	"METALLO GREZZO (R)",
	"TRONCO (R)",
	"PIETRA GREZZA (R)",
	"PELLE GREZZA (R)",
	"PRODOTTO NATURALE (R)",
	"LINGOTTO (R)",
	"TRAVE LEGNO (R)",
	"PIETRA PULITA (R)",
	"PELLE PULITA (R)",
	"CIBO CUOCERE (R)",
	"TRAVE COSTRUZIONE (R)",
	"ROCCIA SQUADRATA (R)",
	"MARTELLO COSTRUZIONE",
	"ASSE LEGNO (R)",
	"ROCCIA",
	"ROCCIA GREZZA (R)",
	"GEMMA (R)",
	"TALISMANO DRAGO",
	"ERBA",
	"MANETTE",
	"MANGANELLO",
	"BENDA",
	"PICCOZZA",
	"ERBA GREZZA (R)",
	"ERBA PULITA (R)",
	"BOCCETTA ALCH.",
	"TRITAERBE",
	"BAVAGLIO",
	"CANNA DA PESCA",
	"STRUM. CUCINA",
	"MATTARELLO",
	"STRUM. RIPULITURA",
	"PASTELLA (R)",
	"PELLE (R)",
	"ARMA DA CACCIA",
	"STR. CONCERIA",
	"ATTO NOTARILE",
	"PORTA",
	"CONTRATTO VEND.",
	"TRAPPOLA",
	"VESTE",
	"RELIQUIA",
	"LIBRO MAGICO WAND",
	"LIBRO MAGICO STAFF",
	"WEAPON 2 HANDS",
	"TISANA CALDA",
	"TISANA RAFFREDDATA",
	"BENDE UNTE",
	"POLVERE",
	"\n"
};
/* object damage */
const char *obj_quality[] =
{/* 0..10% */   " &5..quasi da buttare.&0",
	" &1..in pessimo stato.&0",
	" &1..in cattivo stato.&0",
	" &1..in brutto stato.&0",
/* 40..50% */   " &3..in stato mediocre.&0",
	" &3..in discreto stato.&0",
	" &3..in buono stato.&0",
	" &2..in stato molto buono.&0",
	" &2..in ottimo stato.&0",
/* 90..100% */  " &2..in perfetto stato.&0",
	"\n"
	
};

/* ITEM_WEAR_ (wear bitvector) */
const char *wear_bits[] = {
	"TAKE",
	"FINGER",
	"NECK",
	"BODY",
	"HEAD",
	"LEGS",
	"FEET",
	"HANDS",
	"ARMS",
	"SHIELD",
	"ABOUT",
	"WAIST",
	"WRIST",
	"WIELD",
	"HOLD",
	"LOBSX",
	"SPALLE",
	"POLSI",
	"OCCHI",
	"BOCCA",
	"ALTRO",
	"HANG",
	"VESTE",
	"RELIQUIA",
	"\n"
};


/* ITEM_x (extra bits) */
const char *extra_bits[] = {
	"GLOW",
	"HUM",
	"NORENT",
	"NODONATE",
	"NOINVIS",
	"INVISIBLE",
	"MAGIC",
	"NODROP",
	"BLESS",
	"NOGOOD",
	"NOEVIL",
	"NONEUTRAL",
	"NOPANDION",
	"NOCYRINIC",
	"NOALCIONE",
	"NOGENIDIAN",
	"NOSELL",
	"NOPELOI",
	"LIVE_GRENADE",
	"NOLOCATE",
	"NO_5_LIV",
	"NO_10_LIV",
	"NO_20_LIV",
	"NO_25_LIV",
	"NO_30_LIV",
	"NO_40_LIV",
	"RESTRING (R)",
	"CORPSE (R)",
	"FORGIATO",
	"AFFILATO",
	"RINOM_ALIAS",
	"RINOM_NAME",
	"RINOM_DESCR",
	"NO_IDENT",
	"QUEST",
	"\n"
};


/* APPLY_x */
const char *apply_types[] = {
	"NONE",
	"STR",
	"DEX",
	"INT",
	"WIS",
	"CON",
	"CHA",
	"CLASS",
	"LEVEL",
	"AGE",
	"CHAR_WEIGHT",
	"CHAR_HEIGHT",
	"MAXMANA",
	"MAXHIT",
	"MAXMOVE",
	"GOLD",
	"EXP",
	"ARMOR",
	"HITROLL",
	"DAMROLL",
	"SAVING_PARA",
	"SAVING_ROD",
	"SAVING_PETRI",
	"SAVING_BREATH",
	"SAVING_SPELL",
	"HIT_REGEN",
	"MANA_REGEN",
	"RES_FUOCO",
	"RES_GHIACCIO",
	"RES_ELETTRICITA",
	"RES_ACIDO",
	"RES_OSCURITA",
	"RES_CAOS",
	"RES_ORDINE",
	"RES_LUCE",
	"RES_FISICO",
	"\n"
};


/* CONT_x */
const char *container_bits[] = {
	"CLOSEABLE",
	"PICKPROOF",
	"CLOSED",
	"LOCKED",
	"\n",
};


/* LIQ_x */
const char *drinks[] =
{
	"acqua",
	"birra",
	"vino",
	"birra rossa",
	"birra scura",
	"whisky",
	"limonata",
	"respiro di fuoco",
	"specialita' locale",
	"succo di frutta",
	"latte",
	"te",
	"caffe",
	"sangue",
	"acqua salata",
	"acqua fresca",
	"caffe' e limone",
	"\n"
};


/* other constants for liquids ******************************************/


/* one-word alias for each drink */
const char *drinknames[] =
{
	"acqua",
	"birra",
	"vino",
	"birra",
	"birra",
	"whisky",
	"limonata",
	"fuoco",
	"locale",
	"succo",
	"latte",
	"te",
	"caffe",
	"sangue",
	"sale",
	"acqua",
	"caffelimone",
	"\n"
};

const char *spec_procs[]={
	"Nessuna",
	"Postmaster",
	"Majordome",
	"Cityguard",
	"Receptionist",
	"Cryogenicist",
	"Guild Guard",
	"Guild Master",
	"Abil Master",
	"Puff",
	"Fido",
	"Janitor",
	"Mayor",
	"Snake",
	"Thief",
	"Magic User",
	"Warrior",
	"Archer",
	"Cleric",
	"Sund Earl",
	"Hangman",
	"Blinder",
	"Silktrader",
	"Butcher",
	"Idiot",
	"Athos",
	"Stu",
	"Dragon Fire",
	"Dragon Gas",
	"Dragon Frost",
	"Dragon Acid",
	"Dragon Lightning",
	"Dragon Guard",
	"Trainer",
	"Regina",
	"Horseman",
	"Dragonman",
	"Secondino",
	"Maestro Err.",
	"Maestro Sant.",
	"Manovale",
	"Vendor Manovale",
	"Oste",
	"Attack QuestMob", //Orione
	"Venditore Armate",
	"Guardia Sentinel",
	"Guardia stabber",
	"Untore",
	"Esercito",
	"\n"
};

/* effect of drinks on hunger, thirst, and drunkenness -- see values.doc */
const int drink_aff[][3] = {
	{0, 1, 10},
	{3, 2, 5},
	{5, 2, 5},
	{2, 2, 5},
	{1, 2, 5},
	{6, 1, 4},
	{0, 1, 8},
	{10, 0, 0},
	{3, 3, 3},
	{0, 4, -8},
	{0, 3, 6},
	{0, 1, 6},
	{0, 1, 6},
	{0, 2, -1},
	{0, 1, -2},
	{0, 0, 13},
	{0, 0, 0},
};


/* color of the various drinks */
const char *color_liquid[] =
{
	"trasparente",
	"marrone",
	"chiara",
	"marrone",
	"scura",
	"dorata",
	"rossa",
	"verde",
	"chiara",
	"verde chiara",
	"bianca",
	"marrone",
	"nera",
	"rossa",
	"chiara",
	"chiara cristallina"
};


/* level of fullness for drink containers */
const char *fullness[] =
{
	"meno della meta' ",
	"circa meta' ",
	"piu' che la meta' ",
	""
};


/* str, int, wis, dex, con applies **************************************/


/* [ch] strength apply (all) */
const struct str_app_type str_app[] = {
	{-5, -4, 0, 0},	/* str = 0 */
	{-5, -4, 3, 1},	/* str = 1 */
	{-3, -2, 3, 2},
	{-3, -1, 10, 3},
	{-2, -1, 25, 4},
	{-2, -1, 55, 5},	/* str = 5 */
	{-1, 0, 80, 6},
	{-1, 0, 90, 7},
	{0, 0, 100, 8},
	{0, 0, 100, 9},
	{0, 0, 115, 10},	/* str = 10 */
	{0, 0, 115, 11},
	{0, 1, 150, 13},
	{1, 1, 160, 14},
	{1, 1, 175, 15},
	{1, 2, 185, 16},	/* str = 15 */
	{2, 2, 195, 17},
	{2, 3, 220, 18},
	{3, 4, 255, 20},	/* str = 18 */
	{4, 9, 640, 40},
	{4, 10, 700, 40},	/* str = 20 */
	{4, 11, 810, 40},
	{4, 12, 970, 40},
	{5, 13, 1130, 40},
	{6, 14, 1440, 40},
	{7, 15, 5000, 40},	/* str = 25 */
	{3, 5, 280, 22},	/* str = 18/0 - 18-50 */
	{3, 6, 305, 24},	/* str = 18/51 - 18-75 */
	{4, 6, 340, 26},	/* str = 18/76 - 18-90 */
	{4, 7, 380, 28},	/* str = 18/91 - 18-99 */
	{4, 8, 520, 30}	/* str = 18/100 */
};



/* [dex] skill apply (thieves only) */
const struct dex_skill_type dex_app_skill[] = {
	{-99, -99, -90, -99, -60},	/* dex = 0 */
	{-90, -90, -60, -90, -50},	/* dex = 1 */
	{-75, -80, -40, -80, -45},
	{-60, -70, -30, -70, -40},
	{-45, -60, -30, -60, -35},
	{-30, -50, -20, -50, -30},	/* dex = 5 */
	{-20, -40, -20, -40, -25},
	{-10, -30, -15, -30, -20},
	{-5 , -20, -15, -20, -15},
	{0  , -10, -10, -20, -10},
	{0  , -5, -10, -15, -5},	/* dex = 10 */
	{0, 0, -5, -10, 0},
	{0, 0, 0, -5, 0},
	{5, 0, 0, 0, 0},
	{5, 0, 0, 0, 0},
	{5, 0, 0, 0, 0},		/* dex = 15 */
	{10, 5, 0, 0, 0},
	{10, 10, 0, 5, 5},
	{15, 15, 5, 10, 10},		/* dex = 18 */
	{20, 20, 10, 15, 15},
	{25, 20, 10, 15, 15},		/* dex = 20 */
	{30, 25, 10, 15, 20},
	{35, 25, 15, 20, 20},
	{40, 25, 15, 20, 20},
	{45, 30, 15, 25, 25},
	{50, 30, 15, 25, 25}		/* dex = 25 */
};


/* [dex] apply (all) */
struct dex_app_type dex_app[] = {
	{-7, -7, 6,1},		/* dex = 0 */
	{-7, -6, 5,2},		/* dex = 1 */
	{-6, -4, 4,3},
	{-6, -4, 3,4},
	{-5, -3, 3,5},
	{-4, -2, 2,6},		/* dex = 5 */
	{-3, -1, 2,7},
	{-2, -1, 1,8},
	{-1, -1, 1,9},
	{0, 0, 0,15},
	{0, 0, 0,18},		/* dex = 10 */
	{0, 0, 0,20},
	{0, 0, 0,22},
	{1, 1, -1,24},
	{1, 1, -1,26},
	{2, 2, -2,28},		/* dex = 15 */
	{3, 2, -3,30},
	{4, 3, -4,32},
	{5, 3, -5,35},		/* dex = 18 */
	{6, 4, -6,40},
	{6, 4, -6,50},		/* dex = 20 */
	{7, 5, -7,60},
	{7, 5, -7,70},
	{8, 6, -7,80},
	{8, 7, -8,100},
	{9, 7, -8,200}		/* dex = 25 */
};



/* [con] apply (all) */
struct con_app_type con_app[] = {
	{-4, 20},		/* con = 0 */
	{-3, 25},		/* con = 1 */
	{-3, 30},
	{-3, 35},
	{-2, 40},
	{-2, 45},		/* con = 5 */
	{-1, 50},
	{-1, 55},
	{-1, 60},
	{0, 65},
	{0, 70},		/* con = 10 */
	{0, 75},
	{0, 80},
	{1, 85},
	{1, 88},
	{1, 90},		/* con = 15 */
	{2, 95},
	{2, 97},
	{3, 99},		/* con = 18 */
	{3, 99},
	{4, 99},		/* con = 20 */
	{5, 99},
	{5, 99},
	{5, 99},
	{6, 99},
	{6, 99}		/* con = 25 */
};



/* [int] apply (all) */
struct int_app_type int_app[] = {
	{3},		/* int = 0 */
	{5},		/* int = 1 */
	{7},
	{8},
	{9},
	{10},		/* int = 5 */
	{11},
	{12},
	{13},
	{15},
	{17},		/* int = 10 */
	{19},
	{22},
	{25},
	{30},
	{35},		/* int = 15 */
	{40},
	{45},
	{50},		/* int = 18 */
	{53},
	{55},		/* int = 20 */
	{56},
	{57},
	{58},
	{59},
	{60}		/* int = 25 */
};


/* [wis] apply (all) */
struct wis_app_type wis_app[] = {
	{0},	/* wis = 0 */
	{0},  /* wis = 1 */
	{0},
	{0},
	{0},
	{0},  /* wis = 5 */
	{0},
	{0},
	{0},
	{0},
	{0},  /* wis = 10 */
	{0},
	{0},
	{0},
	{0},
	{0},  /* wis = 15 */
	{0},
	{1},
	{1},
	{1},  /* wis = 15 */
	{2},
	{2},
	{3},	/* wis = 18 */
	{3},
	{4},
	{5},
	{5},
	{6}  /* wis = 25 */
};



const char *spell_wear_off_msg[] = {
	"RESERVED DB.C",		/* 0 */
	"Ti senti meno protetto.",	/* 1 */
	"!Teleport!",
	"Ti senti piu' sfiduciato.",
	"You feel a cloak of blindness disolve.",
	"!Burning Hands!",		/* 5 */
	"!Call Lightning!",
	"Hai piu' fiducia in te stesso.",
	"Senti tornare le forze.",
	"!Clone!",
	"!Color Spray!",		/* 10 */
	"!Control Weather!",
	"!Create Food!",
	"!Create Water!",
	"!Cure Blind!",
	"!Cure Critic!",		/* 15 */
	"!Cure Light!",
	"Ti senti piu' ottimista.",
	"Ti senti meno consapevole.",
	"I tuoi occhi smettono di luccicare.",
	"L'effetto del detect magic svanisce.",/* 20 */
	"L'effetto del detect poison svanisce.",
	"!Dispel Evil!",
	"!Earthquake!",
	"!Enchant Weapon!",
	"!Energy Drain!",		/* 25 */
	"!Fireball!",
	"!Harm!",
	"!Heal!",
	"Ti senti piu' esposto.",
	"!Lightning Bolt!",		/* 30 */
	"!Locate object!",
	"!Magic Missile!",
	"Ti senti meno malato.",
	"Ti senti meno protetto.",
	"!Remove Curse!",		/* 35 */
	"La bianca aura attorno al tuo corpo svanisce.",
	"!Shocking Grasp!",
	"Ti senti meno stanco.",
	"Ti senti piu' debole.",
	"!Summon!",			/* 40 */
	"!Ventriloquate!",
	"!Word of Recall!",
	"!Remove Poison!",
	"Ti senti meno consapevole di cio' che ti sta intorno.",
	"!Animate Dead!",		/* 45 */
	"!Dispel Good!",
	"!Group Armor!",
	"!Group Heal!",
	"!Group Recall!",
	"La tua visione nello scuro sembra svanire.",	/* 50 */
	"I tuoi piedi sembrano meno affidabili.",
	"!Relocate!",
	"!Disintegrate!",
	"Torni con i piedi per terra.",
	"!Minute Meteor!",
	"Ti senti grintoso!",
	"!Area lightining!",
	"Le fiamme che ti circondano si spengono poco a poco.",
	"!GAS BREATH!",
	"Il calore ritorna nel tuo corpo.",
	"Finisci di dissolverti.",
	"!LIGHTNING BREATH!",
	"!Blade Barrier!",
	"!Peace!",
	"!Identify!",
	"!Acid arrow!",
	"!Flame arrow!",
	"Lentamente riposi i piedi sulla terra.",
	"Ti senti meno isolato.",
	"Ritorni alla tua normalita'.",
	"La tua pelle torna normale.",
	"La tua pelle torna alla consistenza normale!",
	"!Group Fly!",
	"!Group Invis!",
	"!Group prot evil!",
	"!group watbreath!",
	"!Cure serious!",
	"!MONSUM 1!",
	"!MONSUM 2!",
	"!MONSUM 3!",
	"!MONSUM 4!",
	"!MONSUM 5!",
	"!Conj elemental!",
	"Lentamente il tuo scudo di protezione si esaurisce.",
	"Le tue immagini si ricombinano in una.",
	"Ritorni consapevole di quello che ti circonda.",
	"La tua conchiglia calda di protezione svanisce.",
	"!portal!",
	"Torni consistente!",
	"!CREATE LIGHT!",
	"!FEAST!",
	"!HEROES!",
	"!REFRESH!",
	"!CONTLIGHT!",
	"!LOCATE TARGET!",
	"!FEAR!",
	"Lo scudo di fuoco si spegne poco a poco.",
	"!DISPELLMAGIC!",
	"I tuoi occhi e la tua mente tornano a separarsi.",
	"Finalmente senti il sangue scorrere ancora nelle tue vene.",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"La carica negativa che ti circonda svanisce.",
	"La tua percezione dei rumori termina.",
	"Riesci a parlare nuovamente.",
	"La tua aura antimagia svanisce.... ora sei piu' vulnerabile.",
	"Il tuo camuffamento ha fine...",
	"L'aiuto degli Dei ti abbandona.",
	"Non fai piu' parte della Cerchia!",
	"!UNUSED!",
};



const char *npc_class_types[] = {
	"Normale",
	"Non morto",
	"\n"
};



const int rev_dir[] =
{
	2,
	3,
	0,
	1,
	5,
	4
};


const int movement_loss[] =
{
	1,	/* Inside     */
	1,	/* City       */
	2,	/* Field      */
	3,	/* Forest     */
	4,	/* Hills      */
	6,	/* Mountains  */
	4,	/* Swimming   */
	1,	/* Unswimable */
	1,	/* Flying     */
	5,     /* Underwater */
	1	/* Road       */
};


const char *weekdays[] = {
	"giorno della Luna",
	"giorno del Toro",
	"giorno del Coraggio",
	"giorno del Tuono",
	"giorno della Liberta'",
	"giorno degli Dei",
	"giorno del Sole"};


const char *month_name[] = {
	"Mese dell'Inverno",		/* 0 */
	"Mese del Lupo Bianco",
	"Mese del Gigante dei Ghiacci",
	"Mese delle Forze Antiche",
	"Mese del Grande Cruccio",
	"Mese della Primavera",
	"Mese della Natura",
	"Mese della Futilita'",
	"Mese del Drago",
	"Mese del Sole",
	"Mese del Caldo",
	"Mese della Guerra",
	"Mese del Crepuscolo",
	"Mese delle Ombre",
	"Mese delle Ombre Lunghe",
	"Mese delle Antiche Oscurita'",
	"Mese del Grande Male",
};


const int sharp[] = {
	0,
	0,
	0,
	1,				/* Slashing */
	0,
	0,
	0,
	0,				/* Bludgeon */
	0,
	0,
	0,
	0};				/* Pierce   */


const char * class_titles_male[NUM_CLASSES][3]=
{
	{"Stregone","Mago","Arcimago"},
	{"Chierico","Sacerdote","Mistico"},
	{"Bandito","Ladro","Assassino"},
	{"Barbaro","Cavaliere", "Campione"},
	{"Scout","Esploratore", "Incursore"},
	{"Cittadino", "Signore", "Nobile"}
};

const char * class_titles_female[NUM_CLASSES][3]=
{
	{"Fattucchiera","Maga","Incantatrice"},
	{"Guaritrice","Vestale","Mistica"},
	{"Brigante","Ladra","Ombra"},
	{"Guerriera","Eroina", "Condottiera"},
	{"Guida","Esploratrice", "Amazzone"},
	{"Cittadina", "Signora", "Nobile"}
};

const char * frasi_resistenze[][6] = {
	{"Sei un ottimo combusitibile!","E' meglio se non giochi con il fuoco...",
	 "Arrostiresti con qualche difficolta'.","Le fiamme non ti spaventano.",
	 "Ti senti come Mangiafuoco!","Sei totalmente immune al fuoco."},
	{"Temi il gelo. Brrrrr...","Non apprezzi i climi polari.",
	 "Sei un tipo caliente","E' come se girassi con la pelliccia dello Yeti.",
	 "Sei come un pinguino!","Sei totalmente immune al ghiaccio."},
	{"Sei un ottimo conduttore elettrico!","Fossi in te non andrei a caccia di fulmini.",
	 "Una scossarella ogni tanto ti galvanizza.","Non temi le tempeste di fulmini.",
	 "I fulmini ti temono!!!","Sei completamente immune all'elettricita'!"},
	{"Ti sciogli facilmente...","Le tue carni si corrodono facilmente.",
	 "La corrosione ti dovrebbe ancora preoccupare","L'acido fatica a penetrare la tua pelle indurita.",
	 "Sei troppo un duro anche per l'acido!","Sei completamente immune all'acido!"},
	{"L'oscurita' potrebbe annientarti facilmente.","Hai molte ragioni per temere l'oscurita'.",
	 "La tua nobilta' d'animo ti da' una piccola protezione dall'oscurita'.","La purezza del tuo spirito ti difende dal potere delle tenebre.",
	 "Un'aura di Luce ti avvolge preservandoti dall'Oscurita'.","La Sacra Luce ti circonda rendendoti resistente alle tenebre."},
	{"La tua instabile mente non potrebbe opporre alcuna resistenza al Caos.","Il potere del Caos e' troppo per te.",
	 "Un certo ordine naturale ti protegge leggermente dal Caos.","Una spinta rigorosa dentro di te ti protegge parzialmente dai poteri di Xhyphys.",
	 "L'ordine nella tua mente ti fa sentire protetto dalle forze del Caos.","Sei l'incarnazione dell'Ordine naturale, e solo una grossa spinta caotica ti potrebbe far vacillare."},
	{"I poteri della natura potrebbero ridurti in un'ordinata poltiglia.","La natura possiede grandi poteri, e tu non hai i mezzi per contrastarla.",
	 "Qualcosa dentro di te sembra contrastare con poca efficacia i poteri della natura.","Grazie ai poteri del Caos, pensi che sfidare la natura non possa essere poi un gesto cosi' folle.",
	 "I poteri del Caos si agitano, proteggendoti dall'asfissiante ordine naturale.","Senti di poter muovere le forze del Caos affinche' i fautori dell'ordine possano provocarti solo danni irrilevanti."},
	{"Devi fuggire il sacro potere della Luce!","La luce di Silue ti potrebbe colpire senza pieta'.",
	 "Un velo di oscurita' ti protegge parzialmente dalla Sacra Luce","La tua anima erge una barriera di oscurita' contro la Sacra Luce",
	 "L'impurita' della tua anima fa si' che Silue non ti possa provocare grandi sofferenze.","L'oscurita' intorno a te e' cosi' grande che poca della Sacra Luce potrebbe penetrarne."},
	{"Sei una mozzarella...","Meglio evitare gli assalti frontali!",
	 "Non ti senti abbastanza protetto da ignorare un pugnale nella schiena!","I colpi nemici non ti impensieriscono.",
	 "La tua pelle ha ormai la resistenza di una corazza.","Resisteresti alla carica di 100 alci del nord!"}
};
	
const char * names[][2]={
	{ "John","Xana"},            /*0*/
	{ "Jack","Emy"},
	{ "Jim","Johanna"},
	{ "Adso","Elle"},
	{ "Donovan","Fran"},
	{ "Duncan","Tessa"},        /*5*/
	{ "Gill","Kim",},
	{ "Karim","Elisa"},
	{ "Liam","Betty"},
	{ "Goro","Siria"},
	{ "Bud","Vera"},           /*10*/
	{ "Bill","Veronica"},
	{  "Benjamin","Truth"},
	{ "Bargle","Hope"},
	{ "Max","Marylin"},
	{ "Axe","Eva"},           /*15*/
	{ "Julian","Jenny"},
	{ "Rusty","Rebecca"},
	{ "Ali","Wilma"},
	{ "Lince","Cindy"},
	{ "Skare","Amanda"},      /*20*/
	{ "Oscar","Ofelia"},
	{ "Jerry","Lidia"},
	{ "Nick","Barbara"},
	{ "Gatsu","Myriam"},
	{ "Art","Alice"},         /*25*/
	{ "Andy","Yvonne"},
	{ "Arthur","Glenda"},
	{ "Fiz","Fiona"},
	{ "Thor","Tina"},
	{ "Randy","Rita"}       /*30*/
};

const int abilita_vector[][16]={
  /*   1       3           6       8 E 9           12      14 M15*/
	{0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},/*0:RSRVD*/
	{0,  1,  1,  2,  2,  2,  3,  3,  3,  5,  6,  6,  7,  7,  8,  0},/*1:SPADA*/
	{0,  1,  1,  2,  2,  2,  3,  3,  3,  5,  6,  6,  7,  7,  8, 20},/*2:ASCIA*/
	{0,  1,  1,  2,  2,  2,  3,  3,  3,  5,  6,  6,  7,  7,  8,  0},/*3:PUGNALE*/
	{0,  1,  1,  2,  2,  2,  3,  3,  3,  5,  6,  6,  7,  7,  8,  15},/*4:MAZZA*/
	{0,  1,  1,  2,  2,  2,  3,  3,  3,  5,  6,  6,  7,  7,  8,  5},/*5:LANCIA*/
	{0,  1,  1,  2,  2,  2,  3,  3,  3,  5,  6,  6,  7,  7,  8, 11},/*6:SCUDO */
	{0, 10, 20, 30, 40, 50, 60, 70, 80,135,150,165,180,195,210,250},/*7:CUOIO*/
  /*mana rispam(*10)*/
  //{0,  3,  3,  6,  6,  6,  9,  9,  9, 15, 18, 18, 21, 21, 24, 30},/*8:MAGLIA*/
  //{0,  3,  6,  6,  9,  9, 12, 12, 15, 27, 30, 33, 36, 39, 42, 50},/*9:PIASTR*/
  //{0,  2,  4,  4,  6,  6,  8,  8, 10, 20, 22, 24, 26, 28, 30, 35},/*10:TRATT*/
	{0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 15, 18},/*8:MAGLIA*/
	{0,  2,  3,  4,  5,  6,  7,  9, 11, 13, 15, 17, 19, 21, 23, 25},/*9:PIASTR*/
	{0,  2,  4,  4,  6,  6,  8,  8, 10, 20, 22, 24, 26, 28, 30, 35},/*10:TRATT*/
  /* % GOLD*/
  /*   1                           8 E 9                    14 M15*/
	{0,  3,  6,  9, 12, 15, 18, 21, 24, 36, 40, 44, 48, 52, 56, 75},/*11:REPAIR*/
	{0,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  6,  6,  6, 22},/*12:BDBILD*/
	{0,  5,  5,  5,  5,  5,  5,  5,  5,  7,  7,  7,  7,  7,  7, 18},/*13:MEDIT */
	{0,  2,  4,  6,  8, 10, 12, 14, 16, 27, 30, 33, 36, 39, 42, 60},/*14:TRAPP */
	{0, 50,100,150,200,250,300,350,400,675,750,825,900,975,1050,1500},/*15:STUD */
  /*% Exp in + (*100)*/
	{0,  5, 10, 15, 20, 25, 30, 35, 40, 90,100,110,120,130,140,200},/*16:DIPLO*/
  /*%FAMA in piu (*10)*/
	{0,  2,  4,  6,  8, 10, 12, 14, 16, 27, 30, 33, 36, 39, 42, 60},/*17:PERCEZ*/
  /* % di percezione */
	{0,  1,  1,  1,  2,  2,  2,  2,  3,  4,  4,  4,  5,  5,  5,  6},/*18:REGEN*/
	{0,  2,  4,  6,  8, 10, 12, 14, 16,22, 25, 27, 30,32, 35, 40}/*19:AGGUATO*/
};

#if defined(OASIS_MPROG)
/*
 * Definitions necessary for MobProg support in OasisOLC
 */
const char *mobprog_types[] = {
	"INFILE",
	"ACT",
	"SPEECH",
	"RAND",
	"FIGHT",
	"DEATH",
	"HITPRCNT",
	"ENTRY",
	"GREET",
	"ALL_GREET",
	"GIVE",
	"BRIBE",
	"\n"
};
#endif
