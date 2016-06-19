/* ************************************************************************
*   File: act.offensive.c                               Part of CircleMUD *
*  Usage: player-level commands of an offensive nature                    *
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
#include "wilderness.h"

/* extern variables */
extern int newbye(struct char_data *ch);
extern struct room_data *world;
extern struct index_data *mob_index; //Orione
extern struct descriptor_data * descriptor_list;
extern struct char_data * character_list;
extern int pk_allowed;
extern const char *dirs[];
extern int rev_dir[];
extern const struct str_app_type str_app[];
extern struct spell_info_type spell_info[];
/* extern functions */
void raw_kill(struct char_data * ch, struct char_data * killer);
void check_player_killer(struct char_data * ch, struct char_data * vict);
void check_mob_killer(struct char_data * ch, struct char_data * vict);
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);
int perform_move(struct char_data *ch, int dir, int specials_check, int show_room);
bool can_do_event(struct char_data *ch, long int comdtype, long pulse);

void critical_hit(struct char_data *ch, struct char_data *victim, int location);
/* Daniel Houghton's revised external functions */
void strike_missile(struct char_data *ch, struct char_data *tch,
	struct obj_data *missile, int dir, int attacktype);
void miss_missile(struct char_data *ch, struct char_data *tch,
	struct obj_data *missile, int dir, int attacktype);
void mob_reaction(struct char_data *ch, struct char_data *vict, int dir);
void fire_missile(struct char_data *ch, char arg1[MAX_INPUT_LENGTH],
	struct obj_data *missile, int pos, int range, int dir);
extern int arena_ok(struct char_data *ch, struct char_data *victim);
int check_abil_weapon(struct obj_data *weapon);
/*Adriano*/
#define NOBREATH_DURATION 20
#define BONUS_BASH_MOUNT 10


ACMD(do_assist)
{
	struct char_data *helpee, *opponent;
        struct char_data *k = NULL;    //Orione
	struct follow_type *f;	       //Orione
	int target_questmob_group = 0; //Orione
	    
    	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato ,come credi di riuscire ad aiutare qualcuno??\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch)){
		send_to_char("Devi prima scendere\n",ch);
		return;
	}
	if (FIGHTING(ch)) {
		send_to_char("Stai gia' combattendo! Come puoi aiutare qualcun altro?\r\n", ch);
		return;
	}
	one_argument(argument, arg);
	
	if (!*arg)
		send_to_char("Chi desideri aiutare?\r\n", ch);
	else if (!(helpee = get_char_room_vis(ch, arg)))
		send_to_char(NOPERSON, ch);
	else if (helpee == ch)
		send_to_char("Non puoi aiutarti piu' di cosi'!!\r\n", ch);
	else
	{
		for (opponent = world[ch->in_room].people;
			opponent && (FIGHTING(opponent) != helpee);
			opponent = opponent->next_in_room)
			;
		
		if (!opponent)
			act("Ma nessuno lo sta combattendo!", FALSE, ch, 0, helpee, TO_CHAR);
		else if (!CAN_SEE(ch, opponent))
			act("Non riesci a vedere chi lo sta combattendo!", FALSE, ch, 0, helpee, TO_CHAR);
    /*    else if (!pk_allowed && !IS_NPC(opponent) && !arena_ok(ch, opponent)) // prevent accidental pkilling
	  act("Usa 'murder' se vuoi veramente attaccare $N.", FALSE, ch, 0, opponent, TO_CHAR); */
		else if (newbye(ch)&&!IS_NPC(opponent)) {
			send_to_char("Non puoi ancora farlo!\r\n",ch);
			return;
		} else if (newbye(opponent)) {
			send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
			return;
		/* Orione Inizio */
		} else if (IS_NPC(opponent) && MOB_FLAGGED(opponent, MOB_QUEST)) {
		    if (IS_AFFECTED(ch, AFF_GROUP)) {
			if (ch->master) k = ch->master;
			else k = ch;

			if (helpee==k)
			    target_questmob_group = TRUE;
	    		else {
			    for (f = k->followers; f; f = f->next) {
				if (helpee==f->follower) {
				    target_questmob_group = TRUE;
				    break;
			        }
			    }
			}

			if (target_questmob_group) {
			    send_to_char("Ti unisci al combattimento!\r\n", ch);
			    act("$N si schiera dalla tua parte!", 0, helpee, 0, ch, TO_CHAR);
			    act("$n si schiera dalla parte di $N.", FALSE, ch, 0, helpee, TO_NOTVICT);
			    hit(ch, opponent, TYPE_UNDEFINED);
			} else {
			    send_to_char("La questione non sembra riguardarti!\r\n", ch);
			    return;
			}
		    } else {
		        send_to_char("La questione non sembra riguardarti!\r\n", ch);
		        return;
		    }
		/* Orione Fine */
		} else if (controllo_scontro (ch, opponent)) {
			return;
		} else {
			send_to_char("Ti unisci al combattimento!\r\n", ch);
			act("$N si schiera dalla tua parte!", 0, helpee, 0, ch, TO_CHAR);
			act("$n si schiera dalla parte di $N.", FALSE, ch, 0, helpee, TO_NOTVICT);
			hit(ch, opponent, TYPE_UNDEFINED);
		}
	}
}


ACMD(do_hit)
{
	struct char_data *vict;
	
	one_argument(argument, arg);
/*	
	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)) {
	  send_to_char("&8Gli Dei ti osservano e sorridono al tuo patetico tentativo.&0\r\n", ch);
	  return;
	}
*/	

	if(AFF_FLAGGED(ch, AFF_IMMOBIL)) {
		send_to_char("Sei legato ,Non riusciresti MAI a colpire qualcuno!!\r\n", ch);
		return;
	}
	
	if(TRANSPORTED_BY(ch)) {
		send_to_char("Devi prima scendere!!\r\n",ch);
		return;
	}
	
	if (!*arg)
		send_to_char("Colpire chi?\r\n", ch);
	else if (!(vict = get_char_room_vis(ch, arg)))
		send_to_char("Non sembra essere qui.\r\n", ch);
	else if (vict == ch) {
		send_to_char("Ti colpisci..OUCH!.\r\n", ch);
		act("$n colpisce se stesso, e si fa pure male!!", FALSE, ch, 0, vict, TO_ROOM);
	}
	else if (PLR_FLAGGED(vict, PLR_FANTASMA)) {
		send_to_char("E' un fantasma ,non riusciresti a colpirlo.\r\n",ch);
		return;
		}
	else if (!PRF_FLAGGED(vict, PRF_RPG) && (!IS_NPC(vict))) {
		send_to_char("Non puoi uccidere una persona che non sta giocando al tuo stesso gioco.\r\n",ch);
		return;
		}
	else if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == vict))
		act("$N e' un buon amico, non puoi colpire $M.", FALSE, ch, 0, vict, TO_CHAR);
	else if (MOB_FLAGGED(vict, MOB_SELVAGGINA) && (!MOB_FLAGGED(vict, MOB_HARD_TAME)))
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
	else if(TRANSPORTED_BY(vict))
		send_to_char("Non e' molto carino colpire qualcuno che non puo difendersi!\r\n",ch);
	else if (controllo_scontro (ch, vict))
		return;
	else {
		if (   !IS_NPC(ch)
		    && !IS_NPC(vict)
		    && !pk_allowed 
		    && !arena_ok(ch, vict) ) {

			if (   IS_AFFECTED(ch, AFF_CHARM) 
			    && (ch->master) 
			    && !IS_NPC(ch->master) 
			    && !IS_NPC(vict) )
				return;			
				// you can't order a charmed pet to attack a player 
	  }
		
		if (newbye(ch) && !IS_NPC(vict)) {
			send_to_char("Non puoi ancora farlo!\r\n",ch);
			return;
		} 
		else if (newbye(vict)) {
			send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
			return;
		}
		
		if ((GET_POS(ch) == POS_STANDING) && (vict != FIGHTING(ch))) {
			//PEPPE COMB
			if (!IS_NPC(ch))
			  GET_DURATA_COMB(ch) = 0;	    
			hit(ch, vict, TYPE_UNDEFINED);
			WAIT_STATE(ch, PULSE_VIOLENCE + 2);
		} 
		else
			send_to_char("Stai gia' dando il meglio di stesso!\r\n", ch);
	}
}



