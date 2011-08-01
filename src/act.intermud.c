
/************************************************************************
* File: act.intermud.c                                                  *
*                                                                       *
* Intermud interface code to CircleMUD                                  *
*                                                                       *
* All rights reserved.  See /doc/license.doc for more information       *
*                                                                       *
* Original version:                                                     *
* Copyright (C) 1995, 96 by Chris Austin (Stryker@Tempus)               *
* Adapted version for Realm of Magic Intermud:                          *
* Copyright (C) 1997, 98 by Christian Loth (Christian@RealmOfMagic)     *
* CircleMUD (C) 1993, 94 by the trustees of Johns Hopkins University.   *
* CircleMUD is based on DikuMUD, which is Copyright (C) 1990, 1991.     *
*                                                                       *
************************************************************************/

#ifdef OLD_CIRCLE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/un.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#else
#include "conf.h"
#include "sysdep.h"
#endif

#ifdef CIRCLE_WINDOWS		/* Includes for Win32 */
#include <direct.h>
#include <mmsystem.h>
#else				/* Includes for UNIX */
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#endif


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "handler.h"
#include "intermud.hh"
#include "screen.h"

/* Structures */

struct bullet  {
  int bytes;
} bullet =  {0};

/* External Variables */

extern struct descriptor_data *descriptor_list;
extern int    intermud_desc;
extern int    connected_to_intermud;
extern char  *level_abbrevs[];

/* Local Variables */

char          message[8000];
char          message2[8000];
char *Service;
char *To;
char *From;
char *Remote_Mud;
char *Our_Mud;
char *IP_Address;
char *Mud_Port;
char *Text;
char *player;
char *KB_in;
char *KB_out;
char *MSG_in;
char *MSG_out;
char *TTL;
char *Muted;
char *Mud_Type;
char *Mud_Version;
char *Intermud_Version;
char *Services;
char *Info;

/* Function Prototypes */

int  mud_recv_data(int fd, char *buf);
int  mud_send_data(int fd, char *buf);
void serv_recv_info(char *serv_message);
void serv_recv_mudlistrpy(char *serv_message);
void serv_recv_intertell(char *message);
void serv_recv_intertellrpy(char *serv_message);
void serv_recv_interwhoreq(char *serv_message); 
void serv_recv_interwhorpy(char *serv_message); 
void serv_recv_interpage(char *serv_message);
void serv_recv_mudinfo(char *serv_message);
void serv_recv_interwiz(char *serv_message);
void serv_recv_stats(char *serv_message);



ACMD(do_mudinfo)
{

  if (connected_to_intermud == 0) {
    send_to_char("Il server Intermud sembra non essere presente! "
                 "Contatta un Implementor!\r\n", ch);
    return;
  }
  
  argument = one_argument(argument, arg);
  
  skip_spaces(&argument);
  
  if (!*arg)
    send_to_char("Su quale mud vorresti informzioni?\r\n", ch);
  else {
    sprintf(message, "mudinfo#@#%s#@#%s", arg, GET_NAME(ch));
  
    mud_send_data(intermud_desc, message);
  }
  return;
}


ACMD(do_mudlist)
{
  if (connected_to_intermud == 0) {
    send_to_char("Il server Intermud sembra non essere presente! "
                 "Contatta un Implementor!\r\n", ch);
    return;
  }
  
  sprintf(message, "mudlist#@#%s", GET_NAME(ch));
  mud_send_data(intermud_desc, message);
  return;
}


ACMD(do_interpage)
{
  char *to;
  char *mud;

  if (connected_to_intermud == 0) {
    send_to_char("Il server Intermud sembra non essere presente! "
                 "Contatta un Implementor!\r\n", ch);
    return;
  }
  
  argument = one_argument(argument, arg);
  
  skip_spaces(&argument);
  
  if (!*arg)
    send_to_char("A chi vuoi fare interpage?\r\n", ch);
  else  {
    to = strtok(arg, "@");
    mud = strtok(NULL, " ");
    if (to == NULL || mud == NULL)  {
      send_to_char("Must be in User@Mud format.\r\n", ch);
      return;
    }
    
    sprintf(message, "\007\007*%s@%s* %s\r\n", to, mud, argument);
    
    send_to_char(message, ch);
    
    sprintf(message, "interpage#@#%s#@#%s#@#%s#@#%s", to, mud, GET_NAME(ch), 
            (*argument ? argument : " "));
  
    mud_send_data(intermud_desc, message);
  }
  return;
}

