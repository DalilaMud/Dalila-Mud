/***************************************************************************
*  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
*                                                                         *
*  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
*  Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                         *
*  In order to use any part of this Merc Diku Mud, you must comply with   *
*  both the original Diku license in 'license.doc' as well the Merc       *
*  license in 'license.txt'.  In particular, you may not remove either of *
*  these copyright notices.                                               *
*                                                                         *
*  Much time and thought has gone into this software and you are          *
*  benefitting.  We hope that you share your changes too.  What goes      *
*  around, comes around.                                                  *
***************************************************************************/
       
/***************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/
	       
/***************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com   *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this  * 
*  code is allowed provided you add a credit line to the effect of:        *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest    *
*  of the standard diku/rom credits. If you use this or a modified version *
*  of this code, let me know via email: moongate@moongate.ams.com. Further *
*  updates will be posted to the rom mailing list. If you'd like to get    *
*  the latest version of quest.c, please send a request to the above add-  *
*  ress. Quest Code v2.00.                                                 *
***************************************************************************/
	       
/*****************************************************************************
***  Quest.c - File per la generazione e gestione delle AutoQuest          ***
***  Questo file e' di proprieta' esclusiva dello staff di DalilaMUD       ***
***  Autore Cereda Massimo (Chardhros) su idee di Raffaele De Vita (Offar) ***
***  Versione: 3.00                                                        ***
***									   ***
***  Riscritto da: Libardi Paolo (Orione)				   ***
***  Versione: 4.00							   ***
***									   ***
*****************************************************************************/

		    
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "handler.h"
#include "interpreter.h"
#include "comm.h"
#include "db.h"

/* Extern Variables */
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct char_data *character_list;
extern struct room_data *world;

/* Extern Functions */
extern void write_aliases(struct char_data *ch);
extern ACMD(do_say);
extern ACMD(do_qcomm);

/* Local Functions */
struct char_data *find_questman(struct char_data *ch);
struct obj_data *find_questobj(struct char_data *ch);
void quest_premi(struct char_data *ch,struct char_data *questman);
void flag_crash_reset(struct char_data *ch, int c);
int generate_quest(struct char_data *ch, struct char_data *questman);

/* Tipi di quest possibili */
const char *tipo_quest[] = {
    "NO_QUEST", "MOB KILL", "GET ITEM", "GIVE ITEM", "GO TO ROOM", "ARTICOLATA"
        };

/* Punti quest assegnati al completamento "corretto" di una quest */
const int ricompense[] = {0, 5, 4, 2, 3, 20};

/* Durata massima della quest, a seconda del tipo */
const int durata_quest[] = {0, 15, 13, 5, 7, 45};
/* Minuti di gioco di attesa per poter richiedere una nuova quest */
const int attesa = 1;
	
#define PURGE_ROOM 1204 //Stanza in cui eliminare mob e obj residui di quest

#define QUEST_MOB_MAX 3 //Numero di mob potenziali obiettivo di quest (sempre almeno 1)
const int QUEST_MOB[] = {
	1700, 1701, 1702 
    };

#define QUEST_OBJ_MAX 3 //Numero di obj potenziali obiettivo di quest (sempre almeno 1)
const int QUEST_OBJ[] = {
	1700, 1701, 1702
    };

#define QUEST_ROOM_MAX 5 //Numero di room potenziali obiettivo di quest (sempre almeno 1)
const int QUEST_ROOM[] = {
    1490500, 1491500, 1494499, 1494500, 1495499, 
    };

/* Ricerca un mob con flag MOB_QUESTMASTER tra quelli presenti nella room del pg */
struct char_data *find_questman(struct char_data *ch) {
    struct char_data *questman = NULL;
    
    for (questman=world[ch->in_room].people;questman;questman=questman->next_in_room) {
	if (!IS_NPC(questman)) continue;
	if (MOB_FLAGGED(questman, MOB_QUESTMASTER))
	    return questman;
    }
    return NULL;
}

/* Procedura di scelta di un mob addatto al pg */
struct char_data *scelta_mob(struct char_data *ch) {
    struct char_data *vcerca = NULL;
    int mcounter, vnum, level_diff;
    
    for (mcounter=0;mcounter<100;mcounter++) {
	vnum = QUEST_MOB[number(0,QUEST_MOB_MAX-1)];
	//ricerca il mob nel mondo, se esiste vuol dire che e' gia' obiettivo di quest
	if (get_char_num(real_mobile(vnum))==NULL) {
	    //dopo read_mobile fare char_to_room appena possibile!!! altrimenti crash!
	    if ((vcerca = read_mobile(vnum, VIRTUAL))!=NULL) { 
		level_diff = GET_LEVEL(vcerca) - GET_LEVEL(ch);
		if ((level_diff>5) || (level_diff<-5)) {
		    char_to_room(vcerca, real_room(PURGE_ROOM));
		    extract_char(vcerca);
		    vcerca=NULL;
		}
	    }
	}
	if (vcerca!=NULL) break;
    }
    
    return vcerca;
}

/* Procedura di scelta di un item */
struct obj_data *scelta_item() {
    struct obj_data *vcerca = NULL;
    int ocounter, vnum;
    
    for (ocounter=0;ocounter<100;ocounter++) {
	vnum = QUEST_OBJ[number(0,QUEST_OBJ_MAX-1)];
	//ricerca l'obj nel mondo, se esiste vuol dire che e' gia' obiettivo di quest
	if (get_obj_num(real_object(vnum))==NULL)
	    vcerca = read_object(vnum, VIRTUAL);
        if (vcerca!=NULL) break;
    }
    
