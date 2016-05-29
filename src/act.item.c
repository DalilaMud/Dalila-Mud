/* ************************************************************************
*   File: act.item.c                                    Part of CircleMUD *
*  Usage: object handling routines -- get/drop and container handling     *
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
#include "dg_scripts.h"
#include "house.h"
#include "wilderness.h"
#include "clan.h"
#include "mestieri.h"

/* extern variables */
extern struct str_app_type str_app[];
extern struct dex_app_type dex_app[];
extern struct index_data *obj_index;
extern struct room_data *world;
extern char *drinks[];
extern int drink_aff[][3];
extern struct abil_info_type abil_info[];
extern struct house_control_rec house_control[MAX_HOUSES];
int get_shopindexfromroom(room_rnum room);
int trade_with(struct obj_data * item, int shop_nr);
void unstow_char(struct char_data *ch);
void stow_char(struct char_data *ch,struct char_data *mount);
void restring(struct obj_data *obj,struct obj_data *arg);
void wield_weapon(struct char_data * ch, struct obj_data * obj);
//void check_abil_obj(struct char_data *ch,struct obj_data *obj,bool wear);
int check_gen_abil(struct char_data *ch,int abilita);
bool check_free_hands(struct char_data *ch,struct obj_data *obj);
extern int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);
extern int objlevel (struct obj_data * obj);
struct obj_data *get_obj_in_list_particular(struct char_data * ch, char *name,struct obj_data * list,int obj_type);
void safe_save_pg(struct char_data *ch);
extern long num_obj_in_obj_mag (struct obj_data * object);
extern long num_obj_in_mag (room_rnum room);
extern struct spell_info_type spell_info[];
extern int get_stipendiomob(struct char_data *ch);
int find_house(room_vnum vnum);


void perform_wear(struct char_data * ch, struct obj_data * obj, int where);

void perform_put(struct char_data * ch, struct obj_data * obj,
	struct obj_data * cont)
{
	if (!drop_otrigger(obj, ch))
		return;
	
	if (GET_OBJ_TYPE(obj)==ITEM_MONEY) {
		send_to_char("Non puoi mettere dei coins in un contenitore.\r\n",ch);
		return;
	}
	if(GET_OBJ_VNUM(obj)==81 && GET_OBJ_VNUM(cont)!=80){
		send_to_char("Gli attrezzi per evadere vanno messi negli appositi contenitori.\r\n",ch);
		return;
	}
	if(GET_OBJ_VNUM(obj)==80){
		send_to_char("Non puoi mettere questo tipo di oggetto in un contenitore.\r\n",ch);
		return;
	}
	if (GET_OBJ_WEIGHT(cont) + GET_OBJ_WEIGHT(obj) > GET_OBJ_VAL(cont, 0))
		act("Non riesci a mettere $p in $P.", FALSE, ch, obj, cont, TO_CHAR);
	else {
		if((cont->in_room!=NOWHERE) && ROOM_FLAGGED(cont->in_room, ROOM_HOUSE)) {
			if( ((ROOM_FLAGGED(cont->in_room,ROOM_BIG_HOUSE)) &&
			    (num_obj_in_mag(cont->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_BIGHOUSE)) ||
			    ((num_obj_in_mag(cont->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_INROOM) &&
			    !ROOM_FLAGGED(cont->in_room,ROOM_BIG_HOUSE))) {
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return;
			}
		}
		/*if ((cont->in_room!=NOWHERE) && (ROOM_FLAGGED(cont->in_room,ROOM_HOUSE)) &&
			((num_obj_in_mag(cont->in_room) + num_obj_in_obj_mag(obj)) > MAX_OBJ_INROOM))	 
		{
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		else if ((cont->in_room!=NOWHERE) && (ROOM_FLAGGED(cont->in_room,ROOM_BIG_HOUSE)) &&
			(ROOM_FLAGGED(cont->in_room,ROOM_BIG_HOUSE)) &&
			((num_obj_in_mag(cont->in_room) + num_obj_in_obj_mag(obj)) > MAX_OBJ_BIGHOUSE))	 
		{
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		} else {  */
		obj_from_char(obj);
		obj_to_obj(obj, cont);
	/*Save room object quando metto un obj nel contenitore Shade*/
		if ((cont->in_room!=NOWHERE)&&(ROOM_FLAGGED(cont->in_room,ROOM_HOUSE)))	
			SET_BIT(ROOM_FLAGS(cont->in_room), ROOM_HOUSE_CRASH);
		act("Metti $p in $P.", FALSE, ch, obj, cont, TO_CHAR);
		act("$n mette $p in $P.", TRUE, ch, obj, cont, TO_ROOM);
		
	}
}

/* The following put modes are supported by the code below:

	1) put <object> <container>
	2) put all.<object> <container>
	3) put all <container>

	<container> must be in inventory or on ground.
	all objects to be put into container must be in inventory.
*/

ACMD(do_put)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct obj_data *obj, *next_obj, *cont;
	struct char_data *tmp_char;
	int obj_dotmode, cont_dotmode, found = 0;
	int put_num, amount;   /* Added by Darksword */
	
	two_arguments(argument, arg1, arg2);
	obj_dotmode = find_all_dots(arg1);
	cont_dotmode = find_all_dots(arg2);
  /* Enrico*/
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
   /*      */
	if (!*arg1)
		send_to_char("Mettere cosa dove?\r\n", ch);
	else if (cont_dotmode != FIND_INDIV)
		send_to_char("Puoi solo mettere una cosa alla volta.\r\n", ch);
	
	
  /* ADDED by Darksword */
	else if (is_number(arg1)) {
		put_num = atoi(arg1);
		argument = two_arguments(argument, arg1, arg2);
		one_argument(argument, arg1);
		if (!*arg2)
			send_to_char("Mettere cosa dove?\r\n", ch);
		else if (!*arg1) {
			sprintf(buf, "Cosa vuoi mettere in %s?\r\n", arg2);
			send_to_char(buf, ch);
		} else {
			generic_find(arg1, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
			if (!cont) {
				sprintf(buf, "Non vedi %s %s qui.\r\n", AN(arg1), arg1);
				send_to_char(buf, ch);
			} else if (GET_OBJ_TYPE(cont) != ITEM_CONTAINER)
				act("$p non e' un contenitore.", FALSE, ch, cont, 0, TO_CHAR);
			else if (IS_SET(GET_OBJ_VAL(cont, 1), CONT_CLOSED))
				send_to_char("Meglio aprirlo prima!\r\n", ch);
			else {
      /* ADDED by Darksword put # <obj> <container> */
				for  (amount = 0; amount < put_num; amount++)	
				    if (!(obj = get_obj_in_list_vis(ch, arg2, ch->carrying))) {
						sprintf(buf, "Non stai portando %s %s.\r\n", AN(arg2), arg2);
						send_to_char(buf, ch);
						amount = put_num;
					} else if (obj == cont) {
						send_to_char("Stai cercandi di metterlo dentro se stesso, e fallisci.\r\n", ch);
						amount = put_num;
					}	else
						perform_put(ch, obj, cont);
						
			}			
		}
  /* END of Darksword */
		
	} else if (!*arg2) {
		sprintf(buf, "Cosa vuoi mettere dentro %s?\r\n",
			((obj_dotmode == FIND_INDIV) ? "esso" : "essi"));
		send_to_char(buf, ch);
	} else {
		generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
		if (!cont) {
			sprintf(buf, "Non vedi %s %s qui.\r\n", AN(arg2), arg2);
			send_to_char(buf, ch);
		} else if (GET_OBJ_TYPE(cont) != ITEM_CONTAINER)
			act("$p non e' un contenitore.", FALSE, ch, cont, 0, TO_CHAR);
		else if (IS_SET(GET_OBJ_VAL(cont, 1), CONT_CLOSED))
			send_to_char("Meglio aprirlo prima!\r\n", ch);
		else {
			if (obj_dotmode == FIND_INDIV) {	/* put <obj> <container> */
				if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
					sprintf(buf, "Non stai portando %s %s.\r\n", AN(arg1), arg1);
					send_to_char(buf, ch);
				} else if (obj == cont)
					send_to_char("Stai cercando di metterlo dentro se stesso, ma fallisci.\r\n", ch);
				else
					perform_put(ch, obj, cont);
			} else {
				for (obj = ch->carrying; obj; obj = next_obj) {
					next_obj = obj->next_content;
					if (obj != cont && CAN_SEE_OBJ(ch, obj) &&
						(obj_dotmode == FIND_ALL || isname(arg1, obj->name))) {
						found = 1;
						perform_put(ch, obj, cont);
					}
				}
				if (!found) {
					if (obj_dotmode == FIND_ALL)
						send_to_char("Non sembra tu abbia niente da mettere dentro.\r\n", ch);
					else {
						sprintf(buf, "Non sembra tu abbia nessun %s.\r\n", arg1);
						send_to_char(buf, ch);
					}
				}
			}
		}
	}
}

int can_take_container(struct char_data * ch, struct obj_data * obj)
{
	struct obj_data *cont;
	
	for (cont= obj->contains; cont; cont=cont->next_content)
	{
		if ((GET_LEVEL(ch)<GET_OBJ_LEVEL(cont)) || ( GET_OBJ_LEVEL(cont)<=0 && (GET_LEVEL(ch)<objlevel(cont))) ) return 0;
		if ((GET_OBJ_TYPE(cont) == ITEM_CONTAINER) && (!can_take_container(ch,cont))) return 0;
	}
	return 1;
}

int can_take_obj(struct char_data * ch, struct obj_data * obj) {

    struct obj_data *cont;	 //Orione
    int questobj_inside = 0; //Orione
	
	if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
	{
		act("$p: non puoi portare cosi' tante cose.", FALSE, ch, obj, 0, TO_CHAR);
		return 0;
	}
	if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) > CAN_CARRY_W(ch))
	{
		act("$p: non puoi portare cosi' tanto peso.", FALSE, ch, obj, 0, TO_CHAR);
		return 0;
	}
	if (!(CAN_WEAR(obj, ITEM_WEAR_TAKE)))
	{
		act("$p: non puoi prenderlo!", FALSE, ch, obj, 0, TO_CHAR);
		return 0;
	}
/*
  if ((GET_LEVEL(ch)<GET_OBJ_LEVEL(obj)) || ((GET_OBJ_LEVEL(obj)<=0) && (GET_LEVEL(ch)<objlevel(obj)) ) )
    {
    act("Sei troppo inesperto per poter prendere $p, rinunci.", FALSE, ch, obj, 0, TO_CHAR);
    return 0;
    }
  if ((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && (!can_take_container(ch,obj)))
    {
    act("All'interno di $p si trova qualcosa di troppo potente per poter essere preso da te.", FALSE, ch, obj, 0, TO_CHAR);
    return 0;
    }
*/

    /* Orione Inizio */
    if (GET_LEVEL(ch)<LVL_IMMORT) {
	if (IS_OBJ_STAT(obj, ITEM_QUEST)) {
	    if (IS_QUESTOR(ch) && \
		((GET_TIPOQUEST(ch)==QUEST_GETITEM) || (GET_TIPOQUEST(ch)==QUEST_GIVITEM) || (GET_TIPOQUEST(ch)==QUEST_GOTROOM)) && \
		(GET_QUESTOBJ(ch)==GET_OBJ_VNUM(obj))) {
		    if ((GET_QUESTMRR(ch)>0) && (GET_QUESTMRR(ch)==real_room(world[ch->in_room].number)))
			GET_QUESTMRR(ch) = -5;
	    } else {
		act("$p: non puoi prendere oggetti destinati ad altri!", FALSE, ch, obj, 0, TO_CHAR);
		return 0;
	    }
	}
	if (GET_OBJ_TYPE(obj)==ITEM_CONTAINER) {
	    for (cont= obj->contains; cont; cont=cont->next_content) {
		if ((GET_OBJ_TYPE(cont) == ITEM_CONTAINER) && (!can_take_obj(ch,cont))) return 0;
		
		if (IS_OBJ_STAT(cont, ITEM_QUEST)) {
		    if (!(IS_QUESTOR(ch) && \
			((GET_TIPOQUEST(ch)==QUEST_GETITEM) || (GET_TIPOQUEST(ch)==QUEST_GIVITEM) || (GET_TIPOQUEST(ch)==QUEST_GOTROOM)) && \
			(GET_QUESTOBJ(ch)==GET_OBJ_VNUM(cont)))) {
			    act("$p: contiene oggetti destinati ad altri!", FALSE, ch, obj, 0, TO_CHAR);
			    return 0;
		    }
		    questobj_inside = TRUE;
		    break;
		}
	    }
	    if (questobj_inside && (GET_QUESTMRR(ch)>0) && (GET_QUESTMRR(ch)==real_room(world[ch->in_room].number)))
		GET_QUESTMRR(ch) = -5;
	}
    }
    /* Orione Fine */

	return 1;
}


void get_check_money(struct char_data * ch, struct obj_data * obj)
{
	if ((GET_OBJ_TYPE(obj) == ITEM_MONEY) && (GET_OBJ_VAL(obj, 0) > 0)) {
		obj_from_char(obj);
		if (GET_OBJ_VAL(obj, 0) > 1) {
			sprintf(buf, "C'erano %d coins.\r\n", GET_OBJ_VAL(obj, 0));
			send_to_char(buf, ch);
		}
		GET_GOLD(ch) += GET_OBJ_VAL(obj, 0);
		extract_obj(obj);
	}
}


void perform_get_from_container(struct char_data * ch, struct obj_data * obj,
	struct obj_data * cont, int mode)
{
	//extern void House_crashsave(room_vnum vnum);
	if (mode == FIND_OBJ_INV || can_take_obj(ch, obj)) {
		if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
			act("$p: non puoi portare niente di piu'.", FALSE, ch, obj, 0, TO_CHAR);
		else if (get_otrigger(obj, ch)) {
			obj_from_obj(obj);
			obj_to_char(obj, ch);
	  /*Save room object quando metto un obj nel contenitore Shade*/
			if ((cont->in_room!=NOWHERE)&&(ROOM_FLAGGED(cont->in_room,ROOM_HOUSE)))	
				SET_BIT(ROOM_FLAGS(cont->in_room), ROOM_HOUSE_CRASH);
			//House_crashsave(world[cont->in_room].number);
			act("Prendi $p da $P.", FALSE, ch, obj, cont, TO_CHAR);
			act("$n prende $p da $P.", TRUE, ch, obj, cont, TO_ROOM);
			get_check_money(ch, obj);
		}
	}
}


