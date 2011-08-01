/********************************************************************************************/
/* File : eserciti.c                                                                         */
/*                                                                                           */
/* Spec-proc e funzioni degli eserciti                                                       */
/*                             Autore: Ardanos                                               */
/*                                                                                           */
/* Copyright (C) 2004 by Dalila 2 Staff                                                      */
/* Questo file e' di proprieta' esclusiva del team di Dalila 2 Mud. Qualsiasi utilizzo       */
/* non autorizzato di questo e' da considerarsi violazione delle norme sul Copyright         */
/*********************************************************************************************/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "clan.h"

#include "eserciti.h"


#define FILE_ESERCITI "etc/eserciti"

extern struct player_index_element *player_table;
extern int top_of_p_table;
extern struct index_data *mob_index;
extern struct room_data *world;
extern struct char_data *character_list;
extern void char_to_room(struct char_data * ch, int room);

ACMD(do_say);
ACMD(do_move);


const char *fronte [] = {
	"!ERRORE!",
	"Nord",
	"Sud",
	"Est",
	"Ovest"
};

const char *marcia [] = {
	"Fermo",
	"In marcia"
};

const char *tipo[] = {
	"!ERRORE!",
	"Fanteria",
	"Cavalleria",
	"Scout",
	"Falangi",
	"Assalitori"
};

const char *schieramento[] = {
	"!ERRORE!",
	"Difesa schierata",
	"Attacco spianato",
	"Attacco sui lati",
	"Schieramento Convesso",
	"Accerchiamento",
	"Con retroguardia"
};

/* sono le stringhe che vengono visualizzate quando si cambia schieramento */

const char *act_schieramento[] = {
	"!ERRORE!",
	"si dispone con la difesa schierata.",
	"si prepara per un attacco frontale.",
	"si prepara per attaccare sui lati.",
	"si dispone con uno schieramento convesso.",
	"si prepara ad accerchiare il nemico.",
	"prepara una retroguardia per la propria difesa."
};

/* Funzione che al load dell'esercito azzera tutti i dati */

void clear_esercito(struct char_data *mob) {
	GET_GENERALE(mob) = -1;
	GET_SOLDATI(mob) = 1;
	GET_FRONTE(mob) = FRONTE_NORD;
	GET_MARCIA(mob) = MARCIA_OFF;
	GET_TIPO(mob) = TIPO_NULL;
	GET_SCHIERAMENTO(mob) = SCHIERAMENTO_NULL;
	GET_MOVIMENTO(mob) = 0;
	GET_ATTACCO(mob) = 0;
	GET_DIFESA(mob) = 0;
//	GET_ID_ARMATA(mob) = 0;
}

/* Questa sara' la risposta al comando esercito quando l'esercito non c'e' */

ACMD(do_esercito) {
	send_to_char("Non vedi nessun esercito davanti a te!\r\n", ch);
}

char *get_nome_generale(long id) {
	int i;
	
	for (i = 0; i <= top_of_p_table; i++)
		if ((player_table + i)->id == id)
			return ((player_table + i)->name);
	
	return NULL;
}

/****** formato del file di salvataggio *********
--------------------------------------------------
#<vnum esercito> <vnum stanza>
<id generale> <numero soldati> <tipo>
<fronte> <schieramento>
<movimento> <attacco> <difesa>
~
#<vnum esercito> <vnum stanza>
<id generale> <numero soldati> <tipo>
<fronte> <schieramento> 
<movimento> <attacco> <difesa>
...
...
...
$
---------------------------------------------------*/

/* La marcia (attiva oppure off) non viene salvata di proposito */

