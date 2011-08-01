/* ************************************************************************
*   File: handler.c                                     Part of CircleMUD *
*  Usage: internal funcs: moving and finding chars/objs                   *
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
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "spells.h"
#include "dg_scripts.h"
#include "events.h"

/* external vars */
extern int top_of_world;
extern struct room_data *world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct descriptor_data *descriptor_list;
extern char *MENU;
extern int abilita_vector[][16];
extern room_rnum r_mortal_start_room[NUM_STARTROOMS +1];
/* external functions */
void free_char(struct char_data * ch);
void stop_fighting(struct char_data * ch);
void remove_follower(struct char_data * ch);
void clearMemory(struct char_data * ch);
extern int objlevel (struct obj_data * obj);
bool can_do_event(struct char_data *ch, long int comdtype, long pulse);
bool test_can_do_event(struct char_data *ch, long int comdtype, long pulse);
bool check_second_weapon(struct char_data *ch,struct obj_data *l_wielded);
void check_abil_obj(struct char_data *ch,struct obj_data *obj,bool wear);

void put_two_flags_to_aff (struct affected_type *af, int bitv1, long long int flag1, int bitv2, long long int flag2)
{
	if (bitv1 < 0 || bitv1 > 3 || bitv2 < 0 || bitv2 > 3){
		mudlog ("SYSERR: bitvector non validi passati a put_two_flags_to_aff", BRF, LVL_QUEST_MASTER, TRUE);
		return;
	}

	if (bitv1 == bitv2){
		af->bitvector[bitv1] |= flag1 | flag2;
		return;
	}

	af->bitvector[bitv1] |= flag1;
	af->bitvector[bitv2] |= flag2;
}

char *fname(char *namelist)
{
	static char holder[30];
	register char *point;
	
	for (point = holder; isalpha(*namelist); namelist++, point++)
		*point = *namelist;
	
	*point = '\0';
	
	return (holder);
}

#define GOLD_VNUM 72
#define WHITESPACE " \t"

int isname(char *str, char *namelist)
{
	char *newlist;
	char *curtok;
	
	if (!namelist)
	  return 0;
	
	// make a copy since strtok 'modifies' strings
	newlist = strdup(namelist); 
	
	for (curtok = strtok(newlist, WHITESPACE); curtok; curtok = strtok(NULL, WHITESPACE))
	if (curtok && is_abbrev(str, curtok)) {
		free(newlist);
		return 1;
	}
	free(newlist);
	return 0;
}



void affect_modify(struct char_data * ch, byte loc, sbyte mod, long long int bitv1, long long int bitv2, long long int bitv3, long long int bitv4, bool add)
{
	int maxabil;
	
	if (add) {
		SET_BIT(AFF_FLAGS_S(ch, 0), bitv1);
		SET_BIT(AFF_FLAGS_S(ch, 1), bitv2);
		SET_BIT(AFF_FLAGS_S(ch, 2), bitv3);
		SET_BIT(AFF_FLAGS_S(ch, 3), bitv4);
	} else {
		REMOVE_BIT(AFF_FLAGS_S(ch, 0), bitv1);
		REMOVE_BIT(AFF_FLAGS_S(ch, 1), bitv2);
		REMOVE_BIT(AFF_FLAGS_S(ch, 2), bitv3);
		REMOVE_BIT(AFF_FLAGS_S(ch, 3), bitv4);
		mod = -mod;
	}
	
	
	maxabil = (IS_NPC(ch) ? 25 : 18);
	
	switch (loc) {
		case APPLY_NONE:
			break;
			
		case APPLY_STR:
			GET_STR(ch) += mod;
			break;
		case APPLY_DEX:
			GET_DEX(ch) += mod;
			break;
		case APPLY_INT:
			GET_INT(ch) += mod;
			break;
		case APPLY_WIS:
			GET_WIS(ch) += mod;
			break;
		case APPLY_CON:
			GET_CON(ch) += mod;
			break;
		case APPLY_CHA:
			GET_CHA(ch) += mod;
			break;
			
		case APPLY_CLASS:
    /* ??? GET_CLASS(ch) += mod; */
			break;
			
		case APPLY_LEVEL:
    /* ??? GET_LEVEL(ch) += mod; */
			break;
			
		case APPLY_AGE:
			ch->player.time.birth -= (mod * SECS_PER_MUD_YEAR);
			break;
			
		case APPLY_CHAR_WEIGHT:
			GET_WEIGHT(ch) += mod;
			break;
			
		case APPLY_CHAR_HEIGHT:
			GET_HEIGHT(ch) += mod;
			break;
			
		case APPLY_MANA:
			GET_MAX_MANA(ch) += mod;
			break;
			
		case APPLY_HIT:
			GET_MAX_HIT(ch) += mod;
			break;
			
		case APPLY_MANA_REGEN:
			ch->char_specials.managain  += mod;
			break;
			
		case APPLY_HIT_REGEN:
			ch->char_specials.hitgain += mod;
			break;
			
		case APPLY_MOVE:
			GET_MAX_MOVE(ch) += mod;
			break;
			
		case APPLY_GOLD:
			break;
			
		case APPLY_EXP:
			break;
			
		case APPLY_AC:
			GET_AC(ch) += mod;
			break;
			
		case APPLY_HITROLL:
			if (!affected_by_spell (ch, SPELLSKILL, DISEASE_RAFFREDDORE))
				GET_HITROLL(ch) += mod;
			break;
			
		case APPLY_DAMROLL:
			GET_DAMROLL(ch) += mod;
			break;
			
		case APPLY_SAVING_PARA:
			GET_SAVE(ch, SAVING_PARA) += mod;
			break;
			
		case APPLY_SAVING_ROD:
			GET_SAVE(ch, SAVING_ROD) += mod;
			break;
			
		case APPLY_SAVING_PETRI:
			GET_SAVE(ch, SAVING_PETRI) += mod;
			break;
			
		case APPLY_SAVING_BREATH:
			GET_SAVE(ch, SAVING_BREATH) += mod;
			break;
			
		case APPLY_SAVING_SPELL:
			GET_SAVE(ch, SAVING_SPELL) += mod;
			break;
			
		case APPLY_RES_FUOCO:
			GET_RES_FUOCO(ch) += mod;
			break;
			
		case APPLY_RES_GHIACCIO:
			GET_RES_GHIACCIO(ch) += mod;
			break;
			
		case APPLY_RES_ELETTRICITA:
			GET_RES_ELETTRICITA(ch) += mod;
			break;
			
		case APPLY_RES_ACIDO:
			GET_RES_ACIDO(ch) += mod;
			break;
			
		case APPLY_RES_SHAARR:
			GET_RES_SHAARR(ch) += mod;
			break;

		case APPLY_RES_XHYPHYS:
			GET_RES_XHYPHYS(ch) += mod;
			break;

		case APPLY_RES_THERION:
			GET_RES_THERION(ch) += mod;
			break;

		case APPLY_RES_SILUE:
			GET_RES_SILUE(ch) += mod;
			break;
			
		case APPLY_RES_FISICO:
			GET_RES_FISICO(ch) += mod;
			break;
			
		default:
			sprintf(buf,"SYSERR: Unknown apply adjust attempt(handler.c, affect_modify)for %s #%d",GET_NAME(ch),GET_MOB_VNUM(ch));
			log(buf);
			break;
			
	} /* switch */
}