void get_from_container(struct char_data * ch, struct obj_data * cont,
	char *arg, int mode)
{
	struct obj_data *obj, *next_obj;
	int obj_dotmode, found = 0;
	
	obj_dotmode = find_all_dots(arg);
	
	if (IS_SET(GET_OBJ_VAL(cont, 1), CONT_CLOSED))
		act("$p e' chiuso.", FALSE, ch, cont, 0, TO_CHAR);
	else if (obj_dotmode == FIND_INDIV) {
		if (!(obj = get_obj_in_list_vis(ch, arg, cont->contains))) {
			sprintf(buf, "Non sembra esserci %s %s dentro $p.", AN(arg), arg);
			act(buf, FALSE, ch, cont, 0, TO_CHAR);
		} else{
			if(GET_OBJ_TYPE(obj)==ITEM_RELIQUIA){
				if(GET_EQ(ch,WEAR_RELIQUIA)){
					send_to_char("Il tuo corpo non resisterebbe alla potenza di piu' reliquie",ch);
					return;
				}
				perform_get_from_container(ch,obj,cont,mode);
				perform_wear(ch,obj,WEAR_RELIQUIA);
				return;
			}
			else perform_get_from_container(ch, obj, cont, mode);
		}} else {
		if (obj_dotmode == FIND_ALLDOT && !*arg) {
			send_to_char("Prendere tutto di cosa?\r\n", ch);
			return;
		}
		for (obj = cont->contains; obj; obj = next_obj) {
			next_obj = obj->next_content;
			if (CAN_SEE_OBJ(ch, obj) &&
				(obj_dotmode == FIND_ALL || isname(arg, obj->name))) {
				found = 1;
				if(GET_OBJ_TYPE(obj)==ITEM_RELIQUIA){
					if(GET_EQ(ch,WEAR_RELIQUIA)){
						send_to_char("il tuo corpo non resisterebbe alla potenza di piu' reliquie",ch);
						return;
					}
					perform_get_from_container(ch,obj,cont,mode);
					perform_wear(ch,obj,WEAR_RELIQUIA);
					return;
				}
				else perform_get_from_container(ch, obj, cont, mode);
			}
		}
		if (!found) {
			if (obj_dotmode == FIND_ALL)
				act("$p sembra essere vuoto.", FALSE, ch, cont, 0, TO_CHAR);
			else {
				sprintf(buf, "Non sembra ci siano %s dentro $p.", arg);
				act(buf, FALSE, ch, cont, 0, TO_CHAR);
			}
		}
	}
}


int perform_get_from_room(struct char_data * ch, struct obj_data * obj)
{
	if (can_take_obj(ch, obj) && get_otrigger(obj, ch)) {
		obj_from_room(obj);
		obj_to_char(obj, ch);
		act("Prendi $p.", FALSE, ch, obj, 0, TO_CHAR);
		act("$n prende $p.", TRUE, ch, obj, 0, TO_ROOM);
		get_check_money(ch, obj);
		return 1;
	}
	return 0;
}


void get_from_room(struct char_data * ch, char *arg)
{
	struct obj_data *obj, *next_obj;
	int dotmode, found = 0;
	dotmode = find_all_dots(arg);
	
	if (dotmode == FIND_INDIV) {
		if (!(obj = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
			sprintf(buf, "Non vedi %s qui.\r\n", arg);
			send_to_char(buf, ch);
		} else{  /*RELIQUIE PEPPE*/
			if(GET_OBJ_TYPE(obj)==ITEM_RELIQUIA){
				if (GET_EQ(ch,WEAR_RELIQUIA)){
					send_to_char("il tuo corpo non resisterebbe alla potenza di piu reliquie",ch);
					return;
				}
				perform_get_from_room(ch,obj);
				perform_wear(ch,obj,WEAR_RELIQUIA);
				return;
			}			
			perform_get_from_room(ch, obj);
		}
	} else {
		if (dotmode == FIND_ALLDOT && !*arg) {
			send_to_char("Prendi tutto cosa?\r\n", ch);
			return;
		}
		for (obj = world[ch->in_room].contents; obj; obj = next_obj) {
			next_obj = obj->next_content;
			if (CAN_SEE_OBJ(ch, obj) &&
				(dotmode == FIND_ALL || isname(arg, obj->name))) {
				found = 1;
				if(GET_OBJ_TYPE(obj)!=ITEM_RELIQUIA){
					perform_get_from_room(ch, obj);
				}
				
				else{
					if (GET_EQ(ch,WEAR_RELIQUIA)){
						send_to_char("il tuo corpo non resisterebbe alla potenza di piu reliquie",ch);
					}
					
					else {
						perform_get_from_room(ch,obj);
						perform_wear(ch,obj,WEAR_RELIQUIA);
					}
				}
			}
		}
		if (!found) {
			if (dotmode == FIND_ALL)
				send_to_char("Non sembra esserci molto qui.\r\n", ch);
			else {
				sprintf(buf, "Non vedi nessun %s qui.\r\n", arg);
				send_to_char(buf, ch);
			}
		}
	}
}

ACMD(do_get)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	int cont_dotmode, found = 0, mode;
	struct obj_data *cont;
	struct char_data *tmp_char;
	int amount, get_num;    /* Added by Darksword */
	extern void corpse_update (struct obj_data *corpo);
	
	two_arguments(argument, arg1, arg2);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
		send_to_char("Le tue mani sembrano essere piene!\r\n", ch);
	else if (!*arg1)
		send_to_char("Prendere cosa?\r\n", ch);
	
 /* ADDED by Darksword */
 /* The following code allows you to get # items from room or container */
	
	else if (is_number(arg1))
	{
		get_num = atoi(arg1);
		argument = two_arguments(argument, arg1, arg2);
		one_argument(argument, arg1);
		if (!*arg2)
			send_to_char("Prendere cosa?\r\n", ch);
		else if (!*arg1)
		{
			for  (amount = 0; amount < get_num; amount++)
				if (!(obj = get_obj_in_list_vis(ch, arg2, world[ch->in_room].contents)))
				{
					sprintf(buf, "Non vedi %s qui.\r\n", arg2);
					send_to_char(buf, ch);
					amount = get_num;
				}
				else perform_get_from_room(ch, obj);
		}
		else
		{
			mode = generic_find(arg1, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
			if (!cont)
			{
				sprintf(buf, "Non hai %s.\r\n", arg1);
				send_to_char(buf, ch);
			}
			else
			if (GET_OBJ_TYPE(cont) != ITEM_CONTAINER)
				act("$p non e' un contenitore.", FALSE, ch, cont, 0, TO_CHAR);
			else
			{
				for  (amount = 0; amount < get_num; amount++)
					if (!(obj = get_obj_in_list_vis(ch, arg2, cont->contains)))
					{
						sprintf(buf, "Non sembra esserci %s dentro %s.\r\n", arg2, arg1);
						send_to_char(buf, ch);
						amount = get_num;
					}
					else get_from_container(ch, cont, arg2, mode);
				if (IS_PC_CORPSE (cont))
					corpse_update (cont);
			}
		}
	}
  /* END of Darksword */
	
	else if (!*arg2) get_from_room(ch, arg1);
	else
	{
		cont_dotmode = find_all_dots(arg2);
		if (cont_dotmode == FIND_INDIV)
		{
			mode = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
			if (!cont)
			{
				sprintf(buf, "Non hai %s.\r\n", arg2);
				send_to_char(buf, ch);
			}
			else if (GET_OBJ_TYPE(cont) != ITEM_CONTAINER)
				act("$p non e' un contenitore.", FALSE, ch, cont, 0, TO_CHAR);
			else
			{
				get_from_container(ch, cont, arg1, mode);
				if (IS_PC_CORPSE (cont))
					corpse_update (cont);
			}
		}
		else
		{
			if (cont_dotmode == FIND_ALLDOT && !*arg2)
			{
				send_to_char("Prendere tutto cosa?\r\n", ch);
				return;
			}
			for (cont = ch->carrying; cont; cont = cont->next_content)
				if (CAN_SEE_OBJ_DARK(ch, cont) && ((cont_dotmode == FIND_ALL) || isname(arg2, cont->name))){
					if (GET_OBJ_TYPE(cont) == ITEM_CONTAINER)
					{
						found = 1;
						get_from_container(ch, cont, arg1, FIND_OBJ_INV);
						if (IS_PC_CORPSE (cont))
							corpse_update (cont);
					}
					else if (cont_dotmode == FIND_ALLDOT)
					{
						found = 1;
						act("$p non e' un contenitore.", FALSE, ch, cont, 0, TO_CHAR);
					}
				}	
			for (cont = world[ch->in_room].contents; cont; cont = cont->next_content)
				if (CAN_SEE_OBJ_DARK(ch, cont) && ((cont_dotmode == FIND_ALL) || isname(arg2, cont->name))){
					if (GET_OBJ_TYPE(cont) == ITEM_CONTAINER)
					{
						get_from_container(ch, cont, arg1, FIND_OBJ_ROOM);
						found = 1;
						if (IS_PC_CORPSE (cont))
							corpse_update (cont);
					}
					else if (cont_dotmode == FIND_ALLDOT)
					{
						act("$p non e' un contenitore.", FALSE, ch, cont, 0, TO_CHAR);
						found = 1;
					}
				}	
			if (!found)
			{
				if (cont_dotmode == FIND_ALL) send_to_char("Non sembra tu possa trovare un contenitore.\r\n", ch);
				else
				{
					sprintf(buf, "Non sembra tu possa trovare nessun %s qui.\r\n", arg2);
					send_to_char(buf, ch);
				}
			}
		}
	}
}


void perform_drop_gold(struct char_data * ch, int amount,
	byte mode, room_rnum RDR)
{
	struct obj_data *obj;
	
	if (amount <= 0)
		send_to_char("Heh heh heh.. siamo giocherelloni oggi, eh?\r\n", ch);
	else if (GET_GOLD(ch) < amount)
		send_to_char("Non hai quei coins!\r\n", ch);
	else {
		if (mode != SCMD_JUNK) {
			WAIT_STATE(ch, PULSE_VIOLENCE);	/* to prevent coin-bombing */
			obj = create_money(amount);
			if (mode == SCMD_DONATE) {
				send_to_char("Tiri dei coins nell'aria e svaniscono in una nuvoletta di fumo!\r\n", ch);
				act("$n tira dei coins in aria che svaniscono in una nuvoletta di fumo!",
					FALSE, ch, 0, 0, TO_ROOM);
				obj_to_room(obj, RDR);
				act("$p improvvisamente appare in una nuvoletta arancione!", 0, 0, obj, 0, TO_ROOM);
			} else {
				if (!drop_wtrigger(obj, ch)) {
					extract_obj(obj);
					return;
				}
				send_to_char("Lasci alcuni coins per terra.\r\n", ch);
				sprintf(buf, "$n lascia cadere %s.", money_desc(amount));
				act(buf, TRUE, ch, 0, 0, TO_ROOM);
				obj_to_room(obj, ch->in_room);
			}
		} else {
			sprintf(buf, "$n fa cadere %s che sparisce in una nuvoletta arancione!",
				money_desc(amount));
			act(buf, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char("Lasci cadere alcuni coins che spariscono in una nuvoletta di fumo arancione!\r\n", ch);
		}
		GET_GOLD(ch) -= amount;
	}
}


#define VANISH(mode) ((mode == SCMD_DONATE || mode == SCMD_JUNK) ? "  Svanisce in una nuvoletta di fumo!" : "")

int perform_drop(struct char_data * ch, struct obj_data * obj,
	byte mode, char *sname, room_rnum RDR)
{
	int value;
	struct obj_data *cont;	 //Orione
    
    	if (!drop_otrigger(obj, ch))
		return 0;
	if ((mode == SCMD_DROP) && !drop_wtrigger(obj, ch))
		return 0;
	
	if (IS_OBJ_STAT(obj, ITEM_NODROP)) {
		sprintf(buf, "Non puoi %s $p, e' stato maledetto!", sname);
		act(buf, FALSE, ch, obj, 0, TO_CHAR);
		return 0;
	}
	
        /* Orione Inizio */
	if (IS_OBJ_STAT(obj, ITEM_QUEST)) {
	    if ((mode == SCMD_DONATE) || (mode == SCMD_JUNK)) {
		sprintf(buf, "Non si possono %s oggetti di quest!\r\n", sname);
		send_to_char(buf, ch);
		return 0;
	    }
	}
	if (GET_OBJ_TYPE(obj)==ITEM_CONTAINER) {
	    for (cont = obj->contains; cont; cont=cont->next_content) {
		if (IS_OBJ_STAT(cont, ITEM_NODROP)) {
		    sprintf(buf, "Contiene oggetti maledetti, non si puo' %s!", sname);
		    act(buf, FALSE, ch, cont, 0, TO_CHAR);
		    return 0;
		}
		if (GET_OBJ_TYPE(cont)==ITEM_CONTAINER) {
		    sprintf(buf, "Troppi contenitori, sembrano delle matrioske!");
		    act(buf, FALSE, ch, obj, 0, TO_CHAR);
		    return 0;
		}
	        if (IS_OBJ_STAT(cont, ITEM_QUEST) && ((mode == SCMD_DONATE) || (mode == SCMD_JUNK))) {
		    sprintf(buf, "Contiene oggetti di quest che non si possono %s!\r\n", sname);
	            send_to_char(buf, ch);
		    return 0;
	        }
	    }
	}
	/* Orione Fine */
	
	if((mode == SCMD_DROP) && (ch->in_room!=NOWHERE) && ROOM_FLAGGED(ch->in_room, ROOM_HOUSE)) {
			if( ((ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE)) &&
			    (num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_BIGHOUSE)) ||
			    ((num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_INROOM) &&
			    !ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE))) {
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return 0;
		}
	}
										    						    /*
	if ((mode == SCMD_DROP) && (ch->in_room!=NOWHERE) && (ROOM_FLAGGED(ch->in_room,ROOM_HOUSE)) &&
		(num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_INROOM))	 
	{
		act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
		return 0;
	}	 
	
	
	if ((mode == SCMD_DROP) && (ch->in_room!=NOWHERE) && (ROOM_FLAGGED(ch->in_room,ROOM_HOUSE)) &&
		(ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE)) &&
			(num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) >  MAX_OBJ_BIGHOUSE))	 
		{
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return 0;
		}*/
	
	if(mode==SCMD_JUNK){
	sprintf(buf, "Perdi un po' di tempo per riuscire a capire come distruggere $p.");
	act(buf, FALSE, ch, obj, 0, TO_CHAR);
	sprintf(buf, "Perde un po' di tempo per capire come distruggere $p.");
	act(buf, TRUE, ch, obj, 0, TO_ROOM);
	WAIT_STATE(ch, PULSE_VIOLENCE*4);
	}
	sprintf(buf, "Decidi di %s $p. %s", sname, VANISH(mode));
	act(buf, FALSE, ch, obj, 0, TO_CHAR);
	sprintf(buf, "$n decide di %s $p. %s", sname, VANISH(mode));
	act(buf, TRUE, ch, obj, 0, TO_ROOM);
	obj_from_char(obj);
	
	if ((mode == SCMD_DONATE) && IS_OBJ_STAT(obj, ITEM_NODONATE))
		mode = SCMD_JUNK;
	
	switch (mode) {
		case SCMD_DROP:
			obj_to_room(obj, ch->in_room);
			return 0;
			break;
		case SCMD_DONATE:
			if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE)){
							send_to_char("Non puoi donare dall'interno di una casa.\r\n", ch);
							return 0;
							break;
			}		
			obj_to_room(obj, RDR);
			act("$p improvvisamente appare in una nuvoletta di fumo!", FALSE, 0, obj, 0, TO_ROOM);
			return 0;
			break;
		case SCMD_JUNK:
			value = MAX(1, MIN(200, GET_OBJ_COST(obj) >> 4));
			extract_obj(obj);
			return value;
			break;
		default:
			log("SYSERR: Incorrect argument passed to perform_drop");
			break;
	}
	
	return 0;
}



