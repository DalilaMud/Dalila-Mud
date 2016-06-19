/**************************************************************************
*   File: magic.c           35                          Part of CircleMUD *
*  Usage: low-level functions for magic; spell template code              *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
**************************************************************************/


#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "spells.h"
#include "handler.h"
#include "db.h"
#include "dg_scripts.h"

#define MAX_SPELL_AFFECTS 5	/* change if more needed */

extern struct room_data *world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct index_data *obj_index;
extern int newbye(struct char_data *ch);
extern int limite_follower(struct char_data *ch);
extern struct descriptor_data *descriptor_list;
extern struct zone_data *zone_table;

extern int mini_mud;
extern int pk_allowed;

extern struct default_mobile_stats *mob_defaults;
extern char weapon_verbs[];
extern int *max_ac_applys;
extern struct apply_mod_defaults *apmd;
extern int arena_ok(struct char_data * ch, struct char_data * victim);

void clearMemory(struct char_data * ch);
void act(char *str, int i, struct char_data * c, struct obj_data * o, void *vict_obj, int j);

void damage(struct char_data * ch, struct char_data * victim, int damage, int weapontype);
void damage_nuovo(struct char_data *ch, struct char_data *victim, int dam, int attacktype, bool maxato, int tipo);

void weight_change_object(struct obj_data * obj, int weight);
void add_follower(struct char_data * ch, struct char_data * leader);
int dice(int number, int size);
void perform_group_heal (int level, struct char_data *ch, int savetype, int casttype);
int char_affected_by_obj (struct char_data *ch, long long int affect, int bitv);

extern struct spell_info_type spell_info[];


struct char_data *read_mobile(int, int);

/*
 * Saving throws for:
 * MCTW
 *   PARA, ROD, PETRI, BREATH, SPELL
 *     Levels 0-40
 */


/*****+*********************************************************\
| By Spini:							|
| Modificati i saving throw. Saving_para e' applicato a tutte	|
| le spell "definitive", come appunto paralize, entra spesso	|
| ed e' influenzato solo dal liv e dalla gilda di chi effettua	|
| il tiro salvezza. Saving_spell e' influenzato dal liv e, se	|
| specificato durante la chiamata, anche da un'abilita' del pg	|
| che effettua il ts, oltre che dalla sua gilda. Saving_rod e'	|
| influenzato dal liv e dalle classi sia di chi effettua il ts	|
| sia di chi lancia la spell (che potrebbe essere eventualmente |
| la stessa persona). Se ne deduce che chiamando questa		|
| funzione per un ts che non sia saving_rod si puo' passare	|
| NULL come secondo parametro, e chiamando questa funzione per	|
| un ts che non sia saving_spell si puo' passare qualsiasi	|
| valore ad abil.						|
\***************************************************************/
int mag_savingthrow(struct char_data * ch, struct char_data * caster, int type, int abil, int modifier)
{
	int save = 1000;
	int classe, classe_caster;

	if (newbye(ch))
		return TRUE;
	if (!(ch))
		return FALSE;
	if (type==SAVING_NEVER)
		return FALSE;
	if (IS_NPC (ch))
		classe = CLASS_WARRIOR;
	else
		classe = GET_CLASS(ch);

	if (type == SAVING_PARA){
		switch (classe){
			case CLASS_MAGIC_USER:
			case CLASS_THIEF:
				save = 35 - (GET_LEVEL (ch) * 3 / 7);
				break;
			default:
				save = 39 - (GET_LEVEL (ch) * 3 / 7);
				break;
		}
	}

	else if (type == SAVING_SPELL){
		switch (classe){
			case CLASS_MAGIC_USER:
				save = 20;
				break;
			case CLASS_CLERIC:
				save = 25;
				break;
			case CLASS_THIEF:
				save = 30;
				break;
			case CLASS_PELOI:
				save = 40;
				break;
			default:
				save = 50;
				break;
		}
		save += 30 - (GET_LEVEL(ch) * 3 / 7);
		switch (abil){
			case SAVING_STR:
				save -= (GET_STR(ch) - 3) * 2 / 3;
				break;
			case SAVING_CON:
				save -= (GET_CON(ch) - 3) * 2 / 3;
				break;
			case SAVING_DEX:
				save -= (GET_DEX(ch) - 3) * 2 / 3;
				break;
			case SAVING_INT:
				save -= (GET_INT(ch) - 3) * 2 / 3;
				break;
			case SAVING_WIS:
				save -= (GET_WIS(ch) - 3) * 2 / 3;
				break;
			case SAVING_CHA:
				save -= (GET_CHA(ch) - 3) * 2 / 3;
				break;
			default:
				break;
		}
	}

	else if (type == SAVING_ROD){
		if (!(caster))
			return TRUE;
		switch (classe){
			case CLASS_MAGIC_USER:
				save = 15;
				break;
			case CLASS_CLERIC:
				save = 21;
				break;
			case CLASS_THIEF:
				save = 28;
				break;
			case CLASS_PELOI:
				save = 39;
				break;
			default:
				save = 50;
				break;
		}
		save += 30 - (GET_LEVEL(ch) * 3 / 7);

		if (IS_NPC (caster))
			classe_caster = CLASS_WARRIOR;
		else
			classe_caster = GET_CLASS(caster);

		switch (classe_caster){
			case CLASS_CLERIC:
				save -= 1;
				break;
			case CLASS_THIEF:
				save -= 3;
				break;
			case CLASS_PELOI:
				save -= 4;
				break;
			default:
				save -= 5;
				break;
		}
	}

	save += GET_SAVE(ch, type);
	save += modifier;

	if (save > 100){
		sprintf (buf, "SYSERR: il char %s ha come percentuale di mancato ts %d!", GET_NAME (ch), save);
		mudlog (buf, BRF, LVL_QUEST_MASTER, TRUE);
	}

	if (MAX(1, save) < number(0, 99))
		return TRUE;
	else
		return FALSE;
}


/* affect_update: called from comm.c (causes spells to wear off) */
void affect_update(void)
{
	static struct affected_type *af, *next;
	static struct char_data *i;
	extern char *spell_wear_off_msg[];
	extern room_rnum r_mortal_start_room[NUM_STARTROOMS +1];
	
	for (i = character_list; i; i = i->next) {
		//PEPPE RELIGIONE
		if (PLR_FLAGGED(i,PLR_FANTASMA)) {
		  if (GET_RESUSCITA(i)==1) {
			  send_to_char("Un vortice di calda e intensa luce ti avvolge.\r\n",i);
			  REMOVE_BIT(PLR_FLAGS(i),PLR_FANTASMA);
			  REMOVE_BIT(PRF_FLAGS(i),PRF_NOHASSLE);
			  REMOVE_BIT(PRF_FLAGS(i),PRF_HOLYLIGHT);
			  
			  char_from_room(i);
	      char_to_room(i, r_mortal_start_room[GET_HOME(i)]);

			  GET_RESUSCITA(i)=0;
			 	act("!!SOUND(Recall.wav)Il corpo di $n si materializza avvolto in un turbine di calda luce.", TRUE, i, 0, 0, TO_ROOM);
			}
		  else if (GET_RESUSCITA(i)==2) {
			  send_to_char("Tra un ora il processo di resurrezione sara' pronto\r\n",i);
			  GET_RESUSCITA(i)--;
			}
		  else if(GET_RESUSCITA(i)>2) {
			  GET_RESUSCITA(i)--;
		  }
		}    
		
		for (af = i->affected; af; af = next) {
			next = af->next;
			if (af->duration >= 1)
				af->duration--;
			else if (af->duration < 0)	/* No action */
				af->duration = -1;	/* GODs only! unlimited */
			else {
				if ((af->natura==SPELLSKILL) && (af->type > 0) && (af->type <= MAX_SPELLS))
					if (!af->next || (af->next->natura != af->natura) ||
						(af->next->type != af->type) || (af->next->duration > 0))
						if (*spell_wear_off_msg[af->type]) {
							send_to_char(spell_wear_off_msg[af->type], i);
							send_to_char("\r\n", i);
						}
				//By Ardanos / Spini: i pg tramortiti non si riprendono mai...
				if (IS_SET (af[0].bitvector[0], AFF_TRAMORTITO))
					GET_POS(i) = POS_STANDING;
				affect_remove(i, af);
			}
		}
  }
}

/*
 *  mag_materials:
 *  Checks for up to 3 vnums (spell reagents) in the player's inventory.
 *
 * No spells implemented in Circle 3.0 use mag_materials, but you can use
 * it to implement your own spells which require ingredients (i.e., some
 * heal spell which requires a rare herb or some such.)
 */
int mag_materials(struct char_data * ch, int item0, int item1, int item2,
	int extract, int verbose)
{
	struct obj_data *tobj;
	struct obj_data *obj0 = NULL, *obj1 = NULL, *obj2 = NULL;
	
	for (tobj = ch->carrying; tobj; tobj = tobj->next_content) {
		if ((item0 > 0) && (GET_OBJ_VNUM(tobj) == item0)) {
			obj0 = tobj;
			item0 = -1;
		} else if ((item1 > 0) && (GET_OBJ_VNUM(tobj) == item1)) {
			obj1 = tobj;
			item1 = -1;
		} else if ((item2 > 0) && (GET_OBJ_VNUM(tobj) == item2)) {
			obj2 = tobj;
			item2 = -1;
		}
	}
	if ((item0 > 0) || (item1 > 0) || (item2 > 0)) {
		if (verbose) {
			switch (number(0, 2)) {
				case 0:
					send_to_char("L'incantesimo non riesce. Usa gli ingredienti giusti!\r\n", ch);
					break;
				case 1:
					send_to_char("Hmmm... Non sembra che tu abbia usato gli ingredienti giusti.\r\n", ch);
					break;
				case 2:
					send_to_char("Hmmm... Sei sicuro di saper fare a fare cio'?\r\n", ch);
					break;
			}
		}
		return (FALSE);
	}
	if (extract) {
		if (item0 < 0) {
			obj_from_char(obj0);
			extract_obj(obj0);
		}
		if (item1 < 0) {
			obj_from_char(obj1);
			extract_obj(obj1);
		}
		if (item2 < 0) {
			obj_from_char(obj2);
			extract_obj(obj2);
		}
	}
	if (verbose) {
		send_to_char("Una nuvoletta di fumo scaturisce dalle tue mani.\r\n", ch);
		act("Una nuvoletta di fumo scaturisce dalle mani di $n.", TRUE, ch, NULL, NULL, TO_ROOM);
	}
	return (TRUE);
}




/*
 * Every spell that does damage comes through here.  This calculates the
 * amount of damage, adds in any modifiers, determines what the saves are,
 * tests for save and calls damage().
 */

extern float no_pandion_damage_modifier;
#define NO_PANDION_DAMAGE(dam) (IS_MAGIC_USER(ch) ? (dam) : ((int) ((float) (dam) * no_pandion_damage_modifier)))

