/**************************************************************************
*   File: fight.c          50                           Part of CircleMUD * 
*  Usage: Combat system                                                   *
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
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "spells.h"
#include "screen.h"
#include "dg_scripts.h"
#include "arena.h"
#include "clan.h"
#include "abilita.h"
#include "wilderness.h"

/* Variables */
FILE *crps_index;
int corpse_maxnum;

/* Structures */
struct char_data *combat_list = NULL;	/* head of l-list of fighting chars */
struct char_data *char_list = NULL; 
struct char_data *next_combat_list = NULL;
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);
void brag(struct char_data * ch, struct char_data * victim);
void junktalismano (struct char_data * ch);
extern int Ascii_Obj_to_store_from(struct obj_data * obj, FILE * fl, int locate);

/* External structures */
extern struct room_data *world;
extern struct message_list fight_messages[MAX_MESSAGES];
extern struct obj_data *object_list;
extern int pk_allowed;		     /* see config.c */
extern int brag_on;		     /* see config.c */
extern int auto_save;		     /* see config.c */
extern int max_exp_solo_gain;	     /* see config.c */
extern int max_exp_group_gain;       /* see config.c */
extern int max_exp_loss;	     /* see config.c */
extern char *dirs[];		     /* see constants.c */
extern int rev_dir[];		     /* see constants.c */
extern struct index_data *mob_index;
extern struct clan_rec clan[MAX_CLANS];
extern struct abil_info_type abil_info[];
extern int newbye(struct char_data *ch);
extern int find_flag(struct char_data *vict, int regno, int FLAG);
extern void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia);

/* External procedures */
void check_danno_oggetto (struct char_data * ch, struct obj_data * obj, float modif);
double potenza(double x, int y);
char *fread_action(FILE * fl, int nr);
char *fread_string(FILE * fl, char *error);
void stop_follower(struct char_data * ch);
ACMD(do_flee);
void do_stopfight(struct char_data * ch);
void critical_hit(struct char_data * ch, struct char_data * victim,int location);
void hit(struct char_data * ch, struct char_data * victim, int type);
void forget(struct char_data * ch, struct char_data * victim);
void remember(struct char_data * ch, struct char_data * victim);
int ok_damage_shopkeeper(struct char_data * ch, struct char_data * victim);
void arena_kill(struct char_data * ch);
void store_mail(long to, long from, char *message_pointer);
bool fire_at_char(struct char_data *ch, struct char_data * list, struct obj_data * obj, int dir);
void group_gain(struct char_data * ch, struct char_data * victim, struct char_data * tank);
void solo_gain(struct char_data * ch, struct char_data * victim);
void unstow_char(struct char_data *ch);
int check_extra_dam(struct char_data *ch,struct char_data *vict,int obj_type);
int check_extra_abil(struct char_data *ch,struct char_data *vict,int obj_type);
int check_gen_abil(struct char_data *ch,int abilita);
int notorieta_reward(struct char_data *ch,struct char_data *victim);
void make_resurces(struct char_data *ch);
void generic_unstow(struct char_data *ch);
void new_group_gain(struct char_data * ch, struct char_data * victim, struct char_data * tank);
void consigli_esperienza(struct char_data * ch,int exp);
int char_affected_by_obj (struct char_data *ch, long long int affect, int bitv);

SPECIAL(cleric);
SPECIAL(magic_user);
SPECIAL(warrior);
SPECIAL(thief);
/* Weapon attack texts */
struct attack_hit_type attack_hit_text[] =
{
	{ "hit", "urt","ALTRO"},		/* 0*/
	{ "sting", "pungol","PUGNALE(1)"},
	{ "whip", "frust","FRUSTA"},
	{ "slash", "squarci","ASCIA(1)"},
	{ "bite", "azzann","ZANNE"},
	{ "bludgeon", "randell","MAZZA(1)"},	/* 5*/
	{ "crush", "frantum","MAZZA(2)"},
	{ "pound", "trapass","LANCIA(1)"},
	{ "claw", "artigli","ARTIGLI"},
	{ "maul", "pest","MAZZA(3)"},
	{ "thrash", "lacer","SPADA(1)"},	/* 10*/
	{ "pierce", "infilz","LANCIA(2)"},
	{ "blast", "sferz","SPADA(2)"},
	{ "punch", "picchi","PUGNI"},
	{ "stab", "pugnal","PUGNALE(2)"},
	{ "slay", "massacr","ASCIA(2)"}		/* 15*/
};

void do_dream( struct char_data *victim,int kotime){
	struct affected_type af[1];
	af[0].natura =TYPE_UNDEFINED;
	af[0].type = TYPE_UNDEFINED;
	af[0].bitvector[0] = AFF_TRAMORTITO;
	af[0].bitvector[1] = 0;
	af[0].bitvector[2] = 0;
	af[0].bitvector[3] = 0;
	af[0].modifier = 0;
	af[0].location = APPLY_AGE;
	af[0].duration = kotime-1;
	affect_to_char(victim, af);
}

/*
 * By Spini:
 * non potendo sopportare di mettere in ogni funzione offensiva tutti
 * i controlli, in tutte le medesime funzioni metto questa funzione in
 * cui si fanno i controlli che hanno tutti. Se un controllo risulta
 * positivo (come l'antidoping) ritorna 1 e la funzione chiamante deve
 * interrompersi
 */
int controllo_scontro (struct char_data *ch, struct char_data *vict)
{
	if (controllo_volo(ch, vict))
		return 1;

	if (affected_by_spell (ch, SPELLSKILL, DISEASE_PESTE)){
		send_to_char ("Il tuo corpo devastato dalla peste non potrebbe anche reggere uno scontro fisico!\r\n", ch);
		return 1;
	}
	
	if(MOB_FLAGGED(vict, MOB_ESERCITO)) {
		send_to_char("Come puoi solamente sperare di combattere contro un INTERO Esercito?!?\r\n", ch);
		return 1;
	}

	return 0;
}

int controllo_volo (struct char_data *ch, struct char_data *vict)
{
	if (FIGHTING(ch) && FIGHTING (ch) == vict)
		return 0;
	if (FIGHTING(vict) && FIGHTING(vict) == ch)
		return 0;

	if (!(AFFECTED(vict, AFF_FLYING, BITV_FLYING) || AFFECTED(vict, AFF_LEVITATE, BITV_LEVITATE))){
		if (AFFECTED(ch, AFF_FLYING, BITV_FLYING) || AFFECTED (ch, AFF_LEVITATE, BITV_LEVITATE)){
			if (char_affected_by_obj (ch, AFF_FLYING, BITV_FLYING) || char_affected_by_obj (ch, AFF_LEVITATE, BITV_LEVITATE)){
				send_to_char ("Qualcosa che indossi ti impedisce di atterrare per attaccarlo.\r\n", ch);
				return 1;
			}
			send_to_char ("Sei costretto ad atterrare per colpirlo.\r\n", ch);
			if (affected_by_spell(ch, SPELLSKILL, SPELL_FLY))
				affect_from_char(ch, SPELLSKILL, SPELL_FLY);
			if (affected_by_spell(ch, SPELLSKILL, SPELL_LEVITATE))
				affect_from_char(ch, SPELLSKILL, SPELL_LEVITATE);
		}
		return 0;
	}

	if (AFFECTED(ch, AFF_FLYING, BITV_FLYING) || AFFECTED(ch, AFF_LEVITATE, BITV_LEVITATE))
		return 0;

	act ("Da quaggiu' non riuscirai mai a colpirlo mentre vola!", TRUE, ch, 0, vict, TO_CHAR);
	act ("Osservi dall'alto il patetico tentativo di $n di colpirti.", TRUE, ch, 0, vict, TO_VICT);
	return 1;
}

#define IS_WEAPON(type) (((type) >= TYPE_HIT) && ((type) < TYPE_HUNGER))

/* The Fight related routines */

void appear(struct char_data * ch)
{
	if (affected_by_spell(ch, SPELLSKILL, SPELL_INVISIBLE))
		affect_from_char(ch, SPELLSKILL, SPELL_INVISIBLE);
	
	REMOVE_BIT(AFF_FLAGS(ch), AFF_INVISIBLE | AFF_HIDE);
	
	if (GET_LEVEL(ch) < LVL_IMMORT)
		act("$n compare lentamente davanti a te.\r\n", FALSE, ch, 0, 0, TO_ROOM);
	else
		act("Avverti una strana presenza e poi $n appare davanti a te.\r\n",
			FALSE, ch, 0, 0, TO_ROOM);
}

void load_messages(void)
{
	FILE *fl;
	int i, type;
	struct message_type *messages;
	char chk[128];
	
	if (!(fl = fopen(MESS_FILE, "r"))) {
		sprintf(buf2, "Error reading combat message file %s", MESS_FILE);
		perror(buf2);
		exit(1);
	}
	for (i = 0; i < MAX_MESSAGES; i++) {
		fight_messages[i].a_type = 0;
		fight_messages[i].number_of_attacks = 0;
		fight_messages[i].msg = 0;
	}
	
	
	fgets(chk, 128, fl);
	while (!feof(fl) && (*chk == '\n' || *chk == '*'))
		fgets(chk, 128, fl);
	
	while (*chk == 'M') {
		fgets(chk, 128, fl);
		sscanf(chk, " %d\n", &type);
		for (i = 0; (i < MAX_MESSAGES) && (fight_messages[i].a_type != type) &&
			(fight_messages[i].a_type); i++);
		if (i >= MAX_MESSAGES) {
			fprintf(stderr, "Too many combat messages.  Increase MAX_MESSAGES and recompile.");
			exit(1);
		}
		CREATE(messages, struct message_type, 1);
		fight_messages[i].number_of_attacks++;
		fight_messages[i].a_type = type;
		messages->next = fight_messages[i].msg;
		fight_messages[i].msg = messages;
		
		messages->die_msg.attacker_msg = fread_action(fl, i);
		messages->die_msg.victim_msg = fread_action(fl, i);
		messages->die_msg.room_msg = fread_action(fl, i);
		messages->miss_msg.attacker_msg = fread_action(fl, i);
		messages->miss_msg.victim_msg = fread_action(fl, i);
		messages->miss_msg.room_msg = fread_action(fl, i);
		messages->hit_msg.attacker_msg = fread_action(fl, i);
		messages->hit_msg.victim_msg = fread_action(fl, i);
		messages->hit_msg.room_msg = fread_action(fl, i);
		messages->god_msg.attacker_msg = fread_action(fl, i);
		messages->god_msg.victim_msg = fread_action(fl, i);
		messages->god_msg.room_msg = fread_action(fl, i);
		fgets(chk, 128, fl);
		while (!feof(fl) && (*chk == '\n' || *chk == '*'))
			fgets(chk, 128, fl);
	}
	
	fclose(fl);
}


void update_pos(struct char_data * victim)
{
	struct char_data *mount=NULL;
	if (!AFF_FLAGGED(victim, AFF_TRAMORTITO) && AFF_FLAGGED(victim, AFF_IMMOBIL))
		GET_POS(victim) = POS_SITTING;
	
	if(AFF_FLAGGED(victim, AFF_TRAMORTITO) && RIDING(victim)) {
		if(!TRANSPORTED(RIDING(victim))) {/*quando un cavaliere sviene a cavallo*/
			mount=RIDING(victim);/*diventa trasportato da esso,smette di cavalcarlo*/
			TRANSPORTED(mount)=victim;
			RIDDEN_BY(mount)=NULL;
			sprintf(buf,"(DEBUG) -Mount- Il PG %s e' stato steso in sella a %s.",GET_NAME(victim),GET_NAME(mount));
			log(buf);
		}
	}
	
	if(AFF_FLAGGED(victim, AFF_CAPTURING)){
		if(AFF_FLAGGED(victim, AFF_TRAMORTITO) || !FIGHTING(victim)) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_CAPTURING);
			return;
		}
		if (GET_HIT(victim) <= 0 && (AFF_FLAGGED(victim, AFF_TRAMORTITO) || AFF_FLAGGED(victim, AFF_CAPTURING)) ) {
			GET_POS(victim) = POS_STUNNED;
			GET_HIT(victim) =- 1;
			return;
		}
	}
	
	if (!AFF_FLAGGED(victim, AFF_CAPTURING)) {
		
		if (GET_HIT(victim) > 0) 
		  WHOKILLED(victim) = NULL;
		  
		if ((GET_HIT(victim) > 0) && (GET_POS(victim) > POS_STUNNED))
			return;
		else if (GET_HIT(victim) > 0)
			GET_POS(victim) = POS_STANDING;
		else if (GET_HIT(victim) <= HIT_DEAD)
			GET_POS(victim) = POS_DEAD;
		else if (GET_HIT(victim) <= HIT_MORTALLYW)
			GET_POS(victim) = POS_MORTALLYW;
		else if (GET_HIT(victim) <= HIT_INCAP)
			GET_POS(victim) = POS_INCAP;
		else
			GET_POS(victim) = POS_STUNNED;
	}
}

void change_fama(struct char_data *ch,struct char_data *victim,int amount)
{
	int diff, fama;
	if(!ch||!victim)
		return;
	
	if(!IS_NPC(ch)&&!IS_NPC(victim))
	{
		diff = GET_LEVEL(ch)-GET_LEVEL(victim);
		if      (diff <= -15) fama= 20;
		else if (diff <= -10) fama= 10;
		else if (diff <= -5 ) fama= 5;
		else if (diff <   0 ) fama= 2;
		else if (diff <   5 ) fama= 0;
		else if (diff <   10) fama= -1;
		else                  fama= -2;
		
		GET_FAMA(ch)+=fama;
		GET_FAMA(victim)-=fama;
		GET_FAMA(victim)=MAX(MIN(GET_FAMA(victim),1000),1);
		GET_FAMA(ch)=MAX(MIN(GET_FAMA(ch),1000),1);
	}
}

void change_notorieta(struct char_data *ch,struct char_data *victim, int event, int amount)
{
	if(ch==NULL || IS_NPC(ch))
		return;
	
	switch(event){
		case KILL:
			amount=0;
			if(victim && !IS_NPC(victim)){
				amount = -25;
				if (GET_CLAN(ch)>=0) {
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_KILLER)) amount = 30;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_THIEF)) amount = 10;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_TKILLER)) amount = 15;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_WKILLER)) amount = 30;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_STRAGE)) amount = 35;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_CORROTTO)) amount = 10;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_MULTA)) amount = 5;
					if (!IS_A_CRIMINAL_CLAN(victim, GET_CLAN(ch))) amount = -50;				  
				}
				if(PLR_FLAGGED(victim,PLR_TKILLER)||PLR_FLAGGED(victim,PLR_THIEF)||PLR_FLAGGED(victim,PLR_WAR_KILLER)||PLR_FLAGGED(victim,PLR_KILLER))
					amount=10;
			}
			break;
		case TKILL:
			if(victim && !IS_NPC(victim)){
				amount = -5;
				if (GET_CLAN(ch)>=0) {
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_KILLER)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_THIEF)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_TKILLER)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_WKILLER)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_STRAGE)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_CORROTTO)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_MULTA)) amount=0;
					if (!IS_A_CRIMINAL_CLAN(victim, GET_CLAN(ch))) amount = -25;				  
				}
				if(PLR_FLAGGED(victim,PLR_TKILLER)||PLR_FLAGGED(victim,PLR_THIEF)||PLR_FLAGGED(victim,PLR_WAR_KILLER)||PLR_FLAGGED(victim,PLR_KILLER))
					amount=10;
			}
			break;
		case WKILL:
			if(victim && !IS_NPC(victim)){
				amount = -5;
				if (GET_CLAN(ch)>=0) {
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_KILLER)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_THIEF)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_TKILLER)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_WKILLER)) amount=25;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_STRAGE)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_CORROTTO)) amount=0;
					if (FLAGGED_CRIME(victim, GET_CLAN(ch), CLAN_MULTA)) amount=0;
					if (!IS_A_CRIMINAL_CLAN(victim, GET_CLAN(ch))) amount = -25;
				}
				if(PLR_FLAGGED(victim,PLR_TKILLER)||PLR_FLAGGED(victim,PLR_THIEF)||PLR_FLAGGED(victim,PLR_WAR_KILLER)||PLR_FLAGGED(victim,PLR_KILLER))
					amount = 10;
			}		
			break;
		case THIEF:
			amount=-amount/100;
			break;
		case WANTED:
			if(victim)
				amount-=GET_LEVEL(victim)/4;
			break;
		case CAPTURE:
			if(victim)
				amount=notorieta_reward(ch,victim)/200;
			break;
		default:
			break;
	}
	GET_NOTORIETA(ch)+=amount;
  /*La notorieta' va da 0 a 1000 Shade*/
	GET_NOTORIETA(ch)= MAX(MIN(GET_NOTORIETA(ch),1000),1);
}

void check_player_killer(struct char_data * ch, struct char_data * vict)
{
	int flag=-1;
//	char buf[256];
	
  /*send_to_char("Entrato in check_player_killer\n",ch);*/
	if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_THIEF)
		&& !PLR_FLAGGED(vict, PLR_WAR_KILLER) && !PLR_FLAGGED(vict, PLR_TKILLER)
		&&!IS_NPC(vict)&&!IS_NPC(ch)
		&& (ch != vict)&&!ROOM_FLAGGED(ch->in_room, ROOM_ARENA))
	{
		if((GET_CLAN(ch)>0) &&(GET_CLAN(vict)>0))
			flag= clan[find_clan_by_id(GET_CLAN(ch))].diplomacy[find_clan_by_id(GET_CLAN(vict))].status;      
		else
			flag= PEACE;
	}
	
	switch(flag){
		/*case WAR:
			if(PLR_FLAGGED(vict,PLR_WAR_KILLER)){
				REMOVE_BIT(PLR_FLAGS(vict),PLR_WAR_KILLER);
				send_to_char("Ora non sei piu un &1WAR KILLER&0 ma non sei certo un angioletto!\r\n",vict);
			}
			if(PLR_FLAGGED(ch,PLR_WAR_KILLER)){
				REMOVE_BIT(PLR_FLAGS(ch),PLR_WAR_KILLER);
				send_to_char("Ora non sei piu un &1WAR KILLER&0 ma non sei certo un angioletto!\r\n",ch);
			}
			break;*/
		case ALLIANCE:
		case SURRENDER:
		case VASSALLO:
			if(!IS_NPC(ch)){
				change_notorieta(ch,vict,WKILL,0);
				//SET_BIT(PLR_FLAGS(ch),PLR_WAR_KILLER);
				ch->player_specials->saved.num_warkills++;
				//sprintf(buf, "WAR KILLER bit viene attribuito a %s per l'attacco di %s a %s.",GET_NAME(ch), GET_NAME(vict), world[vict->in_room].name);
				//mudlog(buf, BRF, LVL_IMMORT, TRUE);
				can_do_event(ch,LAST_TKILLER, 60 * 10);
				//sprintf(buf,"Tradimento verso l'alleato %s", GET_NAME(vict));
				//set_crime(ch,vict,"vivo",buf,find_clan_by_id(GET_CLAN(vict)),CLAN_WKILLER, 1000);
				//send_to_char("Se vuoi essere un &1 WAR KILLER &0, bene ecco...\r\n", ch);
			}
			break;
		case PEACE:
			if(IS_NPC(ch))
				return;
			if(vict!=ch->char_specials.last_vict){
				change_notorieta(ch,vict,TKILL,0);
				//if(!PLR_FLAGGED(ch,PLR_KILLER))
					//SET_BIT(PLR_FLAGS(ch),PLR_TKILLER);
				can_do_event(ch,LAST_TKILLER, 60 * 10);
				//sprintf(buf,"Tentato omicidio nei confronti di %s", GET_NAME(vict));
				//set_crime(ch,vict, "vivo",buf,find_clan_by_id(GET_CLAN(vict)),CLAN_TKILLER, 2000);
				ch->player_specials->saved.num_tkills++;
				//sprintf(buf, "TKILLER bit viene attribuito a %s per l'attacco di %s a %s.",
				//	GET_NAME(ch), GET_NAME(vict), world[vict->in_room].name);
				//mudlog(buf, BRF, LVL_IMMORT, TRUE);
				//send_to_char("Se vuoi essere un &1 TKILLER &0, bene ecco...\r\n", ch);
			}
			break;
		default:break;
	}
}

