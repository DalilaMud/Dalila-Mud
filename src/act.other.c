/* ************************************************************************
*   File: act.other.c      35                           Part of CircleMUD *
*  Usage: Miscellaneous player-level commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
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
#include "screen.h"
#include "house.h"
#include "dg_scripts.h"
#include "clan.h"

/* extern variables */
extern struct str_app_type str_app[];
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct dex_skill_type dex_app_skill[];
extern struct dex_app_type dex_app[];
extern struct spell_info_type spell_info[];
extern struct index_data *mob_index;
extern struct index_data *obj_index; //Orione
extern char *class_abbrevs[];
extern char *AFK_MESSG;
extern int top_of_world;

/* extern procedures */
extern void safe_save_pg(struct char_data *ch);
extern void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia);
extern void perform_wear(struct char_data * ch, struct obj_data * obj, int where);
SPECIAL(shop_keeper);
void write_aliases(struct char_data *ch);
int objlevel (struct obj_data * obj);
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);
void change_notorieta(struct char_data *ch,struct char_data *victim, int event, int amount);
struct obj_data *get_obj_in_list_particular(struct char_data * ch, char *name,struct obj_data * list,int obj_type);

ACMD(do_religione)
{
	extern char *nomi_religioni[];
	int membri_religione(int num_religione);

	struct char_data *victim;
	int rnumero;
	int numero_religiosi;
	char *tmp1, *tmp2;
	
	if (GET_LEVEL(ch) < LVL_QUEST_MASTER && !PLR_FLAGGED(ch, PLR_PAPA)){
		if (*buf)
			sprintf (buf, "Accogli %s nella fantastica religione del %sesimo!\r\n", arg, GET_NAME(ch));
		else
			sprintf (buf, "Non essendo a capo di alcuna religione decidi di arruolare il tuo amico immaginario e senza nome.\r\n");
		send_to_char (buf, ch);
		return;
	}

	half_chop(argument, buf, buf2);

	if (! (*buf && *buf2) && GET_LEVEL(ch) >= LVL_QUEST_MASTER){
		sprintf (buf,	"&6&bUtilizzo di religione:&0\r\n"
				"religione nomepg <arg2>\r\n"
				"Dove arg2 puo' assumere i seguenti valori:\r\n"
				"Nessuna\r\n"
				"Shaarr\r\n"
				"Xhyphys\r\n"
				"Silue\r\n"
				"Therion\r\n"
				"Papa\r\n"
				"Se arg2 e' il nome di una religione il pg diventera' membro della religione,\r\n"
				"se e' papa diventera' papa della sua religione.\r\n"
				"religione show all, invece, mostra tutti i pg di tutte le religioni.\r\n");
		send_to_char (buf, ch);
		return;
	}
	else if (!*buf){
		send_to_char ("Dovresti anche specificare il nome di un pg...\r\n", ch);
		return;
	}

	if (!(victim = get_char_vis(ch, buf))){
		send_to_char(NOPERSON, ch);
		return;
	}

	if (IN_ROOM(ch) != IN_ROOM(victim) && GET_LEVEL(ch) < LVL_QUEST_MASTER){
		send_to_char ("Si deve trovare nella tua stessa stanza per poterlo fare!\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) < LVL_QUEST_MASTER && (!GET_CULTO(ch) || GET_CULTO(ch) >= NUM_RELIGIONI)){
		send_to_char ("Qualcosa non va... sei papa e non hai una religione, contatta i creatori.\r\n", ch);
		return;
	}

	if (GET_LEVEL(ch) < LVL_QUEST_MASTER)
		rnumero = GET_CULTO(ch);
	else
		for (rnumero = 0, tmp1 = nomi_religioni[0], tmp2 = buf2; strcmp(tmp1, "\n") && !is_abbrev(tmp1, tmp2); tmp1 = nomi_religioni[++rnumero]);

	if (rnumero >= NUM_RELIGIONI){
		tmp1 = strdup("Papa");
		tmp2 = buf2;

		if (!is_abbrev (tmp1, tmp2)){
			send_to_char ("Dovresti specificare un nome di una religione o indicare \"papa\"...\r\n", ch);
			return;
		}

		if (PLR_FLAGGED(victim, PLR_PAPA)){
			act ("D'ora in poi $N non sara' piu' Papa.", FALSE, ch, 0, victim, TO_CHAR);
			act ("Hai perso la fiducia degli dei, e con essa il tuo ruolo di Capo religioso!", FALSE, ch, 0, victim, TO_VICT);
		}
		else{
			act ("Innalzi $N a Papa della sua religione.", FALSE, ch, 0, victim, TO_CHAR);
			act ("Complimenti! Sei diventato Capo della tua religione!", FALSE, ch, 0, victim, TO_VICT);
		}
		TOGGLE_BIT(PLR_FLAGS(victim), PLR_PAPA);
		return;
	}

	numero_religiosi = membri_religione(rnumero);
	sprintf (buf, "%d\r\n", numero_religiosi);
	send_to_char (buf, ch);

	if (numero_religiosi >= MAX_MEMBRI_RELIGIONE){
		send_to_char ("Gli adepti di questa religione sono gia' troppi!\r\n", ch);
		return;
	}

	GET_CULTO(victim) = rnumero;
	sprintf (buf, "Inizi $N al culto di %s.", nomi_religioni[rnumero]);
	act (buf, FALSE, ch, 0, victim, TO_CHAR);
}

ACMD(do_quit)
{
	void die(struct char_data * ch, struct char_data * killer);
	void Crash_rentsave(struct char_data * ch, int cost);
	extern int free_rent;
	extern long num_obj_in_obj_mag (struct obj_data * object);
	extern long num_obj_in_mag (room_rnum room);
	room_rnum save_room;
	struct descriptor_data *d, *next_d;
	struct follow_type * f;
	
	if (IS_NPC(ch) || !ch->desc)
		return;
	
	if (subcmd != SCMD_QUIT) {
		send_to_char("Digita QUIT per esteso se vuoi abbandonare il gioco!\r\n",ch);
		return;
	}
	
	
	for (f = ch->followers; f; f = f->next)
	if (   (f->follower) 
	    && IS_NPC(f->follower) 
	    && ((   MOB_FLAGGED(f->follower, MOB_MOUNTABLE) 
				    || (MOB_FLAGGED(f->follower,MOB_HARD_TAME) && AFF_FLAGGED(f->follower,AFF_CHARM))
				 ))
		 ) {
			sprintf(buf,"Devi prima rentare %s", GET_NAME(f->follower));
			send_to_char(buf,ch);
			return;
		}
		
	if((ch->in_room!=NOWHERE) && ROOM_FLAGGED(ch->in_room, ROOM_HOUSE)) {
		if( ((ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE)) &&
		    (num_obj_in_mag(ch->in_room) + IS_CARRYING_N(ch) > MAX_OBJ_BIGHOUSE)) ||
		    ((num_obj_in_mag(ch->in_room) + IS_CARRYING_N(ch) > MAX_OBJ_INROOM) &&
		    !ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE))) {
			act("La stanza non puo' contenere altro.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		}
	}
	/*	
	if (   (ch->in_room!=NOWHERE) 
	    && (ROOM_FLAGGED(ch->in_room,ROOM_HOUSE)) 
	    && (num_obj_in_mag(ch->in_room) + IS_CARRYING_N(ch) > MAX_OBJ_INROOM)
	   ) {
		act("Non puoi quittare, la stanza non puo' contenere altri oggetti.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	
	
	if ((ch->in_room!=NOWHERE)
	&& (ROOM_FLAGGED(ch->in_room,ROOM_HOUSE)) && (ROOM_FLAGGED(ch->in_room,ROOM_BIG_HOUSE))
	&& (num_obj_in_mag(ch->in_room) + IS_CARRYING_N(ch) > MAX_OBJ_BIGHOUSE)
	) {
		act("Non puoi quittare, la stanza non puo' contenere altri oggetti.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}*/
		
	if (subcmd != SCMD_QUIT && GET_LEVEL(ch) < LVL_IMMORT)
		send_to_char("Usa quit--niente meno che quit!\r\n", ch);
	else if (GET_POS(ch) == POS_FIGHTING)
		send_to_char("Niente. Stai combattendo per la tua vita!\r\n", ch);
	else if (GET_POS(ch) < POS_STUNNED) {
		send_to_char("Sei morto prima del tuo tempo...\r\n", ch);
		die(ch, NULL);
	}
	else {
		if (!GET_INVIS_LEV(ch))
			act("$n ha lasciato il gioco.", TRUE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "%s ha quittato.", GET_NAME(ch));
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
		send_to_char("Arrivederci amico.. Torna presto!\r\n", ch);
		if (IS_AFFECTED(ch, AFF_MARK) && (ASSASSIN(ch) != NULL)) {
			AS_ROOM(ch) = AS_MOB_ROOM(ch);
			AS_RNUM(ch) = AS_MOB_RNUM(ch);
			extract_char(ASSASSIN(ch));
		}
		if (IS_AFFECTED(ch, AFF_GROUP)) REMOVE_BIT(AFF_FLAGS(ch), AFF_GROUP);
		
    /*
     * kill off all sockets connected to the same player as the one who is
     * trying to quit.  Helps to maintain sanity as well as prevent duping.
     */
		for (d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;
			if (d == ch->desc) 
			  continue;
			if (d->character && (GET_IDNUM(d->character) == GET_IDNUM(ch)))
				close_socket(d);
		}
		
		save_room = ch->in_room;
		if (free_rent)
			Crash_rentsave(ch, 0);
		extract_char(ch);		/* Char is saved in extract char */
		
      // If someone is quitting in their house, let them load back here
    /*if (ROOM_FLAGGED(room, ROOM_HOUSE))
      {
      save_char(ch, save_room);
      // House_crashsave(save_room);
      }*/
	}
}


ACMD(do_save)
{  
	if (IS_NPC(ch) || !ch->desc)
		return;
	
	if (cmd)
	{
		sprintf(buf, "Saving %s.\r\n", GET_NAME(ch));
		send_to_char(buf, ch);
	}
	write_aliases(ch);
	save_char(ch, ch->in_room);
	Crash_crashsave(ch);
	if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
		House_crashsave(world[ch->in_room].number);
	safe_save_pg(ch);	
}


/* generic function for commands which are normally overridden by
   special procedures - i.e., shop commands, mail commands, etc. */
ACMD(do_not_here)
{
	send_to_char("Mi spiace, non lo puoi fare qui!\r\n", ch);
}


ACMD(do_sneak)
{
	struct affected_type af;
	
	if (!CAN_LEARN_SKILL(ch, SKILL_SNEAK) || GET_SKILL(ch, SKILL_SNEAK)<=0)  
	{
		send_to_char("Non conosci niente della skill sneak.\r\n", ch);
		return;
	}
	
	if (IS_AFFECTED(ch, AFF_SNEAK))
		affect_from_char(ch, SPELLSKILL, SKILL_SNEAK);
	
	if (!test_and_improve(ch, SKILL_SNEAK, 100, 15, dex_app_skill[GET_DEX(ch)].sneak))
	{
		send_to_char("Non hai successo.\r\n", ch);
		return;
	}
	
	send_to_char("Okay, ti muoverai silenziosamente per un po'.\r\n", ch);
	af.natura = SPELLSKILL;
	af.type = SKILL_SNEAK;
	af.duration = GET_LEVEL(ch);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector[0] = AFF_SNEAK;
	af.bitvector[1] = 0;
	af.bitvector[2] = 0;
	af.bitvector[3] = 0;
	affect_to_char(ch, &af);
}


ACMD(do_hide)
{
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere!\r\n",ch);
		return;
	}
	if (!CAN_LEARN_SKILL(ch, SKILL_HIDE) || GET_SKILL(ch, SKILL_HIDE)<=0)
	{
		send_to_char("Non conosci niente della skill hide.\r\n", ch);
		return;
	}
	
	send_to_char("Provi a nasconderti.\r\n", ch);
	
	if (IS_AFFECTED(ch, AFF_HIDE))
		REMOVE_BIT(AFF_FLAGS(ch), AFF_HIDE);
	
	if (!test_and_improve(ch, SKILL_HIDE, 100, 15, dex_app_skill[GET_DEX(ch)].hide))
		return;
	
	SET_BIT(AFF_FLAGS(ch), AFF_HIDE);
}