ACMD(do_drop)
{
	extern room_vnum donation_room_2;
	extern room_vnum donation_room_3;
	extern room_vnum donation_room_5;
	extern room_vnum donation_room_6;
	
	struct obj_data *obj, *next_obj;
	room_rnum RDR = 0;
	byte mode = SCMD_DROP;
	int dotmode, amount = 0;
	char *sname;
	int drop_num;  /* Added by Darksword -- to drop multiple items */
	int c;
	int rndjnk;
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	
	switch (subcmd) {
		case SCMD_JUNK:
			sname = "junkare";
			mode = SCMD_JUNK;
			break;
		case SCMD_DONATE:
			sname = "donare";
			mode = SCMD_DONATE;
			rndjnk = number(0,100);
			
			if (rndjnk <= 15) {
				mode = SCMD_JUNK;
				break;
			}
			
			if (rndjnk <= 40) {
				switch (number(1, 4)) {					
					case 1:
						RDR = real_room(donation_room_2);
						break;
					case 2:
						RDR = real_room(donation_room_3);
						break;
					case 3:
						RDR = real_room(donation_room_5);
						break;
					case 4:
						RDR = real_room(donation_room_6);
						break;
				}
			}			
			if (rndjnk <= 100) {
				
				if ((c=GET_CLAN(ch))==2) {
					RDR = real_room(donation_room_2);
				}
				else
				if ((c=GET_CLAN(ch))==3) {
					RDR = real_room(donation_room_3);
				}
				else
				if ((c=GET_CLAN(ch))==5) {
					RDR = real_room(donation_room_5);
				}
				else
				if ((c=GET_CLAN(ch))==6)	{
					RDR = real_room(donation_room_6);
				}
				else
				if (RDR == NOWHERE) {
					send_to_char("Spiacente, ora non puoi donare.\r\n", ch);
					return;
				}
			}
			break;
		default:
			sname = "posare";
			break;
	}
	
	argument = one_argument(argument, arg);
	
	if (!*arg) {
		sprintf(buf, "Cosa vuoi %s?\r\n", sname);
		send_to_char(buf, ch);
		return;
	} else
	if (is_number(arg)) {
		amount = atoi(arg);
		argument = one_argument(argument, arg);
		if (!str_cmp("coins", arg) || !str_cmp("coin", arg))
			perform_drop_gold(ch, amount, mode, RDR);
		else {
			if (amount  < 0) {
				send_to_char("Non puoi farlo.\r\n", ch);
				return;
			}
			else {
        /* Code to drop multiple items added by Darksword */
        /* added variables  int  drop_num           */
				drop_num = amount;
				for  (amount = 0; amount < drop_num; amount++)
					if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
						sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
						send_to_char(buf, ch);
						amount = drop_num; /* just let them know no more then stop */
					} else
						amount += perform_drop(ch, obj, mode, sname, RDR);
			}
		}
		return;
	} else {
		dotmode = find_all_dots(arg);
    /* Can't junk or donate all */
		if ((dotmode == FIND_ALL) && (subcmd == SCMD_JUNK || subcmd == SCMD_DONATE)) {
			if (subcmd == SCMD_JUNK)
				send_to_char("Go to the dump if you want to junk EVERYTHING!\r\n", ch);
			else
				send_to_char("Go do the donation room if you want to donate EVERYTHING!\r\n", ch);
			return;
		}
		if (dotmode == FIND_ALL) {
			if (!ch->carrying)
				send_to_char("Sembra che tu non abbia niente.\r\n", ch);
			else
				for (obj = ch->carrying; obj; obj = next_obj) {
					next_obj = obj->next_content;
					amount += perform_drop(ch, obj, mode, sname, RDR);
				}
		} else if (dotmode == FIND_ALLDOT) {
			if (!*arg) {
				sprintf(buf, "Cosa vorresti %s?\r\n", sname);
				send_to_char(buf, ch);
				return;
			}
			if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
				sprintf(buf, "Non sembra tu abbia alcun %s.\r\n", arg);
				send_to_char(buf, ch);
			}
			while (obj) {
				next_obj = get_obj_in_list_vis(ch, arg, obj->next_content);
				amount += perform_drop(ch, obj, mode, sname, RDR);
				obj = next_obj;
			}
		} else {
			if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
				sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
				send_to_char(buf, ch);
			} else
				amount += perform_drop(ch, obj, mode, sname, RDR);
		}
	}
	
	if (amount && (subcmd == SCMD_JUNK)) {
		send_to_char("Sei stato premiato dagli Dei!\r\n", ch);
		act("$n e' stato premiato dagli Dei!", TRUE, ch, 0, 0, TO_ROOM);
		GET_GOLD(ch) += 1;
	}
}

void perform_give(struct char_data * ch, struct char_data * vict,
	struct obj_data * obj)
{
	long shop = -1;
	int result;
	
	ACMD(do_eat);
	
