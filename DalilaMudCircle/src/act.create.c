/* ************************************************************************
*   File: act.create.c					Part of CircleMUD *
*  Usage: Player-level object creation stuff				  *
*									  *
*  All rights reserved.	 See license.doc for complete information.	  *
*									  *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.		  *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include <sys/stat.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"

extern int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);

/* struct for syls */
struct syllable {
	char *org;
	char *new;
};

/* extern variables */
extern char *spells[];
extern struct spell_info_type spell_info[];
extern struct syllable syls[];

/* extern procedures */
int mag_manacost(struct char_data * ch, int spellnum);

int factors[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char *get_spell_name(char *argument)
{
	char *s;
	
	s = strtok(argument, "'");
	s = strtok(NULL, "'");
	
	return s;
}


char *potion_names[] = {
	"milky white",
	"bubbling white",
	"glowing ivory",
	"glowing blue",
	"bubbling yellow",
	"light green",
	"gritty brown",
	"blood red",
	"swirling purple",
	"flickering green",
	"cloudy blue",
	"glowing red",
	"sparkling white",
	"incandescent blue",
	"black dark"
};

void make_potion(struct char_data *ch, int potion, struct obj_data *container)
{
	struct obj_data *final_potion;
	struct extra_descr_data *new_descr;
	int can_make = TRUE, mana, dam, num = 0;
	
	/* Modify this list to suit which spells you
	   want to be able to mix. */
	switch (potion) {
		case SPELL_CURE_BLIND:
			num = 0;
			break;
			
		case SPELL_CURE_LIGHT:
			num = 1;
			break;
			
		case SPELL_CURE_CRITIC:
			num = 2;
			break;
			
		case SPELL_DETECT_MAGIC:
			num = 3;
			break;
			
		case SPELL_DETECT_INVIS:
			num = 4;
			break;
			
		case SPELL_DETECT_POISON:
			num = 5;
			break;
			
		case SPELL_REMOVE_POISON:
			num = 6;
			break;
			
		case SPELL_STRENGTH:
			num = 7;
			break;
			
		case SPELL_WORD_OF_RECALL:
			num = 8;
			break;
			
		case SPELL_SENSE_LIFE:
			num = 9;
			break;
			
		case SPELL_WATERWALK:
			num = 10;
			break;
			
		case SPELL_INFRAVISION:
			num = 11;
			break;
			
		case SPELL_HEAL:
			num = 12;
			break;
			
		case SPELL_FEAST:
			num = 13;
			break;
			
		case SPELL_FIRESHD:
			num = 14;
			break;
			
		default:
			can_make = FALSE;
			break;
	}
	
	if (can_make == FALSE) {
		send_to_char("Quello spell non puo' essere mescolato"
			" in una pozione.\r\n", ch);
		return;
	}
	else if ((number(1, 3) == 3) && (GET_LEVEL(ch) < LVL_IMMORT)) {
		send_to_char("Appena inizi a mescolare la pozione, violentemente"
			" esplode!\r\n",ch);
		act("$n inizia a mescolare una pozione, ma esplode improvvisamente!",
			FALSE, ch, 0,0, TO_ROOM);
		extract_obj(container);
		dam = number(15, mag_manacost(ch, potion) * 2);
		GET_HIT(ch) -= dam;
		update_pos(ch);
		return;
	}
	
	/* requires x3 mana to mix a potion than the spell */
	mana = mag_manacost(ch, potion) * 3;
	if (GET_MANA(ch) - mana > 0) {
		if (GET_LEVEL(ch) < LVL_GOD) GET_MANA(ch) -= mana;
		sprintf(buf, "Crei una pozione %s.\r\n",
			spells[potion]);
		send_to_char(buf, ch);
		act("$n crei una pozione!", FALSE, ch, 0, 0, TO_ROOM);
		extract_obj(container);
	}
	else {
		send_to_char("Non hai abbastanza mana per la"
			" pozione!\r\n", ch);
		return;
	}
	
	final_potion = create_obj();
	
	final_potion->item_number = NOTHING;
	final_potion->in_room = NOWHERE;
	sprintf(buf2, "Pozione %s %s", potion_names[num], spells[potion]);
	final_potion->name = str_dup(buf2);
	
	sprintf(buf2, "Una pozione %s e' qui.", potion_names[num]);
	final_potion->description = str_dup(buf2);
	
	sprintf(buf2, "una pozione %s", potion_names[num]);
	final_potion->short_description = str_dup(buf2);
	
	/* extra description coolness! */
	CREATE(new_descr, struct extra_descr_data, 1);
	new_descr->keyword = str_dup(final_potion->name);
	sprintf(buf2, "Sembra essere una pozione %s.", spells[potion]);
	new_descr->description = str_dup(buf2);
	new_descr->next = NULL;
	final_potion->ex_description = new_descr;
	
	GET_OBJ_TYPE(final_potion) = ITEM_POTION;
	GET_OBJ_WEAR(final_potion) = ITEM_WEAR_TAKE;
	GET_OBJ_EXTRA(final_potion) = ITEM_NORENT;
	GET_OBJ_VAL(final_potion, 0) = GET_LEVEL(ch);
	GET_OBJ_VAL(final_potion, 1) = potion;
	GET_OBJ_VAL(final_potion, 2) = -1;
	GET_OBJ_VAL(final_potion, 3) = -1;
	GET_OBJ_COST(final_potion) = GET_LEVEL(ch) * 500;
	GET_OBJ_WEIGHT(final_potion) = 1;
	GET_OBJ_RENT(final_potion) = 0;
	GET_OBJ_TSLOTS(final_potion) = 100;
	GET_OBJ_CSLOTS(final_potion) = 100;
	
	obj_to_char(final_potion, ch);
}

ACMD(do_brew)
{
	struct obj_data *container = NULL;
	struct obj_data *obj, *next_obj;
	char bottle_name[MAX_STRING_LENGTH];
	char spell_name[MAX_STRING_LENGTH];
	char *temp1, *temp2;
	int potion, found = FALSE;
	
	temp1 = one_argument(argument, bottle_name);
	
	/* sanity check */
	if (temp1) {
		temp2 = get_spell_name(temp1);
		if (temp2)
			strcpy(spell_name, temp2);
	} else {
		bottle_name[0] = '\0';
		spell_name[0] = '\0';
	}
	
	
	
	
	if (!*bottle_name || !*spell_name) {
		send_to_char("Cosa vuoi mescolare nella pozione?\r\n", ch);
		return;
	}
	
	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (obj == NULL)
			return;
		else if (!(container = get_obj_in_list_vis(ch, bottle_name,
						ch->carrying)))
			continue;
		else
			found = TRUE;
	}
	if (found != FALSE && (GET_OBJ_TYPE(container) != ITEM_DRINKCON)) {
		send_to_char("Quell'oggetto non e' un contenitore di liquidi!\r\n", ch);
		return;
	}
	if (found == FALSE) {
		sprintf(buf, "Non sembra tu abbia %s!\r\n",
			bottle_name);
		send_to_char(buf, ch);
		return;
	}
	
	if (!spell_name || !*spell_name) {
		send_to_char("Il nome dello spell deve essere tra apici!\r\n",
			ch);
		return;
	}
	
	potion = find_skill_num(spell_name);
	
	if ((potion < 1) || (potion > MAX_SPELLS)) {
		send_to_char("Quale spell?!?\r\n", ch);
		return;
	}
	if (GET_LEVEL(ch) < spell_info[potion].min_level[(int) GET_CLASS(ch)]) {
		send_to_char("Non sai come fare quella pozione!\r\n",
			ch);
		return;
	}
	if (GET_SKILL(ch, potion) == 0) {
		send_to_char("Non sei familiare con l'incatesimo che vuoi creare.\r\n",
			ch);
		return;
	}
	make_potion(ch, potion, container);
}


