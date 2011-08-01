/* ************************************************************************
*   File: spell_parser.c    35                          Part of CircleMUD *
*  Usage: top-level magic routines; outside points of entry to magic sys. *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "spells.h"
#include "handler.h"
#include "comm.h"
#include "db.h"
#include "dg_scripts.h"	//Orione

int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);
int check_mana_mod(struct char_data *ch,int mana,int obj_type);
struct spell_info_type spell_info[TOP_SPELL_DEFINE + 1];
struct abil_info_type  abil_info[TOP_ABILITA_DEFINE+1];

#define SINFO spell_info[spellnum]
#define AINFO abil_info[abilnum];

extern struct room_data *world;
extern int arena_ok(struct char_data * ch, struct char_data * victim); //Orione

/*
 * This arrangement is pretty stupid, but the number of skills is limited by
 * the playerfile.  We can arbitrarily increase the number of skills by
 * increasing the space in the playerfile. Meanwhile, this should provide
 * ample slots for skills.
 */

char *spells[] =
{
	"!RESERVED!",			/* 0 - reserved */
	
  /* SPELLS */
	
	"armor",			/* 1 */
	"teleport",
	"bless",
	"blindness",
	"burning hands",
	"call lightning",
	"charm person",
	"chill touch",
	"clone",
	"color spray",		/* 10 */
	"control weather",
	"create food",
	"create water",
	"cure blind",
	"cure critic",
	"cure light",
	"curse",
	"detect alignment",
	"detect invisibility",
	"detect magic",		/* 20 */
	"detect poison",
	"dispel evil",
	"earthquake",
	"enchant weapon",
	"energy drain",
	"fireball",
	"harm",
	"heal",
	"invisibility",
	"lightning bolt",		/* 30 */
	"locate object",
	"magic missile",
	"poison",
	"protection from evil",
	"remove curse",
	"sanctuary",
	"shocking grasp",
	"sleep",
	"strength",
	"summon",			/* 40 */
	"ventriloquate",
	"word of recall",
	"remove poison",
	"sense life",
	"animate dead",
	"dispel good",
	"group armor",
	"group heal",
	"group recall",
	"infravision",		/* 50 */
	"waterwalk",
	"relocate",
	"disintegrate",
	"fly",
	"minute meteor",
	"cone of cold",
	"chain lightning",
	"fire breath",
	"gas breath",
	"frost breath",
	"acid breath",
	"lightning breath",
	"blade barrier",
	"peace",
	"identify",	/* 65 */
	"acid arrow",
	"flame arrow",
	"levitate",
	"protect from fire",
	"breath water",	/* 70 */
	"barkskin",
	"stoneskin",
	"group fly",
	"group invis",
	"group prot from evil",
	"group breath water",
	"cure serious",
	"evocate zombie",
	"evocate monster",
	"evocate greater monster",/* 80 */
	"evocate dragon",	   
	"summon ancient dragon",
	"conjure elemental",
	"shield", 	
	"mirror image",/* 85 */
	"blink",
	"death dance",
	"portal",
	"wraithform",
	"create light",	/* 90 */
	"feast",
	"heroes feast",
	"refresh",
	"continual light",
	"locate target",	/* 95 */
	"fear",
	"fireshield",
	"dispel magic",
	"wizardeye",
	"materialize armour",	/* 100 */
	"materialize weapon",
	"paralize",
	"call dragon",
	"protect lightning",
	"detect sneak",	/* 105 */
	"silence",
	"antimagic",
	"disguise",
	"aid",
	"congiunzione",	/* 110 */
	"enchant wood",
	"resistenza",
	"time distortion",
	"!UNUSED!", "!UNUSED!",	/* 115 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 120 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 125 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 130 */
	
  /* SKILLS */
	
	"backstab",			/* 131 */
	"bash",
	"hide",
	"kick",
	"pick lock",
	"spot",
	"rescue",
	"sneak",
	"steal",
	"track",			/* 140 */
	"disarm",
	"second attack",
	"third attack",
	"enhanced damage",
	"parry",	/* 145 */
	"dodge",
	"cook",
	"fillet",
	"brew",
	"forge",	/* 150 */
	"scribe",
	"scan",
	"repair",
	"fourth attack",
	"fifth attack",	/* 155 */
	"spy",
	"retreate",
	"throw",
	"bow",
	"sling",        /* 160 */
	"crossbow",
	"read magic", 
	"write magic", 
	"peep", 
	"punch",	/* 165 */
	"second weapon",
	"second back",
	"bash door", 
	"deathblow", "!UNUSED!",	/* 170 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 175 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 180 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 185 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 190 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 195 */
	"!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!", "!UNUSED!",	/* 200 */
	
  /* OBJECT SPELLS AND NPC SPELLS/SKILLS */
	
	"identify",			/* 201 */
	"fire breath",
	"Raffreddore",
	"Peste",
	"Malattia di Bacco",
	"Maledizione di Shaarr",
	"Piaghe alle mani",
	"Male degli Antichi",
	"Malattia di Morfeo",
	
	"\n"				/* the end */
};

char *proficienze[] =
{
	"!RESERVED!",
	
	"montare",                // 1
	"cavalcare",
	"domare",
	"cacciatore",
	"addestratore",
	"sarto",
	"coltivatore",
	"cuoco",
	"minatore",
	"fabbro",       // 10
	"falegname",
	"carpentiere",
	"gioielliere",
	"architetto",
	"erborista",       // 15
	"alchimista",
	"pescatore", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 20
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 25
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 30
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 35
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 40
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 45
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 50
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 55
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 60
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 65
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 70
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 75
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 80
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 85
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 90
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 95
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",       // 100
	
	"\n"
};

char *abilita[] =
{
	"!RESERVED!",
	"Spada", "Ascia", "Pugnale","Mazza", "Lancia",                       //   5
	"Scudo", "Cuoio", "Maglia", "Piastre","Trattativa",                  //  10
	"Riparazione","Body Building","Meditazione", "Trappole","Studio",    //  15
	"Diplomazia", "Percezione", "Rigenerazione", "Agguato", "!NONUSATO!",//  20
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  35
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  40
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  35
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  40
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  45
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  50
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  55
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  60
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  65
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  70
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  75
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  80
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  85
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  90
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",//  95
	"!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!", "!NONUSATO!",// 100
	"\n"
};

struct syllable {
	char *org;
	char *new;
};


struct syllable syls[] = {
	{" ", " "},
	{"ar", "abra"},
	{"ate", "i"},
	{"cau", "kada"},
	{"blind", "nose"},
	{"bur", "mosa"},
	{"cu", "judi"},
	{"de", "oculo"},
	{"dis", "mar"},
	{"ect", "kamina"},
	{"en", "uns"},
	{"gro", "cra"},
	{"light", "dies"},
	{"lo", "hi"},
	{"magi", "kari"},
	{"mon", "bar"},
	{"mor", "zak"},
	{"move", "sido"},
	{"ness", "lacri"},
	{"ning", "illa"},
	{"per", "duda"},
	{"ra", "gru"},
	{"re", "candus"},
	{"son", "sabru"},
	{"tect", "infra"},
	{"tri", "cula"},
	{"ven", "nofo"},
	{"word of", "inset"},
	{"a", "i"}, {"b", "v"}, {"c", "q"}, {"d", "m"}, {"e", "o"}, {"f", "y"}, {"g", "t"},
	{"h", "p"}, {"i", "u"}, {"j", "y"}, {"k", "t"}, {"l", "r"}, {"m", "w"}, {"n", "b"},
	{"o", "a"}, {"p", "s"}, {"q", "d"}, {"r", "f"}, {"s", "g"}, {"t", "h"}, {"u", "e"},
	{"v", "z"}, {"w", "x"}, {"x", "n"}, {"y", "l"}, {"z", "k"}, {"", ""}
};


int mag_manacost(struct char_data * ch, int spellnum)
{
	int mana;
	
	mana = MAX(SINFO.mana_max - (SINFO.mana_change *
				(GET_LEVEL(ch) - SINFO.min_level[(int) GET_CLASS(ch)])),
			SINFO.mana_min);
	
	return mana;
}


/* say_spell erodes buf, buf1, buf2 */
void say_spell(struct char_data * ch, int spellnum, struct char_data * tch,
	struct obj_data * tobj)
{
	char lbuf[256];
	
	struct char_data *i;
	int j, ofs = 0;
	
	*buf = '\0';
	strcpy(lbuf, spells[spellnum]);
	
	while (*(lbuf + ofs)) {
		for (j = 0; *(syls[j].org); j++) {
			if (!strncmp(syls[j].org, lbuf + ofs, strlen(syls[j].org))) {
				strcat(buf, syls[j].new);
				ofs += strlen(syls[j].org);
			}
		}
	}
	
	if (tch != NULL && tch->in_room == ch->in_room) {
		if (tch == ch)
			sprintf(lbuf, "$n chiude gli occhi e pronuncia le parole: '&5%%s&0'.");
		else
			sprintf(lbuf, "$n fissa $N e pronuncia le parole: '&5%%s&0'.");
	} else if (tobj != NULL &&
		((tobj->in_room == ch->in_room) || (tobj->carried_by == ch)))
		sprintf(lbuf, "$n fissa $p e pronuncia le parole: '&5%%s&0'.");
	else
		sprintf(lbuf, "$n pronuncia le parole: '&5%%s&0'.");
	
