/* ************************************************************************
*   File: structs.h          35                         Part of CircleMUD *
*  Usage: header file for central structures and contstants               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


/* preamble *************************************************************/

#define NOWHERE    -1    /* nil reference for room-database	*/
#define NOTHING	   -1    /* nil reference for objects		*/
#define NOBODY	   -1    /* nil reference for mobiles		*/

#define SPECIAL(name) \
   int (name)(struct char_data *ch, void *me, int cmd, char *argument)

/* misc editor defines **************************************************/ 

/* format modes for format_text */
#define FORMAT_INDENT		(1 << 0)


/* room-related defines *************************************************/


/* The cardinal directions: used as index to room_data.dir_option[] */
#define NORTH          0
#define EAST           1
#define SOUTH          2
#define WEST           3
#define UP             4
#define DOWN           5

/*Adriano eventi del PG*/
#define LAST_BASH      0
#define LAST_STEAL     1
#define LAST_BREATH    2
#define LAST_CHKRIDING 3
#define LAST_PICK      4
#define LAST_TKILLER   5
#define LAST_LOOKCAMUF 6
#define LAST_ALLERT    7
#define LAST_TORNADO   8
#define LAST_AGGUATO   9
#define LAST_AFK       10

/*ATTENZIONE NUM_EVENTS E' IL NUMERO DEGLI EVENTI SOPRA DEFINITI! (LAST_???)*/
#define NUM_EVENTS 11
/*Adriano*/


//Le seguenti definizioni servono per le critical hit e i colpi mirati
//PEPPE LOCATION  
#define LOCATION_TESTA     1
#define LOCATION_BRACCIO_D 2
#define LOCATION_BRACCIO_S 3
#define LOCATION_TORSO     4
#define LOCATION_GAMBA_D   5
#define LOCATION_GAMBA_S   6
#define LOCATION_RANDOM    7

// Numeri delle religioni
#define RELIGIONE_NESSUNA	0
#define RELIGIONE_SHAARR	1
#define RELIGIONE_XHYPYS	2
#define RELIGIONE_SILUE		3
#define RELIGIONE_THERION	4
#define NUM_RELIGIONI		5
#define MAX_MEMBRI_RELIGIONE	7


#define CAN_NOT_SHOW_ROOM 0 /* Non permette il look_at_room quando ti muovi  Lance*/
#define CAN_SHOW_ROOM 1  /* Permette il look_at_room quando ti muovi  Lance*/

/* Room flags: used in room_data.room_flags */
/* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved") */
#define ROOM_DARK		(1 << 0)   /* Dark			*/
#define ROOM_DEATH		(1 << 1)   /* Death trap		*/
#define ROOM_NOMOB		(1 << 2)   /* MOBs not allowed		*/
#define ROOM_INDOORS		(1 << 3)   /* Indoors			*/
#define ROOM_PEACEFUL		(1 << 4)   /* Violence not allowed	*/
#define ROOM_SOUNDPROOF		(1 << 5)   /* Shouts, gossip blocked	*/
#define ROOM_NOTRACK		(1 << 6)   /* Track won't go through	*/
#define ROOM_NOMAGIC		(1 << 7)   /* Magic not allowed		*/
#define ROOM_TUNNEL		(1 << 8)   /* room for only 1 pers	*/
#define ROOM_PRIVATE		(1 << 9)   /* Can't teleport in		*/
#define ROOM_GODROOM		(1 << 10)  /* LVL_GOD+ only allowed	*/
#define ROOM_HOUSE		(1 << 11)  /* (R) Room is a house	*/
#define ROOM_HOUSE_CRASH	(1 << 12)  /* (R) House needs saving	*/
#define ROOM_ATRIUM		(1 << 13)  /* (R) The door to a house	*/
#define ROOM_OLC		(1 << 14)  /* (R) Modifyable/!compress	*/
#define ROOM_BFS_MARK		(1 << 15)  /* (R) breath-first srch mrk	*/
#define ROOM_DAMAGE             (1 << 16)
#define ROOM_ARENA              (1 << 17)
#define ROOM_LANDMARK           (1 << 18)
#define ROOM_NO_DRAW            (1 << 19)
#define ROOM_WAR_ROOM           (1 << 20)
#define ROOM_FORGIA             (1 << 21)
#define ROOM_FABBRO             (1 << 22)
#define ROOM_FALEGNAMERIA       (1 << 23)
#define ROOM_CARPENTERIA        (1 << 24)
#define ROOM_PIETRE_PREZIOSE    (1 << 25)
#define ROOM_GIOIELLERIA        (1 << 26)
#define ROOM_SQUADRATURA        (1 << 27)
#define ROOM_PRISON             (1 << 28)
#define ROOM_ERBORISTERIA       (1 << 29)
#define ROOM_LAB_ALCHIMIA       (1 << 30)
#define ROOM_MULINO_RIPUL       ((long long int) 1 << 31)
#define ROOM_CUCINA             ((long long int) 1 << 32)
#define ROOM_SCUOIATURA         ((long long int) 1 << 33)
#define ROOM_CONCERIA           ((long long int) 1 << 34)
#define ROOM_CONSTRUZIONE       ((long long int) 1 << 35)
#define ROOM_STOP_COSTR         ((long long int) 1 << 36)
#define ROOM_FIRST_ROOM         ((long long int) 1 << 37)
#define ROOM_NOSELL             ((long long int) 1 << 38)
#define ROOM_NOBUY              ((long long int) 1 << 39)
#define ROOM_ZECCA              ((long long int) 1 << 40)
#define ROOM_TRADE              ((long long int) 1 << 41)
#define ROOM_MAGAZZINO          ((long long int) 1 << 42)
#define ROOM_EDIFICABILE        ((long long int) 1 << 43)
#define ROOM_TEMPIO			  ((long long int) 1 << 44)
#define ROOM_NOWRAITH		  ((long long int) 1 << 45)
//PEPPE DAMAGE
#define ROOM_DAMAGE_FUOCO        ((long long int) 1 << 46)
#define ROOM_DAMAGE_ACQUA        ((long long int) 1 << 47)
#define ROOM_DAMAGE_ARIA         ((long long int) 1 << 48)
#define ROOM_DAMAGE_TERRA        ((long long int) 1 << 49)
#define ROOM_BIG_HOUSE           ((long long int) 1 << 50) // room da 500 obj



/* Exit info: used in room_data.dir_option.exit_info */
#define EX_ISDOOR		(1 << 0)   /* Exit is a door		*/
#define EX_CLOSED		(1 << 1)   /* The door is closed	*/
#define EX_LOCKED		(1 << 2)   /* The door is locked	*/
#define EX_PICKPROOF		(1 << 3)   /* Lock can't be picked	*/
#define EX_HIDDEN               (1 << 4)   /* exit is hidden */

/* Sector types: used in room_data.sector_type */
#define SECT_INSIDE          0		   /* Indoors			*/
#define SECT_CITY            1		   /* In a city			*/
#define SECT_FIELD           2		   /* In a field		*/
#define SECT_FOREST          3		   /* In a forest		*/
#define SECT_HILLS           4		   /* In the hills		*/
#define SECT_MOUNTAIN        5		   /* On a mountain		*/
#define SECT_WATER_SWIM      6		   /* Swimmable water		*/
#define SECT_WATER_NOSWIM    7		   /* Water - need a boat	*/
#define SECT_UNDERWATER	     8		   /* Underwater		*/
#define SECT_FLYING	     9		   /* Wheee!			*/
#define SECT_ROAD	    10		   /* The room is a road.	*/


/* char and mob-related defines *****************************************/


/* PC classes */
#define CLASS_UNDEFINED	  -1
#define CLASS_MAGIC_USER  0
#define CLASS_CLERIC      1
#define CLASS_THIEF       2
#define CLASS_WARRIOR     3
#define CLASS_PELOI       4
#define CLASS_DARESIANO   5
#define NUM_CLASSES	  6  /* This must be the number of classes!! */

/* NPC classes (currently unused - feel free to implement!) */
#define CLASS_OTHER       0
#define CLASS_UNDEAD      1
#define CLASS_HUMANOID    2
#define CLASS_ANIMAL      3
#define CLASS_DRAGON      4
#define CLASS_GIANT       5


/* Sex */
#define SEX_NEUTRAL   0
#define SEX_MALE      1
#define SEX_FEMALE    2


/* Positions */
#define POS_DEAD	0	/* dead			*/
#define POS_MORTALLYW	1	/* mortally wounded	*/
#define POS_INCAP	2	/* incapacitated	*/
#define POS_STUNNED	3	/* stunned		*/
#define POS_SLEEPING	4	/* sleeping		*/
#define POS_RESTING	5	/* resting		*/
#define POS_SITTING	6	/* sitting		*/
#define POS_FIGHTING	7	/* fighting		*/
#define POS_STANDING	8	/* standing		*/

#define HIT_INCAP	-3
#define HIT_MORTALLYW	-6
#define HIT_DEAD	-11

/*Nuovi flag criminali*/
#define CLAN_THIEF    0
#define CLAN_KILLER   1
#define CLAN_TKILLER  2
#define CLAN_WKILLER  3
#define CLAN_WANTED   4
#define CLAN_STRAGE   5
#define CLAN_CORROTTO 6
#define CLAN_MULTA    7


