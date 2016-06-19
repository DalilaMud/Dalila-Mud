/* ************************************************************************
*   File: limits.c                                      Part of CircleMUD *
*  Usage: limits & gain funcs for HMV, exp, hunger/thirst, idle time      *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************* */

#include <math.h>
#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "spells.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "dg_scripts.h"	//Orione
#include "interpreter.h"

#define READ_TITLE(ch) (GET_SEX(ch) == SEX_MALE ? titles[(int)GET_CLASS(ch)][(int)GET_LEVEL(ch)].title_m : titles[(int)GET_CLASS(ch)][(int)GET_LEVEL(ch)].title_f)

extern int has_boat(struct char_data *ch);
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
extern struct room_data *world;
extern int max_exp_gain;
extern int max_exp_loss;

extern int newbye(struct char_data *ch);
extern int get_stipendiomob(struct char_data *ch);
extern void	store_mail(long to, long from, char *message_pointer);

extern int mag_savingthrow(struct char_data * ch, struct char_data * caster, int type, int abil, int modifier);

int search_noexp (struct char_data *ch);
int noexp(struct char_data *ch);

const char *hunger_mess[]={ " ","&7Inizi ad aver fame.&0\r\n","&1Hai Fame.&0\r\n","&3Hai Molta Fame.&0\r\n","&1Sei sfinito.&0&3Morirai se non mangi qualcosa.&0\r\n"};

const char *thirsty_mess[]={" ","&6Inizi ad avere sete.&0\r\n","&2Hai Sete.&0\r\n","&1Hai Molta Sete.&0\r\n","&1Sei completamente Disidratato.&0\r\n"};

/* When age < 15 return the value p0 */
/* When age in 15..29 calculate the line between p1 & p2 */
/* When age in 30..44 calculate the line between p2 & p3 */
/* When age in 45..59 calculate the line between p3 & p4 */
/* When age in 60..79 calculate the line between p4 & p5 */
/* When age >= 80 return the value p6 */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{
	
	if (age < 15)
		return (p0);		/* < 15   */
	else if (age <= 29)
		return (int) (p1 + (((age - 15) * (p2 - p1)) / 15));	/* 15..29 */
	else if (age <= 44)
		return (int) (p2 + (((age - 30) * (p3 - p2)) / 15));	/* 30..44 */
	else if (age <= 59)
		return (int) (p3 + (((age - 45) * (p4 - p3)) / 15));	/* 45..59 */
	else if (age <= 79)
		return (int) (p4 + (((age - 60) * (p5 - p4)) / 20));	/* 60..79 */
	else
		return (p6);		/* >= 80 */
}


/*
 * The hit_limit, mana_limit, and move_limit functions are gone.  They
 * added an unnecessary level of complexity to the internal structure,
 * weren't particularly useful, and led to some annoying bugs.  From the
 * players' point of view, the only difference the removal of these
 * functions will make is that a character's age will now only affect
 * the HMV gain per tick, and _not_ the HMV maximums.
 */


#define DEFAULT_HIT_GAIN  10
#define DEFAULT_MANA_GAIN 10
#define DEFAULT_MOVE_GAIN 25
#define HIT  0
#define MANA 1
#define MOVE 2
#define LIGHT 1
#define COND  2
#define HEAVY 3
#define TOTAL 4
/*Hunger level affects Hit_gain & Move_gain*/
/*Thirsty level affects Mana_gain & Move_gain*/

int update_gain(struct char_data *ch, int gain, int what_gain){
	int check_condition(struct char_data *ch, int condition);
	int hunger_gain = check_condition(ch, FULL);
	int thirsty_gain = check_condition(ch, THIRST);
	int new_gain = gain;
	
	if(IS_NPC(ch))
		return gain;
		
	switch(what_gain){
		case HIT:
			switch(hunger_gain){
				case LIGHT:
				  new_gain = gain;
				  break;
				case COND:
				  new_gain = gain/2;
				  break;
				case HEAVY:
				  new_gain = 1;
				  break;
				case TOTAL:
				  new_gain = -(30-GET_COND(ch, FULL));
				  break;/*cioe da -5 a -15*/
				default:break;
			}
			break;
		case MANA:
			switch(thirsty_gain){
				case LIGHT:new_gain=gain;break;
				case COND:new_gain=gain/2; break;
				case HEAVY:new_gain=1;break;
				case TOTAL:new_gain=-(30-GET_COND(ch,THIRST));break;/*cioe da -5 a -15*/
				default:break;
			}
			break;
		case MOVE:
			switch(hunger_gain){
				case COND:new_gain=new_gain/2; break;
				case HEAVY:new_gain=new_gain/4;break;
				case TOTAL:new_gain=1;break;
				default:break;
			}
			switch(thirsty_gain){
				case COND:new_gain=new_gain/2; break;
				case HEAVY:new_gain=new_gain/4;break;
				case TOTAL:new_gain=1;break;
				default:break;
			}
		default: break;
	}
	return new_gain;
}
/* manapoint gain pr. game hour */
int mana_gain(struct char_data * ch)
{
	int gain;
	
	if (IS_NPC(ch)) {
		if(AFF_FLAGGED(ch, AFF_POISON) || AFFECTED(ch, AFF_FLYING, BITV_FLYING))
			return 0;
		else
      /* Neat and fast */
			gain = GET_LEVEL(ch);
	}
	else {
		
		//    gain = graf(age(ch).year, 4, 8, 12, 16, 12, 10, 8);
		gain= (DEFAULT_MANA_GAIN + (GET_LEVEL(ch)/10));
    /* Class calculations */
		
    /* Skill/Spell calculations */
		
    /* Position calculations    */
		switch (GET_POS(ch)) {
			case POS_SLEEPING:
				gain <<= 1;
				break;
			case POS_RESTING:
				gain += (gain >> 1);	/* Divide by 2 */
				break;
			case POS_SITTING:
				gain += (gain >> 2);	/* Divide by 4 */
				break;
		}
		if(!IS_NPC(ch)){
			if(GET_CLASS(ch)==CLASS_MAGIC_USER)
				gain=((gain*2) + (GET_LEVEL(ch)/4)) ;
			if(GET_CLASS(ch)==CLASS_CLERIC)
				gain=gain/2;
			if(GET_CLASS(ch)==CLASS_PELOI)
				gain=gain*1.25;
			if(GET_CLASS(ch)==CLASS_THIEF)
				gain=gain*1.1;
		}
				
		gain = gain + ch->char_specials.managain;
		gain = update_gain(ch, gain, MANA);

		if (IS_AFFECTED(ch, AFF_POISON) || AFFECTED(ch, AFF_FLYING, BITV_FLYING))
			gain =0;
	}
  /*Blizzard: Help out poor linkless people not do die by hunger */
	if (!IS_NPC(ch) && !(ch->desc))
		return 0;
	return (gain);
}