	sprintf(buf1, lbuf, spells[spellnum]);
	sprintf(buf2, lbuf, buf);
	
	for (i = world[ch->in_room].people; i; i = i->next_in_room) {
		if (i == ch || i == tch || !i->desc || !AWAKE(i))
			continue;
		if (GET_CLASS(ch) == GET_CLASS(i))
			perform_act(buf1, ch, tobj, tch, i);
		else
			perform_act(buf2, ch, tobj, tch, i);
	}
	
	if (tch != NULL && tch != ch && tch->in_room == ch->in_room) {
		sprintf(buf1, "$n ti fissa e pronuncia le parole: '&5%s&0'.",
			GET_CLASS(ch) == GET_CLASS(tch) ? spells[spellnum] : buf);
		act(buf1, TRUE, ch, NULL, tch, TO_VICT);
	}
}


char *skill_name(int num)
{
	int i = 0;
	
	if (num <= 0) {
		if (num == -1)
			return "UNUSED";
		else
			return "UNDEFINED";
	}
	
	while (num && *spells[i] != '\n') {
		num--;
		i++;
	}
	
	if (*spells[i] != '\n')
		return spells[i];
	else
		return "UNDEFINED";
}


char *abil_name(int num)
{
	int i = 0;
	
	if (num <= 0) {
		if (num == -1)
			return "UNUSED";
		else
			return "UNDEFINED";
	}
	
	while (num && *abilita[i] != '\n') {
		num--;
		i++;
	}
	
	if (*abilita[i] != '\n')
		return spells[i];
	else
		return "UNDEFINED";
}



int find_skill_num(char *name)
{
	int index = 0, ok;
	char *temp, *temp2;
	char first[256], first2[256];
	
	while (*spells[++index] != '\n') {
		if (is_abbrev(name, spells[index]))
			return index;
		
		ok = 1;
		temp = any_one_arg(spells[index], first);
		temp2 = any_one_arg(name, first2);
		while (*first && *first2 && ok) {
			if (!is_abbrev(first2, first))
				ok = 0;
			temp = any_one_arg(temp, first);
			temp2 = any_one_arg(temp2, first2);
		}
		
		if (ok && !*first2)
			return index;
	}
	
	return -1;
}

int find_prof_num(char *name)
{
	int index = 0, ok;
	char *temp, *temp2;
	char first[256], first2[256];
	
	while (*proficienze[++index] != '\n') {
		if (is_abbrev(name, proficienze[index]))
			return index;
		
		ok = 1;
		temp = any_one_arg(proficienze[index], first);
		temp2 = any_one_arg(name, first2);
		while (*first && *first2 && ok) {
			if (!is_abbrev(first2, first))
				ok = 0;
			temp = any_one_arg(temp, first);
			temp2 = any_one_arg(temp2, first2);
		}
		
		if (ok && !*first2)
			return index;
	}
	
	return -1;
}

int find_abil_num(char *name)
{
	int index = 0, ok;
	char *temp, *temp2;
	char first[256], first2[256];
	
	while (*abilita[++index] != '\n') {
		if (is_abbrev(name, abilita[index]))
			return index;
		
		ok = 1;
		temp = any_one_arg(abilita[index], first);
		temp2 = any_one_arg(name, first2);
		while (*first && *first2 && ok) {
			if (!is_abbrev(first2, first))
				ok = 0;
			temp = any_one_arg(temp, first);
			temp2 = any_one_arg(temp2, first2);
		}
		
		if (ok && !*first2)
			return index;
	}
	
	return -1;
}




/*
 * This function is the very heart of the entire magic system.  All
 * invocations of all types of magic -- objects, spoken and unspoken PC
 * and NPC spells, the works -- all come through this function eventually.
 * This is also the entry point for non-spoken or unrestricted spells.
 * Spellnum 0 is legal but silently ignored here, to make callers simpler.
 */
int call_magic(struct char_data * caster, struct char_data * cvict,
	struct obj_data * ovict, int spellnum, int level, int casttype)
{
	int savetype;
	
	if (spellnum < 1 || spellnum > TOP_SPELL_DEFINE)
		return 0;
	
	if (caster->nr != real_mobile(DG_CASTER_PROXY)) {	//Orione
		if (ROOM_FLAGGED(caster->in_room, ROOM_NOMAGIC) && GET_LEVEL(caster) < LVL_IMPL) {
			send_to_char("La tua magia provoca qualche scintilla, una nuvoletta di fumo e scompare.\r\n", caster);
			act("&4&bLa magia di $n provoca qualche scintilla, una nuvoletta di fumo e scompare.&0", FALSE, caster, 0, 0, TO_ROOM);
			return 0;
		}
	
		if (   cvict != NULL 
		    && (   spellnum == SPELL_TELEPORT 
		        || spellnum == SPELL_SUMMON 
		        || spellnum == SPELL_LOCATE_TARGET 
		        || spellnum == SPELL_LOCATE_OBJECT) ) {
			if (ROOM_FLAGGED(IN_ROOM(cvict), ROOM_NOMAGIC)) {
				send_to_char("La tua magia provoca qualche scintilla, una nuvoletta di fumo e scompare.\r\n", caster);
				act("&4&bLa magia di $n provoca qualche scintilla, una nuvoletta di fumo e scompare.&0", FALSE, caster, 0, 0, TO_ROOM);
				return (0);
			}
		}
	
  		/* I want dragons to be able to breathe fire in NOMAGIC areas because it
		 * is more of an ability than a spell.  Note: if the person using the
		 * breath weapon is not an NPC, then the NOMAGIC affect still applies
		 * because I don't have any PC's with breath capabilities.
		 */
		if (   casttype != CAST_BREATH 
		    || (casttype == CAST_BREATH && !IS_NPC(caster)) 
		   ) {
			if (ROOM_FLAGGED(caster->in_room, ROOM_NOMAGIC)) {
				send_to_char("&4&bLa tua magia provoca qualche scintilla, una nuvoletta di fumo e scompare.&0\r\n", caster);
				act("&4&bLa magia di $n provoca qualche scintilla, una nuvoletta di fumo e scompare.&0", FALSE, caster, 0, 0, TO_ROOM);
				return 0;
			}
		}
/*		
		if (   GET_LEVEL(caster) < LVL_IMPL 
		    && IS_SET(ROOM_FLAGS(caster->in_room), ROOM_PEACEFUL) 
		    && (SINFO.violent || IS_SET(SINFO.routines, MAG_DAMAGE)) ) {
			send_to_char("&2&bUn lampo di luce bianca riempie la stanza, scacciando la tua&0 "
				           "violenta stregoneria!&0\r\n", caster);
			act("&2&bUn lampo di luce bianca improvvisamente riempie la stanza,&0 "
				  "&2&bpoi svanisce.&0", FALSE, caster, 0, 0, TO_ROOM);
			return 0;
		}*/
	}

	if (affected_by_spell (caster, SPELLSKILL, DISEASE_PESTE)){
		act ("&1Il tuo corpo straziato soffre orribilmente mentre cerchi di convogliare il flusso magico. Abbandoni ogni tentativo.&0", FALSE, caster, 0, 0, TO_CHAR);
		return 0;
	}

	/* determine the type of saving throw */
	switch (casttype) {
		case CAST_STAFF:
		case CAST_SCROLL:
		case CAST_POTION:
		case CAST_WAND:
			savetype = SAVING_ROD;
			break;
		default:
			savetype = SAVING_SPELL;
			break;
	}
	