/* Funzione per il salvataggio su file di tutti gli eserciti*/
int salva_eserciti() {
	FILE *file;
	char file_temporaneo[64];
	struct char_data *e;
	
	sprintf(file_temporaneo, "%s.temp", FILE_ESERCITI);
	
	if (!(file = fopen(file_temporaneo, "w"))) {
		mudlog("Non riesco ad aprire il file di salvataggio degli eserciti!", BRF, LVL_QUEST_MASTER, TRUE);
		return 0;
	}
	
	
	for(e = character_list; e; e = e->next) {
		if(IS_ESERCITO(e)) {
			if(!GET_ESERCITO(e)) { //se manca la struttura esercito...e' un bel guaio
				sprintf(buf, "L'esercito %d nella room %d e' mancante della struttura Esercito!", 
				GET_MOB_VNUM(e), world[IN_ROOM(e)].number);
				mudlog(buf, BRF, LVL_QUEST_MASTER, TRUE);
				return 0;
			} else {
				fprintf(file, "#%d %d\n", GET_MOB_VNUM(e), world[IN_ROOM(e)].number);
				fprintf(file, "%ld %d %d\n", GET_GENERALE(e), GET_SOLDATI(e), GET_TIPO(e));
				fprintf(file, "%d %d\n", GET_FRONTE(e), GET_SCHIERAMENTO(e));
				fprintf(file, "%d %d %d\n", GET_MOVIMENTO(e), GET_ATTACCO(e), GET_DIFESA(e));
				fprintf(file, "~\n");
			}
		}
	}	
	fprintf(file, "$\n"); //fine del file
	fclose(file);
	
	remove(FILE_ESERCITI);
	rename(file_temporaneo, FILE_ESERCITI);
	
	return 1;
}


void boot_eserciti() {

	FILE *file;
	mob_vnum vnum;
	room_vnum load_room;
	struct char_data *e;
	char line[256];
	
	if (!(file = fopen(FILE_ESERCITI, "r"))) {
		mudlog("Non riesco ad aprire il file degli eserciti per il boot!", BRF, LVL_QUEST_MASTER, TRUE);
		return;
	}
	
	
	for(;;) {
		get_line(file, line);
		
		if (*line == '$') // fine del file
			break;
		
		if (*line == '#') { //ecco un esercito
			sscanf(line, "#%d %d",  &vnum, &load_room);
			e = read_mobile(vnum, VIRTUAL);
			
			get_line(file, line);
			sscanf(line, "%ld %d %d", &GET_GENERALE(e), &GET_SOLDATI(e),
			&GET_TIPO(e));

			get_line(file, line);
			sscanf(line, "%d %d", &GET_FRONTE(e), &GET_SCHIERAMENTO(e));
			
			get_line(file, line);
			sscanf(line, "%d %d %d", &GET_MOVIMENTO(e), &GET_ATTACCO(e), 
			&GET_DIFESA(e));

			char_to_room(e, real_room(load_room));
		}
		
		if (*line == '~') 
			continue;
	}
	
	fclose(file);

}

void mostra_esercito(struct char_data *ch, struct char_data *e) {
	if(GET_ESERCITO(e) == NULL) { 
		/* Manca la struttura con i dati dell'esercito! PERICOLO! */
		sprintf(buf, "mostra_esercito: mob %d e' privo della struttura dati_esercito!", GET_MOB_VNUM(e));
		mudlog(buf, BRF, LVL_GOD, TRUE);
		return;
	}
	
	*buf1 = '\0';
	*buf= '\0';
	
	/* Cosi' dovrebbe esser allineato.... */
	
	send_to_char("Dati relativa all'armata:\r\n", ch);
	send_to_char("&1----------------------------------------------------&0\r\n", ch);
	
	sprintf(buf1, "&2Generale:			%20s&0\r\n", 
	get_nome_generale(GET_GENERALE(e)) ? CAP(get_nome_generale(GET_GENERALE(e))) : "Non Assegnato");
	strcat(buf, buf1);
	sprintf(buf1, "&2Soldati:			%20d&0\r\n", GET_SOLDATI(e));
	strcat(buf, buf1);
	sprintf(buf1, "&2Fronte di marcia:		%20s&0\r\n", fronte[(int)GET_FRONTE(e)]);
	strcat(buf, buf1);
	sprintf(buf1, "&2Stato della marcia:		%20s&0\r\n", marcia[(int)GET_MARCIA(e)]);
	strcat(buf, buf1);
	sprintf(buf1, "&2Tipo di armata:			%20s&0\r\n", tipo[(int)GET_TIPO(e)]);
	strcat(buf, buf1);
	sprintf(buf1, "&2Schieramento:		%28s&0\r\n", schieramento[(int)GET_SCHIERAMENTO(e)]);
	strcat(buf, buf1);
	
	if(GET_LEVEL(ch) >= LVL_QUEST_MASTER) {
		sprintf(buf1, "&4&8Attacco:			%20d&0\r\n", GET_ATTACCO(e));                
		strcat(buf, buf1);
		sprintf(buf1, "&4&8Difesa:				%20d&0\r\n", GET_DIFESA(e));
		strcat(buf, buf1);
		sprintf(buf1, "&4&8Movimento:			%20d&0\r\n", GET_MOVIMENTO(e));
		strcat(buf, buf1);
//		sprintf(buf1, "&4&8Id:				%20d&0\r\n", GET_ID_ARMATA(e));
//		strcat(buf, buf1);
	}
	
	send_to_char(buf, ch);
	send_to_char("&1----------------------------------------------------&0\r\n", ch);

}