    return vcerca;
}


/* Procedura di scelta di una room in wilderness */
room_rnum scegli_room() {
    room_rnum vcerca;
    int vnum;
    
    vnum = QUEST_ROOM[number(0,QUEST_ROOM_MAX-1)];
    vcerca = real_room(vnum);
    if (vcerca==-1) {
	sprintf(buf1, "Autoquest: la room #%d non esiste!", vnum);
	mudlog(buf1, NRM, LVL_QUEST_MASTER, FALSE);
    }
    return vcerca;
}	

/* Verifica se l'obj di quest si trova nell'inventario del pg */
struct obj_data *find_questobj(struct char_data *ch) {
    struct obj_data *obj, *obj_next;
    
    for (obj = ch->carrying;obj;obj = obj_next) {
	obj_next = obj->next_content;
	if ((obj!=NULL) && (GET_OBJ_VNUM(obj)==GET_QUESTOBJ(ch)))
	    break;
    }
    return obj;
}

/* Procedura di assegnazione dei premi per quest completate correttamente */
void quest_premi(struct char_data *ch,struct char_data *questman) {
    char bufsay[MAX_STRING_LENGTH];
    
    sprintf(bufsay, "Congratulazioni!");
    do_say(questman, bufsay, 0, 0);
    sprintf(bufsay, "Come ricompensa ti dono %d punti quest.", ricompense[GET_TIPOQUEST(ch)]);
    do_say(questman, bufsay, 0, 0);
    
    sprintf(buf, "Autoquest: %s ha terminato vittoriosamente la sua quest %s.", GET_NAME(ch), tipo_quest[GET_TIPOQUEST(ch)]);
    mudlog(buf, NRM, LVL_IMMORT, FALSE);
    sprintf(buf, "Autoquest: Ha ricevuto in premio %d punti quest.", ricompense[GET_TIPOQUEST(ch)]);
    mudlog(buf, NRM, LVL_IMMORT, FALSE);
    sprintf(buf, "%s ha completato la sua quest! COMPLIMENTI!", GET_NAME(ch));
    do_qcomm(ch, buf, 0, SCMD_QECHO);
    
    GET_QUESTPOINTS(ch) += ricompense[GET_TIPOQUEST(ch)];
    flag_crash_reset(ch, FALSE);
    write_aliases(ch);
    save_char(ch, NOWHERE);
    Crash_crashsave(ch);
}

/* The main quest function */

#define AQ_PUNT 1
#define AQ_INFO 2
#define AQ_TEMP 3
#define AQ_RICH 4
#define AQ_FINE 5