ACMD(do_steal)
{
	struct char_data *vict;
	struct obj_data *obj;
	char vict_name[MAX_INPUT_LENGTH], obj_name[MAX_INPUT_LENGTH];
	int gold, eq_pos, pcsteal = 0, ohoh = 0;
	int temporaneo=0;
	extern int pt_allowed;
	ACMD(do_gen_comm);
	int modificatore=0;
	int val;
	int coins=0;
	
	argument = one_argument(argument, obj_name);
	one_argument(argument, vict_name);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere!\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, vict_name)))
	{
		send_to_char("Rubare cosa da chi?\r\n", ch);
		return;
	}
	if (vict == ch){
		send_to_char("Dai su, mi sembra stupido!\r\n", ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Cosa pensi di poter rubare ad un fantasma?!?\r\n",ch);
		return;
	}
	
	/* Ardanos: niente steal in prigione! */
	
	if(ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON)) {
		send_to_char("Mmhh..se cercheresti di rubare qualcosa qui, ti beccherebbero subito!\r\n", ch);
		return;
	}
	
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)< LVL_IMMORT){
		send_to_char("Non puoi rubare qualcosa a qualcuno che non sta giocando come te.\r\n",ch);
		return;
	}
	if (!(obj = get_obj_in_list_vis(vict, obj_name, vict->carrying))){
		for (eq_pos = 0; eq_pos < NUM_WEARS; eq_pos++){
			if (GET_EQ(vict, eq_pos) &&
				(isname(obj_name, GET_EQ(vict, eq_pos)->name)) &&
				CAN_SEE_OBJ(ch, GET_EQ(vict, eq_pos)))
			{
				obj = GET_EQ(vict, eq_pos);
				break;
			}
		}
		if (obj)
			if(GET_OBJ_TYPE(obj)==ITEM_RELIQUIA){ //Questo si puo fare meglio poi vedo
				
				
				if(GET_LEVEL(vict)>=LVL_QUEST_MASTER){
					send_to_char("Non penso sarebbe d'accordo con il tuo gesto.\r\n",ch);
					return;
				}
				if (!obj){
					act("$N non ha quella cosa.", FALSE, ch, 0, vict, TO_CHAR);
					return;
				}
				if(GET_EQ(ch,eq_pos)){
					send_to_char("La potenza di piu reliquie potrebbe danneggiarti permanentemente.\r\n",ch);
					return ;
				}
				else{	
					if((number(1,100)) < 71){
						act("Ti sei impadronito della $p.", FALSE, ch, obj, 0, TO_CHAR);
						obj_to_char(unequip_char(vict, eq_pos), ch);
						perform_wear(ch,obj,eq_pos);
					}
					else {
						send_to_char("&8&1Non riesci a sfilargli la reliquia,ritenta sarai piu' fortunato.&0\r\n",ch);
						WAIT_STATE(ch,PULSE_VIOLENCE * 3);
					}
				}
				return ;
			}
	}
	//FINE MODIFICA*/ 
	
	if (!CAN_LEARN_SKILL(ch, SKILL_STEAL) || GET_SKILL(ch, SKILL_STEAL)<=0)
	{
		send_to_char("Non conosci niente della skill steal.\r\n", ch);
		return;
	}
	
	/*if (!(vict = get_char_room_vis(ch, vict_name)))
	{
		send_to_char("Rubare cosa da chi?\r\n", ch);
		return;
	}
	else if (vict == ch)
	{
		send_to_char("Dai su, mi sembra stupido!\r\n", ch);
		return;
	}*/
	
	if (!can_do_event(ch,LAST_STEAL, 3))
	{
		send_to_char("Il furto e' un arte, richiede tempo...!\r\n", ch);
		return;
	}
/*	
	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)) {
		send_to_char("&5Questa stanza ti da' una sensazione di pace... ti passa la voglia.&0\r\n", ch);
		return;
	}
*/	
	/* Orione Inizio */
	if (GET_LEVEL(ch)<LVL_IMMORT) {
	    if (IS_NPC(vict) && MOB_FLAGGED(vict, MOB_QUEST)) {
		if (!(IS_QUESTOR(ch) && (GET_TIPOQUEST(ch)==QUEST_GETITEM) && \
		    (GET_QUESTMRR(ch)==GET_MOB_VNUM(vict)) && (GET_QUESTOBJ(ch)==GET_OBJ_VNUM(obj)))) {
		        send_to_char("Gli astri, che indicano il destino di ogni persona, non brillano verso di te.\r\n", ch);
			sprintf(buf, "%s non puo' rubare l'obj %d al mob di quest %d", GET_NAME(ch), GET_OBJ_VNUM(obj), GET_MOB_VNUM(vict));
			mudlog(buf, NRM, LVL_IMMORT, FALSE);
			return;
	        }
	    }
	}
	/*Orione Fine */
													    	
	if (!pt_allowed && !IS_NPC(vict))
		pcsteal = 1;
	
	if (GET_POS(vict) < POS_SLEEPING||AFF_FLAGGED(vict,AFF_PARALIZE)
		||AFF_FLAGGED(vict,AFF_TRAMORTITO)||AFF_FLAGGED(vict,AFF_SLEEP)||PLR_FLAGGED(vict,PLR_CATARSI))
		temporaneo = -1;		/* ALWAYS SUCCESS */
	
  /* NO NO With Imp's and Shopkeepers, and if player thieving is not allowed */
	if (GET_LEVEL(vict) >= LVL_QUEST_MASTER || pcsteal || GET_MOB_SPEC(vict) == shop_keeper)
		temporaneo = 1;		/* Failure */
	
	if (str_cmp(obj_name, "coins") && str_cmp(obj_name, "gold")){
		if (!(obj = get_obj_in_list_vis(vict, obj_name, vict->carrying))){
			for (eq_pos = 0; eq_pos < NUM_WEARS; eq_pos++)
				if (GET_EQ(vict, eq_pos) &&
					(isname(obj_name, GET_EQ(vict, eq_pos)->name)) &&
					CAN_SEE_OBJ(ch, GET_EQ(vict, eq_pos)))
				{
					obj = GET_EQ(vict, eq_pos);
					break;
				}
			if (!obj){
				act("$N non ha quella cosa.", FALSE, ch, 0, vict, TO_CHAR);
				return;
			}
			else{			/* It is equipment */
				if ((GET_POS(vict) > POS_STUNNED)&&!AFF_FLAGGED(vict,AFF_PARALIZE)&&!AFF_FLAGGED(vict,AFF_SLEEP)&&!AFF_FLAGGED(vict,AFF_TRAMORTITO)){
					send_to_char("Rubare adesso? Impossibile!\r\n", ch);
					return;
				}
				else{
					act("Riesci a sfilare $p.", FALSE, ch, obj, 0, TO_CHAR);
					act("$n ruba $p da $N.", FALSE, ch, obj, vict, TO_NOTVICT);
					obj_to_char(unequip_char(vict, eq_pos), ch);
					/* Orione Inizio */
					if (IS_NPC(vict) && MOB_FLAGGED(vict, MOB_QUEST)) {
					    if (IS_QUESTOR(ch) && (GET_TIPOQUEST(ch)==QUEST_GETITEM) && \
						(GET_QUESTMRR(ch)==GET_MOB_VNUM(vict)) && (GET_QUESTOBJ(ch)==GET_OBJ_VNUM(obj)))
						    GET_QUESTMRR(ch) = -3;
					    else {
						sprintf(buf, "%s ha rubato l'obj %d dal mob %d, obiettivo di una quest", GET_NAME(ch), GET_OBJ_VNUM(obj), GET_MOB_VNUM(vict));
						mudlog(buf, NRM, LVL_IMMORT, TRUE);
		    			    }
					}
					/*Orione Fine */									
				}
			}
		}
		else{			/* obj found in inventory */
			modificatore = dex_app_skill[GET_DEX(ch)].p_pocket-GET_OBJ_WEIGHT(obj);
      /*Adriano by Fizban*/
			if( IS_THIEF(ch))
				modificatore += 15;
			if(GET_POS(vict)==POS_SLEEPING)
				modificatore+=25;
			if ((temporaneo==1) || (AWAKE(vict) && temporaneo==0
					&&!test_and_improve(ch, SKILL_STEAL, 125, 15, modificatore)))
			{
				send_to_char("Non riesci a prendere nulla...!\r\n", ch);
			}
			else{			/* Steal the item */
				if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))){
					if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) < CAN_CARRY_W(ch)){
						obj_from_char(obj);
						obj_to_char(obj, ch);
						send_to_char("Preso!...\r\n", ch);
						/* Orione Inizio */
						if (IS_NPC(vict) && MOB_FLAGGED(vict, MOB_QUEST)) {
						    if (IS_QUESTOR(ch) && (GET_TIPOQUEST(ch)==QUEST_GETITEM) && \
							(GET_QUESTMRR(ch)==GET_MOB_VNUM(vict)) && (GET_QUESTOBJ(ch)==GET_OBJ_VNUM(obj)))
							    GET_QUESTMRR(ch) = -3;
						    else {
							sprintf(buf, "%s ha rubato l'obj %d dal mob %d, obiettivo di una quest", GET_NAME(ch), GET_OBJ_VNUM(obj), GET_MOB_VNUM(vict));
							mudlog(buf, NRM, LVL_IMMORT, TRUE);
						    }
						}
						/*Orione Fine */
					}
				}
				else send_to_char("Non puoi portare quel peso.\r\n", ch);
			}
			if ((GET_LEVEL(ch)>=LVL_AVATAR) || (number(1,100) <= (GET_SKILL(ch, SKILL_STEAL)/2 + modificatore + GET_LEVEL(ch) - GET_LEVEL(vict)))||temporaneo==-1){
				act("Sembra non essersi accorto del tuo gesto...", FALSE, ch, 0, 0, TO_CHAR);
			}
			else{
				ohoh = TRUE;
				val=MIN(100,GET_OBJ_COST(obj)/200);
				act("Oops..ti ha beccato", FALSE, ch, 0, 0, TO_CHAR);
				act("$n tenta di rubarti qualche cosa!", FALSE, ch, 0, vict, TO_VICT);
				act("$n tenta di rubare qualcosa a $N.", TRUE, ch, 0, vict, TO_NOTVICT);
        /*Enrico :come dare il flag thief e il decremento di notorieta*/
				if(!IS_NPC(vict)){
					//SET_BIT(PLR_FLAGS(ch),PLR_THIEF);
					sprintf(buf,"Furto di valori per %d coins ai danni di %s.",GET_OBJ_COST(obj),GET_NAME(vict));
					set_crime(ch,vict, "vivo",buf,find_clan_by_id(GET_CLAN(vict)),CLAN_THIEF, 500);
					if(number(1,9)>(GET_NOTORIETA(ch)/100))
						change_notorieta(ch,NULL,THIEF,val);
				} else {
					if (CLAN_ID(vict)>=0) {
						set_crime(ch,vict, "vivo",buf,find_clan_by_id(CLAN_ID(vict)),CLAN_THIEF, 500);	
						sprintf(buf,"Furto di valori per %d coins ai danni di %s.",GET_OBJ_COST(obj),GET_NAME(vict));
					}
				}
			}
		}
	}
	else{			/* Steal some coins */
		modificatore = dex_app_skill[GET_DEX(ch)].p_pocket;
    /*Adriano by Fizban*/
		if (IS_THIEF(ch))
			modificatore += 15;
		if(GET_POS(vict)==POS_SLEEPING)
			modificatore += 30;
		if ((temporaneo==1) || (AWAKE(vict) && temporaneo==0 && !test_and_improve(ch, SKILL_STEAL, 125, 15, modificatore)&&temporaneo!=-1))
		{
			send_to_char("Non riesci a prendere nulla...!\r\n", ch);
		}
		else
		{
      /* Steal some gold coins */
			gold = (int) (  GET_GOLD(vict) * number(1, GET_LEVEL(ch) ) / 100);
			gold = MIN(10000, gold);
			if(temporaneo==-1)
				gold=GET_GOLD(vict);//se e' paralizzato o incapacitato,gli fotte tutto
			if (gold > 0){
				GET_GOLD(ch) += gold;
				GET_GOLD(vict) -= gold;
				if (gold > 1){
					sprintf(buf, "Bingo!  Hai %d gold coins in piu' ora.\r\n", gold);
					send_to_char(buf, ch);
				}
				else send_to_char("Hmm, dovrai accontentarti di una sola moneta...\r\n", ch);
			}
			else send_to_char("Non puoi prendere nessun coin...\r\n", ch);
		}
		if ((GET_LEVEL(ch)>=LVL_AVATAR) || (number(1,100) <= (GET_SKILL(ch, SKILL_STEAL)/2 + modificatore + GET_LEVEL(ch) - GET_LEVEL(vict)))||temporaneo==-1)
		{
			act("Sembra non essersi accorto del tuo gesto...", FALSE, ch, 0, 0, TO_CHAR);
		}
		else{
			ohoh = TRUE;
			act("Oops..ti ha beccato.", FALSE, ch, 0, 0, TO_CHAR);
			act("Scopri che $n ha le mani nelle tue tasche.", FALSE, ch, 0, vict, TO_VICT);
			act("$n tenta di rubare dei coins da $N.", TRUE, ch, 0, vict, TO_NOTVICT);
      /*Enrico:Flag thief e decremento di notorieta*/
			if(!IS_NPC(vict)){
				val=(GET_NOTORIETA(vict)*0.02);
				//SET_BIT(PLR_FLAGS(ch),PLR_THIEF);	
				coins = (int) (  GET_GOLD(vict) * number(1, GET_LEVEL(ch) ) / 100);
				sprintf(buf,"Furto di %d coins ai danni di %s.",coins ,GET_NAME(vict));
				set_crime(ch,vict, "vivo",buf,find_clan_by_id(GET_CLAN(vict)),CLAN_THIEF, coins);
				change_notorieta(ch,NULL,THIEF,val);
			} else {
				if (CLAN_ID(vict)>=0) {
					sprintf(buf,"Furto di %d coins ai danni di %s.",coins,GET_NAME(vict));
					set_crime(ch,vict, "vivo",buf,find_clan_by_id(CLAN_ID(vict)),CLAN_THIEF,coins);	
				}
			}
		}
	}
	if (ohoh && IS_NPC(vict) && AWAKE(vict))
		hit(vict, ch, TYPE_UNDEFINED);
}