void check_mob_killer(struct char_data * ch, struct char_data * vict)
{
	if(IS_NPC(ch)&& !IS_NPC(vict) && GET_MASTER(ch))
		if(!IS_NPC(GET_MASTER(ch)))
			check_player_killer(GET_MASTER(ch),vict);
}

void check_effective_killer(struct char_data *killer, struct char_data *dead)
{
	
	extern int del_crime_flag(struct char_data *ch, struct char_data *vict, int regno, int Dflag);
	
	char_data *master;
	int flag=-1;
//	char buf[256];
	
	if(!killer || !dead || (killer==dead))
		return;
	if(!IS_NPC(dead)) {
		flag=0;
		if(!IS_NPC(killer)){
			if ((GET_CLAN(killer)>0) && (GET_CLAN(dead)>0))
				flag=clan[find_clan_by_id(GET_CLAN(killer))].diplomacy[find_clan_by_id(GET_CLAN(dead))].status;
			else
				flag = PEACE;
			
			//if(PLR_FLAGGED((dead),PLR_KILLER))
			if (IS_A_CRIMINAL_CLAN(dead, GET_CLAN(killer)))
				flag = WAR;
		}
		
		else{ /*cioe se il killer e' un mob*/
			if(GET_MASTER(killer))
				if(!IS_NPC(master=GET_MASTER(killer))){ /*se il master e' un pg*/
					check_effective_killer(master,dead);
					return;
				}
			return;
		}
	}
	if(killer==dead||killer==NULL||dead==NULL)
		return;
	if(IS_NPC(dead)){
		if(MOB_FLAGGED(dead,MOB_CIVILIAN)){
			//SET_BIT(PLR_FLAGS(killer),PLR_WANTED);
			if (CLAN_ID(dead)>=0) {
				//if (MASTER_ID(dead)>0)
					//sprintf(buf,"Omicidio nei confronti di %s di %s", GET_NAME(dead),NAME(MASTER_ID(dead)));
				//else
					//sprintf(buf,"Omicidio nei confronti di %s", GET_NAME(dead));
				//set_crime(killer,dead, "vivo",buf,find_clan_by_id(CLAN_ID(dead)),CLAN_WANTED,1300);	
				//sprintf(buf,"WANTED  bit viene attribuito a %s per l'uccisione di %s a %s.",GET_NAME(killer), GET_NAME(dead), world[dead->in_room].name);
				//mudlog(buf, BRF, LVL_IMMORT, TRUE);
				//send_to_char("Complimenti ,&4&bWANTED&0, bravo!\r\n", killer);
				change_notorieta(killer,dead,WANTED,0);
				killer->player_specials->saved.num_wanted++;
			}
			return;
		}
	}
	switch(flag){
		case PEACE:
			//sprintf(buf,"PC Killer bit viene attribuito a %s per l'uccisione di %s a %s.",GET_NAME(killer), GET_NAME(dead), world[dead->in_room].name);
			//mudlog(buf, BRF, LVL_IMMORT, TRUE);
			//send_to_char("Complimenti , &1PLAYER KILLER&0, bravo!\r\n", killer);
			change_notorieta(killer,dead,KILL,0);
			//SET_BIT(PLR_FLAGS(killer),PLR_KILLER);
			//sprintf(buf,"Omicidio nei confronti di %s", GET_NAME(dead));
			//set_crime(killer,dead, "vivo",buf,find_clan_by_id(GET_CLAN(dead)),CLAN_KILLER, 5000);
			//del_crime_flag(killer,dead,find_clan_by_id(GET_CLAN(dead)),CLAN_TKILLER);
			can_do_event(killer,LAST_TKILLER, 60 * 10);
			killer->player_specials->saved.num_kills++;
			//REMOVE_BIT(PLR_FLAGS(killer),PLR_TKILLER);
			break;
		case VASSALLO:
		case ALLIANCE:	
		case SURRENDER:
			//sprintf(buf,"WAR Killer bit viene attribuito a %s per l'uccisione di %s a %s.",GET_NAME(killer), GET_NAME(dead), world[dead->in_room].name);
			//mudlog(buf, BRF, LVL_IMMORT, TRUE);
			//send_to_char("Complimenti , &1WAR KILLER&0, bravo!\r\n", killer);
			change_notorieta(killer,dead,KILL,0);
			//SET_BIT(PLR_FLAGS(killer),PLR_WAR_KILLER);
			//sprintf(buf,"Tradimento nei confronti dell'alleato %s", GET_NAME(dead));
			//set_crime(killer,dead,"vivo",buf,find_clan_by_id(GET_CLAN(dead)),CLAN_KILLER, 5000);
			can_do_event(killer,LAST_TKILLER, 60 * 10);
			killer->player_specials->saved.num_warkills++;
			break;
		case WAR:
			//sprintf(buf,"Nessun Killer bit viene attribuito a %s per l'uccisione di %s a %s.",GET_NAME(killer), GET_NAME(dead), world[dead->in_room].name);
			//mudlog(buf, BRF, LVL_IMMORT, TRUE);
			//send_to_char("Un altro Nemico Abbattuto!! La tua &1FAMA&0 cresce a dismisura ,Cavaliere!\r\n", killer);
			//sprintf(buf,"Crimine di guerra nei confronti di %s", GET_NAME(dead));
			//set_crime(killer,dead,"vivo",buf,find_clan_by_id(GET_CLAN(dead)),CLAN_WKILLER, 1000);			
			change_notorieta(killer,dead,KILL,0);
			break;
		default:break;
	}
}

/* start one char fighting another (yes, it is horrible, I know... )  */
void set_fighting(struct char_data * ch, struct char_data * vict)
{
	if (ch == vict)
		return;
	
	assert(!FIGHTING(ch));
	
	ch->next_fighting = combat_list;
	combat_list = ch;
	
	if (IS_AFFECTED(ch, AFF_SLEEP))
		affect_from_char(ch, SPELLSKILL, SPELL_SLEEP);
	
	FIGHTING(ch) = vict;
	GET_POS(ch) = POS_FIGHTING;
	
  /* if (!pk_allowed)
    if(IS_NPC(ch))
      check_mob_killer(ch, vict);
    else
    check_player_killer(ch,vict);*/
}


/* remove a char from the list of fighting chars */
void stop_fighting(struct char_data * ch)
{
	struct char_data *temp;
	
	if (ch == next_combat_list)
		next_combat_list = ch->next_fighting;
	
	REMOVE_FROM_LIST(ch, combat_list, next_fighting);
	ch->next_fighting = NULL;
	FIGHTING(ch) = NULL;
	GET_POS(ch) = POS_STANDING;
	update_pos(ch);
}


#define CADVNUM 44

int make_corpse_from_file (FILE *fl, int n)
{
	struct obj_data *corpse, *money, *obj;
	char *nomi, c;
	int oro, peso, vnum;
	int temp1, temp2;
	extern int max_pc_corpse_time;
	room_vnum stanza;
	int j, temp, i = 0;

	struct obj_data *create_money(int amount);

	if (!(corpse = read_object(CADVNUM, VIRTUAL))){
		log("SYSERR: impossibile creare l'obj *corpse");
		return 0;
	}

	corpse->name = strdup("corpse cadavere");

	fscanf (fl, "STANZA: %d\n", &stanza);
	fscanf (fl, "PG: %s\n", buf);
	fscanf (fl, "ORO: %d\n", &oro);
	fscanf (fl, "PESO: %d\n", &peso);

	if (oro > 0)
		if (!(money = create_money(oro))){
			log ("SYSERR: impossibile creare l'obj *money");
			return 0;
		}

	sprintf (buf2, "Il cadavere di %s giace qui.", buf);
	corpse->description = strdup (buf2);
	corpse->short_description = str_dup (buf);

	GET_OBJ_TYPE (corpse) = ITEM_CONTAINER;
	SET_BIT (GET_OBJ_EXTRA (corpse), ITEM_NODONATE);
	SET_BIT (GET_OBJ_EXTRA (corpse), ITEM_ISCORPSE);
	GET_OBJ_VAL (corpse, 0) = 0;	/* Quanto puo' contenere, 0 */

	GET_OBJ_VAL (corpse, 2) = 0;
	GET_OBJ_VAL (corpse, 3) = 1;
	GET_OBJ_VAL (corpse, 4) = n;	/* Nome del file .crps associato */
	GET_OBJ_WEIGHT (corpse) = peso;
	GET_OBJ_RENT (corpse) = 100000;
	GET_OBJ_TSLOTS(corpse) = 50;
	GET_OBJ_CSLOTS(corpse) = 10;
	GET_OBJ_TIMER(corpse) = max_pc_corpse_time;

	fscanf (fl, "%s\n", buf2);
	while (*buf2 != '$'){
		if (fscanf (fl, "VNUM: %d LOCATE: 0\n", &vnum) < 0){
			log ("SYSERR: impossibile leggere il vnum dell'obj nel cadavere.");
			continue;
		}
		if (vnum < 0){
			log ("SYSERR: letto da un cadavere un obj con vnum -1!");
			continue;
		}
		obj = read_object (vnum, VIRTUAL);
				
		fscanf (fl, "VALORI: %d %d %d %d %d %d %d %d %d %d\n", &GET_OBJ_VAL(obj, 0),
		&GET_OBJ_VAL(obj, 1), &GET_OBJ_VAL(obj, 2), &GET_OBJ_VAL(obj, 3),
		&GET_OBJ_VAL(obj, 4), &GET_OBJ_VAL(obj, 5), &GET_OBJ_VAL(obj, 6),
		&GET_OBJ_VAL(obj, 7), &GET_OBJ_VAL(obj, 8), &GET_OBJ_VAL(obj, 9) );
		fscanf (fl, "EXTRA FLAGS: %Ld\n", &GET_OBJ_EXTRA(obj));
		fscanf (fl, "WEAR  FLAGS: %d\n", &GET_OBJ_WEAR(obj));
		fscanf (fl, "WEIGHT: %d TIMER: %d CSLOTS/TSLOTS: %d/%d\n",  &GET_OBJ_WEIGHT(obj), &GET_OBJ_TIMER(obj), &GET_OBJ_CSLOTS(obj), &GET_OBJ_TSLOTS(obj));
		fscanf (fl, "COST: %d RENT: %d MINLEVEL: %d\n", &GET_OBJ_COST(obj), &GET_OBJ_RENT(obj), &GET_OBJ_LEVEL(obj));
		for (j = 0; j < MAX_OBJ_AFFECT; j++){
			fscanf(fl, "AFFECT NUMERO %d:       LOCATION: %d MODIFIER: %d\n", &temp, &temp1, &temp2);
			obj->affected[j].location = temp1;
			obj->affected[j].modifier = temp2;
		}

		if (IS_OBJ_STAT(obj, ITEM_RESTRING)){
			c = ' ';
			while (c != ':')	//Salta "RESTRING NAME: "
				fscanf (fl, "%c", &c);
			
			nomi = malloc (sizeof (char));
			fscanf (fl, "%c", nomi);
			for (j = 0; nomi[j] != '\n'; j++){
				nomi = realloc (nomi, sizeof(char) * (j+2));
				fscanf (fl, "%c", &nomi[j+1]);
			}

			nomi[j] = '\0';
			obj->name = strdup (nomi);

			c = ' ';
			while (c != ':')	//Salta "RESTRING DESC: "
				fscanf (fl, "%c", &c);
			
			free (nomi);
			nomi = malloc (sizeof (char));
			fscanf (fl, "%c", nomi);
			for (j = 0; nomi[j] != '\n'; j++){
				nomi = realloc (nomi, sizeof(char) * (j+2));
				fscanf (fl, "%c", &nomi[j+1]);
			}
			nomi[j] = '\0';
			obj->short_description = strdup (nomi);

			c = ' ';
			while (c != ':')	//Salta "RESTRING SHDE: "
				fscanf (fl, "%c", &c);
			
			free (nomi);
			nomi = malloc (sizeof (char));
			fscanf (fl, "%c", nomi);
			for (j = 0; nomi[j] != '\n'; j++){
				nomi = realloc (nomi, sizeof(char) * (j+2));
				fscanf (fl, "%c", &nomi[j+1]);
			}
			nomi[j] = '\0';
			obj->description = strdup (nomi);

			c = ' ';
			while (c != ':')	//Salta "RESTRING ACDE: "
				fscanf (fl, "%c", &c);
			
			free (nomi);
			nomi = malloc (sizeof (char));
			fscanf (fl, "%c", nomi);
			for (j = 0; nomi[j] != '\n'; j++){
				nomi = realloc (nomi, sizeof(char) * (j+2));
				fscanf (fl, "%c", &nomi[j+1]);
			}
			nomi[j] = '\0';
			obj->action_description = strdup (nomi);
			free (nomi);
		}

		obj_to_obj (obj, corpse);
		i++;
		fscanf (fl, "%s\n", buf2);
	}
	obj_to_room (corpse, real_room (stanza));
	sprintf (buf, "Fine lettura del cadavere. Conteneva %d objs", i);
	log (buf);
	return 1;
}


void corpse_update (struct obj_data *corpo)
{
	FILE *fl;
	struct obj_data *o, *oc;
	int oro, peso;
	room_rnum stanza;

	if (!(GET_OBJ_VAL (corpo, 4))){
		log ("SYSERR: si sta tentando di salvare un obj che non e' un cadavere di pg!");
		return;
	}

	sprintf (buf, "corpse/%d.crps", GET_OBJ_VAL (corpo, 4));

	if (!(fl = fopen (buf, "r")))
		return;

	if (fscanf (fl, "STANZA: %d\nPG: %s\nORO: %d\nPESO: %d\n", &stanza, buf1, &oro, &peso) < 0){
		sprintf (buf, "SYSERR: Impossibile leggere i dati dal file %s", buf);
		log (buf);
		return;
	}

	if (!(fl = fopen (buf, "w"))){
		sprintf (buf, "SYSERR: Impossibile ricreare il file %s", buf);
		log (buf);
		return;
	}

	fprintf (fl, "STANZA: %d\n", stanza);
	fprintf (fl, "PG: %s\n", buf1);
	fprintf (fl, "ORO: %d\n", oro);
	fprintf (fl, "PESO: %d\n", peso);

	for (o = corpo->contains; o; o = o->next_content){
		Ascii_Obj_to_store_from(o, fl, 0);
		oc = o->contains;
		while (oc){
			Ascii_Obj_to_store_from (oc, fl, 0);
			oc = oc->next_content;
		}
	}

	fprintf (fl, "$\n");

	fclose (fl);

}


void load_corpse(void)
{
	FILE *crps_file;
	struct obj_data *temp;

	if (!(crps_index = fopen ("corpse/index", "r+"))){
		perror ("SYSERR: Impossibile caricare il file di index dei cadaveri");
		exit(1);
	}
	corpse_maxnum = 0;

	fscanf (crps_index, "%s\n", buf1);
	while (*buf1 != '$'){
		sprintf (buf2, "corpse/%s", buf1);
		if (!(crps_file = fopen (buf2, "r"))){
			sprintf (buf2, "SYSERR: %s nel file di index dei corpi. File non esistente!", buf2);
			log (buf2);
			continue;
		}
		corpse_maxnum = MAX (corpse_maxnum, atoi (buf1));

		if (!(make_corpse_from_file (crps_file, atoi(buf1))))
			log("SYSERR: impossibile creare un cadavere.");

		fclose (crps_file);
		fscanf (crps_index, "%s\n", buf1);
	}

	/* Questa e' una pezza schifosa dovuta al fatto che sembra che sul pc
	   di dalila l'ultimo oggetto dell'autorun non viene caricato. Se un
	   giorno questo brutto bug sparira', anche queste righe potranno fare
	   la stessa fine.	*/
	temp = read_object (CADVNUM, VIRTUAL);
	obj_to_room (temp, real_room (1600));
	fclose (crps_index);
}


void corpse_save (struct char_data *ch)
{
	FILE *fl;
	struct obj_data *o, *oc;
	int i;

	if (!(crps_index = fopen ("corpse/index", "r"))){
		perror ("SYSERR: Impossibile caricare il file di index dei cadaveri");
		exit(1);
	}

	sprintf (buf, "corpse/%d.crps", ++corpse_maxnum);
	if (!(fl = fopen (buf, "w"))){
		sprintf (buf1, "SYSERR: impossibile creare il file %s", buf);
		perror (buf1);
		return;
	}

	fprintf (fl, "STANZA: %d\n", world[ch->in_room].number);
	fprintf (fl, "PG: %s\n", GET_NAME (ch));
	fprintf (fl, "ORO: %d\n", GET_GOLD(ch));
	fprintf (fl, "PESO: %d\n", GET_WEIGHT(ch));

	for (o = ch->carrying; o; o = o->next_content){
		Ascii_Obj_to_store_from(o, fl, 0);
		oc = o->contains;
		while (oc){
			Ascii_Obj_to_store_from (oc, fl, 0);
			oc = oc->next_content;
		}
	}
	for (i = 0; i < NUM_WEARS; i++)
		if ((o = GET_EQ(ch, i))){
			Ascii_Obj_to_store_from(o, fl, 0);
			oc = o->contains;
			while (oc){
				Ascii_Obj_to_store_from (oc, fl, 0);
				oc = oc->next_content;
			}
		}

	fprintf (fl, "$\n");

	fclose (fl);


	// Modifica index
	get_line (crps_index, buf1);
	sprintf (buf2, "%s\n", buf1);
	if (*buf2 != '$')
		while (get_line (crps_index, buf1)){
			if (*buf1 == '$')
				sprintf (buf2, "%s%d.crps\n$\n", buf2, corpse_maxnum);
			else
				sprintf (buf2, "%s%s\n", buf2, buf1);
		}
	else
		sprintf (buf2, "%d.crps\n$\n", corpse_maxnum);

	fclose (crps_index);

	if (!(crps_index = fopen ("corpse/index", "w"))){
		perror ("SYSERR: Impossibile rinnovare il file di index dei cadaveri");
		exit(1);
	}

	fprintf (crps_index, buf2);
	fclose (crps_index);
}