ACMD(do_autoquest) {
    char arg1[MAX_STRING_LENGTH], bufsay[MAX_STRING_LENGTH];
    struct char_data *questman = NULL;
    struct obj_data *obj = NULL;
    struct obj_data *questinfoobj = NULL;
    struct char_data *questinfomob = NULL;
    int comando = 0;
    
    one_argument(argument, arg1);
    
    if (is_abbrev(arg1, "punti")) comando = AQ_PUNT;
    else if (is_abbrev(arg1, "informazioni")) comando = AQ_INFO;
    else if (is_abbrev(arg1, "tempo")) comando = AQ_TEMP;
    else if (is_abbrev(arg1, "richiesta")) comando = AQ_RICH;
    else if (is_abbrev(arg1, "fine")) comando = AQ_FINE;
    
    switch(comando) {
	case AQ_PUNT:	// Visualizzazione punti quest del pg
	    sprintf(buf, "Hai %d punti quest.\r\n", GET_QUESTPOINTS(ch));
	    send_to_char(buf, ch);
	    return;
	
	case AQ_INFO:	// Informazioni sulla quest attualmente assegnata al pg
	    if (IS_QUESTOR(ch)) {
		switch(GET_TIPOQUEST(ch)) {
		    case QUEST_MOBKILL:
			if ((GET_QUESTMRR(ch)>0) &&  ((questinfomob=get_char_num(real_mobile(GET_QUESTMRR(ch))))!=NULL)) {
				sprintf(buf, "La tua quest consiste nell'uccidere %s!\r\n", GET_NAME(questinfomob));
				send_to_char(buf, ch);
			} else if ((GET_QUESTMRR(ch)==-1) && ((questman = GET_QUESTGIVER(ch))!=NULL)) {
			    send_to_char("COMPLIMENTI il grosso della quest e' fatto!\r\n", ch);
			    sprintf(buf, "Torna da %s prima che il tempo a tua disposizione finisca!\r\n", GET_NAME(GET_QUESTGIVER(ch)));
			    send_to_char(buf, ch);
			} else if (GET_QUESTMRR(ch)==-2)
			    send_to_char("Uccidere il nemico era compito TUO!\r\n", ch);
			else {
			    sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo MOB KILL", GET_NAME(ch), GET_QUESTMRR(ch));
			    mudlog(buf, NRM, LVL_BUILDER, FALSE);
			    send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			}
			return;
			
		    case QUEST_GETITEM:
			if ((GET_QUESTOBJ(ch)>0) && ((questinfoobj=get_obj_num(real_object(GET_QUESTOBJ(ch))))!=NULL)) {
			    if ((GET_QUESTMRR(ch)>0) &&  ((questinfomob=get_char_num(real_mobile(GET_QUESTMRR(ch))))!=NULL)) {
				sprintf(buf, "La tua quest consiste nel recuperare %s, attualmente in mano a %s!\r\n", questinfoobj->short_description, GET_NAME(questinfomob));
				send_to_char(buf, ch);
			    } else if ((GET_QUESTMRR(ch)==-1) && ((questman = GET_QUESTGIVER(ch))!=NULL)) {
				sprintf(buf, "Prendi %s dal cadavere, e portalo in fretta da %s!\r\n", questinfoobj->short_description, GET_NAME(GET_QUESTGIVER(ch)));
				send_to_char(buf, ch);
			    } else if (GET_QUESTMRR(ch)==-2)
				send_to_char("Uccidere il nemico era compito TUO!\r\n", ch);
			    else if ((GET_QUESTMRR(ch)==-3) && ((questman = GET_QUESTGIVER(ch))!=NULL)) {
				sprintf(buf, "COMPLIMENTI! Hai sottratto %s senza farti scoprire!\r\n", questinfoobj->short_description);
				send_to_char(buf, ch);
				sprintf(buf, "Torna da %s prima che il tempo a tua disposizione finisca!\r\n", GET_NAME(GET_QUESTGIVER(ch)));
				send_to_char(buf, ch);
			    } else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo GET ITEM", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			    }
			} else {
			    sprintf(buf, "QuestObj di %s e' %d per una autoquest di tipo GET ITEM", GET_NAME(ch), GET_QUESTOBJ(ch));
			    mudlog(buf, NRM, LVL_BUILDER, FALSE);
			    send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			}
			return;
			
		    case QUEST_GIVITEM:
			if ((GET_QUESTOBJ(ch)>0) && ((questinfoobj=get_obj_num(real_object(GET_QUESTOBJ(ch))))!=NULL)) {
			    if ((GET_QUESTMRR(ch)>0) &&  ((questinfomob=get_char_num(real_mobile(GET_QUESTMRR(ch))))!=NULL)) {
				sprintf(buf, "La tua quest consiste nel portare %s a %s", questinfoobj->name, GET_NAME(questinfomob));
				send_to_char(buf, ch);
			    } else if ((GET_QUESTMRR(ch)==-4) && ((questman = GET_QUESTGIVER(ch))!=NULL)) {
				sprintf(buf, "Ora che hai consegnato %s, torna da %s per la ricompensa.", questinfoobj->short_description, GET_NAME(questman));
				send_to_char(buf, ch);
			    } else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo GIVE ITEM", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			    }
			} else {
			    sprintf(buf, "QuestObj di %s e' %d per una autoquest di tipo GIVE ITEM", GET_NAME(ch), GET_QUESTOBJ(ch));
			    mudlog(buf, NRM, LVL_BUILDER, FALSE);
			    send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			}
			return;
			
		    case QUEST_GOTROOM:
			if ((GET_QUESTOBJ(ch)>0) && ((questinfoobj=get_obj_num(real_object(GET_QUESTOBJ(ch))))!=NULL)) {
			    if (GET_QUESTMRR(ch)>0) {
				sprintf(buf, "La tua quest consiste nel recuperare %s, abbandonato a %s", questinfoobj->short_description, world[GET_QUESTMRR(ch)].name);
				send_to_char(buf, ch);
			    } else if ((GET_QUESTMRR(ch)==-5) && ((questman = GET_QUESTGIVER(ch))!=NULL)) {
				sprintf(buf, "COMPLIMENTI! Sei riuscito a trovare %s.", questinfoobj->short_description);
				send_to_char(buf, ch);				
				sprintf(buf, "Ora portalo a %s prima che il tempo a tua disposizione finisca", GET_NAME(questman));
				send_to_char(buf, ch);
			    } else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo GO TO ROOM", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			    }
			} else {
			    sprintf(buf, "QuestObj di %s e' %d per una autoquest di tipo GO TO ROOM", GET_NAME(ch), GET_QUESTOBJ(ch));
			    mudlog(buf, NRM, LVL_BUILDER, FALSE);
			    send_to_char("Non e' possibile fornire alcuna informazione.", ch);
			}
			return;
			
		    case QUEST_ARTICOL:
			mudlog("Quest Articolata da Fare!", NRM, LVL_BUILDER, FALSE);
			return;
		}	    
	    }
	    
	    sprintf(buf, "Non stai partecipando a nessuna quest.\r\n");
	    send_to_char(buf,ch);
	    return;
	
	case AQ_TEMP:	// Informazioni sul tempo residuo di quest o di attesa
	    if (IS_QUESTOR(ch)) {
		if (GET_COUNTDOWN(ch)>1) {
		    sprintf(buf, "Ti rimangono %d minuti per completare la quest.\r\n", GET_COUNTDOWN(ch));
		    send_to_char(buf, ch);
		} else if (GET_COUNTDOWN(ch)==1)
		    send_to_char("Ti rimane meno di 1 minuto per completare la quest!\r\n", ch);
		else send_to_char("Puoi affrontare una nuova Quest.\r\n", ch);
	    } else {
		if (GET_COUNTDOWN(ch)>1) {
		    sprintf(buf, "Devono trascorre %d minuti prima che tu possa affrontare una nuova quest.\r\n", GET_COUNTDOWN(ch));
		    send_to_char(buf, ch);
		} else if (GET_COUNTDOWN(ch)==1)
		    send_to_char("Tra meno di 1 minuto potrai affrontare una nuova quest!\r\n", ch);
		else send_to_char("Puoi affrontare una nuova Quest.\r\n", ch);
	    }
	    return;

	case AQ_RICH:
	    if ((questman=find_questman(ch))==NULL) {
		send_to_char("Puoi farlo solo davanti al QuestMaster.\r\n", ch);
		return;
	    }
	    
	    act("$n chiede a $N una quest.", FALSE, ch, NULL, questman, TO_ROOM);
	    act("Chiedi a $N una quest.", FALSE, ch, NULL, questman, TO_CHAR);
	    
	    if (IS_QUESTOR(ch)) {
		do_say(questman, "Stai gia' partecipando ad un'altra quest!", 0, 0);
		sprintf(buf1, "Autoquest: %s ha richiesto una Quest al mob #%d, ma ne sta gia' affrontando un'altra", GET_NAME(ch), GET_MOB_VNUM(questman));
		mudlog(buf1, NRM, LVL_IMMORT,FALSE);
		return;
	    }
	    
	    //ch non e' questor
	    if (GET_COUNTDOWN(ch)>0) {
		if (GET_SEX(ch)==SEX_MALE) 
		    sprintf(bufsay, "Sei molto coraggioso, %s, ma lascia il posto a qualcun altro.", GET_NAME(ch));
		else
		    sprintf(bufsay, "Sei molto coraggiosa, %s, ma lascia il posto a qualcun altro.", GET_NAME(ch));
		do_say(questman, bufsay, 0, 0);
		do_say(questman, "Torna piu' tardi", 0, 0);
		sprintf(buf1, "Autoquest: %s ha richiesto una Quest al mob #%d, ma la potra' affrontare tra %d minuti",  GET_NAME(ch), GET_MOB_VNUM(questman), GET_COUNTDOWN(ch));
		mudlog(buf1, NRM, LVL_IMMORT,FALSE);
		return;
	    }

	    sprintf(bufsay, "Ti ringrazio, nobile %s!", GET_NAME(ch));
	    do_say(questman, bufsay, 0, 0);
	    GET_TIPOQUEST(ch) = generate_quest(ch, questman);
	    
	    if (GET_TIPOQUEST(ch)>QUEST_NO) {
		SET_BIT(PLR_FLAGS(ch), PLR_QUESTOR);
		SET_BIT(PRF_FLAGS(ch), PRF_QUEST);
		GET_QUESTGIVER(ch) = questman;
		GET_COUNTDOWN(ch) = durata_quest[GET_TIPOQUEST(ch)];
		sprintf(bufsay, "Hai %d minuti per completare la quest.", GET_COUNTDOWN(ch));
		do_say(questman, bufsay, 0, 0);
		sprintf(bufsay, "Che ??????? sia con te!");
		do_say(questman, bufsay, 0, 0);
		sprintf(bufsay, "%s sta per affrontare una quest! In Bocca al Tatanka!", GET_NAME(ch)),
		do_qcomm(ch, bufsay, 0, SCMD_QECHO);
	    } else { //if (GET_TIPOQUEST(ch)==QUEST_NO)
		do_say(questman, "Mi dispiace, ma non ho nulla per te in questo momento.", 0, 0);
		do_say(questman, "Riprova piu' tardi.", 0, 0);
		GET_QUESTGIVER(ch) = NULL;
	    }
	    
	    return;
	
	case AQ_FINE:	// Verifiche ed Azioni al completamento della quest
	    if ((questman=find_questman(ch))==NULL) {
		send_to_char("Puoi farlo solo davanti al QuestMaster.\r\n", ch);
		return;
	    }

	    act("$n annuncia a $N di aver completato la quest.", FALSE, ch, NULL, questman, TO_ROOM);
	    act ("Tu annunci a $N di aver completato la quest.", FALSE, ch, NULL, questman, TO_CHAR);
	    if (GET_QUESTGIVER(ch) != questman) {
		do_say(questman,"Non ti ho mai assegnato una quest!", 0, 0);
		do_say(questman,"Forse mi hai preso per qualcun'altro.", 0, 0);				
		return;
	    }
	    
	    if (IS_QUESTOR(ch)) {
		if (GET_COUNTDOWN(ch)>0) {
		    switch(GET_TIPOQUEST(ch)) {
			case QUEST_MOBKILL:
			    if (GET_QUESTMRR(ch)>0)
				do_say(questman, "Non hai ancora completato la quest, ma forse hai abbastanza tempo per farlo!", 0, 0);
			    else if (GET_QUESTMRR(ch)==-1)
				quest_premi(ch, questman);
			    else if (GET_QUESTMRR(ch)==-2) {
				do_say(questman, "Non e' stata la tua mano a togliere il respiro al tuo nemico!", 0, 0);
				do_say(questman, "Non otterrai alcuna ricompensa!", 0, 0);
				sprintf(buf, "Autoquest: %s non ha finito la quest MOB KILL regolarmente", GET_NAME(ch));
				mudlog(buf, NRM, LVL_IMMORT, FALSE);
				sprintf(buf, "%s non ha finito regolarmente la sua quest.", GET_NAME(ch));
				do_qcomm(ch, buf, 0, SCMD_QECHO);
				flag_crash_reset(ch, FALSE);
			    } else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo MOB KILL", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile stabilire lo stato della quest.", ch);
			    }
			    return;
			
			case QUEST_GETITEM:
			    if (GET_QUESTMRR(ch)>0)
				do_say(questman, "Non hai ancora completato la quest, ma forse hai abbastanza tempo per farlo!", 0, 0);
			    else if ((GET_QUESTMRR(ch)==-1) || (GET_QUESTMRR(ch)==-3)) {
				if ((GET_QUESTOBJ(ch)>0) && ((obj=find_questobj(ch))!=NULL)) {
				    act("$n porge $p a $N.", FALSE, ch, obj, questman, TO_ROOM);
				    act("Porgi $p a $N.", FALSE, ch, obj, questman, TO_CHAR);

				    quest_premi(ch, questman);
				    
				    obj_from_char(obj);
				    obj_to_room(obj, real_room(PURGE_ROOM));
				    extract_obj(obj);
				} else
				    do_say(questman, "Bene, ma non vedo l'oggetto che dovevi recuperare!", 0, 0);
				
			    } else if (GET_QUESTMRR(ch)==-2) {
				do_say(questman, "Non e' stata la tua mano a togliere il respiro al tuo nemico!", 0, 0);
				do_say(questman, "Non otterrai alcuna ricompensa!", 0, 0);
				
				if ((GET_QUESTOBJ(ch)>0) && (obj = get_obj_num(real_object(GET_QUESTOBJ(ch))))) {
				    if (obj->carried_by!=NULL) obj_from_char(obj);
				    else if (obj->in_obj!=NULL) obj_from_obj(obj);
				    else obj_from_room(obj);
				    obj_to_room(obj, real_room(PURGE_ROOM));
				    extract_obj(obj);
				}
								
				sprintf(buf, "Autoquest: %s non ha finito la quest MOB KILL regolarmente", GET_NAME(ch));
				mudlog(buf, NRM, LVL_IMMORT, FALSE);
				sprintf(buf, "%s non ha finito regolarmente la sua quest.", GET_NAME(ch));
				do_qcomm(ch, buf, 0, SCMD_QECHO);
				flag_crash_reset(ch, FALSE);
				
			    } else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo GET ITEM", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile stabilire lo stato della quest.", ch);
			    }
			    return;
			
			case QUEST_GIVITEM:
			    if (GET_QUESTMRR(ch)>0)
				do_say(questman, "Non hai ancora completato la quest, ma forse hai abbastanza tempo per farlo!", 0, 0);

			    else if (GET_QUESTMRR(ch)==-4)
				quest_premi(ch, questman);
				
			    else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo GIVE ITEM", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile stabilire lo stato della quest.", ch);
			    }
			    return;
			
			case QUEST_GOTROOM:
			    if (GET_QUESTMRR(ch)>0)
				do_say(questman, "Non hai ancora completato la quest, ma forse hai abbastanza tempo per farlo!", 0, 0);

			    else if (GET_QUESTMRR(ch)==-5) {
				if ((GET_QUESTOBJ(ch)>0) && ((obj=find_questobj(ch))!=NULL)) {
				    act("$n porge $p a $N.", FALSE, ch, obj, questman, TO_ROOM);
				    act("Porgi $p a $N.", FALSE, ch, obj, questman, TO_CHAR);
				    
				    quest_premi(ch, questman);
				    
				    obj_from_char(obj);
				    obj_to_room(obj, real_room(PURGE_ROOM));
				    extract_obj(obj);
				} else
				    do_say(questman, "Bene, ma non vedo l'oggetto che dovevi recuperare!", 0, 0);
				
			    } else {
				sprintf(buf, "QuestMRR di %s e' %d per una autoquest di tipo GIVE ITEM", GET_NAME(ch), GET_QUESTMRR(ch));
				mudlog(buf, NRM, LVL_BUILDER, FALSE);
				send_to_char("Non e' possibile stabilire lo stato della quest.", ch);
			    }
			    return;
			
			case QUEST_ARTICOL:
			    mudlog("Quest Articolata da Fare!", NRM, LVL_BUILDER, FALSE);
			    return;
		    }
		
		} else {
		    //reset flag ed eliminazione mob e/o obj di quest viene
		    //eseguita da quest_update, quando countdown arriva a 0
		    sprintf(bufsay, "Non hai completato la quest in tempo!");
		    do_say(questman, bufsay, 0, 0);
		}
	    } else {
		sprintf(bufsay, "Devi RICHIEDERE la quest prima, %s.", GET_NAME(ch));
		do_say(questman, bufsay, 0, 0);
	    }
										    
	    return;

    }
    
    if (find_questman(ch)==NULL)
	send_to_char("Uso:\r\nautoquest < punti | informazioni | tempo >.\r\n",ch);
    else
	send_to_char("Uso: < punti | informazioni | tempo | richiesta | fine >.\r\n",ch);
}

