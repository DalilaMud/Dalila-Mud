/* ************************************************************************
*   File: act.informative.c       35                    Part of CircleMUD *
*  Usage: Player-level commands of an informative nature                  *
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
#include "screen.h"
#include "clan.h"
#include "wilderness.h"
#include "olc.h"
#include "dg_scripts.h"	//Orione
#include "eserciti.h"

/* extern functions */
int update_max_usage(void);   // in comm.c
int parse_dir_char(char c);   // in act.movement.c
int parse_dir(char *st);      // in act.movement.c
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier); // in act.other.c
extern char * string(char *s1,char *buf,bool flag);
extern int get_stipendiomob(struct char_data *ch);
extern int _parse_name(char *arg, char *name);
extern int reserved_word(char *argument);

extern char *frasi_move_status[]; // in comm.c
extern char *frasi_lunghe_move_status[]; // in comm.c
extern struct index_data *mob_index;
extern int newbye(struct char_data *ch);
int get_move_status(struct char_data * ch); // in comm.c
extern int apply_ac (struct char_data *ch, int eq_pos);
extern void mostra_esercito(struct char_data *ch, struct char_data *esercito);

SPECIAL(manovale);
/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
//extern const struct command_info cmd_info[];
extern struct player_index_element *player_table;
extern int top_of_p_table;
extern char *BANNER_1;
extern char *credits;
extern char *news;
extern char *info;
extern char *motd;
extern char *imotd;
extern char *wizlist;
extern char *immlist;
extern char *policies;
extern char *handbook;
extern char *regni;
extern char *pray;
extern const char *dirs[];
extern const char *dirs1[];
extern char *where[];
extern char *color_liquid[];
extern char *fullness[];
extern char *connected_types[];
extern char *class_abbrevs[];
extern char *room_bits[];
extern char *spells[];
extern char *obj_quality[];
extern struct dex_app_type dex_app[];
extern struct dex_skill_type dex_app_skill[];
extern struct spell_info_type spell_info[];
extern int real_zone(room_vnum number);
extern struct str_app_type str_app[];
void do_list_groups(struct char_data *ch);
long find_class_bitvector(char arg);
int list_scanned_chars(struct char_data * list, struct char_data * ch, int distance, int door, bool nobody);
void peep_at_char(struct char_data * i, struct char_data * ch);
void peep_at_target(struct char_data * ch, char *arg);
char * frase_regno(int id,int type);
int real_pos[NUM_WEARS]=
{WEAR_LIGHT  ,WEAR_HEAD   ,WEAR_EYE    ,WEAR_LOBSX  ,WEAR_LOBDX  ,WEAR_BOCCA,
	WEAR_NECK_1  ,WEAR_NECK_2 ,WEAR_BODY   ,WEAR_SPALLE ,WEAR_ARMS  ,WEAR_HANDS,
	WEAR_FINGER_R,WEAR_FINGER_L,WEAR_SHIELD,WEAR_ABOUT ,WEAR_WAIST,WEAR_WRIST_R,
	WEAR_WRIST_L ,WEAR_WIELD,  WEAR_WIELD_L,WEAR_HOLD  ,WEAR_IMMOBIL,WEAR_HANG,
	WEAR_LEGS,WEAR_FEET,WEAR_ALTRO1 ,WEAR_VESTE, WEAR_RELIQUIA };
char output_eroi_m[MAX_STRING_LENGTH];
char output_eroi_k[MAX_STRING_LENGTH];
char output_eroi_c[MAX_STRING_LENGTH];
char output_eroi_n[MAX_STRING_LENGTH];
char output_eroi_d[MAX_STRING_LENGTH];
char output_eroi_o[MAX_STRING_LENGTH];

void meter_bar(struct char_data * ch, int meter_flag) {
	
	int i, i_percent, max = 0, current = 0, chars = 0;
	float percent = 0;
	char *meter_name [] = {
		"HP ",
		"Mana",
		"Mov"};
	
	*buf2 = '\0';
	
	switch (meter_flag) {
		
		case 0:
			percent = ((float)GET_HIT(ch)) / ((float)GET_MAX_HIT(ch));
			current=GET_HIT(ch);
			max=GET_MAX_HIT(ch);
			break;
			
		case 1:
			percent = ((float)GET_MANA(ch)) / ((float)GET_MAX_MANA(ch));
			current=GET_MANA(ch);
			max=GET_MAX_MANA(ch);
			break;
			
		case 2:
			percent = ((float)GET_MOVE(ch)) / ((float)GET_MAX_MOVE(ch));
			current=GET_MOVE(ch);
			max=GET_MAX_MOVE(ch);
			break;
			
		default:
			break;
	}
	
	i_percent = ((float)percent) * 100.0;
	chars = 50 * percent;
	
	if (chars > 50)
		chars = 50;
	
	for(i=0; i < chars; ++i)
		*(buf2 + i) = '#';
	*(buf2 + i) = '\0';
	
	for(; i < 50; ++i)
		*(buf2 + i) = '-';
	*(buf2 + i) = '\0';
	
	sprintf(buf, "[%s%-50s%s] %s %s%d%s/%s(%d)%s\r\n",CCRED(ch, C_NRM),
		buf2, CCNRM(ch, C_NRM),meter_name[meter_flag],
		CCYEL(ch, C_NRM), current, CCNRM(ch, C_NRM), CCRED(ch, C_NRM),
		max, CCNRM(ch, C_NRM));
	
	send_to_char(buf, ch);
}

void show_obj_to_char(struct obj_data * object, struct char_data * ch,
	int mode)
{
	bool found;
	int condition=0;
	
	*buf = '\0';
	if ((mode == 0) && object->description)
		strcpy(buf, object->description);
	else if (object->short_description && ((mode == 1) || (mode == 2) || (mode == 3) || (mode == 4)))
		strcpy(buf, object->short_description);
	else if (mode == 5)
	{
		if (GET_OBJ_TYPE(object) == ITEM_NOTE)
		{
			if (object->action_description)
			{
				strcpy(buf, "C'e' scritto qualche cosa su :\r\n\r\n");
				strcat(buf, object->action_description);
				page_string(ch->desc, buf, 1);
			}
			else
				act("E' vuoto.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
		else if (GET_OBJ_TYPE(object) != ITEM_DRINKCON)
		{
			strcpy(buf, "Non vedi niente di interessante...");
		}
		else      /* ITEM_TYPE == ITEM_DRINKCON||FOUNTAIN */
			strcpy(buf, "Sembra un contenitore di liquidi.");
	}
	
	if (mode != 3)
	{
		found = FALSE;
		if (IS_OBJ_STAT(object, ITEM_INVISIBLE))
		{
			strcat(buf, " (&7&binvisibile&0)");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, ITEM_BLESS) && IS_AFFECTED(ch, AFF_DETECT_ALIGN))
		{
			strcat(buf, " (&4&baura blu&0)");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, ITEM_MAGIC) && IS_AFFECTED(ch, AFF_DETECT_MAGIC))
		{
			strcat(buf, " (&3&baura gialla&0)");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, ITEM_GLOW))
		{
			strcat(buf, " (&6&baura chiara&0)");
			found = TRUE;
		}
		if (IS_OBJ_STAT(object, ITEM_HUM))
		{
			strcat(buf, " (&8&bemette un ronzio&0)");
			found = TRUE;
		}
	}
	
	if (mode == 1)
		if ((GET_OBJ_TYPE(object) != ITEM_FOOD) &&(GET_OBJ_TYPE(object) != ITEM_DRINKCON) && (GET_OBJ_TYPE(object) != ITEM_SCROLL) && (GET_OBJ_TYPE(object) != ITEM_TREASURE)&&(GET_OBJ_TYPE(object) != ITEM_POTION))
		{
			if (((GET_OBJ_CSLOTS(object) > 0 && GET_OBJ_CSLOTS(object) < 101)) && ((GET_OBJ_TSLOTS(object) > 0 && GET_OBJ_TSLOTS(object) <101)))
			{
				condition = ((GET_OBJ_CSLOTS(object) * 100) / GET_OBJ_TSLOTS(object));
				condition = condition / 10;
				condition = MAX(0, MIN(9, condition));
				sprinttype(condition, obj_quality, buf2);
			}
			else strcpy(buf2, " ERRORE: CSLOT pari a zero, per favore contatta un creatore.");
			strcat(buf, buf2);
		}
		
	strcat(buf, "\r\n");
	page_string(ch->desc, buf, 1);
}

void list_obj_to_char(struct obj_data *list, struct char_data *ch, int mode,
	bool show)
{
  struct obj_data *i, *j;
  char buf[10];
  bool found;
  int num;
	
  found = FALSE;

  for (i = list; i; i = i->next_content) {
	send_to_char(CCGRN(ch, C_NRM), ch);
	num = 0;
	
	for (j = list; j != i; j = j->next_content) {
	  if (j->item_number == NOTHING || IS_OBJ_STAT(j, ITEM_RESTRING) || IS_OBJ_STAT(i, ITEM_RESTRING)) {
		if (j->short_description && i->short_description) {
	      if (!strcmp(j->short_description, i->short_description)) 
	        break;
	    }
	    else {
	      sprintf(buf, "SYSERR: list_obj_to_char NULL obj");
          mudlog(buf, BRF, LVL_IMPL, TRUE);
        }
	  }
	  else if (j->item_number == i->item_number) 
	    break;
	}
		
	if (j!=i) 
	  continue;
	
	for (j = i; j; j = j->next_content) {
	  if (j->item_number==NOTHING || IS_OBJ_STAT(j, ITEM_RESTRING) || IS_OBJ_STAT(i, ITEM_RESTRING)) {
	    if (j->short_description && i->short_description) {
	      if (!strcmp(j->short_description, i->short_description)) 
	        num++;
	    }
	    else {
	      sprintf(buf, "SYSERR: list_obj_to_char NULL obj P2");
          mudlog(buf, BRF, LVL_IMPL, TRUE);
        }
	  }
	  else if (j->item_number == i->item_number) 
	    num++;
	}
			
	if (CAN_SEE_OBJ(ch, i)) {
	  if (!(newbye(ch) && (GET_OBJ_TYPE(i) == ITEM_TRAPPOLA))) {				
		if (num != 1) {
		  sprintf(buf,"(%2i) ", num);
		  send_to_char(buf,ch);
		}
		show_obj_to_char(i, ch, mode);
		found = TRUE;
	  }
	}
  }
  
  if (!found && show)
	send_to_char(" Niente.\r\n", ch);
}



void diag_char_to_char(struct char_data * i, struct char_data * ch)
{
	int percent;
	int tempsex;
	
	if (GET_MAX_HIT(i) > 0)
		percent = (100 * GET_HIT(i)) / GET_MAX_HIT(i);
	else
		percent = -1;		/* How could MAX_HIT be < 1?? */
	
	if (AFF_FLAGGED(i, AFF_DISGUISE) && (i->player.namedisguise))
		tempsex = i->player.sexdisguise;
	else
		tempsex = GET_SEX(i);
	
	strcpy(buf, PERS(i, ch));
	CAP(buf);
	
	if (MOB_FLAGGED(i,MOB_MOUNTABLE)) {
		
		if (GET_COND(i, FULL)>=96)
			strcat(buf, " e' &2completamente sazio&0 ed");
		else if (GET_COND(i, FULL)>=75)
			strcat(buf, " e' &2sazio&0 ed");
		else if (GET_COND(i, FULL)>=50)
			strcat(buf, "");
		else if (GET_COND(i, FULL)>=25)
			strcat(buf, " &3ha fame&0 ed");
		else if (GET_COND(i, FULL)>=0)
			strcat(buf, " sta' &5morendo di fame&0 ed e'");		
		else 
			strcat(buf, " e' &2completamente sazio&0 ed");
	}
	
	if (percent >= 100)
		strcat(buf, " in &2Eccellenti condizioni&0.\r\n");
	else if (percent >= 90)
		strcat(buf, " &2Ha qualche graffio.\r\n");
	else if (percent >= 75)
		strcat(buf, " ha &3Qualche taglio e sbucciatura&0.\r\n");
	else if (percent >= 50)
		strcat(buf, " ha &3Diverse ferite.&0\r\n");
	else if (percent >= 30)
		strcat(buf, " ha &5Squarci aperti e numerose ferite&0.\r\n");
	else if (percent >= 15){
		if (tempsex==SEX_MALE)
			strcat(buf, " sembra &5Veramente messo male&0.\r\n");
		else
			strcat(buf, " sembra &5Veramente messa male&0.\r\n");
	}
	else if (percent >= 0)
		strcat(buf, " e' in &1Pessime condizioni&0.\r\n");
	else
		strcat(buf, " &1Sanguina abbondantemente dalle ferite&0.\r\n");
	
	send_to_char(buf, ch);
	
	if (AFF_FLAGGED(i, AFF_PARALIZE)){
		if (tempsex==SEX_FEMALE)
			send_to_char("E' paralizzata!",ch);
		else send_to_char("E' paralizzato!", ch);
	}
	if(AFF_FLAGGED(i,AFF_TRAMORTITO)){
		if (tempsex==SEX_FEMALE)
			send_to_char("Sembra  &4Tramortita!&0",ch);
		else
			send_to_char("Sembra &4Tramortito!&0", ch);
	}
	if(AFF_FLAGGED(i,AFF_IMMOBIL)){
		if (tempsex==SEX_FEMALE)
			send_to_char("E' &5&bImmobilizzata a Terra&0. ",ch);
		else
			send_to_char("E' &5&bImmobilizzato a Terra&0.", ch);
	}
	
	if (AFF_FLAGGED(i, AFF_DISGUISE) && (i->player.namedisguise) && (number(1, 100) < 2 + GET_INT(ch)))
		if (can_do_event(ch, LAST_LOOKCAMUF, 30))
			send_to_char("&5&bSEMBRA ESSERE UN CAMUFFAMENTO!&0.", ch); 
		
}


void look_at_char(struct char_data * i, struct char_data * ch)
{
	int j,k, found;
	struct obj_data *tmp_obj;
	
	if (!ch->desc)
		return;
	//PEPPEDISGUISE
	if(AFF_FLAGGED(i,AFF_DISGUISE) && (i->player.long_descr_disguise)){
	send_to_char(i->player.long_descr_disguise,ch);
	}
	else{ 
	    if(i->player.description)
		send_to_char(i->player.description, ch);
	    else 
		act("Non vedi niente di speciale in $n.", FALSE, i, 0, ch, TO_VICT);
	}
	
	/* Ardanos
	 * Se il mob e' un esercito, mostro le informazioni e poi esco 
	 * senza far vedere l'eventuale equip */
	 
	if(IS_NPC(i)) {
		if(IS_ESERCITO(i)) {
			mostra_esercito(ch, i);
			return;
		}
	}
	
	diag_char_to_char(i, ch);
	
	
	if (TRANSPORTED(i) && TRANSPORTED(i)->in_room == i->in_room)
	{
		if (TRANSPORTED(i) == ch) act("$n ti sta trasportando.", FALSE, i, 0, ch, TO_VICT);
		else
		{
			sprintf(buf2, "$n sta trasportando %s.", PERS(TRANSPORTED(i), ch));
			act(buf2, FALSE, i, 0, ch, TO_VICT);
		}
	}  
	else 
	if (TRANSPORTED_BY(i) && TRANSPORTED_BY(i)->in_room == i->in_room)
	{
		if (TRANSPORTED_BY(i) == ch)
		{
			act("$n trasportato da te.", FALSE, i, 0, ch, TO_VICT);
		}
		else
		{
			sprintf(buf2, "$n e' trasportato da %s.", PERS(TRANSPORTED_BY(i), ch));
			act(buf2, FALSE, i, 0, ch, TO_VICT);
		}
	}
	
	
	if (RIDING(i) && RIDING(i)->in_room == i->in_room)
	{
		if (RIDING(i) == ch) act("$n ti sta cavalcando.", FALSE, i, 0, ch, TO_VICT);
		else
		{
			sprintf(buf2, "$n sta cavalcando %s.", PERS(RIDING(i), ch));
			act(buf2, FALSE, i, 0, ch, TO_VICT);
			sprintf(buf2, "%s %s.", PERS(RIDING(i), ch), frasi_lunghe_move_status[get_move_status(i)] );
			act(buf2, FALSE, i, 0, ch, TO_VICT);
		}
	}  
	else 
	if (RIDDEN_BY(i) && RIDDEN_BY(i)->in_room == i->in_room)
	{
		if (RIDDEN_BY(i) == ch)
		{
			act("Stai cavalcando $n.", FALSE, i, 0, ch, TO_VICT);
			sprintf(buf2, "$n %s.",frasi_move_status[get_move_status(i)]);
			act(buf2, FALSE, i, 0, ch, TO_VICT);
		}
		else
		{
			sprintf(buf2, "$n e' cavalcato da %s.", PERS(RIDDEN_BY(i), ch));
			act(buf2, FALSE, i, 0, ch, TO_VICT);
			sprintf(buf2, "$n %s.",
				frasi_lunghe_move_status[get_move_status(i)]);
			act(buf2, FALSE, i, 0, ch, TO_VICT);
		}
	}
	
	
/************************************************************************/
	
	//se il pg e' affetto da disguise esce altrimenti mostrerebbe l'equip del pg 
	if(AFF_FLAGGED(i,AFF_DISGUISE)) return;

	found = FALSE;
	for (j = 0; !found && j < NUM_WEARS; j++)
		if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
			found = TRUE;
		
	if (found) {
		act("\r\n&2$n&0 sta usando:\n", FALSE, i, 0, ch, TO_VICT);
		for (j = 0; j < NUM_WEARS; j++){
			k=real_pos[j];
			if (GET_EQ(i, k) && CAN_SEE_OBJ(ch, GET_EQ(i, k))) {
				send_to_char(where[k], ch);
				show_obj_to_char(GET_EQ(i, k), ch, 1);
			}
		}
	}
	
	if (((IS_MANOVALE(i)) && (MANOVALE_OF(i,ch))) || ((GET_LEVEL(ch)>=LVL_IMMORT)&&(GET_LEVEL(ch)>GET_LEVEL(i)))) {
		if ((IS_MANOVALE(i)) && (MANOVALE_OF(i,ch))) send_to_char("\r\nIl tuo manovale ha in inventario:\r\n",ch);
		else send_to_char("\r\nHa in inventario:\r\n",ch);		
		for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
			if (CAN_SEE_OBJ(ch, tmp_obj)) {
				show_obj_to_char(tmp_obj, ch, 1);
				found = TRUE;
			}
		}
		send_to_char("\r\n",ch);		
		sprintf(buf, "Sta portando un peso di %d Kg contro un massimo di %d Kg.\r\n",
			IS_CARRYING_W(i), CAN_CARRY_W(i));
		send_to_char(buf,ch);
		sprintf(buf, "Sta portando %d oggetti contro un massimo di %d oggetti.\r\n",
			IS_CARRYING_N(i), CAN_CARRY_N(i));
		send_to_char(buf,ch);
	}
	
	if (MOB_FLAGGED(i,MOB_STIPENDIO) && (MANOVALE_OF(i,ch))) {
		if (GET_STIPENDIOMOB(i)>0) {
			if (GET_PAGA(i) > 0) {
				if (GET_BANK_GOLD(i) >= get_stipendiomob(i))
					sprintf(buf, "Ha uno stipendio di &2%d&0 coins da pagare approsimativamente entro &2%ld&0 ore.\r\n",get_stipendiomob(i),(GET_BANK_GOLD(i) / get_stipendiomob(i)) * (ORE_PAGA)+GET_PAGA(i));
				else
					sprintf(buf, "Ha uno stipendio di &2%d&0 coins da pagare entro &2%ld&0 ore.\r\n",get_stipendiomob(i),GET_PAGA(i));
			} else if (GET_PAGA(i) == 0)
				sprintf(buf, "Ha uno stipendio di &2%d&0 coins da pagare &1adesso&0.\r\n",get_stipendiomob(i));
			else if (GET_PAGA(i) < 0)
				sprintf(buf, "Ha uno stipendio di &2%d&0 coins, sei in ritardo di &1%ld&0 ore\r\n",get_stipendiomob(i),-GET_PAGA(i));
			send_to_char(buf,ch);
			if ((GET_BANK_GOLD(i)%get_stipendiomob(i))>0) {
				sprintf(buf, "Hai gia' pagato in anticipo &1%d&0 coins.\r\n",GET_BANK_GOLD(i)%get_stipendiomob(i));
				send_to_char(buf,ch);
			}
		}
		
	}
}
void peep_at_char(struct char_data * i, struct char_data * ch)
{
	int found,gold=0;
	struct obj_data *tmp_obj;
	if (ch != i) {
		found = FALSE;
		act("Stai cercando di mettere le mani nella roba di $n: ", FALSE, i, 0, ch, TO_VICT);
		gold=GET_GOLD(i);
		if(gold==0)
			sprintf(buf,"Non ha un soldo!\n");
		else if(gold<=10)
			sprintf(buf,"Ha una manciata di monete nel borsello!\n");
		else if(gold<=50)
			sprintf(buf,"Ha poche monete nel borsello!\n");
		else if(gold<=100)
			sprintf(buf,"Ha diverse monete nel borsello!\n");
		else if(gold<=1000)
			sprintf(buf,"&1Ha molte monete ne borsello!&0\n");
		else if(gold<=5000)
			sprintf(buf,"&5Ha un bel gruzzolo nel borsello!&0\n");
		else if(gold<=10000)
			sprintf(buf,"&3E' ben fornito di grana!&0\n");
		else
			sprintf(buf,"&3&bLe sue tasche sono piene zeppe di soldi!&0\n");
		send_to_char(buf,ch);
		for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
			if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0, 20) < GET_LEVEL(ch))) {
				show_obj_to_char(tmp_obj, ch, 1);
				found = TRUE;
			}
		}
		
		if (!found)
			send_to_char("Non vedi niente.\r\n", ch);
	}
}
void peep_at_target(struct char_data * ch, char *arg)
{
	int bits;
	struct char_data *found_char = NULL;
	struct obj_data *found_obj = NULL;
	
	if (!ch->desc)
		return;
	
	
	if (!*arg) {
		send_to_char("Guardare che cosa?\r\n", ch);
		return;
	}
	bits = generic_find(arg, FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &found_char, &found_obj);
	
  /* Is the target a character? */
	if (found_char != NULL) {
		peep_at_char(found_char, ch);
		if (ch != found_char) {
			if (CAN_SEE(found_char, ch))
				act("$n ti guarda.", TRUE, ch, 0, found_char, TO_VICT);
			act("$n guarda $N.", TRUE, ch, 0, found_char, TO_NOTVICT);
		}
		return;
	}
}

