/* ************************************************************************
*   File: act.movement.c        35                      Part of CircleMUD *
*  Usage: movement commands, door handling, & sleep/rest/etc state        *
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
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "house.h"
#include "dg_scripts.h"
#include "wilderness.h"
#include "olc.h"
#include "events.h"

/* external vars  */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *obj_index;
extern struct index_data *mob_index;
extern struct shop_data  *shop_index;
extern int rev_dir[];
extern const char *dirs[];
extern const char *dirs1[];
extern int movement_loss[];
extern const struct dex_skill_type dex_app_skill[];
extern const struct str_app_type str_app[];
extern int check_proficienza_improve (struct char_data * ch, int prof_num, int prob);
extern void check_trappola(struct char_data *ch);
extern struct obj_data *find_trappola(struct char_data *ch);
/* external functs */
int special(struct char_data *ch, int cmd, char *arg);
void death_cry(struct char_data *ch);
int find_eq_pos(struct char_data * ch, struct obj_data *obj, char *arg);
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);
void mount_char(struct char_data *ch, struct char_data *mount);
void clearMemory(struct char_data * ch);
void group_gain(struct char_data * ch, struct char_data * victim, struct char_data * tank);
void solo_gain(struct char_data * ch, struct char_data * victim);
void add_follower(struct char_data *ch, struct char_data *leader);
//Enrico
void unstow_char(struct char_data *ch);
//Adriano
/* local func*/
void domadragon(struct char_data *ch,struct char_data *vict);

struct obj_data *get_obj_in_list_vis_vnum(struct char_data *ch,int vnumber,struct obj_data * list);
extern int limite_follower(struct char_data *ch);
/*Punti esperienza dati per livello quando domini un drago*/
#define EXP_TAME_XLEV 500
/*Durata del tame*/
#define DURATA_TAME 24
/*HP in % che deve avere il drago per essere domato*/
#define HP_RIM_FOR_TAME 15

#define OBJVNUM_TALISMANO_DRAGO 4
#define SEC_CHK_RIDING 15
//Adriano


/* Questa funzione riceve un carattere e restituisce il numero dell'uscita corrispondente, -1 altrimenti */
int parse_dir_char (char c)
{
	switch (tolower(c)) {
		
		case 'n': return 0;
		case 'e': return 1;
		case 's': return 2;
		case 'w': return 3;
		case 'u': return 4;
		case 'd': return 5;
	}
	return -1;
}

int parse_dir (char *st) {
  int direz;
	
  if ((direz = search_block(st, dirs, TRUE)) != -1)
	return direz;
  else if ((direz = search_block(st, dirs1, TRUE)) != -1)
    return direz;
  else {
    return -1;
  }
		
  return -1;
}


/* simple function to determine if char can walk on water */
int has_boat(struct char_data *ch)
{
	struct obj_data *obj;
	int i;
/*
  if (ROOM_IDENTITY(ch->in_room) == DEAD_SEA)
    return 1;
*/
  if (!IS_NPC(ch) && GET_LEVEL(ch) >= LVL_AVATAR)
    return 1;
	
	if (IS_AFFECTED(ch, AFF_WATERWALK) )
		return 1;
	
  /* non-wearable boats in inventory will do it */
	for (obj = ch->carrying; obj; obj = obj->next_content)
		if (GET_OBJ_TYPE(obj) == ITEM_BOAT && (find_eq_pos(ch, obj, NULL) < 0))
			return 1;
		
  /* and any boat you're wearing will do it too */
	for (i = 0; i < NUM_WEARS; i++)
		if (GET_EQ(ch, i) && GET_OBJ_TYPE(GET_EQ(ch, i)) == ITEM_BOAT)
			return 1;
		
	return 0;
}

int can_fly(struct char_data *ch)
{
	struct obj_data *obj;
	int i;
	
	if (IS_AFFECTED(ch, AFF_FLYING))
		return 1;
	
	if (RIDING(ch) && MOB_FLAGGED(RIDING(ch), MOB_HARD_TAME))
		return 1;
	
  /* non-wearable flying object in inventory will do it */
	for (obj = ch->carrying; obj; obj = obj->next_content)
		if (GET_OBJ_TYPE(obj) == ITEM_FLIGHT && (find_eq_pos(ch, obj, NULL) < 0))
			return 1;
		
  /* and any flight object you're wearing will do it too */
	for (i = 0; i < NUM_WEARS; i++)
		if (GET_EQ(ch, i) && GET_OBJ_TYPE(GET_EQ(ch, i)) == ITEM_FLIGHT)
			return 1;
		
	return 0;
}

int can_lava(struct char_data *ch)
{
	
/*  struct obj_data *obj;
  int i;
*/
	if (IS_AFFECTED(ch, AFF_PROT_FIRE || AFF_WATERWALK))
		return 1;
	
	return 0;
}


int can_under(struct char_data *ch)
{
	if (IS_AFFECTED(ch, AFF_WATERBREATH))
		return 1;
	return 0;
}

int transport_grave(struct char_data *ch){
	
	int str_bonus=str_app[GET_STR(ch)].carry_w;
	struct obj_data *obj=NULL;
	int grave=0,i;
	//int obj_weight=GET_OBJ_WEIGHT(obj);
	int malus;
	for(i=0;i<NUM_WEARS;i++)
		if((obj=GET_EQ(ch,i)))
			if(GET_OBJ_TYPE(obj)==ITEM_CONTAINER)
				grave+=GET_OBJ_WEIGHT(obj);
	if(!ch)
		malus=10;
	if(grave<str_bonus)
		malus= 10;
	else if(grave<2*str_bonus)
		malus= 15;
	else if(grave<3*str_bonus)
		malus= 25;
	else if(grave<4*str_bonus)
		malus= 40;
	else malus = 100;
	if(IS_NPC(ch)&&MOB_FLAGGED(ch,MOB_MOUNTABLE))
		malus=MAX(10,malus * 0.6);
	return malus;
}


/* do_simple_move assumes
 *    1. That there is no master and no followers.
 *    2. That the direction exists.
 *
 *   Returns :
 *   1 : If succes.
 *   0 : If fail
 */
#define VNUM_TORTA_FINTA 80
#define VNUM_KIT_EVASIONE 81

