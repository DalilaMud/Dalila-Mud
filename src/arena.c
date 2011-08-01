/**************************
 * 
 * File: Arena.c
 *
 * Writen by:  Kevin Hoogheem aka Goon
 *             Modified by Billy H. Chan (STROM)
 *
 * Implementation of a event driven arena.. were players pay to kill.
 *
 * Using this code without consent by Goon will make your guts spill
 * out or worse.. Maybe I will hire Lauraina Bobbet to come visit you
 *
 */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "screen.h"
#include "spells.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "arena.h"

/*   external vars  */
extern FILE *player_fl;
extern struct room_data *world;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct zone_data *zone_table;
extern int top_of_zone_table;
extern int restrict;
extern int top_of_world;
extern int top_of_mobt;
extern int top_of_objt;
extern int top_of_p_table;
extern room_rnum r_mortal_start_room[];

int in_arena = ARENA_OFF;
int start_time;
int game_length;
int lo_lim;
int hi_lim;
int cost_per_lev;
int time_to_start;
int time_left_in_game;
long arena_pot;
long bet_pot;

struct hall_of_fame_element *fame_list = NULL;

void send_to_arena(char *messg);

ACMD(do_bet)
{
	long newbet;
	struct char_data *bet_on;
	
	two_arguments(argument, arg, buf1);
	
	if (IS_NPC(ch)){
		send_to_char("I mob non possono scommetere sull'arena.\r\n",ch);
		return;
	}
	
	if(!*arg) {
		if(in_arena == ARENA_OFF){
			send_to_char("Mi dispiace nessuna arena e' attiva.\r\n", ch);
			return;
		} else if (in_arena == ARENA_START) {
			send_to_char("Usage: bet <giocatore> <somma>\r\n",ch);	 
			return;
		} else if (in_arena == ARENA_RUNNING) {
			send_to_char("Il combattimento e' gia' iniziato. Rien ne va plus.\r\n", ch);
			return;
		}
	}
	
	if(isdigit(*arg)) { // se si Scrive bet <somma> <giocatore> -- Ardanos
		send_to_char("Usage: bet <giocatore> <somma>\r\n",ch);	 
		return;
	}
	
	if (in_arena == ARENA_OFF) {
		send_to_char("Mi dispiace l'arena non e' attiva. Aspetta l'apertura per scommettere.\r\n",ch);
	} else if (in_arena == ARENA_RUNNING) {
		send_to_char("Il combattimento e' gia' iniziato. Rien ne va plus.\r\n", ch);
	} else if (!(bet_on = get_char_vis(ch, arg)))
		send_to_char(NOPERSON, ch);
	else if (bet_on == ch)
		send_to_char("Non ha molto senso, no ?\r\n", ch);
	else if(!(zone_table[world[bet_on->in_room].zone].number == ARENA_ZONE && 
			ROOM_FLAGGED(IN_ROOM(bet_on), ROOM_ARENA)))
		send_to_char("Mi dispiace quella persona non e' nell'arena.\r\n", ch);
	else {
		if(GET_AMT_BET(ch) > 0){
			send_to_char("Hai gia' scommesso su qualcun'altro.\r\n", ch);
			return;
		}
		GET_BETTED_ON(ch) = GET_IDNUM(bet_on);
		
		newbet =  atoi(buf1);
		
		if(newbet == 0){
			send_to_char("Scommetti un po' di monete, no ?!?\r\n", ch);
			return;
		}
		if (newbet > GET_GOLD(ch)){
			send_to_char ("Non hai abbastanza denaro. Il gioco ti sta rovinando eh ?!?\n\r",ch);
			return;
		}
		if(newbet > MAX_BET){
			send_to_char("Non posso accettare una scommessa cosi' alta.\r\n", ch);
			return;
		}
		
		*buf2 = '\0';
		GET_GOLD(ch) -= newbet; /* substract the gold - important :) */
		arena_pot += (newbet / 2);
		bet_pot += (newbet / 2);
		GET_AMT_BET(ch) = newbet;
		sprintf(buf2, "Scommetti %ld monete su %s.\r\n", newbet, GET_NAME(bet_on));
		send_to_char(buf2, ch);
		*buf = '\0';
		sprintf(buf,"%s ha scommesso %ld monete su %s.", GET_NAME(ch),
			newbet, GET_NAME(bet_on));
		send_to_arena(buf);
	}
}
ACMD(do_arena)
{
	struct affected_type *af, *next;
	
	if (IS_NPC(ch)){
		send_to_char("I mob non possono partecipare all'arena.\r\n",ch);
		return;
	}
	if (in_arena == ARENA_OFF) {
		send_to_char("Nessun combattimento nell'arena e' stato indetto.\r\n", ch);
	} else if (GET_LEVEL(ch) < lo_lim) {
		sprintf(buf, "Mi dispiace ma devi essere almeno di livello %d per partecipare a questa arena.\r\n", 
			lo_lim);
		send_to_char(buf, ch);
	} else if (PLR_FLAGGED(ch, PLR_KILLER)) {
		send_to_char("Mi dispiace, i criminali ricercati non possono partecipare all'arena.\r\n", ch);
	} else if (GET_LEVEL(ch) > hi_lim) {
		send_to_char("Il tuo livello e' troppo alto per partecipare a questa arena.\r\n",ch);
	} else if (GET_GOLD(ch) < (cost_per_lev * GET_LEVEL(ch))) {
		sprintf(buf, "Ti servono %d monete per poter partecipare all'arena.\r\n",
			(cost_per_lev * GET_LEVEL(ch)) );
		send_to_char(buf, ch);      
	} else if (in_arena == ARENA_RUNNING) {
		send_to_char("E' troppo tardi per unirti alla mischia. Aspetta che inizi la prossima arena.\r\n", ch);
	} else if(ROOM_FLAGGED(IN_ROOM(ch), ROOM_ARENA)) {
		send_to_char("Sei gia' nell'arena!\r\n",ch);
	} else {
		act("$n e' stato risucchiato nell'arena.", FALSE, ch, 0, 0, TO_ROOM);
		GET_ARENA_HIT(ch) = GET_HIT(ch);
		GET_ARENA_MOVE(ch) = GET_MOVE(ch);
		GET_ARENA_MANA(ch) = GET_MANA(ch);
		GET_ARENA_ROOM(ch) = world[ch->in_room].number;
		char_from_room(ch);
		char_to_room(ch, real_room(number(ARENA_PREP_START,ARENA_PREP_END)));
		act("$n cade dal cielo.", FALSE, ch, 0, 0, TO_ROOM);
		send_to_char("Sei stato trasportato all'arena.\r\n",ch);
		look_at_room(ch, 0);
		sprintf(buf, "%s si e' unito al bagno di sangue!\r\n", GET_NAME(ch));
		send_to_arena(buf);
		GET_GOLD(ch) -= (cost_per_lev * GET_LEVEL(ch));
		arena_pot += (cost_per_lev * GET_LEVEL(ch));
		sprintf(buf, "Paghi %d monete per unirti all'arena.\r\n", 
			(cost_per_lev * GET_LEVEL(ch)));
		send_to_char(buf, ch);
  /* ok lets give them there free restore and take away all their */
  /* effects so they have to recast them spells onthemselfs       */
		GET_HIT(ch) = GET_MAX_HIT(ch);
		GET_MANA(ch) = GET_MAX_MANA(ch);
		GET_MOVE(ch) = GET_MAX_MOVE(ch);
		for (af = ch->affected; af; af = next){
			next = af->next;
			if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
				affect_remove(ch, af);
		}
	}	 
}