/* Procedura di generazione della quest */
int generate_quest(struct char_data *ch, struct char_data *questman) {
    struct char_data *questmob = NULL;
    struct obj_data *questitem = NULL;
    room_rnum questroom;
    char bufsay[MAX_STRING_LENGTH];
    int tipo = 0;
    
    tipo = number(1,100);
    if (tipo<22) tipo = QUEST_MOBKILL;
    else if (tipo<44) tipo = QUEST_GETITEM;
    else if (tipo<66) tipo = QUEST_GIVITEM;
    else if (tipo<88) tipo = QUEST_GOTROOM;
    else tipo = QUEST_ARTICOL;

    switch(tipo) {
	case QUEST_MOBKILL:	// Creazione quest di tipo QUEST_MOBKILL
	    questmob = scelta_mob(ch);
	    if (questmob==NULL) {
		mudlog("Autoquest: Ricerca Mob Quest a vuoto. Aggiungere Mob Quest.", NRM, LVL_QUEST_MASTER, FALSE);
		return QUEST_NO;
	    }
	    
	    if ((questroom = scegli_room())==-1) {
		char_to_room(questmob, real_room(PURGE_ROOM));
		extract_char(questmob);
		return QUEST_NO;
	    
	    } else {
		char_to_room(questmob, questroom);
		GET_QUESTMRR(ch) = GET_MOB_VNUM(questmob);
		SET_BIT(MOB_FLAGS(questmob), MOB_QUEST);
	    
		switch(number(0,1)) {
		    case 0:
			sprintf(bufsay, "Un nemico del regno, %s, ha tradito la parola data al Re.", GET_NAME(questmob));
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Il tradimento va pagato col sangue, eliminalo!");
			do_say(questman, bufsay, 0, 0);
			break;
		    case 1:
			sprintf(bufsay, "Un noto criminale, %s, e' scappato di prigione!", GET_NAME(questmob));
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Dalla sua fuga, %s ha ucciso %d civili!", GET_NAME(questmob), number(2,20));
			do_say(questman, bufsay, 0, 0);
			break;
		}
	    
		sprintf(bufsay, "Cerca %s da qualche parte nei pressi di %s!", GET_NAME(questmob), world[questroom].name);
		do_say(questman, bufsay, 0, 0);
		
		sprintf(buf1, "Autoquest: %s ha richiesto una Quest al mob #%d.", GET_NAME(ch), GET_MOB_VNUM(questman));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		sprintf(buf1, "Autoquest: Gli e' stata assegnata una Quest %s con mob #%d.", tipo_quest[tipo], GET_QUESTMRR(ch));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		return QUEST_MOBKILL;
	    }
	    break;

	case QUEST_GETITEM:	// Creazione quest di tipo QUEST_GETITEM
	    questmob = scelta_mob(ch);
	    if (questmob==NULL) {
		sprintf(buf1, "Autoquest: Ricerca Mob Quest a vuoto. Aggiungere Mob Quest per livello %d.", GET_LEVEL(ch));
		mudlog(buf1, NRM, LVL_QUEST_MASTER, FALSE);
		return QUEST_NO;
	    }

	    questitem = scelta_item();
	    if (questitem==NULL) {
		char_to_room(questmob, real_room(PURGE_ROOM));
		extract_char(questmob);
		mudlog("Autoquest: Ricerca Item Quest a vuoto. Aggiungere Item Quest.", NRM, LVL_QUEST_MASTER, FALSE);
		return QUEST_NO;
	    }

	    if ((questroom = scegli_room())==-1) {
		char_to_room(questmob, real_room(PURGE_ROOM));
		extract_char(questmob);
		obj_to_room(questitem, real_room(PURGE_ROOM));
		extract_obj(questitem);
		return QUEST_NO;
	    
	    } else {

		char_to_room(questmob, questroom);
		obj_to_char(questitem, questmob);
		GET_QUESTMRR(ch) = GET_MOB_VNUM(questmob);
		SET_BIT(MOB_FLAGS(questmob), MOB_QUEST);
		GET_QUESTOBJ(ch) = GET_OBJ_VNUM(questitem);
		SET_BIT(GET_OBJ_EXTRA(questitem), ITEM_QUEST);
		
		switch(number(0,1)) {
		    case 0:
			sprintf(bufsay, "Un oggetto unico e di inestimabile valore, %s e' stato rubato dalla Tesoreria Reale!", questitem->short_description);
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Il furto deve passare inosservato, recuperalo!");
			do_say(questman, bufsay, 0, 0);
			break;
		    case 1:
			sprintf(bufsay, "Manca un solo oggetto alla mia collezione, %s!",  questitem->short_description);
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Lo tiene sempre con se' %s, un losco figuro!", GET_NAME(questmob));
			do_say(questman, bufsay, 0, 0);
			break;
		}

		sprintf(bufsay, "Cerca %s da qualche parte nei pressi di %s!", GET_NAME(questmob), world[questroom].name);
		do_say(questman, bufsay, 0, 0);
		
		sprintf(buf1, "Autoquest: %s ha richiesto una Quest al mob #%d.", GET_NAME(ch), GET_MOB_VNUM(questman));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		sprintf(buf1, "Autoquest: Gli e' stata assegnata una Quest %s con mob #%d e obj #%d.", tipo_quest[tipo], GET_QUESTMRR(ch), GET_QUESTOBJ(ch));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		return QUEST_GETITEM;
	    }
	    break;

	case QUEST_GIVITEM:	// Creazione quest di tipo QUEST_GIVITEM
	    questmob = scelta_mob(ch);
	    if (questmob==NULL) {
		sprintf(buf1, "Autoquest: Ricerca Mob Quest a vuoto. Aggiungere Mob Quest per livello %d.", GET_LEVEL(ch));
		mudlog(buf1, NRM, LVL_QUEST_MASTER, FALSE);
		return QUEST_NO;
	    }

	    questitem = scelta_item();
	    if (questitem==NULL) {
		char_to_room(questmob, real_room(PURGE_ROOM));
		extract_char(questmob);
		mudlog("Autoquest: Ricerca Item Quest a vuoto. Aggiungere Item Quest.", NRM, LVL_QUEST_MASTER, FALSE);
		return QUEST_NO;
	    }

	    if ((questroom = scegli_room())==-1) {
		char_to_room(questmob, real_room(PURGE_ROOM));
		extract_char(questmob);
		obj_to_room(questitem, real_room(PURGE_ROOM));
		extract_obj(questitem);
		return QUEST_NO;
	    
	    } else {

		char_to_room(questmob, questroom);
		obj_to_char(questitem, ch);
		GET_QUESTMRR(ch) = GET_MOB_VNUM(questmob);
		SET_BIT(MOB_FLAGS(questmob), MOB_QUEST);
		GET_QUESTOBJ(ch) = GET_OBJ_VNUM(questitem);
		SET_BIT(GET_OBJ_EXTRA(questitem), ITEM_QUEST);
		
		switch(number(0,1)) {
		    case 0:
			sprintf(bufsay, "Ho promesso a %s di fargli avere questo oggetto, %s", GET_NAME(questmob), questitem->short_description);
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Saprei ricompensarti se tu potessi farmi questo favore!");
			do_say(questman, bufsay, 0, 0);
			break;
		    case 1:
			sprintf(bufsay, "Ci sara' una grossa ricompensa per chi portera' questo oggetto a %s!", GET_NAME(questmob));
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Attento pero', %s potrebbe fare gola a molti!", questitem->short_description);
			do_say(questman, bufsay, 0, 0);
			break;
		}

		sprintf(bufsay, "Cerca %s da qualche parte nei pressi di %s!", GET_NAME(questmob), world[questroom].name);
		do_say(questman, bufsay, 0, 0);
		
		sprintf(buf1, "Autoquest: %s ha richiesto una Quest al mob #%d.", GET_NAME(ch), GET_MOB_VNUM(questman));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		sprintf(buf1, "Autoquest: Gli e' stata assegnata una Quest %s con mob #%d e obj #%d.",tipo_quest[tipo], GET_QUESTMRR(ch), GET_QUESTOBJ(ch));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		return QUEST_GIVITEM;
	    }
	    break;

	case QUEST_GOTROOM:	// Creazione quest di tipo QUEST_GOTROOM
	    questitem = scelta_item();
	    if (questitem==NULL) {
		mudlog("Autoquest: Ricerca Item Quest a vuoto. Aggiungere Item Quest.", NRM, LVL_QUEST_MASTER, FALSE);
		return QUEST_NO;
	    }

	    if ((questroom = scegli_room())==-1) {
		obj_to_room(questitem, real_room(PURGE_ROOM));
		extract_obj(questitem);
		return QUEST_NO;
	    
	    } else {

		obj_to_room(questitem, questroom);
		GET_QUESTMRR(ch) = questroom;
		GET_QUESTOBJ(ch) = GET_OBJ_VNUM(questitem);
		SET_BIT(GET_OBJ_EXTRA(questitem), ITEM_QUEST);

		switch(number(0,1)) {
		    case 0:
			sprintf(bufsay, "Un antico tesoro, %s, riposa da secoli in terre lontane.", questitem->short_description);
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Portamelo e la tua ricompensa sara' molto sostanzionsa");
			do_say(questman, bufsay, 0, 0);
			break;
		    case 1:
			sprintf(bufsay, "Ho dovuto abbandonare %s fuggendo dalla mia patria.", questitem->short_description);
			do_say(questman, bufsay, 0, 0);
			sprintf(bufsay, "Ah! Fossi ancora giovane come te per poter tornare a recuperarlo!");
			do_say(questman, bufsay, 0, 0);
			break;
		}

		sprintf(bufsay, "Cerca %s da qualche parte nei pressi di %s!", questitem->short_description, world[questroom].name);
		do_say(questman, bufsay, 0, 0);
		sprintf(buf,"%d",questroom);
		mudlog(buf,NRM,LVL_GOD,FALSE);
		
		sprintf(buf1, "Autoquest: %s ha richiesto una Quest al mob #%d.", GET_NAME(ch), GET_MOB_VNUM(questman));
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		sprintf(buf1, "Autoquest: Gli e' stata assegnata una Quest %s con obj #%d nei pressi di %s.", tipo_quest[tipo], GET_QUESTOBJ(ch), world[questroom].name);
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		return QUEST_GOTROOM;
	    }
	    break;

	case QUEST_ARTICOL:	// Creazione quest di tipo QUEST_ARTICOL
	    mudlog("Quest Articolata da Fare!", NRM, LVL_BUILDER, FALSE);
	    return QUEST_NO;
	    break;
    }

    return tipo;
}

