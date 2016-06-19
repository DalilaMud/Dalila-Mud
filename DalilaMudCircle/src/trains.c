/* ************************************************************************
*   File: trains.c                                                        *
*  Usage: Transporter handling functions for CircleMUD                    *
*     By: Carl Tashian [Guru Meditation]                                  *
*         [tashiacm@ctrvax.vanderbilt.edu]                                *
************************************************************************ */


#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"

/*   external vars  */
extern struct room_data *world;
int transpath = 0;
char curstat1[100], curstat2[100];

void enter_stat(int station, char wherefrom[100], int which);
void leave_stat(int station, char whereto[100], int which);

void train_upd(void)
{
int wdoor, edoor, station[10];
station[0] = real_room(3074); /* room of Deira Station */
station[1] = real_room(5694); /* room of Jiroch Station */
station[2] = real_room(6788); /* room of Lamorka Station */
station[3] = real_room(21121); /* room of Chyrellos Station */
station[4] = real_room(9123); /* room of Cimmura station */
station[5] = real_room(3206); /* room of Deira station */
station[6] = real_room(260); /* room of Kerofk station */
station[7] = real_room(6789); /* room of Lamorka station */
station[8] = real_room(12081); /* room of Roma station */
station[9] = real_room(5695); /* room of Jiroch station */
wdoor = 3; /* west dir number */
edoor = 1; /* east dir number */
switch (transpath)
{
case 0:
   /* leave from VVVVVV to  VVVVVV */
   leave_stat(station[0], "Stazione di Jiroch", 1);
   leave_stat(station[6], "Stazione orientale di Lamorka", 2);
   break;
case 1:
   /* enter station VVVV from VVVV */
   enter_stat(station[1], "Stazione di Deira", 1);
   enter_stat(station[7], "Stazione di Kerofk", 2);
   break;
case 2:
   leave_stat(station[1], "Stazione di Lamorka", 1);
   leave_stat(station[7], "Stazione di Roma", 2);
   break;
case 3:
   enter_stat(station[2], "Stazione di Jiroch", 1);
   enter_stat(station[8], "Stazione orientale di Lamorka", 2);
   break;
case 4:
   leave_stat(station[2], "Stazione di Chyrellos", 1);
   leave_stat(station[8], "Stazione orientale di Jiroch", 2);
   break;
case 5:
   enter_stat(station[3], "Stazione di Lamorka", 1);
   enter_stat(station[9], "Stazione di Roma", 2);
   break;
case 6:
   leave_stat(station[3], "Stazione di Cimmura", 1);
   leave_stat(station[9], "Stazione meridionale di Deira", 2);
   break;
case 7:
   enter_stat(station[4], "Stazione di Chyrellos", 1);
   enter_stat(station[5], "Stazione orientale di Jiroch", 2);
   break;
case 8:
   leave_stat(station[4], "Stazione di Deira", 1);
   leave_stat(station[5], "Stazione di Kerofk", 2);
   break;
case 9:
   enter_stat(station[0], "Stazione di Cimmura", 1);
   enter_stat(station[6], "Stazione meridionale di Deira", 2);
   break;
/* then back to the case 0 again.. */
default:
   log("SYSERR: Big problem with the transporter!");
   break;
} /* switch */
transpath++;
if (transpath==10) transpath=0;
return;
} /* funct */

void enter_stat(int station, char wherefrom[100],
                int which)
{
int transroom=0, wdoor, edoor;
char buf[MAX_STRING_LENGTH];
char whereat[100];
if (which == 1) {
  transroom = real_room(499); /* this MUST be the transporter itself (room) */
  strcpy(whereat, curstat1);
  }
if (which == 2) {
  strcpy(whereat, curstat2);
  transroom = real_room(498); /* transporter room 2 */
  }
wdoor = 3; /* west dir number */
edoor = 1; /* east dir number */
world[transroom].dir_option[wdoor]->to_room = station;
world[station].dir_option[edoor]->to_room = transroom;
send_to_room("Il cocchiere rallenta fino a fermarsi, puoi scendere ad ovest.\n\r", transroom);
sprintf(buf, "Il cocchiere annuncia, 'Benvenuti alla %s'\n\r", whereat);
send_to_room(buf, transroom);
sprintf(buf, "In un batter d'occhio, la carrozza arriva in stazione.\n\r");
send_to_room(buf,station);
send_to_room("...e la porta ad est si apre.\n\r", station);
return;
}

void leave_stat(int station, char whereto[100], int which)
{
int transroom=0, wdoor, edoor;
char buf[MAX_STRING_LENGTH];

if (which == 1) {
  strcpy(curstat1, whereto);
  transroom = real_room(499); /* this MUST be the transporter itself (room) */
  }
if (which == 2) {
  strcpy(curstat2, whereto);
  transroom = real_room(498); /* train 2 */
  }
wdoor = 3; /* west dir number */
edoor = 1; /* east dir number */

world[transroom].dir_option[wdoor]->to_room = NOWHERE;
world[station].dir_option[edoor]->to_room = NOWHERE;
sprintf(buf, "Il cocchiere annuncia, 'Prossima fermata: %s'\n\r", whereto);
send_to_room(buf, transroom);
send_to_room("Il cocchiere chiude la porta, e la carrozza inizia il viaggio.\n\r", transroom);
send_to_room("Il cocchiere chiude la porta e la carrozza scompare all'orrizonte.\n\r", station);
return;
} /* move_from */




