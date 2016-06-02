/* ************************************************************************
*   File: spells.c          35                          Part of CircleMUD *
*  Usage: Implementation of "manual spells".  Circle 2.2 spell compat.    *
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
#include "comm.h"
#include "spells.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "wilderness.h"
//#include "olc.h"
#include "clan.h"
#include "mestieri.h"
#include "dg_scripts.h"	//Orione

extern struct room_data *world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
//extern struct cha_app_type cha_app[];
extern struct int_app_type int_app[];
extern struct index_data *obj_index;
extern struct obj_data * obj;
extern struct descriptor_data *descriptor_list;
extern struct zone_data *zone_table;
extern struct abil_info_type abil_info[];
extern struct spell_info_type skill_info[];

extern int mini_mud;
extern int pk_allowed;

extern struct default_mobile_stats *mob_defaults;
extern char weapon_verbs[];
extern int *max_ac_applys;
extern struct apply_mod_defaults *apmd;
extern void generic_unstow(struct char_data *ch);
extern int limite_follower(struct char_data *ch);


void clearMemory(struct char_data * ch);
void act(char *str, int i, struct char_data * c, struct obj_data * o,
	void *vict_obj, int j);

void damage(struct char_data * ch, struct char_data * victim, int damage, int weapontype);

void weight_change_object(struct obj_data * obj, int weight);
void add_follower(struct char_data * ch, struct char_data * leader);
int mag_savingthrow(struct char_data * ch, struct char_data * caster, int type, int abil, int modifier);
ACMD(do_flee);
ACMD(do_write_magic);
ACMD(do_incanta);

// Extern Function
int mag_manacost(struct char_data * ch, int spellnum);

// Internal vars
static room_rnum *wild_tempestosa; // rnum delle stanze dove e' in corso il temporale causato da control weather
static int stanze_tempestose;

/*
 * Special spells appear below.
 */

void boot_tempeste(void)
{
	stanze_tempestose = 0;
	wild_tempestosa = (int *) malloc (sizeof (int));
}

void tempeste_update(void)
{
	register int i;
	struct char_data *ch;
	int caso;

	for (i = 0; i < stanze_tempestose; i++){
		for (ch = world[wild_tempestosa[i]].people; ch; ch = ch->next_in_room){
			caso = number (0, 1000);

			if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
				continue;

			if (caso < 7){
				if (IS_NPC(ch))
					continue;
				mag_affects (70, ch, ch, DISEASE_RAFFREDDORE, 0);
			}
			else if (caso < 17){
				act ("&3&bUna secca folgore colpisce $n in pieno!&0", FALSE, ch, 0, 0, TO_ROOM);
				act ("&3&bSenti un botto, poi una luce. Sei stato colpito da un fulmine, che male!!&0", FALSE, ch, 0, 0, TO_CHAR);
				damage_nuovo (ch, ch, 300, TYPE_ROOM_DAMAGE_ARIA, FALSE, DANNO_ELETTRICITA);
			}
			else if (caso < 70){
				act ("&7&bUn bagliore illumina per un istante la terra attorno a te. Un fulmine e' caduto non lontano da qui!&0", TRUE, ch, 0, 0, TO_CHAR);
				act ("&7&bUn bagliore illumina per un istante la terra attorno a te. Un fulmine e' caduto non lontano da qui!&0", TRUE, ch, 0, 0, TO_ROOM);
			}
			else if (caso < 123){
				act ("&9Un rombo ti scuote improvvisamente. Therion sta scatenando tuoni e fulmini!&0", FALSE, ch, 0, 0, TO_CHAR);
				act ("&9Un rombo ti scuote improvvisamente. Therion sta scatenando tuoni e fulmini!&0", FALSE, ch, 0, 0, TO_ROOM);
			}
			else
				send_to_char ("Non ti succede nulla di male.\r\n", ch);
		}
	}
}

int is_tempestosa (room_rnum rm)
{
	register int i;

	if (!IS_WILD(rm))
		return 0;

	for (i = 0; i < stanze_tempestose; i++)
		if (rm == wild_tempestosa[i])
			return 1;

	return 0;
}

ASPELL(spell_control_weather)
{
	register int i, j, k, l;
	int zone_number, migliaia, unita, attivazione = 1;
	room_vnum vnum_number;
	room_rnum rnum_number;

	if (!ch)
		return;
	if (GET_POS(ch) < POS_STANDING)
		return;

	if (!IS_WILD(ch->in_room)){ // Questo comprende wilderness e miniwild
		send_to_char ("Vorresti scatenare una tempesta dentro una citta'??\r\n", ch);
		return;
	}

	for (i = 0; i < stanze_tempestose; i++)
		if (ch->in_room == wild_tempestosa[i]){
			send_to_char ("Ti impegni a far terminare la tempesta sopra di te...\r\n", ch);
			attivazione = 0;
		}
	if (!attivazione)
		send_to_char ("Scateni una tempesta di incredibili proporzioni!\r\n", ch);

	if (attivazione){
		if (IS_MINIWILD(ch->in_room)){
			zone_number = world[ch->in_room].number / 10000;
			for (j = 40; j < 57; j++)
				for (k = 40; k < 65; k++){
					vnum_number = zone_number * 10000 + j * 100 + k;
					rnum_number = real_room (vnum_number);
					for (i = 0; i < stanze_tempestose && wild_tempestosa[i] != rnum_number; i++);	
					if (i == stanze_tempestose){
						stanze_tempestose++;
						wild_tempestosa = (int *) realloc (wild_tempestosa, sizeof (int) * stanze_tempestose);
						wild_tempestosa[stanze_tempestose-1] = rnum_number;
					}
				}
			return;
		}

	/* A questo punto deve essere in wilderness */

		migliaia = (world[ch->in_room].number / 1000) % 1000;
		unita = world[ch->in_room].number % 1000;

		for (j = MAX(400, migliaia - 10); j < MIN(600, migliaia + 10); j++)
			for (k = MAX(400, unita - 10); k < MIN(600, unita + 10); k++){
				vnum_number = 1000000 + j * 1000 + k;
				rnum_number = real_room(vnum_number);
				for (i = 0; i < stanze_tempestose && wild_tempestosa[i] != rnum_number; i++);
				if (i == stanze_tempestose){
					stanze_tempestose++;
					wild_tempestosa = (int *) realloc (wild_tempestosa, sizeof (int) * stanze_tempestose);
					wild_tempestosa[stanze_tempestose-1] = rnum_number;
				}
			}

		return;
	}

	/* A questo punto deve essere per forza un control weather per togliere la tempesta */

	if (IS_MINIWILD(ch->in_room)){
		zone_number = world[ch->in_room].number / 10000;
		for (j = 40; j < 57; j++)
			for (k = 40; k < 65; k++){
				vnum_number = zone_number * 10000 + j * 100 + k;
				rnum_number = real_room (vnum_number);
				for (i = 0; i < stanze_tempestose && wild_tempestosa[i] != rnum_number; i++);	
				if (i < stanze_tempestose){
					for (l = i; l < stanze_tempestose-1; l++)
						wild_tempestosa[l] = wild_tempestosa[l+1];
					stanze_tempestose--;
					wild_tempestosa = (int *) realloc (wild_tempestosa, sizeof (int) * stanze_tempestose);
				}
			}
		return;
	}

	/* A questo punto deve essere in wilderness */

	migliaia = (world[ch->in_room].number / 1000) % 1000;
	unita = world[ch->in_room].number % 1000;

	for (j = MAX(400, migliaia - 10); j < MIN(600, migliaia + 10); j++)
		for (k = MAX(400, unita - 10); k < MIN(600, unita + 10); k++){
			vnum_number = 1000000 + j * 1000 + k;
			rnum_number = real_room(vnum_number);
			for (i = 0; i < stanze_tempestose && wild_tempestosa[i] != rnum_number; i++);
			if (i < stanze_tempestose){
				for (l = i; l < stanze_tempestose-1; l++)
					wild_tempestosa[l] = wild_tempestosa[l+1];
				stanze_tempestose--;
				wild_tempestosa = (int *) realloc (wild_tempestosa, sizeof (int) * stanze_tempestose);
			}
		}
}

void costo_fly(void)
{
	struct char_data *i;
	int j;

	for (i = character_list; i; i = i->next){
		if (GET_LEVEL(i) >= LVL_AVATAR)
			continue;
		if (!IS_NPC (i) && AFFECTED(i, AFF_FLYING, BITV_FLYING)){
			alter_mana (i, MAX(1, (int) GET_MAX_MANA(i)/100));
			if (GET_MANA(i) <= 0){
				GET_MANA(i) = 0;
				send_to_char ("Le tue energie sono insufficienti per continuare il volo.\r\n", i);
				affect_from_char (i, SPELLSKILL, SPELL_FLY);
				for (j = 0; j < NUM_WEARS; j++){
					if (!(GET_EQ(i, j)))
						continue;
					if (IS_SET (GET_EQ(i, j)->obj_flags.bitvector[BITV_FLYING], AFF_FLYING)){
						act ("Sei costretto a toglierti $p prima che ti prosciughi anche le ultime forze!", FALSE, i, GET_EQ (i, j), NULL, TO_CHAR);
						obj_to_char(unequip_char(i, j), i);
					}
				}
			}
		}
	}
}

ASPELL(spell_chill_touch)
{
	if (!(ch || victim))
		return;
	if (GET_POS(victim) < POS_SLEEPING || GET_POS(ch) < POS_FIGHTING)
		return;

	if (AFFECTED(victim, AFF_BURNED, BITV_BURNED))
		mag_unaffects(level, ch, victim, SPELL_CHILL_TOUCH, SAVING_SPELL);
	else
		mag_affects(level, ch, victim, SPELL_CHILL_TOUCH, SAVING_SPELL);
}


ASPELL(spell_burning_hands)
{
	if (!(ch || victim))
		return;
	if (GET_POS(victim) < POS_SLEEPING || GET_POS(ch) < POS_FIGHTING)
		return;

	if (AFFECTED(victim, AFF_CHILLED, BITV_CHILLED))
		mag_unaffects(level, ch, victim, SPELL_BURNING_HANDS, SAVING_SPELL);
	else
		mag_affects(level, ch, victim, SPELL_BURNING_HANDS, SAVING_SPELL);

}