int hit_gain(struct char_data * ch)
/* Hitpoint gain pr. game hour */
{
	int gain;
	
	if (IS_NPC(ch)) {
		if(AFF_FLAGGED(ch,AFF_POISON) || AFFECTED(ch, AFF_CHILLED, BITV_CHILLED) || AFFECTED(ch, AFF_BURNED, BITV_BURNED) || affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE) || affected_by_spell (ch, SPELLSKILL, DISEASE_RAFFREDDORE))
			return 0;
		else
			gain = GET_LEVEL(ch);
    /* Neat and fast */
	}
	else {
		// gain = graf(age(ch).year, 8, 12, 20, 32, 16, 10, 4);
		gain=DEFAULT_HIT_GAIN;
    /* Class/Level calculations */
		
    /* Skill/Spell calculations */
		
    /* Position calculations    */
		
		switch (GET_POS(ch)) {
			case POS_SLEEPING:
				gain += (gain >> 1);	/* Divide by 2 */
				break;
			case POS_RESTING:
				gain += (gain >> 2);	/* Divide by 4 */
				break;
			case POS_SITTING:
				gain += (gain >> 3);	/* Divide by 8 */
				break;
		}
		if(!IS_NPC(ch)){
			if(GET_CLASS(ch)==CLASS_WARRIOR)
				gain=gain*2;
			if(GET_CLASS(ch)==CLASS_PELOI)
				gain=gain*1.25;
			if(GET_CLASS(ch)==CLASS_THIEF)
				gain=gain*1.5;
		}
		gain = gain + ch->char_specials.hitgain;
		gain=update_gain(ch,gain,HIT);

		if (IS_AFFECTED(ch, AFF_POISON))
			gain = -5;
		else if (AFFECTED(ch, AFF_CHILLED, BITV_CHILLED) || AFFECTED(ch, AFF_BURNED, BITV_BURNED) || affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE) || affected_by_spell (ch, SPELLSKILL, DISEASE_RAFFREDDORE))
			gain = 0;
    /*Blizzard: Help out poor linkless people not do die by hunger */
		if (!IS_NPC(ch) && !(ch->desc))
			return 0;
	}
  /*Blizzard: Help out poor linkless people not do die by hunger */
	if (!IS_NPC(ch) && !(ch->desc))
		return 0;
	return (gain);
}



int move_gain(struct char_data * ch)
/* move gain pr. game hour */
{
	int gain;
	
	if (IS_NPC(ch)) {
		if(AFF_FLAGGED(ch,AFF_POISON) || (affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE) && GET_MOVE(ch) >= 4))
			return 0;
		else{
			gain=DEFAULT_MOVE_GAIN;
			if(MOB_FLAGGED(ch,MOB_MOUNTABLE))
				gain+=(GET_LEVEL(ch)/2);//da 20 a GET_LEVEL(ch)/2
		}
		return gain;
    /* Neat and fast */
	}
	else {
		//   gain = graf(age(ch).year, 16, 20, 24, 20, 16, 12, 10);
		gain=DEFAULT_MOVE_GAIN;
      /* Class/Level calculations */
		
    /* Skill/Spell calculations */
		
		
    /* Position calculations    */
		switch (GET_POS(ch)) {
			case POS_SLEEPING:
				gain <<= 1;
				break;
			case POS_RESTING:
				gain += (gain >> 1);	/* Divide by 2 */
				break;
			case POS_SITTING:
				gain += (gain >> 2);	/* Divide by 4 */
				break;
		}
	}
	if(!IS_NPC(ch)){
		if(GET_CLASS(ch)==CLASS_PELOI)
			gain=gain*2;
		if(GET_CLASS(ch)==CLASS_WARRIOR)
			gain=gain*1.1;
		if(GET_CLASS(ch)==CLASS_THIEF)
			gain=gain*1.5;
	}
	
	if (IS_AFFECTED(ch, AFF_POISON) || affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE))
		gain =0;
	gain=update_gain(ch,gain,MOVE);
	return (gain);
}



void set_title(struct char_data * ch, char *title)
{
	if (title == NULL)
		title = READ_TITLE(ch);
	
	if (strlen(title) > MAX_TITLE_LENGTH)
		title[MAX_TITLE_LENGTH] = '\0';
	
	if (GET_TITLE(ch) != NULL)
		free(GET_TITLE(ch));
	
	GET_TITLE(ch) = str_dup(title);
}