ACMD(do_kill)
{
	struct char_data *vict;
	
	if ((GET_LEVEL(ch) < LVL_GOD) || IS_NPC(ch)) {
		do_hit(ch, argument, cmd, subcmd);
		return;
	}
	
	//Phantom Commento : Qui passa solo lo staff 
	one_argument(argument, arg);
	
	if (!*arg) {
		send_to_char("Chi vuoi uccidere?\r\n", ch);
	}
	else {
		if (!(vict = get_char_room_vis(ch, arg)))
			send_to_char("Non e' qui!\r\n", ch);
		else if (!IS_NPC(vict) && GET_LEVEL(vict) > LVL_GOD)
		  send_to_char("Pace et Armonia nello Staff :D \r\n", ch);
		else if (ch == vict)
			send_to_char("La vita e' una cosa meravigliosa!!! Parola di Fantasma ;-) !!!\r\n", ch);
		else {
			act("Dissolvi $N in un lampo di luce.", FALSE, ch, 0, vict, TO_CHAR);
			act("$N pone fine ad ogni tua sofferenza.", FALSE, vict, 0, ch, TO_CHAR);
			act("$n trasforma $N in pura energia!", FALSE, ch, 0, vict, TO_NOTVICT);
			raw_kill(vict, ch);
		}
	}
	
}