/* Player flags: used by char_data.char_specials.act */
#define PLR_KILLER	(1 << 0)   /* Player is a player-killer		*/
#define PLR_THIEF	(1 << 1)   /* Player is a player-thief		*/
#define PLR_FROZEN	(1 << 2)   /* Player is frozen			*/
#define PLR_DONTSET     (1 << 3)   /* Don't EVER set (ISNPC bit)	*/
#define PLR_WRITING	(1 << 4)   /* Player writing (board/mail/olc)	*/
#define PLR_MAILING	(1 << 5)   /* Player is writing mail		*/
#define PLR_CRASH	(1 << 6)   /* Player needs to be crash-saved	*/
#define PLR_SITEOK	(1 << 7)   /* Player has been site-cleared	*/
#define PLR_NOSHOUT	(1 << 8)   /* Player not allowed to shout/goss	*/
#define PLR_NOTITLE	(1 << 9)   /* Player not allowed to set title	*/
#define PLR_DELETED	(1 << 10)  /* Player deleted - space reusable	*/
#define PLR_LOADROOM	(1 << 11)  /* Player uses nonstandard loadroom	*/
#define PLR_NOWIZLIST	(1 << 12)  /* Player shouldn't be on wizlist	*/
#define PLR_NODELETE	(1 << 13)  /* Player shouldn't be deleted	*/
#define PLR_INVSTART	(1 << 14)  /* Player should enter game wizinvis	*/
#define PLR_CRYO	(1 << 15)  /* Player is cryo-sa*/
#define PLR_QUESTOR     (1 << 16)
#define PLR_TKILLER     (1 << 17)   /* Player has attacked a player      */
#define PLR_WAR_KILLER  (1 << 18)   /* Player has attacked in war rooms  */
#define PLR_WANTED      (1 << 19)   /* WANTED *dead or alive */
#define PLR_MUTO        (1 << 20)   /*Player muto come un pesce          */
#define PLR_NOTAIO      (1 << 21)   /*Pg che puo' dare inizio alle costruzioni(Architetto)*/
#define PLR_MULTIPLAYER (1 << 22)   /*Pg a cui e' permesso il multiplayer*/
#define PLR_TRUSTED     (1 << 23)   /* Pg a cui sono dati momentaneamente i poteri di QUESTMASTER */
//PEPPE RELIGIONE
#define PLR_FANTASMA    (1 << 24)   /*pg che e' momentaneamente sotto forma di spirito */
#define PLR_MODIFICATO  (1 << 25)   /*il pg e' gia stato modificato secondo le ultime innovazioni*/
#define PLR_CONCENTRATO (1 << 26)   /*il pg si sta concentrando*/
#define PLR_ALL_OLC	(1 << 27)   /*il pg (qm o Dio) puo' editare tutto il mondo */
#define PLR_CATARSI     (1 << 28)   /* il pg e' in catarsi */
#define PLR_GO_GILDA	(1 << 29)   /* il pg deve andare in gilda per farsi dare il title */
#define PLR_PAPA	(1 << 30)   /* il pg e' papa della sua religione */

/* Mobile flags: used by char_data.char_specials.act */
#define MOB_SPEC         (1 << 0)  /* Mob has a callable spec-proc	*/
#define MOB_SENTINEL     (1 << 1)  /* Mob should not move		*/
#define MOB_SCAVENGER    (1 << 2)  /* Mob picks up stuff on the ground	*/
#define MOB_ISNPC        (1 << 3)  /* (R) Automatically set on all Mobs	*/
#define MOB_AWARE	 (1 << 4)  /* Mob can't be backstabbed		*/
#define MOB_AGGRESSIVE   (1 << 5)  /* Mob hits players in the room	*/
#define MOB_STAY_ZONE    (1 << 6)  /* Mob shouldn't wander out of zone	*/
#define MOB_WIMPY        (1 << 7)  /* Mob flees if severely injured	*/
#define MOB_AGGR_EVIL	 (1 << 8)  /* auto attack evil PC's		*/
#define MOB_AGGR_GOOD	 (1 << 9)  /* auto attack good PC's		*/
#define MOB_AGGR_NEUTRAL (1 << 10) /* auto attack neutral PC's		*/
#define MOB_MEMORY	 (1 << 11) /* remember attackers if attacked	*/
#define MOB_HELPER	 (1 << 12) /* attack PCs fighting other NPCs	*/
#define MOB_NOCHARM	 (1 << 13) /* Mob can't be charmed		*/
#define MOB_NOSUMMON	 (1 << 14) /* Mob can't be summoned		*/
#define MOB_NOSLEEP	 (1 << 15) /* Mob can't be slept		*/
#define MOB_NOBASH	 (1 << 16) /* Mob can't be bashed (e.g. trees)	*/
#define MOB_NOBLIND	 (1 << 17) /* Mob can't be blinded		*/
#define MOB_WILDHUNT     (1 << 18) /* Mob va a caccia nella wilderness  */
#define MOB_QUEST        (1 << 19)
#define MOB_QUESTMASTER  (1 << 20)
#define MOB_ASSASSIN     (1 << 21)
#define MOB_HUNTER       (1 << 22)
#define MOB_MOUNTABLE    (1 << 23) /* Mob puo' essere cavalcato         */
#define MOB_HARD_TAME    (1 << 24) /* Drago */
#define MOB_DRAGONBREATH (1 << 25) /* Puo' soffiare */
#define MOB_SAVEINV      (1 << 26) /* salva l'inventerio e l'eq */
#define MOB_SHOPKEEPER   (1 << 27) /* Negoziante */
#define MOB_BOUNTY       (1 << 28) /*Bounty killer*/
#define MOB_NOKILL       (1 << 29) /*NOkill as PLayer NOkill*/
#define MOB_CIVILIAN     (1 << 30) /*Mob not to kill        */
#define MOB_CRIMINAL     ((long long int) 1 << 31)                /*Mob criminali          */
#define MOB_SEARCHER     ((long long int) 1 << 32) /* Cercatore */
#define MOB_SAVE         ((long long int) 1 << 33) /* Flag di salvataggio del mob */
#define MOB_GO_FAST      ((long long int) 1 << 34) /* Il mob si muove con il go alla velocita' doppia */
#define MOB_SELVAGGINA   ((long long int) 1 << 35)
#define MOB_RIS_CARNE    ((long long int) 1 << 36)
#define MOB_RIS_PELLE    ((long long int) 1 << 37)
#define MOB_RIS_STOFFA   ((long long int) 1 << 38)
#define MOB_RIS_PELLICCIA ((long long int) 1 << 39)
#define MOB_RIS_SCAGLIE  ((long long int) 1 << 40)
#define MOB_BGUARD       ((long long int) 1 << 41)/*Mob guardie del corpo*/
#define MOB_NECRO        ((long long int) 1 << 42)/*Mob spogliano i cadaveri*/
#define MOB_AGGR_NO_PROP ((long long int) 1 << 43)  /* SE AGGR, Attacca tutti tranne il padrone*/
#define MOB_AGGR_NO_AL_CLAN ((long long int) 1 << 44)  /*SE AGGR, Accacca tutti tranne i membri dei clan alleati*/
#define MOB_AGGR_NO_EN_CLAN ((long long int) 1 << 45) /*SE AGGR, Attacca tutti tranne i membri dei clan in guerra*/
#define MOB_AGGR_NO_PC_CLAN ((long long int) 1 << 46)  /*SE AGGR, Accacca tutti tranne i membri dei clan in pace*/
#define MOB_AGGR_NO_CLAN ((long long int) 1 << 47)  /*SE AGGR, Accacca tutti tranne i membri del suo clan*/
#define MOB_AGGR_VAS_CLAN ((long long int) 1 << 48)  /*SE AGGR, Accacca tutti tranne i membri dei clan sottomessi*/
#define MOB_AMMAESTRABILE ((long long int) 1 << 49)  /* mob addestrabile*/
#define MOB_WORKER        ((long long int) 1 << 50)  /* mob lavoratore*/
#define MOB_CRIMINALHELPER   ((long long int) 1 << 51)  /* mob lavoratore*/
#define MOB_CRASH         ((long long int) 1 << 52) /*Il mob deve essere salvato*/
#define MOB_SAVE_DELETE   ((long long int) 1 << 53) /*Il mob e' cancellato*/
#define MOB_CRASH_EQ      ((long long int) 1 << 54) /*Il mob deve salvare l'EQ*/
#define MOB_NOEXP	 ((long long int) 1 << 55) /*  Se il mob Folla non si prendono XP */
#define MOB_STIPENDIO	 ((long long int) 1 << 56) // Il mob viene pagato
#define MOB_STIPENDIO_EQ ((long long int) 1 << 57) // Il mob riceve una percentuale dall'eq che porta
#define MOB_ESERCITO     ((long long int) 1 << 58) // e' un esercito  
#define MOB_NODECAPITA ((long long int) 1 << 59)   // il mob non si decapita                                         


/* Preference flags: used by char_data.player_specials.pref */
#define PRF_BRIEF       (1 << 0)  /* Room descs won't normally be shown	*/
#define PRF_COMPACT     (1 << 1)  /* No extra CRLF pair before prompts	*/
#define PRF_DEAF	      (1 << 2)  /* Can't hear shouts			*/
#define PRF_NOTELL	    (1 << 3)  /* Can't receive tells		*/
#define PRF_DISPHP	    (1 << 4)  /* Display hit points in prompt	*/
#define PRF_DISPMANA	  (1 << 5)  /* Display mana points in prompt	*/
#define PRF_DISPMOVE	  (1 << 6)  /* Display move points in prompt	*/
#define PRF_AUTOEXIT	  (1 << 7)  /* Display exits in a room		*/
#define PRF_NOHASSLE	  (1 << 8)  /* Aggr mobs won't attack		*/
#define PRF_QUEST	      (1 << 9)  /* On quest				*/
#define PRF_SUMMONABLE	(1 << 10) /* Can be summoned			*/
#define PRF_NOREPEAT	  (1 << 11) /* No repetition of comm commands	*/
#define PRF_HOLYLIGHT	  (1 << 12) /* Can see in dark			*/
#define PRF_COLOR_1	    (1 << 13) /* Color (low bit)			*/
#define PRF_COLOR_2	    (1 << 14) /* Color (high bit)			*/
#define PRF_NOWIZ	      (1 << 15) /* Can't hear wizline			*/
#define PRF_LOG1	      (1 << 16) /* On-line System Log (low bit)	*/
#define PRF_LOG2	      (1 << 17) /* On-line System Log (high bit)	*/
#define PRF_NOAUCT	    (1 << 18) /* Can't hear auction channel		*/
#define PRF_NOGOSS	    (1 << 19) /* Can't hear gossip channel		*/
#define PRF_NOGRATZ	    (1 << 20) /* Can't hear grats channel		*/
#define PRF_ROOMFLAGS	  (1 << 21) /* Can see room flags (ROOM_x)	*/
#define PRF_AFK         (1 << 22) /*                                    */
#define PRF_DISPGOLD    (1 << 23)
#define PRF_DISPXP      (1 << 24)
#define PRF_DISPDAM     (1 << 25)
#define PRF_AUTODIR     (1 << 26)
#define PRF_ARENA       (1 << 27)
#define PRF_SOUNDS	    (1 << 28) /* Il player usa l'MSP come sonoro    */
#define PRF_HIDE_SNEAK  (1 << 29) /* Char can see sneak people          */
#define PRF_NO_EROE     (1 << 30) /* Non compare nella lista degli eroi */
#define PRF_DISPCAV     ((long long int) 1 << 31) /* Display lo stato del cavallo */
#define PRF_RPG         ((long long int) 1 << 32) /* Flag RPG */
#define PRF_NOKILL      ((long long int) 1 << 33) /* W il pacifisti*/
#define PRF_REGNOTELL   ((long long int) 1 << 34) /* Canale Regno attivo */
#define PRF_NOC		      ((long long int) 1 << 35) /* Elimina il canale OOC OutOfCharter*/
#define PRF_DISPSTATIC  ((long long int) 1 << 36) // Static Prompt