char *garble_spell(int spellnum)
{
	char lbuf[256];
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
	return buf;
}

#define IS_MAGIC_PAPER(paper) (GET_OBJ_TYPE(paper)==ITEM_NOTE&&GET_OBJ_VAL(paper,3)==1)
#define IS_MAGIC_MARKER(marker) (GET_OBJ_TYPE(marker)==ITEM_PEN&&GET_OBJ_VAL(marker,3)==1)

void make_scroll(struct char_data *ch, int scroll, struct obj_data *paper){
	struct obj_data *final_scroll;
	struct extra_descr_data *new_descr;
	int can_make = TRUE, mana, dam = 0;
	
  /* add a case statement here for prohibited spells */
	
	if (can_make == FALSE) {
		send_to_char("Quello spell non puo' essere scritto in una"
			" pergamena.\r\n", ch);
		return;
	}
	
	if(!test_and_improve(ch, SKILL_WRITE_MAGIC, 100, 15, GET_WIS(ch)+GET_INT(ch)-20)) {
		send_to_char("Appena inizi a scrivere,perdi la concentrazione e rinunci.", ch);
		GET_MANA(ch)-=number(10,50);
		return;
	}
	
	if ((number(1, 10) == 10) && (GET_LEVEL(ch) < LVL_IMMORT)) {
		send_to_char("Appena inizi a scrivere le parole magiche, la"
			           " pergamena prende fuoco, e scompare avvolta da fiamme azzurre!\r\n",ch);
		act("$n tenta di scrivere una pergamena magica, che s'incendia all'improvviso!",
			FALSE, ch, 0,0, TO_ROOM);
		extract_obj(paper);
		dam = number(15, mag_manacost(ch, scroll) * 2);
		GET_HIT(ch) -= dam;
		update_pos(ch);
		return;
	}
  /* requires x3 mana to scribe a scroll than the spell */
	
	if(number(1,120)>(GET_SKILL(ch, scroll)+(GET_WIS(ch)-13)*3)){
		send_to_char("Proprio sul piu' bello ti deconcentri e l'unico risultato e' un gran malditesta!",ch);
		GET_MANA(ch)=number(10,50);
		return;
	}
	
	mana = (mag_manacost(ch, scroll) * 3);
	
	if (GET_MANA(ch) < mana) {
		if (GET_LEVEL(ch) < LVL_GOD)
			GET_MANA(ch) -= mana;
		sprintf(buf, "Crei una pergamena di %s.\r\n",spells[scroll]);
		send_to_char(buf, ch);
		act("$n crea una pergamena!", FALSE, ch, 0, 0, TO_ROOM);
		extract_obj(paper);
	}
	else {
		send_to_char("Non hai abbastanza mana per scrivere\r\n"
			           "uno spell cosi potente!\r\n", ch);
		return;
	}
	
	final_scroll = create_obj();
	
	final_scroll->item_number = NOTHING;
	final_scroll->in_room = NOWHERE;
	sprintf(buf2, "pergamena %s %s",spells[scroll], garble_spell(scroll));
	final_scroll->name = str_dup(buf2);
	
	sprintf(buf2,"Una pergamena con iscritto '%s' e' qui.",garble_spell(scroll));
	final_scroll->description = str_dup(buf2);
	sprintf(buf2, "una pergamena %s", garble_spell(scroll));
	final_scroll->short_description = str_dup(buf2);
	
  /* extra description coolness! */
	CREATE(new_descr, struct extra_descr_data, 1);
	new_descr->keyword = str_dup(final_scroll->name);
	sprintf(buf2, "Sembra essere una scroll di %s.", spells[scroll]);
	new_descr->description = str_dup(buf2);
	new_descr->next = NULL;
	final_scroll->ex_description = new_descr;
	
	GET_OBJ_TYPE(final_scroll) = ITEM_SCROLL;
	GET_OBJ_WEAR(final_scroll) = ITEM_WEAR_TAKE;
  //GET_OBJ_EXTRA(final_scroll) = ITEM_NORENT;
	GET_OBJ_VAL(final_scroll, 0) = GET_LEVEL(ch);
	GET_OBJ_VAL(final_scroll, 1) = scroll;
	GET_OBJ_VAL(final_scroll, 2) = -1;
	GET_OBJ_VAL(final_scroll, 3) = -1;
	GET_OBJ_COST(final_scroll) = GET_LEVEL(ch) * 100;
	GET_OBJ_WEIGHT(final_scroll) = 0;
	GET_OBJ_RENT(final_scroll) = 0;
	GET_OBJ_TSLOTS(final_scroll)=100;
	GET_OBJ_CSLOTS(final_scroll)=100;
	obj_to_char(final_scroll, ch);
}