ACMD(do_interwho)
{
  if (connected_to_intermud == 0) {
    send_to_char("Il server Intermud sembra non essere presente! "
                 "Contatta un Implementor!\r\n", ch);
    return;
  }
  
  skip_spaces(&argument);
  
  if (!*argument)
    send_to_char("Da quale mud vorresti avere una who listing?\r\n", ch);
  else  {
    sprintf(message, "who-query#@#%s#@#%s", argument, GET_NAME(ch));
    
    send_to_char("Richiesta inviata, potrebbe volerci qualche minuto.\r\n", ch);
    
    mud_send_data(intermud_desc, message);
  }
  return;
}

ACMD(do_intertel)
{
  char *to;
  char *mud;

  if (connected_to_intermud == 0) {
    send_to_char("Il server Intermud sembra non essere presente! "
                 "Contatta un Implementor!\r\n", ch);
    return;
  }
  
  argument = one_argument(argument, arg);
  
  if (!*arg)
    send_to_char("A chi vuoi fare intertell?\r\n", ch);
  else if (!*argument)
    send_to_char("Cosa gli vorresti dire?\r\n", ch);
  else  {
    to  = strtok(arg, "@");
    mud = strtok(NULL, " ");
    if (to == NULL || mud == NULL)  {
      send_to_char("Formato non valido... deve essere Utente@Mud.\r\n", ch);
      return;
    }
    
    skip_spaces(&argument);
    
    sprintf(message, "intertell#@#%s#@#%s#@#%s#@#%s", to, mud, GET_NAME(ch), argument);
    
    mud_send_data(intermud_desc, message);
  
    sprintf(message, "Tu dici %s@%s, '%s'\r\n", to, mud, argument);
    send_to_char(CCRED(ch, C_NRM), ch);
    send_to_char(message, ch);
    send_to_char(CCNRM(ch, C_NRM), ch);
  }
  return;
}


ACMD(do_interwiz)
{
  
  struct descriptor_data *d;
  
  skip_spaces(&argument);
  
  if (!*argument)
    send_to_char("Che vuoi dire all'intero network?\r\n", ch);
  else if (connected_to_intermud == 0)
    send_to_char("Attualmente non connesso al server intermud, prova piu' tardi.\r\n", ch);
  else  {
    
    sprintf(message, "interwiz#@#%s#@#%s", GET_NAME(ch), argument);
    
    mud_send_data(intermud_desc, message);
    
    if (argument[0] == '*')
      sprintf(message, "%s@%s <--- %s\r\n", GET_NAME(ch), MUDNAME, argument+1);
    else
      sprintf(message, "%s@%s: %s\r\n", GET_NAME(ch), MUDNAME, argument); 
    
    for (d = descriptor_list; d; d = d->next)  {
      if (!d->connected && d->character &&
          (!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING)) &&
          (GET_LEVEL(d->character) >= LVL_IMMORT))  {
	send_to_char(CCMAG(d->character, C_NRM), d->character);
        send_to_char(message, d->character);
	send_to_char(CCNRM(d->character, C_NRM), d->character);
      }
    }
  }
  return;
}