/* Affect bits: used in char_data.char_specials.saved.affected_by */
     /* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved") */
/* By Spini: per permettere di avere non 64 ma 256 affect, ad ogni affect e'
   associato un'altra costante con nome uguale ma con premessa BITV_. Ogni nuova
   affect deve essere munita di un file. Quando un file viene riempito nelle 64
   posizioni (dopo il bit 1 << 63) si reinizia da 1 << 0 ma BITV_ aumenta di uno.
   Se non bastassero 4 file si possono sempre aumentare, ma non lo consiglio a
   nessun coder, ci sto diventando matto!
   IMPORTANTE (anzi, direi, _FONDAMENTALE_)! Per tutte le affect che non abbiano
   come file BITV_ 0 e' indispensabile utilizzare AFFECTED(ch,flag,file) invece
   di AFF_FLAGGED(ch,flag), per quelli che si sanno essere del primo file e'
   opzionale. Ad esempio, AFFECTED(ch, AFF_BLIND, BITV_BLIND).                   */
#define AFF_BLIND             (1 << 0)	   /* (R) Char is blind		*/
#define AFF_INVISIBLE         (1 << 1)	   /* Char is invisible		*/
#define AFF_DETECT_ALIGN      (1 << 2)	   /* Char is sensitive to align*/
#define AFF_DETECT_INVIS      (1 << 3)	   /* Char can see invis chars  */
#define AFF_DETECT_MAGIC      (1 << 4)	   /* Char is sensitive to magic*/
#define AFF_SENSE_LIFE        (1 << 5)	   /* Char can sense hidden life*/
#define AFF_WATERWALK	      (1 << 6)	   /* Char can walk on water	*/
#define AFF_SANCTUARY         (1 << 7)	   /* Char protected by sanct.	*/
#define AFF_GROUP             (1 << 8)	   /* (R) Char is grouped	*/
#define AFF_CURSE             (1 << 9)	   /* Char is cursed		*/
#define AFF_INFRAVISION       (1 << 10)	   /* Char can see in dark	*/
#define AFF_POISON            (1 << 11)	   /* (R) Char is poisoned	*/
#define AFF_PROTECT_EVIL      (1 << 12)	   /* Char protected from evil  */
#define AFF_PROTECT_GOOD      (1 << 13)	   /* Char protected from good  */
#define AFF_SLEEP             (1 << 14)	   /* (R) Char magically asleep	*/
#define AFF_NOTRACK	      (1 << 15)	   /* Char can't be tracked	*/
#define AFF_MARK	      (1 << 16)	   /*                   	*/
#define AFF_PASSDOOR	      (1 << 17)	   /* Translucido	        */
#define AFF_SNEAK             (1 << 18)	   /* Char can move quietly	*/
#define AFF_HIDE              (1 << 19)	   /* Char is hidden		*/
#define AFF_FIRESHD	      (1 << 20)	   /* Room for future expansion	*/
#define AFF_CHARM             (1 << 21)	   /* Char is charmed		*/
#define AFF_FLYING            (1 << 22)    /* Char is Flying            */
#define AFF_WATERBREATH       (1 << 23)    /* Char can breath water     */
#define AFF_PROT_FIRE	      (1 << 24)    /* Char protected from fire  */
#define AFF_SHIELD	      (1 << 25)    /*CHar cast shield           */
#define AFF_DEATHDANCE	      (1 << 26)	   /* Char is Death Dancing	*/
#define AFF_MIRRORIMAGE       (1 << 27)    /* Char affected by mirImg   */
#define AFF_BLINK             (1 << 28)    /* Char is displaced         */
#define AFF_WIZEYE            (1 << 29)    /* Char is covered with acid */
#define AFF_PARALIZE          (1 << 30)    /* Char is on fire           */
#define AFF_TAMED             ((long long int) 1 << 31)    /* Char e' stato domato      */
#define AFF_ACCAMPATO         ((long long int) 1 << 32)
#define AFF_TRAMORTITO        ((long long int) 1 << 33)
#define AFF_IMMOBIL           ((long long int) 1 << 34)
#define AFF_CAPTURING         ((long long int) 1 << 35)
#define AFF_TRANSPORTED       ((long long int) 1 << 36)
/*Nuovi incantesimi Shade*/

#define AFF_PROTECT_LIGHT     ((long long int)  1 << 37)
#define AFF_DET_SNEAK         ((long long int)  1 << 38)
#define AFF_SILENCE           ((long long int)  1 << 39)
#define AFF_DISGUISE          ((long long int)  1 << 40)
#define AFF_ANTIMAGIC         ((long long int)  1 << 41)
#define AFF_AID               ((long long int)  1 << 42)
#define AFF_CONGIUNZIONE      ((long long int)  1 << 43)
//PEPPE CRITICAL
#define AFF_CRITICAL_TESTA_1 ((long long int) 1 << 44)
#define AFF_CRITICAL_BRACCIO_D_1 ((long long int) 1 << 45)
#define AFF_CRITICAL_BRACCIO_S_1 ((long long int) 1 << 46)
#define AFF_CRITICAL_TORSO_1 ((long long int) 1 << 47)
#define AFF_CRITICAL_GAMBA_D_1 ((long long int) 1 << 48)
#define AFF_CRITICAL_GAMBA_S_1 ((long long int) 1 << 49)
#define AFF_CRITICAL_TESTA_2 ((long long int) 1 << 50)
#define AFF_CRITICAL_BRACCIO_D_2 ((long long int) 1 << 51)
#define AFF_CRITICAL_BRACCIO_S_2 ((long long int) 1 << 52)
#define AFF_CRITICAL_TORSO_2 ((long long int) 1 << 53)
#define AFF_CRITICAL_GAMBA_D_2 ((long long int) 1 << 54)
#define AFF_CRITICAL_GAMBA_S_2 ((long long int) 1 << 55)
#define AFF_CRITICAL_TESTA_3 ((long long int) 1 << 56)
#define AFF_CRITICAL_BRACCIO_D_3 ((long long int) 1 << 57)
#define AFF_CRITICAL_BRACCIO_S_3 ((long long int) 1 << 58)
#define AFF_CRITICAL_TORSO_3 ((long long int) 1 << 59)
#define AFF_CRITICAL_GAMBA_D_3 ((long long int) 1 << 60)
#define AFF_CRITICAL_GAMBA_S_3 ((long long int) 1 << 61)
#define AFF_FERITO             ((long long int) 1 << 62)
#define AFF_ENDURANCE          ((long long int) 1 << 63) 
#define AFF_BURNED	       (1 << 0)
#define AFF_CHILLED	       (1 << 1)
#define AFF_LEVITATE	       (1 << 2)
#define AFF_RAFFREDDATO	       (1 << 3)
#define AFF_APPESTATO	       (1 << 4)
#define AFF_UBRIACO	       (1 << 5)
#define AFF_MALEDETTO_SHAARR   (1 << 6)
#define AFF_PIAGATO	       (1 << 7)
#define AFF_MALEDETTO_XHYPHYS  (1 << 8)
#define AFF_ADDORMENTATO       (1 << 9)
#define AFF_VUOTO_2	       (1 << 0)
#define AFF_VUOTO_3            (1 << 0)


/* Ad ogni affect si associa un numero da 0 a 3 che indica
   il bitvector in cui sara' contenuta l'affect stessa.
   By Spini						   */

#define BITV_BLIND             0
#define BITV_INVISIBLE         0
#define BITV_DETECT_ALIGN      0
#define BITV_DETECT_INVIS      0
#define BITV_DETECT_MAGIC      0
#define BITV_SENSE_LIFE        0
#define BITV_WATERWALK	      0
#define BITV_SANCTUARY         0
#define BITV_GROUP             0
#define BITV_CURSE             0
#define BITV_INFRAVISION       0
#define BITV_POISON            0
#define BITV_PROTECT_EVIL      0
#define BITV_PROTECT_GOOD      0
#define BITV_SLEEP             0
#define BITV_NOTRACK	      0
#define BITV_MARK	      0
#define BITV_PASSDOOR	      0
#define BITV_SNEAK             0
#define BITV_HIDE              0
#define BITV_FIRESHD	      0
#define BITV_CHARM             0
#define BITV_FLYING            0
#define BITV_WATERBREATH       0
#define BITV_PROT_FIRE	      0
#define BITV_SHIELD	      0
#define BITV_DEATHDANCE	      0
#define BITV_MIRRORIMAGE       0
#define BITV_BLINK             0
#define BITV_WIZEYE            0
#define BITV_PARALIZE          0
#define BITV_TAMED             0
#define BITV_ACCAMPATO         0
#define BITV_TRAMORTITO        0
#define BITV_IMMOBIL           0
#define BITV_CAPTURING         0
#define BITV_TRANSPORTED       0
/*Nuovi incantesimi Shade*/

#define BITV_PROTECT_LIGHT     0
#define BITV_DET_SNEAK         0
#define BITV_SILENCE           0
#define BITV_DISGUISE          0
#define BITV_ANTIMAGIC         0
#define BITV_AID               0
#define BITV_CONGIUNZIONE     0
//PEPPE CRITICAL
#define BITV_CRITICAL_TESTA_1 0
#define BITV_CRITICAL_BRACCIO_D_1 0
#define BITV_CRITICAL_BRACCIO_S_1 0
#define BITV_CRITICAL_TORSO_1 0
#define BITV_CRITICAL_GAMBA_D_1 0
#define BITV_CRITICAL_GAMBA_S_1 0
#define BITV_CRITICAL_TESTA_2 0
#define BITV_CRITICAL_BRACCIO_D_2 0
#define BITV_CRITICAL_BRACCIO_S_2 0
#define BITV_CRITICAL_TORSO_2 0
#define BITV_CRITICAL_GAMBA_D_2 0
#define BITV_CRITICAL_GAMBA_S_2 0
#define BITV_CRITICAL_TESTA_3 0
#define BITV_CRITICAL_BRACCIO_D_3 0
#define BITV_CRITICAL_BRACCIO_S_3 0
#define BITV_CRITICAL_TORSO_3 0
#define BITV_CRITICAL_GAMBA_D_3 0
#define BITV_CRITICAL_GAMBA_S_3 0
#define BITV_FERITO             0
#define BITV_ENDURANCE          0