ACMD(do_scribe)
{
	struct obj_data *paper = NULL;
	struct obj_data *marker= NULL;
	struct obj_data *obj, *next_obj;
	char paper_name[MAX_STRING_LENGTH];
	char spell_name[MAX_STRING_LENGTH];
	char *temp1, *temp2;
	int scroll = 0, found = FALSE;
	
	temp1 = one_argument(argument, paper_name);
	
  /* sanity check */
	if(temp1) {
		temp2 = get_spell_name(temp1);
		if (temp2)
			strcpy(spell_name, temp2);
	}
	else {
		paper_name[0] = '\0';
		spell_name[0] = '\0';
	}
	if (!*paper_name || !*spell_name) {
		send_to_char("Cosa vuoi scrivere sulla pergamena?\r\n", ch);
		return;
	}
	
	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (obj == NULL)
			return;
		else
		if(!(paper=get_obj_in_list_vis(ch,paper_name,ch->carrying)))
			continue;
		else
			found = TRUE;
	}
	if (found&&(GET_OBJ_TYPE(paper)!=ITEM_NOTE||GET_OBJ_VAL(paper,3)!=1)) {
		send_to_char("Non puoi scrivere su quello!\r\n", ch);
		return;
	}
	if (found == FALSE) {
		sprintf(buf, "Non sembra tu abbia %s!\r\n",paper_name);
		send_to_char(buf, ch);
		return;
	}
	if(!(marker=GET_EQ(ch,WEAR_HOLD))){
		send_to_char("Devi avere un Magic Marker in mano",ch);
		return;
	}
	if(!(IS_MAGIC_MARKER(marker))){
		send_to_char("Devi avere un Magic Marker in mano",ch);
		return;
	}
	if(GET_OBJ_VAL(marker,1)<=0){
		send_to_char("Il Marker e' Scarico.",ch);
		return;
	}
	
	if (!spell_name || !*spell_name) {
		send_to_char("Il nome dello spell deve essere tra apici!\r\n",ch);
		return;
	}
	
	scroll = find_skill_num(spell_name);
	
	if((scroll < 1) || (scroll > MAX_SPELLS)) {
		send_to_char("Scrivere quale spell?!?\r\n", ch);
		return;
	}
	if(GET_LEVEL(ch) < spell_info[scroll].min_level[(int) GET_CLASS(ch)]){
		send_to_char("Non hai ancora imparato quello spell!\r\n",ch);
		return;
	}
	if (GET_SKILL(ch, scroll) == 0) {
		send_to_char("Non conosci niente di quello spell!\r\n",ch);
		return;
	}
	GET_OBJ_VAL(marker,1)-=number(1,2);
	make_scroll(ch, scroll, paper);
}