ACMD(do_intermud)
{
  struct sockaddr_un serv_addr;
  int                servlen;
  
  argument = one_argument(argument, arg);
  
  if (!*arg)  {
    sprintf(buf, "intermud usage :-\r\n\r\n"
	         "    connect    : Connette il mud al server intermud\r\n"
	         "    disconnect : Sconnette il mud dal server intermud\r\n"
	         "    purge      : Forza il purge del DNS del server\r\n"
	         "    reboot     : Forza il reload della mudlist\r\n"
	         "    stats      : Statistiche dal server\r\n"
	         "    debug      : Abilita/disabilita il debug del server\r\n"
	         "    mute <mud> : Blocca i pacchetti provenienti dal mud\r\n"
                 "    shutdown   : Chiude il server\r\n\r\n");
    send_to_char(buf, ch);
  }
  else  {
    if (str_cmp(arg, "connect") == 0)  {
      if (connected_to_intermud == 1)
        send_to_char("Gia' connesso al server.\r\n", ch);
      else  {
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path, MUDSOCK_PATH);
        servlen = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);
  
        if ((intermud_desc = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)  {
          perror("Non riesco ad aprire il socket intermud");
          exit(1);
        }
  
        if (connect(intermud_desc, (struct sockaddr *) &serv_addr, servlen) == -1)  {
          mudlog("WARNING: Non riesco a connettermi al server intermud.", BRF, LVL_IMMORT, TRUE);
          connected_to_intermud = 0;
        }
        else  {
          mudlog("Collegamento effettuato al server intermud", BRF, LVL_IMMORT, TRUE);
          connected_to_intermud = 1;
        }
      }
      return;
    }
    else if (str_cmp(arg, "disconnect") == 0)  {
      if (connected_to_intermud == 0)
        send_to_char("Gia' sconnesso dal server intermud.\r\n", ch);
      else  {
        sprintf(buf, "WARNING: %s ha forzato una sconnessione dal server intermud.", GET_NAME(ch));
        mudlog(buf, BRF, LVL_IMMORT, TRUE);
        close(intermud_desc);
        connected_to_intermud = 0;  
        return;
      }
    }
    else if (str_cmp(arg, "purge") == 0)  {
      if (connected_to_intermud == 0)
        send_to_char("Non connesso al server intermud.\r\n", ch);
      else  {  
        sprintf(buf, "purge-mudlist#@#%s", GET_NAME(ch));
        mud_send_data(intermud_desc, buf);
        sprintf(buf, "WARNING: %s ha forzato un purge della mudlist.", GET_NAME(ch));
        mudlog(buf, BRF, LVL_IMMORT, TRUE);
      }
      return;
    }
    else if (str_cmp(arg, "reboot") == 0)  {
      if (connected_to_intermud == 0)
         send_to_char("Non connesso al server intermud.\r\n", ch);
      else  {
        sprintf(buf, "reboot-mudlist#@#%s", GET_NAME(ch));
        mud_send_data(intermud_desc, buf);
	sprintf(buf, "WARNING: %s issued mudlist request from bootmaster", GET_NAME(ch));
        mudlog(buf, BRF, LVL_IMMORT, TRUE);
      }
      return;
    }
    else if (str_cmp(arg, "stats") == 0)  {
      if (connected_to_intermud == 0)
         send_to_char("Non connesso al server intermud.\r\n", ch);
      else  {
        sprintf(buf, "intermud-stats#@#%s", GET_NAME(ch));
	mud_send_data(intermud_desc, buf);

      }
      return;
    }
    else if (str_cmp(arg, "mute") == 0)  {
      if (connected_to_intermud == 0)
        send_to_char("Non connesso al server intermud.\r\n", ch);
      else  {
        if (!*argument)
	  send_to_char("Devi specificare un nome mud per mute/unmute\r\n", ch);
	else  {
	  skip_spaces(&argument);
	  sprintf(buf, "mute#@#%s#@#%s", argument, GET_NAME(ch));
          mud_send_data(intermud_desc, buf);
	  sprintf(buf, "WARNING: %s toggled mute for %s", GET_NAME(ch), argument);
          mudlog(buf, BRF, LVL_IMMORT, TRUE);
	}
      }
      return;
    }
    else if (str_cmp(arg, "debug") == 0) {
      if (connected_to_intermud == 0)
        send_to_char("Non connesso al server intermud.\r\n", ch);
      else {
        sprintf(buf, "#@#debug#@#");
	mud_send_data(intermud_desc, buf);
      }
    } else if (str_cmp(arg, "shutdown") == 0) {
      if (connected_to_intermud == 0)
        send_to_char("Non connesso al server intermud.\r\n", ch);
      else {
        sprintf(buf, "shutdown#@#%s", GET_NAME(ch));
        mud_send_data(intermud_desc, buf);
        sprintf(buf, "(GC) Intermud Shutdown by %s", GET_NAME(ch));
        mudlog(buf, BRF, LVL_IMMORT, TRUE);
	connected_to_intermud = 0;
      }
    }
  }
  return;
}