void check_autowiz(struct char_data * ch)
{
	char buf[100];
	extern int use_autowiz;
	extern int min_wizlist_lev;
	pid_t getpid(void);
	
	if (use_autowiz && GET_LEVEL(ch) >= LVL_IMMORT) {
		sprintf(buf, "nice ../bin/autowiz %d %s %d %s %d &", min_wizlist_lev,
			WIZLIST_FILE, LVL_IMMORT, IMMLIST_FILE, (int) getpid());
		mudlog("Initiating autowiz.", CMP, LVL_IMMORT, FALSE);
		system(buf);
	}
}
int noexp(struct char_data *ch) {	
	struct follow_type *foll=NULL;
	
	foll=ch->followers;
	while (foll) {
		if (noexp(foll->follower))
			return 1;
		foll=foll->next;
	}
	
	if (IS_NPC(ch)&&MOB_FLAGGED(ch,MOB_NOEXP))
		return 1;
	else
		return 0;
}
int search_noexp (struct char_data *ch) {
	struct char_data *pg;
	pg=ch;
	
	while (pg->master) 
		pg=pg->master;
	
	return noexp(pg);
}

int gain_exp(struct char_data * ch, int gain)
{
	int is_altered = FALSE;
	int num_levels = 0;
	char buf[128];
	
	if (!IS_NPC(ch) && (GET_LEVEL(ch) < 1 || GET_LEVEL(ch) >= LVL_IMMORT - 1))
		return 0;
	
	if (search_noexp(ch)&&(gain>0)) {
		send_to_char("&1Gli Dei ti impediscono di fare XP!&0\r\n",ch);
		return 0;
	}
	
	if (IS_NPC(ch))
	{
		GET_EXP(ch) += gain;
		return gain;
	}

    /* Possibilita' di piagarsi le mani con troppi xp (by Spini) */
	GET_XP_GIORNALIERI(ch) += gain;

	if (GET_LEVEL(ch) >= 60){
		if (GET_XP_GIORNALIERI(ch) > (titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch) + 1].exp - titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp) / 5){
			GET_XP_GIORNALIERI(ch) = 0;
			mag_affects (70, ch, ch, DISEASE_PIAGHE, 0);
		}
	}
	else if (GET_LEVEL(ch) >= 50){
		if (GET_XP_GIORNALIERI(ch) > (titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch) + 1].exp - titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp) / 3){
			GET_XP_GIORNALIERI(ch) = 0;
			mag_affects (70, ch, ch, DISEASE_PIAGHE, 0);
		}
	}
	else if (GET_LEVEL(ch) >= 20){
		if (GET_XP_GIORNALIERI(ch) > (titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch) + 1].exp - titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp) / 2){
			GET_XP_GIORNALIERI(ch) = 0;
			mag_affects (70, ch, ch, DISEASE_PIAGHE, 0);
		}
	}
	
	if (gain > 0)
	{
		if (GET_LEVEL(ch) < LVL_IMMORT)
			gain = MIN(gain, (titles[(int) GET_CLASS(ch)][GET_LEVEL(ch)+2].exp - GET_EXP(ch) - 1));
		GET_EXP(ch) += gain;
		while (GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= titles[(int) GET_CLASS(ch)][GET_LEVEL(ch) + 1].exp)
		{
			GET_LEVEL(ch) += 1;
			num_levels++;
			advance_level(ch);
			is_altered = TRUE;
		}
		
		if (is_altered)
		{
			if (num_levels == 1)
			{
				if (GET_LEVEL(ch)>=20 && GET_CLASS_TITLE(ch) == 0) {
					send_to_char("&2Congratulazioni!!! Ora sei pronto a narrare la tua storia&0!!SOUND(level1.wav L=1 p=1)\r\n", ch);
				}
				
				if (GET_LEVEL(ch)>=40 && GET_CLASS_TITLE(ch) <= 1) {
					send_to_char("&2Congratulazioni!!! Ora sei pronto a dimostrare il tuo valore presso la tua gilda!&0!!(SOUND(level2.wav L=1 P=1)\r\n", ch);
        }
			}
			
			else
			{
				sprintf(buf, "Aumenti %d livelli!\r\n", num_levels);
				send_to_char(buf, ch);
				log("Aumento di piu' di un livello IRREGOLARE in gain_exp!");
			}
			set_title(ch, NULL);
			check_autowiz(ch);
		}
	}
	else if (gain < 0)
	{
		gain = MAX(-max_exp_loss, gain);	/* Cap max exp lost per death */
		GET_EXP(ch) += gain;
		if (GET_EXP(ch) < 0)
			GET_EXP(ch) = 0;
	}
	return gain;
}


void gain_exp_regardless(struct char_data * ch, int gain)
{
	int is_altered = FALSE;
	int num_levels = 0;
	
	GET_EXP(ch) += gain;
	if (GET_EXP(ch) < 0)
		GET_EXP(ch) = 0;
	
	if (!IS_NPC(ch)) {
		while (GET_LEVEL(ch) < LVL_IMPL &&
			GET_EXP(ch) >= titles[(int) GET_CLASS(ch)][GET_LEVEL(ch) + 1].exp) {
			GET_LEVEL(ch) += 1;
			num_levels++;
			advance_level(ch);
			is_altered = TRUE;
		}
		
		if (is_altered) {
			if (num_levels == 1)
				send_to_char("You rise a level!\r\n", ch);
			else {
				sprintf(buf, "You rise %d levels!\r\n", num_levels);
				send_to_char(buf, ch);
			}
			set_title(ch, NULL);
			check_autowiz(ch);
		}
	}
}

int check_condition(struct char_data * ch,int condition)
{
	int ret=0;
	int status;
	
	if (!ch) return 0;
	status=GET_COND(ch,condition);
	if (status==-1)
		return 0;
	if(status<=48)
		ret= LIGHT;
	if(status!=48){
		if (status<=36)
			ret=COND;
		if(status<=24)
			ret=HEAVY;
		if(status<=12){
      /* if((condition==FULL)&&(GET_HIT(ch)==GET_MAX_HIT(ch))&&(!GET_POINTS_EVENT(ch,1)))
	 alter_hit(ch,1);*/
			if((condition==THIRST)&&(GET_MANA(ch)==GET_MAX_MANA(ch)))
				alter_mana(ch,1);
			ret=TOTAL;
		}
	}
	return ret;
}