int do_simple_move(struct char_data *ch, int dir, int need_specials_check, int show_room)
{
	room_rnum was_in;
	int need_movement=0;
	struct char_data *k;
	struct obj_data *obj;
	room_vnum target_vnum;
	room_rnum target_rnum=ch->in_room;
	int same_room = 0,t_same_room = 0, riding = 0, ridden_by = 0, transported = 0,transported_by = 0 ;
	
	int special(struct char_data *ch, int cmd, char *arg);
	
   //shade
	if (ch->in_room == NOWHERE) 
	  return 0;
	
	if (EXIT(ch, dir) && EXIT(ch, dir)->to_room != NOWHERE )
		target_rnum = EXIT(ch, dir)->to_room;
	else 
	  target_rnum = wild_target_room (ch, dir);
	
	target_vnum = world[target_rnum].number;
	
	if (target_rnum == NOWHERE) 
	  return 0;
  /*
   * Check for special routines (North is 1 in command list, but 0 here) Note
   * -- only check if following; this avoids 'double spec-proc' bug
   */
	if (need_specials_check && special(ch, dir + 1, ""))
		return 0;
	
  // check if they're mounted
	if (RIDING(ch))    riding = 1;
	if (RIDDEN_BY(ch)) ridden_by = 1;
  // check if they're transported
	if (TRANSPORTED(ch)) transported = 1;
	if (TRANSPORTED_BY(ch)) transported_by = 1;
	
  // Il drago non e' domato
	if (riding && (MOB_FLAGGED(RIDING(ch),MOB_HARD_TAME)) && (!AFF_FLAGGED(RIDING(ch),AFF_TAMED)))
	{
		act("$N inizia a guardarti con aria alquanto diffidente", FALSE, ch, 0, RIDING(ch), TO_CHAR);
		act("$N inizia a guardare $n con aria alquanto diffidente", FALSE, ch, 0, RIDING(ch), TO_ROOM);
		return 0;
	}
	
	if (riding && !IS_WILD(target_rnum) && IS_NPC(RIDING(ch)) && ROOM_FLAGGED(target_rnum, ROOM_INDOORS))
	{
		act("Non puoi entrare con la cavalcatura. Devi scendere!", FALSE, ch, 0, 0, TO_CHAR);
		return 0;
	}
	
	if (IS_NPC(ch) && MOB_FLAGGED(ch,MOB_MOUNTABLE) && ROOM_FLAGGED(target_rnum, ROOM_INDOORS) && !IS_WILD(target_rnum))
	{
		act("Non puo' entrare qui!", FALSE, ch, 0, 0, TO_CHAR);
		if (ch->master)
			act("$N non puo' entrare qui!", TRUE, ch->master, 0, ch, TO_CHAR);
		return 0;
	}
	
  // if they're mounted, are they in the same room w/ their mount(ee)?
	if (riding && RIDING(ch)->in_room == ch->in_room)
		same_room = 1;
	else if (ridden_by && RIDDEN_BY(ch)->in_room == ch->in_room)
		same_room = 1;
	
	if (transported && TRANSPORTED(ch)->in_room == ch->in_room)
		t_same_room = 1;
	else if (transported_by && TRANSPORTED_BY(ch)->in_room == ch->in_room)
		t_same_room = 1;
	
  // tamed mobiles cannot move about (DAK)
	if (ridden_by && same_room && AFF_FLAGGED(ch, AFF_TAMED))
	{
		send_to_char("Sei stato domato, ora comportati bene !\r\n", ch);
		return 0;
	}
	
	if (transported_by && t_same_room)
	{
		send_to_char("Sei trasportato, scendi prima se vuoi muoverti!\r\n", ch);
		return 0;
	}
	
  /* charmed? */
	if (IS_AFFECTED(ch, AFF_CHARM) && ch->master && ch->in_room == ch->master->in_room) {
		send_to_char("Il pensiero di lasciare il tuo maestro ti fa scoppiare a piangere.\r\n", ch);
		act("$n scoppia in lacrime.", FALSE, ch, 0, 0, TO_ROOM);
		return 0;
	}
	
	if ((SECT(target_rnum) == SECT_UNDERWATER) && GET_LEVEL(ch) < LVL_AVATAR) {
		if (!can_under(ch)) {
			send_to_char("Affogheresti dopo pochi minuti se entri, dovresti respirare in qualche modo....\r\n", ch);
			return 0;
		}
	}
	
  /* if this room or the one we're going to needs a boat, check for one */
	if (((SECT(ch->in_room) == SECT_WATER_NOSWIM) ||
			(SECT(target_rnum) == SECT_WATER_NOSWIM)) && GET_LEVEL(ch)<LVL_AVATAR) {
		if ((riding && !MOB_FLAGGED(RIDING(ch), MOB_HARD_TAME) && !has_boat(RIDING(ch))) || (!has_boat(ch) && !can_fly(ch))) {
			send_to_char("Hai bisogno di una barca per andare la'.\r\n", ch);
			return 0;
		}
	}
	
  /* if this room or the one we're going to needs flight, check for flight */
	if ((SECT(target_rnum) == SECT_FLYING) && GET_LEVEL(ch) < LVL_AVATAR) {
		if (!can_fly(ch)) {
			send_to_char("Devi volare per andare la'.\r\n", ch);
			return 0;
		}
	}
	
  /* if this room or the one we're going to is inside, check for flight */
	if (((SECT(ch->in_room) == SECT_INSIDE) ||
			(SECT(target_rnum) == SECT_INSIDE)) &&
		GET_LEVEL(ch) < LVL_AVATAR) {
		if (can_fly(ch)) {
			send_to_char("Puoi solo volare intorno.\r\n", ch);
			return 0;
		}
	}
	
  /* if this room or the one we're going to needs flight, check for flight */
	if ((SECT(ch->in_room) == SECT_FLYING) && GET_LEVEL(ch) < LVL_IMMORT) {
		if (!can_fly(ch)) {
			send_to_char("Ti schianti a terra.\r\n", ch);
			log_death_trap(ch);
			death_cry(ch);
			GET_DT_CNT(ch) += 1;
			extract_char(ch);
			return 0;
		}
	}
	
  /* move points needed is avg. move loss for src and destination sect type */
	need_movement = (IS_WILD(ch->in_room) ? GET_WILD_TABLE(ch->in_room).move_cost : movement_loss[SECT(ch->in_room)]);
	need_movement += (IS_WILD(target_rnum) ? GET_WILD_TABLE(target_rnum).move_cost : movement_loss[SECT(target_rnum)]);
	
	if(TRANSPORTED(ch))
		need_movement=need_movement*2;
	else need_movement = need_movement*0.5;
	
	if (riding){
    // if((obj=GET_EQ(RIDING(ch),WEAR_HANG)))
		need_movement=need_movement*transport_grave(RIDING(ch))*0.1;
		if (GET_MOVE(RIDING(ch)) < need_movement){
			if (MOB_FLAGGED(RIDING(ch),MOB_HARD_TAME))
				send_to_char("Il drago e' troppo stanco.\r\nAtterri per farlo riposare.\r\n", ch);
			else
				send_to_char("La tua cavalcatura e' troppo stanca.\r\n", ch);
			return 0;
		}
	}
	else{
    // if((obj=GET_EQ(ch,WEAR_HANG)))
		need_movement=need_movement*transport_grave(ch)*0.1;
		if(can_fly(ch)){
			if (GET_MANA(ch) < 20 && !IS_NPC(ch))
			{
				if (need_specials_check && ch->master) send_to_char("Non ha sufficiente energia per seguire.\r\n", ch);
				else send_to_char("Non abbastanza energie per continuare a volare.\r\n", ch);
				return 0;
			}
		}
		//FIATO

		if(GET_FIATO(ch)<need_movement/2 && !IS_MOB(ch)){
		send_to_char("Devi riprendere fiato\r\n",ch);
		return 0;
		}
		if (GET_MOVE(ch) < need_movement  && !IS_NPC(ch)){
			if (need_specials_check && ch->master) 
				send_to_char("Sei troppo esausto per seguire.\r\n", ch);
			else send_to_char("Sei troppo esausto.\r\n", ch);
			return 0;
		}
	}
	
	if (can_do_event(ch,LAST_CHKRIDING, SEC_CHK_RIDING))
		if (riding && !MOB_FLAGGED(RIDING(ch), MOB_HARD_TAME) && GET_PROF(ch, PROF_CAVALCARE) < (number(1, 80) + (need_movement*3)))
		{
			act("$N indietreggia, buttandoti a terra.", FALSE, ch, 0, RIDING(ch), TO_CHAR);
			act("Indietreggi, buttando a terra $n.", FALSE, ch, 0, RIDING(ch), TO_VICT);
			act("$N indietreggia, buttando $n a terra.", FALSE, ch, 0, RIDING(ch), TO_NOTVICT);
			dismount_char(ch);
			GET_POS(ch) = POS_SITTING;
			damage_nuovo(ch, ch, dice(1,6), -1, TRUE, DANNO_FISICO);
			check_proficienza_improve(ch, PROF_CAVALCARE, 6);
			return 0;
		}
	

	if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_ATRIUM)) {
		if (!House_can_enter(ch, target_vnum)) {
			send_to_char("Proprieta' privata!--Non oltrepassare.\r\n", ch);
			return 0;
		}
	}
	
	if (IS_SET(ROOM_FLAGS(target_rnum), ROOM_TUNNEL))
	{
		if (riding || ridden_by || transported || transported_by)
		{
			send_to_char("Non c'e' abbastanza spazio per un cavaliere.\r\n", ch);
			return 0;
		}
		if (num_pc_in_room(&(world[target_rnum])) > 1)
		{
			send_to_char("Non c'e' spazio per piu' di una persona la'.\r\n", ch);
			return 0;
		}
	}
	
  /*Adriano se un PNG e' cavalcato ma non domato non si muove di sua iniziativa*/
	if (ridden_by && IS_NPC(ch) && MOB_FLAGGED(ch,MOB_MOUNTABLE) && !MOB_FLAGGED(ch,MOB_SENTINEL))
		return 0;

  	if (AFFECTED(ch, AFF_LEVITATE, BITV_LEVITATE)){
		send_to_char ("Non puoi muoverti mentre lieviti!\r\n", ch);
		return 0;
	}

	if (RIDING(ch) && AFFECTED(RIDING(ch), AFF_LEVITATE, BITV_LEVITATE)){
		send_to_char ("La tua cavalcatura non puo' muoversi lievitando.\r\n", ch);
		return 0;
	}

  /* Spini: se un cavallo e' incapacitato il padrone non si muove... */
	if (riding){
		if (AFF_FLAGGED (RIDING(ch), AFF_CHARM) && RIDING(ch)->master != ch){
			send_to_char ("La tua cavalcatura non puo' spostarsi dall'influenza del suo padrone!\r\n", ch);
			return 0;
		}
		if ((SECT(target_rnum) == SECT_UNDERWATER) && GET_LEVEL(RIDING(ch)) < LVL_AVATAR)
			if (!can_under(RIDING(ch))) {
				send_to_char("Non stai cavalcando un pesce! Morirebbe senza dubbio la'!\r\n", ch);
				return 0;
			}
		if (AFF_FLAGGED (RIDING(ch), AFF_PARALIZE)){
			send_to_char ("La tua cavalcatura ora come ora ha la stessa mobilita' di una statua antica...\r\n", ch);
			return 0;
		if (AFF_FLAGGED (RIDING(ch), AFF_SLEEP)){
			send_to_char ("La tua cavalcatura e' immersa in un profondo sonno...\r\n", ch);
			return 0;
		}
		if (GET_POS (RIDING(ch)) < POS_STANDING){
			if (GET_POS (RIDING (ch)) == POS_FIGHTING)
				send_to_char ("La tua cavalcatura e' attualmente impegnata in combattimento...\r\n", ch);
			else
				send_to_char ("Che ne diresti di far alzare prima la tua cavalcatura?\r\n", ch);
			return 0;
		}
	}
	}
	
  /*  if (GET_LEVEL(ch) < LVL_IMMORT && !IS_NPC(ch))     CARLO ERA COSI
      alter_move(ch, need_movement);            */
	
	if (GET_LEVEL(ch) < LVL_AVATAR && !IS_NPC(ch)){
		if (!riding)
		{
		    if (!can_fly(ch)){
			if(!PLR_FLAGGED(ch,PLR_FANTASMA))
			alter_move(ch, need_movement);
			GET_SENZA_DORMIRE(ch)++;
		    }
		}	
		else {
			alter_move(RIDING(ch), need_movement);
			if (!number(0, 2))
				GET_SENZA_DORMIRE(ch)++;
		}
	}
	if (riding)
	{
		if (!IS_AFFECTED(RIDING(ch), AFF_SNEAK)){ 
			if (IS_AFFECTED(ch, AFF_SNEAK))
			{
				if (dir<4)
					sprintf(buf2, "$n va verso %s.", dirs1[dir]);
				else sprintf(buf2, "$n va in %s.", dirs1[dir]);
				act(buf2, TRUE, RIDING(ch), 0, 0, TO_ROOM);
			}
			else
			{
				if (MOB_FLAGGED(RIDING(ch),MOB_HARD_TAME))
				{
					if (dir<4) sprintf(buf2, "$n vola verso %s, montando $N.", dirs1[dir]);
					else sprintf(buf2, "$n va in %s.", dirs1[dir]);
				}
				else
				{
					if (dir<4) sprintf(buf2, "$n cavalca $N verso %s.", dirs1[dir]);
					else sprintf(buf2, "$n va in %s.", dirs1[dir]);
				}
				act(buf2, TRUE, ch, 0, RIDING(ch), TO_NOTVICT);
			}
		}
	}
	else if (ridden_by)
	{
		if (!IS_AFFECTED(ch, AFF_SNEAK)){ 
			if (IS_AFFECTED(RIDDEN_BY(ch), AFF_SNEAK))
			{
				if (dir<4)
					sprintf(buf2, "$n va' verso %s.", dirs1[dir]);
				else sprintf(buf2, "$n va' in %s.", dirs1[dir]);
				act(buf2, TRUE, ch, 0, 0, TO_ROOM);
			}
			else
			{
				if (MOB_FLAGGED(ch, MOB_HARD_TAME))
				{
					if (dir<4) sprintf(buf2, "$n vola verso %s, montando $N.", dirs1[dir]);
					else sprintf(buf2, "$n va' in %s.", dirs1[dir]);
				}
				else
				{
					if (dir<4) sprintf(buf2, "$n cavalca $N verso %s.", dirs1[dir]);
					else sprintf(buf2, "$n va' in %s.", dirs1[dir]);
				}
				act(buf2, TRUE, RIDDEN_BY(ch), 0, ch, TO_NOTVICT);
			}	   
		}	
	} else {
		if (!IS_AFFECTED(ch, AFF_SNEAK))
		{
			if (dir<4)
				sprintf(buf2, "$n va' verso %s.", dirs1[dir]);
			else sprintf(buf2, "$n va' in %s.", dirs1[dir]);
			act(buf2, TRUE, ch, 0, 0, TO_ROOM);

		}
	}
	for (k=world[ch->in_room].people; k; k=k->next_in_room)
		if (k!=ch && (PRF_FLAGGED(k, PRF_HIDE_SNEAK) || AFF_FLAGGED(k, AFF_DET_SNEAK)))
		{
			if (dir<4) sprintf(buf2, "$n si defila verso %s.", dirs1[dir]);
			else sprintf(buf2, "$n si defila in %s.", dirs1[dir]);
			if (IS_AFFECTED(ch, AFF_SNEAK)) act(buf2, TRUE, ch, 0, k, TO_VICT);
			if (riding && k!=RIDING(ch) && IS_AFFECTED(RIDING(ch), AFF_SNEAK)) act(buf2, TRUE, RIDING(ch), 0, k, TO_VICT);
			if (ridden_by && k!=RIDDEN_BY(ch) && IS_AFFECTED(RIDDEN_BY(ch), AFF_SNEAK)) act(buf2, TRUE, RIDDEN_BY(ch), 0, k, TO_VICT);
		}
		
  /* see if an entry trigger disallows the move */
	if (!entry_mtrigger(ch))
		return 0;
	if (!enter_wtrigger(&world[target_rnum], ch, dir))
		return 0;
	
	was_in = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, target_rnum);
	
	if (transported && t_same_room && TRANSPORTED(ch)->in_room != ch->in_room)
	{
		char_from_room(TRANSPORTED(ch));
		char_to_room(TRANSPORTED(ch), ch->in_room);
		if (TRANSPORTED(ch)->desc != NULL && show_room==CAN_SHOW_ROOM)
			look_at_room(TRANSPORTED(ch),0);
	}
	else if (transported_by && t_same_room && TRANSPORTED_BY(ch)->in_room != ch->in_room)
	{
		char_from_room(TRANSPORTED_BY(ch));
		char_to_room(TRANSPORTED_BY(ch), ch->in_room);
	}
	
	if (riding && same_room && RIDING(ch)->in_room != ch->in_room)
	{
		char_from_room(RIDING(ch));
		char_to_room(RIDING(ch), ch->in_room);
	}
	else if (ridden_by && same_room && RIDDEN_BY(ch)->in_room != ch->in_room)
	{
		char_from_room(RIDDEN_BY(ch));
		char_to_room(RIDDEN_BY(ch), ch->in_room);
	}
	
	if (!IS_AFFECTED(ch, AFF_SNEAK))
	{
		if (riding && same_room && !IS_AFFECTED(RIDING(ch), AFF_SNEAK))
		{
			if (IS_NPC(RIDING(ch)) && MOB_FLAGGED(RIDING(ch),MOB_HARD_TAME))
				sprintf(buf2, "$n arriva da%s%s, volando su $N.",
					dir < UP  ? " " : (dir == UP ? "l " : "ll'" ),
					dirs1[rev_dir[dir]]);
			else
				sprintf(buf2, "$n arriva da%s%s, cavalcando $N.",
					dir < UP  ? " " : (dir == UP ? "l " : "ll'" ),
					dirs1[rev_dir[dir]]);
			act(buf2, TRUE, ch, 0, RIDING(ch), TO_ROOM);
		}
		else if (ridden_by && same_room && !IS_AFFECTED(RIDDEN_BY(ch), AFF_SNEAK))
		{
			if (IS_NPC(ch) && MOB_FLAGGED(ch,MOB_HARD_TAME))
				sprintf(buf2, "$n arriva da%s%s, con su $N.",
					dir < UP  ? " " : (dir == UP ? "l " : "ll'" ),
					dirs1[rev_dir[dir]]);
			else
				sprintf(buf2, "$n arriva da%s%s, cavalcato da $N.",
					dir < UP  ? " " : (dir == UP ? "l " : "ll'" ),
					dirs1[rev_dir[dir]]);
			act(buf2, TRUE, ch, 0, RIDDEN_BY(ch), TO_ROOM);
		}
		else if (!riding || (riding && !same_room))
		{
			if (transported && t_same_room) {
				act("$n arriva con in spalla $N.", TRUE, ch,0 , TRANSPORTED(ch), TO_ROOM);
			}
			else {	
				    act("$n arriva.", TRUE, ch, 0, 0, TO_ROOM);			    
				    
			}
		}
		
	}
	
	for (k=world[ch->in_room].people; k; k=k->next_in_room)
		if (k!=ch && (PRF_FLAGGED(k, PRF_HIDE_SNEAK) || AFF_FLAGGED(k, AFF_DET_SNEAK)))     
		{
			if (!CAN_SEE(k, ch))
				continue;
			strcpy(buf2, "$n arriva di soppiatto.");
			if (IS_AFFECTED(ch, AFF_SNEAK)) act(buf2, TRUE, ch, 0, k, TO_VICT);
			if (riding && k!=RIDING(ch) && IS_AFFECTED(RIDING(ch), AFF_SNEAK)) act(buf2, TRUE, RIDING(ch), 0, k, TO_VICT);
			if (ridden_by && k!=RIDDEN_BY(ch) && IS_AFFECTED(RIDDEN_BY(ch), AFF_SNEAK)) act(buf2, TRUE, RIDDEN_BY(ch), 0, k, TO_VICT);
		}
		
	if (ch->desc != NULL && show_room==CAN_SHOW_ROOM)
		look_at_room(ch, 0);
	
	if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_PRISON)){
		obj=get_obj_in_list_vis_vnum(ch,VNUM_TORTA_FINTA,ch->carrying);
			while(obj){
				obj_from_char(obj);
				obj=get_obj_in_list_vis_vnum(ch,VNUM_TORTA_FINTA,ch->carrying);
			}
		obj=get_obj_in_list_vis_vnum(ch,VNUM_KIT_EVASIONE,ch->carrying);
			while(obj){
				obj_from_char(obj);
				obj=get_obj_in_list_vis_vnum(ch,VNUM_KIT_EVASIONE,ch->carrying);
			}
	}


	if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DEATH)) {
	  
	  if (PLR_FLAGGED(ch, PLR_FANTASMA))
	    return (0);
		
		if (riding && GET_LEVEL(RIDING(ch)) < LVL_IMMORT) {
			log_death_trap(RIDING(ch));
			SET_BIT(PLR_FLAGS(RIDING(ch)),PLR_FANTASMA);
			SET_BIT(PRF_FLAGS(RIDING(ch)),PRF_NOHASSLE);

			SET_BIT(PRF_FLAGS(RIDING(ch)),PRF_HOLYLIGHT);
			GET_RESUSCITA(RIDING(ch))=0;
			if (!(IS_NPC(RIDING(ch))))
				GET_DT_CNT(RIDING(ch)) += 1;
			death_cry(RIDING(ch));
			extract_char(RIDING(ch));
		}
		
		if (ridden_by && GET_LEVEL(RIDDEN_BY(ch)) < LVL_IMMORT) {
			log_death_trap(RIDDEN_BY(ch));
			SET_BIT(PLR_FLAGS(RIDDEN_BY(ch)),PLR_FANTASMA);
			SET_BIT(PRF_FLAGS(RIDDEN_BY(ch)),PRF_NOHASSLE);

			SET_BIT(PRF_FLAGS(RIDDEN_BY(ch)),PRF_HOLYLIGHT);
			GET_RESUSCITA(RIDDEN_BY(ch))=0;
			if (!(IS_NPC(RIDDEN_BY(ch))))
				GET_DT_CNT(RIDDEN_BY(ch)) += 1;
			death_cry(RIDDEN_BY(ch));
			extract_char(RIDDEN_BY(ch));
		}
		
		if (transported && GET_LEVEL(ch) < LVL_IMMORT) {
			log_death_trap(TRANSPORTED(ch));
			SET_BIT(PLR_FLAGS(TRANSPORTED(ch)),PLR_FANTASMA);
			SET_BIT(PRF_FLAGS(TRANSPORTED(ch)),PRF_NOHASSLE);

			SET_BIT(PRF_FLAGS(TRANSPORTED(ch)),PRF_HOLYLIGHT);
			GET_RESUSCITA(TRANSPORTED(ch))=0;
			if (!(IS_NPC(TRANSPORTED(ch))))
				GET_DT_CNT(TRANSPORTED(ch)) += 1;
			death_cry(TRANSPORTED(ch));
			extract_char(TRANSPORTED(ch));
		}
		
		if (transported_by && GET_LEVEL(TRANSPORTED_BY(ch)) < LVL_IMMORT) {
			log_death_trap(TRANSPORTED_BY(ch));
			SET_BIT(PLR_FLAGS(TRANSPORTED_BY(ch)),PLR_FANTASMA);
			SET_BIT(PRF_FLAGS(TRANSPORTED_BY(ch)),PRF_NOHASSLE);

			SET_BIT(PRF_FLAGS(TRANSPORTED_BY(ch)),PRF_HOLYLIGHT);
			GET_RESUSCITA(TRANSPORTED_BY(ch))=0;
			if (!(IS_NPC(TRANSPORTED_BY(ch))))
				GET_DT_CNT(TRANSPORTED_BY(ch)) += 1;
			death_cry(TRANSPORTED_BY(ch));
			extract_char(TRANSPORTED_BY(ch));
		}
		
		if (GET_LEVEL(ch) < LVL_IMMORT) {
			log_death_trap(ch);
			//PEPPE DT
			SET_BIT(PLR_FLAGS(ch),PLR_FANTASMA);
			SET_BIT(PRF_FLAGS(ch),PRF_NOHASSLE);

			SET_BIT(PRF_FLAGS(ch),PRF_HOLYLIGHT);
			GET_RESUSCITA(ch)=0;
			if (!(IS_NPC(ch)))
				GET_DT_CNT(ch) += 1;
			death_cry(ch);
			extract_char(ch);
		}
		
		return 0;
	}
	
	entry_memory_mtrigger(ch);          //Orione
	if (!greet_mtrigger(ch, dir)) {
		char_from_room(ch);
		char_to_room(ch, was_in);
		look_at_room(ch, 0);
	} else greet_memory_mtrigger(ch);	//Orione

	
  // Chardhros: Controllo Per Trappole
	if (   GET_LEVEL(ch) < LVL_IMMORT
	    && !PLR_FLAGGED(ch, PLR_FANTASMA)
	   ) {
		if (find_trappola(ch))
			check_trappola(ch);
	}    
		
	return 1;
}