#define BITV_BURNED	       1
#define BITV_CHILLED	       1
#define BITV_LEVITATE	       1
#define BITV_RAFFREDDATO       1
#define BITV_APPESTATO	       1
#define BITV_UBRIACO	       1
#define BITV_MALEDETTO_SHAARR  1
#define BITV_PIAGATO	       1
#define BITV_MALEDETTO_XHYPHYS 1
#define BITV_ADDORMENTATO      1
#define BITV_VUOTO_2	       2
#define BITV_VUOTO_3           3


/* Modes of connectedness: used by descriptor_data.state */
#define CON_PLAYING	 0		/* Playing - Nominal state	*/
#define CON_CLOSE	 1		/* Disconnecting		*/
#define CON_GET_NAME	 2		/* By what name ..?		*/
#define CON_NAME_CNFRM	 3		/* Did I get that right, x?	*/
#define CON_PASSWORD	 4		/* Password:			*/
#define CON_NEWPASSWD	 5		/* Give me a password for x	*/
#define CON_CNFPASSWD	 6		/* Please retype password:	*/
#define CON_QSEX	 7		/* Sex?				*/
#define CON_QCLASS	 8		/* Class?			*/
#define CON_RMOTD	 9		/* PRESS RETURN after MOTD	*/
#define CON_MENU	 10		/* Your choice: (main menu)	*/
#define CON_EXDESC	 11		/* Enter a new description:	*/
#define CON_CHPWD_GETOLD 12		/* Changing passwd: get old	*/
#define CON_CHPWD_GETNEW 13		/* Changing passwd: get new	*/
#define CON_CHPWD_VRFY   14		/* Verify new password		*/
#define CON_DELCNF1	 15		/* Delete confirmation 1	*/
#define CON_DELCNF2	 16		/* Delete confirmation 2	*/
#define CON_OEDIT	 17		/*. OLC mode - object edit     .*/
#define CON_REDIT	 18		/*. OLC mode - room edit       .*/
#define CON_ZEDIT	 19		/*. OLC mode - zone info edit  .*/
#define CON_MEDIT	 20		/*. OLC mode - mobile edit     .*/
#define CON_SEDIT	 21		/*. OLC mode - shop edit       .*/
#define CON_QACCETTAROLL 22             /* Accetta punti rollaggio      */
#define CON_QCITTA       23
#define CON_TEXTED	 24
#define CON_QMESTIERI    25             /* Scelta mestieri              */
#define CON_TRIGEDIT	 26		/*. OLC mode - trigger edit    .*/
#define CON_WEDIT	 27		/*. OLC mode - wild  edit      .*/
#define CON_CONF_INIZIAZ 28
#define CON_AGPT_INIZIAZ 29
#define CON_PREMI_TASTO  30
#define CON_SCELTA_ABIL  31
//PEPPE RELIGIONE
#define CON_SCELTA_CULTO 32


/* Character equipment positions: used as index for char_data.equipment[] */
/* NOTE: Don't confuse these constants with the ITEM_ bitvectors
   which control the valid places you can wear a piece of equipment */
#define WEAR_LIGHT      0
#define WEAR_FINGER_R   1
#define WEAR_FINGER_L   2
#define WEAR_NECK_1     3
#define WEAR_NECK_2     4
#define WEAR_BODY       5
#define WEAR_HEAD       6
#define WEAR_LEGS       7
#define WEAR_FEET       8
#define WEAR_HANDS      9
#define WEAR_ARMS      10
#define WEAR_SHIELD    11
#define WEAR_ABOUT     12
#define WEAR_WAIST     13
#define WEAR_WRIST_R   14
#define WEAR_WRIST_L   15
#define WEAR_WIELD     16
#define WEAR_HOLD      17
#define WEAR_LOBSX     18
#define WEAR_LOBDX     19
#define WEAR_SPALLE    20
#define WEAR_IMMOBIL   21
#define WEAR_EYE       22
#define WEAR_BOCCA     23
#define WEAR_ALTRO1    24
#define WEAR_WIELD_L   25
#define WEAR_HANG      26
#define WEAR_VESTE     27
#define WEAR_RELIQUIA  28
#define NUM_WEARS      29	/* This must be the # of eq positions!! */


/* object-related defines ********************************************/


/* Item types: used by obj_data.obj_flags.type_flag */
#define ITEM_LIGHT      1		/* Item is a light source	*/
#define ITEM_SCROLL     2		/* Item is a scroll		*/
#define ITEM_WAND       3		/* Item is a wand		*/
#define ITEM_STAFF      4		/* Item is a staff		*/
#define ITEM_WEAPON     5		/* Item is a weapon		*/
#define ITEM_FIREWEAPON 6		/* Unimplemented		*/
#define ITEM_MISSILE    7		/* Unimplemented		*/
#define ITEM_TREASURE   8		/* Item is a treasure, not gold	*/
#define ITEM_ARMOR      9		/* Item is armor		*/
#define ITEM_POTION    10 		/* Item is a potion		*/
#define ITEM_WORN      11		/* Unimplemented		*/
#define ITEM_OTHER     12		/* Misc object			*/
#define ITEM_TRASH     13		/* Trash - shopkeeps won't buy	*/
#define ITEM_TRAP      14		/* Unimplemented		*/
#define ITEM_CONTAINER 15		/* Item is a container		*/
#define ITEM_NOTE      16		/* Item is note 		*/
#define ITEM_DRINKCON  17		/* Item is a drink container	*/
#define ITEM_KEY       18		/* Item is a key		*/
#define ITEM_FOOD      19		/* Item is food			*/
#define ITEM_MONEY     20		/* Item is money (gold)		*/
#define ITEM_PEN       21		/* Item is a pen		*/
#define ITEM_BOAT      22		/* Item is a boat		*/
#define ITEM_FOUNTAIN  23		/* Item is a fountain		*/
#define ITEM_FLIGHT    24
#define ITEM_PORTAL    25
#define ITEM_THROW     26               /* Item can be thrown as weapon */
#define ITEM_GRENADE   27               /* Item is a grenade      */
#define ITEM_BOW       28               /* shoots arrows */
#define ITEM_SLING     29               /* shoots rocks */
#define ITEM_CROSSBOW  30               /* shoots bolts */
#define ITEM_BOLT      31
#define ITEM_ARROW     32
#define ITEM_ROCK      33
#define ITEM_RENT_MOUNT  34             /* Ricevuta dello stalliere */
#define ITEM_KEY_RENT    35             /* Chiave che puo' essere rentata */
#define ITEM_MOUNT_FOOD  36             /* Cibo per i cavalli           */
#define ITEM_MARTELLO    37
#define ITEM_SEGA        38
#define ITEM_PICCA       39
#define ITEM_SCUOIATORE  40
#define ITEM_FALCE       41
#define ITEM_ASCIA       42
#define ITEM_MARTELLETTO 43
#define ITEM_PIETRA      44
#define ITEM_ALBERO      45
#define ITEM_MINERALE    46
#define ITEM_NATURALE    47
#define ITEM_MET_GREZZO  48
#define ITEM_TRONCO      49
#define ITEM_PIETRA_GREZ 50
#define ITEM_PELLE_GREZ  51
#define ITEM_PRODOTTO    52
#define ITEM_LINGOTTO    53
#define ITEM_TRAVE_LEGNO 54
#define ITEM_PIETRA_PULI 55
#define ITEM_PELLE_PULI  56
#define ITEM_DA_CUOCERE  57
#define ITEM_TRAVE_COST  58
#define ITEM_ROCCIA_SQUA 59
#define ITEM_MART_COSTR  60
#define ITEM_ASSE_LEGNO  61
#define ITEM_ROCCIA      62
#define ITEM_ROCCIA_GREZ 63
#define ITEM_GEMMA       64
#define ITEM_RENT_DRAGON 65
#define ITEM_ERBA        66
#define ITEM_IMMOBIL     67
#define ITEM_MANGANELLO  68
#define ITEM_BENDA       69
#define ITEM_PICCOZZA    70
#define ITEM_ERBA_GREZ   71
#define ITEM_ERBA_PULITA 72
#define ITEM_BOCCETTA    73
#define ITEM_TRITAERBE   74
#define ITEM_BAVAGLIO    75
#define ITEM_CANNA       76
#define ITEM_STRUM_CUCINA 77
#define ITEM_MATTARELLO  78
#define ITEM_RIPULITURA  79
#define ITEM_PASTELLA    80
#define ITEM_PELLI       81
#define ITEM_CACCIA      82
#define ITEM_STRUM_CONCERIA 83
#define ITEM_MANDATO 84
#define ITEM_PORTA 85
#define ITEM_CONTRATTO   86
#define ITEM_TRAPPOLA    87
#define ITEM_VESTE		88
#define ITEM_RELIQUIA    89
#define ITEM_LIBRO_MAGICO_W 90 /*libri magici per wand*/
#define ITEM_LIBRO_MAGICO_S 91 /*libri magici per staff*/
#define ITEM_WEAPON_2HANDS 92 //Orione
#define ITEM_TISANA_CALDA 93
#define ITEM_TISANA_FREDDA 94
#define ITEM_BENDE_UNTE  95
#define ITEM_POLVERE	 96