/* This updates a character by subtracting everything he is affected by */
/* restoring original abilities, and then affecting all again           */
void affect_total(struct char_data * ch)
{
	struct affected_type *af;
	int i, j;
	
	for (i = 0; i < NUM_WEARS; i++) {
		if (GET_EQ(ch, i))
			for (j = 0; j < MAX_OBJ_AFFECT; j++)
				affect_modify(ch, GET_EQ(ch, i)->affected[j].location,
					GET_EQ(ch, i)->affected[j].modifier,
					GET_EQ(ch, i)->obj_flags.bitvector[0], 
					GET_EQ(ch, i)->obj_flags.bitvector[1], 
					GET_EQ(ch, i)->obj_flags.bitvector[2], 
					GET_EQ(ch, i)->obj_flags.bitvector[3], FALSE);
	}
	
	
	for (af = ch->affected; af; af = af->next)
		affect_modify(ch, af->location, af->modifier, af->bitvector[0], af->bitvector[1], af->bitvector[2], af->bitvector[3], FALSE);
	
	ch->aff_abils = ch->real_abils;
	
	for (i = 0; i < NUM_WEARS; i++) {
		if (GET_EQ(ch, i))
			for (j = 0; j < MAX_OBJ_AFFECT; j++)
				affect_modify(ch, GET_EQ(ch, i)->affected[j].location,
					GET_EQ(ch, i)->affected[j].modifier,
					GET_EQ(ch, i)->obj_flags.bitvector[0], 
					GET_EQ(ch, i)->obj_flags.bitvector[1], 
					GET_EQ(ch, i)->obj_flags.bitvector[2], 
					GET_EQ(ch, i)->obj_flags.bitvector[3], TRUE);
	}
	
	
	for (af = ch->affected; af; af = af->next)
		affect_modify(ch, af->location, af->modifier, af->bitvector[0], af->bitvector[1], af->bitvector[2], af->bitvector[3], TRUE);
	
  /* Make certain values are between 0..25, not < 0 and not > 25! */
	
	i = ((IS_NPC(ch) || GET_LEVEL(ch)>=LVL_IMMORT) ? 25 : 18);
	
	GET_DEX(ch) = MAX(0, MIN(GET_DEX(ch), i));
	GET_INT(ch) = MAX(0, MIN(GET_INT(ch), i));
	GET_WIS(ch) = MAX(0, MIN(GET_WIS(ch), i));
	GET_CON(ch) = MAX(0, MIN(GET_CON(ch), i));
	GET_STR(ch) = MAX(0, GET_STR(ch));
	
	if (IS_NPC(ch)) {
		GET_STR(ch) = MIN(GET_STR(ch), i);
	} else {
		if (GET_STR(ch) > 18) {
			i = GET_ADD(ch) + ((GET_STR(ch) - 18) * 10);
			GET_ADD(ch) = MIN(i, 100);
			GET_STR(ch) = 18;
		}
	}
	
	check_regen_rates(ch);    /* update regen rates (for age) */
	
}



/* Insert an affect_type in a char_data structure
   Automatically sets apropriate bits and apply's */
void affect_to_char(struct char_data * ch, struct affected_type * af)
{
	struct affected_type *affected_alloc;
	
	CREATE(affected_alloc, struct affected_type, 1);
	
	*affected_alloc = *af;
	affected_alloc->next = ch->affected;
	ch->affected = affected_alloc;
	
	affect_modify(ch, af->location, af->modifier, af->bitvector[0], af->bitvector[1], af->bitvector[2], af->bitvector[3], TRUE);
	affect_total(ch);
}


/*
 * Ritorna 1 se ch e' sotto l'influenza dell'affect per via di un obj,
 * 0 in caso contrario.
 */
int char_affected_by_obj (struct char_data *ch, long long int affect, int bitv)
{
	int i;

	for (i = 0; i < NUM_WEARS; i++){
		if (!GET_EQ(ch, i))
			continue;
		if (IS_SET (GET_EQ(ch, i)->obj_flags.bitvector[bitv], affect))
			return 1;
	}
	return 0;
}


/*
 * Remove an affected_type structure from a char (called when duration
 * reaches zero). Pointer *af must never be NIL!  Frees mem and calls
 * affect_location_apply
 */
void affect_remove(struct char_data * ch, struct affected_type * af)
{
	struct affected_type *temp;
	
	assert(ch->affected);
	
	affect_modify(ch, af->location, af->modifier, af->bitvector[0], af->bitvector[1], af->bitvector[2], af->bitvector[3], FALSE);
	REMOVE_FROM_LIST(af, ch->affected, next);
	free(af);
	affect_total(ch);
}



/* Call affect_remove with every spell of spelltype "skill" */
void affect_from_char(struct char_data * ch, int natura, int type)
{
	struct affected_type *hjp, *next;
	
	for (hjp = ch->affected; hjp; hjp = next) {
		next = hjp->next;
		if (hjp->natura == natura && hjp->type == type)
			affect_remove(ch, hjp);
	}
}


/* Scorre tutti gli affected_type di ch. Se questi danno solo affect,
 * vengono rimosse, se danno affect e altri valori smettono di dare
 * affect, in caso contrario restano inalterate.
 */
void affect_remove_all (struct char_data *ch, long long int affect, int bitv)
{
	struct affected_type *hjp, *next;
	int mod = 0, i = 0;

	if (bitv < 0 || bitv > 3){
		mudlog ("SYSERR: passato un bitv non valido ad affect_remove_all", BRF, LVL_QUEST_MASTER, TRUE);
		return;
	}
	if (!ch){
		mudlog ("SYSERR: passato un pg non esistente ad affect_remove_all", BRF, LVL_QUEST_MASTER, TRUE);
		return;
	}

	for (hjp = ch->affected; hjp; hjp = next, mod = 0) {
		next = hjp->next;
		if (IS_SET (hjp->bitvector[bitv], affect)){
			if (hjp->location && hjp->modifier)
				mod++;
			for (i = 0; i < 3; i++)
				if (hjp->bitvector[i] && i!=bitv)
					mod++;
			if (hjp->bitvector[bitv] & (!(affect)))
				mod++;
			if (mod)
				REMOVE_BIT(hjp->bitvector[bitv], affect);
			else
				affect_remove(ch, hjp);
		}
	}
}


/*
 * Return if a char is affected by a spell (SPELL_XXX), NULL indicates
 * not affected
 */
bool affected_by_spell(struct char_data * ch, int natura, int type)
{
	struct affected_type *hjp;
	
	for (hjp = ch->affected; hjp; hjp = hjp->next)
		if (hjp->natura == natura && hjp->type == type)
			return TRUE;
		
	return FALSE;
}