ACMD(do_chaos)
{
	char cost[MAX_INPUT_LENGTH], lolimit[MAX_INPUT_LENGTH];
	char hilimit[MAX_INPUT_LENGTH], start_delay[MAX_INPUT_LENGTH];
	char length[MAX_INPUT_LENGTH];
	struct descriptor_data *d;
	
/*Usage: chaos lo hi start_delay cost/lev length*/
	
	if (in_arena != ARENA_OFF) {
		send_to_char("C'e' gia' un'altra arena attiva.\r\n", ch);
		return;
	}
	half_chop(argument, lolimit, buf);
	lo_lim = atoi(lolimit);
	half_chop(buf, hilimit, buf);
	hi_lim = atoi(hilimit);
	half_chop(buf, start_delay, buf);
	start_time = atoi(start_delay);
	half_chop(buf, cost, buf);
	cost_per_lev = atoi(cost);
	strcpy(length, buf);
	game_length = atoi(length);
	
	if (hi_lim > LVL_IMPL ){
		send_to_char("Livmax non puo' superare il livello dei creatori.\r\n", ch);
		return;
	}
	
	if (lolimit < 0)
		silent_end();
	
	
	if(!*lolimit || !*hilimit || !*start_delay || !*cost || !*length){
		send_to_char("Utilizzo: chaos livmin livmax ritardo costo/liv durata\r\n", ch);
		return;
	}
	
	if (lo_lim > hi_lim){
		send_to_char("Livmin non puo' superare Livmax.\r\n", ch);
		return;
	}
	
	if ((hi_lim < 0) || (cost_per_lev < 0) || (game_length < 0 )) {
		send_to_char("Mi piacciono i numeri positivi, grazie.\r\n", ch);
		return;
	}
	
	if ( start_time <= 0){
		send_to_char("Dagli almeno la possibilita' di entrare nell'arena!\r\n", ch);
		return;
	}
	
	if ((GET_LEVEL(ch) < LVL_IMPL) && (cost_per_lev < MIN_ARENA_COST)){
		sprintf(buf, "I creatori hanno messo un minimo di %d monete per livello come biglietto.\r\n", MIN_ARENA_COST);
		send_to_char(buf, ch);
		return;
	}
	
	in_arena = ARENA_START;
	time_to_start = start_time;
	time_left_in_game =0;
	arena_pot =0; 
	bet_pot = 0; 
	/* Ardanos - Resetto tutti i bet degli altri pg connessi */
	for (d = descriptor_list; d; d = d->next) {
        	if (!d->connected && d->descriptor && d->character) {
			d->character->player_specials->saved.bet_amt = 0;
			d->character->player_specials->saved.betted_on = -1;
		}
	}
	start_arena();
	
}