void gain_condition(struct char_data * ch, int condition, int value)
{
	bool intoxicated;
	int status,old_cond;
	
	/*if (!ch) return;*/
	
	old_cond=GET_COND(ch, condition);
	
	if (old_cond == -1)	/* No change */		
		return;
	
	intoxicated = (GET_COND(ch, DRUNK) > 0);
	old_cond+=value;
	GET_COND(ch, condition) =MIN(96,old_cond);
	
  /* update regen rates if we were just on empty */
	if ((condition != DRUNK) && (value > 0) &&
		(GET_COND(ch, condition) == value))
		check_regen_rates(ch);
	
	GET_COND(ch, condition) = MAX(0, GET_COND(ch, condition));
	if(condition!=DRUNK)
		GET_COND(ch, condition) = MIN(96, GET_COND(ch, condition));
	
	if (PLR_FLAGGED(ch, PLR_WRITING))
		return;
	
	switch (condition) {
		case FULL:
			if((status=check_condition(ch,FULL))!=0){
				sprintf(buf,hunger_mess[status]);
				send_to_char(buf,ch);
			}
			break;
		case THIRST:
			if((status=check_condition(ch,THIRST))!=0){
				sprintf(buf,thirsty_mess[status]);
				send_to_char(buf,ch);
			}
			break;
		case DRUNK:
			if (intoxicated)
				send_to_char("Ora sei sobrio.\r\n", ch);
			return;
		default:
			break;
	}
	
}


void check_idling(struct char_data * ch)
{
	extern int free_rent;
	extern void unstow_char(struct char_data *ch);
	void Crash_rentsave(struct char_data *ch, int cost);
	
	if (++(ch->char_specials.timer) > 8) {
		if (GET_WAS_IN(ch) == NOWHERE && ch->in_room != NOWHERE) {
			GET_WAS_IN(ch) = ch->in_room;
			if (FIGHTING(ch)) { 
				stop_fighting(FIGHTING(ch));
				stop_fighting(ch);
			}
			act("$n viene risucchiato dal nulla!", TRUE, ch, 0, 0, TO_ROOM);
			send_to_char("Vieni risucchiato dal nulla!\r\n", ch);
			if (TRANSPORTED_BY(ch))
				unstow_char(TRANSPORTED_BY(ch));	  
			if (RIDING(ch)) 
			  dismount_char(ch);
			save_char(ch, NOWHERE);
			Crash_crashsave(ch);
			char_from_room(ch);
			char_to_room(ch, 1);
		} 
		else if (ch->char_specials.timer > 48) {
			if (ch->in_room != NOWHERE)
				char_from_room(ch);
			char_to_room(ch, 3);
			if (ch->desc)
				close_socket(ch->desc);
			ch->desc = NULL;
			if (free_rent)
				Crash_rentsave(ch, 0);
			else
				Crash_idlesave(ch);
			sprintf(buf, "%s force-rented and extracted (idle).", GET_NAME(ch));
			mudlog(buf, CMP, LVL_GOD, TRUE);
			extract_char(ch);
		}
	}
}