ACMD(do_backstab)
{
	int num_stab = 0;
	struct char_data *vict;
	
	one_argument(argument, buf);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!",ch);
		return;
	}
	if(TRANSPORTED_BY(ch)||RIDING(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, buf))) {
		send_to_char("Chi vuoi colpire alle spalle?\r\n", ch);
		return;
	}
	
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	
	if(PLR_FLAGGED(vict, PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch) < LVL_AVATAR ){
		send_to_char("Ma ti sembra logico cercare di colpire un fantasma alle spalle?!?\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict, PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT ){
		send_to_char("Non puoi farlo,non sta giocando al tuo stesso gioco.\r\n",ch);
		return;
	}
	if (vict == ch) {
		send_to_char("Come puoi farlo su te stesso?\r\n", ch);
		return;
	}
	
	if (RIDING(ch))
	{
		send_to_char("Non puoi farlo se cavalchi.\r\n", ch);
		return;
	}
	
	if (RIDING(vict))
	{
		send_to_char("Non puoi, sta montando una cavalcatura.\r\n", ch);
		return;
	}
	
	if (!GET_EQ(ch, WEAR_WIELD)) {
		send_to_char("Devi avere un'arma impugnata per poterlo fare.\r\n", ch);
		return;
	}
  /* if (GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 3) != TYPE_PIERCE - TYPE_HIT)*/
	if(check_abil_weapon(GET_EQ(ch, WEAR_WIELD)) != ABIL_PUGNALE) {
		send_to_char("Solo con un'arma a punta puoi riuscirci.\r\n", ch);
		return;
	}
	
	if (MOB_FLAGGED(vict, MOB_SELVAGGINA) && (!MOB_FLAGGED(vict, MOB_HARD_TAME))) {
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	
	if (GET_ABIL(ch,ABIL_PUGNALE)!=MASTER_LIV && (subcmd == SCMD_DOPPIO_STAB)) {
		send_to_char("Solo se sei maestro di pugnale puoi usare il doppio backstab!\r\n", ch);
		return;
	}
	
	if(!GET_EQ(ch,WEAR_WIELD_L) && (subcmd == SCMD_DOPPIO_STAB)) {
		send_to_char("Devi avere due pugnali per effettuare il doppio backstab!", ch);
		return;
	}
	
	
	if (!can_do_event(vict, LAST_ALLERT, 60)) {
		send_to_char("&5&bLa vittima e' stato di allerta! Non riusciresti ad attuare il backstab!&0.\r\n", ch); 
		return;
	}		 
	
	if (FIGHTING(vict)) {
		send_to_char("Non puoi farlo su una persona che sta combattendo-- e' troppo allertato!\r\n", ch);
		return;
	}
	

	
/*
  if (!IS_NPC(ch)&& !IS_NPC(vict)&&!pk_allowed && !arena_ok(ch, vict)) {
  send_to_char("You and your victim must both be in the arena!\r\n", ch);
  return;
      }
*/
	if (!CAN_LEARN_SKILL(ch,SKILL_BACKSTAB)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (MOB_FLAGGED(vict, MOB_AWARE)) {
		act("Ti accorgi che  $N sta affondando un'arma su di te!", FALSE, vict, 0, ch, TO_CHAR);
		act("$n ti nota affondare l'arma su di lui!", FALSE, vict, 0, ch, TO_VICT);
		act("$n si accorge che $N sta affondando l'arma su di lui!", FALSE, vict, 0, ch, TO_NOTVICT);
    /*    check_player_killer(ch,vict); */
		hit(vict, ch, TYPE_UNDEFINED);
		return;
	}
	
	if (controllo_scontro (ch, vict))
		return;
	
	if (subcmd == SCMD_DOPPIO_STAB){
		num_stab = 2;
		if (AWAKE(vict) && !test_and_improve(ch, SKILL_BACKSTAB, 100, 15, 0))
			num_stab--;
		if (AWAKE(vict) && !test_and_improve(ch, SKILL_BACKSTAB, 100, 0, 0))
			num_stab--;
	}
	else
		if (!AWAKE(vict) || test_and_improve(ch, SKILL_BACKSTAB, 100, 15, 0))
			num_stab = 1;
	//PEPPE COMB
	if (!IS_NPC(ch))
		GET_DURATA_COMB(ch)=0;
		
	if (GET_ABIL(ch,ABIL_PUGNALE)==MASTER_LIV && (subcmd == SCMD_DOPPIO_STAB)) { // Doppio stab
		if (num_stab)
			hit(ch, vict, SECOND_BACKSTAB);
		else
			damage_nuovo(ch, vict, 0, SKILL_BACKSTAB, TRUE, DANNO_FISICO);

		if (!vict || GET_POS(vict) == POS_DEAD)
			return;
	
		if(GET_EQ(ch,WEAR_WIELD_L) && num_stab == 2)
			hit(ch,vict,SECOND_BACKSTAB);
		else if (GET_EQ(ch, WEAR_WIELD_L) && num_stab < 2)
			damage_nuovo(ch, vict, 0, SKILL_BACKSTAB, TRUE, DANNO_FISICO);
	}
	else{
		if (num_stab)
			hit(ch, vict, SKILL_BACKSTAB);
		else
			damage_nuovo(ch, vict, 0, SKILL_BACKSTAB, TRUE, DANNO_FISICO);
	}
}



ACMD(do_order)
{
	char name[100], message[256];
	char buf[256];
	bool found = FALSE;
	int org_room;
	struct char_data *vict;
	struct follow_type *k;
	
	half_chop(argument, name, message);
	
	if (!*name || !*message)
		send_to_char("Ordinare a CHI di fare COSA?\r\n", ch);
	else if (!(vict = get_char_room_vis(ch, name)) && !is_abbrev(name, "followers"))
		send_to_char("Mi spiace, non e' qui.\r\n", ch);
	else if (ch == vict)
		send_to_char("Problemi? potrei consigliarti un ottimo psichiatra...\r\n", ch);
	
	else {
		if (IS_AFFECTED(ch, AFF_CHARM)) {
			send_to_char("Il tuo superiore non approva che tu gli dia dei comandi.\r\n", ch);
			return;
		}
		
		if (vict) {
			sprintf(buf, "$N ti ordina di '%s'", message);
			act(buf, FALSE, vict, 0, ch, TO_CHAR);
			act("$n ordina qualcosa a $N.", FALSE, ch, 0, vict, TO_ROOM);
			
			if ((vict->master != ch) || !IS_AFFECTED(vict, AFF_CHARM))
				act("$n non ti ascolta.", FALSE, vict, 0, 0, TO_ROOM);
			else {//Per impedire di ordinare ai mob charmati di usare i comandi mobtrigger
				one_argument(message, buf);
						if (   is_abbrev("masound",buf)
						    || is_abbrev("mkill",buf)
						    || is_abbrev("mjunk",buf)
						    || is_abbrev("mechoaround",buf)
						    || is_abbrev("msend",buf)
						    || is_abbrev("mecho",buf)
						    || is_abbrev("mload",buf)
						    || is_abbrev("mpurge",buf)
						    || is_abbrev("mgoto",buf)
						    || is_abbrev("mat",buf)
						    || is_abbrev("mteleport",buf)
						    || is_abbrev("mforce",buf)
						    || is_abbrev("mexp",buf)
						    || is_abbrev("mgold",buf)
						    || is_abbrev("qsay",buf) 
						   ) {
							send_to_char("HUH!.\r\n",ch);
							return;
						}
				
				/* if (CHECK_WAIT(vict)) { // evitiamo i kick infiniti =)
					send_to_char("Purtroppo e' impegnato.",ch);
					return;
				} */

				// By Spini: temporaneo, finché non capisco perché CHECK_WAIT
				// è stato cambiato da > 0 a > 1. Caso mai cancellare questo
				// e fare un'altra macro apposita

				if (vict->desc && vict->desc->wait > 0){
					send_to_char ("Purtroppo e' impegnato.\r\n", ch);
					return;
				}
				
				if (GET_MOVE(ch) <= 10) {
					send_to_char("Sei troppo stanco per dare ordini.", ch);
					return;
				}
						
				GET_MOVE(ch) -= 5;
				
				send_to_char(OK, ch);
				command_interpreter(vict, message);
			}
		} 
		else {			/* This is order "followers" */
			//sprintf(buf, "$n da' l'ordine '%s'.", message);
			//act(buf, FALSE, ch, 0, vict, TO_ROOM);
			
			org_room = ch->in_room;
			
			for (k = ch->followers; k; k = k->next) {
				if (org_room == k->follower->in_room)
					if (IS_AFFECTED(k->follower, AFF_CHARM)) {
						found = TRUE;
						one_argument(message, buf);
						//Per impedire di ordinare ai mob charmati di usare i comandi mobtrigger
						if (   is_abbrev("masound",buf)
						    || is_abbrev("mkill",buf)
						    || is_abbrev("mjunk",buf)
						    || is_abbrev("mechoaround",buf)
						    || is_abbrev("msend",buf)
						    || is_abbrev("mecho",buf)
						    || is_abbrev("mload",buf)
						    || is_abbrev("mpurge",buf)
						    || is_abbrev("mgoto",buf)
						    || is_abbrev("mat",buf)
						    || is_abbrev("mteleport",buf)
						    || is_abbrev("mforce",buf)
						    || is_abbrev("mexp",buf)
						    || is_abbrev("mgold",buf)
						    || is_abbrev("qsay",buf) 
						   ) {
							send_to_char("HUH!.\r\n",ch);
							return;
						}
						
						if (GET_MOVE(ch) <= 10) {
						  send_to_char("Sei troppo stanco per dare ordini.", ch);
						  return;
						}
						
						GET_MOVE(ch) -= 6;
						command_interpreter(k->follower, message);
					}
			}
			if (found)
				send_to_char(OK, ch);
			else
				send_to_char("Nessuno qui ha intenzione di eseguire i tuoi ordini!\r\n", ch);
		}
	}
}



ACMD(do_flee)
{
	extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
	int i, attempt, loss;
	
	if (ch->in_room==NOWHERE) return;

	if (AFFECTED (ch, AFF_LEVITATE, BITV_LEVITATE)){
		send_to_char ("Non puoi scappare mentre lieviti!\r\n", ch);
		return;
	}
	
	if(AFF_FLAGGED(ch,AFF_PARALIZE)){
		send_to_char("Sei &1&bParalizzato&0 non riesci a scappare!",ch);
		return;
	}
	if ((IS_NPC(ch) && GET_MOB_WAIT(ch)) || (!IS_NPC(ch) && CHECK_WAIT(ch)))
	{
		send_to_char("PANICO!! Non puoi scappare!\r\n", ch);
		return;
	}
	
	if (GET_POS(ch) < POS_FIGHTING)
	{
		send_to_char("Uhm... non sei messo bene, non riesci a scappare!\r\n", ch);
		return;
	}
	
	if ((time(NULL) - ch->last_flee)<3){
		send_to_char("Ti stai facendo prendere dal panico!\r\n", ch);
		send_to_char("Non riesci a scappare!\r\n", ch);
		ch->last_flee=time(NULL);
		return;
	}
	
	if (number(0,30)>GET_DEX(ch)){
		send_to_char("Non riesci a scappare!\r\n", ch);
		return;
	}
	
	ch->last_flee=time(NULL);
	
	for (i = 0; i < 6; i++){
		attempt = number(0, NUM_OF_DIRS - 1);	/* Select a random direction */
		if ((IS_IN_WILD(ch) && wild_target_room(ch, attempt)!=-1) || (CAN_GO(ch, attempt) && !IS_SET(ROOM_FLAGS(EXIT(ch, attempt)->to_room), ROOM_DEATH)))
		{
			act("$n va in panico e tenta di fuggire!", TRUE, ch, 0, 0, TO_ROOM);
			if (FIGHTING(ch)){
				if (!IS_NPC(ch) && (IN_ROOM(ch)!= NOWHERE) && !ROOM_FLAGGED(IN_ROOM(ch), ROOM_ARENA)){
	  // VECCHIA VERSIONE DELLA PERDITA XP
	  // loss = GET_MAX_HIT(FIGHTING(ch)) - GET_HIT(FIGHTING(ch));
	  // loss *= GET_LEVEL(FIGHTING(ch));
					loss = MAX(0, titles[(int)GET_CLASS(ch)][(int) MIN(GET_LEVEL(ch)+1, LVL_IMPL)].exp -titles[(int)GET_CLASS(ch)][(int) GET_LEVEL(ch)].exp);
					loss = 0.01 * loss; // Un percento, da tarare
					//se entrambi NON sono pg allora non da hole alla fuga
					if(IS_NPC(FIGHTING(ch)) || IS_NPC(ch))
						gain_exp(ch, -loss);
					GET_SENZA_DORMIRE(ch) += 2;
				}
				if (FIGHTING(FIGHTING(ch)) == ch)
					stop_fighting(FIGHTING(ch));
				if(PLR_FLAGGED(FIGHTING(ch),PLR_CONCENTRATO) && (FIGHTING(FIGHTING(ch)) == ch)){
						REMOVE_BIT(PLR_FLAGS(FIGHTING(ch)),PLR_CONCENTRATO);
					}
				stop_fighting(ch);
			}
			if (do_simple_move(ch, attempt, TRUE, CAN_SHOW_ROOM))
				send_to_char("!!SOUND(flee.wav)Scappi a gambe levate.\r\n", ch);
			else act("$n tenta di fuggire, ma non puo'!", TRUE, ch, 0, 0, TO_ROOM);
			return;
		}
	}
	send_to_char("PANICO!  Non puoi scappare!\r\n", ch);
}

ACMD(do_punch)
{
	struct char_data *vict;
	int modificatore =0;
	int kodam=0,location=0,prob=0;
	char arg1[MAX_INPUT_LENGTH];
	
	two_arguments(argument,arg,arg1);
	
	if(is_abbrev(arg1,"braccio_destro"))
		location=LOCATION_BRACCIO_D;
	else if(is_abbrev(arg1,"braccio_sinistro"))
		location=LOCATION_BRACCIO_S;	
	else if(is_abbrev(arg1,"addome"))
		location=LOCATION_TORSO;
	else if(is_abbrev(arg1,"testa"))
		location=LOCATION_TESTA;	
	else if(!*arg1)

		location=LOCATION_RANDOM;
	if(!*arg1){
	    if(is_abbrev(arg,"braccio_destro"))
		location=LOCATION_BRACCIO_D;
	    else if(is_abbrev(arg,"braccio_sinistro"))
		location=LOCATION_BRACCIO_S;	
	    else if(is_abbrev(arg,"addome"))
		location=LOCATION_TORSO;
	    else if(is_abbrev(arg,"testa"))
	    	location=LOCATION_TESTA;	
		
	    else if(!*arg)

			location=LOCATION_RANDOM;
	}
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, arg))) {
		if (FIGHTING(ch)) {
			vict = FIGHTING(ch);
		}
		else {
			send_to_char("Chi vuoi colpire?\r\n", ch);
			return;
		}
	}
	
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR ){
		send_to_char("Dare un pugno ad un fantasma....ma scherzi?!?!?\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT ){
		send_to_char("Non sembra che stia giocando al tuo stesso gioco.\r\n",ch);
		return;
	}
	if (vict == ch) {
		send_to_char("Ma come siamo spiritosi oggi...\r\n", ch);
		return;
	}
	
	if (!CAN_LEARN_SKILL(ch,SKILL_PUNCH)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (MOB_FLAGGED(vict, MOB_SELVAGGINA)&& (!MOB_FLAGGED(vict, MOB_HARD_TAME))){
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}

	if (controllo_scontro (ch, vict))
		return;
	
	//PEPPE COMB
	//se la posizione e' ancora standing ,quindi non sta gia combattendo
	//metti a zero il tempo di durata del comb,succede solo quando si entra
	//in combattimento con l'abilita'
	if(!IS_NPC(ch))
	    if(GET_POS(ch)==POS_STANDING)
		GET_DURATA_COMB(ch)=0;
	if (!test_and_improve(ch, SKILL_PUNCH, 100, 15, modificatore)){
		damage_nuovo(ch, vict, 0, SKILL_PUNCH, TRUE, DANNO_FISICO);
	}
	else{
		prob=number(1,20);
		if(prob >= 18){//KO!
			act("&1&bMandi $N KO con il tuo micidiale Gancio Destro!&0",FALSE,ch,0,vict,TO_CHAR);
			act("&1&b$n manda KO $N con un  micidiale Gancio Destro!&0",FALSE,ch,0,vict,TO_NOTVICT);
			act("&1&b$n ti colpisce con un  micidiale Gancio Destro che ti manda KO!&0",FALSE,ch,0,vict,TO_VICT);
			kodam=10;
			GET_POS(vict)=POS_SITTING;
			if(!CHECK_WAIT(vict))
			WAIT_STATE(vict,PULSE_VIOLENCE*4);
		}
		
		damage(ch,vict, number(1,str_app[GET_STR(ch)].todam+5)+kodam, SKILL_PUNCH);
	        if(prob>=16 && prob<18)
		    critical_hit(ch,vict,location);
	}
	WAIT_STATE(ch,PULSE_VIOLENCE);
}

ACMD(do_bash)
{
	struct char_data *vict;
	int modificatore =0, ritardo;
	
	one_argument(argument, arg);
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch)||RIDING(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, arg))) {
		if (FIGHTING(ch)) {
			vict = FIGHTING(ch);
		}
		else {
			send_to_char("Chi vuoi caricare?\r\n", ch);
			return;
		}
	}
	
	if (!CAN_LEARN_SKILL(ch,SKILL_BASH)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Non mi sembra logico prendere a spallate un fantasma.\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non sembra abbia voglia di giocare con te.\r\n",ch);
		return;
	}
	if (vict == ch) {
		send_to_char("Ma come siamo spiritosi oggi...\r\n", ch);
		return;
	}
  /* if (!GET_EQ(ch, WEAR_WIELD)) {
     send_to_char("Hai bisogno di impugnare un'arma per aver successo con quello.\r\n", ch);
    return;
    }*/
	
  /*
    if (!IS_NPC(ch)&& !IS_NPC(vict)&&!pk_allowed && !arena_ok(ch, vict)) {
    send_to_char("You and your victim must both be in the arena!\r\n", ch);
    return;
    }
  */
	
  /*Adriano*/
	ritardo = 9;
	if (GET_LEVEL(ch) > 10) ritardo = 11;
	if (GET_LEVEL(ch) > 27) ritardo = 12;
	if (GET_LEVEL(ch) > 43) ritardo = 13;
	if (GET_LEVEL(ch) > 52) ritardo = 15;  
	if (GET_LEVEL(ch) > 70) ritardo = 18;
	
	if (!can_do_event(ch,LAST_BASH, ritardo)){
		send_to_char("Non riesci a scaraventarti sulla tua vittima, riprendi la posizione prima...!\r\n", ch);
		return;
	}
	
	if (MOB_FLAGGED(vict, MOB_SELVAGGINA)&& (!MOB_FLAGGED(vict, MOB_HARD_TAME))){
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	
	if (RIDING(vict) && MOB_FLAGGED(RIDING(vict),MOB_HARD_TAME)){
		send_to_char("Non puoi arrivarci!\r\n", ch);
		return;
	}
	//PEPPE COMB
	if(!IS_NPC(ch))
	    if(GET_POS(ch)==POS_STANDING)
			GET_DURATA_COMB(ch)=0;
	if (GET_MOVE(ch)<20) {
		send_to_char("Sei troppo stanco per effetuare il bash!\r\n", ch);
		return;
	}
	
	if(FIGHTING(ch)){
		send_to_char("Non puoi caricare qualcuno mentre sei gia' ingaggiato in combattimento.\r\n",ch);
		return;
	}
		
	if (controllo_scontro (ch, vict))
		return;
	
	if (RIDING(ch) && !RIDING(vict)) modificatore = + BONUS_BASH_MOUNT;
	if (RIDING(vict) && !RIDING(ch)) modificatore = - BONUS_BASH_MOUNT * 3;
	
	if (MOB_FLAGGED(vict, MOB_NOBASH) || !test_and_improve(ch, SKILL_BASH, 100, 15, modificatore)){
		damage(ch, vict, 0, SKILL_BASH);
		if (RIDING(ch)){
			if (GET_PROF(ch, PROF_MONTARE) < number(1, 100)){
				act("$n cade da cavallo!", FALSE, ch, 0, vict, TO_VICT);
				send_to_char("&3Cadi da cavallo&0!\r\n", ch);
				dismount_char(ch);
				WAIT_STATE(ch, PULSE_VIOLENCE * 3);
				GET_POS(ch) = POS_SITTING;
			}
			else {
				WAIT_STATE(ch, PULSE_VIOLENCE);
				send_to_char("&3Riesci a rimanere a cavallo.&0\r\n", ch);
			}
		}
		else {  //se non sei a cavallo
			send_to_char("&3Cadi a terra.&0\r\n", ch);
			WAIT_STATE(ch, PULSE_VIOLENCE*2);
		
			GET_POS(ch) = POS_SITTING;
		}
	}
	else {
		//damage(ch, vict, MAX(1,str_app[GET_STR(ch)].todam), SKILL_BASH);
		damage(ch, vict, GET_LEVEL(ch)*2, SKILL_BASH);
		if (RIDING(vict)) {
			if (GET_PROF(vict, PROF_MONTARE) < number(1, 100)){
				act("�N cade da cavallo!", FALSE, ch, 0, vict, TO_CHAR);
				send_to_char("&3Cadi da cavallo!&0\r\n", vict);
				dismount_char(vict);
				WAIT_STATE(vict, PULSE_VIOLENCE * 3);
				GET_POS(vict) = POS_SITTING;
			}
			else{
				WAIT_STATE(vict, PULSE_VIOLENCE);
				send_to_char("&3Riesci a rimanere a cavallo.&0\r\n", vict);
			}
		}
		else {
			WAIT_STATE(vict, PULSE_VIOLENCE*2);
			send_to_char("&5Vieni scaraventato a terra.&0\r\n", vict);
			GET_POS(vict) = POS_SITTING;
		}
	}
	GET_MOVE(ch) = GET_MOVE(ch) - MAX(2, number(1,20) - (GET_LEVEL(ch)/15));
}