	if (IS_SET(SINFO.routines, MAG_DAMAGE))
		mag_damage(level, caster, cvict, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_AFFECTS))
		mag_affects(level, caster, cvict, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_UNAFFECTS))
		mag_unaffects(level, caster, cvict, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_POINTS))
		mag_points(level, caster, cvict, spellnum, savetype, casttype);
	
	if (IS_SET(SINFO.routines, MAG_ALTER_OBJS))
		mag_alter_objs(level, caster, ovict, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_GROUPS))
		mag_groups(level, caster, spellnum, savetype, casttype);
	
	if (IS_SET(SINFO.routines, MAG_MASSES))
		mag_masses(level, caster, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_AREAS))
		mag_areas(level, caster, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_SUMMONS))
		mag_summons(level, caster, ovict, spellnum, savetype);
	
	if (IS_SET(SINFO.routines, MAG_CREATIONS))
		mag_creations(level, caster, spellnum);
	
	if (IS_SET(SINFO.routines, MAG_MANUAL))
		switch (spellnum) {
			case SPELL_CHARM:		
			  MANUAL_SPELL(spell_charm);
			  break;
			case SPELL_CREATE_WATER:
				MANUAL_SPELL(spell_create_water);
				break;
			case SPELL_DETECT_POISON:
				MANUAL_SPELL(spell_detect_poison); 
				break;
			case SPELL_ENCHANT_WEAPON:
			  MANUAL_SPELL(spell_enchant_weapon);
			  break;
			case SPELL_IDENTIFY:
				MANUAL_SPELL(spell_identify);
				break;
			case SPELL_LOCATE_OBJECT:
			  MANUAL_SPELL(spell_locate_object);
			  break;
			case SPELL_SUMMON:
				MANUAL_SPELL(spell_summon);
				break;
			case SPELL_WORD_OF_RECALL:
			  MANUAL_SPELL(spell_recall);
			  break;
			case SPELL_TELEPORT:
			  MANUAL_SPELL(spell_teleport);
			  break;
			case SPELL_RELOCATE:
			  MANUAL_SPELL(spell_relocate);
			  break;
			case SPELL_DISINTEGRATE:
			  MANUAL_SPELL(spell_disintegrate);
			  break;
			case SPELL_PIDENTIFY:
			  MANUAL_SPELL(spell_identify);
			  break;
			case SPELL_PEACE:
			  MANUAL_SPELL(spell_peace);
			  break;
			case SPELL_PORTAL:
			  MANUAL_SPELL(spell_portal);
			  break;
			case SPELL_FEAST:
			  MANUAL_SPELL(spell_feast);
			  break;
			case SPELL_REFRESH:
			  MANUAL_SPELL(spell_refresh);
			  break;
			case SPELL_REMOVE_CURSE:
			  MANUAL_SPELL(spell_remove_curse);
			  break;
			case SPELL_LOCATE_TARGET:
			  MANUAL_SPELL(spell_locate_target);
			  break;
			case SPELL_FEAR:
			  MANUAL_SPELL(spell_fear);
			  break;
			case SPELL_DISPEL_MAGIC:
			  MANUAL_SPELL(spell_dispel_magic);
			  break;
			case SPELL_CALL_DRAGON:
			  MANUAL_SPELL(spell_call_dragon);
			  break;
			case SPELL_DISGUISE:
			  MANUAL_SPELL(spell_disguise);
			  break;
			case SPELL_ENCHANT_WOOD:
			  MANUAL_SPELL(spell_enchant_wood);
			  break;
			case SPELL_TIME_DISTORTION:
			  MANUAL_SPELL(spell_time_distortion);
			  break;
			case SPELL_BURNING_HANDS:
			  MANUAL_SPELL(spell_burning_hands);
			  break;
			case SPELL_CHILL_TOUCH:
			  MANUAL_SPELL(spell_chill_touch);
			  break;
			case SPELL_CONTROL_WEATHER:
			  MANUAL_SPELL(spell_control_weather);
			  break;
		}
		
	return 1;
}


/*
 * mag_objectmagic: This is the entry-point for all magic items.  This should
 * only be called by the 'quaff', 'use', 'recite', etc. routines.
 *
 * For reference, object values 0-3:
 * staff  - [0]	level	[1] max charges	[2] num charges	[3] spell num
 * wand   - [0]	level	[1] max charges	[2] num charges	[3] spell num
 * scroll - [0]	level	[1] spell num	[2] spell num	[3] spell num
 * potion - [0] level	[1] spell num	[2] spell num	[3] spell num
 *
 * Staves and wands will default to level 14 if the level is not specified;
 * the DikuMUD format did not specify staff and wand levels in the world
 * files (this is a CircleMUD enhancement).
 */

void mag_objectmagic(struct char_data * ch, struct obj_data * obj,
	char *argument)
{
	int i, k;
	struct char_data *tch = NULL, *next_tch;
	struct obj_data *tobj = NULL;
	
	one_argument(argument, arg);
	
	/* Orione Inizio */
	/* se nella room del pg e' presente un mob di quest, 
	 * impedisco l'uso di staff, wand, scroll e pozioni
	 * se TUTTE le pozioni hanno tch=ch, o non esistono con spell ad area, potrei lasciare le pozioni
	*/
	for (tch = world[ch->in_room].people; tch; tch = next_tch) {
	    next_tch = tch->next_in_room;
	    if (IS_NPC(tch) && MOB_FLAGGED(tch, MOB_QUEST)) {
		sprintf(buf, "L'aura magica di %s annullerebbe l'effetto.\r\n", GET_NAME(tch));
		send_to_char(buf, ch);
		return;
    	    }
	}
	/* Orione Fine */

	k = generic_find(arg, FIND_CHAR_ROOM | FIND_OBJ_INV | FIND_OBJ_ROOM |
			FIND_OBJ_EQUIP, ch, &tch, &tobj);
		       
	switch (GET_OBJ_TYPE(obj)) {
		case ITEM_STAFF:
			act("Batti $p tre volte sul terreno.", FALSE, ch, obj, 0, TO_CHAR);
			if (obj->action_description)
				act(obj->action_description, FALSE, ch, obj, 0, TO_ROOM);
			else
				act("$n batte $p tre volte sul terreno.", FALSE, ch, obj, 0, TO_ROOM);
			
			if (GET_OBJ_VAL(obj, 2) <= 0) {
				act("Sembra ormai senza potere.", FALSE, ch, obj, 0, TO_CHAR);
				act("Non sembra accadere nulla di particolare.", FALSE, ch, obj, 0, TO_ROOM);
			} else {
				GET_OBJ_VAL(obj, 2)--;
				WAIT_STATE(ch, PULSE_VIOLENCE);
				for (tch = world[ch->in_room].people; tch; tch = next_tch) {
					next_tch = tch->next_in_room;
					if (ch == tch)
						continue;
					if (GET_OBJ_VAL(obj, 0))
						call_magic(ch, tch, NULL, GET_OBJ_VAL(obj, 3), GET_OBJ_VAL(obj, 0), CAST_STAFF);
					else
						call_magic(ch, tch, NULL, GET_OBJ_VAL(obj, 3), DEFAULT_STAFF_LVL, CAST_STAFF);
				}
			}
			break;
		case ITEM_WAND:
			if (k == FIND_CHAR_ROOM) {
				if (tch == ch) {
					act("Punti $p verso te stesso.", FALSE, ch, obj, 0, TO_CHAR);
					act("$n punta $p verso se' stesso.", FALSE, ch, obj, 0, TO_ROOM);
				} else {
					act("Punti $p verso $N.", FALSE, ch, obj, tch, TO_CHAR);
					if (obj->action_description != NULL)
						act(obj->action_description, FALSE, ch, obj, tch, TO_ROOM);
					else
						act("$n punta $p verso $N.", TRUE, ch, obj, tch, TO_ROOM);
				}
			} else if (tobj != NULL) {
				act("Punti $p verso $P.", FALSE, ch, obj, tobj, TO_CHAR);
				if (obj->action_description != NULL)
					act(obj->action_description, FALSE, ch, obj, tobj, TO_ROOM);
				else
					act("$n punta $p verso $P.", TRUE, ch, obj, tobj, TO_ROOM);
			} else {
				act("Verso cosa vorresti puntare $p?", FALSE, ch, obj, NULL, TO_CHAR);
				return;
			}
			
			if (GET_OBJ_VAL(obj, 2) <= 0) {
				act("Sembra ormai senza potere.", FALSE, ch, obj, 0, TO_CHAR);
				act("Non sembra accadere nulla di particolare.", FALSE, ch, obj, 0, TO_ROOM);
				return;
			}
			GET_OBJ_VAL(obj, 2)--;
			WAIT_STATE(ch, PULSE_VIOLENCE);
			if (GET_OBJ_VAL(obj, 0))
				call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, 3), GET_OBJ_VAL(obj, 0), CAST_WAND);
			else
				call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, 3), DEFAULT_WAND_LVL, CAST_WAND);
			break;
		case ITEM_BENDE_UNTE:
			if (k == FIND_CHAR_ROOM) {
				if (tch == ch) {
					act("Applichi $p sulle tue mani piagate.", FALSE, ch, obj, 0, TO_CHAR);
					act("$n applica $p sulle sue mani piagate.", FALSE, ch, obj, 0, TO_ROOM);
				} else {
					if (affected_by_spell (ch, SPELLSKILL, DISEASE_PIAGHE)){
						act ("Le tue mani sono cosi' malconce che per prima cosa dovresti pensare a loro!", FALSE, ch, 0, 0, TO_CHAR);
					}
					act("Applichi $p sulle mani di $N.", FALSE, ch, obj, tch, TO_CHAR);
					if (obj->action_description != NULL)
						act(obj->action_description, FALSE, ch, obj, tch, TO_ROOM);
					else
						act("$n applica $p sulle mani di $N.", TRUE, ch, obj, tch, TO_ROOM);
				}
			} else {
				act("Su cosa vorresti applicare $p?", FALSE, ch, obj, NULL, TO_CHAR);
				return;
			}
			if (affected_by_spell (tch, SPELLSKILL, DISEASE_PIAGHE)){
				act ("Le bende applicate sulle tue mani sono miracolose! Ti senti gia' molto meglio!", FALSE, 0, 0, tch, TO_VICT);
				affect_from_char (tch, SPELLSKILL, DISEASE_PIAGHE);
			}
			else
				act ("Sebbene un po' di grasso sulle mani di un guerriero fa sempre bene, hai come l'impressione di aver appena sprecato delle buone bende unte...", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case ITEM_SCROLL:
			if (*arg) {
				if (!k) {
					act("Non c'e' niente qui su cui utilizzare $p.", FALSE,
						ch, obj, NULL, TO_CHAR);
					return;
				}
			} else
				tch = ch;
			
			act("Reciti $p che sparisce avviluppata in arcane fiamme verdi.", TRUE, ch, obj, 0, TO_CHAR);
			if (obj->action_description)
				act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
			else
				act("$n recita $p.", FALSE, ch, obj, NULL, TO_ROOM);
			
			WAIT_STATE(ch, PULSE_VIOLENCE);
			for (i = 1; i < 4; i++) {
				if (tch!=NULL)
					if (GET_POS(tch)<=POS_DEAD)
						break;
				if (!(call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, i), GET_OBJ_VAL(obj, 0), CAST_SCROLL)))
					break;
			}
			if (obj != NULL)
				extract_obj(obj);
			break;
		case ITEM_POTION:
			tch = ch;
			act("Trangugi $p.", FALSE, ch, obj, NULL, TO_CHAR);
			if (obj->action_description)
				act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
			else
				act("$n trangugia $p.", TRUE, ch, obj, NULL, TO_ROOM);
			
			WAIT_STATE(ch, PULSE_VIOLENCE);
			for (i = 1; i < 4; i++)
				if (!(call_magic(ch, ch, NULL, GET_OBJ_VAL(obj, i), GET_OBJ_VAL(obj, 0), CAST_POTION)))
					break;
				
			if (obj != NULL)
				extract_obj(obj);
			break;
		case ITEM_TISANA_CALDA:
			tch = ch;
			act ("Trangugi $p.", FALSE, ch, obj, NULL, TO_CHAR);
			act ("$n trangugia $p.", TRUE, ch, obj, NULL, TO_ROOM);

			WAIT_STATE(ch, PULSE_VIOLENCE);

			act ("Mentre ingoi $p una piacevole sensazione ti invade la gola...", FALSE, ch, obj, 0, TO_CHAR);
			if (affected_by_spell (ch, SPELLSKILL, DISEASE_RAFFREDDORE)){
				act ("Senti che il calore e il potere medicinale delle erbe ti procurano un immediato sollievo alla gola.", FALSE, ch, 0, 0, TO_CHAR);
				affect_from_char (ch, SPELLSKILL, DISEASE_RAFFREDDORE);
			}

			if (obj != NULL)
				extract_obj(obj);
			break;
    /*Per immobilizzare un pg....*/
		case ITEM_IMMOBIL:
			if(tch==ch){
				sprintf(buf,"Nonostante ti metta d'impegno non riesci nell intento di Immobilizzarti!\n");
				send_to_char(buf,ch);
				break;
			}
			if(tch!=NULL){
				if(GET_POS(tch)>=POS_RESTING&&!IS_AFFECTED(tch,AFF_TRAMORTITO)&&!IS_AFFECTED(tch,AFF_SLEEP)&&!IS_AFFECTED(tch,AFF_PARALIZE)){
					sprintf(buf,"Devi renderlo inoffensivo prima!\n");
					send_to_char(buf,ch);
					break;
				}
				if(IS_AFFECTED(tch,AFF_IMMOBIL)){
					send_to_char("Ma se e' gia legato!",ch);
					break;
				}
				else{
					sprintf(buf,"\rLeghi %s come un salame,ora sara difficile che riesca a muoversi!\n",GET_NAME(tch));
					send_to_char(buf,ch);
					act("\r$n lega $N come un salame,ora sara difficile che riesca a muoversi!!\n",FALSE,ch,0,tch,TO_ROOM);
      /*affect_to_char(tch, af);*/
					obj_to_char((obj=unequip_char(ch,WEAR_HOLD)), ch);
					obj_from_char(obj);
      /*  obj_to_char(obj,tch);*/
					equip_char(tch,obj,WEAR_IMMOBIL);
					SET_BIT( AFF_FLAGS(tch),AFF_IMMOBIL);
					break;
				}
			}
  /*********************************/
		default:
			log("SYSERR: Unknown object_type in mag_objectmagic");
			break;
	}
}