ACMD(do_land)
{
	if (char_affected_by_obj(ch, AFF_FLYING, BITV_FLYING) || char_affected_by_obj(ch, AFF_LEVITATE, BITV_LEVITATE)){
		send_to_char ("Degli oggetti che indossi ti obbligano a volare.\r\n", ch);
		return;
	}
	if (!(AFFECTED(ch, AFF_FLYING, BITV_FLYING) || AFFECTED(ch, AFF_LEVITATE, BITV_LEVITATE))){
		send_to_char("Non sei sottoposto ad alcun incantesimo di volo.\r\n", ch);
		return;
	}

	if (affected_by_spell(ch, SPELLSKILL, SPELL_FLY))
	{
		affect_from_char(ch, SPELLSKILL, SPELL_FLY);
		send_to_char("Spezzi l'incantesimo del volo.\r\n", ch);
	}
	if (affected_by_spell(ch, SPELLSKILL, SPELL_LEVITATE))
	{
		affect_from_char(ch, SPELLSKILL, SPELL_LEVITATE);
		send_to_char("Spezzi l'incantesimo del volo.\r\n", ch);
	}
	
	if (AFFECTED(ch, AFF_FLYING, BITV_FLYING)){
		affect_remove_all(ch, AFF_FLYING, BITV_FLYING);
		send_to_char("Spezzi l'incantesimo del volo.\r\n", ch);
	}
	else {
		affect_remove_all(ch, AFF_LEVITATE, BITV_LEVITATE);
		send_to_char("Spezzi l'incantesimo del volo.\r\n", ch);
	}
}


ACMD(do_practice)
{
	void list_skills(struct char_data * ch);
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	if (*arg) send_to_char("La pratica la puoi fare sola nella tua scuola.\r\n", ch);
	else list_skills(ch);
}


ACMD(do_visible)
{
	void appear(struct char_data * ch);
	void perform_immort_vis(struct char_data *ch);
	
	if (GET_LEVEL(ch) >= LVL_IMMORT)
	{
		if (IS_TRUST(ch))
			send_to_char("Non puoi renderti visibile!\r\n",ch);
		else
			perform_immort_vis(ch);
		return;
	}
	
	if IS_AFFECTED(ch, AFF_INVISIBLE)
	{
		appear(ch);
		send_to_char("Rompi l'incantesimo di invisibilita'.\r\n", ch);
	}
	else send_to_char("Sei gia' visibile.\r\n", ch);
}


ACMD(do_title)
{
	skip_spaces(&argument);
	delete_doubledollar(argument);
	
	if (IS_NPC(ch))
		send_to_char("Il tuo titolo va gia' bene cosi'.\r\n", ch);
	else if (PLR_FLAGGED(ch, PLR_NOTITLE))
		send_to_char("Non puoi darti un titolo, forse non avresti dovuto abusarne !\r\n", ch);
	else if (strstr(argument, "(") || strstr(argument, ")"))
		send_to_char("I titoli non possono contenere i caratteri ( o ).\r\n", ch);
	else if (strlen(argument) > MAX_TITLE_LENGTH - 50)
	{
		sprintf(buf, "Mi spiace, i titoli non possono essere piu' lunghi di %d caratteri.\r\n", MAX_TITLE_LENGTH - 50);
		send_to_char(buf, ch);
	}
	else
	{
		set_title(ch, argument);
		sprintf(buf, "Okay, ora sei %s %s.\r\n", GET_NAME(ch), GET_TITLE(ch));
		send_to_char(buf, ch);
	}
}


int perform_group(struct char_data *ch, struct char_data *vict)
{
	if (IS_AFFECTED(vict, AFF_GROUP) || !CAN_SEE(ch, vict)) return 0;
	
	SET_BIT(AFF_FLAGS(vict), AFF_GROUP);
	
	if ((ch != vict) && (GROUPNAME(vict))){
		free(GROUPNAME(vict));
		GROUPNAME(vict) = NULL;
	} else {
		if (GROUPNAME(ch))
			GROUPNAME(vict) =  str_dup(GROUPNAME(ch));
		else GROUPNAME(vict) = NULL;
	}
	if (ch != vict){
		if (GROUPNAME(vict))
			sprintf(buf,"$N e' ora membro del gruppo &5%s&0 comandato da te.",GROUPNAME(vict));
		else
			sprintf(buf,"$N e' ora membro del tuo gruppo.");
		act(buf, FALSE, ch, 0, vict, TO_CHAR);
	}
	if (ch != vict){
		if (GROUPNAME(vict))
			sprintf(buf,"Sei ora membro del gruppo &5%s&0 di $n.",GROUPNAME(vict));
		else
			sprintf(buf,"Sei ora membro del gruppo di $n.");
		act(buf, FALSE, ch, 0, vict, TO_VICT);
	} else {
		if (GROUPNAME(vict))
			sprintf(buf,"Hai formato il gruppo &5%s&0.",GROUPNAME(vict));
		else
			sprintf(buf,"Hai formato un gruppo.");
		act(buf, FALSE, ch, 0, vict, TO_VICT);
	}
	
	if (GROUPNAME(vict))
		sprintf(buf,"$N e' ora membro del gruppo &5%s&0 di $n.",GROUPNAME(vict));
	else
		sprintf(buf,"$N e' ora membro del gruppo di $n.");
	act(buf, FALSE, ch, 0, vict, TO_NOTVICT);
	return 1;
}