	if (IS_OBJ_STAT(obj, ITEM_NODROP))
	{
		act("Non puoi rimuovere $p.  Yeech!", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
  /*Adriano :In fase di sperimantezione*/
	/*if (MOB_FLAGGED(vict,MOB_MOUNTABLE))
	{
		if (GET_OBJ_TYPE(obj)==ITEM_MOUNT_FOOD)
		{
			do_eat(vict, obj->name, 0, SCMD_EAT);
			GET_MOVE(vict)= MIN(GET_MOVE(vict) + (GET_MAX_MOVE(vict) * 0.1), GET_MAX_MOVE(vict));
			act("$N mangia...", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
	}*/
	if(vict && !IS_NPC(vict) && PLR_FLAGGED(vict,PLR_FANTASMA)){
			send_to_char("Non puoi dare nulla ad un fantasma.\r\n",ch);
			return;
	}
	if(vict && !IS_NPC(vict) && !PRF_FLAGGED(vict,PRF_RPG)){
		send_to_char("Non puoi darglielo,non sta interpretando il suo personaggio.\r\n",ch);
		return;
	}
	if (!MOB_FLAGGED(vict,MOB_SHOPKEEPER))
	{
		if (MOB_FLAGGED(vict,MOB_MOUNTABLE) && IS_CARRYING_N(vict) >= GET_LEVEL(vict))
		{
			act("$N sembra avere le sacche piene.", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
		
		if (IS_CARRYING_N(vict) >= CAN_CARRY_N(vict))
		{
			act("$N sembra avere le mani piene.", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
		
		if (GET_OBJ_WEIGHT(obj) + IS_CARRYING_W(vict) > CAN_CARRY_W(vict))
		{
			act("$N non puo' portare cosi' tanto peso.", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
	}
	
	if (!IS_NPC(vict) && ((GET_LEVEL(vict)<GET_OBJ_LEVEL(obj)) || ( GET_OBJ_LEVEL(obj)<=0 && (GET_LEVEL(vict)<objlevel(obj))) ) )
	{
		act("$N e' troppo inesperto perche' possa prendere $p, rinunci.", FALSE, ch, obj, vict, TO_CHAR);
		return;
	}
	
	if (!IS_NPC(vict) && (GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && (!can_take_container(vict,obj)))
	{
		act("All'interno di $p si trova qualcosa di troppo potente perche' $N possa maneggiarlo.", FALSE, ch, obj, vict, TO_CHAR);
		return;
	}
	
	if (!give_otrigger(obj, ch, vict) || !receive_mtrigger(vict, ch, obj))
		return;
	
	if (MOB_FLAGGED(vict, MOB_SHOPKEEPER))
	{
		if (world[vict->in_room].number==-1) return;
		shop = get_shopindexfromroom(world[vict->in_room].number);
		
		if (shop==-1)
		{
			act("Non puoi dargli oggetti se non si trova nel negozio.", FALSE, ch, obj, vict, TO_CHAR);
			return;
		}
		
		if (!((result = trade_with(obj, shop)) == 2))  //  2= OBJECT_OK
		{
			act("$N non tratta oggetti come $p.", FALSE, ch, obj, vict, TO_CHAR);
			return;
		}
	}
	
	obj_from_char(obj);
	obj_to_char(obj, vict);
	
	if (MOB_FLAGGED(vict,MOB_MOUNTABLE))
	{
		if (GET_OBJ_TYPE(obj)==ITEM_MOUNT_FOOD)
		{
			if (GET_COND(vict, FULL) < 96) {/*Non e' Pieno*/
				do_eat(vict, obj->name, 0, SCMD_EAT);
				GET_MOVE(vict)= MIN(GET_MOVE(vict) + (GET_MAX_MOVE(vict) * 0.1), GET_MAX_MOVE(vict));				
				return;
			} else {
				act("$N e' sazio e non vuole $p.", FALSE, ch, obj, vict, TO_CHAR);
				obj_from_char(obj);
				obj_to_char(obj, ch);
				return;
			}
		} 
	}
	
	act("Dai $p a $N.", FALSE, ch, obj, vict, TO_CHAR);
	act("$n ti da $p.", FALSE, ch, obj, vict, TO_VICT);
	act("$n da' $p a $N.", TRUE, ch, obj, vict, TO_NOTVICT);
}

/* utility function for give */
struct char_data *give_find_vict(struct char_data * ch, char *arg)
{
	struct char_data *vict;
	
	if (!*arg) {
		send_to_char("A chi?\r\n", ch);
		return NULL;
	} else if (!(vict = get_char_room_vis(ch, arg))) {
		send_to_char(NOPERSON, ch);
		return NULL;
	} else if (vict == ch) {
		send_to_char("What's the point of that?\r\n", ch);
		return NULL;
	} else
		return vict;
}


void perform_give_gold(struct char_data * ch, struct char_data * vict,
	int amount)
{
	if (amount <= 0) {
		send_to_char("Heh heh heh ... hai intenzione di scherzare oggi, eh?\r\n", ch);
		return;
	}
	if ((GET_GOLD(ch) < amount) && (IS_NPC(ch) || (GET_LEVEL(ch) < LVL_GOD))) {
		send_to_char("Non hai cosi' tanti coins!\r\n", ch);
		return;
	}
	send_to_char(OK, ch);
	sprintf(buf, "$n ti da' %d coins.", amount);
	act(buf, FALSE, ch, 0, vict, TO_VICT);
	sprintf(buf, "$n da' %s a $N.", money_desc(amount));
	act(buf, TRUE, ch, 0, vict, TO_NOTVICT);
	if((MASTER_ID(vict)) &&(GET_STIPENDIOMOB(vict))>0){
		GET_GOLD(ch) -= amount;
		GET_BANK_GOLD(vict) += amount;
		
		if (GET_PAGA(vict)<0) 
			if (GET_BANK_GOLD(vict) >= get_stipendiomob(vict)) {
				GET_BANK_GOLD(vict) -= get_stipendiomob(vict);
				GET_PAGA(vict) += ORE_PAGA;
			}
			
		SET_BIT(MOB_FLAGS(vict),MOB_CRASH);
	} else {
		if (IS_NPC(ch) || (GET_LEVEL(ch) < LVL_GOD))
			GET_GOLD(ch) -= amount;
		GET_GOLD(vict) += amount;
	}
	bribe_mtrigger(vict, ch, amount);
}

ACMD(do_give)
{
	int amount, dotmode;
	struct char_data *vict,*vict1;
	struct obj_data *obj, *next_obj;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int give_num;    /* Added by Darksword -- to give multiple items */
	
	
  /**********************Enrico AFF_IMMOBIL Block coomands*/
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
 /********************************************************/
	if(subcmd==SCMD_STOW)	{
		argument=two_arguments(argument,arg,arg1);
		if(!*arg){
			send_to_char("chi vorresti caricare, e soprattutto su chi?\r\n",ch);
			return;
		}
		if(!(vict=give_find_vict(ch,arg))){
			send_to_char("Non e' qui!",ch);
			return;
		}
		if (GET_POS(vict) > POS_DEAD)
			if (!AFF_FLAGGED(vict,AFF_IMMOBIL)&&!AFF_FLAGGED(vict,AFF_PARALIZE)&&!AFF_FLAGGED(vict,AFF_SLEEP)&&!AFF_FLAGGED(vict,AFF_TRAMORTITO)){
				send_to_char("Non ha alcuna voglia di essere caricata sulle tue spalle!\n",ch);
				return;
			}
			
		if(!*arg1){
			if(TRANSPORTED(ch)){
				send_to_char("Hai gia Qualcuno sulle spalle!\r\n",ch);
				return;
			}
			if(TRANSPORTED_BY(vict)){
				send_to_char("Sta gia in spalla a qualcuno!\r\n",ch);
				return;
			}   
			stow_char(vict,ch);
			sprintf(buf,"Ti carichi %s sulle spalle\n",GET_NAME(vict));
			send_to_char(buf,ch);
			sprintf(buf,"%s ti carica sulle spalle.Buon viaggio!\n",GET_NAME(ch));
			send_to_char(buf,vict);
			act("$n si carica sulle spalle $N\n",FALSE,ch,0,vict,TO_NOTVICT);
		}
		else{       	 
			if(!(vict1=give_find_vict(ch,arg1))){
				send_to_char("Non e' qui!",ch);
				return;
			}
			
			if (!IS_NPC(vict1) || (!vict1->master) || (vict1->master!=ch)) { 
				send_to_char("Non ti ascolta.",ch);
				return;
			}
			
			if(TRANSPORTED(vict1)){
				send_to_char("Ha gia qualcuno caricato sulle spalle.\r\n",ch);
				return;
			}
			if(TRANSPORTED_BY(vict1)||GET_POS(vict1)<POS_STANDING){
				send_to_char("Non e' in grado di Trasportare nessuno ora!\r\n",ch);
				return;
			}
			sprintf(buf,"carichi %s sulle spalle di %s\n",GET_NAME(vict),GET_NAME(vict1));
			send_to_char(buf,ch);
			sprintf(buf,"%s ti carica sulle spalle di %s.Buon viaggio!\n",GET_NAME(ch),GET_NAME(vict1));
			send_to_char(buf,vict);
			sprintf(buf,"%s ti carica  %s sulle spalle!\n",GET_NAME(ch),GET_NAME(vict));
			send_to_char(buf,vict);
			sprintf(buf,"$n carica $N sulle spalle di %s.",GET_NAME(vict1));
			act(buf,FALSE,ch,0,vict,TO_NOTVICT);
			stow_char(vict,vict1);
		}
		return;
	}
	
	argument = one_argument(argument, arg);
	if (!*arg)
		send_to_char("Dare cosa e a chi?\r\n", ch);
	else if (is_number(arg))
	{
		amount = atoi(arg);
		argument = one_argument(argument, arg);
		if (!str_cmp("coins", arg) || !str_cmp("coin", arg))
		{
			argument = one_argument(argument, arg);
			if ((vict = give_find_vict(ch, arg)))
			{
				if ((subcmd==SCMD_LOAD) && (!(MOB_FLAGGED(vict, MOB_MOUNTABLE))))
				{
					send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
					return;
				}
				perform_give_gold(ch, vict, amount);
				//safe_save_pg(ch,vict); 		 
			}
			return;
		}
		else
		{
        /* Code to give multiple items added by Darksword */
        /* added variables  int  give_num                 */
			give_num = amount;
			argument = two_arguments(argument, arg1, arg2);
			if ((vict = give_find_vict(ch, arg1)))
			{
				if ((subcmd==SCMD_LOAD) && (!(MOB_FLAGGED(vict, MOB_MOUNTABLE))))
				{
					send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
					return;
				}
				for  (amount = 0; amount < give_num; amount++)
				{
					if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying)))
					{
						sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
						send_to_char(buf, ch);
						amount = give_num; /* just let them know no more then stop */
					}
					else 
						perform_give(ch, vict, obj);			
					
				}//for
				//safe_save_pg(ch,vict); 	
			}
        /* End added by Darksword */
			
			return;
		}
	}
	else
	{
		one_argument(argument, buf1);
		if (!(vict = give_find_vict(ch, buf1)))
			return;
		if ((subcmd==SCMD_LOAD) && (!(MOB_FLAGGED(vict, MOB_MOUNTABLE))))
		{
			send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
			return;
		}
		dotmode = find_all_dots(arg);
		//lance
		if (!str_cmp("coins", arg) || !str_cmp("coin", arg))
		{
			if (dotmode == FIND_ALLDOT) {
				if ((subcmd==SCMD_LOAD) && (!(MOB_FLAGGED(vict, MOB_MOUNTABLE))))
				{
					send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
					return;
				}
				amount = GET_GOLD(ch);
				perform_give_gold(ch, vict, amount);
				//safe_save_pg(ch,vict); 		 
				return;
			}
		}
		//lance
		if (dotmode == FIND_INDIV) {
			if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
				sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
				send_to_char(buf, ch);
			}
			else {
				perform_give(ch, vict, obj);
				//safe_save_pg(ch,vict); 		 
			}//else
		}
		else
		{
			if (dotmode == FIND_ALLDOT && !*arg) {
				send_to_char("Tutto cosa?\r\n", ch);
				return;
			}
			if (!ch->carrying)
				send_to_char("Non sembra che tu stia portando qualcosa.\r\n", ch);
			else {
				for (obj = ch->carrying; obj; obj = next_obj) {
					next_obj = obj->next_content;
					if (CAN_SEE_OBJ_DARK(ch, obj) &&
						(((dotmode == FIND_ALL) || isname(arg, obj->name))))
						perform_give(ch, vict, obj);
				}
				//safe_save_pg(ch,vict);
			}//else
		}
	}
}

void weight_change_object(struct obj_data * obj, int weight)
{
	struct obj_data *tmp_obj;
	struct char_data *tmp_ch;
	
	if (obj->in_room != NOWHERE) {
		GET_OBJ_WEIGHT(obj) += weight;
	} else if ((tmp_ch = obj->carried_by)) {
		obj_from_char(obj);
		GET_OBJ_WEIGHT(obj) += weight;
		obj_to_char(obj, tmp_ch);
	} else if ((tmp_obj = obj->in_obj)) {
		obj_from_obj(obj);
		GET_OBJ_WEIGHT(obj) += weight;
		obj_to_obj(obj, tmp_obj);
	} else {
		log("SYSERR: Unknown attempt to subtract weight from an object.");
	}
}



void name_from_drinkcon(struct obj_data * obj)
{
	int i;
	char *new_name;
	extern struct obj_data *obj_proto;
	
	for (i = 0; (*((obj->name) + i) != ' ') && (*((obj->name) + i) != '\0'); i++);
	
	if (*((obj->name) + i) == ' ') {
		new_name = str_dup((obj->name) + i + 1);
		if (GET_OBJ_RNUM(obj) < 0 || obj->name != obj_proto[GET_OBJ_RNUM(obj)].name)
			free(obj->name);
		obj->name = new_name;
	}
}



void name_to_drinkcon(struct obj_data * obj, int type)
{
	char *new_name;
	extern struct obj_data *obj_proto;
	extern char *drinknames[];
	
	CREATE(new_name, char, strlen(obj->name) + strlen(drinknames[type]) + 2);
	sprintf(new_name, "%s %s", drinknames[type], obj->name);
	if (GET_OBJ_RNUM(obj) < 0 || obj->name != obj_proto[GET_OBJ_RNUM(obj)].name)
		free(obj->name);
	obj->name = new_name;
}



ACMD(do_drink)
{
	struct obj_data *temp;
	struct affected_type af;
	int amount, weight;
	int on_ground = 0;
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg) {
		send_to_char("Bevi da cosa?\r\n", ch);
		return;
	}
	if (!(temp = get_obj_in_list_vis(ch, arg, ch->carrying))) {
		if (!(temp = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
			act("Non lo trovi!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		} else
			on_ground = 1;
	}
	if ((GET_OBJ_TYPE(temp) != ITEM_DRINKCON) &&
		(GET_OBJ_TYPE(temp) != ITEM_FOUNTAIN)) {
		send_to_char("Non puoi bere da quello!\r\n", ch);
		return;
	}
	if (on_ground && (GET_OBJ_TYPE(temp) == ITEM_DRINKCON)) {
		send_to_char("Devi prima tenerlo quello se vuoi bere.\r\n", ch);
		return;
	}
	if ((GET_COND(ch, DRUNK) > 12) && (GET_COND(ch, THIRST) > 0)) {
    /* The pig is drunk */
		send_to_char("Non sei abbastanza vicino con la bocca a quello per berci.\r\n", ch);
		act("$n tenta di bere...ma sbaglia bocca!", TRUE, ch, 0, 0, TO_ROOM);
		return;
	}
	if ((GET_COND(ch, FULL) > 96) && (GET_COND(ch, THIRST) > 96)) {
		send_to_char("Il tuo stomaco non puo' contenere altro!\r\n", ch);
		return;
	}
	if (!GET_OBJ_VAL(temp, 1)) {
		send_to_char("!!SOUND(Empty.wav)E' vuoto.\r\n", ch);
		return;
	}
	if (subcmd == SCMD_DRINK) {
		sprintf(buf, "$n beve %s da $p.", drinks[GET_OBJ_VAL(temp, 2)]);
		act(buf, TRUE, ch, temp, 0, TO_ROOM);
		
		sprintf(buf, "Bevi %s.\r\n", drinks[GET_OBJ_VAL(temp, 2)]);
		send_to_char(buf, ch);
		
		if (drink_aff[GET_OBJ_VAL(temp, 2)][DRUNK] > 0)
			amount = (25 - GET_COND(ch, THIRST)) / drink_aff[GET_OBJ_VAL(temp, 2)][DRUNK];
		else
			amount = number(3, 10);
		
	} else {
		act("$n assaggia  $p.", TRUE, ch, temp, 0, TO_ROOM);
		sprintf(buf, "Sa' da %s.\r\n", drinks[GET_OBJ_VAL(temp, 2)]);
		send_to_char(buf, ch);
		amount = 1;
	}
	
	amount = MIN(amount, GET_OBJ_VAL(temp, 1));
	
  /* You can't subtract more than the object weighs */
	weight = MIN(amount, GET_OBJ_WEIGHT(temp));
	
	weight_change_object(temp, -weight);	/* Subtract amount */
	
	gain_condition(ch, DRUNK,
		(int) ((int) drink_aff[GET_OBJ_VAL(temp, 2)][DRUNK] * amount) / 4);
	
	gain_condition(ch, FULL,
		(int) ((int) drink_aff[GET_OBJ_VAL(temp, 2)][FULL] * amount) / 4);
	
	gain_condition(ch, THIRST,
		(int) ((int) drink_aff[GET_OBJ_VAL(temp, 2)][THIRST] * amount) / 4);

	if (GET_OBJ_VAL(temp, 2) == LIQ_CAFFELIMONE){
		send_to_char ("Trangugitata l'abominevole bevanda non riesci piu' a controllare gli spasmi del tuo stomaco...\r\n", ch);
		act ("&1$n barcollando raggiunge un muro e vomita orribilmente!&0", FALSE, ch, 0, 0, TO_ROOM);
		act ("&1Arrivi al primo muro che trovi e vomiti abbondantemente!&0", FALSE, ch, 0, 0, TO_CHAR);
		//GET_COND(ch, DRUNK) = 0;
		if (affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO)){
			send_to_char ("&6Senti che, nonostante il dolore, l'odioso liquido ti ha giovato...&0\r\n", ch);
			affect_from_char (ch, SPELLSKILL, DISEASE_BACCO);
			affect_total (ch);
		}
	}
	
	if (GET_COND(ch, DRUNK) > 10)
		send_to_char("Sei ubriaco.\r\n", ch);
	
	if (GET_COND(ch, THIRST) > 78)
		send_to_char("Non sei piu' assetato.\r\n", ch);
	
	if (GET_COND(ch, FULL) >= 96)
		send_to_char("Sei pieno.\r\n", ch);
	
	if (GET_OBJ_VAL(temp, 3)) {	/* The shit was poisoned ! */
		send_to_char("Oops, ha un gusto strano!\r\n", ch);
		act("$n sputa ed emette suoni strani.", TRUE, ch, 0, 0, TO_ROOM);
		
		af.natura = 0;
		af.type = SPELL_POISON;
		af.duration = amount * 5;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector[0] = AFF_POISON;
		af.bitvector[1] = 0;
		af.bitvector[2] = 0;
		af.bitvector[3] = 0;
		affect_join(ch, &af, FALSE, FALSE, FALSE, FALSE);
	}
  /* empty the container, and no longer poison. */
	GET_OBJ_VAL(temp, 1) -= amount;
	if (!GET_OBJ_VAL(temp, 1)) {	/* The last bit */
		GET_OBJ_VAL(temp, 2) = 0;
		GET_OBJ_VAL(temp, 3) = 0;
		name_from_drinkcon(temp);
	}
	return;
}



ACMD(do_eat)
{
	struct obj_data *food;
	struct affected_type af;
	int amount;
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg) {
		send_to_char("Mangiare cosa?\r\n", ch);
		return;
	}
	if (!(food = get_obj_in_list_vis(ch, arg, ch->carrying))) {
		sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
		send_to_char(buf, ch);
		return;
	}
	if (subcmd == SCMD_TASTE && ((GET_OBJ_TYPE(food) == ITEM_DRINKCON) ||
			(GET_OBJ_TYPE(food) == ITEM_FOUNTAIN))) {
		do_drink(ch, argument, 0, SCMD_SIP);
		return;
	}
	if (((GET_OBJ_TYPE(food) != ITEM_FOOD)&&(GET_OBJ_TYPE(food) != ITEM_MOUNT_FOOD)) && (GET_LEVEL(ch) < LVL_GOD)) {
		send_to_char("Non puoi mangiare quello!\r\n", ch);
		return;
	}
	if ((GET_OBJ_TYPE(food) == ITEM_MOUNT_FOOD) && (!IS_NPC(ch))) {
		send_to_char("Non sei un cavallo, non puoi mangiarlo!\r\n", ch);
		return;
	}
	if (GET_COND(ch, FULL) >= 96) {/* Stomach full */
		act("Sei troppo pieno per mangiare!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	if (subcmd == SCMD_EAT) {
		act("Mangi $p.", FALSE, ch, food, 0, TO_CHAR);
		act("$n mangia $p.", TRUE, ch, food, 0, TO_ROOM);
	} else {
		act("Dai un morso a $p.", FALSE, ch, food, 0, TO_CHAR);
		act("$n da' un morso a $p.", TRUE, ch, food, 0, TO_ROOM);
	}
	
	amount = (subcmd == SCMD_EAT ? GET_OBJ_VAL(food, 0) : 1);
	
	gain_condition(ch, FULL, amount);
	
	if (GET_COND(ch, FULL) >= 96)
		act("Sei pieno.", FALSE, ch, 0, 0, TO_CHAR);
	
	if (GET_OBJ_VAL(food, 3) && (GET_LEVEL(ch) < LVL_IMMORT)) {
    /* The shit was poisoned ! */
		send_to_char("Oops, quello ha un sapore strano!\r\n", ch);
		act("$n tossisce ed emette dei rumori strani.", FALSE, ch, 0, 0, TO_ROOM);
		
		af.natura = 0;
		af.type = SPELL_POISON;
		af.duration = amount * 4;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector[0] = AFF_POISON;
		af.bitvector[1] = 0;
		af.bitvector[2] = 0;
		af.bitvector[3] = 0;
		affect_join(ch, &af, FALSE, FALSE, FALSE, FALSE);
	}
	if (subcmd == SCMD_EAT)
		extract_obj(food);
	else {
		GET_OBJ_VAL(food, 0)--;
		GET_OBJ_COST(food) = (GET_OBJ_COST(food) / (GET_OBJ_VAL(food, 0)+1)) * GET_OBJ_VAL(food, 0);
		if (!(GET_OBJ_VAL(food, 0))) {
			send_to_char("Non ne rimane molto.\r\n", ch);
			extract_obj(food);
		}
	}
}


ACMD(do_pour)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct obj_data *from_obj = NULL, *to_obj = NULL;
	int amount;
	
	two_arguments(argument, arg1, arg2);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (subcmd == SCMD_POUR) {
		if (!*arg1) {		/* No arguments */
			act("Cosa vuoi svuotare?", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		if (!(from_obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
			act("Non lo trovi!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		if (GET_OBJ_TYPE(from_obj) != ITEM_DRINKCON) {
			act("Non puoi svuotare quello!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}
	if (subcmd == SCMD_FILL) {
		if (!*arg1) {		/* no arguments */
			send_to_char("Cosa vuoi riempire?  e da dove?\r\n", ch);
			return;
		}
		if (!(to_obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
			send_to_char("Non lo trovi!", ch);
			return;
		}
		if (GET_OBJ_TYPE(to_obj) != ITEM_DRINKCON) {
			act("Non puoi riempire $p!", FALSE, ch, to_obj, 0, TO_CHAR);
			return;
		}
		if (!*arg2) {		/* no 2nd argument */
			act("Da cosa vuoi riempire $p?", FALSE, ch, to_obj, 0, TO_CHAR);
			return;
		}
		if (!(from_obj = get_obj_in_list_vis(ch, arg2, world[ch->in_room].contents))) {
			sprintf(buf, "Non sembra esserci %s qui.\r\n", arg2);
			send_to_char(buf, ch);
			return;
		}
		if (GET_OBJ_TYPE(from_obj) != ITEM_FOUNTAIN) {
			act("Non puoi riempire niente da $p.", FALSE, ch, from_obj, 0, TO_CHAR);
			return;
		}
	}
	if (GET_OBJ_VAL(from_obj, 1) == 0) {
		act("!!SOUND(Empty.wav)$p e' vuoto.", FALSE, ch, from_obj, 0, TO_CHAR);
		return;
	}
	if (subcmd == SCMD_POUR) {	/* pour */
		if (!*arg2) {
			act("Dentro o fuori?", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		if ((is_abbrev(arg2, "out")) || (is_abbrev(arg2,"fuori"))) {
			act("!!SOUND(bolle.wav)$n svuota $p.", TRUE, ch, from_obj, 0, TO_ROOM);
			act("!!SOUND(bolle.wav)Svuoti $p.", FALSE, ch, from_obj, 0, TO_CHAR);
			
			weight_change_object(from_obj, -GET_OBJ_VAL(from_obj, 1)); /* Empty */
			
			GET_OBJ_VAL(from_obj, 1) = 0;
			GET_OBJ_VAL(from_obj, 2) = 0;
			GET_OBJ_VAL(from_obj, 3) = 0;
			name_from_drinkcon(from_obj);
			
			return;
		}
		if (!(to_obj = get_obj_in_list_vis(ch, arg2, ch->carrying))) {
			act("Non lo trovi!", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		if ((GET_OBJ_TYPE(to_obj) != ITEM_DRINKCON) &&
			(GET_OBJ_TYPE(to_obj) != ITEM_FOUNTAIN)) {
			act("Non puoi metterci dentro niente.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}
	if (to_obj == from_obj) {
		act("Non e' molto utile.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	if ((GET_OBJ_VAL(to_obj, 1) != 0) &&
		(GET_OBJ_VAL(to_obj, 2) != GET_OBJ_VAL(from_obj, 2))) {
		act("C'e' un'altro liquido dentro!", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	if (!(GET_OBJ_VAL(to_obj, 1) < GET_OBJ_VAL(to_obj, 0))) {
		act("E' gia pieno.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	if (subcmd == SCMD_POUR) {
		sprintf(buf, "!!SOUND(bolle.wav)Versi %s da %s in %s.",
			drinks[GET_OBJ_VAL(from_obj, 2)], GET_OBJ_NAME(from_obj), GET_OBJ_NAME(to_obj));
		send_to_char(buf, ch);
		sprintf(buf,"$n versa %s da $P in $p",drinks[GET_OBJ_VAL(from_obj,2)]); 
		act(buf, FALSE, ch, to_obj, from_obj, TO_ROOM);
	}
	if (subcmd == SCMD_FILL) {
		act("Riempi $p da $P.", FALSE, ch, to_obj, from_obj, TO_CHAR);
		act("$n riempe $p da $P.", TRUE, ch, to_obj, from_obj, TO_ROOM);
	}
  /* New alias */
	if (GET_OBJ_VAL(to_obj, 1) == 0)
		name_to_drinkcon(to_obj, GET_OBJ_VAL(from_obj, 2));
	
  /* First same type liq. */
	GET_OBJ_VAL(to_obj, 2) = GET_OBJ_VAL(from_obj, 2);
	
  /* Then how much to pour */
	GET_OBJ_VAL(from_obj, 1) -= (amount =
				(GET_OBJ_VAL(to_obj, 0) - GET_OBJ_VAL(to_obj, 1)));
	
	GET_OBJ_VAL(to_obj, 1) = GET_OBJ_VAL(to_obj, 0);
	
	if (GET_OBJ_VAL(from_obj, 1) < 0) {	/* There was too little */
		GET_OBJ_VAL(to_obj, 1) += GET_OBJ_VAL(from_obj, 1);
		amount += GET_OBJ_VAL(from_obj, 1);
		GET_OBJ_VAL(from_obj, 1) = 0;
		GET_OBJ_VAL(from_obj, 2) = 0;
		GET_OBJ_VAL(from_obj, 3) = 0;
		name_from_drinkcon(from_obj);
	}
  /* Then the poison boogie */
	GET_OBJ_VAL(to_obj, 3) =
		(GET_OBJ_VAL(to_obj, 3) || GET_OBJ_VAL(from_obj, 3));
	
  /* And the weight boogie */
	weight_change_object(from_obj, -amount);
	weight_change_object(to_obj, amount);	/* Add weight */
	
	return;
}



void wear_message(struct char_data * ch, struct obj_data * obj, int where)
{
	char *wear_messages[][2] = {
		{"$n accende $p.",
			"Accendi $p."},
		
		{"$n infila $p al dito destro.",
			"Infili $p sul tuo dito destro."},
		
		{"$n infila $p al dito sinistro.",
			"Infili $p sul tuo dito sinistro."},
		
		{"$n indossa $p intorno al collo.",
			"Indossi $p intorno al collo."},
		
		{"$n indossa $p intorno al collo.",
			"Indossi $p intorno al collo."},
		
		{"$n indossa $p attorno al corpo.",
			"Indossi $p attorno al corpo.",},
		
		{"$n indossa $p sulla testa.",
			"Indossi $p sulla testa."},
		
		{"$n mette $p sulle gambe.",
			"Metti $p sulle gambe."},
		
		{"$n infila $p ai piedi.",
			"Infili $p ai piedi."},
		
		{"$n mette $p sulle mani.",
			"Metti $p sulle mani."},
		
		{"$n indossa $p sulle braccia.",
			"Indossi $p sulle tue braccia."},
		
		{"$n usa $p come scudo.",
			"Usi $p come scudo."},
		
		{"$n indossa $p intorno al corpo.",
			"Indossi $p intorno al corpo."},
		
		{"$n indossa $p intorno alla vita.",
			"Indossi $p intorno alla vita."},
		
		{"$n Mette $p al polso destro.",
			"Metti $p al tuo polso destro."},
		
		{"$n mette $p al polso sinistro.",
			"Metti $p al tuo polso sinistro."},
		
		{"$n impugna $p con la destra.",
			"Impugni con la destra $p."},
		
		{"$n afferra $p.",
			"Afferri $p."},
		
		{"$n mette $p all'orecchio.",
			"Metti $p all'orecchio."},
		
		{"$n mette $p nell'altro orecchio.",
			"Metti $p nell'altro orecchio."},
		
		{"$n mette $p sulle spalle.",
			"Metti $p sulle spalle."},
		
		{"$n mette $p ai polsi .",
			"Metti $p ai polsi ."},
		
		{"$n mette $p sugli occhi .",
			"Metti $p sugli occhi ."},
		
		{"$n mette $p alla bocca .",
			"Metti $p alla bocca ."},
		
		{"$n mette $p non si sa dove .... ",
			"Metti $p non si sa dove...."},
		
		{"$n impugna $p con la sinistra.",
			"Impugni con la sinistra $p."},
		
		{"$n inizia a trainare $p.",
			"inizi a trainare $p."},
		
		{"$n si veste con $p.",         /*Jerold*/
			"ti vesti con $p."},     /*Jerold*/
		
		{"Qualcosa sembra muoversi sul corpo di $n, ora noti qualcosa di diverso nel suo aspetto.",
			"$p si attacca al tuo corpo anche contro la tua volonta'."}	
	};
	act(wear_messages[where][0], TRUE, ch, obj, 0, TO_ROOM);
	act(wear_messages[where][1], FALSE, ch, obj, 0, TO_CHAR);
}



void perform_wear(struct char_data * ch, struct obj_data * obj, int where)
{
	
  /*
   * ITEM_WEAR_TAKE is used for objects that do not require special bits
   * to be put into that position (e.g. you can hold any object, not just
   * an object with a HOLD bit.)
   */
	
	int wear_bitvectors[] = {
		ITEM_WEAR_TAKE,   ITEM_WEAR_FINGER, ITEM_WEAR_FINGER, ITEM_WEAR_NECK,
		ITEM_WEAR_NECK,   ITEM_WEAR_BODY,   ITEM_WEAR_HEAD,   ITEM_WEAR_LEGS,
		ITEM_WEAR_FEET,   ITEM_WEAR_HANDS,  ITEM_WEAR_ARMS,   ITEM_WEAR_SHIELD,
		ITEM_WEAR_ABOUT,  ITEM_WEAR_WAIST,  ITEM_WEAR_WRIST,  ITEM_WEAR_WRIST,
		ITEM_WEAR_WIELD,  ITEM_WEAR_TAKE,   ITEM_WEAR_LOBSX,  ITEM_WEAR_LOBSX,
		ITEM_WEAR_SPALLE, ITEM_WEAR_IMMOBIL,ITEM_WEAR_EYE,    ITEM_WEAR_BOCCA,
		ITEM_WEAR_ALTRO1, ITEM_WEAR_WIELD,	 ITEM_WEAR_HANG,   ITEM_WEAR_VESTE,
		ITEM_WEAR_RELIQUIA};
	
	
	char *already_wearing[] = {
		"Stai gia' portando una luce.\r\n",
		"YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
		"Hai gia' qualcosa nelle dita.\r\n",
		"YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
		"Non puoi mettere niente altro al collo.\r\n",
		"Non puoi mettere niente altro sul corpo.\r\n",
		"Non puoi mettere niente altro sulla testa.\r\n",
		"Non puoi  mettere niente altro sulle gambe.\r\n",
		"Stai gia' indossando qualche cosa ai piedi.\r\n",
		"Hai gia' qualche cosa sulle mani.\r\n",
		"Stai gia' indossando qualche cosa sulle braccia.\r\n",
		"Stai gia' usando uno scudo.\r\n",
		"Stai gia' vestendo qualcosa attorno al corpo.\r\n",
		"Hai gia' qualche cosa alla cinta.\r\n",
		"YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
		"Hai gia' qualche cosa attorno ai polsi.\r\n",
		"Stai gia impugnando un arma con la mano destra\r\n",
		"Stai gia' portando qualche cosa.\r\n",
		"Stai gia' usando qualcosa nel tuo orecchio.\r\n",
		"Stai gia' usando qualche cosa nell'altro orecchio.\r\n",
		"Hai gia' qualche cosa sulle spalle.\r\n",
		"Sei gia stato incatenato...\r\n",
		"Hai gia qualcosa sugli occhi..\r\n",
		"Hai gia qualcosa sulla bocca..\r\n",
		"Hai gia' tutto...quello e' in piu'...\r\n",
		"Stai gia impugnando un arma con la mano sinistra\r\n",
		"Stai gia trasportando qualcosa\r\n",
		"Stai gia' vestendo qualcosa. \r\n",
		"la potenza di piu reliquie potrebbe dannegiarti irrimediabilmente.\r\n"		
	};
	
	if (where == WEAR_HOLD || where == WEAR_WIELD || where == WEAR_LIGHT || where == WEAR_SHIELD || where == WEAR_HANG || where == WEAR_WIELD_L)
		if (affected_by_spell (ch, SPELLSKILL, DISEASE_PIAGHE)){
			act ("Le tue mani sono troppo orribilmente piagate per poter afferrare qualsiasi cosa!\r\n", FALSE, ch, 0, 0, TO_CHAR);
			if (where==WEAR_HANG) {
				obj_from_char(obj);
				obj_to_room(obj,ch->in_room);	
			}
			return;
		}
	if ((GET_LEVEL(ch)<GET_OBJ_LEVEL(obj)) || ((GET_OBJ_LEVEL(obj)<=0) && (GET_LEVEL(ch)<objlevel(obj))))
	{
		act("Sei troppo inesperto per poter usere $p, rinunci.", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	if ((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && (!can_take_container(ch,obj)))
	{
		act("All'interno di $p si trova qualcosa di troppo potente per poter essere usato da te.", FALSE, ch, obj, 0, TO_CHAR);
		if (where==WEAR_HANG) {
			obj_from_char(obj);
			obj_to_room(obj,ch->in_room);	
		}
		return;
	}
	
   /* first, make sure that the wear position is valid. */
	if (!CAN_WEAR(obj, wear_bitvectors[where])) {
		act("Non puoi mettere $p la'.", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
  /* for neck, finger, and wrist, try pos 2 if pos 1 is already full */
	if ((where == WEAR_FINGER_R) || (where == WEAR_NECK_1) || (where == WEAR_WRIST_R) || (where == WEAR_LOBSX))
		if (GET_EQ(ch, where))
			where++;
	if(where==WEAR_WIELD)
		if(GET_EQ(ch,where) || AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3))
			where=WEAR_WIELD_L;
	if (GET_EQ(ch, where)) {
		send_to_char(already_wearing[where], ch);
		return;
	}
	
	if(!check_free_hands(ch,obj))
		return;
	if (!wear_otrigger(obj, ch, where))
		return;
	if(where==WEAR_RELIQUIA){
		if(number(1,100)>20){
			wear_message(ch, obj, where);
		}
		obj_from_char(obj);
		equip_char(ch,obj,where);
		return;
	}
	if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3) && where==WEAR_WIELD){
	    act("Non puoi impugnare $p il tuo braccio e' troppo malridotto.", FALSE, ch, obj, 0, TO_CHAR);
	    return;
	    }	    
	if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3) && where==WEAR_WIELD_L){
	    act("Non puoi impugnare $p il tuo braccio e' troppo malridotto.", FALSE, ch, obj, 0, TO_CHAR);
	    return;
	}	        

	/* Orione Inizio */
	if (GET_OBJ_TYPE(obj)==ITEM_WEAPON_2HANDS) {
	   if ((where==WEAR_WIELD) && ((GET_EQ(ch,WEAR_WIELD)!=NULL) || (GET_EQ(ch,WEAR_HOLD)!=NULL))) {
	      act("Non puoi impugnare armi a 2 mani se hai gia' qualcosa in mano.", FALSE, ch, obj, 0, TO_CHAR);
	      return;
	   }
	   if ((where==WEAR_HOLD) || (where==WEAR_WIELD_L)) {
	      act("Non puoi impugnare armi a 2 mani se hai gia' qualcosa in mano.", FALSE, ch, obj, 0, TO_CHAR);
	      return;
	   }
	}
	if ((where==WEAR_HOLD) || (where==WEAR_WIELD_L)) {
	   if ((GET_EQ(ch, WEAR_WIELD)!=NULL) && (GET_OBJ_TYPE(GET_EQ(ch, WEAR_WIELD))==ITEM_WEAPON_2HANDS)) {
	      act("Non puoi tenere in mano altro, se stai gia' impugnando un'arma a 2 mani.", FALSE, ch, obj, 0, TO_CHAR);
	      return;
	   }
	}
	/* Orione Fine */

	if (GET_OBJ_TYPE(obj) == ITEM_POLVERE && GET_OBJ_MAT_TYPE(obj) == GEMME && GET_OBJ_MAT_NUM(obj) == 0 && affected_by_spell (ch, SPELLSKILL, DISEASE_XHYPHYS)){
		act ("&6&bCospargi la tua pelle della polvere di diamanti, imponendo l'ordine della gemma sopra il Caos sovrano.&0", FALSE, ch, 0, 0, TO_CHAR);
		act ("&1Per questa volta il Caos ti abbandona, ma e' pronto a colpirti in qualsiasi momento...&0", FALSE, ch, 0, 0, TO_CHAR);
		act ("&6&bImponendo l'ordine del diamante sopra al Caos, $n si libera della maledizione&0.", FALSE, ch, 0, 0, TO_ROOM);
		obj_from_char(obj);
		affect_from_char (ch, SPELLSKILL, DISEASE_XHYPHYS);
		return;
	}

	wear_message(ch,obj, where);
	obj_from_char(obj);
//  check_abil_obj(ch,obj,TRUE);
	equip_char(ch, obj, where);
}



int find_eq_pos(struct char_data * ch, struct obj_data * obj, char *arg)
{
	int where = -1;
	
	const char *keywords[] = {
		"!RESERVED!",
		"dito",
		"!RESERVED!",
		"collo",
		"!RESERVED!",
		"corpo",
		"testa",
		"gambe",
		"piedi",
		"mani",
		"braccia",
		"scudo",
		"intorno",
		"cinta",
		"polsi",
		"!RESERVED!",
		"!RESERVED!",
		"!RESERVED!",
		"orecchio",
		"orecchio",
		"spalle",
		"polsi",
		"occhi",
		"bocca",
		"boh",
		"!RESERVED!",
		"reliquia",
		"veste",
		"\n"
	};
	
	if (!arg || !*arg) {
		if (CAN_WEAR(obj, ITEM_WEAR_FINGER))      where = WEAR_FINGER_R;
		if (CAN_WEAR(obj, ITEM_WEAR_NECK))        where = WEAR_NECK_1;
		if (CAN_WEAR(obj, ITEM_WEAR_BODY))        where = WEAR_BODY;
		if (CAN_WEAR(obj, ITEM_WEAR_HEAD))        where = WEAR_HEAD;
		if (CAN_WEAR(obj, ITEM_WEAR_LEGS))        where = WEAR_LEGS;
		if (CAN_WEAR(obj, ITEM_WEAR_FEET))        where = WEAR_FEET;
		if (CAN_WEAR(obj, ITEM_WEAR_HANDS))       where = WEAR_HANDS;
		if (CAN_WEAR(obj, ITEM_WEAR_ARMS))        where = WEAR_ARMS;
		if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))      where = WEAR_SHIELD;
		if (CAN_WEAR(obj, ITEM_WEAR_ABOUT))       where = WEAR_ABOUT;
		if (CAN_WEAR(obj, ITEM_WEAR_WAIST))       where = WEAR_WAIST;
		if (CAN_WEAR(obj, ITEM_WEAR_WRIST))       where = WEAR_WRIST_R;
		if (CAN_WEAR(obj, ITEM_WEAR_WIELD))       where = WEAR_WIELD;
		if (CAN_WEAR(obj, ITEM_WEAR_LOBSX))       where = WEAR_LOBSX;
		if (CAN_WEAR(obj, ITEM_WEAR_SPALLE))      where = WEAR_SPALLE;
		if (CAN_WEAR(obj, ITEM_WEAR_EYE))         where=  WEAR_EYE;
		if (CAN_WEAR(obj, ITEM_WEAR_BOCCA))       where=  WEAR_BOCCA;
		if (CAN_WEAR(obj, ITEM_WEAR_RELIQUIA))    where=  WEAR_RELIQUIA;
		if (CAN_WEAR(obj, ITEM_WEAR_VESTE))       where=  WEAR_VESTE;
	}
	else {
		if ((where = search_block(arg, keywords, FALSE)) < 0) {
			sprintf(buf, "'%s'?  Che parte del corpo e' quella?\r\n", arg);
			send_to_char(buf, ch);
		}
	}
	
	return where;
}



ACMD(do_wear)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct obj_data *obj, *next_obj;
	int where, dotmode, items_worn = 0;
	
	two_arguments(argument, arg1, arg2);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg1)
	{
		send_to_char("Vestire cosa?\r\n", ch);
		return;
	}
	dotmode = find_all_dots(arg1);
	
	if (*arg2 && (dotmode != FIND_INDIV))
	{
		send_to_char("Non puoi specificare la stessa parte del corpo per piu' oggetti!\r\n", ch);
		return;
	}
	if (dotmode == FIND_ALL)
	{
		for (obj = ch->carrying; obj; obj = next_obj)
		{
			next_obj = obj->next_content;
			if (CAN_SEE_OBJ_DARK(ch, obj) && (where = find_eq_pos(ch, obj, 0)) >= 0)  /*rimosso da Belgarion per permettere wear all anche di armi*/
			{
				if (!CAN_WEAR(obj, ITEM_WEAR_WIELD))
				{
					items_worn++;
					perform_wear(ch, obj, where);
					//amanu tempo X vestirsi
					WAIT_STATE(ch, PULSE_VIOLENCE);
				}
				else{
					items_worn++;
					wield_weapon(ch, obj);
				}
			}
		}
		if (!items_worn)
			send_to_char("Non mi sembra che tu possa indossare niente.\r\n", ch);
	}
	else if (dotmode == FIND_ALLDOT)
	{
		if (!*arg1) {
			send_to_char("Vestire tutto cosa?\r\n", ch);
			return;
		}
		if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
			sprintf(buf, "Non sembra tu abbia %s.\r\n", arg1);
			send_to_char(buf, ch);
		} else
			while (obj) {
				next_obj = get_obj_in_list_vis(ch, arg1, obj->next_content);
				if ((where = find_eq_pos(ch, obj, 0)) >= 0){
					perform_wear(ch, obj, where);
					//amanu tempo X vestirsi
					WAIT_STATE(ch, PULSE_VIOLENCE);
				}
				else
					act("Non puoi mettere $p.", FALSE, ch, obj, 0, TO_CHAR);
				obj = next_obj;
			}
	} else {
		if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
			sprintf(buf, "Non sembra tu abbia %s.\r\n", arg1);
			send_to_char(buf, ch);
		} else {
			if ((where = find_eq_pos(ch, obj, arg2)) >= 0){
				perform_wear(ch, obj, where);
				//amanu tempo X svestirsi
				WAIT_STATE(ch, PULSE_VIOLENCE);
			}
			else if (!*arg2)
				act("Non puoi mettere $p.", FALSE, ch, obj, 0, TO_CHAR);
		}
	}
}


ACMD(do_wield)
{
	struct obj_data *obj;
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg)
		send_to_char("Impugnare cosa?\r\n", ch);
	else if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying)))
	{
		sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
		send_to_char(buf, ch);
	}
	else
	{
		if (!CAN_WEAR(obj, ITEM_WEAR_WIELD)) send_to_char("Non puoi impugnare quello.\r\n", ch);
		else wield_weapon(ch, obj);
	}
}

void wield_weapon(struct char_data * ch, struct obj_data *obj)
{
	if (GET_OBJ_WEIGHT(obj) > str_app[STRENGTH_APPLY_INDEX(ch)].wield_w)
		send_to_char("E' troppo pesante.\r\n", ch);
	else perform_wear(ch, obj, WEAR_WIELD);
}

ACMD(do_hang){
	struct obj_data *obj;
	char arg1[64],arg2[64];
	struct char_data *vict;
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	
	if(RIDING(ch)){
		send_to_char("Devi scendere da cavallo prima\n",ch);
		return;
	}
	
	if (!*argument){
		send_to_char("Trasportare cosa?\r\n", ch);
		return;
	}
	
	two_arguments(argument,arg1,arg2);
	
	if (!(obj = get_obj_in_list_vis(ch, arg1, world[ch->in_room].contents))){
		sprintf(buf, "Non vedi %s qui.\r\n", arg);
		send_to_char(buf, ch);
		return;
	}
	if(!CAN_WEAR(obj, ITEM_WEAR_HANG)){
		send_to_char("Non puoi trasportare quello!",ch);
		return;
	}
	if(*arg2){
		if(!(vict=give_find_vict(ch,arg2))){
			send_to_char("Non e' qui!",ch);
			return;
		}
		if(vict!=ch&&(!IS_NPC(vict)||(IS_NPC(vict)&&
					!MOB_FLAGGED(vict,MOB_MOUNTABLE)))){
			send_to_char("Non ha affatto voglia di trasportare qualcosa!",ch);
			return;
		}
		if(GET_EQ(vict,WEAR_HANG)){
			send_to_char("Sta gia trasportando qualcos'altro.",ch);
			return;
		}
	}
	else
		vict=ch;
	
	if(GET_EQ(vict,WEAR_HANG)){
		send_to_char("Stai gia trasportando qualcos'altro.",ch);
		return;
	}
	
	if(GET_EQ(vict,WEAR_WIELD)||GET_EQ(vict,WEAR_HOLD)||GET_EQ(vict,WEAR_SHIELD)){
		send_to_char("Hai bisogno di tutte e due le mani libere per farlo!",vict);
		return;
	}
	
	obj_from_room(obj);
	obj_to_char(obj, vict);
	perform_wear(vict,obj,WEAR_HANG);
}
ACMD(do_sgancia){
	struct obj_data *obj;
	if(!RIDING(ch)){
		send_to_char("Sganci un enorme carro dal tuo cavallo immaginario.. bravo!",ch);
		return;
	}
	if(!IS_NPC(RIDING(ch))||(IS_NPC(RIDING(ch))&&!MOB_FLAGGED(RIDING(ch),MOB_MOUNTABLE))){
		send_to_char("Poveretto ,non e' mica un cavallo!",ch);
		return;
	}
	if(!(obj=GET_EQ(RIDING(ch),WEAR_HANG))){
		send_to_char("Non sta trainando nulla attualmente.",ch);
		return;
	}
	obj_to_char((obj=unequip_char(RIDING(ch),WEAR_HANG)), RIDING(ch));
	obj_from_char(obj);
	obj_to_room(obj,ch->in_room);
	act("Sganci $p da $N.",FALSE,ch,obj,RIDING(ch),TO_CHAR);
	act("$n sgancia $p da $N.",FALSE,ch,obj,RIDING(ch),TO_NOTVICT);
}

ACMD(do_grab)
{
	struct obj_data *obj;
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg){
		send_to_char("Tenere cosa?\r\n", ch);
		return;
	}
	if (!(obj = get_obj_in_list_particular(ch, arg, ch->carrying,ITEM_LIGHT))) {
		sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
		send_to_char(buf, ch);
		return;
	}
	if(!check_free_hands(ch,obj))
		return;
	if (GET_OBJ_TYPE(obj) == ITEM_LIGHT)
		perform_wear(ch, obj, WEAR_LIGHT);
	
	else{
		if (!CAN_WEAR(obj, ITEM_WEAR_HOLD) && GET_OBJ_TYPE(obj) != ITEM_WAND   &&
			GET_OBJ_TYPE(obj) != ITEM_STAFF && GET_OBJ_TYPE(obj)!= ITEM_SCROLL &&
			GET_OBJ_TYPE(obj) != ITEM_POTION)
			send_to_char("Non puoi tenere quello.\r\n", ch);
		else
			perform_wear(ch, obj, WEAR_HOLD);
	}
}



void perform_remove(struct char_data * ch, int pos)
{
	struct obj_data *obj;
	if (!(obj = GET_EQ(ch, pos)))
		log("Error in perform_remove: bad pos passed.");
	else if (IS_OBJ_STAT(obj, ITEM_NODROP))
		act("Non puoi rimuovere $p, e' maledetto!", FALSE, ch, obj, 0, TO_CHAR);
	else if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
		act("$p: non puoi portare cosi' tanta roba!", FALSE, ch, obj, 0, TO_CHAR);
	else {
		if(pos==WEAR_HANG) {
			if((ch->in_room!=NOWHERE) && ROOM_FLAGGED(ch->in_room, ROOM_HOUSE)) {
			if( ((ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE)) &&
			    (num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_BIGHOUSE)) ||
			    ((num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_INROOM) &&
			    !ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE))) {
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return;
			}
		}
		/*
			if ((ch->in_room!=NOWHERE) && (ROOM_FLAGGED(ch->in_room,ROOM_HOUSE)) &&
				(num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) > MAX_OBJ_INROOM))	 
			{
				act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
				return;
			}
			
			if ((ch->in_room!=NOWHERE) && (ROOM_FLAGGED(ch->in_room,ROOM_HOUSE)) &&
			(ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE)) &&
			(num_obj_in_mag(ch->in_room) + num_obj_in_obj_mag(obj) >  MAX_OBJ_BIGHOUSE))	 
			{
				act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
				return;
			}*/
		}
			
		obj_to_char(unequip_char(ch, pos), ch);
		if(pos==WEAR_HANG) { //Se e' un carretto lo droppa
			obj_from_char(obj);
			obj_to_room(obj,ch->in_room);
		}	
		act("Smetti di usare $p.", FALSE, ch, obj, 0, TO_CHAR);
		act("$n smette di usare $p.", TRUE, ch, obj, 0, TO_ROOM);
		
	}
}

ACMD(do_remove)
{
	struct obj_data *obj=NULL;
	int i, dotmode, found;
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg) {
		send_to_char("Rimuovere cosa?\r\n", ch);
		return;
	}
	dotmode = find_all_dots(arg);
	
	if (dotmode == FIND_ALL) {
		found = 0;
		for (i = 0; i < NUM_WEARS; i++)
			if (GET_EQ(ch, i)) {
			//RIMODIFICA RELIQUIE
				if(GET_OBJ_TYPE(GET_EQ(ch,i))!=ITEM_RELIQUIA){    
					perform_remove(ch, i);
					//amanu tempo X svestirsi
					WAIT_STATE(ch, PULSE_VIOLENCE);
					found = 1;
				}
				else{
					if(GET_LEVEL(ch)< LVL_IMMORT){
						found = 1;
						if(IS_WILD(ch->in_room)||(ROOM_FLAGGED(ch->in_room,ROOM_TEMPIO))){
							obj_to_room(unequip_char(ch,i),ch->in_room);
							obj=GET_EQ(ch,i);
							send_to_char("Posi a terra una reliquia.\r\n",ch);
							act("$n posa a terra una reliquia.",TRUE,ch,obj,0,TO_ROOM);
						}
						else{
							send_to_char("Puoi posare una reliquia solo in wild o in un tempio.\r\n",ch);
						}
					}
					else{
						perform_remove(ch,i);
						//amanu tempo X svestirsi
						WAIT_STATE(ch, PULSE_VIOLENCE);
						found = 1;
					} 
				}
			}
		if (!found)
			send_to_char("Non stai usando niente.\r\n", ch);
	}
	else if (dotmode == FIND_ALLDOT) {
		if (!*arg)
			send_to_char("Rimuovere tutto di cosa?\r\n", ch);
		else {
			found = 0;
			for (i = 0; i < NUM_WEARS; i++)
				if (GET_EQ(ch, i) && CAN_SEE_OBJ(ch, GET_EQ(ch, i)) &&
					isname(arg, GET_EQ(ch, i)->name)) {
					if(GET_OBJ_TYPE(GET_EQ(ch,i))!=ITEM_RELIQUIA){
						perform_remove(ch, i);
						//amanu tempo X svestirsi
						WAIT_STATE(ch, PULSE_VIOLENCE);
						found = 1;
					}
					else{
						if(GET_LEVEL(ch)< LVL_IMMORT){
							found = 1;
							if(IS_WILD(ch->in_room)||(ROOM_FLAGGED(ch->in_room,ROOM_TEMPIO))){
								obj_to_room(unequip_char(ch,i),ch->in_room);
								send_to_char("Posi a terra una reliquia.\r\n",ch);//provvisorio..mandare un msg anche agli altri in room
								act("$n posa a terra una reliquia.",TRUE,ch,obj,0,TO_ROOM);
							}
							else { 
								send_to_char("Puoi posare una reliquia solo in wild o in un tempio.\r\n",ch);
							}
							
						}  
						else{
							perform_remove(ch,i);
							//amanu tempo X svestirsi
							WAIT_STATE(ch, PULSE_VIOLENCE);
							found = 1;
						} 
					}
				}
			if (!found) {
				sprintf(buf, "Non sembra tu abbia %s.\r\n", arg);
				send_to_char(buf, ch);
			}
		}
	}
	else {
		if (!(obj = get_object_in_equip_vis(ch, arg, ch->equipment, &i))) {
			sprintf(buf, "Non sembra tu stia usando %s.\r\n", arg);
			send_to_char(buf, ch);
		}
		else{
			if(i==WEAR_RELIQUIA){
				if(GET_LEVEL(ch)< LVL_IMMORT){
					if(IS_WILD(ch->in_room)||(ROOM_FLAGGED(ch->in_room,ROOM_TEMPIO))){
						obj_to_room(unequip_char(ch,i),ch->in_room);
						send_to_char("Posi a terra una reliquia.\r\n",ch);//provvisorio..mandare un msg anche agli altri in room
						act("$n posa a terra una reliquia.",TRUE,ch,obj,0,TO_ROOM);
					}
					else{
						send_to_char("Puoi posare una reliquia solo in wild o in un tempio.\r\n",ch);
					}
				}  
				else{
					perform_remove(ch,i);
					//amanu tempo X svestirsi
					WAIT_STATE(ch, PULSE_VIOLENCE);
				} 
			}
			else{ 
				perform_remove(ch, i);
				//amanu tempo X svestirsi
				WAIT_STATE(ch, PULSE_VIOLENCE);
			}
		}
	}
	if(GET_EQ(ch,WEAR_WIELD_L)&&!GET_EQ(ch,WEAR_WIELD)){
		obj_to_char((obj=unequip_char(ch,WEAR_WIELD_L)), ch);
		obj_from_char(obj);
		equip_char(ch, obj, WEAR_WIELD);
	}
}

ACMD(do_pull)
{
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	if (!*arg) {
		send_to_char("Pull what?\r\n", ch);
		return;
	}
/* for now only pull pin will work and must be wielding a grenade */
	if(!str_cmp(arg,"sicura")) {
		if(GET_EQ(ch, WEAR_WIELD)) {
			if(GET_OBJ_TYPE(GET_EQ(ch, WEAR_WIELD)) == ITEM_GRENADE) {
				send_to_char("You pull the pin, the grenade is activated!\r\n", ch);
				SET_BIT(GET_OBJ_EXTRA(GET_EQ(ch, WEAR_WIELD)), ITEM_LIVE_GRENADE);
			} else
				send_to_char("That's NOT a grenade!\r\n", ch);
		} else
			send_to_char("You aren't wielding anything!\r\n", ch);
	}
	
/* put other 'pull' options here later */
	return;
}


ACMD(do_rigenera){
	
	struct char_data *victim;
	
		
	
	one_argument(argument, arg);
	
	if(GET_LEVEL(ch)<71){
		if (affected_by_spell (ch, SPELLSKILL, DISEASE_SHAARR)){
			send_to_char ("&1Shaarr ti ha maledetto. Questi mezzi mortali non ti redimeranno alla sua vista!\r\n", ch);
			return;
		}
		if(ROOM_FLAGGED(ch->in_room,ROOM_TEMPIO)){
		    if(!*arg)
			send_to_char("Specifica una parte del corpo,oppure digita\r\n<<rigenera tariffe>> per conoscere i vari costi.\r\n",ch);
		    if(is_abbrev(arg,"tariffe")){
		    send_to_char("Il costo della rigenerazione e' di:\r\n",ch);
		    send_to_char("500 coins per ferite alla testa (rigenera testa)\r\n",ch);
		    send_to_char("750 coins per ferite agli arti superiori (rigenera braccio_destro o rigenera braccio_sinistro) \r\n",ch);
		    send_to_char("400  coins per ferite all'addome (rigenera addome)\r\n",ch);
		    send_to_char("600 coins per ferite agli arti inferiori (rigenera gamba_destra o rigenera gamba_sinistra\r\n",ch);
		    send_to_char("100 coins per curare le ferite sanguinanti (rigenera ferite)\r\n",ch);
		    send_to_char("1500 coins per rigenerazione completa (rigenera all)\r\n",ch);
		    send_to_char("&8&7Questi prezzi sono momentanei.&0\r\n",ch);
		    return;
		    }
		    
		    if(is_abbrev(arg,"testa")){
				if ((!AFF_FLAGGED(ch,AFF_CRITICAL_TESTA_1)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_TESTA_2)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_TESTA_3))) {
						send_to_char("E' inutile fasciarti la testa se non hai ferite.\r\n",ch);
						return;
				}
			if(GET_GOLD(ch)>=500){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_1);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_2);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_3);
			    GET_GOLD(ch)-=500;
			    send_to_char("Tutte le ferite alla testa sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		    if(is_abbrev(arg,"braccio_destro")){
			if ((!AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_1)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_2)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3))) {
						send_to_char("E' inutile fasciarti il braccio destro se non hai ferite.\r\n",ch);
						return;
				}
		    if(GET_GOLD(ch)>=750){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_1);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_2);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_3);
			    GET_GOLD(ch)-=750;
			    send_to_char("Tutte le ferite al braccio destro sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		    if(is_abbrev(arg,"braccio_sinistro")){
				if ((!AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_1)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_2)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3))) {
						send_to_char("E' inutile fasciarti il braccio sinistro se non hai ferite.\r\n",ch);
						return;
				}
		    if(GET_GOLD(ch)>=750){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_1);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_2);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_3);
			    GET_GOLD(ch)-=750;
			    send_to_char("Tutte le ferite al braccio sinistro sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		    if(is_abbrev(arg,"addome")){
				if ((!AFF_FLAGGED(ch,AFF_CRITICAL_TORSO_1)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_TORSO_2)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_TORSO_3))) {
						send_to_char("E' inutile fasciarti l'addome se non hai ferite.\r\n",ch);
						return;
				}
		    if(GET_GOLD(ch)>=400){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_1);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_2);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_3);
			    GET_GOLD(ch)-=400;
			    send_to_char("Tutte le ferite all'addome sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		    if(is_abbrev(arg,"gamba_destra")){
			if ((!AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_1)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_2)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_3))){
						send_to_char("E' inutile fasciarti la gamba destra se non hai ferite.\r\n",ch);
						return;
				}
		    if(GET_GOLD(ch)>=600){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_1);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_2);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_3);
			    GET_GOLD(ch)-=600;
			    send_to_char("Tutte le ferite alla gamba destra sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
			
		    }
		    if(is_abbrev(arg,"gamba_sinistra")){
				if ((!AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_1)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_2)) && (!AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_3))) {
						send_to_char("E' inutile fasciarti la gamba sinistra se non hai ferite.\r\n",ch);
						return;
				}
		    if(GET_GOLD(ch)>=600){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_1);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_2);
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_3);
			    GET_GOLD(ch)-=600;
			    send_to_char("Tutte le ferite alla gamba sinistra sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		    if(is_abbrev(arg,"ferite")){
				if(!AFF_FLAGGED(ch,AFF_FERITO)){
						send_to_char("E' inutile, non hai ferite.\r\n",ch);
						return;
				}
		    if(GET_GOLD(ch)>=100){
			    REMOVE_BIT(AFF_FLAGS(ch),AFF_FERITO);
			    GET_GOLD(ch)-=100;
			    send_to_char("Tutte le ferite sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		    if(is_abbrev(arg,"all")){
		    if(GET_GOLD(ch)>=1500){
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_1);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_2);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_3);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_1);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_2);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_3);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_1);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_2);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_3);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_1);	
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_2);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_3);		
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_1);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_2);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_3);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_1);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_2);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_3);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_FERITO);
			GET_GOLD(ch)-=1500;
			send_to_char("Tutte le tue ferite sono magicamente guarite.\r\n",ch);
			}
			else{
			    send_to_char("Non hai abbastanza denaro con te,torna quando lo avrai.\r\n",ch);
			    send_to_char("digita il comando <<rigenera tariffe>> per conoscere le tariffe attuali.\r\n",ch);
			}
			return;
		    }
		}
		else 
		send_to_char("Devi essere in un tempio per chiedere di essere curato.\r\n",ch);
		return;
	}
	
	if (!*arg){
		send_to_char("Curare chi?\r\n", ch);
		return;
		}
	if(!(victim=get_char_room_vis(ch,arg))){
		send_to_char("Non c'e' nessuno qui con quel nome.\r\n",ch);		
		return;
		}
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_1);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_2);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_3);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_1);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_2);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_3);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_1);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_2);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_3);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_1);	
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_2);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_3);		
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_1);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_2);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_3);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_1);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_2);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_3);
	REMOVE_BIT(AFF_FLAGS(victim),AFF_FERITO);	
	act("&8&7Riesci a rigenerare completamente $N.&0",FALSE,ch,0,victim,TO_CHAR); 
	act("&8&7$n ti rigenera completamente.&0",FALSE,ch,0,victim,TO_VICT);
	act("&8&7$n rigenera completamente $N.&0",FALSE,ch,0,victim,TO_NOTVICT);
}