/*
 * cast_spell is used generically to cast any spoken spell, assuming we
 * already have the target char/obj and spell number.  It checks all
 * restrictions, etc., prints the words, etc.
 *
 * Entry point for NPC casts.  Recommended entry point for spells cast
 * by NPCs via specprocs.
 */

int cast_spell(struct char_data * ch, struct char_data * tch,
	struct obj_data * tobj, int spellnum)
{
	char buf[256];
	
	if (spellnum < 0 || spellnum > TOP_SPELL_DEFINE) {
		sprintf(buf, "SYSERR: cast_spell trying to call spellnum %d\n", spellnum);
		log(buf);
		return 0;
	}
	if (AFF_FLAGGED(ch, AFF_PARALIZE))
		return 0;
	if (tch) {
	//Non si spella sugli Dei!
		if ((!IS_NPC(tch)) && (GET_LEVEL(tch) >= LVL_QUEST_MASTER) && (GET_LEVEL(tch) > GET_LEVEL(ch)))
			return 0;
	//Non si spella sulla selvaggina
		if ((!IS_NPC(tch)) && MOB_FLAGGED(tch, MOB_SELVAGGINA) && (!MOB_FLAGGED(tch, MOB_HARD_TAME)))
			return 0;
	}	
	if (GET_POS(ch) < SINFO.min_position && GET_POS(ch) != POS_FIGHTING) {
		switch (GET_POS(ch)) {
			case POS_SLEEPING:
				send_to_char("Sogni di immani poteri magici.\r\n", ch);
				break;
			case POS_RESTING:
				send_to_char("Non riesci a concentrarti abbastanza mentre stai riposando.\r\n", ch);
				break;
			case POS_SITTING:
				send_to_char("Non puoi farlo se stai seduto!\r\n", ch);
				break;
			case POS_FIGHTING:
				send_to_char("Non puoi concentrarti per formulare l'incantesimo e nel frattempo evitare i colpi nemici!\r\n", ch);
				break;
			default:
				send_to_char("Non puoi fare nulla di simile!\r\n", ch);
				break;
		}
		return 0;
	}
	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == tch)) {
		send_to_char("Hai paura di poter ferire per sbaglio il tuo signore!\r\n", ch);
		return 0;
	}
	if ((tch != ch) && IS_SET(SINFO.targets, TAR_SELF_ONLY)) {
		send_to_char("Puoi utilizzare questo incantesimo solo su di te!\r\n", ch);
		return 0;
	}
	if ((tch == ch) && IS_SET(SINFO.targets, TAR_NOT_SELF)) {
		send_to_char("Non puoi utilizzare questo incantesimo su te stesso!\r\n", ch);
		return 0;
	}
	if (IS_SET(SINFO.routines, MAG_GROUPS) && !IS_AFFECTED(ch, AFF_GROUP)) {
		send_to_char("Non puoi utilizzare questo incantesimo se non sei in un gruppo!\r\n",ch);
		return 0;
	}
	
	/* By Spini: ora antimagic para solo le spell violente, per non renderlo piu' dannoso
	   che utile, e non ritorna 0 ma 1, in modo da dare il turno di lag al caster e fargli
	   pagare il prezzo in mana */
	if (tch && SINFO.violent && AFF_FLAGGED(tch, AFF_ANTIMAGIC) && (number(1, 100) < 10)) {
		sprintf(buf,"La tua magia viene dissolta dalla barriera antimagia di %s!\r\n", GET_NAME(tch));
		send_to_char(buf, ch);
		return 1;
	}
	send_to_char(OK, ch);
	say_spell(ch, spellnum, tch, tobj);
	
	return (call_magic(ch, tch, tobj, spellnum, GET_LEVEL(ch), CAST_SPELL));
}


/*
 * do_cast is the entry point for PC-casted spells.  It parses the arguments,
 * determines the spell number and finds a target, throws the die to see if
 * the spell can be cast, checks for sufficient mana and subtracts it, and
 * passes control to cast_spell().
 */