ASPELL(spell_dispel_magic)
{
	struct affected_type *af, *next;
	
	if (!victim) {
		send_to_char("Questa magia non ha effetto sugli oggetti!",ch);
		return;
	}
	
	if (ch == NULL)
		victim = ch;
	
	if (mag_savingthrow(victim, ch, 1, SAVING_NONE, 0)&&(victim != ch)) {
		
		sprintf(buf, "La magia non ha effetto su %s!\r\n", GET_NAME(victim));
		act(buf, TRUE, ch, 0, 0, TO_ROOM);
		send_to_char("La tua magia non ha effetto!\r\n", ch);
		
		return;
		
		
	} else 
	if (IS_NPC(victim)) {
		if (AFF_FLAGGED(victim, AFF_SENSE_LIFE)){
			REMOVE_BIT(AFF_FLAGS(victim), AFF_SENSE_LIFE); 
			affect_from_char(victim, SPELLSKILL, SPELL_SENSE_LIFE); }
		if (AFF_FLAGGED(victim, AFF_SANCTUARY)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_SANCTUARY);
			affect_from_char(victim, SPELLSKILL, SPELL_SANCTUARY); }
		if (AFF_FLAGGED(victim, AFF_SHIELD)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_SHIELD);
			affect_from_char(victim, SPELLSKILL, SPELL_SHIELD); }
		if (AFF_FLAGGED(victim, AFF_BLIND)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_BLIND); }
		if (AFF_FLAGGED(victim, AFF_INVISIBLE)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_INVISIBLE);
			affect_from_char(victim, SPELLSKILL, SPELL_INVISIBLE); }
		if (AFF_FLAGGED(victim, AFF_DETECT_ALIGN)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_DETECT_ALIGN);
			affect_from_char(victim, SPELLSKILL, SPELL_DETECT_ALIGN); }
		if (AFF_FLAGGED(victim, AFF_DETECT_INVIS)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_DETECT_INVIS);
			affect_from_char(victim, SPELLSKILL, SPELL_DETECT_INVIS); }
		if (AFF_FLAGGED(victim, AFF_DETECT_MAGIC))
			REMOVE_BIT(AFF_FLAGS(victim), AFF_DETECT_MAGIC);
		affect_from_char(victim, SPELLSKILL, SPELL_DETECT_MAGIC); }
	if (AFF_FLAGGED(victim, AFF_WATERWALK))
		REMOVE_BIT(AFF_FLAGS(victim), AFF_WATERWALK);
	affect_from_char(victim, SPELLSKILL, SPELL_WATERWALK); 
	if (AFF_FLAGGED(victim, AFF_GROUP)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_GROUP); }
	if (AFF_FLAGGED(victim, AFF_CURSE)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_CURSE);
		affect_from_char(victim, SPELLSKILL, SPELL_CURSE); } 
	if (AFF_FLAGGED(victim, AFF_INFRAVISION)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_INFRAVISION);
		affect_from_char(victim, SPELLSKILL, SPELL_INFRAVISION); }
	if (AFF_FLAGGED(victim, AFF_POISON)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_POISON); 
		affect_from_char(victim, SPELLSKILL, SPELL_POISON); }
	if (AFF_FLAGGED(victim, AFF_PROTECT_EVIL)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_PROTECT_EVIL);
		affect_from_char(victim, SPELLSKILL, SPELL_PROT_FROM_EVIL); }
	if (AFF_FLAGGED(victim, AFF_SLEEP)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_SLEEP);
		affect_from_char(victim, SPELLSKILL, SPELL_SLEEP); }
	if (AFF_FLAGGED(victim, AFF_PASSDOOR)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_PASSDOOR);
		affect_from_char(victim, SPELLSKILL, SPELL_WRAITHFORM); }
	if (AFF_FLAGGED(victim, AFF_FIRESHD)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_FIRESHD);
		affect_from_char(victim, SPELLSKILL, SPELL_FIRESHD); }
	if (AFF_FLAGGED(victim, AFF_CHARM)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_CHARM);
		affect_from_char(victim, SPELLSKILL, SPELL_CHARM); }
	if (AFF_FLAGGED(victim, AFF_FLYING)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_FLYING);
		affect_from_char(victim, SPELLSKILL, SPELL_FLY); }
	if (AFF_FLAGGED(victim, AFF_WATERBREATH)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_WATERBREATH);
		affect_from_char(victim, SPELLSKILL, SPELL_WATERBREATH); }
	if (AFF_FLAGGED(victim, AFF_PROT_FIRE)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_PROT_FIRE);
		affect_from_char(victim, SPELLSKILL, SPELL_PROT_FIRE); }
	if (AFF_FLAGGED(victim, AFF_DEATHDANCE)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_DEATHDANCE);
		affect_from_char(victim, SPELLSKILL, SPELL_DEATHDANCE); }
	if (AFF_FLAGGED(victim, AFF_MIRRORIMAGE)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_MIRRORIMAGE);
		affect_from_char(victim, SPELLSKILL, SPELL_MIRROR_IMAGE); }
	if (AFF_FLAGGED(victim, AFF_BLINK)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_BLINK);
		affect_from_char(victim, SPELLSKILL, SPELL_BLINK);}
	
	if (AFF_FLAGGED(victim, AFF_DET_SNEAK  )) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_DET_SNEAK  );
		affect_from_char(victim, SPELLSKILL, SPELL_DET_SNEAK);}
	if (AFF_FLAGGED(victim, AFF_PROTECT_LIGHT)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_PROTECT_LIGHT);
		affect_from_char(victim, SPELLSKILL, SPELL_PROTECT_LIGHT);}
	if (AFF_FLAGGED(victim, AFF_SILENCE)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_SILENCE);
		affect_from_char(victim, SPELLSKILL, SPELL_SILENCE); }
	if (AFF_FLAGGED(victim, AFF_DISGUISE)) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_DISGUISE);
		affect_from_char(victim, SPELLSKILL, SPELL_DISGUISE);}
	if (AFF_FLAGGED(victim, AFF_ANTIMAGIC )) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_ANTIMAGIC );
		affect_from_char(victim, SPELLSKILL, SPELL_ANTIMAGIC);}
	if (AFF_FLAGGED(victim, AFF_CONGIUNZIONE )) {
		REMOVE_BIT(AFF_FLAGS(victim), AFF_CONGIUNZIONE );
		affect_from_char(victim, SPELLSKILL, SPELL_CONGIUNZIONE);
		
	} else
	if (victim->affected) {
		for (af = victim->affected; af; af = next){
			next = af->next;
			if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
				affect_remove(victim, af);
		}
		send_to_char("Un breve lampo di luce di abbaglia!\r\n"
			"Ti senti leggermente differente.\r\n", victim);
		send_to_char("Tutti gli spells sono stati rimossi.\r\n", ch);
	} else {
		send_to_char("Hai rimosso tutti gli spells dalla vittima!\r\n", ch);
		return;
	}
	
  /* if (ch != victim)
  sprintf(buf,"%s ti sfiora le tempie e l'alone magico intorno a te svanisce.", GET_NAME(ch));
    else
  sprintf(buf,"Ti sfiori le tempie e l'alone magico intorno a te svanisce.");
  send_to_char(buf,victim);*/ 
}




ASPELL(spell_fear)
{
	struct char_data *target = (struct char_data *) victim;
	struct char_data *next_target;
	int rooms_to_flee = 0;
	
	
	
	if (ch == NULL)
		return;
	
	send_to_char("AHAHAHAH.....Emani un profondo senso di paura!\r\n", ch);
	act("AHAHAHAHHHH.... $n emana un profondo senso di paura!", TRUE, ch, 0, 0, TO_ROOM);
	
	for (target = world[ch->in_room].people; target; target = next_target) {
		next_target = target->next_in_room;
		
		if (target == NULL) 
			return;
		
		if (target == ch)
			continue;
		
		if (GET_LEVEL(target) >= LVL_IMMORT)
			continue;
		
		if (mag_savingthrow(target, ch, 1, SAVING_WIS, 0)) {
			sprintf(buf, "%s non ha paura di te!\r\n", GET_NAME(target));
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			send_to_char("La tua vittima non ha la paura che tu pensi che abbia!\r\n", ch);
			
			if (IS_NPC(target)  && !MOB_FLAGGED(target, MOB_SELVAGGINA))
				hit(target, ch, TYPE_UNDEFINED);
		}
		else {
			for(rooms_to_flee = level / 10; rooms_to_flee > 0; rooms_to_flee--) {
				send_to_char("Scappi dal terrore!\r\n", target);
				do_flee(target, "", 0, 0);
				stop_fighting(target);
			}
		}
	}
}



ASPELL(spell_locate_target)
{
	struct char_data *i = character_list;
	char name[MAX_INPUT_LENGTH];
	int j;
	
	sprintf(buf, "Chiudi gli occhi, porti le mani alle tempie per concentrarti, e inizi a cercare.\n\r");
    send_to_char(buf, ch);

    act("$n porta le mani alle tempie e chiude gli occhi!", TRUE, ch, 0, 0, TO_ROOM);
	
	if (!victim) {
		send_to_char("Percepisci solo te stesso avvolto in strane fiamme verdi.", ch);
		return;
	}  
	
	strcpy(name, fname(victim->player.name));
	j = level >> 1;
  
	for (i = character_list; i && (j > 0); i = i->next) {
		if (   (!IS_NPC(i) && !isname(name, i->player.name))
		    || (IS_NPC(i) && !isname(name, i->player.short_descr)) )
			continue;
		if (GET_LEVEL(i) < LVL_IMMORT) {
		  if (   i->in_room != NOWHERE 
		      && number(1, 10) > 5 )
			sprintf(buf, "Vedi %s in %s.\n\r", IS_NPC(i) ? i->player.short_descr : i->player.name,
				          world[i->in_room].name);
		  else
			sprintf(buf, "L'immagine di %s e' offuscata da una strana nebbia.\n\r",
				IS_NPC(i) ? i->player.short_descr : i->player.name);
		
		  CAP(buf);
		  send_to_char(buf, ch);
		j--;
	  }
	}
	
	if (j == level >> 1)
	  send_to_char("Non percepisci nulla.\n\r", ch);
}


ASPELL(spell_remove_curse)
{
	int i;
	bool found=FALSE;
	
	if (victim)
	{
		if(affected_by_spell(victim, SPELLSKILL, SPELL_CURSE))
		{
			found=TRUE;
			affect_from_char(victim, SPELLSKILL, SPELL_CURSE);
			send_to_char("Non ti senti piu' cosi' sfortunato.", victim);
		}
		for (i = 0; i < NUM_WEARS; i++) 
		{
			if (GET_EQ(victim, i))
			{
				obj=GET_EQ(victim,i);
				if (IS_OBJ_STAT(obj, ITEM_NODROP))
				{
					found=TRUE;
					REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
					if (GET_OBJ_TYPE(obj) == ITEM_WEAPON)
						GET_OBJ_VAL(obj, 2)++;
					sprintf(buf,"!!SOUND(Recall.wav)Un %s rapidamente diventa blu.\r\n",obj->name);  
					send_to_char(buf , victim);
					if (ch!=victim)
						send_to_char(buf , ch);
				}
			}      
		} // Fine ciclo for oggetti equippati
		
		for (obj = victim->carrying; obj; obj = obj->next_content)
		{
			if (IS_OBJ_STAT(obj, ITEM_NODROP))
			{
				found=TRUE;
				REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
				if (GET_OBJ_TYPE(obj) == ITEM_WEAPON)
					GET_OBJ_VAL(obj, 2)++;
				sprintf(buf,"!!SOUND(Recall.wav)Un %s rapidamente diventa blue.\r\n",obj->name);
				send_to_char(buf , victim);
				if (ch!=victim)
					send_to_char(buf , ch);
			}
		} //Fine ciclo for oggetti in inventario
	}   // Fine if(victim)
	
	else if (obj) 
	{
		for (i = 0; i < NUM_WEARS; i++)
		{
			if (GET_EQ(ch, i))
			{
				obj=GET_EQ(ch,i);
				if (IS_OBJ_STAT(obj, ITEM_NODROP))
				{
					found=TRUE;
					REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
					if (GET_OBJ_TYPE(obj) == ITEM_WEAPON)
						GET_OBJ_VAL(obj, 2)++;
					sprintf(buf,"!!SOUND(Recall.wav)Un %s rapidamente diventa blu.\r\n",obj->name);  
					send_to_char(buf , ch);
				}
			}
		} // Fine ciclo for oggetti equippati
		for (obj = ch->carrying; obj; obj = obj->next_content)
		{
			if (IS_OBJ_STAT(obj, ITEM_NODROP))
			{
				found=TRUE;
				REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
				if (GET_OBJ_TYPE(obj) == ITEM_WEAPON)
					GET_OBJ_VAL(obj, 2)++;
				sprintf(buf,"!!SOUND(Recall.wav)Un %s rapidamente diventa blu.\r\n",obj->name);
				send_to_char(buf , ch);
			}
		} //Fine ciclo for oggetti in inventario
	}
	
	if (!found)
		send_to_char("Non riesci a trovare niente a cui togliere la maledizione.", ch);
	
}