void list_all_char(struct char_data * i, struct char_data * ch, int num)
{
	int tempsex;
	struct  obj_data *obj=NULL;
	char *positions[] = {
		" e' qui in terra, &1morto&0",
		" e' qui in terra, &1&bmortalmente colpito&0",
		" e' qui in terra, &5&bincapacitato&0",
		" e' qui in terra, &8senza sensi&0",
		" sta &6&bdormendo qui&0",
		" si sta &2riposando&0 qui&0",
		" e' qui &2seduto&0",
		" sta &4&bCOMBATTENDO!&0",
		" e' qui &2&bvigile e attento&0"
	};
	char *positiona[] = {
		" e' qui in terra, &1morta&0",
		" e' qui in terra, &1&bmortalmente colpita&0",
		" e' qui in terra, &5&bincapacitata&0",
		" e' qui in terra, &8senza sensi&0",
		" sta &6&bdormendo qui&0",
		" si sta &2riposando qui&0",
		" e' qui &2seduta&0",
		" sta &4&bCOMBATTENDO!&0",
		" e' qui &2&bvigile e attenta&0"
	};
	
	
	if (AFF_FLAGGED(i, AFF_DISGUISE) && (i->player.namedisguise))
		tempsex = i->player.sexdisguise;
	else
		tempsex = GET_SEX(i);
	
	if (IS_NPC(i) && i->player.long_descr && GET_POS(i) == GET_DEFAULT_POS(i)) {
		
		if (IS_AFFECTED(i, AFF_INVISIBLE))
			strcpy(buf, "*");
		else
			*buf = '\0';
		
		if (IS_AFFECTED(ch, AFF_DETECT_ALIGN)) {
			if (IS_EVIL(i)) 
			  strcat(buf, "&1(Aura rossa)&0 ");
			else if (IS_GOOD(i)) 
			  strcat(buf, "&4(Aura blue)&0 ");
		}
		
		strcat(buf, i->player.long_descr);
		
		if (num > 1) {
			while (   (buf[strlen(buf)-1]=='\r') 
			       ||	(buf[strlen(buf)-1]=='\n') 
			       || (buf[strlen(buf)-1]==' ')
			      )
			       { buf[strlen(buf)-1] = '\0'; }
			
			sprintf(buf2," [%d]\r\n", num);
			strcat(buf, buf2);
		}
		send_to_char(buf, ch);
		
    /*Enrico add:Block for AFF_IMMOBIL characters*/
		if (IS_AFFECTED(i, AFF_IMMOBIL)) {
			if (tempsex==SEX_FEMALE)
				act(" $n e' immobilizzata da qualcosa che ha ai polsi.", FALSE, i, 0, ch, TO_VICT);
			else
				act(" $n e' immobilizzato da qualcosa che ha ai polsi.", FALSE, i, 0, ch, TO_VICT);
		}
		
		if (IS_AFFECTED(i, AFF_SANCTUARY))
			act("...emana una luce brillante.", FALSE, i, 0, ch, TO_VICT);
		
		if (IS_AFFECTED(i, AFF_BLIND)) {
			if (tempsex==SEX_FEMALE)
				act("...e' completamente cieca!", FALSE, i, 0, ch, TO_VICT);
			else
				act("...e' completamente cieco!", FALSE, i, 0, ch, TO_VICT);
		}
		
		if (IS_AFFECTED(i, AFF_MIRRORIMAGE))
			act("...vedi tante immagini di $n.", FALSE, i, 0, ch, TO_VICT);
		
		if (IS_AFFECTED(i, AFF_PASSDOOR))
			act("...sembra trasparente!", FALSE, i, 0, ch, TO_VICT);
		
		if((obj=GET_EQ(i, WEAR_HANG))) {
			act("..sta trasportando $p.", FALSE, i, obj, ch, TO_VICT);
		}
		return;
	}
	
	if (IS_NPC(i)) {
		strcpy(buf, i->player.short_descr);
		CAP(buf);
	}
	else {
		if(   GET_CLAN(i) != 0
		   && GET_CLAN(ch) != 0
		   && ROOM_FLAGGED(ch->in_room, ROOM_WAR_ROOM)
		  )
			switch( clan[find_clan_by_id(GET_CLAN(ch))].diplomacy[find_clan_by_id(GET_CLAN(i))].status)
			{
				case WAR:       strcpy(buf2, "&1(G)&0 ");break;
				case ALLIANCE:  strcpy(buf2, "&5(A)&0 ");break;
				case PEACE:     strcpy(buf2, "&7(P)&0 ");break;
				default:        strcpy(buf2, "");        break;
			}
		else strcpy(buf2, "");
		
		if (AFF_FLAGGED(i, AFF_DISGUISE) && (i->player.namedisguise)) {
			sprintf(buf, "%s%s %s", buf2, i->player.namedisguise, GET_TITLE(i));
			CAP(buf);
	  }
		else
			sprintf(buf, "%s%s %s",buf2, i->player.name, GET_TITLE(i));
		strcat(buf, "&0");
	}
	
	if (IS_AFFECTED(i, AFF_INVISIBLE)) 
	  strcat(buf, " &7&b(invisibile)&3");
	if (IS_AFFECTED(i, AFF_HIDE))
		strcat(buf, (tempsex==SEX_FEMALE ? " &4&b(nascosta)&0" : " &4&b(nascosto)&0"));
	if (!IS_NPC(i) && !i->desc)
		strcat(buf, (tempsex==SEX_FEMALE ? " &1(scollegata)&0" : " &1(scollegato)&0"));
	if (PLR_FLAGGED(i, PLR_WRITING))
		strcat(buf, " &3&b(sta scrivendo)&0");
	if (PRF_FLAGGED(i,PRF_AFK))	
		strcat(buf," &3&b(AFK)&0");
	if (!PRF_FLAGGED(i, PRF_RPG) && !IS_NPC(i) && (!(AFF_FLAGGED(i, AFF_DISGUISE))))
		strcat(buf," &3&b(NOGDR)&0");
	if (PLR_FLAGGED(i, PLR_FANTASMA) && !IS_NPC(i))
		strcat(buf,"&8&7(FANTASMA)&0");	
	if ((GET_LEVEL(ch) >= LVL_QUEST_MASTER) && (PLR_FLAGGED(i, PLR_MULTIPLAYER)))
		strcat(buf," &8&1(MP)&0");
	if ((GET_LEVEL(ch) >= LVL_QUEST_MASTER) && (AFF_FLAGGED(i, AFF_DISGUISE)))
		strcat(buf," &8&1(Disguised)&0");	
  
  /*Lance*/
	if (GROUPNAME(i)) {
		sprintf(buf2, " <&5%s&0>", GROUPNAME(i));
		strcat(buf, buf2);
	}
	
	if (RIDING(i) && RIDING(i)->in_room == i->in_room) {  
		if (RIDING(i) == ch) 
		  strcat(buf, " e' qui, ti sta cavalcando");
		else {
			strcat(buf, " e' qui, sta cavalcando ");
			strcat(buf, PERS(RIDING(i), ch));
		}      
	}
	//se il pg non sta lottando e non e' flaggato da disguise allora...
	else if (GET_POS(i) != POS_FIGHTING) {
		if (!(AFF_FLAGGED(i, AFF_DISGUISE)))
		  strcat(buf, (tempsex==SEX_FEMALE ? positiona[(int) GET_POS(i)] : positions[(int) GET_POS(i)]));
	}
	else {
		if (FIGHTING(i)) {
			strcat(buf, " e' qui che &4&bcombatte&0 ");
			if (FIGHTING(i) == ch)
				strcat(buf, "&1Te!&0");
			else {
				if (i->in_room == FIGHTING(i)->in_room)
					strcat(buf, PERS(FIGHTING(i), ch));
				else
					strcat(buf, "con chi o con cosa, non si sa'!!!");
				strcat(buf, " ");
			}
		}
		else 		/* NIL fighting pointer */
			strcat(buf, " e' qui che combatte il suo nemico immagginario");
	}
	
	
	if (TRANSPORTED(i) && TRANSPORTED(i)->in_room == i->in_room) {  
		if (TRANSPORTED(i) == ch) 
		  strcat(buf, ", ti sta portando in spalla");
		else {
			strcat(buf, ", sta portando in spalla ");
			strcat(buf, PERS(TRANSPORTED(i), ch));
		}      
	}
	
	
	if (IS_AFFECTED(ch, AFF_DETECT_ALIGN)) {
		if (IS_EVIL(i))
			strcat(buf, " &1(Aura rossa)&0");
		else if (IS_GOOD(i))
			strcat(buf, " &4(Aura blue)&0");
		
	}
	
  strcat(buf, ".");
  strcat(buf, "\r\n");
	
	send_to_char(buf, ch);
	
	if (IS_AFFECTED(i, AFF_IMMOBIL)) {
		if (tempsex == SEX_FEMALE)
			act("...e' immobilizzata da qualcosa che ha ai polsi.", FALSE, i, 0, ch, TO_VICT);
		else
			act("...e' immobilizzato da qualcosa che ha ai polsi.", FALSE, i, 0, ch, TO_VICT);
	}
	if (IS_AFFECTED(i, AFF_SANCTUARY))
		act("...emana una luce brillante.", FALSE, i, 0, ch, TO_VICT);
	
	if (IS_AFFECTED(i, AFF_BLIND)) {
		if (tempsex==SEX_FEMALE)
			act("...e' completamente cieca!", FALSE, i, 0, ch, TO_VICT);
		else
			act("...e' completamente cieco!", FALSE, i, 0, ch, TO_VICT);
	}
	
	if(PLR_FLAGGED(i,PLR_CATARSI))
		        act("...sta meditando profondamente.", FALSE, i, 0, ch, TO_VICT);
	
	if (IS_AFFECTED(i, AFF_MIRRORIMAGE))
		act("...vedi tante immagini di questa persona.", FALSE, i, 0, ch, TO_VICT);
	if (IS_AFFECTED(i, AFF_PASSDOOR))
		act("...sembra trasparente!", FALSE, i, 0, ch, TO_VICT);
	if ((obj = GET_EQ(i, WEAR_HANG))) {
		act("..sta trasportando $p.", FALSE, i, obj, ch, TO_VICT);
		if (RIDING(i))
		if ((obj = GET_EQ(RIDING(i), WEAR_HANG)))
			act("$n sta trainando $p.", FALSE, RIDING(i), obj, ch, TO_VICT);
	}
}


void list_char_to_char(struct char_data *list, struct char_data *ch)
{
	struct char_data *i, *plr_list[100];
	int num, counter, locate_list[100], found=FALSE;
	
	num = 0;
	
	for (i=list; i; i = i->next_in_room) {
		send_to_char(CCYEL(ch, C_NRM), ch);
		if(i != ch)  {
			if (   (RIDDEN_BY(i) && (RIDDEN_BY(i)->in_room == i->in_room))
				  || TRANSPORTED_BY(i) 
				 )
				continue;
			
			if (CAN_SEE(ch, i)) {
				if (num < 100) {
					found = FALSE;
					for (counter=0;(counter < num && !found);counter++) {
						if (i->nr == plr_list[counter]->nr &&
							
/* for odd reasons, you may want to seperate same nr mobs by factors
other than position, the aff flags, and the fighters. (perhaps you want
to list switched chars differently.. or polymorphed chars?) */
							
							(GET_POS(i) == GET_POS(plr_list[counter])) &&
							(AFF_FLAGS(i)==AFF_FLAGS(plr_list[counter])) &&
							(FIGHTING(i) == FIGHTING(plr_list[counter])) &&
							!strcmp(GET_NAME(i), GET_NAME(plr_list[counter])))
						{
							locate_list[counter] += 1;
							found=TRUE;
						}
					}
					if (!found) {
						plr_list[num] = i;
						locate_list[num] = 1;
						num++;
					}
				} 
				else {
					list_all_char(i, ch, 0);
				}
			}
		}
	}
	if (num) {
		for (counter=0; counter<num; counter++) {
			if (locate_list[counter] > 1) {
				list_all_char(plr_list[counter],ch,locate_list[counter]);
			} else {
				list_all_char(plr_list[counter],ch,0);
			}
		}
	}
}




void do_auto_exits(struct char_data * ch)
{
	int door;
	
	*buf = '\0';
	
	for (door = 0; door < NUM_OF_DIRS; door++)
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
			!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
			sprintf(buf, "%s%c ", buf, LOWER(*dirs[door]));
		else
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
			!IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
			sprintf(buf, "%s^%c ", buf, LOWER(*dirs[door]));
	sprintf(buf2, "%s[ Uscite: %s]%s\r\n", CCCYN(ch, C_NRM),
		*buf ? buf : "Nessuna! ", CCNRM(ch, C_NRM));
	
	send_to_char(buf2, ch);
}

void do_auto_dir(struct char_data *ch)
{
	int door;
	
	*buf = '\0';
	
	if (IS_AFFECTED(ch, AFF_BLIND)) {
		if(GET_SEX(ch)==SEX_FEMALE)
			send_to_char("Non puoi vedere niente, sei cieca!\r\n", ch);
		else
			send_to_char("Non puoi vedere niente, sei cieco!\r\n", ch);
		return;
	}
	for (door = 0; door < NUM_OF_DIRS; door++) {
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
			!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
			if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
				sprintf(buf2, "%-5s - [%5d] %s\r\n", dirs[door],
					world[EXIT(ch, door)->to_room].number,
					world[EXIT(ch, door)->to_room].name);
			else {
				sprintf(buf2, "%-5s - ", dirs[door]);
				if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
					strcat(buf2, "Troppo buio per dirlo.\r\n");
				else{
					strcat(buf2, world[EXIT(ch, door)->to_room].name);
					strcat(buf2, "\r\n");
				}
			}if (!IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
				strcat(buf, CAP(buf2));
		} else if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
			IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
			if (GET_LEVEL(ch) >= LVL_QUEST_MASTER) {
				sprintf(buf2, "%-5s - [%5d] %s - CHIUSA", dirs[door],
					world[EXIT(ch, door)->to_room].number,
					world[EXIT(ch, door)->to_room].name);
				if (IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED))
					strcat(buf2, " - BLOCCATA");
				if (IS_SET(EXIT(ch, door)->exit_info, EX_PICKPROOF))
					strcat(buf2, " - RESISTENTE");
				if (IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
					strcat(buf2, " - NASCOSTA");
				strcat(buf2, "\r\n");
			}else
			{
				if (EXIT(ch, door)->keyword)
					sprintf(buf2, "%-5s - La %s e' chiusa\r\n", dirs[door],
						fname(EXIT(ch, door)->keyword));
				else
				{
					sprintf(buf2, "%-5s - PORTA SENZA NOME - RIPORTARE A UN CREATORE IL BUG INDICANDO LA STANZA %d\r\n",
						dirs[door], world[ch->in_room].number);
					mudlog(buf2, NRM, LVL_IMMORT, TRUE);
				}
			}
			if (!IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN))
				strcat(buf, CAP(buf2));
		}
	}
	send_to_char("\r\n&2****->Uscite:<-****&0\r\n", ch);
	
	if (*buf)
		send_to_char(buf, ch);
	else
		send_to_char(" Nessuna.\r\n", ch);
}


ACMD(do_exits)
{
	int door;
	
	*buf = '\0';
	
	if (IS_AFFECTED(ch, AFF_BLIND)) {
		if(GET_SEX(ch)==SEX_FEMALE)
			send_to_char("Non puoi vedere niente, sei cieca!\r\n", ch);
		else
			send_to_char("Non puoi' vedere niente sei cieco!\r\n", ch);
		return;
	}
	for (door = 0; door < NUM_OF_DIRS; door++)
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
			!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
			if (GET_LEVEL(ch) >= LVL_QUEST_MASTER)
				sprintf(buf2, "%-5s - [%5d] %s\r\n", dirs[door],
					world[EXIT(ch, door)->to_room].number,
					world[EXIT(ch, door)->to_room].name);
			else {
				sprintf(buf2, "%-5s - ", dirs[door]);
				if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
					strcat(buf2, "Troppo buio per dirlo.\r\n");
				else {
					strcat(buf2, world[EXIT(ch, door)->to_room].name);
					strcat(buf2, "\r\n");
				}
			}
			strcat(buf, CAP(buf2));
		}
	send_to_char("&2****->Uscite:<-****&0\r\n", ch);
	
	if (*buf)
		send_to_char(buf, ch);
	else
		send_to_char(" Nessuna.\r\n", ch);
}