/* Per ora e' richiamata ogni 2 secondi. Per i test */

void movimento_eserciti() {
	struct char_data *e;
	bool marcia_ok = TRUE;
	

	for(e = character_list; e; e = e->next) {
		if(IS_ESERCITO(e)) {
			if(GET_MARCIA(e)) {
				marcia_ok = TRUE;
				if(GET_FRONTE(e) == FRONTE_NORD) {
					if(!perform_move(e, SCMD_NORTH -1 , 0, CAN_SHOW_ROOM)) 
						marcia_ok = FALSE;
				} else if(GET_FRONTE(e) == FRONTE_SUD) {
					if(!perform_move(e, SCMD_SOUTH -1 , 0, CAN_SHOW_ROOM)) 
						marcia_ok = FALSE;
				} else if(GET_FRONTE(e) == FRONTE_EST) {
					if(!perform_move(e, SCMD_EAST -1 , 0, CAN_SHOW_ROOM)) 
						marcia_ok = FALSE;
				} else if(GET_FRONTE(e) == FRONTE_OVEST) {
					if(!perform_move(e, SCMD_WEST - 1, 0, CAN_SHOW_ROOM)) 
						marcia_ok = FALSE;
				}
				if(!marcia_ok) {// l'esercito ha trovato un ostacolo o un impedimento. Fermare la marcia
					act("$n arresta la sua marcia davanti all'ostacolo", TRUE, e, NULL, NULL, TO_ROOM);
					GET_MARCIA(e) = FALSE;
				}
			}
		}
	}
	
};