ACMD(do_forge)
{
  /* PLEASE NOTE!!!  This command alters the object_values of the target
	   weapon, and this will save to the rent files.  It should not cause
	   a problem with stock Circle, but if your weapons use the first
	   position [ GET_OBJ_VAL(weapon, 0); ], then you WILL have a problem.
	   This command stores the character's level in the first value to
	   prevent the weapon from being "forged" more than once by mortals.
	   Install at your own risk.  You have been warned...*/
	
	int med_dam(int dam,int vector[]);
	
	extern int objlevel (struct obj_data * obj);
	
	struct obj_data *weapon = NULL;
	struct obj_data *obj, *next_obj;
	char weapon_name[MAX_STRING_LENGTH];
	char forge_level[3];
	int found = FALSE, add_dam = 0,dice1,dice2;
	int  medium_damage;
	int i,fabbro,liv,livello;
	
	
	two_arguments(argument, weapon_name,forge_level);
	
	if (!*weapon_name) {
		send_to_char("Cosa desideri forgiare?\r\n", ch);
		return;
	}
	
	if((liv=GET_ABIL(ch,ABIL_RIPARAZIONE))<=8){
		send_to_char("Non conosci ancora l'arte del forgiare",ch);
		return;
	}
	if(!*forge_level)
		add_dam=(liv-8)+MAX(0,liv-14);/*per default il massimo possibile*/
	else
		add_dam=atoi(forge_level);
	if(add_dam<=0)
		return;
	if(add_dam>((liv-8)+MAX(0,liv-14))){/*massimo medium ragg:10*/
		send_to_char("Non sei ancora in grado di forgiare un arma Cosi Bene!\n",ch);
		return;
	}
	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (obj == NULL)
			return;
		else if (!(weapon = get_obj_in_list_vis(ch, weapon_name,
						ch->carrying)))
			continue;
		else
			found = TRUE;
	}
	if (found == FALSE) {
		sprintf(buf, "Non sembra tu abbia %s!\r\n",
			weapon_name);
		send_to_char(buf, ch);
		return;
	}
	
	if (found && (GET_OBJ_TYPE(weapon) != ITEM_WEAPON)) {
		sprintf(buf, "Non sembra %s possa diventare una"
			" buona arma...\r\n", weapon_name);
		send_to_char(buf, ch);
		return;
	}
	if (IS_OBJ_STAT(weapon, ITEM_FORGED) && (GET_LEVEL(ch) < LVL_GRGOD)) {
		send_to_char("Non puoi forgiare un'arma piu' di una volta!\r\n", ch);
		return;
	}
	for (i=0; i<MAX_OBJ_AFFECT; i++)
		if (weapon->affected[i].location == APPLY_DAMROLL||(weapon->affected[i].location == APPLY_HITROLL))
		{
			send_to_char("Non riesci a forgiare quell'arma, e' troppo potente.\r\n", ch);
			return;
		}
  /* determine success probability*/
	if((fabbro=GET_PROF(ch,PROF_FABBRO))<50){
		send_to_char("Devi essere un buon fabbro per forgiare un arma!\n",ch);
		return;
	}
	if (GET_ABIL(ch,ABIL_RIPARAZIONE) <EXPERT_LIV){
		send_to_char("Non sei abbastanza abile nell arte della Riparazione.\n",ch);
		return;
	}
  /*determina quanti check si devono fare:a seconda dell medium_damage*/
	for(i=0;i<add_dam;i++){
		if(number(20,100)>(fabbro+(GET_DEX(ch)-10)+(GET_WIS(ch)-10))-5*i)
			break;
	}
	add_dam=i;
	if(add_dam==0||(number(1,100)<(add_dam*7)))
	{
		send_to_char("Appena inizi a lavorare attorno all'arma,"
			" essa esplode!\r\n", ch);
		act("$n tenta di forgiare un'arma, ma esplode!",
			FALSE, ch, 0,0, TO_ROOM);
		extract_obj(weapon);
		GET_HIT(ch) -= number(30, 60);
		update_pos(ch);
		return;
	}
	else{
		livello = (GET_OBJ_LEVEL(weapon)>0 ? GET_OBJ_LEVEL(weapon) :
				MAX(1, objlevel(weapon)));
		send_to_char("Hai dato una nuova vita all'arma!\r\n", ch);
		act("$n ha dato una nuova vita all'arma!",
			FALSE, ch, 0, 0, TO_ROOM);
    /* Enrico:Calcolo del nuovo Dam med e rispettivi dadi*/
		medium_damage=GET_OBJ_VAL(weapon,1) * (GET_OBJ_VAL(weapon,2)+1) /2;
		medium_damage+=add_dam;
		medium_damage=med_dam(medium_damage,factors);
		GET_OBJ_VAL(weapon,1)=medium_damage/100;
		dice1=medium_damage/100;
		GET_OBJ_VAL(weapon,2)=medium_damage-dice1*100;
		dice2=medium_damage-dice1*100;
		SET_BIT(GET_OBJ_EXTRA(weapon), ITEM_FORGED);
		GET_OBJ_LEVEL(weapon) = livello;
		GET_OBJ_COST(weapon)+=add_dam*1000;
	}
}