/* Update PCs, NPCs, and objects */
void point_update(void)
{
	void update_char_objects(struct char_data * ch);	/* handler.c */
	void extract_obj(struct obj_data * obj);	/* handler.c */
	extern int can_under(struct char_data *ch);
	struct char_data *i, *next_char;
	struct obj_data *j, *next_thing, *jj, *next_thing2;
	bool hunger=FALSE;
	bool poisoned=FALSE;
	bool affogo=FALSE;
	
  /* characters */
	for (i = character_list; i; i = next_char) {
		next_char = i->next;
		if (!ROOM_FLAGGED(IN_ROOM(i),ROOM_PRISON)) {
			hunger=FALSE;
			poisoned=FALSE; 
			affogo=FALSE;
			//PEPPE FAME
			if ((!newbye(i)) && (!PLR_FLAGGED(i,PLR_FANTASMA)) && (PRF_FLAGGED(i,PRF_RPG))){
			    gain_condition(i, FULL, -1);
			    gain_condition(i, THIRST, -1);
			}
			if (!(affected_by_spell(i, SPELLSKILL, DISEASE_BACCO))){
				gain_condition(i, DRUNK, -1);
				if (GET_COND(i, DRUNK) > 4 && !(number(0, 2))){
					mag_affects (70, i, i, DISEASE_BACCO, 0);
					send_to_char ("La tua ubriachezza e' diventata ormai patologica... dovrai trovare una cura o continuerai a barcollare e mancare ogni nemico!\r\n", i);
				}
			}
			if(!IS_NPC(i)&&GET_COND(i,FULL)<=24&&GET_COND(i,FULL)!=-1)
				hunger=TRUE;
			if (IS_AFFECTED(i, AFF_POISON))
				poisoned=TRUE;
			
			if (((i->in_room) != NOWHERE) && (SECT(i->in_room) == SECT_UNDERWATER) && (GET_LEVEL(i) < LVL_IMMORT) && !can_under(i)) 
				affogo=TRUE;
			
			//Chardhros - Morte Cavalcature Affamate
			if (MOB_FLAGGED(i, MOB_MOUNTABLE) && GET_COND(i,FULL)==0) {
				damage(i, i, 999, TYPE_HUNGER);
			}
			
			if(GET_POS(i)>=POS_STUNNED){
				if(hunger)
					damage(i, i, 3, TYPE_HUNGER);
				if(poisoned)
					damage(i, i, 15, SPELL_POISON);
				if(affogo)
					damage(i, i, 15, -1);   
				
			}
			else if(GET_POS(i)<=POS_INCAP&&GET_POS(i)!=POS_DEAD){
				if(poisoned)
					damage(i, i, 15, SPELL_POISON);
				else if(hunger)
					damage(i, i, 5, TYPE_HUNGER);
				else
					damage(i, i, 3, TYPE_SUFFERING);
			}
			//update_pos(i);
			
			if (!IS_NPC(i)) {
				update_char_objects(i);
				if (GET_LEVEL(i) < LVL_QUEST_MASTER)
					check_idling(i);
			}
		}
	}
	
	
  /* objects */
	for (j = object_list; j; j = next_thing) {
		next_thing = j->next;	/* Next in object list */
		
    /* If this is a corpse */
		if ((GET_OBJ_TYPE(j) == ITEM_CONTAINER) && GET_OBJ_VAL(j, 3)) {
			
      /* timer count down */
			if (GET_OBJ_TIMER(j) > 0)
				GET_OBJ_TIMER(j)--;
			
			if (!GET_OBJ_TIMER(j)) {
				
				if (j->carried_by)
					act("$p decays in your hands.", FALSE, j->carried_by, j, 0, TO_CHAR);
				else if ((j->in_room != NOWHERE) && (world[j->in_room].people)) {
					act("Una disgustosa orda di vermi consuma $p.",
						TRUE, world[j->in_room].people, j, 0, TO_ROOM);
					act("Una disgustosa orda di vermi consuma $p.",
						TRUE, world[j->in_room].people, j, 0, TO_CHAR);
				}

				for (jj = j->contains; jj; jj = next_thing2) {
					next_thing2 = jj->next_content;	/* Next in inventory */
					obj_from_obj(jj);
					
					if (j->in_obj)
						obj_to_obj(jj, j->in_obj);
					else if (j->carried_by)
						obj_to_room(jj, j->carried_by->in_room);
					else if (j->in_room != NOWHERE) {
						if (!ROOM_FLAGGED(j->in_room,ROOM_HOUSE)) {
							obj_to_room(jj, j->in_room);
						} else {
							obj_to_room(jj, j->in_room);
							extract_obj(jj);
						}
					}
					else
						assert(FALSE);
				}
				extract_obj(j);
			}	  
			
		}
		
		
		else if (GET_OBJ_TYPE(j) == ITEM_PORTAL){	//Orione
			if (GET_OBJ_TIMER(j) > 0)
				GET_OBJ_TIMER(j)--;
			if (GET_OBJ_TIMER(j) == 1)
			{
				if ((j->in_room != NOWHERE) &&(world[j->in_room].people)) {
					act("$p comincia a svanire!",
						FALSE, world[j->in_room].people, j, 0, TO_ROOM);
					act("$p comincia a svanire!",
						FALSE, world[j->in_room].people, j, 0, TO_CHAR);
				}
			}
			
			if (GET_OBJ_TIMER(j) == 0)
			{
				if ((j->in_room != NOWHERE) &&(world[j->in_room].people)) {
					act("$p scompare in una nuvola di fumo!",
						FALSE, world[j->in_room].people, j, 0, TO_ROOM);
					act("$p scompare in una nuvola di fumo!",
						FALSE, world[j->in_room].people, j, 0, TO_CHAR);
				}
				extract_obj(j);
			}
		}
		
		
		else if  ((GET_OBJ_TYPE(j) == ITEM_TRAPPOLA)&&(GET_TRAP_TIPO(j) == TRAP_SCATTATA)) {	//Orione
			if (GET_OBJ_TIMER(j) > 0)
				GET_OBJ_TIMER(j)--;
			if (GET_OBJ_TIMER(j) == 0) {	
				if ((j->in_room != NOWHERE) &&(world[j->in_room].people)) {
					act("$p si disfa sotto i tuoi occhi!",
						FALSE, world[j->in_room].people, j, 0, TO_ROOM);
					act("$p si disfa sotto i tuoi occhi!",
						FALSE, world[j->in_room].people, j, 0, TO_CHAR);
				}
				extract_obj(j);	
			} 
		}

		else if (GET_OBJ_TYPE(j) == ITEM_TISANA_CALDA){    //By Spini
			if (GET_OBJ_TIMER(j) > 0)
				GET_OBJ_TIMER(j)--;
			else{
				if (j->carried_by)
					act("$p si raffredda diventando inutilizzabile.", FALSE, j->carried_by, j, 0, TO_CHAR);
				else if ((j->in_room != NOWHERE) && (world[j->in_room].people)) {
					act("$p e' rimasta ferma per troppo tempo. Ormai non curerebbe piu' nessuno!",
						TRUE, world[j->in_room].people, j, 0, TO_ROOM);
					act("$p e' rimasta ferma per troppo tempo. Ormai non curerebbe piu' nessuno!",
						TRUE, world[j->in_room].people, j, 0, TO_CHAR);
				}

				sprintf (buf, "una tisana raffreddata");
				j->short_description = str_dup(buf);
				sprintf (buf, "Una tisana ormai fredda ed inutile e' stata abbandonata qui a terra.");
				j->short_description = str_dup(buf);
				GET_OBJ_TYPE(j) = ITEM_TISANA_FREDDA;
			}
		}

		//Orione Inizio
		/* If the timer is set, count it down and at 0, try the trigger */
		/* note to .rej hand-patchers: make this last in your point-update() */
		else if (GET_OBJ_TIMER(j)>0) {
			GET_OBJ_TIMER(j)--;
			if (!GET_OBJ_TIMER(j))
				timer_otrigger(j);
			}
		//Orione Fine
	}
}


