/* ************************************************************************
*   File: act.comm.c                                    Part of CircleMUD *
*  Usage: Player-level communication commands                             *
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
#include "screen.h"
#include "clan.h"
#include "clan2.h"
#include "dg_scripts.h"

bool ooc_attivo = 1;


/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct char_data *character_list;

/* internal functions */
void list_tb(struct char_data *ch);
void add_to_tb(struct char_data *ch, struct char_data *vict, char *argument);

/* extern functions */
char    *makedrunk(char *string ,struct char_data *ch);


ACMD(do_say)
{
	skip_spaces(&argument);
	
	if (!*argument)
	{
		send_to_char("Si, ma cosa vuoi dire?\r\n", ch);
		return;
	}
	
	if (AFF_FLAGGED(ch,AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}	
	
	if(PLR_FLAGGED(ch,PLR_FANTASMA))
	{
		send_to_char("Cerchi di parlare ma sembra che nessuno riesca a comprenderti.", ch);
		return;
	}
	
	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
	{
		send_to_char("Cerchi di parlare ma non riesci a proferire dei suoni.", ch);
		act("&5$n cerca di dire qualcosa ma non riesci a sentire niente.&0", FALSE, ch, 0, 0, TO_ROOM|DG_NO_TRIG);
		return;
	}
	else {
		switch (*(argument+strlen(argument)-1))
		{
			case '?':
				strcpy(buf1, "Chiedi");
				strcpy(buf2, "chiede");
				break;
			case '!':
				strcpy(buf1, "Esclami");
				strcpy(buf2, "esclama");
				break;
			default:
				strcpy(buf1, "Dici");
				strcpy(buf2, "dice");
				break;
		}
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else {
			if (!IS_NPC(ch))
				argument = makedrunk(argument, ch);
			sprintf(buf, "&5%s, '%s'&0", buf1, argument);
			act(buf, FALSE, ch, 0, argument, TO_CHAR);
		}
		sprintf(buf, "&5$n %s, '%s'&0", buf2, argument);
		act(buf, FALSE, ch, 0, 0, TO_ROOM|DG_NO_TRIG);
	}
 /* trigger check */
	speech_mtrigger(ch, argument);
	speech_wtrigger(ch, argument);
}


ACMD(do_gsay)
{
	struct char_data *k;
	struct follow_type *f;
	
	skip_spaces(&argument);
	
	if (!IS_AFFECTED(ch, AFF_GROUP)) {
		send_to_char("Non sei membro di nessun gruppo!\r\n", ch);
		return;
	}
	
	if (AFF_FLAGGED(ch,AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	if (!*argument)
	{
		send_to_char("SI, ma cosa vuoi dire al gruppo?\r\n", ch);
		return;
	}
	else {
		
		if (ch->master)
		{ k = ch->master; }
		else
		{ k = ch; }
		argument = makedrunk(argument, ch);
		sprintf(buf, "&2$n dice al gruppo, '%s'&0", argument);
		
		if (IS_AFFECTED(k, AFF_GROUP) && (k != ch))
			act(buf, FALSE, ch, 0, k, TO_VICT | TO_SLEEP);
		for (f = k->followers; f; f = f->next)
			if (IS_AFFECTED(f->follower, AFF_GROUP) && (f->follower != ch))
				act(buf, FALSE, ch, 0, f->follower, TO_VICT | TO_SLEEP);
			
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else {
			sprintf(buf, "&2Dici al gruppo, '%s'&0", argument);
			act(buf, FALSE, ch, 0, 0, TO_CHAR | TO_SLEEP);
		}
	}
}


void perform_tell(struct char_data *ch, struct char_data *vict, char *arg)
{
	send_to_char(CCRED(vict, C_NRM), vict);
	sprintf(buf, "$n ti dice, '%s'", arg);
	act(buf, FALSE, ch, 0, vict, TO_VICT | TO_SLEEP);
	send_to_char(CCNRM(vict, C_NRM), vict);
	
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
	else {
		send_to_char(CCRED(ch, C_CMP), ch);
		sprintf(buf, "Dici a $N, '%s'", arg);
		act(buf, FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		send_to_char(CCNRM(ch, C_CMP), ch);
	}
	
	GET_LAST_TELL(vict) = GET_IDNUM(ch);
	add_to_tb(vict, ch, arg);    // Aggiunge alla lista dei messaggi tell
}

bool is_tell_ok(struct char_data *ch, struct char_data *vict)
{
	if (ch == vict)
		send_to_char("Tenti di dirti qualche cosa.\r\n", ch);
	else if (PRF_FLAGGED(ch, PRF_NOTELL)&&(!IS_NPC(ch)))
		send_to_char("Non puoi dire niente algi altri se hai NOTELL attivato.\r\n", ch);
	else if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
		send_to_char("Le pareti sembrano assorbire le tue parole.\r\n", ch);
	else if (!IS_NPC(vict) && !vict->desc)        /* linkless */
	{
		if (GET_SEX(vict)==SEX_FEMALE)
			act("$N e' disconnessa al momento.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		else
			act("$N e' disconnesso al momento.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	}
	else if (PLR_FLAGGED(vict, PLR_WRITING))
		act("$N sta scrivendo un messaggio al momento.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if ((PRF_FLAGGED(vict, PRF_NOTELL)&&(!IS_NPC(ch))) || ROOM_FLAGGED(vict->in_room, ROOM_SOUNDPROOF))
		act("$N non ti puo' ascoltare.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if (PRF_FLAGGED(vict, PRF_AFK))
		act("$N e' afk ora, tenta piu' tardi.",
			FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else
		return 1;
	
	return 0;
	
}

void perform_send(struct char_data *ch, struct char_data *vict, char *arg)
{
	send_to_char(CCRED(vict, C_NRM), vict);
	sprintf(buf, "$n ti manda il messaggio, '%s'", arg);
	act(buf, FALSE, ch, 0, vict, TO_VICT | TO_SLEEP);
	send_to_char(CCNRM(vict, C_NRM), vict);
	
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
	else {
		send_to_char(CCRED(ch, C_CMP), ch);
		sprintf(buf, "Mandi il messaggio a $N, '%s'", arg);
		act(buf, FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		send_to_char(CCNRM(ch, C_CMP), ch);
	}
	
	GET_LAST_TELL(vict) = GET_IDNUM(ch);
}




/*
 * Yes, do_tell probably could be combined with whisper and ask, but
 * called frequently, and should IMHO be kept as tight as possible.
 */
ACMD(do_tell)
{
	struct char_data *vict;
	char *tmpStr = 0 ;
	
	
	if (AFF_FLAGGED(ch,AFF_SILENCE)) {
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}


	
	half_chop(argument, buf, buf2);
	tmpStr = makedrunk(buf2, ch);
	strncpy(buf2, tmpStr, strlen(tmpStr)+1);

	if (!*buf && !*buf2)
		list_tb(ch);
	else if (!*buf || !*buf2)
		send_to_char("A chi vuoi dire cosa??\r\n", ch);
	else if (!(vict = get_char_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else if (IS_NPC(vict)&&(!(vict->desc)))
		send_to_char(NOPERSON, ch);	
	else if (PLR_FLAGGED(ch, PLR_FANTASMA) && AWAKE(vict)) {
		send_to_char("Puoi parlare solo nel sonno dei mortali.\r\n", ch);
	}
	else if(PLR_FLAGGED(vict,PLR_CATARSI)) {
		send_to_char("Non puoi parlare ad una persona che sta meditando.\r\n", ch);
	}
	else if (is_tell_ok(ch, vict))
		perform_tell(ch, vict, buf2);
}


ACMD(do_reply)
{
	struct char_data *tch = character_list;
	
	if (AFF_FLAGGED(ch,AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	skip_spaces(&argument);
	
	if (GET_LAST_TELL(ch) == NOBODY)
		send_to_char("Non c'e' nessuno a cui rispondere.\r\n", ch);
	else if (!*argument)
		send_to_char("Quale' la tua risposta?\r\n", ch);
	else {
    /*
     * Make sure the person you're replying to is still playing by searching
     * for them.  Note, now last tell is stored as player IDnum instead of
     * a pointer, which is much better because it's safer, plus will still
     * work if someone logs out and back in again.
     */
		
		while (tch != NULL && GET_IDNUM(tch) != GET_LAST_TELL(ch))
			tch = tch->next;
		
		if (tch == NULL)
			send_to_char("Non e' piu' qui.\r\n", ch);
		else if (is_tell_ok(ch, tch))
		{
			argument = makedrunk(argument, ch);
			perform_tell(ch, tch, argument);
		}
	}
}

ACMD(do_send)
{
	struct char_data *vict;
	char *tmpStr = 0;
	
	if (AFF_FLAGGED(ch,AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	half_chop(argument, buf, buf2);
	tmpStr = makedrunk(buf2,ch);
	strncpy(buf2, tmpStr,strlen(tmpStr)+1);
	if (!*buf || !*buf2)
		send_to_char("A CHI vuoi mandare il messaggio??\r\n", ch);
	else if (!(vict = get_char_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	
	else if (is_tell_ok(ch, vict))
		perform_send(ch, vict, buf2);
}


ACMD(do_spec_comm)
{
	struct char_data *vict;
	char *action_sing, *action_plur, *action_others, *action_verbo;
	char *tmpStr = 0 ;
	
	if (AFF_FLAGGED(ch,AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	if (subcmd == SCMD_WHISPER) {
		action_sing = "Sussurri a ";
		action_plur = "$n ti sussurra";
		action_others = "$n sussurra qualche cosa a $N.";
		action_verbo = "sussurrare";
	} else {
		action_sing = "Chiedi a";
		action_plur = "$n ti chiede";
		action_others = "$n chiede a $N una cosa.";
		action_verbo = "chiedere";
	}
	
	half_chop(argument, buf, buf2);
	tmpStr = makedrunk(buf2,ch);
	strncpy(buf2, tmpStr, strlen(tmpStr)+1);
	if (!*buf || !*buf2) {
		sprintf(buf, "A chi vuoi %s.. e cosa??\r\n", action_verbo);
		send_to_char(buf, ch);
	} else if (!(vict = get_char_room_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else if (vict == ch)
		send_to_char("Non puoi avvicinare la tua bocca abbastanza al tuo orecchio...\r\n", ch);
	else {
		sprintf(buf, "%s, '%s'", action_plur, buf2);
		act(buf, FALSE, ch, 0, vict, TO_VICT);
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else {
			sprintf(buf, "%s %s, '%s'\r\n", action_sing, GET_NAME(vict), buf2);
			act(buf, FALSE, ch, 0, 0, TO_CHAR);
		}
		act(action_others, FALSE, ch, 0, vict, TO_NOTVICT);
	}
}



#define MAX_NOTE_LENGTH 1000	/* arbitrary */

ACMD(do_write)
{
	struct obj_data *paper = 0, *pen = 0;
	char *papername, *penname;
	
	papername = buf1;
	penname = buf2;
	
	two_arguments(argument, papername, penname);
	
	if (!ch->desc)
		return;
	
	if (!*papername) {		/* nothing was delivered */
		send_to_char("Scrivere?  Con cosa?  Su cosa?  Cosa stai tentando di fare?\r\n", ch);
		return;
	}
	if (*penname) {		/* there were two arguments */
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying))) {
			sprintf(buf, "Non hai  %s.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
		if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying))) {
			sprintf(buf, "Non hai %s.\r\n", penname);
			send_to_char(buf, ch);
			return;
		}
	} else {		/* there was one arg.. let's see what we can find */
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying))) {
			sprintf(buf, "Non c'e' %s tra le cose che porti.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
		if (GET_OBJ_TYPE(paper) == ITEM_PEN) {	/* oops, a pen.. */
			pen = paper;
			paper = 0;
		} else if (GET_OBJ_TYPE(paper) != ITEM_NOTE) {
			send_to_char("Quella cosa non ha niente a che fare con lo scrivere.\r\n", ch);
			return;
		}
    /* One object was found.. now for the other one. */
		if (!GET_EQ(ch, WEAR_HOLD)) {
			sprintf(buf, "Non puoi scrivere solo con  %s %s.\r\n", AN(papername),
				papername);
			send_to_char(buf, ch);
			return;
		}
		if (!CAN_SEE_OBJ(ch, GET_EQ(ch, WEAR_HOLD))) {
			send_to_char("La cosa che hai tra le mani e' invisibile!\r\n", ch);
			return;
		}
		if (pen)
			paper = GET_EQ(ch, WEAR_HOLD);
		else
			pen = GET_EQ(ch, WEAR_HOLD);
	}
	
	
  /* ok.. now let's see what kind of stuff we've found */
	if (GET_OBJ_TYPE(pen) != ITEM_PEN)
		act("$p non e' buono per scrivere.", FALSE, ch, pen, 0, TO_CHAR);
	else if (GET_OBJ_TYPE(paper) != ITEM_NOTE)
		act("Non puoi scrivere sulla $p.", FALSE, ch, paper, 0, TO_CHAR);
	else if (paper->action_description)
		send_to_char("C'e' gia scritto qualche cosa su quello.\r\n", ch);
	else {
      /* we can write - hooray! */
      /* this is the PERFECT code example of how to set up:
       * a) the text editor with a message already loaed
       * b) the abort buffer if the player aborts the message
       */
		ch->desc->backstr = NULL;
		send_to_char("Scrivi la tua mail.  (/s saves /h per help)\r\n", ch);
      /* ok, here we check for a message ALREADY on the paper */
		if (paper->action_description) {
 	/* we str_dup the original text to the descriptors->backstr */
			ch->desc->backstr = str_dup(paper->action_description);
 	/* send to the player what was on the paper (cause this is already */
 	/* loaded into the editor) */
			send_to_char(paper->action_description, ch);
		}
		act("$n inizia a scrivere una nota.", TRUE, ch, 0, 0, TO_ROOM);
      /* assign the descriptor's->str the value of the pointer to the text */
      /* pointer so that we can reallocate as needed (hopefully that made */
      /* sense :>) */
		ch->desc->str = &paper->action_description;
		ch->desc->max_str = MAX_NOTE_LENGTH;
		SET_BIT(GET_OBJ_EXTRA(paper), ITEM_RESTRING);
	}
}



ACMD(do_page)
{
	struct descriptor_data *d;
	struct char_data *vict;
	
	half_chop(argument, arg, buf2);
	
	if (IS_NPC(ch))
		send_to_char("Monsters can't page.. go away.\r\n", ch);
	else if (!*arg)
		send_to_char("Whom do you wish to page?\r\n", ch);
	else {
		sprintf(buf, "\007\007*%s* %s\r\n", GET_NAME(ch), buf2);
		if (!str_cmp(arg, "all")) {
			if (GET_LEVEL(ch) > LVL_GOD) {
				for (d = descriptor_list; d; d = d->next)
					if (!d->connected && d->character)
						act(buf, FALSE, ch, 0, d->character, TO_VICT);
			} else
				send_to_char("You will never be godly enough to do that!\r\n", ch);
			return;
		}
		if ((vict = get_char_vis(ch, arg)) != NULL) {
			act(buf, FALSE, ch, 0, vict, TO_VICT);
			if (PRF_FLAGGED(ch, PRF_NOREPEAT))
				send_to_char(OK, ch);
			else
				act(buf, FALSE, ch, 0, vict, TO_CHAR);
			return;
		} else
			send_to_char("Non c'e' quella persona in questo mondo!\r\n", ch);
	}
}


/**********************************************************************
 * generalized communication func, originally by Fred C. Merkel (Torg) *
  *********************************************************************/

ACMD(do_gen_comm)
{
	extern int level_can_shout;
	extern int holler_move_cost;
	struct descriptor_data *i;
	char color_on[24];
	struct char_data *vict;
	
  /* Array of flags which must _not_ be set in order for comm to be heard */
	static long long int channels[] = {
		PRF_DEAF,
		PRF_DEAF,
		PRF_NOGOSS,
		PRF_NOAUCT,
		PRF_NOGRATZ,
		PRF_NOC,

	};
	
  /*
   * com_msgs: [0] Message if you can't perform the action because of noshout
   *           [1] name of the action
   *           [2] message if you're not on the channel
   *           [3] a color string.
   */
	static char *com_msgs[][5] = {
		{"Non puoi gridare!!\r\n",
			"grida",
			"Disabilita il tuo noshut prima!\r\n",
			KYEL,
			"Gridi"
		},
		
		{"Non puoi urlare!!\r\n",
			"urla",
			"Disabilita il tuo noshut prima!\r\n",
			KYEL,
			"Urli"
		},
		
		{"Non puoi usare  gossip!!\r\n",
			"dice",
			"Non hai ancora un canale attivo!!\r\n",
			KYEL,
			"Dici"
		},
		
		{"Non puoi usare auction!!\r\n",
			"auction",
			"Non hai ancora un canale attivo!!\r\n",
			KMAG,
			"auction"},
		
		{"Non puoi usare congratulate!\r\n",
			"congratulazioni",
			"Non hai ancora un canale attivo!!\r\n",
			KGRN,
			"congrat"},
		
		{"Non puoi usare il canale OOC!\r\n",
			"OOC",
			"Attiva il canale OOC!!\r\n",
			KCYN,
			"OOC"}
		
	};
	
  /* to keep pets, etc from being ordered to shout */
	if (!ch->desc)
		return;
	if (ch && ch->in_room==NOWHERE)
		return;
	
	if (subcmd==SCMD_AUCTION)
	{
		send_to_char("In seguito a un utilizzo non appropriato da parte di qualcuno, il canale di comunicazione\r\n"
			"per le aste e' stato temporaneamente disattivato per tutti....\r\n", ch);
		return;
	}
	
	if (((subcmd==SCMD_AUCTION) || (subcmd==SCMD_HOLLER) || (subcmd==SCMD_SHOUT) || (subcmd==SCMD_OOC))
		&& ((ch->player.time.played / 3600)<15) && !IS_NPC(ch) && (GET_LEVEL(ch)<10)){
		send_to_char("Per poter usare questo canale devi aver giocato 15 ore o essere arrivato al 10 livello.\r\n", ch);
		return;
	}
	
	if (AFF_FLAGGED(ch, AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	if (PLR_FLAGGED(ch, PLR_NOSHOUT)) {
		send_to_char(com_msgs[subcmd][0], ch);
		return;
	}
	
	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF)) {
		send_to_char("Le pareti sembrano assorbire le tue parole.\r\n", ch);
		return;
	}
  /* level_can_shout defined in config.c */
	if (GET_LEVEL(ch) < level_can_shout) {
		sprintf(buf1, "Devi essere di livello %d prima che tu possa %s.\r\n",
			level_can_shout, com_msgs[subcmd][1]);
		send_to_char(buf1, ch);
		return;
	}
  /* make sure the char is on the channel */
	if (PRF_FLAGGED(ch, channels[subcmd]) && subcmd != SCMD_CONTROL_OOC) {
		send_to_char(com_msgs[subcmd][2], ch);
		return;
	}
	
	if ((ooc_attivo == FALSE) && (subcmd==SCMD_OOC)) {
		send_to_char("A causa degli abusi di alcuni player, l'ooc e' disattivato.\r\n", ch);
		return;
	}
	
	/* controllo dell'ooc da parte dei wiz */
	if(subcmd == SCMD_CONTROL_OOC) {
		if(ooc_attivo == FALSE) {
			send_to_char("OOC riattivato.\r\n", ch);
			sprintf(buf1,"(GC) : %s ha riattivato l'ooc", GET_NAME(ch));
			ooc_attivo = TRUE;
		} else {
			send_to_char("OOC disattivato.\r\n", ch);
			sprintf(buf1,"(GC) : %s ha disabilitato l'ooc", GET_NAME(ch));
			ooc_attivo = FALSE;
		}
		mudlog(buf1, NRM, LVL_QUEST_MASTER, TRUE);
		return;
	}
	
  /* skip leading spaces */
	skip_spaces(&argument);
	
  /* make sure that there is something there to say! */
	if (!*argument) {
		sprintf(buf1, "Si, %s, bene, %s dobbiamo, ma COSA???\r\n",
			com_msgs[subcmd][1], com_msgs[subcmd][1]);
		send_to_char(buf1, ch);
		return;
	}
	if (subcmd == SCMD_HOLLER) {
		if (GET_MOVE(ch) < holler_move_cost) {
			if (GET_SEX(ch)==SEX_FEMALE)
				send_to_char("Sei troppo stanca per gridare!\r\n", ch);
			else
				send_to_char("Sei troppo stanco per gridare!\r\n", ch);
			
			return;
		} else
      /*GET_MOVE(ch) -= holler_move_cost;*/
			alter_move(ch, holler_move_cost);
	}
	
  /* Adding in drunk text here */
	argument = makedrunk(argument,ch);
	
  /* set up the color on code */
	strcpy(color_on, com_msgs[subcmd][3]);
	
	if (subcmd == SCMD_GRATZ) {
		if (!*argument)
		{
			send_to_char("Con chi ti congratuli?\r\n", ch);
		}else {
			if ((vict = get_player_vis(ch, argument, 0))) {
				if (ch==vict) {send_to_char("Come sei modesto!", ch);return;}
				sprintf(buf,"Ti congratuli con %s.\r\n", PERS(vict, ch));
				send_to_char(buf, ch);
				sprintf(buf,"%s si congratula con te.\r\n", PERS(ch, vict));
				send_to_char(buf, vict);
			} else send_to_char(NOPERSON, ch);
		}
		return;
	}
	
  /* first, set up strings to be given to the communicator */
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
	else {
		if (COLOR_LEV(ch) >= C_CMP)
			sprintf(buf1, "%s%s, '%s'%s", color_on, com_msgs[subcmd][4],
				argument, KNRM);
		else
			sprintf(buf1, "%s, '%s'", com_msgs[subcmd][4], argument);
		act(buf1, FALSE, ch, 0, 0, TO_CHAR | TO_SLEEP);
	}
	
	sprintf(buf, "$n %s, '%s'", com_msgs[subcmd][1], argument);
	
	sprintf(buf1,"(CANALI): %s %s %s", GET_NAME(ch), com_msgs[subcmd][1], argument);
	log(buf1);
	
  /* now send all the strings out */
	for (i = descriptor_list; i; i = i->next) {
		if (   !i->connected 
		    && i != ch->desc 
		    && i->character 
		    && !PRF_FLAGGED(i->character, channels[subcmd]) 
		    && !PLR_FLAGGED(i->character, PLR_WRITING) 
		    && !ROOM_FLAGGED(i->character->in_room, ROOM_SOUNDPROOF)
		   ) {
			
			if (   (subcmd == SCMD_SHOUT)
			    && (   (world[ch->in_room].zone != world[i->character->in_room].zone) 
			        || GET_POS(i->character) < POS_RESTING))
				continue;
			
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(color_on, i->character);
			act(buf, FALSE, ch, 0, i->character, TO_VICT | TO_SLEEP);
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(KNRM, i->character);
		}
	}
}


ACMD(do_qcomm)
{
	struct descriptor_data *i;
	
	if (!PRF_FLAGGED(ch, PRF_QUEST)) {
		send_to_char("Non stai partecipando alla quest!!\r\n", ch);
		return;
	}
	
	if (AFF_FLAGGED(ch, AFF_SILENCE)) {
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF) || PLR_FLAGGED(ch, PLR_NOSHOUT)) {
		send_to_char("Credevi che lo lasciassimo per sempre!?\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) <= 10 && ch->player.time.played < 5 * 3600) {
	  send_to_char("Non puoi ancora usare questo canale di comunicazione.", ch);
	  return;
	}
	
	skip_spaces(&argument);
	
	if (!*argument) {
		sprintf(buf, "%s?  Si ok, %s noi dobbiamo, ma COSA??\r\n", CMD_NAME,
			CMD_NAME);
		CAP(buf);
		send_to_char(buf, ch);
	} else {
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else {
			if (subcmd == SCMD_QSAY)
				sprintf(buf, "(quest)Dici, '%s'", argument);
			else
				strcpy(buf, argument);
			act(buf, FALSE, ch, 0, argument, TO_CHAR);
		}
		
		if (subcmd == SCMD_QSAY)
		{
			argument = makedrunk(argument, ch);
			sprintf(buf, "$n (quest)-dice, '%s'", argument);
		}
		else { 
		  strcpy(buf, argument); 
		}
		
		sprintf(buf1,"(Qsay): %s dice %s", GET_NAME(ch), argument);
	  log(buf1);
		
		for (i = descriptor_list; i; i = i->next)
			if (   !i->connected 
			    && i != ch->desc 
			    && PRF_FLAGGED(i->character, PRF_QUEST))
				act(buf, 0, ch, 0, i->character, TO_VICT | TO_SLEEP);
	}
}
ACMD (do_ctell)
{
	struct descriptor_data *i;
	int minlev=1, c=0;
	char level_string[6]="\0\0\0\0\0\0";
	
	skip_spaces (&argument);
	
	
	if (AFF_FLAGGED(ch, AFF_SILENCE)) {
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	if (PLR_FLAGGED(ch, PLR_NOSHOUT)) {
		send_to_char("Non puoi usare questo canale.\r\n", ch);
		return;
	}
	
	if (PRF_FLAGGED(ch, PRF_REGNOTELL)) {
		send_to_char("Non sei in grado di comunicare con il regno.\r\n", ch);
		return;
	}
	
/*
 * The syntax of ctell for imms is different then for morts
 * mort: ctell <bla bla bla>    imms: ctell <clan_num> <bla bla bla>
 * Imms cannot actually see ctells but they can send them
 */
	if (GET_LEVEL(ch) >= LVL_GOD){
		c = atoi(argument);
		if ( find_clan_by_id(c) == -1 ){
			send_to_char ("Non esiste un regno con quel numero.\r\n", ch);
			return;
		}
		while ((*argument != ' ') && (*argument != '\0'))
			argument++;
		while (*argument == ' ') 
		  argument++;
	}
	else
	if((c=GET_CLAN(ch))==0 || GET_CLAN_RANK(ch)==0) {
		send_to_char ("Non fai parte di nessun regno.\r\n", ch);
		return;
	}
	
	skip_spaces (&argument);
	
	if (!*argument){
		send_to_char ("Cosa vuoi dire al tuo regno?\r\n", ch);
		return;
	}
	
	
	/*if (*argument == '#'){
		argument++;
		minlev = atoi (argument);
		if (minlev > clan[c].ranks){
			send_to_char ("Nessuno ha il grado cosi' alto per poterti sentire!\r\n", ch);
			return;
		}
		while (*argument != ' ') argument++;
		while (*argument == ' ') argument++;
		sprintf (level_string, "(%d)", minlev);
	}*/
	if (!argument) {
		send_to_char("Cosa vuoi dire al Regno?\r\n",ch);
		return;
	}
	
	if (GET_LEVEL(ch) > 10) {
	  if (GET_MANA(ch) < 5) {
	    send_to_char("Non hai sufficiente mana per comunicare con il regno.\r\n",ch);
		  return;
		}
	  GET_MANA(ch) -= 5;
	}
	
	if (PRF_FLAGGED(ch,PRF_NOREPEAT))
		sprintf (buf1, OK);
	else {
		argument = makedrunk(argument, ch);
		sprintf (buf1, "&4&bDici al Regno%s, '%s'&0\r\n", 
		                level_string, 
		                argument );
	}
	send_to_char (buf1, ch);
	
	
	
	for (i = descriptor_list; i; i=i->next){
		if ((!i->connected) && (!PLR_FLAGGED(i->character, PLR_WRITING | PLR_MAILING)) && (i != ch->desc || !(PRF_FLAGGED(i->character, PRF_NOREPEAT)))) {
			if (i->character !=NULL && i->character->player_specials->saved.clan == c){
				if (i->character->player_specials->saved.clan_rank >= minlev){
					if (!PRF_FLAGGED(i->character, PRF_REGNOTELL)){
						if (strcmp (i->character->player.name, ch->player.name)){
							sprintf (buf, "&4&b%s dice al Regno %s, '%s'&0\r\n",
								            (((IS_AFFECTED(ch, AFF_INVISIBLE)) &&
										          (!IS_AFFECTED(i->character, AFF_DETECT_INVIS))) ?
									          "Qualcuno" : ch->player.name),
									          level_string, 
									          argument );
							send_to_char (buf, i->character);
						}
					}
				}
			}
		}
	}
	if (clan[find_clan_by_id(c)].name != NULL) {
		sprintf (buf1, "(REGNO_TELL %s)&%d %s >: %s",
	                clan[find_clan_by_id(c)].name,
/*	                GET_HOME(ch) == 1 ? "Aldaria" :
	                GET_HOME(ch) == 3 ? "Tolnedra" :
	                GET_HOME(ch) == 4 ? "Elenia" : 
	                GET_HOME(ch) == 5 ? "Sendaria": "NoRegno",*/
	                GET_CLAN(ch) != 9 ? GET_CLAN(ch) : 1, //colore - se il regno e' il n.9 uso il colore n. 1
	                ch->player.name,
	                argument);
		mudlog(buf1, NRM, LVL_QUEST_MASTER, TRUE);
	} else {
		sprintf(buf1, "(REGNO_TELL -error-) --> il pg %s ha il campo clan (%d) che porta a un nome di regno NULL!",
			ch->player.name, GET_CLAN(ch));
		mudlog(buf1, NRM, LVL_QUEST_MASTER, TRUE);
	}
	return;
}



void list_tb(struct char_data *ch)
{
	int i;
	
	send_to_char("Ultimi messaggi ricevuti\r\n"
		"-----------------------------\r\n", ch);
	
	for (i = TELLS_STORED-1; i >= 0; i--)
		if (GET_TELLS(ch,i))
			send_to_char(GET_TELLS(ch,i), ch);
}

void add_to_tb(struct char_data *ch, struct char_data *vict, char *argument)
{
	char *new_tell, *old_tell;
	int i;
	
	if (IS_NPC(vict)) return; // Non registrare i tell ai mobs
	
	sprintf(buf,"%s%s%s > %s%s%s\r\n",
		CCRED(ch, C_NRM), PERS(vict, ch), CCNRM(ch, C_NRM),
		CCCYN(ch, C_NRM), argument, CCNRM(ch, C_NRM));
	
	CREATE(new_tell, char, strlen(buf) +1);
	strcpy(new_tell, buf);
	
	for(i = 0; i <= TELLS_STORED-1; i++)
	{
		if ((i == TELLS_STORED-1) && GET_TELLS(ch, i)) free(GET_TELLS(ch, i));   // Eliminiamo l'ultimo messaggio della lista
		
		old_tell=GET_TELLS(ch, i);
		GET_TELLS(ch, i) = new_tell;
		new_tell = old_tell;
	}
}

ACMD (do_gtell)
{
	struct descriptor_data *i;
	int minlev=1, c=0;
	char level_string[6]="\0\0\0\0\0\0";
	
	skip_spaces (&argument);
	
/*
 * The syntax of ctell for imms is different then for morts
 * mort: ctell <bla bla bla>    imms: ctell <clan_num> <bla bla bla>
 * Imms cannot actually see ctells but they can send them
 */
	if (AFF_FLAGGED(ch,AFF_SILENCE))
	{
		send_to_char("Non riesci a parlare, una strana forza ti impedisce di emettere suoni?\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) >= LVL_GOD){
		c = atoi (argument);
		if ((c <= 0) || (c > new_num_of_clans)){
			send_to_char ("Non esiste una gilda con quel numero.\r\n", ch);
			return;
		}
		while ((*argument != ' ') && (*argument != '\0'))
			argument++;
		while (*argument == ' ') argument++;
	}
	else
	if((c=NEW_GET_CLAN(ch))==0 || NEW_GET_CLAN_RANK(ch)==0) {
		send_to_char ("Non fai parte di nessuna gilda.\r\n", ch);
		return;
	}
	
	skip_spaces (&argument);
	
	if (!*argument){
		send_to_char ("Cosa vuoi dire alla tua gilda?\r\n", ch);
		return;
	}
	
	
	if (*argument == '#'){
		argument++;
		minlev = atoi (argument);
		if (minlev > new_clan[c].ranks){
			send_to_char ("Nessuno ha il grado cosi' alto per poterti sentire!\r\n", ch);
			return;
		}
		while (*argument != ' ') argument++;
		while (*argument == ' ') argument++;
		sprintf (level_string, "(%d)", minlev);
	}
	
	if (PRF_FLAGGED(ch,PRF_NOREPEAT))
		sprintf (buf1, OK);
	else
	{
		argument = makedrunk(argument, ch);
		sprintf (buf1, "&4&bDici alla gilda%s, '%s'&0\r\n",level_string, argument);
	}
	send_to_char (buf1, ch);
	
	for (i = descriptor_list; i; i=i->next){
		if (i->character !=NULL && i->character->player_specials->saved.new_clan == c){
			if (i->character->player_specials->saved.new_clan_rank >= minlev){
				if (strcmp (i->character->player.name, ch->player.name)){
					sprintf (buf, "&5&b%s dice alla tua gilda%s, '%s'&0\r\n",
						(((IS_AFFECTED(ch, AFF_INVISIBLE)) &&
								(!IS_AFFECTED(i->character, AFF_DETECT_INVIS))) ?
							"Qualcuno" : ch->player.name), level_string, argument);
					send_to_char (buf, i->character);
				}
			}
		}
	}
	
	return;
}