void mag_damage(int level, struct char_data * ch, struct char_data * victim,
	int spellnum, int savetype)
{
	int is_mage = 0, is_cleric = 0;
	int dam = 0;
	int abil = SAVING_NONE;
	int maxato = TRUE, attacktype = DANNO_FISICO;
	int caso;
	int casi_totali = 0;
	int i;
	struct affected_type af[MAX_SPELL_AFFECTS];
	char *posizione = NULL;

	for (i = 0; i < MAX_SPELL_AFFECTS; i++) {
		af[i].natura = SPELLSKILL;
		af[i].type = spellnum;
		af[i].bitvector[0] = 0;
		af[i].bitvector[3] = 0;
		af[i].bitvector[1] = 0;
		af[i].bitvector[2] = 0;
		af[i].modifier = 0;
		af[i].location = APPLY_NONE;
	}
	
	if (victim == NULL || ch == NULL)
		return;
	
	if (newbye(ch)&&!IS_NPC(victim)) {
		send_to_char("Senti un dolore lancinante alla testa.\r\n",ch);
		return;
	} else if (newbye(victim)) {
		send_to_char("Senti un dolore lancinate alla testa quando guardi il Newbye.\r\n",ch);
		return;
	}

	if (!(AFFECTED(victim, AFF_FLYING, BITV_FLYING) || AFFECTED(victim, AFF_LEVITATE, BITV_LEVITATE))){
		if (AFFECTED(ch, AFF_FLYING, BITV_FLYING) || AFFECTED (ch, AFF_LEVITATE, BITV_LEVITATE)){
			if (char_affected_by_obj (ch, AFF_FLYING, BITV_FLYING) || char_affected_by_obj (ch, AFF_LEVITATE, BITV_LEVITATE)){
				send_to_char ("Gli oggetti che indossi prosciugano le tue forze per mantenerti in volo. Non puoi certo pensare di concentrarle sugli altri!\r\n", ch);
				return;
			}
			send_to_char ("Sei costretto ad atterrare per colpirlo.\r\n", ch);
			if (affected_by_spell(ch, SPELLSKILL, SPELL_FLY))
				affect_from_char(ch, SPELLSKILL, SPELL_FLY);
			if (affected_by_spell(ch, SPELLSKILL, SPELL_LEVITATE))
				affect_from_char(ch, SPELLSKILL, SPELL_LEVITATE);
		}
	}
	
	is_mage = (GET_CLASS(ch) == CLASS_MAGIC_USER);
	is_cleric = (GET_CLASS(ch) == CLASS_CLERIC);
	
	switch (spellnum) {
    /* Mostly mages */
		case SPELL_MAGIC_MISSILE:
			dam = dice(1, GET_LEVEL(ch)) + 3;
			dam = NO_PANDION_DAMAGE(dam);
			abil = SAVING_DEX;
			if (AFF_FLAGGED(victim, AFF_SHIELD))
			{
				send_to_char("&6La tua barriera magica annulla l'incantesimo!&0\r\n", victim);
				send_to_char("Il tuo &1magic missile&0 scompare!!\r\n", ch);
				dam = 0;
			}
			break;
			
		case SPELL_CHILL_TOUCH:	/* chill touch also has an affect */
			dam = dice(2, GET_LEVEL(ch));
			dam = NO_PANDION_DAMAGE(dam);
			abil = SAVING_CON;
			attacktype = DANNO_GHIACCIO;
			break;
			
		case SPELL_BURNING_HANDS:
			dam = dice(1, GET_LEVEL(ch)) + 2;
			dam = NO_PANDION_DAMAGE(dam);
			attacktype = DANNO_FUOCO;
			break;
			
		case SPELL_SHOCKING_GRASP:
			dam = dice(GET_LEVEL(ch), 4);
			dam = NO_PANDION_DAMAGE(dam);
			attacktype = DANNO_ELETTRICITA;
			break;
			
		case SPELL_LIGHTNING_BOLT:
			dam = dice(GET_LEVEL(ch), 3);
			dam = NO_PANDION_DAMAGE(dam);
			abil = SAVING_DEX;
			if (IS_AFFECTED(ch, AFF_PROTECT_LIGHT))
			{
				dam = dam/6;
				act("&5La tua protezione dal fulmine disturba l'energia del fulmine.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			if (IS_AFFECTED(victim, AFF_PROTECT_LIGHT)) {
				dam = dam/4;
				act("&5La protezione dal fulmine di $N dissipa l'energia del tuo fulmine.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			attacktype = DANNO_ELETTRICITA;
			break;	
		case SPELL_COLOR_SPRAY:
			caso = number (1, 100);
			dam = 0;
			if (caso == 1){
				act("&5Un getto violetto parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&5Un getto violetto partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&5Un getto violetto partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				attacktype = DANNO_GHIACCIO;
				maxato = FALSE;
			}
			if (caso <= 5){
				act("&4&bUn getto color indaco parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&4&bUn getto color indaco partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&4&bUn getto color indaco partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				attacktype = DANNO_GHIACCIO;
				maxato = FALSE;
				return;
			}
			if (caso <= 12){
				act("&6&bUn getto azzurro parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&6&bUn getto azzurro partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&6&bUn getto azzurro partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				af[0].duration = 2;
				af[0].bitvector[0] = AFF_SLEEP;
			
				if (GET_POS(victim) > POS_SLEEPING) {
					act("Hai sonno... molto sonno...  Zzzz......", FALSE, victim, 0, 0, TO_CHAR);
					if (GET_SEX(ch)==SEX_MALE)
						act("$n cade addormentato.", TRUE, victim, 0, 0, TO_ROOM);
					else
						act("$n cade addormentata.", TRUE, victim, 0, 0, TO_ROOM);
					GET_POS(victim) = POS_SLEEPING;
					if(FIGHTING(ch))
						stop_fighting(ch);
					if(FIGHTING(victim))
						stop_fighting(victim);
				}
				affect_join(victim, af, FALSE, FALSE, FALSE, FALSE);
				return;
			}
			if (caso <= 20){
				act("&2&bUn getto verde parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&2&bUn getto verde partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&2&bUn getto verde partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				af[0].location = APPLY_HITROLL;
				af[0].modifier = -3;
				af[0].duration = 1;
				af[0].bitvector[0] = AFF_BLIND;
			
				af[1].location = APPLY_AC;
				af[1].modifier = 30;
				af[1].duration = 1;
				af[1].bitvector[0] = AFF_BLIND;
			
				affect_join(victim, af, FALSE, FALSE, FALSE, FALSE);
				affect_join(victim, af+1, FALSE, FALSE, FALSE, FALSE);

				if (GET_SEX(victim) == SEX_MALE){
					act("$n sembra diventato cieco!", TRUE, victim, 0, 0, TO_ROOM);
					act("Sei stato accecato!", FALSE, victim, 0, 0, TO_CHAR);
				}
				else{
					act("$n sembra diventata cieca!", TRUE, victim, 0, 0, TO_ROOM);
					act("Sei stata accecata!", FALSE, victim, 0, 0, TO_CHAR);
				}
				return;
			}
			if (caso <= 30){
				act("&3&bUn getto giallo parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&3&bUn getto giallo partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&3&bUn getto giallo partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				dam = dice (4, GET_LEVEL(ch));
				attacktype = DANNO_ACIDO;
			}
			else if (caso <= 60){
				act("&3Un getto arancio parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&3Un getto arancio partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&3Un getto arancio partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				if (!mag_savingthrow(victim, ch, savetype, SAVING_INT, 0)){
					af[0].duration = 1 + GET_LEVEL(ch)/10;
					af[0].bitvector[0] = AFF_SILENCE;
					affect_join(victim, af, FALSE, FALSE, FALSE, FALSE);
					act ("Vieni ammutolito da $n.", FALSE, ch, 0, victim, TO_VICT);
				}
				return;
			}
			else{
				act("&1&bUn getto rosso parte dalla mano di $n e ti colpisce in pieno!&0", FALSE, ch, 0, victim, TO_VICT);
				act("&1&bUn getto rosso partito dalla tua mano investe $N in pieno.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&1&bUn getto rosso partito dalla mano di $n investe $N in pieno!&0", TRUE, ch, 0, victim, TO_NOTVICT);
				dam = dice (3, GET_LEVEL(ch));
				attacktype = DANNO_FUOCO;
			}
			break;
			
		case SPELL_FIREBALL:
			dam = dice(GET_LEVEL(ch), 6);
			dam = NO_PANDION_DAMAGE(dam);
			abil = SAVING_DEX;
			if (IS_AFFECTED(ch, AFF_PROT_FIRE))
			{
				dam = dam/6;
				act("&5La tua protezione dal fuoco disturba l'energia del FIREBALL.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			
			if (IS_AFFECTED(victim, AFF_PROT_FIRE))
			{
				dam = dam/4;
				act("&5La protezione dal fuoco di $N dissipa il FIREBALL.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			attacktype = DANNO_FUOCO;
			break;
			
		case SPELL_DISINTEGRATE:
			dam = dice(GET_LEVEL(ch), 7);
			dam = NO_PANDION_DAMAGE(dam);
			break;
			
    /* Mostly clerics */
		case SPELL_DISPEL_EVIL:
			dam = dice(3, GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim));
			if (IS_EVIL(ch))
			{
				victim = ch;
				dam = GET_HIT(ch) - 1;
			}
			else if (IS_GOOD(victim))
			{
				act("$N e' protetto dagli dei.", FALSE, ch, 0, victim, TO_CHAR);
				dam = 0;
				return;
			}
			attacktype = DANNO_SILUE;
			break;
			
		case SPELL_DISPEL_GOOD:
			dam = dice(3, GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim));
			if (IS_GOOD(ch))
			{
				victim = ch;
				dam = GET_HIT(ch) - 1;
			}
			else if (IS_EVIL(victim))
			{
				act("$N e' protetto dagli dei.", FALSE, ch, 0, victim, TO_CHAR);
				dam = 0;
				return;
			}
			attacktype = DANNO_SHAARR;
			break;
			
		case SPELL_CALL_LIGHTNING:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 4);
			
			if (IS_AFFECTED(ch, AFF_PROTECT_LIGHT))
			{
				dam = dam/2;
				act("&5La tua protezione dal fulmine disturba l'energia del fulmine.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			
			if (IS_AFFECTED(victim, AFF_PROTECT_LIGHT)) {
				dam = dam/4;
				act("&5La protezione dal fulmine di $N dissipa l'energia del tuo fulmine.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			attacktype = DANNO_ELETTRICITA;
			break;
			
		case SPELL_HARM:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 1);
			abil = SAVING_INT;
			break;
			
		case SPELL_ENERGY_DRAIN:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 5);
			dam = NO_PANDION_DAMAGE(dam);
			attacktype = DANNO_XHYPHYS;
			break;
			
			
		case SPELL_ACID_ARROW:
			dam = dice(3, GET_LEVEL(ch));
			dam = NO_PANDION_DAMAGE(dam);
			abil = SAVING_DEX;
			for (i = 0; i < NUM_WEARS; i++)
				if (GET_EQ(victim, i))
					casi_totali++;
			if (casi_totali > 0){
				caso = number (0, casi_totali-1);
				for (i = 0; i <= caso; i++)
					if (!(GET_EQ(victim, i)))
						caso++;
				if (!(GET_EQ(victim, caso))){
					send_to_char ("Nimrhuin ha fatto un errore di distrazione - tirategli le orecchie! La tua spell perde pure il danno...\r\n", ch);
					return;
				}
				GET_OBJ_CSLOTS(GET_EQ(victim, caso)) -= 5;
				if (GET_OBJ_CSLOTS(GET_EQ(victim, caso)) <= 0){
					act ("&2&bLa freccia acida di $n dissolve $p&2&b. Sei fortunato di essere ancora intatto!&0", FALSE, ch, GET_EQ(victim, caso), victim, TO_VICT);
					act ("&2&bLa tua freccia acida dissolve $p&2&b sul corpo di $N.&0", FALSE, ch, GET_EQ(victim, caso), victim, TO_CHAR);
					act ("&2&bLa freccia acida di $n dissolve $p&2&b sul corpo di $N, che resta intatto per miracolo!&0", FALSE, ch, GET_EQ(victim, caso), victim, TO_NOTVICT);
					extract_obj(GET_EQ(victim, caso));
				}
				else{
					act ("&2&bLa freccia acida di $n colpisce $p&2&b e la rovina un po'.&0", FALSE, ch, GET_EQ(victim, caso), victim, TO_VICT);
					act ("&2&bLa tua freccia acida colpisce $p&2&b sul corpo di $N, rovinandola.&0", FALSE, ch, GET_EQ(victim, caso), victim, TO_CHAR);
					act ("&2&bLa freccia acida di $n colpisce $p&2&b sul corpo di $N, rovinandola un po'.&0", FALSE, ch, GET_EQ(victim, caso), victim, TO_NOTVICT);
				}
			}
			attacktype = DANNO_ACIDO;
			break;
			
		case SPELL_FLAME_ARROW:
			dam = dice(GET_LEVEL(ch), 5);
			abil = SAVING_DEX;
			attacktype = DANNO_FUOCO;
			break;
			
			
			
    /* Area spells */
		case SPELL_MINUTE_METEOR:
			dam = dice(GET_LEVEL(ch), 8);
			dam = NO_PANDION_DAMAGE(dam);
			break;
		case SPELL_CONE_OF_COLD:
			dam = dice(GET_LEVEL(ch), 2);
			dam = NO_PANDION_DAMAGE(dam);
			abil = SAVING_CON;
			attacktype = DANNO_GHIACCIO;
			break;
		case SPELL_AREA_LIGHTNING:
			dam = dice(GET_LEVEL(ch), 4);
			dam = NO_PANDION_DAMAGE(dam);
			if (IS_AFFECTED(ch, AFF_PROTECT_LIGHT))
			{
				dam = dam/2;
				act("&5La tua protezione dal fulmine disturba l'energia del fulmine.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			
			if (IS_AFFECTED(victim, AFF_PROTECT_LIGHT)) {
				dam = dam/3;
				act("&5La protezione dal fulmine di $N dissipa l'energia del tuo fulmine.&0", FALSE, ch, 0, victim, TO_CHAR);
			}
			abil = SAVING_CON;
			attacktype = DANNO_ELETTRICITA;
			break;
		case SPELL_EARTHQUAKE:
			dam = dice(GET_LEVEL(ch), 4);
			dam = NO_PANDION_DAMAGE(dam);
			attacktype = DANNO_THERION;
			break;
		case SPELL_FIRE_BREATH:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 5);
			abil = SAVING_CON;
			attacktype = DANNO_FUOCO;
			break;
		case SPELL_GAS_BREATH:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 5);
			abil = SAVING_CON;
			attacktype = DANNO_ACIDO;
			break;
		case SPELL_FROST_BREATH:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 5);
			abil = SAVING_CON;
			attacktype = DANNO_GHIACCIO;
			break;
		case SPELL_ACID_BREATH:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 5);
			attacktype = DANNO_ACIDO;
			break;
		case SPELL_LIGHTNING_BREATH:
			dam = dice(GET_LEVEL(ch)*GET_LEVEL(ch)/GET_LEVEL(victim), 5);
			if (IS_AFFECTED(ch, AFF_PROTECT_LIGHT)) 
			{ 
				dam = dam/2; 
				act("&5La tua protezione dal fulmine disturba l'energia del fulmine.&0", FALSE, ch, 0, victim, TO_CHAR); 
			} 
			
			if (IS_AFFECTED(victim, AFF_PROTECT_LIGHT)) { 
				
				dam = dam/2; 
				act("&5La protezione dal fulmine di $N dissipa l'energia del tuo fulmine.&0", FALSE, ch, 0, victim, TO_CHAR); 
			} 
			attacktype = DANNO_ELETTRICITA;
			abil = SAVING_CON;
			break;
		case SPELL_BLADEBARRIER:
			dam = dice(GET_LEVEL(ch), 3);
			abil = SAVING_CON;
			if (!(mag_savingthrow(victim, ch, savetype, SAVING_DEX, -10) || mag_savingthrow(victim, ch, savetype, SAVING_CON, -10))){
				do{
					caso = number(1, 6);
					if (AFFECTED(victim, AFF_CRITICAL_TESTA_3, 0) && AFFECTED(victim, AFF_CRITICAL_BRACCIO_D_3, 0) && AFFECTED(victim, AFF_CRITICAL_BRACCIO_S_3, 0) && AFFECTED(victim, AFF_CRITICAL_TORSO_3, 0) && AFFECTED(victim, AFF_CRITICAL_GAMBA_D_3, 0) && AFFECTED(victim, AFF_CRITICAL_GAMBA_S_3, 0)){
						caso = 0;
						break;
					}
					if (caso == 1 && AFFECTED(victim, AFF_CRITICAL_TESTA_3, 0))
						caso = 0;
					else if (caso == 2 && AFFECTED(victim, AFF_CRITICAL_BRACCIO_D_3, 0))
						caso = 0;
					else if (caso == 3 && AFFECTED(victim, AFF_CRITICAL_BRACCIO_S_3, 0))
						caso = 0;
					else if (caso == 4 && AFFECTED(victim, AFF_CRITICAL_TORSO_3, 0))
						caso = 0;
					else if (caso == 5 && AFFECTED(victim, AFF_CRITICAL_GAMBA_D_3, 0))
						caso = 0;
					else if (caso == 6 && AFFECTED(victim, AFF_CRITICAL_GAMBA_S_3, 0))
						caso = 0;
				} while (!(caso));
				switch (caso){
				case 1:
					posizione = "la testa";
					if (AFFECTED(victim, AFF_CRITICAL_TESTA_2, 0)){
						WAIT_STATE(victim,PULSE_VIOLENCE * 10);
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_3);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else if (AFFECTED(victim, AFF_CRITICAL_TESTA_1, 0)){
						WAIT_STATE(victim,PULSE_VIOLENCE * 4);
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_2);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else{
						WAIT_STATE(victim,PULSE_VIOLENCE * 10);
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_1);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					break;
				case 2:
					posizione = "l braccio destro";
					if (AFFECTED(victim, AFF_CRITICAL_BRACCIO_D_2, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_3);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else if (AFFECTED(victim, AFF_CRITICAL_BRACCIO_D_1, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_2);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else{
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_1);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					break;
				case 3:
					posizione = "l braccio sinistro";
					if (AFFECTED(victim, AFF_CRITICAL_BRACCIO_S_2, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_3);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else if (AFFECTED(victim, AFF_CRITICAL_BRACCIO_S_1, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_2);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else{
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_1);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					break;
				case 4:
					posizione = "l'addome";
					if (AFFECTED(victim, AFF_CRITICAL_TORSO_2, 0)){
						if(GET_EQ(victim,WEAR_BODY)){
							GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=15;
							if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
								extract_obj(GET_EQ(victim,WEAR_BODY));
								act("&8&2La tua barriera di lame distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
								act("&8&5La barriera di lame di $n distrugge la tua armatura .&0", FALSE, ch, 0, victim, TO_VICT);
								act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
							}
						}
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_3);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else if (AFFECTED(victim, AFF_CRITICAL_TORSO_1, 0)){
						if(GET_EQ(victim,WEAR_BODY)){
							GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=10;
							if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
								extract_obj(GET_EQ(victim,WEAR_BODY));
								act("&8&2La tua barriera di lame distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
								act("&8&5La barriera di lame di $n distrugge la tua armatura .&0", FALSE, ch, 0, victim, TO_VICT);
								act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
							}
						}
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_2);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else{
						if(GET_EQ(victim,WEAR_BODY)){
							GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=5;
							if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
								extract_obj(GET_EQ(victim,WEAR_BODY));
								act("&8&2La tua barriera di lame distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
								act("&8&5La barriera di lame di $n distrugge la tua armatura .&0", FALSE, ch, 0, victim, TO_VICT);
								act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
							}
						}
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_1);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					break;
				case 5:
					posizione = "la gamba destra";
					if (AFFECTED(victim, AFF_CRITICAL_GAMBA_D_2, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_3);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else if (AFFECTED(victim, AFF_CRITICAL_GAMBA_D_1, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_2);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else{
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_1);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					break;
				case 6:
					posizione = "la gamba sinistra";
					if (AFFECTED(victim, AFF_CRITICAL_GAMBA_S_2, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_3);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else if (AFFECTED(victim, AFF_CRITICAL_GAMBA_S_1, 0)){
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_2);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					else{
						SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_1);
						SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
					}
					break;
				}
				sprintf (buf, "&9&bLa barriera di lame di $n colpisce $N al%s.&0", posizione);
				act (buf, FALSE, ch, 0, victim, TO_NOTVICT);
				sprintf (buf, "&9&bLa tua barriera di lame colpisce $N al%s.&0", posizione);
				act (buf, FALSE, ch, 0, victim, TO_CHAR);
				sprintf (buf, "&9&bLa barriera di lame di $n ti colpisce al%s.&0", posizione);
				act (buf, FALSE, ch, 0, victim, TO_VICT);
			}
			break;
			
			
	} /* switch(spellnum) */
  /*Enrico add:extra damage for 2nd &3rd Class Title*/
	dam+=5*dam*GET_CLASS_TITLE(ch)/100;/*5% in +per il 2o liv,10% per il 3 liv*/
  /* divide damage by two if victim makes his saving throw */
	if (mag_savingthrow(victim, ch, savetype, abil, 0))
		dam >>= 1;
	
  /* and finally, inflict the damage */
	damage_nuovo(ch, victim, dam, spellnum, maxato, attacktype);
}


/*
 * Every spell that does an affect comes through here.  This determines
 * the effect, whether it is added or replacement, whether it is legal or
 * not, etc.
 *
 * affect_join(vict, aff, add_dur, avg_dur, add_mod, avg_mod)
*/

void mag_affects(int level, struct char_data * ch, struct char_data * victim,
	int spellnum, int savetype)
{
	struct affected_type af[MAX_SPELL_AFFECTS];
	int is_mage = FALSE, is_cleric = FALSE;
	bool accum_affect = FALSE, accum_duration = FALSE;
	char *to_vict = NULL, *to_room = NULL;
	char *to_victf = NULL, *to_roomf = NULL;
	int i;
	
	
	if (victim == NULL || ch == NULL)
		return;

	if (PLR_FLAGGED(victim, PLR_FANTASMA)){
		if (!PLR_FLAGGED(ch, PLR_FANTASMA))
			send_to_char ("Che vorresti fare ad un fantasma?", ch);
		return;
	}

	if (GET_LEVEL(victim) <= 10 && spellnum > MAX_SKILLS && spellnum < TOP_SPELL_DEFINE)
		return;
	
	is_mage = (GET_CLASS(ch) == CLASS_MAGIC_USER);
	is_cleric = (GET_CLASS(ch) == CLASS_CLERIC);
	
	for (i = 0; i < MAX_SPELL_AFFECTS; i++) {
		af[i].natura = SPELLSKILL;
		af[i].type = spellnum;
		af[i].bitvector[0] = 0;
		af[i].bitvector[3] = 0;
		af[i].bitvector[1] = 0;
		af[i].bitvector[2] = 0;
		af[i].modifier = 0;
		af[i].location = APPLY_NONE;
	}
	
	switch (spellnum) {

		/* By Spini:
		 * per comodità inserite qui le malattie, per quanto esse non siano
		 * magie normali, ma mantengono la stessa forma.
		 */

		case DISEASE_RAFFREDDORE:
			af[0].duration = -1;
			af[0].bitvector[BITV_RAFFREDDATO] = AFF_RAFFREDDATO;
			af[0].location = APPLY_MANA_REGEN;
			af[0].modifier = -30;
			af[1].duration = -1;
			af[1].bitvector[BITV_RAFFREDDATO] = AFF_RAFFREDDATO;
			af[1].location = APPLY_RES_GHIACCIO;
			af[1].modifier = -30;
			to_victf = "&6&bTi sei beccata un bel raffreddore!&0";
			to_vict = "&6&bTi sei beccato un bel raffreddore!&0";
			to_room = to_roomf = "&6&b$n mostra qualche segno di malessere.&0";
			break;
		case DISEASE_PESTE:
			af[0].duration = -1;
			af[0].bitvector[BITV_APPESTATO] = AFF_APPESTATO;
			if (RIDING(victim)){
				act ("Il dolore e' insopportabile, non puoi concentrarti per cavalcare!", FALSE, victim, 0, 0, TO_CHAR);
				dismount_char (victim);
			}
			GET_PESTE(victim) = 0;
			to_victf = to_vict = "&1&bDegli orribili bubboni spuntano sotto le tue ascelle!&0";
			to_room = to_roomf = "&1&b$n si cosparge di orribili bubboni.&0";
			break;
		case DISEASE_BACCO:
			af[0].duration = -1;
			af[0].bitvector[BITV_UBRIACO] = AFF_UBRIACO;
			to_vict = to_victf = "&5&bIn alto i calici, brindiamo a Bacco! Ti senti male, la tua ubriachezza e' diventata patologica...&0";
			to_room = to_roomf = "&5&b$n, ormai perso ogni senso del pudore, si accascia al suolo malato.&0";
			break;
		case DISEASE_SHAARR:
			af[0].duration = -1;
			af[0].bitvector[BITV_MALEDETTO_SHAARR] = AFF_MALEDETTO_SHAARR;
			to_vict = to_victf = "&1La tua incapacita' a combattere non e' passata inosservata a Shaarr. Fa' si che la tua immagine si rivaluti davanti a lei, o sarai dannato per l'eternita'!&0";
			to_room = "&1Una malefica aura rossastra copre $n, maledetto da Shaarr.&0";
			to_roomf = "&1Una malefica aura rossastra copre $n, maledetta da Shaarr.&0";
			break;
		case DISEASE_PIAGHE:
			af[0].duration = 0;
			af[0].bitvector[BITV_PIAGATO] = AFF_PIAGATO;			if (GET_EQ(victim, WEAR_HOLD))
				obj_to_char(unequip_char(victim, WEAR_HOLD), victim);
			if (GET_EQ(victim, WEAR_WIELD))
				obj_to_char(unequip_char(victim, WEAR_WIELD), victim);
			if (GET_EQ(victim, WEAR_LIGHT))
				obj_to_char(unequip_char(victim, WEAR_LIGHT), victim);
			if (GET_EQ(victim, WEAR_SHIELD))
				obj_to_char(unequip_char(victim, WEAR_SHIELD), victim);
			if (GET_EQ(victim, WEAR_WIELD_L))
				obj_to_char(unequip_char(victim, WEAR_WIELD_L), victim);
			if (GET_EQ(victim, WEAR_HANG))
				obj_to_room(unequip_char(victim, WEAR_HANG), victim->in_room);
			to_victf = to_vict = "&1&bLe tue mani si coprono di orribili piaghe, costringendoti a mollare tutto cio' che porti!&0";
			to_room = to_roomf = "&1&b$n molla cio' che aveva impugnato, le sue mani coperte da orribili piaghe!&0";
			break;
		case DISEASE_XHYPHYS:
			af[0].duration = -1;
			af[0].bitvector[BITV_MALEDETTO_XHYPHYS] = AFF_MALEDETTO_XHYPHYS;
			to_vict = to_victf = "&5Il Caos non perde occasione per dimostrare il suo potere. Questa volta a tuo discapito.&0";
			to_room = to_roomf = "&5Il caos ghermisce $n, che ormai giace inerte al suo volere.&0";
			break;
		case DISEASE_MORFEO:
			af[0].duration = -1;
			af[0].bitvector[BITV_ADDORMENTATO] = AFF_ADDORMENTATO;
			act ("&7&bLa tua eterna veglia e' finalmente terminata. Per ordine di Morfeo, d'ora in poi ti riposerai molto di piu'.&0", FALSE, victim, 0, 0, TO_CHAR);
			GET_POS (victim) = POS_SLEEPING;
			to_room = to_roomf = "&7&bMorfeo costringe $n a dormire. Dormire...&0";
			break;
		
		case SPELL_SHIELD:
			af[0].duration = 2 + GET_LEVEL(ch)/5;
			af[0].bitvector[0] = AFF_SHIELD;
			to_victf = to_vict = "Crei uno scudo di forza.";
			to_roomf = to_room = "Uno scudo di forza circonda $n.";
			af[0].location = APPLY_AC;
			af[0].modifier = -5;
			break;
			
		case SPELL_DEATHDANCE:
			af[0].duration = 1;
			af[0].bitvector[0] = AFF_DEATHDANCE;
			to_vict = "Ti senti come dire... indemoniato!!";
			to_victf = "Ti senti come dire... indemoniata!!";
			to_roomf = to_room = "Gli spiriti dei morti entrano in $n.";
			break;

		case SPELL_WRAITHFORM:
			af[0].duration = 2 + GET_LEVEL(ch)/14;
			af[0].bitvector[0] = AFF_PASSDOOR;
			to_victf = to_vict = "Diventi trasparente!";
			to_roomf = to_room = "$n diventa trasparente.";
			break;

		case SPELL_BURNING_HANDS:
			af[0].duration = 2;
			af[0].bitvector[BITV_BURNED] = AFF_BURNED;
			to_victf = to_vict = "I tuoi vestiti iniziano a bruciare, aiuto!";
			to_roomf = to_room = "$n avvampa nelle fiamme!";
			break;

		case SPELL_CHILL_TOUCH:
			af[0].duration = 2;
			af[0].bitvector[BITV_CHILLED] = AFF_CHILLED;
			to_victf = to_vict = "Il sangue quasi ti gela nelle vene per il freddo che provi!";
			to_roomf = to_room = "La pelle di $n diventa violacea e gelata.";
			break;

		case SPELL_FIRESHD:
			af[0].duration = 1;
			af[0].bitvector[0] = AFF_FIRESHD;
			to_victf = to_vict = "!!SOUND(frshldon.wav L=1 P=1)Ti avvolgi con un potente scudo di fuoco.";
			to_roomf = to_room = "!!SOUND(frshldon.wav L=1 P=1)Uno scudo di fuoco avvolge $n";
			break;
			
		case SPELL_ARMOR:
			af[0].location = APPLY_AC;
			af[0].modifier = -8;
			af[0].duration = 2 + GET_LEVEL(ch)/5;
			accum_duration = FALSE;
			to_victf = to_vict = "Senti che Qualcuno ti sta proteggendo.";
			break;
			
		case SPELL_WIZEYE:
			af[0].duration = 1 + GET_LEVEL(ch)/6;
			af[0].bitvector[0] = AFF_WIZEYE;
			to_victf = to_vict = "Gli occhi e la tua mente si uniscono.";
			break;
			
		case SPELL_BLESS:
			af[0].location = APPLY_HITROLL;
			af[0].modifier = 1;
			af[0].duration = 2 + GET_LEVEL(ch)/6;
			
			af[1].location = APPLY_SAVING_SPELL;
			af[1].modifier = -1;
			af[1].duration = 2 + GET_LEVEL(ch)/6;
			
			accum_duration = FALSE;
			to_vict = "Ti senti piu' valoroso.";
			to_victf = "Ti senti piu' valorosa.";
			break;
			
		case SPELL_BLINDNESS:
			if (MOB_FLAGGED(victim,MOB_NOBLIND) || mag_savingthrow(victim, ch, savetype, SAVING_INT, 0)) {
				send_to_char("Fallisci.\r\n", ch);
				return;
			}
			
			if (IS_FLYING(ch) && !IS_FLYING(victim)){
				send_to_char ("Non potrai mai accecarlo da quassu'!\r\n", ch);
				return;
			}
			
			af[0].location = APPLY_HITROLL;
			af[0].modifier = -3;
			af[0].duration = 1;
			af[0].bitvector[0] = AFF_BLIND;
			
			af[1].location = APPLY_AC;
			af[1].modifier = 30;
			af[1].duration = 1;
			af[1].bitvector[0] = AFF_BLIND;
			
			to_room = "$n sembra diventato cieco!";
			to_roomf = "$n sembra diventata cieca!";
			to_vict = "Sei stato accecato!";
			to_victf = "Sei stata accecata!";
			break;
			
		case SPELL_CURSE:
			if (mag_savingthrow(victim, ch, savetype, SAVING_NONE, 0)) {
				send_to_char(NOEFFECT, ch);
				return;
			}
			
			af[0].location = APPLY_HITROLL;
			af[0].duration = 2;
			af[0].modifier = -1;
			af[0].bitvector[0] = AFF_CURSE;
			
			af[1].location = APPLY_DAMROLL;
			af[1].duration = 2;
			af[1].modifier = -1;
			af[1].bitvector[0] = AFF_CURSE;
			
			accum_duration = TRUE;
			accum_affect = TRUE;
			to_room = "$n viene per un momento circondato da un alone rosso!";
			to_roomf = "$n viene per un momento circondata da un alone rosso!";
			to_victf = to_vict = "Senti che ti sta accadendo qualcosa di molto spiacevole.";
			break;
			
		case SPELL_DETECT_ALIGN:
			af[0].duration = 1 + GET_LEVEL(ch)/10 ;
			af[0].bitvector[0] = AFF_DETECT_ALIGN;
			accum_duration = TRUE;
			to_vict = "Ti formicolano gli occhi.";
			break;
			
		case SPELL_DETECT_INVIS:
			af[0].duration = 1 + GET_LEVEL(ch)/10 ;
			af[0].bitvector[0] = AFF_DETECT_INVIS;
			accum_duration = FALSE;
			to_victf = to_vict = "Ti formicolano gli occhi.";
			break;
			
		case SPELL_DETECT_MAGIC:
			af[0].duration = 1 + GET_LEVEL(ch)/10 ;
			af[0].bitvector[0] = AFF_DETECT_MAGIC;
			accum_duration = FALSE;
			to_victf = to_vict = "Ti formicolano gli occhi.";
			break;
			
		case SPELL_INFRAVISION:
			af[0].duration = 3 + GET_LEVEL(ch)/8;
			af[0].bitvector[0] = AFF_INFRAVISION;
			accum_duration = FALSE;
			to_victf = to_vict = "I tuoi occhi risplendono rossi come il fuoco.";
			to_roomf = to_room = "Gli occhi di $n risplendono rossi come il fuoco.";
			break;
			
		case SPELL_INVISIBLE:
			if (!victim)
				victim = ch;
			
			af[0].duration = 2 + (GET_LEVEL(ch)/10);
			af[0].modifier = -30;
			af[0].location = APPLY_AC;
			af[0].bitvector[0] = AFF_INVISIBLE;
			accum_duration = FALSE;
			to_victf = to_vict = "!!SOUND(invis.wav L=1 P=1)Svanisci.";
			to_roomf = to_room = "$n sbiadisce lentamente fino a non essere piu' visibile...";
			break;
			
		case SPELL_POISON:
			if (mag_savingthrow(victim, ch, savetype, SAVING_CON, 0)) {
				send_to_char(NOEFFECT, ch);
				return;
			}
			
			af[0].location = APPLY_STR;
			af[0].duration = 2;
			af[0].modifier = -2;
			af[0].bitvector[0] = AFF_POISON;
			to_victf = to_vict = "Ti senti veramente male!";
			to_room = "$n sembra molto malato, ha i brividi.";
			to_roomf = "$n sembra molto malata, ha i brividi.";
			break;
			
		case SPELL_PROT_FROM_EVIL:
			af[0].duration = 4;
			af[0].bitvector[0] = AFF_PROTECT_EVIL;
			accum_duration = TRUE;
			to_victf = to_vict = "Ti senti invulnerabile!!";
			break;
			
		case SPELL_SANCTUARY:
			af[0].duration = 1;
			af[0].bitvector[0] = AFF_SANCTUARY;
			
			
			to_victf = to_vict = "!!SOUND(sancon.wav L=1 P=1)La luce divina ti avvolge.";
			to_room = "$n e' avvolto da una luce divina.";
			to_roomf = "$n e' avvolta da una luce divina."; 
			break;
			
		case SPELL_PARALIZE:
			if (MOB_FLAGGED(victim, MOB_NOSLEEP))
				return;
			if (mag_savingthrow(victim, ch, SAVING_PARA, 0, 0))
				return;
			if (IS_FLYING(ch) && !IS_FLYING(victim)){
				send_to_char ("Non potrai mai paralizzarlo da quassu'!\r\n", ch);
				return;
			}
			af[0].duration = 1 + GET_LEVEL(ch)/14;
			af[0].bitvector[0] = AFF_PARALIZE;
			to_vict = "Tutte le parti del corpo si bloccano...sei stato &1PARALIZZATO&0!";
			to_victf = "Tutte le parti del corpo si bloccano...sei stata &1PARALIZZATA&0!"; 
			to_room = "$n si blocca improvvisamente...e' stato &1paralizzato&0!";
			to_roomf = "$n si blocca improvvisamente...e' stata &1paralizzata&0!";
			break;
			
		case SPELL_SLEEP:
    //   if (!pk_allowed && !arena_ok(ch, victim) && !IS_NPC(ch) && !IS_NPC(victim))
    // return;
			if(IS_NPC(victim))
				if (MOB_FLAGGED(victim, MOB_NOSLEEP))
					return;
			if (IS_FLYING(ch) && !IS_FLYING(victim)){
				send_to_char ("Non potrai mai addormentarlo da quassu'!\r\n", ch);
				return;
			}
				
			if ((!IS_NPC(victim) && (GET_LEVEL(victim) > (GET_LEVEL(ch) + (GET_SKILL(ch,SPELL_SLEEP)/10))))
				|| (IS_NPC(victim) && (GET_LEVEL(victim) + 10 > GET_LEVEL(ch))))
			{
				if (GET_SEX(ch)==SEX_MALE)
					act("$n barcolla ma rimane sveglio.", TRUE, victim, 0, 0, TO_ROOM);
				else
					act("$n barcolla ma rimane sveglia.", TRUE, victim, 0, 0, TO_ROOM);
				return; 
			}
			
			if (mag_savingthrow(victim, ch, SAVING_PARA, 0, 0))
				return;
			
			af[0].duration = 2;
			af[0].bitvector[0] = AFF_SLEEP;
			
			if (GET_POS(victim) > POS_SLEEPING) {
				act("Hai sonno... molto sonno...  Zzzz......", FALSE, victim, 0, 0, TO_CHAR);
				if (GET_SEX(ch)==SEX_MALE)
					act("$n cade addormentato.", TRUE, victim, 0, 0, TO_ROOM);
				else
					act("$n cade addormentata.", TRUE, victim, 0, 0, TO_ROOM);
				GET_POS(victim) = POS_SLEEPING;
				if(FIGHTING(ch))
					stop_fighting(ch);
			}
			break;
			
		case SPELL_STRENGTH:
			af[0].location = APPLY_STR;
			af[0].duration = (GET_LEVEL(ch)/5) + 1;
			af[0].modifier =number(1,2);
    //af[0].modifier = 1 + (level > 18);
			
			to_victf = to_vict = "Ti senti molto piu' forte!";
			break;
			
		case SPELL_SENSE_LIFE:
			to_victf = to_vict = "Senti che la tua attenzione migliora.";
			af[0].duration = 1 + GET_LEVEL(ch)/7;
			af[0].bitvector[0] = AFF_SENSE_LIFE;
			accum_duration = TRUE;
			break;
			
		case SPELL_WATERWALK:
			af[0].duration = 2 + GET_LEVEL(ch)/7;
			af[0].bitvector[0] = AFF_WATERWALK;
			accum_duration = TRUE;
			to_victf = to_vict = "Senti indurirsi il terreno sotto di te.";
			break;
			
		case SPELL_FLY:
			af[0].duration = 2 + GET_LEVEL(ch)/10;
			af[0].bitvector[0] = AFF_FLYING;
			accum_duration = FALSE;
			to_victf = to_vict = "!!SOUND(fly.wav L=1 P=1)Ti alzi in volo.";
			to_roomf = to_room = "$n si alza in volo.";
			break;
			
			
		case SPELL_LEVITATE:
			af[0].duration = 3 + GET_LEVEL(ch)/8;
			af[0].bitvector[BITV_LEVITATE] = AFF_LEVITATE;
			accum_duration = TRUE;
			to_victf = to_vict = "!!SOUND(fly.wav L=1 P=1)Inizi a galleggiare nell'aria.";
			to_roomf = to_room = "$n inizia a galleggiare nell'aria.";
			break;
			
		case SPELL_PROT_FIRE:
			af[0].duration = 1 + GET_LEVEL(ch)/8;
			af[0].bitvector[0] = AFF_PROT_FIRE;
			accum_duration = FALSE;
			to_victf = to_vict = "Una barriera di aria fredda ti avvolge proteggendoti dal fuoco.";
			break;
			
		case SPELL_WATERBREATH:
			af[0].duration = 1 + GET_LEVEL(ch)/6;
			af[0].bitvector[0] = AFF_WATERBREATH;
			accum_duration = FALSE;
			to_victf = to_vict = "E' come se ti fossero spuntate le branchie.";
			break;
			
		case SPELL_STONESKIN:
			af[0].location = APPLY_AC;
			af[0].modifier = -12;
			af[0].duration = 1 + GET_LEVEL(ch)/8;
			to_victf = to_vict = "Senti che la tua pelle diventa di pietra.";
			break;
			
		case SPELL_BARKSKIN:
			af[0].location = APPLY_AC;
			af[0].modifier = -10;
			af[0].duration = 1 + GET_LEVEL(ch)/6;
			to_victf = to_vict = "Senti che la tua pelle si indurisce.";
			break;
			
		case SPELL_MIRROR_IMAGE:
			af[0].duration = 1;
			af[0].bitvector[0] = AFF_MIRRORIMAGE;
			to_victf = to_vict = "La tua immagine si divide!!";
			to_roomf = to_room = "$n si divide in molte immagini identiche!!";
			break;
			
		case SPELL_BLINK:
			af[0].duration = 2;
			af[0].bitvector[0] = AFF_BLINK;
			to_victf = to_vict = "Non ti senti molto differente.";
			to_roomf = to_room = "Vedi $n comparire a qualche passo di distanza da dove era prima.";
			break;
			
		case SPELL_PROTECT_LIGHT:
			af[0].duration = 4 + GET_LEVEL(ch)/4;
			af[0].bitvector[0] = AFF_PROTECT_LIGHT;
			to_victf = to_vict = "Una carica energetica negativa ti avvolge proteggendoti dal fulmine.";
			to_roomf = to_room = "Vedi i capelli di $n drizzarsi per un'istante.";
			break;
			
		case SPELL_DET_SNEAK:
			af[0].duration = 1 + GET_LEVEL(ch)/4;
			af[0].bitvector[0] = AFF_DET_SNEAK;
			to_victf = to_vict = "Senti anche il piu' debole dei rumori......";
			accum_duration = TRUE;
			break;
			
		case SPELL_SILENCE:
			if (mag_savingthrow(victim, ch, savetype, SAVING_INT, 0))
				return;  
			af[0].duration = 1 + GET_LEVEL(ch)/10;
			af[0].bitvector[0] = AFF_SILENCE;
			to_victf = to_vict = "Vieni ammutolito da $N.";
			break;
			
		case SPELL_ANTIMAGIC:
			af[0].duration = 6;
			af[0].bitvector[0] = AFF_ANTIMAGIC;
			to_victf = to_vict = "Vieni circondato da un aura antimagia.";
			to_roomf = to_room = "Vedi $n circondato da un aura antimagia.";
			break;
			
		case SPELL_AID:
			if (!victim)
				victim = ch;
			af[0].duration = 4 + (GET_LEVEL(ch)/2);
			af[0].modifier = number(1,4) + GET_LEVEL(ch)/5;
			af[0].location = APPLY_HIT;
			af[0].bitvector[0] = AFF_AID;
			accum_duration = FALSE;
			to_victf = to_vict = "Senti gli Dei con te!";
			to_room = "$n viene aiutato dagli Dei.";
			to_roomf = "$n viene aiutata dagli Dei.";
			break;
			
		case SPELL_CONGIUNZIONE:
			if (!victim)
				break;
			af[0].duration = 4 + (GET_LEVEL(ch)/4);    
			af[0].bitvector[0] = AFF_CONGIUNZIONE;
			accum_duration = FALSE;
			to_victf = to_vict = "Entri in congiunzione con i tuoi compagni!";
			to_roomf = to_room = "$n entra a far parte della cerchia di congiunzione.";
			break;
		
		case SPELL_ENDURANCE:
			if (!victim)
				victim = ch;
			af[0].duration = 2 + (GET_LEVEL(ch)/2);
			af[0].bitvector[0] = AFF_ENDURANCE;
			accum_duration = FALSE;
			to_victf = to_vict = "Una buona respirazione e' alla base di un ottima forma.";
			to_room = "$n respira a pieni polmoni.";
			to_roomf = "$n respira a pieni polmoni.";
			break;
			
	}
	
  /*Enrico add:Extra spell duration for 2nd &3rd Class Titles*/
	for(i=0;i<MAX_SPELL_AFFECTS;i++){
		if(af[i].duration>0)
			af[i].duration+=5*af[i].duration*GET_CLASS_TITLE(ch)/100;
	}
	
  /*
   * If this is a mob that has this affect set in its mob file, do not
   * perform the affect.  This prevents people from un-sancting mobs
   * by sancting them and waiting for it to fade, for example.
   
  if (IS_NPC(victim) && !affected_by_spell(victim, SPELLSKILL, spellnum))
    for (i = 0; i < MAX_SPELL_AFFECTS; i++)
      if (IS_AFFECTED(victim, af[i].bitvector[0])) { 
	send_to_char(NOEFFECT, ch);
	return;
        }

  
   * If the victim is already affected by this spell, and the spell does
   * not have an accumulative effect, then fail the spell.
   */
	if (affected_by_spell(victim, SPELLSKILL, spellnum) && !(accum_duration||accum_affect)) {
		send_to_char(NOEFFECT, ch);
		return;
	}
	
	for (i = 0; i < MAX_SPELL_AFFECTS; i++)
		if ((af[i].bitvector[0]) || (af[i].bitvector[1]) || (af[i].bitvector[2]) || (af[i].bitvector[3]) || (af[i].location != APPLY_NONE))
			affect_join(victim, af+i, accum_duration, FALSE, accum_affect, FALSE);
	if (GET_SEX(ch) == SEX_MALE) {	
		if (to_vict != NULL)
			act(to_vict, FALSE, victim, 0, ch, TO_CHAR);
	} else if (to_victf != NULL)
		act(to_victf, FALSE, victim, 0, ch, TO_CHAR);
	if (GET_SEX(ch) == SEX_MALE) {
		if (to_room != NULL)
			act(to_room, TRUE, victim, 0, ch, TO_ROOM);
	} else if (to_roomf != NULL)
		act(to_roomf, TRUE, victim, 0, ch, TO_ROOM);
}


/*
 * This function is used to provide services to mag_groups.  This function
 * is the one you should change to add new group spells.
 */

void perform_mag_groups(int level, struct char_data * ch,
	struct char_data * tch, int spellnum, int savetype)
{
	switch (spellnum) {
		case SPELL_GROUP_ARMOR:
			mag_affects(level, ch, tch, SPELL_ARMOR, savetype);
			break;
		case SPELL_GROUP_RECALL:
			spell_recall(level, ch, tch, NULL);
			break;
		case SPELL_GROUP_FLY:
			mag_affects(level, ch, tch, SPELL_FLY, savetype);
			break;
		case SPELL_GROUP_INVIS:
			mag_affects(level, ch, tch, SPELL_INVISIBLE, savetype);
			break;
		case SPELL_GROUP_PROT_EVIL:
			mag_affects(level, ch, tch, SPELL_PROT_FROM_EVIL, savetype);
			break;
		case SPELL_GROUP_WATBREATH:
			mag_affects(level, ch, tch, SPELL_WATERBREATH, savetype);
			break;
		case SPELL_HEROES_FEAST:
			spell_feast(level, ch, tch, NULL);
			break;
			
	}
}


/*
 * Every spell that affects the group should run through here
 * perform_mag_groups contains the switch statement to send us to the right
 * magic.
 *
 * group spells affect everyone grouped with the caster who is in the room,
 * caster last.
 *
 * To add new group spells, you shouldn't have to change anything in
 * mag_groups -- just add a new case to perform_mag_groups.
 */

void mag_groups(int level, struct char_data * ch, int spellnum, int savetype, int casttype)
{
	struct char_data *tch, *k;
	struct follow_type *f, *f_next;
	
	if (ch == NULL)
		return;
	
	if (!IS_AFFECTED(ch, AFF_GROUP))
		return;

	if (spellnum == SPELL_GROUP_HEAL){
		perform_group_heal (level, ch, savetype, casttype);
		return;
	}

	if (ch->master != NULL)
		k = ch->master;
	else
		k = ch;
	for (f = k->followers; f; f = f_next) {
		f_next = f->next;
		tch = f->follower;
		if (tch->in_room != ch->in_room)
			continue;
		if (!IS_AFFECTED(tch, AFF_GROUP))
			continue;
		if (ch == tch)
			continue;
		perform_mag_groups(level, ch, tch, spellnum, savetype);
	}
	
	if ((k != ch) && IS_AFFECTED(k, AFF_GROUP))
		perform_mag_groups(level, ch, k, spellnum, savetype);
	perform_mag_groups(level, ch, ch, spellnum, savetype);
}

void perform_group_heal (int level, struct char_data *ch, int savetype, int casttype)
{
	int curati = 0, mana = 0;
	float rapporto;
	struct char_data *tch, *k;
	struct follow_type *f, *f_next;

	if (ch->master != NULL)
		k = ch->master;
	else
		k = ch;
	for (f = k->followers; f; f = f_next) {
		f_next = f->next;
		tch = f->follower;
		if (tch->in_room != ch->in_room)
			continue;
		if (!IS_AFFECTED(tch, AFF_GROUP))
			continue;
		if (tch == ch)
			continue;
		curati += GET_HIT (tch) > GET_MAX_HIT (tch) / 2 ? GET_MAX_HIT (tch) - GET_HIT (tch) : GET_MAX_HIT (tch) / 2;
	}
	if (k != ch)
		curati += GET_HIT (k) > GET_MAX_HIT (k) / 2 ? GET_MAX_HIT (k) - GET_HIT (k) : GET_MAX_HIT (k) / 2;
	curati += GET_HIT (ch) > GET_MAX_HIT (ch) / 2 ? GET_MAX_HIT (ch) - GET_HIT (ch) : GET_MAX_HIT (ch) / 2;

	if (casttype == CAST_SPELL)
		mana = (GET_LEVEL (ch) * 0.35) + (curati * 0.43);

	if (GET_MANA (ch) >= mana){
		for (f = k->followers; f; f = f_next) {
			f_next = f->next;
			tch = f->follower;
			if (tch->in_room != ch->in_room)
				continue;
			if (!IS_AFFECTED(tch, AFF_GROUP))
				continue;
			if (tch == ch)
				continue;
			GET_HIT (tch) += GET_HIT(tch) > GET_MAX_HIT (tch) / 2 ? GET_MAX_HIT (tch) - GET_HIT (tch) : GET_MAX_HIT (tch) / 2;
			if (!(affected_by_spell (tch, SPELLSKILL, DISEASE_SHAARR)))
				REMOVE_BIT(AFF_FLAGS(tch), AFF_FERITO);
		}
		if ((k != ch) && IS_AFFECTED(k, AFF_GROUP)){
			GET_HIT(k) += GET_HIT (k) > GET_MAX_HIT (k) / 2 ? GET_MAX_HIT (k) - GET_HIT (k) : GET_MAX_HIT (k) / 2;
			if (!(affected_by_spell (k, SPELLSKILL, DISEASE_SHAARR)))
				REMOVE_BIT(AFF_FLAGS(k), AFF_FERITO);
		}
		GET_HIT(ch) += GET_HIT (ch) > GET_MAX_HIT (ch) / 2 ? GET_MAX_HIT (ch) - GET_HIT (ch) : GET_MAX_HIT (ch) / 2;
		if (!(affected_by_spell (ch, SPELLSKILL, DISEASE_SHAARR)))
			REMOVE_BIT(AFF_FLAGS(ch), AFF_FERITO);
		alter_mana (ch, mana);
		return;
	}

	curati = 0;
	rapporto = (float) GET_MANA (ch) / mana;

	for (f = k->followers; f; f = f_next) {
		f_next = f->next;
		tch = f->follower;
		if (tch->in_room != ch->in_room)
			continue;
		if (!IS_AFFECTED(tch, AFF_GROUP))
			continue;
		if (ch == tch)
			continue;
		GET_HIT (tch) += MIN ((GET_HIT (tch) > GET_MAX_HIT (tch) / 2) ? (GET_MAX_HIT (tch) - GET_HIT (tch)) : (GET_MAX_HIT (tch) / 2), GET_MAX_HIT (tch) * rapporto / 2);
		curati += MIN ((GET_HIT (tch) > GET_MAX_HIT (tch) / 2) ? (GET_MAX_HIT (tch) - GET_HIT (tch)) : (GET_MAX_HIT (tch) / 2), GET_MAX_HIT (tch) * rapporto / 2);
	}
	if ((k != ch) && IS_AFFECTED(k, AFF_GROUP)){
		GET_HIT (k) += MIN ((GET_HIT(k) > GET_MAX_HIT(k) / 2) ? (GET_MAX_HIT (k) - GET_HIT (k)) : (GET_MAX_HIT(k) / 2), GET_MAX_HIT (k) * rapporto / 2);
		curati += MIN ((GET_HIT (k) > GET_MAX_HIT (k) / 2) ? (GET_MAX_HIT (k) - GET_HIT (k)) : (GET_MAX_HIT (k) / 2), GET_MAX_HIT (k) * rapporto / 2);
	}
	GET_HIT (k) += MIN ((GET_HIT(ch) > GET_MAX_HIT(ch) / 2) ? (GET_MAX_HIT (ch) - GET_HIT (ch)) : (GET_MAX_HIT(ch) / 2), GET_MAX_HIT (ch) * rapporto / 2);
	curati += MIN ((GET_HIT (ch) > GET_MAX_HIT (ch) / 2) ? (GET_MAX_HIT (ch) - GET_HIT (ch)) : (GET_MAX_HIT (ch) / 2), GET_MAX_HIT (ch) * rapporto / 2);

	GET_MANA (ch) = 0;
}

/*
 * mass spells affect every creature in the room except the caster.
 *
 * No spells of this class currently implemented as of Circle 3.0.
 */

void mag_masses(int level, struct char_data * ch, int spellnum, int savetype)
{
	struct char_data *tch, *tch_next;
	
	for (tch = world[ch->in_room].people; tch; tch = tch_next) {
		tch_next = tch->next_in_room;
		if (tch == ch)
			continue;
		
		switch (spellnum) {
		}
	}
}


/*
 * Every spell that affects an area (room) runs through here.  These are
 * generally offensive spells.  This calls mag_damage to do the actual
 * damage -- all spells listed here must also have a case in mag_damage()
 * in order for them to work.
 *
 *  area spells have limited targets within the room.
*/

void mag_areas(int level, struct char_data * ch, int spellnum, int savetype)
{
	struct char_data *tch, *next_tch;
	char *to_char = NULL;
	char *to_room = NULL;
        struct char_data *k = NULL; //Orione
	struct follow_type *f;	    //Orione
	int target_questmob_group = 0;       //Orione
	extern struct index_data *mob_index; //Orione
		    	
	if (ch == NULL)
		return;
	
  /*
   * to add spells to this fn, just add the message here plus an entry
   * in mag_damage for the damaging part of the spell.
   */
	switch (spellnum) {
		case SPELL_EARTHQUAKE:
			to_char = "Compi un gesto veloce con la mano e la terra intorno a te comincia a tremare!";
			to_room ="$n compie un rapido gesto con la mano e la terra comincia a tremare violentemente!";
			break;
		case SPELL_FIRE_BREATH:
			to_room ="Con un ruggito, $n soffia e la stanza viene avvolta dalle fiamme!";
			to_char = "Con un ruggito soffi fuoco, arrostendo qualsiasi cosa nei paraggi!";
			break;
		case SPELL_GAS_BREATH:
			to_room ="Con un ruggito, $n riempie la stanza di gas mortali!";
			to_char ="Reciti un incantesimo mortale, riempiendo la stanza di vapori infernali!";
			break;
		case SPELL_FROST_BREATH:
			to_room ="Con un poderoso ruggito, $n congela tutto con un soffio di ghiaccio!";
			to_char ="Ruggisci, avvolgendo la stanza nei ghiacci eterni!";
			break;
		case SPELL_ACID_BREATH:
			to_room ="Un getto d'acido scaturisce dalla bocca di $n, corrodendo tutto!";
			to_char ="Soffi una nube acida nella stanza!";
			break;
		case SPELL_LIGHTNING_BREATH:
			to_room ="$n ruggisce, lanciando dappertutto potenti fulmini!";
			to_char ="Apri la bocca e generi una potente ondata di elettricita'!";
			break;
		case SPELL_FLAME_ARROW:
			to_char = "Dalle tue mani scaturisce una freccia infuocata dritta sul bersaglio.";
			to_room = "$n lancia una freccia di fuoco.";
			break;
		case SPELL_ACID_ARROW:
			to_char = "Uno schizzo d'acido ti colpisce.";
			to_room = "$n viene colpito da uno schizzo d'acido.";
			break;
		case SPELL_MINUTE_METEOR:
			to_char = "$n lancia uno sciame di meteore!";
			to_char = "Lanci uno sciame di meteore!";
			break;
		case SPELL_CONE_OF_COLD:
			to_room ="$n evoca una folata di vento congelante!";
			to_char ="Crei dal nulla una folata di vento congelante.";
			break;
		case SPELL_AREA_LIGHTNING:
			to_room ="Con un grido furioso, $n evoca una tempesta di fulmini!";
			to_char ="Concentri la tua rabbia e la trasformi in pura energia che scarichi sui tuoi nemici!";
			break;
		case SPELL_BLADEBARRIER:
			to_room ="$n ride ed evoca una BLADE BARRIER!";
			to_char ="Ti concentri sulle lame. Lame. LAMEEE!!!";
			break;
		case SPELL_DISINTEGRATE:
			to_room ="$n lancia la sua ira disintegrande.";
			to_char ="Lanci la tua ira disintegrante!!";
			break;
		case SPELL_COLOR_SPRAY:
			to_room ="&3&b$n lascia partire un &1&barc&0&3oba&blen&2o mu&6lti&4col&0&5ore&3&b dalla sua mano!&0";
			to_char ="&3&bUn &1&barc&0&3oba&blen&2o mu&6lti&4col&0&5ore&3&b parte dalla tua mano in direzione dei nemici.&0";
	}
	if (to_char != NULL)
		act(to_char, FALSE, ch, 0, 0, TO_CHAR);
	if (to_room != NULL)
		act(to_room, FALSE, ch, 0, 0, TO_ROOM);
	
	
	for (tch = world[ch->in_room].people; tch; tch = next_tch) {
		next_tch = tch->next_in_room;
		
		
		
    /*
     * The skips: 1: the caster
     *            2: immortals
     *            3: if no pk on this mud, skips over all players
     *            4: pets (charmed NPCs)
     *		  5: questmob	//Orione
     * 3) players can only hit players in CRIMEOK rooms 
     * 4) players can only hit charmed mobs in CRIMEOK rooms
     * 5) ch puo' colpire questmob solo se e' ch destinato a quel questmob
     *    oppure se appartiene al gruppo cui appartiene anche il pg cui quel questmob e' destinato
     *    e pg cui e' destinato e mob stanno gia' combattendo
     */
		
		if (tch == ch)
			continue;
		if (!IS_NPC(tch) && GET_LEVEL(tch) >= LVL_QUEST_MASTER)
			continue;
		if PLR_FLAGGED(tch, PLR_FANTASMA)
		  continue;
		if (!pk_allowed && !arena_ok(ch, tch) && !IS_NPC(ch) && !IS_NPC(tch))
			continue;
		if (!IS_NPC(ch) && IS_NPC(tch) && IS_AFFECTED(tch, AFF_CHARM))
			continue;
		if (MOB_FLAGGED(tch, MOB_SELVAGGINA) && (!MOB_FLAGGED(tch, MOB_HARD_TAME)))
			continue;
		/* Orione Inizio */
		if (!IS_NPC(ch) && IS_NPC(tch) && MOB_FLAGGED(tch, MOB_QUEST)) {
		    if (FIGHTING(tch)==NULL) { //mob non in combattimento
			//se ch non e' il pg cui e' destinato, non puo' attaccarlo
			if (!(IS_QUESTOR(ch) && \
			    ((GET_TIPOQUEST(ch)==QUEST_MOBKILL) || (GET_TIPOQUEST(ch)==QUEST_GETITEM)) && \
			    (GET_QUESTMRR(ch)==GET_MOB_VNUM(tch))))
			        continue;
		    } else if (ch && AFF_FLAGGED(ch, AFF_GROUP)) {
			//mob in combattimento, ch gruppato
		        if (ch->master) k = ch->master;
		        else k = ch;

			if (IS_QUESTOR(k) && \
			    ((GET_TIPOQUEST(k)==QUEST_MOBKILL) || (GET_TIPOQUEST(k)==QUEST_GETITEM)) && \
			    (GET_QUESTMRR(k)==GET_MOB_VNUM(tch)))
				target_questmob_group = TRUE;
			if (!target_questmob_group) {
			    for (f = k->followers; f; f = f->next) {
			        if (IS_QUESTOR(f->follower) && \
			            ((GET_TIPOQUEST(f->follower)==QUEST_MOBKILL) || (GET_TIPOQUEST(f->follower)==QUEST_GETITEM)) && \
				    (GET_QUESTMRR(f->follower)==GET_MOB_VNUM(tch))) {
					target_questmob_group = TRUE;
					break;
				}
			    }
			}
			//se il mob non e' destinato a nessuno del gruppo, non si puo' attaccarlo
			if (!target_questmob_group) continue;
   		    } else if (!(!IS_NPC(ch) && IS_QUESTOR(ch) && \
				((GET_TIPOQUEST(ch)==QUEST_MOBKILL) || (GET_TIPOQUEST(ch)==QUEST_GETITEM)) && \
				(GET_QUESTMRR(ch)==GET_MOB_VNUM(tch)))) 
					//il mob sta combattendo, ma ch non appartiene al gruppo attualmente contro il mob
					continue;
		}
		/* Orione Fine */
			    																									    																    				
		mag_damage(GET_LEVEL(ch), ch, tch, spellnum, 1);
	}
}


/*
  Every spell which summons/gates/conjours a mob comes through here.

  None of these spells are currently implemented in Circle 3.0; these
  were taken as examples from the JediMUD code.  Summons can be used
  for spells like clone, ariel servant, etc.
*/

static char *mag_summon_msgs[] = {
	"\r\n",
	"$n compie uno strano gesto; senti che una forte brezzacomincia a soffiare!\r\n",
	"$n rianima un cadavere!\r\n",
	"$N appare in una nuvola di denso fumo blu!\r\n",
	"$N appare in una nuvola di denso fumo verde!\r\n",
	"$N appare in una nuvola di denso fumo rosso!\r\n",
	"$N scompare in una nuvola nera!\r\n",
	"Appena $n compie uno strano gesto, senti che una forte brezza comincia a soffiare.\r\n",
	"Appena $n compie uno strano gesto, vieni avvolto da un forte calore.\r\n",
	"Appena $n compie uno strano gesto, un improvviso gelo t'invade.\r\n",
	"Appena $n compie uno strano gesto, una nube di polvere s'innalza nell'aria.\r\n",
	"$n si divide magicamente!\r\n",
	"$n rianima un cadavere!\r\n"
};

#define MOB_MONSUM_I		    1203	/* lesser level */
#define MOB_MONSUM_II		    1204	/* regular */
#define MOB_MONSUM_III	    1205	/* greater */
#define MOB_MONSUM_IV		    1206	/* dragon  */
#define MOB_MONSUM_V		    1206	/* ancient */
#define MOB_GATE_I		      4020
#define MOB_GATE_II		      4020
#define MOB_GATE_III		    4020
#define MOB_ELEMENTAL_BASE	4020
#define MOB_CLONE		        4020
#define MOB_ZOMBIE		      1176
#define MOB_AERIALSERVANT	  4020


static char *mag_summon_fail_msgs[] = {
	"\r\n",
	"Non ci sono creature di questo tipo.\r\n",
	"Uh oh...\r\n",
	"Whoooops.\r\n",
	"Oh cacchio!\r\n",
	"Gli elementi resistono!\r\n",
	"Hai fallito.\r\n",
	"Non c'e' nessun cadavere!\r\n"
};


void mag_summons(int level, struct char_data * ch, struct obj_data * obj,
	int spellnum, int savetype)
{
	struct char_data *mob;
	struct obj_data *tobj, *next_obj;
	int pfail = 0;
	int msg = 0, fmsg = 0;
	int mob_num = 0;
	int change_mob = 0;
	int handle_corpse = 0;
	char mob_name[256];
	char mob_alias[256];
	char mob_sdes[256];
	char mob_ldes[256];
	
	if (ch == NULL)
		return;
	
	switch (spellnum) {
	  case SPELL_MONSUM_I:
			if (   (obj == NULL) 
			    || (GET_OBJ_TYPE(obj) != ITEM_CONTAINER) 
			    || (!GET_OBJ_VAL(obj, 3))
			   ) {
				act(mag_summon_fail_msgs[7], FALSE, ch, 0, 0, TO_CHAR);
				return;
			}
			handle_corpse = 1;
			change_mob = 1;
			msg = 15;
			mob_num = MOB_MONSUM_I;
			pfail = 20;
			strcpy(mob_name, (obj)->short_description);
			strcpy(mob_sdes, (obj)->short_description);
			sprintf(mob_alias, "cadavere zombi %s", mob_name);
			sprintf(mob_ldes, "il cadavere in putrefazione di %s e' qui che barcolla lentamente.\r\n", mob_sdes);
			break;
		case SPELL_ANIMATE_DEAD:
			msg = 4;
			mob_num = MOB_MONSUM_I;
			pfail = 20;
			change_mob = 0;
			break;
		case SPELL_MONSUM_II:
			msg = 4;
			mob_num = MOB_MONSUM_II;
			pfail = 25;
			change_mob = 0;
			break;
		case SPELL_MONSUM_III:
			msg = 4;
			mob_num = MOB_MONSUM_III;
			pfail = 30;
			change_mob = 0;
			break;
		case SPELL_MONSUM_IV:
			msg = 4;
			mob_num = MOB_MONSUM_IV;
			pfail = 35;
			change_mob = 0;
			break;
		case SPELL_MONSUM_V:
			msg = 4;
			mob_num = MOB_MONSUM_V;
			pfail = 40;
			change_mob = 0;
			break;
		case SPELL_CONJ_ELEMENTAL:
			msg = 4;
			mob_num = MOB_ELEMENTAL_BASE;
			pfail = 20;
			change_mob = 0;
			break;
		case SPELL_CLONE:
			mob_num = MOB_CLONE;
			pfail = 50;
			strcpy(mob_name, GET_NAME(ch));
			strcpy(mob_sdes, GET_DESCR(ch));
			strcpy(mob_ldes, GET_L_DESCR(ch));
			change_mob = 1;
			break;
		default:
			return;
	}
	
	if (IS_AFFECTED(ch, AFF_CHARM)) {
		send_to_char("You are too giddy to have any followers!\r\n", ch);
		return;
	}
	
	if (number(0, 101) < pfail) {
		send_to_char(mag_summon_fail_msgs[fmsg], ch);
		return;
	}
	
	if (limite_follower(ch)) {
		send_to_char("Non sei in grado di controllare la volonta' di altri esseri, rinunci.", ch);
		return;
	}
	
	if ((mob_num == -1) || (real_mobile(mob_num) ==-1))
		return;
	
	if (!(mob = read_mobile(real_mobile(mob_num), REAL))) {
		send_to_char("I seem to have goofed.\r\n", ch);
		sprintf(buf, "SYSERR: spell_summons, spell %d, mob %d: mob not found",
			spellnum, mob_num);
		log(buf);
		return;
	}
	
	char_to_room(mob, ch->in_room);
	IS_CARRYING_W(mob) = 0;
	IS_CARRYING_N(mob) = 0;
	SET_BIT(AFF_FLAGS(mob), AFF_CHARM);
	add_follower(mob, ch);
	act(mag_summon_msgs[fmsg], FALSE, ch, 0, mob, TO_ROOM);
	load_mtrigger(mob); //Orione
	
	if (change_mob) {
		mob->player.name        = strdup(mob_alias);
		mob->player.short_descr = strdup(mob_sdes);
		mob->player.long_descr  = strdup(mob_ldes);
	}
	
	if (handle_corpse) {
		for (tobj = obj->contains; tobj; tobj = next_obj) {
			next_obj = tobj->next_content;
			obj_from_obj(tobj);
			obj_to_char(tobj, mob);
		}
		extract_obj(obj);
	}
}


void mag_points(int level, struct char_data *ch, struct char_data *victim,
	int spellnum, int savetype, int casttype)
{
	int hit = 0;
	int move = 0;
	int mana = 0;
	
	if (victim == NULL)
		return;
	
	switch (spellnum) {
		case SPELL_CURE_LIGHT:
			if (GET_MAX_HIT (victim) < GET_HIT (victim))
				GET_HIT (victim) = GET_MAX_HIT (victim);

			hit = MAX (GET_MAX_HIT (victim) / 20, 5) + number (0, 5 + (GET_LEVEL (ch) / 10));
			if (casttype == CAST_SPELL)
				mana = (GET_LEVEL(ch) / 20) + (MIN (hit, GET_MAX_HIT (victim) - GET_HIT (victim)) * 0.30);
			if (mana > GET_MANA (ch) && !IS_NPC (ch)){
				hit *= (float) GET_MANA(ch) / mana;
				mana = GET_MANA(ch);
				send_to_char("Alcune sbucciature smettono di sanguinare.\r\n", victim);
			}
			else
				send_to_char("Ti senti meglio.\r\n", victim);
			break;
			
		case SPELL_CURE_SERIOUS:
			hit = MAX (GET_MAX_HIT (victim) * 0.15, 15) + number (0, 5 + (GET_LEVEL (ch) / 9));
			if (casttype == CAST_SPELL)
				mana = (GET_LEVEL(ch) / 12) + (MIN (hit, GET_MAX_HIT (victim) - GET_HIT (victim)) * 0.32);
			if (mana > GET_MANA (ch) && !IS_NPC (ch)){
				hit *= (float) GET_MANA(ch) / mana;
				mana = GET_MANA(ch);
				send_to_char("Qualche ferita del tuo corpo si cicatrizza.\r\n", victim);
			}
			else
				send_to_char("Ti senti molto meglio!\r\n", victim);
			break;
			
		case SPELL_CURE_CRITIC:
			hit = MAX (GET_MAX_HIT (victim) * 0.3, 30) + number (0, 5 + (GET_LEVEL (ch) / 8));
			if (casttype == CAST_SPELL)
				mana = (GET_LEVEL (ch) * 0.18) + (MIN (hit, GET_MAX_HIT (victim) - GET_HIT (victim)) * 0.34);
			if (mana > GET_MANA (ch) && !IS_NPC (ch)){
				hit *= (float) GET_MANA(ch) / mana;
				mana = GET_MANA(ch);
				send_to_char("Alcune profonde ferite si chiudono rapidamente.\r\n", victim);
			}
			else{
				send_to_char("Ti senti molto ma molto meglio!\r\n", victim);
				if (!(affected_by_spell (victim, SPELLSKILL, DISEASE_SHAARR)))
					REMOVE_BIT(AFF_FLAGS(victim),AFF_FERITO);
			}
			break;
		case SPELL_HEAL:
			hit = MAX (GET_MAX_HIT (victim) / 2, 50) + number (0, 5 + (GET_LEVEL (ch) / 6));
			if (casttype == CAST_SPELL)
				mana = (GET_LEVEL (ch) * 0.35) + (MIN (hit, GET_MAX_HIT (victim) - GET_HIT (victim)) * 0.38);
			if (mana > GET_MANA (ch) && !IS_NPC (ch)){
				hit *= (float) GET_MANA(ch) / mana;
				mana = GET_MANA(ch);
				act("&3&bL'intercessione di $n ti regala un po' di benessere.&0", TRUE, ch, 0, victim, TO_VICT);
				act("&3&bLe tue energie non bastano per curare $N!&0", TRUE, ch, 0, victim, TO_CHAR);
				act("&3&bLe preghiere di $n permettono a $N di recuperare un po' le forze.&0", TRUE, ch, 0, victim, TO_NOTVICT);
			}
			else{
				if (!(affected_by_spell (victim, SPELLSKILL, DISEASE_SHAARR)))
					REMOVE_BIT(AFF_FLAGS(victim), AFF_FERITO);
				if (victim == ch){
				  	act("&3&bInvochi l'aiuto divino.&0", FALSE, ch, 0, 0, TO_CHAR);
					act("&3&b$n congiunge le sue mani in preghiera, le sue ferite vengono risanate.&0", FALSE, ch, 0, 0, TO_ROOM);
				}
				else{
					act("&3&b$n congiunge le mani in preghiera curando le ferite di $N.&0", FALSE, ch, 0, victim, TO_NOTVICT);
					act("&3&b$n invoca l'aiuto divino, sanando le tue ferite.&0", FALSE, ch, 0, victim, TO_VICT);
					act("&3&bInvochi l'aiuto divino su $N.&0", FALSE, ch, 0, victim, TO_CHAR);
				}
			}
			break;

		case SPELL_DEATHDANCE:
			
			if (newbye(ch)&&!IS_NPC(victim)) {
				send_to_char("Senti un dolore lancinante alla testa.\r\n",ch);
				return;
			} else if (newbye(victim)) {
				send_to_char("Senti un dolore lancinate alla testa quando guardi il Newbye.\r\n",ch);
				return;
			}
			
			hit = 0 - (GET_MAX_HIT(victim) / 2);
			move = 0 - (GET_MAX_MOVE(victim) / 4);
			break;
	}
	alter_hit(victim, -hit);
	alter_move(victim, -move);
	if (!IS_NPC (ch))
		alter_mana(ch, mana);
}


void mag_unaffects(int level, struct char_data * ch, struct char_data * victim,
	int spellnum, int type)
{
	int spell = 0;
	char *to_vict = NULL, *to_room = NULL;
	
	if (victim == NULL)
		return;
	
	switch (spellnum) {
		case SPELL_CURE_BLIND:
		case SPELL_HEAL:
			spell = SPELL_BLINDNESS;
			to_vict = "Riacquisti la vista!";
			to_room = "Gli occhi di $n brillano per un momento.";
			break;
		case SPELL_BURNING_HANDS:
			spell = SPELL_CHILL_TOUCH;
			to_vict = "L'asfissiante sensazione di gelo ti abbandona.";
			to_room = "La pelle di $n riacquista colore.";
			break;
		case SPELL_CHILL_TOUCH:
			spell = SPELL_BURNING_HANDS;
			to_vict = "Smetti di bruciare come una candela!";
			to_room = "$n smette di bruciare e rotolarsi come gli warg infiammati da Gandalf.";
			break;
		case SPELL_REMOVE_POISON:
			spell = SPELL_POISON;
			to_vict = "Una sensazione di calore ti pervade!";
			to_room = "$n sembra piu' in salute.";
			break;
		case SPELL_REMOVE_CURSE:
			spell = SPELL_CURSE;
			to_vict = "Ti senti un po' meno sfortunato...";
			break;
			
		default:
			sprintf(buf, "SYSERR: unknown spellnum %d passed to mag_unaffects", spellnum);
			log(buf);
			return;
			break;
	}
	
	if (!affected_by_spell(victim, SPELLSKILL, spell)) {
		send_to_char(NOEFFECT, ch);
		return;
	}
	
	affect_from_char(victim, SPELLSKILL, spell);
	if (to_vict != NULL)
		act(to_vict, FALSE, victim, 0, ch, TO_CHAR);
	if (to_room != NULL)
		act(to_room, TRUE, victim, 0, ch, TO_ROOM);
	if (spellnum == SPELL_REMOVE_POISON)
		check_regen_rates(victim);	/* speed up regen rate immediately */
}


void mag_alter_objs(int level, struct char_data * ch, struct obj_data * obj,
	int spellnum, int savetype)
{
	char *to_char = NULL;
	char *to_room = NULL;
	
	if (obj == NULL)
		return;
	
	switch (spellnum) {
		case SPELL_BLESS:
			if (!IS_OBJ_STAT(obj, ITEM_BLESS) &&
				(GET_OBJ_WEIGHT(obj) <= 5 * GET_LEVEL(ch))) {
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_BLESS);
				to_char = "$p glows briefly.";
			}
			break;
		case SPELL_CURSE:
			if (!IS_OBJ_STAT(obj, ITEM_NODROP)) {
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_NODROP);
				if ((GET_OBJ_TYPE(obj) == ITEM_WEAPON) || (GET_OBJ_TYPE(obj)==ITEM_WEAPON_2HANDS))
					GET_OBJ_VAL(obj, 2)--;
				to_char = "$p briefly glows red.";
			}
			break;
		case SPELL_INVISIBLE:
			if (!IS_OBJ_STAT(obj, ITEM_NOINVIS | ITEM_INVISIBLE)) {
				SET_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);
				to_char = "$p svanisce.";
			}
			break;
		case SPELL_POISON:
			if (((GET_OBJ_TYPE(obj) == ITEM_DRINKCON) ||
					(GET_OBJ_TYPE(obj) == ITEM_FOUNTAIN) ||
					(GET_OBJ_TYPE(obj) == ITEM_FOOD)) && !GET_OBJ_VAL(obj, 3)) {
				GET_OBJ_VAL(obj, 3) = 1;
				to_char = "$p steams briefly.";
			}
			break;
		case SPELL_REMOVE_CURSE:
			if (IS_OBJ_STAT(obj, ITEM_NODROP)) {
				REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
				if ((GET_OBJ_TYPE(obj) == ITEM_WEAPON) || (GET_OBJ_TYPE(obj)==ITEM_WEAPON_2HANDS))
					GET_OBJ_VAL(obj, 2)++;
				to_char = "$p briefly glows blue.";
			}
			break;
		case SPELL_REMOVE_POISON:
			if (((GET_OBJ_TYPE(obj) == ITEM_DRINKCON) ||
					(GET_OBJ_TYPE(obj) == ITEM_FOUNTAIN) ||
					(GET_OBJ_TYPE(obj) == ITEM_FOOD)) && GET_OBJ_VAL(obj, 3)) {
				
				GET_OBJ_VAL(obj, 3) = 0;
				to_char = "$p steams briefly.";
			}
			break;
	}
	
	if (to_char == NULL)
		send_to_char(NOEFFECT, ch);
	else
		act(to_char, TRUE, ch, obj, 0, TO_CHAR);
	
	if (to_room != NULL)
		act(to_room, TRUE, ch, obj, 0, TO_ROOM);
	else if (to_char != NULL)
		act(to_char, TRUE, ch, obj, 0, TO_ROOM);
	
}



void mag_creations(int level, struct char_data * ch, int spellnum)
{
	struct obj_data *tobj;
	int z=0;
	
	if (ch == NULL)
		return;
	level = MAX(MIN(level, LVL_IMPL), 1);
	
	switch (spellnum) {
		case SPELL_CREATE_FOOD:
			z = 10;
			break;
			
		case SPELL_CREATE_LIGHT:
			z = 18609;
			break;
		case SPELL_CONTINUAL_LIGHT:
			z = 11;
			break; 
		case SPELL_MATER_WEAP:
			if (GET_LEVEL(ch)<40)  {
				z=1213;
				break;
			}
			else z=1222;
			break;
		case SPELL_MATER_ARMOUR:
			if (GET_LEVEL(ch)<40) { 
				z = 1212;
				break;
			}
			else z=1221;
			break;
			
		default:
			send_to_char("Spell unimplemented, it would seem.\r\n", ch);
			return;
			break;
	}
	
	if (!(tobj = read_object(z, VIRTUAL))) {
		send_to_char("I seem to have goofed.\r\n", ch);
		sprintf(buf, "SYSERR: spell_creations, spell %d, obj %d: obj not found",
			spellnum, z);
		log(buf);
		return;
	}
	obj_to_char(tobj, ch);
	act("$n crea $p.", FALSE, ch, tobj, 0, TO_ROOM);
	act("Crei $p.", FALSE, ch, tobj, 0, TO_CHAR);
	load_otrigger(tobj); //Orione
}