int perform_move(struct char_data *ch, int dir, int need_specials_check, int show_room)
{
	room_rnum was_in;
	struct follow_type *k, *next;
	room_rnum target_room = -1;
	
	if (ch == NULL || dir < 0 || dir >= NUM_OF_DIRS || FIGHTING(ch))
		return 0;
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)) {
		send_to_char("Ma cosa vuoi fare, sei legato mani e piedi!!!",ch);
		return 0;
	}
	
	if (TRANSPORTED_BY(ch)) {
		send_to_char("Devi prima scendere.\r\n",ch);
		return 0;
	}
	
	if (AFF_FLAGGED(ch, AFF_PARALIZE)||AFF_FLAGGED(ch,AFF_TRAMORTITO))
		return 0;
	
	if (IS_IN_WILD(ch)) 
	  target_room = wild_target_room(ch, dir);
	else if (EXIT(ch, dir) && EXIT(ch, dir)->to_room!=NOWHERE) 
	  target_room = EXIT(ch, dir)->to_room;
	
	
	if (target_room == -1)
	{ send_to_char("Non puoi andare in quella direzione...\r\n", ch); return 0;}
	
	if (EXIT(ch, dir) && IS_SET(EXIT(ch, dir)->exit_info, EX_HIDDEN) && IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED))
	{ send_to_char("Non puoi andare in quella direzione...\r\n", ch); return 0;}
	
	if (EXIT(ch, dir) && IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED) && !AFF_FLAGGED(ch, AFF_PASSDOOR))
	{
		if (EXIT(ch, dir)->keyword)
		{
			sprintf(buf2, "La %s sembra essere chiusa.\r\n", fname(EXIT(ch, dir)->keyword));
			send_to_char(buf2, ch);
		}
		else send_to_char("Sembra essere chiusa.\r\n", ch);
		return 0;
	}
	if (EXIT(ch, dir) && IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED) && AFF_FLAGGED(ch, AFF_PASSDOOR) && ((ROOM_FLAGGED(IN_ROOM(ch), ROOM_NOWRAITH))||(ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_NOMAGIC)) ) )
	{
		if (EXIT(ch, dir)->keyword)
		{
			sprintf(buf2, "La %s sembra essere chiusa con una barriera magica.\r\n", fname(EXIT(ch, dir)->keyword));
			send_to_char(buf2, ch);
		}
		else send_to_char("Sembra essere chiusa con una barriera magica.\r\n", ch);
		return 0;
	}
	if (!ch->followers)
		return (do_simple_move(ch, dir, need_specials_check, show_room));
	
	was_in = ch->in_room;
	if (!do_simple_move(ch, dir, need_specials_check, show_room)) return 0;
	
	for (k = ch->followers; k; k = next)
	{
		next = k->next;
		if ((k->follower->in_room == was_in) && (GET_POS(k->follower) >= POS_STANDING))
		{
			act("Segui $N.\r\n", FALSE, k->follower, 0, ch, TO_CHAR);
			perform_move(k->follower, dir, 1, show_room);
		}
	}
	return 1;
}

ACMD(do_move)
{
  /*
   * This is basically a mapping of cmd numbers to perform_move indices.
   * It cannot be done in perform_move because perform_move is called
   * by other functions which do not require the remapping.
   */
	int nr_times, loop;
	
	if(!*argument) nr_times = 1;
	if(*argument) {
		if(atoi(argument) <= 0) nr_times = 1;
		else nr_times = atoi(argument);
	}
	
	if (nr_times <= 15) {
		for(loop = 0; loop < nr_times; loop++)
			perform_move(ch, subcmd - 1, 0, CAN_SHOW_ROOM);
		send_to_char("\n\r",ch);
	}
	else
		send_to_char("Please limit your speedwalking to 15 moves per direction.\r\n", ch);
}