void damage_update(void){
	
    struct char_data *ch, *next_char;
    int dam;

    for (ch = character_list; ch; ch = next_char) {
	next_char = ch->next;
	//PEPPE DAMAGE
	dam=GET_LEVEL(ch)/2;
	//prova PEPPE
	if(!ch->in_room)
	    continue;
	
	if(ROOM_FLAGGED(ch->in_room,ROOM_DAMAGE)){
		        if(GET_LEVEL(ch)<LVL_AVATAR){
			if((AFF_FLAGGED(ch,AFF_PROT_FIRE)) || (AFF_FLAGGED(ch,AFF_FIRESHD)))
			    dam=dam/2;
			send_to_char("Il calore che emana questa stanza ti sta dannegiando\r\n",ch);    
			damage(ch,ch,dam,TYPE_ROOM_DAMAGE_FUOCO);
			}
			continue;
	}
	if(ROOM_FLAGGED(ch->in_room,ROOM_DAMAGE_FUOCO)){
		        if(GET_LEVEL(ch)<LVL_AVATAR){
			if((AFF_FLAGGED(ch,AFF_PROT_FIRE)) || (AFF_FLAGGED(ch,AFF_FIRESHD)))
			    dam=dam/2;
			send_to_char("Il calore che emana questa stanza ti sta dannegiando\r\n",ch);    
			damage(ch,ch,dam,TYPE_ROOM_DAMAGE_FUOCO);
			}
			continue;
			
	}
	if(ROOM_FLAGGED(ch->in_room,ROOM_DAMAGE_ACQUA)){
		        if(GET_LEVEL(ch)<LVL_AVATAR){
			if(AFF_FLAGGED(ch,AFF_WATERBREATH))
			    dam=dam/2;
			send_to_char("Non riesci a respirare.\r\n",ch);
			damage(ch,ch,dam,TYPE_ROOM_DAMAGE_ACQUA);
			}
			continue;
	}
	
	if(ROOM_FLAGGED(ch->in_room,ROOM_DAMAGE_ARIA)){
		        if(GET_LEVEL(ch)<LVL_AVATAR){
			if(AFF_FLAGGED(ch,AFF_FLYING))
			    dam=dam/2;
			send_to_char("Vieni sballottato qua e la da violente raffiche di vento\r\n",ch);    
			damage(ch,ch,dam,TYPE_ROOM_DAMAGE_ARIA);
			}
			continue;
	}
	if(ROOM_FLAGGED(ch->in_room,ROOM_DAMAGE_TERRA)){
		        if(GET_LEVEL(ch)<LVL_AVATAR){
			damage(ch,ch,dam,TYPE_ROOM_DAMAGE_TERRA);
			}
			continue;
	}
    }

}

void gain_fiato(void){
	
    struct char_data *ch, *next_char;
    int fiato_max;

    for (ch = character_list; ch; ch = next_char) {
	next_char = ch->next;
	fiato_max=((GET_CON(ch)+GET_CON(ch)+GET_DEX(ch))/4 + GET_MOVE(ch)/15);
	if(AFF_FLAGGED(ch,AFF_ENDURANCE))
				fiato_max=fiato_max*2;
	if(GET_FIATO(ch)>=fiato_max || fiato_max-GET_FIATO(ch)<5)
	    GET_FIATO(ch)=fiato_max;
	else
	GET_FIATO(ch)=GET_FIATO(ch)+5;
    }	
}


/*
 * By Spini:
 * Se sei affetto dalla maledizione di Shaarr ogni tanto ti becchi un
 * critico casuale. Lo so, e' brutto non usare le costanti, ma sarebbe
 * stato incredibilmente piu' lungo, e cosi' e' piu' snello! Comunque
 * l'importante e' che funzioni... 44 e' il numero di AFF_CRITICAL_TESTA_1,
 * il piu' basso, e 43 + 18 = 61 e' quello di AFF_CRITICAL_GAMBA_S_3 che
 * e' l'ultimo.
 */
void ferite_shaarr_update(void){
	struct char_data *ch;
	long long int posizione;

	for (ch = character_list; ch; ch = ch->next){
		if (IS_NPC(ch))
			continue;
		if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
			continue;
		if (PLR_FLAGGED(ch, PLR_FANTASMA))
			continue;
		if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON))
			continue;
		if (affected_by_spell (ch, SPELLSKILL, DISEASE_SHAARR))
			if (!(number(0, 13))){
				posizione = ((long long int) 1 << (43 + number (1, 18)));
				if (!(AFFECTED(ch, (long long int) posizione, 0)) || !AFFECTED(ch, AFF_FERITO, 0)){
					act ("&1Shaarr, sdegnata dalla tua debolezza sul campo di battaglia, infierisce sul tuo corpo con dolorose ferite!&0", FALSE, ch, 0, 0, TO_CHAR);
					SET_BIT (AFF_FLAGS(ch), (long long int) posizione);
					SET_BIT (AFF_FLAGS(ch), AFF_FERITO);
				}
			}
	}
}


void check_raffreddore(void){
	struct char_data *ch;
	ACMD(do_action);

	for (ch = character_list; ch; ch = ch->next){
		if (IS_NPC(ch))
			continue;
		if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
			continue;
		if (PLR_FLAGGED(ch, PLR_FANTASMA))
			continue;
		if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON))
			continue;
		if (affected_by_spell (ch, SPELLSKILL, DISEASE_RAFFREDDORE))
			if (!(number(0, 4)))
				do_action (ch, "starnutisci", find_command("starnutisci"), 0);
	}
}


void check_male_antichi(void){
	struct char_data *ch;

	for (ch = character_list; ch; ch = ch->next){
		if (IS_NPC(ch))
			continue;
		if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
			continue;
		if (PLR_FLAGGED(ch, PLR_FANTASMA))
			continue;
		if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON))
			continue;
		if (affected_by_spell (ch, SPELLSKILL, DISEASE_XHYPHYS))
			if (!(number(0, 2))){
				spell_teleport(70, ch, ch, NULL);
				act ("&5Il potere del caos e' immenso! Tienilo sempre in mente...&0", FALSE, ch, 0, 0, TO_CHAR);
			}
	}
}