void affect_join(struct char_data * ch, struct affected_type * af,
	bool add_dur, bool avg_dur, bool add_mod, bool avg_mod)
{
	struct affected_type *hjp;
	bool found = FALSE;
	
	for (hjp = ch->affected; !found && hjp; hjp = hjp->next) {
		
		if ((hjp->natura == af->natura) && (hjp->type == af->type) && (hjp->location == af->location)) {
			if (add_dur)
				af->duration += hjp->duration;
			if (avg_dur)
				af->duration >>= 1;
			
			if (add_mod)
				af->modifier += hjp->modifier;
			if (avg_mod)
				af->modifier >>= 1;
			
			affect_remove(ch, hjp);
			affect_to_char(ch, af);
			found = TRUE;
		}
	}
	if (!found)
		affect_to_char(ch, af);
}


/* move a player out of a room */
void char_from_room(struct char_data * ch)
{
	struct char_data *temp;
	
	if (ch == NULL || ch->in_room == NOWHERE) {
		log("SYSERR: NULL or NOWHERE in handler.c, char_from_room");
		return;
	}
	
	if (FIGHTING(ch) != NULL)
		stop_fighting(ch);
	
	if (GET_EQ(ch, WEAR_LIGHT) != NULL)
		if (GET_OBJ_TYPE(GET_EQ(ch, WEAR_LIGHT)) == ITEM_LIGHT)
			if (GET_OBJ_VAL(GET_EQ(ch, WEAR_LIGHT), 2))	/* Light is ON */
				world[ch->in_room].light--;
			
	REMOVE_FROM_LIST(ch, world[ch->in_room].people, next_in_room);
	ch->in_room = NOWHERE;
	ch->next_in_room = NULL;
	if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_SAVE))
		SET_BIT(MOB_FLAGS(ch), MOB_CRASH);
}


/* place a character in a room */
void char_to_room(struct char_data * ch, int room)
{
	if (!ch || room < 0 || room > top_of_world)
		log("SYSERR: Illegal value(s) passed to char_to_room");
	
	else {
		ch->next_in_room = world[room].people;
		world[room].people = ch;
		ch->in_room = room;
		
		if (GET_EQ(ch, WEAR_LIGHT))
			if (GET_OBJ_TYPE(GET_EQ(ch, WEAR_LIGHT)) == ITEM_LIGHT)
				if (GET_OBJ_VAL(GET_EQ(ch, WEAR_LIGHT), 2))	/* Light ON */
					world[room].light++;
				
		if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_SAVE))
			SET_BIT(MOB_FLAGS(ch), MOB_CRASH);  
	}
}

void update_weight(struct char_data *ch){
	int i,n_tot=0,w_tot=0;
	struct obj_data *obj=NULL;
	for(i=0;i<NUM_WEARS;i++){
		if(GET_EQ(ch,i))
			if(GET_OBJ_TYPE(GET_EQ(ch,i))!=ITEM_CONTAINER)
				w_tot+=GET_OBJ_WEIGHT(GET_EQ(ch,i));
	}
	for(obj=ch->carrying;obj;obj=obj->next_content){
		w_tot+=GET_OBJ_WEIGHT(obj);
		n_tot++;
	}
	IS_CARRYING_W(ch)= w_tot;
	IS_CARRYING_N(ch)= n_tot;
	affect_total(ch);
}

/* give an object to a char   */
void obj_to_char(struct obj_data * object, struct char_data * ch)
{
	if (object && ch) {
		object->next_content = ch->carrying;
		ch->carrying = object;
		object->carried_by = ch;
		object->in_room = NOWHERE;
		update_weight(ch);
      //IS_CARRYING_W(ch) += GET_OBJ_WEIGHT(object);
      //IS_CARRYING_N(ch)++;
		
      /* set flag for crash-save system, but not on mobs! */
		if (!IS_NPC(ch))
			SET_BIT(PLR_FLAGS(ch), PLR_CRASH);
		
		if (IS_NPC(ch) && MOB_FLAGGED(ch,MOB_SAVEINV))
			SET_BIT(MOB_FLAGS(ch), MOB_CRASH_EQ);
		
	} else
		log("SYSERR: NULL obj or char passed to obj_to_char");
}


/* take an object from a char */
void obj_from_char(struct obj_data * object)
{
	struct obj_data *temp;
	struct char_data *ch;
	if (object == NULL) {
		log("SYSERR: NULL object passed to obj_from_char");
		return;
	}
	REMOVE_FROM_LIST(object, object->carried_by->carrying, next_content);
	
  /* set flag for crash-save system, but not on mobs! */
	if (!IS_NPC(object->carried_by))
		SET_BIT(PLR_FLAGS(object->carried_by), PLR_CRASH);
	if (IS_NPC(object->carried_by) && MOB_FLAGGED(object->carried_by ,MOB_SAVEINV))
		SET_BIT(MOB_FLAGS(object->carried_by), MOB_CRASH_EQ);	
	ch=object->carried_by;
  /*
  IS_CARRYING_W(object->carried_by) -= GET_OBJ_WEIGHT(object);
  IS_CARRYING_N(object->carried_by)--;*/
	object->carried_by = NULL;
	object->next_content = NULL;
	update_weight(ch);
}



/* Return the effect of a piece of armor in position eq_pos */
int apply_ac(struct char_data * ch, int eq_pos)
{
	double factor;
	int ac;
	
	
	assert(GET_EQ(ch, eq_pos));
	
	if (!(GET_OBJ_TYPE(GET_EQ(ch, eq_pos)) == ITEM_ARMOR))
		return 0;
	
	ac = GET_OBJ_VAL(GET_EQ(ch, eq_pos), 0);
	
	switch (eq_pos) {
		
		case WEAR_SHIELD:
			factor = 1.5;
			break;
		case WEAR_BODY:
			factor = 1.5;
			break;			/* x1.5 */
		case WEAR_HEAD:
			if (ac>8) factor = 1;
			else factor = 1;
			break;			/* x1 */
		case WEAR_LEGS:
			if (ac>8) factor = 1;
			else factor = 1;
			break;			/* */
		case WEAR_FEET:
			if (ac>8) factor = 1;
			else factor = 1;
			break;		
		case WEAR_ARMS:
			if (ac>8) factor = 1;
			else factor = 1;
			break;		
		case WEAR_HANDS:
			if (ac>8) factor = 1;
			else factor = 1;
			break;			
		default:
			if (ac>8) factor = 1;
			else factor = 1;
			break;			/* all others */
	}
	
	return (int) (factor * GET_OBJ_VAL(GET_EQ(ch, eq_pos), 0));
}



