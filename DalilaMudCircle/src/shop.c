/* ************************************************************************
*   File: shop.c                                        Part of CircleMUD *
*  Usage: shopkeepers: loading config files, spec procs.                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/***
 * The entire shop rewrite for Circle 3.0 was done by Jeff Fink.  Thanks Jeff!
 ***/

#define __SHOP_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"
#include "shop.h"
#include "clan.h"
#include "dg_scripts.h"

#define WHITESPACE " \t"
#define MAX_OBJ_IN_MAG 300

/* External variables */
extern struct str_app_type str_app[];
extern struct dex_app_type dex_app[];
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct char_data *mob_proto;
extern struct obj_data *obj_proto;
extern struct room_data *world;
extern struct time_info_data time_info;
extern char *item_types[];
extern char *extra_bits[];
extern int top_of_world;
extern void House_crashsave(room_vnum vnum);

#define SHOPS_FILE "etc/Shops"

/* Forward/External function declarations */
ACMD(do_tell);
ACMD(do_action);
ACMD(do_echo);
ACMD(do_say);
char *fname(char *namelist);
void sort_keeper_objs(struct char_data * keeper, int shop_nr);
void get_obj_from_mag(int shop_nr, struct char_data * keeper, int qta);
void put_obj_to_mag(int shop_nr, struct char_data * keeper, int qta);
void approv_shop();
extern int can_take_container(struct char_data * ch, struct obj_data * obj);
extern int objlevel (struct obj_data * obj);
extern int invalid_class(struct char_data *ch, struct obj_data *obj);
extern int invalid_align(struct char_data *ch, struct obj_data *obj);
void Crash_MobEqSave(struct char_data * ch, room_vnum room);
int  Crash_MobEqLoad(struct char_data * ch, room_vnum room);
extern int check_gen_abil(struct char_data *ch,int abil);
/* Local variables */
struct shop_data *shop_index;
int top_shop = 0;
int cmd_say, cmd_tell, cmd_emote, cmd_slap, cmd_puke;
void list_shop_own (struct char_data * ch, int mode);
int IsYourShop(struct char_data * ch,int shop_nmr, int msg);
void shopping_exchange(char *arg, struct char_data * ch,
	struct char_data * keeper, int shop_nr,int type);
void make_obj_gold(long mag,long shop_nr);
void perform_make_obj_gold();
long num_obj_in_mag (room_rnum room);
long num_same_obj_in_obj (struct obj_data * obj, struct obj_data * object);
long num_obj_in_obj_mag (struct obj_data * object);
long num_same_obj_in_mag (struct obj_data *obj ,room_rnum room);
long get_num_shop_from_mag(long vnum);
void list_prod(struct char_data *ch, int shop_nr, char *argument,struct char_data *keeper);
void get_obj_from_mag_with_name(int shop_nr, struct char_data * keeper, int qta,
	char *name);
void show_prodotti(int shop_nr, struct char_data *ch, struct char_data *keeper);
void load_shop(); 
void save_shop();

int is_ok_char(struct char_data * keeper, struct char_data * ch, int shop_nr)
{
	char buf[200];
	
	if (!(CAN_SEE(keeper, ch))) {
		do_say(keeper, MSG_NO_SEE_CHAR, cmd_say, 0);
		return (FALSE);
	}
	if (IS_GOD(ch))
		return (TRUE);
	
	if ((IS_GOOD(ch) && NOTRADE_GOOD(shop_nr)) ||
		(IS_EVIL(ch) && NOTRADE_EVIL(shop_nr)) ||
		(IS_NEUTRAL(ch) && NOTRADE_NEUTRAL(shop_nr))) {
		sprintf(buf, "%s %s", GET_NAME(ch), MSG_NO_SELL_ALIGN);
		do_tell(keeper, buf, cmd_tell, 0);
		return (FALSE);
	}
	if (IS_NPC(ch))
		return (TRUE);
	
	if ((IS_MAGIC_USER(ch) && NOTRADE_MAGIC_USER(shop_nr)) ||
		(IS_CLERIC(ch) && NOTRADE_CLERIC(shop_nr)) ||
		(IS_THIEF(ch) && NOTRADE_THIEF(shop_nr)) ||
		(IS_WARRIOR(ch) && NOTRADE_WARRIOR(shop_nr)) ||
		(IS_PELOI(ch) && NOTRADE_THIEF(shop_nr))) {
		sprintf(buf, "%s %s", GET_NAME(ch), MSG_NO_SELL_CLASS);
		do_tell(keeper, buf, cmd_tell, 0);
		return (FALSE);
	}
	
	if ((find_clan_by_id(GET_CLAN(ch))>-1) && (find_clan_by_id(SHOP_CLAN(shop_nr))>-1) &&
		(clan[find_clan_by_id(SHOP_CLAN(shop_nr))].diplomacy[find_clan_by_id(GET_CLAN(ch))].status == WAR))
	{
		sprintf(buf, "%s %s", GET_NAME(ch), MSG_NO_SELL_CLAN);
		do_tell(keeper, buf, cmd_tell, 0);
		return (FALSE);
	}
	
	return (TRUE);
}


int is_open(struct char_data * keeper, int shop_nr, int msg)
{
	char buf[200];
	
	*buf = 0;
	if (SHOP_OPEN1(shop_nr) > time_info.hours)
		strcpy(buf, MSG_NOT_OPEN_YET);
	else if (SHOP_CLOSE1(shop_nr) < time_info.hours){
		if (SHOP_OPEN2(shop_nr) > time_info.hours)
			strcpy(buf, MSG_NOT_REOPEN_YET);
		else if (SHOP_CLOSE2(shop_nr) < time_info.hours)
			strcpy(buf, MSG_CLOSED_FOR_DAY);
		}
	if (!(*buf))
		return (TRUE);
	if (msg)
		do_say(keeper, buf, cmd_tell, 0);
	return (FALSE);
}


int is_ok(struct char_data * keeper, struct char_data * ch, int shop_nr)
{
	if (is_open(keeper, shop_nr, TRUE))
		return (is_ok_char(keeper, ch, shop_nr));
	else
		return (FALSE);
}


void push(struct stack_data * stack, int pushval)
{
	S_DATA(stack, S_LEN(stack)++) = pushval;
}


int top(struct stack_data * stack)
{
	if (S_LEN(stack) > 0)
		return (S_DATA(stack, S_LEN(stack) - 1));
	else
		return (NOTHING);
}


int pop(struct stack_data * stack)
{
	if (S_LEN(stack) > 0)
		return (S_DATA(stack, --S_LEN(stack)));
	else {
		log("Illegal expression in shop keyword list");
		return (0);
	}
}


void evaluate_operation(struct stack_data * ops, struct stack_data * vals)
{
	int oper;
	
	if ((oper = pop(ops)) == OPER_NOT)
		push(vals, !pop(vals));
	else if (oper == OPER_AND)
		push(vals, pop(vals) && pop(vals));
	else if (oper == OPER_OR)
		push(vals, pop(vals) || pop(vals));
}


int find_oper_num(char token)
{
	int index;
	
	for (index = 0; index <= MAX_OPER; index++)
		if (strchr(operator_str[index], token))
			return (index);
	return (NOTHING);
}


int evaluate_expression(struct obj_data * obj, char *expr)
{
	struct stack_data ops, vals;
	char *ptr, *end, name[200];
	int temp, index;
	
	if (!expr)
		return TRUE;
	
	if (!isalpha(*expr))
		return TRUE;
	
	ops.len = vals.len = 0;
	ptr = expr;
	while (*ptr) {
		if (isspace(*ptr))
			ptr++;
		else {
			if ((temp = find_oper_num(*ptr)) == NOTHING) {
				end = ptr;
				while (*ptr && !isspace(*ptr) && (find_oper_num(*ptr) == NOTHING))
					ptr++;
				strncpy(name, end, ptr - end);
				name[ptr - end] = 0;
				for (index = 0; *extra_bits[index] != '\n'; index++)
					if (!str_cmp(name, extra_bits[index])) {
						push(&vals, IS_SET(GET_OBJ_EXTRA(obj), 1 << index));
						break;
					}
				if (*extra_bits[index] == '\n')
					push(&vals, isname(name, obj->name));
			} else {
				if (temp != OPER_OPEN_PAREN)
					while (top(&ops) > temp)
						evaluate_operation(&ops, &vals);
				
				if (temp == OPER_CLOSE_PAREN) {
					if ((temp = pop(&ops)) != OPER_OPEN_PAREN) {
						log("Illegal parenthesis in shop keyword expression");
						return (FALSE);
					}
				} else
					push(&ops, temp);
				ptr++;
			}
		}
	}
	while (top(&ops) != NOTHING)
		evaluate_operation(&ops, &vals);
	temp = pop(&vals);
	if (top(&vals) != NOTHING) {
		log("Extra operands left on shop keyword expression stack");
		return (FALSE);
	}
	return (temp);
}


int trade_with(struct obj_data * item, int shop_nr)
{
	int counter;
	
	if (GET_OBJ_COST(item) < 1)
		return (OBJECT_NOTOK);
	
	if (IS_OBJ_STAT(item, ITEM_NOSELL) || IS_OBJ_STAT(item, ITEM_NODROP))
		return (OBJECT_NOTOK);
	if(SHOP_PG_PROD(shop_nr))
		if(!IS_OBJ_STAT(item,ITEM_RESTRING))
			return (OBJECT_NOTOK);
	for (counter = 0; SHOP_BUYTYPE(shop_nr, counter) != NOTHING; counter++)
		if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(item)){
			if ((GET_OBJ_VAL(item, 2) == 0) &&
				((GET_OBJ_TYPE(item) == ITEM_WAND) ||
					(GET_OBJ_TYPE(item) == ITEM_STAFF)))
				return (OBJECT_DEAD);
			else if (evaluate_expression(item, SHOP_BUYWORD(shop_nr, counter)))
				return (OBJECT_OK);
		}	
	return (OBJECT_NOTOK);
}


int same_obj(struct obj_data * obj1, struct obj_data * obj2)
{
	int index;
	
	if (!obj1 || !obj2)
		return (obj1 == obj2);
	
	if (GET_OBJ_RNUM(obj1) != GET_OBJ_RNUM(obj2))
		return (FALSE);
	
	if (GET_OBJ_COST(obj1) != GET_OBJ_COST(obj2))
		return (FALSE);
	
	if (GET_OBJ_EXTRA(obj1) != GET_OBJ_EXTRA(obj2))
		return (FALSE);
	
	for (index = 0; index < MAX_OBJ_AFFECT; index++)
		if ((obj1->affected[index].location != obj2->affected[index].location) ||
			(obj1->affected[index].modifier != obj2->affected[index].modifier))
			return (FALSE);
		
	return (TRUE);
}