ACMD(do_colpo_mortale)
{
	struct char_data *vict;
	//int modificatore =0, ritardo;
	
	one_argument(argument, arg);
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch)||RIDING(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, arg))) {
		if (FIGHTING(ch)) {
			vict = FIGHTING(ch);
		}
		else {
			send_to_char("Chi vuoi colpire mortalmente?\r\n", ch);
			return;
		}
	}
	
	if (!CAN_LEARN_SKILL(ch,SKILL_COLPO_MORTALE)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(GET_LEVEL(vict)>=71){
		send_to_char("Quando provi a concentrarti vedi solo pecorelle e maialini!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Non mi sembra logico cercare di colpire mortalmente un fantasma.\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non sembra abbia voglia di giocare con te.\r\n",ch);
		return;
	}
	if (vict == ch) {
		send_to_char("Te lo farei fare sul serio... \r\n", ch);
		return;
	}
	if (!GET_EQ(ch, WEAR_WIELD)) {
    	    send_to_char("Hai bisogno di impugnare un'arma per aver successo con quello.\r\n", ch);
	return;
	}	
	
	
	if (MOB_FLAGGED(vict, MOB_SELVAGGINA)&& (!MOB_FLAGGED(vict, MOB_HARD_TAME))){
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	
	if (RIDING(vict) && MOB_FLAGGED(RIDING(vict),MOB_HARD_TAME)){
		send_to_char("Non puoi arrivarci!\r\n", ch);
		return;
	}
	//PEPPE COMB
	if(!IS_NPC(ch))
	    if(GET_POS(ch)==POS_STANDING)
			GET_DURATA_COMB(ch)=0;
	
	if (controllo_scontro (ch, vict))
		return;
	
	if (!test_and_improve(ch, SKILL_COLPO_MORTALE, 100, 15, 0))
	{
		send_to_char("Non riesci a concentrarti abbastanza per fare questo colpo!\r\n", ch);
		WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
		return;
	}	
	
WAIT_STATE(ch,4 * PULSE_VIOLENCE);
	damage(ch, vict, 1, TYPE_HIT);
	act("&2Ti concentri per colpire mortalmente $N&0",FALSE,ch,0,vict,TO_CHAR);
	act("&2Vedi $n concentrarsi profondamente,hai paura di quello che potrebbe succedere.&0",FALSE,ch,0,vict,TO_VICT);
	act("&2$n si concentra profondamente, fissa con sguardo attento $N.&0", FALSE,ch,0,vict,TO_NOTVICT);

	SET_BIT(PLR_FLAGS(ch),PLR_CONCENTRATO);                    
	GET_TCONCENTRATO(ch)= 14; //14 sta per 1 il numero dell'abilita per cui si ci concentra e 4 il miniround di lag vedere perform_concentrato
	return ;
}

ACMD(do_rescue)
{
	struct char_data *vict, *tmp_ch;
	
	one_argument(argument, arg);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei Legato e vorresti addirittura salvare qualcuno???\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, arg))) {
		send_to_char("Chi vorresti salvare??\r\n", ch);
		return;
	}
	if (vict == ch) {
		send_to_char("E se invece scappassi?\r\n", ch);
		return;
	}
	
	if (!CAN_LEARN_SKILL(ch,SKILL_RESCUE)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (FIGHTING(ch) == vict) {
		send_to_char("Come puoi salvare qualcuno che stai tentando di uccidere??\r\n", ch);
		return;
	}
	for (tmp_ch = world[ch->in_room].people; tmp_ch &&
		(FIGHTING(tmp_ch) != vict); tmp_ch = tmp_ch->next_in_room);
	
	if (!tmp_ch) {
		act("Ma non sta comattendo nessuno!", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	
        /* Orione Inizio */
	if (IS_QUESTOR(vict) && IS_NPC(tmp_ch) && MOB_FLAGGED(tmp_ch, MOB_QUEST) && \
	    (GET_QUESTMRR(vict)==GET_MOB_VNUM(tmp_ch))) {
		send_to_char("Non e' possibile salvare qualcuno dal proprio destino!\r\n", ch);
		return;
	}
	/* Orione Fine */

	if (controllo_scontro (ch, tmp_ch))
		return;
			
	if (!test_and_improve(ch, SKILL_RESCUE, 100, 15, 0))
	{
		send_to_char("Non riesci a salvarlo!\r\n", ch);
		return;
	}
	check_player_killer(ch,tmp_ch);
	send_to_char("Banzai!  Alla riscossaaaaaa...\r\n", ch);
	act("Vieni salvato da $N, rimani esterefatto!!", FALSE, vict, 0, ch, TO_CHAR);
	act("$n salva eroicamente $N!", FALSE, ch, 0, vict, TO_NOTVICT);
	
	if (FIGHTING(vict) == tmp_ch)
		stop_fighting(vict);
	if (FIGHTING(tmp_ch))
		stop_fighting(tmp_ch);
	if (FIGHTING(ch))
		stop_fighting(ch);
	
	set_fighting(ch, tmp_ch);
	set_fighting(tmp_ch, ch);
	
	WAIT_STATE(vict, 2 * PULSE_VIOLENCE);
}



ACMD(do_kick)
{
	struct char_data *vict;
	int location=0;
	char arg1[MAX_INPUT_LENGTH];
	
	two_arguments(argument, arg,arg1);
	
	if(is_abbrev(arg1,"gamba_destra"))
		location=LOCATION_GAMBA_D;
	else if(is_abbrev(arg1,"gamba_sinistra"))
		location=LOCATION_GAMBA_S;	
	else if(is_abbrev(arg1,"addome"))
		location=LOCATION_TORSO;
	else if(!*arg1)  

			location=LOCATION_RANDOM;
	if(!*arg1){
	if(is_abbrev(arg,"gamba_destra"))
		location=LOCATION_GAMBA_D;
	else if(is_abbrev(arg,"gamba_sinistra"))
		location=LOCATION_GAMBA_S;	
	else if(is_abbrev(arg,"addome"))
		location=LOCATION_TORSO;
	else if(!*arg)

		location=LOCATION_RANDOM;
	}    	
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, arg))) {
		if (FIGHTING(ch)) {
			vict = FIGHTING(ch);
		} else {
			send_to_char("A chi vorresti tirare un calcio?\r\n", ch);
			return;
		}
	}
	
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Non e' logico dare un calcio ad un fantasma...non trovi?!?\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non pare stia giocando al tuo stesso gioco.\r\n",ch);
		return;
	}		
	if (!CAN_LEARN_SKILL(ch,SKILL_KICK)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (vict == ch) {
		send_to_char("Ma come siamo spiritosi oggi...\r\n", ch);
		return;
	}
	
	if (MOB_FLAGGED(vict, MOB_SELVAGGINA) && (!MOB_FLAGGED(vict, MOB_HARD_TAME))) {
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	
	if (RIDING(vict) && MOB_FLAGGED(RIDING(vict),MOB_HARD_TAME))
	{
		send_to_char("Non puoi arrivarci!\r\n", ch);
		return;
	}

	if (controllo_scontro (ch, vict))
		return;
	
	//PEPPE COMB
	if(!IS_NPC(ch))
	    if(GET_POS(ch)==POS_STANDING)
		GET_DURATA_COMB(ch)=0;
	if (!test_and_improve(ch, SKILL_KICK, 100, 15, ((GET_AC(vict)/10-10)/2)))
		damage(ch, vict, 0, SKILL_KICK);
	else
	{
		damage(ch, vict, MAX(5,MIN(GET_LEVEL(ch),60)), SKILL_KICK);
		if(number(1,20)>=19)
		    critical_hit(ch,vict,location);
		if (RIDING(vict) && GET_PROF(ch, PROF_MONTARE)< (number(1, 160)-(GET_PROF(ch, PROF_MONTARE)/2)))
		{
			act("$n cade da cavallo!", FALSE, ch, 0, vict, TO_CHAR);
			send_to_char("Cadi da cavallo!\r\n", vict);
			dismount_char(vict);
			GET_POS(vict) = POS_SITTING;
			WAIT_STATE(vict, PULSE_VIOLENCE * 2);
		}	
	}
	WAIT_STATE(ch, PULSE_VIOLENCE);
}





ACMD(do_shoot)
{
	struct obj_data *missile;
	char arg2[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	int dir, range;
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!GET_EQ(ch, WEAR_WIELD)) {
		send_to_char("Non stai impugnando un'arma da tiro!\r\n", ch);
		return;
	}
	
	if (!GET_EQ(ch, WEAR_HOLD)) {
		send_to_char("Devi tenere un proiettile!\r\n", ch);
		return;
	}
	
	missile = GET_EQ(ch, WEAR_HOLD);
	
	if ((GET_OBJ_TYPE(GET_EQ(ch, WEAR_WIELD)) == ITEM_SLING) &&
		(GET_OBJ_TYPE(missile) == ITEM_ROCK))
		range = GET_EQ(ch, WEAR_WIELD)->obj_flags.value[0];
	else
	if ((GET_OBJ_TYPE(missile) == ITEM_ARROW) &&
		(GET_OBJ_TYPE(GET_EQ(ch, WEAR_WIELD)) == ITEM_BOW))
		range = GET_EQ(ch, WEAR_WIELD)->obj_flags.value[0];
	else
	if ((GET_OBJ_TYPE(missile) == ITEM_BOLT) &&
		(GET_OBJ_TYPE(GET_EQ(ch, WEAR_WIELD)) == ITEM_CROSSBOW))
		range = GET_EQ(ch, WEAR_WIELD)->obj_flags.value[0];
	
	else {
		send_to_char("Devi impugnare un arma da tiro e tenere il proiettile!\r\n", ch);
		return;
	}
	
	two_arguments(argument, arg1, arg2);
	
	if (!*arg1 || !*arg2) {
		send_to_char("Prova con: shoot <qualcuno> <direzione>\r\n", ch);
		return;
	}
	
	if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch)) {
		send_to_char("E' troppo buio per farlo.\r\n", ch);
		return;
	} else if (IS_AFFECTED(ch, AFF_BLIND)) {
		send_to_char("&4Non vedi che un infinito buio...&0\r\n", ch);
		return;
	}
	
	if ((dir = search_block(arg2, dirs, FALSE)) < 0) {
		send_to_char("In quale direzione?\r\n", ch);
		return;
	}
	
	if (!(IS_IN_WILD(ch) && wild_target_room(ch, dir)!=-1) && (!CAN_GO(ch, dir)))
	{
		send_to_char("Qualcosa blocca la tua linea di tiro!\r\n", ch);
		return;
	}
	
	if (range > 3)
		range = 3;
	if (range < 1)
		range = 1;
	
	fire_missile(ch, arg1, missile, WEAR_HOLD, range, dir);
	
}