void start_arena()
{
	
	if(time_to_start == 0){
		show_jack_pot();
		in_arena = ARENA_RUNNING;    /* start the blood shed */
		time_left_in_game = game_length;
		start_game();
	} else {
		if(time_to_start >1){
			sprintf(buf1, "L'arena e' aperta dal livello %d al livello %d.\r\n",
				lo_lim, hi_lim);
			send_to_arena(buf1);
			sprintf(buf1, "%d monete per livello per entrare. %d ore all'inizio.\r\n",
				cost_per_lev, time_to_start);
			send_to_arena(buf1);
			sprintf(buf1, "Digitare ARENA per entrare.\r\n");
			send_to_arena(buf1);
		}else{
			sprintf(buf1, "L'arena e' aperta dal livello %d al livello %d.\r\n",
				lo_lim, hi_lim);
			send_to_arena(buf1);
			sprintf(buf1, "%d monete per livello per entrare. Un'ora all'inizio.\r\n",
				cost_per_lev);
			send_to_arena(buf1);
			sprintf(buf1, "Digitare ARENA per entrare.\r\n");
			send_to_arena(buf1);
		}
		time_to_start--;
	}
}

void show_jack_pot()
{
	sprintf(buf1, "\007\007Pronti a COMBATTERE!!!!!!!!\r\n");
	send_to_arena(buf1);  
	sprintf(buf1, "Il montepremi per questa arena e' di %ld monete.\r\n", arena_pot);
	send_to_arena(buf1);
	sprintf(buf1, "Ci sono %ld monete nel monte scommesse.\r\n", bet_pot);
	send_to_arena(buf1);
	
}

void start_game()
{
	register struct char_data *i;
	struct descriptor_data *d;
	
	for (d = descriptor_list; d; d = d->next) {
		if (!d->connected) {
			i = d->character;
			if (zone_table[world[i->in_room].zone].number == ARENA_ZONE &&
				ROOM_FLAGGED(IN_ROOM(i), ROOM_ARENA) && (i->in_room != NOWHERE)) {
				send_to_char("\r\nI cancelli si aprono e vieni spinto all'interno dell'arena.\r\n", i);
				char_from_room(i);
				char_to_room(i, real_room(number(ARENA_START_ROOM,ARENA_END_ROOM)));
				look_at_room(i, 0);
			}
		}
	}
	do_game();
}