void destroy_corpse_file (struct obj_data *j)
{
	if (GET_OBJ_VAL (j, 4)){ // ovvero se e' un cadavere di pg
		sprintf (buf, "corpse/%d.crps", GET_OBJ_VAL (j, 4));
		remove (buf);
		if (!(crps_index = fopen ("corpse/index", "r"))){
			perror ("SYSERR: Impossibile caricare il file di index dei cadaveri");
			exit(1);
		}			// il file di index viene aggiornato
		sprintf (buf, "%d.crps", GET_OBJ_VAL (j, 4));
		if (buf2[0])
		buf2[0] = '\0';
		do{
			get_line (crps_index, buf1);
			if (strcmp (buf, buf1))
				sprintf (buf2, "%s%s\n", buf2, buf1);
			if (*buf1 == '$')
				break;
		}
		while (1);
		fclose (crps_index);
		if (!(crps_index = fopen ("corpse/index", "w"))){
			perror ("SYSERR: Impossibile rinnovare il file di index dei cadaveri");
			exit(1);
		}
		fprintf (crps_index, buf2);
		fclose (crps_index);
	/* Questo if e' un sanity check abbastanza inutile, che dovrebbe servire ad evitare grossi problemi
	   qualora alcuni numeri vengano saltati. La presenza di questo sanity check, poi, potrebbe comportare,
	   in caso di decomposizioni simultanee, lo stesso salto di alcuni numeri nei salvataggi successivi,
	   ma visto che questo non dovrebbe creare problemi, questa e' la versione meno pulita ma piu' sicura.
	   Quanti questo sono riuscito a mettere in una sola frase! */
		if (GET_OBJ_VAL (j, 4) == corpse_maxnum)
			corpse_maxnum--;
		GET_OBJ_VAL (j, 4) = 0;
	}
}


void make_corpse(struct char_data * ch)
{
	struct obj_data *corpse, *o;
	struct obj_data *money;
	int i;
	extern int max_npc_corpse_time, max_pc_corpse_time;
	
	struct obj_data *create_money(int amount);
	
	corpse = create_obj();
	
	corpse->item_number = NOTHING;
	corpse->in_room = NOWHERE;
	corpse->name = str_dup("corpse cadavere");
	
	sprintf(buf2, "Il cadavere di %s giace qui.", GET_NAME(ch));
	corpse->description = str_dup(buf2);
	
	sprintf(buf2, "%s", GET_NAME(ch));
	corpse->short_description = str_dup(buf2);
	
	GET_OBJ_TYPE(corpse) = ITEM_CONTAINER;
	SET_BIT(GET_OBJ_EXTRA(corpse), ITEM_NODONATE);
	SET_BIT(GET_OBJ_EXTRA(corpse), ITEM_ISCORPSE);
	GET_OBJ_VAL(corpse, 0) = 0;   /* You can't store stuff in a corpse */
	
	if(AFF_FLAGGED(ch,AFF_CURSE)||(IS_NPC(ch)&&number(1,100)<5))
		GET_OBJ_VAL(corpse, 2) =number(1,10) ;/*Sorry it's Poisoned*/
	else
		GET_OBJ_VAL(corpse, 2) = 0;/*IT's good Food*/
	GET_OBJ_VAL (corpse, 4) = corpse_maxnum+1;	/* Il numero del file associato */
	GET_OBJ_VAL(corpse, 3) = 1;   /* corpse identifier */
	GET_OBJ_WEIGHT(corpse) = GET_WEIGHT(ch) + IS_CARRYING_W(ch);
	GET_OBJ_RENT(corpse) = 100000;
/* make a corpse have extremely poor condition!! */
	GET_OBJ_TSLOTS(corpse) = GET_LEVEL(ch);
	GET_OBJ_CSLOTS(corpse) = MAX(20,MIN(1,GET_LEVEL(ch)/3));
	if (IS_NPC(ch))
		GET_OBJ_TIMER(corpse) = max_npc_corpse_time;
	else{
		if (SECT(ch->in_room) != SECT_WATER_SWIM && SECT(ch->in_room) != SECT_WATER_NOSWIM)
			corpse_save (ch);
		GET_OBJ_TIMER(corpse) = max_pc_corpse_time;
	}
	
	
  /* transfer character's inventory to the corpse */
	corpse->contains = ch->carrying;
	for (o = corpse->contains; o != NULL; o = o->next_content)
		o->in_obj = corpse;
	object_list_new_owner(corpse, NULL);
	
  /* transfer character's equipment to the corpse */
	for (i = 0; i < NUM_WEARS; i++)
		if (GET_EQ(ch, i)){
			if(CAN_WEAR(GET_EQ(ch,i),ITEM_WEAR_HANG))
				obj_to_room(unequip_char(ch, i),ch->in_room);
			else
				obj_to_obj(unequip_char(ch, i), corpse);
		}
  /* transfer gold */
	if (GET_GOLD(ch) > 0) {
    /* following 'if' clause added to fix gold duplication loophole */
		if (IS_NPC(ch) || (!IS_NPC(ch) && ch->desc)) {
			money = create_money(GET_GOLD(ch));
			obj_to_obj(money, corpse);
		}
		GET_GOLD(ch) = 0;
	}
	ch->carrying = NULL;
	IS_CARRYING_N(ch) = 0;
	IS_CARRYING_W(ch) = 0;
	
	obj_to_room(corpse, ch->in_room);
}


/* When ch kills victim */
void change_alignment(struct char_data * ch, struct char_data * victim)
{
  /*
   * new alignment change algorithm: if you kill a monster with alignment A,
   * you move 1/20th of the way to having alignment -A.  Simple and fast.
   */
	GET_ALIGNMENT(ch) += (-GET_ALIGNMENT(victim) - GET_ALIGNMENT(ch))/20;
}



void death_cry(struct char_data * ch)
{
	int door, was_in;
	
	act("!!SOUND(mortemob1.wav L=1 P=1)Ti si gela il sangue quando senti il grido di morte di $n.", FALSE, ch, 0, 0, TO_ROOM);
	was_in = ch->in_room;
	
	for (door = 0; door < NUM_OF_DIRS; door++) {
		if ((ch->in_room!=NOWHERE) && CAN_GO(ch, door)) {
			ch->in_room = world[was_in].dir_option[door]->to_room;
			act("!!SOUND(mortemob1.wav L=1 P=1)Ti si gela il sangue quando senti uno straziante grido di morte.", FALSE, ch, 0, 0, TO_ROOM);
			ch->in_room = was_in;
		}
	}
}

void raw_kill(struct char_data * ch,  struct char_data * killer)
{
	struct affected_type *af, *next;
		
	if (FIGHTING(ch)) {
		if(PLR_FLAGGED(FIGHTING(ch), PLR_CONCENTRATO)){
			REMOVE_BIT(PLR_FLAGS(FIGHTING(ch)), PLR_CONCENTRATO);
		}
	stop_fighting(ch);
	}
	
	for (af = ch->affected; af; af = next){
		next = af->next;
		if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
			affect_remove(ch, af);
	}
	
	//scendi da cavallo o da qualcuno
	generic_unstow(ch);
	if(TRANSPORTED(ch))
		unstow_char(ch);
	
	/* Mail al padrone del MOB */
	
	if ( IS_NPC(ch) && MASTER_ID(ch)>0 ) {
		if (killer)
			sprintf(buf,"Il cadavere di %s e' stato ritrovato a %s.\r\n"
				"Del suo omicidio e' stato accusato %s.\r\n",GET_NAME(ch),world[ch->in_room].name,GET_NAME(killer));
		else
			sprintf(buf,"Il cadavere di %s e' stato ritrovato a %s.\r\n"
				"Nessuna traccia dell'omicida e' stata ritrovata.\r\n",GET_NAME(ch),world[ch->in_room].name);
		store_mail(MASTER_ID(ch),MAIL_GUARDIE,buf);	
	}
	
	if (killer) {
		GET_POS(ch) = POS_STANDING;
		if (death_mtrigger(ch, killer))
			death_cry(ch);
		GET_POS(ch) = POS_DEAD;
	} else
		death_cry(ch);
	
   /*selvaggina*/
	if (IS_MOB(ch) && MOB_FLAGGED(ch,MOB_SELVAGGINA) && (MOB_FLAGGED(ch,MOB_RIS_CARNE) ||
			MOB_FLAGGED(ch,MOB_RIS_PELLE) || MOB_FLAGGED(ch,MOB_RIS_STOFFA) ||
			MOB_FLAGGED(ch,MOB_RIS_PELLICCIA) || MOB_FLAGGED(ch,MOB_RIS_SCAGLIE)))
		make_resurces(ch);
	else make_corpse(ch);
	
	junktalismano(ch);
	if(!IS_NPC(ch)){/*Annullo gli affects e setto il pg sazio e senza sete*/
		
		GET_COND(ch,0)=0;
		GET_COND(ch,1)=96;
		GET_COND(ch,2)=96;
		GET_HITROLL(ch)=0;
		GET_DAMROLL(ch)=0;
		ch->char_specials.saved.affected_by[0]=0;
		ch->char_specials.saved.affected_by[1]=0;
		ch->char_specials.saved.affected_by[2]=0;
		ch->char_specials.saved.affected_by[3]=0;
	}
	
	extract_char(ch);
}

void die(struct char_data * ch,  struct char_data * killer)
{
  float perdita;
	extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
	struct char_data *ch_quest; 		 //Orione
	extern struct char_data *character_list; //Orione
		
	WHOKILLED(ch) = NULL;
	
        /* Orione Inizio */
	if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_QUEST)) {
    if (IS_QUESTOR(killer) && (GET_QUESTMRR(killer)==GET_MOB_VNUM(ch))) {
		  if ((GET_TIPOQUEST(killer)==QUEST_MOBKILL) || (GET_TIPOQUEST(killer)==QUEST_GETITEM))
		    GET_QUESTMRR(killer) = -1; //ucciso il suo mob, quindi setto GET_QUESTMRR a -1
		  else { //ucciso il suo mob di quest, ma tipoquest e' QUEST_GIVEITEM o QUEST_GOTROOM
		    sprintf(buf, "%s ha ucciso il mob %d, obiettivo di una quest %d", GET_NAME(killer), GET_MOB_VNUM(ch), GET_TIPOQUEST(killer));
		    mudlog(buf, NRM, LVL_IMMORT, TRUE);
		  }
    } 
    else { //ricerca del ch destinato, ctrl il tipo di quest, eventualmente segnalo errore, e settaggio GET_QUESTMRR a -2
		  for (ch_quest = character_list; ch_quest; ch_quest = ch_quest->next) {
		    if (!ch_quest->desc) 
		      continue;
		    if (IS_NPC(ch_quest)) 
		      continue;
		    if (!IS_QUESTOR(ch_quest)) 
		      continue;

		    if (GET_QUESTMRR(ch_quest)==GET_MOB_VNUM(ch)) {
			    if ((GET_TIPOQUEST(ch_quest)==QUEST_MOBKILL) || (GET_TIPOQUEST(ch_quest)==QUEST_GETITEM))
			      //trovato il pg destinato, la quest era MOBKILL o GETITEM, quindi imposto a GET_QUESTMRR -2
			      GET_QUESTMRR(ch_quest) = -2;
			    else { //il pg destinato esiste, ma la quest non era MOBKILL o GETITEM
			      sprintf(buf, "%s ha ucciso il mob %d, obiettivo di una quest %d", GET_NAME(killer), GET_MOB_VNUM(ch), GET_TIPOQUEST(ch_quest));
			      mudlog(buf, NRM, LVL_IMMORT, TRUE);
			    }
			  break;
		    }
	    }  
    }
	}
	/* Orione Fine */
																																			    	
	if (!IS_NPC(ch) && ROOM_FLAGGED(ch->in_room, ROOM_ARENA))
		arena_kill(ch);
	else {
		//PEPPE RELIGIONE
		//se ch e' un player, anche il suo killer lo e'
		//e ch e' in modalita' GDR allora non prende la hole
		if (!IS_NPC(ch)) {
			if (killer) {
				if ((!IS_NPC(killer)) && (PRF_FLAGGED(ch, PRF_RPG))){
					perdita = 0;
					if (GET_LEVEL(killer) >= GET_LEVEL(ch))
						GET_MORTI_DA_PG(killer) = 0;
					if (++GET_MORTI_DA_PG(ch) == 5)
						mag_affects (70, killer, ch, DISEASE_SHAARR, 0);

					if (affected_by_spell (killer, SPELLSKILL, DISEASE_SHAARR) && GET_LEVEL(ch) >= GET_LEVEL(killer)){
						act ("&1&bL'immagine che Shaarr ha di te si e' leggermente rivalutata. Sei libero dalla sua maledizione.&0", FALSE, killer, 0, 0, TO_CHAR);
						affect_from_char (killer, SPELLSKILL, DISEASE_SHAARR);
					}
				}
				else {
	    		switch (ch->player_specials->saved.morti_per_livello) {
					  case 0:  perdita = 0.50; break;
					  case 1:  perdita = 0.25; break;
					  case 2:  perdita = 0.10; break;
					  default: perdita = 0.05; break;
				  } 
				  ch->player_specials->saved.morti_per_livello ++;
			  }
			  
			  if (!ROOM_FLAGGED(ch->in_room, ROOM_WAR_ROOM)) {
				  perdita *= MAX(0, titles[(int)GET_CLASS(ch)][(int)MIN(GET_LEVEL(ch)+1, LVL_IMPL)].exp - titles[(int)GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp);
				  gain_exp(ch, -perdita);
			  }
		  }
		  //setta il bit fantasma al player e mette il tempo di resuscitamento negativo   
		  //setta anche il bit nohassle per non far attacare il fantasma dai mob aggressivi
		  SET_BIT(PLR_FLAGS(ch),PLR_FANTASMA);
		  SET_BIT(PRF_FLAGS(ch),PRF_NOHASSLE);
		  SET_BIT(PRF_FLAGS(ch),PRF_HOLYLIGHT);
		  GET_MOVE(ch)=GET_MAX_MOVE(ch);
		  GET_MANA(ch)=GET_MAX_MANA(ch);
		  GET_RESUSCITA(ch)=0;
		}
		check_effective_killer(killer,ch);
		change_fama(killer,ch,0);
		raw_kill(ch, killer);
	}
}

void perform_group_gain(struct char_data * ch, int hi_level, int tot_levels, int num_pg,struct char_data * victim, struct char_data * tank, struct char_data * killer)
{
	extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
	int exp,extra;
	
	exp = ((titles[(int) GET_CLASS(ch)][(int) (GET_LEVEL(ch)+1)].exp) - (titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp));
	
	if (GET_LEVEL(ch) <= 5)        exp = ((double) exp * 0.040);
	else if (GET_LEVEL(ch) <= 10)  exp = ((double) exp * 0.0375);
	else if (GET_LEVEL(ch) <= 15)  exp = ((double) exp * 0.035);
	else if (GET_LEVEL(ch) <= 20)  exp = ((double) exp * 0.0325);
	else if (GET_LEVEL(ch) <= 30)  exp = ((double) exp * 0.03);
	else if (GET_LEVEL(ch) <= 40)  exp = ((double) exp * 0.025);
	else if (GET_LEVEL(ch) <= 45)  exp = ((double) exp * 0.0225);
	else if (GET_LEVEL(ch) <= 50)  exp = ((double) exp * 0.02);
	else if (GET_LEVEL(ch) <= 55)  exp = ((double) exp * 0.015);
	else if (GET_LEVEL(ch) <= 60)  exp = ((double) exp * 0.01);
	else exp = ((double) exp * 0.005);
	
	if (GET_LEVEL(victim)/ (float) hi_level <= 2.0) {
		if (GET_LEVEL(victim)/ (float) hi_level < 1.0)
			exp = exp * potenza(GET_LEVEL(victim) / (float) hi_level, 3);
		else exp = exp * GET_LEVEL(victim) / (float) hi_level;}
	else exp = exp * 2;
	
	if (num_pg ==1) {
		solo_gain(ch,victim);
		return;
	}
	
	if (num_pg <=10) exp = exp * (40 + GET_LEVEL(ch)) / (float) (100 + tot_levels);
	else              exp = exp * 0.15;
	
	if (tank && tank==ch)                   exp = exp * 1.1;
	if (killer && killer==ch)               exp = exp * 1.05;
	if (RIDING(ch))                         exp = exp * 0.9;
	if (MOB_FLAGGED(victim, MOB_MOUNTABLE)||AFF_FLAGGED(victim,AFF_CHARM))
		exp = MIN(exp, 1);
	
	exp = MIN(exp, max_exp_group_gain);
	exp = MAX(exp, 1);
	
	/* Chardhros : Modifica per impedire gruppi con troppi
                     Level di differenza */
	if (GET_LEVEL(ch)<51) {
		if (hi_level - GET_LEVEL(ch) > 9) exp = MIN (exp, 1);
	} else if (GET_LEVEL(ch)>51) {
		/* Puo' gruppare con tutti. Sono gli atri che 
                al limite non gruppano con lui */
	}
	
	
	
	if(IS_NPC(victim)){
		if(GET_MOB_SPEC(victim)==magic_user)
			exp =exp* 1.70;
		if(GET_MOB_SPEC(victim)==cleric)
			exp =exp* 1.70;
		if(GET_MOB_SPEC(victim)==warrior)
			exp =exp* 1.70;
		if(GET_MOB_SPEC(victim)==thief)
			exp =exp* 1.25;
	}
	exp = gain_exp(ch, exp);
	
	if (exp > 0)
	{
		if (exp == 1) send_to_char("Ricevi la tua parte di esperienza,  un solo miserabile XP.\r\n", ch);
		else
		{
			//sprintf(buf2, "Ricevi la tua parte di esperienza, %d XP.\r\n", exp);
			//send_to_char(buf2, ch);
			if((extra=check_gen_abil(ch,ABIL_STUDIO))>0){
				extra=extra*exp/10000;
				sprintf(buf,"La tua abilita nello studiare i nemici ti frutta preziosi insegnamenti&0\n");
				send_to_char(buf,ch);
				GET_EXP(ch)+=extra;
			}
		}
		consigli_esperienza(ch,exp);
	}
	change_alignment(ch, victim);
}

void xp_gain(struct char_data *ch,struct char_data *victim,int missile,struct char_data *tank) {
	if (victim->in_room<=0)
		return;
	if (ch->in_room<=0)
		return;
	if (!ROOM_FLAGGED(ch->in_room, ROOM_ARENA) && !ROOM_FLAGGED(victim->in_room, ROOM_ARENA))
		if ((ch != victim) && (IS_NPC(victim) || victim->desc)) {
			if (!missile) {
				if (IS_AFFECTED(ch, AFF_GROUP)) {
					group_gain(ch, victim, tank);						
				} else { 
					solo_gain(ch, victim);						
				}					
			}
		}
}