void incoming_intermud_message(int intermud_desc)
{
  int   numbytes;
  
  numbytes = mud_recv_data(intermud_desc, message);
  
  if (numbytes <= 0)  {
    sprintf(message, "WARNING: Link al server caduto, usa connect per ricollegare il server");
    mudlog(message, BRF, LVL_IMMORT, TRUE);
    close(intermud_desc);
    connected_to_intermud = 0;
  }
  else
    message[numbytes] = '\0';
  

  log(message);

  if (!strncmp(message, "mudlog", 6))
    serv_recv_info(message);
  else if (!strncmp(message, "inc-mudlist", 11))
    serv_recv_mudlistrpy(message);
  else if (!strncmp(message, "inc-tell-nr", 11))
    serv_recv_intertellrpy(message);
  else if (!strncmp(message, "inc-tell", 8))
    serv_recv_intertell(message);
  else if (!strncmp(message, "inc-who-query", 13))
    serv_recv_interwhoreq(message);
  else if (!strncmp(message, "inc-who-reply", 13))
    serv_recv_interwhorpy(message);
  else if (!strncmp(message, "inc-interpage", 13))
    serv_recv_interpage(message);
  else if (!strncmp(message, "inc-mudinfo", 11))
    serv_recv_mudinfo(message);
  else if (!strncmp(message, "inc-stats", 9))
    serv_recv_stats(message);
  else if (!strncmp(message, "inc-interwiz", 12))
    serv_recv_interwiz(message);
  else {
    fprintf(stderr, "ERROR: Messaggio (%s) sconosciuto ricevuto dal server\n", message);
    return;
  }
}


void serv_recv_info(char *serv_message)
{
  Service = strtok(serv_message, "#@#");
  Info    = strtok(NULL, "\0");
  Info += 2;
  
  mudlog(Info, BRF, LVL_IMMORT, TRUE);
}

void serv_recv_mudlistrpy(char *serv_message)
{
  struct descriptor_data *d;
  
  Service = strtok(serv_message, "#@#");
  To = strtok(NULL, "#@#");
  Info = strtok(NULL, "\0");
  Info += 2;
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && (str_cmp(To, GET_NAME(d->character)) == 0)) {
      send_to_char(Info, d->character);
      break;
    }
}


void serv_recv_intertell(char *serv_message)
{
  struct descriptor_data *d;
  int                     player_found = 0;
  
  Service = strtok(serv_message, "#@#");
  From = strtok(NULL, "#@#");
  Remote_Mud = strtok(NULL, "#@#");
  To = strtok(NULL, "#@#");
  Text = strtok(NULL, "\0");
  Text += 2;
    
  sprintf(message2, "%s@%s ti dice, '%s'\r\n", From, Remote_Mud, Text);
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && d->character &&
	(str_cmp(To, GET_NAME(d->character)) == 0))  {
      if (GET_INVIS_LEV(d->character) >= 1)
        continue;
      if (PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
	player_found = 1;
      else  {
	send_to_char(CCRED(d->character, C_NRM), d->character);
	send_to_char(message2, d->character);
	send_to_char(CCNRM(d->character, C_NRM), d->character);
	player_found = 2;
      }
      break;
    }
    
  if (player_found == 0)  {
    sprintf(message2, "intertell-nr#@#%s#@#%s#@#Server#@#%s is not online at the moment.", From, Remote_Mud, To);
    mud_send_data(intermud_desc, message2);
  }
  else if (player_found == 1)  {
    sprintf(message2, "intertell-nr#@#%s#@#%s#@#Server#@#%s he is writing something at the moment", From, Remote_Mud, To);
    mud_send_data(intermud_desc, message2);
  }
  else  {
    sprintf(message2, "intertell-nr#@#%s#@#%s#@#Server#@#Message received.", From, Remote_Mud);
    mud_send_data(intermud_desc, message2);
  }
}
 