int find_door(struct char_data *ch, char *type, char *dir, char *cmdname)
{
	int door;
	
	if (*dir) {			/* a direction was specified */
		if ((door = search_block(dir, dirs, FALSE)) == -1) {	/* Partial Match */
			send_to_char("Quella non e' una direzione.\r\n", ch);
			return -1;
		}
		if (EXIT(ch, door) && !IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
			if (EXIT(ch, door)->keyword)
				if (isname(type, EXIT(ch, door)->keyword))
					return door;
				else {
					sprintf(buf2, "Non vedo %s la'.\r\n", type);
					send_to_char(buf2, ch);
					return -1;
				} else
				return door;
		else {
			send_to_char("Non sembra esserci niente qui.\r\n", ch);
			return -1;
		}
	} else {			/* try to locate the keyword */
		if (!*type) {
			sprintf(buf2, "Cosa vuoi %s?\r\n", cmdname);
			send_to_char(buf2, ch);
			return -1;
		}
		for (door = 0; door < NUM_OF_DIRS; door++)
			if (EXIT(ch, door) && !IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
				if (EXIT(ch, door)->keyword)
					if (isname(type, EXIT(ch, door)->keyword))
						return door;
					
		sprintf(buf2, "Non sembra esserci %s qui.\r\n", type);
		send_to_char(buf2, ch);
		return -1;
	}
}


int has_key(struct char_data *ch, int key)
{
	struct obj_data *o;
	
	for (o = ch->carrying; o; o = o->next_content)
		if (GET_OBJ_VNUM(o) == key)
			return 1;
		
	if (GET_EQ(ch, WEAR_HOLD))
		if (GET_OBJ_VNUM(GET_EQ(ch, WEAR_HOLD)) == key)
			return 1;
		
	return 0;
}



#define NEED_OPEN	1
#define NEED_CLOSED	2
#define NEED_UNLOCKED	4
#define NEED_LOCKED	8

char *cmd_door[] =
{
	"open",
	"close",
	"unlock",
	"lock",
	"pick",
	"sfonda"
};

const int flags_door[] =
{
	NEED_CLOSED | NEED_UNLOCKED,
	NEED_OPEN,
	NEED_CLOSED | NEED_LOCKED,
	NEED_CLOSED | NEED_UNLOCKED,
	NEED_CLOSED | NEED_LOCKED
};

#define DOOR_IS_OPENABLE(ch, obj, door)	((obj) ? \
			((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && \
			(IS_SET(GET_OBJ_VAL(obj, 1), CONT_CLOSEABLE))) :\
			(IS_SET(EXIT(ch, door)->exit_info, EX_ISDOOR)))
			
#define DOOR_IS_OPEN(ch, obj, door)	((obj) ? \
			(!IS_SET(GET_OBJ_VAL(obj, 1), CONT_CLOSED)) :\
			(!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)))
#define DOOR_IS_UNLOCKED(ch, obj, door)	((obj) ? \
			(!IS_SET(GET_OBJ_VAL(obj, 1), CONT_LOCKED)) :\
			(!IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED)))
#define DOOR_IS_PICKPROOF(ch, obj, door) ((obj) ? \
			(IS_SET(GET_OBJ_VAL(obj, 1), CONT_PICKPROOF)) : \
			(IS_SET(EXIT(ch, door)->exit_info, EX_PICKPROOF)))

#define DOOR_IS_CLOSED(ch, obj, door)	(!(DOOR_IS_OPEN(ch, obj, door)))
#define DOOR_IS_LOCKED(ch, obj, door)	(!(DOOR_IS_UNLOCKED(ch, obj, door)))
#define DOOR_KEY(ch, obj, door)		((obj) ? (GET_OBJ_VAL(obj, 2)) : \
					(EXIT(ch, door)->key))
#define DOOR_LOCK(ch, obj, door)	((obj) ? (GET_OBJ_VAL(obj, 1)) : \
					(EXIT(ch, door)->exit_info))


#define EXITN(room, door)		(world[room].dir_option[door])
#define OPEN_DOOR(room, obj, door)	((obj) ?\
		(TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_CLOSED)) :\
		(TOGGLE_BIT(EXITN(room, door)->exit_info, EX_CLOSED)))
#define LOCK_DOOR(room, obj, door)	((obj) ?\
		(TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_LOCKED)) :\
		(TOGGLE_BIT(EXITN(room, door)->exit_info, EX_LOCKED)))

void do_doorcmd(struct char_data *ch, struct obj_data *obj, int door, int scmd)
{
	int other_room = 0;
	struct room_direction_data *back = 0;
	
	sprintf(buf, "$n %ss ", cmd_door[scmd]);
	if (!obj && ((other_room = EXIT(ch, door)->to_room) != NOWHERE))
		if ((back = world[other_room].dir_option[rev_dir[door]]))
			if (back->to_room != ch->in_room)
				back = 0;
			
	switch (scmd) {
		case SCMD_OPEN:
		case SCMD_CLOSE:
			OPEN_DOOR(ch->in_room, obj, door);
			if (back)
				OPEN_DOOR(other_room, obj, rev_dir[door]);
			send_to_char(OK, ch);
			break;
		case SCMD_UNLOCK:
		case SCMD_LOCK:
			LOCK_DOOR(ch->in_room, obj, door);
			if (back)
				LOCK_DOOR(other_room, obj, rev_dir[door]);
			send_to_char("*Click*\r\n", ch);
			break;
		case SCMD_PICK:
			LOCK_DOOR(ch->in_room, obj, door);
			if (back)
				LOCK_DOOR(other_room, obj, rev_dir[door]);
			send_to_char("La serratura non resiste alla tua maestria.\r\n", ch);
			strcpy(buf, "$n abilmente scassina la serratura del ");
			break;
		case SCMD_BASH_LOCK:
			if (DOOR_IS_LOCKED(ch, obj, door)) {
				LOCK_DOOR(ch->in_room, obj, door);
				if (back) 	  
					LOCK_DOOR(other_room, obj, rev_dir[door]);    
			}
			
			if (DOOR_IS_CLOSED(ch, obj, door)) {
				OPEN_DOOR(ch->in_room, obj, door);
				if (back)	  
					OPEN_DOOR(other_room, obj, rev_dir[door]);  
			}	
			
			strcpy(buf, "$n sfonda ");
			break;
	}
	
  /* Notify the room */
	sprintf(buf + strlen(buf), "%s%s.", ((obj) ? "" : "la "), (obj) ? "$p" :
		(EXIT(ch, door)->keyword ? "$F" : "porta"));
	if (!(obj) || (obj->in_room != NOWHERE))
		act(buf, FALSE, ch, obj, obj ? 0 : EXIT(ch, door)->keyword, TO_ROOM);
	
  /* Notify the other room */
	if ((scmd == SCMD_OPEN || scmd == SCMD_CLOSE) && back) {
		sprintf(buf, "!!SOUND(door1.wav)The %s is %s%s from the other side.\r\n",
			(back->keyword ? fname(back->keyword) : "door"), cmd_door[scmd],
			(scmd == SCMD_CLOSE) ? "d" : "ed");
		if (world[EXIT(ch, door)->to_room].people) {
			act(buf, FALSE, world[EXIT(ch, door)->to_room].people, 0, 0, TO_ROOM);
			act(buf, FALSE, world[EXIT(ch, door)->to_room].people, 0, 0, TO_CHAR);
		}
	}
}


int ok_pick(struct char_data *ch, int keynum, int pickproof, int scmd)
{
	if (scmd == SCMD_PICK)
	{
		if (ROOM_FLAGGED(ch->in_room, ROOM_PRISON)) {
			send_to_char("Sei in prigione, prova con evadi!",ch);
			return 0;
		}
		if (keynum < 0)
			send_to_char("Strano. Non trovi il buco della serratura.\r\n", ch);
		else if (pickproof) {
			if (!can_do_event(ch,LAST_PICK, 60)) {
				send_to_char("Scassinare la serratura richiede tempo.....\r\n", ch);
				return 0;
			}
			if (number(1, 10000) > GET_SKILL(ch, SKILL_PICK_LOCK) + dex_app_skill[GET_DEX(ch)].p_locks) {
				send_to_char("La serratura resiste ai tuoi tentativi di forzarla.\r\n", ch);
				act ("$n tenta invano di scassinare la porta!", TRUE, ch, 0, 0, TO_ROOM);
				return 0;
			} else {
				send_to_char("Con grande maestria e molta fortuna forzi la serratura. Che impresa!\r\n", ch);
				return 1;
			}  
		} else if (!test_and_improve(ch, SKILL_PICK_LOCK, 100, 15, dex_app_skill[GET_DEX(ch)].p_locks))
			send_to_char("Non riesci a forzare la serratura.\r\n", ch);
		else
			return 1;
		return 0;
	}
	
	if (scmd == SCMD_BASH_LOCK)
	{
		if (ROOM_FLAGGED(ch->in_room, ROOM_PRISON)) {
			send_to_char("Sei in prigione, prova con evadi!",ch);
			return 0;
		}
		if (pickproof) {
			send_to_char("E' troppo robusta ti romperesti una spalla!\r\n", ch);  
		} else
		if (!test_and_improve(ch, SKILL_BASH_LOCK, 100, str_app[GET_STR(ch)].todam*10, 0))
			send_to_char("Non riesci a sfondarla.\r\n", ch);
		else{
			send_to_char("Riesci a sfondarla!!!\r\n", ch);
			return 1;
		}
		return 0;  
	}    
	
	return 1;
}

ACMD(do_gen_door)
{
	int door = -1, keynum;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
	struct obj_data *obj = NULL;
	struct char_data *victim = NULL;
	
	skip_spaces(&argument);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	
	if (!*argument) {
		sprintf(buf, "%s cosa?\r\n", cmd_door[subcmd]);
		send_to_char(CAP(buf), ch);
		return;
	}
	
	two_arguments(argument, type, dir);
	if (!generic_find(type, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj))
		door = find_door(ch, type, dir, cmd_door[subcmd]);
	
	if (subcmd == SCMD_BASH_LOCK) {
		if ((obj) || (door >= 0)){
			if (DOOR_IS_OPEN(ch, obj, door))
				send_to_char("E' gia' aperta.\r\n",ch);	
			else if (ok_pick(ch, 0, DOOR_IS_PICKPROOF(ch, obj, door), SCMD_BASH_LOCK)){	  
				do_doorcmd(ch, obj, door, subcmd);	  
			}  
		return;
		}
	}
	
	if ((obj) || (door >= 0)) {
		keynum = DOOR_KEY(ch, obj, door);
		if (!(DOOR_IS_OPENABLE(ch, obj, door)))
			act("Non puoi usare $F su quella!", FALSE, ch, 0, cmd_door[subcmd], TO_CHAR);
		else if (!DOOR_IS_OPEN(ch, obj, door) &&
			IS_SET(flags_door[subcmd], NEED_OPEN))
			send_to_char("Ma e' gia' chiusa!\r\n", ch);
		else if (!DOOR_IS_CLOSED(ch, obj, door) &&
			IS_SET(flags_door[subcmd], NEED_CLOSED))
			send_to_char("Ma e' gia aperta!\r\n", ch);
		else if (!(DOOR_IS_LOCKED(ch, obj, door)) &&
			IS_SET(flags_door[subcmd], NEED_LOCKED))
			send_to_char("Oh...dopo tutto non era chiusa...bene!\r\n", ch);
		else if (!(DOOR_IS_UNLOCKED(ch, obj, door)) &&
			IS_SET(flags_door[subcmd], NEED_UNLOCKED))
			send_to_char("Sembra chiusa a chiave.\r\n", ch);
		else if (!has_key(ch, keynum) && (GET_LEVEL(ch) < LVL_GOD) &&
			((subcmd == SCMD_LOCK) || (subcmd == SCMD_UNLOCK)))
			send_to_char("Non sembra tu abbia la chiave giusta.\r\n", ch);
		else if (ok_pick(ch, keynum, DOOR_IS_PICKPROOF(ch, obj, door), subcmd))
			do_doorcmd(ch, obj, door, subcmd);
		else send_to_char("!!SOUND(No3.wav)No no!\r\n", ch);
	}
	return;
}