/* Take/Wear flags: used by obj_data.obj_flags.wear_flags */
#define ITEM_WEAR_TAKE		(1 << 0)  /* Item can be takes		*/
#define ITEM_WEAR_FINGER	(1 << 1)  /* Can be worn on finger	*/
#define ITEM_WEAR_NECK		(1 << 2)  /* Can be worn around neck 	*/
#define ITEM_WEAR_BODY		(1 << 3)  /* Can be worn on body 	*/
#define ITEM_WEAR_HEAD		(1 << 4)  /* Can be worn on head 	*/
#define ITEM_WEAR_LEGS		(1 << 5)  /* Can be worn on legs	*/
#define ITEM_WEAR_FEET		(1 << 6)  /* Can be worn on feet	*/
#define ITEM_WEAR_HANDS		(1 << 7)  /* Can be worn on hands	*/
#define ITEM_WEAR_ARMS		(1 << 8)  /* Can be worn on arms	*/
#define ITEM_WEAR_SHIELD	(1 << 9)  /* Can be used as a shield	*/
#define ITEM_WEAR_ABOUT		(1 << 10) /* Can be worn about body 	*/
#define ITEM_WEAR_WAIST 	(1 << 11) /* Can be worn around waist 	*/
#define ITEM_WEAR_WRIST		(1 << 12) /* Can be worn on wrist 	*/
#define ITEM_WEAR_WIELD		(1 << 13) /* Can be wielded		*/
#define ITEM_WEAR_HOLD		(1 << 14) /* Can be held		*/
#define ITEM_WEAR_LOBSX         (1 << 15)
#define ITEM_WEAR_SPALLE        (1 << 16)
#define ITEM_WEAR_IMMOBIL       (1 << 17)
#define ITEM_WEAR_EYE           (1 << 18)
#define ITEM_WEAR_BOCCA         (1 << 19)
#define ITEM_WEAR_ALTRO1        (1 << 20)
#define ITEM_WEAR_HANG          (1 << 21) /*carri e affini */
#define ITEM_WEAR_VESTE		  (1 << 22)
#define ITEM_WEAR_RELIQUIA      (1 << 23)


/* Extra object flags: used by obj_data.obj_flags.extra_flags */
#define ITEM_GLOW          (1 << 0)	/* Item is glowing		*/
#define ITEM_HUM           (1 << 1)	/* Item is humming		*/
#define ITEM_NORENT        (1 << 2)	/* Item cannot be rented	*/
#define ITEM_NODONATE      (1 << 3)	/* Item cannot be donated	*/
#define ITEM_NOINVIS	   (1 << 4)	/* Item cannot be made invis	*/
#define ITEM_INVISIBLE     (1 << 5)	/* Item is invisible		*/
#define ITEM_MAGIC         (1 << 6)	/* Item is magical		*/
#define ITEM_NODROP        (1 << 7)	/* Item is cursed: can't drop	*/
#define ITEM_BLESS         (1 << 8)	/* Item is blessed		*/
#define ITEM_ANTI_GOOD     (1 << 9)	/* Not usable by good people	*/
#define ITEM_ANTI_EVIL     (1 << 10)	/* Not usable by evil people	*/
#define ITEM_ANTI_NEUTRAL  (1 << 11)	/* Not usable by neutral people	*/
#define ITEM_ANTI_MAGIC_USER (1 << 12)	/* Not usable by mages		*/
#define ITEM_ANTI_CLERIC   (1 << 13)	/* Not usable by clerics	*/
#define ITEM_ANTI_THIEF	   (1 << 14)	/* Not usable by thieves	*/
#define ITEM_ANTI_WARRIOR  (1 << 15)	/* Not usable by warriors	*/
#define ITEM_NOSELL	   (1 << 16)	/* Shopkeepers won't touch it	*/
#define ITEM_ANTI_PELOI    (1 << 17)    /* anti peloi */
#define ITEM_LIVE_GRENADE  (1 << 18)    /* grenade's pin has been pulled */
#define ITEM_NOLOCATE      (1 << 19)    /* Non e' individuato dallo spell locate object */
#define ITEM_NO_5_LIV       (1 << 20)    /* Almeno apprentice - sesto livello */
#define ITEM_NO_10_LIV       (1 << 21)    /* Almeno acolyte - 11esimo livello */
#define ITEM_NO_20_LIV       (1 << 22)    /* Almeno adept - 21esimo livello */
#define ITEM_NO_25_LIV       (1 << 23)    /* Almeno veteran - 26esimo livello */
#define ITEM_NO_30_LIV       (1 << 24)    /* Almeno knight - 31esimo livello */
#define ITEM_NO_40_LIV       (1 << 25)    /* Almeno great knight - 41esimo livello */
#define ITEM_RESTRING        (1 << 26)    /* L'oggetto ha descrizioni proprie e uniche */
#define ITEM_ISCORPSE        (1 << 27)    /* E' un corpo - cadavere... */
#define ITEM_FORGED          (1 << 28)    /*X le armi:Ogg. Forgiato    */
#define ITEM_AFFILATO        (1 << 29)    /*Flag temporaneo X armi affilate*/
#define ITEM_RINOM_ALIAS     (1 << 30)    /*Flag temporanee X gli obj riominabili*/
#define ITEM_RINOM_NAME      ((long long int) 1 << 31) 
#define ITEM_RINOM_DESCR     ((long long int) 1 << 32) 
#define ITEM_NO_IDENT        ((long long int) 1 << 33)
#define ITEM_QUEST	     ((long long int) 1 << 34)    /* Oggetto obiettivo di Quest */ //Orione

/* Modifier constants used with obj affects ('A' fields) */
#define APPLY_NONE              0	/* No effect			*/
#define APPLY_STR               1	/* Apply to strength		*/
#define APPLY_DEX               2	/* Apply to dexterity		*/
#define APPLY_INT               3	/* Apply to constitution	*/
#define APPLY_WIS               4	/* Apply to wisdom		*/
#define APPLY_CON               5	/* Apply to constitution	*/
#define APPLY_CHA		6	/* Apply to charisma		*/
#define APPLY_CLASS             7	/* Reserved			*/
#define APPLY_LEVEL             8	/* Reserved			*/
#define APPLY_AGE               9	/* Apply to age			*/
#define APPLY_CHAR_WEIGHT      10	/* Apply to weight		*/
#define APPLY_CHAR_HEIGHT      11	/* Apply to height		*/
#define APPLY_MANA             12	/* Apply to max mana		*/
#define APPLY_HIT              13	/* Apply to max hit points	*/
#define APPLY_MOVE             14	/* Apply to max move points	*/
#define APPLY_GOLD             15	/* Reserved			*/
#define APPLY_EXP              16	/* Reserved			*/
#define APPLY_AC               17	/* Apply to Armor Class		*/
#define APPLY_HITROLL          18	/* Apply to hitroll		*/
#define APPLY_DAMROLL          19	/* Apply to damage roll		*/
#define APPLY_SAVING_PARA      20	/* Apply to save throw: paralz	*/
#define APPLY_SAVING_ROD       21	/* Apply to save throw: rods	*/
#define APPLY_SAVING_PETRI     22	/* Apply to save throw: petrif	*/
#define APPLY_SAVING_BREATH    23	/* Apply to save throw: breath	*/
#define APPLY_SAVING_SPELL     24	/* Apply to save throw: spells	*/
#define APPLY_HIT_REGEN        25
#define APPLY_MANA_REGEN       26
#define APPLY_RES_FUOCO	       27	/* Resistenze - inizio		*/
#define APPLY_RES_GHIACCIO     28
#define APPLY_RES_ELETTRICITA  29
#define APPLY_RES_ACIDO        30
#define APPLY_RES_SHAARR       31
#define APPLY_RES_XHYPHYS      32
#define APPLY_RES_THERION      33
#define APPLY_RES_SILUE        34
#define APPLY_RES_FISICO       35	/* Resistenze - fine (by Spini)	*/

/* Container flags - value[1] */
#define CONT_CLOSEABLE      (1 << 0)	/* Container can be closed	*/
#define CONT_PICKPROOF      (1 << 1)	/* Container is pickproof	*/
#define CONT_CLOSED         (1 << 2)	/* Container is closed		*/
#define CONT_LOCKED         (1 << 3)	/* Container is locked		*/


/* Some different kind of liquids for use in values of drink containers */
#define LIQ_WATER      0
#define LIQ_BEER       1
#define LIQ_WINE       2
#define LIQ_ALE        3
#define LIQ_DARKALE    4
#define LIQ_WHISKY     5
#define LIQ_LEMONADE   6
#define LIQ_FIREBRT    7
#define LIQ_LOCALSPC   8
#define LIQ_SLIME      9
#define LIQ_MILK       10
#define LIQ_TEA        11
#define LIQ_COFFE      12
#define LIQ_BLOOD      13
#define LIQ_SALTWATER  14
#define LIQ_CLEARWATER 15
#define LIQ_CAFFELIMONE 16

#define DANNO_FUOCO    0
#define DANNO_GHIACCIO 1
#define DANNO_ELETTRICITA 2
#define DANNO_ACIDO    3
#define DANNO_SHAARR   4
#define DANNO_XHYPHYS  5
#define DANNO_THERION  6
#define DANNO_SILUE    7
#define DANNO_FISICO   8


/* other miscellaneous defines *******************************************/

/* act events*/
#define DRAGON_RECALL_EVENT 0
#define PESCA_EVENT 1
/*Max events*/
#define MAX_ACT_EVENTS 2


/* Player conditions */
#define DRUNK        0
#define FULL         1
#define THIRST       2


/* Sun state for weather */
#define SUN_DARK	0
#define SUN_RISE	1
#define SUN_LIGHT	2
#define SUN_SET		3


/* Sky conditions for weather */
#define SKY_CLOUDLESS	0
#define SKY_CLOUDY	1
#define SKY_RAINING	2
#define SKY_LIGHTNING	3


/* Rent codes */
#define RENT_UNDEF      0
#define RENT_CRASH      1
#define RENT_RENTED     2
#define RENT_CRYO       3
#define RENT_FORCED     4
#define RENT_TIMEDOUT   5
#define RENT_CAMP       6

/* Casi per change_notorieta' */
#define KILL     0
#define TKILL    1
#define WKILL    2
#define THIEF    3
#define WANTED   4
#define QUEST    5
#define CAPTURE  6
#define CAPTURED 7
/* other #defined constants **********************************************/

/*
 * **DO**NOT** blindly change the number of levels in your MUD merely by
 * changing these numbers and without changing the rest of the code to match.
 * Other changes throughout the code are required.  See coding.doc for
 * details.
 */


#define LVL_IMPL		      91
#define LVL_IMPLEMENTOR		90
#define LVL_BUILDER     	85
#define LVL_GOD         	80
#define LVL_QUEST_MASTER	76
#define LVL_MINIBUILD     75
#define LVL_AVATAR		    73
#define LVL_IMMORT		    71