ACMD(do_fillet) {
	struct obj_data *obj, *fillet;
	char name[50];
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	
	one_argument(argument, arg);
	
	if (!*arg)
		send_to_char("Filettare cosa?\r\n", ch);
	else if (!(obj = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
		sprintf(buf, "Non sembra ci sia %s.\r\n", arg);
		send_to_char(buf, ch);
	}
	else
	if (GET_OBJ_TYPE(obj) != ITEM_CONTAINER || GET_OBJ_VAL(obj, 3) != 1) {
		send_to_char("Non puoi usare fillet su quello.\r\n", ch);
	}
	else {
		if(GET_OBJ_CSLOTS(obj)<=0){
			send_to_char("Non c'e piu' nulla da tagliare qui...", ch);
			return;
		}
		if (!test_and_improve(ch, SKILL_FILLET, 100, 15, 0)) {
			send_to_char("Sembra che tu non riesca a trovare il posto giusto per tagliare.", ch);
		}
		else {
			if (real_object(5)==-1){ // Il vnum 5 e' un fillet generico !!!
				send_to_char("Oggetto fillet non presente in memoria. Per favore contatta un creatore.", ch);
				return;
			}
			fillet = create_obj();
			fillet->item_number=real_object(5);//Il vnum 5 e' un fillet generico !!
			fillet->in_room = NOWHERE;
			fillet->name = str_dup("filetto");
			strcpy(name, "Un filetto ");
			strcat(name, strstr(obj->description, "di"));
			fillet->description = str_dup(name);
			strcpy(name, "Un filetto ");
			strcat(name, strstr(obj->short_description, "di"));
			fillet->short_description = str_dup(name);
			GET_OBJ_TYPE(fillet) = ITEM_FOOD;
			GET_OBJ_WEAR(fillet) = ITEM_WEAR_TAKE;
			GET_OBJ_EXTRA(fillet) = ITEM_RESTRING;
			GET_OBJ_VAL(fillet, 0) =MIN(6,MAX(1,number(1,GET_OBJ_TSLOTS(obj)/10)));
			if(GET_OBJ_VAL(obj,1)==0)
				GET_OBJ_VAL(fillet, 3) = 0;/*Good food*/
			else
				GET_OBJ_VAL(fillet, 3) = 1;/*sorry ,it's Shit!*/
			GET_OBJ_WEIGHT(fillet) = 1;
			GET_OBJ_CSLOTS(fillet) = 20;
			GET_OBJ_TSLOTS(fillet) = 20;
			GET_OBJ_TIMER(fillet) = 0;
			GET_OBJ_TIMER(obj) -= 5;
			GET_OBJ_CSLOTS(obj)-=1;
			act("$n ricava un filetto dal $p.", FALSE, ch, obj, 0, TO_ROOM);
			act("Ricavi un filetto dal $p.", FALSE, ch, obj, 0, TO_CHAR);
			obj_to_char(fillet, ch);
		}
	}
}

ACMD(do_cook) {
	struct obj_data *obj;
	
	one_argument(argument, arg);
	
	if (!*arg)
		send_to_char("Cook what?\r\n", ch);
	else if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
		sprintf(buf, "I can't seem to find %s %s.\r\n", AN(arg), arg);
		send_to_char(buf, ch);
	} else if (GET_OBJ_TYPE(obj) != ITEM_FOOD)	{
		send_to_char("Non lo puoi cucinare .\r\n", ch);
	} else {
		act("$n cucina $p.", FALSE, ch, obj, 0, TO_ROOM);
		act("Cucini $p.", FALSE, ch, obj, 0, TO_CHAR);
		if (GET_OBJ_VAL(obj, 2) == 0 && test_and_improve(ch, SKILL_COOK, 100, 15, 0)) {
			GET_OBJ_VAL(obj, 3) = 0;
			GET_OBJ_VAL(obj, 2) = 1;
			strcat(obj->short_description, " (fully cooked)");
		}
	}
}
ACMD(do_repair)
{
	struct obj_data *repair;
	int chance;
	struct obj_data *obj;
	int mag_materials(struct char_data * ch, int item0, int item1, int item2, int extract, int verbose);
	
	if (!CAN_LEARN_ABIL(ch, ABIL_RIPARAZIONE) || (GET_ABIL(ch, ABIL_RIPARAZIONE) <= 0)) {
		send_to_char("Non conosci l'arte di riparare!\r\n", ch);
		return;
	}
	
	one_argument(argument, arg);
	
	if (!*arg) {
		send_to_char("Riparare cosa?\r\n", ch);
		return;
	}
	
	if (!(repair = get_obj_in_list_vis(ch, arg, ch->carrying))) {
		sprintf(buf, "Non sembra tu abbia %s %s.\r\n", AN(arg), arg);
		send_to_char(buf, ch);
		return;
	}
	
	if ((GET_OBJ_CSLOTS(repair) == 0) && (GET_OBJ_TSLOTS(repair) == 0)) {
		act("$p sembra essere indistruttibile!", FALSE, ch, repair, 0,
			TO_CHAR); return;
	}
	
	if (GET_OBJ_CSLOTS(repair) == GET_OBJ_TSLOTS(repair)) {
		act("$p sembra essere in perfette condizioni!", FALSE, ch, repair, 0,
			TO_CHAR); return;
	}
	if(!(obj=GET_EQ(ch,WEAR_HOLD))){
		send_to_char("Devi avere un martello impugnato per fare questo.\r\n",ch);
		return;
	}
	if(GET_OBJ_TYPE(obj)!=ITEM_MARTELLO){
		send_to_char("Devi avere un martello impugnato per fare questo.\r\n",ch);
		return;
	}
	
	if (GET_OBJ_CSLOTS(repair) < 0) {
		act("Rovini completamente $p che si sfalda in mille pezzi!", FALSE, ch,
			repair, 0, TO_CHAR); act("$n tenta di riparare $p, ma si rompe in mille pezzi!", TRUE, ch, repair, 0, TO_ROOM); extract_obj(repair); return;
	}
	if (!mag_materials(ch, 13, 0, 0, 1,1)){
		send_to_char("Devi avere un lingotto per poter riparare.\r\n",ch);
		return;
	}
	if(GET_PROF(ch,PROF_FABBRO)<30){
		send_to_char("Devi essere un fabbro decente per poter riparare.!",ch);
		return;
	}
	
	chance=number(1,100);
	
	if(ROOM_FLAGGED(ch->in_room,ROOM_FORGIA))
		chance-=10;
	if ((chance=number(1,100)) >
		(check_gen_abil(ch,ABIL_RIPARAZIONE)+(GET_INT(ch)+GET_WIS(ch)-26)*2.5)) {
		act("Tenti maldestramente di riparare $p ma lo danneggi ancora di piu'!",
			FALSE, ch, repair, 0, TO_CHAR); act("$n tenta di riparare $p, ma lo danneggia ancora di piu'!", TRUE, ch, repair, 0, TO_ROOM);
		GET_OBJ_CSLOTS(repair) = MAX(GET_OBJ_CSLOTS(repair) - 10, 1); 
		GET_OBJ_TSLOTS(repair) = MAX(GET_OBJ_TSLOTS(repair) - 10, 1); 
		return;
	}
	else {
		act("Ripari $p e ritorna come nuovo!",
			FALSE, ch, repair, 0, TO_CHAR); act("$n ripara $p, facendolo diventare come nuovo!\r\n", TRUE, ch, repair, 0, TO_ROOM);
		GET_OBJ_TSLOTS(repair) = MAX(GET_OBJ_TSLOTS(repair) - number(1,5),1);
		GET_OBJ_CSLOTS(repair) =GET_OBJ_TSLOTS(repair);
		return ;
	}
}