void do_game()
{
	
	if(num_in_arena() == 1){
		in_arena = ARENA_OFF; 
		find_game_winner();
	}else if(time_left_in_game == 0){
		in_arena = ARENA_OFF; 
		do_end_game();
	}else if (num_in_arena() == 0){
		in_arena = ARENA_OFF;
		silent_end();
	} else if ((time_left_in_game % 5) || time_left_in_game <= 4) {
		sprintf(buf, "Bzzzt    Qui' e' l'arena che si inserisce... Con %d ore rimaste da giocare sono ancora in piedi %d gladiatori.\r\n",
			time_left_in_game, num_in_arena());
		send_to_arena(buf);
	} else if(time_left_in_game == 1){
		sprintf(buf, "Bzzzt    Qui' e' l'arena che si inserisce... Quando manca ormai solo un'ora di gioco\r\n");
		sprintf(buf+strlen(buf),"e sono ancora in piedi %d gladiatori.\r\n", num_in_arena());
		send_to_arena(buf);
	}
	time_left_in_game--;
}

void find_game_winner()
{
	register struct char_data *i;
	struct descriptor_data *d;
	struct hall_of_fame_element *fame_node;
	struct affected_type *af, *next;
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected) {
			i = d->character;
			if (zone_table[world[i->in_room].zone].number == ARENA_ZONE &&
				ROOM_FLAGGED(IN_ROOM(i), ROOM_ARENA) && (!IS_NPC(i))
				&& (i->in_room != NOWHERE) && GET_LEVEL(i) < LVL_IMMORT){
				GET_HIT(i) = GET_ARENA_HIT(i);
				GET_MANA(i) = GET_ARENA_MANA(i);
				GET_MANA(i) = GET_ARENA_MOVE(i);
				GET_POS(i) = POS_STANDING;
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TESTA_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TESTA_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TESTA_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_D_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_D_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_D_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_S_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_S_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_S_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TORSO_1);	
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TORSO_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TORSO_3);		
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_D_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_D_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_D_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_S_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_S_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_S_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_FERITO);	
	
				if (i->affected)
					for (af = i->affected; af; af = next){
						next = af->next;
						if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
							affect_remove(i, af);
					}
				char_from_room(i);
#ifdef SAME_ROOM
				char_to_room(i, real_room(GET_ARENA_ROOM(i)));
#else 
				char_to_room(i, r_mortal_start_room[GET_HOMETOWN(i)]);
#endif
				look_at_room(i, 0);
				act("$n cade dal cielo.", FALSE, i, 0, 0, TO_ROOM); 
				sprintf(buf, "Dopo %d ore di battaglia il vincitore e' %s.\r\n",
					game_length - time_left_in_game, GET_NAME(i));
				send_to_arena(buf);
				
				GET_GOLD(i) += arena_pot;
				sprintf(buf, "Ricevi in premio %ld monete per avere vinto l'arena.\r\n",
					(arena_pot/2));
				send_to_char(buf, i);
				sprintf(buf2, "%s riceve in premio %ld monete per avere vinto l'arena.\r\n", 
					GET_NAME(i), (arena_pot/2));
				send_to_arena(buf2);
				log(buf2);     
				
				CREATE(fame_node, struct hall_of_fame_element, 1);
				strncpy(fame_node->name, GET_NAME(i), MAX_NAME_LENGTH);
				fame_node->name[MAX_NAME_LENGTH] = '\0';
				fame_node->date = time(0);
				fame_node->award = (arena_pot/2);
				fame_node->next = fame_list;
				fame_list = fame_node;
				write_fame_list();
				find_bet_winners(i);
			}
		}
}

void silent_end()
{
	in_arena = ARENA_OFF;
	start_time = 0;
	game_length = 0;
	time_to_start = 0;
	time_left_in_game = 0;
	arena_pot = 0;
	bet_pot = 0;
	sprintf(buf, "Sembra che nessuno sia stato cosi' coraggioso da entrare nell'arena.\r\n");
	send_to_arena(buf);
}

void do_end_game()
{
	register struct char_data *i;
	struct descriptor_data *d;
	struct affected_type *af, *next;
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected) {
			i = d->character;
			if (ROOM_FLAGGED(IN_ROOM(i), ROOM_ARENA)
				&& (i->in_room != NOWHERE) && (!IS_NPC(i))){
				
				GET_HIT(i) = GET_ARENA_HIT(i);
				GET_MOVE(i) = GET_ARENA_MOVE(i);
				GET_MANA(i) = GET_ARENA_MANA(i);
				GET_POS(i) = POS_STANDING;
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TESTA_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TESTA_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TESTA_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_D_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_D_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_D_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_S_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_S_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_BRACCIO_S_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TORSO_1);	
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TORSO_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_TORSO_3);		
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_D_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_D_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_D_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_S_1);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_S_2);
				REMOVE_BIT(AFF_FLAGS(i),AFF_CRITICAL_GAMBA_S_3);
				REMOVE_BIT(AFF_FLAGS(i),AFF_FERITO);	

				if (i->affected)
					for (af = i->affected; af; af = next){
						next = af->next;
						if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
							affect_remove(i, af);
					}
				char_from_room(i);
				