ACMD(do_throw)
{
	
/* sample format: throw monkey east
   this would throw a throwable or grenade object wielded
   into the room 1 east of the pc's current room. The chance
   to hit the monkey would be calculated based on the pc's skill.
   if the wielded object is a grenade then it does not 'hit' for
   damage, it is merely dropped into the room. (the timer is set
   with the 'pull pin' command.) */
	
	struct obj_data *missile;
	int dir, range = 1;
	char arg2[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	two_arguments(argument, arg1, arg2);
	
/* only two types of throwing objects:
   ITEM_THROW - knives, stones, etc
   ITEM_GRENADE - calls tick_grenades.c . */
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(GET_EQ(ch, WEAR_WIELD))) {
		send_to_char("Devi impugnare qualcosa prima!\r\n", ch);
		return;
	}
	
	missile = GET_EQ(ch, WEAR_WIELD);
	
	if (!((GET_OBJ_TYPE(missile) == ITEM_THROW) ||
			(GET_OBJ_TYPE(missile) == ITEM_GRENADE))) {
		send_to_char("Devi impugnare un'arma da lancio prima!\r\n", ch);
		return;
	}
	
	if (GET_OBJ_TYPE(missile) == ITEM_GRENADE) {
		if (!*arg1) {
			send_to_char("Prova con: throw <direction>\r\n", ch);
			return;
		}
		if ((dir = search_block(arg1, dirs, FALSE)) < 0) {
			send_to_char("In quale direzione?\r\n", ch);
			return;
		}
	} else {
		
		two_arguments(argument, arg1, arg2);
		
		if (!*arg1 || !*arg2) {
			send_to_char("Prova con: throw <qualcuno> <direzione>\r\n", ch);
			return;
		}
		
/* arg2 must be a direction */
		
		if ((dir = search_block(arg2, dirs, FALSE)) < 0) {
			send_to_char("In quale direzione?\r\n", ch);
			return;
		}
	}
	
/* make sure we can go in the direction throwing. */
	
	if (!(IS_IN_WILD(ch) && wild_target_room(ch, dir)!=-1) && (!CAN_GO(ch, dir)))
	{
		send_to_char("Qualcosa blocca la tua linea di tiro!\r\n", ch);
		return;
	}
	
	fire_missile(ch, arg1, missile, WEAR_WIELD, range, dir);
}