void equip_char(struct char_data * ch, struct obj_data * obj, int pos)
{
	int j;
	int invalid_class(struct char_data *ch, struct obj_data *obj);
	int invalid_align(struct char_data *ch, struct obj_data *obj);
	
	//assert(pos >= 0 && pos < NUM_WEARS);
	
	if (GET_EQ(ch, pos)) {
		sprintf(buf, "SYSERR: Char is already equipped: %s, %s", GET_NAME(ch),
			obj->short_description);
		log(buf);
		return;
	}
	if (obj->carried_by) {
		log("SYSERR: EQUIP: Obj is carried_by when equip.");
		return;
	}
	if (obj->in_room != NOWHERE) {
		log("SYSERR: EQUIP: Obj is in_room when equip.");
		return;
	}
	if (!IS_NPC(ch) && (invalid_align(ch, obj) || invalid_class(ch, obj)))
	{
		act("Ricevi una scossa da $p che ti cade per terra.", FALSE, ch, obj, 0, TO_CHAR);
		act("$n riceve una scossa da $p che gli cade per terra.", FALSE, ch, obj, 0, TO_ROOM);
//      check_abil_obj(ch,obj,FALSE);
		obj_to_room(obj, ch->in_room);
	  //obj_to_room_in_water(obj, ch->in_room);
	//obj_to_char(obj, ch);changed to drop in inventory instead of ground
		return;
	}
	
	if (!IS_NPC(ch) &&
		( (GET_LEVEL(ch)<GET_OBJ_LEVEL(obj)) || ( GET_OBJ_LEVEL(obj)<=0 && (GET_LEVEL(ch)<objlevel(obj)) ) ) )
	{
		act("Sei troppo inesperto per utilizzare $p, rinunci.", FALSE, ch, obj, 0, TO_CHAR);
		act("$n e' troppo inesperto per usare $p, rinuncia.", FALSE, ch, obj, 0, TO_ROOM);
//    check_abil_obj(ch,obj,FALSE);
		obj_to_char(obj, ch);
		return;
	}
	if(pos==WEAR_WIELD_L)
		if(check_second_weapon(ch,obj)==FALSE){
			send_to_char("Non sei in grado di impugnare un'altra arma di questo tipo, rinunci.\n",ch);
			act("$n non e' in grado di impugnare $p, rinuncia.", FALSE, ch, obj, 0, TO_ROOM);
//      check_abil_obj(ch,obj,FALSE);
			obj_to_char(obj, ch);
			return;
		}
	if ( (IS_OBJ_STAT(obj, ITEM_NO_5_LIV) && (GET_LEVEL(ch) <= 5)) ||
		(IS_OBJ_STAT(obj, ITEM_NO_10_LIV) && (GET_LEVEL(ch) <= 10)) ||
		(IS_OBJ_STAT(obj, ITEM_NO_20_LIV) && (GET_LEVEL(ch) <= 20)) ||
		(IS_OBJ_STAT(obj, ITEM_NO_25_LIV) && (GET_LEVEL(ch) <= 25)) ||
		(IS_OBJ_STAT(obj, ITEM_NO_30_LIV) && (GET_LEVEL(ch) <= 30)) ||
		(IS_OBJ_STAT(obj, ITEM_NO_40_LIV) && (GET_LEVEL(ch) <= 40)) ) {
		act("Sei troppo inesperto per utilizzare $p, rinunci.", FALSE, ch, obj, 0, TO_CHAR);
		act("$n e' troppo inesperto per usare $p, rinuncia.", FALSE, ch, obj, 0, TO_ROOM);
//      check_abil_obj(ch,obj,FALSE);
		obj_to_char(obj, ch);
		return;
	}
	
	GET_EQ(ch, pos) = obj;
	obj->worn_by = ch;
	obj->worn_on = pos;
	
  /* if(pos!=WEAR_HANG)
    IS_CARRYING_W(ch) += GET_OBJ_WEIGHT(obj);il peso e' calcolato anche dagli ogg addosso tranne che gli ogg hangabili*/
	if (GET_OBJ_TYPE(obj) == ITEM_ARMOR)
		GET_AC(ch) -= apply_ac(ch, pos);
	update_weight(ch);
	if (ch->in_room != NOWHERE) {
		if (pos == WEAR_LIGHT && GET_OBJ_TYPE(obj) == ITEM_LIGHT)
			if (GET_OBJ_VAL(obj, 2))	/* if light is ON */
				world[ch->in_room].light++;
	} //else log("SYSERR: ch->in_room = NOWHERE when equipping char.");
	
	for (j = 0; j < MAX_OBJ_AFFECT; j++)
		affect_modify(ch, obj->affected[j].location,
			obj->affected[j].modifier,
			obj->obj_flags.bitvector[0], 
			obj->obj_flags.bitvector[1], 
			obj->obj_flags.bitvector[2], 
			obj->obj_flags.bitvector[3],  TRUE);
	
	check_abil_obj(ch,obj,TRUE);
	affect_total(ch);
}



struct obj_data *unequip_char(struct char_data * ch, int pos)
{
	int j;
	struct obj_data *obj;
	
	assert(pos >= 0 && pos < NUM_WEARS);
	assert(GET_EQ(ch, pos));
	
	obj = GET_EQ(ch, pos);
	obj->worn_by = NULL;
	obj->worn_on = -1;
	
	if (GET_OBJ_TYPE(obj) == ITEM_BENDA)
		REMOVE_BIT( AFF_FLAGS(ch),AFF_BLIND);
	
	if (GET_OBJ_TYPE(obj) == ITEM_ARMOR)
		GET_AC(ch) += apply_ac(ch, pos);
	
	if (ch->in_room != NOWHERE) {
		if (pos == WEAR_LIGHT && GET_OBJ_TYPE(obj) == ITEM_LIGHT)
			if (GET_OBJ_VAL(obj, 2))	/* if light is ON */
				world[ch->in_room].light--;
	} //else
   // log("SYSERR: ch->in_room = NOWHERE when equipping char.");
	
	GET_EQ(ch, pos) = NULL;
	
	for (j = 0; j < MAX_OBJ_AFFECT; j++)
		affect_modify(ch, obj->affected[j].location,
			obj->affected[j].modifier,
			obj->obj_flags.bitvector[0], 
			obj->obj_flags.bitvector[1], 
			obj->obj_flags.bitvector[2], 
			obj->obj_flags.bitvector[3], FALSE);
	
	check_abil_obj(ch,obj,FALSE);
	affect_total(ch);
	return (obj);
}


int get_number(char **name)
{
	int i;
	char *ppos;
	char number[MAX_INPUT_LENGTH];
	
	*number = '\0';
	
	if ((ppos = strchr(*name, '.'))) {
		*ppos++ = '\0';
		strcpy(number, *name);
		strcpy(*name, ppos);
		
		for (i = 0; *(number + i); i++)
			if (!isdigit(*(number + i)))
				return 0;
			
		return (atoi(number));
	}
	return 1;
}



/* Search a given list for an object number, and return a ptr to that obj */
struct obj_data *get_obj_in_list_num(int num, struct obj_data * list)
{
	struct obj_data *i;
	
	for (i = list; i; i = i->next_content)
		if (GET_OBJ_RNUM(i) == num)
			return i;
		
	return NULL;
}



/* search the entire world for an object number, and return a pointer  */
struct obj_data *get_obj_num(int nr)
{
	struct obj_data *i;
	
	for (i = object_list; i; i = i->next)
		if (GET_OBJ_RNUM(i) == nr)
			return i;
		
	return NULL;
}



/* search a room for a char, and return a pointer if found..  */
struct char_data *get_char_room(char *name, int room)
{
	struct char_data *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	
	strcpy(tmp, name);
	if (!(number = get_number(&tmp)))
		return NULL;
	