void group_gain(struct char_data * ch, struct char_data * victim, struct char_data * tank)
{
	int tot_members=0;
	int pc_members=0;
	int hi_level=0;
	int low_level=0;
	int tot_levels=0;
	struct char_data *k;
	struct follow_type *f;
	
	if (!(k = ch->master))
		k = ch;
	
	if ( IS_AFFECTED(k, AFF_GROUP) && (k->in_room == ch->in_room) )
	{
		tot_members = 1;
		low_level = hi_level = GET_LEVEL(k);
		tot_levels = GET_LEVEL(k);
		if ( (!IS_NPC(k)) && (k->desc) ) pc_members = 1;
	}
	
	for (f = k->followers; f; f = f->next)
		if ( IS_AFFECTED(f->follower, AFF_GROUP) && (f->follower->in_room == ch->in_room) )
		{
			tot_members++;
			hi_level = MAX(hi_level, GET_LEVEL(f->follower));
			low_level = MIN(GET_LEVEL(f->follower), low_level);
			tot_levels += GET_LEVEL(f->follower);
			if ( (!IS_NPC(f->follower)) && (f->follower->desc) ) pc_members++;
		}
		
	if (tot_members == 0)
	{
		log ("Errore in group gain: nessun membro valido nel gruppo");
		return;
	}
	
	if ( IS_AFFECTED(k, AFF_GROUP) && (k->in_room == ch->in_room) && (k->desc) &&
		(!IS_NPC(k)) )
{
		if ((GET_LEVEL(k) - low_level > 15)&&(GET_LEVEL(k)==hi_level)) /* Chardhros: Formula xp per differenze >15 level */
			solo_gain(k,victim);
		else
			perform_group_gain(k, hi_level, tot_levels, tot_members, victim, tank, ch);
	
}
	for (f = k->followers; f; f = f->next)
		if ( IS_AFFECTED(f->follower, AFF_GROUP) && (f->follower->in_room == ch->in_room) && (f->follower->desc) && (!IS_NPC(f->follower)) ){
			if ((GET_LEVEL(f->follower) - low_level > 15)&&(GET_LEVEL(f->follower)==hi_level)) /* Chardhros: Formula xp per differenze >15 level */
				solo_gain(f->follower,victim);
			else
				perform_group_gain(f->follower, hi_level, tot_levels, tot_members, victim, tank, ch);
		}
}


void solo_gain(struct char_data * ch, struct char_data * victim)
{
	extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
	int exp,extra;
	
	exp = ((titles[(int) GET_CLASS(ch)][(int) (GET_LEVEL(ch)+1)].exp) - (titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp));
	
	if (GET_LEVEL(ch) <= 5)        exp = ((double) exp * 0.025);
	else if (GET_LEVEL(ch) <= 10)  exp = ((double) exp * 0.0225);
	else if (GET_LEVEL(ch) <= 15)  exp = ((double) exp * 0.02);
	else if (GET_LEVEL(ch) <= 20)  exp = ((double) exp * 0.015);
	else if (GET_LEVEL(ch) <= 30)  exp = ((double) exp * 0.0125);
	else if (GET_LEVEL(ch) <= 40)  exp = ((double) exp * 0.01);
	else if (GET_LEVEL(ch) <= 45)  exp = ((double) exp * 0.0075);
	else if (GET_LEVEL(ch) <= 50)  exp = ((double) exp * 0.005);
	else if (GET_LEVEL(ch) <= 55)  exp = ((double) exp * 0.0025);
	else if (GET_LEVEL(ch) <= 60)  exp = ((double) exp * 0.0015);
	else exp = ((double) exp * 0.001);
	
	if (IS_NPC(ch))
		exp = (int) (GET_EXP(ch)*0.1);
	
	else
	{
		if (((float) GET_LEVEL(victim)/GET_LEVEL(ch)) > 1.3)
			exp = (int) (exp * potenza(1.3, 4));
		else if ((float) GET_LEVEL(victim)/GET_LEVEL(ch) >= 1) exp = (int) (exp * potenza((double) GET_LEVEL(victim)/GET_LEVEL(ch), 4));
		else exp = (int) (exp * potenza((double) GET_LEVEL(victim)/GET_LEVEL(ch), 3));
	}
	
	exp = MAX(MIN(exp,max_exp_solo_gain), 1);
	
	
	if (RIDING(ch))
		exp -= (int) (exp*10/100);
	if (MOB_FLAGGED(victim, MOB_MOUNTABLE)||AFF_FLAGGED(victim,AFF_CHARM))
		exp = 1;
	if(IS_NPC(victim)){
		if(GET_MOB_SPEC(victim)==magic_user)
			exp =exp* 1.70;
		if(GET_MOB_SPEC(victim)==cleric)
			exp =exp* 1.70;
		if(GET_MOB_SPEC(victim)==warrior)
			exp =exp* 1.70;
		if(GET_MOB_SPEC(victim)==thief)
			exp =exp* 1.25;
	}
	exp = gain_exp(ch, exp);
	
	if (exp > 0)
	{
		if (exp == 1) send_to_char("Guadagni un solo miserabile XP.\r\n", ch);
		else
		{
			//sprintf(buf2, "Guadagni %d XP.\r\n", exp);
			//send_to_char(buf2, ch);
			if((extra=check_gen_abil(ch,ABIL_STUDIO))>0){
				extra=extra*exp/10000;
				sprintf(buf,"La tua abilita nello studiare i nemici ti frutta preziosi insegnamenti&0\n");
				send_to_char(buf,ch);
				GET_EXP(ch)+=extra;
			}
		}
		consigli_esperienza(ch,exp);
	}
	change_alignment(ch, victim);
}

char *replace_string(char *str, char *weapon_singular, char *weapon_plural)
{
	static char buf[256];
	char *cp;
	
	cp = buf;
	
	for (; *str; str++) {
		if (*str == '#') {
			switch (*(++str)) {
				case 'W':
					for (; *weapon_plural; *(cp++) = *(weapon_plural++));
					break;
				case 'w':
					for (; *weapon_singular; *(cp++) = *(weapon_singular++));
					break;
				default:
					*(cp++) = '#';
					break;
			}
		} else
			*(cp++) = *str;
		
		*cp = 0;
	}				/* For */
	
	return (buf);
}