void look_at_room(struct char_data *ch, int ignore_brief)
{
	int x, y;
//	room_rnum r = 0;
	
	if (!ch->desc)
		return;
	
	if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch)) {
		send_to_char("E' un bel buio qui...\r\n", ch);
		return;
	} else if (IS_AFFECTED(ch, AFF_BLIND)) {
		send_to_char("&4Non vedi che un infinito buio...&0\r\n", ch);
		return;
	}
	
	if (IS_IN_WILD(ch) && !ROOM_FLAGGED(ch->in_room, ROOM_NO_DRAW)) {
		x = wild_time_modify(ch, GET_WILD_MAX_X_RANGE(ch));
		y = wild_time_modify(ch, GET_WILD_MAX_Y_RANGE(ch));
		/*
		if (IS_IN_MINIWILD(ch))
		  r = real_room(MINIWILD_VNUM(real_zone(world[ch->in_room].number), x , y));
		else
		  r = real_room(WILD_VNUM(x, y));
		*/  
		sprintf(buf2, "!!WILD (%d)(%d)\r\n", 
		              ((x*2)+1),
		              ((y*2)+1) );
		send_to_char(buf2, ch);
	  wilderness_draw(ch);
	  send_to_char("!!ENDWILD\r\n", ch);
	}
	
	send_to_char(CCCYN(ch, C_NRM), ch);
	if (PRF_FLAGGED(ch, PRF_ROOMFLAGS)) {
		sprintbit((long long int) ROOM_FLAGS(ch->in_room), room_bits, buf);
		sprintf(buf2, "[%5d] %s [ %s]", world[ch->in_room].number,
			world[ch->in_room].name, buf);
		send_to_char(buf2, ch);
	} 
	else
		send_to_char(world[ch->in_room].name, ch);
	
	send_to_char(CCNRM(ch, C_NRM), ch);
	send_to_char("\r\n", ch);
	
	if (   (ignore_brief) 
	    || (ROOM_FLAGGED(ch->in_room, ROOM_DEATH)) 
	    || (   (!PRF_FLAGGED(ch, PRF_BRIEF)) 
	        && (!IS_IN_WILD(ch) || ROOM_FLAGGED(ch->in_room, ROOM_NO_DRAW)) 
	       )
	   )
		send_to_char(world[ch->in_room].description, ch);
	
  /* autoexits */
	if (PRF_FLAGGED(ch, PRF_AUTOEXIT))
		do_auto_exits(ch);
	
  /* autodir */
	if (PRF_FLAGGED(ch, PRF_AUTODIR))
		do_auto_dir(ch);
	
  /* now list characters & objects */
	send_to_char(CCGRN(ch, C_NRM), ch);
	list_obj_to_char(world[ch->in_room].contents, ch, 0, FALSE);
	send_to_char(CCYEL(ch, C_NRM), ch);
	list_char_to_char(world[ch->in_room].people, ch);
	send_to_char(CCNRM(ch, C_NRM), ch);
}



void look_in_direction(struct char_data * ch, int dir)
{
	
	int room, nextroom, orig_room = ch->in_room;
	int distance;
	
	
	if (IS_AFFECTED(ch, AFF_WIZEYE)){
		if(dir<4)
			sprintf(buf, "Guardi piu' attentamente a %s.\r\n", dirs1[dir]);
		else
			sprintf(buf, "Guardi piu' attentamente in %s.\r\n", dirs1[dir]);
		send_to_char(buf, ch);
		
		if (EXIT(ch, dir) && EXIT(ch, dir)->to_room != NOWHERE &&
			!IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED)) {
			send_to_char(EXIT(ch, dir)->general_description, ch);
			send_to_char("\r\n",ch);
			send_to_char(world[EXIT(ch, dir)->to_room].description, ch);
			send_to_char("\r\n",ch);
			list_obj_to_char(world[EXIT(ch,dir)->to_room].contents, ch, 0, FALSE);
			send_to_char(CCYEL(ch, C_NRM), ch);
			list_char_to_char(world[EXIT(ch,dir)->to_room].people, ch);
			send_to_char(CCNRM(ch, C_NRM), ch);
		}
	}
	else
	if (EXIT(ch, dir)) {
		if (EXIT(ch, dir)->general_description)
			send_to_char(EXIT(ch, dir)->general_description, ch);
		else
			send_to_char("Non vedi niente di speciale.\r\n", ch);
		
		if (IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED) && EXIT(ch, dir)->keyword && !(IS_SET(EXIT(ch,dir)->exit_info, EX_HIDDEN))) {
			sprintf(buf, "La %s chiusa.\r\n", fname(EXIT(ch, dir)->keyword));
			send_to_char(buf, ch);
		} else if (IS_SET(EXIT(ch, dir)->exit_info, EX_ISDOOR) && EXIT(ch, dir)->keyword && !(IS_SET(EXIT(ch,dir)->exit_info, EX_HIDDEN)))  {
			sprintf(buf, "La %s e' aperta.\r\n", fname(EXIT(ch, dir)->keyword));
			send_to_char(buf, ch);
		}
		
		if CAN_GO2(orig_room, dir)
			nextroom = EXIT2(orig_room, dir)->to_room;
		else
			nextroom = NOWHERE;
		
		for (distance = 0; ((nextroom != NOWHERE) && (distance < 3)); distance++) {
			
			if (world[nextroom].people)
				list_scanned_chars(world[nextroom].people, ch, distance, dir, FALSE);
			
			room = nextroom;
			if CAN_GO2(room, dir)
				nextroom = EXIT2(room, dir)->to_room;
			else
				nextroom = NOWHERE;
			
		}
	}
	else
		send_to_char("Niente di speciale la'...\r\n", ch);
}



void look_in_obj(struct char_data * ch, char *arg)
{
	struct obj_data *obj = NULL;
	struct char_data *dummy = NULL;
	int amt, bits;
	*buf = '\0';
	if (!*arg)
		send_to_char("Guardare dentro a cosa??\r\n", ch);
	else if (!(bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM |
					FIND_OBJ_EQUIP, ch, &dummy, &obj))) {
		sprintf(buf, "Non sembra esserci %s %s qui.\r\n", AN(arg), arg);
		send_to_char(buf, ch);
	} else if ((GET_OBJ_TYPE(obj) != ITEM_DRINKCON) &&
		(GET_OBJ_TYPE(obj) != ITEM_FOUNTAIN) &&
		(GET_OBJ_TYPE(obj) != ITEM_CONTAINER))
		send_to_char("Niente di speciale dentro.\r\n", ch);
	else {
		if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER) {
			if (IS_SET(GET_OBJ_VAL(obj, 1), CONT_CLOSED))
				send_to_char("E' chiusa.\r\n", ch);
			else {
				send_to_char(fname(obj->name), ch);
				switch (bits) {
					case FIND_OBJ_INV:
						send_to_char(" (portata): \r\n", ch);
						break;
					case FIND_OBJ_ROOM:
						send_to_char(" (qui): \r\n", ch);
						break;
					case FIND_OBJ_EQUIP:
						send_to_char(" (usata): \r\n", ch);
						break;
				}
				
				list_obj_to_char(obj->contains, ch, 2, TRUE);
			}
		} else {		/* item must be a fountain or drink container */
			if (GET_OBJ_VAL(obj, 1) <= 0)
				send_to_char("E' vuota.\r\n", ch);
			else {
				if (GET_OBJ_VAL(obj,0) <= 0 || GET_OBJ_VAL(obj,1)>GET_OBJ_VAL(obj,0)) {
					sprintf(buf, "Il suo contenuto sembra essere tenebroso.\r\n"); /* BUG */
				} else {
					amt = (GET_OBJ_VAL(obj, 1) * 3) / GET_OBJ_VAL(obj, 0);
					sprinttype(GET_OBJ_VAL(obj, 2), color_liquid, buf2);
					sprintf(buf, "E' %spiena di un liquido %s.\r\n", fullness[amt], buf2);
				}
				send_to_char(buf, ch);
			}
		}
	}
}



char *find_exdesc(char *word, struct extra_descr_data * list)
{
	struct extra_descr_data *i;
	
	for (i = list; i; i = i->next)
		if (isname(word, i->keyword))
			return (i->description);
		
	return NULL;
}


/*
 * Given the argument "look at <target>", figure out what object or char
 * matches the target.  First, see if there is another char in the room
 * with the name.  Then check local objs for exdescs.
 */
void look_at_target(struct char_data * ch, char *arg)
{
	int bits, found = 0, j;
	struct char_data *found_char = NULL;
	struct obj_data *obj = NULL, *found_obj = NULL;
	char *desc;
	if (!ch->desc)
		return;
	
	
	if (!*arg) {
		send_to_char("Guardare che cosa?\r\n", ch);
		return;
	}
	bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP |
			FIND_CHAR_ROOM, ch, &found_char, &found_obj);
	
  /* Is the target a character? */
	if (found_char != NULL) {
		look_at_char(found_char, ch);
		if (ch != found_char) {
			if (CAN_SEE(found_char, ch))
				act("$n ti guarda.", TRUE, ch, 0, found_char, TO_VICT);
			act("$n guarda $N.", TRUE, ch, 0, found_char, TO_NOTVICT);
		}
		return;
	}
  /* Does the argument match an extra desc in the room? */
	if ((desc = find_exdesc(arg, world[ch->in_room].ex_description)) != NULL) {
		page_string(ch->desc, desc, 0);
		return;
	}
	
 /* Does the argument match an extra desc in the char's equipment? */
	for (j = 0; j < NUM_WEARS && !found; j++)
		if (GET_EQ(ch, j) && CAN_SEE_OBJ(ch, GET_EQ(ch, j)))
			if ((desc = find_exdesc(arg, GET_EQ(ch, j)->ex_description)) != NULL) {
				send_to_char(desc, ch);
				
				
				found = 1;
			}
  /* Does the argument match an extra desc in the char's inventory? */
	for (obj = ch->carrying; obj && !found; obj = obj->next_content) {
		if (CAN_SEE_OBJ(ch, obj))
			if ((desc = find_exdesc(arg, obj->ex_description)) != NULL) {
				send_to_char(desc, ch);
				
				
				found = 1;
			}
	}
	
  /* Does the argument match an extra desc of an object in the room? */
	for (obj = world[ch->in_room].contents; obj && !found; obj =
			obj->next_content)
		if (CAN_SEE_OBJ(ch, obj))
			if ((desc = find_exdesc(arg, obj->ex_description)) != NULL) {
				send_to_char(desc, ch);
				
				
				found = 1;
			}
	if (bits) {                   /* If an object was found back in
                                    * generic_find */
		if (!found)
			show_obj_to_char(found_obj, ch, 5);	/* Show no-description */
		else
			show_obj_to_char(found_obj, ch, 6);	/* Find hum, glow etc */
	} else if (!found)
		send_to_char("Non lo vedi qui.\r\n", ch);
}


ACMD(do_look)
{
	static char arg2[MAX_INPUT_LENGTH];
	int look_type;
	
	if (!ch->desc)
		return;
	
	if (GET_POS(ch) < POS_SLEEPING || IS_AFFECTED(ch, AFF_TRAMORTITO))
		send_to_char("Vedi solo delle stelle.\r\n", ch);
	else if (IS_AFFECTED(ch, AFF_BLIND)) {
		if (GET_SEX(ch)==SEX_FEMALE)
			send_to_char("Non vedi niente, sei cieca!!\r\n", ch);
		else
			send_to_char("Non vedi niente, sei cieco!!\r\n", ch);
	}
	else if (IS_AFFECTED(ch, AFF_PARALIZE)){
		if (GET_SEX(ch)==SEX_FEMALE)
			send_to_char("Non puoi fare niente, sei paralizzata!!\r\n", ch);
		else
			send_to_char("Non puoi fare niente, sei paralizzato!!\r\n", ch);
	}
	else if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch)) {
		send_to_char("E' un buio profondo...\r\n", ch);
		list_char_to_char(world[ch->in_room].people, ch);	/* glowing red eyes */
	} 
	else {
		half_chop(argument, arg, arg2);
		
		if (subcmd == SCMD_READ) {
			if (!*arg)
				send_to_char("Leggere cosa?\r\n", ch);
			else
				look_at_target(ch, arg);
			return;
		}
		if (!*arg)			/* "look" alone, without an argument at all */
			look_at_room(ch, 1);
		else if (is_abbrev(arg, "in"))
			look_in_obj(ch, arg2);
    /* did the char type 'look <direction>?' */
		else if ((look_type = search_block(arg, dirs, FALSE)) >= 0)
			look_in_direction(ch, look_type);
		else if (is_abbrev(arg, "a"))
			look_at_target(ch, arg2);
		else
			look_at_target(ch, arg);
	}
}

ACMD(do_peep)
{
	if (!ch->desc)
		return;
	
	if (GET_POS(ch) < POS_SLEEPING || IS_AFFECTED(ch, AFF_TRAMORTITO))
		send_to_char("Vedi solo delle stelle.\r\n", ch);
	else if (IS_AFFECTED(ch, AFF_BLIND)){
		if (GET_SEX(ch)==SEX_FEMALE)
			send_to_char("Non vedi niente, sei cieca!!\r\n", ch);
		else
			send_to_char("Non vedi niente, sei cieco!!\r\n", ch);
	}
	else if (IS_AFFECTED(ch, AFF_IMMOBIL))
		send_to_char("Non puoi fare niente, sei stato immobilizzato!!\r\n", ch);
	else if (IS_AFFECTED(ch, AFF_PARALIZE)){
		if (GET_SEX(ch)==SEX_FEMALE)
			send_to_char("Non puoi fare niente, sei paralizzata!!\r\n", ch);
		else
			send_to_char("Non puoi fare niente, sei paralizzato!!\r\n", ch);
	}
	else if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch)) {
		send_to_char("E' un buio profondo...\r\n", ch);
		list_char_to_char(world[ch->in_room].people, ch);	/* glowing red eyes */
	}
	else {
		one_argument(argument, arg);
		if (!*arg)			/* "look" alone, without an argument at all */
			send_to_char("Chi vuoi sbirciare?\r\n", ch);
		else{
			if(test_and_improve(ch, SKILL_PEEP, 100, 15, dex_app_skill[GET_DEX(ch)].p_pocket))
				peep_at_target(ch, arg);
			else{
				send_to_char("Non ci riesci!\r\n",ch);
				return;
			}
		}
	}
}

ACMD(do_players)
{
	int i;
	struct char_file_u fch;
	char tmp_name[MAX_INPUT_LENGTH];
	char bigbuf[MAX_STRING_LENGTH*10];
	FILE *dump_fl = NULL;
	bigbuf[0] = '\0';
	
	touch("etc/dumplayers");
	if (!(dump_fl = fopen("etc/dumplayers", "r+b"))) {
		perror("fatal error opening DUMP Player File");
		exit(1);
	}
	
	for (i = 0; i <= top_of_p_table; i++) {
		sprintf(bigbuf, "[%-20.20s %-2.2i ]", (player_table + i)->name, (player_table + i)->level);
		log(bigbuf);
		
		if (   (_parse_name((player_table + i)->name, tmp_name)) 
		    || strlen(tmp_name) < 2 
		    || strlen(tmp_name) > MAX_NAME_LENGTH 
		    || fill_word(strcpy(buf, tmp_name)) 
		    || reserved_word(buf)
		   ) {
		  continue;
		}
		
		if (   ((player_table + i)->level < 1)
		    && ((player_table + i)->level > 91) 
		   ) {
		  continue;
		}
		
		if (load_char((player_table + i)->name, &fch) == -1) {
			log("NoT-FounD");
			continue;
		}
		  
		if (!IS_SET(fch.char_specials_saved.act, PLR_DELETED)) {
		  log("Dumped");
		  //SAVEIT
			fwrite(&fch, sizeof(struct char_file_u), 1, dump_fl);
		}  
	} //End For
  
  fclose(dump_fl);
  
  rename("etc/players", "etc/players_phan");
  rename("etc/dumplayers", "etc/players");
}

ACMD(do_examine)
{
	int bits;
	struct char_data *tmp_char;
	struct obj_data *tmp_object;
	one_argument(argument, arg);
	
	if (!*arg) {
		send_to_char("Esaminare cosa?\r\n", ch);
		return;
	}
	look_at_target(ch, arg);
	
	bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_CHAR_ROOM |
			FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);
	
	if (tmp_object) {
		if ((GET_OBJ_TYPE(tmp_object) == ITEM_DRINKCON) ||
			(GET_OBJ_TYPE(tmp_object) == ITEM_FOUNTAIN) ||
			(GET_OBJ_TYPE(tmp_object) == ITEM_CONTAINER)) {
			send_to_char("Quando guardi dentro vedi:\r\n", ch);
			look_in_obj(ch, arg);
		}
	}
	
 // don't report condition if npc/pc
	if ((tmp_char) && (!tmp_object)) {
		return;
	}
	
}

ACMD(do_search)
{
	int door, chance = 1;
	
	*buf = '\0';
	
	if (IS_AFFECTED(ch, AFF_BLIND)) {
		send_to_char("Your blind, you can't see a damned thing!", ch);
		return;
	}
	
	send_to_char("\r\nInizi a cercare...\r\n", ch);
	
	for (door = 0; door < NUM_OF_DIRS; door++) {
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE) {
			if (IS_SET(EXIT(ch, door)->exit_info, EX_HIDDEN)) {
				if (GET_INT(ch) >= 17)
					chance += 5;
				if (IS_THIEF(ch)) chance += 10;
				if (number(1,20) <= chance) {
					sprintf(buf, "\r\nHai trovato una porta segreta %s.\r\n",
						dirs1[door]);
					send_to_char(buf, ch);
					REMOVE_BIT(EXIT(ch, door)->exit_info, EX_HIDDEN);
					SET_BIT (EXIT(ch, door)->exit_info, EX_ISDOOR);
				}
			}
		}
	}
	WAIT_STATE (ch, PULSE_VIOLENCE);
	return;
}

ACMD(do_gold)
{
	if (GET_GOLD(ch) == 0)
		send_to_char("Sei povero in canna!\r\n", ch);
	else if (GET_GOLD(ch) == 1)
		send_to_char("Hai solo uno sporco e miserabile coin.\r\n", ch);
	else {
		sprintf(buf, "Hai %d coins.\r\n", GET_GOLD(ch));
		send_to_char(buf, ch);
	}
}

extern char *hometown[];
const char *allin_mess[]= {"&1Demoniaco&0", "&1Perfido&0", "&1Malvagio&0", "&1Crudele&0",
	"Neutrale" , "Neutrale" ,"&6Buono&0", "&6Santo&0", "&6Beato&0", "&6Celestiale&0"};
const char *armor_mess[]= {
	"&2Blindato&0", "&2Corazzato&0", "&3Quasi Corazzato&0", "&3Molto Protetto&0",
	"Protetto", "Abbastanza Protetto", "&8Poco Protetto&0", "&8Per niente Protetto&0", "&1Vulnerabile&0", "&1Nudo&0" };
