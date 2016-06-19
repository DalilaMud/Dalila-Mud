/* ************************************************************************
*   File: spec_procs.c                                  Part of CircleMUD *
*  Usage: implementation of special procedures for mobiles/objects/rooms  *
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
#include "clan.h"
#include "mail.h"
#include "dg_scripts.h"

/*   external vars  */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct time_info_data time_info;
//extern const struct command_info cmd_info[];
extern struct str_app_type str_app[];
extern struct dex_app_type dex_app[];
extern const char *mail_type[];

/* extern functions */
void add_follower(struct char_data * ch, struct char_data * leader);
int find_abil_num( char *name);
ACMD(do_flee);
ACMD(do_say);
extern int limite_follower(struct char_data *ch);

struct social_type {
	char *cmd;
	int next_line;
};


/* ********************************************************************
*  Special Functions for mobiles                                     *
******************************************************************** */

int spell_sort_info[MAX_SKILLS+1];
int proficienze_sort_info[MAX_PROFICIENZE+1];
int abilita_sort_info[MAX_ABILITA+1];

extern char *spells[];
extern char *proficienze[];
extern char *abilita[];

void sort_spells(void)
{
	int a, b, tmp;
	
  /* initialize array */
	for (a = 1; a < MAX_SKILLS; a++)
		spell_sort_info[a] = a;
	
	for (a = 1; a < MAX_PROFICIENZE; a++)
		proficienze_sort_info[a] = a;
	
  /* Sort.  'a' starts at 1, not 0, to remove 'RESERVED' */
	for (a = 1; a < MAX_SKILLS - 1; a++)
		for (b = a + 1; b < MAX_SKILLS; b++)
			if (strcmp(spells[spell_sort_info[a]], spells[spell_sort_info[b]]) > 0) {
				tmp = spell_sort_info[a];
				spell_sort_info[a] = spell_sort_info[b];
				spell_sort_info[b] = tmp;
			}
			
  /* Sort.  'a' starts at 1, not 0, to remove 'RESERVED' */
	for (a = 1; a < MAX_PROFICIENZE - 1; a++)
		for (b = a + 1; b < MAX_PROFICIENZE; b++)
			if (strcmp(proficienze[proficienze_sort_info[a]], proficienze[proficienze_sort_info[b]]) > 0) {
				tmp = proficienze_sort_info[a];
				proficienze_sort_info[a] = proficienze_sort_info[b];
				proficienze_sort_info[b] = tmp;
			}
}

char *how_good(int percent)
{
	static char buf[256];
	
	if (percent == 0)
		strcpy(buf, " (non imparata)");
	else if (percent <= 10)
		strcpy(buf, " (pessimo)");
	else if (percent <= 20)
		strcpy(buf, " (cattivo)");
	else if (percent <= 40)
		strcpy(buf, " (mediocre)");
	else if (percent <= 55)
		strcpy(buf, " (medio)");
	else if (percent <= 70)
		strcpy(buf, " (discreto)");
	else if (percent <= 80)
		strcpy(buf, " (buono)");
	else if (percent <= 85)
		strcpy(buf, " (molto buono)");
	else
		strcpy(buf, " (ottimo)");
	
	return (buf);
}

char *prac_types[] = {
	"spell",
	"skill"
};

#define LEARNED_LEVEL	0	/* % known which is considered "learned" */
#define MAX_PER_PRAC	1	/* max percent gain in skill per practice */
#define MIN_PER_PRAC	2	/* min percent gain in skill per practice */
#define PRAC_TYPE	3	/* should it say 'spell' or 'skill'?	 */

/* actual prac_params are in class.c */
extern int prac_params[4][NUM_CLASSES];

#define LEARNED(ch) (prac_params[LEARNED_LEVEL][(int)GET_CLASS(ch)])
#define MINGAIN(ch) (prac_params[MIN_PER_PRAC][(int)GET_CLASS(ch)])
#define MAXGAIN(ch) (prac_params[MAX_PER_PRAC][(int)GET_CLASS(ch)])
#define SPLSKL(ch)  (prac_types[prac_params[PRAC_TYPE][(int)GET_CLASS(ch)]])

void list_skills(struct char_data * ch)
{
	extern char *spells[];
	extern struct spell_info_type spell_info[];
	int i, sortpos;
	
	if (!GET_PRACTICES(ch)) strcpy(buf, "Non hai nessuna pratica.\r\n");
	else
		sprintf(buf, "Hai %d sessioni di pratica.\r\n", GET_PRACTICES(ch));
	
	strcat(buf, "Conosci i seguenti spells e skills:\r\n");
	//  sprintf(buf, "%sConosci le seguenti %ss e skills:\r\n", buf, SPLSKL(ch));
	
	strcpy(buf2, buf);
	
	for (sortpos = 1; sortpos < MAX_SKILLS; sortpos++) {
		i = spell_sort_info[sortpos];
		if (strlen(buf2) >= MAX_STRING_LENGTH - 32) {
			strcat(buf2, "**OVERFLOW**\r\n");
			break;
		}
		if (GET_LEVEL(ch) >= spell_info[i].min_level[(int) GET_CLASS(ch)]) {
			sprintf(buf, "%-20s %s\r\n", spells[i], how_good(GET_SKILL(ch, i)));
			strcat(buf2, buf);
		}
	}
	
	page_string(ch->desc, buf2, 1);
}

void list_proficienze(struct char_data * ch)
{
	extern char *proficienze[];
	int i, sortpos;
	
	strcpy(buf2, "Ecco il tuo grado di conoscenza delle proficienze:\r\n");
	
	for (sortpos = 1; sortpos < MAX_PROFICIENZE; sortpos++)
	{
		i = proficienze_sort_info[sortpos];
		if (strlen(buf2) >= MAX_STRING_LENGTH - 32)
		{
			strcat(buf2, "**OVERFLOW**\r\n");
			break;
		}
		if (str_cmp(proficienze[i], "!NONUSATO!")){
			//sprintf(buf,"%-20s %s\r\n",proficienze[i],how_good(GET_PROF(ch, i)));
			sprintf(buf,"%-20s &6%d&0\r\n",proficienze[i],GET_PROF(ch, i));
			strcat(buf2, buf);
		}
	}
	
	page_string(ch->desc, buf2, 1);
}

void list_abilita_guild(struct char_data *ch){
	extern struct abil_info_type abil_info[];
	extern char *abilita[];
	char *title="&2Apprendista&0";
	int i,j, sortpos,liv,can_learn;
	char *choice=" ";
	
	
	if (GET_ABIL_PRACS(ch)==0)
		send_to_char("Non sei pronto per imparare ,ora.\r\n",ch);
	
	if(GET_ABIL_PRACS(ch)>0){
		sprintf(buf, "Hai &1&b%d&0 Punti Abilita da utilizzare.\r\n", GET_ABIL_PRACS(ch));
		send_to_char(buf,ch);
	}
	
	for(i=0,can_learn=0;i<3;i++){
		if((ch)->player_specials->saved.abil_choice[i]==0)
			can_learn++;
	}
	
	if(can_learn!=0){
		sprintf(buf,"Devi scegliere ancora &1&b%d&0 abilita in cui specializzarti qui.\n",can_learn);
		send_to_char(buf,ch);
	}
	strcpy(buf2, "&2Abilita della Gilda&0:\r\n");
	for (sortpos = 1; sortpos < MAX_ABILITA; sortpos++)
	{
      /* i = abilita_sort_info[sortpos];*/
		i=sortpos;
		if (strlen(buf2) >= MAX_STRING_LENGTH - 32)
		{
			strcat(buf2, "**OVERFLOW**\r\n");
			break;
		}
		if (str_cmp(abilita[i], "!NONUSATO!"))
		{
			if(abil_info[i].class_users[(int)GET_CLASS(ch)]==1){
				if((liv=GET_ABIL(ch,i))==15)
					title="&1Maestro&0";
				if(liv<15)
					title="&6Esperto&0";
				if(liv<9)
					title="&2Apprendista&0";
				if(liv<=0)
					title=" ";
				if(abil_info[i].start_abilita[(int)GET_CLASS(ch)]){
					for(j=0;j<3;j++){
						if((ch)->player_specials->saved.abil_choice[j]==i){
							choice="&1*&0";
							break;
						} else choice = " ";
					}
				}
				sprintf(buf,"%s%-20s %2d %s\r\n",choice,abilita[i], GET_ABIL(ch, i),title);
				strcat(buf2, buf);
			}
		}
	}
	page_string(ch->desc, buf2, 1);
}

void list_abilita(struct char_data *ch)
{
	extern struct abil_info_type abil_info[];
	extern char *abilita[];
	int i, sortpos,liv;
	char *title="&2Apprendista&0";
	if (GET_ABIL_PRACS(ch)==0)
		send_to_char("Non sei pronto per imparare ,ora.\r\n",ch);
	if(GET_ABIL_PRACS(ch)>0){
		sprintf(buf, "Hai &1&b%d&0 Punti Abilita da utilizzare.\r\n", GET_ABIL_PRACS(ch));
		send_to_char(buf,ch);
	}
	strcpy(buf2, "Abilita della tua classe\n\r");
	
	for (sortpos = 1; sortpos < MAX_ABILITA; sortpos++)
	{
		i=sortpos;
      /*  i = abilita_sort_info[sortpos];*/
		if (strlen(buf2) >= MAX_STRING_LENGTH - 32)
		{
			strcat(buf2, "**OVERFLOW**\r\n");
			break;
		}
		if (str_cmp(abilita[i], "!NONUSATO!"))
		{
			if(abil_info[i].class_users[(int)GET_CLASS(ch)]==1||GET_LEVEL(ch)>=LVL_IMMORT){
				if((liv=GET_ABIL(ch,i))==15)
					title="&1Maestro&0";
				if(liv<15)
					title="&6Esperto&0";
				if(liv<9)
					title="&2Apprendista&0";
				if(liv<=0)
					title=" ";
				sprintf(buf, "%-20s %2d %s\r\n", abilita[i], GET_ABIL(ch, i),title);
				strcat(buf2, buf);
			}
		}
	}
	
	page_string(ch->desc, buf2, 1);
}
void npc_retreat(struct char_data *thief){
	int i;
	if (!thief)
		return;
	if(GET_POS(thief)<POS_FIGHTING)
		return;
	for(i=0;i<NUM_OF_DIRS;i++){
		if (CAN_GO(thief, i) &&
			!IS_SET(ROOM_FLAGS(EXIT(thief,i)->to_room),ROOM_DEATH)){
			act("&1$n&0  si ritira dal combattimento e fugge.", FALSE, thief, 0, 0,TO_ROOM);
			send_to_char("Ti ritiri dal combattimento e fuggi.\r\n", thief);
			do_simple_move(thief, i, FALSE, CAN_SHOW_ROOM);
			if(FIGHTING(thief))
				stop_fighting(thief);
			return ;
		}
	}
}

void npc_kick(struct char_data *kicker ,struct char_data *kicked,int bravura,int danno){
	bravura+=MIN(100,MAX(50,GET_LEVEL(kicker)*2.5))+dex_app[GET_DEX(kicked)].defensive*1.5;
  /*da 50 a 100*/
	danno+=MIN(60,MAX(5,GET_LEVEL(kicker)))+str_app[GET_STR(kicker)].todam*5;
	if(GET_POS(kicked)==POS_STANDING){
		bravura+=(10-dex_app[GET_DEX(kicked)].reaction);
	}
	else{
		bravura+=5*(POS_FIGHTING - GET_POS(kicked));
		danno *= 1 + (POS_FIGHTING - GET_POS(kicked)) / 3;
	}
	if(number(1,100)>bravura)
		damage(kicker, kicked, 0, SKILL_KICK);
	else{
		damage(kicker, kicked, danno, SKILL_KICK);
		if (RIDING(kicked) && GET_PROF(kicked, PROF_MONTARE)< (number(1, 160)-(GET_PROF(kicked, PROF_MONTARE)/2)))
		{
			act("$n cade da cavallo!", FALSE, kicker, 0, kicked, TO_CHAR);
			send_to_char("Cadi da cavallo!\r\n", kicked);
			dismount_char(kicked);
			GET_POS(kicked) = POS_SITTING;
			//WAIT_STATE(kicked, PULSE_VIOLENCE * 3);
		}
	}
	//WAIT_STATE(kicker, PULSE_VIOLENCE * 3);
}


void npc_bash(struct char_data *ch ,struct char_data *vict,int bravura){
	if(IS_NPC(vict)){
		if (MOB_FLAGGED(vict, MOB_NOBASH))
		{
			damage(ch, vict, 0, SKILL_BASH);
			GET_POS(ch)=POS_SITTING;
			return;
		}
	}
	bravura+=MIN(100,MAX(50,GET_LEVEL(ch)*2.5))+
		dex_app[GET_DEX(vict)].defensive*1.5;
	if(GET_POS(vict)<POS_FIGHTING)
		bravura+=5*(POS_FIGHTING - GET_POS(vict));
	if(number(1,100)<=bravura){
		damage(ch, vict, 1, SKILL_BASH);
    /*if (RIDING(vict)) {
      if (GET_PROF(vict, PROF_MONTARE) < number(1, 100)){
	act("$N cade da cavallo!", FALSE, ch, 0, vict, TO_CHAR);
	send_to_char("&3Cadi da cavallo!&0\r\n", vict);
	dismount_char(vict);
	WAIT_STATE(vict, PULSE_VIOLENCE * 2);
	GET_POS(vict) = POS_SITTING;
      }
      else{
	WAIT_STATE(vict, PULSE_VIOLENCE);
	send_to_char("&3Riesci a rimanere a cavallo.&0\r\n", vict);
      }
    }
    else {*/
		WAIT_STATE(vict, PULSE_VIOLENCE);
		send_to_char("&3Vieni scaraventato a terra.&0\r\n", vict);
		GET_POS(vict) = POS_SITTING;
		return;
	}
	else{
		damage(ch, vict, 0, SKILL_BASH);
		GET_POS(ch)=POS_SITTING;
	}
}

void npc_disarm(struct char_data *ch,struct char_data *vict){
	int bravura=MAX(40,30+(GET_LEVEL(ch)-GET_LEVEL(vict))*5);
	//  dex_app[GET_DEX(vict)].defensive*1.5;
	struct obj_data *weapon=NULL;
	if (!(weapon = GET_EQ(vict, WEAR_WIELD)))
		return;
	if (bravura<number(1,100)) {
		if(number(1,100)>70){
			act("&3&bEviti di farti disarmare da $N con un abile mossa e lo mandi a terra!&0",FALSE,ch,0,vict,TO_VICT);
			act("&3&b$N evita il tuo tentativo ,tu ti sbilanci e cadi per terra!&0",FALSE,ch,0,vict,TO_CHAR);
			act("$N si sbilancia e cade per terra!",FALSE,ch,0,vict,TO_NOTVICT);
			GET_POS(ch)=POS_SITTING;
		}
		else{
			act("Non riesci a disarmare $N!", FALSE, ch, 0, vict, TO_CHAR);
			act("&8$N prova a disarmarti ma eviti il suo tentativo e gli ridi in faccia!&0",FALSE,vict,0,ch,TO_CHAR);
		}
	}
	else {
		obj_to_room(unequip_char(vict, WEAR_WIELD), vict->in_room);
		act("Sei riuscito a disarmare il tuo nemico!", FALSE, ch, 0, 0, TO_CHAR);
		act("$p vola via dalle mani di $N", FALSE, ch,weapon, vict, TO_CHAR);
		act("&1Sei appena stato disarmato da $N, complimenti!&0", FALSE, vict, 0, ch, TO_CHAR);
		act("&1$p&0 vola via dalle tue mani!", FALSE, vict, weapon, 0, TO_CHAR);
		act("&1$n disarma abilmente $N,&3&b$p&0&1 cade a terra.&0", FALSE, ch, weapon,vict, TO_NOTVICT);
	}
}


#define NUM_ARCHERS      8            /* # of rooms archers can shoot from */
#define NUM_TARGETS      3            /* # of rooms an archer can shoot at */
#define HIT_CHANCE       70           /* accuracy 70% chance to hit */
#define ARCHER_NUM_DICE  2            /*  archer damage dice */
#define ARCHER_SIZE_DICE 5            /*  archer does 2d5 each hit */