/* message for doing damage with a weapon */
void dam_message(int dam, struct char_data * ch, struct char_data * victim,
	int w_type)
{
	char *buf;
	int msgnum;
	
	static struct dam_weapon_type {
		char *to_room;
		char *to_char;
		char *to_victim;
	} dam_weapons[] = {
		
    /* use #w for singular (i.e. "slash") and #W for plural (i.e. "slashes") */
		
		{
			"$n tenta di colpire $N, ma fallisce.",	/* 0: 0     */
			"Tenti di colpire $N, ma fallisci.",
			"$n tenta di colpirti, ma fallisce."
		},
		
		{
			"$n sfiora $N con il suo colpo.",	/* 1: 1..2  */
			"Sfiori $N con il tuo colpo.",
			"$n ti sfiora con il suo colpo."
		},
		
		{
			"$n #Wa $N di striscio.",		/* 2: 3..4  */
			"Tu #Wi $N di striscio.",
			"$n ti #Wa di striscio."
		},
		
		{
			"$n #Wa $N.",			/* 3: 5..6  */
			"Tu #Wi $N.",
			"$n ti #Wa."
		},
		
		{
			"$n #Wa duramente $N.",			/* 4: 7..10  */
			"Tu #Wi duramente $N.",
			"$n ti #Wa duramente."
		},
		
		{
			"$n #Wa molto duramente $N.",		/* 5: 11..14  */
			"Tu #Wi molto duramente $N.",
			"$n ti #Wa molto duramente."
		},
		
		{
			"$n colpisce $N con violenza.",	/* 6: 15..19  */
			"Tu colpisci $N con violenza.",
			"$n ti colpisce con violenza."
		},
		
		{
			"$n #Wa brutalmente $N.",	/* 7: 19..23 */
			"Tu #Wi brutalmente $N.",
			"$n ti #Wa brutalmente."
		},
		
		{
			"$n #Wa e mutila $N.",	/* 8: 23..28 */
			"Tu #wi e mutili $N.",
			"$n ti #Wa e ti mutila."
		},
		
		{
			"$n mutila $N ripetutamente e con rabbia.", 	/* 9: 28..32 */
			"Mutili $N ripetutamente e con rabbia.",
			"$n ti mutila ripetutamente e con rabbia."
		},
		
		{
			"$n devasta $N.",  	/* 10: 32..36 */
			"Il tuo colpo devasta totalmente $N.",
			"$n ti devasta."
		},
		
		{
			"$n decima $N con ferocia.",	/* 11: 36..44 */
			"Tu decimi $N con ferocia.",
			"$n ti decima con ferocia."
		},
		
		{
			"$n massacra $N riducendolo in piccoli pezzi sanguinolenti.",      /* 12: 44..50 */
			"Massacri $N riducendolo in piccoli pezzi sanguinolenti.",
			"$n ti massacra riducendoti in piccoli pezzi sanguinolenti."
		},
		
		{
			"$n DEMOLISCE $N.",	/* 13: 50..100 */
			"Il tuo colpo DEMOLISCE $N.",
			"$n ti DEMOLISCE."
		},
		
		{
			"$n ANNICHILISCE $N.",	/* 14: 100..150 */
			"Il tuo colpo ANNICHILISCE $N.",
			"$n ti ANNICHILISCE."
		},
		
		{
			"$n DISINTEGRA $N con il suo mortale colpo!!",	/* 15: 150..250   */
			"Tu DISINTEGRI $N con il tuo mortale colpo!!",
			"$n ti DISINTEGRA con il suo mortale colpo!!"
		},
		
		{
			"$n ANNULLA $N con il suo LETALE colpo!!",	/* 16: > 250   */
			"ANNULLI $N con il tuo LETALE colpo!!",
			"$n ti ANNULLA con il suo LETALE colpo!!"
		}
		
	};
	w_type -= TYPE_HIT;		/* Change to base of table with text */
	
	if (dam == 0)		msgnum = 0;
	else if (dam <= 2)  msgnum = 1;
	else if (dam <= 4)  msgnum = 2;
	else if (dam <= 10) msgnum = 3;
	else if (dam <= 15) msgnum = 4;
	else if (dam <= 30) msgnum = 5;
	else if (dam <= 40) msgnum = 6;
	else if (dam <= 50) msgnum = 7;
	else if (dam <= 60)	msgnum = 8;
	else if (dam <= 65)	msgnum = 9;
	else if (dam <= 70)	msgnum = 10;
	else if (dam <= 75)	msgnum = 11;
	else if (dam <= 80)	msgnum = 12;
	else if (dam <= 85)	msgnum = 13;
	else if (dam <= 90)	msgnum = 14;
	else if (dam <= 92)	msgnum = 15;
	else            	msgnum = 16;
	
	
  /* damage message to onlookers */
	buf = replace_string(dam_weapons[msgnum].to_room, attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	act(buf, FALSE, ch, NULL, victim, TO_NOTVICT);
	
  /* damage message to damager */
	send_to_char(CCYEL(ch, C_CMP), ch);
	buf = replace_string(dam_weapons[msgnum].to_char,
			attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	act(buf, FALSE, ch, NULL, victim, TO_CHAR);
	send_to_char(CCNRM(ch, C_CMP), ch);
	
  /* damage message to damagee */
	send_to_char(CCRED(victim, C_CMP), victim);
	buf = replace_string(dam_weapons[msgnum].to_victim,
			attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
	act(buf, FALSE, ch, NULL, victim, TO_VICT | TO_SLEEP);
	send_to_char(CCNRM(victim, C_CMP), victim);
}


/*
 * message for doing damage with a spell or skill
 *  C3.0: Also used for weapon damage on miss and death blows
 */
int skill_message(int dam, struct char_data * ch, struct char_data * vict,
	int attacktype)
{
	int i, j, nr;
	struct message_type *msg;
	
	struct obj_data *weap = GET_EQ(ch, WEAR_WIELD);
	if(attacktype==SECOND_WEAPON||attacktype==SECOND_BACKSTAB)
		weap=GET_EQ(ch,WEAR_WIELD_L);
	if(attacktype==SECOND_BACKSTAB)
		attacktype=SKILL_BACKSTAB;
	for (i = 0; i < MAX_MESSAGES; i++) {
		if (fight_messages[i].a_type == attacktype) {
			nr = dice(1, fight_messages[i].number_of_attacks);
			for (j = 1, msg = fight_messages[i].msg; (j < nr) && msg; j++)
				msg = msg->next;
			
			if (!IS_NPC(vict) && (GET_LEVEL(vict) >= LVL_QUEST_MASTER)) {
				act(msg->god_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR);
				act(msg->god_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT);
				act(msg->god_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT);
			} else if (dam != 0) {
				if (GET_POS(vict) == POS_DEAD) {
					send_to_char(CCYEL(ch, C_CMP), ch);
					act(msg->die_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR);
					send_to_char(CCNRM(ch, C_CMP), ch);
					
					send_to_char(CCRED(vict, C_CMP), vict);
					act(msg->die_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
					send_to_char(CCNRM(vict, C_CMP), vict);
					
					act(msg->die_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT);
				} else {
					send_to_char(CCYEL(ch, C_CMP), ch);
					act(msg->hit_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR);
					send_to_char(CCNRM(ch, C_CMP), ch);
					
					send_to_char(CCRED(vict, C_CMP), vict);
					act(msg->hit_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
					send_to_char(CCNRM(vict, C_CMP), vict);
					
					act(msg->hit_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT);
				}
			} else if (ch != vict) {	/* Dam == 0 */
				send_to_char(CCYEL(ch, C_CMP), ch);
				act(msg->miss_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR);
				send_to_char(CCNRM(ch, C_CMP), ch);
				
				send_to_char(CCRED(vict, C_CMP), vict);
				act(msg->miss_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);

				send_to_char(CCNRM(vict, C_CMP), vict);
				
				act(msg->miss_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT);
			}
			return 1;
		}
	}
	return 0;
}


void damage_nuovo(struct char_data *ch, struct char_data *victim, int dam, int attacktype, bool maxato, int tipo)
{
	//extern struct descriptor_data *descriptor_list;
	struct char_data * tank;
	//struct descriptor_data *i=NULL;
	
/* Daniel Houghton's missile modification */
	bool missile = FALSE;
	
	if(!victim)
		return;

	if (GET_POS(victim) <= POS_DEAD) {
		sprintf (buf, "SYSERR: Attempt to damage a corpse. %s tentava di colpire %s.", GET_NAME(ch), GET_NAME (victim));
		log(buf);
		die(victim, ch);
		return;			/* -je, 7/7/92 */
	}
	
	/* You can't damage an immortal! */
	if (!IS_NPC(victim) && (GET_LEVEL(victim) >= LVL_QUEST_MASTER))
		return;
  
  /* An Immortal don't Fight! */
	if (!IS_NPC(victim) && !IS_NPC(ch) && (GET_LEVEL(ch) >= LVL_QUEST_MASTER))
		return;	

	/* You can't damage a Newbye! */
	if ( (newbye(victim)) 
	    && ( (attacktype==TYPE_ROOM_DAMAGE_FUOCO) 
	    ||   (attacktype==TYPE_ROOM_DAMAGE_ACQUA) 
	    ||   (attacktype==TYPE_ROOM_DAMAGE_ARIA)
	    ||   (attacktype==TYPE_ROOM_DAMAGE_TERRA)
	    ||   (attacktype==TYPE_TRAPPOLE)
	    ||   (!IS_NPC(ch)) ) )
		return;
		//dam=0;

/* Daniel Houghton's missile modification */
	if (ch->in_room != victim->in_room)
		missile = TRUE;
	
	
	/* peaceful rooms - but let imps attack */
/*	
	if (   ch->nr != real_mobile(DG_CASTER_PROXY) //Orione
	    && ch != victim 
	    && ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL) 
	    && GET_LEVEL(ch) < LVL_IMPL) {
		 send_to_char("Questa stanza emana una cosi' piacevole e pacifica sensazione...\r\n", ch);
		 return;
	}
	
	if (   ch == victim 
	    && attacktype == TYPE_TRAPPOLE 
	    && ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL) 
	    ) {
		sprintf(buf,"SYSERR: Trappola in room Peacefull #%d!", ch->in_room);	
		mudlog(buf,BRF,LVL_BUILDER,TRUE); 
		return;
	}
*/	

	/* Ardanos: niente attacchi in prigione! */
	
	if(ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON)) {
		send_to_char("C'e' poco spazio per combattere in prigione!\r\n", ch);
		return;
	}
	
  /* shopkeeper protection */
	if (!ok_damage_shopkeeper(ch, victim))
		return;
		
	/* Modifica By ardanos per catarsi */
	
	if(PLR_FLAGGED(victim,PLR_CATARSI)) {
		REMOVE_BIT(PLR_FLAGS(victim), PLR_CATARSI);
		send_to_char("Qualcuno ha disturbato la tua concentrazione, colpendoti!\r\n", victim);
		act("$n viene risvegliato dalla meditazione.", TRUE, victim, 0, 0, TO_ROOM);	
	}
	
/* Daniel Houghton's missile modification, ma pure Spini's fly/levitate modification! */
	if ((victim != ch) && (!missile) && (!(AFFECTED(victim, AFF_FLYING, BITV_FLYING) || AFFECTED(victim, AFF_LEVITATE, BITV_LEVITATE)) || (AFFECTED(ch, AFF_FLYING, BITV_FLYING) || AFFECTED(ch, AFF_LEVITATE, BITV_LEVITATE)))) {
    /* Start the attacker fighting the victim */
		if (GET_POS(ch) > POS_STUNNED && (FIGHTING(ch) == NULL)) {
			if(IS_NPC(ch))
			  check_mob_killer(ch, victim);
			else 
			  check_player_killer(ch,victim);
			set_fighting(ch, victim);
		}
		
    /* Start the victim fighting the attacker */
		if (GET_POS(victim) > POS_STUNNED && (FIGHTING(victim) == NULL)) {
			set_fighting(victim, ch);
			if (MOB_FLAGGED(victim, MOB_MEMORY) && !IS_NPC(ch))
				remember(victim, ch);
		}
	}
	
  /* If you attack a pet, it hates your guts */
	if (victim->master == ch)
		stop_follower(victim);
	
  /* If the attacker is invisible, he becomes visible */
	if (IS_AFFECTED(ch, AFF_INVISIBLE | AFF_HIDE))
		appear(ch);
	
  /* Cut damage in half if victim has sanct, to a minimum 1 */
	if (IS_AFFECTED(victim, AFF_SANCTUARY) && dam >= 2)
		dam = dam / 2;
	
  /* Check for PK if this is not a PK MUD */
  /* if (!pk_allowed) {
     if(IS_NPC(ch))
     check_mob_killer(ch, victim);
     else
     check_player_killer(ch,victim);
     check_killer(ch, victim);Enrico:old penal Flags
     if (PLR_FLAGGED(ch, PLR_KILLER) && (ch != victim))
     dam = 0;
     }*/
	
	if (RIDING(ch))
		dam += (dam *10)/100;
	
	if ((attacktype >= 0) && (IS_WEAPON(attacktype))) {
		if (test_and_improve(ch, SKILL_ENH_DAMAGE, 200, 2, 0)) {
			act("$n imprime piu' violenza nel colpo.\r\n", TRUE, ch, 0, 0, TO_ROOM);
			send_to_char("Imprimi piu' violenza nel colpo.\r\n", ch);
			dam = dam * 1.2;
		}
	}

	dam *= abil_armatura (victim, attacktype);
	
	if (ch != victim && AFF_FLAGGED(victim, AFF_FIRESHD)) {
		act("!!SOUND(fireball.wav L=1 P=1)$n lancia una palla di fuoco.\n\r", TRUE, victim, 0, 0, TO_ROOM);
  //send_to_char("!!SOUND(fireball.wav L=1 P=1)Il tuo scudo di fuoco ti protegge e lanci una palla di fuoco.\r\n", victim);
		if (AFF_FLAGGED(ch, AFF_FIRESHD)) {
			act("Lo scudo di $n dissipa la palla di fuoco.\r\n", TRUE, ch, 0,0, TO_ROOM);
    //send_to_char("La tua palla di fuoco e' dissipata da una magia corrispondente\r\n", victim);
		}
		else {
			if (AFF_FLAGGED(ch, AFF_PROT_FIRE)) {
				act("Lo schermo di $n smorza la palla di fuoco.\r\n", TRUE, ch, 0,0, TO_ROOM);
				damage(victim, ch, dam/4, SPELL_FIREBALL);
			}
			else 
			  damage(victim, ch, dam/2, SPELL_FIREBALL);
		}
	}
	
	if (ch != victim && AFF_FLAGGED(ch, AFF_DEATHDANCE) && (GET_LEVEL(victim) < LVL_QUEST_MASTER))
		dam += number(1, (GET_LEVEL(ch)/6));

	/***************************************************************\
	| Resistenze: (by Spini)					|
	| Le varie resistenze aiutano i pg a difendersi dai rispettivi	|
	| danni (passati alla funzione damage). Gli elementali vanno in	|
	| percentuale e al massimo assorbono tutto il danno, fisico	|
	| al massimo riduce di un terzo, ma si esprime sempre con	|
	| valori da 0 a 100, come pure le divinita' che proteggono	|
	| solo dai 3/4 del danno al massimo.				|
	\***************************************************************/
	switch (tipo){
		case DANNO_FUOCO:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_FUOCO(victim))) / 100);
			break;
		case DANNO_GHIACCIO:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_GHIACCIO(victim))) / 100);
			break;
		case DANNO_ELETTRICITA:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_ELETTRICITA(victim))) / 100);
			break;
		case DANNO_ACIDO:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_ACIDO(victim))) / 100);
			break;
		case DANNO_SHAARR:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_SHAARR(victim))) * 3.0 / 400);
			break;
		case DANNO_XHYPHYS:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_XHYPHYS(victim))) * 3.0 / 400);
			break;
		case DANNO_THERION:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_THERION(victim))) * 3.0 / 400);
			break;
		case DANNO_SILUE:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_SILUE(victim))) * 3.0 / 400);
			break;
		case DANNO_FISICO:
			dam *= 1 - ((double) MAX(0, MIN(100,GET_RES_FISICO(victim))) / 300);
			break;
		default:
			break;
	}
	
 /* Set the maximum damage per round and subtract the hit points */
	if (maxato)
		dam = MIN(dam, 200);
	dam = MAX(dam, 0);
	
  /*Se si vuole catturare la vittima ...Bisogna flaggarla!*/
	if (  (  (PRF_FLAGGED(ch, PRF_NOKILL) && (!IS_NPC(ch))) 
	       || MOB_FLAGGED(ch, MOB_NOKILL) )
	    && ( !IS_NPC(victim) || (IS_NPC(victim) && MOB_FLAGGED(victim, MOB_CRIMINAL)) ) 
	    && !AFF_FLAGGED(victim, AFF_CAPTURING)
	    && victim != ch
	    && attacktype != TYPE_TRAPPOLE
	   )
		SET_BIT( AFF_FLAGS(victim), AFF_CAPTURING);
	
	
  /* Chardhros: Messaggio danno delle Trappole!! */
	if (attacktype == TYPE_TRAPPOLE) {
		if (dam > 19) {
			if (ch->desc) act("&1Una trappola scatta e ti colpisce brutalmente.&0",TRUE,ch,0,0,TO_CHAR);
			  act("&5Una trappola scatta e colpisce $n brutalmente.&0",TRUE,ch,0,0,TO_ROOM);
		} 
		else if (dam > 15 ) {
			if (ch->desc) act("&1Una trappola scatta e ti colpisce con violenza.&0",TRUE,ch,0,0,TO_CHAR);
			  act("&5Una trappola scatta e colpisce $n con violenza.&0",TRUE,ch,0,0,TO_ROOM);					
		} 
		else if (dam > 11 ) {
			if (ch->desc) act("&1Una trappola scatta e ti colpisce duramente.&0",TRUE,ch,0,0,TO_CHAR);
			  act("&5Una trappola scatta e colpisce $n duramente.&0",TRUE,ch,0,0,TO_ROOM);					
		} 
		else if (dam > 0 ) {
			if (ch->desc) act("&1Una trappola scatta e ti colpisce.&0",TRUE,ch,0,0,TO_CHAR);
			  act("&5Una trappola scatta e colpisce $n.&0",TRUE,ch,0,0,TO_ROOM);
		} 
		else {
			if (ch->desc) act("&3Una trappola scatta ma non ti fa nulla.&0",TRUE,ch,0,0,TO_CHAR);
			  act("&5Una trappola scatta e colpisce $n che rimane incolume.&0",TRUE,ch,0,0,TO_ROOM);
	  }
	}
	
	alter_hit(victim, dam);
	update_pos(victim);
  /*
   * skill_message sends a message from the messages file in lib/misc.
   * dam_message just sends a generic "You hit $n extremely hard.".
   * skill_message is preferable to dam_message because it is more
   * descriptive.
   *
   * If we are _not_ attacking with a weapon (i.e. a spell), always use
   * skill_message. If we are attacking with a weapon: If this is a miss or a
   * death blow, send a skill_message if one exists; if not, default to a
   * dam_message. Otherwise, always send a dam_message.
   */
	if ((!IS_WEAPON(attacktype)) || missile)
		skill_message(dam, ch, victim, attacktype);
	else {
		if (GET_POS(victim) == POS_DEAD || dam == 0) {
			if (!skill_message(dam, ch, victim, attacktype))
				dam_message(dam, ch, victim, attacktype);
		}
		else {
			dam_message(dam, ch, victim, attacktype);
		}
	}
  /* Use send_to_char -- act() doesn't send message if you are DEAD. */
	
	if (  AFF_FLAGGED(victim, AFF_CAPTURING) 
	    && (MOB_FLAGGED(ch, MOB_NOKILL) || (PRF_FLAGGED(ch, PRF_NOKILL) && (!IS_NPC(ch)))) ) {
		
		switch (GET_POS(victim)) {
			case POS_DEAD:
			case POS_MORTALLYW:
			case POS_INCAP:
			case POS_STUNNED:
				act("$n manda Knock Out $N con un violento colpo alla nuca!\n", FALSE, ch, 0, victim, TO_ROOM);
				sprintf(buf,"%s ti colpisce violentemente alla nuca e ti manda KO..Buona Notte!\n", GET_NAME(ch));
				send_to_char(buf,victim);
				WHOKILLED(victim) = ch;
				act("Colpisci $N con un magistrale colpo alla nuca e lo mandi KO!!\n",FALSE,ch,0,victim,TO_CHAR);
				REMOVE_BIT(AFF_FLAGS(victim), AFF_CAPTURING);
		    break;
			default:
		    break;
		}
	}
	
	if(!AFF_FLAGGED(victim, AFF_CAPTURING) || GET_POS(victim) > POS_STUNNED)
		switch (GET_POS(victim)) {
			case POS_MORTALLYW:
				act("$n e' ferito a morte e morira' presto se non viene aiutato.", TRUE, victim, 0, 0, TO_ROOM);
				send_to_char("Sei ferito a morte e morirai presto se non vieni aiutato.\r\n", victim);
				break;
			case POS_INCAP:
				act("$n e' in agonia... lo aspetta una lenta morte se non viene aiutato.", TRUE, victim, 0, 0, TO_ROOM);
				send_to_char("Sei agonizzante e ti aspetta una lenta morte se non vieni aiutato.\r\n", victim);
				break;
			case POS_STUNNED:
				act("$n e' svenuto, ma riprendera' conoscenza molto probabilmente.", TRUE, victim, 0, 0, TO_ROOM);
				send_to_char("Sei svenuto, ma riprenderai conoscenza molto probabilmente.\r\n", victim);
				break;
			case POS_DEAD:
				act("$n e' morto!  R.I.P.", FALSE, victim, 0, 0, TO_ROOM);
				send_to_char("Sei morto! Mi spiace...!!SOUND(death.wav L=1 P=1)\r\n", victim);
				break;
				
			default:			/* >= POSITION SLEEPING */
				if (   (GET_HIT(victim) < (GET_MAX_HIT(victim)/2)) 
				    && MOB_FLAGGED(victim, MOB_CRIMINAL) 
				    && MOB_FLAGGED(victim, MOB_WIMPY) ) {
        /*i ladri Sono notoriamente dei vigliacchi e scappano subito:ma non solo!
         si nascondono e restano hide finche non recuperano HP*/
					
           if (GET_POS(victim) < POS_FIGHTING) { 
					   send_to_char("Umh...non sei messo bene non riesci a scappare\r\n",victim);
					 }    
					 else {
					   do_flee(victim, "", 0, 0);
					   do_flee(victim, "", 0, 0);
					   do_flee(victim, "", 0, 0);
				     do_flee(victim, "", 0, 0);
					 }
					 if (!FIGHTING(victim)) {
						 GET_POS(victim) = POS_RESTING;
						 SET_BIT(AFF_FLAGS(victim), AFF_HIDE);
					 }
				 }
				 
				 if (dam > (GET_HIT(victim) / 2))
					 act("&2&bFa VERAMENTE male!&0", FALSE, victim, 0, 0, TO_CHAR);
				
				 if (GET_HIT(victim) < (GET_MAX_HIT(victim) / 6)) {
					 sprintf(buf2, "%sVorresti che le tue ferite smettessero di sanguinare tanto!!%s\r\n",CCRED(victim, C_SPR), CCNRM(victim, C_SPR));
					 send_to_char(buf2, victim);
					 if (MOB_FLAGGED(victim, MOB_WIMPY) && (ch != victim)) {
					   if (GET_POS(victim) < POS_FIGHTING) { 
					     send_to_char("Umh...non sei messo bene non riesci a scappare\r\n",victim);
					   }
					   else if (!AFF_FLAGGED(ch, AFF_PARALIZE) && !AFF_FLAGGED(ch, AFF_TRAMORTITO) && !AFF_FLAGGED (ch, AFF_IMMOBIL) && !AFF_FLAGGED (ch, AFF_CHARM) && !AFF_FLAGGED (ch, AFF_TAMED))
					     do_flee(victim, "", 0, 0);
					 }
				 }
				 
				 if (!IS_NPC(victim) && GET_WIMP_LEV(victim) && (victim != ch) &&
					   GET_HIT(victim) < GET_WIMP_LEV(victim)) {
					 if (GET_POS(victim) < POS_FIGHTING) {
					   send_to_char("Umh...non sei messo bene non riesci a scappare\r\n",victim);
					 }
					 else if (!AFF_FLAGGED(ch, AFF_PARALIZE) && !AFF_FLAGGED(ch, AFF_TRAMORTITO) && !AFF_FLAGGED (ch, AFF_IMMOBIL) && !AFF_FLAGGED (ch, AFF_CHARM) && !AFF_FLAGGED (ch, AFF_TAMED)){
					   send_to_char("Balzi indietro e tenti di fuggire!\r\n", victim);
					   do_flee(victim, "", 0, 0);
					 }
				 }
			 break;
		}
    
  /* Help out poor linkless people who are attacked */
	if (!IS_NPC(victim) && !(victim->desc)) {
	//do_flee(victim, "", 0, 0);
		if (!FIGHTING(victim)) {
		  act("$n viene salvato dalla potenza divina.", FALSE, victim, 0, 0, TO_ROOM);
			GET_WAS_IN(victim) = victim->in_room;
			char_from_room(victim);
			char_to_room(victim, 0);
		}
	}
	
  /* stop someone from fighting if they're stunned or worse */
	if (FIGHTING(victim) == NULL) 
	  tank=NULL;
	else 
	  tank=FIGHTING(victim);
	
	if ((GET_POS(victim) < POS_STUNNED) && (FIGHTING(victim) != NULL))
		stop_fighting(victim);
	
  /* Uh oh.  Victim died. */
	if (GET_POS(victim) == POS_DEAD) {
		
	  xp_gain(ch, victim, missile, tank);
		
		if (!IS_NPC(victim)) {
			if (ch != victim) {
				sprintf(buf2, "%s e' stato ucciso da %s a %s", GET_NAME(victim), GET_NAME(ch),world[victim->in_room].name);
			}
			else {
				if (attacktype==SPELL_POISON)
					sprintf(buf2,"%s e' morto Avvelenato in %s",GET_NAME(victim), world[victim->in_room].name);
				if (attacktype==TYPE_SUFFERING)
					sprintf(buf2,"%s e' morto di Sofferenze in %s",GET_NAME(victim), world[victim->in_room].name);
				if (attacktype==TYPE_HUNGER)
					sprintf(buf2,"%s e' morto di Fame in %s",GET_NAME(victim), world[victim->in_room].name);
				if (attacktype==TYPE_TRAPPOLE)
					sprintf(buf2,"%s e' morto in una trappola in %s",GET_NAME(victim), world[victim->in_room].name);
			}
			mudlog(buf2, BRF, LVL_IMMORT, TRUE);
			
			if (!IS_NPC(ch)) {
				/*
				for (i = descriptor_list; i; i = i->next) {
					if (   !i->connected && i != victim->desc && i->character 
					    && !PRF_FLAGGED(i->character, PRF_NOGOSS) 
					    && !PLR_FLAGGED(i->character, PLR_WRITING) 
					    && !ROOM_FLAGGED(i->character->in_room, ROOM_SOUNDPROOF) ) {
						if (COLOR_LEV(i->character) >= C_NRM)
							send_to_char(CCRED(i->character, C_NRM), i->character);
						
						act(buf2, FALSE, ch, 0, i->character, TO_VICT | TO_SLEEP);
						
						if (COLOR_LEV(i->character) >= C_NRM)
							send_to_char(CCNRM(i->character, C_NRM), i->character);
					}
				}*/
			}
			else {
				/*
				if (brag_on)
					if (IS_NPC(ch))
						brag(ch, victim);  */  /* <- Insert brag call here */
						
				if (MOB_FLAGGED(ch, MOB_MEMORY))
					forget(ch, victim);
				if ((MOB_FLAGGED(ch, MOB_ASSASSIN)) && (HUNTING(ch) == victim)) {
					extract_char(ch);
					REMOVE_BIT(AFF_FLAGS(victim), AFF_MARK);
				}
			}
		}
		
		if(IS_NPC(victim))
			if (MOB_FLAGGED(victim, MOB_ASSASSIN) && HUNTING(victim))
				REMOVE_BIT(AFF_FLAGS(HUNTING(victim)), AFF_MARK);
		
		if (!IS_NPC(victim) && !ROOM_FLAGGED(ch->in_room, ROOM_ARENA))
			GET_RIP_CNT(victim) += 1;
		
		if (!IS_NPC(ch) && !ROOM_FLAGGED(ch->in_room, ROOM_ARENA))
			GET_KILL_CNT(ch) += 1;

		if (!IS_NPC(ch))
			GET_SENZA_DORMIRE(ch) += 5;
		
		die(victim, ch);
	}
}


void damage(struct char_data *ch, struct char_data *victim, int dam, int attacktype)
{
	damage_nuovo(ch, victim, dam, attacktype, TRUE, DANNO_FISICO);
}