	for (i = world[room].people; i && (j <= number); i = i->next_in_room)
		if (isname(tmp, i->player.name))
			if (++j == number)
				return i;
			
	return NULL;
}



/* search all over the world for a char num, and return a pointer if found */
struct char_data *get_char_num(int nr)
{
	struct char_data *i;
	
	for (i = character_list; i; i = i->next)
		if (GET_MOB_RNUM(i) == nr)
			return i;
		
	return NULL;
}



/* put an object in a room */
void obj_to_room(struct obj_data * object, int room)
{
	if (!object || room < 0 || room > top_of_world)
		log("SYSERR: Illegal value(s) passed to obj_to_room");
	else if (GET_OBJ_WEIGHT(object) > 0 && (SECT(room) == SECT_WATER_SWIM ||
			SECT(room) == SECT_WATER_NOSWIM)) {
		object->in_room = room;	/* act() target room. */
		act("$p sprofonda.", FALSE, 0, object, 0, TO_ROOM);
		log ("Un oggetto e' sprofondato.");
		extract_obj(object);
	} else {
		object->next_content = world[room].contents;
		world[room].contents = object;
		object->in_room = room;
		object->carried_by = NULL;
		if (ROOM_FLAGGED(room, ROOM_HOUSE))
			SET_BIT(ROOM_FLAGS(room), ROOM_HOUSE_CRASH);
	}
}


/* Take an object from a room */
void obj_from_room(struct obj_data * object)
{
	struct obj_data *temp;
	
	if (!object || object->in_room == NOWHERE) {
		log("SYSERR: NULL object or obj not in a room passed to obj_from_room");
		return;
	}
	
	REMOVE_FROM_LIST(object, world[object->in_room].contents, next_content);
	
	if (ROOM_FLAGGED(object->in_room, ROOM_HOUSE))
		SET_BIT(ROOM_FLAGS(object->in_room), ROOM_HOUSE_CRASH);
	object->in_room = NOWHERE;
	object->next_content = NULL;
}


/* put an object in an object (quaint)  */
void obj_to_obj(struct obj_data * obj, struct obj_data * obj_to)
{
	struct obj_data *tmp_obj;
	
	if (!obj || !obj_to || obj == obj_to) {
		log("SYSERR: NULL object or same source and target obj passed to obj_to_obj");
		return;
	}
	
	obj->next_content = obj_to->contains;
	obj_to->contains = obj;
	obj->in_obj = obj_to;
	
	for (tmp_obj = obj->in_obj; tmp_obj->in_obj; tmp_obj = tmp_obj->in_obj)
		GET_OBJ_WEIGHT(tmp_obj) += GET_OBJ_WEIGHT(obj);
	
  /* top level object.  Subtract weight from inventory if necessary. */
	GET_OBJ_WEIGHT(tmp_obj) += GET_OBJ_WEIGHT(obj);
	if (tmp_obj->carried_by)
		IS_CARRYING_W(tmp_obj->carried_by) += GET_OBJ_WEIGHT(obj);
}


/* remove an object from an object */
void obj_from_obj(struct obj_data * obj)
{
	struct obj_data *temp, *obj_from;
	
	if (obj->in_obj == NULL) {
		log("error (handler.c): trying to illegally extract obj from obj");
		return;
	}
	obj_from = obj->in_obj;
	REMOVE_FROM_LIST(obj, obj_from->contains, next_content);
	
  /* Subtract weight from containers container */
	for (temp = obj->in_obj; temp->in_obj; temp = temp->in_obj)
		GET_OBJ_WEIGHT(temp) -= GET_OBJ_WEIGHT(obj);
	
  /* Subtract weight from char that carries the object */
	GET_OBJ_WEIGHT(temp) -= GET_OBJ_WEIGHT(obj);
	if (temp->carried_by)
		IS_CARRYING_W(temp->carried_by) -= GET_OBJ_WEIGHT(obj);
	
	obj->in_obj = NULL;
	obj->next_content = NULL;
}


/* Set all carried_by to point to new owner */
void object_list_new_owner(struct obj_data * list, struct char_data * ch)
{
	if (list) {
		object_list_new_owner(list->contains, ch);
		object_list_new_owner(list->next_content, ch);
		list->carried_by = ch;
	}
}


/* Extract an object from the world */
void extract_obj(struct obj_data * obj)
{
	struct obj_data *temp;
	void destroy_corpse_file (struct obj_data *j);
	
	if (IS_PC_CORPSE (obj))
		destroy_corpse_file (obj);

	if (obj->worn_by != NULL)
		if (unequip_char(obj->worn_by, obj->worn_on) != obj)
			log("SYSERR: Inconsistent worn_by and worn_on pointers!!");
	if (obj->in_room != NOWHERE)
		obj_from_room(obj);
	else if (obj->carried_by)
		obj_from_char(obj);
	else if (obj->in_obj)
		obj_from_obj(obj);
	
  /* Get rid of the contents of the object, as well. */
	while (obj->contains)
		extract_obj(obj->contains);
	
	REMOVE_FROM_LIST(obj, object_list, next);
	
	if (GET_OBJ_RNUM(obj) >= 0)
		(obj_index[GET_OBJ_RNUM(obj)].number)--;
	
	if (SCRIPT(obj))
		extract_script(SCRIPT(obj));
	  
	free_obj(obj);
}



void update_object(struct obj_data * obj, int use)
{
	/* dont update objects with a timer trigger */
	if (!SCRIPT_CHECK(obj, OTRIG_TIMER) && (GET_OBJ_TIMER(obj) > 0))	//Orione
		GET_OBJ_TIMER(obj) -= use;
	if (obj->contains)
		update_object(obj->contains, use);
	if (obj->next_content)
		update_object(obj->next_content, use);
}


void update_char_objects(struct char_data * ch)
{
	int i;
	if (GET_EQ(ch, WEAR_LIGHT) != NULL)
		if (GET_OBJ_TYPE(GET_EQ(ch, WEAR_LIGHT)) == ITEM_LIGHT)
			if (GET_OBJ_VAL(GET_EQ(ch, WEAR_LIGHT), 2) > 0) {
				i = --GET_OBJ_VAL(GET_EQ(ch, WEAR_LIGHT), 2);
				if (i == 1) {
					act("La tua &3&bLuce&0 inizia a spegnersi.", FALSE, ch, 0, 0, TO_CHAR);
					act("La &3&bLuce&0 di $n inizia a spegnersi.", FALSE, ch, 0, 0, TO_ROOM);
				} else if (i == 0) {
					act("La tua &3&bLuce&0 si spegne definitivamente.", FALSE, ch, 0, 0, TO_CHAR);
					act("La &3&bLuce&0 di $n si spegne.", FALSE, ch, 0, 0, TO_ROOM);
					world[ch->in_room].light--;
				}
			}
			
	for (i = 0; i < NUM_WEARS; i++)
		if (GET_EQ(ch, i))
			update_object(GET_EQ(ch, i), 2);
		
	if (ch->carrying)
		update_object(ch->carrying, 1);
	
}