void print_group(struct char_data *ch)
{
	struct char_data *k;
	struct follow_type *f;
	int hit_perc, mana_perc, mov_perc;
	
	if (!IS_AFFECTED(ch, AFF_GROUP))
		send_to_char("Ma non sei membro di nessun gruppo!\r\n", ch);
	else
	{
		if (GROUPNAME(ch))
			sprintf(buf,"Il gruppo &5%s&0 e' formato da:\r\n", GROUPNAME(ch));
		else
			sprintf(buf,"Il tuo gruppo e' formato da:\r\n");
		send_to_char(buf, ch);
		k = (ch->master ? ch->master : ch);
		if (IS_AFFECTED(k, AFF_GROUP))
		{
			mana_perc = GET_MAX_MANA(k) ? GET_MANA(k)*100/GET_MAX_MANA(k) : 0;
			hit_perc = GET_MAX_HIT(k) ? GET_HIT(k)*100/GET_MAX_HIT(k) : 0;
			mov_perc = GET_MAX_MOVE(k) ? GET_MOVE(k)*100/GET_MAX_MOVE(k) : 0;
			
			if (!IS_AFFECTED(k, AFF_CONGIUNZIONE))
				sprintf(buf, "     [%3d%% H %3d%% M %3d%% V] [%s] $N (Capo del gruppo)",
					hit_perc, mana_perc, mov_perc, CLASS_ABBR(k));
			else	  
				sprintf(buf, "     [%3d%% H %3d%% M %3d%% V] [%s] $N (Capo del gruppo e della Cerchia)",
					hit_perc, mana_perc, mov_perc, CLASS_ABBR(k));
//					GET_HIT(k), GET_MANA(k), GET_MOVE(k), CLASS_ABBR(k));
			act(buf, FALSE, ch, 0, k, TO_CHAR);
		}
		
		for (f = k->followers; f; f = f->next)
		{
			if (!IS_AFFECTED(f->follower, AFF_GROUP)) continue;
			mana_perc = GET_MAX_MANA(f->follower) ? GET_MANA(f->follower)*100/GET_MAX_MANA(f->follower) : 0;
			hit_perc = GET_MAX_HIT(f->follower) ? GET_HIT(f->follower)*100/GET_MAX_HIT(f->follower) : 0;
			mov_perc = GET_MAX_MOVE(f->follower) ? GET_MOVE(f->follower)*100/GET_MAX_MOVE(f->follower) : 0;
			
			if (!IS_AFFECTED(f->follower, AFF_CONGIUNZIONE))
				sprintf(buf, "     [%3d%% H %3d%% M %3d%% V] [%s] $N",
					hit_perc, mana_perc, mov_perc, CLASS_ABBR(f->follower));
			else
				sprintf(buf, "     [%3d%% H %3d%% M %3d%% V] [%s] $N (Nella Cerchia)",
					hit_perc, mana_perc, mov_perc, CLASS_ABBR(f->follower));
			act(buf, FALSE, ch, 0, f->follower, TO_CHAR);
		}
	}
}


ACMD(do_group)
{
	struct char_data *vict;
	struct follow_type *f;
	int found;
	
	half_chop(argument, buf, buf2);
	
	if (!*buf)
	{
		print_group(ch);
		return;
	}	
	
	if ((*buf2) && (strlen(buf2)>30))
	{
		send_to_char("Nome del gruppo troppo lungo. Max 30 ch!", ch);
		return;
	}
	
	if (ch->master)
	{
		act("Non puoi arruolare membri nel gruppo se non sei il capo del gruppo.",
			FALSE, ch, 0, 0, TO_CHAR);
		return;
	}
	
	if (!ch->followers){
		act("Non puoi creare un gruppo da solo.", FALSE, ch, 0, 0, TO_CHAR);
		return;}
	
	
	if (!str_cmp(buf, "tutti") || !str_cmp(buf, "all"))
	{
		if (*buf2) GROUPNAME(ch) =  str_dup(buf2);
		perform_group(ch, ch);
		for (found = 0, f = ch->followers; f; f = f->next)
			if (RIDING(ch) && MOB_FLAGGED(RIDING(ch),MOB_MOUNTABLE))
			{ if (f->follower != RIDING(ch)) found += perform_group(ch, f->follower);}
			else found += perform_group(ch, f->follower);
		if (!found)
			send_to_char("Tutti stanno gia' nel tuo gruppo.\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else if ((vict->master != ch) && (vict != ch))
		act("$N deve fare follow per entrare nel gruppo.", FALSE, ch, 0, vict, TO_CHAR);
	else if (vict == ch)	 
		act("Non puoi creare un gruppo da solo.", FALSE, ch, 0, vict, TO_CHAR);
	else if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT ){
		send_to_char("Non puoi creare un gruppo con un fantasma.\r\n",ch);
		return;
		}
	else if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non puoi gruppare con personaggi che non giocano in modalita gdr.\r\n",ch);
		return;
	}	 	
	else
	{
		if (!IS_AFFECTED(vict, AFF_GROUP))
			if (RIDING(ch) && MOB_FLAGGED(RIDING(ch),MOB_MOUNTABLE))
			{ if (vict != RIDING(ch)) perform_group(ch, vict); }
			else {perform_group(ch, vict);}
		else
		{
			if (ch != vict) {
				if (GROUPNAME(vict))
					sprintf(buf,"$N non e' piu' membro del gruppo &5%s&0 comandato da te.",GROUPNAME(vict));
				else
					sprintf(buf,"$N non e' piu' membro del tuo gruppo.");
				act(buf, FALSE, ch, 0, vict, TO_CHAR);
			}
			if (GROUPNAME(vict))
				sprintf(buf,"Sei stato messo fuori dal gruppo &5%s&0 di $n!",GROUPNAME(vict));
			else
				sprintf(buf,"Sei stato messo fuori dal gruppo di $n!");
			act(buf, FALSE, ch, 0, vict, TO_VICT);
			if (GROUPNAME(vict))
				sprintf(buf,"$N e' stato messo fuori dal gruppo &5%s&0 di $n!",GROUPNAME(vict));
			else
				sprintf(buf,"$N e' stato messo fuori dal gruppo di $n!");
			act(buf, FALSE, ch, 0, vict, TO_NOTVICT);
			REMOVE_BIT(AFF_FLAGS(vict), AFF_GROUP);
			if (GROUPNAME(vict)){ free(GROUPNAME(vict)); GROUPNAME(vict)= NULL;};
		}
	}
}


ACMD(do_ungroup)
{
	struct follow_type *f, *next_fol;
	struct char_data *tch;
	void stop_follower(struct char_data * ch);
	room_rnum to_room;
	
	one_argument(argument, buf);
	
	if (!*buf)
	{
		if (ch->master || !(IS_AFFECTED(ch, AFF_GROUP)))
		{
			send_to_char("Ma tu non sei capo di nessun gruppo!\r\n", ch);
			return;
		}
		if (GROUPNAME(ch))
			sprintf(buf2, "%s ha sciolto il gruppo &5%s&0.\r\n", GET_NAME(ch), GROUPNAME(ch));
		else
			sprintf(buf2, "%s ha sciolto il gruppo.\r\n", GET_NAME(ch));
		for (f = ch->followers; f; f = next_fol)
		{
			next_fol = f->next;
			if (IS_AFFECTED(f->follower, AFF_GROUP))
			{
				REMOVE_BIT(AFF_FLAGS(f->follower), AFF_GROUP);
				send_to_char(buf2, f->follower);
				
				if (GROUPNAME(f->follower)){
					free(GROUPNAME(f->follower));
					GROUPNAME(f->follower)= NULL;
				}
				if (!IS_AFFECTED(f->follower, AFF_CHARM)){
					if (IS_NPC (f->follower) && IS_SET (MOB_FLAGS (f->follower), MOB_AMMAESTRABILE) && MOB_FLAGGED (f->follower, MOB_MOUNTABLE) && !MOB_FLAGGED (f->follower, MOB_HARD_TAME)){
						act ("$n smette di avere fiducia in te e scappa via.", FALSE, f->follower, 0, ch, TO_VICT);
						act ("$n smette di avere fiducia in $N e scappa via.", FALSE, f->follower, 0, ch, TO_NOTVICT);
						do {
							to_room = number(0, top_of_world);
						} while (world[to_room].number < 1400400 || world[to_room].number > 1599599 || !(strcmp(world[to_room].name, "Mare di Daresia")));
						dismount_char(f->follower);
						char_from_room (f->follower);
						char_to_room (f->follower, to_room);
					}
					stop_follower(f->follower);
				}
				else if (IS_NPC (f->follower) && IS_SET (MOB_FLAGS (f->follower), MOB_AMMAESTRABILE)){
					act ("$n smette di avere fiducia in te e scappa via.", FALSE, f->follower, 0, ch, TO_VICT);
					act ("$n smette di avere fiducia in $N e scappa via.", FALSE, f->follower, 0, ch, TO_NOTVICT);
					do {
						to_room = number(0, top_of_world);
					} while (world[to_room].number < 1400400 || world[to_room].number > 1599599 || !(strcmp(world[to_room].name, "Mare di Daresia")));
					dismount_char(f->follower);
					char_from_room (f->follower);
					char_to_room (f->follower, to_room);
				}
			}
		}
		
		REMOVE_BIT(AFF_FLAGS(ch), AFF_GROUP);
		
		if (GROUPNAME(ch)){
			sprintf(buf, "Hai sciolto il gruppo &5%s&0.\r\n",GROUPNAME(ch));
			free(GROUPNAME(ch));
			GROUPNAME(ch)= NULL;
		} else sprintf(buf, "Hai sciolto il gruppo.\r\n");
		send_to_char(buf, ch);
		return;
	}
	
	if (!(tch = get_char_room_vis(ch, buf)))
	{
		send_to_char("Non c'e' questa persona!\r\n", ch);
		return;
	}
	
	if (tch->master != ch)
	{
		send_to_char("Quella persona non ti sta seguendo!\r\n", ch);
		return;
	}
	
	if (!IS_AFFECTED(tch, AFF_GROUP))
	{
		send_to_char("Quella persona non e' nel tuo gruppo.\r\n", ch);
		return;
	}
	
	REMOVE_BIT(AFF_FLAGS(tch), AFF_GROUP);
	if (GROUPNAME(tch))
		sprintf(buf,"$N non e' piu parte del gruppo &5%s&0 comandato da te.",GROUPNAME(tch));
	else
		sprintf(buf,"$N non e' piu parte del tuo gruppo.");
	act(buf, FALSE, ch, 0, tch, TO_CHAR);
	if (GROUPNAME(tch))
		sprintf(buf,"Sei stato allontanato dal gruppo &5%s&0 di $n!",GROUPNAME(tch));
	else
		sprintf(buf,"Sei stato allontanato dal gruppo di $n!");
	act(buf, FALSE, ch, 0, tch, TO_VICT);
	if (GROUPNAME(tch))
		sprintf(buf,"$N e' stato allontanato dal gruppo &5%s&0 di $n!",GROUPNAME(tch));
	else
		sprintf(buf,"$N e' stato allontanato dal gruppo di $n!");
	act(buf, FALSE, ch, 0, tch, TO_NOTVICT);
	
	if (GROUPNAME(tch)){
		free(GROUPNAME(tch));
		GROUPNAME(tch)= NULL;
	}
	if (!IS_AFFECTED(tch, AFF_CHARM) && (!IS_NPC(tch) || !IS_SET (MOB_FLAGS (tch), MOB_AMMAESTRABILE) || !MOB_FLAGGED (tch, MOB_MOUNTABLE) || MOB_FLAGGED (tch, MOB_HARD_TAME)))
		stop_follower(tch);
}


