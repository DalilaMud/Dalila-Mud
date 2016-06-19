/* ************************************************************************
*   File: shop.h                                        Part of CircleMUD *
*  Usage: shop file definitions, structures, constants                    *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


struct shop_buy_data {
   int type;
   char *keywords;
} ;

#define BUY_TYPE(i)		((i).type)
#define BUY_WORD(i)		((i).keywords)

struct shop_data_save {
   int virtual;
   long   bank;
   long   valore2;
   long   valore3;
   long   valore4;
   long   valore5;
   int   valore6;
   int   valore7;
   int   valore8;
   int   valore9;
   int   valore10;
   char   valore11[MAX_INPUT_LENGTH+1];
   char   valore12[MAX_INPUT_LENGTH+1];
   char   valore13[MAX_INPUT_LENGTH+1];
   char   valore14[MAX_INPUT_LENGTH+1];
   char   valore15[MAX_INPUT_LENGTH+1];   
};

struct shop_data {
   int	 virtual;		/* Virtual number of this shop		*/
   int	*producing;		/* Which item to produce (virtual)	*/
   float profit_buy;		/* Factor to multiply cost with		*/
   float profit_sell;		/* Factor to multiply cost with		*/
   struct shop_buy_data *type;	/* Which items to trade			*/
   char	*no_such_item1;		/* Message if keeper hasn't got an item	*/
   char	*no_such_item2;		/* Message if player hasn't got an item	*/
   char	*missing_cash1;		/* Message if keeper hasn't got cash	*/
   char	*missing_cash2;		/* Message if player hasn't got cash	*/
   char	*do_not_buy;		/* If keeper dosn't buy such things	*/
   char	*message_buy;		/* Message when player buys item	*/
   char	*message_sell;		/* Message when player sells item	*/
   int	 temper1;		/* How does keeper react if no money	*/
   int	 bitvector;		/* Can attack? Use bank? Cast here?	*/
   int	 keeper;		/* The mobil who owns the shop (virtual)*/
   int	 with_who;		/* Who does the shop trade with?	*/
   int	*in_room;		/* Where is the shop?			*/
   int	 open1, open2;		/* When does the shop open?		*/
   int	 close1, close2;	/* When does the shop close?		*/
   int	 bankAccount;		/* Store all gold over 15000 (disabled)	*/
   int	 lastsort;		/* How many items are sorted in inven?	*/
   int   proprietario;
   int   clan;
   int   valore;
   long   valore1;
   long   valore2;
   long   valore3;
   long   valore4;
   long   valore5;
   SPECIAL (*func);		/* Secondary spec_proc for shopkeeper	*/
};


#define MAX_TRADE	5	/* List maximums for compatibility	*/
#define MAX_PROD	5	/*	with shops before v3.0		*/
#define VERSION3_TAG	"v4.0"	/* The file has v3.0 shops in it!	*/
#define MAX_SHOP_OBJ	100	/* "Soft" maximum for list maximums	*/


/* Pretty general macros that could be used elsewhere */
#define IS_GOD(ch)		(!IS_NPC(ch) && (GET_LEVEL(ch) >= LVL_GOD))
#define GET_OBJ_NUM(obj)	(obj->item_number)
#define END_OF(buffer)		((buffer) + strlen((buffer)))


/* Possible states for objects trying to be sold */
#define OBJECT_DEAD		0
#define OBJECT_NOTOK		1
#define OBJECT_OK		2


/* Types of lists to read */
#define LIST_PRODUCE		0
#define LIST_TRADE		1
#define LIST_ROOM		2


/* Whom will we not trade with (bitvector for SHOP_TRADE_WITH()) */
#define TRADE_NOGOOD		1
#define TRADE_NOEVIL		2
#define TRADE_NONEUTRAL		4
#define TRADE_NOMAGIC_USER	8
#define TRADE_NOCLERIC		16
#define TRADE_NOTHIEF		32
#define TRADE_NOWARRIOR		64
#define TRADE_NOPELOI           128

struct stack_data {
   int data[100];
   int len;
} ;

#define S_DATA(stack, index)	((stack)->data[(index)])
#define S_LEN(stack)		((stack)->len)


/* Which expression type we are now parsing */
#define OPER_OPEN_PAREN		0
#define OPER_CLOSE_PAREN	1
#define OPER_OR			2
#define OPER_AND		3
#define OPER_NOT		4
#define MAX_OPER		4

#ifdef __SHOP_C__
const char *operator_str[] = {
	"[({",
	"])}",
	"|+",
	"&*",
	"^'"
} ;
#endif