#define LVL_GRGOD	LVL_GOD
#define LVL_FREEZE	LVL_IMMORT

#define NUM_OF_DIRS	6	/* number of directions in a room (nsewud) */

#define OPT_USEC	100000	/* 10 passes per second */
#define PASSES_PER_SEC	(1000000 / OPT_USEC)
#define RL_SEC		* PASSES_PER_SEC

#define PULSE_ZONE      (10 RL_SEC)
#define PULSE_MOBILE    (10 RL_SEC)
#define PULSE_VIOLENCE  (3 RL_SEC)

#define SMALL_BUFSIZE		1024
#define LARGE_BUFSIZE		(12 * 1024)
#define GARBAGE_SPACE		32
#define HISTORY_SIZE		5	/* Keep last 5 commands. */
#define MAX_STRING_LENGTH	8192
#define MAX_INPUT_LENGTH	256	/* Max length per *line* of input */
#define MAX_RAW_INPUT_LENGTH	512	/* Max size of *raw* input */
#define MAX_MESSAGES		60
#define MAX_NAME_LENGTH		20  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_PWD_LENGTH		10  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_TITLE_LENGTH	80  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_POOF_LENGTH         80  /* Used in char_file_u *DO*NOT*CHANGE* */
#define HOST_LENGTH		30  /* Used in char_file_u *DO*NOT*CHANGE* */
#define SHTDESC_LENGTH		50  /* Used in mob_file_u *DO*NOT*CHANGE* */
#define LNGDESC_LENGTH		150  /* Used in mob_file_u *DO*NOT*CHANGE* */
#define EXDSCR_LENGTH		240 /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_TONGUE		3   /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_SKILLS		200 /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_PROFICIENZE         100 /* Usato in char_file_u */
#define MAX_ABILITA             100 /* Usato in char_file_u */
#define MAX_AFFECT		32  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_OBJ_AFFECT		6 /* Used in obj_file_elem *DO*NOT*CHANGE* */
#define TELLS_STORED            10  /* Numero di tell ricevuti da memorizzare */
#define MAX_GBL_VAR_SAVED 128  /* Numero massimo di global var salvabili*/
#define MAX_NAME_VAR_LENGTH 20  /*Lunghezza massima nome variabile */
#define MAX_VAR_LENGTH  150    /*Lunghezza massima variabile globale dei trigger */



/**********************************************************************
* Structures                                                          *
**********************************************************************/


typedef signed char		sbyte;
typedef unsigned char		ubyte;
typedef signed short int	sh_int;
typedef unsigned short int	ush_int;
typedef char			bool;

#ifndef CIRCLE_WINDOWS
typedef char			byte;
#endif

typedef int  room_vnum;
typedef int  obj_vnum;
typedef int  mob_vnum;

typedef int room_rnum;
typedef int obj_rnum;
typedef int mob_rnum;

/* Extra description: used in objects, mobiles, and rooms */
struct extra_descr_data {
	char	*keyword;                 /* Keyword in look/examine          */
	char	*description;             /* What to see                      */
	struct extra_descr_data *next; /* Next in list                     */
};


/* object-related structures ******************************************/


#define NUM_OBJ_VAL_POSITIONS 10	//Orione
/* object flags; used in obj_data */
struct obj_flag_data {
	int	value[NUM_OBJ_VAL_POSITIONS];	/* Values of the item (see list) */	//Orione
	byte type_flag;	/* Type of item			    */
	int	wear_flags;	/* Where you can wear it	    */
	long long int	extra_flags;	/* If it hums, glows, etc.	    */ //Orione
	int	weight;		/* Weigt what else                  */
	int	cost;		/* Value when sold (gp.)            */
	int	cost_per_day;	/* Cost to keep pr. real day        */
	int	timer;		/* Timer for object                 */
	long long int bitvector[4];	/* To set chars bits                */
	int  curr_slots;     // Current amount of SLOTS obj has
	int  total_slots;    // Total amount of SLOTS an obj has
	int  min_level;      /* Livello minimo per equippare */
	int  tipo_materiale;
	int  num_materiale;
};


/* Used in obj_file_elem *DO*NOT*CHANGE* */
struct obj_affected_type {
	byte location;      /* Which ability to change (APPLY_XXX) */
	sbyte modifier;     /* How much it changes by              */
};


/* ================== Memory Structure for Objects ================== */
struct obj_data {
	obj_rnum item_number;	/* Where in data-base -               */
	
	room_rnum in_room;		/* In what room -1 when conta/carr    */
	struct obj_flag_data obj_flags;/* Object information               */
	struct obj_affected_type affected[MAX_OBJ_AFFECT];  /* affects */
	
	char	*name;                    /* Title of object :get etc.        */
	char	*description;		  /* When in room                     */
	char	*short_description;       /* when worn/carry/in cont.         */
	char	*action_description;      /* What to write when used          */
	struct extra_descr_data *ex_description; /* extra descriptions     */
	struct char_data *carried_by;  /* Carried by :NULL in room/conta   */
	struct char_data *worn_by;	  /* Worn by?			      */
	sh_int worn_on;		  /* Worn where?		      */
	
	struct obj_data *in_obj;       /* In what object NULL when none    */
	struct obj_data *contains;     /* Contains objects                 */
	long id;                       /* used by DG triggers              */
	struct trig_proto_list *proto_script; /* list of default triggers  */
	struct script_data *script;    /* script info for the object       */
	
	
	struct obj_data *next_content; /* For 'contains' lists             */
	struct obj_data *next;         /* For the object list              */
};
/* ======================================================================= */


/* ====================== File Element for Objects ======================= */
/*                 BEWARE: Changing it will ruin rent files		   */
struct obj_file_elem {
	obj_vnum item_number;
	sh_int locate;  /* that's the (1+)wear-location (when equipped) or
		      (20+)index in obj file (if it's in a container) BK */
	int	value[NUM_OBJ_VAL_POSITIONS];	//Orione	
	byte  type_flag;
	int	extra_flags;
	int  wear_flags;
	int	weight;
	int	cost;
	int	cost_per_day;
	int	timer;
	long long int bitvector[4];
	struct obj_affected_type affected[MAX_OBJ_AFFECT];
	int  curr_slots;     // Current amount of SLOTS obj has
	int  total_slots;    // Total amount of SLOTS an obj has
	int  min_level;
	int  tipo_materiale;
	int  num_materiale;
};


/* header block for rent files.  BEWARE: Changing it will ruin rent files  */
struct rent_info {
	int	time;
	int	rentcode;
	int	net_cost_per_diem;
	int	gold;
	int	account;
	int	nitems;
	int	spare0;
	int	spare1;
	int	spare2;
	int	spare3;
	int	spare4;
	int	spare5;
	int	spare6;
	int	spare7;
};
/* ======================================================================= */


/* room-related structures ************************************************/


struct room_direction_data {
	char	*general_description;       /* When look DIR.			*/
	
	char	*keyword;		/* for open/close			*/
	
	sh_int exit_info;		/* Exit info				*/
	obj_vnum key;		/* Key's number (-1 for no key)		*/
	room_rnum to_room;		/* Where direction leads (NOWHERE)	*/
};


/* ================== Memory Structure for room ======================= */
struct room_data {
	room_vnum number;		/* Rooms number	(vnum)		      */
	int zone;                    /* Room zone (for resetting)          */
	int	sector_type;            /* sector type (move/hide)            */
	char	*name;                  /* Rooms name 'You are ...'           */
	char	*description;           /* Shown when entered                 */
	struct extra_descr_data *ex_description; /* for examine/look       */
	struct room_direction_data *dir_option[NUM_OF_DIRS]; /* Directions */
	long long int room_flags;		/* DEATH,DARK ... etc                 */
	
	bool wild_modif;             /* Modificata rispetto allo standard  */
	int  wild_rnum;              /* Tipo di wilderness - wild_table    */
	
	byte light;                  /* Number of lightsources in room     */
	SPECIAL(*func);
	
	struct trig_proto_list *proto_script; /* list of default triggers  */
	struct script_data *script;  /* script info for the object         */
	
	struct obj_data *contents;   /* List of items in room              */
	struct char_data *people;    /* List of NPC / PC in room           */
	
};
/* ====================================================================== */


/* char-related structures ************************************************/


/* memory structure for characters */
struct memory_rec_struct {
	long	id;
	struct memory_rec_struct *next;
};

typedef struct memory_rec_struct memory_rec;


/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data {
	int hours, day, month;
	sh_int year;
};


/* These data contain information about a players time data */
struct time_data {
	time_t birth;    /* This represents the characters age                */
	time_t logon;    /* Time of the last logon (used to calculate played) */
	int	played;     /* This is the total accumulated time played in secs */
};


/* general player-related info, usually PC's and NPC's */
struct char_player_data {
	char	passwd[MAX_PWD_LENGTH+1]; /* character's password      */
	char	*name;	       /* PC / NPC s name (kill ...  )         */
	char	*short_descr;  /* for NPC 'actions'                    */
	char	*long_descr;   /* for 'look'			       */
	char	*description;  /* Extra descriptions                   */
	char	*title;        /* PC / NPC's title                     */
	char *poofin;       /* PC poofin                            */
	char *poofout;      /* PC poofout                           */
	byte sex;           /* PC / NPC's sex                       */
	byte class;         /* PC / NPC's class		       */
	byte level;         /* PC / NPC's level                     */
	int	hometown;      /* PC s Hometown (zone)                 */
	struct time_data time;  /* PC's AGE in days                 */
	ubyte weight;       /* PC / NPC's weight                    */
	ubyte height;       /* PC / NPC's height                    */
	char	*namedisguise; /*nome camuffato*/
	//PEPPEDISGUISE
	char    *long_descr_disguise;/*descrizione camuffata al look dettagliato*/
	char    *l_descr_disguise; /*descrizione camuffata al look normale*/
	int	classdisguise; /*classe camuffata*/
	int	livdisguise; /*liv camuffato*/
	int	sexdisguise; /*sex camuffato*/
	int	clandisguise; /*clan camuffato*/
	int	clanrankdisguise; /*rank camuffato*/
	int	classtitledisguise; /*classtitle camuffato*/
};