ACMD(do_report)
{
	struct char_data *k;
	struct follow_type *f;
	
	if (!IS_AFFECTED(ch, AFF_GROUP))
	{
		send_to_char("Ma non sei membro di nessun gruppo!\r\n", ch);
		return;
	}
	sprintf(buf, "%s riporta: %d/%dH, %d/%dM, %d/%dV\r\n",
		GET_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
		GET_MANA(ch), GET_MAX_MANA(ch),
		GET_MOVE(ch), GET_MAX_MOVE(ch));
	CAP(buf);
	k = (ch->master ? ch->master : ch);
	for (f = k->followers; f; f = f->next)
		if (IS_AFFECTED(f->follower, AFF_GROUP) && f->follower != ch)
			send_to_char(buf, f->follower);
	if (k != ch)
		send_to_char(buf, k);
	send_to_char("Fai rapporto al gruppo.\r\n", ch);
}


ACMD(do_split)
{
	int amount, num, share;
	struct char_data *k;
	struct follow_type *f;
	
	if (IS_NPC(ch)) return;
	
	one_argument(argument, buf);
	
	if (is_number(buf))
	{
		amount = atoi(buf);
		if (amount <= 0)
		{
			send_to_char("Mi spiace, non puoi farlo.\r\n", ch);
			return;
		}
		if (amount > GET_GOLD(ch))
		{
			send_to_char("Non sembra che tu abbia abbastanza monete da dividere.\r\n", ch);
			return;
		}
		k = (ch->master ? ch->master : ch);
		if (IS_AFFECTED(k, AFF_GROUP) && (k->in_room == ch->in_room)) num = 1;
		else num = 0;
		
		for (f = k->followers; f; f = f->next)
			if (IS_AFFECTED(f->follower, AFF_GROUP) &&
				(!IS_NPC(f->follower)) &&
				(f->follower->in_room == ch->in_room))
				num++;
			
		if (num && IS_AFFECTED(ch, AFF_GROUP)) share = amount / num;
		else
		{
			send_to_char("Con chi desideri dividere le tue monete ?\r\n", ch);
			return;
		}
		
		GET_GOLD(ch) -= share * (num - 1);
		if (IS_AFFECTED(k, AFF_GROUP) && (k->in_room == ch->in_room)
			&& !(IS_NPC(k)) && k != ch)
		{
			GET_GOLD(k) += share;
			sprintf(buf, "%s divide %d coins; la tua parte e' %d.\r\n", GET_NAME(ch),
				amount, share);
			send_to_char(buf, k);
		}
		for (f = k->followers; f; f = f->next)
		{
			if (IS_AFFECTED(f->follower, AFF_GROUP) &&
				(!IS_NPC(f->follower)) &&
				(f->follower->in_room == ch->in_room) &&
				f->follower != ch)
			{
				GET_GOLD(f->follower) += share;
				sprintf(buf, "%s divide %d monete; la tua parte e' %d.\r\n", GET_NAME(ch),
					amount, share);
				send_to_char(buf, f->follower);
			}
		}
		sprintf(buf, "Dividi %d monete con %d membri -- %d monete a testa.\r\n",
			amount, num, share);
		send_to_char(buf, ch);
	}
	else
	{
		send_to_char("Quante monete vuoi dividere con il gruppo ?\r\n", ch);
		return;
	}
}


void gen_item_use(struct char_data * ch, struct obj_data * obj,char *argument)
{
	int k;
	struct char_data *tch = NULL;
	struct obj_data *tobj = NULL;
	
	one_argument(argument, arg);
	
	k = generic_find(arg, FIND_CHAR_ROOM | FIND_OBJ_INV | FIND_OBJ_ROOM |
			FIND_OBJ_EQUIP, ch, &tch, &tobj);
	
	
	if(tch==ch){
		sprintf(buf,"Nonostante ti metta d'impegno non ci riesci.\n");
		send_to_char(buf,ch);
		return;
	}
	if(tch!=NULL){
		if((GET_POS(tch)>=POS_RESTING)||(!IS_AFFECTED(tch,AFF_TRAMORTITO)&&!IS_AFFECTED(tch,AFF_SLEEP)&&!IS_AFFECTED(tch,AFF_PARALIZE)&&!IS_AFFECTED(tch,AFF_IMMOBIL))){
			sprintf(buf,"Devi renderlo inoffensivo prima!\n");
			send_to_char(buf,ch);
			return;
		}
		
		switch (GET_OBJ_TYPE(obj)) {
			case ITEM_IMMOBIL:  /*Per immobilizzare un pg....*/
				
				if(IS_AFFECTED(tch,AFF_IMMOBIL)){
					send_to_char("Ma se e' gia legato!",ch);
					return;
				}
				else{
					sprintf(buf,"\rLeghi %s come un salame,ora sara difficile che riesca a muoversi!\n",GET_NAME(tch));
					send_to_char(buf,ch);
					act("\r$n lega $N come un salame,ora sara difficile che riesca a muoversi!!\n",FALSE,ch,0,tch,TO_ROOM);
					obj_to_char((obj=unequip_char(ch,WEAR_HOLD)), ch);
					obj_from_char(obj);
					equip_char(tch,obj,WEAR_IMMOBIL);
					SET_BIT( AFF_FLAGS(tch),AFF_IMMOBIL);
					return;
				}
				break;
			case ITEM_BAVAGLIO:
				if(GET_EQ(tch,WEAR_BOCCA)){
					send_to_char("E' gia imbavagliato!.\n",ch);
					return;
				}
				else{
					sprintf(buf,"\rTappi la bocca a %s con %s.\n",GET_NAME(tch),obj->short_description);
					send_to_char(buf,ch);
					act("\r$n tappa la bocca a $N con $p!!\n",FALSE,ch,obj,tch,TO_ROOM);
					obj_to_char((obj=unequip_char(ch,WEAR_HOLD)), ch);
					obj_from_char(obj);
					equip_char(tch,obj,WEAR_BOCCA);
					SET_BIT( PLR_FLAGS(tch),PLR_MUTO);
					return;
				}
				break;
			case ITEM_BENDA:
				if(IS_AFFECTED(tch,AFF_BLIND)){
					send_to_char("E' gia cecato il poverello!\n",ch);
					return;
				}
				else{
					sprintf(buf,"\rBendi %s con perizia,sara' difficile che riesca a vederci!\n",GET_NAME(tch));
					send_to_char(buf,ch);
					act("\r$n benda $N con perizia,sara difficile che riesca a vederci!!\n",FALSE,ch,0,tch,TO_ROOM);
					obj_to_char((obj=unequip_char(ch,WEAR_HOLD)), ch);
					obj_from_char(obj);
					equip_char(tch,obj,WEAR_EYE);
					SET_BIT( AFF_FLAGS(tch),AFF_BLIND);
					return;
				}
				break;
      //add here new cases
			default:
				log("SYSERR: Unknown object_type in gen_item_use");
				break;
		}
	}
}
ACMD(do_use)
{
	struct obj_data *mag_item, *tmp_item;
	int equipped = 1;
	
	half_chop(argument, arg, buf);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)) {
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch)) {
		send_to_char("Devi prima scendere\r\n",ch);
		return;
	}
	if (!*arg) {
		sprintf(buf2, "Cosa vuoi usare %s?\r\n", CMD_NAME);
		send_to_char(buf2, ch);
		return;
	}
	mag_item = GET_EQ(ch, WEAR_HOLD);
	
	if (!mag_item || !isname(arg, mag_item->name))
	{
		switch (subcmd) {
			case SCMD_RECITE:
				
				equipped = 0;
				if (!(mag_item = get_obj_in_list_vis(ch, arg, ch->carrying))) {
					sprintf(buf2, "Non sembra tu abbia  %s.\r\n", arg);
					send_to_char(buf2, ch);
					return;
				}
				break;
			case SCMD_QUAFF:
				equipped = 0;
				if (!(mag_item = get_obj_in_list_particular(ch, arg, ch->carrying, ITEM_POTION))) {
					sprintf(buf2, "Non sembra tu abbia  %s.\r\n", arg);
					send_to_char(buf2, ch);
					return;
				}
				break;
			case SCMD_USE:
			/* Ardanos: oltre alle bende_unte ci sono anche le bende, i bavagli e le manette che devono esser holdate
			 * quindi bisogna anche checkare il mag_item (che e' WEAR_HOLD) */
				if (!mag_item || (!(tmp_item = get_obj_in_list_vis(ch, arg, ch->carrying)) || GET_OBJ_TYPE (tmp_item) == ITEM_BENDE_UNTE)){  
					sprintf(buf2, "Non sembra tu stia tenendo %s.\r\n", arg);
					send_to_char(buf2, ch);
					return;
				}
				mag_item = tmp_item;
				break;
			default:
				log("SYSERR: Unknown subcmd passed to do_use");
				return;
				break;
		}
	}
	switch (subcmd) {
		case SCMD_QUAFF:
			if (GET_OBJ_TYPE(mag_item) != ITEM_POTION && GET_OBJ_TYPE(mag_item) != ITEM_TISANA_CALDA) {
				send_to_char("Puoi solo usare quaff per bere le pozioni.", ch);
				return;
			}
			break;
		case SCMD_RECITE:
			if (GET_OBJ_TYPE(mag_item) != ITEM_SCROLL) {
				send_to_char("Puoi usare solo recite per leggere gli scroll.", ch);
				return;
			}
			if(!test_and_improve(ch, SKILL_READ_MAGIC, 80, 15, (GET_INT(ch)+GET_WIS(ch)-20)*2)) {
				send_to_char("Non riesci a comprendere appieno i simboli magici che hai letto!\r\n", ch);
				return;
			}
			break;
		case SCMD_USE:
			if (   (GET_OBJ_TYPE(mag_item) != ITEM_WAND) 
			    && (GET_OBJ_TYPE(mag_item) != ITEM_STAFF)
			    && (GET_OBJ_TYPE(mag_item) != ITEM_IMMOBIL)
			    && (GET_OBJ_TYPE(mag_item) != ITEM_BENDA)
			    && (GET_OBJ_TYPE(mag_item) != ITEM_BAVAGLIO)
			    && (GET_OBJ_TYPE(mag_item) != ITEM_BENDE_UNTE)
				 ) {
				send_to_char("Non riesci a trovare un modo per usarlo.\r\n", ch);
				return;
			}
			if (((GET_OBJ_TYPE(mag_item) == ITEM_WAND) || (GET_OBJ_TYPE(mag_item) == ITEM_STAFF)) &&
				(IS_WARRIOR(ch) || IS_DARESIANO(ch))) {
				send_to_char("Quelli come te non hanno molto feeling con questi oggetti...\r\n", ch);
				return;
			}
			break;
	}
	
	if (!IS_NPC(ch) && ( (GET_LEVEL(ch)<GET_OBJ_LEVEL(mag_item)) || ( GET_OBJ_LEVEL(mag_item)<=0 && (GET_LEVEL(ch)<objlevel(mag_item)) ) ) )
	{
		act("Sei troppo inesperto per utilizzare $p, rinunci.", FALSE, ch, mag_item, 0, TO_CHAR);
		act("$n e' troppo inesperto per usare $p, rinuncia.", FALSE, ch, mag_item, 0, TO_ROOM);
		return;
	}
	if(   (GET_OBJ_TYPE(mag_item) == ITEM_IMMOBIL)
	   || (GET_OBJ_TYPE(mag_item) == ITEM_BENDA)  
	   || (GET_OBJ_TYPE(mag_item) == ITEM_BAVAGLIO)
		)
		gen_item_use(ch, mag_item, buf);
	else
		mag_objectmagic(ch, mag_item, buf);
}