#define SHOP_NUM(i)		(shop_index[(i)].virtual)
#define SHOP_KEEPER(i)		(shop_index[(i)].keeper)
#define SHOP_OPEN1(i)		(shop_index[(i)].open1)
#define SHOP_CLOSE1(i)		(shop_index[(i)].close1)
#define SHOP_OPEN2(i)		(shop_index[(i)].open2)
#define SHOP_CLOSE2(i)		(shop_index[(i)].close2)
#define SHOP_ROOM(i, num)	(shop_index[(i)].in_room[(num)])
#define SHOP_BUYTYPE(i, num)	(BUY_TYPE(shop_index[(i)].type[(num)]))
#define SHOP_BUYWORD(i, num)	(BUY_WORD(shop_index[(i)].type[(num)]))
#define SHOP_PRODUCT(i, num)	(shop_index[(i)].producing[(num)])
#define SHOP_BANK(i)		(shop_index[(i)].bankAccount)
#define SHOP_BROKE_TEMPER(i)	(shop_index[(i)].temper1)
#define SHOP_BITVECTOR(i)	(shop_index[(i)].bitvector)
#define SHOP_TRADE_WITH(i)	(shop_index[(i)].with_who)
#define SHOP_SORT(i)		(shop_index[(i)].lastsort)
#define SHOP_BUYPROFIT(i)	(shop_index[(i)].profit_buy)
#define SHOP_SELLPROFIT(i)	(shop_index[(i)].profit_sell)
#define SHOP_PROPRIETARIO(i)    (shop_index[(i)].proprietario)
#define SHOP_CLAN(i)            (shop_index[(i)].clan)
#define SHOP_VALORE(i)          (shop_index[(i)].valore)
#define SHOP_VALORE1(i)          (shop_index[(i)].valore1)
#define SHOP_VALORE2(i)          (shop_index[(i)].valore2)
#define SHOP_VALORE3(i)          (shop_index[(i)].valore3)
#define SHOP_VALORE4(i)          (shop_index[(i)].valore4)
#define SHOP_VALORE5(i)          (shop_index[(i)].valore5)
#define SHOP_FUNC(i)		(shop_index[(i)].func)

#define NOTRADE_GOOD(i)		(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOGOOD))
#define NOTRADE_EVIL(i)		(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOEVIL))
#define NOTRADE_NEUTRAL(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NONEUTRAL))
#define NOTRADE_MAGIC_USER(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOMAGIC_USER))
#define NOTRADE_CLERIC(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOCLERIC))
#define NOTRADE_THIEF(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOTHIEF))
#define NOTRADE_WARRIOR(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_NOWARRIOR))
#define NOTRADE_PELOI(i)	(IS_SET(SHOP_TRADE_WITH((i)), TRADE_PELOI))

/* Constant list for printing out who we sell to */
#ifdef __SHOP_C__
const char *trade_letters[] = {
	"Good",			/* First, the alignment based ones */
	"Evil",
	"Neutral",
	"Pandion",		/* Then the class based ones */
	"Cyrinic",
	"Alcioni",
	"Genidian",
	"Peloi",
	"\n"
} ;
#endif


#define WILL_START_FIGHT	1
#define WILL_BANK_MONEY		2
#define NO_SELL      		3
#define NO_BUY		         4
#define PG_PROD            5

#define SHOP_KILL_CHARS(i)	(IS_SET(SHOP_BITVECTOR((i)), (1 << (WILL_START_FIGHT - 1))))
#define SHOP_USES_BANK(i)	(IS_SET(SHOP_BITVECTOR((i)), (1 << (WILL_BANK_MONEY - 1))))
#define SHOP_NO_SELL(i)	        (IS_SET(SHOP_BITVECTOR((i)), (1 << (NO_SELL - 1))))
#define SHOP_NO_BUY(i)	        (IS_SET(SHOP_BITVECTOR((i)), (1 << (NO_BUY - 1))))
#define SHOP_PG_PROD(i)         (IS_SET(SHOP_BITVECTOR((i)), (1 << (PG_PROD - 1))))

#ifdef __SHOP_C__
char *shop_bits[] = {
	"WILL_FIGHT",
	"USES_BANK",
	"NO_SELL",
 	"NO_BUY",
	"PG_PROD",
	"\n"
} ;
#endif

#define MIN_OUTSIDE_BANK	0 // Era 5000
#define MAX_OUTSIDE_BANK	0 // Era 15000

#define MSG_NOT_OPEN_YET	"Torna piu' tardi."
#define MSG_NOT_REOPEN_YET	"Mi spiace, siamo chiusi, ma puoi tornare domani.."
#define MSG_CLOSED_FOR_DAY	"Mi spiace, torna domani."
#define MSG_NO_STEAL_HERE	"$n e' un ladro bastardo!!!"
#define MSG_NO_SEE_CHAR		"Non commercio con chi non posso vedere!"
#define MSG_NO_SELL_ALIGN	"Sparisci da qui prima che io chiami le guardie!"
#define MSG_NO_SELL_CLASS	"Non trattiamo con quelli come te qui!"
#define MSG_NO_SELL_CLAN        "Non tratto con quelli della tua cricca!"
#define MSG_NO_USED_WANDSTAFF	"Non compro quel tipo di oggetti usati!"
#define MSG_CANT_KILL_KEEPER	"Sparisci da qui prima che io chiami le guardie!"

int get_shopindexfromroom(room_rnum room);
int get_shopindexfromnum(char *neg);
int IsYourShop(struct char_data * ch, int shop_nmr, int msg);
void list_shop_own (struct char_data * ch, int mode);
void shop_crash_save();
void shop_crash_load();
void save_shop();
extern struct shop_data *shop_index;





