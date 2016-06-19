/* ************************************************************************
*   File: spells.h           35                         Part of CircleMUD *
*  Usage: header file: constants and fn prototypes for spell system       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */
 
#define SAVING_NONE	0
#define SAVING_STR	1
#define SAVING_CON	2
#define SAVING_DEX	3
#define SAVING_INT	4
#define SAVING_WIS	5
#define SAVING_CHA	6

#define ROOM_DISTORSIONE_1	82
#define ROOM_DISTORSIONE_2	83
#define ROOM_DISTORSIONE_3	84

#define DEFAULT_STAFF_LVL	12
#define DEFAULT_WAND_LVL	12

#define CAST_UNDEFINED	-1
#define CAST_SPELL	0
#define CAST_POTION	1
#define CAST_WAND	2
#define CAST_STAFF	3
#define CAST_SCROLL	4
#define CAST_BREATH	5

#define MAG_DAMAGE	(1 << 0)
#define MAG_AFFECTS	(1 << 1)
#define MAG_UNAFFECTS	(1 << 2)
#define MAG_POINTS	(1 << 3)
#define MAG_ALTER_OBJS	(1 << 4)
#define MAG_GROUPS	(1 << 5)
#define MAG_MASSES	(1 << 6)
#define MAG_AREAS	(1 << 7)
#define MAG_SUMMONS	(1 << 8)
#define MAG_CREATIONS	(1 << 9)
#define MAG_MANUAL	(1 << 10)
#define MAG_PRE_COSTO	(1 << 11)

#define SPELLSKILL  0
#define PROFICIENZE 1
#define ABILITA     2

#define TYPE_UNDEFINED               -1
#define SPELL_RESERVED_DBC            0  /* SKILL NUMBER ZERO -- RESERVED */

/* PLAYER SPELLS -- Numbered from 1 to MAX_SPELLS */

#define SPELL_ARMOR                   1 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_TELEPORT                2 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BLESS                   3 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BLINDNESS               4 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BURNING_HANDS           5 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CALL_LIGHTNING          6 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CHARM                   7 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CHILL_TOUCH             8 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CLONE                   9 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_COLOR_SPRAY            10 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CONTROL_WEATHER        11 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_FOOD            12 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_WATER           13 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_BLIND             14 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_CRITIC            15 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_LIGHT             16 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURSE                  17 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_ALIGN           18 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_INVIS           19 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_MAGIC           20 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_POISON          21 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISPEL_EVIL            22 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_EARTHQUAKE             23 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ENCHANT_WEAPON         24 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ENERGY_DRAIN           25 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_FIREBALL               26 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HARM                   27 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HEAL                   28 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_INVISIBLE              29 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LIGHTNING_BOLT         30 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LOCATE_OBJECT          31 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_MAGIC_MISSILE          32 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_POISON                 33 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_PROT_FROM_EVIL         34 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_CURSE           35 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SANCTUARY              36 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SHOCKING_GRASP         37 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SLEEP                  38 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_STRENGTH               39 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SUMMON                 40 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_VENTRILOQUATE          41 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_WORD_OF_RECALL         42 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_POISON          43 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SENSE_LIFE             44 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ANIMATE_DEAD	     45 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISPEL_GOOD	     46 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_GROUP_ARMOR	     47 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_GROUP_HEAL	     48 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_GROUP_RECALL	     49 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_INFRAVISION	     50 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_WATERWALK		     51 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_RELOCATE   	     52 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISINTEGRATE           53 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_FLY		     54
#define SPELL_MINUTE_METEOR	     55
#define SPELL_CONE_OF_COLD	     56
#define SPELL_AREA_LIGHTNING	     57
#define SPELL_FIRE_BREATH            58
#define SPELL_GAS_BREATH             59
#define SPELL_FROST_BREATH           60
#define SPELL_ACID_BREATH            61
#define SPELL_LIGHTNING_BREATH       62
#define SPELL_BLADEBARRIER           63
#define SPELL_PEACE		     64
#define SPELL_PIDENTIFY              65
#define SPELL_ACID_ARROW	     66
#define SPELL_FLAME_ARROW	     67
#define SPELL_LEVITATE               68
#define SPELL_PROT_FIRE              69
#define SPELL_WATERBREATH	     70
#define SPELL_BARKSKIN               71
#define SPELL_STONESKIN		     72
#define SPELL_GROUP_FLY		     73
#define SPELL_GROUP_INVIS            74
#define SPELL_GROUP_PROT_EVIL        75
#define SPELL_GROUP_WATBREATH        76
#define SPELL_CURE_SERIOUS	     77
#define SPELL_MONSUM_I		     78
#define SPELL_MONSUM_II		     79
#define SPELL_MONSUM_III	     80
#define SPELL_MONSUM_IV              81
#define SPELL_MONSUM_V               82
#define SPELL_CONJ_ELEMENTAL	     83
#define SPELL_SHIELD	             84
#define SPELL_MIRROR_IMAGE           85
#define SPELL_BLINK                  86
#define SPELL_DEATHDANCE	     87
#define SPELL_PORTAL                 88
#define SPELL_WRAITHFORM             89
#define SPELL_CREATE_LIGHT           90
#define SPELL_FEAST                  91
#define SPELL_HEROES_FEAST           92
#define SPELL_REFRESH                93
#define SPELL_CONTINUAL_LIGHT        94
#define SPELL_LOCATE_TARGET          95
#define SPELL_FEAR                   96
#define SPELL_FIRESHD                97
#define SPELL_DISPEL_MAGIC           98
#define SPELL_WIZEYE                 99
#define SPELL_MATER_ARMOUR          100
#define SPELL_MATER_WEAP            101
#define SPELL_PARALIZE              102
#define SPELL_CALL_DRAGON           103
#define SPELL_PROTECT_LIGHT         104
#define SPELL_DET_SNEAK             105
#define SPELL_SILENCE               106
#define SPELL_ANTIMAGIC             107
#define SPELL_DISGUISE              108
#define SPELL_AID                   109
#define SPELL_CONGIUNZIONE          110
#define SPELL_ENCHANT_WOOD          111
//PEPPE_AMANU
#define SPELL_ENDURANCE             112
#define SPELL_TIME_DISTORTION	    113