ACMD(do_wimpy)
{
	int wimp_lev;
	
	one_argument(argument, arg);
	
	if (!*arg)
	{
		if (GET_WIMP_LEV(ch))
		{
			sprintf(buf, "Ora il tuo wimpy e' %d punti hit.\r\n", GET_WIMP_LEV(ch));
			send_to_char(buf, ch);
			return;
		}
		else
		{
			send_to_char("Al momento non hai settato il wimpy.  (sicuro sicuro..?)\r\n", ch);
			return;
		}
	}
	if (isdigit(*arg))
	{
		if ((wimp_lev = atoi(arg)))
		{
			if (wimp_lev < 0)
				send_to_char("Heh, heh, heh.. oggi hai voglia di scherzare, eh ?\r\n", ch);
			else if (wimp_lev > GET_MAX_HIT(ch))
				send_to_char("Non ha molto senso, vero ?\r\n", ch);
			else if (wimp_lev > (GET_MAX_HIT(ch) / 2))
				send_to_char("Non puoi mettere il wimpy al di sopra della meta' dei tuoi hit points.\r\n", ch);
			else
			{
				sprintf(buf, "Okay, wimpy messo a  %d hit points.\r\n", wimp_lev);
				send_to_char(buf, ch);
				GET_WIMP_LEV(ch) = wimp_lev;
			}
		}
		else
		{
			send_to_char("Okay, ora combatterai fino all'ultimo.\r\n", ch);
			GET_WIMP_LEV(ch) = 0;
		}
	}
	else send_to_char("Specifica a quanti hit points vuoi mettere il wimpy (0 per disabilitare).\r\n", ch);
}


ACMD(do_display)
{
	size_t i;
	
	if (IS_NPC(ch)) {
		send_to_char("I mob non hanno bisogno del prompt, vattene via.\r\n", ch);
		return;
	}
	skip_spaces(&argument);
	
	if (!*argument) {
		send_to_char("Utilizzo: prompt { { H | M | V | X | D | C | static | all | none  }\r\n", ch);
		return;
	}
	
	if ((!str_cmp(argument, "on")) || (!str_cmp(argument, "all")))
		SET_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA | PRF_DISPMOVE | PRF_DISPXP | PRF_DISPDAM | PRF_DISPCAV);
	else if (!str_cmp(argument, "static"))
		SET_BIT(PRF_FLAGS(ch), PRF_DISPSTATIC);
	else {
		if (!str_cmp(argument, "none") || !str_cmp(argument, "off"))
		  REMOVE_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA | PRF_DISPMOVE | PRF_DISPDAM | PRF_DISPXP | PRF_DISPCAV | PRF_DISPSTATIC);
		if (str_cmp(argument, "none") && str_cmp(argument, "off"))
		for (i = 0; i < strlen(argument); i++) {
				switch (LOWER(argument[i])) {
					case 'h':
						SET_BIT(PRF_FLAGS(ch), PRF_DISPHP);
						break;
					case 'm':
						SET_BIT(PRF_FLAGS(ch), PRF_DISPMANA);
						break;
					case 'v':
						SET_BIT(PRF_FLAGS(ch), PRF_DISPMOVE);
						break;
					case 'x':
						SET_BIT(PRF_FLAGS(ch), PRF_DISPXP);
						break;
					case 'd':
						SET_BIT(PRF_FLAGS(ch), PRF_DISPDAM);
						break;
					case 'c':
						SET_BIT(PRF_FLAGS(ch), PRF_DISPCAV);
						break;
					default:
						send_to_char("Usage: prompt { { H | M | V | X | G | D | C} | static | all | none }\r\n", ch);
						return;
						break;
				}
			}
	}
	send_to_char(OK, ch);
}


ACMD(do_gen_write)
{
	FILE *fl;
	char *tmp, *filename, buf[MAX_STRING_LENGTH];
	struct stat fbuf;
	extern int max_filesize;
	time_t ct;
	
	switch (subcmd)
	{
		case SCMD_PRAY:
			filename = PRAY_FILE;
			break;
		case SCMD_BUG:
			filename = BUG_FILE;
			break;
		case SCMD_TYPO:
			filename = TYPO_FILE;
			break;
		case SCMD_IDEA:
			filename = IDEA_FILE;
			break;
		default:
			return;
	}
	
	ct = time(0);
	tmp = asctime(localtime(&ct));
	
  /*  if (IS_NPC(ch))
    {
    send_to_char("I mostri non hanno il diritto di avere idee, vattene via.\r\n", ch);
    return;
    }*/
	
	skip_spaces(&argument);
	delete_doubledollar(argument);
	
	if (!*argument)
	{
		send_to_char("Deve esserci un errore...\r\n", ch);
		return;
	}
	sprintf(buf, "%s %s: %s", GET_NAME(ch), CMD_NAME, argument);
	mudlog(buf, CMP, LVL_IMMORT, FALSE);
	
	if (stat(filename, &fbuf) < 0)
	{
		perror("Error statting file - do_gen_write");
		return;
	}
	
	if (fbuf.st_size >= max_filesize)
	{
		send_to_char("Mi spiace il file e' pieno al momento, prova piu' tardi.\r\n", ch);
		return;
	}
	
	if (!(fl = fopen(filename, "a")))
	{
		perror("do_gen_write");
		send_to_char("Mi spiace non rieso ad aprire il file.\r\n", ch);
		return;
	}
	fprintf(fl, "%-8s (%6.6s) [%5d] %s\n", GET_NAME(ch), (tmp + 4),
		world[ch->in_room].number, argument);
	fclose(fl);
	if (SCMD_PRAY)
	{
		sprintf(buf, "%s inoltra una preghiera ai creatori.", GET_NAME(ch));
		mudlog(buf, NRM, LVL_GOD, TRUE);
		send_to_char("Okay. La preghiera e' stata inoltrata ai Creatori!\r\n", ch);
	}
	else send_to_char("Okay. Grazie !\r\n", ch);
}


#define TOG_OFF 0
#define TOG_ON  1
#define PRF_TOG_CHK(ch,flag) ((TOGGLE_BIT(PRF_FLAGS(ch), (flag))) & (flag))