#ifdef SAME_ROOM
				char_to_room(i, real_room(GET_ARENA_ROOM(i)));
#else	
				char_to_room(i, r_mortal_start_room[GET_HOMETOWN(i)]);
#endif
				
				look_at_room(i, 0);
				act("$n cade dal cielo.", FALSE, i, 0, 0, TO_ROOM);
			}
		}
	sprintf(buf, "Dopo %d ore di battaglia tutto si conclude con un pareggio.",game_length);
	send_to_arena(buf);
	time_left_in_game = 0;
}

int num_in_arena()
{
	register struct char_data *i;
	struct descriptor_data *d;
	int num = 0;
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected) {
			i = d->character;
			if (zone_table[world[i->in_room].zone].number == ARENA_ZONE && 
				ROOM_FLAGGED(IN_ROOM(i), ROOM_ARENA)
				&& (i->in_room != NOWHERE)) {
				if(GET_LEVEL(i) < LVL_IMMORT)
					num++;
			}
		}
	return num;
}

ACMD(do_awho)
{
	struct descriptor_data *d;
	struct char_data *tch;
	int num =0;
	*buf2 = '\0';
	
	if (in_arena == ARENA_OFF) {
		send_to_char("There is no Arena going on right now.\r\n", ch);
		return;
	}
	
	sprintf(buf,"   Gladiatori nell'Arena\r\n");
	sprintf(buf+strlen(buf),"---------------------------------------\r\n");
	sprintf(buf+strlen(buf),"Durata = %-3d   Ore all'inizio = %-3d\r\n",
		game_length, time_to_start);
	sprintf(buf+strlen(buf),"Livelli Ammessi: da %d a %d\r\n", lo_lim, hi_lim);
	sprintf(buf+strlen(buf),"         Montepremi = %ld\r\n", arena_pot);
	sprintf(buf+strlen(buf),"---------------------------------------\r\n");
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected){
			tch = d->character;
			if (zone_table[world[tch->in_room].zone].number == ARENA_ZONE && 
				ROOM_FLAGGED(IN_ROOM(tch), ROOM_ARENA) &&
				(tch->in_room != NOWHERE) && GET_LEVEL(tch)<LVL_IMMORT){
				sprintf(buf+strlen(buf), "%-20.20s%s",
					GET_NAME(tch),(!(++num % 3) ? "\r\n" : ""));
			}
		}	
	strcat(buf, "\r\n\r\n");
	page_string(ch->desc, buf, 1);	
}