void critical_hit(struct char_data * ch, struct char_data * victim , int location){

    int severity=0,prob_crit=0,mov=0;
    
    if(location==LOCATION_RANDOM)
	location=number(1,6);
    
    severity=number(1,8);
    prob_crit=location*10+severity;
/*
    if(ROOM_FLAGGED(ch->in_room,ROOM_PEACEFUL)){
    act("&8&3Gli Dei ti osservano e sorridono al tuo patetico tentativo.&0", FALSE, ch, 0, victim, TO_CHAR);
    act("&8&3Gli Dei osservano il comportamento di $n e ridono al suo patetico tentativo&0", FALSE, ch, 0, victim, TO_NOTVICT);
    return;	
    }
*/    
    if (GET_POS(victim)< POS_SITTING){
	return ;
	}
    switch(prob_crit){
	    //Testa
	    case 11:
	    case 12: 
		    act("&8&3Colpisci $N alla testa, ma non sembra subire gravi conseguenze.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&3$n ti colpisce alla testa... fiiuu... per questa volta sembra esserti andata bene.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&3$n colpisce $N alla testa, ma non sembra che $N abbia riportato delle conseguenze.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return;
	    case 13:
	    case 14:
	    case 15:
		    act("&8&2Colpisci $N alla testa, rimane leggermente intontito.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce leggermente alla testa... rimani intontito per un po'.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N leggermente alla testa, $N rimane intontito per un po'.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_1)){
		    WAIT_STATE(victim,PULSE_VIOLENCE * 2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    else{
			if(!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_2)){
			    send_to_char("&8&2E' gia' il secondo colpo alla testa, non ti senti tanto bene.&0\r\n",victim);
			    WAIT_STATE(victim,PULSE_VIOLENCE * 4);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);			
			}
			else{	
			    send_to_char("&8&2Le tue ferite alla testa stanno diventando preoccupanti.&0\r\n",victim);
			    WAIT_STATE(victim,PULSE_VIOLENCE * 10);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		
			}
		    
		    }
		    return;
	    case 16:
	    case 17:
		    act("&8&2Colpisci $N alla testa, rimane intontito.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce alla testa... rimani intontito per un po'.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla testa, $N rimane intontito per un po'.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!(AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_2))){
			    WAIT_STATE(victim,PULSE_VIOLENCE * 4);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_1);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }		   
		    else{	
			send_to_char("&8&2Le tue ferite alla testa stanno diventando preoccupanti.&0\r\n",victim);
			WAIT_STATE(victim,PULSE_VIOLENCE * 10);
			SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_3);
			SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    return;
	    case 18:
		    act("&8&2Colpisci $N con una violenta botta alla testa, rimane intontito.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5Senti un gran dolore alla testa, rimani intontito.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4Con un colpo spettacolare $n colpisce $N alla testa, $N rimane intontito.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    WAIT_STATE(victim,PULSE_VIOLENCE * 10);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TESTA_3);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    return;
		//Braccia
	    case 21:
	    case 22:
		    act("&8&3Colpisci $N al braccio destro, non sembra averne risentito molto.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&3$n ti colpisce al braccio destro, non subisci danni rilevanti.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&3$n colpisce $N al braccio destro, $N non sembra averne risentito particolarmente.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return;
	    case 31:
	    case 32:
	    	    act("&8&2Colpisci $N al braccio sinistro, non sembra averne risentito molto.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&2$n ti colpisce al braccio sinistro, non subisci danni rilevanti.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&2$n colpisce $N al braccio sinistro, $N non sembra averne risentito particolarmente.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return;
	    case 23:
	    case 24:
	    case 25:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3)){
		    act("&8&2Colpisci quello che una volta era il braccio destro di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n colpisce la zona dove una volta c'era il tuo braccio destro.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce dove una volta c'era il braccio destro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&2Colpisci leggermente il braccio destro di $N che urla dal dolore.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce leggermente al braccio destro, perdi sensibilita' nel braccio.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce leggermente $N al braccio destro, $N sembra meno brillante di prima adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_1)){
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	    
		    }
		    else{
			if(!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_2)){
			    send_to_char("&8&2E' gia il secondo colpo al braccio destro, non ti senti tanto bene.&0\r\n",victim);
			    if (GET_EQ(victim,WEAR_WIELD)){
				obj_to_room(unequip_char(victim,WEAR_WIELD),victim->in_room);
				act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }    
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			}
			else{	
			    send_to_char("&8&2Le tue ferite al braccio destro stanno diventando preoccupanti.&0\r\n",victim);
			    if (GET_EQ(victim,WEAR_WIELD)){
				obj_to_room(unequip_char(victim,WEAR_WIELD),victim->in_room);
				act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }    
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		
	    		}
		    
		    }
		    return;		    
	    case 33:
	    case 34:
	    case 35:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3)){
		    act("&8&2Colpisci quello che una volta era il braccio sinistro di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n colpisce la zona dove una volta c'era il tuo braccio sinistro.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce dove una volta c'era il braccio sinistro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&2Colpisci leggermente il braccio sinistro di $N che urla dal dolore.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce leggermente al braccio sinistro, perdi sensibilita' nel braccio.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce leggermente $N al braccio sinistro, $N sembra meno brillante di prima adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_1)){
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    else{
			if(!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_2)){
			    send_to_char("&8&2E' gia il secondo colpo al braccio sinistro, non ti senti tanto bene.&0\r\n",victim);
			    if (GET_EQ(victim,WEAR_WIELD_L)){
				obj_to_room(unequip_char(victim,WEAR_WIELD_L),victim->in_room);
				act("&8&2$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			}
			else{	
			    send_to_char("&8&2Le tue ferite al braccio sinistro stanno diventando preoccupanti.&0\r\n",victim);
			    if (GET_EQ(victim,WEAR_WIELD_L)){
				obj_to_room(unequip_char(victim,WEAR_WIELD_L),victim->in_room);
				act("&8&2$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			}
		    
		    }
		    return;
	    case 26:
	    case 27:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3)){
		    act("&8&2Colpisci quello che una volta era il braccio destro di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n colpisce la zona dove una volta c'era il tuo braccio destro.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce dove una volta c'era il braccio destro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&2colpisci il braccio destro di $N che urla dal dolore.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce al braccio destro, perdi sensibilita' nel braccio.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla braccio destro, $N sembra meno blillante di prima adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_2)){
			    if (GET_EQ(victim,WEAR_WIELD)){
				obj_to_room(unequip_char(victim,WEAR_WIELD),victim->in_room);
				act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }    
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_1);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    else{	
			    send_to_char("&8&2Le tue ferite al braccio destro stanno diventando preoccupanti.&0\r\n",victim);
			    if (GET_EQ(victim,WEAR_WIELD)){
				obj_to_room(unequip_char(victim,WEAR_WIELD),victim->in_room);
				act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }    
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    return;
	    case 36:	    
	    case 37:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3)){
		    act("&8&2Colpisci quello che una volta era il braccio sinistro di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n colpisce la zona dove una volta c'era il tuo braccio sinistro.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce dove una volta c'era il braccio sinistro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&2Colpisci il braccio sinistro di $N che urla di dolore.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce al braccio sinistro, perdi sensibilita' nel braccio.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla braccio sinistro, $N sembra meno blillante di prima adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_2)){
			    if (GET_EQ(victim,WEAR_WIELD_L)){
				obj_to_room(unequip_char(victim,WEAR_WIELD_L),victim->in_room);
				act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }    
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_1);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    else{	
			    send_to_char("&8&2Le tue ferite al braccio sinistro stanno diventando preoccupanti.&0\r\n",victim);
			    if (GET_EQ(victim,WEAR_WIELD_L)){
				obj_to_room(unequip_char(victim,WEAR_WIELD_L),victim->in_room);
				act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
				act("&8&5Lasci cadere cio'  che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
				act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			    }    
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    }
		    return;
	    case 28:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3)){
			act("&8&2Colpisci quello che una volta era il braccio destro di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era il tuo braccio destro.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era il braccio destro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&2Tranci di netto il braccio destro di $N, complimenti.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce al braccio destro con una tale violenza da tranciartelo completamente.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n trancia di netto il braccio destro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if (GET_EQ(victim,WEAR_WIELD)){
			    obj_to_room(unequip_char(victim,WEAR_WIELD),victim->in_room);
			    act("&8&2$N lascia cadere l'arma che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    }
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_D_3);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    return ;
	    case 38:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3)){
			act("&8&2Colpisci quello che una volta era il braccio sinistro di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era il tuo braccio sinistro.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era il braccio sinistro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
	    	    act("&8&2Tranci di netto il braccio sinistro di $N, complimenti.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce al braccio sinistro con una tale violenza da tranciartelo completamente.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n trancia di netto il braccio sinistro di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if (GET_EQ(victim,WEAR_WIELD_L)){
			    obj_to_room(unequip_char(victim,WEAR_WIELD_L),victim->in_room);
			    act("&8&2$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5Lasci cadere cio' che avevi impugnato.&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$N lascia cadere cio' che aveva impugnato.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    }
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_BRACCIO_S_3);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);    
		    return ;
		//torso    
	    case 41:
	    case 42:
		    act("&8&3Colpisci l'addome di $N troppo piano per fargli veramente del male.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&3$n ti colpisce all'addome, ma non riporti particolari conseguenze.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&3$n colpisce $N all'addome, non sembra che $N ne abbia risentito.&0", FALSE, ch, 0, victim, TO_NOTVICT);    
		    return ;
	    case 43:
	    case 44:
	    case 45:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3)){
			act("&8&2Colpisci quello che una volta era l'addome di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era il tuo addome.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era l'addome di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			
			return ;
		    }   
		    act("&8&2Colpisci leggermente $N all'addome.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce leggermente all'addome.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce leggermente all'addome $N.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_1)){
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);    
		    if(GET_EQ(victim,WEAR_BODY)){
		    GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=5;
			if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
			    extract_obj(GET_EQ(victim,WEAR_BODY));
			    act("&8&2Il tuo colpo distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5$n con un colpo spettacolare ditrugge la tua armatura .&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			}
		    }
		    }
		    else{
			if(!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_2)){
			    send_to_char("&8&2E' gia il secondo colpo all'addome, non ti senti tanto bene.&0\r\n",victim); 
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			if(GET_EQ(victim,WEAR_BODY)){
			GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=5;
			if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
			    extract_obj(GET_EQ(victim,WEAR_BODY));
			    act("&8&2Il tuo colpo distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5$n con un colpo spettacolare ditrugge la tua armatura .&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			}
			}
			}
			else{	
			    send_to_char("&8&2Le tue ferite all'addome stanno diventando preoccupanti.&0\r\n",victim);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			    if(GET_EQ(victim,WEAR_BODY)){
			    GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=5;
			if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
			    extract_obj(GET_EQ(victim,WEAR_BODY));
			    act("&8&2Il tuo colpo distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5$n con un colpo spettacolare ditrugge la tua armatura.&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			}
			}
			}
		    
		    }
		    return ;
	    case 46:
	    case 47:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3)){
			act("&8&2Colpisci quello che una volta era l'addome di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era il tuo addome.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era l'addome di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			return ;
		    }
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_2)){
			act("&8&2Colpisci $N all'addome.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n ti colpisce all'addome.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce all'addome $N.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_1);
			SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_2);
			SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	
			if(GET_EQ(victim,WEAR_BODY)){
			GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=10;
			if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
			    extract_obj(GET_EQ(victim,WEAR_BODY));
			    act("&8&2Il tuo colpo distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5$n con un colpo spettacolare ditrugge la tua armatura.&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			}
			}
		    }
		    else{	
			send_to_char("&8&2Le tue ferite all'addome stanno diventando preoccupanti.&0\r\n",victim);
			SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_3);
			SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	
			if(GET_EQ(victim,WEAR_BODY)){
			GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=10;
			if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
			    extract_obj(GET_EQ(victim,WEAR_BODY));
			    act("&8&2Il tuo colpo distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5$n con un colpo spettacolare ditrugge la tua armatura.&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			}
			}
		    }
		    return ;	    
	    case 48:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3)){
			act("&8&2Colpisci quello che una volta era l'addome di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era il tuo addome.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era l'addome di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&2Con un colpo spettacolare colpisci $N all'addome violentemente.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce all'addome violentemente.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce violentemente all'addome $N.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_TORSO_3);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	    
		    if(GET_EQ(victim,WEAR_BODY)){
		    GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))-=15;
			if(GET_OBJ_CSLOTS(GET_EQ(victim,WEAR_BODY))<0){
			    extract_obj(GET_EQ(victim,WEAR_BODY));
			    act("&8&2Il tuo colpo distrugge l'armatura di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			    act("&8&5$n con un colpo spettacolare ditrugge la tua armatura.&0", FALSE, ch, 0, victim, TO_VICT);
			    act("&8&4$n con un colpo spettacolare distrugge l'armatura di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
			}
		    }	
		    return ;
	    //Gambe
	    case 51:
	    case 52:
	    	    act("&8&3Colpisci $N alla gamba destra, ma non abbastanza forte da fargli male.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce alla gamba destra, ma non ti fa particolari danni.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla gamba destra, non sembra avergli fatto torppo male.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
	    case 61:
	    case 62:
	    	    act("&8&3Colpisci $N alla gamba sinistra, ma non abbastanza forte da fargli male.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce alla gamba sinistra, ma non ti fa particolari danni.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla gamba sinistra, non sembra avergli fatto torppo male.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
	    case 53:
	    case 54:
	    case 55:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3)){
			act("&8&2Colpisci quella che una volta era la gamba destra di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era la tua gamba destra.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era la gamba destra di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&3Colpisci leggermente $N alla gamba destra, avra' problemi a muoversi per un po.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce leggermente alla gamba destra, con la gamba cosi conciata non sara' semplice muoversi.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce leggermente $N alla gamba destra, $N zoppica adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_1)){
			SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_1);
			SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		
			mov=(GET_MOVE(victim)*30)/100;
			if(GET_MOVE(victim)-mov<=0)
			    GET_MOVE(victim)=0;
			else GET_MOVE(victim)=GET_MOVE(victim)-mov;	    
		    }
		    else{
			if(!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_2)){
			    send_to_char("&8&2E' gia il secondo colpo alla gamba destra, non ti senti tanto bene.&0\r\n",victim); 
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	    
			    mov=(GET_MOVE(victim)*80)/100;
			    if(GET_MOVE(victim)-mov<=0)
				GET_MOVE(victim)=0;
			    else GET_MOVE(victim)=GET_MOVE(victim)-mov;	    
			}
			else{	
			    send_to_char("&8&2Le tue ferite alla gamba destra stanno diventando preoccupanti, cadi a terra.&0\r\n",victim);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		    
			    GET_MOVE(victim)=10;  
			    GET_POS(victim)=POS_SITTING;
			}
		    
		    }
		    return ;
	    case 63:
	    case 64:
	    case 65:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3)){
			act("&8&2Colpisci quella che una volta era la gamba sinistra di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era la tua gamba sinistra.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era la gamba sinistra di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }
		    act("&8&3Colpisci leggermente $N alla gamba sinistra, avra' problemi a muoversi per un po.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce leggermente alla gamba sinistra, con la gamba cosi conciata non sara' semplice muoversi.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce leggermente $N alla gamba sinistra, $N zoppica adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_1)){
			SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_1);
		        SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		
			mov=(GET_MOVE(victim)*30)/100;
			if(GET_MOVE(victim)-mov<=0)
			    GET_MOVE(victim)=0;
			else GET_MOVE(victim)=GET_MOVE(victim)-mov;	    
		    }
		    else{
			if(!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_2)){
			    send_to_char("&8&2E' gia il secondo colpo alla gamba sinistra, non ti senti tanto bene.&0\r\n",victim); 
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	
			    mov=(GET_MOVE(victim)*80)/100;
			    if(GET_MOVE(victim)-mov<=0)
				GET_MOVE(victim)=0;
			    else GET_MOVE(victim)=GET_MOVE(victim)-mov;	    
			}
			else{	
			    send_to_char("&8&2Le tue ferite alla gamba sinistra stanno diventando preoccupanti,cadi a terra.&0\r\n",victim);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		    
			    GET_MOVE(victim)=10;  
			    GET_POS(victim)=POS_SITTING;
			}
		    
		    }
		    return ;
	    case 56:
	    case 57:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3)){
			act("&8&2Colpisci quella che una volta era la gamba destra di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era la tua gamba destra.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era la gamba destra di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }	    		    
		    act("&8&3Colpisci $N alla gamba destra, avra' problemi a muoversi per un po.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce alla gamba destra, con la gamba cosi conciata non sara' semplice muoversi.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla gamba destra, $N zoppica adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_2)){
			    send_to_char("&8&2E' gia il secondo colpo alla gamba destra,non ti senti tanto bene.&0\r\n",victim); 
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_1);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);		    
			    mov=(GET_MOVE(victim)*80)/100;
			    if(GET_MOVE(victim)-mov<=0)
				GET_MOVE(victim)=0;
			    else GET_MOVE(victim)=GET_MOVE(victim)-mov;	    
			}
			else{	
			    send_to_char("&8&2Le tue ferite alla gamba destra stanno diventando preoccupanti,cadi a terra.&0\r\n",victim);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			    GET_MOVE(victim)=10;  
			    GET_POS(victim)=POS_SITTING;
			}
		    return ;
	    case 66:
	    case 67:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3)){
			act("&8&2Colpisci quella che una volta era la gamba sinistra di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era la tua gamba sinistra.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era la gamba sinistra di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }	    		    
		    act("&8&3Colpisci $N alla gamba sinistra, avra' problemi a muoversi per un po.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5$n ti colpisce alla gamba sinistra, con la gamba cosi conciata non sara' semplice muoversi.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4$n colpisce $N alla gamba sinistra, $N zoppica adesso.&0", FALSE, ch, 0, victim, TO_NOTVICT);	    
		    if(!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_2)){
			    send_to_char("&8&2E' gia il secondo colpo alla gamba sinistra, non ti senti tanto bene.&0\r\n",victim); 
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_1);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_2);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			    mov=(GET_MOVE(victim)*80)/100;
			    if(GET_MOVE(victim)-mov<=0)
				GET_MOVE(victim)=0;
			    else GET_MOVE(victim)=GET_MOVE(victim)-mov;	    
			}
			else{	
			    send_to_char("&8&2Le tue ferite alla gamba sinistra stanno diventando preoccupanti,cadi a terra.&0\r\n",victim);
			    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_3);
			    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
			    GET_MOVE(victim)=10;  
			    GET_POS(victim)=POS_SITTING;
			}
		    return ;	    
	    case 58:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3)){
			act("&8&2Colpisci quella che una volta era la gamba destra di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era la tua gamba destra.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era la gamba destra di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }	    		    
		    act("&8&3Con un colpo spettacolare colpisci violentemente $N alla gamba destra, cade a terra.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5Con un colpo spettacolare $n ti colpisce violentemente alla gamba destra, cadi a terra.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4Con un colpo spettacolare $n colpisce violentemente $N alla gamba destra, $N cade a terra.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_D_3);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);
		    GET_MOVE(victim)=10;  
		    GET_POS(victim)=POS_SITTING;
		    return ;	    
	    case 68:
		    if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3)){
			act("&8&2Colpisci quella che una volta era la gamba sinistra di $N.&0", FALSE, ch, 0, victim, TO_CHAR);
			act("&8&5$n colpisce la zona dove una volta c'era la tua gamba sinistra.&0", FALSE, ch, 0, victim, TO_VICT);
			act("&8&4$n colpisce dove una volta c'era la gamba sinistra di $N, sara' difficile per $N vincere questo combattimento.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    return ;
		    }	    		    
		    act("&8&3Con un colpo spettacolare colpisci violentemente $N alla gamba sinistra, cade a terra.&0", FALSE, ch, 0, victim, TO_CHAR);
		    act("&8&5Con un colpo spettacolare $n ti colpisce violentemente alla gamba sinistra, cadi a terra.&0", FALSE, ch, 0, victim, TO_VICT);
		    act("&8&4Con un colpo spettacolare $n colpisce violentemente $N alla gamba sinistra, $N cade a terra.&0", FALSE, ch, 0, victim, TO_NOTVICT);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_1);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_2);
		    SET_BIT(AFF_FLAGS(victim),AFF_CRITICAL_GAMBA_S_3);
		    SET_BIT(AFF_FLAGS(victim),AFF_FERITO);	    
		    GET_MOVE(victim)=10;  
		    GET_POS(victim)=POS_SITTING;
		    return ;	    	    
	    default:
		    return ;
		    	    
	    }
} 