/* Extract a ch completely from the world, and leave his stuff behind */
void extract_char(struct char_data * ch)
{
	struct char_data *k, *temp;
	struct descriptor_data *t_desc;
	struct obj_data *obj;
	int i, freed = 0;
	long Loadroom;
	extern void Save_mob(void);
	extern struct char_data *combat_list;
	
	ACMD(do_return);
	void die_follower(struct char_data * ch);
	void dismount_char(struct char_data *ch);
	
	if (IS_NPC(ch)) {
		if (MOB_FLAGGED(ch, MOB_ASSASSIN) && HUNTING(ch))
			REMOVE_BIT(AFF_FLAGS(HUNTING(ch)), AFF_MARK);
		if (MOB_FLAGGED(ch, MOB_SAVE) && (GET_MOB_ID(ch)>0) && (!MOB_FLAGGED(ch, MOB_SAVE_DELETE))) {
			SET_BIT(MOB_FLAGS(ch), MOB_SAVE_DELETE);
			SET_BIT(MOB_FLAGS(ch), MOB_CRASH);
			        //save_mob(ch, ch->in_room);
			Save_mob();
		}  
		
	}

	if (affected_by_spell(ch, SPELLSKILL, SPELL_DISGUISE))
		affect_from_char(ch, SPELLSKILL, SPELL_DISGUISE);
	
	if (!IS_NPC(ch) && !ch->desc) {
		for (t_desc = descriptor_list; t_desc; t_desc = t_desc->next)
			if (t_desc->original == ch)
				do_return(t_desc->character, "", 0, 0);
	}
	if (ch->in_room == NOWHERE) {
		log("SYSERR: NOWHERE extracting char. (handler.c, extract_char)");
		exit(1);
	}
	if (ch->followers || ch->master)
		die_follower(ch);
	if (RIDING(ch) || RIDDEN_BY(ch))
		dismount_char(ch);
	
  /* Forget snooping, if applicable */
	if (ch->desc) {
		if (ch->desc->snooping) {
			ch->desc->snooping->snoop_by = NULL;
			ch->desc->snooping = NULL;
		}
		if (ch->desc->snoop_by) {
			SEND_TO_Q("Your victim is no longer among us.\r\n",
				ch->desc->snoop_by);
			ch->desc->snoop_by->snooping = NULL;
			ch->desc->snoop_by = NULL;
		}
	}
  /* transfer objects to room, if any */
	while (ch->carrying) {
		obj = ch->carrying;
		obj_from_char(obj);
		obj_to_room(obj, ch->in_room);
	}
	
  /* transfer equipment to room, if any */
	for (i = 0; i < NUM_WEARS; i++)
		if (GET_EQ(ch, i))
			obj_to_room(unequip_char(ch, i), ch->in_room);
		
	if (FIGHTING(ch))
		stop_fighting(ch);
	
	for (k = combat_list; k; k = temp) {
		temp = k->next_fighting;
		if (FIGHTING(k) == ch)
			stop_fighting(k);
	}
  /* cancel point update */
	for (i = 0; i < 3; i++)
		if (GET_POINTS_EVENT(ch, i))
		{
			event_cancel(GET_POINTS_EVENT(ch, i));
			GET_POINTS_EVENT(ch, i) = NULL;
		}
		
  /* cancel act event */
	for (i = 0; i < MAX_ACT_EVENTS; i++)
		if (GET_POINTS_ACT_EVENTS(ch, i))
		{
			event_cancel(GET_POINTS_ACT_EVENTS(ch, i));
			GET_POINTS_ACT_EVENTS(ch, i) = NULL;
		}
		
	Loadroom = world[ch->in_room].number;
	char_from_room(ch);
	
  /* pull the char from the list */
	REMOVE_FROM_LIST(ch, character_list, next);
	
	if (ch->desc && ch->desc->original)
		do_return(ch, "", 0, 0);
	
	if (!IS_NPC(ch)) {
		if ((Loadroom!=NOWHERE) && (Loadroom>3) && (GET_POS(ch)>POS_DEAD) &&
			(!IS_SET(ROOM_FLAGS(real_room(Loadroom)), ROOM_DEATH)))
			save_char(ch, real_room(Loadroom));
		else
			save_char(ch, r_mortal_start_room[GET_HOME(ch)]);
		Crash_delete_crashfile(ch);
	}
	else {
		if (GET_MOB_RNUM(ch) > -1)		/* if mobile */
			mob_index[GET_MOB_RNUM(ch)].number--;
		clearMemory(ch);		/* Only NPC's can have memory */
		if (SCRIPT(ch))
			extract_script(SCRIPT(ch));
		if (SCRIPT_MEM(ch))						//Orione
			extract_script_mem(SCRIPT_MEM(ch));	//Orione
		free_char(ch);
		freed = 1;
	}
	
	if (!freed && ch->desc != NULL) {
		STATE(ch->desc) = CON_MENU;
		SEND_TO_Q(MENU, ch->desc);
	} else {  /* if a player gets purged from within the game */
		if (!freed)
			free_char(ch);
	}
}



/* ***********************************************************************
* Here follows high-level versions of some earlier routines, ie functions*
* which incorporate the actual player-data                               *.
*********************************************************************** */


struct char_data *get_player_vis(struct char_data * ch, char *name, int inroom)
{
	struct char_data *i;
	
	for (i = character_list; i; i = i->next)
	if (   !IS_NPC(i) 
	    && (!inroom || i->in_room == ch->in_room) 
	    && !str_cmp(i->player.name, name) 
	    && CAN_SEE(ch, i)
	   ) {
	  if (AFF_FLAGGED(i, AFF_DISGUISE) && isname(name, i->player.name))
		  return NULL;
		return i;
	}
	return NULL;
}


struct char_data *get_char_room_vis(struct char_data * ch, char *name)
{
	struct char_data *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	
  /* JE 7/18/94 :-) :-) */
	if (!str_cmp(name, "self") || !str_cmp(name, "me"))
		return ch;
	
  /* 0.<name> means PC with name */
	strcpy(tmp, name);
	if (!(number = get_number(&tmp)))
		return get_player_vis(ch, tmp, 1);
	
	for (i = world[ch->in_room].people; i && j <= number; i = i->next_in_room)
	if (   isname(tmp, i->player.name) 
	    || ((   AFF_FLAGGED(i,AFF_DISGUISE) 
	         && (i->player.namedisguise) 
	         && isname(tmp, i->player.namedisguise)
	       ))
	    )
	if (CAN_SEE(ch, i))
	if (++j == number) {
				//PEPPEDISGUISE    
		if (AFF_FLAGGED(i, AFF_DISGUISE) && isname(tmp, i->player.name)) {
			return NULL;
		}		    
		return i;
	}
	return NULL;
}


struct char_data *get_char_vis(struct char_data * ch, char *name)
{
	struct char_data *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	
	
  /* check the room first */
	if ((i = get_char_room_vis(ch, name)) != NULL) {
	    return i;
	}	
	
	strcpy(tmp, name);
	if (!(number = get_number(&tmp)))
		return get_player_vis(ch, tmp, 0);
	