ACMD(do_gen_tog)
{
	long long int result;
	extern int nameserver_is_slow;
	
	char *tog_messages[][2] = {
		{"Non puoi essere evocato da altri giocatori.\r\n",
			"Puoi essere evocato da altri giocatori.\r\n"},
		{"Nohassle disabled.\r\n",
			"Nohassle enabled.\r\n"},
		{"Modo breve disattivato.\r\n",
			"Modo breve attivato.\r\n"},
		{"Modo compatto disabilitato.\r\n",
			"Modo compatto abilitato.\r\n"},
		{"You can now hear tells.\r\n",
			"You are now deaf to tells.\r\n"},
		{"Aste abilitate.\r\n",
			"Aste disabilitate.\r\n"},
		{"You can now hear shouts.\r\n",
			"You are now deaf to shouts.\r\n"},
		{"You can now hear gossip.\r\n",
			"You are now deaf to gossip.\r\n"},
		{"You can now hear the congratulation messages.\r\n",
			"You are now deaf to the congratulation messages.\r\n"},
		{"You can now hear the Wiz-channel.\r\n",
			"You are now deaf to the Wiz-channel.\r\n"},
		{"Non fai piu' parte della Quest.\r\n",
			"Okay, sei parte della Quest!\r\n"},
		{"You will no longer see the room flags.\r\n",
			"You will now see the room flags.\r\n"},
		{"You will now have your communication repeated.\r\n",
			"You will no longer have your communication repeated.\r\n"},
		{"HolyLight mode off.\r\n",
			"HolyLight mode on.\r\n"},
		{"Nameserver_is_slow changed to NO; IP addresses will now be resolved.\r\n",
			"Nameserver_is_slow changed to YES; sitenames will no longer be resolved.\r\n"},
		{"Autoexits disattivato.\r\n",
			"Autoexits attivato.\r\n"},
		{"Sei gia' stato afk di recente, non provarci!\r\n",
		  AFK_MESSG},
		{"Autodir disattivato.\r\n",
			"Autodir attivato.\r\n"},
		{"You will no longer seee arena messages.\r\n",
			"You will now see arena messages.\r\n"} ,
		{"Disattivato il protocollo MSP per i suoni.\r\n",
			"Attivato il protocollo MSP per i suoni.\r\n"},
		{"Disattivato il detect hide e sneak.\r\n",
			"Attivato il detect hide e sneak.\r\n"},
		{"Non verrai piu' elencato nella classifica del comando eroi.\r\n",
			"Verrai elencato nella classifica del comando eroi.\r\n"},
		{"Non sei piu' in modalita' Gioco di Ruolo.\r\n",
			"Ora ti trovi in modalita' Gioco di Ruolo, comportati di conseguenza.\r\n"},
		{" "," "},
		{"Attivato il canale Clan.\r\n",
			"Disattivato il canale Clan.\r\n"},
		{"Attivato il canale OOC.\r\n",
			"Disattivato il canale OOC.\r\n"}
	};
	
	
	//if (IS_NPC(ch))
	if (!ch->desc)
		return;
	
	switch (subcmd) {
		case SCMD_NOSUMMON:
			result = PRF_TOG_CHK(ch, PRF_SUMMONABLE);
			break;
		case SCMD_NOHASSLE:
			result = PRF_TOG_CHK(ch, PRF_NOHASSLE);
			break;
		case SCMD_BRIEF:
			result = PRF_TOG_CHK(ch, PRF_BRIEF);
			break;
		case SCMD_COMPACT:
			result = PRF_TOG_CHK(ch, PRF_COMPACT);
			break;
		case SCMD_NOTELL:
			result = PRF_TOG_CHK(ch, PRF_NOTELL);
			break;
		case SCMD_NOAUCTION:
			result = PRF_TOG_CHK(ch, PRF_NOAUCT);
			break;
		case SCMD_DEAF:
			result = PRF_TOG_CHK(ch, PRF_DEAF);
			break;
		case SCMD_NOGOSSIP:
			result = PRF_TOG_CHK(ch, PRF_NOGOSS);
			break;
		case SCMD_NOGRATZ:
			result = PRF_TOG_CHK(ch, PRF_NOGRATZ);
			break;
		case SCMD_NOWIZ:
			result = PRF_TOG_CHK(ch, PRF_NOWIZ);
			break;
		case SCMD_QUEST:
			result = PRF_TOG_CHK(ch, PRF_QUEST);
			break;
		case SCMD_ROOMFLAGS:
			result = PRF_TOG_CHK(ch, PRF_ROOMFLAGS);
			break;
		case SCMD_NOREPEAT:
			result = PRF_TOG_CHK(ch, PRF_NOREPEAT);
			break;
		case SCMD_HOLYLIGHT:
			result = PRF_TOG_CHK(ch, PRF_HOLYLIGHT);
			break;
		case SCMD_SLOWNS:
			result = (nameserver_is_slow = !nameserver_is_slow);
			break;
		case SCMD_AUTOEXIT:
			result = PRF_TOG_CHK(ch, PRF_AUTOEXIT);
			break;
		case SCMD_AFK:
			result = 0;
			if (can_do_event(ch,LAST_AFK, 300)) {
				result = SET_BIT(PRF_FLAGS(ch), PRF_AFK);
				act("$n (AFK) Lascia il gioco.", TRUE, ch, 0, 0, TO_ROOM);
			}
			break;
		case SCMD_AUTODIR:
			result = PRF_TOG_CHK(ch, PRF_AUTODIR);
			break;
		case SCMD_ARENA:
			result = PRF_TOG_CHK(ch, PRF_ARENA);
			break;
		case SCMD_SOUNDS:
			result = PRF_TOG_CHK(ch, PRF_SOUNDS);
			break;
		case SCMD_HIDE_SNEAK:
			result = PRF_TOG_CHK(ch, PRF_HIDE_SNEAK);
			break;
		case SCMD_NOEROE:
			result = PRF_TOG_CHK(ch, PRF_NO_EROE);
			break;
/*
		case SCMD_RPG:{
			if((time(NULL)-ch->last_gdr)<180){
			    send_to_char("Devono passare almeno tre minuti dall'ultimo cambio.\r\n",ch);
			    return;
			    }
			else{
				result = PRF_TOG_CHK(ch, PRF_RPG);
				ch->last_gdr=time(NULL);
				if(!PRF_FLAGGED(ch,PRF_RPG)){
				    SET_BIT(PRF_FLAGS(ch),PRF_NOHASSLE);
				}
				else 
				    REMOVE_BIT(PRF_FLAGS(ch),PRF_NOHASSLE);    
				break;
			    }
			}
*/
		case SCMD_NOKILL:
			result = PRF_TOG_CHK(ch,PRF_NOKILL);
			if (PRF_FLAGGED(ch, PRF_NOKILL))
				act("OK da ora sei un pacifista.", TRUE, ch, 0, 0, TO_CHAR);
			else
				act("Ritorni ad essere quel bastardo sanguinario che eri.", TRUE, ch, 0, 0, TO_CHAR);
			break;
		case SCMD_NOCTELL:
			result = PRF_TOG_CHK(ch, PRF_REGNOTELL);
			break;
		case SCMD_NOOOC:
			result = PRF_TOG_CHK(ch, PRF_NOC);
			break;
		default:
			log("SYSERR: Unknown subcmd in do_gen_toggle");
			return;
			break;
	}
	
	if (result) 
	  send_to_char(tog_messages[subcmd][TOG_ON], ch);
	else 
	  send_to_char(tog_messages[subcmd][TOG_OFF], ch);
}


/* How to make a string look drunk... by Apex (robink@htsa.hva.nl) */
/* Modified and enhanced for envy(2) by the Maniac from Mythran    */
/* Ported to Stock Circle 3.0 by Haddixx (haddixx@megamed.com)     */

char * makedrunk (char *string, struct char_data * ch)
{
	
/* This structure defines all changes for a character */
	struct drunk_struct drunk[] =
	{
		{3, 10,
			{"a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh"}},
		{8, 5,
			{"b", "b", "b", "B", "B", "vb"}},
		{3, 5,
			{"c", "c", "C", "cj", "sj", "zj"}},
		{5, 2,
			{"d", "d", "D"}},
		{3, 3,
			{"e", "e", "eh", "E"}},
		{4, 5,
			{"f", "f", "ff", "fff", "fFf", "F"}},
		{8, 2,
			{"g", "g", "G"}},
		{9, 6,
			{"h", "h", "hh", "hhh", "Hhh", "HhH", "H"}},
		{7, 6,
			{"i", "i", "Iii", "ii", "iI", "Ii", "I"}},
		{9, 5,
			{"j", "j", "jj", "Jj", "jJ", "J"}},
		{7, 2,
			{"k", "k", "K"}},
		{3, 2,
			{"l", "l", "L"}},
		{5, 8,
			{"m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M"}},
		{6, 6,
			{"n", "n", "nn", "Nn", "nnn", "nNn", "N"}},
		{3, 6,
			{"o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo"}},
		{3, 2,
			{"p", "p", "P"}},
		{5, 5,
			{"q", "q", "Q", "ku", "ququ", "kukeleku"}},
		{4, 2,
			{"r", "r", "R"}},
		{2, 5,
			{"s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss"}},
		{5, 2,
			{"t", "t", "T"}},
		{3, 6,
			{"u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu"}},
		{4, 2,
			{"v", "v", "V"}},
		{4, 2,
			{"w", "w", "W"}},
		{5, 6,
			{"x", "x", "X", "ks", "iks", "kz", "xz"}},
		{3, 2,
			{"y", "y", "Y"}},
		{2, 9,
			{"z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ", "Z"}}
	};
	
	char buf[1024];      /* this should be enough (?) */
	char temp;
	int pos = 0;
	int randomnum;
	
	if(GET_COND(ch, DRUNK) > 0)  /* character is drunk */
	{
		do
		{
			temp = toupper(*string);
			if( (temp >= 'A') && (temp <= 'Z') )
			{
				if(GET_COND(ch, DRUNK) > drunk[(temp - 'A')].min_drunk_level)
				{
					randomnum = number(0, (drunk[(temp - 'A')].number_of_rep));
					strcpy(&buf[pos], drunk[(temp - 'A')].replacement[randomnum]);
					pos += strlen(drunk[(temp - 'A')].replacement[randomnum]);
				}
				else
					buf[pos++] = *string;
			}
			else
			{
				if ((temp >= '0') && (temp <= '9'))
				{
					temp = '0' + number(0, 9);
					buf[pos++] = temp;
				}
				else
					buf[pos++] = *string;
			}
		}while (*string++);
		
		buf[pos] = '\0';          /* Mark end of the string... */
		strcpy(string, buf);
		return(string);
	}
	return (string); /* character is not drunk, just return the string */
}

int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier)
{
	extern char *spells[];
	int r;
	
	if (IS_NPC(ch)) return 0;
	
	if (!CAN_LEARN_SKILL(ch, skill_num) || ((GET_SKILL(ch, skill_num)<=0)&&(GET_LEVEL(ch)<LVL_IMMORT))  )
		return 0;
	
	if (GET_LEVEL(ch)>=LVL_AVATAR)
		return 1;
	
	r=number(1,test+1);
	if (r <= (GET_SKILL(ch, skill_num)+modifier))
		return 1;
	
	if ((time(NULL)-ch->last_learned)<120) return 0; // Due minuti minimo per imparare qualcos'altro
	if (GET_SKILL(ch, skill_num)==100) return 0;
	r=number(1,100*20); // 20 indica la somma di INT e WIS per il PG medio
	if (r <= prob*(GET_INT(ch)+GET_WIS(ch)) + (GET_ABIL(ch, ABIL_STUDIO) / 3))
	{
		ch->last_learned=time(NULL);
		if (skill_num<=130 && ROOM_FLAGGED(ch->in_room, ROOM_NOMAGIC))
		{
			send_to_char("Per un attimo ti sembra di imparare qualcosa... ma e' solo un'impressione.\r\n", ch);
			return 0;
		}
		GET_SKILL(ch, skill_num)++;
		sprintf(buf, "Sbagliando si impara... migliori nell%s %s.\r\n",
			skill_num<=MAX_SPELLS ? "o spell" : "a skill", spells[skill_num]);
		send_to_char(buf, ch);
	}
	return 0;
}

int check_proficienza (struct char_data * ch, int prof_num, int test, int modifier)
{
	int r;
	
	if (GET_PROF(ch, prof_num) < 1) 
	  return 0;
	
	r = number(1, test+1);
	if (r <= (GET_PROF(ch, prof_num) + modifier))
		return 1;
	else 
	  return 0;
}

int check_proficienza_improve (struct char_data * ch, int prof_num, int prob)
{
	extern char *proficienze[];
	extern int classe_mestieri[TOT_PROF_MESTIERI][2];
	int r;
	
	if (GET_PROF(ch, prof_num)==0) return 0;
	if ((time(NULL)-ch->last_learned)<60) return 0; // Due minuti minimo per imparare qualcos'altro
	if ((GET_PROF(ch, prof_num)==100)||(GET_PROF(ch, prof_num)==0)) return 0; //se e' il massimo o non la conosci esci
	r=number(1,100*20); // 20 Indica la somma dei valori medi di INT e WIS
  //prob *= 1.5; // Correzione 12-09-99
	prob *= 1.25; // Correzione 29-09-99
	if (classe_mestieri[prof_num][2] == 2) prob *= 2; // se prof. secondaria
	if (r <= 0.25*prob*(GET_INT(ch)+GET_WIS(ch)) + (GET_ABIL(ch, ABIL_STUDIO) / 3))
	{
		ch->last_learned=time(NULL);
		GET_PROF(ch, prof_num)++;
		sprintf(buf, "Sbagliando si impara... migliori nella proficienza %s.\r\n", proficienze[prof_num]);
		send_to_char(buf, ch);
		return 1;
	}
	return 0;
}