ACMD(do_cast)
{
	struct char_data *tch = NULL;
	struct char_data *next_tch; //Orione
  struct char_data *k = NULL;
	struct follow_type *f;
	struct obj_data *tobj = NULL;
	char *s, *t;
	int mana, spellnum, i, target = 0, num, lev, med;
  int target_questmob_group = 0;       //Orione
	int target_cast_area = 0;	     //Orione
	int target_questmob = 0;	     //Orione
	extern struct index_data *mob_index; //Orione
	extern int pk_allowed;		     //Orione
		    	
	if (IS_NPC(ch))
		return;
	
	if(AFF_FLAGGED(ch, AFF_IMMOBIL)) {
		send_to_char("Sei legato!!\n",ch);
		return;
	}
  /* get: blank, spell name, target name */
	s = strtok(argument, "'");

	if (s == NULL) {
		send_to_char("Quale incantesimo vuoi pronunciare e su chi?\r\n", ch);
		return;
	}
	
	s = strtok(NULL, "'");
	
	if (s == NULL) {
		send_to_char("I nomi degli incantesimi devono essere racchiusi dai simboli magici Sacri: '\r\n", ch);
		return;
	}
	
	t = strtok(NULL, "\0");
	
  /* spellnum = search_block(s, spells, 0); */
	spellnum = find_skill_num(s);
	
	if ((spellnum < 1) || (spellnum > MAX_SPELLS)) {
		send_to_char("Quale incantesimo vuoi pronunciare?!?\r\n", ch);
		return;
	}
	if (GET_LEVEL(ch) < SINFO.min_level[(int) GET_CLASS(ch)]) {
		send_to_char("Non conosci ancora questo incantesimo!\r\n", ch);
		return;
	}

	if(ROOM_FLAGGED(ch->in_room, ROOM_NOMAGIC)) {
		send_to_char("Non te la senti di lanciare un incantesimo in questo luogo\r\n",ch); 
		return;
	}

	if ((GET_SKILL(ch, spellnum) == 0) && (GET_LEVEL(ch) < LVL_QUEST_MASTER)) {
		send_to_char("Non hai molta esperienza con questo incantesimo.\r\n", ch);
		return;
	}
  /* Find the target */
	if (t != NULL) {
		one_argument(strcpy(arg, t), t);
		skip_spaces(&t);
	}
	if (IS_SET(SINFO.targets, TAR_IGNORE)) {
		target = TRUE;
	} 
	else if (t != NULL && *t) {
		if (!target && (IS_SET(SINFO.targets, TAR_CHAR_ROOM))) {
			if ((tch = get_char_room_vis(ch, t)) != NULL)
				target = TRUE;
		}
		if (!target && IS_SET(SINFO.targets, TAR_CHAR_WORLD))
		if ((tch = get_char_vis(ch, t)))
			target = TRUE;
			
		if (!target && IS_SET(SINFO.targets, TAR_OBJ_INV))
		if ((tobj = get_obj_in_list_vis(ch, t, ch->carrying)))
			target = TRUE;
			
		if (!target && IS_SET(SINFO.targets, TAR_OBJ_EQUIP)) {
			for (i = 0; !target && i < NUM_WEARS; i++)
			if (   GET_EQ(ch, i) 
			    && CAN_SEE_OBJ(ch, GET_EQ(ch, i)) 
			    && isname(t, GET_EQ(ch, i)->name) ) {
				tobj = GET_EQ(ch, i);
				target = TRUE;
			}
		}
		if (!target && IS_SET(SINFO.targets, TAR_OBJ_ROOM))
		if ((tobj = get_obj_in_list_vis(ch, t, world[ch->in_room].contents)))
			target = TRUE;
			
		if (!target && IS_SET(SINFO.targets, TAR_OBJ_WORLD))
		if ((tobj = get_obj_vis(ch, t)))
			target = TRUE;
			
	} 
	else {			/* if target string is empty */
		if (!target && IS_SET(SINFO.targets, TAR_FIGHT_SELF))
		if (FIGHTING(ch) != NULL) {
			tch = ch;
			target = TRUE;
		}
		if (!target && IS_SET(SINFO.targets, TAR_FIGHT_VICT))
		if (FIGHTING(ch) != NULL) {
			tch = FIGHTING(ch);
			target = TRUE;
		}
    /* if no target specified, and the spell isn't violent, default to self */
		if (   !target 
		    && IS_SET(SINFO.targets, TAR_CHAR_ROOM) 
		    && !SINFO.violent ) {
			tch = ch;
			target = TRUE;
		}
		if (!target) {
			sprintf(buf, "A %s dovrebbe esser diretto l'incantesimo?\r\n",
				          IS_SET(SINFO.targets, TAR_OBJ_ROOM | TAR_OBJ_INV | TAR_OBJ_WORLD) ?
				          "cosa" : "chi");
			send_to_char(buf, ch);
			return;
		}
	}
	

	if (target && (tch == ch) && SINFO.violent) {
		send_to_char("Non dovresti dirigere questo incantesimo su di te... potrebbe non essere salutare!\r\n", ch);
		return;
	}

	if(tch) {
		if(IS_NPC(tch)) {
			if(MOB_FLAGGED(tch, MOB_ESERCITO)) {
				send_to_char("Non puoi lanciare incantesimi ad un esercito!\r\n", ch);
				return;
			}
		}
	}
	
	/*if(ROOM_FLAGGED(tch->in_room,ROOM_NOMAGIC)){
		send_to_char("Il luogo dove si trova al momento non ti permette di effetuare il lancio della magia\r\n",ch); 
		return;
	}*/
	
	if (tch)
			if ((GET_LEVEL(tch) >= LVL_QUEST_MASTER) && (GET_LEVEL(tch) > GET_LEVEL(ch))) {
				send_to_char("Non sarebbe molto salutare.\r\n",ch);
				return;
			}
	
	if (!target) {
		send_to_char("Non trovi il bersaglio giusto!\r\n", ch);
		return;
	}
	//PEPPE VISUALIZ
	if (tch) {
	  if (   target 
	      && PLR_FLAGGED(tch, PLR_FANTASMA) 
	      && !IS_NPC(tch) 
	      && GET_LEVEL(ch) < LVL_QUEST_MASTER ) {
	    send_to_char("Sarebbe insensato lanciare uno spell su di un fantasma.\r\n",ch);
	    return;
	  }
	}

	//Non si spella sulla selvaggina - Orione
	if (tch && MOB_FLAGGED(tch, MOB_SELVAGGINA) && (!MOB_FLAGGED(tch, MOB_HARD_TAME))) {
		act("Incantesimi?! Prova a cacciarlo.", FALSE, ch, 0, tch, TO_CHAR);
		return;
	}

  // Orione Inizio
	//    I cast a target contro mob di quest sono possibili solo se il mob e' destinato al pg caster,
  // nel qual caso il pg puo' attaccarli; altrimenti devono essere desitnati ad un membro del gruppo
	// cui appartiene il pg caster, e questo membro e' l'unico a poter sferrare il primo attacco
	// (fisico o magico) contro tale mob, ma puo' poi poi essere assistito dai compagni di gruppo.
	
	if (GET_LEVEL(ch) < LVL_IMMORT) {
	  if (tch && IS_NPC(tch) && MOB_FLAGGED(tch, MOB_QUEST)) {
		  if (FIGHTING(tch)==NULL) {
		    if (!(   IS_QUESTOR(ch) 
		          && (   (GET_TIPOQUEST(ch) == QUEST_MOBKILL) 
		              || (GET_TIPOQUEST(ch) == QUEST_GETITEM) ) 
		          && (GET_QUESTMRR(ch)==GET_MOB_VNUM(tch)) ) ) {
			    //mob non in combattimento, non e' ch il pg questor destinato
    			send_to_char("Il destino non ha riservato a te questo onore!\r\n",ch);
			    return;
		    }
		  } 
		  else if (ch && AFF_FLAGGED(ch, AFF_GROUP)) {
		    if (ch->master) 
		      k = ch->master;
		    else 
		      k = ch;

		    //mob in combattimento, e' contro qualcuno del gruppo cui appartiene ch?
		    //se non ci sono errori dovrebbe combattere contro il pg cui e' destinato
		    if (   IS_QUESTOR(k) 
		        && (   (GET_TIPOQUEST(k) == QUEST_MOBKILL) 
		            || (GET_TIPOQUEST(k) == QUEST_GETITEM) ) 
		        && (GET_QUESTMRR(k) == GET_MOB_VNUM(tch)) )
			    target_questmob_group = TRUE;
		    if (!target_questmob_group) {
			    for (f = k->followers; f; f = f->next) {
			      if (   IS_QUESTOR(f->follower) 
			          &&	(   (GET_TIPOQUEST(f->follower) == QUEST_MOBKILL) 
			              || (GET_TIPOQUEST(f->follower) == QUEST_GETITEM) ) 
			          && (GET_QUESTMRR(f->follower) == GET_MOB_VNUM(tch)) ) {
				      target_questmob_group = TRUE;
				      break;
			      }
			    }
		    }
	    
	    	//mob in combattimento, ma contro qualcuno che non appartiene al gruppo di cui fa parte ch
		    //se non ci sono errori dovrebbe combattere contro il pg cui e' destinato
		    if (!target_questmob_group) {
			    send_to_char("Non appartieni alla compagnia predestinata a farlo.\r\n",ch);
		      return;
		    }
		  } 
		  else if (!(   !IS_NPC(ch) 
		             && IS_QUESTOR(ch) 
		             && (   (GET_TIPOQUEST(ch) == QUEST_MOBKILL) 
		                 || (GET_TIPOQUEST(ch) == QUEST_GETITEM) ) 
		             && (GET_QUESTMRR(ch) == GET_MOB_VNUM(tch)) ) ) { 
		            //mob in combattimento, ch non gruppato
		    				//se non ci sono errori dovrebbe combattere contro il pg cui e' destinato
		    send_to_char("La questione non sembra riguardarti!\r\n",ch);
		    return;
		  } 
	  }
  }
	// Orione Fine
    																																											    								    
  // Orione Inizio
	// I cast ad area richiedono che almeno uno dei target sia valido: si escludono quindi 
	// i casi in cui e' presente il solo pg caster, immortali, altri pg ma non si puo' fare pkilling,
	// mob charmati, e mob di quest; nell'ultimo caso e' pero' possibile attaccare il mob se:
	// 1) il pg caster e' il pg cui il mob di quest e' destinato
	// 2) il mob di quest e' destinato ad un membro del gruppo di cui il pg caster fa parte,
	//    e questo pg sta gia' combattendo contro il mob
	
  if (GET_LEVEL(ch) < LVL_IMMORT) {
	  if (!tch && SINFO.violent) {
		  for (tch = world[ch->in_room].people; tch; tch = next_tch) {
		    next_tch = tch->next_in_room;
		    if (tch == ch)
		      continue;
		    if (!IS_NPC(tch) && GET_LEVEL(tch) >= LVL_IMMORT)
			    continue;
		    if (!pk_allowed && !arena_ok(ch, tch) && !IS_NPC(ch) && !IS_NPC(tch))
		      continue;
		    if (!IS_NPC(ch) && IS_NPC(tch) && IS_AFFECTED(tch, AFF_CHARM))
		      continue;
		    if (MOB_FLAGGED(tch, MOB_SELVAGGINA) && (!MOB_FLAGGED(tch, MOB_HARD_TAME)))
		      continue;
		    if (!IS_NPC(ch) && IS_NPC(tch) && MOB_FLAGGED(tch, MOB_QUEST)) {
			    target_questmob = TRUE;
		      continue;
		    }
		    target_cast_area = TRUE;
		  }
		  if (!target_cast_area) {
		    //i target sono solo caster, immos, pg, mob charmati e/o questmob
	    	if (target_questmob) { 
	    		  //almeno 1 target e' questmob
			     // imposto tch come il mob di quest
			     // DIFETTO: se ci sono 2 mob di quest presenti nella stessa room
			     //	imposta come tch solo il primo e non considera gli altri
			    for (tch = world[ch->in_room].people; tch; tch = next_tch) {
			      next_tch = tch->next_in_room;
			      if (IS_NPC(tch) && MOB_FLAGGED(tch, MOB_QUEST)) 
			        break;
			    }
			
			    if (FIGHTING(tch)==NULL) {
			      if (!(   IS_QUESTOR(ch) 
			            && (   (GET_TIPOQUEST(ch) == QUEST_MOBKILL) 
			                || (GET_TIPOQUEST(ch) == QUEST_GETITEM) ) 
			            && (GET_QUESTMRR(ch) == GET_MOB_VNUM(tch)) ) ) {
				        //mob non in combattimento, non e' ch il pg questor destinato
				      send_to_char("Il destino non ha riservato a te questo onore!\r\n",ch);
				      return;
			      }
			    } 
			    else if (ch && AFF_FLAGGED(ch, AFF_GROUP)) {
			      if (ch->master) 
			        k = ch->master;
			      else 
			        k = ch;
					    												    
			      //mob in combattimento, e' contro qualcuno del gruppo cui appartiene ch?
			      //se non ci sono errori dovrebbe combattere contro il pg cui e' destinato
			      if (   IS_QUESTOR(k) 
			          && (   (GET_TIPOQUEST(k) == QUEST_MOBKILL) 
			              || (GET_TIPOQUEST(k) == QUEST_GETITEM) ) 
			          && (GET_QUESTMRR(k) == GET_MOB_VNUM(tch)) )
				      target_questmob_group = TRUE;
	    		  if (!target_questmob_group) {
				      for (f = k->followers; f; f = f->next) {
				        if (   IS_QUESTOR(f->follower) 
				            && (   (GET_TIPOQUEST(f->follower) == QUEST_MOBKILL) 
				                || (GET_TIPOQUEST(f->follower) == QUEST_GETITEM) ) 
				            && (GET_QUESTMRR(f->follower) == GET_MOB_VNUM(tch)) ) {
					        target_questmob_group = TRUE;
					        break;
				        }
				      }
			      }

			      //mob in combattimento, ma contro qualcuno che non appartiene al gruppo di cui fa parte ch
			      //se non ci sono errori dovrebbe combattere contro il pg cui e' destinato
			      if (!target_questmob_group) {
				      send_to_char("Non appartieni alla compagnia predestinata a farlo.\r\n",ch);
				      return;
			      }
			    } 
			    else { 
			    	//mob in combattimento, ch non gruppato
			      //se non ci sono errori dovrebbe combattere contro il pg cui e' destinato
			      send_to_char("La questione non sembra riguardarti!\r\n",ch);
			      return;
			    }
			  } 
			  else { 
				  //i target sono solo caster ed eventualmente immos, pg o mob charmati
			    send_to_char("Gli Dei impediscono alle tue parole di nuocere a bersagli inermi\r\n",ch);
			    return;
		    }
		  }
		  tch = NULL;
	  }
  }
	/* Orione Fine */
	    
	num = 1;
	lev = 0;
	
	if (ch && AFF_FLAGGED(ch, AFF_GROUP) && AFF_FLAGGED(ch, AFF_CONGIUNZIONE)) {
		if (ch->master)
			k = ch->master;
		else
			k = ch;	  
		lev = GET_LEVEL(k);
		for (f = k->followers; f; f = f->next)
		if (   AFF_FLAGGED(f->follower, AFF_GROUP) 
		    && AFF_FLAGGED(f->follower, AFF_CONGIUNZIONE) 
		    && (f->follower->in_room == ch->in_room) 
		    && GET_CLASS(f->follower) == CLASS_MAGIC_USER ) {
			lev += GET_LEVEL(f->follower);
			num ++;
		}	  
	}
	
	mana = mag_manacost(ch, spellnum);
	mana += check_mana_mod(ch, mana, ITEM_ARMOR);
	
	if (num > 1) {
		med = (int) ((lev / num) / 10);  
		med = MIN(num, med);
		mana -= (int) (mana * (med * 0.1));
	}	
	
	if ((mana > 0) && (GET_MANA(ch) < mana) && (GET_LEVEL(ch) < LVL_QUEST_MASTER)) {
		mana = GET_MANA(ch);
		send_to_char("La tua magia non e' sufficiente per questo incantesimo.\r\n", ch);
		send_to_char("Disperdi la tua magia inutilmente!\r\n", ch);
		alter_mana(ch, mana);
		return;
	}
	
	//PEPPE COMB
	if (!IS_NPC(ch))
	if (GET_POS(ch) == POS_STANDING)
		GET_DURATA_COMB(ch)=0;
	
	if (!test_and_improve(ch, spellnum, 100, 15, 0)) {
		WAIT_STATE(ch, PULSE_VIOLENCE);
		if (!tch || !skill_message(0, ch, tch, spellnum))
			send_to_char("Perdi la tua concentrazione!\r\n", ch);
		if (mana > 0)
			alter_mana(ch, mana/2);
		if (SINFO.violent && tch && IS_NPC(tch))
			hit(tch, ch, TYPE_UNDEFINED);
	}
	else { /* cast spell returns 1 on success; subtract mana & set waitstate.
		  Se è richiesto di calcolare il mana prima della spell, indistintamente
		  dal successo della stessa, lo fa.					 */
		if (mana > 0 && IS_SET (SINFO.routines, MAG_PRE_COSTO))
			alter_mana(ch, mana);
		if (cast_spell(ch, tch, tobj, spellnum)) {
			WAIT_STATE(ch, PULSE_VIOLENCE);
			if (mana > 0 && !IS_SET (SINFO.routines, MAG_PRE_COSTO))
				alter_mana(ch, mana);
		}
	}
}