ASPELL(spell_create_water)
{
	int water;
	
	void name_to_drinkcon(struct obj_data * obj, int type);
	void name_from_drinkcon(struct obj_data * obj);
	
	if (ch == NULL || obj == NULL)
		return;
	level = MAX(MIN(level, LVL_IMPL), 1);
	
	if (GET_OBJ_TYPE(obj) == ITEM_DRINKCON) {
		if ((GET_OBJ_VAL(obj, 2) != LIQ_WATER) && (GET_OBJ_VAL(obj, 1) != 0)) {
			//name_from_drinkcon(obj);
			//GET_OBJ_VAL(obj, 2) = LIQ_SLIME;
			//name_to_drinkcon(obj, LIQ_SLIME);
			GET_OBJ_VAL(obj,1) = 0;
			GET_OBJ_VAL(obj,2) = 0;
			GET_OBJ_VAL(obj,3) = 0;
			name_from_drinkcon(obj);
			act("Il liquido contenuto in $p si trasforma in vapore.", FALSE, ch, obj, 0, TO_CHAR);
			act("Da $p di $n fuoriesce improvvisamente del vapore.", TRUE, ch, obj, 0, TO_ROOM);
		} else {
			water = MAX(GET_OBJ_VAL(obj, 0) - GET_OBJ_VAL(obj, 1), 0);
			if (water > 0) {
				GET_OBJ_VAL(obj, 2) = LIQ_CLEARWATER;
				GET_OBJ_VAL(obj, 1) += water;
				weight_change_object(obj, water);
				name_from_drinkcon(obj);
				name_to_drinkcon(obj, LIQ_CLEARWATER);
				act("$p is filled.", FALSE, ch, obj, 0, TO_CHAR);
			}
		}
	}
}


ASPELL(spell_feast)
{
	
	if (victim == NULL)
		victim = ch;
	
	GET_COND(victim, 1) = 100;
	GET_COND(victim, 2) = 100;
	victim->points.move = victim->points.max_move;
	affect_total(victim);
	if (ch != victim)
		sprintf(buf,"%s ti sfiora il ventre e ti senti sazio e riposato.", PERS(ch,victim));
	else
		sprintf(buf,"Ti sfiori il ventre e ti senti sazio e riposato.");
	send_to_char(buf,victim);
	
}

ASPELL(spell_refresh)
{
	
	if (victim == NULL)
		victim = ch;
	
	
	victim->points.move = victim->points.max_move;
	affect_total(victim);
	
	if (ch != victim)
		sprintf(buf,"!!SOUND(Recall.wav)%s indica i tuoi piedi e ti improvvisamente  riposato.", PERS(ch,victim));
	else
		sprintf(buf,"!!SOUND(Recall.wav)Ti indichi i piedi e ti senti improvvisamente riposato.");
	send_to_char(buf,victim);
}

ASPELL(spell_recall)
{
	extern room_rnum r_mortal_start_room[];
	
	if (victim == NULL || IS_NPC(victim))
		return;
	
	act("!!SOUND(Recall.wav)$n scompare.", TRUE, victim, 0, 0, TO_ROOM);
	generic_unstow(victim);
	char_from_room(victim);
	char_to_room(victim, r_mortal_start_room[GET_HOME(victim)]);
	if RIDING(victim)
	{
		char_from_room(RIDING(victim));
		char_to_room(RIDING(victim), r_mortal_start_room[GET_HOME(victim)]);
	}
	if TRANSPORTED(victim)
	{
		char_from_room(TRANSPORTED(victim));
		char_to_room(TRANSPORTED(victim), r_mortal_start_room[GET_HOME(victim)]);
	}	
	act("!!SOUND(Recall.wav)$n appare in mezzo alla stanza.", TRUE, victim, 0, 0, TO_ROOM);
	look_at_room(victim, 0);
	entry_memory_mtrigger(ch); //Orione
	greet_mtrigger(ch, -1);    //Orione
	greet_memory_mtrigger(ch);	//Orione
}


ASPELL(spell_teleport)
{
	room_rnum to_room;
	extern int top_of_world;
	
	if (victim == NULL)
		return;
	
	do {
		to_room = number(0, top_of_world);
	} while (ROOM_FLAGGED(to_room, ROOM_PRIVATE | ROOM_DEATH));
	
	act("!!SOUND(Recall.wav)$n scompare lentamente.",
		FALSE, victim, 0, 0, TO_ROOM);
	generic_unstow(victim);	  
	char_from_room(victim);
	char_to_room(victim, to_room);
	act("!!SOUND(Recall.wav)$n appare lentamente e prende forma.",
		FALSE, victim, 0, 0, TO_ROOM);
	if RIDING(victim)
	{
		char_from_room(RIDING(victim));
		char_to_room(RIDING(victim), to_room);
	}
	if TRANSPORTED(victim)
	{
		char_from_room(TRANSPORTED(victim));
		char_to_room(TRANSPORTED(victim), to_room);
	}	
	act("!!SOUND(Recall.wav)$n ti dissolvi.", FALSE, victim, 0, 0, TO_ROOM);
	look_at_room(victim, 0);
	entry_memory_mtrigger(ch); //Orione
	greet_mtrigger(ch, -1);    //Orione
	greet_memory_mtrigger(ch);	//Orione
}

#define SUMMON_FAIL "Fallisci.\r\n"