void hit(struct char_data * ch, struct char_data * victim, int type)
{
	struct obj_data *r_wielded = GET_EQ(ch, WEAR_WIELD);
	struct obj_data *l_wielded=GET_EQ(ch,WEAR_WIELD_L);
	struct obj_data *wielded=NULL;
	int w_type, victim_ac, calc_thaco, diceroll,critical_prob, parry_dice, parry_prob;
	int dam = 0;
	extern int thaco[NUM_CLASSES][LVL_IMPL+1];
	extern struct str_app_type str_app[];
	extern struct dex_app_type dex_app[];
	
	int backstab_mult(int level);
	
  /* Ardanos: niente attacchi in prigione! */
	
	if(ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON)) {
		send_to_char("C'e' poco spazio per combattere in prigione!\r\n", ch);
		return;
	}

  /* check if the character has a fight trigger */
	fight_mtrigger(ch);
	
  /* Do some sanity checking, in case someone flees, etc. */
	if(!victim){
		stop_fighting(ch);
		return;
	}
	
	if (ch->in_room != victim->in_room) {
		if (FIGHTING(ch) && FIGHTING(ch) == victim)
			stop_fighting(ch);
		return;
	}
	
	if (   AFF_FLAGGED(ch, AFF_PARALIZE)
	    || AFF_FLAGGED(ch, AFF_TRAMORTITO)
	    || AFF_FLAGGED(ch, AFF_SLEEP) ) {
		if(FIGHTING(ch))
			stop_fighting(ch);
		return;
	}
	
	if (   AFF_FLAGGED(ch,AFF_HIDE)
	    || AFF_FLAGGED(ch,AFF_INVISIBLE) )
		REMOVE_BIT(AFF_FLAGS(ch), AFF_INVISIBLE | AFF_HIDE);
	
	if (   type == SECOND_WEAPON
	    || type == SECOND_BACKSTAB )/*scelgo l'arma Dx o Rx*/
		wielded = l_wielded;
	else
		wielded = r_wielded;
	
  /* Find the weapon type (for display purposes only) */
	if (wielded && ((GET_OBJ_TYPE(wielded) == ITEM_WEAPON) || (GET_OBJ_TYPE(wielded)==ITEM_WEAPON_2HANDS)))
		w_type = GET_OBJ_VAL(wielded, 3) + TYPE_HIT;
	else {
		if (IS_NPC(ch) && (ch->mob_specials.attack_type != 0))
			w_type = ch->mob_specials.attack_type + TYPE_HIT;
		else
			w_type = TYPE_HIT;
	}
	
 /* Calculate the THAC0 of the attacker */
	if (!IS_NPC(ch))
		calc_thaco = thaco[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)];
	else		/* THAC0 for monsters is set in the HitRoll */
		calc_thaco = 20;
	
	calc_thaco -= str_app[STRENGTH_APPLY_INDEX(ch)].tohit;
	calc_thaco -= affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(ch);
	//PEPPE CRITICAL
	if (   AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_1) 
	    || AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_1) )
	  calc_thaco += (affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(ch))/2;
	
	if (   AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_2) 
	    || AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_2) 
	    || AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_D_3) 
	    || AFF_FLAGGED(ch,AFF_CRITICAL_GAMBA_S_3) ) {
	  calc_thaco += (affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(ch));
	  calc_thaco += str_app[STRENGTH_APPLY_INDEX(ch)].tohit;
	}
	
	calc_thaco -= (int) ((GET_INT(ch)+GET_WIS(ch) - 26) / 3);
  /* Intelligence & wisdom helps!(Enrico :not so much now) */
	// calc_thaco -= (int) ((GET_WIS(ch) - 13) / 2.5);	/* So does wisdom */
	
  /* Calculate the raw armor including magic armor.  Lower AC is better. */
	
	victim_ac = GET_AC(victim) / 10;
	
	if (AWAKE(victim))
		victim_ac = MAX(-10, victim_ac);	/* -10 is lowest */
	
	victim_ac += dex_app[GET_DEX(victim)].defensive;
	
	if (   AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_2) 
	    || AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_2) )
	  victim_ac -= dex_app[GET_DEX(victim)].defensive;
  
  /* roll the die and take your chances... */
	diceroll = number(1, 20);
	critical_prob=diceroll;
	
	
  /* decide whether this is a hit or a miss */
	if ((   (   (diceroll < 20) 
	         && AWAKE(victim) ) 
	     && (   (diceroll == 1) 
	         || ((calc_thaco - diceroll) > victim_ac) ) )) {
		
    /* the attacker missed the victim */
		if (type == SKILL_BACKSTAB || type==SECOND_BACKSTAB)
			damage(ch, victim, 0, SKILL_BACKSTAB);
		else
			damage(ch, victim, 0, w_type);
	}
	else {
		parry_prob = 22 + GET_DEX (victim);
		parry_dice = number (1, 100);

		if (abil_scudo (ch, victim)){
			act ("$N blocca magistralmente il colpo di $n con il suo scudo!", FALSE, ch, 0, victim, TO_NOTVICT);
			act ("$N blocca magistralmente il tuo colpo con il suo scudo!", FALSE, ch, 0, victim, TO_CHAR);
			act ("Blocchi magistralmente il colpo di $n con il tuo scudo!", FALSE, ch, 0, victim, TO_VICT);
			dam = 0;
		}
		else if (AFF_FLAGGED(victim, AFF_BLINK) && (number(1, 100) <= GET_LEVEL(victim) / 2)) {
			act("$N balza MAGICAMENTE indietro di qualche metro, al sicuro dai tuoi attacchi.", FALSE, ch, 0, victim, TO_CHAR);
			act("Balzi MAGICAMENTE indietro di qualche metro, al sicuro dall'attacco di $n.", FALSE, ch, 0, victim, TO_VICT);
			dam = 0;
		}
		else if (test_and_improve(victim, SKILL_PARRY, 100, 2, 0) && parry_dice <= parry_prob) {

			act("$N para ABILMENTE il tuo attacco.!!SOUND(sword1.wav L=1 P=1)", FALSE, ch, 0, victim, TO_CHAR);
			act("Pari ABILMENTE l'attacco di $n.!!SOUND(sword2.wav L=1 P=1)", FALSE, ch, 0, victim, TO_VICT);
			dam = 0;
		}
		else if (   AFF_FLAGGED(victim, AFF_MIRRORIMAGE) 
		         && (number(1, 34) > GET_INT(ch)) 
		         && (number(1, 22) <= GET_INT(victim)) ) {
			act("Una delle immagini illusorie di $N si disperde e viene immediatamente rimpiazzata!", FALSE, ch, 0, victim, TO_CHAR);
			act("!!SOUND(fight3.wav L=1 P=1)Una delle tue immagini prende il colpo di $n, si dissolve e viene rimpiazzata.", FALSE, ch, 0, victim, TO_VICT);
			hitprcnt_mtrigger(victim);
			return;
		} 
		else {
	/* Decapitazione e paralisi da mazza */
			if (check_extra_abil(ch, victim, ITEM_WEAPON) == DECAPITATO)
				return;
			if(!victim)
				return;

      /* Start with the damage bonuses: the damroll and strength apply */
      /* okay, we know the guy has been hit.  now calculate damage. */
			dam = str_app[STRENGTH_APPLY_INDEX(ch)].todam;
			dam += GET_DAMROLL(ch);
			
			if (wielded) {
      /* Add weapon-based damage if a weapon is being wielded */
				dam += dice(GET_OBJ_VAL(wielded, 1), GET_OBJ_VAL(wielded, 2));
			} 
			else {
				
   /* If no weapon, add bare hand damage instead */
				if (IS_NPC(ch)) {
					dam += dice(ch->mob_specials.damnodice, ch->mob_specials.damsizedice);
				} 
				else {
					dam += number(0, 2);	/* Max 2 bare hand damage for players */
				}
			}
			
    /*
     * Include a damage multiplier if victim isn't ready to fight:
     *
     * Position sitting  1.33 x normal
     * Position resting  1.66 x normal
     * Position sleeping 2.00 x normal
     * Position stunned  2.33 x normal
     * Position incap    2.66 x normal
     * Position mortally 3.00 x normal
     *
     * Note, this is a hack because it depends on the particular
     * values of the POSITION_XXX constants.
     */
		}
		
		if (GET_POS(victim) < POS_FIGHTING)
			dam *= 1 + (POS_FIGHTING - GET_POS(victim)) / 3;
		
    /* at least 1 hp damage min per hit */
		dam = MAX(1, dam);
		dam*=check_extra_dam(ch,victim,ITEM_WEAPON);/*Danni extra causati dalle abilita*/
			
		if (type == SKILL_BACKSTAB || type == SECOND_BACKSTAB) {
			dam *= backstab_mult(GET_LEVEL(ch));
			if (   (GET_CLASS(ch) == CLASS_THIEF) 
			    && (GET_LEVEL(ch) >= 40) 
			    && (number(0,2)==0) 
			    && (GET_ABIL(ch,ABIL_PUGNALE) == MASTER_LIV) ) {
				cast_spell(ch, victim, NULL,SPELL_PARALIZE);
			}
			if(type == SKILL_BACKSTAB)
				damage(ch, victim, dam, SKILL_BACKSTAB);
			if(type == SECOND_BACKSTAB) {
				/* Danno del doppio stab By Ardanos */
				dam = MIN(dam, number (50, 150));
				damage(ch, victim, dam, SECOND_BACKSTAB);
			}
		}
		else
			damage(ch, victim, dam, w_type);
	}
	
	if (! (victim && ch))
		return;
	
  /* check if the victim has a hitprcnt trigger */
	hitprcnt_mtrigger(victim);
	
	if (critical_prob==20) {
		if(!IS_NPC(ch))
			critical_hit(ch, victim, LOCATION_RANDOM);
		else {
			if (number(1, 100) < 20)
				if (number(1, 100) < GET_LEVEL(ch))
					critical_hit(ch,victim,LOCATION_RANDOM);
		}
	}
}

//Amanu perform_concentrato
void perform_concentrato(void){
    extern struct char_data *character_list;
    struct char_data *ch, *next_char;

    for (ch = character_list; ch; ch = next_char) {
		next_char = ch->next;
//		if(!FIGHTING(ch))
//			REMOVE_BIT(PLR_FLAGS(ch),PLR_CONCENTRATO);
		if(PLR_FLAGGED(ch,PLR_CONCENTRATO)){
		//sprintf(buf,"CONCENTRATO %d.\r\n",GET_TCONCENTRATO(ch)%10);
		//send_to_char(buf,ch);
			GET_TCONCENTRATO(ch)--;
			if(GET_TCONCENTRATO(ch)%10==0){
				REMOVE_BIT(PLR_FLAGS(ch),PLR_CONCENTRATO);
				switch(GET_TCONCENTRATO(ch)/10){
					case 1:
					    if (!FIGHTING(ch)) {
					      REMOVE_BIT(PLR_FLAGS(ch), PLR_CONCENTRATO);
					      break;
					    }
							if (!GET_EQ(ch, WEAR_WIELD)) {
    							send_to_char("Hai bisogno di impugnare un'arma per aver successo con quello.\r\n", ch);
								break;
							}	
							if(number(1,100)<(20 + GET_LEVEL(ch) - GET_LEVEL(FIGHTING(ch)))) {
							//RIUSCITO
								REMOVE_BIT(PLR_FLAGS(ch),PLR_CONCENTRATO);
							//send_to_char("RIUSCITO\r\n",ch);
								act("&2Con uno splendido colpo stordisci completamente $N.&0",FALSE,ch,0,FIGHTING(ch),TO_CHAR);
								act("&2$n finalmente sembra voler fare la sua mossa,un movimento velocissimo e poi vedi solo buio.&0",FALSE,ch,0,FIGHTING(ch),TO_VICT);
								act("&2Con uno splendido colpo ottimamente assestato $n manda a tappeto $N.&0",FALSE,ch,0,FIGHTING(ch),TO_NOTVICT);
								alter_hit(FIGHTING(ch),GET_HIT(FIGHTING(ch)));
								GET_POS(FIGHTING(ch))=POS_STUNNED;
						  }
						  else {	
								REMOVE_BIT(PLR_FLAGS(ch),PLR_CONCENTRATO);
							//send_to_char("FALLITO\r\n",ch);
								act("&2Non riesci ad infliggere un corpo mortale a $N&0",FALSE,ch,0,FIGHTING(ch),TO_CHAR);
								act("&2$n cerca di colpirti con un colpo straordinario,ma per tua fortuna ti manca.&0",FALSE,ch,0,FIGHTING(ch),TO_VICT);
								act("&2Solo la fortuna ha salvato $N dal potentissimo colpo che ha sferrato $n!&0",FALSE,ch,0,FIGHTING(ch),TO_NOTVICT);
						  }
						break;
				  default:
						send_to_char("Bug 123 contattare un creatore,\r\n",ch);
						break;
				}
			}
		}
	}
}


/* control the fights going on.  Called every 2 seconds from comm.c. */
void perform_violence(void)
{
	int j;
	struct char_data *ch;
	int check_abil_weapon(struct obj_data *weapon);
	extern struct index_data *mob_index;
        //extern struct spell_info_type spell_info[]; 
	
	unsigned int attacks = 2, total_attacks = 1;
	for (ch = combat_list; ch; ch = next_combat_list) {
		next_combat_list = ch->next_fighting;
		attacks = 0;
		total_attacks = 1;
		// Enrico ADD :se un pg ne trasporta un altro, se inizia a combattere
		// lo scarica a terra automaticamente
		if (AFF_FLAGGED(ch, AFF_PARALIZE)||AFF_FLAGGED(ch,AFF_TRAMORTITO))
			continue;
		if (TRANSPORTED(ch)){
			sprintf(buf,"Ti scrolli dalle spalle %s, ora combatti meglio no?\r\n"
				,GET_NAME(TRANSPORTED(ch)));
			send_to_char(buf,ch);
			sprintf(buf,"%s ti rovescia per terra ,STA COMBATTENDO!!\r\n"
				,GET_NAME(ch));
			send_to_char(buf,TRANSPORTED(ch));
			act("$n si scrolla dalle spalle $N, ora combatte meglio!",FALSE,ch,0
				,TRANSPORTED(ch),TO_ROOM);	
			unstow_char(ch);
			
		}
		
		GET_DURATA_COMB(ch)++;
		
		if (IS_NPC(ch)){
			if (GET_POS(ch)==POS_SITTING) {
				attacks=0;
				//GET_MOB_WAIT(ch) -= PULSE_VIOLENCE;
				//if(number(1,10)>2){
				if(!CHECK_WAIT(ch)){
					GET_POS(ch) = POS_FIGHTING;
					act("&4&b$n si alza in piedi rapidamente!&0",TRUE,ch,0,0,TO_ROOM);
				}
				else
					act("&3&b$n prova a rialzarsi, ma non ci riesce!!&0",TRUE,ch,0,0,TO_ROOM);
			}
			else{
				if((AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3)) && (AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3))){
				    send_to_char("&8&7Riesci a fare poco con le braccia in quello stato.&0\r\n",ch);
				    attacks=0;
				}
				else {
					if (GET_LEVEL(ch)<6) attacks = 1;
					if (GET_LEVEL(ch)<10 && GET_LEVEL(ch) > 5) attacks = number (1,2);
					if (GET_LEVEL(ch)<20 && GET_LEVEL(ch) > 9) attacks = number (1,2);
					if (GET_LEVEL(ch)<30 && GET_LEVEL(ch) > 19) attacks = number(2,3);
					if (GET_LEVEL(ch)<40 && GET_LEVEL(ch) > 29) attacks = number (2,3);
					if (GET_LEVEL(ch)<50 && GET_LEVEL(ch) > 39) attacks = number (2,4);
					if (GET_LEVEL(ch)<60 && GET_LEVEL(ch) > 49) attacks = number (2,4);
					if (GET_LEVEL(ch)>59) attacks = number (3,6);

				// DD sui mob moltiplica gli attacchi per 1.5
				// arrotondato per eccesso (By Spini)
					if (AFF_FLAGGED(ch, AFF_DEATHDANCE)){
						attacks *= 15;
						switch (attacks % 10){
							case 0:
								attacks /= 10;
								break;
							case 5:
								attacks = (attacks / 10) + 1;
								break;
						}
					}

					if((AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3)) ||  (AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3))){	
						send_to_char("&8&7Sei molto limitato negli attacchi con il braccio conciato in quel modo.&0\r\n",ch);
						if(attacks!=1)
							attacks=attacks/2;
					}
				}
			}
		}
		else /* PC */
		{
			if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
				attacks=0;
				send_to_char("&1SEI LEGATO!!! NON RIESCI A DIFENDERTI!!!&0\n",ch);
			}
			if (GET_POS(ch)==POS_SITTING){
				attacks=0;
				send_to_char("&3&bNon puoi combattere da seduto!!&0\r\n", ch);
			}
			if (GET_MOVE(ch)<1){
			    send_to_char("sei esausto non riesci a combattere,cerchi invano di riprendere fiato.\r\n",ch);
			    GET_MOVE(ch)=0;
			}



			else {
				
				

				if((AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3)) && (AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3))){
				    send_to_char("&8&7Riesci a fare poco con le braccia in quello stato.&0\r\n",ch);
				    attacks=0;
				    }

				if(PLR_FLAGGED(ch,PLR_CONCENTRATO)){
					send_to_char("&8&7Ti stai concentrando.&0\r\n",ch);
					attacks=0;
				}
				else {

			/* Attacchi totali, dipendenti dalle skill del pg e
			   dall'abilita' spada (by Spini)                   */

				if (test_and_improve(ch, SKILL_SECOND_ATTACK, 100, 2, 0))
					total_attacks++;
				if (test_and_improve(ch, SKILL_THIRD_ATTACK, 100, 2, 0))
					total_attacks++;
				if (test_and_improve(ch, SKILL_FOURTH_ATTACK, 100, 2, 0))
					total_attacks++;
				if (test_and_improve(ch, SKILL_FIFTH_ATTACK, 100, 2, 0))
					total_attacks++;
				if (check_abil_spada (ch))
					total_attacks++;

				//fa in modo che se la vittima e' stesa l'attacchi subito
				//senza aspettare ulteriori round     
				if(GET_POS(FIGHTING(ch))<=POS_STUNNED)
					attacks=1;

				else if(GET_DURATA_COMB(ch)%3==0){
					attacks = 0;
					switch (total_attacks){
						case 2:
						case 3:
							attacks = 1;
							break;
						case 4:
						case 5:
						case 6:
							attacks = 2;
							break;
					}
					if (AFF_FLAGGED(ch, AFF_DEATHDANCE))
						attacks++;
				}

				else if(GET_DURATA_COMB(ch)%3==2){
					attacks = 0;
					switch (total_attacks){
						case 3:
						case 4:
							attacks = 1;
							break;
						case 5:
						case 6:
							attacks = 2;
							break;
					}
					if (AFF_FLAGGED(ch, AFF_DEATHDANCE))
						attacks++;
				}

				else {		// ovvero if(GET_DURATA_COMB(ch)%3==1)
					if (total_attacks == 6)
						attacks = 2;
					else
						attacks = 1;
					if (AFF_FLAGGED(ch, AFF_DEATHDANCE))
						attacks++;
				}

			if((AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_D_3)) ||  (AFF_FLAGGED(ch,AFF_CRITICAL_BRACCIO_S_3))){	
			    send_to_char("&8&7Sei molto limitato negli attacchi con il braccio conciato in quel modo.&0\r\n",ch);
			    if(attacks!=1)
				attacks=attacks/2;
			    }
			}
			}

		}
		if(AFF_FLAGGED(ch,AFF_SLEEP)){
			attacks=0;
			if(number(0,10)>3){
				affect_from_char(ch, SPELLSKILL, SPELL_SLEEP);
				REMOVE_BIT (AFF_FLAGS (ch), AFF_SLEEP);
				GET_POS(ch)=POS_SITTING;
				act("&3&bIl colpo ricevuto ti sveglia dal torpore in cui eri caduto!&0",FALSE,ch,0,0,TO_CHAR);
				
			}
		}
		
		while(attacks-- > 0) {
			if (FIGHTING(ch) == NULL || ch->in_room != FIGHTING(ch)->in_room) {
				stop_fighting(ch);
				continue;
			}
			
			if (FIGHTING(ch)!=NULL)
				if (GET_POS(FIGHTING(ch))<=POS_STUNNED &&
					((PRF_FLAGGED(ch,PRF_NOKILL)&&(!IS_NPC(ch)))||MOB_FLAGGED(ch,MOB_NOKILL))
					&&( !IS_NPC(FIGHTING(ch))||(IS_NPC(FIGHTING(ch))&&MOB_FLAGGED(FIGHTING(ch),MOB_CRIMINAL)))){
					GET_POS(FIGHTING(ch))=POS_STUNNED;
					GET_HIT(FIGHTING(ch))=-1;
					do_dream(FIGHTING(ch),2);
					GET_POS(FIGHTING(ch))=POS_STUNNED;
					GET_HIT(FIGHTING(ch))=-1;
					REMOVE_BIT(AFF_FLAGS(FIGHTING(ch)),AFF_CAPTURING);
					change_fama(ch,FIGHTING(ch),0);
					stop_fighting(ch);
					if(AFF_FLAGGED(ch,AFF_CAPTURING))/*caso particolare:se 2 pg nokill*/
						REMOVE_BIT(AFF_FLAGS(ch),AFF_CAPTURING);  /*       si incontrano*/
					attacks=0;
					send_to_char("Domi a fatica la tua furia omicida e risparmi la vita al tuo avversario.\r\n", ch);
					continue;
				}
				
				
			if (IS_NPC(ch)) {
				if (GET_MOB_WAIT(ch) > 0) {
					GET_MOB_WAIT(ch) -= PULSE_VIOLENCE;
					continue;
				}
				GET_MOB_WAIT(ch) = 0;
				
			}
			if (GET_POS(ch)== POS_FIGHTING){
				//IL PG SI STANCA MENTRE COMBATTE
				if(!IS_NPC(ch))
				    GET_MOVE(ch)=GET_MOVE(ch)-1;
				    
				hit(ch, FIGHTING(ch), TYPE_UNDEFINED);
				if(GET_EQ(ch,WEAR_WIELD_L))
					hit(ch,FIGHTING(ch),SECOND_WEAPON);
				if (!IS_NPC(ch))
					for (j = 0; j < NUM_WEARS; j++)
						if (ch->equipment[j]) check_danno_oggetto(ch, ch->equipment[j], 5);
			}
		}
		
		if (MOB_FLAGGED(ch, MOB_SPEC) && mob_index[GET_MOB_RNUM(ch)].func != NULL)
			(mob_index[GET_MOB_RNUM(ch)].func) (ch, ch, 0, "");
	}
}