void spell_level(int spell, int class, int level)
{
	char buf[256];
	int bad = 0;
	
	if (spell < 0 || spell > TOP_SPELL_DEFINE) {
		sprintf(buf, "SYSERR: attempting assign to illegal spellnum %d", spell);
		log(buf);
		return;
	}
	
	if (class < 0 || class >= NUM_CLASSES) {
		sprintf(buf, "SYSERR: assigning '%s' to illegal class %d",
			skill_name(spell), class);
		log(buf);
		bad = 1;
	}
	
	if (level < 1 || level > LVL_IMPL) {
		sprintf(buf, "SYSERR: assigning '%s' to illegal level %d",
			skill_name(spell), level);
		log(buf);
		bad = 1;
	}
	
	if (!bad)
		spell_info[spell].min_level[class] = level;
}

//Per intenderci:-1 in abil_info[abil].class_users[i] significa che
//quella classe sta abilita non potra MAI praccarla
//0 significa che puo praccarla
void give_abilita(int abilita,int class,bool at_start)
{
	char buf[256];
	
	
	if (abilita < 0 || abilita > TOP_ABILITA_DEFINE) {
		sprintf(buf, "SYSERR: attempting assign to illegal abilnum %d", abilita);
		log(buf);
		return;
	}
	
	
	if (class < 0 || class >= NUM_CLASSES) {
		sprintf(buf, "SYSERR: assigning '%s' to illegal class %d",
			abil_name(abilita), class);
		log(buf);
		return;
	}
	
	abil_info[abilita].class_users[class]=1;
	abil_info[abilita].start_abilita[class]=at_start;
}


void unused_abil(int abil)
{
	int i;
	for (i = 0; i < NUM_CLASSES; i++)
		abil_info[abil].class_users[i] =-1;
	abil_info[abil].start_abilita[i]=FALSE;
}



/* Assign the spells on boot up */
void spello(int spl, int max_mana, int min_mana, int mana_change, int minpos,
	int targets, int violent, int routines)
{
	int i;
	
	for (i = 0; i < NUM_CLASSES; i++)
		spell_info[spl].min_level[i] = LVL_IMMORT;
	spell_info[spl].mana_max = max_mana;
	spell_info[spl].mana_min = min_mana;
	spell_info[spl].mana_change = mana_change;
	spell_info[spl].min_position = minpos;
	spell_info[spl].targets = targets;
	spell_info[spl].violent = violent;
	spell_info[spl].routines = routines;
}


