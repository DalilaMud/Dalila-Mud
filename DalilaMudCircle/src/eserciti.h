/********************************************************************************************/
/* File : eserciti.h                                                                         */
/*                                                                                           */
/* Header e dichiarazioni varie per gli eserciti                                             */
/*                             Autore: Ardanos                                               */
/*                                                                                           */
/* Copyright (C) 2004 by Dalila 2 Staff                                                      */
/* Questo file e' di proprieta' esclusiva del team di Dalila 2 Mud. Qualsiasi utilizzo       */
/* non autorizzato di questo e' da considerarsi violazione delle norme sul Copyright         */
/*********************************************************************************************/

/**** Appunti sugli eserciti
Si dividono in tipi:
fanteria, truppe standard
cavalleria, elite
scout: truppe veloci ma poco potenti
falangi: niente attacco, tanta difesa
assalitori: niente difesa, molto attacco 

All'inizio, si ha un numero uguale per tutti di armate
Poi l'arruolamento di nuove truppe avverra' tramite il deposito di X quantitativi di ferro, di legno, di cibo, etc.

Ogni unita' avra' un valore di
Movimento
Attacco
Difesa

Per aumentare un valore, o paghi, oppure cresce di 1 se sopravvive ad una battaglia, con dei max dato dal tipo 
di armata (es: una falange non potra' raggiungere i 10/10 in attacco)

Movimento tramite landmark, piazzati da qualche parte, in cui salvare le coordinate e la direzione da prendere

Mettere nella struttura esercito un campo id per distinguere le truppe con specifiche e landmark diversi

*/


#define IS_ESERCITO(ch)		MOB_FLAGGED(ch, MOB_ESERCITO) 
#define IS_GENERALE(ch, e)	(GET_IDNUM(ch) == GET_GENERALE(e))
#define GET_ESERCITO(ch)	((ch)->mob_specials.esercito)
#define GET_GENERALE(ch)	((ch)->mob_specials.esercito->generale)
#define GET_SOLDATI(ch)		((ch)->mob_specials.esercito->numero_soldati)
#define GET_FRONTE(ch)		((ch)->mob_specials.esercito->fronte)
#define GET_MARCIA(ch)		((ch)->mob_specials.esercito->marcia)
#define GET_TIPO(ch)		((ch)->mob_specials.esercito->tipo)
#define GET_SCHIERAMENTO(ch)	((ch)->mob_specials.esercito->schieramento)
#define GET_MOVIMENTO(ch)	((ch)->mob_specials.esercito->movimento)
#define GET_ATTACCO(ch)		((ch)->mob_specials.esercito->attacco)
#define GET_DIFESA(ch)		((ch)->mob_specials.esercito->difesa)
//#define GET_ID_ARMATA(ch)	((ch)->mob_specials.esercito->id)

#define FRONTE_NULL     0
#define FRONTE_NORD	1
#define FRONTE_SUD	2
#define FRONTE_EST	3
#define FRONTE_OVEST	4

#define MAX_TIPI_FRONTE 4

#define MARCIA_OFF	0
#define MARCIA_NORMALE	1

#define TIPO_NULL	0
#define TIPO_FANTERIA	1
#define TIPO_CAVALLERIA	2
#define TIPO_SCOUT	3
#define TIPO_FALANGI	4
#define TIPO_ASSALITORI	5

#define MAX_TIPI_ESERCITO 5

#define SCHIERAMENTO_NULL		0
#define SCHIERAMENTO_DIFESA_SCHIERATA	1
#define SCHIERAMENTO_ATTACCO_SPIANATO	2
#define SCHIERAMENTO_ATTACCO_SUI_LATI	3
#define SCHIERAMENTO_CONVESSO		4
#define SCHIERAMENTO_ACCERCHIAMENTO	5
#define SCHIERAMENTO_RETROGUARDIA	6

#define MAX_TIPI_SCHIERAMENTO 6

struct dati_esercito {
	long generale; // id del generale
	int numero_soldati;
	int fronte;   // fronte di marcia
	int marcia;   // indica il tipo di marcia
	int tipo;     // indica il tipo di armata
	int schieramento; // tipo di schieramento delle truppe
	// Ecco i tre valori che caratterizzano l'armata
	int movimento;
	int attacco;
	int difesa;
	
	int timer; // timer per i movimenti. Non vine salvato su file
	
//	int id; //identificativo dell'armata. Deve essere un numero unico
};

void clear_esercito(struct char_data *mob);
void boot_eserciti();