ACMD(do_scarica)
{
	int amount, dotmode;
	struct char_data *vict;
	struct obj_data *obj, *next_obj;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int give_num;
	
	void perform_scarica_gold(struct char_data * ch, struct char_data * vict,int amount);
	void perform_scarica(struct char_data *ch,struct char_data *vict,struct obj_data *obj);
	
  /***********************    Enrico Add:1) Blocco per gli AFF_IMMOBIL  */
  /*			                 2) comando Unstow              */
	
	if (AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\n",ch);
		return;
	}
	
	if (subcmd==SCMD_UNSTOW)
	{
		argument=one_argument(argument,arg);
		if (!*arg)
		{
			if (!TRANSPORTED(ch))
				send_to_char("Non hai nessuno sulle spalle!\r\n",ch);
			else
			{
				sprintf(buf,"Posi dolcemente [mica tanto] %s a terra.\r\n",GET_NAME(TRANSPORTED(ch)));
				send_to_char(buf,ch);
				sprintf(buf,"%s ti sbatte pesantemente a terra. Ahia!.\r\n",GET_NAME(ch));
				send_to_char(buf,TRANSPORTED(ch));
				act("$n si scrolla di dosso $N che cade pesantemente per terra. Crack!",FALSE,ch,0,TRANSPORTED(ch),TO_ROOM);
				if (GET_POS(TRANSPORTED(ch))>POS_SITTING)
					GET_POS(TRANSPORTED(ch))=POS_SITTING;
				unstow_char(ch);
			}
		}
		else
		{
			if ((vict=give_find_vict(ch,arg)))
			{
				if (!TRANSPORTED(vict))
				{
					send_to_char("Non ha nessuno caricato sulle spalle!\r\n",ch);
					return;
				}
				else {
					if (IS_NPC(vict) && (vict->master) && (vict->master == ch))
					{
						sprintf(buf,"Posi dolcemente [mica tanto] %s a terra.\r\n",
							GET_NAME(TRANSPORTED(vict)));
						send_to_char(buf,ch);
						sprintf(buf,"%s ti sbatte pesantemente  a terra. Ahia!.\r\n",
							GET_NAME(ch));
						send_to_char(buf,TRANSPORTED(vict));
						act("$n si scrolla di dosso $N che cade pesantemente per terra.Crack!",
							FALSE,ch,0,TRANSPORTED(vict),TO_ROOM);
						GET_POS(TRANSPORTED(vict))=POS_SITTING;
						unstow_char(vict);
					} else 
						send_to_char("Non ti ascolta.",vict);		  
				}
			}
			else
				send_to_char("Chi e' che vorresti scaricare?\r\n",ch);
		}
		return;
	}
  /***********************************************************************/
	
	argument = one_argument(argument, arg);
	
	if (!*arg)
		send_to_char("Scaricare cosa e da chi?\r\n", ch);
	else if (is_number(arg))
	{
		
		amount = atoi(arg);
		argument = one_argument(argument, arg);
		if (!str_cmp("coins", arg) || !str_cmp("coin", arg))
		{
			argument = one_argument(argument, arg);
			if ((vict = give_find_vict(ch, arg)))
				perform_scarica_gold(ch, vict, amount);
			//safe_save_pg(ch,vict);
			return;
		}
		else
		{
			give_num = amount;
			argument = two_arguments(argument, arg1, arg2);
			if ((vict = give_find_vict(ch, arg1))){
				for (amount = 0; amount < give_num; amount++)
					if (!(obj = get_obj_in_list_vis(ch, arg, vict->carrying))) {
						sprintf(buf, "Non sembra abbia %s.\r\n", arg);
						send_to_char(buf, ch);
						amount = give_num; /* just let them know no more then stop */
					} else perform_scarica(ch, vict, obj);      
			//safe_save_pg(ch,vict);
			return;
			}
		}
		
	}
	else
	{
		one_argument(argument, buf1);
		if (!(vict = give_find_vict(ch, buf1)))
			return;
		if (!MOB_FLAGGED(vict, MOB_MOUNTABLE))
		{
			send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
			return;
		}
		dotmode = find_all_dots(arg);
		if (dotmode == FIND_INDIV)
		{
			if (!(obj = get_obj_in_list_vis(ch, arg, vict->carrying)))
			{
				sprintf(buf, "Non sembra che abbia %s.\r\n", arg);
				send_to_char(buf, ch);
			}
			else{
				perform_scarica(ch, vict, obj);
				//safe_save_pg(ch,vict);
			}
		}
		else
		{
			if (dotmode == FIND_ALLDOT && !*arg)
			{
				send_to_char("Tutto cosa?\r\n", ch);
				return;
			}
			if (!vict->carrying)
				send_to_char("Non sembra che stia portando qualcosa.\r\n", ch);
			else {
				for (obj = vict->carrying; obj; obj = next_obj)
				{
					next_obj = obj->next_content;
					if (CAN_SEE_OBJ(ch, obj) && ((dotmode == FIND_ALL || isname(arg, obj->name))))
						perform_scarica(ch, vict, obj);
				}
				//safe_save_pg(ch,vict);
			}//else
		}
	}
}