/* Char's abilities.  Used in char_file_u *DO*NOT*CHANGE* */
struct char_ability_data {
	sbyte str;
	sbyte str_add;      /* 000 - 100 if strength 18             */
	sbyte intel;
	sbyte wis;
	sbyte dex;
	sbyte con;
	sbyte cha;
};

struct player_fee{
	bool has_fee;
	int clan_fee;
	int idclan;
	int char_fee;
	int idchar;
};
typedef struct player_fee fee;
/* Char's points.  Used in char_file_u *DO*NOT*CHANGE* */
struct char_point_data {
	sh_int mana;
	sh_int max_mana;     /* Max move for PC/NPC			   */
	sh_int hit;
	sh_int max_hit;      /* Max hit for PC/NPC                      */
	sh_int move;
	sh_int max_move;     /* Max move for PC/NPC                     */
	
	sh_int armor;        /* Internal -100..100, external -10..10 AC */
	int	gold;           /* Money carried                           */
	int	bank_gold;	/* Gold the char has in a bank account	   */
	int	exp;            /* The experience of the player            */
	int  fama;           /* La fama del giocatore                   */
	int  notorieta;     /* La notorieta' del giocatore             */
	
	sbyte hitroll;       /* Any bonus or penalty to the hit roll    */
	sbyte damroll;       /* Any bonus or penalty to the damage roll */
};


/*
 * char_special_data_saved: specials which both a PC and an NPC have in
 * common, but which must be saved to the playerfile for PC's.
 *
 * WARNING:  Do not change this structure.  Doing so will ruin the
 * playerfile.  If you want to add to the playerfile, use the spares
 * in player_special_data.
 */
struct char_special_data_saved {
	int	alignment;		 /* +-1000 for alignments                */
	long	idnum;			 /* player's idnum; -1 for mobiles	*/
	long	long int act;		 /* act flag for NPC's; player flag for PC's */
	long long int affected_by[4];    /* Bitvector for spells/skills affected by */
	sh_int apply_saving_throw[5]; /* Saving throw (Bonuses)		*/
};


/* Special playing constants shared by PCs and NPCs which aren't in pfile */
struct char_special_data {
	struct char_data *fighting;	/* Opponent				*/
	struct char_data *hunting;	/* Char hunted by this char		*/
	struct char_data *wildhunt;  /* Char hunted in wilderness            */
	struct char_data *riding;    /* Animale cavalcato da char            */
	struct char_data *ridden_by; /* Chi lo cavalca ?                     */
	struct char_data *transported;  /* chi e' trasportato sulle spalle?(oltre al cavaliere ovviamente)*/
	struct char_data *transported_by;  /* chi lo trasporta sulle spalle?(oltre al cavaliere ovviamente)*/
	byte position;		/* Standing, fighting, sleeping, etc.	*/
	
	int	carry_weight;		/* Carried weight			*/
	byte carry_items;		/* Number of items carried		*/
	int	timer;			/* Timer for update			*/
	sbyte hitgain;
	sbyte managain;
	int   pesca_stat;    /*per il pescatore*/
	struct char_data *assassin;
	time_t last_agguato;
	struct char_data *last_vict;
	struct char_special_data_saved saved; /* constants saved in plrfile	*/
	int mob_abil[5];/*per i maestri erranti:abilita che possono insegnare*/
	char	*groupname;
	struct char_data *whokilled;
	time_t catarsi; //indica quando si e' entrati in catarsi
	ubyte client; /* Identifica dove inviare la mappa wilderness */ //Orione
	short distorto;	/* Indica quando si e' stati spediti nella dimensione parallela... */
	room_rnum distorto_back;	/* Indica la stanza da cui si e' stati spediti nella dimensione parallela */
	room_rnum guardia_sentinel_back;	/* Una guardia sentinel torna indietro in questa casella... */
	long	secondi_senza_dormire;	/* Piu' aumentano piu' e' facile beccarsi la malattia di morfeo */
};


/*
 *  If you want to add new values to the playerfile, do it here.  DO NOT
 * ADD, DELETE OR MOVE ANY OF THE VARIABLES - doing so will change the
 * size of the structure and ruin the playerfile.  However, you can change
 * the names of the spares to something more meaningful, and then use them
 * in your new code.  They will automatically be transferred from the
 * playerfile into memory when players log in.
 */
struct player_special_data_saved {
	byte skills[MAX_SKILLS+1];	/* array of skills plus skill 0		*/
	byte proficienze[MAX_PROFICIENZE+1];     /* array delle proficienze  */
	int prof_choice[2];
	byte abilita[MAX_ABILITA+1]; /* array delle abilita' */
	int abil_choice[3];
	byte PADDING0;		/* used to be spells_to_learn		*/
	bool talks[MAX_TONGUE];	/* PC s Tongues 0 for NPC		*/
	int	wimp_level;		/* Below this # of hit points, flee!	*/
	byte freeze_level;		/* Level of god who froze char, if any	*/
	sh_int invis_level;		/* level of invisibility		*/
	room_vnum load_room;		/* Which room to place char in		*/
	long long pref;              /* preference flags for PC's.		*/
	ubyte bad_pws;		/* number of bad password attemps	*/
	sbyte conditions[3];         /* Drunk, full, thirsty			*/
	fee stipendio;
   /* spares below for future expansion.  You can change the names from
      'sparen' to something meaningful, but don't change the order.  */
	
	ubyte clan_rank;
	ubyte wild_max_x_range;
	ubyte wild_max_y_range;
	ubyte class_title;
	ubyte stato_sociale;
	ubyte new_clan_rank;
	ubyte trust_level;	/* Level before are Trusted */
	ubyte spare5;            /* var: numero del culto al posto di spare 5 PEPPE RELIGIONE*/
	ubyte resuscita;	/* var: numero di ore mancanti per resuscitare al posto di spare 6*/
	ubyte fiato;            /* var: numero di fiato*/
	ubyte TeTconcentrazione; /*simula il tempo e il tipoo di prac per cui si sta concentrando */       
	ubyte spare9;
	ubyte spare10;
	int spells_to_learn;		/* How many can you learn yet this level*/
	int clan;
	int privilegi;              /*privilegi all interno del regno*/
	int olc_zone;
	int gohome;
	int questmrr;	//Orione: Mob-Room-Risultato
	int questobj;
	int questpoints;
	int tipoquest;	//Orione: tipo di quest assegnata
	int countdown;
	int shop;
	int arena_hit;
	int arena_move;
	int arena_mana;
	int arena_room;
	int bet_amt;
	int betted_on;
	int rip_cnt;
	int kill_cnt;
	int dt_cnt;
	int morti_per_livello;
	int abilita_to_learn;
	int num_thief;
	int num_kills;
	int num_tkills;
	int num_warkills;
	int num_wanted;
	int new_clan;
	int new_privilegi;
	int culto;
	struct char_data *questgiver;
	long	as_room;
	long	as_rnum;
	long	new_stipendio_hasfee;  //da sistemare con un tranfert (simula il player_fee per i new_clan)
	long	new_stipendio_clanfee;
	long    numero_unita_armate;    
	long	morte_peste;
	long	morti_da_pg;
	long	xp_giornalieri;
	long	spare27;
	long 	spare28;
	long	spare29;
	long	spare30;
};


/*
 * Specials needed only by PCs, not NPCs.  Space for this structure is
 * not allocated in memory for NPCs, but it is for PCs and the portion
 * of it labelled 'saved' is saved in the playerfile.  This structure can
 * be changed freely; beware, though, that changing the contents of
 * player_special_data_saved will corrupt the playerfile.
 */
struct player_special_data {
	struct player_special_data_saved saved;
	
	struct alias *aliases;	        /* Character's aliases			*/
	long last_tell;		        /* idnum of last tell from		*/
	void *last_olc_targ;	         	/* olc control				*/
	int last_olc_mode;		        /* olc control				*/
	char *previous_tells[TELLS_STORED];  /* Ultimi tell ricevuti  */

	int durata_comb;
};


/* Specials used by NPCs, not PCs */
struct mob_special_data {
	long idmob;              /* Id mob saved*/
	byte last_direction;     /* The last direction the monster went     */
	int	attack_type;        /* The Attack Type Bitvector for NPC's     */
	byte default_pos;        /* Default position for NPC                */
	memory_rec *memory;	    /* List of attackers to remember	       */
	byte damnodice;          /* The number of damage dice's	       */
	byte damsizedice;        /* The size of the damage dice's           */
	int wait_state;	    /* Wait state for bashed mobs	       */
   /*Lance go*/
	int   clanid;           // serve per assegnare il clan al mob
	long  masterid;        // serve per assegnare un padrone al mob
	int   pospath;
	int   lastdir;
	char  *go_path;
	long  go_chtarget;
	char  *go_nametarget;
	long  go_room;
	int   search_mode;
	int   cmd_mode;
	char  *searchcommand;
	long  savedmaster;
	int   newclanid;           // serve per assegnare il nuovo clan al mob
	long    stipendiomob; // Stipendio base per i mob
	long	paga;	     // Tempo alla prossima Paga
	sbyte conditions[3];         /* Drunk, full, thirsty			*/
	
	/* Puntatore alla struttura degli eserciti */
	struct dati_esercito *esercito;
};


/* An affect structure.  Used in char_file_u *DO*NOT*CHANGE* */
struct affected_type {
	sh_int natura;        /* 0 e' sk/sp - 1 e' proficienza - 2 e' abilita' */
	sh_int type;          /* The type of spell that caused this      */
	sh_int duration;      /* For how long its effects will last      */
	sbyte modifier;       /* This is added to apropriate ability     */
	byte location;        /* Tells which ability to change(APPLY_XXX)*/
	long long int bitvector[4];  /* Tells which bits to set (AFF_XXX)       */
	struct affected_type *next;
};


/* Structure used for chars following other chars */
struct follow_type {
	struct char_data *follower;
	struct follow_type *next;
};

struct last_same_com {
	long int type_event;
	long times;
	struct last_same_com *next;
};

struct char_resistenze {
	int res_fuoco;
	int res_ghiaccio;
	int res_elettricita;
	int res_acido;
	int res_shaarr;
	int res_xhyphys;
	int res_therion;
	int res_silue;
	int res_fisico;
};

struct nomi_titolanti {
	char nome[MAX_NAME_LENGTH+1];
	int titolo;
	byte class;
	time_t richiesta;
};