void serv_recv_intertellrpy(char *serv_message)
{
  struct descriptor_data *d;
  int                     player_found = 0;
  
  Service = strtok(serv_message, "#@#");
  From = strtok(NULL, "#@#");
  Remote_Mud = strtok(NULL, "#@#");
  To      = strtok(NULL, "#@#");
  Text    = strtok(NULL, "\0");
  Text += 2;
    
  sprintf(message2, "%s@%s ti dice, '%s'\r\n", From, Remote_Mud, Text);
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && d->character &&
	(str_cmp(To, GET_NAME(d->character)) == 0))  {
      if (PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
	player_found = 1;
      else  {
	send_to_char(CCRED(d->character, C_NRM), d->character);
	send_to_char(message2, d->character);
	send_to_char(CCNRM(d->character, C_NRM), d->character);
	player_found = 2;
      }
      break;
    }  
}


void serv_recv_interwhoreq(char *serv_message)
{
  struct descriptor_data *d;
  
  Service = strtok(serv_message, "#@#");
  From    = strtok(NULL, "#@#");
  Remote_Mud = strtok(NULL, "\0");
  Remote_Mud += 2;
  
  sprintf(message2, "who-reply#@#%s#@#%s#@#", From, Remote_Mud);
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected &&  d->character) {
	  if ((GET_LEVEL(d->character)< LVL_IMMORT) || (GET_INVIS_LEV(d->character)<LVL_IMMORT))
	  { 
      sprintf(message2, "%s[%3d] %s %s", message2, 
              GET_LEVEL(d->character),
              GET_NAME(d->character), GET_TITLE(d->character));
      if (PLR_FLAGGED(d->character, PLR_WRITING))
	strcat(message2, " (writing)");
      else if (PLR_FLAGGED(d->character, PLR_MAILING))
	strcat(message2, " (mailing)");
      strcat(message2, "\n");
	  }
    }

  strcat(message2, "\0");
    
  mud_send_data(intermud_desc, message2);
}


void serv_recv_interwhorpy(char *serv_message)
{
  struct descriptor_data *d;
 
  Service = strtok(serv_message, "#@#");
  To = strtok(NULL, "#@#");
  Remote_Mud = strtok(NULL, "#@#");
  Text = strtok(NULL, "\0");
  Text += 2;
  
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && (str_cmp(To, GET_NAME(d->character)) == 0) &&
        (!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING)))  {
        
      send_to_char(Text, d->character);
      send_to_char("\r\n", d->character);
    }
}


void serv_recv_interpage(char *serv_message)
{
  struct descriptor_data *d;
  int                     player_found = 0;
  
  Service = strtok(serv_message, "#@#");
  From = strtok(NULL, "#@#");
  Remote_Mud = strtok(NULL, "#@#");
  To      = strtok(NULL, "#@#");
  Text    = strtok(NULL, "\0");
  Text += 2;
    
  
  sprintf(message2, "\007\007*%s@%s* %s\r\n", From, Remote_Mud, Text);
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && (str_cmp(To, GET_NAME(d->character)) == 0))  {
      if (GET_INVIS_LEV(d->character) == GET_LEVEL(d->character))
        continue;
      if (PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
	player_found = 1;
      else  {
	send_to_char(message2, d->character);
	player_found = 2;
      }
      break;
    }
    
  if (player_found == 0)  {
    sprintf(message2, "intertell-nr#@#%s#@#%s#@#Server#@#%s is not currently on-line.", From, Remote_Mud, To);
    mud_send_data(intermud_desc, message2);
  }
  if (player_found == 1)  {
    sprintf(message2, "intertell-nr#@#%s#@#%s#@#Server#@#%s is currently mailing or writing", From, Remote_Mud, To);
    mud_send_data(intermud_desc, message2);
  }
}