ACMD(do_slay)
{
	struct char_data *vict;
	
	if ((GET_LEVEL(ch) < LVL_IMPL) || IS_NPC(ch)) {
    sprintf(buf, "SLAY usato da %s (Comando LVL_IMPL).", GET_NAME(ch) );
		mudlog(buf, BRF, LVL_IMMORT, TRUE);
		return;
	}
	one_argument(argument, arg);
	
	if (!*arg) {
		send_to_char("Specificare un bersaglio.\r\n", ch);
	} 
	else {
		if (!(vict = get_char_room_vis(ch, arg)))
			send_to_char("Non e' qui.\r\n", ch);
		else if (ch == vict)
			send_to_char("La vita e' una cosa meravigliosa!!!\r\n", ch);
		else {
			act("Lo colpisci con una immensa sfera d'energia!", FALSE, ch, 0, vict, TO_CHAR);
			act("$N ti colpisce con una sfera d'energia disintegrando ogni tuo atomo!", FALSE, vict, 0, ch, TO_CHAR);
			act("$n lancia una gigantesca sfera d'energia contro $N, che scompare tra spirali di fumo!", FALSE, ch, 0, vict, TO_NOTVICT);
			raw_kill(vict, ch);
		}
	}
}

ACMD(do_disarm)
{
	struct obj_data *obj;
	struct char_data *vict;
//  void check_abil_obj(struct char_data *ch,struct obj_data *obj,bool wear);
	one_argument(argument, buf);
	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	
	if (!CAN_LEARN_SKILL(ch,SKILL_DISARM)){
		send_to_char("Non otterresti un buon risultato.\r\n",ch);
		return;
	}
	
	if (!*buf) {
		send_to_char("Chi vorresti disarmare?\r\n", ch);
		return;
	}
	else if (!(vict = get_char_room_vis(ch, buf))) {
		send_to_char(NOPERSON, ch);
		return;
	} else
		
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	} 
	else if (PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Non puoi disarmare un fantasma.\r\n",ch);
		return;
	}
	else if (!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non puoi farlo,non sta giocando al tuo stesso gioco.\r\n",ch);
		return;
	}
	else if (vict == ch) {
		send_to_char("Non c'e' bisogno che ti disarmi, per mettere via l'arma.\r\n", ch);
		return;
	}
	else if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == vict)) {
		send_to_char("Il solo pensiero di disarmare il tuo capo ti fa ribrezzo.\r\n", ch);
		return;
	}