/* ================== Structure for player/non-player ===================== */
struct char_data {
	int pfilepos;			 /* playerfile pos		  */
	mob_rnum  nr;                         /* Mob's rnum			  */
	room_rnum in_room;                    /* Location (real room number)	  */
	room_rnum was_in_room;		 /* location for linkdead people  */
	
	struct char_player_data player;       /* Normal data                   */
	struct char_ability_data real_abils;	 /* Abilities without modifiers   */
	struct char_ability_data aff_abils;	 /* Abils with spells/stones/etc  */
	struct char_point_data points;        /* Points                        */
	struct char_special_data char_specials;	/* PC/NPC specials	  */
	struct player_special_data *player_specials; /* PC specials		  */
	struct mob_special_data mob_specials;	/* NPC specials		  */
	struct char_resistenze resistenze;			/* Resistenze del PC/NPC	*/
	
	struct affected_type *affected;       /* affected by what spells       */
	struct obj_data *equipment[NUM_WEARS];/* Equipment array               */
	
	struct obj_data *carrying;            /* Head of list                  */
	struct descriptor_data *desc;         /* NULL for mobiles              */
	
	long id;                            /* used by DG triggers             */
	struct trig_proto_list *proto_script; /* list of default triggers      */
	struct script_data *script;         /* script info for the object      */
	struct script_memory *memory;       /* for mob memory triggers         */	//Orione
   
	struct char_data *next_in_room;     /* For room->people - list         */
	struct char_data *next;             /* For either monster or ppl-list  */
	struct char_data *next_fighting;    /* For fighting list               */
	
	struct follow_type *followers;        /* List of chars followers       */
	struct char_data *master;             /* Who is char following?        */
	struct reward *ch_reward;                /*Criminal Flags */
	
	int iniz_classe;                      // Utilizzate per l'iniziazione a un ordine
	int iniz_ptagg[6];
	int iniz_da_aggiungere;
	
	struct event *points_event[3];	 /* event for regen               */
	struct event *points_act_event[MAX_ACT_EVENTS];	 /* event for other function */
	time_t last_flee;                     /* Usato per il camp dopo il flee */
	time_t last_learned;                  /* Non si puo' imparare troppo spesso */
	time_t last_lavoro;                   /* Aspettare tra un lavoro e un altro */
	//PEPPE RELIGIONE
	time_t last_gdr;
	struct last_same_com *last_same_command;
};
/* ====================================================================== */


/* ==================== File Structure for Player ======================= */
/*             BEWARE: Changing it will ruin the playerfile		  */
struct char_file_u {
   /* char_player_data */
	char	name[MAX_NAME_LENGTH+1];
	char	description[EXDSCR_LENGTH];
	char	title[MAX_TITLE_LENGTH+1];
	char poofin[MAX_POOF_LENGTH+1];
	char poofout[MAX_POOF_LENGTH+1];
	byte sex;
	byte class;
	byte level;
	sh_int hometown;
	time_t birth;   /* Time of birth of character     */
	int	played;    /* Number of secs played in total */
	ubyte weight;
	ubyte height;
	
	char	pwd[MAX_PWD_LENGTH+1];    /* character's password */
	
	struct char_special_data_saved char_specials_saved;
	struct player_special_data_saved player_specials_saved;
	struct char_ability_data abilities;
	struct char_point_data points;
	struct affected_type affected[MAX_AFFECT];
	
	time_t last_logon;		/* Time (in secs) of last logon */
	char host[HOST_LENGTH+1];	/* host of last logon */
};
/* ====================================================================== */


struct save_trig_var_data {
	char name[MAX_NAME_VAR_LENGTH+1];	 /* name of variable  */
	char value[MAX_VAR_LENGTH+1]; /* value of variable */
};

 //=======================Mob save structure=====================
 /*             BEWARE: Changing it will ruin the mobsfile		  */
struct smob_file_u {
 	/* char_mob_data */
	long  mob_vnum;
	long  idmob;
	char	name[MAX_NAME_LENGTH+1];
	char	short_descr[SHTDESC_LENGTH+1];  /* for NPC 'actions'                    */
	char	long_descr[LNGDESC_LENGTH+1];   /* for 'look'			       */
	char	description[EXDSCR_LENGTH];
	char	title[MAX_TITLE_LENGTH+1];
	byte sex;
	byte class;
	byte level;
	ubyte weight;
	ubyte height;
	
	struct char_special_data_saved char_specials_saved;
	struct player_special_data_saved player_specials_saved;
	struct char_ability_data abilities;
	struct char_point_data points;
	struct affected_type affected[MAX_AFFECT];
	struct save_trig_var_data global_var[MAX_GBL_VAR_SAVED];
	byte position;
	
 	/*Lance go*/
	int   clanid;
	long  masterid;
	int   pospath;
	int   lastdir;
	char  go_path[MAX_INPUT_LENGTH+1];
	long  go_chtarget;
	char  go_nametarget[MAX_NAME_LENGTH+1];
	long  go_room;
	int   search_mode;
	int   cmd_mode;
	char  searchcommand[MAX_INPUT_LENGTH+1];
	long  savedmaster;
	long  stipendiomob;
	long  paga;
	long  speare3;
	long  speare4;
	long  speare5;
	long  speare6;
	int  newclanid;
	int  home;
	int  condition;
	int  numero_unita_armate;
	int  speare11;
	int  speare12;
	
};
/* ====================================================================== */

/* descriptor-related structures ******************************************/


struct txt_block {
	char	*text;
	int aliased;
	struct txt_block *next;
};


struct txt_q {
	struct txt_block *head;
	struct txt_block *tail;
};


struct descriptor_data {
	socket_t	descriptor;		/* file descriptor for socket		*/
	char	host[HOST_LENGTH+1];	/* hostname				*/
	byte	bad_pws;		/* number of bad pw attemps this login	*/
	byte idle_tics;		/* tics idle at password prompt		*/
	int	connected;		/* mode of 'connectedness'		*/
	int	wait;			/* wait for how many loops		*/
	int	desc_num;		/* unique num assigned to desc		*/
	time_t login_time;		/* when the person connected		*/
	char *showstr_head;		/* for keeping track of an internal str	*/
	char **showstr_vector;	/* for paging through texts		*/
	int  showstr_count;		/* number of pages to page through	*/
	int  showstr_page;		/* which page are we currently showing?	*/
	char	**str;			/* for the modify-str system		*/
	char *backstr;		/* added for handling abort buffers     */
	size_t max_str;		/*		-			*/
	long	mail_to;		/* name for mail system			*/
	int	prompt_mode;		/* control of prompt-printing		*/
	char	inbuf[MAX_RAW_INPUT_LENGTH];  /* buffer for raw input		*/
	char	last_input[MAX_INPUT_LENGTH]; /* the last input			*/
	char small_outbuf[SMALL_BUFSIZE];  /* standard output buffer		*/
	char *output;		/* ptr to the current output buffer	*/
	char **history;		/* History of commands, for ! mostly.	*/
	int	history_pos;		/* Circular array position.		*/
	int  bufptr;			/* ptr to end of current output		*/
	int	bufspace;		/* space left in the output buffer	*/
	struct txt_block *large_outbuf; /* ptr to large buffer, if we need it */
	struct txt_q input;		/* q of unprocessed input		*/
	struct char_data *character;	/* linked to char			*/
	struct char_data *original;	/* original char if switched		*/
	struct descriptor_data *snooping; /* Who is this char snooping	*/
	struct descriptor_data *snoop_by; /* And who is snooping this char	*/
	struct descriptor_data *next; /* link to next descriptor		*/
	struct olc_data *olc;	     /*. OLC info - defined in olc.h   .*/
	char *storage;
};


/* other miscellaneous structures ***************************************/


struct msg_type {
	char	*attacker_msg;  /* message to attacker */
	char	*victim_msg;    /* message to victim   */
	char	*room_msg;      /* message to room     */
};


struct message_type {
	struct msg_type die_msg;	/* messages when death			*/
	struct msg_type miss_msg;	/* messages when miss			*/
	struct msg_type hit_msg;	/* messages when hit			*/
	struct msg_type god_msg;	/* messages when hit on god		*/
	struct message_type *next;	/* to next messages of this kind.	*/
};


struct message_list {
	int	a_type;			/* Attack type				*/
	int	number_of_attacks;	/* How many attack messages to chose from. */
	struct message_type *msg;	/* List of messages.			*/
};


struct dex_skill_type {
	sh_int p_pocket;
	sh_int p_locks;
	sh_int traps;
	sh_int sneak;
	sh_int hide;
};


struct dex_app_type {
	sh_int reaction;
	sh_int miss_att;
	sh_int defensive;
	sh_int item_carry;
};


struct str_app_type {
	sh_int tohit;    /* To Hit (THAC0) Bonus/Penalty        */
	sh_int todam;    /* Damage Bonus/Penalty                */
	sh_int carry_w;  /* Maximum weight that can be carrried */
	sh_int wield_w;  /* Maximum weight that can be wielded  */
};


struct wis_app_type {
	byte bonus;       /* how many practices player gains per lev */
};


struct int_app_type {
	byte learn;       /* how many % a player learns a spell/skill */
};


struct con_app_type {
	sh_int hitp;
	sh_int shock;
};


struct title_type {
	char	*title_m;
	char	*title_f;
	int exp;
};


/* element in monster and object index-tables   */
struct index_data {
	int	virtual;    /* virtual number of this mob/obj           */
	int	number;     /* number of existing units of this mob/obj	*/
	SPECIAL(*func);
	char *farg;         /* string argument for special function     */
	struct trig_data *proto;     /* for triggers... the trigger     */
};

 /* linked list for mob/object prototype trigger lists */
struct trig_proto_list {
	int vnum;                             /* vnum of the trigger   */
	struct trig_proto_list *next;         /* next trigger          */
};

struct drunk_struct
{
	int     min_drunk_level;
	int     number_of_rep;
	char    *replacement[11];
};

struct hiscore_data
{
	char name[MAX_NAME_LENGTH+1];
	int level;
	int kill;
	int fama;
	int notorieta;
	int rip;
	int dt;
	int anzianita;
	long soldi;
};


struct reward {
	long idpg;
	long regno;
	long gilda;
	int flag;
	int  vivo_morto;
	long taglia;
	char *descr;
	struct reward *next;
};