const char *fama_mess[]= {
	"&1Sconosciuto&0", "&2Uno Qualunque&0", "&2Poco Conosciuto&0", "&3Conosciuto&0", "&3Molto Conosciuto&0",
	"&0Abbastanza Famoso&0", "&0Famoso&0", "&8Celebre&0", "&8Eroe&0", "&1Mitico&0", "&1Leggendario&0" };
const char *notorieta_mess[]= {
	"&1Born Killer!&0","&2Delinquente&0","&3Canaglia&0", "&4Mela Marcia&0","&4Teppista&0",
	"&5Cittadino&0","&6Cittadino Modello&0","&7Cacciatore di Taglie&0","&8Guardia Cittadina&0","&1Paladino del popolo&0"};

ACMD(do_score)
{
	extern int real_zone(room_vnum number);
	extern struct str_app_type str_app[];
	extern char *nomi_religioni[];
	struct time_info_data playing_time;
	struct time_info_data real_time_passed(time_t t2, time_t t1);
	int tempcount;
	
	sprintf(buf, "------------------------------------------------------------\r\n");
  //sprintf(buf+strlen(buf), "Nome: %s %s\r\n", GET_NAME(ch), GET_TITLE(ch));
	if (AFF_FLAGGED(ch,AFF_DISGUISE) && (ch->player.namedisguise))
		sprintf(buf+strlen(buf), "Nome: &7%s&0 (%s) %s\r\n", GET_NAME(ch), ch->player.namedisguise ,  GET_TITLE(ch));
	else
		sprintf(buf+strlen(buf), "Nome: &7%s&0 %s\r\n", GET_NAME(ch), GET_TITLE(ch));
	sprintf(buf+strlen(buf), "Sesso: %s\r\n", ( GET_SEX(ch)==SEX_MALE ? "Maschile" : "Femminile"));
	sprintf(buf+strlen(buf), "Altezza: %d cm Peso: %d lbs\r\n", GET_HEIGHT(ch), GET_WEIGHT(ch));
	sprintf(buf+strlen(buf), "Hai %d anni", GET_AGE(ch));
	if ((age(ch).month == 0) && (age(ch).day == 0))
		strcat(buf, " e oggi e' il tuo compleanno\r\n");
	else strcat(buf, "\r\n");
	sprintf(buf+strlen(buf), "Citta' natale: %s\r\n", hometown[GET_HOME(ch)-1]);
	sprintf(buf+strlen(buf), "Ordine: %s Livello: %d \r\n", CLASS_ABBR(ch),GET_LEVEL(ch));
	tempcount=(char)((GET_ALIGNMENT(ch)+1000)/200);
	tempcount = MAX(0, MIN(tempcount, 9));
	sprintf(buf+strlen(buf), "Allineamento: %s\r\n", allin_mess[tempcount]);
	if (!IS_NPC(ch))
		sprintf(buf+strlen(buf), "Sei stato iniziato alla seguente religione: %s.\r\n", nomi_religioni[GET_CULTO(ch)]);
	if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT)
	{
        tempcount = (char) (GET_FAMA(ch)/100);
		tempcount = MAX(0, MIN(tempcount, 10));
		sprintf(buf+strlen(buf), "Notorieta': %s\r\n", fama_mess[tempcount]);
		tempcount=(char)((GET_NOTORIETA(ch))/100);
		tempcount = MAX(0, MIN(tempcount, 9));
		sprintf(buf+strlen(buf), "Fama: %s\r\n",notorieta_mess[tempcount]);
		sprintf(buf+strlen(buf), "Punti Quest: %d\r\n", GET_QUESTPOINTS(ch)); // Orione: visualizzazione questpoints
	}
	sprintf(buf+strlen(buf), "Punti Ferita: %d%% Mana: %d%% Punti Movimento: %d%%\r\n",
		((GET_HIT(ch)*100)/ GET_MAX_HIT(ch)),((GET_MANA(ch)*100)/GET_MAX_MANA(ch)),((GET_MOVE(ch)*100)/GET_MAX_MOVE(ch)));	
	tempcount=(char) ((GET_AC(ch)+100)/20);
	tempcount = MAX(0, MIN(tempcount, 9));
	sprintf(buf+strlen(buf), "Classe Armatura: %s\r\n", armor_mess[tempcount]);
	sprintf(buf+strlen(buf), "Stai portando un peso di %d Kg contro un massimo di %d Kg.\r\n",
		IS_CARRYING_W(ch), CAN_CARRY_W(ch));
	sprintf(buf+strlen(buf), "Stai portando %d oggetti contro un massimo di %d oggetti.\r\n",
		IS_CARRYING_N(ch), CAN_CARRY_N(ch));
	sprintf(buf+strlen(buf), "Monete: %d Estratto Conto: %d\r\n", GET_GOLD(ch), GET_BANK_GOLD(ch));
	playing_time =real_time_passed((time(0) - ch->player.time.logon) + ch->player.time.played, 0);
	sprintf(buf+strlen(buf), "Stai giocando da %d giorni e %d ore.\r\n", playing_time.day, playing_time.hours);
	
	switch (GET_POS(ch))
	{
		case POS_DEAD:
			if (GET_SEX(ch)==SEX_FEMALE) strcat(buf, "Sei MORTA!\r\n");
			else strcat(buf, "Sei MORTO!\r\n");
			break;
		case POS_MORTALLYW:
			if (GET_SEX(ch)==SEX_FEMALE) strcat(buf, "Sei ferita mortalmente! Dovresti cercare aiuto!\r\n");
			else strcat(buf, "Sei ferito mortalmente! Dovresti cercare aiuto!\r\n");
			break;
		case POS_INCAP:
			strcat(buf, "Sei incapace di muoverti, senti i sensi che ti abbandonano!\r\n");
			break;
		case POS_STUNNED:
			if (GET_SEX(ch)==SEX_FEMALE) strcat(buf, "Sei svenuta! Non ti puoi muovere!\r\n");
			else strcat(buf, "Sei svenuto! Non ti puoi muovere!\r\n");
			break;
		case POS_SLEEPING:
			strcat(buf, "Stai dormendo.\r\n");
			break;
		case POS_RESTING:
			strcat(buf, "Ti stai riposando.\r\n");
			break;
		case POS_SITTING:
			if (GET_SEX(ch)==SEX_FEMALE) strcat(buf, "Sei seduta.\r\n");
			else strcat(buf, "Sei seduto.\r\n");
			break;
		case POS_FIGHTING:
			if (FIGHTING(ch)) sprintf(buf+strlen(buf), "Stai combattendo contro %s.\r\n", PERS(FIGHTING(ch), ch));
			else strcat(buf, "Stai combattendo contro l'aria fresca.\r\n");
			break;
		case POS_STANDING:
			if (RIDING(ch))        
				strcat(buf, "Stai cavalcando.\r\n");
			else if (TRANSPORTED_BY(ch))
				strcat(buf,"Sei trasportato sulle spalle.\r\n");
			else strcat(buf, "Sei in piedi.\r\n");
			break;
		default:
			strcat(buf, "Stai galleggiando.\r\n");
			break;
	}
	
  /*Enrico add :block for AFF_IMMOBIL characters*/
	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
		strcat(buf, "Sei Immobilizzato.\r\n");
	
	if (GET_COND(ch, DRUNK) > 10 && GET_LEVEL(ch)< LVL_QUEST_MASTER){
		if (GET_SEX(ch)==SEX_FEMALE)
			strcat(buf, "Sei ubriaca.\r\n");
		else strcat(buf, "Sei ubriaco.\r\n");
	}
	if (GET_COND(ch, FULL) <48 && GET_LEVEL(ch)< LVL_QUEST_MASTER){
		if (GET_SEX(ch)==SEX_FEMALE)
			strcat(buf, "Sei affamata.\r\n");
		else
			strcat(buf, "Sei affamato.\r\n");
	}
	if (GET_COND(ch, THIRST) <48 && GET_LEVEL(ch)< LVL_QUEST_MASTER){
		if (GET_SEX(ch)==SEX_FEMALE)
			strcat(buf, "Sei assetata.\r\n");
		else
			strcat(buf, "Sei assetato.\r\n");
	}
	if (GET_SEX(ch)==SEX_FEMALE)
		sprintf(buf+strlen(buf), "Sei morta %d volte e hai ucciso %d nemici.\r\n", GET_RIP_CNT(ch), GET_KILL_CNT(ch));
	else
		sprintf(buf+strlen(buf), "Sei morto %d volte e hai ucciso %d nemici.\r\n", GET_RIP_CNT(ch), GET_KILL_CNT(ch));
	if (ch->player_specials->saved.morti_per_livello){
		if (GET_SEX(ch)==SEX_FEMALE)
			sprintf(buf+strlen(buf), "Sei morta %d volte da quando sei a questo livello.\r\n", ch->player_specials->saved.morti_per_livello);
		else
			sprintf(buf+strlen(buf), "Sei morto %d volte da quando sei a questo livello.\r\n", ch->player_specials->saved.morti_per_livello);
	}
	else {
		if (GET_SEX(ch)==SEX_FEMALE)
			strcat(buf, "Non sei mai morta da quando sei a questo livello.\r\n");
		else
			strcat(buf, "Non sei mai morto da quando sei a questo livello.\r\n");
	}
	if (GET_SEX(ch)==SEX_FEMALE)
		sprintf(buf+strlen(buf), "Sei finita in una deathtrap %d volte.\r\n", GET_DT_CNT(ch));
	else
		sprintf(buf+strlen(buf), "Sei finito in una deathtrap %d volte.\r\n", GET_DT_CNT(ch));
	
	if (GET_OLC_ZONE(ch)>0)
		sprintf(buf+strlen(buf), "Sei gestore di un feudo (da %d a %d).\r\n", GET_OLC_ZONE(ch)*100, zone_table[real_zone(GET_OLC_ZONE(ch)*100)].top);
	if (GET_ASHOP(ch)>0)
		sprintf(buf, "%sSei gestore di un negozio (numero %d).\r\n", buf, GET_ASHOP(ch));
	
	
	sprintf(buf+strlen(buf), "------------------------------------------------------------\r\n");
	sprintf(buf+strlen(buf), "Incantesimi attivi:\r\n");
	for (tempcount=1;tempcount<MAX_SPELLS; tempcount++)
		if (affected_by_spell(ch, SPELLSKILL, tempcount))
			sprintf (buf+strlen(buf), "%s\r\n", spells[tempcount]);
	if (IS_AFFECTED(ch, AFF_SNEAK))
		strcat(buf, "Ti stai muovendo silenziosamente.\r\n");
	if (IS_AFFECTED(ch, AFF_HIDE)){
		if (GET_SEX(ch)==SEX_FEMALE)
			strcat(buf, "Sei nascosta alla vista.\r\n");
		else
			strcat(buf, "Sei nascosto alla vista.\r\n");
	}
	sprintf(buf+strlen(buf), "------------------------------------------------------------\r\n");
	send_to_char(buf, ch);

	// Mostra le malattie (by Spini)
	sprintf(buf, "Attualmente hai i sintomi delle seguenti malattie:\r\n");
	for (tempcount = MAX_SKILLS+1; tempcount < TOP_SPELL_DEFINE; tempcount++)
		if (affected_by_spell(ch, SPELLSKILL, tempcount))
			sprintf (buf+strlen(buf), "%s\r\n", spells[tempcount]);
	sprintf(buf+strlen(buf), "------------------------------------------------------------\r\n");
	send_to_char(buf, ch);

	//se sei un fantasma nello score ti avverte :)
	if(PLR_FLAGGED(ch,PLR_FANTASMA))
	send_to_char("&8&7Al momento sei un Fantasma.&0\r\n",ch);

	if (PLR_FLAGGED(ch, PLR_GO_GILDA))
		send_to_char ("&1Puoi recarti alla tua gilda per ottenere il titolo superiore.&0\r\n", ch);
	
}