void serv_recv_mudinfo(char *serv_message)
{
  struct descriptor_data *d;
  
  Service = strtok(serv_message, "#@#");
  To = strtok(NULL, "#@#");
  Text = strtok(NULL, "\0"); 
  Text += 2;
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && (str_cmp(To, GET_NAME(d->character)) == 0)) {
      send_to_char(Text, d->character);
      break;
    }
}


void serv_recv_stats(char *serv_message)
{
  struct descriptor_data *d;
  
  Service = strtok(serv_message, "#@#");
  To = strtok(NULL, "#@#");
  Text = strtok(NULL, "\0");
  Text += 2;
    
  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && (str_cmp(To, GET_NAME(d->character)) == 0)) {
      send_to_char(Text, d->character);
      break;
    }
}
 

void serv_recv_interwiz(char *serv_message)
{
  struct descriptor_data *d;
  
  Service = strtok(serv_message, "#@#");
  From    = strtok(NULL, "#@#");
  Remote_Mud = strtok(NULL, "#@#");
  Text = strtok(NULL, "\0");
  Text += 2;
    
    
  if (Text[0] == '*')
    sprintf(message2, "%s@%s <--- %s\r\n", From, Remote_Mud, Text+1);
  else
    sprintf(message2, "%s@%s: %s\r\n", From, Remote_Mud, Text);
    
  for (d = descriptor_list; d; d = d->next)  
    if (!d->connected &&
	(!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING)) &&
	(GET_LEVEL(d->character) >= LVL_IMMORT))  {
      send_to_char(CCMAG(d->character, C_NRM), d->character);
      send_to_char(message2, d->character);
      send_to_char(CCNRM(d->character, C_NRM), d->character);
    }
}


void init_intermud_socket(void)
{
  struct sockaddr_un serv_addr;
  int                servlen;
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, MUDSOCK_PATH);
  servlen = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);
  
  if ((intermud_desc = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)  {
    perror("Can't open intermud socket");
    exit(1);
  }
  
  if (connect(intermud_desc, (struct sockaddr *) &serv_addr, servlen) == -1)  {
    fprintf(stderr, "WARNING: Intermud server is unavailable, continuing, ERRNO = %d\n", errno);
    connected_to_intermud = 0;
  }
  else  {
    fprintf(stderr, "Connected to intermud server OK.\n");
    connected_to_intermud = 1;
  }
}

int mud_recv_data(int fd, char *buf)
{
  int buflen;
  int cc;
  
  cc = recv(fd, &bullet, sizeof(struct bullet), 0);
  if (cc <= 0)
    return(cc);
  else  {
    buflen = bullet.bytes;
    while (buflen > 0)  {
      cc = recv(fd, buf, buflen, 0);
      if (cc <= 0)  {
        fprintf(stderr, "ERROR: mud_recv_data, fatal\n");
        exit(1);
      }
      buf += cc;
      buflen -= cc;
    }
    return(bullet.bytes);
  }
}

int mud_send_data(int fd, char *buf)
{
  int buflen;
  int cc;
  
  if (strlen(buf) <= 0)  {
    fprintf(stderr, "ERROR: mud_send_data requested to send 0 bytes, exiting\n");
    exit(0);
  }
  
  bullet.bytes = strlen(buf);
  
  cc = send(fd, &bullet, sizeof(struct bullet), 0);
  if (cc <= 0)
    return(cc);
  else  {
    buflen = bullet.bytes;
    while (buflen > 0)  {
      cc = send(fd, buf, buflen, 0);
      if (cc <= 0)  {
        fprintf(stderr, "ERROR: mud_send_data, fatal\n");
      }
      buf += cc;
      buflen -= cc;
    }
    return(bullet.bytes);
  }
}