	for (i = character_list; i && (j <= number); i = i->next)
	if (   (   isname(tmp, i->player.name) 
	        || (   AFF_FLAGGED(i, AFF_DISGUISE) 
	            && (i->player.namedisguise) 
	            && isname(tmp, i->player.namedisguise)
	           )
	       ) 
	    && CAN_SEE(ch, i)
	   )
	if (++j == number) {
	    //PEPPEDISGUISE    
		if (AFF_FLAGGED(i, AFF_DISGUISE) && isname(tmp, i->player.name)) {
			return NULL;
		}
	  return i;
	}
	return NULL;
}



struct obj_data *get_obj_in_list_vis(struct char_data * ch, char *name,
	struct obj_data * list)
{
	struct obj_data *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	
	strcpy(tmp, name);
	if (!(number = get_number(&tmp)))
		return NULL;
	
	for (i = list; i && (j <= number); i = i->next_content)
		if (isname(tmp, i->name))
			if (CAN_SEE_OBJ_DARK(ch, i))
				if (++j == number)
					return i;
				
	return NULL;
}

//PEPPE VNUM LIST
struct obj_data *get_obj_in_list_vis_vnum(struct char_data * ch, int vnumber,
	struct obj_data * list)
{
	struct obj_data *i;
	for (i = list; i ; i = i->next_content){
		if(GET_OBJ_VNUM(i)==vnumber)
		return i;
		}
				
	return NULL;
}


struct obj_data *get_obj_in_list_particular(struct char_data * ch, char *name,struct obj_data * list,int obj_type){
	struct obj_data *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	
	strcpy(tmp, name);
	if (!(number = get_number(&tmp)))
		return NULL;
	
	for (i = list; i && (j <= number); i = i->next_content)
		if (isname(tmp, i->name))
			if (CAN_SEE_OBJ_DARK(ch, i) || GET_OBJ_TYPE(i) == obj_type)
				if (++j == number)
					return i;
				
	return NULL;
}

/* search the entire world for an object, and return a pointer  */
struct obj_data *get_obj_vis(struct char_data * ch, char *name)
{
	struct obj_data *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	
  /* scan items carried */
	if ((i = get_obj_in_list_vis(ch, name, ch->carrying)))
		return i;
	
  /* scan room */
	if ((i = get_obj_in_list_vis(ch, name, world[ch->in_room].contents)))
		return i;
	
	strcpy(tmp, name);
	if (!(number = get_number(&tmp)))
		return NULL;
	
  /* ok.. no luck yet. scan the entire obj list   */
	for (i = object_list; i && (j <= number); i = i->next)
		if (isname(tmp, i->name))
			if (CAN_SEE_OBJ(ch, i) && (!i->carried_by || CAN_SEE(ch, i->carried_by)))
				if (++j == number)
					return i;
				
	return NULL;
}



struct obj_data *get_object_in_equip_vis(struct char_data * ch,
	char *arg, struct obj_data * equipment[], int *j)
{
	for ((*j) = 0; (*j) < NUM_WEARS; (*j)++)
		if (equipment[(*j)])
			if (CAN_SEE_OBJ_DARK(ch, equipment[(*j)]))
				if (isname(arg, equipment[(*j)]->name))
					return (equipment[(*j)]);
				
	return NULL;
}


char *money_desc(int amount)
{
	static char buf[128];
	
	if (amount <= 0) {
		log("SYSERR: Try to create negative or 0 money.");
		return NULL;
	}
	if (amount == 1)
		strcpy(buf, "una moneta d'oro");
	else if (amount <= 10)
		strcpy(buf, "un mucchietto di monete d'oro");
	else if (amount <= 20)
		strcpy(buf, "una manciata di monete d'oro");
	else if (amount <= 75)
		strcpy(buf, "un po' di monete d'oro");
	else if (amount <= 200)
		strcpy(buf, "un mucchietto di monete d'oro");
	else if (amount <= 1000)
		strcpy(buf, "un mucchio di monete d'oro");
	else if (amount <= 5000)
		strcpy(buf, "un bel mucchio di monete d'oro");
	else if (amount <= 10000)
		strcpy(buf, "un cumulo di monete d'oro");
	else if (amount <= 20000)
		strcpy(buf, "un grosso cumulo di monete d'oro");
	else if (amount <= 75000)
		strcpy(buf, "un'enorme cumulo monete d'oro");
	else if (amount <= 150000)
		strcpy(buf, "una piccola montagna di monete d'oro");
	else if (amount <= 250000)
		strcpy(buf, "una montagna di monete d'oro");
	else if (amount <= 500000)
		strcpy(buf, "una grande montagna di monete d'oro");
	else if (amount <= 1000000)
		strcpy(buf, "un'enorme montagna di monete d'oro");
	else
		strcpy(buf, "una montagna assolutamente colossale di monete d'oro");
	
	return buf;
}


struct obj_data *create_money(int amount)
{
	struct obj_data *obj;
	struct extra_descr_data *new_descr;
	char buf[200];
	
	if (amount <= 0 || amount >= 100000000) {
		log("SYSERR: Try to create negative, 0 money or more then 100M coins.");
		return NULL;
	}
  //obj = create_obj();
	obj = read_object(GOLD_VNUM, VIRTUAL);
	CREATE(new_descr, struct extra_descr_data, 1);
	
	if (amount == 1) {
		obj->name = str_dup("coin gold moneta oro soldo");
		obj->short_description = str_dup("una moneta d'oro");
		obj->description = str_dup("Una miserabile moneta d'oro e' qui per terra.");
		new_descr->keyword = str_dup("coin gold moneta oro soldi");
		new_descr->description = str_dup("E' solo una semplicissima monetina.");
	} else {
		obj->name = str_dup("coins gold monete oro soldi");
		obj->short_description = str_dup(money_desc(amount));
		sprintf(buf, "%s e' qui per terra.", money_desc(amount));
		obj->description = str_dup(CAP(buf));
		
		new_descr->keyword = str_dup("coins gold monete oro");
		if (amount < 10) {
			sprintf(buf, "Ci sono %d monete.", amount);
			new_descr->description = str_dup(buf);
		} else if (amount < 100) {
			sprintf(buf, "Ci sono circa %d monete.", 10 * (amount / 10));
			new_descr->description = str_dup(buf);
		} else if (amount < 1000) {
			sprintf(buf, "Sembra che ci siano circa %d monete.", 100 * (amount / 100));
			new_descr->description = str_dup(buf);
		} else if (amount < 100000) {
			sprintf(buf, "Pensi che ci siano, forse, %d monete.",
				1000 * ((amount / 1000) + number(0, (amount / 1000))));
			new_descr->description = str_dup(buf);
		} else
			new_descr->description = str_dup("Ci sono un MUCCHIO di monete.");
	}
	
	new_descr->next = NULL;
	obj->ex_description = new_descr;
	
	GET_OBJ_TYPE(obj) = ITEM_MONEY;
	GET_OBJ_WEAR(obj) = ITEM_WEAR_TAKE;
	GET_OBJ_VAL(obj, 0) = amount;
	GET_OBJ_COST(obj) = amount;
	SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
  //obj->item_number = NOTHING;
	