void check_malattia_morfeo(void){
	struct char_data *ch;
	struct affected_type *af;

	for (ch = character_list; ch; ch = ch->next){
		if (IS_NPC(ch))
			continue;
		if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
			continue;
		if (PLR_FLAGGED(ch, PLR_FANTASMA))
			continue;
		if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON))
			continue;
		if (affected_by_spell (ch, SPELLSKILL, DISEASE_MORFEO)){
			GET_SENZA_DORMIRE(ch) = 0;
			if (!(number(0, 360)) && !AFFECTED(ch, AFF_SLEEP, BITV_SLEEP)){
				CREATE (af, struct affected_type, 1);
				af->natura = SPELLSKILL;
				af->type = DISEASE_MORFEO;
				af->duration = 2;
				af->modifier = 0;
				af->location = 0;
				af->bitvector[0] = af->bitvector[1] = af->bitvector[2] = af->bitvector[3] = 0;
				af->bitvector[BITV_SLEEP] = AFF_SLEEP;
				af->next = NULL;

				act ("&7&bMorfeo prende il sopravvento su di te. Non puoi pensare ad altro che dormire...&0", FALSE, ch, 0, 0, TO_CHAR);
				GET_POS(ch) = POS_SLEEPING;
				affect_to_char (ch, af);
			}
		}
		else{
			if (GET_POS(ch) == POS_SLEEPING)
				GET_SENZA_DORMIRE(ch) = 0;
		/* Il "400000 -" dovrebbe essere inutile, ma con questo random non si sa mai... */
		//	if ((400000 - number(0, 400000)) < sqrt (GET_SENZA_DORMIRE(ch)))
		//		mag_affects (70, ch, ch, DISEASE_MORFEO, 0);
		}
	}
}


void check_peste(void){
	void raw_kill (struct char_data *ch, struct char_data *killer);
	struct char_data *ch, *tch;
	struct affected_type *af, *next;

	for (ch = character_list; ch; ch = ch->next) {
		if (IS_NPC(ch))
			continue;
		if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
			continue;
		if (PLR_FLAGGED(ch, PLR_FANTASMA))
			continue;
		if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON))
			continue;
		if (!(affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE)))
			continue;
		for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
			if (affected_by_spell (tch, SPELLSKILL, DISEASE_PESTE) || GET_LEVEL(tch) >= LVL_QUEST_MASTER || IS_NPC(tch))
				continue;
			if (GET_CON(tch) > number(13, 20))
				continue;
			mag_affects(70, ch, tch, DISEASE_PESTE, 0);
		}

		GET_PESTE(ch)++;

		if (!number(0, 2400)){
			act ("&1&bQuando iniziavi ad abituarti ai tuoi brutti bubboni vedi che improvvisamente avvizziscono... ti stai guarendo dalla peste!&0", FALSE, ch, 0, 0, TO_CHAR);
			act ("&1&bI bubboni sulla pelle di $n avvizziscono improvvisamente!&0", FALSE, ch, 0, 0, TO_ROOM);
			affect_from_char (ch, SPELLSKILL, DISEASE_PESTE);
		}

		if (GET_PESTE(ch) == 240){
			SET_BIT(PLR_FLAGS(ch),PLR_FANTASMA);
			SET_BIT(PRF_FLAGS(ch),PRF_NOHASSLE);
			SET_BIT(PRF_FLAGS(ch),PRF_HOLYLIGHT);
			GET_MOVE(ch)=GET_MAX_MOVE(ch);
			GET_MANA(ch)=GET_MAX_MANA(ch);
			GET_RESUSCITA(ch)=0;
			GET_RIP_CNT(ch) += 1;
			for (af = ch->affected; af; af = next){
				next = af->next;
				if (af->type > MAX_SKILLS && af->type < TOP_SPELL_DEFINE)
				affect_remove(ch, af);
			}
			raw_kill(ch, NULL);
			continue;
		}
		else if (GET_PESTE(ch) >= 120)
			alter_hit(ch, GET_HIT(ch) / (240 - GET_PESTE(ch)));
	}
}


void chill_burn_update(void){
	struct char_data *ch;

	for (ch = character_list; ch; ch = ch->next) {
		if (AFFECTED(ch, AFF_BURNED, BITV_BURNED)){
			alter_hit (ch, MIN (MAX (1, GET_MAX_HIT(ch) / 50), GET_HIT(ch) - 1));
			send_to_char ("&1La tua pelle brucia causandoti grandi sofferenze.&0\r\n", ch);
		}
		else if (AFFECTED(ch, AFF_CHILLED, BITV_CHILLED)){
			alter_hit (ch, MIN (MAX (1, GET_MAX_HIT(ch) / 50), GET_HIT(ch) - 1));
			send_to_char ("&6&bLa morsa di gelo che ti stringe ti fa provare un grande dolore.&0\r\n", ch);
		}
	}
}