/*	
	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)) {
		send_to_char("Questa stanza ti da' una sensazione di pace... ti passa la voglia.\r\n", ch);
		return;
	}*/

	if (controllo_scontro (ch, vict))
		return;
	
	//PEPPE COMB
	if(!IS_NPC(ch))
	    if(GET_POS(ch)==POS_STANDING)
		GET_DURATA_COMB(ch)=0;

	//PEPPE DISARM
	if (!(obj = GET_EQ(vict, WEAR_WIELD_L)) && !(obj = GET_EQ(vict, WEAR_WIELD)))
		act("$N e' GIA' senza armi!", FALSE, ch, 0, vict, TO_CHAR);
	else if (!test_and_improve(ch, SKILL_DISARM, 100, 15, 0)) {
		act("Non riesci a disarmare $N!", FALSE, ch, 0, vict, TO_CHAR);
		act("$N prova a disarmarti ma eviti il suo tentativo e gli ridi in faccia!",FALSE,vict,0,ch,TO_CHAR);
	}
	else if (dice(2, GET_STR(ch)) + GET_LEVEL(ch) >= dice(2, GET_STR(vict)) + GET_LEVEL(vict))
	{
		if (GET_EQ(vict, WEAR_WIELD_L))
			obj_to_room(unequip_char(vict, WEAR_WIELD_L), vict->in_room);
		else	
		if (GET_EQ(vict, WEAR_WIELD))
			obj_to_room(unequip_char(vict, WEAR_WIELD), vict->in_room);
		act("Sei riuscito a disarmare il tuo nemico!", FALSE, ch, 0, 0, TO_CHAR);
		act("$p vola via dalle mani di $N", FALSE, ch, obj, vict, TO_CHAR);
		act("Sei appena stato disarmato da $N, complimenti!", FALSE, vict, 0, ch, TO_CHAR);
		act("$p vola via dalle tue mani!", FALSE, vict, obj, 0, TO_CHAR);
		act("$n disarma abilmente $N, $p cade a terra.", FALSE, ch, obj,vict, TO_NOTVICT);
	}
	else
	{
		act("Eviti di farti disarmare da $N con un abile mossa e lo mandi a terra!",FALSE,vict,0,ch,TO_CHAR);
		act("$N evita il tuo tentativo ,tu ti sbilanci e cadi per terra!",FALSE,ch,0,vict,TO_CHAR);
		act("$n prova a disarmare $N ma si sbilancia e cade per terra!",FALSE,vict,0,ch,TO_ROOM);
		GET_POS(ch)=POS_SITTING;
	}
	if(IS_NPC(vict))
    /* check_player_killer(ch,vict);Prima il disarmato diventava KILLER!!!*/
		hit(vict , ch, TYPE_UNDEFINED);
	WAIT_STATE(ch, PULSE_VIOLENCE);
}

// FIRE_BREATH:
// dam = dice(GET_LEVEL(ch), 10);

/*Adriano*/
ACMD(do_dragonbreath)
{
	struct char_data *vict;
	struct follow_type *f;
	struct obj_data *obj;
	int i;
	int dam;
	struct char_data * othervict;
	struct char_data * k;
	bool okdam;
	
	one_argument(argument, arg);
	
	if (!RIDING(ch))
	{
		send_to_char("Non credo che tu possa farlo!\r\n", ch);
		return;
	}
	
	if (!MOB_FLAGGED(RIDING(ch),MOB_DRAGONBREATH))
	{
		sprintf(buf,"%s non puo' soffiare nulla.\r\n", GET_NAME(RIDING(ch)));
		send_to_char(buf, ch);
		return;
	}
	
	if (!can_do_event(ch,LAST_BREATH, NOBREATH_DURATION))
	{
		sprintf(buf,"Il drago non puo' soffiare, deve recuperare le forze.\r\n");
		send_to_char(buf, ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, arg)))
	{
		if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch)))
			vict = FIGHTING(ch);
		else
		{
			act("&1$n ordina a $N di soffiare.&0", TRUE, ch, 0, RIDING(ch), TO_ROOM);
			act("&1Senti il battito del cuore del drago farsi sempre piu' veloce.&0", TRUE, RIDING(ch), 0, 0, TO_ROOM);
			act("&1$n crea un forte vento inspirando l'aria nelle sue narici.&0", TRUE, RIDING(ch), 0, 0, TO_ROOM);
			act("!!SOUND(dragon1.wav L=1 P=1)&1Immediatamente il soffio di $N incenerisce la zona circostante.&0", TRUE, ch, 0, RIDING(ch), TO_CHAR);
			for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room)
				if (vict && vict!=RIDING(ch) && vict!=ch){
					act("!!SOUND(dragon1.wav L=1 P=1)&1Immediatamente il soffio di $n incenerisce la zona di fianco a te.&0", TRUE, RIDING(ch), 0, 0, TO_VICT);
					act("&1Vieni scaraventato a terra!&0", FALSE, ch, 0, vict, TO_VICT);
					WAIT_STATE(vict, PULSE_VIOLENCE);
				}
			return;
		}
	}
	
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR ){
		send_to_char("E' un fantasma,non mi sembra logico farlo.\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non sembra stia giocando al tuo stesso gioco.\r\n",ch);
		return;
	}	
	if (vict == ch)
	{
		send_to_char("Non credo che tu lo voglia fare!\r\n", ch);
		return;
	}
/*	
	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL))
	{
		send_to_char("Questa stanza suscita un senso di pace...\r\n", ch);
		return;
	}
*/	
	if (AFF_FLAGGED(RIDING(ch), AFF_PARALIZE))
	{
		send_to_char("Il tuo drago non si puo' muovere, figurati soffiare!\n\r",ch);
		return;
	}
	
	
  // Il drago non e' domato
	if (RIDING(ch) && (MOB_FLAGGED(RIDING(ch),MOB_HARD_TAME)) && (!AFF_FLAGGED(RIDING(ch),AFF_TAMED)))
	{
		act("$N inizia a guardarti con aria alquanto diffidente", FALSE, ch, 0, RIDING(ch), TO_CHAR);
		act("$N inizia a guardare $n con aria alquanto diffidente", FALSE, ch, 0, RIDING(ch), TO_ROOM);
		return;
	}
	
	sprintf(buf,"&1Ordini a %s&1 di incenerire %s&1!.&0", GET_NAME(RIDING(ch)),GET_NAME(vict));
	act(buf, FALSE, ch, 0, vict, TO_CHAR);
	act("!!SOUND(dragon1.wav L=1 P=1)&1UN RUGGITO ENORME! Il soffio incenerisce l'area intorno a $N!&0", FALSE, ch, 0, vict, TO_CHAR);
	sprintf(buf,"!!SOUND(dragon1.wav L=1 P=1)&1$n&1 ordina a %s&1 di incenerire $N&1!.&0", GET_NAME(RIDING(ch)));
	act(buf, FALSE, ch, 0, vict, TO_NOTVICT);
	
	for (i = 0;i < 3;i++)
	{
		obj = create_obj();
		sprintf(buf, "Un mucchietto di cenere ancora fumante.");
		obj->description = str_dup(buf);
		sprintf(buf, "un mucchietto di cenere");
		obj->short_description = str_dup(buf);
		sprintf(buf, "cenere");
		obj->name = str_dup(buf);
		GET_OBJ_TYPE(obj) = ITEM_TRASH;
		GET_OBJ_WEAR(obj) = ITEM_WEAR_TAKE;
		GET_OBJ_EXTRA(obj) = ITEM_NODONATE;
		GET_OBJ_WEIGHT(obj) = 1;
		GET_OBJ_TIMER(obj) = 1;
		GET_OBJ_CSLOTS(obj) = 3;
		GET_OBJ_TSLOTS(obj) = 3;
		obj_to_room(obj,ch->in_room);
	}
	
  /*By Offar*/
	dam = dice(GET_LEVEL(RIDING(ch)), 10);
	sprintf(buf,"Il drago infligge : %d",dam);
	log(buf);
	
	for (othervict = character_list; othervict; othervict = othervict->next)
	{
		if ((othervict!=vict) && (othervict->in_room == ch->in_room) &&
			(othervict!=ch) && (othervict!=RIDING(ch)))
		{
			okdam = TRUE;
			if (AFF_FLAGGED(ch, AFF_GROUP) && (ch->master)) k=ch->master;
			else k = ch;
			for (f = k->followers; f; f = f->next)
				if (AFF_FLAGGED(f->follower, AFF_GROUP) && f->follower->in_room == ch->in_room)
				{
					if ((f->follower == othervict) || (RIDING(f->follower) == othervict))
					{
						okdam = FALSE;
						break;
					}
				}
			if (okdam)
			{
				GET_POS(othervict) = POS_SITTING;
				act("&1Vieni investito anche tu dalle fiamme.&0", FALSE, ch, 0, vict, TO_NOTVICT);
				act("&1La potenza del soffio e tale da scaraventarti a terra.&0", FALSE, ch, 0, vict, TO_NOTVICT);
				damage_nuovo(othervict, othervict, MAX(dam / 2, 1), -1, TRUE, DANNO_FUOCO);
			}
		}
	} //for
	
	act("!!SOUND(dragon1.wav L=1 P=1)&1Riesci appena a sentire il ruggito dell'essere che hai difronte quando&0", FALSE, ch, 0, vict, TO_VICT);
	act("&1un cono di fuoco INCENDIA tutta l'area in cui ti trovi!&0", FALSE, ch, 0, vict, TO_VICT);
	act("&1La potenza del soffio e tale da scaraventarti a terra.&0", FALSE, ch, 0, vict, TO_VICT);
	
	if (!pk_allowed)                             // Cosa succede qui ?????
	{
		pk_allowed = TRUE;                       // Non mi pare molto bello...
		if(IS_NPC(ch))
			check_mob_killer(ch, vict);
		else
			check_player_killer(ch,vict);
		damage(ch, vict, MAX(dam, 1), -1);
		pk_allowed = FALSE;
    /* check_killer(ch, vict);*/
	}
	else
		damage(ch, vict, MAX(dam, 1), -1);
	
	if (vict)
	{
		if (RIDING(vict)) dismount_char(vict);
		GET_POS(vict) = POS_SITTING;
		WAIT_STATE(vict, PULSE_VIOLENCE);
	}
}