/* Insert new spells here, up to MAX_SPELLS */
#define MAX_SPELLS		    130

/* PLAYER SKILLS - Numbered from MAX_SPELLS+1 to MAX_SKILLS */
#define SKILL_BACKSTAB              131 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_BASH                  132 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_HIDE                  133 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_KICK                  134 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_PICK_LOCK             135 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_SPOT                  136
#define SKILL_RESCUE                137 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_SNEAK                 138 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_STEAL                 139 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_TRACK		    140 /* Reserved Skill[] DO NOT CHANGE */
/* New skills may be added here up to MAX_SKILLS (200) */
#define SKILL_DISARM                141
#define SKILL_SECOND_ATTACK         142
#define SKILL_THIRD_ATTACK          143
#define SKILL_ENH_DAMAGE            144
#define SKILL_PARRY		    145
#define SKILL_DODGE		    146
#define SKILL_COOK		    147
#define SKILL_FILLET		    148
#define SKILL_BREW		    149
#define SKILL_FORGE		    150
#define SKILL_SCRIBE		    151
#define SKILL_SCAN                  152
#define SKILL_REPAIR                153
#define SKILL_FOURTH_ATTACK         154
#define SKILL_FIFTH_ATTACK          155
#define SKILL_SPY                   156
#define SKILL_RETREAT               157
#define SKILL_THROW                 158
#define SKILL_BOW                   159
#define SKILL_SLING                 160
#define SKILL_CROSSBOW              161
#define SKILL_READ_MAGIC            162
#define SKILL_WRITE_MAGIC           163
#define SKILL_PEEP                  164
#define SKILL_PUNCH                 165
#define SECOND_WEAPON               166
#define SECOND_BACKSTAB             167
#define SKILL_BASH_LOCK             168
#define SKILL_COLPO_MORTALE         169
/*
 *  NON-PLAYER AND OBJECT SPELLS AND SKILLS
 *  Spini colse queste spell inutilizzate e le inizio' ad utilizzare
 *  come malattie. Non essendoci pg che castano malattie nel modo
 *  classico, il problema del mancato salvataggio non si pone. Sarebbe
 *  bene evitare di usarlo per altri motivi troppo divergenti...
 */

#define SPELL_IDENTIFY               201
#define SPELL_NULLA		     202

#define DISEASE_RAFFREDDORE	     203
#define DISEASE_PESTE		     204
#define DISEASE_BACCO		     205
#define DISEASE_SHAARR		     206
#define DISEASE_PIAGHE		     207
#define DISEASE_XHYPHYS		     208
#define DISEASE_MORFEO		     209

#define TOP_SPELL_DEFINE	     299
/* NEW NPC/OBJECT SPELLS can be inserted here up to 299 */