void strike_missile(struct char_data *ch, struct char_data *tch,
	struct obj_data *missile, int dir, int attacktype)
{
	int dam;
	extern struct str_app_type str_app[];
	
	
	
	sprintf(buf , "&1&bHai colpito %s!&0\r\n", GET_NAME(tch));
	send_to_char(buf, ch);
	/*Simula l'incavolatura perche' colpito*/
	//SET_BIT(MOB_FLAGS(tch), MOB_AGGRESSIVE); 
	SET_BIT(MOB_FLAGS(tch), MOB_MEMORY);
	remember(tch, ch);
	sprintf(buf, "$P arriva volando da %s e colpisce %s.",
		dirs[rev_dir[dir]], GET_NAME(tch));
	act(buf, FALSE, tch, 0, missile, TO_ROOM);
	sprintf(buf, "$P arriva volando da %s e ti COLPISCE!",
		dirs[rev_dir[dir]]);
	act(buf, FALSE, tch, 0, missile, TO_CHAR);
	dam = str_app[STRENGTH_APPLY_INDEX(ch)].todam;
	dam += dice(missile->obj_flags.value[1],
			missile->obj_flags.value[2]);
	dam += GET_DAMROLL(ch);
	
	damage(ch, tch, dam, attacktype);
	return;
}


void miss_missile(struct char_data *ch, struct char_data *tch,
	struct obj_data *missile, int dir, int attacktype)
{
	sprintf(buf, "$P arriva volando da %s e colpisce il terreno!",
		dirs[rev_dir[dir]]);
	act(buf, FALSE, tch, 0, missile, TO_ROOM);
	act(buf, FALSE, tch, 0, missile, TO_CHAR);
	sprintf(buf,"HAi Mancato %s!\r\n", GET_NAME(tch));
	send_to_char(buf, ch);
	/*Simula l'incavolatura*/
	//SET_BIT(MOB_FLAGS(tch), MOB_AGGRESSIVE);
	SET_BIT(MOB_FLAGS(tch), MOB_MEMORY);
	remember(tch, ch);
}


void mob_reaction(struct char_data *ch, struct char_data *vict, int dir)
{
	if (IS_NPC(vict) && !FIGHTING(vict) && GET_POS(vict) > POS_STUNNED){
		
    /* can remember so charge! */
		if (IS_SET(MOB_FLAGS(vict), MOB_MEMORY)) {
			remember(vict, ch);
			sprintf(buf, "$n sembra spaventato!");
			act(buf, FALSE, vict, 0, 0, TO_ROOM);
			if (GET_POS(vict) == POS_STANDING) {
				if (!do_simple_move(vict, rev_dir[dir], 1, CAN_SHOW_ROOM))
					act("$n inciampa mentre cerca di scappare!", FALSE, vict, 0, 0, TO_ROOM);
			} else
				GET_POS(vict) = POS_STANDING;
			
    /* can't remember so try to run away */
		} else {
			do_flee(vict, "", 0, 0);
		}
	}	
}


void fire_missile(struct char_data *ch, char *arg1,
	struct obj_data *missile, int pos, int range, int dir)
{
	room_rnum room_wild_target_room (room_rnum room, int dir);
	bool shot = FALSE, found = FALSE;
	int attacktype;
	int room, nextroom, distance;
	struct char_data *vict;

	room = ch->in_room;
	
	if (IS_IN_WILD(ch)) 
		nextroom = room_wild_target_room(ch->in_room, dir);
	else if (EXIT2(room, dir) && EXIT2(room, dir)->to_room!=NOWHERE) 
		nextroom = EXIT(ch, dir)->to_room;
	else
		nextroom = NOWHERE;
	
	if (GET_OBJ_TYPE(missile) == ITEM_GRENADE) {
		send_to_char("Lanciato!\r\n", ch);
		sprintf(buf, "$n lancia %s %s.",
			GET_EQ(ch,WEAR_WIELD)->short_description, dirs[dir]);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "%s arriva volando da %s.\r\n",
			missile->short_description, dirs[rev_dir[dir]]);
		send_to_room(buf, nextroom);
		obj_to_room(unequip_char(ch, pos), nextroom);
		return;
	}
	
	for (distance = 1; ((nextroom != NOWHERE) && (distance <= range)); distance++) {
		
		for (vict = world[nextroom].people; vict ; vict= vict->next_in_room) {
			if ((isname(arg1, GET_NAME(vict))) && (CAN_SEE(ch, vict))) {
				found = TRUE;
				break;
			}
		}
		
		if (found == 1) {
			switch(GET_OBJ_TYPE(missile)) {
				case ITEM_THROW:
					send_to_char("You throw it!\r\n", ch);
					sprintf(buf, "$n lancia %s %s.", GET_EQ(ch,WEAR_WIELD)->short_description,
						dirs[dir]);
					attacktype = SKILL_THROW;
					break;
				case ITEM_ARROW:
					act("$n prende la mira e tira!", TRUE, ch, 0, 0, TO_ROOM);
					send_to_char("Prendi la mira e tiri!\r\n", ch);
					attacktype = SKILL_BOW;
					break;
				case ITEM_ROCK:
					act("$n prende la mira e tira!", TRUE, ch, 0, 0, TO_ROOM);
					send_to_char("Prendi la mira e tiri!\r\n", ch);
					attacktype = SKILL_SLING;
					break;
				case ITEM_BOLT:
					act("$n prende la mira e tira!", TRUE, ch, 0, 0, TO_ROOM);
					send_to_char("Prendi la mira e tiri!\r\n", ch);
					attacktype = SKILL_CROSSBOW;
					break;
				default:
					attacktype = TYPE_UNDEFINED;
					break;
			}
			
			if (attacktype != TYPE_UNDEFINED)
				shot = test_and_improve(ch, attacktype,100, 15, 0);
			else
				shot = FALSE;
			
			if (shot == TRUE) {
				strike_missile(ch, vict, missile, dir, attacktype);
				if ((number(0, 1)) || (attacktype == SKILL_THROW))
					obj_to_char(unequip_char(ch, pos), vict);
				else
					extract_obj(unequip_char(ch, pos));
			} else {
      /* ok missed so move missile into new room */
				miss_missile(ch, vict, missile, dir, attacktype);
				if ((!number(0, 2)) || (attacktype == SKILL_THROW))
					obj_to_room(unequip_char(ch, pos), vict->in_room);
				else
					extract_obj(unequip_char(ch, pos));
			}
			
      /* either way mob remembers */
			if(!IS_SET(MOB_FLAGS(vict), MOB_SENTINEL))
				mob_reaction(ch, vict, dir);
			WAIT_STATE(ch, PULSE_VIOLENCE);
			return;
			
		}
		
		room = nextroom;
		if (IS_IN_WILD(ch)) 
			nextroom = room_wild_target_room(room, dir);
		else if (EXIT2(room, dir) && EXIT2(room, dir)->to_room!=NOWHERE) 
			nextroom = EXIT2(room, dir)->to_room;
		else
			nextroom = NOWHERE;
	}
	
	send_to_char("Non riesco a trovare il tuo bersaglio!\r\n", ch);
	return;
	
}


void tick_grenade(void)
{
	struct obj_data *i, *tobj;
	struct char_data *tch, *next_tch;
	int s, t, dam, door;
  /* grenades are activated by pulling the pin - ie, setting the
     one of the extra flag bits. After the pin is pulled the grenade
     starts counting down. once it reaches zero, it explodes. */
	
	for (i = object_list; i; i = i->next) {
		
		if (IS_SET(GET_OBJ_EXTRA(i), ITEM_LIVE_GRENADE)) {
      /* update ticks */
			if (i->obj_flags.value[0] >0)
				i->obj_flags.value[0] -=1;
			else {
				t = 0;
				
        /* blow it up */
        /* checks to see if inside containers */
        /* to avoid possible infinite loop add a counter variable */
				s = 0; /* we'll jump out after 5 containers deep and just delete
                       the grenade */
				
				for (tobj = i; tobj; tobj = tobj->in_obj) {
					s++;
					if (tobj->in_room != NOWHERE) {
						t = tobj->in_room;
						break;
					} else
					if ((tch = tobj->carried_by)) {
						t = tch->in_room;
						break;
					} else
					if ((tch = tobj->worn_by)) {
						t = tch->in_room;
						break;
					}
					if (s == 5)
						break;
				}
				
        /* then truly this grenade is nowhere?!? */
				if (t <= 0) {
					sprintf(buf, "serious problem, grenade truly in nowhere\r\n");
					log(buf);
					extract_obj(i);
				} else { /* ok we have a room to blow up */
					
        /* peaceful rooms */
/*					if (ROOM_FLAGGED(t, ROOM_PEACEFUL)) {
						sprintf(buf, "You hear %s explode harmlessly, with a loud POP!\n\r",
							i->short_description);
						send_to_room(buf, t);
						extract_obj(i);
						return;
					}
*/					
					dam = dice(i->obj_flags.value[1], i->obj_flags.value[2]);
					
					sprintf(buf, "Oh no - %s explodes!  KABOOOOOOOOOM!!!\r\n",
						i->short_description);
					send_to_room(buf, t);
					
					for (door = 0; door < NUM_OF_DIRS; door++)
						if (CAN_GO2(t, door))
							send_to_room("You hear a loud explosion!\r\n",
								world[t].dir_option[door]->to_room);
						
					for (tch = world[t].people; tch; tch = next_tch) {
						next_tch= tch->next_in_room;

						
						if (GET_POS(tch) <= POS_DEAD) {
							log("SYSERR: Attempt to damage a corpse.");
							return;			/* -je, 7/7/92 */
						}
						
            /* You can't damage an immortal! */
						if (IS_NPC(tch) || (GET_LEVEL(tch) < LVL_QUEST_MASTER)) {
							
							GET_HIT(tch) -= dam;
							act("$n is blasted!", TRUE, tch, 0, 0, TO_ROOM);
							act("You are caught in the blast!", TRUE, tch, 0, 0, TO_CHAR);
							update_pos(tch);
							
							if (GET_POS(tch) <= POS_DEAD) {
								make_corpse(tch);
								death_cry(tch);
								extract_char(tch);
							}
						}
						
					}
               /* ok hit all the people now get rid of the grenade and
                  any container it might have been in */
					
					extract_obj(i);
					
				}
			} /* end else stmt that took care of explosions */
		} /* end if stmt that took care of live grenades */
	} /* end loop that searches the mud for objects. */
	
	return;
	
}



bool range_hit(struct char_data *ch, struct char_data * victim, struct obj_data *obj)
{
	int w_type = 315, victim_ac, calc_thaco, dam=0, diceroll;
	extern int thaco[NUM_CLASSES][LVL_IMPL+1];
	extern struct str_app_type str_app[];
	extern struct dex_app_type dex_app[];
	
	
	if (!IS_NPC(ch))
		calc_thaco = thaco[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)];
	else          /* THAC0 for monsters is set in the HitRoll */
		calc_thaco = 20;
	
	calc_thaco -= dex_app[GET_DEX(ch)].miss_att;
	calc_thaco -= (affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(ch));
	diceroll = number(1, 20);
	
	victim_ac = GET_AC(victim) / 10;
	
	if (AWAKE(victim))
		victim_ac += dex_app[GET_DEX(victim)].defensive;
	if (GET_POS(victim) == POS_SITTING)
		victim_ac -= 1;
	if (GET_POS(victim) <= POS_RESTING)
		victim_ac -= 2;
	victim_ac = MAX(-20, victim_ac);      /* -20 is lowest */
	
	if ((((diceroll < 20) && AWAKE(victim)) &&
			((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac)))) {
		return FALSE;
	} else {
		if (AFF_FLAGGED(victim, AFF_SHIELD) || test_and_improve(victim, SKILL_DODGE, 120, 2, 0)) {
			act("$N schiva il tuo attacco.", FALSE, ch, 0, victim, TO_CHAR);
			act("Schivi $p.", FALSE, ch, obj, victim, TO_VICT);
			return FALSE;
		} else if (AFF_FLAGGED(victim, AFF_BLINK) &&
			(number(1, 50) <= GET_LEVEL(victim))) {
			act("$N si teletrasporta lontano da $p che gli hai lanciato.", FALSE, ch, obj, victim, TO_CHAR);
			act("Ti teletrasporti lontano da $p che $n ti ha scagliato.", FALSE, ch, obj, victim, TO_VICT);
		} else if (AFF_FLAGGED(victim, AFF_MIRRORIMAGE) &&
			(number(1, 40) > GET_INT(ch)) && (number(1, 40) <= GET_INT(victim))) {
			act("Una delle immagini illusorie di $N si dissolve e viene rimpiazzata all'istante!", FALSE, ch, 0, victim, TO_CHAR);
			act("Una delle tue immagini viene colpita da $p, si dissolve e viene subito rimpiazzata.", FALSE, ch, 0, victim, TO_VICT);
		} else {
    /* okay, we know the guy has been hit.  now calculate damage. */
			dam = str_app[STRENGTH_APPLY_INDEX(ch)].todam;
			if (GET_OBJ_TYPE(obj) == ITEM_MISSILE) {
				dam += GET_DAMROLL(ch);
				dam += dice(GET_OBJ_VAL(obj, 1), GET_OBJ_VAL(obj, 2));
			}
			if ((GET_OBJ_TYPE(obj) == ITEM_WEAPON) || (GET_OBJ_TYPE(obj)==ITEM_WEAPON_2HANDS))
				dam += (GET_OBJ_VAL(obj, 0) + dice(GET_OBJ_VAL(obj, 1), GET_OBJ_VAL(obj, 2)) / 2);
			else
				dam += number(0, (GET_OBJ_WEIGHT(obj) / 5));
		}
		dam = MAX(1, dam);          /* at least 1 hp damage min per hit */
		damage(ch, victim, dam, w_type);
		
		return TRUE;
	}
	return FALSE;
}

bool fire_at_char(struct char_data *ch, struct char_data * list, struct obj_data * obj, int dir)
{
	struct char_data *vict = list;
	char msgbuf[256];
	
	while (vict) {	/* while there's a victim */
    /* check to hit */
		if (!pk_allowed) {
			if(IS_NPC(ch))
				check_mob_killer(ch, vict);
			else
				check_player_killer(ch,vict);
       /* check_killer(ch, vict);*/
			
			if (PLR_FLAGGED(ch, PLR_KILLER) && (ch != vict))
				send_to_char("Uno strano muro di forza ti protegge!!!\r\n", vict);
		} else {
			if (range_hit(ch, vict, obj)) {
				if (vict) {
					sprintf(msgbuf, "$p vola lontano da %s.", dirs[rev_dir[dir]]);
					act(msgbuf, FALSE, vict, obj, 0, TO_CHAR);
					if (IS_NPC(vict) && !IS_NPC(ch) && GET_POS(ch) > POS_STUNNED) {
						SET_BIT(MOB_FLAGS(vict), MOB_MEMORY);
						remember(vict, ch);
						HUNTING(vict) = ch;
					}
				}
				return FALSE;
			}
		}
		sprintf(msgbuf, "$p vola nella direzione da cui sei venuto da %s.", dirs[rev_dir[dir]]);
		act(msgbuf, FALSE, vict, obj, 0, TO_CHAR);
		if (!(vict = vict->next_in_room))
			return TRUE;
	}
	return TRUE;	/* missed everyone */
}

void do_stopfight(struct char_data * ch)
{
	if (FIGHTING(ch))
	{
		send_to_char("Tenti di arrestare la tua furia.",ch);
		if ((FIGHTING(FIGHTING(ch)) && (FIGHTING(FIGHTING(ch))!=ch)) && (!IS_NPC(ch))) {
			stop_fighting(ch);
			send_to_char("Smetti di combattere.",ch);
			WAIT_STATE(ch, PULSE_VIOLENCE * 2);
		} else {
			sprintf(buf,"%s ti sta attaccando!", GET_NAME(FIGHTING(ch)));
			send_to_char(buf,ch);
		}
	}
	else
	{
		send_to_char("Ma se non stai combattendo!",ch);
	}
}

void junktalismano (struct char_data * ch)
{
	struct obj_data *talismano;
	if (IS_NPC(ch) && ch->master && MOB_FLAGGED(ch,MOB_HARD_TAME) && MOB_FLAGGED(ch,MOB_MOUNTABLE))
	{
		talismano = get_obj_in_list_vis(ch->master, "dragonsign", (ch->master)->carrying);
		if (talismano && (GET_OBJ_TYPE(talismano)==ITEM_RENT_DRAGON) &&
			GET_OBJ_VAL(talismano,3) == GET_MOB_VNUM(ch) && GET_OBJ_VAL(talismano,8)==GET_IDNUM(ch->master))
		{
			sprintf(buf, "Il talismano del drago ti si frantuma nelle mani.\r\n");
			send_to_char(buf, ch->master);
			extract_obj(talismano);
		}
	}
}


void generic_unstow(struct char_data *ch)
{
	if (ch){
		if(TRANSPORTED_BY(ch))
			unstow_char(TRANSPORTED_BY(ch));
		else
		if(RIDING(ch))
			dismount_char(ch);
	}	
}

void consigli_esperienza(struct char_data *ch,int exp){

	extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
	int exp_x_lvl=0,moltiplicatore=0,consiglio=0;
	
	exp_x_lvl= ((titles[(int) GET_CLASS(ch)][(int) (GET_LEVEL(ch)+1)].exp) - (titles[(int) GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp));
	moltiplicatore=(GET_LEVEL(ch)/8)+1;
	consiglio= exp_x_lvl / (moltiplicatore*exp);

	if(consiglio <= 20)
		send_to_char("Questo combattimento ti ha insegnato piu di quello che potevi imparare in 10 giorni di studio.\r\n",ch);
	if(consiglio>20 && consiglio <=40)
		send_to_char("Se tutti i combattimenti fossero come questo in pochi giorni saresti a capo della gilda.\r\n",ch);
	if(consiglio>40 && consiglio <=60)
		send_to_char("E' stato uno scontro duro, ti senti soddisfatto dell'esperienza maturata e di essere rimasto vivo.\r\n",ch);
	if(consiglio>60 && consiglio <=90)
		send_to_char("Ti asciughi il sudore... questo si che e' stato un combattimento duro.\r\n",ch);
	if(consiglio>90 && consiglio <=140)
		send_to_char("Hai sudato, ma sapevi perfettamente come affrontare questo tipo di combattimento.\r\n",ch);
	if(consiglio>140 && consiglio <=200)
		send_to_char("E' stato uno sconto abbastanza facile, all'inizio ti aspettavi di peggio.\r\n",ch);
	if(consiglio>200)
		send_to_char("Non so che cosa pensi di poter imparare da un combattimento del genere.\r\n",ch);
	
	return;

}