//PEPPE CRITICAL
ACMD(do_stato){
	struct char_data *victim;
	extern const char * frasi_resistenze[][6];
	
	one_argument(argument,arg);
	
	if(!*arg)
	    victim=ch;
	else if(!(victim=get_char_room_vis(ch,arg))){
	send_to_char("Non c'e' nessun con quel nome.\r\n",ch);
	return;
	}
	
	send_to_char("&8&6Stato fisico attuale:\r\n",ch);
	send_to_char("&8&1TESTA :&0",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_3))
	    send_to_char("&8&7Danni critici&0\r\n",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_2) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_3)))
	    send_to_char("&8&7Danni seri&0\r\n",ch);
	if((AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_3)))    	
	    send_to_char("&8&7Danni superficiali&0\r\n",ch);
	if((!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TESTA_3)))	    
	    send_to_char("&8&7Nessun danno&0\r\n",ch);

	send_to_char("&8&1BRACCIO DESTRO :&0",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3))
	    send_to_char("&8&7Danni critici&0\r\n",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_2) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3)))
	    send_to_char("&8&7Danni seri&0\r\n",ch);
	if((AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3)))    	
	    send_to_char("&8&7Danni superficiali&0\r\n",ch);
	if((!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_D_3)))	    
	    send_to_char("&8&7Nessun danno&0\r\n",ch);


	send_to_char("&8&1BRACCIO SINISTRO :&0",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3))
	    send_to_char("&8&7Danni critici&0\r\n",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_2) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3)))
	    send_to_char("&8&7Danni seri&0\r\n",ch);
	if((AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3)))    	
	    send_to_char("&8&7Danni superficiali&0\r\n",ch);
	if((!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_BRACCIO_S_3)))	    
	    send_to_char("&8&7Nessun danno&0\r\n",ch);

	send_to_char("&8&1ADDOME:&0",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3))
	    send_to_char("&8&7Danni critici&0\r\n",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_2) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3)))
	    send_to_char("&8&7Danni seri&0\r\n",ch);
	if((AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3)))    	
	    send_to_char("&8&7Danni superficiali&0\r\n",ch);
	if((!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_TORSO_3)))	    
	    send_to_char("&8&7Nessun danno&0\r\n",ch);


	send_to_char("&8&1GAMBA DESTRA :&0",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3))
	    send_to_char("&8&7Danni critici&0\r\n",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_2) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3)))
	    send_to_char("&8&7Danni seri&0\r\n",ch);
	if((AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3)))    	
	    send_to_char("&8&7Danni superficiali&0\r\n",ch);
	if((!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_D_3)))	    
	    send_to_char("&8&7Nessun danno&0\r\n",ch);


	send_to_char("&8&1GAMBA SINISTRA :&0",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3))
	    send_to_char("&8&7Danni critici&0\r\n",ch);
	if(AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_2) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3)))
	    send_to_char("&8&7Danni seri&0\r\n",ch);
	if((AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3)))    	
	    send_to_char("&8&7Danni superficiali&0\r\n",ch);
	if((!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_1)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_2)) && (!AFF_FLAGGED(victim,AFF_CRITICAL_GAMBA_S_3)))	    
	    send_to_char("&8&7Nessun danno&0\r\n",ch);

	send_to_char("&8&1FERITE :&0",ch);
	if(AFF_FLAGGED(victim,AFF_FERITO))
	    send_to_char("&8&7Sanguinanti&0\r\n",ch);
	else
	    send_to_char("&8&7Rimarginate&0\r\n",ch);

	if (ch != victim)
		return;

	send_to_char ("\r\n\r\n&4&bResistenze:\r\n", ch);
	sprintf (buf, "&0&1Fuoco:        ");
	if (GET_RES_FUOCO(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[0][0]);
	else if (GET_RES_FUOCO(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[0][1]);
	else if (GET_RES_FUOCO(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[0][2]);
	else if (GET_RES_FUOCO(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[0][3]);
	else if (GET_RES_FUOCO(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[0][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[0][5]);

	sprintf (buf, "%s&6&bGhiaccio:     ", buf);
	if (GET_RES_GHIACCIO(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[1][0]);
	else if (GET_RES_GHIACCIO(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[1][1]);
	else if (GET_RES_GHIACCIO(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[1][2]);
	else if (GET_RES_GHIACCIO(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[1][3]);
	else if (GET_RES_GHIACCIO(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[1][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[1][5]);

	sprintf (buf, "%s&3Elettricita': ", buf);
	if (GET_RES_ELETTRICITA(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[2][0]);
	else if (GET_RES_ELETTRICITA(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[2][1]);
	else if (GET_RES_ELETTRICITA(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[2][2]);
	else if (GET_RES_ELETTRICITA(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[2][3]);
	else if (GET_RES_ELETTRICITA(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[2][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[2][5]);

	sprintf (buf, "%s&2Acido:        ", buf);
	if (GET_RES_ACIDO(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[3][0]);
	else if (GET_RES_ACIDO(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[3][1]);
	else if (GET_RES_ACIDO(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[3][2]);
	else if (GET_RES_ACIDO(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[3][3]);
	else if (GET_RES_ACIDO(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[3][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[3][5]);

	sprintf (buf, "%s&1Oscurita':    ", buf);
	if (GET_RES_SHAARR(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[4][0]);
	else if (GET_RES_SHAARR(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[4][1]);
	else if (GET_RES_SHAARR(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[4][2]);
	else if (GET_RES_SHAARR(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[4][3]);
	else if (GET_RES_SHAARR(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[4][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[4][5]);

	sprintf (buf, "%s&0&5Caos:         ", buf);
	if (GET_RES_XHYPHYS(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[5][0]);
	else if (GET_RES_XHYPHYS(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[5][1]);
	else if (GET_RES_XHYPHYS(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[5][2]);
	else if (GET_RES_XHYPHYS(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[5][3]);
	else if (GET_RES_XHYPHYS(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[5][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[5][5]);

	sprintf (buf, "%s&0Ordine:       ", buf);
	if (GET_RES_THERION(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[6][0]);
	else if (GET_RES_THERION(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[6][1]);
	else if (GET_RES_THERION(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[6][2]);
	else if (GET_RES_THERION(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[6][3]);
	else if (GET_RES_THERION(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[6][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[6][5]);

	sprintf (buf, "%s&7&bLuce:         ", buf);
	if (GET_RES_SILUE(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[7][0]);
	else if (GET_RES_SILUE(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[7][1]);
	else if (GET_RES_SILUE(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[7][2]);
	else if (GET_RES_SILUE(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[7][3]);
	else if (GET_RES_SILUE(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[7][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[7][5]);

	sprintf (buf, "%s&9&bDanno fisico: ", buf);
	if (GET_RES_FISICO(ch) < 0) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[8][0]);
	else if (GET_RES_FISICO(ch) < 16) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[8][1]);
	else if (GET_RES_FISICO(ch) < 31) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[8][2]);
	else if (GET_RES_FISICO(ch) < 51) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[8][3]);
	else if (GET_RES_FISICO(ch) < 90) sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[8][4]);
	else sprintf(buf, "%s%s\r\n", buf, frasi_resistenze[8][5]);

	sprintf (buf, "%s&0\r\n", buf);
	send_to_char (buf, ch);
}


ACMD(do_bar)
{
  /* Then in do_score I put in these:*/
	meter_bar(ch, 0);
	meter_bar(ch, 1);
	meter_bar(ch, 2);
}


ACMD(do_inventory)
{
	send_to_char("Stai portando:\r\n", ch);
	list_obj_to_char(ch->carrying, ch, 1, TRUE);
}


ACMD(do_equipment)
{
	int i,j, found = 0;
	
	send_to_char("\tStai usando:\r\n\n", ch);
	for (i = 0; i < NUM_WEARS; i++) {
		j=real_pos[i];
		if (GET_EQ(ch, j)) {
			if (CAN_SEE_OBJ(ch, GET_EQ(ch, j))) {
				send_to_char(where[j], ch);
				show_obj_to_char(GET_EQ(ch, j), ch, 1);
				found = TRUE;
			} else {
				send_to_char(where[j], ch);
				send_to_char("Qualcosa.\r\n", ch);
				found = TRUE;
			}
		}
	}
	if (!found) {
		send_to_char(" Niente.\r\n", ch);
	}
}


ACMD(do_time)
{
	int weekday, day;
	extern struct time_info_data time_info;
	extern const char *weekdays[];
	extern const char *month_name[];
	
	sprintf(buf, "Sono le %d %s del ",
		((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
		((time_info.hours >= 12) ? "pm" : "am"));
	
  /* 35 days in a month */
	weekday = ((35 * time_info.month) + time_info.day + 1) % 7;
	
	strcat(buf, weekdays[weekday]);
	strcat(buf, ".\r\n");
	send_to_char(buf, ch);
	
	day = time_info.day + 1;	/* day in [1..35] */
	
	sprintf(buf, "E' il giorno %d del %s, Anno %d.\r\n",
		day, month_name[(int) time_info.month], time_info.year);
	send_to_char(buf, ch);
}


ACMD(do_weather)
{
	extern struct zone_data *zone_table;
	static char *sky_look[] = {
		"poco nuvoloso",
		"nuvoloso",
		"piovoso",
		"illuminato a giorno da lampi"};
	
	if (OUTSIDE(ch)) {
		sprintf(buf, "Il cielo e' %s e %s.\r\n",
			sky_look[zone_table[world[ch->in_room].zone].sky],
			(zone_table[world[ch->in_room].zone].change >= 0 ?
				"senti un vento caldo da sud.." :
				"..che tempaccio eh..."));
		send_to_char(buf, ch);
	} else
		send_to_char("Sei in un luogo chiuso, non hai idea di come sia il tempo fuori!\r\n", ch);
}


ACMD(do_help)
{
	extern int top_of_helpt;
	extern struct help_index_element *help_table;
	extern char *help;
	
	int chk, bot, top, mid, minlen;
	
	if (!ch->desc)
		return;
	
	skip_spaces(&argument);
	
	if (!*argument) {
		page_string(ch->desc, help, 0);
		return;
	}
	if (!help_table) {
		send_to_char("Nessun aiuto disponibile.\r\n", ch);
		return;
	}
	
	bot = 0;
	top = top_of_helpt;
	minlen = strlen(argument);
	
	for (;;) {
		mid = (bot + top) / 2;
		
		if (bot > top) {
			send_to_char("Non c'e' aiuto per quella parola.\r\n", ch);
			return;
		} else if (!(chk = strn_cmp(argument, help_table[mid].keyword, minlen))) {
      /* trace backwards to find first matching entry. Thanks Jeff Fink! */
			while ((mid > 0) &&
				(!(chk = strn_cmp(argument, help_table[mid - 1].keyword, minlen))))
				mid--;
			page_string(ch->desc, help_table[mid].entry, 0);
			return;
		} else {
			if (chk > 0)
				bot = mid + 1;
			else
				top = mid - 1;
		}
	}
}

/* this little function will compare apples and pears and return
-1, 0 or 1 depending on which fruit weighs most
*/
int compareChars(const void *l, const void *r) {
	
	struct char_data **left;
	struct char_data **right;
	int levleft, levright;
	
	left = (struct char_data **)l;
	right = (struct char_data **)r;
	
	if (AFF_FLAGGED(*left, AFF_DISGUISE) && ((*left)->player.namedisguise))
		levleft = (*left)->player.livdisguise;
	else
		levleft = GET_LEVEL(*left);
	
	if (AFF_FLAGGED(*right,AFF_DISGUISE) && ((*right)->player.namedisguise))
		levright = (*right)->player.livdisguise;
	else
		levright = GET_LEVEL(*right);
	
	
	if(levleft < levright)
		return 1;
	else if(levleft == levright)
		return 0;
	else
		return -1;
}

struct char_data *theWhoList[500];

#define WHO_FORMAT "formato: who [-s] [-q] [-o]\r\n"
#define WHO_IMMORT "formato: who [livmin[-livmax]] [-n nome] [-c classlist] [-o] [-k] [-r] [-z] [-s] [-q]\r\n"

ACMD(do_who)
{
	extern const char* class_titles_male[][3];
	extern const char* class_titles_female[][3];
	
	struct descriptor_data *d;
	struct char_data *tch;
	char name_search[MAX_INPUT_LENGTH];
	char mode;
	char *s1 = NULL;
	size_t i;
	int low = 0, high = LVL_IMPL, localwho = 0, questwho = 0;
	int showclass = 0, short_list = 0, outlaws = 0, num_can_see = 0;
	int who_room = 0, ooc_list = 0;
	int noElements = 0;
	int curEl;
	int lung=0;
	char tmp_buf[50];
	int tempclass,templevel,tempsex, tempclanrank, tempclan, tempclasstitle;
	bzero(tmp_buf, sizeof(tmp_buf));
	skip_spaces(&argument);
	strcpy(buf, argument);
	
	s1= string(s1, buf, 0);
	name_search[0] = '\0';
	
	while (*buf)
	{
		half_chop(buf, arg, buf1);
		if (isdigit(*arg) && (GET_LEVEL(ch) >= LVL_IMMORT)) {
			sscanf(arg, "%d-%d", &low, &high);
			strcpy(buf, buf1);
		}
		else if (*arg == '-') {
			mode = *(arg + 1);	// just in case; we destroy arg in the switch
			switch (mode) {
				case 'o':
					ooc_list = 1;
					strcpy(buf, buf1);
					break;
				case 'k':
					if (GET_LEVEL(ch) >= LVL_IMMORT) {
						outlaws = 1;					
						strcpy(buf, buf1);						
						break;
					} 
					else 
					  send_to_char(WHO_FORMAT, ch);
					return;
				case 'z':
					if (GET_LEVEL(ch) >= LVL_IMMORT) { 
						localwho = 1;
						strcpy(buf, buf1);
						break;
					} 
					else
					  send_to_char(WHO_FORMAT, ch);
					return;
				case 's':
					short_list = 1;
					strcpy(buf, buf1);
					break;
				case 'q':
					questwho = 1;
					strcpy(buf, buf1);
					break;
					/*case 'l':
					if (GET_LEVEL(ch)>=LVL_IMMORT) {
						half_chop(buf1, arg, buf);
						sscanf(arg, "%d-%d", &low, &high);
					} else
						strcpy(buf, buf1);
					break;*/
				case 'n':
					if (GET_LEVEL(ch) >= LVL_IMMORT) {
						half_chop(buf1, name_search, buf);											
						break;
					} 
					else 
					  send_to_char(WHO_FORMAT, ch);
					return;
				case 'r':
					if (GET_LEVEL(ch)>=LVL_IMMORT) {
						who_room = 1;
						strcpy(buf, buf1);
						break;
					} 
					else 
					  send_to_char(WHO_FORMAT, ch);
					return;
				case 'c':
					if (GET_LEVEL(ch) >= LVL_IMMORT) {
						half_chop(buf1, arg, buf);
						for (i = 0; i < strlen(arg); i++)
							showclass |= find_class_bitvector(arg[i]);
						break;
					} 
					else 
					  send_to_char(WHO_FORMAT, ch);
					return;
				default:
					if (GET_LEVEL(ch) >= LVL_IMMORT) 
						send_to_char(WHO_IMMORT, ch);
					else
						send_to_char(WHO_FORMAT, ch);
					return;
					break;
			}             // end of switch
		}
		else {			// endif
			if (GET_LEVEL(ch) >= LVL_IMMORT) 
				send_to_char(WHO_IMMORT, ch);
			else
				send_to_char(WHO_FORMAT, ch);
			return;
		}
	}				// end while (parser)
	
  //  players and add them to the array
	for (noElements = 0, d = descriptor_list; d; d = d->next) {
		if (d->connected) 
		  continue;
		
		if (d->original)   
		  tch = d->original;
		else if(!(tch = d->character)) 
		  continue;
		
		theWhoList[noElements++] = tch;
	}
	
  // Sort it using the built in libc-quicksort routine
	qsort(theWhoList, noElements, sizeof(struct char_data  *), compareChars);
	
	//sprintf(buf, "Giocatori in Dalila World:\r\n");
	sprintf(buf, BANNER_1);
	s1 = string(s1, buf, 1);
	//sprintf(buf, "-------\r\n");
	//s1 = string(s1, buf, 1);
  
  // Print it
	for(curEl = 0; curEl < noElements; curEl++) {
		tch = theWhoList[curEl];
    // put normal printing routine in this for {}
    //if (*name_search && str_cmp(GET_NAME(tch), name_search) && !strstr(GET_TITLE(tch), name_search))
    //  continue;
		
		if (AFF_FLAGGED(tch, AFF_DISGUISE) && (tch->player.namedisguise)) {
			/*
			if (GET_LEVEL(tch) >= LVL_IMMORT) {
			  templevel = tch->player.livdisguise;
			  tempsex = tch->player.sexdisguise;
			  tempclass = tch->player.classdisguise;
			  tempclanrank = tch->player.clanrankdisguise;
			  tempclan = tch->player.clandisguise;
			  tempclasstitle = tch->player.classtitledisguise;
			}
			else */
			if (GET_LEVEL(ch) >= LVL_QUEST_MASTER) {
				templevel = GET_LEVEL(tch);
			  tempsex = GET_SEX(tch);
			  tempclass = GET_CLASS(tch);
			  tempclanrank = GET_CLAN_RANK(tch);
			  tempclan = GET_CLAN(tch);
			  tempclasstitle = GET_CLASS_TITLE(tch);
			}
			else
			  continue;
			    
		} 
		else {
			templevel = GET_LEVEL(tch);
			tempsex = GET_SEX(tch);
			tempclass = GET_CLASS(tch);
			tempclanrank = GET_CLAN_RANK(tch);
			tempclan = GET_CLAN(tch);
			tempclasstitle = GET_CLASS_TITLE(tch);
		}
		
		
		if (   *name_search 
		    && str_cmp(GET_NAME(tch), name_search) 
		    && !strstr(GET_TITLE(tch), name_search) 
		    && !AFF_FLAGGED(tch, AFF_DISGUISE) )
			continue;    
		
		if (   *name_search 
		    && !strstr(GET_TITLE(tch), name_search) 
		    && AFF_FLAGGED(tch, AFF_DISGUISE)
		    && (tch->player.namedisguise) 
		    && str_cmp(tch->player.namedisguise, name_search) )
			continue;
		
		if (GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
			continue;
		
		if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER) && !PLR_FLAGGED(tch, PLR_THIEF))
			continue;
		
		if (questwho && !PRF_FLAGGED(tch, PRF_QUEST))
			continue;
		
		if (localwho && world[ch->in_room].zone != world[tch->in_room].zone)
			continue;
		
		if (who_room && (tch->in_room != ch->in_room))
			continue;
		
		if (showclass && !(showclass & (1 << tempclass)))
			continue;
		
		if (ooc_list && PRF_FLAGGED(tch, PRF_NOC))
			continue;
		
		if (short_list && CAN_SEE(ch, tch)) {
			sprintf(buf, "%s", (templevel >= LVL_IMMORT ? CCYEL(ch, C_SPR) : ""));
			
			if (templevel <= (LVL_IMMORT - 1)) { 
				strcat(buf,"&0[");
								
				if (AFF_FLAGGED(tch, AFF_DISGUISE) && (tch->player.namedisguise))
				  strcat(buf, class_abbrevs[tch->player.classdisguise]);
				else
				  strcat(buf, CLASS_ABBR(tch));				
			}
			
			strcat(buf, (templevel >= LVL_IMMORT ? "* " : "]&0 "));
			
			
	
			if (AFF_FLAGGED(tch, AFF_DISGUISE) && (tch->player.namedisguise)) {
				strcat(buf, tch->player.namedisguise);
				CAP(buf);
			} 
			else
		    strcat(buf, GET_NAME(tch));
			
			strcat(buf, "\r\n");
      s1 = string(s1, buf, 1);
			//send_to_char(buf, ch);
	
		} //End ShortList
		else {
			sprintf(buf, "%s", (templevel >= LVL_IMMORT ? CCYEL(ch, C_SPR) : ""));
			if (!CAN_SEE(ch, tch)) {
				if (templevel < LVL_IMMORT) {
					strcat (buf, "&5[Invisibile]&0&3-----------&0&4&uQualcuno&0\r\n");
					s1 = string(s1, buf, 1);
					num_can_see++;
				}
				continue;
			}
			
			num_can_see++;
			
			if (templevel < LVL_IMMORT-1) { 
				strcat(buf,"["); 
				
				if (tempsex == SEX_FEMALE) {
					if (tempclasstitle == 0) 
					  strcat(buf, "&4");
					else if (tempclasstitle == 1) 
					  strcat(buf, "&8&3");
					else if (tempclasstitle == 2) 
					  strcat(buf,"&5");
					strcat(buf, class_titles_female[(int) tempclass][(int) tempclasstitle]);
					strcat(buf,"&0");
					lung = lung+strlen(class_titles_female[(int) tempclass][(int) tempclasstitle])+1;
				}
				else {
				  if (tempclasstitle == 0) 
				    strcat(buf,"&1");
					else if(tempclasstitle == 1) 
					  strcat(buf,"&8&6");
					else if(tempclasstitle == 2) 
					  strcat(buf,"&8&2"); 
					strcat(buf, class_titles_male[(int) tempclass][(int) tempclasstitle]);
					lung = lung+strlen(class_titles_male[(int) tempclass][(int) tempclasstitle])+1;
				  strcat(buf,"&0");
			  }   	 
			}
			
			else if (templevel < LVL_IMMORT) {
				if (tempsex==SEX_FEMALE)  {     
					if (tempclass==CLASS_MAGIC_USER){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bFata&0"); lung=lung+5; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bNinfa&0"); lung=lung+6; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bMegera&0"); lung=lung+7; }         
					}
					if (tempclass==CLASS_CLERIC){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bTaumaturga&0"); lung=lung+11; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bDivinatrice&0"); lung=lung+12; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bInquisitrice&0"); lung=lung+13; }
					}
					if (tempclass==CLASS_THIEF){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bSignora della notte&0"); lung=lung+20; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bVendicatrice&0"); lung=lung+13; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bAvvelenatrice&0"); lung=lung+14; }
					}
					if (tempclass==CLASS_WARRIOR){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bPaladina&0"); lung=lung+9; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bValkyria&0"); lung=lung+9; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bFuria&0"); lung=lung+6; }
					}
				
					if (tempclass==CLASS_PELOI){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bRosa dei venti&0"); lung=lung+15; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bMusa&0"); lung=lung+5; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bPredatrice&0"); lung=lung+11; }
					}			
			  } 
			  else {
					if (tempclass==CLASS_MAGIC_USER){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bOracolo&0"); lung=lung+8; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bMaestro di magia&0"); lung=lung+17; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bNegromante&0"); lung=lung+11; }         
					}
					if (tempclass==CLASS_CLERIC){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bProfeta&0"); lung=lung+8; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bDruido&0"); lung=lung+7; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bInquisitore&0"); lung=lung+12; }
					}
					if (tempclass==CLASS_THIEF){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bGiustiziere&0"); lung=lung+12; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bSicario&0"); lung=lung+8; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bCarnefice&0"); lung=lung+10; }
					}
				  if (tempclass==CLASS_WARRIOR){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bTemplare&0"); lung=lung+9; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bMaestro d'armi&0"); lung=lung+15; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bDevastatore&0"); lung=lung+12; }
					}
				
				  if (tempclass==CLASS_PELOI){
						if (IS_GOOD(tch)){                  strcat(buf, "[&7&bPatriota&0"); lung=lung+9; }
						else if (IS_NEUTRAL(tch)){          strcat(buf, "[&4&bRanger&0"); lung=lung+7; }
						else if (IS_EVIL(tch)){             strcat(buf, "[&1&bPredatore&0"); lung=lung+10; }
					
					}
				}
			}			
		 
			else if(templevel<LVL_AVATAR) {        strcat(buf, "[&8Immortale&0"); lung=lung+10; }
			else if(templevel<LVL_MINIBUILD)    {  strcat(buf, "[&4SemiDio&0"); lung=lung+8; }
			else if(templevel<LVL_QUEST_MASTER)    {  strcat(buf, "[&4Dio&0"); lung=lung+4; }
			else if(templevel<LVL_GOD) {           strcat(buf, "[&5Quest Master&0"); lung=lung+13; }
			else if(templevel<LVL_BUILDER) {       strcat(buf, "[&6Builder&0"); lung=lung+8; }
			else if(templevel<LVL_IMPLEMENTOR) {   strcat(buf, "[&3Build Master&0"); lung=lung+13; }
			else if(templevel<LVL_IMPL) {          strcat(buf, "[&6Implementor&0"); lung=lung+12; }		
			else { strcat(buf, "[&1Supervisor&0"); lung=lung+11; }
			
			strcat(buf, (templevel >= LVL_IMMORT ? CCYEL(ch, C_SPR) : ""));
			strcat(buf, "]&0");
			
			strcat(buf,"&3");
			while(lung<22) {
				strcat(buf,"-");
			  lung++;
			}
			strcat(buf,"&0");
			lung=0;
			
			if (AFF_FLAGGED(tch, AFF_DISGUISE) && (tch->player.namedisguise))
				strcat(buf, tch->player.namedisguise);
			else
				strcat(buf, GET_NAME(tch));
			strcat(buf, " ");
			strcat(buf, GET_TITLE(tch));
			
      //  Add the clan title to the title between []. Think about no allowing
      //  brakets in titles to prevent simulating clan titles
			
			if (   (clan_num = find_clan_by_id(tempclan)) >= 0 
			    && clan_num < num_of_clans ) {
				if(tempclanrank > 0)
					sprintf(buf, "%s &0[%s %s &0]", buf,
						clan[clan_num].rank_name[tempclanrank -1],
						frase_regno(clan_num,1));
				else
					sprintf(buf,"%s &0[&bApplying to %s %d&0]", buf, frase_regno(clan_num,0), clan_num);
			}
			
			if (GET_INVIS_LEV(tch))
				sprintf(buf, "%s (i%d)", buf, GET_INVIS_LEV(tch));
			else if (IS_AFFECTED(tch, AFF_INVISIBLE))
				strcat(buf, " (invis)");
			
			if (PLR_FLAGGED(tch, PLR_MAILING))
				strcat(buf, " (mailing)");
			else if (PLR_FLAGGED(tch, PLR_WRITING))
				strcat(buf, " (writing)");
			
			if (PRF_FLAGGED(tch, PRF_DEAF))
				strcat(buf, " (deaf)");
			if (PRF_FLAGGED(tch, PRF_NOTELL))
				strcat(buf, " (notell)");
			if (PRF_FLAGGED(tch, PRF_QUEST))
				strcat(buf, " (quest)");
			
			if (GET_LEVEL(ch) >= LVL_MINIBUILD) {
				if (PLR_FLAGGED(tch, PLR_THIEF))
					strcat(buf, " (&5THIEF&0)");
				if (PLR_FLAGGED(tch, PLR_KILLER))
					strcat(buf, " (&3KILLER&0)");
				if (PLR_FLAGGED(tch, PLR_TKILLER))
					strcat(buf, " (&7TKILLER&0)");
				if (PLR_FLAGGED(tch, PLR_WAR_KILLER))
					strcat(buf, " (&1WAR KILLER&0)");
				if (PLR_FLAGGED(tch, PLR_WANTED))
					strcat(buf, " (&4&bWANTED&0)");
				if (PLR_FLAGGED(tch, PLR_TRUSTED))
					strcat(buf, " (&1&uTRUST&0)");
				if (PLR_FLAGGED(tch,PLR_MULTIPLAYER))
					strcat(buf," (&8&1MP&0)");
				if (AFF_FLAGGED(tch, AFF_DISGUISE))
				  strcat(buf," (&7&bDisguised&0)");
			}
			if (PRF_FLAGGED(tch, PRF_AFK))
				strcat(buf, " (AFK)");
			if (!PRF_FLAGGED(tch, PRF_RPG))
				strcat(buf, " (NOGDR)");
			if (PLR_FLAGGED(tch,PLR_FANTASMA))
				strcat(buf, " &7(FANTASMA)&0");	
			if (templevel >= LVL_QUEST_MASTER)
				strcat(buf, CCNRM(ch, C_SPR));
			strcat(buf, "\r\n");
			s1 = string(s1, buf, 1);
		}
      // endif shortlist
	}				// end of for
	
	if (num_can_see == 0)
		sprintf(buf, "\r\nNessuno!\r\n");
	else if (num_can_see == 1)
		sprintf(buf, "\r\nUn solo giocatore presente.\r\n");
	else
		sprintf(buf, "\r\n%d giocatori presenti.\r\n", num_can_see);
	
	s1 = string(s1, buf, 1);
	sprintf(buf, "Numero massimo di giocatori connessi dall'ultimo reboot: %d\r\n", update_max_usage());
	s1 = string(s1, buf, 1);
	page_string(ch->desc, s1, 1);
	free(s1);	
}

#define USERS_FORMAT "format: users [-l minlevel[-maxlevel]] [-n name] [-h host] [-c classlist] [-o] [-p]\r\n"

ACMD(do_users)
{
	extern char *connected_types[];
	char line[200], line2[220], idletime[10], classname[20];
	char state[30], *timeptr, *format, mode;
	char name_search[MAX_INPUT_LENGTH], host_search[MAX_INPUT_LENGTH];
	struct char_data *tch;
	struct descriptor_data *d;
	size_t i;
	int low = 0, high = LVL_IMPL, num_can_see = 0;
	int showclass = 0, outlaws = 0, playing = 0, deadweight = 0;
	
	host_search[0] = name_search[0] = '\0';
	
	strcpy(buf, argument);
	while (*buf) {
		half_chop(buf, arg, buf1);
		
		if (*arg == '-') {
			mode = *(arg + 1);  /* just in case; we destroy arg in the switch */
			switch (mode) {
				case 'o':
				case 'k':
					outlaws = 1;
					playing = 1;
					strcpy(buf, buf1);
					break;
				case 'p':
					playing = 1;
					strcpy(buf, buf1);
					break;
				case 'd':
					deadweight = 1;
					strcpy(buf, buf1);
					break;
				case 'l':
					playing = 1;
					half_chop(buf1, arg, buf);
					sscanf(arg, "%d-%d", &low, &high);
					break;
				case 'n':
					playing = 1;
					half_chop(buf1, name_search, buf);
					break;
				case 'h':
					playing = 1;
					half_chop(buf1, host_search, buf);
					break;
				case 'c':
					playing = 1;
					half_chop(buf1, arg, buf);
					for (i = 0; i < strlen(arg); i++)
						showclass |= find_class_bitvector(arg[i]);
					break;
				default:
					send_to_char(USERS_FORMAT, ch);
					return;
					break;
			}				/* end of switch */
			
		} else {			/* endif */
			send_to_char(USERS_FORMAT, ch);
			return;
		}
	}				/* end while (parser) */
	strcpy(line,
		"\r\n Num Class           Name         State             Idl Login@   Site\r\n");
	strcat(line,
		" --- --------------- ------------ ----------------- --- -------- ------------------------\r\n");
	send_to_char(line, ch);
	
	one_argument(argument, arg);
	
	for (d = descriptor_list; d; d = d->next) {
		if (d->connected && playing)
			continue;
		if (!d->connected && deadweight)
			continue;
		if (!d->connected) {
			if (d->original)
				tch = d->original;
			else if (!(tch = d->character))
				continue;
			
			if (*host_search && !strstr(d->host, host_search))
				continue;
			if (*name_search && str_cmp(GET_NAME(tch), name_search))
				continue;
			if (!CAN_SEE(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
				continue;
			if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER) &&
				!PLR_FLAGGED(tch, PLR_THIEF))
				continue;
			if (showclass && !(showclass & (1 << GET_CLASS(tch))))
				continue;
			
			if (d->original)
				sprintf(classname, "[%2d %10s]", GET_LEVEL(d->original), CLASS_ABBR(d->original));
			else
				sprintf(classname, "[%2d %10s]", GET_LEVEL(d->character), CLASS_ABBR(d->character));
		} else
			strcpy(classname, "[-- ----------]");
		
		timeptr = asctime(localtime(&d->login_time));
		timeptr += 11;
		*(timeptr + 8) = '\0';
		
		if (!d->connected && d->original)
			strcpy(state, "Switched");
		else
			strcpy(state, connected_types[d->connected]);
		
		if (d->character && !d->connected && GET_LEVEL(d->character) < LVL_GOD)
			sprintf(idletime, "%3d", d->character->char_specials.timer *
				SECS_PER_MUD_HOUR / SECS_PER_REAL_MIN);
		else
			strcpy(idletime, "");
		
		format = "%4d %-15s %-12s %-17s %-3s %-8s ";
		
		if (d->character && d->character->player.name) {
			if (d->original)
				sprintf(line, format, d->desc_num, classname,
					d->original->player.name, state, idletime, timeptr);
			else
				sprintf(line, format, d->desc_num, classname,
					d->character->player.name, state, idletime, timeptr);
		} else
			sprintf(line, format, d->desc_num, "[-- ---------]", "UNDEFINED",
				state, idletime, timeptr);
		
		if (d->host && *d->host)
			sprintf(line + strlen(line), "[%12s]\r\n", d->host);
		else
			strcat(line, "[Host unknown]\r\n");
		
		if (d->connected) {
			sprintf(line2, "%s%s%s", CCGRN(ch, C_SPR), line, CCNRM(ch, C_SPR));
			strcpy(line, line2);
		}
		if (d->connected || (!d->connected && CAN_SEE(ch, d->character))) {
			send_to_char(line, ch);
			num_can_see++;
		}
	}
	
	sprintf(line, "\r\n%d visible sockets connected.\r\n", num_can_see);
	send_to_char(line, ch);
	
	sprintf(line, "Massimo di giocatori connessi dal Reboot: %d\r\n", update_max_usage());
	send_to_char(line, ch);
}


/* Generic page_string function for displaying text */
ACMD(do_gen_ps)
{
	extern char circlemud_version[];
	
	switch (subcmd) {
		case SCMD_CREDITS:
			page_string(ch->desc, credits, 0);
			break;
		case SCMD_NEWS:
			page_string(ch->desc, news, 0);
			break;
		case SCMD_INFO:
			page_string(ch->desc, info, 0);
			break;
		case SCMD_WIZLIST:
			page_string(ch->desc, wizlist, 0);
			break;
		case SCMD_IMMLIST:
			page_string(ch->desc, immlist, 0);
			break;
		case SCMD_READPRAY:
			page_string(ch->desc, pray, 0);
			break;
		case SCMD_HANDBOOK:
			page_string(ch->desc, handbook, 0);
			break;
		case SCMD_POLICIES:
			page_string(ch->desc, policies, 0);
			break;
		case SCMD_MOTD:
			page_string(ch->desc, motd, 0);
			break;
		case SCMD_IMOTD:
			page_string(ch->desc, imotd, 0);
			break;
		case SCMD_CLEAR:
			send_to_char("\033[H\033[J", ch);
			break;
		case SCMD_VERSION:
			send_to_char(circlemud_version, ch);
			send_to_char(strcat(strcpy(buf, DG_SCRIPT_VERSION), "\r\n"), ch);	//Orione
			break;
		case SCMD_WHOAMI:
			send_to_char(strcat(strcpy(buf, GET_NAME(ch)), "\r\n"), ch);
			break;
		case SCMD_REGNI:
			page_string(ch->desc, regni, 0);
			break;
			
		default:
			return;
			break;
	}
}

void perform_mortal_where(struct char_data * ch, char *arg)
{
	register struct char_data *i;
	register struct descriptor_data *d;
	
	if (!*arg) {
		send_to_char("Giocatori nella tua zona\r\n--------------------\r\n", ch);
		for (d = descriptor_list; d; d = d->next)
			if (!d->connected) {
				i = (d->original ? d->original : d->character);
				if (i && CAN_SEE(ch, i) &&(!AFF_FLAGGED(i,AFF_DISGUISE))&& (!ROOM_FLAGGED(i->in_room, ROOM_HOUSE)) && (i->in_room != NOWHERE) &&
					(world[ch->in_room].zone == world[i->in_room].zone)) {
					sprintf(buf, "%-20s - %s\r\n", PERS(i, ch), world[i->in_room].name);
					send_to_char(buf, ch);
				}
			}
	} else {			/* print only FIRST char, not all. */
		for (i = character_list; i; i = i->next)
			if (world[i->in_room].zone == world[ch->in_room].zone && CAN_SEE(ch, i) && (!ROOM_FLAGGED(i->in_room, ROOM_HOUSE)) &&
				(i->in_room != NOWHERE) && isname(arg, i->player.name)&&(!AFF_FLAGGED(i,AFF_DISGUISE))) {
				sprintf(buf, "%-25s - %s\r\n", PERS(i, ch), world[i->in_room].name);
				send_to_char(buf, ch);
				return;
			}
		send_to_char("Nessuno in giro con quel nome.\r\n", ch);
	}
}

char* print_object_location(int num, struct obj_data * obj, struct char_data * ch, int recur)
{
	
	char *s1=NULL;
	
	s1=string(s1,buf,0);
	if (num > 0)
		sprintf(buf, "O%3d. %-25s  - ", num, obj->short_description);
	else
		sprintf(buf, "%33s", " - ");
	
	if (obj->in_room > NOWHERE) {
		sprintf(buf + strlen(buf), "[%5d] %s\r\n",world[obj->in_room].number
			, world[obj->in_room].name);
/*send_to_char(buf, ch);*/
		s1=string(s1,buf,1);
	} else if (obj->carried_by) {
		sprintf(buf + strlen(buf), "carried by %s\r\n",
			PERS(obj->carried_by, ch));
/* send_to_char(buf, ch);*/
		s1=string(s1,buf,1);
	} else if (obj->worn_by) {
		sprintf(buf + strlen(buf), "worn by %s\r\n",
			PERS(obj->worn_by, ch));
/* send_to_char(buf, ch);*/
		s1=string(s1,buf,1);
	} else if (obj->in_obj) {
		sprintf(buf + strlen(buf), "inside %s%s\r\n",
			obj->in_obj->short_description, (recur ? ", which is" : " "));
/*send_to_char(buf, ch);*/
		s1=string(s1,buf,1);
		if (recur)
			s1 = string(s1,print_object_location(0, obj->in_obj, ch, recur),1);
	} else {
		sprintf(buf + strlen(buf), "in an unknown location\r\n");
/*send_to_char(buf, ch);*/
		s1=string(s1,buf,1);
	}
	return s1;
}

void perform_immort_where(struct char_data * ch, char *arg){
	register struct char_data *i;
	register struct obj_data *k;
	struct descriptor_data *d;
	int num = 0, found = 0, overflow = 0;
	char *s1=NULL,*s2=NULL;
	s1=string(s1,arg,0);
	if (!*arg) {
		s1=string(s1,"Giocatori\r\n-------\r\n",1);
		for (d = descriptor_list; (d)&&(overflow<1000); d = d->next)
			if (!d->connected) {
				i = (d->original ? d->original : d->character);
				if (i && CAN_SEE(ch, i) && (i->in_room != NOWHERE)) {
					if (d->original)
						sprintf(buf, "%-20s - [%5d] %s (in %s)\r\n",
							GET_NAME(i), world[d->character->in_room].number,
							world[d->character->in_room].name, GET_NAME(d->character));
					else
						sprintf(buf, "%-20s - [%5d] %s\r\n", GET_NAME(i),
							world[i->in_room].number, world[i->in_room].name);
					
					s1=string(s1,buf, 1);
					overflow++;
					//page_string(ch->desc,s1,1);
				}
			}
		page_string(ch->desc,s1,1);
	} else {
		for (i = character_list; (i)&&(overflow<1000); i = i->next)
			if (CAN_SEE(ch, i) && i->in_room != NOWHERE && isname(arg, i->player.name))
			{
				found = 1;
				sprintf(buf, "M%3d. %-25s - [%5d] %s\r\n", ++num, GET_NAME(i),
					world[i->in_room].number, world[i->in_room].name);
				//send_to_char(buf,ch);
				s1=string(s1,buf,1);
				overflow++;
			}
			
			
		for (num = 0, k = object_list; (k)&&(overflow<1000); k = k->next)
			if (CAN_SEE_OBJ(ch, k) && isname(arg, k->name) &&
				(!k->carried_by || CAN_SEE(ch, k->carried_by))) {
				found = 1;
				
				s2=print_object_location(++num, k, ch, TRUE);
				s1=string(s1,s2,1);
				overflow++;
			}
		if (overflow >= 1000)
			s1=string(s1,"&1**OVERFLOW**&0\r\n",1);
		page_string(ch->desc,s1,1);
		free(s1);
		free(s2);
		if (!found)
			send_to_char("Impossibile trovarlo.\r\n", ch);
	}
}


ACMD(do_attributes)
{
	sprintf(buf, "\r\n            Attributi per %s\r\n", GET_NAME(ch));
	
	
	if ( ((GET_CLASS(ch) == CLASS_WARRIOR)||(GET_CLASS(ch) == CLASS_PELOI)) && ( GET_STR(ch) == 18) )
		sprintf(buf, "%sForza: %d/%3d  Intelligenza: %d  Saggezza: %d\r\nDestrezza: %d  Costituzione: %d  Carisma : %d\r\n", buf,
			GET_STR(ch), GET_ADD(ch), GET_INT(ch), GET_WIS(ch),
			GET_DEX(ch), GET_CON(ch), GET_CHA(ch));
	else
		sprintf(buf, "%sForza: %d  Intelligenza: %d  Saggezza: %d\r\nDestrezza: %d  Costituzione: %d  Carisma : %d\r\n", buf,
			GET_STR(ch), GET_INT(ch), GET_WIS(ch), GET_DEX(ch),
			GET_CON(ch), GET_CHA(ch));
	
	sprintf(buf, "%sHitroll: %d   Damroll: %d\r\n", buf, (affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(ch)), (ch)->points.damroll);
	
	send_to_char(buf, ch);
}




ACMD(do_where)
{
	one_argument(argument, arg);
	
	if (GET_LEVEL(ch) >= LVL_IMMORT)
		perform_immort_where(ch, arg);
	else
	  send_to_char("\r\n&bChiudi gli occhi, ti concentri ma non riesci a vedere nessuno.&0\r\n", ch);
		//perform_mortal_where(ch, arg);
}


// PEPPE RELIGIONE
ACMD(do_resuscita)
{
    if(PLR_FLAGGED(ch,PLR_FANTASMA)){
		if(ROOM_FLAGGED(ch->in_room,ROOM_TEMPIO)){
			if(GET_RESUSCITA(ch)==0){
				GET_RESUSCITA(ch)= (2 + (GET_LEVEL(ch)/7));
				send_to_char("La tua richiesta di resuscitare e' stata accettata...dopo i soliti riti riavrai il tuo corpo,\r\nstai piu' attento la prossima volta.\r\n",ch);
				return;
			}
			else {
				send_to_char("Quante volte vuoi chiedere di essere resuscitato?!?!?!?\r\n",ch);
				return;
			}
		}
		else{ 
	    send_to_char("Devi essere in un tempio o in un santuario per chiedere di resuscitare\r\n",ch);
	    return;
	    }
    }
    else 
	send_to_char("Non puoi chiedere di resuscitare se non sei morto!!!\r\n",ch);
    return;
}    
    
ACMD(do_levels)
{
	int i;
	
	if (IS_NPC(ch)) {
		send_to_char("You ain't nothin' but a hound-dog.\r\n", ch);
		return;
	}
	*buf = '\0';
	
	for (i = 1; i < LVL_IMMORT; i++) {
		sprintf(buf + strlen(buf), "[%2d] %8d-%-8d : ", i,
			titles[(int) GET_CLASS(ch)][i].exp, titles[(int) GET_CLASS(ch)][i + 1].exp);
		switch (GET_SEX(ch)) {
			case SEX_MALE:
			case SEX_NEUTRAL:
				strcat(buf, titles[(int) GET_CLASS(ch)][i].title_m);
				break;
			case SEX_FEMALE:
				strcat(buf, titles[(int) GET_CLASS(ch)][i].title_f);
				break;
			default:
				send_to_char("Oh caro. Sembri essere senza sesso!\r\n", ch);
				break;
		}
		strcat(buf, "\r\n");
	}
	send_to_char(buf, ch);
}



ACMD(do_consider)
{
	struct char_data *victim;
	int diff, hitroll_ch, hitroll_vict;
	struct obj_data *wielded; /* Orione: var per le armi impugante*/

	one_argument(argument, buf);
	
	if (!(victim = get_char_room_vis(ch, buf)))
	{
		send_to_char("Chi vuoi uccidere?\r\n", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char("Facile. Veramente!\r\n", ch);
		return;
	}
	if(PLR_FLAGGED(victim,PLR_FANTASMA) && !IS_NPC(victim)){
	    send_to_char("Prova a colpirlo se ti riesce...e' un fantasma.\r\n",ch);
	    return;
	}
	if(!PRF_FLAGGED(victim,PRF_RPG) && !IS_NPC(victim)){
	    send_to_char("Non sembra stia giocando al tuo stesso gioco.\r\n",ch);
	    return;
	}
	hitroll_ch = (affected_by_spell(ch, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(ch));
	hitroll_vict = (affected_by_spell(victim, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(victim));
	diff = ((GET_LEVEL(victim) - GET_LEVEL(ch)) + ((GET_AC(ch) - (GET_AC(victim))) / 10) + (((hitroll_vict + GET_DAMROLL(victim)) / 2) - (hitroll_ch + GET_DAMROLL(ch))));
	
	/* Orione: modifica dovuta alle armi impugnate, o al barehand nel caso di mob sprovvisti di arma*/
	wielded = GET_EQ(ch, WEAR_WIELD);
	if (wielded && GET_OBJ_TYPE(wielded) == ITEM_WEAPON)//calcolo del danno medio, se e' impugnata un'arma
		diff -= (((GET_OBJ_VAL(wielded, 2) + 1) / 2.0) * GET_OBJ_VAL(wielded, 1));
	else if (IS_NPC(ch)) //calcolo del barehand se ch e' mob
		diff -= (((ch->mob_specials.damnodice + 1) / 2.0) * ch->mob_specials.damsizedice);
	if (!IS_NPC(ch)) { // solo i pg possono impugnare 2 armi
		wielded = GET_EQ(ch, WEAR_WIELD_L);
		if (wielded && GET_OBJ_TYPE(wielded) == ITEM_WEAPON) //calcolo del danno medio se e' impugnata la SECONDA arma
			diff -= (((GET_OBJ_VAL(wielded, 2) + 1) / 2.0) * GET_OBJ_VAL(wielded, 1));
	}
		
	wielded = GET_EQ(victim, WEAR_WIELD);
	if (wielded && GET_OBJ_TYPE(wielded) == ITEM_WEAPON)//calcolo del danno medio, se e' impugnata un'arma
		diff += (((GET_OBJ_VAL(wielded, 2) + 1) / 2.0) * GET_OBJ_VAL(wielded, 1));
	else if (IS_NPC(victim)) //calcolo del barehand se victim e' mob
		diff += (((victim->mob_specials.damnodice + 1) / 2.0) * victim->mob_specials.damsizedice);
	if (!IS_NPC(victim)) { // solo i pg possono impugnare 2 armi
		wielded = GET_EQ(victim, WEAR_WIELD_L);
		if (wielded && GET_OBJ_TYPE(wielded) == ITEM_WEAPON) //calcolo del danno medio se e' impugnata la SECONDA arma
			diff += (((GET_OBJ_VAL(wielded, 2) + 1) / 2.0) * GET_OBJ_VAL(wielded, 1));
	}
	
	/* Orione: modifica dovuta agli spell attivi: sanctuary,mirroriamge,fireshield,deathdance*/
   if (IS_AFFECTED(ch, AFF_SANCTUARY) || IS_AFFECTED(ch, AFF_MIRRORIMAGE) || IS_AFFECTED(ch, AFF_FIRESHD) || IS_AFFECTED(ch, AFF_DEATHDANCE))
   	diff -= 5;
   if (IS_AFFECTED(victim, AFF_SANCTUARY) || IS_AFFECTED(victim, AFF_MIRRORIMAGE) || IS_AFFECTED(victim, AFF_FIRESHD) || IS_AFFECTED(victim, AFF_DEATHDANCE))
   	diff += 5;
		
	if (diff <= -19)
		send_to_char("Sembri un Dio al suo confronto!!\r\n", ch);
	else if (diff <= -16)
		send_to_char("Probabilmente lo ucciderai in pochi attacchi.\r\n", ch);
	else if (diff <= -14)
		send_to_char("Sembra tremare al solo vederti.\r\n", ch);
	else if (diff <= -12)
		send_to_char("Sembra un avversario molto debole.\r\n", ch);
	else if (diff <= -10)
		send_to_char("Potresti ucciderlo facilmente.\r\n", ch);
        else if (diff <= -8)
		send_to_char("Sembri in grado di annientarlo.\r\n", ch);
	else if (diff <= -6)
		send_to_char("Al tuo confronto sembra un novizio.\r\n", ch);
        else if (diff <= -3)
		send_to_char("Sembra poco meno forte di te.\r\n", ch);
	else if (diff <= 2)
		send_to_char("Potresti ucciderlo oppure morire, scegli tu.\r\n", ch);
	else if (diff <= 5)
		send_to_char("Sembra sia appena poco piu' forte di te.\r\n", ch);
	else if (diff <= 7)
		send_to_char("Al tuo confronto sembra un veterano.\r\n", ch);
	else if (diff <= 9)
		send_to_char("Sembra in grado di farti del male.\r\n", ch);
	else if (diff <= 11)
		send_to_char("Potrebbe ucciderti se e' il suo giorno fortunato.\r\n", ch);
	else if (diff <= 13)
		send_to_char("Sembra un avversario molto forte.\r\n", ch);
        else if (diff <= 15)
		send_to_char("Sembri tremare al solo vederlo!!\r\n", ch);
	else if (diff <= 18)
		send_to_char("Probabilmente ti uccidera' in pochi attacchi.\r\n", ch);
        else
		send_to_char("Hai fatto testamento??\r\n", ch);
}




ACMD(do_diagnose)
{
	struct char_data *vict;
	
	one_argument(argument, buf);
	
	if (*buf) {
		if (!(vict = get_char_room_vis(ch, buf))) {
			send_to_char(NOPERSON, ch);
			return;
		} else
			diag_char_to_char(vict, ch);
	} else {
		if (FIGHTING(ch))
			diag_char_to_char(FIGHTING(ch), ch);
		else
			send_to_char("Diagnose chi?\r\n", ch);
	}
}


const char *ctypes[] = {
	"off", "sparse", "normal", "complete", "\n"};

ACMD(do_color)
{
	int tp;
	
	//if (IS_NPC(ch))
	if (!ch->desc)
		return;
	
	one_argument(argument, arg);
	
	if (!*arg) {
		sprintf(buf, "Your current color level is %s.\r\n", ctypes[COLOR_LEV(ch)]);
		send_to_char(buf, ch);
		return;
	}
	if (((tp = search_block(arg, ctypes, FALSE)) == -1)) {
		send_to_char("Usage: color { Off | Sparse | Normal | Complete }\r\n", ch);
		return;
	}
	REMOVE_BIT(PRF_FLAGS(ch), PRF_COLOR_1 | PRF_COLOR_2);
	SET_BIT(PRF_FLAGS(ch), (PRF_COLOR_1 * (tp & 1)) | (PRF_COLOR_2 * (tp & 2) >> 1));
	
	sprintf(buf, "Ora il tuo %scolor%s e' %s.\r\n", CCRED(ch, C_SPR),
		CCNRM(ch, C_OFF), ctypes[tp]);
	send_to_char(buf, ch);
}


ACMD(do_toggle)
{
	//if (IS_NPC(ch))
	if (!ch->desc)
		return;
	if (GET_WIMP_LEV(ch) == 0)
		strcpy(buf2, "OFF");
	else
		sprintf(buf2, "%-3d", GET_WIMP_LEV(ch));
	
	sprintf(buf,
		"Hit Pnt Display: %-3s    "
		"     Brief Mode: %-3s    "
		" Summon Protect: %-3s\r\n"
		
		"   Move Display: %-3s    "
		"   Compact Mode: %-3s    "
		"       On Quest: %-3s\r\n"
		
		"   Mana Display: %-3s    "
		"         NoTell: %-3s    "
		"   Repeat Comm.: %-3s\r\n"
		
		" Auto Show Exit: %-3s    "
		"           Deaf: %-3s    "
		"     Wimp Level: %-3s\r\n"
		
		" Gossip Channel: %-3s    "
		"Auction Channel: %-3s    "
		"  Grats Channel: %-3s\r\n"
		
		"   Clan Channel: %-3s    "
		"    OOC Channel: %-3s    "
		"    Color Level: %s",
		
		ONOFF(PRF_FLAGGED(ch, PRF_DISPHP)),
		ONOFF(PRF_FLAGGED(ch, PRF_BRIEF)),
		ONOFF(!PRF_FLAGGED(ch, PRF_SUMMONABLE)),
		
		ONOFF(PRF_FLAGGED(ch, PRF_DISPMOVE)),
		ONOFF(PRF_FLAGGED(ch, PRF_COMPACT)),
		YESNO(PRF_FLAGGED(ch, PRF_QUEST)),
		
		ONOFF(PRF_FLAGGED(ch, PRF_DISPMANA)),
		ONOFF(PRF_FLAGGED(ch, PRF_NOTELL)),
		YESNO(!PRF_FLAGGED(ch, PRF_NOREPEAT)),
		
		ONOFF(PRF_FLAGGED(ch, PRF_AUTOEXIT)),
		YESNO(PRF_FLAGGED(ch, PRF_DEAF)),
		buf2,
		
		ONOFF(!PRF_FLAGGED(ch, PRF_NOGOSS)),
		ONOFF(!PRF_FLAGGED(ch, PRF_NOAUCT)),
		ONOFF(!PRF_FLAGGED(ch, PRF_NOGRATZ)),
		
		ONOFF(!PRF_FLAGGED(ch, PRF_REGNOTELL)),
		ONOFF(!PRF_FLAGGED(ch, PRF_NOC)),
		ctypes[COLOR_LEV(ch)]);
	
	send_to_char(buf, ch);
}


struct sort_struct {
	int sort_pos;
	byte is_social;
} *cmd_sort_info = NULL;

int num_of_cmds;


void sort_commands(void)
{
	int a, b, tmp;
	
	ACMD(do_action);
	
	num_of_cmds = 0;
	
  /*
   * first, count commands (num_of_commands is actually one greater than the
   * number of commands; it inclues the '\n'.
   */
	while (*cmd_info[num_of_cmds].command != '\n')
		num_of_cmds++;
	
  /* create data array */
	CREATE(cmd_sort_info, struct sort_struct, num_of_cmds);
	
  /* initialize it */
	for (a = 1; a < num_of_cmds; a++) {
		cmd_sort_info[a].sort_pos = a;
		cmd_sort_info[a].is_social = (cmd_info[a].command_pointer == do_action);
	}
	
  /* the infernal special case */
	cmd_sort_info[find_command("insult")].is_social = TRUE;
	
  /* Sort.  'a' starts at 1, not 0, to remove 'RESERVED' */
	for (a = 1; a < num_of_cmds - 1; a++)
		for (b = a + 1; b < num_of_cmds; b++)
			if (strcmp(cmd_info[cmd_sort_info[a].sort_pos].command,
					cmd_info[cmd_sort_info[b].sort_pos].command) > 0) {
				tmp = cmd_sort_info[a].sort_pos;
				cmd_sort_info[a].sort_pos = cmd_sort_info[b].sort_pos;
				cmd_sort_info[b].sort_pos = tmp;
			}
}



ACMD(do_commands)
{
	int no, i, cmd_num;
	int wizhelp = 0, socials = 0;
	struct char_data *vict;
	
	one_argument(argument, arg);
	
	if (*arg) {
		if (!(vict = get_char_vis(ch, arg)) || IS_NPC(vict)) {
			send_to_char("Chi e' quello?\r\n", ch);
			return;
		}
		if (GET_LEVEL(ch) < GET_LEVEL(vict)) {
			send_to_char("Non puoi vedere i comandi di persone di livello piu' alto.\r\n", ch);
			return;
		}
	} else
		vict = ch;
	
	if (subcmd == SCMD_SOCIALS)
		socials = 1;
	else if (subcmd == SCMD_WIZHELP)
		wizhelp = 1;
	
	sprintf(buf, "I seguenti %s%s sono disponibili a %s:\r\n",
		wizhelp ? "privileged " : "",
		socials ? "socials" : "commands",
		vict == ch ? "te" : GET_NAME(vict));
	
  /* cmd_num starts at 1, not 0, to remove 'RESERVED' */
	for (no = 1, cmd_num = 1; cmd_num < num_of_cmds; cmd_num++) {
		i = cmd_sort_info[cmd_num].sort_pos;
		if (cmd_info[i].minimum_level >= 0 &&
			GET_LEVEL(vict) >= cmd_info[i].minimum_level &&
			(cmd_info[i].minimum_level >= LVL_IMMORT) == wizhelp &&
			(wizhelp || socials == cmd_sort_info[i].is_social)) {
			sprintf(buf + strlen(buf), "%-11s", cmd_info[i].command);
			if (!(no % 7))
				strcat(buf, "\r\n");
			no++;
		}
	}
	
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
}

int list_scanned_chars(struct char_data * list, struct char_data * ch, int distance, int door, bool nobody)
{
	const char *how_far[] = {
		"vicino verso",
		"piu' in la'  verso",
		"in lontananza verso"
	};
	
  /*  const char *dirs1[] =
{
  "nord",
  "est",
  "sud",
  "ovest",
  "alto",
  "basso",
  "\n"
};  */
	
	struct char_data *i;
	int count = 0;
	int risposta;
	*buf = '\0';
	
/* this loop is a quick, easy way to help make a grammatical sentence
   (i.e., "You see x, x, y, and z." with commas, "and", etc.) */
	
	for (i = list; i; i = i->next_in_room)
		
/* put any other conditions for scanning someone in this if statement -
   i.e., if (CAN_SEE(ch, i) && condition2 && condition3) or whatever */
		
		if (CAN_SEE(ch, i))
			count++;
		
	if (!count) {
		if (nobody) {
			printf(buf, "Nessuno in vista verso %s\r\n", dirs1[door]);
			send_to_char(buf, ch);
		}
		return count;
	}
	
	risposta = count;
	
	for (i = list; i; i = i->next_in_room) {
		
   /* make sure to add changes to the if statement above to this one also, using
   or's to join them.. i.e.,
   if (!CAN_SEE(ch, i) || !condition2 || !condition3) */
		
		if (!CAN_SEE(ch, i))
			continue;
		
		if (AFF_FLAGGED(i, AFF_DISGUISE) && (i->player.namedisguise)) {
			sprintf(buf1, "%s", i->player.namedisguise);
	  }
	  else {
	   sprintf(buf1, "%s", GET_NAME(i));
			
	  }
	  
	  CAP(buf1); 
		
		if (!*buf)
			sprintf(buf, "Vedi %s", buf1);
		else
			strcat(buf, buf1);
			
		if (--count > 1)
			strcat(buf, ", ");
		else if (count == 1)
			strcat(buf, " e ");
		else
			sprintf(buf+strlen(buf), " %s %s.\r\n", how_far[distance], dirs1[door]);
	}
	send_to_char(buf, ch);
	return risposta;
}

/* utils.h: #define EXIT(ch, door)  (world[(ch)->in_room].dir_option[door]) */
#define _2ND_EXIT(ch, door) (world[EXIT(ch, door)->to_room].dir_option[door])
#define _3RD_EXIT(ch, door) (world[_2ND_EXIT(ch, door)->to_room].dir_option[door])


ACMD(do_scan)
{
  /* >scan
     You quickly scan the area.
     You see John, a large horse and Frank close by north.
     You see a small rabbit a ways off south.
     You see a huge dragon and a griffon far off to the west.

  */
	int door, count;
	
	*buf = '\0';
	
	if (!CAN_LEARN_SKILL(ch, SKILL_SCAN) || GET_SKILL(ch, SKILL_SCAN) <= 0) {
		send_to_char("Non conosci nulla della skill scan.\r\n",ch);
		return;
	}
	
	if (IS_AFFECTED(ch, AFF_BLIND)) {
		send_to_char("Non vedi niente, sei cieco!\r\n", ch);
		return;
	}
  /* may want to add more restrictions here, too */
	
	if (test_and_improve(ch, SKILL_SCAN, 100, 15, 0))
	{
		send_to_char("Rapidamente controlli chi c'e' nelle vicinanze.\r\n", ch);
		send_to_char("&1--------------------------------------------------------&0\r\n",ch);
		for (door = 0; door < NUM_OF_DIRS ; door++)
			if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
				!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
			{
				send_to_char("&2",ch);
				count=list_scanned_chars(world[EXIT(ch, door)->to_room].people, ch, 0, door, FALSE);
				if (_2ND_EXIT(ch, door) && _2ND_EXIT(ch, door)->to_room != NOWHERE &&
					!IS_SET(_2ND_EXIT(ch, door)->exit_info, EX_CLOSED))
				{
                /* check the second room away */
					count += list_scanned_chars(world[_2ND_EXIT(ch, door)->to_room].people, ch, 1, door, FALSE);
					
					if (_3RD_EXIT(ch, door) && _3RD_EXIT(ch, door)->to_room != NOWHERE &&
						!IS_SET(_3RD_EXIT(ch, door)->exit_info, EX_CLOSED))
                      /* check the third room */
						count += list_scanned_chars(world[_3RD_EXIT(ch, door)->to_room].people, ch, 2, door, FALSE);
				}
				
				if (!count)
				{
					strcpy(buf, "Nessuno in vista verso ");
					if (door==4) strcat(buf, "l'");
					if (door==5) strcat(buf, "il ");
					sprintf(buf+strlen(buf), "%s.\r\n", dirs1[door]);
					send_to_char(buf, ch);
				}
				
				send_to_char("\r\n&1", ch);
				send_to_char(EXIT(ch, door)->general_description, ch);
				send_to_char("&0", ch);
				send_to_char(world[EXIT(ch, door)->to_room].description, ch);
				send_to_char("&1--------------------------------------------------------&0\r\n",ch);
			}
	}
	else send_to_char("Malgrado la tua concentrazione fallisci nel compito.\r\n",ch);
	
}

ACMD(do_spy)
{
	
	int door=0, count;
	
	*buf = '\0';
	one_argument(argument, arg);
	if (!*arg) {
		send_to_char("Spiare dove?\r\n", ch);
		return;
	}
	door=parse_dir(arg);
	if (door==-1) {
		send_to_char("Spiare dove?\r\n", ch);
		return;
	}
	
	if (!CAN_LEARN_SKILL(ch, SKILL_SPY) || GET_SKILL(ch, SKILL_SPY) <= 0) {
		send_to_char("Non conosci niente della skill spy.\r\n",ch);
		return;
	}	 
	
	if (IS_AFFECTED(ch, AFF_BLIND)) {
		send_to_char("Non vedi niente, sei cieco!\r\n", ch);
		return;
	}
  /* may want to add more restrictions here, too */
	
	if (test_and_improve(ch, SKILL_SPY, 100, 15, 0))
	{
		if(door<4) sprintf(buf, "Guardi piu' attentamente a %s.\r\n", dirs1[door]);
		else sprintf(buf, "Guardi piu' attentamente in %s.\r\n", dirs1[door]);
		send_to_char(buf, ch);
		send_to_char("&1--------------------------------------------------------&0\r\n",ch);
		
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
			!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
		{
			send_to_char("&2", ch);
			count=list_scanned_chars(world[EXIT(ch, door)->to_room].people, ch, 0, door, FALSE);
			if (_2ND_EXIT(ch, door) && _2ND_EXIT(ch, door)->to_room != NOWHERE &&
				!IS_SET(_2ND_EXIT(ch, door)->exit_info, EX_CLOSED))
			{
        /* check the second room away */
				count += list_scanned_chars(world[_2ND_EXIT(ch, door)->to_room].people, ch, 1, door, FALSE);
				if (_3RD_EXIT(ch, door) && _3RD_EXIT(ch, door)->to_room != NOWHERE &&
					!IS_SET(_3RD_EXIT(ch, door)->exit_info, EX_CLOSED))
          /* check the third room */
					count += list_scanned_chars(world[_3RD_EXIT(ch, door)->to_room].people, ch, 2, door, FALSE);
			}
			
			if (!count)
			{
				strcpy(buf, "Nessuno in vista verso ");
				if (door==4) strcat(buf, "l'");
				if (door==5) strcat(buf, "il ");
				sprintf(buf+strlen(buf), "%s.\r\n", dirs1[door]);
				send_to_char(buf, ch);
			}
			
			send_to_char("\r\n&1", ch);
			send_to_char(EXIT(ch, door)->general_description, ch);
			send_to_char("&0", ch);
			send_to_char(world[EXIT(ch, door)->to_room].description, ch);
			send_to_char("&1--------------------------------------------------------&0\r\n",ch);
		}
		else // Non esiste la direzione, oppure non e' collegata a una stanza, oppure la porta e' chiusa
		{
			strcpy(buf, "Non puoi spiare niente verso ");
			if (door==4) strcat(buf, "l'");
			if (door==5) strcat(buf, "il ");
			sprintf(buf+strlen(buf), "%s.\r\n", dirs1[door]);
			send_to_char(buf, ch);
		}
	}
	else send_to_char("Malgrado la tua concentrazione fallisci nel compito.\r\n",ch);
}

void do_list_groups(struct char_data *ch)
{
	struct descriptor_data *i;
	struct char_data *person;
	struct follow_type *f;
	int count = 0;
	char buf[MAX_STRING_LENGTH*2],tbuf[MAX_STRING_LENGTH];
	
	sprintf(buf,"&3[------ Gruppi di avventurieri ------]\n\r&0");
	
  /* go through the descriptor list */
	for (i = descriptor_list;i;i=i->next)
	{
  /* find everyone who is a master  */
		if (!i->connected)
		{
			person = (i->original ? i->original:i->character);
		/* list the master */
			if (!person->master && IS_AFFECTED(person, AFF_GROUP))
			{
				sprintf(tbuf, "\n\r&6&b- %s -\n\r&0",GROUPNAME(person) ? GROUPNAME(person): "-");
				strcat(buf,tbuf);
				if (CAN_SEE(ch, person)) {
					sprintf(tbuf, "&4%s\n\r&0",fname(GET_NAME(person)));
					strcat(buf,tbuf);						
				} 				
	  		/* list the members that ch can see */
				count = 0;
				for(f=person->followers; f; f=f->next)
				{
					if (IS_AFFECTED(f->follower, AFF_GROUP) && !IS_NPC(f->follower)) {
						count++;
						if (CAN_SEE(ch, f->follower) && strlen(GET_NAME(f->follower))>1 )
						{
							sprintf(tbuf,"&3%s\n\r&0", fname(GET_NAME(f->follower)));
							strcat(buf,tbuf);
						} else {
							sprintf(tbuf,"&4Qualcuno\n\r");
							strcat(buf,tbuf);
						}
					}
					
					
				}
			}
		}
	}
	strcat(buf,"\n\r&3[---------- Fine Lista --------------]\n\r&0");
	page_string(ch->desc,buf,1);
}

int objlevel (struct obj_data * obj){
	int punti=-6;
	int medium_damage;
	int i;
	
	switch (GET_OBJ_TYPE(obj))
	{
		case ITEM_WEAPON:
			medium_damage = (GET_OBJ_VAL(obj,1) * (GET_OBJ_VAL(obj,2)+1) ) /2;
			punti += medium_damage*3;
			break;
		case ITEM_ARMOR:
			punti += 35*(GET_OBJ_VAL(obj,0)) / 10;
			break;
		case ITEM_BOAT:
		case ITEM_LIGHT:
			if (GET_OBJ_VAL(obj,2)==-1) punti += 35;
			break;
		case ITEM_DRINKCON:
			punti += GET_OBJ_VAL(obj,0) / 15;
		case ITEM_CONTAINER:
			punti += (GET_OBJ_VAL(obj,0)-GET_OBJ_WEIGHT(obj)) / 15;
			if(CAN_WEAR(obj,ITEM_WEAR_HANG));
			punti=0;
			break;
		case ITEM_BOW:
		case ITEM_ARROW:
			punti =2;
			break;
		default:
			break;
	}
	
	for (i=0; i<MAX_OBJ_AFFECT; i++)
		switch (obj->affected[i].location)
		{
			case APPLY_STR:
			case APPLY_DEX:
			case APPLY_INT:
			case APPLY_WIS:
			case APPLY_CON:
			case APPLY_CHA:
			case APPLY_HITROLL:
			case APPLY_DAMROLL:
				if (obj->affected[i].modifier > 0) punti += 20*obj->affected[i].modifier;
				if (obj->affected[i].modifier < 0) punti += 5 * obj->affected[i].modifier;
				break;
			case APPLY_AC:
				if (obj->affected[i].modifier > 0) punti -= obj->affected[i].modifier * 1;
				if (obj->affected[i].modifier < 0) punti -= obj->affected[i].modifier * 6;
				break;
			case APPLY_MANA:
			case APPLY_HIT:
			case APPLY_MOVE:
				if (obj->affected[i].modifier > 0) punti += obj->affected[i].modifier * 3;
				if (obj->affected[i].modifier < 0) punti += obj->affected[i].modifier / 3;
				break;
			case APPLY_HIT_REGEN:
			case APPLY_MANA_REGEN:
				if (obj->affected[i].modifier > 0) punti += obj->affected[i].modifier * 6;
				if (obj->affected[i].modifier < 0) punti += obj->affected[i].modifier * 2;
				break;
			case APPLY_SAVING_PARA:
			case APPLY_SAVING_ROD:
			case APPLY_SAVING_PETRI:
			case APPLY_SAVING_BREATH:
			case APPLY_SAVING_SPELL:
				if (obj->affected[i].modifier > 0) punti -= obj->affected[i].modifier * 1;
				if (obj->affected[i].modifier < 0) punti -= obj->affected[i].modifier * 6;
				break;
			default: break;
		}
	
	if (IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR))     punti -= 2;
	if (IS_OBJ_STAT(obj, ITEM_ANTI_THIEF))       punti -= 2;
	if (IS_OBJ_STAT(obj, ITEM_ANTI_MAGIC_USER))  punti -= 2;
	if (IS_OBJ_STAT(obj, ITEM_ANTI_CLERIC))      punti -= 2;
	if (IS_OBJ_STAT(obj, ITEM_ANTI_PELOI))       punti -= 2;
	
	if (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))     punti -= 2;
	if (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL))  punti -= 2;
	if (IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))     punti -= 2;
	
	return punti;
}

ACMD(do_wildx)
{
	int numero;
	
	one_argument(argument, arg);
	if (!*arg)
	{
		send_to_char("Uso:  wildx <raggio> dove raggio e' la semidistanza massima visualizzata nella wilderness.\r\n", ch);
		return;
	}
	numero = atoi(arg);
	if (numero < 1 || numero > 8)
	{
		send_to_char("Deve essere compreso tra 1 e 8.\r\n", ch);
		return;
	}
	GET_WILD_MAX_X_RANGE(ch)=numero;
}

ACMD(do_wildy)
{
	int numero;
	
	one_argument(argument, arg);
	if (!*arg)
	{
		send_to_char("Uso:  wildy <raggio> dove raggio e' la semidistanza massima visualizzata nella wilderness.\r\n", ch);
		return;
	}
	numero = atoi(arg);
	if (numero < 1 || numero > 6)
	{
		send_to_char("Deve essere compreso tra 1 e 6.\r\n", ch);
		return;
	}
	GET_WILD_MAX_Y_RANGE(ch)=numero;
}

// Per passare a compareHigh la regola di sorting :
static char hiscore_sorting_mode='l';                   // E' globale !!! 

int compareHiscore(const void *l, const void *r)
{
	struct hiscore_data *left;
	struct hiscore_data *right;
	
	left = (struct hiscore_data *)l;
	right = (struct hiscore_data *)r;
	
	switch (hiscore_sorting_mode)
	{
		case 'l': return right->level      - left->level; break;
		case 'k': return right->kill       - left->kill; break;
		case 'n': return right->fama       - left->fama; break;
		case 'f': return right->notorieta  - left->notorieta; break;
		case 'm': return right->rip        - left->rip; break;
		case 'd': return right->dt         - left->dt; break;
		case 'a': return right->anzianita  - left->anzianita; break;
		case 'c': return right->soldi      - left->soldi; break;
		case 'b': return left->fama        - right->fama; break;
		case 'i': return left->notorieta   - right->notorieta; break;
		default : return 0;
	}
	return 0;
}

#define NUM_EROI 6

void boot_hiscore (void)
{
	FILE *fp;
	struct char_file_u ch;
	int max[NUM_EROI][10][2];
	int record[NUM_EROI];
	register int i, j, k, l;
	char nome[NUM_EROI][10][MAX_NAME_LENGTH];
	struct time_info_data playing_time;
	struct time_info_data real_time_passed(time_t t2, time_t t1);

	if (!(fp = fopen(PLAYER_FILE, "r"))){
		log ("Errore in fase di apertura del file players!");
		return;
	}
	
	for (i = 0; i < NUM_EROI; i++)
		for (j = 0; j < 2; j++)
			for (k = 0; k < 10; k++)
				max[i][k][j] = 0;

	for (i = 0; (!(feof(fp))); i++)
	{
		fread (&ch, sizeof (struct char_file_u), 1, fp);
		if (feof(fp))
			break;
		if (ch.level > 70 || IS_SET(ch.char_specials_saved.act, PLR_DELETED) || IS_SET(ch.player_specials_saved.pref, PRF_NO_EROE) || *ch.name < 'A' || *ch.name > 'Z')
			continue;
		record[0] = ch.player_specials_saved.kill_cnt;
		record[1] = ch.points.fama;
		record[2] = ch.player_specials_saved.rip_cnt;
		record[3] = ch.player_specials_saved.dt_cnt;
		record[4] = ch.points.gold + ch.points.bank_gold;
		record[5] = ch.played;
		
		for (k = 0; k < NUM_EROI; k++)
			for (j = 0; j < 10; j++)
				if (record[k] > max[k][j][1]){
					for (l = 9; l > j; l--){
						max[k][l][1] = max[k][l-1][1];
						max[k][l][0] = max[k][l-1][0];
					}
					max[k][j][0] = i;
					max[k][j][1] = record[k];
					break;
				}
	}
	rewind (fp);
	
	for (i = 0; (!(feof(fp))); i++)
	{
		fread (&ch, sizeof (struct char_file_u), 1, fp);
		for (j = 0; j < 10; j++)
			for (k = 0; k < NUM_EROI; k++)
				if (i == max[k][j][0])
					sprintf (nome[k][j], "%s", ch.name);
	}
	rewind (fp);
	sprintf (output_eroi_k, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n", "&2", "&4");
	sprintf (output_eroi_k, "%s%sNome             Kills%s\r\n", output_eroi_k, "&1", "&0");
	for (i = 0; i < 10; i++)
		sprintf (output_eroi_k, "%s%-16s %d\r\n", output_eroi_k, nome[0][i], max[0][i][1]);

	sprintf (output_eroi_n, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\r\n", "&2", "&4");
	sprintf (output_eroi_n, "%s%sNome             Notorieta'%s\r\n", output_eroi_n, "&1", "&0");
	for (i = 0; i < 10; i++)
		sprintf (output_eroi_n, "%s%-16s %s\r\n", output_eroi_n, nome[1][i], fama_mess[MAX(0, MIN(10, max[1][i][1]/100))]);

	sprintf (output_eroi_m, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n", "&2", "&4");
	sprintf (output_eroi_m, "%s%sNome             Morti%s\r\n", output_eroi_m, "&1", "&0");
	for (i = 0; i < 10; i++)
		sprintf (output_eroi_m, "%s%-16s %d\r\n", output_eroi_m, nome[2][i], max[2][i][1]);

	sprintf (output_eroi_d, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\r\n", "&2", "&4");
	sprintf (output_eroi_d, "%s%sNome             DT%s\r\n", output_eroi_d, "&1", "&0");
	for (i = 0; i < 10; i++)
		sprintf (output_eroi_d, "%s%-16s %d\r\n", output_eroi_d, nome[3][i], max[3][i][1]);

	sprintf (output_eroi_c, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n", "&2", "&4");
	sprintf (output_eroi_c, "%s%sNome             Soldi%s\r\n", output_eroi_c, "&1", "&0");
	for (i = 0; i < 10; i++)
		sprintf (output_eroi_c, "%s%-16s %d\r\n", output_eroi_c, nome[4][i], max[4][i][1]);
	
	sprintf (output_eroi_o, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n", "&2", "&4");
	sprintf (output_eroi_o, "%s%sNome             Tempo di gioco%s\r\n", output_eroi_o, "&1", "&0");
	for (i = 0; i < 10; i++) {
		playing_time =real_time_passed(max[5][i][1], 0);
		sprintf (output_eroi_o, "%s%-16s %d giorni e %d ore\r\n", output_eroi_o, nome[5][i], playing_time.day,playing_time.hours);
	}
		

}

/* Hiscore: modificato da Spini. Ora la funzione Boot_hiscore viene chiamata solo al reboot
   e do_hiscore si limita a stampare l'output da essa prodotto. E' un peccato aggiornarsi cosi'
   poco, ma prima faceva laggare troppo. */
ACMD(do_hiscore)
{
	/*extern int top_of_p_file;
	
	struct char_file_u temp;
	struct hiscore_data *player;
	int i, t;
	FILE *fp;
	int tempcount, tempcount2;
	
	if (!(fp = fopen(PLAYER_FILE, "r+")))
	{
		send_to_char("Impossibile accedere alla banca dati - segnalare l'errore a Kabir\r\n", ch);
		return;
	}
	
	one_argument(argument, arg);
	if (!*arg || (*arg!='l' && *arg!='k' && *arg!='m' && *arg!='d' && *arg!='a' && *arg!='f' && *arg!='n' && *arg!='i' && *arg!='b' && *arg!='c'))
	{
		strcpy(buf, "Uso:  Eroi <l> <k> <f> <n> <m> <d> <i> <b>.\r\n");
		strcat(buf, "Eroi l - Mostra i primi 10 giocatori per livello raggiunto.\r\n");
		strcat(buf, "Eroi k - Mostra i primi 10 giocatori per numero di nemici uccisi.\r\n");
		strcat(buf, "Eroi f - Mostra i primi 10 giocatori per fama.\r\n");
		strcat(buf, "Eroi n - Mostra i primi 10 giocatori per notorieta'.\r\n");
		strcat(buf, "Eroi m - Mostra i primi 10 giocatori per numero di volte che sono morti.\r\n");
		strcat(buf, "Eroi d - Mostra i primi 10 giocatori per numero di cadute nelle death traps.\r\n");
		strcat(buf, "Eroi a - Mostra i primi 10 giocatori per anzianita'.\r\n");
		strcat(buf, "Eroi c - Mostra i primi 10 giocatori per denaro'.\r\n");
		strcat(buf, "Eroi i - Mostra gli ultimi 10 giocatori per fama.\r\n");
		strcat(buf, "Eroi b - Mostra gli ultimi 10 giocatori per notorieta'.\r\n");	
		send_to_char(buf, ch);
		return;
	}
	
	CREATE(player, struct hiscore_data, top_of_p_file);
	
	for (i = 0; i < top_of_p_file; i++)
	{
		fread(&temp, sizeof(struct char_file_u), 1, fp);
		if (!IS_SET(temp.char_specials_saved.act, PLR_DELETED) &&
			!IS_SET(temp.player_specials_saved.pref, PRF_NO_EROE))
		{
			strcpy(player[i].name, temp.name);
			player[i].level = temp.level;
			player[i].kill = temp.player_specials_saved.kill_cnt;
			player[i].fama = temp.points.fama;
			player[i].notorieta = temp.points.notorieta;
			player[i].rip = temp.player_specials_saved.rip_cnt;
			player[i].dt = temp.player_specials_saved.dt_cnt;
			player[i].anzianita = temp.played;
			player[i].soldi = temp.points.gold + temp.points.bank_gold;
		}
	}
	
	fclose(fp);
	
	hiscore_sorting_mode = *arg;
	qsort(player, top_of_p_file, sizeof(struct hiscore_data), compareHiscore);
	
	sprintf(buf, "\r\n%sI 10 personaggi piu' eroici di Dalila\r\n", QGRN);
	sprintf(buf+strlen(buf), "%s-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n", QBLU);
	sprintf(buf+strlen(buf), "%sNome             Liv   Kills   Fama                Notorieta'             Anzianita' Morti   DT  Soldi%s\r\n", QGRN, QNRM);
	for (i = 0, t = 0; i < 10 && t < top_of_p_file; t++)
	{
		if (player[t].level < LVL_IMMORT) 
		{
			tempcount = (char) ((player[t].fama)/100);
			tempcount = MAX(0, MIN(tempcount, 10));
			tempcount2 = (char) ((player[t].notorieta)/100);
			tempcount2 = MAX(0, MIN(tempcount2, 9));
			sprintf(buf+strlen(buf), "%s%-16s%s %3i  %6i   %-24s   %-24s   %4dh %2dm  %5i %4i %ld\r\n", QRED, player[t].name, 
				QNRM, player[t].level, player[t].kill,notorieta_mess[tempcount2], fama_mess[tempcount], 
				(player[t].anzianita / 3600), ((player[t].anzianita / 3600) % 60),
				player[t].rip, player[t].dt, GET_LEVEL(ch)>LVL_GOD ? player[t].soldi : 0);
			i++;
		}
	}
	send_to_char(buf, ch);
	free(player);*/

	one_argument(argument, arg);
	if (!*arg || (*arg!='k' && *arg!='m' && *arg!='n' && *arg!='d' && *arg!='c' && *arg!='o'))
	{
		strcpy(buf, "Uso:  Eroi <k> <m> <n> <d> <c> <o>.\r\n");
		strcat(buf, "Eroi k - Mostra i primi 10 giocatori per numero di nemici uccisi.\r\n");
		strcat(buf, "Eroi n - Mostra i primi 10 giocatori per notorieta'.\r\n");
		strcat(buf, "Eroi m - Mostra i primi 10 giocatori per numero di volte che sono morti.\r\n");
		strcat(buf, "Eroi d - Mostra i primi 10 giocatori per numero di cadute nelle death traps.\r\n");
		strcat(buf, "Eroi c - Mostra i primi 10 giocatori per denaro'.\r\n");
		strcat(buf, "Eroi o - Mostra i primi 10 giocatori per ore di gioco.\r\n");
		send_to_char(buf, ch);
		return;
	}

	if (*arg == 'k')
		send_to_char (output_eroi_k, ch);
	else if (*arg == 'n')
		send_to_char (output_eroi_n, ch);
	else if (*arg == 'm')
		send_to_char (output_eroi_m, ch);
	else if (*arg == 'd')
		send_to_char (output_eroi_d, ch);
	else if (*arg == 'c')
		send_to_char (output_eroi_c, ch);
	else
		send_to_char (output_eroi_o, ch);
}

ACMD(do_mestieri)
{
	void calcola_percentuali_mestieri(int * primo, int *secondo);
	extern char *proficienze[];
	
	int primo[MAX_PROFICIENZE+1];
	int secondo[MAX_PROFICIENZE+1];
	int i;
	
	calcola_percentuali_mestieri(primo, secondo);
	
	strcpy(buf, QGRN);
	strcat(buf,  "\r\n"
		"  GAZZETTINO STATISTICO DELLA DARESIA - DISTRIBUZIONE DEGLI IMPIEGHI\r\n");
	strcat(buf, QBLU);
	strcat(buf,  "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\r\n");
	strcat(buf, QGRN);
	strcat(buf,  "Professione                        Primo Lavoro         Secondo Lavoro\r\n");
	strcat(buf, QNRM);
	for (i=4; i <= TOT_PROF_MESTIERI; i++)
		sprintf(buf+strlen(buf), "%s%c%-32s  %s%4d%s / 100 %%           %s%4d%s / 100 %%\r\n", QRED,
			(proficienze[i][0]+'A'-'a'), proficienze[i]+1,
			QGRN, primo[i], QNRM, QGRN, secondo[i], QNRM);
	send_to_char(buf, ch);
}