ACMD(do_proficienze)
{
	void list_proficienze(struct char_data * ch);
	
	list_proficienze(ch);
}
ACMD(do_abilita)
{
	void list_abilita_guild(struct char_data * ch);
	void list_abilita(struct char_data * ch);
	if(GET_LEVEL(ch)<LVL_IMMORT)
		list_abilita_guild(ch);
	else
		list_abilita(ch);
}


ACMD(do_free){
	int check_abil_weapon(struct obj_data *weapon);
	struct char_data *get_char_room_vis(struct char_data * ch, char *name);
	int chance,difficolta=0,type_weapon=0;
	struct obj_data *manette,*weapon=NULL;
	struct char_data *vict=NULL;
	if(!*argument){
		if(!IS_AFFECTED(ch,AFF_IMMOBIL)){
			send_to_char("Ma se hai le mani libere!\n",ch);
			return;
		}
		if(IS_AFFECTED(ch,AFF_TRAMORTITO)){
			send_to_char("L'unica cosa che vorresti adesso e' che il malditesta ti passasse!\n",ch);
			return;
		}
		if(!GET_EQ(ch,WEAR_IMMOBIL))
		{
			send_to_char("Con grande facilita ti liberi! Sei libero!\n",ch);
			REMOVE_BIT(AFF_FLAGS(ch),AFF_IMMOBIL);
			return;
		}
		if(GET_HIT(ch)<10|| GET_MOVE(ch)<60){
			send_to_char("Sei Troppo Debole anche solo per Pensarci!\r\n",ch);
			return;
		}
		chance=GET_DEX(ch)+GET_STR(ch)+GET_ADD(ch)/2;
		if (CAN_LEARN_SKILL(ch, SKILL_PICK_LOCK) && GET_SKILL(ch,SKILL_PICK_LOCK))
			chance+=10;
		if (CAN_LEARN_SKILL(ch, SKILL_ENH_DAMAGE) && GET_SKILL(ch,SKILL_ENH_DAMAGE))
			chance +=10;
		chance +=number(1,40);
		if(GET_EQ(ch,WEAR_WIELD))
			chance +=30;
		manette=GET_EQ(ch,WEAR_IMMOBIL);
		difficolta=GET_OBJ_CSLOTS(manette)+GET_OBJ_VAL(manette,0)*10;
    /*ecco il tentativo vero e proprio*/
		alter_hit(ch,7);
		alter_move(ch,50);
		if(!CAN_LEARN_SKILL(ch, SKILL_SNEAK) || !GET_SKILL(ch,SKILL_SNEAK))
			act("$n si divincola furiosamente cercando di liberarsi.\n",FALSE,ch,0,0,TO_ROOM);
		if(chance>difficolta){
			REMOVE_BIT(AFF_FLAGS(ch),AFF_IMMOBIL);
			obj_to_char((manette=unequip_char(ch,WEAR_IMMOBIL)),ch);
			send_to_char("Et voila! Sei libero!\n",ch);
			GET_POS(ch)=POS_STANDING;
		}
		else{
			GET_OBJ_CSLOTS(manette)-=(GET_DAMROLL(ch)+MAX(1,str_app[GET_STR(ch)].todam))*10/MAX(1,GET_OBJ_VAL(manette,0));
			
			
			if (GET_OBJ_CSLOTS(manette) <= 0){
				send_to_char("Riesci finalmente a spezzare le catene che ti immobilizzavano,sei libero!\n",ch);
				obj_to_char((manette=unequip_char(ch,WEAR_IMMOBIL)),ch);
				obj_from_char(manette);
				REMOVE_BIT(AFF_FLAGS(ch),AFF_IMMOBIL);
				GET_POS(ch)=POS_STANDING;
				return;
			}
			send_to_char("Nonostante i tuoi sforzi non riesci a liberarti, e ti fai pure male!",ch);
			GET_OBJ_CSLOTS(manette)-=(GET_DAMROLL(ch)+MAX(1,str_app[GET_STR(ch)].todam))*5;
			return;
		}
	}
	else{
		if(!(vict=get_char_room_vis(ch,argument))){
			send_to_char("Non e' qui!",ch);
			return;
		}
		if(!GET_EQ(vict,WEAR_IMMOBIL)){
			send_to_char("Non e' ammanettato!",ch);
			return;
		}
		if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		    send_to_char("Non puoi liberare un fantasma.\r\n",ch);
		    return;
		    }
		if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		    send_to_char("Non sembra stia giocando al tuo stesso gioco.\r\n",ch);
		    return;
		}
		if(!(weapon=GET_EQ(ch,WEAR_WIELD))||(weapon&&(type_weapon=check_abil_weapon(weapon)!=ABIL_PUGNALE&&type_weapon!=ABIL_SPADA&&type_weapon!=ABIL_ASCIA))){
			send_to_char("Hai bisogno di una lama per farlo!",ch);
			return;
		}
		obj_to_char((manette=unequip_char(vict,WEAR_IMMOBIL)),vict);
		obj_from_char(manette);
		REMOVE_BIT(AFF_FLAGS(vict),AFF_IMMOBIL);
		act("Liberi $N.Ne sara' felice.",FALSE,ch,0,vict,TO_CHAR);
		act("$n ti libera i polsi.Ringrazialo!.",FALSE,ch,0,vict,TO_VICT);
		act("$n libera i polsi a $N.",FALSE,ch,0,vict,TO_NOTVICT);
	}
}

ACMD(do_prigioniero){
	if(TRANSPORTED(ch))
		send_to_char("Devi andare in prigione per consegnare il prigioniero!!\n",ch);
	
	else
		send_to_char("Non hai nessun prigioniero!",ch);
	return;
}

#define VNUM_KIT_EVASIONE 81

ACMD(do_evadi)
{
	long cella;
	int fuga;
	int coef_evasione;
	int fortuna;
	int tentativo;
	int riuscita;
	int regno = -1;
	struct obj_data *get_obj_in_list_vis_vnum(struct char_data *ch,int vnumber,struct obj_data * list);
	
	
	if (!can_do_event(ch,LAST_PICK, 30)) /*Impostata tentativo di evasione ogni 30 sec. sfruttato LAST_PICK per non creare LAST_EVADI */
	{
		send_to_char("&3&bL'evasione e' un arte. C'e' bisogno di tempo!&0\r\n", ch);
		return;
	}
	
	if (!ROOM_FLAGGED(ch->in_room, ROOM_PRISON))
	{
		send_to_char("E da dove vuoi evadere? Questa non mi sembra una prigione!\r\n", ch);
		return;
		
	}
	else {
		{
			if ((cella = world[ch->in_room].number) == 6813) {
				fuga = 6812;
				regno = 3; //Sendaria
			}else
			if ((cella = world[ch->in_room].number) == 8578){
				fuga = 8579;
				regno = 1; //Tolnedra
			}else
			if ((cella = world[ch->in_room].number) == 9836){
				fuga = 9835;
				regno = 1; //Tolnedra
				
			}else
			if ((cella = world[ch->in_room].number) == 12542){
				fuga = 12680;
				regno = 2; //Deira
				
			}else
			if ((cella = world[ch->in_room].number) == 21261){
				fuga = 21260;
				regno = 0; //Elenia
			} else 
			if ((cella = world[ch->in_room].number) == 50137){
				fuga = 50135;
				regno = 5; //Alorn
			} 
			else
				fuga = 0;
		}
		
	}
	
	if(!(get_obj_in_list_vis_vnum(ch,VNUM_KIT_EVASIONE,ch->carrying))){
	    send_to_char("Non puoi evadere se non hai gli strumenti necessari.\r\n",ch);
	    return;
	}
	
	if (fuga != 0)
	{
		
		coef_evasione = (((GET_SKILL(ch, SKILL_PICK_LOCK))*0.1)+GET_DEX(ch)+GET_INT(ch));
		fortuna = number(1,70);
		riuscita = number(110,700);
		
		if ((tentativo=coef_evasione+fortuna) >= riuscita)
		{
			if ((GET_SKILL(ch, SKILL_PICK_LOCK)) > 99)
			{
				send_to_char("&2&bRiesci con molta maestria ad aprire la porta e ad uscire dalla cella. Evadi!&0\r\n", ch);
				send_to_char("&1&bClick!! la porta si richiude alle tue spalle.&0\r\n", ch);
				act("Con molta maestria $n evade!",TRUE,ch,0,0,TO_NOTVICT);
			}
			else
			if (GET_DEX(ch) > GET_INT(ch))
			{
				send_to_char("&2&bSfruttando una attimo di disattenzione del carceriere riesci a sgattaiolare dalla cella...EVADI!&0\r\n", ch);
				send_to_char("&1&bClick!! la porta si richiude alle tue spalle.&0\r\n", ch);
				act("Con molta maestria $n evade!",TRUE,ch,0,0,TO_NOTVICT);
			}
			else
			if (GET_DEX(ch) < GET_INT(ch))
			{
				send_to_char("&2&bStudi attentamente la situazione e cosi' facendo ti accorgi che il guardiano non ha chiuso bene la cella. Evadi!!!&0\r\n", ch);
				send_to_char("&1&bClick!! la porta si richiude alle tue spalle.&0\r\n", ch);
				act("Con molta maestria $n evade!",TRUE,ch,0,0,TO_NOTVICT);
			}
			else
			{
				send_to_char("&2&bAttiri la sentinella con uno dei tuoi discorsi...&0\r\n", ch);
				send_to_char("&2&b...ad un tratto, visto l'argomento da te trattato, il guardiano si addormenta...&0\r\n", ch);
				send_to_char("&2&b...sfili le chiavi dalla cinta del tuo carceriere e riesci ad evadere!!!&0\r\n", ch);
				send_to_char("&1&bClick!! la porta si richiude alle tue spalle.&0\r\n", ch);
				act("Con molta maestria $n evade!",TRUE,ch,0,0,TO_NOTVICT);
			}
			char_from_room(ch);
			char_to_room(ch, real_room(fuga));
			look_at_room(ch, 0);
			set_crime(ch,0,"vivo","Evasione",regno,CLAN_MULTA, 1000);
			return;
		}
		else
		{
			send_to_char("&1&bIl tuo tentativo di evasione non riesce!&0\r\n", ch);
		}
	}
	
	else
	{
		send_to_char("&1&bDopo un attento studio della stanza deduci che non vi e' nessuna possibilita' di evasione!&0\r\n", ch);
	}
	
}