ACMD(do_enter)
{
	int door;
	
	if (AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	
	one_argument(argument, buf);
	if (*buf) {			/* an argument was supplied, search for door
				 * keyword */
		
		for (door = 0; door < NUM_OF_DIRS; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->keyword)
					if (!str_cmp(EXIT(ch, door)->keyword, buf)) {
						perform_move(ch, door, 1, CAN_SHOW_ROOM);
						return;
					}
		sprintf(buf2, "Non c'e' %s qui.\r\n", buf);
		send_to_char(buf2, ch);
	} else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_INDOORS))
		send_to_char("Sei gia' dentro.\r\n", ch);
	else {
    /* try to locate an entrance */
		for (door = 0; door < NUM_OF_DIRS; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->to_room != NOWHERE)
					if ((!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED) || AFF_FLAGGED(ch, AFF_PASSDOOR)) &&
						IS_SET(ROOM_FLAGS(EXIT(ch, door)->to_room), ROOM_INDOORS)) {
						perform_move(ch, door, 1, CAN_SHOW_ROOM);
						return;
					}
		send_to_char("Non trovi niente per poter entrare.\r\n", ch);
	}
}


ACMD(do_leave)
{
	int door;
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_INDOORS))
		send_to_char("Sei fuori..dove vuoi andare?\r\n", ch);
	else {
		for (door = 0; door < NUM_OF_DIRS; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->to_room != NOWHERE)
					if ((!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED) || AFF_FLAGGED(ch, AFF_PASSDOOR)) &&
						!IS_SET(ROOM_FLAGS(EXIT(ch, door)->to_room), ROOM_INDOORS)) {
						perform_move(ch, door, 1, CAN_SHOW_ROOM);
						return;
					}
		send_to_char("Non si vedono uscite all'esterno qui.\r\n", ch);
	}
}