/* Reset dei flag in caso di crash */
void flag_crash_reset(struct char_data *ch, int c) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_QUESTOR);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_QUEST);
    GET_TIPOQUEST(ch) = QUEST_NO;
    
    if (c) GET_COUNTDOWN(ch) = 0;
    else GET_COUNTDOWN(ch) = attesa;
    
    GET_QUESTMRR(ch) = 0;
    GET_QUESTOBJ(ch) = 0;
    GET_QUESTGIVER(ch) = NULL;
    if (c) send_to_char("&2Adesso puoi affrontare un'altra quest.&0\r\n",ch);
}

/* Aggiornamento periodico del tempo residuo di quest o attesa */
/* Controlla inoltre che non si sia verificato un crash, nel qual caso elimina i residui */
void quest_update() {
    struct char_data *ch, *questmob = NULL;
    struct obj_data *questitem = NULL;

    for (ch=character_list;ch;ch=ch->next) {
	if (!ch->desc) continue;
	if (IS_NPC(ch)) continue;
	
	if ((GET_COUNTDOWN(ch)==1) && (!(IS_QUESTOR(ch)))) {
	    GET_COUNTDOWN(ch)--;
	    send_to_char("&2Adesso puoi affrontare un'altra quest.&0\r\n",ch);
	}
	
	if (GET_COUNTDOWN(ch)>0)
	    GET_COUNTDOWN(ch)--;
	    
	if (IS_QUESTOR(ch)) {

	    if ((GET_COUNTDOWN(ch)>0) && (GET_COUNTDOWN(ch)<6))
		send_to_char("&1SBRIGATI, il tempo per finire la quest sta per terminare!&0\r\n", ch);
		
	    if (GET_COUNTDOWN(ch)==0) {
		sprintf(buf1,"Autoquest: %s ha finito il tempo a disposizione per la quest %s.", GET_NAME(ch), tipo_quest[GET_TIPOQUEST(ch)]);
		mudlog(buf1, NRM, LVL_IMMORT, FALSE);
		
		//reset tempo di attesa per la nuova quest
		GET_COUNTDOWN(ch) = attesa;
		sprintf(buf, "Hai finito il tempo a tua disposizione!\r\nPotrai affrontare un'altra quest fra %d minuti.\r\n", GET_COUNTDOWN(ch));
		send_to_char(buf, ch);
		
		sprintf(buf1,"%s non ha terminato la sua quest in tempo utile!", GET_NAME(ch));
		do_qcomm(ch,buf1,0,SCMD_QECHO);
		
		//1)eliminazione mob e/o obj
		if ((GET_TIPOQUEST(ch)==QUEST_MOBKILL) || (GET_TIPOQUEST(ch)==QUEST_GETITEM) || (GET_TIPOQUEST(ch)==QUEST_GIVITEM)) {
		    if ((GET_QUESTMRR(ch)>0) && ((questmob=get_char_num(real_mobile(GET_QUESTMRR(ch))))!=NULL)) {
			char_from_room(questmob);
			char_to_room(questmob, real_room(PURGE_ROOM));
			extract_char(questmob);
		    }
		}
		if ((GET_TIPOQUEST(ch)==QUEST_GETITEM) || (GET_TIPOQUEST(ch)==QUEST_GIVITEM) || (GET_TIPOQUEST(ch)==QUEST_GOTROOM)) {
		    if ((GET_QUESTOBJ(ch)>0) && ((questitem=get_obj_num(real_object(GET_QUESTOBJ(ch))))!=NULL)) {
			if (questitem->carried_by!=NULL) obj_from_char(questitem);
			else if (questitem->in_obj!=NULL) obj_from_obj(questitem);
			else obj_from_room(questitem);
			obj_to_room(questitem, real_room(PURGE_ROOM));
			extract_obj(questitem);
		    }
		}

		//2)reset flag
		REMOVE_BIT(PLR_FLAGS(ch), PLR_QUESTOR);
		REMOVE_BIT(PRF_FLAGS(ch), PRF_QUEST);
		GET_TIPOQUEST(ch) = QUEST_NO;
		GET_QUESTMRR(ch) = 0;
		GET_QUESTOBJ(ch) = 0;
		GET_QUESTGIVER(ch) = NULL;
	    
	    } else {
		switch(GET_TIPOQUEST(ch)) {
		    case QUEST_MOBKILL:
			if ((GET_QUESTMRR(ch)>0) && (get_char_num(real_mobile(GET_QUESTMRR(ch)))==NULL))
			    flag_crash_reset(ch, TRUE);
			break;
		    case QUEST_GETITEM:
		    case QUEST_GIVITEM:
			if (((GET_QUESTMRR(ch)>0) && ((questmob=get_char_num(real_mobile(GET_QUESTMRR(ch))))==NULL)) || \
			 ((GET_QUESTOBJ(ch)>0) && ((questitem=get_obj_num(real_object(GET_QUESTOBJ(ch))))==NULL))) {
			    if ((GET_QUESTMRR(ch)>0) && (questmob!=NULL)) {
				char_from_room(questmob);
				char_to_room(questmob, real_room(PURGE_ROOM));
				extract_char(questmob);
			    }

			    if ((GET_QUESTOBJ(ch)>0) && (questitem=get_obj_num(real_object(GET_QUESTOBJ(ch))))) {
				if (questitem->carried_by!=NULL) obj_from_char(questitem);
				else if (questitem->in_obj!=NULL) obj_from_obj(questitem);
				else obj_from_room(questitem);
				obj_to_room(questitem, real_room(PURGE_ROOM));
				extract_obj(questitem);
			    }
			    flag_crash_reset(ch, TRUE);
			}
			break;
		    case QUEST_GOTROOM:
			if ((GET_QUESTOBJ(ch)>0) && (get_obj_num(real_object(GET_QUESTOBJ(ch)))==NULL))
			    flag_crash_reset(ch, TRUE);
			break;
		}		
	    }
	}
    }
}