	return obj;
}


/* Generic Find, designed to find any object/character                    */
/* Calling :                                                              */
/*  *arg     is the sting containing the string to be searched for.       */
/*           This string doesn't have to be a single word, the routine    */
/*           extracts the next word itself.                               */
/*  bitv..   All those bits that you want to "search through".            */
/*           Bit found will be result of the function                     */
/*  *ch      This is the person that is trying to "find"                  */
/*  **tar_ch Will be NULL if no character was found, otherwise points     */
/* **tar_obj Will be NULL if no object was found, otherwise points        */
/*                                                                        */
/* The routine returns a pointer to the next word in *arg (just like the  */
/* one_argument routine).                                                 */

int generic_find(char *arg, int bitvector, struct char_data * ch,
	struct char_data ** tar_ch, struct obj_data ** tar_obj)
{
	int i, found;
	char name[256];
	
	one_argument(arg, name);
	
	if (!*name)
		return (0);
	
	*tar_ch = NULL;
	*tar_obj = NULL;
	
	if (IS_SET(bitvector, FIND_CHAR_ROOM)) {	/* Find person in room */
		if ((*tar_ch = get_char_room_vis(ch, name))) {
			return (FIND_CHAR_ROOM);
		}
	}
	if (IS_SET(bitvector, FIND_CHAR_WORLD)) {
		if ((*tar_ch = get_char_vis(ch, name))) {
			return (FIND_CHAR_WORLD);
		}
	}
	if (IS_SET(bitvector, FIND_OBJ_EQUIP)) {
		for (found = FALSE, i = 0; i < NUM_WEARS && !found; i++)
			if (GET_EQ(ch, i) && CAN_SEE_OBJ(ch, GET_EQ(ch, i)) &&
				isname(name, GET_EQ(ch, i)->name)) {
				*tar_obj = GET_EQ(ch, i);
				found = TRUE;
			}
		if (found) {
			return (FIND_OBJ_EQUIP);
		}
	}
	if (IS_SET(bitvector, FIND_OBJ_INV)) {
		if ((*tar_obj = get_obj_in_list_vis(ch, name, ch->carrying))) {
			return (FIND_OBJ_INV);
		}
	}
	if (IS_SET(bitvector, FIND_OBJ_ROOM)) {
		if ((*tar_obj = get_obj_in_list_vis(ch, name, world[ch->in_room].contents))) {
			return (FIND_OBJ_ROOM);
		}
	}
	if (IS_SET(bitvector, FIND_OBJ_WORLD)) {
		if ((*tar_obj = get_obj_vis(ch, name))) {
			return (FIND_OBJ_WORLD);
		}
	}
	return (0);
}


/* a function to scan for "all" or "all.x" */
int find_all_dots(char *arg)
{
	if (!strcmp(arg, "all") || !strcmp(arg, "tutto") || !strcmp(arg, "tutti"))
		return FIND_ALL;
	else
	if (!strncmp(arg, "all.", 4)) {
		strcpy(arg, arg + 4);
		return FIND_ALLDOT;
	}
	else
	if (!strncmp(arg, "tutto.", 6) || !strncmp(arg, "tutti.", 6)) {
		strcpy(arg, arg + 6);
		return FIND_ALLDOT;
	}
	else
		return FIND_INDIV;
}

int num_materiale_in_list_vis(struct char_data * ch, struct obj_data *primo, struct obj_data * list)
{
	struct obj_data * i;
	int numero = 0;
	
	for (i = list; i; i = i->next_content)
		if (CAN_SEE_OBJ(ch, i) &&
			GET_OBJ_TYPE(i)   == GET_OBJ_TYPE(primo)    &&    // STESSO TIPO DI OGGETTO, ES. ITEM_LINGOTTO
			GET_OBJ_MAT_TYPE(i) == GET_OBJ_MAT_TYPE(primo)  &&    // STESSO TIPO DI MATERIALE (che e' individuato dai due numeri)
			GET_OBJ_MAT_NUM(i)  == GET_OBJ_MAT_NUM(primo))
			numero++;
		
	return numero;
}

// dismount_char() / fr: Daniel Koepke (dkoepke@california.com)
//   If a character is mounted on something, we dismount them.  If
//   someone is mounting our character, then we dismount that someone.
//   This is used for cleaning up after a mount is cancelled by
//   something (either intentionally or by death, etc.)
void dismount_char(struct char_data *ch)
{
	if (RIDING(ch))
	{
		RIDDEN_BY(RIDING(ch)) = NULL;
		RIDING(ch) = NULL;
	}
	
	if (RIDDEN_BY(ch))
	{
		RIDING(RIDDEN_BY(ch)) = NULL;
		RIDDEN_BY(ch) = NULL;
	}
}

void unstow_char(struct char_data *ch)
{
  /*Si applica solo al character che ha in spalla un altro character*/
	
	if (TRANSPORTED(ch))
	{
		TRANSPORTED_BY(TRANSPORTED(ch)) = NULL;
		TRANSPORTED(ch) = NULL;
	}
	
	if (TRANSPORTED_BY(ch))
	{
		TRANSPORTED(TRANSPORTED_BY(ch)) = NULL;
		TRANSPORTED_BY(ch) = NULL;
	}
	
}


// mount_char() / fr: Daniel Koepke (dkoepke@california.com)
//   Sets _ch_ to mounting _mount_.  This does not make any checks
//   what-so-ever to see if the _mount_ is mountable, etc.  That is
//   left up to the calling function.  This does not present any
//   messages, either.
void mount_char(struct char_data *ch, struct char_data *mount)
{
	RIDING(ch) = mount;
	RIDDEN_BY(mount) = ch;
}

void stow_char(struct char_data *ch,struct char_data *mount)
{ 
	TRANSPORTED_BY(ch)=mount;
	TRANSPORTED(mount)=ch;
	if(RIDING(ch))
		dismount_char(ch);
}

/*Adriano times_event*/
bool can_do_event(struct char_data *ch, long int comdtype, long pulse)
{
	struct last_same_com *ev;
	for (ev=ch->last_same_command;ev;ev=ev->next)
		if (ev && ev->type_event==comdtype)
		{
			if (time(NULL) - ev->times >= pulse)
			{
				ev->times = time(NULL);
				return TRUE;
			}
		}
	return FALSE;
}

/*Adriano times_event*/
bool test_can_do_event(struct char_data *ch, long int comdtype, long pulse)
{
	struct last_same_com *ev;
	for (ev=ch->last_same_command;ev;ev=ev->next)
		if (ev && ev->type_event==comdtype)
		{
			if (time(NULL) - ev->times >= pulse)
				return TRUE;
		}
	return FALSE;
}



int get_no_play(struct char_data *ch, struct char_file_u chdata, int time_out, int inplay)
{
	int playing = TRUE;
	
	if(inplay) {
		if (ch && (((time(0) - ch->player.time.logon) / SECS_PER_REAL_DAY) > time_out))
			playing = FALSE;
	}
	else {
		if (((time(0) - chdata.last_logon )/ SECS_PER_REAL_DAY) > time_out)
			playing = FALSE;
	}
	return playing;
}