ACMD(do_affila)/*By Blizzard*/
{
  /* PLEASE NOTE!!!  This command alters the object_values of the target
	   weapon, and this will save to the rent files.  It should not cause
	   a problem with stock Circle, but if your weapons use the first
	   position [ GET_OBJ_VAL(weapon, 0); ], then you WILL have a problem.
	   This command stores the character's level in the first value to
	   prevent the weapon from being "forged" more than once by mortals.
	   Install at your own risk.  You have been warned...
	*/
	struct obj_data *weapon = NULL;
	struct obj_data *obj, *next_obj;
	char weapon_name[MAX_STRING_LENGTH];
	int liv_abil,prof_fabbro,i,livello;
	int found=FALSE;
	extern int objlevel (struct obj_data * obj);
	
	one_argument(argument,weapon_name);
	if (!*weapon_name) {
		send_to_char("Cosa desideri affilare?\r\n", ch);
		return;
	}
	for (obj = ch->carrying; obj; obj = next_obj) {
		next_obj = obj->next_content;
		if (obj == NULL)
			return;
		else if (!(weapon = get_obj_in_list_vis(ch, weapon_name,
						ch->carrying)))
			continue;
		else
			found = TRUE;
	}
	if (found == FALSE) {
		sprintf(buf, "Non sembra tu abbia %s!\r\n",
			weapon_name);
		send_to_char(buf, ch);
		return;
	}
	livello = (GET_OBJ_LEVEL(weapon)>0 ? GET_OBJ_LEVEL(weapon) :
			MAX(1, objlevel(weapon)));
	if (found && (GET_OBJ_TYPE(weapon) != ITEM_WEAPON)) {
		sprintf(buf, "Non sembra %s possa diventare una"
			" buona arma...\r\n", weapon_name);
		send_to_char(buf, ch);
		return;
	}
	if (IS_OBJ_STAT(weapon, ITEM_AFFILATO) && (GET_LEVEL(ch) < LVL_GRGOD)) {
		send_to_char("Quest'arma e' gia affilata come un rasoio!!\r\n", ch);
		return;
	}
	
	for (i=0; i<MAX_OBJ_AFFECT; i++){
		if ((weapon->affected[i].location==0) ||
			(weapon->affected[i].location==APPLY_HITROLL))
			break;
	}
	if(i==MAX_OBJ_AFFECT)
	{
		send_to_char("Non riesci ad affilare quell'arma, e' troppo potente.\r\n"
			, ch);
		return;
	}
	
  /* determine success probability */
	
	if((prof_fabbro=GET_PROF(ch,PROF_FABBRO))<30){
		send_to_char("Devi essere un fabbro decente per affilare un arma!\n",ch);
		return;
	}
	if ((liv_abil=GET_ABIL(ch,ABIL_RIPARAZIONE)) <=0){
		send_to_char("Non sai nemmeno da dove cominciare!!.\n",ch);
		return;
	}
	if(GET_LEVEL(ch)>=LVL_GOD)
		liv_abil=20;
	if(number(1,100)>prof_fabbro+GET_STR(ch)/2+GET_INT(ch)/2){
		sprintf(buf,"Provi ad affilare %s,ma l'unico risultato che ottieni e' rovinarla!\n",weapon->name);
		send_to_char(buf,ch);
		act("$N prova ad affilare $p ,ma maldestramente riesce solo a rovinarla.",FALSE,0,weapon,ch,TO_NOTVICT);
		GET_OBJ_TSLOTS(weapon) -=5;
		GET_OBJ_CSLOTS(weapon) -=5;
		if (GET_OBJ_CSLOTS(weapon)<=0 || GET_OBJ_TSLOTS(weapon)<=0)
		{
			sprintf(buf, "%s si sbriciola e cade per terra!\r\n", weapon->name);
			act("$p di $n si sbriciola e cade per terra.",FALSE,ch,weapon,0,TO_NOTVICT);
			send_to_char(buf, ch);
			extract_obj(weapon);
		}
		return;
	}
	else{
		sprintf(buf,"Rendi %s affilata come un rasoio.\n",weapon->name);
		send_to_char(buf,ch);
		act("$n affila $p ,ora e' tagliente come un rasoio!",FALSE,ch,weapon,0,TO_NOTVICT);
		GET_OBJ_TSLOTS(weapon) -=number(3,7);
		if (GET_OBJ_CSLOTS(weapon)<=0 || GET_OBJ_TSLOTS(weapon)<=0)
		{
			sprintf(buf, "%s si sbriciola e cade per terra!\r\n", weapon->name);
			act("$p di $N si sbriciola e cade per terra.",FALSE,0,weapon,ch,TO_NOTVICT);
			send_to_char(buf, ch);
			extract_obj(weapon);
		}
		if(GET_OBJ_CSLOTS(weapon)>GET_OBJ_TSLOTS(weapon))
			GET_OBJ_CSLOTS(weapon)=GET_OBJ_TSLOTS(weapon);
    /*resistenza dell affilatura*/
		GET_OBJ_VAL(weapon,0)=1+MAX(1,number(liv_abil/2,liv_abil));
		SET_BIT(GET_OBJ_EXTRA(weapon), ITEM_AFFILATO);
		if(weapon->affected[i].location==APPLY_HITROLL)
			weapon->affected[i].modifier +=1;
		if(weapon->affected[i].location==0){
			weapon->affected[i].location=APPLY_HITROLL;
			weapon->affected[i].modifier +=1;
		}
		GET_OBJ_LEVEL(weapon) = livello;
		
		if (weapon->worn_by == ch) {
			if (GET_EQ(ch, WEAR_WIELD)==weapon){
				unequip_char(ch, WEAR_WIELD);
				equip_char(ch, weapon,WEAR_WIELD);
			}
			if (GET_EQ(ch, WEAR_WIELD_L)==weapon){
				unequip_char(ch, WEAR_WIELD_L);
				equip_char(ch, weapon, WEAR_WIELD_L);
			}
		}
		
	}
}