void perform_scarica(struct char_data *ch,struct char_data *vict,struct obj_data *obj)
{
	if (IS_OBJ_STAT(obj, ITEM_NODROP))
	{
		act("Non puoi rimuovere $p.  Yeech!", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
	if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
	{
		send_to_char("Sei gia' carico all'inverosimile, non puoi portare cosi' tante cose.\r\n", ch);
		return;
	}
	
	if (GET_OBJ_WEIGHT(obj) + IS_CARRYING_W(ch) > CAN_CARRY_W(ch))
	{
		send_to_char("Sei gia' carico all'inverosimile, non puoi portare cosi' tanto peso.\r\n", ch);
		return;
	}
	if (!MOB_FLAGGED(vict, MOB_MOUNTABLE))
	{
		send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
		return;
	}
	if (!give_otrigger(obj, ch, vict) || !receive_mtrigger(vict, ch, obj))
		return;
	
	obj_from_char(obj);
	obj_to_char(obj, ch);
	act("Scarichi $p da $N.", FALSE, ch, obj, vict, TO_CHAR);
	act("$n scarica $p da $N.", TRUE, ch, obj, vict, TO_NOTVICT);
}

void perform_scarica_gold(struct char_data * ch, struct char_data * vict,int amount)
{
	if (amount <= 0)
	{
		send_to_char("Heh heh heh ... hai intenzione di scherzare oggi, eh?\r\n", ch);
		return;
	}
	if (GET_GOLD(vict) < amount)
	{
		send_to_char("Non non porta cosi tanti coins!\r\n", ch);
		return;
	}
	if (!MOB_FLAGGED(vict, MOB_MOUNTABLE))
	{
		send_to_char("Ti sembra un mulo o qualcosa del genere?!?\r\n", ch);
		return;
	}
	send_to_char(OK, ch);
	sprintf(buf, "$n scarichi %d coins.", amount);
	act(buf, FALSE, ch, 0, vict, TO_VICT);
	sprintf(buf, "$n scarica %s da $N.", money_desc(amount));
	act(buf, TRUE, ch, 0, vict, TO_NOTVICT);
	GET_GOLD(vict) -= amount;
	GET_GOLD(ch) += amount;
}


/***Bug dei coins passati ad un pg prima dei reboot Shade***/
void safe_save_pg(struct char_data *ch)
{
	struct char_data *vict;
	extern void Crash_MobEqSave(struct char_data * ch, room_vnum room);
	
	if ((ch)&& (ch->in_room!=NOWHERE)){
		for (vict = world[ch->in_room].people ;vict;vict = vict->next_in_room) {
			if (!IS_NPC(vict) && (ch!=vict))
				save_char(vict, ch->in_room);
			if (IS_NPC(vict)) {
				if (MOB_FLAGGED(vict,MOB_SAVE) && MOB_FLAGGED(vict,MOB_CRASH)){
					save_mob(vict, vict->in_room);
					REMOVE_BIT(MOB_FLAGS(vict), MOB_CRASH);
				}
				if (MOB_FLAGGED(vict,MOB_SAVEINV)&& MOB_FLAGGED(vict,MOB_CRASH_EQ)) {
					Crash_MobEqSave(vict, GET_MOB_ID(vict));
					REMOVE_BIT(MOB_FLAGS(vict), MOB_CRASH_EQ);
				}
			}
		}	 
	}
}