#define PROF_RESERVED_DBC              0
/* Lista delle proficienze */
#define PROF_MONTARE                   1
#define PROF_CAVALCARE                 2
#define PROF_DOMARE                    3
#define PROF_CACCIARE                  4
#define PROF_ADDESTRATORE              5
#define PROF_SARTO                     6
#define PROF_COLTIVARE                 7
#define PROF_CUCINARE                  8
#define PROF_MINATORE                  9
#define PROF_FABBRO                   10
#define PROF_FALEGNAMERIA             11
#define PROF_CARPENTERIA              12
#define PROF_GIOIELLERIA              13
#define PROF_ARCHITETTURA             14
#define PROF_ERBORISTERIA             15
#define PROF_ALCHIMIA                 16
#define PROF_PESCATORE                17

#define TOT_PROF_MESTIERI             17 // DEVE ESSERE PARI ALL'ULTIMO MESTIERE

/*Enrico:ABILTA_OFFAR*/
#define TOP_ABILITA_DEFINE            20
#define MAX_ABILITA_LEVELS 15
#define NUM_ABILITA_TITLES 3
#define BASE                0
#define EXPERT              1
#define MASTER              2
#define EXPERT_LIV    9
#define MASTER_LIV   15
#define SINGLE_DAM    1
#define DOUBLE_DAM    2
#define TRIPLE_DAM    3


#define ABIL_RESERVED                  0
               /*ARMI*/
#define ABIL_SPADA                     1
#define ABIL_ASCIA                     2
#define ABIL_PUGNALE                   3
#define ABIL_MAZZA                     4
#define ABIL_LANCIA                    5
              /*DIFESA*/
#define ABIL_SCUDO                     6
#define ABIL_ARM_BASE                  7
#define ABIL_ARM_MEDIA                 8
#define ABIL_ARM_PESANTE               9
              /*ALTRO*/
#define ABIL_TRATTATIVA               10
#define ABIL_RIPARAZIONE              11
#define ABIL_BODYBUILDING             12
#define ABIL_MEDITAZIONE              13
#define ABIL_TRAPPOLE                 14
#define ABIL_STUDIO                   15
/*SOSPESE*/
#define ABIL_DIPLOMAZIA               16
#define ABIL_PERCEZIONE               17
/*-------*/
#define ABIL_REGENERATION             18
#define ABIL_AGGUATO                  19

// Se si aggiungono abilita', aggiornare TOP_ABIL_DEFINE (che e' pari all'ultima abilita' piu' uno)

struct abil_info_type{
int class_users[NUM_CLASSES];
bool start_abilita[NUM_CLASSES];
};
/* WEAPON ATTACK TYPES */

#define TYPE_HIT                     300
#define TYPE_STING                   301
#define TYPE_WHIP                    302
#define TYPE_SLASH                   303
#define TYPE_BITE                    304
#define TYPE_BLUDGEON                305
#define TYPE_CRUSH                   306
#define TYPE_POUND                   307
#define TYPE_CLAW                    308
#define TYPE_MAUL                    309
#define TYPE_THRASH                  310
#define TYPE_PIERCE                  311
#define TYPE_BLAST		     312
#define TYPE_PUNCH		     313
#define TYPE_STAB		     314
#define TYPE_SLAY                    315
/* new attack types can be added here - up to TYPE_SUFFERING */
#define TYPE_HUNGER                  398
#define TYPE_SUFFERING		     399
#define TYPE_TRAPPOLE		     400  // Chardhros: Danno fatto da trappola
//PEPPE DAMAGE
#define TYPE_ROOM_DAMAGE_FUOCO            401  //Amanu: Danno fatto da una room_damage
#define TYPE_ROOM_DAMAGE_ACQUA            402  //Amanu: Danno fatto da una room_damage
#define TYPE_ROOM_DAMAGE_TERRA            403  //Amanu: Danno fatto da una room_damage
#define TYPE_ROOM_DAMAGE_ARIA             404  //Amanu: Danno fatto da una room_damage


#define SAVING_NEVER -1
#define SAVING_PARA   0
#define SAVING_ROD    1
#define SAVING_PETRI  2
#define SAVING_BREATH 3
#define SAVING_SPELL  4


#define TAR_IGNORE        1
#define TAR_CHAR_ROOM     2
#define TAR_CHAR_WORLD    4
#define TAR_FIGHT_SELF    8
#define TAR_FIGHT_VICT   16
#define TAR_SELF_ONLY    32 /* Only a check, use with i.e. TAR_CHAR_ROOM */
#define TAR_NOT_SELF     64 /* Only a check, use with i.e. TAR_CHAR_ROOM */
#define TAR_OBJ_INV     128
#define TAR_OBJ_ROOM    256
#define TAR_OBJ_WORLD   512
#define TAR_OBJ_EQUIP  1024
#define TAR_NOT_WRITE  2048 /* Non sono si possono scrivere su pergamene */