SPECIAL(archer)
{
	struct char_data *targ;
	int i, j, k;
	int damage;
	void make_corpse(struct char_data * targ);
	void death_cry(struct char_data * targ);
	
	
	room_vnum to_from_rooms[NUM_ARCHERS][NUM_TARGETS + 1] = {
   /* archer room     target room #1     #2       #3 */
		{ 476,              477,          478,     -1},   /* archer room #1 */
		{ 480,              479,          480,     481},
		{ 469,              470,          471,     472},    /* room #2 */
		{ 484,              485,          486,     487},
		{ 485,              484,          486,     487},
		{ 486,              485,          484,     487},
		{ 487,              488,          489,     490},
		{ 489,              490,          488,     487}
	};
	
	char *mssgs[] = {
		"Senti un dolore acuto alla tua spalla come se una freccia di avesse colpito!",
		"Senti un rumore sordo appena la freccia si confica nella spalla di $N!",
		"Una freccia ti passa vicino all'orecchio, mancandoti di poco!",
		"Una freccia manca di poco $N!"
	};
	
	if(cmd)
		return FALSE;
	
	if(GET_POS(ch) != POS_STANDING)
		return FALSE;
	
	for(i = 0; i < NUM_ARCHERS; i++) {
		if(real_room(to_from_rooms[i][0]) == ch->in_room) {
			for(j = 1; j <= NUM_TARGETS; j++) {
				if((k = real_room(to_from_rooms[i][j])) >= 0) {
					for(targ = world[k].people; targ; targ = targ->next_in_room) {
						if(!IS_NPC(targ) && (GET_LEVEL(targ) < LVL_IMMORT)) {
							if(number(1, 100) <= HIT_CHANCE) {
								act(mssgs[0], 1, ch, 0, targ, TO_VICT);
								act(mssgs[1], 1, ch, 0, targ, TO_NOTVICT);
								damage = number(5, 30);
								
								GET_HIT(targ) -= damage + (number(1, 5));
                /*  these above numbers can be changed for different
                 *  damage levels.
                 */
								update_pos(targ);
								
								switch (GET_POS(targ)) {
									case POS_MORTALLYW:
										act("$n e' ferito a morte e morira' presto se non viene aiutato.", TRUE, targ, 0, 0, TO_ROOM);
										send_to_char("Sei ferito a morte e morirai presto se non vieni aiutato.\r\n", targ);
										break;
									case POS_INCAP:
										act("$n e' in agonia... lo aspetta una lenta morte se non viene aiutato.", TRUE, targ, 0, 0, TO_ROOM);
										send_to_char("Sei agonizzante e ti aspetta una lenta morte se non vieni aiutato.\r\n", targ);
										break;
									case POS_STUNNED:
										act("$n e' svenuto, ma riprendera' conoscenza molto probabilmente.", TRUE, targ, 0, 0, TO_ROOM);
										send_to_char("Sei svenuto, ma riprenderai conoscenza molto probabilmente.\r\n", targ);
										break;
									case POS_DEAD:
										act("$n e' morto!  R.I.P.", FALSE, targ, 0, 0, TO_ROOM);
										send_to_char("Sei morto! Mi spiace...\r\n", targ);
										break;
										
									default:
										if (damage > (GET_MAX_HIT(targ) / 4))
											act("Fa VERAMENTE male!", FALSE, targ, 0, 0, TO_CHAR);
								}
/* Help out poor linkless people who are attacked */
								if (!IS_NPC(targ) && !(targ->desc)) {
									do_flee(targ, "", 0, 0);
									if (!FIGHTING(targ)) {
										act("$n viene salvato dalla potenza divina.", FALSE, targ, 0, 0, TO_ROOM);
										GET_WAS_IN(targ) = targ->in_room;
										char_from_room(targ);
										char_to_room(targ, 0);
									}
								}
  /* stop someone from fighting if they're stunned or worse */
								if ((GET_POS(targ) <= POS_STUNNED))
									stop_fighting(targ);
								
  /* Uh oh.  Victim died. */
								
								if (GET_POS(targ) == POS_DEAD) {
									
									
									
									sprintf(buf2, "%s e' stato ucciso dal Grande Arciere a %s", GET_NAME(targ), world[targ->in_room].name);
									
									
									REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER | PLR_THIEF);
									if (FIGHTING(targ))
										stop_fighting(targ);
									
									death_cry(targ);
									
									make_corpse(targ);
									extract_char(targ);
									return FALSE;
								}
								
								
								return TRUE;
							} else {
								act(mssgs[2], 1, ch, 0, targ, TO_VICT);
								act(mssgs[3], 1, ch, 0, targ, TO_NOTVICT);
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}


SPECIAL(cleric)
{
	struct char_data *vict;
	struct char_data *cleric = (struct char_data *) me;
	int cleric_liv=GET_LEVEL(cleric);
	
	if (cmd)
		return FALSE;
	
	if(GET_POS(cleric)<=POS_SITTING)
		return FALSE;
	
	if (GET_POS(cleric) == POS_STANDING){
		if(AFF_FLAGGED(cleric,AFF_BLIND)&&cleric_liv>15){
			cast_spell(cleric, cleric, NULL, SPELL_CURE_BLIND);
			return FALSE;
		}
		if(AFF_FLAGGED(cleric,AFF_CURSE)){
			cast_spell(cleric, cleric, NULL, SPELL_REMOVE_CURSE);
			return FALSE;
		}
		if(AFF_FLAGGED(cleric,AFF_POISON)&&cleric_liv>20){
			cast_spell(cleric, cleric, NULL, SPELL_REMOVE_POISON);
			return FALSE;
		}
		if(!AFF_FLAGGED(cleric,AFF_DETECT_ALIGN)&&cleric_liv>=10){
			cast_spell(cleric,cleric,NULL,SPELL_DETECT_ALIGN);
			return FALSE;
		}
		if(!AFF_FLAGGED(cleric,AFF_SENSE_LIFE)&&cleric_liv>=20){
			cast_spell(cleric,cleric,NULL,SPELL_SENSE_LIFE);
			return FALSE;
		}
		if(!affected_by_spell(cleric, SPELLSKILL,SPELL_ARMOR)&&cleric_liv>=10){
			cast_spell(cleric,cleric,NULL,SPELL_ARMOR);
			return FALSE;
		}
		if(!affected_by_spell(cleric, SPELLSKILL,SPELL_BLESS)){
			cast_spell(cleric,cleric,NULL,SPELL_BLESS);
			return FALSE;
		}
		if(!AFF_FLAGGED(cleric,AFF_SANCTUARY)&&cleric_liv>45){
			cast_spell(cleric,cleric,NULL,SPELL_SANCTUARY);
			return FALSE;
		}
		if(GET_HIT(cleric)<GET_MAX_HIT(cleric)){
			if( cleric_liv>=40){
				cast_spell(cleric, cleric, NULL, SPELL_HEAL);
				return FALSE;
			}
			if(cleric_liv<40&&cleric_liv>=25){
				cast_spell(cleric, cleric, NULL, SPELL_CURE_CRITIC);
				return FALSE;
			}
			if(cleric_liv<25&&cleric_liv>10){
				cast_spell(cleric, cleric, NULL, SPELL_CURE_SERIOUS);
				return FALSE;
			}
			if(cleric_liv<=10){
				cast_spell(cleric, cleric, NULL, SPELL_CURE_LIGHT);
				return FALSE;
			}
		}
		return FALSE;
	}
  /* pseudo-randomly choose someone in the room who is fighting me */
	for (vict = world[cleric->in_room].people; vict; vict = vict->next_in_room)
		if (FIGHTING(vict) == cleric && !number(0, 4))
			break;
		
  /* if I didn't pick any of those, then just slam the guy I'm fighting */
	if (vict == NULL)
		vict = FIGHTING(cleric);
	if(GET_HIT(cleric)<(2*GET_MAX_HIT(cleric)/3)&&number(1,10)>4){/*si cura*/
		if( cleric_liv>=40)
			cast_spell(cleric, cleric, NULL, SPELL_HEAL);
		
		if(cleric_liv<40&&cleric_liv>=25)
			cast_spell(cleric, cleric, NULL, SPELL_CURE_CRITIC);
		
		if(cleric_liv>10&&cleric_liv<25)
			cast_spell(cleric, cleric, NULL, SPELL_CURE_SERIOUS);
		
		if(cleric_liv<=10)
			cast_spell(cleric, cleric, NULL, SPELL_CURE_LIGHT);
		
		if((cleric_liv>=15)&&GET_HIT(cleric)<=GET_MAX_HIT(cleric)/4&&CAN_SEE(cleric,vict)){
			cast_spell(cleric, vict, NULL, SPELL_PEACE);
			do_flee(cleric,0,0,0);
			do_flee(cleric,0,0,0);
			do_flee(cleric,0,0,0);
		}
		return FALSE;
	}
	else{
		if(!CAN_SEE(cleric,vict)){
			if(AFF_FLAGGED(cleric,AFF_BLIND))
				if(number(1,10)>7)
					cast_spell(cleric, cleric, NULL, SPELL_CURE_BLIND);
			if(!AFF_FLAGGED(cleric,AFF_INFRAVISION))
				if(number(1,10)>6)
					cast_spell(cleric, cleric, NULL, SPELL_INFRAVISION);
			return FALSE;
		}
		if(!AFF_FLAGGED(vict,AFF_CURSE)){
			cast_spell(cleric, vict, NULL, SPELL_CURSE);
			return FALSE;
		}
		if(!AFF_FLAGGED(vict,AFF_BLIND)&&GET_LEVEL(cleric)>=20){
			cast_spell(cleric, vict, NULL, SPELL_BLINDNESS);
			return FALSE;
		}
		if(!AFF_FLAGGED(vict,AFF_PARALIZE)&&(cleric_liv>=40)&&(number(1,10)>5)){
			cast_spell(cleric, vict, NULL, SPELL_PARALIZE);
			return FALSE;
		}
		
		switch(number(GET_LEVEL(cleric)/4,GET_LEVEL(cleric))) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				if(number(1,10)<6)/*50 %*/
					cast_spell(cleric, vict, NULL, SPELL_BURNING_HANDS);
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				if(number(1,10)>5){
					cast_spell(cleric, vict, NULL, SPELL_PEACE);
					hit(cleric,vict,TYPE_UNDEFINED);
				}
				else
					cast_spell(cleric, vict, NULL, SPELL_CHILL_TOUCH);
				break;
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
				if(number(1,10)<5)
					cast_spell(cleric, vict, NULL, SPELL_BLADEBARRIER);
				else
				if(number(1,10)<5)
					cast_spell(cleric, vict, NULL, SPELL_EARTHQUAKE);
				break;
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
				if(number(1,10)<5)
					cast_spell(cleric, vict, NULL, SPELL_EARTHQUAKE);
				break;
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
				if (IS_EVIL(cleric)){
					cast_spell(cleric, vict, NULL, SPELL_DISPEL_GOOD);
					break;
				}
				if (IS_GOOD(cleric)){
					cast_spell(cleric, vict, NULL, SPELL_DISPEL_EVIL);
					break;
				}
				if (IS_EVIL(vict)){
					cast_spell(cleric, vict, NULL, SPELL_DISPEL_EVIL);
					break;
				}
				if (IS_GOOD(vict)){
					cast_spell(cleric, vict, NULL, SPELL_DISPEL_GOOD);
					break;
				}
				else{
					cast_spell(cleric, vict, NULL, SPELL_CALL_LIGHTNING);
					break;
				}
			case 31:
			case 32:
			case 33:
				if(number(1,10)<5)
					cast_spell(cleric, vict, NULL, SPELL_CALL_LIGHTNING);
				break;
			case 34:
			case 35:
			case 36:
			case 37:
				if(number(1,10)<5)
					cast_spell(cleric, vict, NULL, SPELL_FLAME_ARROW);
				break;
			default:
				if(number(1,10)<5)
					cast_spell(cleric, vict, NULL, SPELL_HARM);
				break;
		}
	}
	return FALSE;
}

SPECIAL(tokens)
{ /* token machines & conductors in one proc! */
	struct obj_data *o=NULL, *obj, *temptok;
	bool has_token;
	int tokennum, transroom=0, transroom1=0, transroom2=0;
	ACMD(do_look);
	tokennum = 422; /* obj number of your tokens! */
	transroom1 = real_room(499); /* transporter rooms */
	transroom2 = real_room(498);
	
	if (!CMD_IS("buy") && !CMD_IS("compra")) {
		return(FALSE); }
	else if(CMD_IS("buy") || CMD_IS("compra")){
		if (GET_GOLD(ch) < 500) {
			send_to_char("Non hai abbastanza soldi! Un biglietto costa 500 coins.\n\r", ch);
			return(TRUE);
		}
		send_to_char("Hai un biglietto per la carrozza.\n\r", ch);
		act("$n compra un biglietto per la carrozza.",
			FALSE, ch, 0, 0, TO_ROOM);
		GET_GOLD(ch) -= 500;
		temptok = read_object(422, VIRTUAL);
		obj_to_char(temptok, ch);
		return(TRUE);
	} else if (cmd == 2) {
     /* transporter there? */
		if (world[ch->in_room].dir_option[1]->to_room != NOWHERE)
		{
     /* if so, which one? */
			if (world[ch->in_room].dir_option[1]->to_room == transroom1)
				transroom = transroom1;
			else if (world[ch->in_room].dir_option[1]->to_room == transroom2)
				transroom = transroom2;
			has_token = FALSE;
			for (obj = ch->carrying; obj; obj = obj->next_content)
				if (obj_index[obj->item_number].virtual == tokennum) {
					has_token = TRUE;
					o = obj; }
			if (!has_token) {
				send_to_char("Hai bisogno del biglietto per salire sulla carrozza.\n\r", ch);
				return(TRUE);
			}
			else {
				send_to_char("The conductor takes your token, and you enter the transporter\n\r", ch);
				act("The conductor takes $n's token, and $e leaves east.\n\r", FALSE, ch, 0, 0, TO_ROOM);
				obj_from_char(o);
				extract_obj(o);
				char_from_room(ch);
				char_to_room(ch, transroom);
				do_look(ch, "", 0, 0);
				return(TRUE);
			}
		} /* endif transporter is there */
	} /* end if cmd = 2 */
	return(FALSE); /* all other cmds */
} /* end spec proc */

SPECIAL(trainer)
{
	char buf[256];
	bool str, dex, con, intel, wis, cha;
	extern int prac_per_hit;
	extern int prac_per_stat;
	bool hit_train=FALSE;
	
	if (IS_NPC(ch) || !CMD_IS("train"))
		return 0;
	
	if (GET_PRACTICES(ch) < prac_per_hit ) {
		send_to_char("Non hai abbastanza pratiche ora.\r\n", ch);
		return 1;
	}
	
	skip_spaces(&argument);
	
	str = (ch->real_abils.str < 18);
	str = str || ( ch->real_abils.str == 18 && ch->real_abils.str_add < 100 && (IS_WARRIOR(ch) || IS_PELOI(ch)) );
	dex = (ch->real_abils.dex < 18);
	con = (ch->real_abils.con < 18);
	intel = (ch->real_abils.intel < 18);
	wis = (ch->real_abils.wis < 18);
	cha = (ch->real_abils.cha < 18);
	
	if (!*argument) {
		
		send_to_char("Posso insegnarti molte cose, ma dovrai essere disposto a fare un sacrificio.\r\n", ch);
		sprintf(buf, "Ti costa %d pratiche migliorare i tuoi attributi, oppure\r\n", prac_per_stat);
		send_to_char(buf, ch);
		sprintf(buf, "con %d pratiche puoi migliorare di 3 health o mana, o 5 punti movimento.\r\n", prac_per_hit);
		send_to_char(buf, ch);
		send_to_char("Ti e' permesso di migliorare le seguenti stat:\r\n",ch);
		strcpy(buf, "    &1health mana move&0");
		if (GET_PRACTICES(ch) >= prac_per_stat ) {
			if (str)
				strcat(buf, " &1str&0");
			if (dex)
				strcat(buf, " &1dex&0");
			if (con)
				strcat(buf, " &1con&0");
			if (intel)
				strcat(buf, " &1int&0");
			if (wis)
				strcat(buf, " &1wis&0");
			if (cha)
				strcat(buf, " &1cha&0");
		}
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return 1;
	}
	
	if (!strcmp(argument, "health"))
	{ GET_MAX_HIT(ch) += 3; hit_train=TRUE; }
	else if (!strcmp(argument, "mana"))
	{ GET_MAX_MANA(ch) += 3; hit_train=TRUE; }
	else if (!strcmp(argument, "move"))
	{ GET_MAX_MOVE(ch) += 5; hit_train=TRUE; }
	
	if (!hit_train && GET_PRACTICES(ch) < prac_per_stat )
	{
		send_to_char("Non hai abbastanza pratiche ora.\r\n", ch);
		return 1;
	}
	
	
	if (!strcmp(argument, "str") && str)
	{
		if (ch->real_abils.str == 18) {
			if (ch->real_abils.str_add >= 90)
				ch->real_abils.str_add = 100;
			else ch->real_abils.str_add += 10;
		}
		else {
			ch->real_abils.str += 1;
			if (ch->real_abils.str == 18) ch->real_abils.str_add = 0;
		}
	}
	else  if (!strcmp(argument, "dex") && dex)
		ch->real_abils.dex += 1;
	else  if (!strcmp(argument, "con") && con)
		ch->real_abils.con += 1;
	else  if (!strcmp(argument, "int") && intel)
		ch->real_abils.intel += 1;
	else  if (!strcmp(argument, "wis") && wis)
		ch->real_abils.wis += 1;
	else  if (!strcmp(argument, "cha") && cha)
		ch->real_abils.cha += 1;
	else {
		send_to_char("Non puoi imparare ora.\r\n", ch);
		return 1;
	}
	if (hit_train) GET_PRACTICES(ch) -= prac_per_hit;
	else GET_PRACTICES(ch) -= prac_per_stat;
	
	affect_total(ch);
	send_to_char("Congratulazioni, hai migliorato le tue capacita'.\r\n", ch);
	return 1;
}


SPECIAL(slot)
/*void play_slots(struct char_data *ch)*/
{
	int num1, num2, num3, win = 0;
	char *slot_msg[] = {
		"*YOU SHOULDN'T SEE THIS*",
		"&R&5una mela&0",              /* 1 */
		"&R&4un dragone d'oro&0",
		"&R&3un martello dei nani&0",
		"&R&2un tempio&0",
		"&R&1un arco&0",               /* 5 */
		"&5un boccale&0",
		"&4una bottiglia&0",
		"&3una pagnotta&0",
		"&2una campana&0",
		"&1un mendicante&0",                   /* 10 */
	};
	
	if(CMD_IS("pull") || CMD_IS("tira")){
		if (GET_GOLD(ch) < 1) {
			send_to_char("Non hai abbastanza denaro per poter giocare alla slot machine!\r\n", ch);
			return 1;
		} else
			GET_GOLD(ch) -= 50;
		
		act("$n tira la leva della slot machine.",
			TRUE, ch, 0, FALSE, TO_ROOM);
		send_to_char("Tiri la leva della slot machine.\r\n", ch);
		
     /* very simple roll 3 random numbers from 1 to 10 */
		num1 = number(1, 10);
		num2 = number(1, 10);
		num3 = number(1, 10);
		
		if (num1 == num2 && num2 == num3) {
	/* all 3 are equal! Woohoo! */
			if (num1 == 1)
				win += 5000;
			else if (num1 == 2)
				win += 2500;
			else if (num1 == 3)
				win += 1500;
			else if (num1 == 4)
				win += 1000;
			else if (num1 == 5)
				win += 500;
			else if (num1 == 10)
				win += 1;
		}
		
		sprintf(buf, "Ottieni %s, %s, %s, ", slot_msg[num1],
			slot_msg[num2], slot_msg[num3]);
		if (win > 1)
			sprintf(buf+strlen(buf), "vinci %d monete !\r\n", win);
		else if (win == 1)
			strcat(buf, "vinci una moneta 1 !\r\n");
		else
			strcat(buf, "perdi.\r\n");
		send_to_char(buf, ch);
		GET_GOLD(ch) += win;
		
		return 1;
	}
	else return 0;
}


/*
SPECIAL(trainer) {
	return do_train(ch, me, cmd, argument);
return 1;
}*/

SPECIAL(guild)
{
	int skill_num, percent;
	
	extern struct spell_info_type spell_info[];
	extern struct int_app_type int_app[];
	
	if (IS_NPC(ch) || !CMD_IS("practice"))
		return 0;
	
	skip_spaces(&argument);
	
	if (!*argument)
	{
		list_skills(ch);
		return 1;
	}
	
	if (GET_PRACTICES(ch) <= 0)
	{
		send_to_char("Non sembri pronto per imparare ora.\r\n", ch);
		return 1;
	}
	
	if (GET_LEVEL(ch) > GET_LEVEL((struct char_data *) me))
	{
		send_to_char("Sei piu' esperto di me, dovrai trovarti un altro Maestro.\r\n", ch);
		return 1;
	}
	
	skill_num = find_skill_num(argument);
	
	if (skill_num < 1 || GET_LEVEL(ch) < spell_info[skill_num].min_level[(int) GET_CLASS(ch)])
	{
		sprintf(buf, "Non conosci niente di quello %s.\r\n", SPLSKL(ch));
		send_to_char(buf, ch);
		return 1;
	}
	
	if (GET_SKILL(ch, skill_num) >= LEARNED(ch))
	{
		send_to_char("Hai gia' imparato tutto in quell'area.\r\n", ch);
		return 1;
	}
	
	if (GET_SKILL(ch, skill_num) >= 70)
	{
		send_to_char("Puoi ancora migliorare... ma solo con l'esercizio costante.\r\n", ch);
		return 1;
	}
	
	send_to_char("Ascolti gli insegnamenti del tuo Maestro...\r\n", ch);
	GET_PRACTICES(ch)--;
	percent = GET_SKILL(ch, skill_num);
	percent += MAX(MAXGAIN(ch), MAX(MINGAIN(ch), int_app[GET_INT(ch)].learn));
	SET_SKILL(ch, skill_num, MIN(MIN(LEARNED(ch), 70) , percent) );
	
	if (GET_SKILL(ch, skill_num) >= LEARNED(ch)) send_to_char("Hai imparato tutto in quell'area.\r\n", ch);
	else if (GET_SKILL(ch, skill_num) >= 70)
		send_to_char("Hai raggiunto la massima conoscenza teorica in quell'area... ora ti aspetta la pratica.\r\n", ch);
	
	return 1;
}

SPECIAL(abil_guild)
{
	int abil_num,liv,liv_abil,found=0,can_learn=-1,i;
	void update_abilita(struct char_data *ch,int abilita,int new_liv);
	extern struct abil_info_type abil_info[];
	extern char *abilita[];
	if (IS_NPC(ch) ||( !CMD_IS("learn") && !CMD_IS("impara")))
		return 0;
	if (CMD_IS("learn") || CMD_IS("impara")){
		
		skip_spaces(&argument);
		
		if (!*argument)
		{
			list_abilita_guild(ch);
			return 1;
		}
		
		abil_num = find_abil_num(argument);
		sprintf(buf,"ABIL NUM:%d\n",abil_num);
		send_to_char(buf,ch);
		if (abil_num < 1 || abil_num>TOP_ABILITA_DEFINE ||abil_info[abil_num].class_users[(int)GET_CLASS(ch)]!=1)
		{
			sprintf(buf, "Non puoi imparare nulla di questa abilita.\r\n");
			send_to_char(buf, ch);
			return 1;
		}
		liv=GET_LEVEL(ch);
		
		if ((liv_abil=GET_ABIL(ch, abil_num)) ==15)
		{
			send_to_char("Sei Gia MAESTRO in quest abilita!.\r\n", ch);
			return 1;
		}
		if(liv_abil==8)
			if(liv<20){
				send_to_char("Sei ancora troppo inesperto per diventare ESPERTO!\n",ch);
				send_to_char("Ripassa quando avrai raggiuto il 20imo livello di esperienza.\n",ch);
				return 1;
			}
		if(liv_abil==14)
			if(liv<40){
				send_to_char("Sei ancora troppo inesperto per diventare MAESTRO!\n",ch);
				send_to_char("Ripassa quando avrai raggiuto il 40imo livello di esperienza.\n",ch);
				return 1;
			}
		if (GET_ABIL_PRACS(ch) < liv_abil+1)
		{
			send_to_char("Non sembri pronto per imparare ora.\r\n", ch);
			return 1;
		}
		
		
    /*cerca se hai gia scelto le 3 abilita*/
		for(i=0;i<3;i++){
			if((ch)->player_specials->saved.abil_choice[i]==0)
				can_learn=i;
			if((ch)->player_specials->saved.abil_choice[i]==abil_num){
				found=abil_num;
				break;
			}
		}
		if(found!=abil_num){
			if(can_learn !=-1){/*cioe se puo ancora imparare una abilita*/
				(ch)->player_specials->saved.abil_choice[can_learn]=abil_num;
				sprintf(buf,"Bene ,Iniziamo le lezioni di %s.\n",argument);
				send_to_char(buf,ch);
			}
			else{
				send_to_char("Mi spiace,non hai scelto di specializzarti in questa abilita.\n",ch);
				return 1;
			}
		}
		
		send_to_char("Ascolti gli insegnamenti del tuo Maestro...\r\n", ch);
		GET_ABIL_PRACS(ch)-=(liv_abil+1);
		update_abilita(ch,abil_num,liv_abil+1);
		//SET_ABIL(ch, abil_num, liv_abil+1);
		
		if (GET_ABIL(ch, abil_num)==EXPERT_LIV)
			send_to_char("Bene,ora sei un vero &1ESPERTO&0 in questa abilita.\r\n", ch);
		if (GET_ABIL(ch, abil_num)==MASTER_LIV) {
			send_to_char("Ti ho insegnato tutto quello che sapevo in questa abilita!.\r\n", ch);
			sprintf(buf,"Ora sei &1&bMaestro&0 di %s\n",abilita[abil_num]);
			send_to_char(buf,ch);
		}
	}
	return 1;
}


SPECIAL(dump)
{
	struct obj_data *k;
	int value = 0;
	
	ACMD(do_drop);
	char *fname(char *namelist);
	
	for (k = world[ch->in_room].contents; k; k = world[ch->in_room].contents) {
		act("$p svanisce in una nuvola di fumo!", FALSE, 0, k, 0, TO_ROOM);
		extract_obj(k);
	}
	
	if (!CMD_IS("drop"))
		return 0;
	
	do_drop(ch, argument, cmd, 0);
	
	for (k = world[ch->in_room].contents; k; k = world[ch->in_room].contents) {
		act("$p vanishes in a puff of smoke!", FALSE, 0, k, 0, TO_ROOM);
		value += MAX(1, MIN(100, GET_OBJ_COST(k) / 10));
		extract_obj(k);
	}
	
	if (value) {
		act("Sei stato premiato per la tua azione.", FALSE, ch, 0, 0, TO_CHAR);
		act("$n e' stato premiato essere un buon cittadino.", TRUE, ch, 0, 0, TO_ROOM);
		
		if (GET_LEVEL(ch) < 3)
			gain_exp(ch, value);
		else
			GET_GOLD(ch) += value;
	}
	return 1;
}
#define ASSASSIN_PRICE(as) (GET_LEVEL(as) * 100000)

/*SPECIAL(kriss)
{
struct char_data *vict;
struct obj_data *weap = GET_EQ(ch, WEAR_WIELD);
if (GET_POS(ch) == POS_FIGHTING && GET_POS(vict)==POS_FIGHTING) {
if (weap==21011)
  if (!AFF_FLAGGED(vict, AFF_PARALIZE))
  cast_spell(ch, vict, NULL, SPELL_PARALIZE);

return TRUE;
}
else return FALSE;
}
*/
SPECIAL(assassin)
{
	int as_loc;
	struct char_data *as, *vict;
	
	as_loc = ch->in_room + 1;
	
	if (CMD_IS("lista")) {
		send_to_char("Available assassins are:\r\n", ch);
		for (as = world[as_loc].people;as;as = as->next_in_room) {
			sprintf(buf, "%8d - %s\r\n", ASSASSIN_PRICE(as), GET_NAME(as));
			send_to_char(buf, ch);
		}
		return 1;
	} else if (CMD_IS("hire")) {
		two_arguments(argument, buf, buf2);
		if(!*buf) {
			send_to_char("Hire who?\r\n", ch);
			return 1;
		}
		if (!(as = get_char_room(buf, as_loc))) {
			send_to_char("Non c'e' nessuno con quel nome!\r\n", ch);
			return 1;
		}
		if(!IS_NPC(as)) {
			send_to_char("STAI FUORI DA QUELLA STANZA, ORA !!!\r\n", as);
			sprintf(buf, "%s e' nella camera dell'assassino.\r\n", GET_NAME(as));
			mudlog(buf, BRF, LVL_IMMORT, TRUE);
			send_to_char("Non puoi assoldare giocatori.\r\n", ch);
			return 1;
		}
		if (!*buf2) {
			send_to_char("Chi vuoi morto?\r\n", ch);
			return 1;
		}
		if (GET_GOLD(ch) < ASSASSIN_PRICE(as)) {
			send_to_char("You don't have enough gold!\r\n", ch);
			return 1;
		}
		if ((vict = get_player_vis(as, buf2, 0))) {
			GET_GOLD(ch) -= ASSASSIN_PRICE(as);
			as = read_mobile(GET_MOB_RNUM(as), REAL);
			char_to_room(as, ch->in_room);
			SET_BIT(MOB_FLAGS(as), MOB_ASSASSIN);
			SET_BIT(MOB_FLAGS(as), MOB_HUNTER);
			remember(as, vict);
			HUNTING(as) = vict;
			ASSASSIN(vict) = as;
			SET_BIT(AFF_FLAGS(vict), AFF_MARK);
			send_to_char("Non ti possiamo contattare sul risultato dell'azione..."
				" ragioni di sicurezza capisci.\r\n", ch);
			act("$n assolda $N per un lavoretto.", FALSE, ch, 0, as, TO_ROOM);
			sprintf(buf, "&+R%s assolda %s per uccidere %s.&+w\r\n", GET_NAME(ch),
				GET_NAME(as), GET_NAME(vict));
			mudlog(buf, BRF, LVL_IMMORT, TRUE);
			return 1;
		} else {
			send_to_char("L'assassino non trova il giocatore!\r\n", ch);
			return 1;
		}
	}
	return 0;
}

SPECIAL(mayor)
{
	ACMD(do_gen_door);
	
	static char open_path[] =
		"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";
	
	static char close_path[] =
		"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";
	
	static char *path;
	static int index;
	static bool move = FALSE;
	
	if (!move) {
		if (time_info.hours == 6) {
			move = TRUE;
			path = open_path;
			index = 0;
		} else if (time_info.hours == 20) {
			move = TRUE;
			path = close_path;
			index = 0;
		}
	}
	if (cmd || !move || (GET_POS(ch) < POS_SLEEPING) ||
		(GET_POS(ch) == POS_FIGHTING))
		return FALSE;
	
	switch (path[index]) {
		case '0':
		case '1':
		case '2':
		case '3':
			perform_move(ch, path[index] - '0', 1,CAN_SHOW_ROOM);
			break;
			
		case 'W':
			GET_POS(ch) = POS_STANDING;
			act("$n sbadiglia ed emette forti rumori.", FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'S':
			GET_POS(ch) = POS_SLEEPING;
			act("$n si sdraia e si addormenta.", FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'a':
			act("$n dice 'Ciao caro!'", FALSE, ch, 0, 0, TO_ROOM);
			act("$n fa' l'occhiolino.", FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'b':
			act("$n says 'Che brutta vista!  Devo far fare qualcosa di quella discarica!'",
				FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'c':
			act("$n dice 'Vandali!  I giovani d'oggi non hanno rispetto di niente!'",
				FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'd':
			act("$n says 'Buongiorno, cittadini!'", FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'e':
			act("$n says 'Dichiaro ufficialmente i negozi aperti!'", FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'E':
			act("$n says 'Dichiaro ufficialmente la citta' aperta!'", FALSE, ch, 0, 0, TO_ROOM);
			break;
			
		case 'O':
			do_gen_door(ch, "gate", 0, SCMD_UNLOCK);
			do_gen_door(ch, "gate", 0, SCMD_OPEN);
			break;
			
		case 'C':
			do_gen_door(ch, "gate", 0, SCMD_CLOSE);
			do_gen_door(ch, "gate", 0, SCMD_LOCK);
			break;
			
		case '.':
			move = FALSE;
			break;
			
	}
	
	index++;
	return FALSE;
}


/* ********************************************************************
*  General special procedures for mobiles                             *
******************************************************************** */
void npc_backstab(struct char_data *ch,struct char_data *vict){
	struct obj_data *weapon=GET_EQ(ch,WEAR_WIELD);
	int liv=MIN(99,50+GET_LEVEL(ch));
	bool can_back=TRUE;
	if(!ch||!vict)
		return ;
	if(FIGHTING(ch))
		return;
	if(!weapon)
		can_back=FALSE;
	if(weapon)
		if(GET_OBJ_VAL(weapon,3)!=1&&GET_OBJ_VAL(weapon,3)!=14){
			send_to_char("Devi avere un pugnale,per farlo!",ch);
			can_back=FALSE;
		}
	if(can_back==TRUE){
		hit(ch, vict, SKILL_BACKSTAB);
		return;
	}
	if (FIGHTING(vict)) {
		send_to_char("Non puoi farlo su una persona che sta combattendo-- e' troppo allertato!\r\n", ch);
		return;
	}
	if (IS_NPC(vict))
		if(MOB_FLAGGED(vict, MOB_AWARE)) {
			act("Ti accorgi che  $N sta affondando un'arma su di te!", FALSE, vict, 0, ch, TO_CHAR);
			act("$e ti nota affondare l'arma su di $m!", FALSE, vict, 0, ch, TO_VICT);
			act("$n si accorge che $N sta affondando l'arma su di $m!", FALSE, vict, 0, ch, TO_NOTVICT);
			hit(vict, ch, TYPE_UNDEFINED);
			return ;
		}
		
	if ((AWAKE(vict) || number(1,100)>liv)&&can_back==TRUE)
		damage(ch, vict, 0, SKILL_BACKSTAB);
	else{
		hit(ch, vict, TYPE_UNDEFINED);
	}
}

void npc_steal(struct char_data * ch, struct char_data * victim)
{
	int gold;
	int level=50+GET_LEVEL(ch);
	if (IS_NPC(victim))
		return;
	if(!CAN_SEE(ch,victim))
		return;
	if (GET_LEVEL(victim) >= LVL_IMMORT)
		return;
	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
		return;
	if(TRANSPORTED_BY(ch))
		return;
//	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL))
//		return;
	
	if (AWAKE(victim) && (number(1,100)>level)) {
		act("Scopri che $n ha $s le mani nelle tue tasche.", FALSE, ch, 0, victim, TO_VICT);
		act("$n tenta di rubare dei soldi da $N.", TRUE, ch, 0, victim, TO_NOTVICT);
	} else {
    /* Steal some gold coins */
		gold = (int) ((GET_GOLD(victim) * number(1, 30)) / 100);
		if ((gold > 0)&&GET_GOLD(victim) >= gold) {
			GET_GOLD(ch) += gold;
			GET_GOLD(victim) -= gold;
		}
	}
}


SPECIAL(snake)
{
	if (cmd)
		return FALSE;
	
	if (GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	if (FIGHTING(ch)){
		if (AFF_FLAGGED(FIGHTING(ch), AFF_PARALIZE))
			return FALSE;
	}
	
	if (FIGHTING(ch) && (FIGHTING(ch)->in_room == ch->in_room) &&
		(number(0, 42 - GET_LEVEL(ch)) == 0)) {
		act("$n &2&bmorde&0 $N!", 1, ch, 0, FIGHTING(ch), TO_NOTVICT);
		act("$n ti &2&bmorde&0!", 1, ch, 0, FIGHTING(ch), TO_VICT);
		call_magic(ch, FIGHTING(ch), 0, SPELL_POISON, GET_LEVEL(ch), CAST_SPELL);
		return TRUE;
	}
	return FALSE;
}

SPECIAL(thief)
{
	struct char_data *cons,*vict;
	struct char_data *thief = (struct char_data *) me;
	bool found=FALSE;
	int amount,i;
	memory_rec *names;
	if(FIGHTING(thief)){
		if(GET_HIT(thief)<GET_MAX_HIT(thief)/3){
			if(number(1,10)>7){
				for(i=0;i<NUM_OF_DIRS;i++){
					if (CAN_GO(thief, i) &&
						!IS_SET(ROOM_FLAGS(EXIT(thief,i)->to_room),ROOM_DEATH))
					{
						act("&1$n&0 si ritira dal combattimento e fugge.", TRUE, thief, 0, 0,
							TO_ROOM);
						send_to_char("Ti ritiri dal combattimento e fuggi.\r\n", thief);
						do_flee(thief, "", 0, 0);
						return FALSE;
					}
				}
			}
		}
		return FALSE;
	}
	if (MOB_FLAGGED(thief, MOB_MEMORY) && MEMORY(thief)) {
		for (vict = world[thief->in_room].people; vict && !found; vict = vict->next_in_room) {
			if (!CAN_SEE(thief, vict) || PRF_FLAGGED(vict, PRF_NOHASSLE))
				continue;
			for (names = MEMORY(thief); names && !found; names = names->next)
				if (names->id == GET_IDNUM(vict)) {
					found = TRUE;
					act("&1&b$n&0 ti urla da dietro le spalle '&1MUORI BASTARDO!&0'",FALSE,thief,0,vict,TO_VICT);
					act("&1&b$n&0  urla  '&1MUORI BASTARDO!&0'",FALSE,thief,0,vict,TO_NOTVICT);
					npc_backstab(thief,vict);
					return FALSE;
				}
		}
	}
	if(cmd){
		if(!CMD_IS("ladro"))
			return FALSE;
		if(CMD_IS("ladro") && IS_SET(PLR_FLAGS(ch), PLR_THIEF)) {
			if ((amount = atoi(argument)) <= 0) {
				send_to_char("Quanto vuoi cambiare?\r\n", ch);
				return FALSE;
			}
			if (GET_GOLD(ch) < amount) {
				send_to_char("Non hai quei coins!\r\n", ch);
				return FALSE;
			}
			GET_GOLD(ch) -= amount;
			gain_exp(ch, amount/2);
			sprintf(buf, "%s, siamo i migliori in questo mondo.\r\n", GET_NAME(ch));
			send_to_char(buf, ch);
			act("Si percepisce un'aria di intesa...niente piu'.", TRUE, ch, 0, FALSE, TO_ROOM);
			return TRUE;
		}
	}
	if (GET_POS(thief) != POS_STANDING)
		return FALSE;
	
	for (cons = world[thief->in_room].people; cons; cons = cons->next_in_room)
		if (!IS_NPC(cons) && (GET_LEVEL(cons) < LVL_IMMORT)) {
			npc_steal(thief, cons);
			return FALSE;
		}
	return FALSE;
}


SPECIAL(untore)
{
	struct char_data *vict;

	if (cmd)
		return FALSE;

	for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room) {
		if (affected_by_spell (vict, SPELLSKILL, DISEASE_PESTE) || GET_LEVEL(vict) >= LVL_QUEST_MASTER || IS_NPC(vict))
			continue;
		if (GET_WIS(vict) > number(13, 18)){
			if (number (0, 2))
				act ("$n compie degli strani gesti alle tue spalle...", FALSE, ch, 0, vict, TO_VICT);
			else
				act ("Vedi $n cospargere un'ignobile pomata sui tuoi vestiti!", FALSE, ch, 0, vict, TO_VICT);
		}
		if (GET_CON(vict) > number(15, 20))
			continue;
		mag_affects(70, ch, vict, DISEASE_PESTE, 0);
	}

	return FALSE;
}


SPECIAL(magic_user)
{
	struct char_data *vict;
	struct char_data *mage = (struct char_data *) me;
	int mage_liv=GET_LEVEL(mage);
	
	
	if (cmd)
		return FALSE;
	
	if (GET_POS(mage) <=POS_SITTING)
		return FALSE;
	
	if (GET_POS(mage) == POS_STANDING){
		if(!AFF_FLAGGED(mage,AFF_DETECT_MAGIC)){
			cast_spell(mage,mage,NULL,SPELL_DETECT_MAGIC);
			return FALSE;
		}
		if(!AFF_FLAGGED(mage,AFF_DETECT_INVIS)&&mage_liv>=15){
			cast_spell(mage,mage,NULL,SPELL_DETECT_INVIS);
			return FALSE;
		}
		if(!AFF_FLAGGED(mage,AFF_INFRAVISION)&&mage_liv>=10){
			cast_spell(mage,mage,NULL,SPELL_INFRAVISION);
			return FALSE;
		}
		if(!AFF_FLAGGED(mage,AFF_DETECT_ALIGN)&&mage_liv>=25){
			cast_spell(mage,mage,NULL,SPELL_DETECT_ALIGN);
			return FALSE;
		}
		if(!AFF_FLAGGED(mage,AFF_SENSE_LIFE)&&mage_liv>=30){
			cast_spell(mage,mage,NULL,SPELL_SENSE_LIFE);
			return FALSE;
		}
		if(!AFF_FLAGGED(mage,AFF_SHIELD)){
			cast_spell(mage,mage,NULL,SPELL_SHIELD);
			return FALSE;
		}
		if(!affected_by_spell(mage, SPELLSKILL, SPELL_STONESKIN)&&mage_liv>=30){
			cast_spell(mage,mage,NULL,SPELL_STONESKIN);
			return FALSE;
		}
		if(!affected_by_spell(mage, SPELLSKILL, SPELL_ARMOR)&&mage_liv>=10){
			cast_spell(mage,mage,NULL,SPELL_ARMOR);
			return FALSE;
		}
		if(!affected_by_spell(mage, SPELLSKILL, SPELL_STRENGTH)&&mage_liv>=7){
			cast_spell(mage,mage,NULL,SPELL_STRENGTH);
			return FALSE;
		}
		if(!AFF_FLAGGED(mage,AFF_INVISIBLE)&&mage_liv>=15&&(GET_HIT(mage)<=2*GET_MAX_HIT(mage)/3)&&!FIGHTING(ch)){
			cast_spell(mage,mage,NULL,SPELL_INVISIBLE);
			return FALSE;
		}
		if(!affected_by_spell(mage, SPELLSKILL,SPELL_MIRROR_IMAGE)&&mage_liv>=15){
			cast_spell(mage,mage,NULL,SPELL_MIRROR_IMAGE);
			return FALSE;
		}
		if(!affected_by_spell(mage, SPELLSKILL,SPELL_BLINK)&&mage_liv>=25){
			cast_spell(mage,mage,NULL,SPELL_BLINK);
			return FALSE;
		}
		if(!affected_by_spell(mage, SPELLSKILL,SPELL_FIRESHD)&&mage_liv>=45){
			cast_spell(mage,mage,NULL,SPELL_FIRESHD);
			return FALSE;
		}
		return FALSE;
	}
  /* pseudo-randomly choose someone in the room who is fighting me */
	for (vict = world[mage->in_room].people; vict; vict = vict->next_in_room)
		if (FIGHTING(vict) == mage && !number(0, 4))
			break;
		
  /* if I didn't pick any of those, then just slam the guy I'm fighting */
		
	if (FIGHTING(mage)){
		if (AFF_FLAGGED(FIGHTING(mage), AFF_PARALIZE))
			return FALSE;
	}
	if (vict == NULL)
		vict = FIGHTING(mage);
	if(!CAN_SEE(mage,vict))
		return FALSE;
	if(number(1,10)<=5){
		if ( (mage_liv>= 20) && (number(1,10)>3) &&!AFF_FLAGGED(vict,AFF_BLIND)){
			cast_spell(mage, vict, NULL, SPELL_BLINDNESS);
			return FALSE;
		}
		if ( (mage_liv>=15) && (number(1,10)>3) &&!AFF_FLAGGED(vict,AFF_POISON)){
			cast_spell(mage, vict, NULL, SPELL_POISON);
			return FALSE;
		}
		if ( (mage_liv>= 35) && (number(1,10)>7) ){
			cast_spell(mage, vict, NULL, SPELL_DISINTEGRATE);
			return FALSE;
		}
		if ( (mage_liv>= 40) && (number(1,10)>5) &&!AFF_FLAGGED(vict,AFF_PARALIZE))
		{
			cast_spell(mage, vict, NULL, SPELL_PARALIZE);
			return FALSE;
		}
		if ( (mage_liv>= 15) && (number(1,10)>6) ){
			cast_spell(mage, vict, NULL, SPELL_SLEEP);
			return FALSE;
		}
	}
	switch (number(GET_LEVEL(mage)/2-1,GET_LEVEL(mage))) {
		case 2:
		case 3:
		case 4:
		case 5:
			cast_spell(mage, vict, NULL, SPELL_MAGIC_MISSILE);
			break;
		case 6:
		case 7:
		case 8:
			cast_spell(mage, vict, NULL, SPELL_CHILL_TOUCH);
			break;
		case 9:
		case 10:
		case 11:
			cast_spell(mage, vict, NULL, SPELL_BURNING_HANDS);
			break;
		case 12:
		case 13:
			cast_spell(mage, vict, NULL, SPELL_SHOCKING_GRASP);
			break;
		case 14:
		case 15:
			cast_spell(mage, vict, NULL, SPELL_ACID_ARROW);
			break;
		case 16:
		case 17:
		case 18:
			cast_spell(mage, vict, NULL, SPELL_FLAME_ARROW);
			break;
		case 19:
		case 20:
		case 21:
			cast_spell(mage, vict, NULL, SPELL_COLOR_SPRAY);
			break;
		case 22:
		case 23:
		case 24:
			cast_spell(mage, vict, NULL, SPELL_AREA_LIGHTNING);
		case 25:
		case 26:
		case 27:
			cast_spell(mage, vict, NULL, SPELL_LIGHTNING_BOLT);
			break;
		case 28:
		case 29:
		case 30:
			cast_spell(mage, vict, NULL, SPELL_ENERGY_DRAIN);
		case 31:
		case 32:
		case 33:
			cast_spell(mage, vict, NULL, SPELL_FIREBALL);
			break;
		default:
			cast_spell(mage, vict, NULL,SPELL_LIGHTNING_BREATH);
	}
	return FALSE;
}
/*Blizzard:Warrior classico Bash ,Kick,disarm ,retreate,
  A liv alti casta:,Fear,barkskin e armor.        */
SPECIAL(warrior){
	struct char_data *vict;
	struct char_data *warrior = (struct char_data *) me;
	int warrior_liv=GET_LEVEL(warrior);
	
	if (cmd)
		return FALSE;
	
	if(GET_POS(warrior)==POS_SITTING || AFF_FLAGGED(ch, AFF_PARALIZE))
		return FALSE;
	
	if(GET_POS(warrior)!=POS_FIGHTING && GET_POS(warrior)>POS_STUNNED){
		if(GET_HIT(warrior)<GET_MAX_HIT(warrior)&& warrior_liv>30){
			cast_spell(warrior,warrior,NULL,SPELL_CURE_LIGHT);
			return FALSE;
		}
		if(warrior_liv>15&&!AFF_FLAGGED(warrior,AFF_SHIELD)){
			cast_spell(warrior,warrior,NULL,SPELL_SHIELD);
			return FALSE;
		}
		if(warrior_liv>20&&!affected_by_spell(warrior, SPELLSKILL, SPELL_BARKSKIN)){
			cast_spell(warrior,warrior,NULL,SPELL_BARKSKIN);
			return FALSE;
		}
		if(warrior_liv>30&&!affected_by_spell(warrior, SPELLSKILL, SPELL_ARMOR)){
			cast_spell(warrior,warrior,NULL,SPELL_ARMOR);
			return FALSE;
		}
		return FALSE;
	}
	
  /* if I didn't pick any of those, then just slam the guy I'm fighting */
	
	if (FIGHTING(warrior)){
		if (AFF_FLAGGED(FIGHTING(warrior), AFF_PARALIZE))
			return FALSE;
		vict = FIGHTING(warrior);
		if(!vict)
			return FALSE;
		if(GET_HIT(warrior)<GET_MAX_HIT(warrior)/4){
			if(warrior_liv>20){
				if(number(1,10)>4)
					cast_spell(warrior,vict,NULL,SPELL_FEAR);
				else
				if(MOB_FLAGGED(warrior,MOB_WIMPY))
					npc_retreat(warrior);
			}
			else
			if(GET_HIT(warrior)<10)
				npc_retreat(warrior);
			return FALSE;
		}
		//   if(!CAN_SEE(warrior,vict))
		//return FALSE;
		if(GET_EQ(vict,WEAR_WIELD)){
			if(number(1,10)>7&&CAN_SEE(warrior,vict)){
				npc_disarm(warrior,vict);
				return FALSE;
			}
		}
		if(number(1,10)>5){
			if(number(1,10)>5 && GET_POS(vict)!=POS_SITTING)
				npc_bash(warrior,vict,10);
			else
				npc_kick(warrior,vict,10,5);
			return FALSE;
		}
	}
	return FALSE;
}
/* ********************************************************************
*  Special procedures for mobiles                                      *
******************************************************************** */
void random_insult(struct char_data *ch,struct char_data *vict){
	switch(number(0,10)){
		case 0:act("$n impreca 'Cento frustate , $N , non te le leva nessuno!",FALSE,ch,0,vict,TO_ROOM);return;
		case 1:act("$n grida 'Bastardo ,ora ti sbatto in prigione!", FALSE, ch, 0, 0, TO_ROOM);return;
		case 2:act("$n grida 'Finalmente avrai quello che ti meriti,criminale!", FALSE, ch, 0, 0, TO_ROOM);return;
		case 3:act("$n dice 'Vedrai che bel trattamento avrai!", FALSE, ch, 0, 0, TO_ROOM);return;
		case 4:act("$n sbraita 'Infrangere la legge non paga mai!", FALSE, ch, 0,0, TO_ROOM);return;
		case 5:act("$n ghigna 'Mi sa che stavolta avro la promozione!", FALSE, ch, 0,
				0, TO_ROOM);return;
		case 6:act("$n impreca 'I Bastardi come te, $N , andrebbero uccisi da piccoli!",FALSE,ch,0,vict,TO_ROOM);
		case 7:act("$n grida 'Finalmente, $N , ti abbiamo preso",FALSE,ch,0,vict,TO_ROOM);return;
		case 8:act("$n ghigna 'Ora pagherai per quello che hai fatto, $N!",FALSE,ch,0,vict,TO_ROOM);return;
		case 9:act("$n dice 'Io personalmente, $N , ti avrei ucciso ,ma..",FALSE,ch,0,vict,TO_ROOM);return;
		case 10:act("$n impreca 'Ti assicuro che non uscirai vivo,$N,dalla cella!",FALSE,ch,0,vict,TO_ROOM);return;
	}
}

SPECIAL(guild_guard)
{
	extern void ingresso_ordine (struct char_data *ch, int classe);
	extern int costo_iniziazione;
	extern struct nomi_titolanti *pg_titolanti;
	extern int num_titolanti;
	
	int i;
	extern int guild_info[][3];
	struct char_data *guard = (struct char_data *) me;
	int classe = -1;
	FILE *titolanti_f;
	
	if (CMD_IS("iniziazione"))
	{
		if (IS_NPC(ch))
		{
			send_to_char("I mob non possono diventare dei veri eroi...\r\n", ch);
			return TRUE;
		}
		if (IS_AFFECTED(guard, AFF_BLIND))
		{
			send_to_char("Se potessi vederti, ti aiuterei...\r\n", ch);
			return TRUE;
		}
		for (i = 0; (guild_info[i][0] != -1 && classe == -1); i++)
			if (world[ch->in_room].number == guild_info[i][1])
				classe = guild_info[i][0];
		if (classe == -1)
		{
			send_to_char("Bug in spec_proc guild_guard, guardia della gilda senza una classe. Riportare il bug ai creatori.\r\n", ch);
			return TRUE;
		}
		if (!IS_DARESIANO(ch))
		{
			if (GET_CLASS(ch) == classe)
				send_to_char("Fai gia' parte del nostro ordine, non scherzare.\r\n", ch);
			else
				send_to_char("Solo coloro che non sono ancora entrati in un ordine combattente possono\r\n"
					"chiedere di essere iniziati al nostro ordine.\r\n", ch);
			return TRUE;
		}
		if (GET_GOLD(ch) < costo_iniziazione){
			sprintf(buf, "L'iniziazione al nostro ordine costa %d monete d'oro, somma che al momento tu non possiedi.\r\n", costo_iniziazione);
			send_to_char(buf, ch);
			return TRUE;
		}
		ingresso_ordine(ch, classe);
		return TRUE;
	}
	else if (CMD_IS("rtitolo"))
	{
		if (IS_NPC(ch)){
			send_to_char ("Un mob vorrebbe un titolo?\r\n", ch);
			return TRUE;
		}
		if (IS_AFFECTED(guard, AFF_BLIND)){
			send_to_char("Se potessi vederti, ti aiuterei...\r\n", ch);
			return TRUE;
		}
		for (i = 0; (guild_info[i][0] != -1 && classe == -1); i++)
			if (world[ch->in_room].number == guild_info[i][1])
				classe = guild_info[i][0];
		if (GET_CLASS(ch) != classe){
			send_to_char ("Che ne diresti di andare alla tua gilda per chiedere il titolo?\r\n", ch);
			return TRUE;
		}
		if (PLR_FLAGGED(ch, PLR_GO_GILDA)){
			REMOVE_BIT(PLR_FLAGS(ch), PLR_GO_GILDA);
			sprintf (buf, "Complimenti! Hai guadagnato il %s titolo di gilda!\r\n", GET_CLASS_TITLE(ch) ? "terzo" : "secondo");
			send_to_char (buf, ch);
			GET_CLASS_TITLE(ch)++;
			return TRUE;
		}
		else{
			if (GET_LEVEL(ch) < 20 || (GET_LEVEL(ch) < 40 && GET_CLASS_TITLE(ch))){
				send_to_char ("Non sei ancora in grado di conseguire un nuovo titolo di classe!\r\n", ch);
				return TRUE;
			}
			if (GET_CLASS_TITLE(ch) > 1){
				send_to_char ("Non e' ancora stato creato un titolo di classe cosi' alto!\r\n", ch);
				return TRUE;
			}
			for (i = 0; i < num_titolanti; i++)
				if (!strcmp(GET_NAME(ch), pg_titolanti[i].nome)){
					send_to_char ("Hai gia' fatto richiesta per il titolo!\r\n", ch);
					return TRUE;
				}
			sprintf (buf, "La guardia prende nota della tua richiesta, annuisce brevemente e ti congeda.\r\n");
			send_to_char (buf, ch);

			num_titolanti++;
			pg_titolanti = realloc (pg_titolanti, sizeof (struct nomi_titolanti) * num_titolanti);
			strcpy(pg_titolanti[num_titolanti-1].nome, GET_NAME(ch));
			pg_titolanti[num_titolanti-1].titolo = GET_CLASS_TITLE(ch) + 2;
			pg_titolanti[num_titolanti-1].class = GET_CLASS(ch);
			pg_titolanti[num_titolanti-1].richiesta = time(NULL);
			remove ("etc/titolanti");
			if (!(titolanti_f = fopen ("etc/titolanti", "wb"))){
				send_to_char ("Attenzione: errore nell'allocazione di memoria, contatta un creatore!\r\n", ch);
				log ("SYSERR: errore in fase di allocazione di memoria nella spec_proc guildguard.");
				return TRUE;
			}
			for (i = 0; i < num_titolanti; i++)
				fwrite (pg_titolanti+i, sizeof (struct nomi_titolanti), 1, titolanti_f);
			return TRUE;
		}
	}
	
	if (!IS_MOVE(cmd) || IS_AFFECTED(guard, AFF_BLIND))
		return FALSE;
	
	if (GET_LEVEL(ch) >= LVL_IMMORT)
		return FALSE;
	
	for (i = 0; guild_info[i][0] != -1; i++) {
		if ((IS_NPC(ch) || GET_CLASS(ch) != guild_info[i][0]) &&
			world[ch->in_room].number == guild_info[i][1] &&
			cmd == guild_info[i][2]) {
			send_to_char("La guardia blocca il tuo passaggio umiliandoti.\r\n", ch);
			act("La guarda umilia $n, bloccando la strada.", FALSE, ch, 0, 0, TO_ROOM);
			return TRUE;
		}
	}
	
	return FALSE;
}



SPECIAL(puff)
{
	
	//ACMD(do_say);
	if (cmd)
		return (0);
	
	switch (number(0, 60)) {
		case 0:
			do_say(ch, "Mio dio!  E' pieno di stelle!", 0, 0);
			return (1);
		case 1:
			do_say(ch, "Come si fa' a pescare qui?", 0, 0);
			return (1);
		case 2:
			do_say(ch, "Sono un drago, femmina, bellissima.", 0, 0);
			return (1);
		case 3:
			do_say(ch, "Ho raggiunto uno stato piacevole di benessere.", 0, 0);
			return (1);
		default:
			return (0);
	}
}



SPECIAL(fido)
{
	
	struct obj_data *i, *temp, *next_obj;
	
	if (cmd || !AWAKE(ch))
		return (FALSE);
	
	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if (GET_OBJ_TYPE(i) == ITEM_CONTAINER && GET_OBJ_VAL(i, 3)) {
			act("$n divora selvaggiamente un corpo.", FALSE, ch, 0, 0, TO_ROOM);
			for (temp = i->contains; temp; temp = next_obj) {
				next_obj = temp->next_content;
				obj_from_obj(temp);
				obj_to_room(temp, ch->in_room);
			}
			extract_obj(i);
			return (TRUE);
		}
	}
	return (FALSE);
}



SPECIAL(janitor)
{
	struct obj_data *i;
	
	if (cmd || !AWAKE(ch))
		return (FALSE);
	
	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if (!CAN_WEAR(i, ITEM_WEAR_TAKE))
			continue;
		if (GET_OBJ_TYPE(i) != ITEM_DRINKCON && GET_OBJ_COST(i) >= 15)
			continue;
		act("$n raccoglie un po' di spazzatura.", FALSE, ch, 0, 0, TO_ROOM);
		obj_from_room(i);
		obj_to_char(i, ch);
		return TRUE;
	}
	
	return FALSE;
}


SPECIAL(cityguard)
{
	struct char_data *tch=NULL, *evil;
	struct char_data *vict;
	int max_evil,prison_room,i;
	bool track_n_move(struct char_data *ch,int target);
	void unstow_char(struct char_data *ch);
	void stow_char(struct char_data *ch,struct char_data *mount);
	int find_prison(struct char_data *ch);
	void do_doorcmd(struct char_data *ch,struct obj_data *obj,int door,int scmd);
	int idclan;
	
	
   //Il secondino deve avere un clan
	if (CLAN_ID(ch)>0)
		idclan = CLAN_ID(ch);
	else
		return FALSE;
	
	if (FIGHTING(ch)){
		if (AFF_FLAGGED(FIGHTING(ch), AFF_PARALIZE))
			return FALSE;
	}
	if (cmd || !AWAKE(ch) || FIGHTING(ch))
		return FALSE;
		
	max_evil = 1000;
	evil = 0;	
	
	
/*new criminal*/
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (!IS_NPC(tch) && (!TRANSPORTED_BY(tch)) && (!AFF_FLAGGED(tch,AFF_TRAMORTITO)) && CAN_SEE(ch, tch)){
			
	//LE guardie non picchiano i fantasmi e i nogdr
	if (PLR_FLAGGED(tch,PLR_FANTASMA))
		return FALSE;	
			 
    /*KILLER internazionale*/
			if (IS_SET(PLR_FLAGS(tch), PLR_KILLER)) {
				act("$n grida 'HEY!!! FERMO! Tu sei ricercato internazionale!\r\n Maledetto PLAYER KILLER!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*THIEF internazionale*/
			if (IS_SET(PLR_FLAGS(tch), PLR_THIEF)) {
				act("$n grida 'HEY!!!  Tu sei un Ladro internazionale!!! PLAYER THIEF!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*THIEF del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_THIEF)) {
				act("$n grida 'HEY!!!  Tu sei un LADRO ricercato!!! PLAYER THIEF!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*KILLER del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_KILLER)) {
				act("$n grida 'HEY!!! FERMO Tu sei un KILLER ricercato!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*TKILLER del regno*/
			if (FLAGGED_CRIME(tch, CLAN_ID(ch), CLAN_TKILLER)) {
				act("$n grida 'HEY!!! Sei ricecato per TENTATO OMICIDIO!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
      /*CLAN_WKILLER del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_THIEF)) {
				act("$n grida 'HEY!!!  Sei ricecato per ALTO TRADIMENTO!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
     /*CLAN_WANTED del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_WANTED)) {
				act("$n grida 'HEY!!! Sei un ricercato !!! Vieni con me!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
     /*CLAN_STRAGE  del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_STRAGE)) {
				act("$n grida 'HEY!!! FERMO!!! SEI L'AUTORE DELLA STRAGE!!!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
     /*CLAN_CORROTTO  del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_CORROTTO)) {
				act("$n grida 'HEY!!! FERMO!!! SEI L'AUTORE DELLA STRAGE!!!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*CLAN_MULTA del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_MULTA)) {
      //act("$n grida 'Paga la multa!!!'", FALSE, ch, 0, 0, TO_ROOM);
      //hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
		}//Can see
	}
	
	if(!FIGHTING(ch)){
		for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
			if((IS_CRIMINAL(tch)||IS_A_CRIMINAL_CLAN(tch,idclan))&&AFF_FLAGGED(tch,AFF_TRAMORTITO)&&!TRANSPORTED_BY(tch))
			{
				act("$n grida 'Bastardo ,ora ti sbatto in prigione!", FALSE, ch, 0, 0
					, TO_ROOM);
				act("$n si carica $N sulle spalle.", FALSE, ch, 0, tch, TO_ROOM);
				stow_char(tch,ch);	 
				return (TRUE);
			}
		}
		
		if((tch=TRANSPORTED(ch))){
			if((prison_room=find_prison(ch))!=0){
				track_n_move(ch,prison_room);
				i=0;
				while(i<NUM_OF_DIRS && (!EXIT(ch, i)||EXIT(ch, i)->to_room != real_room(prison_room)))
					i++;
				if(i<NUM_OF_DIRS) {
					act("$n apre la cella",FALSE,ch,0,0,TO_ROOM);
					act("$n entra nella cella",FALSE, ch,0,0,TO_ROOM);		
					char_from_room(TRANSPORTED(ch));
					char_to_room(TRANSPORTED(ch), real_room(prison_room));
					act("$n apre la cella",FALSE,TRANSPORTED(ch),0,0,TO_ROOM);
					act("$n entra nella cella",FALSE, TRANSPORTED(ch),0,0,TO_ROOM);
					act("$N sbatte a terra in malo modo $n",FALSE,TRANSPORTED(ch),0,ch,TO_ROOM);	
					act("$N ti sbatte a terra in malo modo ",FALSE,TRANSPORTED(ch),0,ch,TO_VICT);
					act("$n butta in cella $N",FALSE,ch ,0,TRANSPORTED(ch),TO_ROOM);
					vict = TRANSPORTED(ch);
					unstow_char(ch);
					save_char(vict, vict->in_room);
					act("$N esce dalla cella",FALSE, vict,0,ch,TO_ROOM);
					act("$n esce dalla cella",FALSE, ch,0,0,TO_ROOM);
					act("$n chiude la cella",FALSE, ch,0,0,TO_ROOM);	  
				}
				else
				if(TRANSPORTED(ch))
					random_insult(ch,TRANSPORTED(ch));
			}
			return (TRUE);
		}
	}
	
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (CAN_SEE(ch, tch) && FIGHTING(tch)) {
			if ((GET_ALIGNMENT(tch) < max_evil) &&
				(IS_NPC(tch) || IS_NPC(FIGHTING(tch)))&&!MOB_FLAGGED(tch,MOB_CIVILIAN)) {
				max_evil = GET_ALIGNMENT(tch);
				evil = tch;
			}
		}
	}
	
	if (evil && (GET_ALIGNMENT(FIGHTING(evil)) >= 0)) {
		if (!IS_NPC(evil) && (!TRANSPORTED_BY(evil)) && (!AFF_FLAGGED(evil,AFF_TRAMORTITO))){
			act("$n screams 'PROTEGGETE GLI INNOCENTI!  BANZAI!  CARICA!  ARARARAGGGHH!'", FALSE, ch, 0, 0, TO_ROOM);
			hit(ch, evil, TYPE_UNDEFINED);
			return (TRUE);
		}
	}
	
	
	return (FALSE);
}


SPECIAL(guardia_stabber)
{
	struct char_data *tch=NULL, *evil;
	struct char_data *vict;
	int max_evil,prison_room,i;
	bool track_n_move(struct char_data *ch,int target);
	void unstow_char(struct char_data *ch);
	void stow_char(struct char_data *ch,struct char_data *mount);
	int find_prison(struct char_data *ch);
	void do_doorcmd(struct char_data *ch,struct obj_data *obj,int door,int scmd);
	int idclan;
	
	
   //Il secondino deve avere un clan
	if (CLAN_ID(ch)>0)
		idclan = CLAN_ID(ch);
	else
		return FALSE;
	
	if (FIGHTING(ch)){
		if (AFF_FLAGGED(FIGHTING(ch), AFF_PARALIZE))
			return FALSE;
	}
	if (cmd || !AWAKE(ch) || FIGHTING(ch))
		return FALSE;
		
	max_evil = 1000;
	evil = 0;	
	
	
/*new criminal*/
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (!IS_NPC(tch) && (!TRANSPORTED_BY(tch)) && (!AFF_FLAGGED(tch,AFF_TRAMORTITO)) && CAN_SEE(ch, tch)){
			
	//LE guardie non picchiano i fantasmi e i nogdr
	if (PLR_FLAGGED(tch,PLR_FANTASMA))
		return FALSE;	
			 
    /*KILLER internazionale*/
			if (IS_SET(PLR_FLAGS(tch), PLR_KILLER)) {
				act("$n grida 'HEY!!! FERMO! Tu sei ricercato internazionale!\r\n Maledetto PLAYER KILLER!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
    /*THIEF internazionale*/
			if (IS_SET(PLR_FLAGS(tch), PLR_THIEF)) {
				act("$n grida 'HEY!!!  Tu sei un Ladro internazionale!!! PLAYER THIEF!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
    /*THIEF del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_THIEF)) {
				act("$n grida 'HEY!!!  Tu sei un LADRO ricercato!!! PLAYER THIEF!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
    /*KILLER del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_KILLER)) {
				act("$n grida 'HEY!!! FERMO Tu sei un KILLER ricercato!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
    /*TKILLER del regno*/
			if (FLAGGED_CRIME(tch, CLAN_ID(ch), CLAN_TKILLER)) {
				act("$n grida 'HEY!!! Sei ricecato per TENTATO OMICIDIO!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
      /*CLAN_WKILLER del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_THIEF)) {
				act("$n grida 'HEY!!!  Sei ricecato per ALTO TRADIMENTO!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
     /*CLAN_WANTED del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_WANTED)) {
				act("$n grida 'HEY!!! Sei un ricercato !!! Vieni con me!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
     /*CLAN_STRAGE  del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_STRAGE)) {
				act("$n grida 'HEY!!! FERMO!!! SEI L'AUTORE DELLA STRAGE!!!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
     /*CLAN_CORROTTO  del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_CORROTTO)) {
				act("$n grida 'HEY!!! FERMO!!! SEI L'AUTORE DELLA STRAGE!!!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				npc_backstab(ch, tch);
				return (TRUE);
			}
			
    /*CLAN_MULTA del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_MULTA)) {
      //act("$n grida 'Paga la multa!!!'", FALSE, ch, 0, 0, TO_ROOM);
      //hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
		}//Can see
	}
	
	if(!FIGHTING(ch)){
		for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
			if((IS_CRIMINAL(tch)||IS_A_CRIMINAL_CLAN(tch,idclan))&&AFF_FLAGGED(tch,AFF_TRAMORTITO)&&!TRANSPORTED_BY(tch))
			{
				act("$n grida 'Bastardo ,ora ti sbatto in prigione!", FALSE, ch, 0, 0
					, TO_ROOM);
				act("$n si carica $N sulle spalle.", FALSE, ch, 0, tch, TO_ROOM);
				stow_char(tch,ch);	 
				return (TRUE);
			}
		}
		
		if((tch=TRANSPORTED(ch))){
			if((prison_room=find_prison(ch))!=0){
				track_n_move(ch,prison_room);
				i=0;
				while(i<NUM_OF_DIRS && (!EXIT(ch, i)||EXIT(ch, i)->to_room != real_room(prison_room)))
					i++;
				if(i<NUM_OF_DIRS) {
					act("$n apre la cella",FALSE,ch,0,0,TO_ROOM);
					act("$n entra nella cella",FALSE, ch,0,0,TO_ROOM);		
					char_from_room(TRANSPORTED(ch));
					char_to_room(TRANSPORTED(ch), real_room(prison_room));
					act("$n apre la cella",FALSE,TRANSPORTED(ch),0,0,TO_ROOM);
					act("$n entra nella cella",FALSE, TRANSPORTED(ch),0,0,TO_ROOM);
					act("$N sbatte a terra in malo modo $n",FALSE,TRANSPORTED(ch),0,ch,TO_ROOM);	
					act("$N ti sbatte a terra in malo modo ",FALSE,TRANSPORTED(ch),0,ch,TO_VICT);
					act("$n butta in cella $N",FALSE,ch ,0,TRANSPORTED(ch),TO_ROOM);
					vict = TRANSPORTED(ch);
					unstow_char(ch);
					save_char(vict, vict->in_room);
					act("$N esce dalla cella",FALSE, vict,0,ch,TO_ROOM);
					act("$n esce dalla cella",FALSE, ch,0,0,TO_ROOM);
					act("$n chiude la cella",FALSE, ch,0,0,TO_ROOM);	  
				}
				else
				if(TRANSPORTED(ch))
					random_insult(ch,TRANSPORTED(ch));
			}
			return (TRUE);
		}
	}
	
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (CAN_SEE(ch, tch) && FIGHTING(tch)) {
			if ((GET_ALIGNMENT(tch) < max_evil) &&
				(IS_NPC(tch) || IS_NPC(FIGHTING(tch)))&&!MOB_FLAGGED(tch,MOB_CIVILIAN)) {
				max_evil = GET_ALIGNMENT(tch);
				evil = tch;
			}
		}
	}
	
	if (evil && (GET_ALIGNMENT(FIGHTING(evil)) >= 0)) {
		if (!IS_NPC(evil) && (!TRANSPORTED_BY(evil)) && (!AFF_FLAGGED(evil,AFF_TRAMORTITO))){
			act("$n screams 'PROTEGGETE GLI INNOCENTI!  BANZAI!  CARICA!  ARARARAGGGHH!'", FALSE, ch, 0, 0, TO_ROOM);
			npc_backstab(ch, evil);
			return (TRUE);
		}
	}
	
	// In teoria qui si arriva solo se non si e' fatto nulla...
	SET_BIT(AFF_FLAGS(ch), AFF_HIDE);
	
	return (FALSE);
}


SPECIAL(guardia_sentinel)
{
	struct char_data *tch=NULL, *evil;
	struct char_data *vict;
	int max_evil,prison_room,i;
	bool track_n_move(struct char_data *ch,int target);
	void unstow_char(struct char_data *ch);
	void stow_char(struct char_data *ch,struct char_data *mount);
	int find_prison(struct char_data *ch);
	void do_doorcmd(struct char_data *ch,struct obj_data *obj,int door,int scmd);
	int idclan;
	
	
   //Il secondino deve avere un clan
	if (CLAN_ID(ch)>0)
		idclan = CLAN_ID(ch);
	else
		return FALSE;
	
	if (FIGHTING(ch)){
		if (AFF_FLAGGED(FIGHTING(ch), AFF_PARALIZE))
			return FALSE;
	}
	if (cmd || !AWAKE(ch) || FIGHTING(ch))
		return FALSE;
		
	max_evil = 1000;
	evil = 0;	
	
	
/*new criminal*/
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (!IS_NPC(tch) && (!TRANSPORTED_BY(tch)) && (!AFF_FLAGGED(tch,AFF_TRAMORTITO)) && CAN_SEE(ch, tch)){
			
	//LE guardie non picchiano i fantasmi e i nogdr
	if (PLR_FLAGGED(tch,PLR_FANTASMA))
		return FALSE;	
			 
    /*KILLER internazionale*/
			if (IS_SET(PLR_FLAGS(tch), PLR_KILLER)) {
				act("$n grida 'HEY!!! FERMO! Tu sei ricercato internazionale!\r\n Maledetto PLAYER KILLER!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*THIEF internazionale*/
			if (IS_SET(PLR_FLAGS(tch), PLR_THIEF)) {
				act("$n grida 'HEY!!!  Tu sei un Ladro internazionale!!! PLAYER THIEF!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*THIEF del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_THIEF)) {
				act("$n grida 'HEY!!!  Tu sei un LADRO ricercato!!! PLAYER THIEF!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*KILLER del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_KILLER)) {
				act("$n grida 'HEY!!! FERMO Tu sei un KILLER ricercato!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*TKILLER del regno*/
			if (FLAGGED_CRIME(tch, CLAN_ID(ch), CLAN_TKILLER)) {
				act("$n grida 'HEY!!! Sei ricecato per TENTATO OMICIDIO!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
      /*CLAN_WKILLER del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_THIEF)) {
				act("$n grida 'HEY!!!  Sei ricecato per ALTO TRADIMENTO!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
     /*CLAN_WANTED del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_WANTED)) {
				act("$n grida 'HEY!!! Sei un ricercato !!! Vieni con me!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
     /*CLAN_STRAGE  del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_STRAGE)) {
				act("$n grida 'HEY!!! FERMO!!! SEI L'AUTORE DELLA STRAGE!!!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
     /*CLAN_CORROTTO  del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_CORROTTO)) {
				act("$n grida 'HEY!!! FERMO!!! SEI L'AUTORE DELLA STRAGE!!!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
    /*CLAN_MULTA del regno*/
			if (FLAGGED_CRIME(tch,CLAN_ID(ch), CLAN_MULTA)) {
      //act("$n grida 'Paga la multa!!!'", FALSE, ch, 0, 0, TO_ROOM);
      //hit(ch, tch, TYPE_UNDEFINED);
				return (TRUE);
			}
			
		}//Can see
	}
	
	if(!FIGHTING(ch)){
		for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
			if((IS_CRIMINAL(tch)||IS_A_CRIMINAL_CLAN(tch,idclan))&&AFF_FLAGGED(tch,AFF_TRAMORTITO)&&!TRANSPORTED_BY(tch))
			{
				act("$n grida 'Bastardo, ora ti sbatto in prigione!", FALSE, ch, 0, 0
					, TO_ROOM);
				act("$n si carica $N sulle spalle.", FALSE, ch, 0, tch, TO_ROOM);
				stow_char(tch,ch);
				if (MOB_FLAGGED(ch, MOB_SENTINEL))
					ch->char_specials.guardia_sentinel_back = world[ch->in_room].number;
				return (TRUE);
			}
		}
		
		if((tch=TRANSPORTED(ch))){
			if((prison_room=find_prison(ch))!=0){
				track_n_move(ch,prison_room);
				i=0;
				while(i<NUM_OF_DIRS && (!EXIT(ch, i)||EXIT(ch, i)->to_room != real_room(prison_room)))
					i++;
				if(i<NUM_OF_DIRS) {
					act("$n apre la cella",FALSE,ch,0,0,TO_ROOM);
					act("$n entra nella cella",FALSE, ch,0,0,TO_ROOM);		
					char_from_room(TRANSPORTED(ch));
					char_to_room(TRANSPORTED(ch), real_room(prison_room));
					act("$n apre la cella",FALSE,TRANSPORTED(ch),0,0,TO_ROOM);
					act("$n entra nella cella",FALSE, TRANSPORTED(ch),0,0,TO_ROOM);
					act("$N sbatte a terra in malo modo $n",FALSE,TRANSPORTED(ch),0,ch,TO_ROOM);	
					act("$N ti sbatte a terra in malo modo ",FALSE,TRANSPORTED(ch),0,ch,TO_VICT);
					act("$n butta in cella $N",FALSE,ch ,0,TRANSPORTED(ch),TO_ROOM);
					vict = TRANSPORTED(ch);
					unstow_char(ch);
					save_char(vict, vict->in_room);
					act("$N esce dalla cella",FALSE, vict,0,ch,TO_ROOM);
					act("$n esce dalla cella",FALSE, ch,0,0,TO_ROOM);
					act("$n chiude la cella",FALSE, ch,0,0,TO_ROOM);	  
				}
				else
				if(TRANSPORTED(ch))
					random_insult(ch,TRANSPORTED(ch));
			}
			return (TRUE);
		}

		if (MOB_FLAGGED(ch, MOB_SENTINEL) && ch->char_specials.guardia_sentinel_back && ch->in_room != ch->char_specials.guardia_sentinel_back)
			track_n_move(ch, ch->char_specials.guardia_sentinel_back);
	}
	
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (CAN_SEE(ch, tch) && FIGHTING(tch)) {
			if ((GET_ALIGNMENT(tch) < max_evil) &&
				(IS_NPC(tch) || IS_NPC(FIGHTING(tch)))&&!MOB_FLAGGED(tch,MOB_CIVILIAN)) {
				max_evil = GET_ALIGNMENT(tch);
				evil = tch;
			}
		}
	}
	
	if (evil && (GET_ALIGNMENT(FIGHTING(evil)) >= 0)) {
		if (!IS_NPC(evil) && (!TRANSPORTED_BY(evil)) && (!AFF_FLAGGED(evil,AFF_TRAMORTITO))){
			act("$n screams 'PROTEGGETE GLI INNOCENTI!  BANZAI!  CARICA!  ARARARAGGGHH!'", FALSE, ch, 0, 0, TO_ROOM);
			hit(ch, evil, TYPE_UNDEFINED);
			return (TRUE);
		}
	}
	
	
	return (FALSE);
}


//void load_mtrigger(char_data *ch);

#define PET_SELPRICE(pet) (GET_LEVEL(pet) * 100)
#define PET_BUYPRICE(pet) (GET_LEVEL(pet) * 70)

SPECIAL(pet_shops)
{
	char buf[MAX_STRING_LENGTH], pet_name[256];
	int pet_room;
	struct char_data *pet;
	
	pet_room = ch->in_room + 1;
	
	if (CMD_IS("lista")) {
		send_to_char("Questa e' la disponibilita':\r\n", ch);
		for (pet = world[pet_room].people; pet; pet = pet->next_in_room) {
			sprintf(buf, "%8d - %s\r\n", PET_SELPRICE(pet), GET_NAME(pet));
			send_to_char(buf, ch);
		}
		return (TRUE);
	} else if (CMD_IS("buy") || CMD_IS("compra")) {
		
		argument = one_argument(argument, buf);
		argument = one_argument(argument, pet_name);
		
		if (!(pet = get_char_room(buf, pet_room))) {
			send_to_char("Quello non e' disponibile!\r\n", ch);
			return (TRUE);
		}
		
		if (GET_GOLD(ch) < PET_SELPRICE(pet)) {
			send_to_char("Non hai abbastanza soldi per quello!\r\n", ch);
			return (TRUE);
		}
		if (limite_follower(ch)) {
			send_to_char("Non hai abbastanza carisma per comprarlo!\r\n", ch);
			return (TRUE);
		}
		
		GET_GOLD(ch) -= PET_SELPRICE(pet);
		
		pet = read_mobile(GET_MOB_RNUM(pet), REAL);
		
		if (*pet_name) {
			sprintf(buf, "%s %s", pet->player.name, pet_name);
      /* free(pet->player.name); don't free the prototype! */
			pet->player.name = str_dup(buf);
			
			sprintf(buf, "%sSu una piccola targhetta legata al collo con una catenella e' scritto 'Il mio nome e' %s'\r\n",
				pet->player.description, pet_name);
      /* free(pet->player.description); don't free the prototype! */
			pet->player.description = str_dup(buf);
		}
		
		GET_EXP(pet) = 0;
		
		REMOVE_BIT(MOB_FLAGS(pet), MOB_AGGRESSIVE);
		REMOVE_BIT(MOB_FLAGS(pet), MOB_SPEC);
		SET_BIT(MOB_FLAGS(pet), MOB_SENTINEL);
		
		char_to_room(pet, ch->in_room);
		//load_mtrigger(pet); //Orione
		
		if (!MOB_FLAGGED(pet, MOB_MOUNTABLE)) {
			SET_BIT(AFF_FLAGS(pet), AFF_CHARM);
			if (pet->master)
			  stop_follower(pet);
		   add_follower(pet, ch);
      /* Be certain that pets can't get/carry/use/wield/wear items */
      /* I cavalli invece possono */
			IS_CARRYING_W(pet) = 1000;
			IS_CARRYING_N(pet) = 100;
			send_to_char("Divertiti con quello.\r\n", ch);
		}
		else { 
			send_to_char("Eccoti la tua nuova cavalcatura.\r\n", ch);
			SET_BIT(AFF_FLAGS(pet), AFF_TAMED);   
			SET_BIT(MOB_FLAGS(pet), MOB_SAVE);
		}
		
		act("$n acquista $N.", FALSE, ch, 0, pet, TO_ROOM);
		
		return 1;
	} 
	else if (CMD_IS("sell") || CMD_IS("vendi")) {
		
		argument = one_argument(argument, buf);
		
		if (!(pet = get_char_room_vis(ch, buf))) {
			send_to_char("Non vedi nessuno con quel nome qui.!\r\n", ch);
			return (TRUE);
		}
		
		if (IS_NPC(pet) && MOB_FLAGGED(pet, MOB_AMMAESTRABILE)) {
			if (pet->master == ch) {
				if (RIDING(ch) && RIDING(ch)==pet)
					dismount_char(ch);
				sprintf(buf,"Vendi %s.\r\n", GET_NAME(pet));
				send_to_char(buf, ch);
				act("$n vende $N.", FALSE, ch, 0, pet, TO_ROOM);
				char_from_room(pet);
				char_to_room(pet, pet_room);
				stop_follower(pet);
				REMOVE_BIT(AFF_FLAGS(pet), AFF_CHARM);
				REMOVE_BIT(MOB_FLAGS(pet), MOB_SAVE);
				GET_GOLD(ch) += PET_BUYPRICE(pet);
				return 1;
			} 
			else {
				sprintf(buf,"Ma se %s nemmeno ti da' retta!\r\n", GET_NAME(pet));
				send_to_char(buf, ch);
				return (TRUE);
			}
			
		} 
		else {
			sprintf(buf,"Non puoi vendere %s.\r\n", GET_NAME(pet));
			send_to_char(buf, ch);
			return (TRUE);
		}
		
	} 
	else if (CMD_IS("valutate") || CMD_IS("valuta")) {
		
		argument = one_argument(argument, buf);
		
		if (!(pet = get_char_room_vis(ch, buf))) {
			send_to_char("Non vedi nessuno con quel nome qui.!\r\n", ch);
			return (TRUE);
		}
		
		sprintf(buf,"Per %s ti posso dare %d coins.\r\n",GET_NAME(pet) , PET_BUYPRICE(pet) );
		send_to_char(buf, ch);
		return 1;
	} /* All commands except list and buy */
	return 0;
}



/* ********************************************************************
*  Special procedures for objects                                     *
******************************************************************** */


SPECIAL(bank)
{
	int amount;
	
	if (CMD_IS("bilancio")) {
		if (GET_BANK_GOLD(ch) > 0)
			sprintf(buf, "Il tuo conto e' di %d coins.\r\n",
				GET_BANK_GOLD(ch));
		else
			sprintf(buf, "Non hai coins depositati.\r\n");
		send_to_char(buf, ch);
		return 1;
	} else if (CMD_IS("deposita")) {
		if ((amount = atoi(argument)) <= 0) {
			send_to_char("Quanto vuoi depositare?\r\n", ch);
			return 1;
		}
		if (GET_GOLD(ch) < amount) {
			send_to_char("Non hai quei coins!\r\n", ch);
			return 1;
		}
		GET_GOLD(ch) -= amount;
		GET_BANK_GOLD(ch) += amount;
		sprintf(buf, "Depositi %d coins.\r\n", amount);
		send_to_char(buf, ch);
		act("$n fa una operazione bancaria.", TRUE, ch, 0, FALSE, TO_ROOM);
		return 1;
	} else if (CMD_IS("preleva")) {
		if ((amount = atoi(argument)) <= 0) {
			send_to_char("Quanto vuoi prelevare?\r\n", ch);
			return 1;
		}
		if (GET_BANK_GOLD(ch) < amount) {
			send_to_char("Non hai quei coins depositati!\r\n", ch);
			return 1;
		}
		GET_GOLD(ch) += amount;
		GET_BANK_GOLD(ch) -= amount;
		sprintf(buf, "Ritiri dal conto %d monete.\r\n", amount);
		send_to_char(buf, ch);
		act("$n fa un'operazione bancaria.", TRUE, ch, 0, FALSE, TO_ROOM);
		return 1;
	} else
		return 0;
}
SPECIAL (portal)
{
	struct obj_data *obj = (struct obj_data *) me;
	struct obj_data *port;
	char obj_name[MAX_STRING_LENGTH];
	
	if (!CMD_IS("enter")) return FALSE;
	
	argument = one_argument(argument,obj_name);
	if (!(port = get_obj_in_list_vis(ch, obj_name, world[ch->in_room].contents)))	{
		return(FALSE);
	}
	
	if (port != obj)
		return(FALSE);
	
	if (port->obj_flags.value[1] <= 0 ||
		port->obj_flags.value[1] > 32000) {
		send_to_char("Il portale non porta da nessuna parte.\n\r", ch);
		return TRUE;
	}
	
	act("$n entra $p, e svanisce!", FALSE, ch, port, 0, TO_ROOM);
	act("Entri $p, e sei trasportato in un altro luogo", FALSE, ch, port, 0, TO_CHAR);
	char_from_room(ch);
	char_to_room(ch, port->obj_flags.value[1]);
	look_at_room(ch,0);
	act("$n appare improvvisamente dal nulla!", FALSE, ch, 0, 0, TO_ROOM);
	return TRUE;
}

SPECIAL(silktrader)
{
	ACMD(do_say);
	if (cmd)
		return 0;
	
	if (world[ch->in_room].sector_type == SECT_CITY)
		switch (number(0, 30)) {
			case 0:
				act("$n adocchio un passante.", FALSE, ch, 0, 0,TO_ROOM);
				do_say(ch, "Venghino siori e siore, e guardate questo prezioso tessuto!", 0, 0);
				return(1);
			case 1:
				act("$n ti dice, 'Non e' grazioso!'", FALSE, ch, 0, 0,TO_ROOM);
				act("$n mostra un prezioso tessuto.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 2:
				act("$n mostra un paio di guanti.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 3:
				act("$n gracchia, 'Prendete questa roba ...mai visto un simile tessuto in tutta Daresia!", FALSE, ch, 0, 0,TO_ROOM);
				act("$n sogghigna.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 4:
				do_say(ch, "Vieni nel mio bazar!", 0, 0);
				return(1);
			case 5:
				act("$n copre il suo occhio con una mano.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 6:
				do_say(ch, "Hai mai visto un orco vestire un simile tessuto?", 0, 0);
				do_say(ch, "Non penso proprio!", 0, 0);
				act("$n butta la testa indietro e ride!", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 7:
				act("$n ti da' un bicchiere di vino.", FALSE, ch, 0, 0,TO_ROOM);
				do_say(ch, "Vieni, siediti e da' un'occhiata in giro.", 0, 0);
				return(1);
			case 8:
				act("$n looks at you.", FALSE, ch, 0, 0,TO_ROOM);
				act("$n scuote la testa tristemente.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 9:
				act("$n gioocherella con delle mappe.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 10:
				do_say(ch, "Qui qui! Pezzenti e nobili venite e fate la vostra offerta!", 0, 0);
				return(1);
			case 11:
				do_say(ch, "I prezzi cosi' bassi sono solo per oggi!", 0, 0);
				act("$n beve un bicchiere di buon vino novello.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
		}
		
	if (world[ch->in_room].sector_type != SECT_CITY)
		switch (number(0, 20)) {
			case 0:
				do_say(ch, "Ah! Viandanti! Venite e date un'occhiata a questo stupendo tessuto!", 0, 0);
				return(1);
			case 1:
				act("$n ti guarda.", FALSE, ch, 0, 0,TO_ROOM);
				do_say(ch, "Sei poco attirato dall'offerta.", 0, 0);
				do_say(ch, "Il tessuto ....e' l'affare del secolo.", 0, 0);
				act("$n sorride.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 2:
				do_say(ch, "Avventurieri udite il mio richiamo!", 0, 0);
				return(1);
			case 3:
				act("$n si aggiusta il mantello.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 4:
				act("$n ti dice, 'Certi pericoli ti aspettano la' fuori, quindi affrontali nel caldo tepore del mio tessuto'", FALSE, ch, 0, 0,TO_ROOM);
				act("$n ammicca.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 5:
				do_say(ch, "Puoi indicarmi la piu' vicina taverna?", 0, 0);
				return(1);
			case 6:
				do_say(ch, "Sentito l'ultima barzelletta?", 0, 0);
				act("$n ride tra se e se.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 7:
				do_say(ch, "Cosa capita viandante? Perdi qualche secondo qui a vedere le ultime novita' in fatto di moda!", 0, 0);
				return(1);
			case 8:
				do_say(ch, "Attento viaggiatore, io sono in esilio!", 0, 0);
				act("$n grigna.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
			case 9:
				act("$n ti tocca la spalla.", FALSE, ch, 0, 0,TO_ROOM);
				do_say(ch, "Una parola di avviso. Attenzione ad ogni boccale con scritto 'mushroom' o 'pumpkin'.", 0, 0);
				act("$n se la ride.", FALSE, ch, 0, 0,TO_ROOM);
				return(1);
				
		}
	return(0);
}


SPECIAL(athos)
{
	ACMD(do_say);
	if(cmd)
		return 0;
	switch (number(0, 20)) {
		case 0:
			act("$n si specchia nel bicchiere di vino.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 1:
			act("$n sogghigna.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 2:
			act("$n ti chiede, 'Hai visto quella donna, graziosa, con un cuore di pietra?'", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Quel mostro sara' la morte di tutti noi.", 0, 0);
			return(1);
		case 3:
			do_say(ch, "Dio ci salvi!", 0, 0);
			return(1);
		case 4:
			do_say(ch, "Tutti per uno .. uno per...", 0, 0);
			act("$n sorseggia un bicchiere di vino.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 5:
			act("$n sembra avere un'aria da filosofo.", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Donne - L'eterna puniziooone di Dio sull'uomo.", 0, 0);
			return(1);
		case 6:
			act("$n appoggia il bicchiere di vino sul tavolo di legno.", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Sapete, insiemo potremo farcela a toglierla dalla sua terra e dalla sua casa!", 0, 0);
			return(1);
		default: return(FALSE);
			break; }
	return(0);
}
SPECIAL(regina)
{
	ACMD(do_say);
	if(cmd)
		return 0;
	switch (number(0, 20)) {
		case 0:
			act("$n si aggiusta i capelli.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 1:
			act("$n si guarda nello specchio e si tocca le labbra.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 2:
			act("$n ti chiede, 'Hai per caso visto un uomo robusto, con grosse braccia e un arco?'", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Il Re prima di morire ci vide baciarci e lo caccio'.", 0, 0);
			return(1);
		case 3:
			do_say(ch, "Che trista la vita!", 0, 0);
			return(1);
		case 4:
			do_say(ch, "La vita non e' piu' la stessa qui al Castello.", 0, 0);
			act("$n si asciuga una lacrima con il fazzoletto.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 5:
			act("$n e' triste.", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Che vita grama.", 0, 0);
			return(1);
		case 6:
			act("$n grida: Voglio rivederlo ancora una volta.", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Sapete, ora che il Re e' morto sono libera!", 0, 0);
			return(1);
		default: return(FALSE);
			break; }
	return(0);
}



SPECIAL(hangman)
{
	
	ACMD(do_say);
	if(cmd) return 0;
	switch (number(0, 15)) {
		case 0:
			act("$n mette il laccio intorno al collo e lo stringe a cappio.", FALSE, ch, 0, 0,TO_ROOM);
			do_say(ch, "Tu sei il prossimo, sporco essere!", 0, 0);
			do_say(ch, "Sto solo scherzando.", 0, 0);
			act("$n ti batte la mano sulla testa.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 1:
			do_say(ch, "Ero stato mandato in esilio ma ora sono stato riabilitato nella societa'!", 0, 0);
			do_say(ch, "Muahaha!", 0, 0);
			return(1);
		case 2:
			do_say(ch, "Nessuno ha un coltello da prestarmi?", 0, 0);
			return(1);
		case 3:
			act("$n improvvisamente tira la leva.", FALSE, ch, 0, 0,TO_ROOM);
			act("Con un bagliore riflesso dal metallo la pesante ghigliottina viene giu'!", FALSE, ch, 0, 0,TO_ROOM);
			act("Una testa insanguinata cade dalla piattaforma e rotola vicino ai tuoi piedi.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n guarda in su e sospira.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n grida, 'Il prossimo!'", FALSE, ch, 0, 0, TO_ROOM);
			return(1);
		case 4:
			act("$n sistema il suo strumento di morte.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		default:
			return(FALSE);
			break;
	}
	return(0);
}



SPECIAL(butcher)
{
	
	ACMD(do_say);
	if(cmd) return 0;
	switch (number(0, 40)) {
		case 0:
			do_say(ch, "I need a Union.", 0, 0);
			act("$n glares angrily.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n rummages about for an axe.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 1:
			act("$n gnaws on a toothpick.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 2:
			act("$n runs a finger along the edge of a giant meat cleaver.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n grins evilly.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 3:
			do_say(ch, "Pork for sale!", 0, 0);
			return(1);
		case 4:
			act("$n whispers to you, 'I've got some great damage eq in the back room. Wanna see?'", FALSE, ch, 0, 0,TO_ROOM);
			act("$n throws back his head and cackles with insane glee!", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 5:
			act("$n yawns.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 6:
			act("$n throws an arm around the headless body of an ogre and asks to have his picture taken.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 7:
			act("$n listlessly grabs a cleaver and hurls it into the wall behind your head.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 8:
			act("$n juggles some fingers.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 9:
			act("$n eyes your limbs.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n chuckles.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 10:
			do_say(ch, "Hi, Alice.", 0, 0);
			return(1);
		case 11:
			do_say(ch, "Everyone looks like food to me these days.", 0, 0);
			act("$n sighs loudly.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 12:
			act("$n throws up his head and shouts wildly.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n shouts, 'Bring out your dead!'", FALSE, ch, 0, 0, TO_ROOM);
			return(1);
		case 13:
			do_say(ch, "The worms crawl in, the worms crawl out..", 0, 0);
			return(1);
		case 14:
			act("$n sings 'Brave, brave Sir Patton...'", FALSE, ch, 0, 0,TO_ROOM);
			act("$n whistles a tune.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n smirks.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 15:
			do_say(ch, "Get Lurch to bring me over a case and I'll sport you a year's supply of grilled ogre.", 0, 0);
			return(1);
		default: return(FALSE);
			break; }
	return(0);
}



SPECIAL(stu)
{
	ACMD(do_flee);
	ACMD(do_say);
	if(cmd)
		return 0;
	
	switch (number(0, 60)) {
		case 0:
			do_say(ch, "I'm so damn cool, I'm too cool to hang out with myself!", 0, 0);
			break;
		case 1:
			do_say(ch, "I'm really the NICEST guy you ever MEET!", 0, 0);
			break;
		case 2:
			do_say(ch, "Follow me for exp, gold and lessons in ADVANCED C!", 0, 0);
			break;
		case 3:
			do_say(ch, "Mind if I upload 200 megs of pregnant XXX gifs with no descriptions to your bbs?", 0, 0);
			break;
		case 4:
			do_say(ch, "Sex? No way! I'd rather jog 20 miles!", 0, 0);
			break;
		case 5:
			do_say(ch, "I'll take you OUT!!   ...tomorrow", 0, 0);
			break;
		case 6:
			do_say(ch, "I invented Mud you know...", 0, 0);
			break;
		case 7:
			do_say(ch, "Can I have a cup of water?", 0, 0);
			break;
		case 8:
			do_say(ch, "I'll be jogging down ventnor ave in 10 minutes if you want some!", 0, 0);
			break;
		case 9:
			do_say(ch, "Just let me pull a few strings and I'll get ya a site, they love me! - doesnt everyone?", 0, 0);
			break;
		case 10:
			do_say(ch, "Pssst! Someone tell Mercy to sport me some levels.", 0, 0);
			act("$n nudges you with his elbow.", FALSE, ch, 0, 0,TO_ROOM);
			break;
		case 11:
			do_say(ch, "Edgar! Buddy! Let's group and hack some ogres to tiny quivering bits!", 0, 0);
			break;
		case 12:
			act("$n tells you, 'Skylar has bad taste in women!'", FALSE, ch, 0, 0,TO_ROOM);
			act("$n screams in terror!", FALSE, ch, 0, 0,TO_ROOM);
			do_flee(ch, 0, 0, 0);
			break;
		case 13:
			if (number(0, 32767)<10){
				act("$n whispers to you, 'Dude! If you fucking say 'argle bargle' to the glowing fido he'll raise you a level!'", FALSE, ch, 0, 0,TO_ROOM);
				act("$n flexes.", FALSE, ch, 0, 0,TO_ROOM);}
			return(1);
		default:
			return(FALSE);
			break;
			return(1);
	}
	return 0;
}


SPECIAL(sund_earl)
{
	ACMD(do_say);
	if (cmd)
		return(FALSE);
	switch (number(0, 20)) {
		case 0:
			do_say(ch, "E' una gran bella giornata, non trovi?.", 0, 0);
			return(1);
		case 1:
			act("$n si guarda in giro con orgoglio.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 2:
			do_say(ch, "Mai visto un Regno meglio di questo!!.", 0, 0);
			act("$n annuisce.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 3:
			do_say(ch, "Devo ricordarmi di suggerire al Re nuove strategie.", 0, 0);
			return(1);
		case 4:
			do_say(ch, "Molti sono i nostri nemici, ma insieme li annienteremo", 0, 0);
			act("$n accarezza la sua spada.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 5:
			do_say(ch, "E' giusto aiutare il prossimo", 0, 0);
			return(1);
		case 6:
			do_say(ch, "Chi non rispetta le regole rischia la forca o le prigioni!!", 0, 0);
			act("$n ti saluta con riverenza.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 7:
			act("$n Osserva le opere edilizie con grande compiacimento.", FALSE, ch, 0, 0,TO_ROOM);
			return(1);
		case 8:
			do_say(ch, "Odio la guerra!! Preferisco gli intrighi e i sotterfugi!", 0, 0);
			act("$n sogghigna.", FALSE, ch, 0, 0, TO_ROOM);
			return(1);
		default: return(FALSE);
			break;
			return(0);
	}
}

SPECIAL(blinder)
{
	if (cmd)
		return FALSE;
	
	if (GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	
	if (FIGHTING(ch) && (FIGHTING(ch)->in_room == ch->in_room) &&
		(number(0, 100)+GET_LEVEL(ch) >= 50)) {
		act("$n sussurra, 'Quindi, $N! Non vuoi condividere la mia tristezza!", 1, ch, 0, FIGHTING(ch), TO_NOTVICT);
		act("$n sussurra, 'Cosi', $N! Dovresti condividere la mia tristezza!", 1, ch, 0, FIGHTING(ch), TO_VICT);
		act("Il mantello di $n si apre improvvisamente appena indica $N.", 1, ch, 0, FIGHTING(ch), TO_NOTVICT);
		act("Il mantello di $n si apre improvvisamente appena ti indica.", 1, ch, 0, FIGHTING(ch), TO_VICT);
		act("Un bagliore di luce esplode sulla faccia di $N!", 1, ch, 0, FIGHTING(ch), TO_NOTVICT);
		act("Un bagliore di luce esplode sulla tua faccia!", 1, ch, 0, FIGHTING(ch), TO_VICT);
    /*   call_magic(ch, FIGHTING(ch), 0, SPELL_BLINDNESS, GET_LEVEL(ch), CAST_SPELL,NULL);*/
		return TRUE;
	}
	return FALSE;
}


SPECIAL(idiot)
{
	
	ACMD(do_say);
	if(cmd) return FALSE;
	switch (number(0, 40)) {
		case 0:
			do_say(ch, "anche un idiota puo' essere un dio", 0, 0);
			do_say(ch, "e Stu e' un idiota", 0, 0);
			do_say(ch, "Stu potrebbe essere un dio.", 0, 0);
			act("$n sorride.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 1:
			act("$n sfodera una spada sulla testa.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 2:
			act("$n non pensa tu possa confrontarti per un duello.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 3:
			do_say(ch, "Roma e' stata veramente costruita in un giorno.", 0, 0);
			act("$n snickers.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 4:
			act("$n salta e cammina sulle mani.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 5:
			act("$n fa la ruota in torno alla stanza.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 6:
			do_say(ch, "Quanti cammelli possono passare dalla cruna di un ago?", 0, 0);
			act("$n si ferma ride e poi riparte.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 7:
			do_say(ch, "Uh huh. Uh huh huh.", 0, 0);
			return TRUE;
		case 8:
			act("$n ti guarda.", FALSE, ch, 0, 0,TO_ROOM);
			act("$n piroetta graziosamente.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 9:
			act("$n tamburella con il dito sulla.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 10:
			act("$n ha una tremenda guerra con i propri pensieri.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 11:
			do_say(ch, "Tutto questo...ha un significato!.", 0, 0);
			act("$n piange sulla tua spalla.", FALSE, ch, 0, 0,TO_ROOM);
			return TRUE;
		case 12:
			do_say(ch, "Ho visto un gobbo che aveva i vestiti uguali ai tuoi, molto simili...", 0, 0);
			return TRUE;
		default:
			return FALSE;
	}
	return FALSE;
}


SPECIAL(marbles)
{
	struct obj_data *tobj = (struct obj_data *)me;
	
	if (tobj->in_room == NOWHERE)
		return 0;
	
	if (CMD_IS("north") || CMD_IS("south") || CMD_IS("east") || CMD_IS("west") ||
		CMD_IS("up") || CMD_IS("down")) {
		if (number(1, 100) - GET_DEX(ch) > 50) {
			act("Scivoli su una $p e cadi.", FALSE, ch, tobj, 0, TO_CHAR);
			act("$n scivola su una $p e cade.", FALSE, ch, tobj, 0, TO_ROOM);
			GET_POS(ch) = POS_SITTING;
			return 1;
		}
		else {
			act("You slip on $p, but manage to retain your balance.", FALSE, ch, tobj, 0, TO_CHAR);
			act("$n slips on $p, but manages to retain $s balance.", FALSE, ch, tobj, 0, TO_ROOM);
		}
	}
	return 0;
}

/* I can't find an example of a mobile casting an area spell so I'm going to
	 * fake it using the magic user special.
	 */
SPECIAL(dragon_fire)
{
/*   struct char_data *dragon = (struct char_data *) me; */
/* I don't know what 'cmd' is but we never do anything if we don't breathe
	   * fire unless we're fighting.
	   */
	if (cmd || GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	
	  /* Only breathe fire 20% of the time */
	if (number(0, 4))
		return FALSE;
	
  /* We could actually pass GET_LEVEL(ch) instead of 0 for the level of the
    * breath so we could have tougher dragons.  Right now, it does damage
	   * equal to a fireball in all cases.
	   */
	call_magic(ch, NULL, NULL, SPELL_FIRE_BREATH, 0, CAST_BREATH);
  /*  damage(dragon, FIGHTING(dragon), <damage here>, 0);*/
	
/* If you use the damage call, you don't need the spell, but if you use
 * the spell, you should add the no_magic room information below.
 */
	return TRUE;
}

SPECIAL(dragon_gas)
{
	  /*   struct char_data *dragon = (struct char_data *) me; */
	
	  /* I don't know what 'cmd' is but we never do anything if we don't breathe
	   * gas unless we're fighting.
	   */
	if (cmd || GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	
	  /* Only breathe gas 20% of the time */
	if (number(0, 4))
		return FALSE;
  /* We could actually pass GET_LEVEL(ch) instead of 0 for the level of the
    * breath so we could have tougher dragons.  Right now, it does damage
	   * equal to a fireball in all cases.
	   */
	call_magic(ch, NULL, NULL, SPELL_GAS_BREATH, 0, CAST_BREATH);
	  /*  damage(dragon, FIGHTING(dragon), <damage here>, 0);*/
/* If you use the damage call, you don't need the spell, but if you use
 * the spell, you should add the no_magic room information below.
 */
	return TRUE;
}

SPECIAL(dragon_frost)
{
	  /*   struct char_data *dragon = (struct char_data *) me;
 */
	
   /* I don't know what 'cmd' is but we never do anything if we don't breathe
	   * frost unless we're fighting.
	   */
	if (cmd || GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	
	  /* Only breathe frost 20% of the time */
	if (number(0, 4))
		return FALSE;
	
  /* We could actually pass GET_LEVEL(ch) instead of 0 for the level of the
   * breath so we could have tougher dragons.  Right now, it does damage
	   * equal to a fireball in all cases.
	   */
	call_magic(ch, NULL, NULL, SPELL_FROST_BREATH, 0, CAST_BREATH);
 /*  damage(dragon, FIGHTING(dragon), <damage here>, 0);*/
/* If you use the damage call, you don't need the spell, but if you use
 * the spell, you should add the no_magic room information below.
 */
	return TRUE;
}

SPECIAL(dragon_acid)
{
	  /*   struct char_data *dragon = (struct char_data *) me;
*/
	
   /* I don't know what 'cmd' is but we never do anything if we don't breathe
	   * acid unless we're fighting.
	   */
	if (cmd || GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	
	  /* Only breathe acid 20% of the time */
	if (number(0, 4))
		return FALSE;
	
    /* We could actually pass GET_LEVEL(ch) instead of 0 for the level of the
      * breath so we could have tougher dragons.  Right now, it does damage
	   * equal to a fireball in all cases.
	   */
	call_magic(ch, NULL, NULL, SPELL_ACID_BREATH, 0, CAST_BREATH);
	  /*  damage(dragon, FIGHTING(dragon), <damage here>, 0);*/
/* If you use the damage call, you don't need the spell, but if you use
 * the spell, you should add the no_magic room information below.
 */
	return TRUE;
}

SPECIAL(dragon_lightning)
{
  /*struct char_data *dragon = (struct char_data *) me;*/
	
/* I don't know what 'cmd' is but we never do anything if we don't breathe
	   * lightning unless we're fighting.
	   */
	if (cmd || GET_POS(ch) != POS_FIGHTING)
		return FALSE;
	
	  /* Only breathe lightning 20% of the time */
	if (number(0, 4))
		return FALSE;
	
  /* We could actually pass GET_LEVEL(ch) instead of 0 for the level of the
   * breath so we could have tougher dragons.  Right now, it does damage
 * equal to a fireball in all cases.
*/
	call_magic(ch, NULL, NULL, SPELL_LIGHTNING_BREATH, 0, CAST_BREATH);
 /*  damage(dragon, FIGHTING(dragon), <damage here>, 0);*/
/* If you use the damage call, you don't need the spell, but if you use
 * the spell, you should add the no_magic room information below.
 */
	return TRUE;
}

	/* This is the special for the brass dragon in the desert */
SPECIAL(dragon_guard)
{
	int i;
	extern int guild_info[][3];
	struct char_data *dragon = (struct char_data *) me;
	char *buf = "The dragon growls at you, blocking your way.\r\n";
	char *buf2 = "The dragon scares $n by growling fiercely, stopping $m in $s tracks.";
	
  /* Check to see if a character is trying to move past the dragon */
	if (IS_MOVE(cmd) && !IS_AFFECTED(dragon, AFF_BLIND)) {
		for (i = 0; guild_info[i][0] != -1; i++) {
			if ((IS_NPC(ch) || GET_CLASS(ch) != guild_info[i][0]) &&
				world[ch->in_room].number == guild_info[i][1] &&
				cmd == guild_info[i][2]) {
				send_to_char(buf, ch);
				act(buf2, FALSE, ch, 0, 0, TO_ROOM);
				return TRUE;
			}
		}
	}
	
	  /* Nothing left to do except cast spells if we are fighting */
	if (cmd || GET_POS(ch) != POS_FIGHTING)
		return FALSE;
   /* Only breathe lightning 20% of the time */
	if (number(0, 4))
		return FALSE;
	
   /* We could actually pass GET_LEVEL(ch) instead of 0 for the level of the
* breath so we could have tougher dragons.  Right now, it does damage
* equal to a fireball in all cases.
*/
	call_magic(ch, NULL, NULL, SPELL_LIGHTNING_BREATH, 0, CAST_BREATH);
/*  damage(dragon, FIGHTING(dragon), <damage here>, 0);*/
/* If you use the damage call, you don't need the spell, but if you use
 * the spell, you should add the no_magic room information below.
 */
	return TRUE;
}

/* Seguono le routine per la procedura dello stalliere, scritte da Lance */

#define GREAT_HEIGHT 0
#define SMALL_HEIGHT 1
#define MAX_KG_SMALL_MOUNT 300
#define MOVE_FOR_MIN 50
#define MOVE_FOR_LEVEL 20
#define OBJVNUM_PEGNO_STALLA 6
#define COST_RENT_SMALL 100
#define COST_RENT_GREAT 200
#define COST_MOUNT_LIV 10

int get_renthorse_offer(struct char_data * ch,struct char_data * recep)
{
	int offer;
	
  /* RIDING(ch) deve esistere prima di chiamare questa funzione */
	
	if (RIDING(ch)->player.weight < MAX_KG_SMALL_MOUNT) offer = COST_RENT_SMALL;
	else offer = COST_RENT_GREAT;
	
	offer += RIDING(ch)->player.level * COST_MOUNT_LIV;
	sprintf(buf,"$n ti dice, 'Viene %d monete al giorno.'",offer);
	act(buf,FALSE, recep, 0, ch, TO_VICT);
	return offer;
}

int gen_horserent(struct char_data * ch, struct char_data * recep,
	int cmd, char *arg , int staz)
{
	ACMD(do_action);
	extern struct char_data *give_find_vict(struct char_data * ch, char *arg);
	extern struct char_data *mob_proto;
	
	int cost = 0;
	struct obj_data *ricev;
	struct char_data *mount;
	float num_of_mud_hours, num_of_days;
	struct extra_descr_data *new_descr;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	mob_rnum real_mount;
	
	char *action_table[] = { "smile", "dance", "sigh", "blush", "burp",
		"cough", "fart", "twiddle", "yawn" };
	
	mount = RIDING(ch);
	
	if (!ch->desc || IS_NPC(ch)) return FALSE;
	
	if (!cmd && !number(0, 5))
	{
		do_action(recep, NULL, find_command(action_table[number(0, 8)]), 0);
		return FALSE;
	}
	
	if (!CMD_IS("offer") && !CMD_IS("rent") && !CMD_IS("give")&& !CMD_IS("dai")&& !CMD_IS("offerta"))
		return FALSE;
	
	if (!AWAKE(recep))
	{
		send_to_char("Non e' in grado di parlarti...\r\n", ch);
		return TRUE;
	}
	
	if (!CAN_SEE(recep, ch))
	{
		act("$n dice, 'Non tratto con persone che non posso vedere!'", FALSE, recep, 0, 0, TO_ROOM);
		return TRUE;
	}
	
	if (CMD_IS("rent"))
	{
		if (mount)
		{
			if (!CAN_SEE(recep,mount)) /*NON VEDE LA CAVALCATURA*/
			{
				act("$n ti dice, 'Non vedo nessuna cavalcatura con te!'", FALSE, recep, 0, ch, TO_VICT);
				return TRUE;
			}
			
			if ((staz==SMALL_HEIGHT) && (mount->player.height > MAX_KG_SMALL_MOUNT))
			{
				act("$n ti dice, 'Hei! Porta fuori quel bestione!.'",FALSE, recep, 0, ch, TO_VICT);
				return TRUE;
			}
			
      /*Mounted*/
			if (!MOB_FLAGGED(mount, MOB_MOUNTABLE))
			{
				act("$n ti dice, 'Accetto soltanto cavalcature!'", FALSE, recep, 0, ch, TO_VICT);
				return TRUE;
			}
			
      /*La cavalcatura non deve avere oggetti*/
			if (IS_CARRYING_N(mount)!=0)
			{
				act("$n ti dice, 'Mi spiace ma non rispondo della roba che hai sulla cavalcatura, prenditela!.'",FALSE, recep, 0, ch, TO_VICT);
				return TRUE;
			}
			
			if (!(cost = get_renthorse_offer(ch,recep))) return TRUE;
			sprintf(buf, "$n ti dice, 'La custodia della tua cavalcatura ti costa %d monete al giorno.'", cost);
			
      /*Troppo caro*/
			if (cost > GET_GOLD(ch))
			{
				act("$n ti dice, '...ma vedo che non puoi pagare tanto.'", FALSE, recep, 0, ch, TO_VICT);
				return TRUE;
			}
			
      /*Ricevuta*/
			ricev = read_object(OBJVNUM_PEGNO_STALLA, VIRTUAL);
			SET_BIT(GET_OBJ_EXTRA(ricev), ITEM_RESTRING);
			sprintf(buf, "Una ricevuta per la cavalcatura rilasciato da %s e' stato lasciato qui", GET_NAME(recep));
			ricev->description = str_dup(buf);
			sprintf(buf, "Una ricevuta per la cavalcatura (%s) rilasciato da %s", GET_NAME(mount), GET_NAME(recep));
			ricev->short_description = str_dup(buf);
			CREATE(new_descr, struct extra_descr_data, 1);
			new_descr->keyword = str_dup(ricev->name);
			sprintf(buf, "Un pegno per la cavalcatura (%s) rilasciato da %s.\r\n"
				"Per riavere la cavalcatura consegnare questo pegno allo stalliere.\r\n"
				"Costo della custodia al giorno: %d monete.\r\n", GET_NAME(mount), GET_NAME(recep), cost);
			new_descr->description = str_dup(buf);
			ricev->ex_description = new_descr;
			
			GET_OBJ_VAL(ricev,0) = GET_MOB_VNUM(mount);
			GET_OBJ_VAL(ricev,1) = GET_MOVE(mount);
			GET_OBJ_VAL(ricev,2) = world[IN_ROOM(mount)].number;
			GET_OBJ_VAL(ricev,3) = time(0);
			
      /*consegna l'oggetto (pegno)*/
			obj_to_char(ricev,ch);
      /*smonti da cavallo*/
			dismount_char(ch);
			act("Scendi dalla tua cavalcatura.",FALSE, recep, 0, ch, TO_VICT);
			act("$n scende dalla sua cavalcatura.", FALSE, recep, 0, ch, TO_NOTVICT);
      /*Rimuovi mount*/
			extract_char(mount);
			act("!!SOUND(cavallo.wav)$n porta la tua cavalcatura nella stalla.",FALSE, recep, 0, ch, TO_VICT);
			act("!!SOUND(cavallo.wav)$n porta la cavalcatura di $N nella stalla.", FALSE, recep, 0, ch, TO_NOTVICT);
		}
		else
		{
			act("$n ti dice, 'Non vedo nessuna cavalcatura con te!'", FALSE, recep, 0, ch, TO_VICT);
			return TRUE;
		}
	}
	
  /*Offerta*/
	if (CMD_IS("offer")||CMD_IS("offerta"))
	{
		if (RIDING(ch))
		{
			get_renthorse_offer(ch,recep);
			act("$N fa un offerta a $n.", FALSE, ch, 0, recep, TO_ROOM);
		}
		else
		{
			act("$n ti dice, 'Non vedo nessuna cavalcatura con te!'", FALSE, recep, 0, ch, TO_VICT);
			return 1;
		}
	}
	
	if (CMD_IS("give")||CMD_IS("dai"))
	{
		if (cmd && *arg && arg != "")
			arg = one_argument(arg, arg1);
		if (cmd && *arg && arg != "")
			one_argument(arg, arg2);
		
		if (give_find_vict(ch, arg2)==recep)
		{
			ricev = get_obj_in_list_vis(ch, arg1, ch->carrying);
			if (ricev && (GET_OBJ_TYPE(ricev)==ITEM_RENT_MOUNT) && (world[IN_ROOM(recep)].number == GET_OBJ_VAL(ricev,2)))
			{
				real_mount = real_mobile(GET_OBJ_VAL(ricev, 0));
				if (real_mount == -1)
				{
					send_to_char("Quella cavalcatura non e' piu' disponibile, contattare un creatore per un rimpiazzo\r\n", ch);
					return TRUE;
				}
				
				num_of_days = (float) (time(0) - GET_OBJ_VAL(ricev,3)) / SECS_PER_REAL_DAY;
				cost = (mob_proto[real_mount].player.weight < MAX_KG_SMALL_MOUNT ? COST_RENT_SMALL : COST_RENT_GREAT);
				cost += mob_proto[real_mount].player.level * COST_MOUNT_LIV;
				cost = (float) cost * MAX(num_of_days,1);
				
				if (cost > GET_GOLD(ch) + GET_BANK_GOLD(ch))
				{
					sprintf(buf, "$n ti dice, 'Ti servono %d monete per ritirare la tua cavalcatura", cost);
					act(buf, FALSE, recep, 0, ch, TO_VICT);
					return TRUE;
				}
				else
				{
					GET_BANK_GOLD(ch) -= MAX(cost - GET_GOLD(ch), 0);
					GET_GOLD(ch) = MAX(GET_GOLD(ch) - cost, 0);
					save_char(ch, NOWHERE);
					act("!!SOUND(cavallo.wav)$n ti porta la tua cavalcatura.",FALSE, recep, 0, ch, TO_VICT);
					act("!!SOUND(cavallo.wav)$n prende la cavalcatura di $N dalla stalla.", FALSE, recep, 0, ch, TO_NOTVICT);
					mount = read_mobile(real_mount, REAL);
					char_to_room(mount, ch->in_room);
					num_of_mud_hours = (float) (time(0) - GET_OBJ_VAL(ricev, 3)) / SECS_PER_MUD_HOUR;
					GET_MOVE(mount) = MIN(GET_OBJ_VAL(ricev, 1) + move_gain(mount) * num_of_mud_hours, GET_MAX_MOVE(mount));
					extract_obj(ricev);
				}
			}
			else act("$n ti dice,'Mi spiace, ma la cosa non mi riguarda'.",FALSE, recep, 0, ch, TO_VICT);
		} /*if Arg2==recep*/
		else return FALSE;
	} /*Give*/
	
	return TRUE;
}


SPECIAL(horseman)
{
	return  gen_horserent(ch, (struct char_data *)me, cmd, argument,SMALL_HEIGHT);
}

SPECIAL(dragonman)
{
	return  gen_horserent(ch, (struct char_data *)me, cmd, argument,GREAT_HEIGHT);
}

SPECIAL(dragonact)
{
	int caso;
	struct char_data *vict;
	struct char_data *drago;
	extern void dismount_char(struct char_data *ch);
	extern int parse_dir(char *st);
	
	if (parse_dir(cmd_info[cmd].command)!=-1) return FALSE;
	if (CMD_IS("vola")) return FALSE;
	
	if (!ch || !RIDING(ch)) return FALSE;
	
	caso = number(0,100);
	
	if (caso > 7) return FALSE;
	
	drago = RIDING(ch);
	
	if (FIGHTING(ch) || FIGHTING(drago)) return FALSE;
	
	switch (caso)
	{
		case 0:
			 /* pseudo-randomly choose someone in the room who is fighting me */
			if (GET_POS(drago) > POS_SLEEPING)
			{
				for (vict = world[drago->in_room].people; vict; vict = vict->next_in_room)
					if (vict && vict!=drago && vict!=ch && !number(0, 2) && !RIDDEN_BY(vict)){
						if (!number(0, 1))
						{
							act("&8$n ti colpisce accidentalmente con la coda!.\r\nCadi a terra.&0\r\n",FALSE, drago, 0, vict, TO_VICT);
							act("&8$n ti colpisce accidentalmente $N con la coda!.\r\n$N cade a terra.&0\r\n",FALSE, drago, 0, vict, TO_NOTVICT);
							if (RIDING(vict))
								dismount_char(vict);
							GET_POS(vict) = POS_SITTING;
						}
						else
							act("&8$n ti sfiora accidentalmente con la coda!&0\r\n",FALSE, drago, 0, vict, TO_VICT);
					}
			}
			break;
		case 1:
			{ /**/
				if (GET_POS(drago) > POS_SLEEPING)
					act("&8$n muove la coda e per poco non rade al suolo la zona!&0\r\n",FALSE, drago, 0, ch, TO_ROOM);
				break;
			}
		case 2:
			{ /**/
				if (GET_POS(drago) > POS_SLEEPING)
					act("&8$n emette un enorme boato!&0\r\n",FALSE, drago, 0, ch, TO_ROOM);
				break;
			}
		case 3:
			if (GET_POS(drago) > POS_SLEEPING)
			{
				for (vict = world[drago->in_room].people; vict; vict = vict->next_in_room)
					if (vict && vict!=ch && vict!=drago && !number(0, 1) && !RIDDEN_BY(vict))
					{
						act("&8$n provoca una raffica di vento sbuffando verso di te!&0\r\n",FALSE, drago, 0, vict, TO_VICT);
						act("&8$n provoca una raffica di vento sbuffando!&0\r\n",FALSE, drago, 0, vict, TO_NOTVICT);
					}
					
			}
			break;
		case 4:
			if (GET_POS(drago) > POS_SLEEPING)
				act("&8$n apre le sue enormi ali!&0\r\n",FALSE, drago, 0, ch, TO_ROOM);
			break;
		case 5:
			if (GET_POS(drago) > POS_SLEEPING){
				act("&8$n volta la testa verso il suo padrone!&0\r\n",FALSE, drago, 0, ch, TO_ROOM);
			}
			break;
		case 6:
			if (GET_POS(ch) > POS_SLEEPING){
				act("&8$n ti osserva!&0\r\n",FALSE, drago, 0, ch, TO_ROOM);
			}
			break;
		case 7:
			if (GET_POS(drago) > POS_SLEEPING)
				act("&8$n si accuccia a terra!&0\r\n",FALSE, drago, 0, ch, TO_ROOM);
			break;
		default: ;
	}
	
	return FALSE;
}


#define VNUM_TORTA_FINTA 80

#define VNUM_KIT_EVASIONE 81



SPECIAL(secondino)

{

	ACMD(do_say);

	ACMD(do_look);

	void unstow_char(struct char_data *ch);

	void stow_char(struct char_data *ch,struct char_data *mount);

	void do_doorcmd(struct char_data *ch,struct obj_data *obj,int door,int scmd);

	int find_prison(struct char_data *ch);

	char * regno_da_prigione(int room_prigione);
	void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia);

	struct obj_data *get_obj_in_list_vis_vnum(struct char_data *ch,int vnumber,struct obj_data * list);

	int crime_reward(struct char_data *ch, int clan);

	void perform_give_gold(struct char_data * ch, struct char_data * vict,

		int amount);

	int notorieta_reward(struct char_data *hunter,struct char_data *prisoner);

	struct char_data *secondino = (struct char_data *) me;

	struct char_data *tch,*vict;

	extern sh_int find_clan_by_id(int idnum);

	extern struct clan_rec clan[MAX_CLANS];

	

	char str[MAX_STRING_LENGTH],str2[MAX_STRING_LENGTH];

	struct obj_data *obj;

	int i, j, prison_room;

	bool found=FALSE;

	int idclan;

	long coins;

	

  //Il secondino deve avere un clan

	if (CLAN_ID(secondino)>0)

		idclan = CLAN_ID(secondino);

	else

		return FALSE;

	

	if(!TRANSPORTED(secondino)){

		for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {

			if(!IS_NPC(tch)){

				if(TRANSPORTED(tch))

				{

					vict=TRANSPORTED(tch);

					if((IS_NPC(vict)&&(MOB_FLAGGED(vict,MOB_CRIMINAL)))||IS_CRIMINAL(vict)||IS_A_CRIMINAL_CLAN(vict,idclan))

					{

						do_say(secondino,"Bene vedo che mi hai portato un prigioniero!",0,0);

						WAIT_STATE(secondino,PASSES_PER_SEC);

						act("$n si frega le mani soddisfatto.",FALSE,secondino,0,0,TO_ROOM);

						WAIT_STATE(secondino,PASSES_PER_SEC);

						do_say(secondino,"Forza consegnamelo,ti daro' subito la ricompensa!",0,0);

					} else{

						do_say(secondino,"Perche ti porti sulle spalle quel povero cristo?",0,0);

						do_say(secondino,"Che ti ha fatto di male?",0,0);

					}

				}

				else

				if(!IS_NPC(tch))

					if(found==FALSE)

						if (number(1,10)>7){

							if (CLAN_ID(secondino)<=0)

								act("$n ha una crisi di identita' non sa a che regno appartiene.",FALSE,secondino,0,0,TO_ROOM);

							do_say(secondino,"Hai mica visto per caso qualche criminale in giro?",0,0);

							found=TRUE;

						}

				}		

		}

		

	if (CMD_IS("consegna")) {

		argument = two_arguments(argument, str,str2);

		prison_room=find_prison(secondino);

		if(!*str){

		    send_to_char("Cosa vuoi consegnare ai prigionieri?\r\n",ch);

		    return TRUE;

		}

		if(!*str2){

		    send_to_char("Sarebbe un idea specificare a chi consegnare gli oggetti.\r\n",ch);

		    return TRUE;

		}

		if(!(obj = get_obj_in_list_vis(ch,str,ch->carrying))){

		    sprintf(buf,"Non sembra tu abbia %s.\r\n",str);

		    send_to_char(buf,ch);

		    return TRUE;

		}

		

		

		    

		if(!(vict=get_char_vis(secondino,str2))){

		    send_to_char(NOPERSON,ch);

		    return TRUE;

		    }

		if(vict->in_room != real_room(prison_room)){

		    send_to_char("Non c'e' nessuno in cella con quel nome.\r\n",ch);

		    return TRUE;

		}    

		if(GET_OBJ_VNUM(obj)!=VNUM_TORTA_FINTA){

		    do_say(secondino,"Non posso consegnare questo tipo di oggetti ai prigionieri.",0,0);

		    return TRUE;

		}		    

		if (IS_NPC(ch)){

			do_say(secondino, "Non mi ingannare! Se vuoi consegnare il pacco prenditi i tuoi rischi e non farlo fare ad un tuo schiavo!", 0, 0);

			return TRUE;

		}

		act("$n inizia ad esaminare con cautela i beni da consegnare al prigioniero.",FALSE,secondino,0,0,TO_ROOM);

		if(number(1,100)>GET_LEVEL(ch)+10){

		do_say(secondino,"Bene bene,puoi portarglielo anche tu.",0,0);

		act("$n apre la porta e ti spinge nella cella.",FALSE,secondino,0,0,TO_ROOM);    

		send_to_char("&8&7Sara' una mia impressione,ma ti hanno beccato.&0\r\n",ch);

		
		obj_from_char(obj);



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

		char_from_room(ch);

		char_to_room(ch,real_room(prison_room));

		do_look(ch,"",0,0);

		send_to_char("Il carceriere ti deride,si accende una sigaretta e si allontana.&0\r\n",ch);

		sprintf(buf,"Tentava di far evadere %s.",GET_NAME(vict));
		set_crime(ch,vict,"vivo",buf,find_clan_by_id(CLAN_ID(secondino)),CLAN_CORROTTO, 10000);

		return TRUE;

		}

		act("$n entra in cella e consegna l'oggetto.",FALSE,secondino,0,0,TO_ROOM);

		obj_from_char(obj);

		obj_to_char(obj,vict);

		send_to_char("Il tuo carceriere ti chiama in disparte e ti consegna un oggetto.\r\n",vict);

		do_say(secondino,"Potete anche andare ora.",0,0);

		act("$n si siede beato sulla sua sedia e si accende una sigaretta.",FALSE,secondino,0,0,TO_ROOM);

		return TRUE;

	}		    



		

		if(CMD_IS("prigioniero")){

			if(!TRANSPORTED(ch)){

				do_say(secondino,"Mi sa che ti e' scappato ,non hai nessuno con te,pollo!",0,0);

				act("$n ridacchia discretamente e si accende una sigaretta.",FALSE,secondino,0,0,TO_ROOM);

				return TRUE;

			}

			if(TRANSPORTED(ch)){

				vict=TRANSPORTED(ch);

				act("$n squadra per bene $N.",FALSE,secondino,0,vict,TO_ROOM);

				WAIT_STATE(secondino,PASSES_PER_SEC);

				act("$n consulta freneticamente le foto segnaletiche dei ricercati.",FALSE,secondino,0,0,TO_ROOM);

				WAIT_STATE(secondino,PASSES_PER_SEC);

         /*new criminal*/

				if((!IS_NPC(vict)&&!(IS_CRIMINAL(vict) || IS_A_CRIMINAL_CLAN(vict,idclan) ))||(IS_NPC(vict)&&!MOB_FLAGGED(vict,MOB_CRIMINAL))){

					do_say(secondino,"Beh ,un prigioniero ce l'hai ma non e' ricercato!",0,0);

					WAIT_STATE(secondino,PASSES_PER_SEC);

					do_say(secondino,"QUINDI LASCIALO ANDARE SUBITO O ARRESTO TE!",0,0);

					act("$n e' notevolmente alterato.",FALSE,secondino,0,0,TO_ROOM);

					return TRUE;

				}

				

				if (IS_A_CRIMINAL_CLAN(vict,idclan)||IS_CRIMINAL(vict)||(IS_NPC(vict)&&MOB_FLAGGED(vict,MOB_CRIMINAL))){

					do_say(secondino,"Ottimo lavoro, un'altro criminale assicurato alla giustizia!",0,0);

					sprintf(buf,"%s prende dalle tue spalle %s,e se lo carica in spalla.",GET_NAME(secondino),GET_NAME(vict));

					act(buf,FALSE,ch,0,0,TO_CHAR);

					sprintf(buf,"$n prende $N dalle spalle di %s,e se lo carica in spalla.",GET_NAME(ch));

					act(buf,FALSE,secondino,0,vict,TO_NOTVICT);

					unstow_char(ch);

					stow_char(vict,secondino);
					
					//WAIT_STATE(secondino,PASSES_PER_SEC);

					//act("$n apre una cassaforte e prende del denaro.",FALSE,secondino,0,0,TO_ROOM);

					//WAIT_STATE(secondino,PASSES_PER_SEC);

					//coins = crime_reward(vict,idclan);

					//GET_GOLD(secondino)=coins;

					//perform_give_gold(secondino,ch,coins);

					//clan[find_clan_by_id(idclan)].economy.treasure-=coins;

					//clan[find_clan_by_id(idclan)].economy.current_uscite-=coins;

					//sprintf(buf, "(REGNI) %s preleva %ld coins nel regno di %s per pagare %s.", GET_NAME(secondino), coins ,clan[find_clan_by_id(idclan)].name,GET_NAME(ch));

					//mudlog(buf, BRF, LVL_GOD, TRUE);

					send_to_char("Complimenti ,la tua &1&bNotorieta&0 e' aumentata.\n",ch);

					GET_NOTORIETA(ch)+=notorieta_reward(ch,vict);

					

					if (!IS_NPC(vict)) {

						vict->player_specials->saved.num_kills = 0;

						vict->player_specials->saved.num_tkills = 0;

						vict->player_specials->saved.num_thief = 0;

						vict->player_specials->saved.num_warkills = 0;

						vict->player_specials->saved.num_wanted = 0;

						save_char(vict, vict->in_room);

					}

					return TRUE;

				}

			} //for

		}

		return FALSE;

	}

	

	

	if(TRANSPORTED(secondino)){

		prison_room=find_prison(secondino);

		vict=TRANSPORTED(secondino);

		if((obj=get_obj_in_list_vis_vnum(vict,VNUM_TORTA_FINTA,vict->carrying))){

		    send_to_char("Devo prima prendere un po di cose che non puoi portare in cella\r\n.",ch);

		    obj_from_char(obj);

		    return TRUE;

		    }

		if((obj=get_obj_in_list_vis_vnum(vict,VNUM_KIT_EVASIONE,vict->carrying))){

		    send_to_char("Devo prima prendere un po di cose che non puoi portare in cella\r\n.",ch);

		    obj_from_char(obj);

		    return TRUE;

		    }

		obj=get_obj_in_list_vis_vnum(vict,VNUM_TORTA_FINTA,vict->carrying);

		while(obj){

		    obj_from_char(obj);

		    obj=get_obj_in_list_vis_vnum(vict,VNUM_TORTA_FINTA,vict->carrying);

		}

		obj=get_obj_in_list_vis_vnum(vict,VNUM_KIT_EVASIONE,vict->carrying);

		while(obj){

		    obj_from_char(obj);

		    obj=get_obj_in_list_vis_vnum(vict,VNUM_KIT_EVASIONE,vict->carrying);

		}

		if(prison_room!=0){

			for(i=0,j=-1;i<NUM_OF_DIRS;i++){

				if(EXIT(secondino, i)){

					if( EXIT(secondino, i)->to_room == real_room(prison_room))

						j=i;

				}

			}

			

			if(j>=0) {	  

				//do_doorcmd(secondino,NULL, j, SCMD_UNLOCK);

				//do_doorcmd(secondino,NULL, j, SCMD_OPEN);

				act("$n apre la cella",FALSE,secondino,0,0,TO_ROOM);

				act("$n entra nella cella",FALSE, secondino,0,0,TO_ROOM);

				//perform_move(secondino, j, 0,CAN_SHOW_ROOM);

				char_from_room(TRANSPORTED(secondino));

				char_to_room(TRANSPORTED(secondino), real_room(prison_room));

				act("$n apre la cella",FALSE,TRANSPORTED(secondino),0,0,TO_ROOM);

				act("$n entra nella cella",FALSE, TRANSPORTED(secondino),0,0,TO_ROOM);

				act("$N sbatte a terra in malo modo $n",FALSE,TRANSPORTED(secondino),0,secondino,TO_ROOM);	

				act("$N ti sbatte a terra in malo modo ",FALSE,TRANSPORTED(secondino),0,secondino,TO_VICT);

				act("$n butta in cella $N",FALSE,secondino ,0,TRANSPORTED(secondino),TO_ROOM);

				vict = TRANSPORTED(secondino);

				unstow_char(secondino);

				save_char(vict, vict->in_room);

				//i=0;

				//while(i<NUM_OF_DIRS &&!CAN_GO(secondino,i))

				//  i++;/*qui crasha*/

				//if(i<NUM_OF_DIRS)

				//  perform_move(secondino, i, 0,CAN_SHOW_ROOM);	

				act("$N esce dalla cella",FALSE, vict,0,secondino,TO_ROOM);

				act("$n esce dalla cella",FALSE, secondino,0,0,TO_ROOM);

				act("$n chiude la cella",FALSE, secondino,0,0,TO_ROOM);

				//do_doorcmd(secondino,NULL, j, SCMD_CLOSE);

				//do_doorcmd(secondino,NULL, j, SCMD_LOCK);

			}

		} //prigione !=0

		return FALSE;

	}

	return FALSE;

}




#define ABIL_MAESTRO_ERR 5

SPECIAL(maestro_errante){
	struct char_data *maestro=(struct char_data *)me;
	int abil_num,liv_abil,i;
	bool found=FALSE;
	void update_abilita(struct char_data *ch,int abilita,int new_liv);
	extern struct abil_info_type abil_info[];
	extern char *abilita[];
	ACMD(do_say);
	if(cmd){
		if (!CMD_IS("learn") && !CMD_IS("impara"))
			return FALSE;
		skip_spaces(&argument);
		if (!*argument)
		{
			do_say(maestro,"Posso insegnarti i rudimenti di queste abilita:",0,0);
			for(i=0;i<ABIL_MAESTRO_ERR;i++){
				sprintf(buf,"\t%s\n",abilita[maestro->char_specials.mob_abil[i]]);
				send_to_char(buf,ch);
			}
			do_say(maestro,"Le lezioni costano 10K l'una",0,0);
			return TRUE;
		}
		
		abil_num = find_abil_num(argument);
		//sprintf(buf,"ABIL NUM:%d\n",abil_num);
		//send_to_char(buf,ch);
		if (abil_num < 1 || abil_num>TOP_ABILITA_DEFINE || abil_info[abil_num].class_users[(int)GET_CLASS(ch)]!=1)
		{
			do_say(maestro, "&2&bNon sembri molto portato per questa abilita&0!.",0,0);
			return TRUE;
		}
		
		if ((liv_abil=GET_ABIL(ch, abil_num))>0)
		{
			do_say(maestro,"Posso solo insegnarti le basi di questa abilita.",0,0);
			do_say(maestro,"&4Basi che gia possiedi....mi spiace!&0",0,0);
			return TRUE;
		}
		if (GET_ABIL_PRACS(ch) ==0)
		{
			send_to_char("&2Non sembri pronto per imparare ora.&0", ch);
			return TRUE;
		}
		for(i=0,found=FALSE;i<ABIL_MAESTRO_ERR;i++){
			if(maestro->char_specials.mob_abil[i]==abil_num)
				found=TRUE;
		}
		if(found==TRUE){
			if(GET_GOLD(ch)>=10000){
				sprintf(buf,"%s, con molta calma ti insegna i rudimenti di &1&b%s&0.\r\n",GET_NAME(maestro),abilita[abil_num]);
				send_to_char(buf,ch);
				send_to_char("E si prende le sue &1 50000&0 monete!",ch);
				GET_GOLD(ch)-=10000;
				GET_ABIL_PRACS(ch)-=1;
				update_abilita(ch,abil_num,liv_abil+1);
				return TRUE;
			} else {
				do_say(maestro,"&4Le mie lezioni costano&0 10mila monete&4 l'una,tu non le hai!&0",0,0);
				do_say(maestro,"&4Torna quando sarai piu ricco!&0",0,0);
			}
		}
		if(found==FALSE){
			do_say(maestro,"Posso insegnarti i rudimenti di queste abilita:",0,0);
			for(i=0;i<ABIL_MAESTRO_ERR;i++){
				sprintf(buf,"\t&5&b%s&0\n",abilita[maestro->char_specials.mob_abil[i]]);
				send_to_char(buf,ch);
			}
			do_say(maestro,"Le lezioni costano 50K l una.",0,0);
			return TRUE;
		}
	}
	switch(number(1,8)){
		case 1:
		case 3:
		case 5:
		case 7:
			do_say(maestro,"&1Ave a te viandante!&0",0,0);
			break;
		case 2:
		case 4:
		case 6:
		case 8:
		default:
			do_say(maestro,"&1Posso esserti d' aiuto?&0 <digita LEARN>",0,0);
			break;
	}
	return FALSE;
}

SPECIAL(maestro_santuario){
	struct char_data *maestro=(struct char_data *)me;
	int abil_num,abil_learn,liv_abil,gold_need=0,i;
	bool can_learn=FALSE;
	char tmp_buf[300];
	void update_abilita(struct char_data *ch,int abilita,int new_liv);
	extern struct abil_info_type abil_info[];
	extern char *abilita[];
	ACMD(do_say);
	
	abil_num=maestro->char_specials.mob_abil[0];
	if(abil_num>0&&abil_num<20)
		can_learn=TRUE;
	
	if(cmd){
		if (!CMD_IS("learn") && !CMD_IS("impara"))
			return FALSE;
		skip_spaces(&argument);
		if (!*argument)
		{
			if(can_learn==TRUE){
				sprintf(tmp_buf,"Ti posso insegnare molte cose sull' abilita &1&b%s&0\n\r",abilita[abil_num]);
				send_to_char(tmp_buf,ch);
				do_say(maestro,"Naturalmente solo se darai un piccolo contributo a questo santuario..",0,0);
				do_say(maestro,"Diciamo...&1&b1000&0 monete",0,0);
			}
			else
				do_say(maestro,"Mi spiace ..non ti posso insegnare nulla!",0,0);
			return TRUE;
		}
		abil_learn = find_abil_num(argument);
		if(abil_num!=abil_learn){
			do_say(maestro, "Non posso insegnarti nulla di questa abilita!",0,0);
			return TRUE;
		}
		if (abil_info[abil_num].class_users[(int)GET_CLASS(ch)]<0)
		{
			do_say(maestro, "&2&bNon sembri molto portato per questa abilita&0!.",0,0);
			return TRUE;
		}
		
		can_learn=(abil_info[abil_num].class_users[(int)GET_CLASS(ch)]>=0);
		
		if((liv_abil=GET_ABIL(ch,abil_num))<=0){
			for(i=0,can_learn=FALSE;i<3;i++)
				if(ch->player_specials->saved.abil_choice[i]==abil_num)
					can_learn=TRUE;
				
			if(can_learn==FALSE){
				do_say(maestro,"Ma non conosci proprio Nulla di questa abilita!",0,0);
				do_say(maestro,"Torna quando saprai almeno di cosa stiamo parlando",0,0);
				return TRUE;
			} else{
				do_say(maestro,"Bene ,abbiamo un Neofita qui....",0,0);
				do_say(maestro,"Allora che aspettiamo!Forza al lavoro!",0,0);
			}
		}
		
		if (((GET_CLASS_TITLE(ch)<1) && (liv_abil >= 8)) || ((GET_CLASS_TITLE(ch)<2) && (liv_abil >= 14)))
		{
			do_say(maestro,"Non sei abbastanza esperto per poter progredire nell'abilità.",0,0);
			do_say(maestro,"Torna quando sarai piu' esperto.",0,0);
			return TRUE;
		}
		
		if(liv_abil==14)
			gold_need=10000;
		if(liv_abil<14)
			gold_need=2000;
		if(liv_abil==8)
			gold_need=1000;
		if(liv_abil<8)
			gold_need=500;
		if(liv_abil<=4)
			gold_need=300;
		
		if(liv_abil==15){
			do_say(maestro,"Caspita ,sei un &1&bMaestro&0 in questa abilita!",0,0);
			do_say(maestro,"Sei piu bravo di me,non posso insegnarti nulla che tu gia non sappia!",0,0);
			return TRUE;
		}
		if(GET_GOLD(ch)<gold_need){
			do_say(maestro,"Io non insegno gratis!",0,0);
			sprintf(buf,"Chiedo solo &1&b%d&0 monete a lezione.",gold_need);
			do_say(maestro,buf,0,0);
			do_say(maestro,"Ma tu non hai nemmeno quelle!",0,0);
			return TRUE;
		}
		if(GET_ABIL_PRACS(ch)<(liv_abil+1)){
			do_say(maestro,"Mi spiace non sei pronto per imparare ,ora",0,0);
			return TRUE;
		}
		do_say(maestro,"Bene ,iniziamo le lezioni!",0,0);
		sprintf(buf,"$n inizia ad illustrarti e a mostrarti i segreti di &1&b%s&0.",abilita[abil_num]);
		act(buf,FALSE,maestro,0,ch,TO_VICT);
		act("$n inizia a gesticolare e a parlare con $N",FALSE,maestro,0,ch,TO_NOTVICT);
		sprintf(buf,"Aumenti la tua conoscenza in &1&b%s&0.\r\n",abilita[abil_num]);
		send_to_char(buf,ch);
		GET_ABIL_PRACS(ch)-=(liv_abil+1);
		GET_GOLD(ch)-=gold_need;
		update_abilita(ch,abil_num,liv_abil+1);
		sprintf(buf,"Dai &1&b%d&0 monete al maestro\n\r",gold_need);
		send_to_char(buf,ch);
		do_say(maestro,"Bene ,torna presto a trovarmi..ciao!",0,0);
		return TRUE;
	}
	
	return FALSE;
}

#define COMANDI_MANOVALE 	( (is_abbrev(buf3,"get")) || (is_abbrev(buf3,"prendi")) ||   \
				(is_abbrev(buf3,"put")) || (is_abbrev(buf3,"metti")) ||      \
				(is_abbrev(buf3,"drop")) || (is_abbrev(buf3,"posa")) ||      \
				(is_abbrev(buf3,"give")) || (is_abbrev(buf3,"dai")) ||       \
				(is_abbrev(buf3,"wear")) || (is_abbrev(buf3,"indossa")) ||   \
				(is_abbrev(buf3,"remove")) || (is_abbrev(buf3,"rimuovi")) || \
				(is_abbrev(buf3,"hold")) || (is_abbrev(buf3,"impugna")) ||   \
				(is_abbrev(buf3,"aggancia")) || (is_abbrev(buf3,"hang")) ||  \
				(is_abbrev(buf3,"segui")) || (is_abbrev(buf3,"follow")) || \
				(is_abbrev(buf3,"siediti")) || (is_abbrev(buf3,"sit")) ||    \
				(is_abbrev(buf3,"alzati")) || (is_abbrev(buf3,"stand")) ||   \
				(is_abbrev(buf3,"parla")) || (is_abbrev(buf3,"say")) )

#define SOTTOCOMANDI_MANOVALE	"Comandi manovale che puoi usare:\r\nmanovale <manovale> padrone <nuovopadrone>\r\nmanovale <manovale> ritrovo\r\nmanovale <manovale> riposa\r\n"

SPECIAL(manovale) {
/*
	struct char_data *manovale = (struct char_data *) me;
	
	char name[100], message[256];
	char buf3[256];
	char *nome;
	bool found = FALSE;
	struct char_data *vict;
	struct char_data * k;
	
	if(cmd) {		
		if (CMD_IS("manovale")) {
			half_chop(argument, name, message);
			if (!*name || !*message) {
				send_to_char(SOTTOCOMANDI_MANOVALE,ch);
				return TRUE;
			}
			else if (!(vict = get_char_room_vis(ch, name))) {
				//if ((!is_abbrev(name, "all"))||(!is_abbrev(name, "tutti")))
//				return FALSE;		
				
			} else if (ch == vict)
				return FALSE;
			else if (!MANOVALE_OF(vict,ch)) {
				send_to_char("Non sei il suo padrone.\n\r",ch);
				return TRUE;
			}
			if (vict == manovale) {
				nome = one_argument(message,buf3);
				if (is_abbrev(buf3,"padrone")) {
					
					skip_spaces(&nome);
					vict = get_char_room_vis(ch, nome);
					
					if (!vict) {
						send_to_char("Non lo vedi.\r\n",ch);
						return TRUE;
					}
					
					
					MASTER_ID(manovale)=GET_IDNUM(vict);
					CLAN_ID(manovale)=GET_CLAN(vict);
					sprintf(buf1,"Diventi il padrone di %s.\r\n",GET_NAME(manovale));
					send_to_char(buf1,vict);
					sprintf(buf1,"%s diventa il padrone di %s.\r\n",GET_NAME(vict),GET_NAME(manovale));
					send_to_char(buf1,ch);
					save_mob(manovale, manovale->in_room);
					//SET_BIT(MOB_FLAGS(manovale),MOB_CRASH);
					return TRUE;					
				} else  if (is_abbrev(buf3,"ritrovo")) {
					GET_HOME(manovale) = IN_ROOM(manovale);
					sprintf(buf3,"Questo e' il nuovo ritrovo di %s\r\n",GET_NAME(manovale));
					send_to_char(buf3,ch);
					save_mob(manovale, manovale->in_room);
					//SET_BIT(MOB_FLAGS(manovale),MOB_CRASH);
					return TRUE;
				} else  if (is_abbrev(buf3,"riposa")) {
					act("Vedi $n allontanarsi.", FALSE, manovale, 0, 0, TO_ROOM);
					char_from_room(manovale);
					char_to_room(manovale, GET_HOME(manovale));
					act("Vedi $n avvicinarsi.", FALSE, manovale, 0, 0, TO_ROOM);
					if (manovale->master) 
						stop_follower(manovale);
					save_mob(manovale, manovale->in_room);
					//SET_BIT(MOB_FLAGS(manovale),MOB_CRASH);
					return TRUE;
				} else {
					send_to_char(SOTTOCOMANDI_MANOVALE,ch);
					return TRUE;
				}
			}
			return FALSE;
		} 
		else if ((CMD_IS("ordina"))||(CMD_IS("order"))) {
			half_chop(argument, name, message);
			
			if (!*name || !*message)
				return FALSE;
			
			else if ((!(vict = get_char_room_vis(ch, name)))&&((!is_abbrev(name, "all")&&!is_abbrev(name, "tutti")))) {
				return FALSE;
				
			} else if (ch == vict)
				return FALSE;
			
			if (vict == manovale) {
				sprintf(buf3, "$N ti ordina di '%s'", message);
				act(buf3, FALSE, vict, 0, ch, TO_CHAR);
				act("$n ordina qualcosa a $N.", FALSE, ch, 0, vict, TO_ROOM);
				
				if (!MANOVALE_OF(manovale,ch)) {
					send_to_char("Non sei il suo padrone.\n\r",ch);
					return TRUE; 
				}
				
				one_argument(message,buf3);
				
				if (COMANDI_MANOVALE) {
					send_to_char(OK, ch);
					command_interpreter(vict, message);
					return TRUE;
				} else {
					send_to_char("Non puoi!\r\n", ch);
					return TRUE;
				}
				return FALSE;
			} else if ((is_abbrev(name, "all"))||(is_abbrev(name, "tutti"))) {			// This is order "all" 
			sprintf(buf3, "$n da' l'ordine '%s'.", message);
				act(buf3, FALSE, ch, 0, vict, TO_ROOM);
				
				found = FALSE;
				for (k = world[ch->in_room].people; k; k = k->next_in_room) {
					
					if (MANOVALE_OF(k,ch)){
						found = TRUE;
						one_argument(message,buf3);
						
						if (COMANDI_MANOVALE) {
							send_to_char(OK, ch);
							command_interpreter(k, message);								
						} else 
							send_to_char("Non puoi!\r\n", ch);							
					}
				}
				if (!found)
					send_to_char("Non sei il loro padrone.\r\n",ch);
				return TRUE;					
			}
		}
		return FALSE;
	}*/
	return FALSE;
}


/*
#define PREZZO_MANOVALE		40000
#define STIPENDIO_MANOVALE	4000
#define VNUM_MANOVALE		9743

SPECIAL(vendor_manovale) {
	char str[MAX_STRING_LENGTH],str2[MAX_STRING_LENGTH];
	int rnum;
	struct char_data *manovale;
	struct char_data *vendor = (struct char_data *) me;
	
	if (CMD_IS("lista")) {
		send_to_char("Questa e' la disponibilita':\r\n", ch);
		sprintf(buf,"Manovale       -    &1%d&0\r\n",PREZZO_MANOVALE);
		send_to_char(buf, ch);
		return (TRUE);
	} else if (CMD_IS("buy") || CMD_IS("compra")) {
		
		argument = one_argument(argument, str);
		argument = one_argument(argument, str2);
		
		if (!is_abbrev(str,"manovale")) {
			send_to_char("Quello non e' disponibile!\r\n", ch);
			return TRUE;
		}
		if (GET_GOLD(ch) < PREZZO_MANOVALE) {
			send_to_char("Non hai abbastanza soldi per quello!\r\n", ch);
			return (TRUE);
		}
		if (limite_follower(ch)) {
			send_to_char("Non hai abbastanza carisma per assumerlo!\r\n", ch);
			return (TRUE);
		}
		
		rnum = real_mobile(VNUM_MANOVALE);
		manovale = read_mobile(rnum, REAL);
		
		if (*str2) {
			
			if(strlen(manovale->player.name)+1+strlen(str2)>MAX_NAME_LENGTH-1) {
				char_to_room(manovale, ch->in_room);
				extract_char(manovale);
				send_to_char("Nome troppo lungo!\r\n", ch);
				return (TRUE);	
			}
*/			
			/*strncpy(buf,manovale->player.name,MAX_NAME_LENGTH);
			strcat(buf, " ");
			strcat(buf, str2);
			manovale->player.name = strdup(buf);*/
/*			
			strcpy(buf, manovale->player.short_descr);
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			manovale->player.short_descr = strdup(buf);
			
			
			strcpy(buf, manovale->player.long_descr);
			buf[strlen(buf)-2] = '\0';
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			strcat(buf, ".\r\n");
			manovale->player.long_descr = strdup(buf);
			
			strcpy(buf, manovale->player.description);
			sprintf(buf, "%sGuardandolo riconosci che e' &2%s&0.\r\n\r\n",
				buf, str2);
			manovale->player.description = strdup(buf);
		}
		
		GET_STIPENDIOMOB(manovale) = STIPENDIO_MANOVALE;
		GET_PAGA(manovale) = ORE_PAGA;
		SET_BIT(MOB_FLAGS(manovale),MOB_STIPENDIO);
		MASTER_ID(manovale) = GET_IDNUM(ch);
		CLAN_ID(manovale)=GET_CLAN(ch);
		GET_HOME(manovale) = IN_ROOM(ch);
		
		char_to_room(manovale, ch->in_room);
		do_say(vendor,"Ecco il tuo nuovo manovale!",0,0);
		GET_GOLD(ch) -= PREZZO_MANOVALE;
		act("$n assume $N.", FALSE, ch, 0,manovale, TO_ROOM);
		save_mob(manovale, manovale->in_room);
		return TRUE;
		
	}
	return FALSE;
} // All commands except list and buy
*/


#define PREZZO_MANOVALE_PIGRO		10000
#define PREZZO_MANOVALE_ALLENATO    20000
#define PREZZO_MANOVALE_ESPERTO     30000
#define PREZZO_MANOVALE_CAMPIONE    40000
#define VNUM_MANOVALE_PIGRO			1601
#define VNUM_MANOVALE_ALLENATO		1602
#define VNUM_MANOVALE_ESPERTO		1603
#define VNUM_MANOVALE_CAMPIONE		1604


SPECIAL(vendor_manovale) {
	char str[MAX_STRING_LENGTH],str2[MAX_STRING_LENGTH];
	int rnum;
	struct char_data *manovale;
	struct char_data *vendor = (struct char_data *) me;
	
	if (CMD_IS("lista")) {
		send_to_char("Questa e' la disponibilita':\r\n", ch);
		sprintf(buf,"Manovale pigro      -    &1%d&0\r\n",PREZZO_MANOVALE_PIGRO);
		send_to_char(buf, ch);
		sprintf(buf,"Manovale allenato      -    &1%d&0\r\n",PREZZO_MANOVALE_ALLENATO);
		send_to_char(buf, ch);
		sprintf(buf,"Manovale esperto      -    &1%d&0\r\n",PREZZO_MANOVALE_ESPERTO);
		send_to_char(buf, ch);
		sprintf(buf,"Manovale campione      -    &1%d&0\r\n",PREZZO_MANOVALE_CAMPIONE);
		send_to_char(buf, ch);
		return (TRUE);
	} else if (CMD_IS("buy") || CMD_IS("compra")) {
		
		argument = one_argument(argument, str);
		argument = one_argument(argument, str2);
		
		if (!is_abbrev(str,"pigro") && !is_abbrev(str,"allenato") && !is_abbrev(str,"esperto") && !is_abbrev(str,"campione")) {
			send_to_char("Quello non e' disponibile!\r\n", ch);
			return TRUE;
		}
	if(is_abbrev(str,"pigro")){
			if (GET_GOLD(ch) < PREZZO_MANOVALE_PIGRO) {
			send_to_char("Non hai abbastanza soldi per quello!\r\n", ch);
			return (TRUE);
		}
		if (limite_follower(ch)) {
			send_to_char("Non hai abbastanza carisma per assumerlo!\r\n", ch);
			return (TRUE);
		}
		rnum = real_mobile(VNUM_MANOVALE_PIGRO);
		manovale = read_mobile(rnum, REAL);
		
		if (*str2) {
			
			if(strlen(manovale->player.name)+1+strlen(str2)>MAX_NAME_LENGTH-1+20) {
				char_to_room(manovale, ch->in_room);
				extract_char(manovale);
				send_to_char("Nome troppo lungo!\r\n", ch);
				return (TRUE);	
			}
			
			/*strncpy(buf,manovale->player.name,MAX_NAME_LENGTH);
			strcat(buf, " ");
			strcat(buf, str2);
			manovale->player.name = strdup(buf);*/
			
			strcpy(buf, manovale->player.short_descr);
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			manovale->player.short_descr = strdup(buf);
			
			
			strcpy(buf, manovale->player.long_descr);
			buf[strlen(buf)-2] = '\0';
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			strcat(buf, ".\r\n");
			manovale->player.long_descr = strdup(buf);
			
			strcpy(buf, manovale->player.description);
			sprintf(buf, "%sGuardandolo riconosci che e' &2%s&0.\r\n\r\n",
				buf, str2);
			manovale->player.description = strdup(buf);
		}
		
		//MASTER_ID(manovale) = GET_IDNUM(ch);
		//CLAN_ID(manovale)=GET_CLAN(ch);
		//GET_HOME(manovale) = IN_ROOM(ch);
		char_to_room(manovale, ch->in_room);
		do_say(vendor,"Ecco il tuo nuovo manovale!",0,0);
		GET_GOLD(ch) -= PREZZO_MANOVALE_PIGRO;
		act("$n assume $N.", FALSE, ch, 0,manovale, TO_ROOM);
		add_follower(manovale, ch);
		save_mob(manovale, manovale->in_room);
		return TRUE;
			
	}
	if(is_abbrev(str,"allenato")){
			if (GET_GOLD(ch) < PREZZO_MANOVALE_ALLENATO) {
			send_to_char("Non hai abbastanza soldi per quello!\r\n", ch);
			return (TRUE);
		}
		if (limite_follower(ch)) {
			send_to_char("Non hai abbastanza carisma per assumerlo!\r\n", ch);
			return (TRUE);
		}
		rnum = real_mobile(VNUM_MANOVALE_ALLENATO);
		manovale = read_mobile(rnum, REAL);
		
		if (*str2) {
			
			if(strlen(manovale->player.name)+1+strlen(str2)>MAX_NAME_LENGTH-1+20) {
				char_to_room(manovale, ch->in_room);
				extract_char(manovale);
				send_to_char("Nome troppo lungo!\r\n", ch);
				return (TRUE);	
			}
			
			/*strncpy(buf,manovale->player.name,MAX_NAME_LENGTH);
			strcat(buf, " ");
			strcat(buf, str2);
			manovale->player.name = strdup(buf);*/
			
			strcpy(buf, manovale->player.short_descr);
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			manovale->player.short_descr = strdup(buf);
			
			
			strcpy(buf, manovale->player.long_descr);
			buf[strlen(buf)-2] = '\0';
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			strcat(buf, ".\r\n");
			manovale->player.long_descr = strdup(buf);
			
			strcpy(buf, manovale->player.description);
			sprintf(buf, "%sGuardandolo riconosci che e' &2%s&0.\r\n\r\n",
				buf, str2);
			manovale->player.description = strdup(buf);
		}
		
		//MASTER_ID(manovale) = GET_IDNUM(ch);
		//CLAN_ID(manovale)=GET_CLAN(ch);
		//GET_HOME(manovale) = IN_ROOM(ch);
		char_to_room(manovale, ch->in_room);
		do_say(vendor,"Ecco il tuo nuovo manovale!",0,0);
		GET_GOLD(ch) -= PREZZO_MANOVALE_ALLENATO;
		act("$n assume $N.", FALSE, ch, 0,manovale, TO_ROOM);
		add_follower(manovale, ch);
		save_mob(manovale, manovale->in_room);
		return TRUE;
			
	}
	if(is_abbrev(str,"esperto")){
			if (GET_GOLD(ch) < PREZZO_MANOVALE_ESPERTO) {
			send_to_char("Non hai abbastanza soldi per quello!\r\n", ch);
			return (TRUE);
		}
		if (limite_follower(ch)) {
			send_to_char("Non hai abbastanza carisma per assumerlo!\r\n", ch);
			return (TRUE);
		}
		rnum = real_mobile(VNUM_MANOVALE_ESPERTO);
		manovale = read_mobile(rnum, REAL);
		
		if (*str2) {
			
			if(strlen(manovale->player.name)+1+strlen(str2)>MAX_NAME_LENGTH-1+20) {
				char_to_room(manovale, ch->in_room);
				extract_char(manovale);
				send_to_char("Nome troppo lungo!\r\n", ch);
				return (TRUE);	
			}
			
			/*strncpy(buf,manovale->player.name,MAX_NAME_LENGTH);
			strcat(buf, " ");
			strcat(buf, str2);
			manovale->player.name = strdup(buf);*/
			
			strcpy(buf, manovale->player.short_descr);
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			manovale->player.short_descr = strdup(buf);
			
			
			strcpy(buf, manovale->player.long_descr);
			buf[strlen(buf)-2] = '\0';
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			strcat(buf, ".\r\n");
			manovale->player.long_descr = strdup(buf);
			
			strcpy(buf, manovale->player.description);
			sprintf(buf, "%sGuardandolo riconosci che e' &2%s&0.\r\n\r\n",
				buf, str2);
			manovale->player.description = strdup(buf);
		}
		
		//MASTER_ID(manovale) = GET_IDNUM(ch);
		//CLAN_ID(manovale)=GET_CLAN(ch);
		//GET_HOME(manovale) = IN_ROOM(ch);
		char_to_room(manovale, ch->in_room);
		do_say(vendor,"Ecco il tuo nuovo manovale!",0,0);
		GET_GOLD(ch) -= PREZZO_MANOVALE_ESPERTO;
		act("$n assume $N.", FALSE, ch, 0,manovale, TO_ROOM);
		add_follower(manovale, ch);
		save_mob(manovale, manovale->in_room);
		return TRUE;
			
	}
	if(is_abbrev(str,"campione")){
			if (GET_GOLD(ch) < PREZZO_MANOVALE_CAMPIONE) {
			send_to_char("Non hai abbastanza soldi per quello!\r\n", ch);
			return (TRUE);
		}
		if (limite_follower(ch)) {
			send_to_char("Non hai abbastanza carisma per assumerlo!\r\n", ch);
			return (TRUE);
		}
		rnum = real_mobile(VNUM_MANOVALE_CAMPIONE);
		manovale = read_mobile(rnum, REAL);
		
		if (*str2) {
			
			if(strlen(manovale->player.name)+1+strlen(str2)>MAX_NAME_LENGTH-1+20) {
				char_to_room(manovale, ch->in_room);
				extract_char(manovale);
				send_to_char("Nome troppo lungo!\r\n", ch);
				return (TRUE);	
			}
			
			/*strncpy(buf,manovale->player.name,MAX_NAME_LENGTH);
			strcat(buf, " ");
			strcat(buf, str2);
			manovale->player.name = strdup(buf);*/
			
			strcpy(buf, manovale->player.short_descr);
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			manovale->player.short_descr = strdup(buf);
			
			
			strcpy(buf, manovale->player.long_descr);
			buf[strlen(buf)-2] = '\0';
			*str2=UPPER(*str2);
			strcat(buf, " di nome ");
			strcat(buf, str2);
			strcat(buf, ".\r\n");
			manovale->player.long_descr = strdup(buf);
			
			strcpy(buf, manovale->player.description);
			sprintf(buf, "%sGuardandolo riconosci che e' &2%s&0.\r\n\r\n",
				buf, str2);
			manovale->player.description = strdup(buf);
		}
		
		//MASTER_ID(manovale) = GET_IDNUM(ch);
		//CLAN_ID(manovale)=GET_CLAN(ch);
		//GET_HOME(manovale) = IN_ROOM(ch);
		char_to_room(manovale, ch->in_room);
		do_say(vendor,"Ecco il tuo nuovo manovale!",0,0);
		GET_GOLD(ch) -= PREZZO_MANOVALE_CAMPIONE;
		act("$n assume $N.", FALSE, ch, 0,manovale, TO_ROOM);
		add_follower(manovale, ch);
		save_mob(manovale, manovale->in_room);
		return TRUE;
		}	
	}
	return FALSE;
} /* All commands except list and buy */


SPECIAL(oste){
	
	int fame=0;
	int costo=0;
	int percentuale;

	if (CMD_IS("sfama")) {
		
		if(GET_COND(ch, FULL)>=96){
			send_to_char("Non hai proprio fame in questo momento.|r\n",ch);
			return FALSE;
		}
		fame=96-GET_COND(ch, FULL);
		//Da rivedere
		costo=fame*GET_LEVEL(ch)/2;
		
		if(GET_GOLD(ch)>=costo){
			GET_COND(ch,FULL)=96;
			GET_GOLD(ch)=GET_GOLD(ch)-costo;
			send_to_char("hai mangiato fino ad essere sazio.\r\n",ch);
			sprintf(buf,"Ti viene presentato un conto di &1%d&0\r\n",costo);
			send_to_char(buf,ch);
		}
		else {
			percentuale=(fame*GET_GOLD(ch))/costo;
			GET_COND(ch,FULL)=GET_COND(ch,FULL)+percentuale;
			costo=GET_GOLD(ch);
			GET_GOLD(ch)=0;
			send_to_char("Hai mangiato fino a che le tue finanze te lo hanno permesso.\r\n",ch);
			sprintf(buf,"Ti viene presentato un conto di &1%d&0\r\n",costo);
			send_to_char(buf,ch);
		}
	return TRUE;
	}
	else 
		return FALSE;
} 

/* Orione Inizio */
SPECIAL(attack_questmob) {
    char name[MAX_STRING_LENGTH], objname[MAX_STRING_LENGTH];
    char bufsay[MAX_STRING_LENGTH];
    struct char_data *victim;
    struct char_data *k = NULL;
    struct follow_type *f;
    int target_questmob_group = 0;
    struct obj_data *obj;
			        
    if (!(GET_LEVEL(ch)<LVL_IMMORT))
        return FALSE;
				        
    argument = one_argument(argument, name);
					    
    if ((CMD_IS("assassina") || CMD_IS("murder") || CMD_IS("backstab") || CMD_IS("pugnala") || \
	CMD_IS("bash") || CMD_IS("urta") || CMD_IS("hit") || CMD_IS("colpisci") || \
        CMD_IS("kick") || CMD_IS("calcio") || CMD_IS("kill") || CMD_IS("uccidi") || \
	CMD_IS("punch") || CMD_IS("pugno") || CMD_IS("deathblow")) && ((victim = get_char_room_vis(ch, name)) == me)) {

        //se il mob non sta combattendo, verifico che ch sia il pg cui e' destinato
        if (FIGHTING(victim)==NULL) {
	    if (IS_NPC(ch)) {
	        sprintf(bufsay, "Non sembra essere un incarico affidato a te, %s", GET_NAME(ch));
	        do_say(victim, bufsay, 0, 0);
	        return TRUE;
	    }
	
	    //ch e' sicuramente pg
	    if (!(IS_QUESTOR(ch))) {
	        sprintf(bufsay, "Il destino non ha riservato a te questo onore, %s!", GET_NAME(ch));
	        do_say(victim, bufsay, 0, 0);
	        return TRUE;
	    }
																							
	    //ch e' sicuramente QUESTOR
	    if ((GET_TIPOQUEST(ch)!=QUEST_MOBKILL) && (GET_TIPOQUEST(ch)!=QUEST_GETITEM)) {
	        do_say(victim, "Gli scopi della tua missione son ben piu' nobili di questo!", 0, 0);
	        return TRUE;
	    }
																													        
	    //ch ha sicuramente quest di tipo QUEST_MOBKILL o QUEST_GETITEM
	    if (GET_QUESTMRR(ch) != GET_MOB_VNUM(victim)) {
	        sprintf(bufsay, "Temo tu stia sbagliando persona, %s.", GET_NAME(ch));
	        do_say(victim, bufsay, 0, 0);
	        return TRUE;
	    }
																																					    
	    //ch ha assegnata una quest QUEST_MOBKILL o QUEST_GETITEM,
	    //il cui mob corrisponde alla vittima specificata
	    do_say(victim, "Il destino fa incrociare le nostre strade e le nostre armi!", 0, 0);
	    return FALSE;
																																								    
	} else if (ch && AFF_FLAGGED(ch, AFF_GROUP)) {
	    if (ch->master) k = ch->master;
	    else k = ch;
																																												    
	    //mob in combattimento, e' contro qualcuno del gruppo cui appartiene ch?
	    if (IS_QUESTOR(k) && \
	        ((GET_TIPOQUEST(k)==QUEST_MOBKILL) || (GET_TIPOQUEST(k)==QUEST_GETITEM)) && \
	        (GET_QUESTMRR(k)==GET_MOB_VNUM(victim)))
	            target_questmob_group = TRUE;
	    if (!target_questmob_group) {
	        for (f = k->followers; f; f = f->next) {
	            if (IS_QUESTOR(f->follower) && \
		        ((GET_TIPOQUEST(f->follower)==QUEST_MOBKILL) || (GET_TIPOQUEST(f->follower)==QUEST_GETITEM)) && \
		        (GET_QUESTMRR(f->follower)==GET_MOB_VNUM(victim))) {
		            target_questmob_group = TRUE;
		            break;
		    }
		}
	    }
																																																																					
	    //mob in combattimento, ma contro qualcuno che non appartiene al gruppo di cui fa parte ch
	    if (!target_questmob_group) {
	        send_to_char("Non appartieni alla compagnia predestinata a farlo.\r\n",ch);
	        return TRUE;
	    }
	} else { //il mob non e' destinato a ch, e ch non e' gruppato
	    send_to_char("La questione non sembra riguardarti!\r\n", ch);
	    return TRUE;
	}
    }

    strcpy(objname, name);
    one_argument(argument, name);

    if ((CMD_IS("dai") || CMD_IS("give")) && ((victim = get_char_room_vis(ch, name)) == me)) {
        if (IS_NPC(ch)) {
	    sprintf(bufsay, "Non sembra essere un incarico affidato a te, %s", GET_NAME(ch));
	    do_say(victim, bufsay, 0, 0);
	    return TRUE;
	}

        //ch e' sicuramente pg
	if (!(IS_QUESTOR(ch))) {
	    do_say(victim, "Non credo nessuno ci abbia mai presentati!", 0, 0);
	    return TRUE;
	}

        //ch e' sicuramente QUESTOR
	if (GET_TIPOQUEST(ch)!=QUEST_GIVITEM) {
	    do_say(victim, "Gli scopi della tua missione son ben diversi da questo!", 0, 0);
	    return TRUE;
	}

        //ch ha sicuramente quest di tipo QUEST_GIVITEM
	if (GET_QUESTMRR(ch) != GET_MOB_VNUM(victim)) {
	    sprintf(bufsay, "Temo tu stia sbagliando persona, %s.", GET_NAME(ch));
	    do_say(victim, bufsay, 0, 0);
	    return TRUE;
	}

        //ch ha assegnata una quest QUEST_GIVITEM
	//il cui mob corrisponde alla vittima specificata
	if (!(obj = get_obj_in_list_vis(ch, objname, ch->carrying)))
	    return FALSE;
	if (GET_QUESTOBJ(ch)!=GET_OBJ_VNUM(obj)) {
	    do_say(victim, "Non e' questo l'oggetto che attendevo!", 0, 0);
	    return TRUE;
	}
				
	do_say(victim, "Bene, hai superato la parte piu' difficile del tuo incarico!", 0, 0);
	GET_QUESTMRR(ch) = -4;
    }
    return FALSE;
}
/* Orione Fine */


#define PREZZO_ARMATA_FANTERIA     1000000
#define PREZZO_ARMATA_CAVALLERIA   2000000
#define VNUM_FANTERIA_SENDARIA     6780
#define VNUM_FANTERIA_ELENIA       21497
#define VNUM_FANTERIA_TOLNEDRA     9830
#define VNUM_FANTERIA_ALDARIA      12600
#define VNUM_CAVALLERIA_SENDARIA   6698
#define VNUM_CAVALLERIA_ELENIA     21499
#define VNUM_CAVALLERIA_TOLNEDRA   9831
#define VNUM_CAVALLERIA_ALDARIA    12601


SPECIAL(vendor_armate) {
	char str[MAX_STRING_LENGTH],str2[MAX_STRING_LENGTH];
	int rnum;
	int type=0,vnum_armata=0;
	struct char_data *armata;
	struct char_data *vendor = (struct char_data *) me;
	struct char_data *vict;
	
	if (CMD_IS("assegna")){
		
	two_arguments(argument,str,str2);
		if(!*str2 || !*str){
			send_to_char("Assegna <tipo unita'><nome pg>\r\n",ch);
			return(TRUE);
		}

		if(is_abbrev(str,"fanteria")){
			if (GET_GOLD(ch) < PREZZO_ARMATA_FANTERIA) {
				send_to_char("Non hai abbastanza soldi per reclutare un armata di fanti!\r\n", ch);
				return (TRUE);
			}
			type=1;
		}
		else if(is_abbrev(str,"cavalleria")){
				if (GET_GOLD(ch) < PREZZO_ARMATA_CAVALLERIA) {
					send_to_char("Non hai abbastanza soldi per reclutare un armata di cavallieri!\r\n", ch);
					return (TRUE);
				}
				type=2;
			}

		else{
				send_to_char("Abbiamo disponibili solo unita' di fanteria e cavalleria.\r\n",ch);
				return (TRUE);
		}
		if(!(vict=get_char_vis(vendor,str2))){
			send_to_char(NOPERSON,ch);
			return TRUE;
		}

		if(type==1){
				if(GET_CLAN(ch)==0)
						vnum_armata=VNUM_FANTERIA_ELENIA;
				if(GET_CLAN(ch)==2)
						vnum_armata=VNUM_FANTERIA_ALDARIA;
				if(GET_CLAN(ch)==1)
						vnum_armata=VNUM_FANTERIA_TOLNEDRA;
				if(GET_CLAN(ch)==3)
						vnum_armata=VNUM_FANTERIA_SENDARIA;	
		}
		if(type==2){
				if(GET_CLAN(ch)==0)
						vnum_armata=VNUM_CAVALLERIA_ELENIA;
				if(GET_CLAN(ch)==2)
						vnum_armata=VNUM_CAVALLERIA_ALDARIA;
				if(GET_CLAN(ch)==1)
						vnum_armata=VNUM_CAVALLERIA_TOLNEDRA;
				if(GET_CLAN(ch)==3)
						vnum_armata=VNUM_CAVALLERIA_SENDARIA;	
		}
		rnum = real_mobile(vnum_armata);
		armata = read_mobile(rnum, REAL);
		GET_UNITA_ARMATE(armata)=number(20,40);
		//GET_STIPENDIOMOB(armata) = STIPENDIO_ARMATA;
		//GET_PAGA(armata) = ORE_PAGA;
		//SET_BIT(MOB_FLAGS(armata),MOB_STIPENDIO);
		MASTER_ID(armata) = GET_IDNUM(vict);
		CLAN_ID(armata)=GET_CLAN(ch);
		
		char_to_room(armata, vict->in_room);
		do_say(vendor,"Speriamo non sia pericoloso dargli in mano un armata!",0,0);
		if(type==1)
		GET_GOLD(ch) -= PREZZO_ARMATA_FANTERIA;
		if(type==2)
		GET_GOLD(ch) -= PREZZO_ARMATA_CAVALLERIA;
		act("$n e' stata assegnata a $N.", FALSE, armata, 0,vict, TO_ROOM);
		save_mob(armata, armata->in_room);
		return TRUE;
		
	}
	return FALSE;
}