ACMD(do_ahall)
{
	char site[MAX_INPUT_LENGTH], format[MAX_INPUT_LENGTH], *timestr;
	char format2[MAX_INPUT_LENGTH];
	struct hall_of_fame_element *fame_node;
	
	*buf = '\0';
	*buf2 = '\0';
	
	if (!fame_list) {
		send_to_char("Nessuno ha scritto il proprio nome nella Hall of Fame.\r\n", ch);
		return;
	}
	
	sprintf(buf2, "%s|---------------------------------------|%s\r\n",
		CCBLU(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf2+strlen(buf2), "%s|%sVincitori passati di Arena%s|%s\r\n",
		CCBLU(ch, C_NRM), CCNRM(ch, C_NRM),
		CCBLU(ch, C_NRM),CCNRM(ch, C_NRM));
	sprintf(buf2+strlen(buf2), "%s|---------------------------------------|%s\r\n\r\n",
		CCBLU(ch, C_NRM), CCNRM(ch, C_NRM));
	
	strcpy(format, "%-10.10s  %-16.16s  %-40s\r\n");
	sprintf(buf, format, 
		"Data",
		"Vincita",
		"Nome"
	);
	strcat(buf2, buf);
	sprintf(buf, format,
		"---------------------------------",
		"------------------------------------------------------------");
	strcat(buf2, buf);    
	
	strcpy(format2, "%-10.10s  %-16d  %s\r\n");
	
	for (fame_node = fame_list; fame_node; fame_node = fame_node->next) {
		if (fame_node->date) {
			timestr = asctime(localtime(&(fame_node->date)));
			*(timestr + 10) = 0;
			strcpy(site, timestr);
		} else
			strcpy(site, "Unknown");
		sprintf(buf, format2, site, fame_node->award, CAP(fame_node->name));
		strcat(buf2, buf);
	}
	
	page_string(ch->desc, buf2, 1);	
	return; 
}

void load_hall_of_fame(void)
{
	FILE *fl;
	int date, award;
	char name[MAX_NAME_LENGTH + 1];
	struct hall_of_fame_element *next_node;
	
	fame_list = 0;
	
	if (!(fl = fopen(HALL_FAME_FILE, "r"))) {
		perror("Non riesco ad aprire il file Hall of Fame");
		return;
	}
	while (fscanf(fl, "%s %d %d", name, &date, &award) == 3) {
		CREATE(next_node, struct hall_of_fame_element, 1);
		strncpy(next_node->name, name, MAX_NAME_LENGTH);
		next_node->name[MAX_NAME_LENGTH] = '\0';
		next_node->date = date;
		next_node->award = award;
		next_node->next = fame_list;
		fame_list = next_node;
	}
	
	fclose(fl);
}


void write_fame_list(void)
{
	FILE *fl;
	
	if (!(fl = fopen(HALL_FAME_FILE, "w"))) {
/*    syserrlog("Errore in scrittura _hall_of_fame_list", FALSE); */
		log("Errore in scrittura _hall_of_fame_list");
		return;
	}
	write_one_fame_node(fl, fame_list);/* recursively write from end to start */
	fclose(fl);
	
	return;
}

void write_one_fame_node(FILE * fp, struct hall_of_fame_element * node)
{
	if (node) {
		write_one_fame_node(fp, node->next);
		fprintf(fp, "%s %ld %ld\n", node->name, 
			(long) node->date, node->award);
	}
}

void find_bet_winners(struct char_data *winner)
{
	register struct char_data *i;
	struct descriptor_data *d;
	
	*buf1 = '\0';
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected) {
			i = d->character;
			if ((!IS_NPC(i)) && (i->in_room != NOWHERE) &&
				(GET_BETTED_ON(i) == GET_IDNUM(winner)) && GET_AMT_BET(i) > 0){
				sprintf(buf1, "Hai vinto %d monete per la tua scommessa.\r\n",
					GET_AMT_BET(i)*2);
				send_to_char(buf1, i);
				GET_GOLD(i) += GET_AMT_BET(i)*2;
				GET_BETTED_ON(i) = 0;
				GET_AMT_BET(i) = 0;
			}
		}
}

int arena_ok(struct char_data * ch, struct char_data * victim)
{ 
	if (ROOM_FLAGGED(ch->in_room, ROOM_ARENA) && 
		ROOM_FLAGGED(victim->in_room, ROOM_ARENA))
		return TRUE;
	else
		return FALSE;
}

void arena_kill(struct char_data * ch)
{
	struct affected_type *af, *next;

	if (FIGHTING(ch))
		stop_fighting(ch);
	
	if (ch->affected)
		for (af = ch->affected; af; af = next){
			next = af->next;
			if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
				affect_remove(ch, af);
		}
	if (!(in_arena == ARENA_OFF)) {	
		GET_HIT(ch) = GET_ARENA_HIT(ch);
		GET_MOVE(ch) = GET_ARENA_MOVE(ch);
		GET_MANA(ch) = GET_ARENA_MANA(ch);		
	} else {
		GET_HIT(ch) = 1;
		GET_MOVE(ch) = 1;
		GET_MANA(ch) = 1; 
	}
	GET_POS(ch) = POS_STANDING;
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
	char_from_room(ch);
	
#ifdef SAME_ROOM
	char_to_room(ch, real_room(GET_ARENA_ROOM(ch)));
#else
	char_to_room(ch, r_mortal_start_room[GET_HOMETOWN(ch)]);
#endif
	send_to_char("\r\nSei stato eliminato dall'arena. Un vortice ti riporta nella\r\n",ch);
	send_to_char("stessa stanza in cui ti trovavi quando ti eri iscritto all'arena\r\n",ch);
	act("$n cade dal cielo.", FALSE, ch, 0, 0, TO_ROOM);
	look_at_room(ch, 0);
}