int shop_producing(struct obj_data * item, int shop_nr)
{
	int counter;
	
	if (GET_OBJ_RNUM(item) < 0)
		return (FALSE);
	
	for (counter = 0; SHOP_PRODUCT(shop_nr, counter) != NOTHING; counter++)
		if (same_obj(item, &obj_proto[SHOP_PRODUCT(shop_nr, counter)]))
			return (TRUE);
	return (FALSE);
}


int transaction_amt(char *arg)
{
	int num;
	char *arg_rem;
	
	arg_rem = one_argument(arg, buf);
	if (*buf)
		if ((is_number(buf))) {
			if (*arg_rem) {
				num = atoi(buf);
				strcpy(arg, arg + strlen(buf) + 1);
				return (num);
			}
		}
	return (1);
}


char *times_message(struct obj_data * obj, char *name, int num)
{
	static char buf[256];
	char *ptr;
	
	if (obj)
		strcpy(buf, obj->short_description);
	else {
		if ((ptr = strchr(name, '.')) == NULL)
			ptr = name;
		else
			ptr++;
		sprintf(buf, "%s %s", AN(ptr), ptr);
	}
	
	if (num > 1)
		sprintf(END_OF(buf), " (x %d)", num);
	return (buf);
}


struct obj_data *get_slide_obj_vis(struct char_data * ch, char *name,
	struct obj_data * list)
{
	struct obj_data *i, *last_match = 0;
	int j, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp;
	
	strcpy(tmpname, name);
	tmp = tmpname;
	if (!(number = get_number(&tmp)))
		return (0);
	
	for (i = list, j = 1; i && (j <= number); i = i->next_content)
		if (isname(tmp, i->name))
			if (CAN_SEE_OBJ(ch, i) && !same_obj(last_match, i)) {
				if (j == number)
					return (i);
				last_match = i;
				j++;
			}
	return (0);
}


struct obj_data *get_hash_obj_vis(struct char_data * ch, char *name,
	struct obj_data * list)
{
	struct obj_data *loop, *last_obj = 0;
	int index;
	
	if (is_number(name))
		index = atoi(name);
	else if (is_number(name + 1))
		index = atoi(name + 1);
	else
		return (0);
	
	for (loop = list; loop; loop = loop->next_content)
		if (CAN_SEE_OBJ(ch, loop) && (loop->obj_flags.cost > 0))
			if (!same_obj(last_obj, loop)) {
				if (--index == 0)
					return (loop);
				last_obj = loop;
			}
	return (0);
}


struct obj_data *get_purchase_obj(struct char_data * ch, char *arg,
	struct char_data * keeper, int shop_nr, int msg)
{
	char buf[MAX_STRING_LENGTH], name[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	
	one_argument(arg, name);
	do {
		if (*name == '#' || is_number(name))
			obj = get_hash_obj_vis(ch, name, keeper->carrying);
		else
			obj = get_slide_obj_vis(ch, name, keeper->carrying);
		if (!obj) {
			if (msg) {
				sprintf(buf, shop_index[shop_nr].no_such_item1, GET_NAME(ch));
				do_tell(keeper, buf, cmd_tell, 0);
			}
			return (0);
		}
		if (GET_OBJ_COST(obj) <= 0) {
			extract_obj(obj);
			obj = 0;
		}
	} while (!obj);
	return (obj);
}


int buy_price(struct obj_data * obj, int shop_nr)
{
	return ((int) (GET_OBJ_COST(obj) * SHOP_BUYPROFIT(shop_nr)));
}


void shopping_buy(char *arg, struct char_data * ch,
	struct char_data * keeper, int shop_nr)
{
	char tempstr[200], buf[MAX_STRING_LENGTH];
	struct obj_data *obj, *last_obj = NULL;
	int goldamt = 0, buynum, bought = 0,buy_pr=0,trade_mod=0;
	
	if (!(is_ok(keeper, ch, shop_nr)))
		return;
	