ACMD(do_stand)
{
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	switch (GET_POS(ch)) {
		case POS_STANDING:
			act("Sei gia' in piedi.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_SITTING:
			act("Ti alzi.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n si alza.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_STANDING;
			if(FIGHTING(ch))
				GET_POS(ch)=POS_FIGHTING;
			break;
		case POS_RESTING:
			act("Smetti di riposarti e ti alzi.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n smette di riposarsi, e si alza.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_STANDING;
			break;
		case POS_SLEEPING:
			act("Devi prima svegliarti.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_FIGHTING:
			act("Do you not consider fighting as standing?", FALSE, ch, 0, 0, TO_CHAR);
			break;
		default:
			act("Smetti di galleggiare e posi i piedi sul terreno.",
				FALSE, ch, 0, 0, TO_CHAR);
			act("$n smette di galleggiare e posa i piedi sul terreno.",
				TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_STANDING;
			break;
	}
}


ACMD(do_sit)
{
	switch (GET_POS(ch)) {
		case POS_STANDING:
			if (RIDING(ch)) dismount_char(ch);
			act("Ti siedi.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n si siede.", FALSE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_SITTING;
			break;
		case POS_SITTING:
			send_to_char("Sei gia' seduto.\r\n", ch);
			break;
		case POS_RESTING:
			act("Smetti di riposare e ti alzi.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n smette di riposare.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_SITTING;
			break;
		case POS_SLEEPING:
			act("Ti devi prima svegliare.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_FIGHTING:
			act("Sedersi mentre stai combattendo? Sei matto?", FALSE, ch, 0, 0, TO_CHAR);
			break;
		default:
			if (RIDING(ch)) dismount_char(ch);
			act("Smetti di galleggiare e ti siedi.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n smette di galleggiare e si siede.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_SITTING;
			break;
	}
}


ACMD(do_rest)
{
	switch (GET_POS(ch)) {
		case POS_STANDING:
			if (RIDING(ch)) dismount_char(ch);
			act("Ti siedi a riposare le stanche ossa.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n si siede e riposa.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_RESTING;
			break;
		case POS_SITTING:
			act("Riposi le stanche ossa.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n riposa.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_RESTING;
			break;
		case POS_RESTING:
			act("Stai gia' riposando.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_SLEEPING:
			act("Ti devi prima svegliare.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_FIGHTING:
			act("Riposare mentre combatti?  Sei pazzo?", FALSE, ch, 0, 0, TO_CHAR);
			break;
		default:
			if (RIDING(ch)) dismount_char(ch);
			act("Smetti di galleggiare e ti siedi a riposare.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n smette di riposare e si siede a riposare.", FALSE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_SITTING;
			break;
	}
}


ACMD(do_sleep)
{
	switch (GET_POS(ch)) {
		case POS_STANDING:
		case POS_SITTING:
		case POS_RESTING:
			if (RIDING(ch)) dismount_char(ch);
			send_to_char("Dormi.\r\n", ch);
			act("$n si sdraia e dorme.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_SLEEPING;
			break;
		case POS_SLEEPING:
			send_to_char("Stai gia' dormendo.\r\n", ch);
			break;
		case POS_FIGHTING:
			send_to_char("Dormire mentre combatti? Sei pazzo?\r\n", ch);
			break;
		default:
			if (RIDING(ch)) dismount_char(ch);
			act("Smetti di galleggiare e ti metti a dormire.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n smette di galleggiare e si addormenta.", TRUE, ch, 0, 0, TO_ROOM);
			GET_POS(ch) = POS_SLEEPING;
			break;
	}
}

ACMD(do_wake)
{
	struct char_data *vict;
	int self = 0;
	
	one_argument(argument, arg);
	if (*arg) {
		if (GET_POS(ch) == POS_SLEEPING)
			send_to_char("Forse ti devi svegliare prima.\r\n", ch);
		else if ((vict = get_char_room_vis(ch, arg)) == NULL)
			send_to_char(NOPERSON, ch);
		else if (vict == ch)
			self = 1;
		else if (GET_POS(vict) > POS_SLEEPING)
			act("$N e' gia sveglio.", FALSE, ch, 0, vict, TO_CHAR);
		else if (IS_AFFECTED(vict, AFF_SLEEP))
			act("Non puoi svegliare $N !", FALSE, ch, 0, vict, TO_CHAR);
		else if (GET_POS(vict) < POS_SLEEPING)
			act("$N's in pretty bad shape!", FALSE, ch, 0, vict, TO_CHAR);
		else {
			act("Svegli $N.", FALSE, ch, 0, vict, TO_CHAR);
			act("Sei svegliato $n.", FALSE, ch, 0, vict, TO_VICT | TO_SLEEP);
			GET_POS(vict) = POS_SITTING;
		}
		if (!self)
			return;
	}
	if (IS_AFFECTED(ch, AFF_SLEEP))
		send_to_char("Non puoi svegliarti!\r\n", ch);
	else if (GET_POS(ch) > POS_SLEEPING)
		send_to_char("Sei gia' sveglio...\r\n", ch);
	else {
		send_to_char("Ti svegli e ti siedi.\r\n", ch);
		act("$n si sveglia.", TRUE, ch, 0, 0, TO_ROOM);
		GET_POS(ch) = POS_SITTING;
	}
}


ACMD(do_follow)
{
	struct char_data *leader;
	
	void stop_follower(struct char_data *ch);
	void add_follower(struct char_data *ch, struct char_data *leader);
	
	one_argument(argument, buf);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
   
	
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (*buf) {
		if (!(leader = get_char_room_vis(ch, buf))) {
			send_to_char(NOPERSON, ch);
			return;
		}
	} else {
		send_to_char("Chi vuoi seguire?\r\n", ch);
		return;
	}
	//se un mob cerca di seguire un altro mob che non sia se stesso non ci riesce
	//l'ultimo controllo serve a lasciare ai pg la possibilita' di seguire i mob
	if(IS_NPC(leader) && (ch!=leader) && (IS_NPC(ch)) && GET_CHA(leader)<20)
	{
	    send_to_char("Non riesci a seguirlo.\r\n",ch);
	    return;
	} 
	if (ch->master == leader) {
		act("Stai gia' seguendo $N.", FALSE, ch, 0, leader, TO_CHAR);
		return;
	}
	if (ch != leader) //Altrimenti sta cercando di sfollarsi
		if (limite_follower(leader)) {
			act("Non hai abbastanza carisma per farti seguire da $N.", FALSE, leader, 0, ch, TO_CHAR);
			return;
		}
		
	if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master)) {
		act("But you only feel like following $N!", FALSE, ch, 0, ch->master, TO_CHAR);
	} else {			/* Not Charmed follow person */
		if (leader == ch) {
			if (!ch->master) {
				send_to_char("Stai gia' seguente te stesso.\r\n", ch);
				return;
			}
			stop_follower(ch);
		} else {
			if (circle_follow(ch, leader)) {
				act("Scusa, ma tu segui lui, lui segue te...no no..no!.", FALSE, ch, 0, 0, TO_CHAR);
				return;
			}
			if (ch->master)
				stop_follower(ch);
			REMOVE_BIT(AFF_FLAGS(ch), AFF_GROUP);
			add_follower(ch, leader);
		}
	}
}

ACMD(do_gohome)
{
	
	if (GET_GOHOME(ch) == 0)
	{
		send_to_char("You don't have a gohome!\r\n", ch);
		return;
	}
	
	if (real_room(GET_GOHOME(ch)) < 0)
	{
		sprintf(buf, "Invalid Gohome, Room %d does not exist.\r\n",
			GET_GOHOME(ch));
		send_to_char(buf, ch);
		return;
	}
	
	sprintf(buf, "%s fades into nothingness and is gone.\r\n",
		GET_NAME(ch));
	send_to_room(buf, ch->in_room);
	char_from_room(ch);
	char_to_room(ch, real_room(GET_GOHOME(ch)));
	sprintf(buf, "%s appears in a flash of blinding light!\r\n",
		GET_NAME(ch));
	send_to_room(buf, ch->in_room);
	look_at_room(ch, 0);
	
}


ACMD(do_givehome)
{
	struct char_data *vict;
	int home;
	
	half_chop(argument, arg, buf);
	
	if (!*arg || !*buf || !isdigit(*buf))
	{
		send_to_char("Usage: givehome <player> <location>\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_vis(ch, arg)))
	{
		send_to_char(NOPERSON, ch);
		return;
	}
	
	if (GET_GOHOME(vict) != 0)
	{
		send_to_char("Character already has a gohome, remove the old one first!\r\n", ch);
		return;
	}
	
	home = atoi(buf);
	
	if (real_room(home) < 0)
	{
		send_to_char("Room does not exist.\r\n", ch);
		return;
	}
	
	GET_GOHOME(vict) = home;
	GET_LOADROOM(vict) = home;
	sprintf(buf1, "You make %s a gohome!\r\n", GET_NAME(vict));
	send_to_char(buf1, ch);
	sprintf(buf1, "%s makes you a gohome!\r\n", GET_NAME(ch));
	send_to_char(buf1, vict);
}

ACMD(do_takehome)
{
	struct char_data *vict;
	
	skip_spaces(&argument);
	
	if (!*argument)
	{
		send_to_char("Usage: takehome <player>\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_vis(ch, argument)))
	{
		send_to_char(NOPERSON, ch);
		return;
	}
	
	if (GET_GOHOME(vict) == 0)
	{
		sprintf(buf, "%s does not even have a gohome!\r\n", GET_NAME(vict));
		send_to_char(buf, ch);
		return;
	}
	
	GET_GOHOME(vict) = 0;
	sprintf(buf, "%s removes your gohome!\r\n", GET_NAME(ch));
	send_to_char(buf, vict);
	sprintf(buf, "You remove %s gohome!\r\n", GET_NAME(vict));
	send_to_char(buf, ch);
}

ACMD(do_retreat)
{
	int dir = 0;
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	if (!FIGHTING(ch))
	{
		send_to_char("Non stai combattendo!", ch);
		return;
	}
	if (!*arg)
	{
		send_to_char("Retreat dove?!?", ch);
		return;
	}
	
	dir = parse_dir(arg);
	if (dir==-1)
	{
		send_to_char("In che direzione vuoi ritirarti ?!?!?", ch);
		return;
	}
	
	if (test_and_improve(ch, SKILL_RETREAT, 100, 15, 0))
	{
		if (CAN_GO(ch, dir) && !IS_SET(ROOM_FLAGS(EXIT(ch,dir)->to_room), ROOM_DEATH)) {
			act("$n si ritira dal combattimento e fugge.", TRUE, ch, 0, 0,
				TO_ROOM);
			send_to_char("Ti ritiri dal combattimento e fuggi.\r\n", ch);
			WAIT_STATE(ch, PULSE_VIOLENCE);
			do_simple_move(ch, dir, TRUE, CAN_SHOW_ROOM);
 /*if (FIGHTING(FIGHTING(ch)) == ch)
 stop_fighting(FIGHTING(ch));*/
//Se ch si ritira e l'opponente sta preparando Db perde la concentrazione
			if(FIGHTING(ch)){
				if(PLR_FLAGGED(FIGHTING(ch),PLR_CONCENTRATO)){
					REMOVE_BIT(PLR_FLAGS(FIGHTING(ch)),PLR_CONCENTRATO);
				}
			}
			if(PLR_FLAGGED(ch,PLR_CONCENTRATO)){
					REMOVE_BIT(PLR_FLAGS(ch),PLR_CONCENTRATO);
			}
			stop_fighting(ch);

		} else {
			act("$n tenta di ritirarsi dal combattimento ma non sa' dove!", TRUE,
				ch,
				0, 0, TO_ROOM);
			send_to_char("Non puoi ritirarti in quella direzione!", ch);
			return;
		}
	} else {
		send_to_char("Fallisci nel tentativo di ritirarti!\r\n", ch);
		WAIT_STATE(ch, PULSE_VIOLENCE);
		return;
	}
}

// Mounts (DAK)
ACMD(do_monta)
{
	char arg[MAX_INPUT_LENGTH];
	struct char_data *vict;
	
	if (affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE))
	{
		send_to_char ("Devi ringraziare il tuo forte corpo se ancora sei vivo, non chiedergli anche di cavalcare!\r\n", ch);
		return;
	}

	if (AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if(GET_EQ(ch,WEAR_HANG)){
		send_to_char("Devi prima liberarti di quello che stai trainando.\r\n",ch);
		return;
	}
	one_argument(argument, arg);
	if (!arg || !*arg)
	{
		send_to_char("Montare su chi ?\r\n", ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, arg)))
	{
		send_to_char("Non c'e' nessuno con quel nome qui.\r\n", ch);
		return;
	}
	if (!IS_NPC(vict) && GET_LEVEL(ch) < LVL_AVATAR)
	{
		send_to_char("Ehh... no.\r\n", ch);
		return;
	}
	if (RIDING(ch))
	{
		send_to_char("Stai gia' montando una cavalcatura.\r\n", ch);
		return;
	}
	
	if (RIDDEN_BY(ch)) {
		send_to_char("Sei tu che sei gia' montato da qualcun'altro !!!\r\n", ch);
		return;
	}
	
	if (RIDING(vict))
	{
		send_to_char("Sta montando qualcun'altro !\r\n", ch);
		return;
	}
	
	if (RIDDEN_BY(vict))
	{
		send_to_char("Qualcuno lo sta gia' montando.\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) < LVL_IMMORT && IS_NPC(vict) && !MOB_FLAGGED(vict, MOB_MOUNTABLE))
	{
		send_to_char("Non puoi montarlo!\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) < LVL_IMMORT && IS_NPC(vict) && MOB_FLAGGED(vict,MOB_HARD_TAME) && (GET_LEVEL(ch)< 50))
	{
		send_to_char("Non si fara' montare certamente da te!!!!\r\n", ch);
		return;
	}	
	
	if (!GET_PROF(ch, PROF_MONTARE))
	{
		send_to_char("Prima dovresti imparare a *come* montare a cavallo.\r\n", ch);
		return;
	}
	
	if (MOB_FLAGGED(vict,MOB_HARD_TAME) && (!AFF_FLAGGED(vict, AFF_TAMED) || !AFF_FLAGGED(vict, AFF_CHARM)))
	{
		send_to_char("Forse sara' meglio domarlo prima.\r\n", ch);
		return;
	}
	
	if (MOB_FLAGGED(vict,MOB_HARD_TAME) && AFF_FLAGGED(vict, AFF_TAMED) && AFF_FLAGGED(vict, AFF_CHARM) && (vict->master!=ch))
	{
		send_to_char("Non credo si faccia montare da te.\r\n", ch);
		return;
	}
	
	if (GET_PROF(ch, PROF_MONTARE) <= number(1, 101))
	{
		act("Provi a montare $N, ma scivoli e cadi.", FALSE, ch, 0, vict, TO_CHAR);
		act("$n prova a montare su di te, ma scivola e cade.", FALSE, ch, 0, vict, TO_VICT);
		act("$n prova a montare $N, ma scivola e cade.", TRUE, ch, 0, vict, TO_NOTVICT);
		damage_nuovo(ch, ch, dice(1, 2), -1, TRUE, DANNO_FISICO);
		return;
		check_proficienza_improve(ch, PROF_MONTARE, 4);
	}
	
	act("Monti in groppa a $N.", FALSE, ch, 0, vict, TO_CHAR);
	act("$n ti monta in groppa.", FALSE, ch, 0, vict, TO_VICT);
	act("$n monta in groppa a $N.", TRUE, ch, 0, vict, TO_NOTVICT);
	mount_char(ch, vict);
	
	if (IS_NPC(vict) && !AFF_FLAGGED(vict, AFF_TAMED) && GET_PROF(ch, PROF_MONTARE) <= number(1, 101))
	{
		act("$N bruscamente si imbizzarrisce, buttandoti violentemente a terra", FALSE, ch, 0, vict, TO_CHAR);
		act("$N si imbizzarrisce, e butta violentemente $n a terra !", TRUE, ch, 0, vict, TO_NOTVICT);
		act("Ti imbizzarrisci e butti $n violentemente a terra.", FALSE, ch, 0, vict, TO_VICT);
		check_proficienza_improve(ch, PROF_MONTARE, 4);
		dismount_char(ch);
		damage_nuovo(vict, ch, dice(1,3), -1, TRUE, DANNO_FISICO);
	}
}


ACMD(do_smonta)
{
	if (!RIDING(ch) && !TRANSPORTED_BY(ch))
	{
		send_to_char("Non stai montando niente.\r\n", ch);
		return;
	}
	if (SECT(ch->in_room) == SECT_WATER_NOSWIM && !has_boat(ch))
	{
		send_to_char("Per poi affogare ???\r\n", ch);
		return;
	}
	
	if (TRANSPORTED_BY(ch)) {
		act("Scendi da $N.", FALSE, ch, 0, TRANSPORTED_BY(ch), TO_CHAR);
		act("$n scende dalle tue spalle.", FALSE, ch, 0, TRANSPORTED_BY(ch), TO_VICT);
		act("$n scende da $N.", TRUE, ch, 0, TRANSPORTED_BY(ch), TO_NOTVICT);
		unstow_char(TRANSPORTED_BY(ch)); 
	} else {
		act("Scendi da $N.", FALSE, ch, 0, RIDING(ch), TO_CHAR);
		act("$n scende dalle tue spalle.", FALSE, ch, 0, RIDING(ch), TO_VICT);
		act("$n scende da $N.", TRUE, ch, 0, RIDING(ch), TO_NOTVICT);
		dismount_char(ch);}
}


ACMD(do_scrolla)
{
	if (!RIDDEN_BY(ch))
	{
		send_to_char("Non hai nessuno in groppa !\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(ch, AFF_TAMED))
	{
		send_to_char("Sei stato domato !\r\n", ch);
		return; 
	}
	
	act("Ti imbizzarrisci, gettando $N a terra.", FALSE, ch, 0, RIDDEN_BY(ch), TO_CHAR);
	act("$n si imbizzarrisce, gettandoti a terra.", FALSE, ch, 0, RIDDEN_BY(ch), TO_VICT);
	act("$n si imbizzarrisce, gettando $N a terra.", FALSE, ch, 0, RIDDEN_BY(ch), TO_NOTVICT);
	GET_POS(RIDDEN_BY(ch)) = POS_SITTING;
	if (number(0, 4))
	{
		send_to_char("Che botta !!!\r\n", RIDDEN_BY(ch));
		damage_nuovo(RIDDEN_BY(ch), RIDDEN_BY(ch), dice(2,4), -1, TRUE, DANNO_FISICO);
	}
	dismount_char(ch);
	
  // you might want to call set_fighting() or some non-sense here if you
  // want the mount to attack the unseated rider or vice-versa.
}

ACMD(do_doma)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct affected_type af;
	struct char_data *vict;
	struct char_data *padrone;
	
	if (subcmd == SCMD_SUBMITDRAGON)
	{
		argument = one_argument(argument, arg);
		if (!arg || !*arg)
		{
			send_to_char("Sottomettere chi ?\r\n", ch);
			return;
		}
		argument = one_argument(argument, arg2);
		if (!arg2 || !*arg2)
		{
			send_to_char("Sottomettere a chi ?\r\n", ch);
			return;
		}
		if (!(padrone = get_char_room_vis(ch, arg2)))
		{
			send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
			return;
		}
	}
	else
	{
		one_argument(argument, arg);
		if (!arg || !*arg)
		{
			send_to_char("Domare chi ?\r\n", ch);
			return;
		}
	}
	if (!(vict = get_char_room_vis(ch, arg)))
	{
		send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
		return;
	}
	if (GET_LEVEL(ch) < LVL_IMMORT && IS_NPC(vict) && !MOB_FLAGGED(vict, MOB_MOUNTABLE))
	{
		send_to_char("Non puoi farlo.\r\n", ch);
		return;
	}
	
	if (MOB_FLAGGED(vict, MOB_AMMAESTRABILE) && (!vict->master || (vict->master!=ch)))
	{
		send_to_char("Dovrebbe essere prima ammaestrato.\r\n", ch);
		return;
	}
	
	if (!GET_PROF(ch, PROF_DOMARE))
	{
		send_to_char("Non hai idea di come si possa domare qualcuno.\r\n", ch);
		return;
	}
	if (!IS_NPC(vict) && GET_LEVEL(ch) < LVL_IMMORT)
	{
		send_to_char("Non puoi farlo.\r\n", ch);
		return;
	}
	if (subcmd == SCMD_SUBMITDRAGON)
	{
		if (!MOB_FLAGGED(vict,MOB_HARD_TAME))
		{
			send_to_char("Puoi sottomettere solo draghi.\r\n", ch);
			return;
		}
		
		if ((GET_LEVEL(ch) >= LVL_AVATAR) && (!AFF_FLAGGED(vict,AFF_TAMED) || !AFF_FLAGGED(vict,AFF_CHARM)))
		{
			padrone = get_char_room_vis(ch, arg2); // Cosi' evitiamo il warning
			domadragon(padrone,vict);
			if (padrone!=ch) {
				sprintf(buf,"Sottometti %s al volere di %s",GET_NAME(vict),GET_NAME(padrone));
				act(buf, FALSE, ch , 0, vict, TO_CHAR);
				sprintf(buf,"%s sottomette %s al volere di %s.",GET_NAME(ch),GET_NAME(vict),GET_NAME(padrone));
			}
			else
			{
				act("$N viene sottomesso al tuo volere.", FALSE, ch, 0, vict, TO_CHAR);
				sprintf(buf,"%s sottomette %s al suo volere.",GET_NAME(ch),GET_NAME(vict));
			}
			act("Ti pieghi al volere di $n.", FALSE, padrone, 0, vict, TO_VICT);
			act(buf, FALSE, ch, 0, vict, TO_NOTVICT);
			return;
		} else
		{
			act("E' gia' sottomesso!.", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
	}
	
  /*Adriano by FizBan*/
	if (vict && MOB_FLAGGED(vict,MOB_HARD_TAME) && ((GET_LEVEL(vict)>GET_PROF(ch, PROF_DOMARE) + 10) || (GET_LEVEL(ch) < 50)))
	{
		send_to_char("Non si fara' mai domare da te.\r\n", ch);
		return;
	}
	
	if (vict && !AFF_FLAGGED(vict,AFF_TAMED) && MOB_FLAGGED(vict,MOB_HARD_TAME) && vict != FIGHTING(ch))
	{
		send_to_char("Solo con la forza potrai domarlo.\r\n", ch);
		return;
	}
	if (vict && AFF_FLAGGED(vict,AFF_TAMED) && MOB_FLAGGED(vict,MOB_HARD_TAME) && vict != FIGHTING(ch))
	{
		send_to_char("Per ora sembra mansueto ... ma chi puo' dirlo in futuro.\r\n", ch);
		return;
	}
	
	if (vict && FIGHTING(ch) && (!MOB_FLAGGED(vict,MOB_HARD_TAME)))
	{
		send_to_char("Sta combattendo, non credo che si fara' domare ora.\r\n", ch);
		return;
	}
	
	if MOB_FLAGGED(vict,MOB_HARD_TAME)
	{
	 // Se la vittima sta combattendo con il pg e il pg cerca di domarlo
		if ((GET_PROF(ch, PROF_DOMARE)/GET_LEVEL(vict)*6 > number(1, 101) &&
				GET_HIT(vict)*100/GET_MAX_HIT(vict) <= HP_RIM_FOR_TAME) || GET_LEVEL(ch) >= LVL_AVATAR)
		{
		 /*TAME DRAGO */
			domadragon(ch,vict);
			check_proficienza_improve(ch, PROF_DOMARE, 10);
		}
		else
		{
			sprintf(buf2, "%s non ti considera nemmeno e continua a combattere.\r\n",GET_NAME(vict));
			send_to_char(buf2, ch);
			return;
		}
	}
	else  //se non e' un drago...
	{
		if (GET_PROF(ch, PROF_DOMARE) < number(1, 101) - GET_CHA(ch) + 9)
		{
			send_to_char("Non riesci a domarlo.\r\n", ch);
			check_proficienza_improve(ch, PROF_DOMARE, 2);
			return;
		}
		
		if (IS_NPC(vict))
		{
			REMOVE_BIT(MOB_FLAGS(vict), MOB_AGGRESSIVE);
			SET_BIT(MOB_FLAGS(vict), MOB_SENTINEL);
		}
		
	  /*TAME CAVALLO*/
		af.natura = PROFICIENZE;
		af.type = PROF_DOMARE;
		af.duration = DURATA_TAME;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector[0] = AFF_TAMED;
		af.bitvector[1] = 0;
		af.bitvector[2] = 0;
		af.bitvector[3] = 0;
		affect_join(vict, &af, FALSE, FALSE, FALSE, FALSE);
	}
	act("Domi $N.", FALSE, ch, 0, vict, TO_CHAR);
	act("$n ti doma.", FALSE, ch, 0, vict, TO_VICT);
	act("$n doma $N.", FALSE, ch, 0, vict, TO_NOTVICT);
}


void domadragon(struct char_data *ch,struct char_data *vict)
{
	struct affected_type af;
	struct affected_type af2;
	int durata_tame_dragon;
	struct obj_data * talismano;
	
	SPECIAL(dragonact);
	
	if (vict && vict == FIGHTING(ch))
	{
		stop_fighting(ch);
		stop_fighting(vict);
		clearMemory(vict);
		if (AFF_FLAGGED(ch, AFF_GROUP))
			group_gain(ch, vict, NULL);
		else
			solo_gain(ch, vict);
		gain_exp(ch, (GET_LEVEL(vict) * EXP_TAME_XLEV));
		sprintf(buf2, "Ricevi %d punti esperienza per averlo domato.\r\n", GET_LEVEL(vict) * EXP_TAME_XLEV);
		send_to_char(buf2, ch);
	}
	
	if (vict->master)
		stop_follower(vict);
	add_follower(vict,ch);
	
	if (IS_NPC(vict))
	{
		REMOVE_BIT(MOB_FLAGS(vict), MOB_AGGRESSIVE);
		REMOVE_BIT(MOB_FLAGS(vict), MOB_SPEC);
	}
	
	SET_BIT(MOB_FLAGS(vict), MOB_SENTINEL);
	
	durata_tame_dragon = MAX(number(GET_PROF(ch, PROF_DOMARE) - 20,GET_PROF(ch, PROF_DOMARE) + 20),1) *
		(SECS_PER_REAL_HOUR / SECS_PER_MUD_HOUR);
	
  /*TAME*/
	af.natura = PROFICIENZE;
	af.type = PROF_DOMARE;
	af.duration = durata_tame_dragon;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector[0] = AFF_TAMED;
	af.bitvector[1] = 0;
	af.bitvector[2] = 0;
	af.bitvector[3] = 0;
	affect_join(vict, &af, FALSE, FALSE, FALSE, FALSE);
	
  /*CHARM DRAGO*/
	af2.natura = SPELLSKILL;
	af2.type = SPELL_CHARM;
	af2.duration = durata_tame_dragon;
	af2.modifier = 0;
	af2.location = APPLY_NONE;
	af2.bitvector[0] = AFF_CHARM;
	af2.bitvector[1] = 0;
	af2.bitvector[2] = 0;
	af2.bitvector[3] = 0;
	affect_join(vict, &af2, FALSE, FALSE, FALSE, FALSE);
	
	 /*Talismano*/
	talismano = read_object(OBJVNUM_TALISMANO_DRAGO, VIRTUAL);
	if (talismano)
	{
		/*consegna l'oggetto*/
		GET_OBJ_VAL(talismano, 1) = GET_HIT(vict);
		GET_OBJ_VAL(talismano, 2) = GET_MOVE(vict);
		GET_OBJ_VAL(talismano, 3) = GET_MOB_VNUM(vict);
		GET_OBJ_VAL(talismano, 4) = time(0);
		GET_OBJ_VAL(talismano, 5) = durata_tame_dragon;
		GET_OBJ_VAL(talismano, 6) = GET_MANA(vict);
		GET_OBJ_VAL(talismano, 7) = 1;
		GET_OBJ_VAL(talismano, 8) = GET_IDNUM(ch);       // OBJ_VAL e' un intero, ch e' un char_data ???
		GET_OBJ_CSLOTS(talismano) = 99;
		GET_OBJ_TSLOTS(talismano) = 99;
		obj_to_char(talismano, ch);
		act("Strappi un talismano dal collo del drago.", FALSE, ch, 0, vict, TO_CHAR);
		act("$n strappa un talismano dal collo del drago.", FALSE, ch, 0, vict, TO_NOTVICT);
	}
}


/*NUOVE ROUTINE DI LANCE*/
#define PULSES_PER_MUD_HOUR     (SECS_PER_MUD_HOUR*PASSES_PER_SEC)

struct event_data
{
	struct char_data *ch;
	int times;
	struct obj_data *talismano;
};

ACMD(do_freedragon)
{
	struct obj_data *talismano;
	struct char_data *drago;
	long int tame_aff_time=0;
	struct affected_type *aff;
	
	drago = RIDING(ch);
	
	if (!drago)
	{
		send_to_char("Non stai cavalcando nessun drago.\n\r",ch);
		return;
	}
	
	if (drago && !MOB_FLAGGED(drago,MOB_HARD_TAME))
	{
		send_to_char("Non stai cavalcando un drago!\n\r",ch);
		return;
	}
	
	if (AFF_FLAGGED(drago, AFF_PARALIZE))
	{
		send_to_char("Il tuo drago non si puo' muovere!\n\r",ch);
		return;
	}
	
	talismano = get_obj_in_list_vis(ch, "dragonsign", ch->carrying);
	if (talismano && (GET_OBJ_TYPE(talismano)==ITEM_RENT_DRAGON) &&
		GET_OBJ_VAL(talismano, 3) == GET_MOB_VNUM(drago))
	{
		if (drago->affected)
		{
			for (aff = drago->affected; aff; aff = aff->next)
			{
				if (aff->type == PROF_DOMARE)
		  //ore
					tame_aff_time = aff->duration;
			}
		}
	 //    GET_OBJ_VAL(talismano, 0) = GET_NAME(drago);         // Problema qui
		GET_OBJ_VAL(talismano, 1) = GET_HIT(drago);
		GET_OBJ_VAL(talismano, 2) = GET_MOVE(drago);
		GET_OBJ_VAL(talismano, 3) = GET_MOB_VNUM(drago);
		GET_OBJ_VAL(talismano, 4) = time(0);
		GET_OBJ_VAL(talismano, 5) = tame_aff_time;
		GET_OBJ_VAL(talismano, 6) = GET_MANA(drago);
		GET_OBJ_VAL(talismano, 7) = 0;
	 /*smonti dal drago*/
		dismount_char(ch);
		act("Dai una pacca amicevole a $N.",FALSE, ch, 0, drago, TO_CHAR);
		act("$n da' una pacca amicevole a $N.",TRUE, ch, 0, drago, TO_ROOM);
		act("Scendi da $N.",FALSE, ch, 0, drago, TO_CHAR);
		act("$n scende da $N.", FALSE, ch, 0, drago, TO_ROOM);
		stop_follower(drago);
	 /*Rimuovi mount*/
		sprintf(buf,"&8Vedi &0$N&8 volare via in spirali sempre piu' alte ed infine allontanarsi all'orizzonte.&0\n\r");
		act(buf,TRUE, ch, 0, drago,TO_ROOM);
		act(buf,FALSE, ch, 0, drago, TO_CHAR);
		extract_char(drago);
	}
	else
		send_to_char ("Se lo liberi non tornera' piu', sei senza talismano!\r\n", ch);
}

/*
 *  Object Talismano
 *  Val 0 - Nome del drago
 *  Val 1 - HP del drago
 *  Val 2 - move del drago
 *  Val 3 - VNUM mob
 *  Val 4 - time rent
 *  Val 5 - ore di tame rimaste
 *  Val 6 - mana
 *  Val 7 - il drago e' presente
 *  Val 8 - Proprietario
 *
 *
 */

EVENTFUNC(recalldragon_event)
{
	struct char_data *ch;
	struct char_data *drago;
	struct event_data *evtime = (struct event_data *) event_obj;
	float num_of_mud_hours, num_of_mud_hours_rim;
	mob_rnum real_drago;
	int durata_tame_dragon;
	struct affected_type af;
	struct affected_type af2;
	int gain;
	void add_follower(struct char_data *ch, struct char_data *leader);
	
	gain = 2 + (evtime->times*2);
	ch = evtime->ch;
	
	if (!ch)
	{
	 /*sgancia l'evento*/
		free(event_obj);
		return 0;
	}
	
	if (   ROOM_FLAGGED(IN_ROOM(ch),ROOM_INDOORS) 
	    || !IS_WILD(IN_ROOM(ch)) 
	    || ROOM_FLAGGED(IN_ROOM(ch),ROOM_TUNNEL) 
	    || ROOM_FLAGGED(IN_ROOM(ch),ROOM_ARENA) 
	    || SECT(IN_ROOM(ch)) == SECT_WATER_NOSWIM 
	    || SECT(IN_ROOM(ch)) == SECT_FLYING 
	    || SECT(IN_ROOM(ch)) == SECT_UNDERWATER 
	    || SECT(IN_ROOM(ch)) == SECT_WATER_SWIM 
	   ) {
		sprintf(buf,"Il drago non puo' raggiungerti qui' dove sei. Ti devi spostare.\r\n");
		send_to_char(buf, ch);
		return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
	}
	
	evtime->times++;
	
	switch (evtime->times)
	{
		case 1:
			sprintf(buf,"Speri che il drago abbia sentito il tuo richiamo.");
			act(buf, FALSE, ch, 0, ch, TO_CHAR);
			return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
		case 2:
			sprintf(buf,"mmm, forse il drago si e' perso.");
			act(buf, FALSE, ch, 0, ch, TO_CHAR);
			return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
		case 3:
			sprintf(buf,"&8Intravedi in lontananza un enorme essere che si avvicina volando.&0");
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			act(buf,FALSE, ch, 0, 0, TO_CHAR);
			return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
		case 4:
			sprintf(buf,"&8Vedi in cielo un drago che vola scendendo, pian piano, in spirali sempre piu' piccole.&0");
			act(buf,TRUE, ch, 0, 0, TO_ROOM);
			act(buf,FALSE, ch, 0, 0, TO_CHAR);
			return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
		case 5:
			sprintf(buf,"&8Il volo radente del drago ti scompiglia i capelli.&0");
			act(buf, TRUE, ch, 0, 0, TO_ROOM);
			act(buf,FALSE, ch, 0, 0, TO_CHAR);
			return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
		case 6:
		//talismano = get_obj_in_list_vis(ch, "dragonsign", ch->carrying);
			if (!evtime->talismano)
			{
				send_to_char("Il talismano ha perso il suo potere.\r\n", ch);
				GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT) = NULL;
				free(event_obj);
				return 0;
			}
			real_drago = real_mobile(GET_OBJ_VAL(evtime->talismano, 3));
			if (real_drago == -1)
			{
				send_to_char("Il drago non e' piu' disponibile contattare un creatore.\r\n", ch);
		  /*sgancia l'evento*/
				GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT) = NULL;
				free(event_obj);
				return 0;
			}
			num_of_mud_hours_rim = GET_OBJ_VAL(evtime->talismano, 5);
			num_of_mud_hours = (float) (time(0) - GET_OBJ_VAL(evtime->talismano, 4)) / SECS_PER_MUD_HOUR;
			durata_tame_dragon = num_of_mud_hours_rim - num_of_mud_hours;
			if (durata_tame_dragon >0)
			{
				drago = read_mobile(real_drago, REAL);
				if (drago)
				{
					char_to_room(drago, ch->in_room);
					if (evtime->talismano) GET_OBJ_VAL(evtime->talismano,7) = 1;
			 /*Configura mob*/
					REMOVE_BIT(MOB_FLAGS(drago), MOB_AGGRESSIVE);
					REMOVE_BIT(MOB_FLAGS(drago), MOB_SPEC);
					SET_BIT(MOB_FLAGS(drago), MOB_SENTINEL);
					SET_BIT(MOB_FLAGS(drago), MOB_MOUNTABLE);
					SET_BIT(MOB_FLAGS(drago), MOB_DRAGONBREATH);
					SET_BIT(MOB_FLAGS(drago), MOB_HARD_TAME);
					GET_MANA(drago) = MIN(GET_OBJ_VAL(evtime->talismano, 6) + move_gain(drago) * num_of_mud_hours, GET_MAX_MANA(drago));
					GET_HIT(drago) = MIN(GET_OBJ_VAL(evtime->talismano, 1) + move_gain(drago) * num_of_mud_hours, GET_MAX_HIT(drago));
					GET_MOVE(drago) = MIN(GET_OBJ_VAL(evtime->talismano, 2) + move_gain(drago) * num_of_mud_hours, GET_MAX_MOVE(drago));
					
			 /*TAME*/
					af.natura = PROFICIENZE;
					af.type = PROF_DOMARE;
					af.duration = durata_tame_dragon;
					af.modifier = 0;
					af.location = APPLY_NONE;
					af.bitvector[0] = AFF_TAMED;
					af.bitvector[1] = 0;
					af.bitvector[2] = 0;
					af.bitvector[3] = 0;
					affect_join(drago, &af, FALSE, FALSE, FALSE, FALSE);
					
			 /*CHARM DRAGO*/
					af2.natura = SPELLSKILL;
					af2.type = SPELL_CHARM;
					af2.duration = durata_tame_dragon;
					af2.modifier = 0;
					af2.location = APPLY_NONE;
					af2.bitvector[0] = AFF_CHARM;
					af2.bitvector[1] = 0;
					af2.bitvector[2] = 0;
					af2.bitvector[3] = 0;
					affect_join(drago, &af2, FALSE, FALSE, FALSE, FALSE);
					
					sprintf(buf,"&8$N atterra davanti a &n sollevando un enorme polverone.&0\r\n");
					act(buf, TRUE, ch, 0, drago, TO_ROOM);
					act(buf,FALSE, ch, 0, drago, TO_CHAR);
					add_follower(drago,ch);
					GET_OBJ_VAL(evtime->talismano, 7) = 1;
				}  //if drago
			}
			else
			{
				sprintf(buf,"Il drago non ha risposto alla tua chiamata.\r\n");
				act(buf, TRUE, ch, 0, 0, TO_CHAR);
				if (evtime->talismano)
					extract_obj(evtime->talismano);
			}  // fine tempo di tame
			
		/*sgancia l'evento*/
			GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT) = NULL;
			free(event_obj);
			return 0;
	} //end switch
	return 0;
}

ACMD(do_recalldragon)
{
	struct event_data *evtime;
	struct obj_data *talismano;
	long time;
	int gain;
	
	talismano = get_obj_in_list_vis(ch, "dragonsign", ch->carrying);
	if (!talismano || (GET_OBJ_TYPE(talismano)!=ITEM_RENT_DRAGON))
	{
		send_to_char("Seee! ti piacerebbe avere un drago!\r\n",ch);
		return;
	}
	
	if (GET_OBJ_VAL(talismano, 7) == 1)
	{
		send_to_char("Dovrebbe essere nei paraggi...se e' ancora vivo, cercalo!\r\n",ch);
		return;
	}
	
	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_INDOORS) || !IS_WILD(IN_ROOM(ch)))
	{
		sprintf(buf,"Il drago non puo' raggiungerti qui' dove sei. Ti devi spostare.\r\n");
		send_to_char(buf, ch);
		return;
	}
	
	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_SOUNDPROOF))
	{
		sprintf(buf,"Il drago non puo' sentire il tuo richiamo da qui' dove sei. Ti devi spostare.\r\n");
		send_to_char(buf,ch);
		return;
	}
	
	act("Richiami il drago leggendo le iscrizioni del talismano.",FALSE, ch, 0, 0, TO_CHAR);
	act("$n prende un talismano e pronuncia frasi incomprensibili.",TRUE, ch, 0, 0, TO_ROOM);
	gain = 2;
	time = PULSES_PER_MUD_HOUR / (gain ? gain : 1);
	CREATE(evtime, struct event_data, 1);
	evtime->times = 0;
	evtime->talismano = talismano;
	evtime->ch = ch;
	
	if (   !GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT) 
	    || (time < event_time(GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT)))
	   ) {
		if (GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT))
			event_cancel(GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT));
		
		GET_POINTS_ACT_EVENTS(ch, DRAGON_RECALL_EVENT) = event_create(recalldragon_event, evtime, time);
	}
}


ACMD(do_dragonfly)
{
	int get_need_movement(struct char_data *ch, int dir);
	int nr_times, loop, max_movement,old_move, movecost, can_show;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int dir;
	
	if (!RIDING(ch)){
		send_to_char("Non stai cavalcando nessun drago.\r\n",ch);
		return;
	}
	
	if (!MOB_FLAGGED(RIDING(ch),MOB_HARD_TAME))
	{
		send_to_char("Quello che cavalchi non e' un drago.\r\n",ch);
		return;
	}
	
	argument = one_argument(argument, arg);
	if (!arg || !*arg)
	{
		send_to_char("In che direzione?\r\n", ch);
		return;
	}
	
	if ((dir = parse_dir(arg)) == -1)
		if ((dir = parse_dir_char(arg[1])) == -1)
		{
			sprintf(buf,"%s non e' una direzione.\r\n", arg);
			send_to_char(buf, ch);
			return;
		}
		
	if (dir < 0 || dir > 3)
	{
		send_to_char("Non e' una direzione valida.\r\n", ch);
		return;
	}
	
	argument = one_argument(argument, arg2);
	if (!arg2 || !*arg2)
		nr_times = 1;
	else {
		if(atoi(arg2) <= 0) nr_times = 0;
		else nr_times = atoi(arg2);
	}
	
	max_movement = 1 + GET_LEVEL(RIDING(ch))/20;
	can_show  = CAN_NOT_SHOW_ROOM;
	if (nr_times <= max_movement) {
		for(loop = 0; loop < nr_times; loop++)
		{
			old_move = GET_MOVE(RIDING(ch));
			movecost = get_need_movement(ch, dir);
			if (nr_times == loop + 1) can_show  = CAN_SHOW_ROOM;
			if (perform_move(ch, dir, 0, can_show) == 0)
			{
				if (loop > 0) {
					send_to_char("\r\n",ch);
					look_at_room(ch, 0);
				}
				break;
			}
			GET_MOVE(RIDING(ch)) = MAX(0 , old_move - (movecost * 2));
		}
		send_to_char("\r\n",ch);
	}
	else
	{
		sprintf(buf,"Non sei abbastanza esperto per volare piu' veloce di %d passi alla volta.\r\n", max_movement);
		send_to_char(buf, ch);
	}
}

int get_need_movement(struct char_data *ch, int dir)
{
	int need_movement=0;
	room_rnum target_rnum=ch->in_room;
	
	if (EXIT(ch, dir) && EXIT(ch, dir)->to_room != NOWHERE )
		target_rnum=EXIT(ch, dir)->to_room;
	else target_rnum = wild_target_room (ch, dir);
	
	if (target_rnum!=-1)
	{
		need_movement = (IS_WILD(ch->in_room) ? GET_WILD_TABLE(ch->in_room).move_cost : movement_loss[SECT(ch->in_room)]);
		need_movement += (IS_WILD(target_rnum) ? GET_WILD_TABLE(target_rnum).move_cost : movement_loss[SECT(target_rnum)]);
		need_movement = need_movement /2;
	}
	
	return need_movement;
}