ASPELL(spell_summon)
{
	int num = 0, j, caso;
	char_data *i;

	if (ch == NULL || victim == NULL)
		return;
	
	if (GET_LEVEL(victim) > MIN(LVL_IMMORT - 1, level + 3)) {
		send_to_char(SUMMON_FAIL, ch);
		return;
	}

	if (MOB_FLAGGED(victim, MOB_AGGRESSIVE)) {
		act("Uno strano canto esce dalle tue labbra attraendo $N\r\n"
			"attraverso il tempo e lo spazio. Ma ti accorgi che $E e'\r\n"
			"un essere malvagio che potrebbe nuocerti. Riporti $M indietro.",
			FALSE, ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC(victim) && !PRF_FLAGGED(victim, PRF_SUMMONABLE)) {
		send_to_char("Una luce violacea ti avvolge qualcuno ha cercato di trasferirti magicamente.\r\n", victim);
		
		sprintf(buf, "Non riesci a trasferire magicamente %s .\r\n", GET_NAME(victim));
		send_to_char(buf, ch);
		
		sprintf(buf, "%s non riesce ad evocare %s a %s.",
			GET_NAME(ch), GET_NAME(victim), world[ch->in_room].name);
		mudlog(buf, BRF, LVL_IMMORT, TRUE);
		return;
	}
	
	
	if (MOB_FLAGGED(victim, MOB_NOSUMMON) || (IS_NPC(victim) && mag_savingthrow(victim, ch, SAVING_SPELL, SAVING_INT, 0))) {
		send_to_char(SUMMON_FAIL, ch);
		return;
	}
	
	/* Orione Inizio */
	if (!IS_NPC(victim) && IS_QUESTOR(victim)) {
            sprintf(buf, "Deve saper affrontare da %s il proprio destino!\r\n", GET_SEX(victim)==SEX_MALE ? "solo" : "sola");
	    send_to_char(buf, ch);
    	    sprintf(buf, "%s non riesce ad evocare %s che e' in quest.", GET_NAME(ch), GET_NAME(victim));
	    mudlog(buf, BRF, LVL_IMMORT, FALSE);
	    return;
	}
        if (!IS_NPC(ch) && IS_QUESTOR(ch)) {
	    sprintf(buf, "Devi saper affrontare da %s il tuo destino!\r\n", GET_SEX(ch)==SEX_MALE ? "solo" : "sola");
	    send_to_char(buf, ch);
	    sprintf(buf, "%s, in quest, non riesce ad evocare %s.", GET_NAME(ch), GET_NAME(victim));
	    mudlog(buf, BRF, LVL_IMMORT, FALSE);
	    return;
	}
	/* Orione Fine */
	
	/* Orione Inizio */
	if (!IS_NPC(victim) && AFF_FLAGGED(victim, AFF_PARALIZE)) {
	    sprintf(buf, "Fallisci perche' %s e' imprigionato da catene magiche.\r\n", GET_NAME(ch));
	    send_to_char(buf, ch);
		
	    sprintf(buf, "%s non riesce ad evocare %s a %s.", GET_NAME(ch), GET_NAME(victim), world[ch->in_room].name);
	    mudlog(buf, BRF, LVL_IMMORT, TRUE);
	    return;
	}
	/* Orione Fine */
	
	if (number(1, 90) >= GET_LEVEL(ch)) {
		send_to_char("Qualcosa disturba il tuo incantesimo.\r\n", ch);
		
		if (number(0,1)){
			spell_teleport(70, ch, ch, NULL);
			return;
		}

		for (i = character_list; i; i = i->next)
			num++;
		do{
			victim = character_list;
			caso = number(0, num-1);
			for (j = 0; j < caso; j++)
				victim = victim->next;
		} while (MOB_FLAGGED(victim, MOB_AGGRESSIVE) || !IS_NPC(victim) || MOB_FLAGGED(victim, MOB_NOSUMMON) || GET_LEVEL(victim) > 70);
	}

	if (affected_by_spell (victim, SPELLSKILL, DISEASE_PESTE))
		mag_affects (70, victim, ch, DISEASE_PESTE, 0);
						    			    			
	act("!!SOUND(Recall.wav)$n svanisce improvvisamente.", TRUE, victim, 0, 0, TO_ROOM);
	generic_unstow(victim);
	char_from_room(victim);
	char_to_room(victim, ch->in_room);
	if RIDING(victim)
	{
		char_from_room(RIDING(victim));
		char_to_room(RIDING(victim), ch->in_room);
	}
	if TRANSPORTED(victim)
	{
		char_from_room(TRANSPORTED(victim));
		char_to_room(TRANSPORTED(victim), ch->in_room);
	}	
	act("!!SOUND(Recall.wav)$n arriva immediatamente.", TRUE, victim, 0, 0, TO_ROOM);
	look_at_room(victim, 0);
	entry_memory_mtrigger(ch); //Orione
	greet_mtrigger(ch, -1);    //Orione
	greet_memory_mtrigger(ch);	//Orione
}



ASPELL(spell_locate_object)
{
	struct obj_data *i;
	char name[MAX_INPUT_LENGTH];
	int j;
	
	if (!obj) {
		send_to_char("Quale oggetto?",ch);
		return;
	}
	
	//strcpy(name, fname(obj->name));
	strcpy(name, obj->name);
	j = level >> 1;
	
	for (i = object_list; i && (j > 0); i = i->next) {
		if (!isname(name, i->name))
			continue;
		if (IS_SET(GET_OBJ_EXTRA(i), ITEM_NOLOCATE))
			sprintf(buf, "Un velo magico offusca la tua mente. Non riesci a individuare %s.\r\n", 
				i->short_description);
		else if (i->carried_by)
			sprintf(buf, "%s e' portato da %s.\n\r",
				i->short_description, PERS(i->carried_by, ch));
		else if (i->in_room != NOWHERE)
			sprintf(buf, "%s e' in %s.\n\r", i->short_description,
				world[i->in_room].name);
		else if (i->in_obj)
			sprintf(buf, "%s e' in %s.\n\r", i->short_description,
				i->in_obj->short_description);
		else if (i->worn_by)
			sprintf(buf, "%s e' addosso a %s.\n\r",
				i->short_description, PERS(i->worn_by, ch));
		else
			sprintf(buf, "La posizione di %s e' incerta.\n\r",
				i->short_description);
		
		CAP(buf);
		send_to_char(buf, ch);
		j--;
	}
	
	if (j == level >> 1)
		send_to_char("Non riesci a percepire niente.\n\r", ch);
}



ASPELL(spell_charm)
{
	struct affected_type af;
	
	if (victim == NULL || ch == NULL)
		return;
	if (!IS_NPC(victim) && GET_LEVEL(victim) >= LVL_IMPL) {
	  send_to_char("La tua magia si disperde nel nulla.\r\n", ch);
	  return;
	}
	if (victim == ch)
		send_to_char("Devi migliorare ancora.\r\n", ch);
	else if (!IS_NPC(victim) && !PRF_FLAGGED(victim, PRF_SUMMONABLE))
		send_to_char("Non sei in grado di controllare la sua mente!\r\n", ch);
	else if (IS_AFFECTED(victim, AFF_SANCTUARY))
		send_to_char("La tua vittima resiste perche' protetta dal sanctuary!\r\n", ch);
	else if (MOB_FLAGGED(victim, MOB_NOCHARM))
		send_to_char("La tua vittima resiste!\r\n", ch);
	else if (IS_AFFECTED(ch, AFF_CHARM))
		send_to_char("Non puoi farti seguire dalla vittima.\r\n", ch);
	else if (IS_AFFECTED(victim, AFF_CHARM) || level < GET_LEVEL(victim))
		send_to_char("Fallisci.\r\n", ch);
	else if (limite_follower(ch))
		send_to_char("Non hai abbastanza carisma per farlo.",ch);
    /* player charming another player - no legal reason for this */
    /* else if (!pk_allowed && !IS_NPC(victim))
       send_to_char("You fail - shouldn't be doing it anyway.\r\n", ch);*/
	else if (circle_follow(victim, ch))
		send_to_char("Spiacente. Questo non lo puoi fare.\r\n", ch);
	else if (mag_savingthrow(victim, ch, SAVING_PARA, 0, 0))
		send_to_char("La tua vittima resiste!\r\n", ch);	
	else {
		if (victim->master)
			stop_follower(victim);
		
		add_follower(victim, ch);
		
		af.natura = SPELLSKILL;
		af.type = SPELL_CHARM;
		
		if (GET_INT(victim))
			af.duration = 12 * 18 / GET_INT(victim);
		else
			af.duration = 12 * 18;
		
		af.modifier = 0;
		af.location = 0;
		af.bitvector[0] = AFF_CHARM;
		af.bitvector[1] = af.bitvector[2] = af.bitvector[3] = 0;
		affect_to_char(victim, &af);
		act("!!SOUND(Blessoff.wav)Non ti sembra che $n sia un buon maestro da seguire?", FALSE, ch, 0, victim, TO_VICT);
		if (IS_NPC(victim)) {
			SET_BIT(MOB_FLAGS(victim), MOB_NOKILL);
			REMOVE_BIT(MOB_FLAGS(victim), MOB_AGGRESSIVE);
			//REMOVE_BIT(MOB_FLAGS(victim), MOB_SPEC);
		}
	}
}



ASPELL(spell_identify)
{
	
	int i;
	int found=0;
	
	struct time_info_data age(struct char_data * ch);
	
	extern char *spells[];
	
	extern char *item_types[];
	extern char *extra_bits[];
	extern char *apply_types[];
	extern char *affected_bits[];
	extern char *affected_bits_1[];
	extern char *affected_bits_2[];
	extern char *affected_bits_3[];
	extern struct attack_hit_type attack_hit_text[];
	extern int objlevel (struct obj_data * obj);
	
	if (obj)
	{
		//Prova obj non identificabili
		if(IS_OBJ_STAT(obj,ITEM_NO_IDENT)){
			send_to_char("Nonostante le tue arti magiche non riesci a sapere nulla di quest'oggetto\r\n.",ch);
			return;
		} 
		send_to_char("!!SOUND(Identify.wav)Con la magia impari:\r\n", ch);
		sprintf(buf, "Oggetto '%s', Tipo di Oggetto: ", obj->short_description);
		sprinttype(GET_OBJ_TYPE(obj), item_types, buf2);
		strcat(buf, buf2);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		
		sprintf(buf, "Livello minimo di utilizzo dell'oggetto:      %d\r\n",
			GET_OBJ_LEVEL(obj)>0 ? GET_OBJ_LEVEL(obj) : MAX(1, objlevel(obj)));
		sprintf(buf+strlen(buf), "Livello consigliato di utilizzo dell'oggetto: %d\r\n",
			objlevel(obj));
		sprintf(buf+strlen(buf), "Peso: %d, Valore: %d, Rent: %d\r\n",
			GET_OBJ_WEIGHT(obj), GET_OBJ_COST(obj), GET_OBJ_RENT(obj));
		send_to_char(buf, ch);
		
		if (obj->obj_flags.bitvector[0] || GET_OBJ_BITVECTOR(obj, 1) || GET_OBJ_BITVECTOR(obj, 2) || GET_OBJ_BITVECTOR(obj, 3))
		{
			send_to_char("Questo oggetto ti dara' le abilita' seguenti:  ", ch);
			sprintbit(obj->obj_flags.bitvector[0], affected_bits, buf);
			sprintbit(obj->obj_flags.bitvector[1], affected_bits_1, buf1);
			if (strcmp(buf1, "NOBITS ")){
				if (strcmp(buf, "NOBITS "))
					strcat(buf, buf1);
				else
					sprintf (buf, "%s", buf1);
			}
			sprintbit(obj->obj_flags.bitvector[2], affected_bits_2, buf1);
			if (strcmp(buf1, "NOBITS ")){
				if (strcmp(buf, "NOBITS "))
					strcat(buf, buf1);
				else
					sprintf (buf, "%s", buf1);
			}
			sprintbit(obj->obj_flags.bitvector[3], affected_bits_3, buf1);
			if (strcmp(buf1, "NOBITS ")){
				if (strcmp(buf, "NOBITS "))
					strcat(buf, buf1);
				else
					sprintf (buf, "%s", buf1);
			}
			strcat(buf, "\r\n");
			send_to_char(buf, ch);
		}
		send_to_char("L'oggetto e': ", ch);
		sprintbit(GET_OBJ_EXTRA(obj), extra_bits, buf);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		
		switch (GET_OBJ_TYPE(obj))
		{
			case ITEM_SCROLL:
			case ITEM_POTION:
				sprintf(buf, "Questo %s lancia i seguenti incantesimi: ", item_types[(int) GET_OBJ_TYPE(obj)]);
				
				if (GET_OBJ_VAL(obj, 1) >= 1)
					sprintf(buf, "%s %s", buf, spells[GET_OBJ_VAL(obj, 1)]);
				if (GET_OBJ_VAL(obj, 2) >= 1)
					sprintf(buf, "%s %s", buf, spells[GET_OBJ_VAL(obj, 2)]);
				if (GET_OBJ_VAL(obj, 3) >= 1)
					sprintf(buf, "%s %s", buf, spells[GET_OBJ_VAL(obj, 3)]);
				sprintf(buf, "%s\r\n", buf);
				send_to_char(buf, ch);
				break;
			case ITEM_WAND:
			case ITEM_STAFF:
				sprintf(buf, "Questo %s lancia il seguente incantesimo: ", item_types[(int) GET_OBJ_TYPE(obj)]);
				sprintf(buf, "%s %s\r\n", buf, spells[GET_OBJ_VAL(obj, 3)]);
				sprintf(buf+strlen(buf), "Ha %d caric%s al massimo and ne riman%s %d.\r\n",
					GET_OBJ_VAL(obj, 1), GET_OBJ_VAL(obj, 1) == 1 ? "a" : "he",
					(GET_OBJ_VAL(obj, 2) == 1) ? "e" : "gono", GET_OBJ_VAL(obj, 2));
				send_to_char(buf, ch);
				break;
			case ITEM_WEAPON:
				sprintf(buf, "I danni inferti sono  '%dd%d'", GET_OBJ_VAL(obj, 1),
					GET_OBJ_VAL(obj, 2));
				sprintf(buf+strlen(buf), " per un danno medio per turno di %.1f.\r\n",
					(((GET_OBJ_VAL(obj, 2) + 1) / 2.0) * GET_OBJ_VAL(obj, 1)));
				sprintf(buf+strlen(buf),"ARMA di Tipo:%s\n"
					,attack_hit_text[GET_OBJ_VAL(obj, 3)].weapon);
				send_to_char(buf, ch);
				break;
			case ITEM_ARMOR:
				sprintf(buf, "Il bonus AC e' %d\r\n", GET_OBJ_VAL(obj, 0));
				send_to_char(buf, ch);
				break;
		}
		found = FALSE;
		for (i = 0; i < MAX_OBJ_AFFECT; i++)
		{
			if ((obj->affected[i].location != APPLY_NONE) &&
				(obj->affected[i].modifier != 0))
			{
				if (!found)
				{
					send_to_char("Puo' avere effetti su di te in questo modo:\r\n", ch);
					found = TRUE;
				}
				sprinttype(obj->affected[i].location, apply_types, buf2);
				sprintf(buf, "   Effetto: %s Di %d\r\n", buf2, obj->affected[i].modifier);
				send_to_char(buf, ch);
			}
		}
		return;
	} 
	
	else if (victim)
	{		// victim
      /*sprintf(buf, "Name: %s\r\n", GET_NAME(victim));
    send_to_char(buf, ch);
    if (!IS_NPC(victim))
      {
      sprintf(buf, "%s is %d years, %d months, %d days and %d hours old.\r\n",
	      GET_NAME(victim), age(victim).year, age(victim).month,
	      age(victim).day, age(victim).hours);
      send_to_char(buf, ch);
      }
    sprintf(buf, "Height %d cm, Weight %d pounds\r\n",
	    GET_HEIGHT(victim), GET_WEIGHT(victim));
    sprintf(buf, "%sLevel: %d, Hits: %d, Mana: %d\r\n", buf,
	    GET_LEVEL(victim), GET_HIT(victim), GET_MANA(victim));
    sprintf(buf, "%sAC: %d, Hitroll: %d, Damroll: %d\r\n", buf,
	    GET_AC(victim), (affected_by_spell(victim, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(victim)), GET_DAMROLL(victim));
    sprintf(buf, "%sStr: %d/%d, Int: %d, Wis: %d, Dex: %d, Con: %d, Cha: %d\r\n",
	    buf, GET_STR(victim), GET_ADD(victim), GET_INT(victim),
	GET_WIS(victim), GET_DEX(victim), GET_CON(victim), GET_CHA(victim));
	send_to_char(buf, ch);*/
		send_to_char("Puoi identificare solamente oggetti!\r\n",ch);
	}
}


ASPELL(spell_enchant_weapon)
{
	extern int objlevel (struct obj_data * obj);
	int i,j;
	int livello;
	
	if (ch == NULL)
		return;
	
	if (obj == NULL)
	{
		send_to_char("Ahi, dovevi incantare un'arma!.\r\n", ch);
		return;
	}
	
	livello = (GET_OBJ_LEVEL(obj)>0 ? GET_OBJ_LEVEL(obj) : MAX(1, objlevel(obj)));
	
	if (GET_OBJ_TYPE(obj) != ITEM_WEAPON)
	{
		send_to_char("Solo le armi possono essere incantate.\r\n", ch);
		return;
	}
	
	if (IS_SET(GET_OBJ_EXTRA(obj), ITEM_MAGIC))
	{
		send_to_char("Un'arma magica non puo' essere incantata.\r\n", ch);
		return;
	}
	
	for (i = 0; i < MAX_OBJ_AFFECT; i++) {
		if (obj->affected[i].location == APPLY_NONE || obj->affected[i].location == APPLY_HITROLL)
			break;
	} 
	for (j = 0; j < MAX_OBJ_AFFECT; j++){
		if ((obj->affected[j].location == APPLY_NONE && j!=i) || obj->affected[j].location == APPLY_DAMROLL)
			break;
	}
	if (i == MAX_OBJ_AFFECT && j == MAX_OBJ_AFFECT){
		send_to_char ("L'arma e' gia' troppo potente perche' le tue misere arti magiche possano migliorarla\r\n", ch);
		return;
	}
	
	SET_BIT(GET_OBJ_EXTRA(obj), ITEM_MAGIC);
	
	if (i != MAX_OBJ_AFFECT){
		obj->affected[i].location = APPLY_HITROLL;
		obj->affected[i].modifier += 1;
		if (obj->affected[i].modifier == 0)
			obj->affected[i].location = APPLY_NONE;
	}
	
	if (j != MAX_OBJ_AFFECT){
		obj->affected[j].location = APPLY_DAMROLL;
		obj->affected[j].modifier += 1;
		if (obj->affected[j].modifier == 0)
			obj->affected[j].location = APPLY_NONE;
	}

	if (IS_GOOD(ch))
	{
		SET_BIT(GET_OBJ_EXTRA(obj), ITEM_ANTI_EVIL);
		act("!!SOUND(Magic4.wav)$p emette un bagliore bluastro.", FALSE, ch, obj, 0, TO_CHAR);
	}
	else if (IS_EVIL(ch))
	{
		SET_BIT(GET_OBJ_EXTRA(obj), ITEM_ANTI_GOOD);
		act("!!SOUND(Magic4.wav)$p emette un bagliore rossastro.", FALSE, ch, obj, 0, TO_CHAR);
	}
	else act("!!SOUND(Magic4.wav)$p emette un bagliore giallastro.", FALSE, ch, obj, 0, TO_CHAR);
	GET_OBJ_LEVEL(obj) = livello;
}

ASPELL(spell_detect_poison)
{
	if (victim) {
		if (victim == ch) {
			if (IS_AFFECTED(victim, AFF_POISON))
				send_to_char("You can sense poison in your blood.\r\n", ch);
			else
				send_to_char("You feel healthy.\r\n", ch);
		} else {
			if (IS_AFFECTED(victim, AFF_POISON))
				act("!!SOUND(Deviloff.wav)You sense that $E is poisoned.", FALSE, ch, 0, victim, TO_CHAR);
			else
				act("!!SOUND(Deviloff.wav)You sense that $E is healthy.", FALSE, ch, 0, victim, TO_CHAR);
		}
	}
	
	if (obj) {
		switch (GET_OBJ_TYPE(obj)) {
			case ITEM_DRINKCON:
			case ITEM_FOUNTAIN:				
			case ITEM_FOOD:
				if (GET_OBJ_VAL(obj, 3))
					act("!!SOUND(Deviloff.wav)I tuoi sensi dicono che $p ha tracce di veleno.",FALSE,ch,obj,0,TO_CHAR);
				else
					act("!!SOUND(Deviloff.wav)$p non reca tracce di veleno.", FALSE, ch, obj, 0,TO_CHAR);
				break;
			default:
				send_to_char("Questo oggetto non puo' essere avvelenato.\r\n", ch);
		}
	}
}

ASPELL(spell_disintegrate)
{
	struct obj_data *foolz_objs;
	int i;
    /* Note this is for disintegrating all items the vict carries also..
       may wish to comment this off if you feel it too powerful */
	for (i = 0; i < NUM_WEARS; i++) 
		if (GET_EQ(victim, i))
			unequip_char(victim, i);
	while (victim->carrying) {
		foolz_objs = victim->carrying;
		extract_obj(foolz_objs);
		act("!!SOUND(Distruzione.wav)L'equipaggiamento di $N si e' disintegrato!", FALSE, ch, 0, victim, TO_CHAR);
	}   
}
ASPELL(spell_relocate)
{
	if (ch == NULL || victim == NULL)
		return;
	
	if (GET_LEVEL(victim) > MIN(LVL_IMMORT - 1, level + 3)) {
		send_to_char(SUMMON_FAIL, ch);
		return;
	}
	if (MOB_FLAGGED(victim, MOB_NOSUMMON))
	{
		send_to_char(SUMMON_FAIL, ch);
		return;
	}
	if (!pk_allowed) {
		if (MOB_FLAGGED(victim, MOB_AGGRESSIVE)) {
			act("As the words escape your lips and a rift travels\r\n"
				"through time and space toward $N, you realize that $E is\r\n"
				"aggressive and might harm you, so you wisely close it.",
				FALSE, ch, 0, victim, TO_CHAR);
			return;
		}
		if (!IS_NPC(victim) && !PRF_FLAGGED(victim, PRF_SUMMONABLE) &&
			!PLR_FLAGGED(victim, PLR_KILLER)) {
			sprintf(buf, "%s just tried to relocate to you.\r\n"
				"%s failed because you have summon protection on.\r\n"
				"Type NOSUMMON to allow other players to relocate to you.\r\n",
				GET_NAME(ch),
				(ch->player.sex == SEX_MALE) ? "He" : "She");
			send_to_char(buf, victim);
			
			sprintf(buf, "You failed because %s has summon protection on.\r\n",
				GET_NAME(victim));
			send_to_char(buf, ch);
			
			sprintf(buf, "%s failed relocating to %s at %s.",
				GET_NAME(ch), GET_NAME(victim), world[ch->in_room].name);
			mudlog(buf, BRF, LVL_IMMORT, TRUE);
			return;
		}
	}
	
	act("!!SOUND(Vento.wav)$n apre un portale ed entra.", TRUE, ch, 0, 0, TO_ROOM);
	act("!!SOUND(Vento.wav)Apri un portale e ci entri.", FALSE, ch, 0, 0, TO_CHAR);
	char_from_room(ch);
	char_to_room(ch, victim->in_room);
	act("!!SOUND(Vento.wav)Si apre un portale e $n esce.", TRUE, ch, 0, 0, TO_ROOM);
	look_at_room(ch, 6);
}
ASPELL(spell_peace)
{
	struct char_data *temp;
	
	act("$n tenta di arrestare il combattimento.", TRUE, ch, 0, 0, TO_ROOM);
	act("Tenti di arrestare il combattimento.", FALSE, ch, 0, 0, TO_CHAR);
	

	if (IS_EVIL(ch)) {
		send_to_char ("Sogghigni pensando a quanto inutile possa essere il valore della pace in confronto al male cui ti sei votato.\r\n", ch);
		return;
	}
	for (temp = world[ch->in_room].people; temp; temp = temp->next_in_room)
		if (FIGHTING(temp) && !PLR_FLAGGED(temp,PLR_CONCENTRATO)) {
			stop_fighting(temp);
			if (IS_NPC(temp)&& PLR_FLAGGED(ch,PLR_THIEF)) {
				clearMemory(temp);
			}

			if (ch != temp) {
				act("!!SOUND(Magia.wav)$n smette di combattere.", TRUE, temp, 0, 0, TO_ROOM);
				act("!!SOUND(Magia.wav)Smetti di combattere.", TRUE, temp, 0, 0, TO_CHAR);
			}
		}
	return;
}

#define PORTAL 73
ASPELL(spell_portal)
{
  /* create a magic portal */
	struct obj_data *tmp_obj, *tmp_obj2;
	struct extra_descr_data *ed;
	struct room_data *rp, *nrp;
	struct char_data *tmp_ch = (struct char_data *) victim;
	int mag_materials(struct char_data * ch, int item0, int item1, int item2,
		int extract, int verbose);
	char buf[512];
	
	assert(ch);
	assert((level >= 0) && (level <= LVL_IMPL));
	
  /*    check target room for legality. */
	
	rp = &world[ch->in_room];
	tmp_obj = read_object(PORTAL, VIRTUAL);
	if (!rp || !tmp_obj) {
		send_to_char("The magic fails\n\r", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	if (IS_SET(rp->room_flags, ROOM_NOMAGIC)) {
		send_to_char("La magia di Eldritch ti ostacola.\n\r", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	if (IS_SET(rp->room_flags, ROOM_TUNNEL)) {
		send_to_char("Non c'e' spazio nella stanza!\n\r", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	if (!(nrp = &world[tmp_ch->in_room])) {
		char str[180];
		sprintf(str, "%s not in any room", GET_NAME(tmp_ch));
		log(str);
		send_to_char("Non trovi l'obbiettivo\r\n", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	if (ROOM_FLAGGED(tmp_ch->in_room, ROOM_NOMAGIC)) {
		send_to_char("Your target is protected against your magic.\r\n", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	if (IS_IN_WILD(ch)) {
		send_to_char("Questo incantesimo non puo' essere lanciato nella wilderness.\r\n", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	if (world[ch->in_room].zone != world[tmp_ch->in_room].zone) {
		send_to_char("La tua vittima e' troppo lontana...\r\n", ch);
		extract_obj(tmp_obj);
		return;
	}
	
	sprintf(buf, "Attraverso il portale puoi vedere %s", nrp->name);
	
	CREATE(ed , struct extra_descr_data, 1);
	ed->next = tmp_obj->ex_description;
	tmp_obj->ex_description = ed;
	CREATE(ed->keyword, char, strlen(tmp_obj->name) + 1);
	strcpy(ed->keyword, tmp_obj->name);
	ed->description = str_dup(buf);
	
	tmp_obj->obj_flags.value[0] = level/5;
	tmp_obj->obj_flags.value[1] = tmp_ch->in_room;
	
	obj_to_room(tmp_obj,ch->in_room);
	
	act("!!SOUND(Magia.wav)$p immediatamente appare.",TRUE,ch,tmp_obj,0,TO_ROOM);
	act("!!SOUND(Magia.wav)$p immediatamente appare.",TRUE,ch,tmp_obj,0,TO_CHAR);
	
/* Portal at other side */
	rp = &world[ch->in_room];
	tmp_obj2 = read_object(PORTAL, VIRTUAL);
	if (!rp || !tmp_obj2) {
		send_to_char("La magia fallisce.\n\r", ch);
		extract_obj(tmp_obj2);
		return;
	}
	sprintf(buf, "Dall'altra parte del portale puoi vedere %s", rp->name);
	
	CREATE(ed , struct extra_descr_data, 1);
	ed->next = tmp_obj2->ex_description;
	tmp_obj2->ex_description = ed;
	CREATE(ed->keyword, char, strlen(tmp_obj2->name) + 1);
	strcpy(ed->keyword, tmp_obj2->name);
	ed->description = str_dup(buf);
	
	tmp_obj2->obj_flags.value[0] = level/5;
	tmp_obj2->obj_flags.value[1] = ch->in_room;
	
	obj_to_room(tmp_obj2,tmp_ch->in_room);
	
	act("!!SOUND(Magia.wav)$p appare immediatamente.",TRUE,tmp_ch,tmp_obj2,0,TO_ROOM);
	act("!!SOUND(Magia.wav)$p appare immediatamente.",TRUE,tmp_ch,tmp_obj2,0,TO_CHAR);
	
}

ASPELL(spell_call_dragon)
{
#define ASTRAL_TARGET_ROOM 6
	
	struct char_data * k;
	struct follow_type * f;
	room_rnum room_cast;
	bool stanza[1000]; //E' orribile, ma funziona...
	int i;
	
	if (ch == NULL)
		return;
	
	if (real_room(ASTRAL_TARGET_ROOM) == NOWHERE)
	{
		send_to_char("La stanza in mezzo alle nuvole e' chiusa al momento... Nessun drago ti ci puo' portare.\r\n", ch);
		mudlog("ASTRAL_TARGET_ROOM non esiste in spell_call_dragon", NRM, LVL_GOD, TRUE);
		return;
	}
	
	send_to_char("Evochi un potente drago e ci sali sopra, preparandoti a viaggiare.\r\n", ch);
	act("$n evoca un potente drago.", TRUE, ch, 0, 0, TO_ROOM);  
	
	if (!IS_AFFECTED(ch, AFF_GROUP))
	{
		send_to_char("Il drago ti porta in mezzo alle nuvole e poi scompare.\r\n", ch);
		act("$n sale in groppa al drago e scompare.", TRUE, ch, 0, 0, TO_ROOM);
		char_from_room(ch);
		char_to_room(ch, real_room(ASTRAL_TARGET_ROOM));
		act("$n arriva in groppa ad un drago maestoso, scende, e il drago scompare.", TRUE, ch, 0, 0, TO_ROOM);
		look_at_room(ch, 0);
	}    
	else 
	{
		memset(stanza, FALSE, 1000);
		room_cast=ch->in_room;
		
		if (ch->master) k = ch->master;
		else k = ch;
		
		if ( (AFF_FLAGGED(k, AFF_GROUP)) && (k->in_room==room_cast) && (GET_POS(k)>= POS_STANDING) )
		{
			stanza[0]=TRUE;
			act("$n sale in groppa al drago maestoso.", TRUE, k, 0, 0, TO_ROOM);
			if (k!=ch) send_to_char("Sali in groppa al drago maestoso, preparandoti a viaggiare.\r\n", k);
		} 
		i=1;
		for (f = k->followers; f; f = f->next)
			if ( (IS_AFFECTED(f->follower, AFF_GROUP)) && (f->follower->in_room==room_cast) &&
				(GET_POS(f->follower)>= POS_STANDING) )
			{
				if (i>=1000) {log("ERRORE in spell_call_dragon: piu' di mille giocatori gruppati ?");}
				else stanza[i++]=TRUE;
				act("$n sale in groppa al drago maestoso.", TRUE, f->follower, 0, 0, TO_ROOM);
				if (f->follower!=ch) send_to_char("Sali in groppa al drago maestoso preparandoti a viaggiare.\r\n", f->follower);
			}
			
		send_to_room("Un drago maestoso appare.\r\n", real_room(ASTRAL_TARGET_ROOM));
		
		if (stanza[0])
		{
			char_from_room(k);
			char_to_room(k, real_room(ASTRAL_TARGET_ROOM));
			send_to_char("Il drago maestoso ti trasporta coi tuoi compagni in mezzo alle nuvole.\r\n", k);
		} 
		i=1;
		for (f = k->followers; f; f = f->next)
			if (stanza[i++])
			{
				char_from_room(f->follower);
				char_to_room(f->follower, real_room(ASTRAL_TARGET_ROOM));
				send_to_char("Il drago maestoso ti trasporta coi tuoi compagni in mezzo alle nuvole.\r\n", f->follower);
			}
			
		send_to_room("Il drago maestoso scompare con tutti i suoi cavalieri.\r\n", room_cast);
		
		if (stanza[0])
		{
			act("$n scende dal drago.", TRUE, k, 0, 0, TO_ROOM);
			send_to_char("Scendi dal drago maestoso.\r\n", k);
		} 
		i=1;
		for (f = k->followers; f; f = f->next)
			if (stanza[i++])
			{
				act("$n scende dal drago.", TRUE, f->follower, 0, 0, TO_ROOM);
				send_to_char("Scendi dal drago maestoso.\r\n", f->follower);
			}
			
		send_to_room("Il drago maestoso scompare.\r\n", real_room(ASTRAL_TARGET_ROOM));
		
		if (stanza[0])
			look_at_room(k, 0); 
		i=1;
		for (f = k->followers; f; f = f->next)
			if (stanza[i++])
				look_at_room(f->follower, 0);
			
	}
	
#undef ASTRAL_TARGET_ROOM
}



ACMD(do_write_magic)
{
	int bits;
	char *arg1;
	char arg2[MAX_INPUT_LENGTH];
	long counter,spellnum = -1,pos=-1;
	struct obj_data *obj,*pen;
	struct char_data *tmp_char;
	extern struct spell_info_type spell_info[];
	extern char *spells[];
	void do_dream( struct char_data *victim,int kotime);
	
	if (!ch) return;
	
	arg1 = strtok(argument, "'");
	
	if (arg1 == NULL)
	{
		send_to_char("Quale incantesimo vuoi scrivere e su cosa?\r\n", ch);
		return;
	}
	
	arg1 = strtok(NULL, "'");
	if (arg1 == NULL)
	{
		send_to_char("I nomi degli incantesimi devono essere racchiusi dai simboli magici Sacri: '\r\n", ch);
		return;
	}
	argument = strtok(NULL, "\0");
	
	if (!argument) {
		send_to_char("Che incantesimo vuoi scrivere?\r\n",ch);
		return;
	}
	
	one_argument(argument, arg2);
	
	if (!*arg1)
	{
		send_to_char("Che incantesimo vuoi scrivere?\r\n",ch);
		return;
	}
	
	if (!*arg2)
	{
		send_to_char("Su cosa lo vuoi scrivere?\r\n",ch);
		return;
	}
	
	
	spellnum = find_skill_num(arg1);
	
	if ((spellnum < 1) || (spellnum > MAX_SPELLS)) {
		send_to_char("Questo incantesimo non esiste!\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) < spell_info[spellnum].min_level[(int) GET_CLASS(ch)]) {
		send_to_char("Non conosci ancora questo incantesimo!\r\n", ch);
		return;
	}
	if ((GET_SKILL(ch, spellnum) == 0) && (GET_LEVEL(ch)<LVL_QUEST_MASTER)) {
		send_to_char("Non hai molta esperienza con questo incantesimo.\r\n", ch);
		return;
	}
	
	if (IS_SET(spell_info[spellnum].targets, TAR_NOT_SELF)){
		send_to_char("Puoi scrivere incantesimi che si possono lanciare su te stesso.\r\n", ch);
		return;
	}
	
	if (IS_SET(spell_info[spellnum].targets, TAR_NOT_WRITE)) {
		send_to_char("Non puoi scrivere questo incantesimo su una pergamena.\r\n",ch);
		return;
	}
	
	if (GET_MANA(ch) < (mag_manacost(ch, spellnum) * 2)){
		send_to_char("Non hai abbastanza mana per poter creare l'incantesimo.\r\n",ch);
		return;
	}

	if ((spellnum == SPELL_CURE_LIGHT && GET_MANA (ch) < 20) || (spellnum == SPELL_CURE_SERIOUS && GET_MANA (ch) < 45) || (spellnum == SPELL_CURE_CRITIC && GET_MANA (ch) < 95)){
		send_to_char("Non hai abbastanza mana per poter creare l'incantesimo.\r\n",ch);
		return;
	}
	
	bits = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM , ch, &tmp_char, &obj);
	
	if (!obj)
	{
		sprintf(buf,"Non hai %s!\r\n", arg2);
		send_to_char(buf,ch);
		return;
	}
	
	if (GET_OBJ_TYPE(obj)!=ITEM_SCROLL)
	{
		send_to_char("Puoi scriverlo solo su pergamena!\r\n",ch);
		return;
	}
	
	if (((pen = GET_EQ(ch, WEAR_HOLD))==NULL) || (GET_OBJ_TYPE(pen)!=ITEM_PEN))
	{
		send_to_char("Devi avere una penna magica in mano per scrivere la pergamena!\r\n",ch);
		return;
	}
	
	if (!IS_SET(GET_OBJ_EXTRA(pen), ITEM_MAGIC))
	{
		send_to_char("Devi avere una penna incantata!\r\n",ch);
		return;
	}
	
	if (!CAN_LEARN_SKILL(ch, spellnum))
	{
		send_to_char("Non conosci nulla di questo incantesimo!\r\n",ch);
		return;
	}
	
	if (GET_SKILL(ch, spellnum) >= number(0,100)){
		//PEPPE PERGAMENE
		//for (counter = 1; counter < 4; counter++) {
		//commentata la riga sopra e assegnato a counter 1 si fa in modo che ogni perga contenga al piu
		//1 incantesimo ,inserendo nel for sopra al posto di 4 il numero n si fa in modo che ogni perga contenga
		//al piu' n-1 incantesimi
			counter=1;
			if (GET_OBJ_VAL(obj, counter)==0){
				pos=counter;
				//break;
			}
		//}
		
		if (pos==-1) {
			send_to_char("La pergamena non puo' contenere altri incantesimi.\r\n", ch);
			return;
		}
		
		GET_OBJ_VAL(obj,pos) = spellnum;
		GET_OBJ_VAL(obj, 0) = GET_LEVEL(ch);
		GET_MANA(ch) -= (mag_manacost(ch, spellnum) * 2);
		if (spellnum == SPELL_CURE_LIGHT)
			GET_MANA(ch) -= 20;
		if (spellnum == SPELL_CURE_SERIOUS)
			GET_MANA(ch) -= 45;
		if (spellnum == SPELL_CURE_CRITIC)
			GET_MANA(ch) -= 95;

		sprintf(buf,"!!SOUND(Magia.wav)Grazie alle tue arti magiche riesci a scrivere l'incantesimo %s sulla pergamena.\r\n",spells[spellnum]);
		send_to_char(buf,ch);
		
		/* modifica by ardanos */
		if(number(1, 100) < 13) { // 15% di possibilita' di svenire
			struct affected_type af[1];
			send_to_char("Lo sforzo mentale per la scrittura della pergamena ti fa perdere i sensi!\r\n", ch);
			act("$n sviene a causa dello sforzo mentale.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_STUNNED;
			af[0].natura =TYPE_UNDEFINED;
			af[0].type = TYPE_UNDEFINED;
			af[0].bitvector[0] = AFF_TRAMORTITO;
			af[0].bitvector[1] = 0;
			af[0].bitvector[2] = 0;
			af[0].bitvector[3] = 0;
			af[0].modifier = 0;
			af[0].location = APPLY_NONE;
			af[0].duration = 3;
			affect_to_char(ch, af);
		}
	} 
	else {
		GET_MANA(ch) -= mag_manacost(ch, spellnum);
		extract_obj(obj);
		sprintf(buf,"Nel processo di scrittura rovini la pergamena.\r\n");
		send_to_char(buf,ch);
	}
}


ACMD(do_incanta){
	
	char arg1[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	
	one_argument(argument, arg1);
	
	if (!ch) return;
	
	if ((GET_CLASS(ch) != CLASS_CLERIC) && (GET_CLASS(ch)!=CLASS_MAGIC_USER))
	{
		send_to_char("Solo i Pandion e i Cyrinic possono farlo?\r\n",ch);
		return;
	}
	
	if (!*arg1)
	{
		send_to_char("Che oggetto vuoi incantare?\r\n",ch);
		return;
	}
	
	if ((obj = GET_EQ(ch, WEAR_HOLD))==NULL)
	{
		send_to_char("Devi avere in mano un oggetto da incantare!\r\n",ch);
		return;
	}
	
	if (GET_OBJ_TYPE(obj)!=ITEM_PEN) {
		send_to_char("Puoi incantare solo i seguenti oggetti:\r\n"
			" - Penne per scrivere incantesimi\r\n",ch);
		return;
	}
	
	if ((GET_INT(ch) + GET_WIS(ch)) >= number(1,40)) {
		GET_MANA(ch) -= 5;
		SET_BIT(GET_OBJ_EXTRA(obj), ITEM_MAGIC);
		sprintf(buf,"!!SOUND(Magia.wav)&7%s risplende per un momento abbagliandoti la vista.&0", CAP(obj->short_description));
		act(buf, FALSE, ch, 0, 0, TO_CHAR);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
	} else {
		GET_MANA(ch) -= 3;
		obj_to_char((obj=unequip_char(ch,WEAR_HOLD)), ch);
		extract_obj(obj);
		sprintf(buf,"L'oggetto si frantuma magicamente.\r\n");
		send_to_char(buf,ch);
	}
}

ASPELL(spell_disguise)
{
	struct affected_type af;
	
	if (ch == victim) {
		send_to_char("A che servirebbe?\r\n",ch);
		return;
	}
	
	if (!victim) {
		send_to_char("Di chi vuoi assumere le sembianze?\r\n",ch);
		return;
	}
	
	if (GET_LEVEL(victim) >= LVL_AVATAR) {
		send_to_char("Non puoi travestirti come lui!\r\n",ch);
		return;
	}
	
	if (affected_by_spell(ch, SPELLSKILL, SPELL_DISGUISE)) {
		send_to_char(NOEFFECT, ch);
		return;
	}
	
	if (ch->player.namedisguise)
		free(ch->player.namedisguise);
	ch->player.namedisguise = strdup(GET_NAME(victim));
	ch->player.classdisguise = GET_CLASS(victim);
	ch->player.livdisguise = GET_LEVEL(victim);
	ch->player.sexdisguise = GET_SEX(victim);
	ch->player.clanrankdisguise = GET_CLAN_RANK(victim);
	ch->player.clandisguise = GET_CLAN(victim);
	ch->player.classtitledisguise= GET_CLASS_TITLE(victim);
	ch->player.long_descr_disguise= GET_DESCR(victim);
	
	free(ch->player.l_descr_disguise);
	if (IS_NPC(victim))
	  ch->player.l_descr_disguise = GET_L_DESCR(victim);
	  
	af.natura = SPELLSKILL;
	af.type = SPELL_DISGUISE;
	
	if (GET_WIS(ch))
		af.duration = 12 * 18 / GET_WIS(ch);
	else
		af.duration = 12 * 18;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector[0] = AFF_DISGUISE;
	af.bitvector[1] = af.bitvector[2] = af.bitvector[3] = 0;
	act("$n prende le sembianze di $N.", TRUE, ch, 0, victim, TO_NOTVICT);
	affect_to_char(ch, &af);
	sprintf(buf, "Prendi le sembianze di %s",ch->player.namedisguise );
	send_to_char(buf, ch);
	
}


ASPELL(spell_time_distortion)
{
	room_rnum destinazione;

	if (ch == NULL || victim == NULL)
		return;

	if (MOB_FLAGGED(victim, MOB_NOSUMMON) || (60 - GET_INT(victim)) < number(1, 100)){
		send_to_char(SUMMON_FAIL, ch);
		return;
	}

	if (GET_LEVEL(victim) > 70){
		send_to_char ("Certi esseri non amano essere sballottati in giro per lo spazio-tempo!", ch);
		return;
	}

	if (FIGHTING(victim))
		stop_fighting (victim);
	if ((FIGHTING(ch)) && FIGHTING(ch) == victim)
		stop_fighting (ch);

	victim->char_specials.distorto = 1;
	victim->char_specials.distorto_back = victim->in_room;

	if (world[real_room(ROOM_DISTORSIONE_1)].people == NULL)
		destinazione = real_room(ROOM_DISTORSIONE_1);
	else if (world[real_room(ROOM_DISTORSIONE_2)].people == NULL)
		destinazione = real_room(ROOM_DISTORSIONE_2);
	else
		destinazione = real_room(ROOM_DISTORSIONE_3);

	act("!!SOUND(Recall.wav)$n si dissolve per apparire chissa' dove.", TRUE, victim, 0, 0, TO_ROOM);
	generic_unstow(victim);
	char_from_room(victim);
	char_to_room(victim, destinazione);
	look_at_room(victim, 0);
}


void perform_distorsione(void)
{
	struct char_data *i;
	struct char_data *ultimo;
	
	/* Il pg "ultimo" e' necessario perche' quando il tempo scade e il pg se ne torna nella casella originale
		il next_in_room e' un pg che non c'entra nulla con quelli che invece vanno controllati. ultimo, invece,
		contiene sempre l'ultimo pg analizzato della stanza. Ad ogni giro, se i non se ne e' andato diventa
		ultimo, altrimenti ultimo resta il precedente e, andandosene i, ultimo->next_in_room cambia	*/

	for (ultimo = i = world[real_room(ROOM_DISTORSIONE_1)].people; i; i = ultimo->next_in_room){
		i->char_specials.distorto += 1;
		/* Dopo 2 o 3 turni che si sta nella stanza della distorsione si ha il 33% di possibilita'
		   di tornare indietro. Al quarto turno e' certo */
		if ((i->char_specials.distorto > 2 && i->char_specials.distorto < 5 && !(number(0,2))) || i->char_specials.distorto >= 5){
			i->char_specials.distorto = 0;
			char_from_room (i);
			char_to_room (i, i->char_specials.distorto_back);
			look_at_room(i, 0);
			act("$n ritorna dalla sua assenza spazio-temporale.", TRUE, i, NULL, NULL, TO_ROOM);
		}
		else
			ultimo = i;
	}

	for (ultimo = i = world[real_room(ROOM_DISTORSIONE_2)].people; i; i = ultimo->next_in_room){
		i->char_specials.distorto += 1;
		/* Dopo 2 o 3 turni che si sta nella stanza della distorsione si ha il 33% di possibilita'
		   di tornare indietro. Al quarto turno e' certo */
		if ((i->char_specials.distorto > 2 && i->char_specials.distorto < 5 && !(number(0,2))) || i->char_specials.distorto >= 5){
			i->char_specials.distorto = 0;
			char_from_room (i);
			char_to_room (i, i->char_specials.distorto_back);
			look_at_room(i, 0);
			act("$n ritorna dalla sua assenza spazio-temporale.", TRUE, i, NULL, NULL, TO_ROOM);
		}
		else
			ultimo = i;
	}

	for (ultimo = i = world[real_room(ROOM_DISTORSIONE_3)].people; i; i = ultimo->next_in_room){
		i->char_specials.distorto += 1;
		/* Dopo 2 o 3 turni che si sta nella stanza della distorsione si ha il 33% di possibilita'
		   di tornare indietro. Al quarto turno e' certo */
		if ((i->char_specials.distorto > 2 && i->char_specials.distorto < 5 && !(number(0,2))) || i->char_specials.distorto >= 5){
			i->char_specials.distorto = 0;
			char_from_room (i);
			char_to_room (i, i->char_specials.distorto_back);
			look_at_room(i, 0);
			act("$n ritorna dalla sua assenza spazio-temporale.", TRUE, i, NULL, NULL, TO_ROOM);
		}
		else
			ultimo = i;
	}
}


ACMD(do_scopriti){
	if (affected_by_spell(ch, SPELLSKILL, SPELL_DISGUISE)) {
		send_to_char("Togli il tuo camuffamento e torni ad essere quello di prima.", ch);
		act("$n toglie il suo camuffamento e torna a essere...", TRUE, ch, 0, 0, TO_ROOM);
		affect_from_char(ch, SPELLSKILL, SPELL_DISGUISE);
		act("$n...mmm, era camuffato!", TRUE, ch, 0, 0, TO_ROOM);
	}
}

ASPELL(spell_enchant_wood)
{
	extern int objlevel (struct obj_data * obj);
	int j;
	int livello,classe,livspell, riuscita,danno,carichemax,moltiplicatore,fattura, livmin;
	char *materiale;
	struct obj_data *objlibro;
	
	
	if (ch == NULL)
		return;
	
	if (obj == NULL)
	{
		send_to_char("Ahi, dovevi incantare un'oggetto!.\r\n", ch);
		return;
	}
	
	if (!(GET_OBJ_MAT_TYPE(obj) == ALBERI))
	{
		send_to_char("Solo oggetti di legno!!!\r\n", ch);
		return;
	}
	
	livello = (GET_OBJ_LEVEL(obj)>0 ? GET_OBJ_LEVEL(obj) : MAX(1, objlevel(obj)));
	
	
	if (IS_SET(GET_OBJ_EXTRA(obj), ITEM_MAGIC))
	{
		send_to_char("Un oggetto magico non puo' essere incantato.\r\n", ch);
		return;
	}
	
	if (GET_MANA(ch) < (GET_LEVEL(ch)*2))
	{
		send_to_char("Ti mancano le energie per farlo.\r\n", ch);
		return;
	}
	
	
	switch (GET_OBJ_TYPE(obj))
	{
		case ITEM_WAND:
		case ITEM_STAFF:
			objlibro = GET_EQ(ch, WEAR_HOLD);
			if (objlibro==NULL)
			{
				send_to_char("&1&bDevi avere in mano un libro magico per incantare l'oggetto!&0\r\n",ch);
				break;
			}
			
			
			if (GET_OBJ_TYPE(objlibro) == ITEM_LIBRO_MAGICO_S)
			{
				if (GET_OBJ_TYPE(obj) == ITEM_WAND)
				{
					send_to_char("&1&bIl libro magico che stai portando incanta solamente i bastoni!&0\r\n",ch);
					break;
				}
			}
			
			if (GET_OBJ_TYPE(objlibro) == ITEM_LIBRO_MAGICO_W)
			{
				if (GET_OBJ_TYPE(obj) == ITEM_STAFF)
				{
					send_to_char("&1&bIl libro magico che stai portando incanta solamente le bacchette&0\r\n",ch);
					break;
				}
			}
			
			materiale = alberi[GET_OBJ_MAT_NUM(obj)].nome;
			
			{
				if (!(strcmp(materiale, "betulla")))
				{
					classe = 1;
					livspell =number(10,20);
				}
				else if (!(strcmp(materiale, "ciliegio")))
				{
					classe = 4;
					livspell =number(30,50);
				}
				else if (!(strcmp(materiale, "pioppo")))
				{
					classe = 1;
					livspell =number(10,20);
				}
				else if (!(strcmp(materiale, "abete")))
				{
					classe = 3;
					livspell=number(20,40);
				}
				else if (!(strcmp(materiale, "noce")))
				{
					classe = 2;
					livspell =number(15,30);
				}
				else if (!(strcmp(materiale, "castagno")))
				{
					classe = 6;
					livspell =number(50,70);
				}
				else if (!(strcmp(materiale, "quercia")))
				{
					classe = 5;
					livspell =number(40,60);
				}
				else if (!(strcmp(materiale, "mogano")))
				{
					classe = 4;
					livspell =number(30,50);
				}
				else if (!(strcmp(materiale, "frassino")))
				{
					classe = 1;
					livspell =number(10,20);
				}
				else
				{
					send_to_char("&b&1MA CHE CACCHIO DI MATERIALE STAI INCANTANDO?&0\r\n", ch);
					classe = 0;
					livspell =number(0,1);
				}
				
				
				if (GET_OBJ_VAL(obj, 0)== 5)
				{
					fattura = 2;
				}
				else if (GET_OBJ_VAL(obj, 0)== 4)
				{
					fattura = 1;
				}
				else if (GET_OBJ_VAL(obj, 0)== 3)
				{
					fattura = 0;
				}
				else if (GET_OBJ_VAL(obj, 0)== 2)
				{
					fattura = 0;
				}
				else
				{
					fattura = -1;
				}
				
				if (classe < GET_OBJ_VAL(objlibro, 2))
				{
					send_to_char("&1&bLa qualita' della bacchetta non e' sufficientemente pregiata per il tipo di incantesimo.&0\r\n", ch);
					break;
				}
				
				
				if (!(strcmp(GET_OBJ_NAME(objlibro), "libro magico nero")))
				{
					danno= GET_HIT(ch)-GET_MAX_HIT(ch)-1;
					riuscita = 20;
					livspell = number (80,90);
					moltiplicatore=3;
					carichemax = number(1,4);
					
					if (fattura < 2)
					{
						if (GET_OBJ_TYPE(obj) == ITEM_WAND)
						{
							send_to_char("&1&bQuesto particolare incantesimo necessita di bacchette di massima qualita'.&0\r\n", ch);
							break;
						}
						else
						{
							send_to_char("&1&bQuesto particolare incantesimo necessita di bastoni di massima qualita'.&0\r\n", ch);
							break;
						}
					}
				}
				
				else if (!(strcmp(GET_OBJ_NAME(objlibro), "libro magico bianco")))
				{
					danno= GET_HIT(ch)-((GET_MAX_HIT(ch)/number(1,3))+1);
					riuscita = 20;
					livspell = number (80,90);
					moltiplicatore=3;
					carichemax = number(1,4);
					
					if (fattura < 2)
					{
						if (GET_OBJ_TYPE(obj) == ITEM_WAND)
						{
							send_to_char("&1&bQuesto particolare incantesimo necessita di bacchette di massima qualita'.&0\r\n", ch);
							break;
						}
						else
						{
							send_to_char("&1&bQuesto particolare incantesimo necessita di bastoni di massima qualita'.&0\r\n", ch);
							break;
						}
					}
				}
				
				else
					
				{
					danno= GET_HIT(ch)-(classe*10);
					riuscita = (3*(GET_SKILL(ch,SPELL_ENCHANT_WOOD)))/classe;
					carichemax = ((((GET_SKILL(ch,SPELL_ENCHANT_WOOD))*GET_LEVEL(ch))/1000)+(classe/2))+fattura;
					moltiplicatore=1;
				}
				
				
				if (riuscita <= (number(0,100)))
					
				{
					extract_obj(objlibro);
					extract_obj(obj);
					GET_HIT(ch) = danno;
					
					if (GET_HIT(ch) <= 0)
					{
						{
							if (GET_OBJ_TYPE(obj) == ITEM_WAND)
								send_to_char("&1&bL'incantesimo non riesce! Il libro e la bacchetta esplodono disintegrandoti.&0\r\n", ch);
							else
								send_to_char("&1&bL'incantesimo non riesce! Il libro ed il bastone esplodono disintegrandosi.&0\r\n", ch);
						}
						
						act("&1&b$n viene disintegrato dalla magia che stava sperimentando.&0", TRUE, ch, 0, 0, TO_ROOM);
						extract_char(ch);
						GET_POS(ch) = POS_DEAD;
						break;
					}
					
					else
						
					{
						if (GET_OBJ_TYPE(obj) == ITEM_WAND)
							send_to_char("&1&bL'incantesimo non riesce! Il libro e la bacchetta esplodono colpendoti in pieno viso.&0\r\n", ch);
						else
							send_to_char("&1&bL'incantesimo non riesce! Il libro ed il bastone esplodono colpendoti in pieno viso.&0\r\n", ch);
					}
					
					act("&1&b$n viene colpito in pieno volto da un esplosione.&0", TRUE, ch, 0, 0, TO_ROOM);
					break;
				}
				
				
				GET_OBJ_VAL(obj, 0) = livspell; /*Livello dello Spell*/
				GET_OBJ_VAL(obj, 1) = carichemax; /* Massimo numero di cariche */
				GET_OBJ_VAL(obj, 2) = GET_OBJ_VAL(obj, 1);/* Numero di cariche */
				GET_OBJ_VAL(obj, 3) = GET_OBJ_VAL(objlibro, 3);  /*Spell number*/
				GET_OBJ_COST(obj) = (((carichemax*alberi[GET_OBJ_MAT_NUM(obj)].val_comm*livspell)/150)+100)*moltiplicatore;
				GET_OBJ_RENT(obj) = GET_OBJ_COST(obj)/10;
				
				{
					livmin = (livspell + number(-5,5));
					if (livmin > 70)
					{
						livmin = (livspell - number(1,10));
						GET_OBJ_LEVEL(obj) = livmin;
					}
					else
						GET_OBJ_LEVEL(obj) = livmin;
					
				}
				
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_MAGIC);
				
				{
					if (GET_OBJ_TYPE(obj) == ITEM_WAND)
					{
						sprintf(buf, "Una bacchetta magica in %s e' abbandonata qui.", alberi[GET_OBJ_MAT_NUM(obj)].nome);
						obj->description = str_dup(buf);
						sprintf(buf, "una bacchetta magica di %s", skill_name(GET_OBJ_VAL(objlibro, 3)));
						obj->short_description = str_dup(buf);
						sprintf(buf, "bacchetta magica %s", alberi[GET_OBJ_MAT_NUM(obj)].nome);
						obj->name = str_dup(buf);
						send_to_char("&2&bLa bacchetta risplende mentre la magia passa attraverso il tuo corpo...&0\r\n", ch);
						act("&2&bUna bacchetta in mano a $n risplende magicamente...&0", TRUE, ch, 0, 0, TO_ROOM);
					}
					
					else
					{
						sprintf(buf, "Un bastone  magico in %s e' abbandonato qui.", alberi[GET_OBJ_MAT_NUM(obj)].nome);
						obj->description = str_dup(buf);
						sprintf(buf, "un bastone magico di %s", skill_name(GET_OBJ_VAL(objlibro, 3)));
						obj->short_description = str_dup(buf);
						sprintf(buf, "bastone magico %s", alberi[GET_OBJ_MAT_NUM(obj)].nome);
						obj->name = str_dup(buf);
						send_to_char("&2&bIl bastone risplende mentre la magia passa attraverso il tuo corpo...&0\r\n", ch);
						act("&2&bUn bastone in mano a $n risplende magicamente...&0", TRUE, ch, 0, 0, TO_ROOM);
					}
				}
				extract_obj(objlibro);
				send_to_char("&3&b...il libro magico, lentamente, scompare nel nulla.&0\r\n", ch);
				act("&3&b...mentre il libro che teneva sparisce in una nuvola argentea.&0", TRUE, ch, 0, 0, TO_ROOM);
			}
			break;
			
		case ITEM_TREASURE:
			
			{
				for (j = 0; j < MAX_OBJ_AFFECT; j++)
				{
					if (obj->affected[j].location == APPLY_NONE)
						break;
				}
			}
			
			if (GET_LEVEL(ch)<=10)
				if (AFF_FLAGGED(ch, AFF_DETECT_ALIGN))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_DETECT_ALIGN);
					affect_from_char(ch, SPELLSKILL, SPELL_DETECT_ALIGN);
				}
				
			if (GET_LEVEL(ch)>10)
				if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_DETECT_MAGIC);
					affect_from_char(ch, SPELLSKILL, SPELL_DETECT_MAGIC);
				}
				
			if (GET_LEVEL(ch)>20)
				if (AFF_FLAGGED(ch, AFF_WATERWALK))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_WATERWALK);
					affect_from_char(ch, SPELLSKILL , SPELL_WATERWALK);
				}
				
			if (GET_LEVEL(ch)>30)
				if (AFF_FLAGGED(ch, AFF_CURSE))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_CURSE);
					affect_from_char(ch, SPELLSKILL, SPELL_CURSE);
				}
				
			if (GET_LEVEL(ch)>40)
				if (AFF_FLAGGED(ch, AFF_PROTECT_EVIL))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_PROTECT_EVIL);
					affect_from_char(ch, SPELLSKILL, SPELL_PROT_FROM_EVIL);
				}
				
			if (GET_LEVEL(ch)>50)
				if (AFF_FLAGGED(ch, AFF_PROTECT_GOOD))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_DETECT_ALIGN);
					affect_from_char(ch, SPELLSKILL ,SPELL_DETECT_ALIGN);
				}
				
			if (GET_LEVEL(ch)>50)
				if (AFF_FLAGGED(ch, AFF_PROT_FIRE))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_PROT_FIRE);
					affect_from_char(ch, SPELLSKILL ,SPELL_PROT_FIRE);
				}
				
			if (GET_LEVEL(ch)>50)
				if (AFF_FLAGGED(ch, AFF_PROTECT_LIGHT))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_PROTECT_LIGHT);
					affect_from_char(ch, SPELLSKILL ,SPELL_PROTECT_LIGHT);
				}
				
			if (GET_LEVEL(ch)>60)
				if (AFF_FLAGGED(ch, AFF_SHIELD))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_SHIELD);
					affect_from_char(ch, SPELLSKILL ,SPELL_SHIELD);
				}
				
			if (GET_LEVEL(ch)>65)
				if (AFF_FLAGGED(ch, AFF_ANTIMAGIC))
				{
					SET_BIT(obj->obj_flags.bitvector[0] , AFF_ANTIMAGIC);
					affect_from_char(ch, SPELLSKILL , SPELL_ANTIMAGIC);
				}
				
				
			GET_MANA(ch) -= (GET_LEVEL(ch)*2);
			
			if (IS_GOOD(ch))
			{
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_ANTI_EVIL);
				act("!!SOUND(Magic4.wav)$p emette un bagliore bluastro.", FALSE, ch, obj, 0, TO_CHAR);
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_MAGIC);
			}
			else if (IS_EVIL(ch))
			{
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_ANTI_GOOD);
				act("!!SOUND(Magic4.wav)$p emette un bagliore rossastro.", FALSE, ch, obj, 0, TO_CHAR);
				SET_BIT(GET_OBJ_EXTRA(obj), ITEM_MAGIC);
			}
			else act("!!SOUND(Magic4.wav)$p emette un bagliore giallastro.", FALSE, ch, obj, 0, TO_CHAR);
			GET_OBJ_LEVEL(obj) = livello;
			SET_BIT(GET_OBJ_EXTRA(obj), ITEM_MAGIC);
			
			break;
			
		default:
			send_to_char("Questo oggetto non puo' essere incantato.\r\n", ch);
			
			
	}
}