	if (SHOP_NO_BUY(shop_nr))
	{
		sprintf(buf, "%s Non vendo nulla.",
			GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	
	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);
	
	if ((buynum = transaction_amt(arg)) < 0)
	{
		sprintf(buf, "%s Un numero negativo ?  Prova a vendermi (sell) qualcosa.", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	
	if (!(*arg) || !(buynum))
	{
		sprintf(buf, "%s Cosa vuoi comprare ??", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	
	if (!(obj = get_purchase_obj(ch, arg, keeper, shop_nr, TRUE)))
		return;
		
	if(keeper->carrying == NULL) {  // Ardanos : Se in questo punto il keeper ha inventario vuoto,
	                                // poco piu' avanti crasha (perche' l'inventario ora non dovrebbe
	                                // esser vuoto, eppure a volte lo e') (soprattutto al fornaio di
	                                // lamorka, vnum 6653). Meglio mettere un check!
		do_tell(keeper,shop_index[shop_nr].no_such_item1,cmd_tell,0);
		sprintf(buf, " (BUG) Al negozio %d il keeper ha l'inventario vuoto!", world[keeper->in_room].number);
		mudlog(buf, CMP, LVL_GOD, FALSE);
		return;
	}
	
	buy_pr=(int)(buy_price(obj, shop_nr));
	trade_mod=(int)check_gen_abil(ch,10);
	trade_mod=(trade_mod*buy_pr)/100;
	if (((buy_pr-trade_mod) > GET_GOLD(ch)) && !IS_GOD(ch))
	{
		sprintf(buf, shop_index[shop_nr].missing_cash2, GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		
		switch (SHOP_BROKE_TEMPER(shop_nr))
		{
			case 0:
				do_action(keeper, GET_NAME(ch), cmd_puke, 0);
				return;
			case 1:
				do_echo(keeper, "fuma la sua pipa.", cmd_emote, SCMD_EMOTE);
				return;
			default:
				return;
		}
	}
	
	if ((IS_CARRYING_N(ch) + 1 > CAN_CARRY_N(ch)))
	{
		sprintf(buf, "%s: Non puoi portare altri oggetti oltre a quelli che gia' hai.\n\r",
			fname(obj->name));
		send_to_char(buf, ch);
		return;
	}
	
	if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) > CAN_CARRY_W(ch)) {
		sprintf(buf, "%s: Non puoi portare cosi' tanto peso.\n\r",
			fname(obj->name));
		send_to_char(buf, ch);
		return;
	}
	
	if (invalid_class(ch,obj))
	{
		act("$p non si addice a un membro del tuo ordine.", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
	if (invalid_align(ch,obj))
	{
		act("$p non si addice a uno del tuo allineamento.", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
	if  ((find_clan_by_id(GET_CLAN(ch))>-1) && (find_clan_by_id(SHOP_CLAN(shop_nr))>-1) &&
		(clan[find_clan_by_id(SHOP_CLAN(shop_nr))].diplomacy[find_clan_by_id(GET_CLAN(ch))].status==WAR))
	{
		act("Non vendo niente a quelli del tuo clan!", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
	if (!IS_NPC(ch) && ((GET_LEVEL(ch)<GET_OBJ_LEVEL(obj)) || ( GET_OBJ_LEVEL(obj)<=0 && (GET_LEVEL(ch)<objlevel(obj))) ) )
	{
		act("Sei troppo inesperto per poter comprare $p, rinunci.", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	if (!IS_NPC(ch) && (GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && (!can_take_container(ch,obj)))
	{
		act("All'interno di $p si trova qualcosa di troppo potente per poter essere comprato da te.", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
	while ((obj) && ((GET_GOLD(ch) >= (buy_pr-trade_mod)) || IS_GOD(ch))
		&& (IS_CARRYING_N(ch) < CAN_CARRY_N(ch)) && (bought < buynum)
		&& (IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj) <= CAN_CARRY_W(ch)))
	{
		bought++;
      /* Test if producing shop ! */
		if (shop_producing(obj, shop_nr)) {
			obj = read_object(GET_OBJ_RNUM(obj), REAL);
			load_otrigger(obj); // Orione
		}
		else
		{
			obj_from_char(obj);
			SHOP_SORT(shop_nr)--;
		}
		obj_to_char(obj, ch);
		
		goldamt += buy_pr-trade_mod;
		if (!IS_GOD(ch))
			GET_GOLD(ch) -= buy_pr-trade_mod;
		
		last_obj = obj;
		obj = get_purchase_obj(ch, arg, keeper, shop_nr, FALSE);
		if (!same_obj(obj, last_obj))
			break;
	}
	
	if (bought < buynum) {
		if (!obj || !same_obj(last_obj, obj))
			sprintf(buf, "%s Ho solo %d da venderti.", GET_NAME(ch), bought);
		else if (GET_GOLD(ch) < buy_pr-trade_mod)
			sprintf(buf, "%s Puoi solo permetterti %d.", GET_NAME(ch), bought);
		else if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
			sprintf(buf, "%s Puoi solo portare %d.", GET_NAME(ch), bought);
		else if (IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj) > CAN_CARRY_W(ch))
			sprintf(buf, "%s Puoi solo portare %d.", GET_NAME(ch), bought);
		else
			sprintf(buf, "%s Something screwy only gave you %d.", GET_NAME(ch),
				bought);
		do_tell(keeper, buf, cmd_tell, 0);
	}
	if (!IS_GOD(ch))
		GET_GOLD(keeper) += goldamt;
	if (trade_mod>0){
		sprintf(buf,"&5Contratti accanitamente con $N&0");
		act(buf,FALSE,ch,0,keeper,TO_CHAR);
		sprintf(buf,"&5$n contratta accanitamente con $N&0");
		act(buf,FALSE,ch,0,keeper,TO_NOTVICT);
	}
	sprintf(tempstr, times_message(ch->carrying, 0, bought));
	sprintf(buf, "$n compra %s.", tempstr);
	act(buf, FALSE, ch, obj, 0, TO_ROOM);
	
	sprintf(buf, shop_index[shop_nr].message_buy, GET_NAME(ch),buy_pr);
	do_tell(keeper, buf, cmd_tell, 0);
	if((int)trade_mod>=1){
		sprintf(buf,"$n dice:&3Siccome mi sei simpatico tel lo faccio pagare %d monete..&0",goldamt);
		act(buf,FALSE,keeper,0,0,TO_ROOM);
	}
	sprintf(buf, "Ora hai %s.\n\r", tempstr);
	send_to_char(buf, ch);
	
	if (SHOP_USES_BANK(shop_nr))
		if (GET_GOLD(keeper) > MAX_OUTSIDE_BANK) {
			SHOP_BANK(shop_nr) += (GET_GOLD(keeper) - MAX_OUTSIDE_BANK);
			GET_GOLD(keeper) = MAX_OUTSIDE_BANK;
		}
}


struct obj_data *get_selling_obj(struct char_data * ch, char *name,
	struct char_data * keeper, int shop_nr, int msg)
{
	char buf[MAX_STRING_LENGTH];
	struct obj_data *obj;
	int result;
	
	if (!(obj = get_obj_in_list_vis(ch, name, ch->carrying))) {
		if (msg) {
			sprintf(buf, shop_index[shop_nr].no_such_item2, GET_NAME(ch));
			do_tell(keeper, buf, cmd_tell, 0);
		}
		return (0);
	}
	if(ROOM_FLAGGED(ch->in_room, ROOM_ZECCA)){
		if(GET_OBJ_TYPE(obj) != ITEM_LINGOTTO && GET_OBJ_TYPE(obj) != ITEM_MET_GREZZO){
			send_to_char("Qui siamo in una zecca e accetto solo lingotti o pietre,d'oro o d'argento!\r\n",ch);
			return(0);
		}
		if((GET_OBJ_MAT_NUM(obj) != 1 && GET_OBJ_MAT_NUM(obj) != 5)){
			send_to_char("Puoi vendermi Oro o Argento,null'altro.\n\r",ch);
			return (0);
		}
	}
	if ((result = trade_with(obj, shop_nr)) == OBJECT_OK)
		return (obj);
	
	switch (result) {
		case OBJECT_NOTOK:
			sprintf(buf, shop_index[shop_nr].do_not_buy, GET_NAME(ch));
			break;
		case OBJECT_DEAD:
			sprintf(buf, "%s %s", GET_NAME(ch), MSG_NO_USED_WANDSTAFF);
			break;
		default:
			sprintf(buf, "Illegal return value of %d from trade_with() (shop.c)",
				result);
			log(buf);
			sprintf(buf, "%s An error has occurred.", GET_NAME(ch));
			break;
	}
	if (msg)
		do_tell(keeper, buf, cmd_tell, 0);
	return (0);
}


int sell_price(struct char_data * ch, struct obj_data * obj, int shop_nr)
{
	return ((int) (GET_OBJ_COST(obj) * SHOP_SELLPROFIT(shop_nr)));
}


struct obj_data *slide_obj(struct obj_data * obj, struct char_data * keeper,
	int shop_nr)
/*
   This function is a slight hack!  To make sure that duplicate items are
   only listed once on the "list", this function groups "identical"
   objects together on the shopkeeper's inventory list.  The hack involves
   knowing how the list is put together, and manipulating the order of
   the objects on the list.  (But since most of DIKU is not encapsulated,
   and information hiding is almost never used, it isn't that big a deal) -JF
*/
{
	struct obj_data *loop;
	int temp;
	
	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);
	
  /* Extract the object if it is identical to one produced */
	if (shop_producing(obj, shop_nr)) {
		temp = GET_OBJ_RNUM(obj);
		extract_obj(obj);
		return (&obj_proto[temp]);
	}
	SHOP_SORT(shop_nr)++;
	loop = keeper->carrying;
	obj_to_char(obj, keeper);
	keeper->carrying = loop;
	while (loop) {
		if (same_obj(obj, loop)) {
			obj->next_content = loop->next_content;
			loop->next_content = obj;
			return (obj);
		}
		loop = loop->next_content;
	}
	keeper->carrying = obj;
	return (obj);
}


void sort_keeper_objs(struct char_data * keeper, int shop_nr)
{
	struct obj_data *list = 0, *temp;
	if (keeper->carrying==NULL)
		return;
	while (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper)) {
		temp = keeper->carrying;
		obj_from_char(temp);
		if (!temp)
			return;
		temp->next_content = list;
		list = temp;
	}
	
	while (list) {
		temp = list;
		list = list->next_content;
		if ((shop_producing(temp, shop_nr)) &&
			!(get_obj_in_list_num(GET_OBJ_RNUM(temp), keeper->carrying))) {
			obj_to_char(temp, keeper);
			SHOP_SORT(shop_nr)++;
		} else
			(void) slide_obj(temp, keeper, shop_nr);
	}
}


void shopping_sell(char *arg, struct char_data * ch,
	struct char_data * keeper, int shop_nr)
{
	char tempstr[200], buf[MAX_STRING_LENGTH], name[200];
	struct obj_data *obj=NULL, *tag = 0;
	int sellnum, sold = 0, goldamt = 0,trade_mod=0,sell_pr=0;
	
	if (!(is_ok(keeper, ch, shop_nr)))
		return;
	
	if (SHOP_NO_BUY(shop_nr)) {
		sprintf(buf, "%s Non compro nulla.", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	
	if (   (find_clan_by_id(GET_CLAN(ch)) > -1) 
	    && (find_clan_by_id(SHOP_CLAN(shop_nr)) > -1) 
	    && (clan[find_clan_by_id(SHOP_CLAN(shop_nr))].diplomacy[find_clan_by_id(GET_CLAN(ch))].status == WAR) ) {
		act("Non compro niente da quelli del tuo clan!", FALSE, ch, obj, 0, TO_CHAR);
		return;
	}
	
	if ((sellnum = transaction_amt(arg)) < 0) {
		sprintf(buf, "%s A negative amount?  Try buying something.", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	
	if (!(*arg) || !(sellnum)) {
		sprintf(buf, "%s Cosa vuoi vendere??", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	
	one_argument(arg, name);
	if (!(obj = get_selling_obj(ch, name, keeper, shop_nr, TRUE)))
		return;
	
	sell_pr = sell_price(ch, obj, shop_nr);
	trade_mod = check_gen_abil(ch, 10);
	trade_mod = trade_mod*sell_pr/100;

	if (GET_GOLD(keeper) + SHOP_BANK(shop_nr) < sell_pr+trade_mod) {
		sprintf(buf, shop_index[shop_nr].missing_cash1, GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}

	while (   (obj) && (GET_GOLD(keeper) + SHOP_BANK(shop_nr) >= sell_pr+trade_mod) 
	       && (sold < sellnum)) {
		
		sell_pr = sell_price(ch, obj, shop_nr);
        trade_mod = check_gen_abil(ch,10);
        trade_mod = trade_mod*sell_pr/100;

		sold++;
		
		goldamt += sell_pr+trade_mod;
		GET_GOLD(keeper) -= sell_pr+trade_mod;
		
		obj_from_char(obj);
		tag = slide_obj(obj, keeper, shop_nr);
		obj = get_selling_obj(ch, name, keeper, shop_nr, FALSE);
	}
	
	if (sold < sellnum) {
		if (!obj)
			sprintf(buf, "%s Hai solo %d di quelli.", GET_NAME(ch), sold);
		else if (GET_GOLD(keeper) + SHOP_BANK(shop_nr) <
			sell_pr+trade_mod)
			sprintf(buf, "%s Non posso permettermi %d di quelli.",
				GET_NAME(ch), sold);
		else
			sprintf(buf, "%s Something really screwy made me buy %d.",
				GET_NAME(ch), sold);
		
		do_tell(keeper, buf, cmd_tell, 0);
	}
	
	GET_GOLD(ch) += goldamt;
	strcpy(tempstr, times_message(0, name, sold));
	
	if(check_gen_abil(ch,10) > 0) {
		act("$n contratta furiosamente con $N",FALSE,ch,0,keeper,TO_ROOM);
		sprintf(buf,"&6%s sei veramente uno sporco strozzino!&0",GET_NAME(ch));
		do_say(keeper,buf,0,0);
		//act(buf,TRUE,keeper,0,0,TO_ROOM);
	}
	sprintf(buf, "$n vende %s.", tempstr);
	act(buf, FALSE, ch, obj, 0, TO_ROOM);
	sprintf(buf, shop_index[shop_nr].message_sell, GET_NAME(ch), goldamt);
	do_tell(keeper, buf, cmd_tell, 0);
	sprintf(buf, "Il negoziante ora ha %s.\n\r", tempstr);
	send_to_char(buf, ch);

	if (GET_GOLD(keeper) < MIN_OUTSIDE_BANK) {
		goldamt = MIN(MAX_OUTSIDE_BANK - GET_GOLD(keeper), SHOP_BANK(shop_nr));
		SHOP_BANK(shop_nr) -= goldamt;
		GET_GOLD(keeper) += goldamt;
	}
	put_obj_to_mag(shop_nr, keeper, MAX_SHOP_OBJ);
	make_obj_gold(real_room(SHOP_VALORE1(shop_nr)), shop_nr);
}


void shopping_value(char *arg, struct char_data * ch,
	struct char_data * keeper, int shop_nr)
{
	char buf[MAX_STRING_LENGTH];
	struct obj_data *obj;
	char name[MAX_INPUT_LENGTH];
	int trade_mod=0,sell_pr=0;
	if (!(is_ok(keeper, ch, shop_nr)))
		return;
	if (!(*arg)) {
		sprintf(buf, "%s Cosa ti devo valutare??", GET_NAME(ch));
		do_tell(keeper, buf, cmd_tell, 0);
		return;
	}
	one_argument(arg, name);
	if (!(obj = get_selling_obj(ch, name, keeper, shop_nr, TRUE)))
		return;
	trade_mod=check_gen_abil(ch,10);
	trade_mod=trade_mod*(sell_pr=sell_price(ch, obj, shop_nr))/100;
	if(check_gen_abil(ch,10)>0){
		act("&6Contratti accaniamente con $N",FALSE,ch,0,keeper,TO_CHAR);
		act("$n contratta accanitamente con $N",FALSE,ch,0,keeper,TO_ROOM);
	}
	sprintf(buf, "%s Ti daro' %d coins per quello!", GET_NAME(ch),
		sell_pr+trade_mod);
	do_tell(keeper, buf, cmd_tell, 0);
	
	return;
}

char* armor_quality(struct obj_data *obj){
	static char quality[20];
	if( GET_OBJ_VAL(obj,0)>=15)
		sprintf(quality,"&6Piastre     &0");
	else if( GET_OBJ_VAL(obj,0)>7)
		sprintf(quality,"&4Media       &0");
	else
		sprintf(quality,"&1Cuoio       &0");
	return quality;
}

char* weapon_quality(struct obj_data *obj){
	static char quality[20];
	int med_dam=GET_OBJ_VAL(obj,1) * (GET_OBJ_VAL(obj,2)+1) /2;
	if(med_dam>=35)
		sprintf(quality,"&1&bMicidiale   &0");
	else if(med_dam>=30)
		sprintf(quality,"&5&bMortifero   &0");
	else if(med_dam>=25)
		sprintf(quality,"&4&bAltissimo   &0");
	
	else if(med_dam>=20)
		sprintf(quality,"&4Alto        &0");
	else if(med_dam>=15)
		sprintf(quality,"&5&bMedio-Alto  &0");
	else if(med_dam>=12)
		sprintf(quality,"&2&bMedio       &0");
	else if(med_dam>=8)
		sprintf(quality,"&2Medio-Basso &0");
	else if(med_dam>=5)
		sprintf(quality,"&3&bBasso       &0");
	else
		sprintf(quality,"&3Ridicolo    &0");
	return quality;
}

char *list_object(struct obj_data * obj, int cnt, int index, int shop_nr)
{
	static char buf[256];
	char buf2[300], buf3[200],buf4[200];
	extern char *drinks[];
	int level=0;
	if (shop_producing(obj, shop_nr))
		strcpy(buf2, "   &1&bU&0");
	else
		sprintf(buf2, "%4d", cnt);
	sprintf(buf, "%2d)%s ", index, buf2);
	
  /* Compile object name and information */
	strcpy(buf3, obj->short_description);
	if ((GET_OBJ_TYPE(obj) == ITEM_DRINKCON) && (GET_OBJ_VAL(obj, 1)))
		sprintf(END_OF(buf3), " di  %s", drinks[GET_OBJ_VAL(obj, 2)]);
	if(GET_OBJ_TYPE(obj) ==ITEM_ARMOR)
		sprintf(buf4,armor_quality(obj));
	else if ((GET_OBJ_TYPE(obj) ==ITEM_WEAPON) || (GET_OBJ_TYPE(obj)==ITEM_WEAPON_2HANDS))
		sprintf(buf4,weapon_quality(obj));
	else
		sprintf(buf4,"Nulla       ");
  /* FUTURE: */
  /* Add glow/hum/etc */
	if(GET_OBJ_LEVEL(obj)>0)
		level=GET_OBJ_LEVEL(obj);
	else
		level=objlevel(obj);
	
	if ((GET_OBJ_TYPE(obj) == ITEM_WAND) || (GET_OBJ_TYPE(obj) == ITEM_STAFF))
		if (GET_OBJ_VAL(obj, 2) < GET_OBJ_VAL(obj, 1))
			strcat(buf3, " (parzialmente usata)");
		
	sprintf(buf2, " %-42s %-6s %d %6d\n\r", buf3,buf4,level, buy_price(obj, shop_nr));
	strcat(buf, CAP(buf2));
	return (buf);
}

void approv_shop()
{
	long shop_nr,cl, shopcoins =0 ;
	struct char_data *keeper;
	extern struct clan_rec clan[MAX_CLANS];
	int entrate;
	
	mudlog("Approvvigionamento negozi e vendita oggetti", CMP, LVL_GOD, FALSE);

	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
		if ((SHOP_KEEPER(shop_nr)>0) && (real_mobile(SHOP_KEEPER(shop_nr))))
		{
			if ((keeper = get_char_num(SHOP_KEEPER(shop_nr)))) {
				shopcoins = SHOP_VALORE(shop_nr);
				
				//Approvvigiona COINS
				if (   (SHOP_CLAN(shop_nr)>-1) 
				    && (find_clan_by_id(SHOP_CLAN(shop_nr))>-1) ) {
				    //&& ((GET_GOLD(keeper) + SHOP_BANK(shop_nr))<shopcoins)
				    	
					cl = find_clan_by_id(SHOP_CLAN(shop_nr));
					clan[cl].economy.current_shops_balance += (GET_GOLD(keeper) + SHOP_BANK(shop_nr));
					clan[cl].economy.treasure += (GET_GOLD(keeper) + SHOP_BANK(shop_nr));
					entrate = (GET_GOLD(keeper) + SHOP_BANK(shop_nr));
					GET_GOLD(keeper) = 0;		

					if (clan[cl].economy.treasure < shopcoins) {
						if (clan[cl].economy.treasure <= 0) shopcoins = 0;
						else shopcoins = clan[cl].economy.treasure;
					}	

					//sprintf(buf,"Approvvigionamento del negozio di %s con %ld coins (%s)",
					//	GET_NAME(&mob_proto[SHOP_KEEPER(shop_nr)]),
					//	shopcoins, clan[cl].name);
					SHOP_BANK(shop_nr) =  shopcoins;

					clan[cl].economy.current_shops_balance -= shopcoins;
					clan[cl].economy.treasure -= shopcoins;

					if (entrate >= shopcoins)
						clan[cl].economy.current_entrate += entrate - shopcoins;
					else
						clan[cl].economy.current_uscite += shopcoins - entrate;			
					//mudlog(buf, CMP, LVL_GOD, FALSE);
				}
				/*
				//Approvvigiona OBJ
				if ((SHOP_VALORE1(shop_nr)>0) && ((mag = real_room(SHOP_VALORE1(shop_nr)))!=NOWHERE))
				{
					put_obj_to_mag(shop_nr, keeper, MAX_SHOP_OBJ);
					act("$n riporta tutto in magazzino.", TRUE, keeper, 0, 0, TO_ROOM);
					//get_obj_from_mag(shop_nr, keeper, SHOP_VALORE2(shop_nr));
					SET_BIT(ROOM_FLAGS(mag), ROOM_HOUSE_CRASH);			
					House_crashsave(world[mag].number);
					if (ROOM_FLAGGED(mag,ROOM_MAGAZZINO))
						make_obj_gold(mag,shop_nr);
				} 
		
			*/
			}//Kepper
		} 
  // perform_make_obj_gold();
}

long num_obj_in_mag (room_rnum room)
{
	long count = 0;
	struct obj_data *object;
	
	if (room<0 || room>top_of_world) return 0;
	
	for (object=world[room].contents; object; object=object->next_content)
	{
		if (object->contains)
			count += num_obj_in_obj_mag(object);
		if (GET_OBJ_TYPE(object)!=ITEM_MONEY) count ++;
	}
	return count;
}

long num_same_obj_in_obj (struct obj_data * obj, struct obj_data * object)
{
	long count=0;
	struct obj_data *cont;
	
	for (cont=object->contains; cont; cont=cont->next_content)
	{
		if (cont->contains) count += num_same_obj_in_obj(obj, cont);
		if ((GET_OBJ_MAT_TYPE(cont) == GET_OBJ_MAT_TYPE(obj))&&
			(GET_OBJ_MAT_NUM(cont)  == GET_OBJ_MAT_NUM(obj)) &&
			(same_obj(cont,obj))
			)
			count ++;
	}
	return count;
}

long num_same_obj_in_mag (struct obj_data *obj ,room_rnum room)
{
	long count = 0;
	struct obj_data *object;
	
	if (room<0 || room>top_of_world) return 0;
	
	for (object=world[room].contents; object; object=object->next_content)
	{
		if (object->contains)
			count += num_same_obj_in_obj(obj,object);
		if ((GET_OBJ_MAT_TYPE(object) == GET_OBJ_MAT_TYPE(obj))&&
			(GET_OBJ_MAT_NUM(object)  == GET_OBJ_MAT_NUM(obj)) &&
			(same_obj(object,obj))
			)
			count ++;
	}
	return count;
}


long num_obj_in_obj_mag (struct obj_data * object)
{
	long count=0;
	struct obj_data *cont;
	
	for (cont=object->contains; cont; cont=cont->next_content)
	{
		if (cont->contains)
			count += num_obj_in_obj_mag(cont);
		count++;
	}
	return count;
}

long get_num_shop_from_mag(long vnum)
{
	long shop_nr;
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
		if ((real_room(SHOP_VALORE1(shop_nr))!=NOWHERE) && 
			(vnum == SHOP_VALORE1(shop_nr)))	   
			return shop_nr;	   
	return -1;	   
}

void perform_make_obj_gold()
{
	long mag,current_room,i;
	extern struct zone_data *zone_table;
	extern int top_of_zone_table;
	
	for (i = 0; i <= top_of_zone_table; i++)
		for (current_room = (zone_table[i].number * 100); current_room <= zone_table[i].top; current_room++) {
			if (((current_room != NOWHERE)&&(mag = real_room(current_room))!=NOWHERE)&&(ROOM_FLAGGED(mag,ROOM_MAGAZZINO))){
				make_obj_gold(mag,get_num_shop_from_mag(mag));
			} 
		}
}


void make_obj_gold(long mag,long shop_nr)
{
	long num_obj,num_same_obj,num_max_obj,cost=0,venduti=0,cl=-1,addebitati = 0;
	struct obj_data *obj,*next,*temp = NULL,*money = NULL;
	extern struct obj_data *create_money(int amount);
	extern void save_clans();
	
	num_obj = num_obj_in_mag(mag);
	if (num_obj>MAX_OBJ_IN_MAG+20)
	{   	     
		
		while (num_obj_in_mag(mag)>MAX_OBJ_IN_MAG) {
			temp = NULL;
			num_max_obj = 0;		   
			for (obj = world[mag].contents; obj; obj = next) {
				next = obj->next_content;
				if (GET_OBJ_TYPE(obj)!=ITEM_MONEY){
					num_same_obj = num_same_obj_in_mag(obj,mag);
					if ((num_same_obj > num_max_obj) || !temp)
					{
						num_max_obj = num_same_obj;
						temp = obj;
					}
				}
			} //for
			if (temp){
				cost += GET_OBJ_COST(temp);
				venduti++;
				if ((shop_nr != -1) && (SHOP_CLAN(shop_nr)>-1) && ((cl = find_clan_by_id(SHOP_CLAN(shop_nr)))>-1)) {
					clan[cl].economy.current_shops_balance += GET_OBJ_COST(temp);
					clan[cl].economy.treasure += GET_OBJ_COST(temp);
					clan[cl].economy.current_entrate += GET_OBJ_COST(temp);
					addebitati += GET_OBJ_COST(temp);
				} else {
					money = create_money(GET_OBJ_COST(temp));
					obj_to_room(money,mag);
				}
				obj_from_room(temp);
				extract_obj(temp);
			}
		}//while
		save_clans();
		SET_BIT(ROOM_FLAGS(mag), ROOM_HOUSE_CRASH);				 
		if (ROOM_FLAGGED(mag,ROOM_HOUSE))
			House_crashsave(world[mag].number);
		sprintf(buf,"(REGNI) Il magazzino %d vende %ld oggetti per %ld coins di cui %ld addebitati al regno %s.",world[mag].number ,venduti,cost,addebitati,clan[cl].name);
		mudlog(buf, CMP, LVL_GOD, FALSE);
	} //if
}


void get_obj_from_mag(int shop_nr, struct char_data * keeper, int qta)
{
	long rroom;
	struct obj_data *obj,*next,*temp;
	int i=0,counter;
	
	if (SHOP_VALORE1(shop_nr)>0)
		if ((rroom = real_room(SHOP_VALORE1(shop_nr)))!= NOWHERE)
			for (counter = 0; SHOP_BUYTYPE(shop_nr, counter) != NOTHING; counter++){
				i=0;
				for (obj = world[rroom].contents; obj && i<qta; obj = next) {
					next = obj->next_content;				
					if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(obj))
						if (trade_with(obj, shop_nr) == OBJECT_OK)
							if ((!(temp = get_obj_in_list_vis(keeper, obj->name, keeper->carrying))) ||
								((temp) &&
									((GET_OBJ_MAT_TYPE(temp) != GET_OBJ_MAT_TYPE(obj)) ||
										(GET_OBJ_MAT_NUM(temp)  != GET_OBJ_MAT_NUM(obj)) ||
										(!same_obj(temp,obj))
									)
								)
								){
								i++;
								obj_from_room(obj);
								obj_to_char(obj, keeper);
							}
				}
				
				if (i<qta)
					for (obj = world[rroom].contents; obj && i<qta; obj = next) {
						next = obj->next_content;
						if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(obj))
							if (trade_with(obj, shop_nr)==OBJECT_OK) {
								i++;
								obj_from_room(obj);
								obj_to_char(obj, keeper);
							}
					}
					
			}  //for type
}

void put_obj_to_mag(int shop_nr, struct char_data * keeper,int qta)
{
	long rroom;
	struct obj_data *obj,*next;
	int i=0;
	
	if (!keeper) return;
	if (SHOP_VALORE1(shop_nr)>0)
		if ((rroom = real_room(SHOP_VALORE1(shop_nr)))!= NOWHERE)
			if (keeper->carrying) {
				for (obj = keeper->carrying; obj && i<qta; obj = next) {
					i++;
					next = obj->next_content;
					obj_from_char(obj);
					obj_to_room(obj,rroom);
				}
				SHOP_SORT(shop_nr) = 0;
			}
}


void shopping_list(char *arg, struct char_data * ch,
	struct char_data * keeper, int shop_nr)
{
	char buf[MAX_STRING_LENGTH], name[200];
	struct obj_data *obj, *last_obj = 0;
	int cnt = 0, index = 0;  
	
	if (!keeper)
		return;

	if (!(is_ok(keeper, ch, shop_nr)))
		return;
	
	one_argument(arg, name);
	
	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);
	
	strcpy(buf,
		" ## Disp. Oggetto                                  Danno M./Ac  Liv Costo\n\r");
	strcat(buf,
		"&1----------------------------------------------------------------------------&0\n\r");
	if (keeper->carrying)
		for (obj = keeper->carrying; obj && strlen (buf) < 7500; obj = obj->next_content)
			if (ch && CAN_SEE_OBJ(ch, obj) && (obj->obj_flags.cost > 0)) {
				if (!last_obj) {
					last_obj = obj;
					cnt = 1;
				} else if (same_obj(last_obj, obj))
					cnt++;
				else {
					index++;
					if (!(*name) || isname(name, last_obj->name))
						strcat(buf, list_object(last_obj, cnt, index, shop_nr));
					cnt = 1;
					last_obj = obj;
				}
			}
	index++;
	if (!last_obj)
		if (*name)
			strcpy(buf, "Al momento quello non e' disponibile.\n\r");
		else
			strcpy(buf, "Al momento non e' in vendita niente.\n\r");
	else if (!(*name) || isname(name, last_obj->name))
		strcat(buf, list_object(last_obj, cnt, index, shop_nr));
	strcat(buf,
		"&1----------------------------------------------------------------------------&0\n\r");
	page_string(ch->desc, buf, 1);
	
}


int ok_shop_room(int shop_nr, int room)
{
	int index;
	
	for (index = 0; SHOP_ROOM(shop_nr, index) != NOWHERE; index++)
		if (SHOP_ROOM(shop_nr, index) == room)
			return (TRUE);
	return (FALSE);
}


SPECIAL(shop_keeper)
{
	char argm[MAX_INPUT_LENGTH];
	struct char_data *keeper = (struct char_data *) me;
	int shop_nr,idclan;
	
	if ((!keeper) || (!ch)) return FALSE;
	
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
  if (SHOP_KEEPER(shop_nr) == keeper->nr)
	  break;
		
	if (shop_nr >= top_shop)
		return (FALSE);
	
	if (SHOP_FUNC(shop_nr))	/* Check secondary function */
	if ((SHOP_FUNC(shop_nr)) (ch, me, cmd, arg))
		return (TRUE);
		
	if (keeper == ch) {
		if (cmd)
			SHOP_SORT(shop_nr) = 0;	/* Safety in case "drop all" */
		return (FALSE);
	}
	
	if (!ok_shop_room(shop_nr, world[ch->in_room].number))
		return (0);
	
	if (!AWAKE(keeper))
		return (FALSE);
	
	if (CMD_IS("steal") || CMD_IS("ruba")) {
		sprintf(argm, "$N grida '%s'", MSG_NO_STEAL_HERE);
		do_action(keeper, GET_NAME(ch), cmd_slap, 0);
		act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		return (TRUE);
	}
	
	if (CMD_IS("incassa")) {
		if (IsYourShop(ch, shop_nr, 0)) {
			sprintf(buf,"(REGNI) %s incassa %d coins dal negozio %d.",GET_NAME(ch),(GET_GOLD(keeper)+SHOP_BANK(shop_nr)),SHOP_NUM(shop_nr));
			mudlog(buf,NRM,LVL_GOD,TRUE);
			GET_GOLD(ch) += GET_GOLD(keeper) + SHOP_BANK(shop_nr);
			GET_GOLD(keeper)=0;
			SHOP_BANK(shop_nr)=0;
			sprintf(argm, "Ecco tutto il ricavato di oggi, il resto e' depositato in banca.");
		}
		else 
		  sprintf(argm, "Ehi, vai a lavorare fannullone!");
		act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		return (TRUE);
	}
	
	if (CMD_IS("saldo")) {
		if (IsYourShop(ch , shop_nr, 0))
			sprintf(argm, "Il ricavato complessivo e' di %d coin.", SHOP_BANK(shop_nr)+GET_GOLD(keeper));
		else
			sprintf(argm, "Ehi, vai a lavorare fannullone!");
		act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		return (TRUE);
	}
	
	if (CMD_IS("bonifico")) {
		if (   (GET_CLAN(ch) != -1) 
		    && (SHOP_CLAN(shop_nr) == GET_CLAN(ch)) 
		    && (IS_SET(GET_CLAN_PRIV(ch), CP_MNGSHOP)) ) {
			idclan = find_clan_by_id(GET_CLAN(ch));
			if (idclan == -1)
				sprintf(argm, "Clan non trovato contattare un creatore.");
			else {
				sprintf(buf,"(REGNI) %s effetua un bonifico di %d coins dal negozio %d al regno %s.",GET_NAME(ch),SHOP_BANK(shop_nr),SHOP_NUM(shop_nr),clan[idclan].name);
				mudlog(buf,NRM,LVL_GOD,TRUE);
				clan[idclan].economy.treasure += SHOP_BANK(shop_nr);
				SHOP_BANK(shop_nr)=0;
				sprintf(argm, "Ho dato istruzioni alla banca per la transazione.");
			}
		}
		else 
		if (SHOP_PROPRIETARIO(shop_nr) == GET_IDNUM(ch)) {
			GET_BANK_GOLD(ch) += SHOP_BANK(shop_nr) + GET_GOLD(keeper);
			SHOP_BANK(shop_nr)=0;
			GET_GOLD(keeper)=0;
			sprintf(argm, "Ho dato istruzioni alla banca per la transazione.");
		}		
		else
			sprintf(argm, "E perche' mai devo dire alla banca di spedirti dei soldi?");
		act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		return (TRUE);
	}
	
	if (CMD_IS("buy") || CMD_IS("compra")) {
		if (ROOM_FLAGGED(ch->in_room, ROOM_NOBUY)) {
			send_to_char("Qui non Compriamo nulla!\r\n",ch);
			return (TRUE);
		}
		shopping_buy(argument, ch, keeper, shop_nr);
		return (TRUE);
	}
	if (CMD_IS("sell") || CMD_IS("vendi")) {
		if (ROOM_FLAGGED(ch->in_room,ROOM_NOSELL)) {
			send_to_char("Qui non Vendiamo nulla,Compriamo e basta!\r\n",ch);
			return (TRUE);
		}
		shopping_sell(argument, ch, keeper, shop_nr);
		return (TRUE);
	}
	if (CMD_IS("value") || CMD_IS("valuta")) {
		shopping_value(argument, ch, keeper, shop_nr);
		return (TRUE);
	}
	if (CMD_IS("lista")) {
		list_prod(ch, shop_nr, argument , keeper);
		//shopping_list(argument, ch, keeper, shop_nr);
		return (TRUE);
	}
	if (CMD_IS("prodotti")) {
		show_prodotti(shop_nr,ch,keeper);
		return (TRUE);
	}
	if (CMD_IS("conia")) {
		if (!ROOM_FLAGGED(ch->in_room, ROOM_ZECCA)) {
			send_to_char("Non siamo in una Zecca!",ch);
			return (TRUE);
		}
		
		if (   (GET_CLAN(ch)!=-1) 
		    && (SHOP_CLAN(shop_nr) == GET_CLAN(ch)) 
		    && (IS_SET(GET_CLAN_PRIV(ch),CP_ZECCA_EXP)) ) {
			idclan = find_clan_by_id(GET_CLAN(ch));
			if (idclan == -1) {
				sprintf(argm, "Clan non trovato contattare un creatore.");
				act(argm, FALSE, ch, 0, keeper, TO_CHAR);
			} 
			else
				shopping_exchange(argument,ch,keeper,shop_nr,1);
		} 
		else {
			sprintf(argm, "Fila via ,chi ti credi di essere!");
			act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		}
		return (TRUE);
	}
	
	if(CMD_IS("trade")) {
		if(!ROOM_FLAGGED(ch->in_room, ROOM_TRADE)){
			send_to_char("Non facciamo queste cose qui.",ch);
			return (TRUE);
		}
		if ((GET_CLAN(ch)!=-1) && (SHOP_CLAN(shop_nr) == GET_CLAN(ch)) &&
			(IS_SET(GET_CLAN_PRIV(ch),CP_ECONOMY_AQ))){
			idclan = find_clan_by_id(GET_CLAN(ch));
			if (idclan == -1) {
				sprintf(argm, "Clan non trovato contattare un creatore.");
				act(argm, FALSE, ch, 0, keeper, TO_CHAR);
			} else
				shopping_exchange(argument,ch,keeper,shop_nr,2);
		}
		else {
			sprintf(argm, "Fila via ,chi ti credi di essere!");
			act(argm, FALSE, ch, 0, keeper, TO_CHAR);
		}
		return (TRUE);
	}
	return (FALSE);
}

int ok_damage_shopkeeper(struct char_data * ch, struct char_data * victim)
{
	char buf[200];
	int index;
	
	if (IS_MOB(victim) && (mob_index[GET_MOB_RNUM(victim)].func == shop_keeper))
		for (index = 0; index < top_shop; index++)
			if ((GET_MOB_RNUM(victim) == SHOP_KEEPER(index)) && !SHOP_KILL_CHARS(index)) {
				do_action(victim, GET_NAME(ch), cmd_slap, 0);
				sprintf(buf, "%s %s", GET_NAME(ch), MSG_CANT_KILL_KEEPER);
				do_tell(victim, buf, cmd_tell, 0);
				return (FALSE);
			}
	return (TRUE);
}


int add_to_list(struct shop_buy_data * list, int type, int *len, int *val)
{
	if (*val >= 0){
		if (*len < MAX_SHOP_OBJ) {
			if (type == LIST_PRODUCE)
				*val = real_object(*val);
			if (*val >= 0) {
				BUY_TYPE(list[*len]) = *val;
				BUY_WORD(list[(*len)++]) = 0;
			} else
				*val = 0;
			return (FALSE);
		} else
			return (TRUE);
	}		
	return (FALSE);
}


int end_read_list(struct shop_buy_data * list, int len, int error)
{
	if (error) {
		sprintf(buf, "Raise MAX_SHOP_OBJ constant in shop.h to %d", len + error);
		log(buf);
	}
	BUY_WORD(list[len]) = 0;
	BUY_TYPE(list[len++]) = NOTHING;
	return (len);
}


void read_line(FILE * shop_f, char *string, void *data)
{
	if (!get_line(shop_f, buf) || !sscanf(buf, string, data)) {
		fprintf(stderr, "Error in shop #%d\n", SHOP_NUM(top_shop));
		exit(1);
	}
}


int read_list(FILE * shop_f, struct shop_buy_data * list, int new_format,
	int max, int type)
{
	int count, temp, len = 0, error = 0;
	
	if (new_format) {
		do {
			read_line(shop_f, "%d", &temp);
			error += add_to_list(list, type, &len, &temp);
		} while (temp >= 0);
	} else
		for (count = 0; count < max; count++) {
			read_line(shop_f, "%d", &temp);
			error += add_to_list(list, type, &len, &temp);
		}
	return (end_read_list(list, len, error));
}


int read_type_list(FILE * shop_f, struct shop_buy_data * list,
	int new_format, int max)
{
	int index, num, len = 0, error = 0;
	char *ptr;
	
	if (!new_format)
		return (read_list(shop_f, list, 0, max, LIST_TRADE));
	do {
		fgets(buf, MAX_STRING_LENGTH - 1, shop_f);
		if ((ptr = strchr(buf, ';')) != NULL)
			*ptr = 0;
		else
			*(END_OF(buf) - 1) = 0;
		for (index = 0, num = NOTHING; *item_types[index] != '\n'; index++)
			if (!strn_cmp(item_types[index], buf, strlen(item_types[index]))) {
				num = index;
				strcpy(buf, buf + strlen(item_types[index]));
				break;
			}
		ptr = buf;
		if (num == NOTHING) {
			sscanf(buf, "%d", &num);
			while (!isdigit(*ptr))
				ptr++;
			while (isdigit(*ptr))
				ptr++;
		}
		while (isspace(*ptr))
			ptr++;
		while (isspace(*(END_OF(ptr) - 1)))
			*(END_OF(ptr) - 1) = 0;
		error += add_to_list(list, LIST_TRADE, &len, &num);
		if (*ptr)
			BUY_WORD(list[len - 1]) = str_dup(ptr);
	} while (num >= 0);
	return (end_read_list(list, len, error));
}


void boot_the_shops(FILE * shop_f, char *filename, int rec_count)
{
	char *buf, buf2[150];
	int temp, count, new_format = 0;
	struct shop_buy_data list[MAX_SHOP_OBJ + 1];
	int done = 0;
	
	sprintf(buf2, "beginning of shop file %s", filename);
	
	while (!done) {
		buf = fread_string(shop_f, buf2);
		if (*buf == '#') {		/* New shop */
			sscanf(buf, "#%d\n", &temp);
			sprintf(buf2, "shop #%d in shop file %s", temp, filename);
			free(buf);		/* Plug memory leak! */
			if (!top_shop)
				CREATE(shop_index, struct shop_data, rec_count);
			
			SHOP_NUM(top_shop) = temp;
			temp = read_list(shop_f, list, new_format, MAX_PROD, LIST_PRODUCE);
			CREATE(shop_index[top_shop].producing, int, temp);
			for (count = 0; count < temp; count++)
				SHOP_PRODUCT(top_shop, count) = BUY_TYPE(list[count]);
			
			read_line(shop_f, "%f", &SHOP_BUYPROFIT(top_shop));
			read_line(shop_f, "%f", &SHOP_SELLPROFIT(top_shop));
			
			temp = read_type_list(shop_f, list, new_format, MAX_TRADE);
			CREATE(shop_index[top_shop].type, struct shop_buy_data, temp);
			for (count = 0; count < temp; count++) {
				SHOP_BUYTYPE(top_shop, count) = BUY_TYPE(list[count]);
				SHOP_BUYWORD(top_shop, count) = BUY_WORD(list[count]);
			}
			
			shop_index[top_shop].no_such_item1 = fread_string(shop_f, buf2);
			shop_index[top_shop].no_such_item2 = fread_string(shop_f, buf2);
			shop_index[top_shop].do_not_buy = fread_string(shop_f, buf2);
			shop_index[top_shop].missing_cash1 = fread_string(shop_f, buf2);
			shop_index[top_shop].missing_cash2 = fread_string(shop_f, buf2);
			shop_index[top_shop].message_buy = fread_string(shop_f, buf2);
			shop_index[top_shop].message_sell = fread_string(shop_f, buf2);
			read_line(shop_f, "%d", &SHOP_BROKE_TEMPER(top_shop));
			read_line(shop_f, "%d", &SHOP_BITVECTOR(top_shop));
			read_line(shop_f, "%d", &SHOP_KEEPER(top_shop));
			
			SHOP_KEEPER(top_shop) = real_mobile(SHOP_KEEPER(top_shop));
			read_line(shop_f, "%d", &SHOP_TRADE_WITH(top_shop));
			
			temp = read_list(shop_f, list, new_format, 1, LIST_ROOM);
			CREATE(shop_index[top_shop].in_room, int, temp);
			for (count = 0; count < temp; count++)
				SHOP_ROOM(top_shop, count) = BUY_TYPE(list[count]);
			
			read_line(shop_f, "%d", &SHOP_OPEN1(top_shop));
			read_line(shop_f, "%d", &SHOP_CLOSE1(top_shop));
			read_line(shop_f, "%d", &SHOP_OPEN2(top_shop));
			read_line(shop_f, "%d", &SHOP_CLOSE2(top_shop));
			
      /*new adriano*/
			read_line(shop_f, "%d", &SHOP_PROPRIETARIO(top_shop));
			read_line(shop_f, "%d", &SHOP_CLAN(top_shop));
			read_line(shop_f, "%d", &SHOP_VALORE(top_shop));
			read_line(shop_f, "%d", &SHOP_BANK(top_shop));
			read_line(shop_f, "%d", &SHOP_VALORE1(top_shop));
			read_line(shop_f, "%d", &SHOP_VALORE2(top_shop));
			read_line(shop_f, "%d", &SHOP_VALORE3(top_shop));
			read_line(shop_f, "%d", &SHOP_VALORE4(top_shop));
			read_line(shop_f, "%d", &SHOP_VALORE5(top_shop));
			
			SHOP_SORT(top_shop) = 0;
			SHOP_FUNC(top_shop) = 0;
			top_shop++;
		} else {
			if (*buf == '$')		/* EOF */
				done = TRUE;
			else if (strstr(buf, VERSION3_TAG))	/* New format marker */
				new_format = 1;
			free(buf);		/* Plug memory leak! */
		}
	}
}


void assign_the_shopkeepers(void)
{
	int index;
	
	cmd_say = find_command("say");
	cmd_tell = find_command("tell");
	cmd_emote = find_command("emote");
	cmd_slap = find_command("slap");
	cmd_puke = find_command("puke");
	for (index = 0; index < top_shop; index++) {
		if (SHOP_KEEPER(index) == NOBODY)
			continue;
		if (mob_index[SHOP_KEEPER(index)].func)
			SHOP_FUNC(index) = mob_index[SHOP_KEEPER(index)].func;
		mob_index[SHOP_KEEPER(index)].func = shop_keeper;
	}
}


char *customer_string(int shop_nr, int detailed)
{
	int index, cnt = 1;
	static char buf[256];
	
	*buf = 0;
	for (index = 0; *trade_letters[index] != '\n'; index++, cnt *= 2)
		if (!(SHOP_TRADE_WITH(shop_nr) & cnt))
			if (detailed) {
				if (*buf)
					strcat(buf, ", ");
				strcat(buf, trade_letters[index]);
			} else
				sprintf(END_OF(buf), "%c", *trade_letters[index]);
		else if (!detailed)
			strcat(buf, "_");
		
	return (buf);
}


void list_all_shops(struct char_data * ch)
{
	int shop_nr;
	char nomepg[8];
	char nomeclan[8];
	char megabuffer[51200];
	char space[] = "       \0";
	
	strcpy(megabuffer, "\r\n");
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		strcpy(nomepg,"");
		strcpy(nomeclan,"");
		if (!(shop_nr % 19))
		{
			strcat(megabuffer, " ##   Virtual   Where   Keeper    Buy   Sell  Propr.   Clan      Val.  Customers  \r\n");
			strcat(megabuffer, "----------------------------------------------------------------------------------\r\n");
		}

		sprintf(buf2, "%3d   %6d   %6d    ", shop_nr + 1, SHOP_NUM(shop_nr), SHOP_ROOM(shop_nr, 0));

		if (SHOP_KEEPER(shop_nr) < 0)
			strcpy(buf1, "<NONE>");
		else
			sprintf(buf1, "%6d", mob_index[SHOP_KEEPER(shop_nr)].virtual);
		
		if ((SHOP_PROPRIETARIO(shop_nr)>0) && (!get_name_by_id(SHOP_PROPRIETARIO(shop_nr))))
			strcpy(nomepg,"<UNDEF>");
		else
			strncpy(nomepg,( (SHOP_PROPRIETARIO(shop_nr)>0) ? get_name_by_id(SHOP_PROPRIETARIO(shop_nr)) : "NESSUNO") , 7);

		strncpy(nomeclan,( (SHOP_CLAN(shop_nr)>-1) ? clan[SHOP_CLAN(shop_nr)].name : "NESSUNO") ,7);

		if (7-strlen(nomepg  )>0) strncat(nomepg,space,(7 - strlen(nomepg)));
		if (7-strlen(nomeclan)>0) strncat(nomeclan,space,(7 - strlen(nomeclan)));

		sprintf(END_OF(buf2), "%s   %3.2f   %3.2f  %s  %s   %6d %d  ", buf1,
			SHOP_SELLPROFIT(shop_nr), SHOP_BUYPROFIT(shop_nr),
			nomepg, nomeclan, SHOP_VALORE(shop_nr), SHOP_CLAN(shop_nr));
		strcat(buf2, customer_string(shop_nr, FALSE));
		sprintf(END_OF(megabuffer), "%s \n\r", buf2);
	}
	
	page_string(ch->desc, megabuffer, 1);
}

void handle_detailed_list(char *buf, char *buf1, struct char_data * ch)
{
	if ((strlen(buf1) + strlen(buf) < 78) || (strlen(buf) < 20))
		strcat(buf, buf1);
	else {
		strcat(buf, "\n\r");
		send_to_char(buf, ch);
		sprintf(buf, "            %s", buf1);
	}
}


void list_detailed_shop(struct char_data * ch, int shop_nr)
{
	struct obj_data *obj;
	struct char_data *k;
	int index, temp;
	
	sprintf(buf, "Vnum:       [%5d], Rnum: [%5d]\n\r", SHOP_NUM(shop_nr),
		shop_nr + 1);
	send_to_char(buf, ch);
	
	strcpy(buf, "Rooms:      ");
	for (index = 0; SHOP_ROOM(shop_nr, index) != NOWHERE; index++) {
		if (index)
			strcat(buf, ", ");
		if ((temp = real_room(SHOP_ROOM(shop_nr, index))) != NOWHERE)
			sprintf(buf1, "%s (#%d)", world[temp].name, world[temp].number);
		else
			sprintf(buf1, "<UNKNOWN> (#%d)", SHOP_ROOM(shop_nr, index));
		handle_detailed_list(buf, buf1, ch);
	}
	if (!index)
		send_to_char("Rooms:      None!\n\r", ch);
	else {
		strcat(buf, "\n\r");
		send_to_char(buf, ch);
	}
	
	strcpy(buf, "Shopkeeper: ");
	if (SHOP_KEEPER(shop_nr) >= 0) {
		sprintf(END_OF(buf), "%s (#%d), Special Function: %s\n\r",
			GET_NAME(&mob_proto[SHOP_KEEPER(shop_nr)]),
			mob_index[SHOP_KEEPER(shop_nr)].virtual, YESNO(SHOP_FUNC(shop_nr)));
		if ((k = get_char_num(SHOP_KEEPER(shop_nr)))) {
			send_to_char(buf, ch);
			sprintf(buf, "Coins:      [%9d], Bank: [%9d] (Total: %d)\n\r",
				GET_GOLD(k), SHOP_BANK(shop_nr), GET_GOLD(k) + SHOP_BANK(shop_nr));
		}
	} else
		strcat(buf, "<NONE>\n\r");
	send_to_char(buf, ch);
	
	strcpy(buf1, customer_string(shop_nr, TRUE));
	sprintf(buf, "Customers:  %s\n\r", (*buf1) ? buf1 : "None");
	send_to_char(buf, ch);
	
	strcpy(buf, "Produces:   ");
	for (index = 0; SHOP_PRODUCT(shop_nr, index) != NOTHING; index++) {
		obj = &obj_proto[SHOP_PRODUCT(shop_nr, index)];
		if (index)
			strcat(buf, ", ");
		sprintf(buf1, "%s (#%d)", obj->short_description,
			obj_index[SHOP_PRODUCT(shop_nr, index)].virtual);
		handle_detailed_list(buf, buf1, ch);
	}
	if (!index)
		send_to_char("Produces:   Nothing!\n\r", ch);
	else {
		strcat(buf, "\n\r");
		send_to_char(buf, ch);
	}
	
	strcpy(buf, "Buys:       ");
	for (index = 0; SHOP_BUYTYPE(shop_nr, index) != NOTHING; index++) {
		if (index)
			strcat(buf, ", ");
		sprintf(buf1, "%s (#%d) ", item_types[SHOP_BUYTYPE(shop_nr, index)],
			SHOP_BUYTYPE(shop_nr, index));
		if (SHOP_BUYWORD(shop_nr, index))
			sprintf(END_OF(buf1), "[%s]", SHOP_BUYWORD(shop_nr, index));
		else
			strcat(buf1, "[all]");
		handle_detailed_list(buf, buf1, ch);
	}
	if (!index)
		send_to_char("Buys:       Nothing!\n\r", ch);
	else {
		strcat(buf, "\n\r");
		send_to_char(buf, ch);
	}
	
	sprintf(buf, "Buy at:     [%4.2f], Sell at: [%4.2f], Open: [%d-%d, %d-%d]%s",
		SHOP_SELLPROFIT(shop_nr), SHOP_BUYPROFIT(shop_nr), SHOP_OPEN1(shop_nr),
		SHOP_CLOSE1(shop_nr), SHOP_OPEN2(shop_nr), SHOP_CLOSE2(shop_nr), "\n\r");
	
	send_to_char(buf, ch);
	
	sprintbit((long) SHOP_BITVECTOR(shop_nr), shop_bits, buf1);
	sprintf(buf, "Bits:       %s\n\r", buf1);
	send_to_char(buf, ch);
}


void show_shops(struct char_data * ch, char *arg)
{
	int shop_nr;
	
	if (!*arg)
		list_all_shops(ch);
	else {
		if (!str_cmp(arg, ".")) {
			for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
				if (ok_shop_room(shop_nr, world[ch->in_room].number))
					break;
				
			if (shop_nr == top_shop) {
				send_to_char("This isn't a shop!\n\r", ch);
				return;
			}
		} else if (is_number(arg))
			shop_nr = atoi(arg) - 1;
		else
			shop_nr = -1;
		
		if ((shop_nr < 0) || (shop_nr >= top_shop)) {
			send_to_char("Illegal shop number.\n\r", ch);
			return;
		}
		list_detailed_shop(ch, shop_nr);
	}
}

// trova il numero ordinale del negozio dal numero di room
int get_shopindexfromroom(room_rnum room)
{
	int shop_nr;
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
		if (ok_shop_room(shop_nr, room))
		{
			break;
		}
	if (shop_nr == top_shop)
		return -1;
	
	return shop_nr;
}

int get_shopindexfromnum(char *neg)
{
	int shop_nr;
	int I;
	
	I = atoi(neg);
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
		if (SHOP_NUM(shop_nr) == I)
		{
			break;
		}
	if (shop_nr == top_shop)
		return -1;
	
	if (is_number(neg))
		return shop_nr;   //atoi(arg) - 1;
	
	return  -1;
}

/*Lance*/
int IsYourShop(struct char_data * ch, int shop_nmr, int msg)
{
	if ( (SHOP_PROPRIETARIO(shop_nmr) == GET_IDNUM(ch)) ||
		( (GET_CLAN(ch)!=-1) &&
			(SHOP_CLAN(shop_nmr) == GET_CLAN(ch)) &&
			(IS_SET(GET_CLAN_PRIV(ch),CP_MNGSHOP))))
		return 1;
	
	if (msg)
		send_to_char("Non puoi gestire questo negozio.\r\n", ch);
	return 0;
}

void list_shop_own (struct char_data * ch, int mode)
{
	int shop_nr;
	struct char_data *keeper;
	
	strcpy(buf,"");
	sprintf(buf2,"\r\n&6N° Negozio  Room       Bank/Coins     Keeper&0\r\n");
	strcat (buf2,"---------------------------------------------------------------------\r\n");
	
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		if ( (mode && (SHOP_PROPRIETARIO(shop_nr) == GET_IDNUM(ch))) ||
			(!mode && (GET_CLAN(ch)!=-1) && (SHOP_CLAN(shop_nr) == GET_CLAN(ch)) &&
				(IS_SET(GET_CLAN_PRIV(ch),CP_MNGSHOP) ) ))
		{
			keeper = get_char_num(SHOP_KEEPER(shop_nr));
			if (keeper) {
				sprintf(buf,"[%6d]    [%6d]   [%10d]   %s\r\n",
					SHOP_NUM(shop_nr),
					SHOP_ROOM(shop_nr,0),
					SHOP_BANK(shop_nr)+GET_GOLD(keeper),
					GET_NAME(&mob_proto[SHOP_KEEPER(shop_nr)]));
			}else{				
				sprintf(buf,"[%6d]    [%6d]   [%10d]   %s\r\n",
					SHOP_NUM(shop_nr),
					SHOP_ROOM(shop_nr,0),
					SHOP_BANK(shop_nr),
					GET_NAME(&mob_proto[SHOP_KEEPER(shop_nr)]));
			}
			strcat(buf2,buf);
		}
	} //for
	if (strcmp(buf,"") == 0)
		strcpy(buf2,"Nessun negozio in gestione!\r\n");
	page_string(ch->desc, buf2, 1);
}

ACMD(do_shoplist_pg)
{
	list_shop_own (ch, 1);
}

void shop_crash_save()
{
	int shop_nr;
	struct char_data *vict;
	
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		if ((shop_nr < top_shop) && (SHOP_KEEPER(shop_nr)!=-1))
		{
			vict = get_char_num(SHOP_KEEPER(shop_nr));
			if (vict && MOB_FLAGGED(vict,MOB_SHOPKEEPER) && MOB_FLAGGED(vict,MOB_SAVEINV))
				Crash_MobEqSave(vict, SHOP_ROOM(shop_nr,0));
		}
	}
}

void shop_crash_load()
{
	int shop_nr;
	struct char_data *vict;
	
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		if ((shop_nr < top_shop) && (SHOP_KEEPER(shop_nr)!=-1))
		{
			vict = get_char_num(SHOP_KEEPER(shop_nr));
			if (vict && MOB_FLAGGED(vict,MOB_SHOPKEEPER) && MOB_FLAGGED(vict,MOB_SAVEINV))
				Crash_MobEqLoad(vict, SHOP_ROOM(shop_nr,0));
		}
	}
}


void shopping_exchange(char *arg, struct char_data * ch,
	struct char_data * keeper, int shop_nr,int type)
{
	char buff[100];
	struct obj_data *obj, *last_obj = NULL;
	int goldamt = 0, buynum, bought = 0,buy_pr=0;
	int cl=find_clan_by_id(GET_CLAN(ch));
	bzero(buff,sizeof(buff));
	if (!(is_ok(keeper, ch, shop_nr)))
		return;
	if (SHOP_SORT(shop_nr) < IS_CARRYING_N(keeper))
		sort_keeper_objs(keeper, shop_nr);
	
	if ((buynum = transaction_amt(arg)) < 0)
	{
		sprintf(buff, "%s Un numero negativo ?", GET_NAME(ch));
		do_tell(keeper, buff, cmd_tell, 0);
		return;
	}
	if (!(*arg) || !(buynum)){
		sprintf(buff, "%s Cosa vuoi coniare??", GET_NAME(ch));
		do_tell(keeper, buff, cmd_tell, 0);
		return;
	}
	if (!(obj = get_purchase_obj(ch, arg, keeper, shop_nr, TRUE)))
		return;
	switch(type){
		case 1:/*oro e argento*/
			if((GET_OBJ_TYPE(obj)!=ITEM_LINGOTTO&&GET_OBJ_TYPE(obj)!=ITEM_MET_GREZZO)
				||(GET_OBJ_MAT_NUM(obj)!=1&&GET_OBJ_MAT_NUM(obj)!=5)){
				send_to_char("Posso coniare monete solo da lingotti o pietre d'oro o d'argento.",ch);
				return;
			}
			break;
		case 2:/*gioielli e pietre preziose*/
			if ((GET_OBJ_TYPE(obj)!=ITEM_GEMMA) && (GET_OBJ_TYPE(obj)!=ITEM_TREASURE)){
				send_to_char("Posso esportare solo Pietre preziose e gioielli",ch);
				return;
			}
			break;
	}
	buy_pr=(int)(buy_price(obj, shop_nr));
	while(bought < buynum){
		bought++;
		obj_from_char(obj);
		SHOP_SORT(shop_nr)--;
		goldamt += buy_pr;
		last_obj = obj;
		obj = get_purchase_obj(ch, arg, keeper, shop_nr, FALSE);
		if (!same_obj(obj, last_obj))
			break;
	}
	if (bought < buynum) {
		if (!obj || !same_obj(last_obj, obj))
			sprintf(buff, "%s Ne ho solo %d.\r\n ", GET_NAME(ch), bought);
		else
			sprintf(buff, "%s Something screwy only gave you %d.", GET_NAME(ch),
				bought);
		do_tell(keeper, buff, cmd_tell, 0);
	}
	if(type==1)
		sprintf(buff,"Hai coniato %d %s in %d monete d'oro.\r\n",bought,last_obj->name,goldamt);
	if(type==2)
		sprintf(buff,"Hai venduto all estero %d %s,guadagnando %d monete d'oro.\r\n",bought,last_obj->name,goldamt);
	send_to_char(buff,ch);
	clan[cl].economy.treasure+=goldamt;
	clan[cl].economy.current_entrate+=goldamt;
	clan[cl].economy.current_zecca_income+=goldamt;
	save_clans();
} 

void shop_to_store(long shop_nr, struct shop_data_save *ss) {
	ss->virtual = SHOP_NUM(shop_nr);
	ss->bank= SHOP_BANK(shop_nr);
}

void shop_from_store(long shop_nr, struct shop_data_save *ss) {
	SHOP_NUM(shop_nr) = ss->virtual;
	SHOP_BANK(shop_nr)= ss->bank;
}

void save_shop() {
	long i=0;
	int shop_nr;
	struct shop_data_save *ss;
	FILE *shops_fl = NULL;
	struct char_data * keeper;
	
	mudlog("Save shops...", CMP, LVL_GOD, FALSE);
	if (!(shops_fl = fopen(SHOPS_FILE, "wb")))
	{
		mudlog("Error to rewrite shop file", CMP, LVL_GOD, FALSE);
		return;
	}
	
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++){
		i++;
		keeper = get_char_num(SHOP_KEEPER(shop_nr));
		if (keeper && MOB_FLAGGED(keeper,MOB_SHOPKEEPER)) {
			SHOP_BANK(shop_nr) = SHOP_BANK(shop_nr) + GET_GOLD(keeper);
			GET_GOLD(keeper) =0;
		}
		CREATE(ss, struct shop_data_save,1);
		shop_to_store(shop_nr,ss);
		fseek(shops_fl, (long) (shop_nr * sizeof(struct shop_data_save)), SEEK_SET);
		fwrite(ss, sizeof(struct shop_data_save), 1, shops_fl);
		free(ss);
	}
	fclose(shops_fl);
	sprintf(buf, "%ld Shops saved." , i);
	mudlog(buf, CMP, LVL_GOD, FALSE);
}

void load_shop()
{
	long i=0;
	int shop_nr;
	struct shop_data_save *ss;
	FILE *shops_fl = NULL;
	
	if (!(shops_fl = fopen(SHOPS_FILE, "r+b")))
	{
		log("Error to load shop file");
		return;
	}
	
	for (; !feof(shops_fl);) {
		i++;
		CREATE(ss, struct shop_data_save,1);
		fread(ss, sizeof(struct shop_data_save), 1, shops_fl);
		for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
			if (SHOP_NUM(shop_nr) == ss->virtual)
				shop_from_store(shop_nr,ss);
		free(ss);
	}
	fclose(shops_fl);
	
}

void list_prod(struct char_data *ch, int shop_nr, char *argument, struct char_data *keeper)
{
	
	if (SHOP_VALORE1(shop_nr)>0) {
		if (!*argument)
		{
			send_to_char("Uso: lista <nome prodotto>\r\n",ch);
			return;
		}
		put_obj_to_mag(shop_nr, keeper,MAX_SHOP_OBJ);
		get_obj_from_mag_with_name(shop_nr,keeper, SHOP_VALORE2(shop_nr) ,argument);
		shopping_list(argument, ch, keeper, shop_nr);
	} else shopping_list("", ch, keeper, shop_nr);
	
}


void get_obj_from_mag_with_name(int shop_nr, struct char_data * keeper, int qta,char *name)
{
	long rroom;
	struct obj_data *obj,*next,*temp;
	int i=0,counter;
	
	if ((!*name) || (!keeper) || (qta<=0)) return;
	
	skip_spaces(&name);
	
	if (SHOP_VALORE1(shop_nr)>0)
		if ((rroom = real_room(SHOP_VALORE1(shop_nr)))!= NOWHERE)
			for (counter = 0; SHOP_BUYTYPE(shop_nr, counter) != NOTHING; counter++){
				i=0;
				for (obj = world[rroom].contents; obj && i<qta; obj = next) {
					next = obj->next_content;
					if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(obj))
						if (trade_with(obj, shop_nr) == OBJECT_OK)
							if (isname(name,obj->name))
								if ((!(temp = get_obj_in_list_vis(keeper, obj->name, keeper->carrying))) ||
									((temp) &&
										((GET_OBJ_MAT_TYPE(temp) != GET_OBJ_MAT_TYPE(obj)) ||
											(GET_OBJ_MAT_NUM(temp)  != GET_OBJ_MAT_NUM(obj)) ||
											(!same_obj(temp,obj))
										)
									)
									){
									i++;
									obj_from_room(obj);
									obj_to_char(obj, keeper);
								}
				}
				
				if (i<qta)
					for (obj = world[rroom].contents; obj && i<qta; obj = next) {
						next = obj->next_content;
						if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(obj))
							if (isname(name,obj->name))
								if (trade_with(obj, shop_nr)==OBJECT_OK ) {
									i++;
									obj_from_room(obj);
									obj_to_char(obj, keeper);
								}
					}
					
			}  //for type
}

void show_prodotti(int shop_nr, struct char_data *ch, struct char_data *keeper)
{
	struct obj_data *obj;
	int counter;
	char sttrtemp[5000];
	long rroom;
	char name[MAX_INPUT_LENGTH];
	
	strcpy(name,"");
	strcpy(sttrtemp,"");
	send_to_char("Lista dei prodotti in magazzino:\r\n",ch);
	send_to_char("---------------------------------\r\n",ch) ;
	
	for (counter = 0; SHOP_BUYTYPE(shop_nr, counter) != NOTHING; counter++)
		for (obj = keeper->carrying; obj; obj = obj->next_content){
			if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(obj))
				if (trade_with(obj, shop_nr) == OBJECT_OK)
					if (!strstr(sttrtemp,obj->name)) {
						strcat(sttrtemp," ");
						strcat(sttrtemp,obj->name);
						strcpy(name,obj->name);
						sprintf(buf,"%s\r\n",CAP(name));
						send_to_char(buf,ch);
					}
		}
	
	
	if (SHOP_VALORE1(shop_nr)>0)
		if ((rroom = real_room(SHOP_VALORE1(shop_nr)))!= NOWHERE)
			for (counter = 0; SHOP_BUYTYPE(shop_nr, counter) != NOTHING; counter++)
				for (obj = world[rroom].contents; obj; obj = obj->next_content){
					if (SHOP_BUYTYPE(shop_nr, counter) == GET_OBJ_TYPE(obj))
						if (trade_with(obj, shop_nr) == OBJECT_OK)
							if (!strstr(sttrtemp,obj->name)) {
								strcat(sttrtemp," ");
								strcat(sttrtemp,obj->name);
								strcpy(name,obj->name);
								sprintf(buf,"%s\r\n",CAP(name));
								send_to_char(buf,ch);
							}
				}
			
}