void unused_spell(int spl)
{
	int i;
	
	for (i = 0; i < NUM_CLASSES; i++)
		spell_info[spl].min_level[i] = LVL_IMPL + 1;
	spell_info[spl].mana_max = 0;
	spell_info[spl].mana_min = 0;
	spell_info[spl].mana_change = 0;
	spell_info[spl].min_position = 0;
	spell_info[spl].targets = 0;
	spell_info[spl].violent = 0;
	spell_info[spl].routines = 0;
}

#define skillo(skill) spello(skill, 0, 0, 0, 0, 0, 0, 0);


/*
 * Arguments for spello calls:
 *
 * spellnum, maxmana, minmana, manachng, minpos, targets, violent?, routines.
 *
 * spellnum:  Number of the spell.  Usually the symbolic name as defined in
 * spells.h (such as SPELL_HEAL).
 *
 * maxmana :  The maximum mana this spell will take (i.e., the mana it
 * will take when the player first gets the spell).
 *
 * minmana :  The minimum mana this spell will take, no matter how high
 * level the caster is.
 *
 * manachng:  The change in mana for the spell from level to level.  This
 * number should be positive, but represents the reduction in mana cost as
 * the caster's level increases.TA
 *
 * minpos  :  Minimum position the caster must be in for the spell to work
 * (usually fighting or standing). targets :  A "list" of the valid targets
 * for the spell, joined with bitwise OR ('|').
 *
 * violent :  TRUE or FALSE, depending on if this is considered a violent
 * spell and should not be cast in PEACEFUL rooms or on yourself.  Should be
 * set on any spell that inflicts damage, is considered aggressive (i.e.
 * charm, curse), or is otherwise nasty.
 *
 * routines:  A list of magic routines which are associated with this spell
 * if the spell uses spell templates.  Also joined with bitwise OR ('|').
 *
 * See the CircleMUD documentation for a more detailed description of these
 * fields.
 */

/*
 * NOTE: SPELL LEVELS ARE NO LONGER ASSIGNED HERE AS OF Circle 3.0 bpl9.
 * In order to make this cleaner, as well as to make adding new classes
 * much easier, spell levels are now assigned in class.c.  You only need
 * a spello() call to define a new spell; to decide who gets to use a spell
 * or skill, look in class.c.  -JE 5 Feb 1996
 */