ACMD(do_agguato)
{
	int check_gen_abil(struct char_data *ch,int abilita);
	void do_dream(struct char_data *ch,int kotime);
	struct char_data *vict=NULL;
	struct obj_data *obj;
	int percent,i,chance=0;
	
	if(check_gen_abil(ch,ABIL_AGGUATO)<=0){
		send_to_char("&5Non hai la piu pallida idea di come si possa fare un agguato!&0",ch);
		return ;
	}
	
	if(FIGHTING(ch)){
		send_to_char("&5Stai Combattendo,pensa a difenderti piuttosto!&0",ch);
		return;
	}
	
	if(AFF_FLAGGED(ch,AFF_IMMOBIL)){
		send_to_char("&5Sei legato ,non sei in grado di stordire NESSUNO!!!&0\r\n",ch);
		return;
	}
	
	/* Ardanos: niente attacchi in prigione! */
	
	if(ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON)) {
		send_to_char("Come fai ad agguate una persona mentre sei in prigione?!?\r\n", ch);
		return;
	}
	
	one_argument(argument,arg);
	
	if(!*arg|| !(vict = get_char_room_vis(ch,arg)))
	{
		send_to_char("&5A chi vorresti fare un agguato???&0",ch);
		return ;
	}
	
	if(IS_NPC(vict))
	{
		send_to_char("&5Questa bastardata la puoi fare solo ai pg!&0",ch);
		return ;
	}
	
	if (newbye(ch)&&!IS_NPC(vict)) {
		send_to_char("Non puoi ancora farlo!\r\n",ch);
		return;
	} else if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Che mente diabolica..agguatare un fantasma.\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT ){
		send_to_char("Non sembra stia giocando al tuo stesso gioco.\r\n",ch);
		return;
	} 
	if (ch == vict) {
		send_to_char("&5Se lo sapesse tua madre sarebbe cosi' triste.... :(&0\r\n", ch);
		return;
	}
	if(RIDING(vict)){
		send_to_char("&5Come pensi di riuscire a colpirlo se sta cavalcando?&0\r\n",ch);
		return;
	}
	if(RIDING(ch)){
		send_to_char("&5Bhe certo...tutti riescono a prendere di sorpresa qualcuno arrivandogli alle spalle cavalcando.&0\r\n",ch);
		return;
	}
	if (!can_do_event(vict,LAST_AGGUATO, 30)) {
		send_to_char("&5&bLa vittima e' stato di allerta! Non riusciresti ad attuare il tuo piano!&0.\r\n", ch); 
		return;
	}		 
	
	for (i=0;i<NUM_WEARS;i++){
		if(i!=WEAR_HOLD && i!=WEAR_ABOUT && i!=WEAR_LOBSX && i!=WEAR_LOBDX)
			if(GET_EQ(ch,i)){
				send_to_char("&5Sei troppo rumoroso cosi vestito,togliti di dosso tutto quello che hai.&0",ch);
				return;
			}
	}
	if(!AFF_FLAGGED(ch,AFF_SNEAK)){
		send_to_char("&5Sei troppo rumoroso e troppo visibile per tentare un agguato!&0",ch);
		return;
	}

	if (controllo_scontro (ch, vict))
		return;
	
	if((obj=GET_EQ(ch,WEAR_HOLD))){
		if(GET_OBJ_TYPE(obj)!=ITEM_MANGANELLO){
			send_to_char("&5Non stai impugnando l'oggetto adatto.&0",ch);
			return;
		}
		
		if(TRANSPORTED_BY(ch)||RIDING(ch)){
			send_to_char("Devi prima scendere!!\r\n",ch);
			return;
		}
		
		if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == vict)) {
			act("$N e' un buon amico, non puoi colpire $M.", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
/*		
		if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)) {
			send_to_char("&5Questa stanza ti da' una sensazione di pace... ti passa la voglia.&0\r\n", ch);
			return;
		}
*/
    /* Chardhros! :-) */
		if (GET_LEVEL(vict)>=LVL_QUEST_MASTER) {
			act("&1Non puoi far male ad un Dio! Rinunci.&0",FALSE,ch,0,vict,TO_CHAR);
			act("&1Quella carogna di $n avrebbe voluto stenderti! GRIN!&0",FALSE,ch,0,vict,TO_VICT);
			act("$n cerca di manganellare $N, CHE PATETICO!",FALSE,ch,0,vict,TO_NOTVICT);
			return;
		}
		//PEPPE COMB
		if(!IS_NPC(ch))
		    if(GET_POS(ch)==POS_STANDING)
			GET_DURATA_COMB(ch)=0;
    /*ora tenta l'agguato*/
		chance=check_gen_abil(ch,ABIL_AGGUATO);
		if(GET_DEX(ch)>16)
			chance+=5;
		if(GET_DEX(ch)<14)
			chance-=5;
		chance+=5*(GET_LEVEL(ch)-GET_LEVEL(vict))/7;
		if(!CAN_SEE(vict,ch))
			chance+=5;
		if((percent=number(1,100))>chance){/*scazzo!!*/
			act("&2Sbagli mira e colpisci $N ad una spalla!&0",FALSE,ch,0,vict,TO_CHAR);
			act("&2Eviti miracolosamente la violenta manganellata di $n&0",FALSE,ch,0,vict,TO_VICT);
			act("&2$NEvita miracolosamente la violenta manganellata di $n&0",FALSE,ch,0,vict,TO_NOTVICT);
			damage(ch, vict, 1, TYPE_HIT);
			return;
		}
		else{/*AFFONDATO!!!!*/
			act("&2Colpisci alla nuca con violenza inaudita $Ns,che cade a terra privo d sensi&0",FALSE,ch,0,vict,TO_CHAR);
			act("&2BONG!!Una manganellata ti raggiunge alla nuca e ti manda nel mondo dei sogni! &0",FALSE,ch,0,vict,TO_VICT);
			act("&2$N viene colpito violentemente alla nuca e cade a terra svenuto!&0",FALSE,ch,0,vict,TO_NOTVICT);
			alter_hit(vict,100);
			do_dream(vict,5);
			GET_POS(vict)=POS_SLEEPING;
			return;                     
		}
	}
	send_to_char("&5Devi avere un manganello impugnato!&0",ch);
}