void ferite_update(void){
	
    struct char_data *ch, *next_char;
    int ferite=0;

    for (ch = character_list; ch; ch = next_char) {
	next_char = ch->next;
	ferite=0;
	if(AFF_FLAGGED(ch,AFF_FERITO)){
	    if(AFF_FLAGGED(ch,AFF_CRITICAL_TESTA_3))
		ferite +=6;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_TESTA_2))
		ferite +=4;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_TESTA_1))
		ferite +=2;
	    if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3))
		ferite +=6;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_2))
		ferite +=4;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_1))
		ferite +=2;
	    if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3))
		ferite +=6;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_2))
		ferite +=4;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_1))
		ferite +=2;
	    if(AFF_FLAGGED(ch,AFF_CRITICAL_TORSO_3))
		ferite +=6;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_TORSO_2))
		ferite +=4;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_TORSO_1))
		ferite +=2;
	    if(AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_3))
		ferite +=6;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_2))
		ferite +=4;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_1))
		ferite +=2;			
	    if(AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_3))
		ferite +=6;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_2))
		ferite +=4;
	    else if(AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_1))
		ferite +=2;
	    if(GET_HIT(ch)-ferite < 10){
		send_to_char("&8&7Le tue ferite non perdono piu sangue,ma sarebbe bene farci dare un occhiata.&0\r\n",ch);
		}
	    else{ 
		GET_HIT(ch)-=ferite;
		if(ferite<=10)
		send_to_char("&8&1Perdi sangue da varie ferite.&0\r\n",ch);
		else if(ferite<=20)
		send_to_char("&8&1Perdi molto sangue dalle varie ferite che hai accumulato durante i passati scontri.&0\r\n",ch);
		else if(ferite<=30)
		send_to_char("&8&1Hai delle ferite molto profonde,sarebbe meglio rivolgerti ad un curatore o morirai dissanguato.&0\r\n",ch);
	    } 	
	}		
    }				
}


int limite_follower(struct char_data *ch) {
	struct follow_type *k;
	int i=0;
	
	if (GET_LEVEL(ch)>=LVL_IMMORT)
		return FALSE;
	
	for (k = ch->followers; k; k = k->next)
		if (IS_NPC(k->follower))
			i++;
		
	if (GET_CLASS(ch) == CLASS_MAGIC_USER) {
		if (GET_CHA(ch)/2 < i+1)
			return TRUE;
	} 
	else if (GET_CHA(ch)/4 < i+1)
		return TRUE;
	
	return FALSE;
}



void stipendio_mob(void) {
	
	int i;
	struct char_data *mob;
	
	mudlog("- Mob Pay Start-", CMP, LVL_GOD, FALSE);
	for (mob = character_list; mob; mob = mob->next){
		if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&MOB_FLAGGED(mob,MOB_STIPENDIO)){
			if((MASTER_ID(mob)>0) &&(GET_STIPENDIOMOB(mob))>0){				
				GET_PAGA(mob) -= 1;
				SET_BIT(MOB_FLAGS(mob), MOB_CRASH);
				if (GET_PAGA(mob)==0) {
					if (GET_BANK_GOLD(mob)<get_stipendiomob(mob)) {
						sprintf(buf,"Sei in &1ritardo&0 con il pagamento dello stipendio\r\ndi %s.\r\n",GET_NAME(mob));
						store_mail(MASTER_ID(mob),MAIL_SINDACATO,buf);
					} else {
						GET_BANK_GOLD(mob) -= get_stipendiomob(mob);
						GET_PAGA(mob) += ORE_PAGA;
					}
				} else if ( (GET_PAGA(mob)<0) && ( GET_PAGA(mob) >= (-LICENZIAMENTO_MANOVALE) )) {
					if (GET_BANK_GOLD(mob) >= get_stipendiomob(mob)) {
						GET_BANK_GOLD(mob) -= get_stipendiomob(mob);
						GET_PAGA(mob) += ORE_PAGA;
					}
				} else if (GET_PAGA(mob)< (-LICENZIAMENTO_MANOVALE)) {
					if (GET_BANK_GOLD(mob) < get_stipendiomob(mob)) {
						sprintf(buf,"A causa di ritardi nel pagamento dello stipendio\r\n"
							"%s ha presentato le &1DIMISSIONI&0.\r\n",GET_NAME(mob));
						store_mail(MASTER_ID(mob),MAIL_SINDACATO,buf);
						
						act("Vedi $n abbastanza scocciato allontanarsi.", FALSE, mob, 0, 0, TO_ROOM);
						
					// Elimina gli obj del manovale
						for (i = 0; i < NUM_WEARS; i++)
							if (GET_EQ(mob, i))
								extract_obj(unequip_char(mob, i));
							
						while (mob->carrying) 
							extract_obj(mob->carrying);
						
					//Elimina il Mob
						extract_char(mob);
					} else {
						GET_BANK_GOLD(mob) -= get_stipendiomob(mob);
						GET_PAGA(mob) += ORE_PAGA;
					}
				}
				
			}//if
			
		} //if
	}//for
	mudlog("- Mob Pay End -", CMP, LVL_GOD, FALSE);	
}

void calculate_eq_tax(struct obj_data * obj, int *cost)
{
	if (obj) {
		if (GET_OBJ_TYPE(obj)!=ITEM_CONTAINER)
			*cost += MAX(1, (GET_OBJ_COST(obj)*PERCENTUALE_EQ)/100);
		
		calculate_eq_tax(obj->contains, cost);
		calculate_eq_tax(obj->next_content, cost);
	}
}


int get_stipendiomob(struct char_data *ch) {
	int j,tassa_equip = 0;
	
	if (!IS_NPC(ch))
		return 0;
	
	if (GET_MASTER(ch)) 
		if ((GET_IDNUM(GET_MASTER(ch))==MASTER_ID(ch))&&(IN_ROOM(GET_MASTER(ch))==IN_ROOM(ch)) )
			return GET_STIPENDIOMOB(ch);
		
	if (!MOB_FLAGGED(ch,MOB_STIPENDIO_EQ))
		return GET_STIPENDIOMOB(ch);	
	
	calculate_eq_tax(ch->carrying,&tassa_equip);
	
	for (j = 0; j < NUM_WEARS; j++)
		calculate_eq_tax(GET_EQ(ch,j), &tassa_equip);
	
	return (GET_STIPENDIOMOB(ch) + tassa_equip);
}