void mag_assign_spells(void)
{
	int i;
	
  /* Do not change the loop below */
	for (i = 1; i <= TOP_SPELL_DEFINE; i++)
		unused_spell(i);
  /* Do not change the loop above */
	
	spello(SPELL_ARMOR, 30, 20, 2, POS_FIGHTING,
		TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_BLESS, 40, 30, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_OBJ_INV, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);
	
	spello(SPELL_BLINDNESS, 70, 50, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_NOT_SELF, FALSE, MAG_AFFECTS);
	
	spello(SPELL_BURNING_HANDS, 20, 8, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE | MAG_MANUAL);
	
	spello(SPELL_CALL_LIGHTNING, 85, 58, 5, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_CHARM,70, 50, 2, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_NOT_SELF | TAR_NOT_WRITE, TRUE, MAG_MANUAL);
	
	spello(SPELL_WIZEYE, 40, 25, 4,  POS_STANDING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_CHILL_TOUCH, 25, 10, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE | MAG_MANUAL);
	
	spello(SPELL_CLONE, 80, 65, 5, POS_STANDING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_MANUAL);
	
	spello(SPELL_COLOR_SPRAY, 100, 70, 3, POS_FIGHTING,
		TAR_IGNORE, TRUE, MAG_AREAS);
	
	spello(SPELL_CONTROL_WEATHER, 55, 35, 5, POS_STANDING,
		TAR_IGNORE, FALSE, MAG_MANUAL);
	
	spello(SPELL_CREATE_FOOD, 25, 5, 4, POS_STANDING,
		TAR_IGNORE, FALSE, MAG_CREATIONS);
	
	spello(SPELL_CREATE_LIGHT, 25, 10, 4, POS_STANDING,
		TAR_IGNORE, FALSE, MAG_CREATIONS);
	
	spello(SPELL_CONTINUAL_LIGHT, 65, 40, 5, POS_STANDING,
		TAR_IGNORE, FALSE, MAG_CREATIONS);
	
	spello(SPELL_CREATE_WATER, 30, 10, 2, POS_STANDING,
		TAR_OBJ_INV | TAR_OBJ_EQUIP, FALSE, MAG_MANUAL);
	
	spello(SPELL_LOCATE_TARGET, 60, 40, 5, POS_STANDING,
		TAR_CHAR_WORLD | TAR_NOT_WRITE, FALSE, MAG_MANUAL);
	
	spello(SPELL_CURE_BLIND, 60, 30, 2, POS_STANDING,
		TAR_CHAR_ROOM, FALSE, MAG_UNAFFECTS);
	
	spello(SPELL_CURE_CRITIC, 60, 30, 5, POS_FIGHTING,
		TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_PRE_COSTO);
	
	spello(SPELL_CURE_LIGHT, 20, 10, 3, POS_FIGHTING,
		TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_PRE_COSTO);
	
	spello(SPELL_CURSE, 50, 30, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_OBJ_INV, TRUE, MAG_AFFECTS | MAG_ALTER_OBJS);
	
	spello(SPELL_DETECT_ALIGN, 50, 30, 2, POS_STANDING,
		TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_DETECT_INVIS, 55, 25, 3, POS_STANDING,
		TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_DETECT_MAGIC, 55, 25, 3, POS_STANDING,
		TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_DETECT_POISON, 35, 25, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_MANUAL);
	
	spello(SPELL_DISINTEGRATE, 250, 170, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_FIRESHD, 200, 150, 5, POS_RESTING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_DISPEL_EVIL, 50, 30, 2, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_DISPEL_GOOD, 50, 30, 2, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_EARTHQUAKE, 75, 45, 4, POS_FIGHTING,
		TAR_IGNORE, TRUE, MAG_AREAS);
	
	spello(SPELL_FEAR, 30, 18, 2, POS_FIGHTING,
		TAR_IGNORE, TRUE, MAG_MANUAL);
	
	spello(SPELL_ENCHANT_WEAPON, 100, 80, 10, POS_STANDING,
		TAR_OBJ_INV | TAR_OBJ_EQUIP, FALSE, MAG_MANUAL);
	
	spello(SPELL_ENERGY_DRAIN, 120, 90, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE | MAG_MANUAL);
	
	spello(SPELL_GROUP_ARMOR, 70, 50, 4, POS_STANDING,
		TAR_IGNORE, FALSE, MAG_GROUPS);
	
	spello(SPELL_FIREBALL, 90, 70, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_GROUP_HEAL, 100, 60, 2, POS_STANDING,
		TAR_NOT_WRITE | TAR_IGNORE, FALSE, MAG_GROUPS | MAG_PRE_COSTO);
	
	spello(SPELL_HARM, 65, 40, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_HEAL, 100, 30, 10, POS_FIGHTING,
		TAR_NOT_WRITE | TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_AFFECTS | MAG_UNAFFECTS | MAG_PRE_COSTO);
	
	spello(SPELL_INFRAVISION, 50, 30, 2, POS_STANDING,
		TAR_CHAR_ROOM,  FALSE, MAG_AFFECTS);
	
	spello(SPELL_INVISIBLE, 50, 35, 3, POS_STANDING,
		TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);
	
	spello(SPELL_LIGHTNING_BOLT, 24, 15, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_LOCATE_OBJECT, 75, 50, 3, POS_STANDING,
		TAR_OBJ_WORLD  | TAR_NOT_WRITE, FALSE, MAG_MANUAL);
	
	spello(SPELL_MAGIC_MISSILE, 19, 10, 3, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_POISON, 60, 46, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_NOT_SELF | TAR_OBJ_INV, TRUE, MAG_AFFECTS | MAG_ALTER_OBJS);
	
	spello(SPELL_RELOCATE, 100, 70, 3, POS_STANDING, TAR_CHAR_WORLD | TAR_NOT_SELF, FALSE, MAG_MANUAL);
	
	spello(SPELL_PROT_FROM_EVIL, 80, 60, 3, POS_STANDING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_REMOVE_CURSE, 40, 30, 1, POS_STANDING,
		TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP, FALSE, MAG_MANUAL);
	
	spello(SPELL_DISPEL_MAGIC, 250, 200, 1, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT  | TAR_NOT_WRITE, FALSE, MAG_MANUAL);
	
	spello(SPELL_SANCTUARY, 150, 120, 2, POS_STANDING,
		TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_SHOCKING_GRASP, 50, 30, 2, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_PARALIZE, 110, 90, 2, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AFFECTS);
	
	spello(SPELL_SLEEP, 50, 35, 3, POS_STANDING,
		TAR_CHAR_ROOM, TRUE, MAG_AFFECTS);
	
	spello(SPELL_STRENGTH, 30, 15, 3, POS_STANDING,
		TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_SUMMON, 200, 170, 3, POS_STANDING,
		TAR_CHAR_WORLD | TAR_NOT_SELF  | TAR_NOT_WRITE, FALSE, MAG_MANUAL);
	
	spello(SPELL_TELEPORT, 150, 100, 5, POS_STANDING,
		TAR_CHAR_WORLD | TAR_NOT_SELF, FALSE, MAG_MANUAL);
	
	spello(SPELL_WORD_OF_RECALL, 200, 150, 4, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_MANUAL);
	
	spello(SPELL_REMOVE_POISON, 40, 20, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_UNAFFECTS | MAG_ALTER_OBJS);
	
	spello(SPELL_SENSE_LIFE, 45, 30, 3, POS_STANDING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_FLY, 65, 45, 2, POS_STANDING,
		TAR_CHAR_ROOM | TAR_SELF_ONLY | TAR_NOT_WRITE, FALSE, MAG_AFFECTS);
	
	spello(SPELL_MINUTE_METEOR, 130, 105, 3, POS_FIGHTING, 
		TAR_IGNORE, TRUE, MAG_AREAS);
	
	spello(SPELL_AREA_LIGHTNING, 85, 55, 5,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AREAS);
	
	spello(SPELL_CONE_OF_COLD, 40, 30, 2,
		POS_FIGHTING, TAR_IGNORE, TRUE, MAG_AREAS);
	
	spello(SPELL_FIRE_BREATH, 100, 70, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AREAS);
	
	spello(SPELL_GAS_BREATH, 100, 70, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AREAS);
	
	spello(SPELL_FROST_BREATH, 100, 70, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AREAS);
	
	spello(SPELL_ACID_BREATH, 100, 70, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AREAS);
	
	spello(SPELL_LIGHTNING_BREATH, 100, 70, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AREAS);
	
	spello(SPELL_BLADEBARRIER, 75, 55, 3,
		POS_FIGHTING, TAR_IGNORE, TRUE, MAG_AREAS);
	
	spello(SPELL_PEACE, 70, 40, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_MANUAL);
	
	spello(SPELL_PIDENTIFY, 75 , 60, 3,
		POS_STANDING, TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_MANUAL);  // Rimosso TAR_CHAR_ROOM
	
	spello(SPELL_LEVITATE, 50, 30, 2,
		POS_STANDING, TAR_CHAR_ROOM | TAR_SELF_ONLY | TAR_NOT_WRITE, FALSE, MAG_AFFECTS);
	
	spello(SPELL_PROT_FIRE, 70, 50, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_WATERWALK, 50, 30, 2,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_WATERBREATH, 55, 35, 2,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_GROUP_FLY, 80, 60, 4,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_GROUPS);
	
	spello(SPELL_GROUP_INVIS, 80, 60, 4,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_GROUPS);
	
	spello(SPELL_GROUP_PROT_EVIL, 80, 60, 3,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_GROUPS);
	
	spello(SPELL_ACID_ARROW, 25, 15, 2,
		POS_STANDING, TAR_CHAR_ROOM, TRUE, MAG_DAMAGE);
	
	spello(SPELL_FLAME_ARROW, 80, 53, 3,
		POS_STANDING, TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);
	
	spello(SPELL_CURE_SERIOUS, 40, 20, 4,
		POS_FIGHTING, TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_PRE_COSTO);
	
	spello(SPELL_GROUP_WATBREATH, 75, 45, 3,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_GROUPS);
	
	spello(SPELL_STONESKIN, 60, 40,  2,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_BARKSKIN, 40, 30,  2,
		POS_STANDING, TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_CONJ_ELEMENTAL, 90, 60, 2,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_SUMMONS);
	
	spello(SPELL_MONSUM_I, 70, 45, 2,
		POS_STANDING, TAR_OBJ_ROOM, FALSE, MAG_SUMMONS);
	
	spello(SPELL_MONSUM_II, 100, 75, 2,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_SUMMONS);
	
	spello(SPELL_MONSUM_III, 150, 100, 2,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_SUMMONS);
	
	spello(SPELL_MONSUM_IV, 200, 180, 2,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_SUMMONS);
	
	spello(SPELL_MONSUM_V, 240, 200, 5,
		POS_STANDING, TAR_IGNORE, FALSE, MAG_SUMMONS);
	
	spello(SPELL_SHIELD,  20, 10, 2,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);
	
	spello(SPELL_MIRROR_IMAGE, 150, 120, 3,
		POS_STANDING, TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
	
	spello(SPELL_BLINK,  60, 45, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_SELF_ONLY | TAR_NOT_WRITE, FALSE, MAG_AFFECTS);
	
	spello(SPELL_DEATHDANCE,  160, 130, 3,
		POS_FIGHTING, TAR_CHAR_ROOM | TAR_SELF_ONLY | TAR_NOT_WRITE, FALSE, MAG_AFFECTS | MAG_POINTS);
	
	spello(SPELL_PORTAL, 100, 50, 5,
		POS_STANDING, TAR_CHAR_WORLD | TAR_NOT_SELF, FALSE, MAG_MANUAL);
	
	spello(SPELL_MATER_ARMOUR, 80, 30, 5,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_CREATIONS);
	
	spello(SPELL_MATER_WEAP, 80, 30, 5,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_CREATIONS);
	
	spello(SPELL_FEAST, 50, 30, 2,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_MANUAL);
	
	spello(SPELL_HEROES_FEAST, 100, 60, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_GROUPS);
	
	spello(SPELL_REFRESH, 45, 30, 3,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_MANUAL);
	
	spello(SPELL_WRAITHFORM, 100, 80, 2,
		POS_STANDING, TAR_CHAR_ROOM | TAR_SELF_ONLY  | TAR_NOT_WRITE, FALSE, MAG_AFFECTS);
	
	spello(SPELL_CALL_DRAGON, 75, 50, 3, POS_STANDING, TAR_IGNORE, FALSE, MAG_MANUAL);
	
	spello(SPELL_PROTECT_LIGHT, 70, 30, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);

	spello(SPELL_DET_SNEAK, 60, 30, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);

	spello(SPELL_SILENCE, 70, 30, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);

	spello(SPELL_ANTIMAGIC, 100, 50, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);

	spello(SPELL_DISGUISE, 250, 230, 2,
		POS_STANDING, TAR_CHAR_ROOM | TAR_NOT_WRITE, FALSE, MAG_MANUAL);

	spello(SPELL_AID, 70, 30, 4,
		POS_STANDING, TAR_CHAR_ROOM, FALSE, MAG_AFFECTS);

	spello(SPELL_CONGIUNZIONE, 70, 30, 4,
		POS_STANDING, TAR_CHAR_ROOM | TAR_SELF_ONLY  | TAR_NOT_WRITE, FALSE, MAG_AFFECTS);

	spello(SPELL_ENCHANT_WOOD, 60, 20, 2,
		POS_STANDING, TAR_OBJ_INV | TAR_NOT_WRITE, FALSE, MAG_MANUAL);
	
  /* NON-castable spells should appear here */
	spello(SPELL_IDENTIFY, 0, 0, 0, 0,
		TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_MANUAL); // Rimosso TAR_CHAR_ROOM
	
//PEPPE_AMANU
	spello(SPELL_ENDURANCE,  70, 30, 4,
		POS_STANDING, TAR_CHAR_ROOM | TAR_SELF_ONLY | TAR_NOT_WRITE, FALSE, MAG_AFFECTS);

	spello(SPELL_TIME_DISTORTION, 120, 80, 5, POS_FIGHTING,
		TAR_CHAR_ROOM | TAR_NOT_SELF, FALSE, MAG_MANUAL);
	
 /*
   * Declaration of skills - this actually doesn't do anything except
   * set it up so that immortals can use these skills by default.  The
   * min level to use the skill for other classes is set up in class.c.
   */
	
	skillo(SKILL_BACKSTAB);
	skillo(SKILL_BASH);
	skillo(SKILL_HIDE);
	skillo(SKILL_KICK);
	skillo(SKILL_PICK_LOCK);
	skillo(SKILL_SPOT);
	skillo(SKILL_RESCUE);
	skillo(SKILL_SNEAK);
	skillo(SKILL_STEAL);
	skillo(SKILL_TRACK);
	skillo(SKILL_DISARM);
	skillo(SKILL_SECOND_ATTACK);
	skillo(SKILL_THIRD_ATTACK);
	skillo(SKILL_ENH_DAMAGE);
	skillo(SKILL_PARRY);
	skillo(SKILL_DODGE);
	skillo(SKILL_BREW);
	skillo(SKILL_FORGE);
	skillo(SKILL_SCRIBE);
	skillo(SKILL_COOK);
	skillo(SKILL_SCAN);
	skillo(SKILL_FILLET);
	skillo(SKILL_REPAIR);
	skillo(SKILL_FOURTH_ATTACK);
	skillo(SKILL_FIFTH_ATTACK);
	skillo(SKILL_SPY);
	skillo(SKILL_RETREAT);
	skillo(SKILL_BOW);
	skillo(SKILL_SLING);
	skillo(SKILL_CROSSBOW);
	skillo(SKILL_THROW);
	skillo(SKILL_COLPO_MORTALE);
}