int comando_esercito(struct char_data *ch, struct char_data *e, int cmd, char *argument) {
        char arg1[MAX_STRING_LENGTH],arg2[MAX_STRING_LENGTH],buffer[MAX_STRING_LENGTH];
	long id_generale;
	int argomento_numerico;
	bool ch_immortale = ((GET_LEVEL(ch) < LVL_QUEST_MASTER) ? FALSE : TRUE);
	bool rank_ok = (GET_CLAN_RANK(ch) > 25);

	if(IS_NPC(ch)) {
		send_to_char("I mob non possono giocare con gli eserciti..\r\n", ch);
		return 1;
	}
	
	if((CLAN_ID(e) != GET_CLAN(ch)) && !ch_immortale) { // se si e' di clan diversi
		send_to_char("Questo esercito non ti ascoltera' MAI!\r\n", ch);
		return 1;
	}
	
	if(!strcmp(argument, "")) {
		send_to_char("Ecco le azioni che puoi far fare a un esercito\r\n", ch);
		if(rank_ok || ch_immortale) 
			send_to_char("Esercito generale <nomepg>        : assegna un nuovo comandante all'armata\r\n", ch);
		/*Ecco i comandi per il generale*/
		if(GET_IDNUM(ch) == GET_GENERALE(e) || ch_immortale || rank_ok) {
			send_to_char("Esercito schieramento <tipo>      : determina lo schieramento dell'armata\r\n", ch);
			send_to_char("Esercito fronte <direzione>       : determina la direzione di marcia\r\n", ch);
			send_to_char("Esercito marcia                   : inizia la marcia verso la direzione del fronte\r\n", ch);
			send_to_char("Esercito alt                      : ferma la marcia\r\n", ch);
		}
			
		/* Comandi solo per immortali */
		if(ch_immortale) {
			send_to_char("\r\n------ [ &2Comandi da immortale per la gestione degli eserciti&0 ] ----------\r\n\r\n", ch);
			send_to_char("Esercito numero_soldati <numero>  : setta il numero di soldati dell'armata\r\n", ch);
			send_to_char("Esercito tipo <numero>            : setta il tipo dell'armata\r\n", ch);
			send_to_char("Esercito attacco <numero>         : setta la potenza di attacco\r\n", ch);
			send_to_char("Esercito difesa <numero>          : setta la difesa\r\n", ch);
			send_to_char("Esercito movimento <numero>       : setta il movimento\r\n", ch);
			send_to_char("Esercito salva                    : salva lo stato di tutte le armate\r\n", ch);
		}
		return 1;
	}
	
	two_arguments(argument, arg1, arg2);
	
	/* Assegnamento di un comandante */
	/* L'assegnamento di un comandante puo' esser fatto da uno con rank >= 25, oppure da
	 * un immortale. Tutti gli altri comandi invece verranno utilizzati anche dal generale, qualunque sia il suo rank */
	 
	if(!strcmp(arg1, "generale") && (rank_ok || ch_immortale)) {
		if(!strcmp(arg2, "")) {
			send_to_char("Manca il nome del generale\r\n", ch);
			return 1;
		}
		
		id_generale = get_id_by_name(arg2);
		if(id_generale == -1) 
			send_to_char("Non esiste alcuna persona con questo nome\r\n", ch);
		else {
			GET_GENERALE(e) = id_generale;
			sprintf(buffer, "Il nuovo generale sara' %s\r\n", CAP(arg2));
			send_to_char(buffer, ch);
			sprintf(buffer, "ARMATA!Da oggi in poi obbedirete al generale %s!", CAP(arg2));
			do_say(ch, buffer, find_command("say"), 0);
			do_say(e, "AGLI ORDINI!", find_command("say"), 0);
		}
		return 1;
	}
	
	/* Assegnamento dello schieramento */
	if(!strcmp(arg1, "schieramento") && (rank_ok || ch_immortale)) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Inserisci il numero relativo al tipo di schieramento:\r\n"
			"1)Difesa schierata\r\n"
			"2)Attacco spianato\r\n"
			"3)Attacco sui lati\r\n"
			"4)Schieramento Convesso\r\n"
			"5)Accerchiamento\r\n"
			"6)Con retroguardia\r\n" ,ch);
			return 1;
		}
		argomento_numerico = atoi(arg2);
		if(argomento_numerico < 1 || argomento_numerico > MAX_TIPI_SCHIERAMENTO) {
			send_to_char("Opzione non valida\r\n", ch);
		} else {
			GET_SCHIERAMENTO(e) = argomento_numerico;
			sprintf(buffer, "$n %s", act_schieramento[GET_SCHIERAMENTO(e)]);
			act(buffer, TRUE, e, NULL, NULL, TO_ROOM);
		}
		return 1;
	}
	
	/* Assegnamento del fronte */
	if(!strcmp(arg1, "fronte") && (rank_ok || ch_immortale)) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Inserisci il numero relativo alla direzione di marcia:\r\n"
			"1)Nord\r\n"
			"2)Sud\r\n"
			"3)Est\r\n"
			"4)Ovest\r\n" ,ch);
			return 1;
		}
		argomento_numerico = atoi(arg2);
		if(argomento_numerico < 1 || argomento_numerico > MAX_TIPI_FRONTE) {
			send_to_char("Opzione non valida\r\n", ch);
		} else {
			GET_FRONTE(e) = argomento_numerico;
			sprintf(buffer, "$n si schiera verso %s", fronte[GET_FRONTE(e)]);
			act(buffer, TRUE, e, NULL, NULL, TO_ROOM);
		}
		return 1;
	}
	
	if(!strcmp(arg1, "marcia") && (rank_ok || ch_immortale)) {
		GET_MARCIA(e) = TRUE;
		sprintf(buffer, "$n inizia a marciare verso %s", fronte[GET_FRONTE(e)]);
		act(buffer, TRUE, e, NULL, NULL, TO_ROOM);
		return 1;
	}
	
	if(!strcmp(arg1, "alt") && (rank_ok || ch_immortale)) {
		GET_MARCIA(e) = FALSE;
		act("$n ferma la sua marcia.", TRUE, e, NULL, NULL, TO_ROOM);
		return 1;
	}
	
	
	/* Assegnamento del numero di soldati dell'armata. Comando solo per immortali */
	if(!strcmp(arg1, "numero_soldati") && ch_immortale) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Manca il numero di soldato\r\n", ch);
			return 1;
		}
		
		argomento_numerico = atoi(arg2);
		GET_SOLDATI(e) = argomento_numerico;
		sprintf(buffer, "L'armata ora ha %d soldati\r\n", argomento_numerico);
		send_to_char(buffer, ch);
		return 1;
	}
	
	/* Assegnamento del tipo di Armata. Comando solo per Immortali */
	if(!strcmp(arg1, "tipo") && ch_immortale) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Inserisci il numero relativo al tipo di armata:\r\n"
			"1) Fanteria\r\n"
			"2) Cavalleria\r\n"
			"3) Scout\r\n"
			"4) Falangi\r\n"
			"5) Assalitori\r\n", ch);
			return 1;
		}

		
		argomento_numerico = atoi(arg2);
		if(argomento_numerico < 1 || argomento_numerico > MAX_TIPI_ESERCITO) {
			send_to_char("Opzione non valida\r\n", ch);
		} else {
			GET_TIPO(e) = argomento_numerico;
			sprintf(buffer, "Settato tipo %s\r\n", tipo[(int)GET_TIPO(e)]);
			send_to_char(buffer, ch);
		}
		return 1;
	}
	
	
	/* Assegnamento della potenza di attacco */
	if(!strcmp(arg1, "attacco") && ch_immortale) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Devi inserire un numero (il limite MAX non e' ancora stato deciso)\r\n", ch);
			return 1;
		}

		argomento_numerico = atoi(arg2);
		GET_ATTACCO(e) = argomento_numerico;
		sprintf(buffer, "Potenza di attacco = %d\r\n", GET_ATTACCO(e));
		send_to_char(buffer, ch);
		return 1;
	}
	
	/* Assegnamento della potenza di difesa */
	if(!strcmp(arg1, "difesa") && ch_immortale) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Devi inserire un numero (il limite MAX non e' ancora stato deciso)\r\n", ch);
			return 1;
		}

		argomento_numerico = atoi(arg2);
		GET_DIFESA(e) = argomento_numerico;
		sprintf(buffer, "Potenza di difesa = %d\r\n", GET_DIFESA(e));
		send_to_char(buffer, ch);
		return 1;
	}
	
	/* Assegnamento del movimento */
	if(!strcmp(arg1, "movimento") && ch_immortale) {
		if(!strcmp(arg2, "") || !is_number(arg2)) {
			send_to_char("Devi inserire un numero (il limite MAX non e' ancora stato deciso)\r\n", ch);
			return 1;
		}

		argomento_numerico = atoi(arg2);
		GET_MOVIMENTO(e) = argomento_numerico;
		sprintf(buffer, "Movimento = %d\r\n", GET_MOVIMENTO(e));
		send_to_char(buffer, ch);
		return 1;
	}
	
	/* Assegnamento del movimento */
	if(!strcmp(arg1, "salva") && ch_immortale) {
		send_to_char("Salvataggio delle armate in corso...\r\n", ch);
		if(salva_eserciti()) 
			send_to_char("Salvataggio delle armate completato!\r\n", ch);
		else
			send_to_char("ERRORE!!!\r\n", ch);
		return 1;
	}
		
		
	
	/* qui finisce se il pg non puo' dare comandi all'esercito */
	else {
		send_to_char("Non provare a fare il furbo....\r\n", ch);
		return 1;
	}
	
	return 0;
}

SPECIAL(esercito) {

	if (CMD_IS("esercito")) {
		return(comando_esercito(ch, (struct char_data *) me, cmd, argument));
	}
	
	return 0;
}