struct spell_info_type {
   byte min_position;	/* Position for caster	 */
   int mana_min;	/* Min amount of mana used by a spell (highest lev) */
   int mana_max;	/* Max amount of mana used by a spell (lowest lev) */
   int mana_change;	/* Change in mana used by spell from lev to lev */

   int min_level[NUM_CLASSES];
   int routines;
   byte violent;
   int targets;         /* See below for use with TAR_XXX  */
};

/* Possible Targets:

   bit 0 : IGNORE TARGET
   bit 1 : PC/NPC in room
   bit 2 : PC/NPC in world
   bit 3 : Object held
   bit 4 : Object in inventory
   bit 5 : Object in room
   bit 6 : Object in world
   bit 7 : If fighting, and no argument, select tar_char as self
   bit 8 : If fighting, and no argument, select tar_char as victim (fighting)
   bit 9 : If no argument, select self, if argument check that it IS self.

*/

#define SPELL_TYPE_SPELL   0
#define SPELL_TYPE_POTION  1
#define SPELL_TYPE_WAND    2
#define SPELL_TYPE_STAFF   3
#define SPELL_TYPE_SCROLL  4


/* Attacktypes with grammar */

struct attack_hit_type {
   char	*singular;
   char	*plural;
   char *weapon;
};


#define ASPELL(spellname) \
void	spellname(int level, struct char_data *ch, \
		  struct char_data *victim, struct obj_data *obj)

#define MANUAL_SPELL(spellname)	spellname(level, caster, cvict, ovict);
ASPELL(spell_refresh);
ASPELL(spell_feast);
ASPELL(spell_create_water);
ASPELL(spell_recall);
ASPELL(spell_teleport);
ASPELL(spell_summon);
ASPELL(spell_locate_object);
ASPELL(spell_charm);
ASPELL(spell_information);
ASPELL(spell_identify);
ASPELL(spell_enchant_weapon);
ASPELL(spell_detect_poison);
ASPELL(spell_disintegrate);
ASPELL(spell_relocate);
ASPELL(spell_peace);
ASPELL(spell_portal);
ASPELL(spell_remove_curse);
ASPELL(spell_locate_target);
ASPELL(spell_fear);
ASPELL(spell_dispel_magic);
ASPELL(spell_call_dragon);
ASPELL(spell_disguise);
ASPELL(spell_enchant_wood);
ASPELL(spell_time_distortion);
ASPELL(spell_burning_hands);
ASPELL(spell_chill_touch);
ASPELL(spell_control_weather);
/* basic magic calling functions */

int find_skill_num(char *name);

void mag_damage(int level, struct char_data *ch, struct char_data *victim,
  int spellnum, int savetype);

void mag_affects(int level, struct char_data *ch, struct char_data *victim,
  int spellnum, int savetype);

void mag_group_switch(int level, struct char_data *ch, struct char_data *tch,
  int spellnum, int savetype);

void mag_groups(int level, struct char_data *ch, int spellnum, int savetype, int casttype);

void mag_masses(int level, struct char_data *ch, int spellnum, int savetype);

void mag_areas(int level, struct char_data *ch, int spellnum, int savetype);

void mag_summons(int level, struct char_data *ch, struct obj_data *obj,
 int spellnum, int savetype);

void mag_points(int level, struct char_data *ch, struct char_data *victim,
 int spellnum, int savetype, int casttype);

void mag_unaffects(int level, struct char_data *ch, struct char_data *victim,
  int spellnum, int type);

void mag_alter_objs(int level, struct char_data *ch, struct obj_data *obj,
  int spellnum, int type);

void mag_creations(int level, struct char_data *ch, int spellnum);

int	call_magic(struct char_data *caster, struct char_data *cvict,
  struct obj_data *ovict, int spellnum, int level, int casttype);

void	mag_objectmagic(struct char_data *ch, struct obj_data *obj,
			char *argument);

int	cast_spell(struct char_data *ch, struct char_data *tch,
  struct obj_data *tobj, int spellnum);


/* other prototypes */
void spell_level(int spell, int class, int level);
void init_spell_levels(void);
char *skill_name(int num);
void give_abilita(int abilita,int class,bool at_start);
void init_abilita(void);
char *abil_name(int num);











