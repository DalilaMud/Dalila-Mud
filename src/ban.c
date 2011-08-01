/* ************************************************************************
*   File: ban.c                                         Part of CircleMUD *
*  Usage: banning/unbanning/checking sites and player names               *
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

struct ban_list_element *ban_list = NULL;
struct unlockedip_list_element *unlocked_ip_list = NULL;

char *ban_types[] = {
  "no",
  "new",
  "select",
  "all",
  "ERROR"
};


void load_banned(void)
{
  FILE *fl;
  int i, date;
  char site_name[BANNED_SITE_LENGTH + 1], ban_type[100];
  char name[MAX_NAME_LENGTH + 1];
  struct ban_list_element *next_node;

  ban_list = 0;

  if (!(fl = fopen(BAN_FILE, "r"))) {
    perror("Unable to open banfile");
    return;
  }
  while (fscanf(fl, " %s %s %d %s ", ban_type, site_name, &date, name) == 4) {
    CREATE(next_node, struct ban_list_element, 1);
    strncpy(next_node->site, site_name, BANNED_SITE_LENGTH);
    next_node->site[BANNED_SITE_LENGTH] = '\0';
    strncpy(next_node->name, name, MAX_NAME_LENGTH);
    next_node->name[MAX_NAME_LENGTH] = '\0';
    next_node->date = date;

    for (i = BAN_NOT; i <= BAN_ALL; i++)
      if (!strcmp(ban_type, ban_types[i]))
	next_node->type = i;

    next_node->next = ban_list;
    ban_list = next_node;
  }

  fclose(fl);
}


int isbanned(char *hostname)
{
  int i;
  struct ban_list_element *banned_node;
  char *nextchar;

  if (!hostname || !*hostname)
    return (0);

  i = 0;
  for (nextchar = hostname; *nextchar; nextchar++)
    *nextchar = LOWER(*nextchar);

  for (banned_node = ban_list; banned_node; banned_node = banned_node->next)
    if (strstr(hostname, banned_node->site))	/* if hostname is a substring */
      i = MAX(i, banned_node->type);

  return i;
}


void _write_one_node(FILE * fp, struct ban_list_element * node)
{
  if (node) {
    _write_one_node(fp, node->next);
    fprintf(fp, "%s %s %ld %s\n", ban_types[node->type],
	    node->site, (long) node->date, node->name);
  }
}



void write_ban_list(void)
{
  FILE *fl;

  if (!(fl = fopen(BAN_FILE, "w"))) {
    perror("write_ban_list");
    return;
  }
  _write_one_node(fl, ban_list);/* recursively write from end to start */
  fclose(fl);
  return;
}


ACMD(do_ban)
{
  char flag[MAX_INPUT_LENGTH], site[MAX_INPUT_LENGTH],
	format[MAX_INPUT_LENGTH], *nextchar, *timestr;
  int i;
  struct ban_list_element *ban_node;

  *buf = '\0';

  if (!*argument) {
    if (!ban_list) {
      send_to_char("No sites are banned.\r\n", ch);
      return;
    }
    strcpy(format, "%-25.25s  %-8.8s  %-10.10s  %-16.16s\r\n");
    sprintf(buf, format,
	    "Banned Site Name",
	    "Ban Type",
	    "Banned On",
	    "Banned By");
    send_to_char(buf, ch);
    sprintf(buf, format,
	    "---------------------------------",
	    "---------------------------------",
	    "---------------------------------",
	    "---------------------------------");
    send_to_char(buf, ch);

    for (ban_node = ban_list; ban_node; ban_node = ban_node->next) {
      if (ban_node->date) {
	timestr = asctime(localtime(&(ban_node->date)));
	*(timestr + 10) = 0;
	strcpy(site, timestr);
      } else
	strcpy(site, "Unknown");
      sprintf(buf, format, ban_node->site, ban_types[ban_node->type], site,
	      ban_node->name);
      send_to_char(buf, ch);
    }
    return;
  }
  two_arguments(argument, flag, site);
  if (!*site || !*flag) {
    send_to_char("Usage: ban {all | select | new} site_name\r\n", ch);
    return;
  }
  if (!(!str_cmp(flag, "select") || !str_cmp(flag, "all") || !str_cmp(flag, "new"))) {
    send_to_char("Flag must be ALL, SELECT, or NEW.\r\n", ch);
    return;
  }
  for (ban_node = ban_list; ban_node; ban_node = ban_node->next) {
    if (!str_cmp(ban_node->site, site)) {
      send_to_char("That site has already been banned -- unban it to change the ban type.\r\n", ch);
      return;
    }
  }

  CREATE(ban_node, struct ban_list_element, 1);
  strncpy(ban_node->site, site, BANNED_SITE_LENGTH);
  for (nextchar = ban_node->site; *nextchar; nextchar++)
    *nextchar = LOWER(*nextchar);
  ban_node->site[BANNED_SITE_LENGTH] = '\0';
  strncpy(ban_node->name, GET_NAME(ch), MAX_NAME_LENGTH);
  ban_node->name[MAX_NAME_LENGTH] = '\0';
  ban_node->date = time(0);

  for (i = BAN_NEW; i <= BAN_ALL; i++)
    if (!str_cmp(flag, ban_types[i]))
      ban_node->type = i;

  ban_node->next = ban_list;
  ban_list = ban_node;

  sprintf(buf, "%s has banned %s for %s players.", GET_NAME(ch), site,
	  ban_types[ban_node->type]);
  mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
  send_to_char("Site banned.\r\n", ch);
  write_ban_list();
}


ACMD(do_unban)
{
  char site[80];
  struct ban_list_element *ban_node, *temp;
  int found = 0;

  one_argument(argument, site);
  if (!*site) {
    send_to_char("A site to unban might help.\r\n", ch);
    return;
  }
  ban_node = ban_list;
  while (ban_node && !found) {
    if (!str_cmp(ban_node->site, site))
      found = 1;
    else
      ban_node = ban_node->next;
  }

  if (!found) {
    send_to_char("That site is not currently banned.\r\n", ch);
    return;
  }
  REMOVE_FROM_LIST(ban_node, ban_list, next);
  send_to_char("Site unbanned.\r\n", ch);
  sprintf(buf, "%s removed the %s-player ban on %s.",
	  GET_NAME(ch), ban_types[ban_node->type], ban_node->site);
  mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);

  free(ban_node);
  write_ban_list();
}


/**************************************************************************
 *  Code to check for invalid names (i.e., profanity, etc.)		  *
 *  Written by Sharon P. Goza						  *
 **************************************************************************/

typedef char namestring[MAX_NAME_LENGTH];

namestring *invalid_list = NULL;
int num_invalid = 0;

int Valid_Name(char *newname)
{
  int i;

  char tempname[MAX_INPUT_LENGTH];

  /* return valid if list doesn't exist */
  if (!invalid_list || num_invalid < 1)
    return 1;

  /* change to lowercase */
  strcpy(tempname, newname);
  for (i = 0; tempname[i]; i++)
    tempname[i] = LOWER(tempname[i]);

  /* Does the desired name contain a string in the invalid list? */
  for (i = 0; i < num_invalid; i++)
    if (strstr(tempname, invalid_list[i]))
      return 0;

  return 1;
}


void Read_Invalid_List(void)
{
  FILE *fp;
  int i = 0;
  char string[80];

  if (!(fp = fopen(XNAME_FILE, "r"))) {
    perror("Unable to open invalid name file");
    return;
  }
  /* count how many records */
  while (fgets(string, 80, fp) != NULL && strlen(string) > 1)
    num_invalid++;

  rewind(fp);

  CREATE(invalid_list, namestring, num_invalid);

  for (i = 0; i < num_invalid; i++) {
    fgets(invalid_list[i], 80, fp);	/* read word */
    invalid_list[i][strlen(invalid_list[i]) - 1] = '\0'; /* cleave off \n */
  }

  fclose(fp);
}

/* Orione Inizio */
void load_unlockedip_list (void) {
   FILE *fuip;
   struct unlockedip_list_element *next_node, uiptmp;
   
   if (!(fuip = fopen(UNLOCKED_IP_FILE, "rb"))) {
   	log("SYSERR: unable to open Unlocked IP file");
	  return;
   }

   while (fread(&uiptmp, sizeof(uiptmp), 1, fuip) >= sizeof(uiptmp)) {
   	CREATE(next_node, struct unlockedip_list_element, 1);	//Crea un nuovo elemento della lista
	  memcpy(next_node, &uiptmp, sizeof(uiptmp));	//Copia le informazioni appena caricate nel nuovo elemento
	  next_node->next = unlocked_ip_list;		//Assegnamento a next del nodo primo della lista
	  unlocked_ip_list = next_node;			//Riassegnamento del nodo appena creato come primo della lista
   }

   fclose(fuip);
}

int isunlocked(char *ip) {
   struct unlockedip_list_element *uip_node = NULL;
   struct descriptor_data *d;
   extern struct descriptor_data *descriptor_list;
   int cont = 0;

   if (!ip || !*ip)
	return 0;

   for (d = descriptor_list; d; d=d->next)
	 if ((d->character) && (!str_cmp(ip, d->host)) && (GET_LEVEL(d->character)<LVL_IMMORT))
		 cont++; 

   for (uip_node = unlocked_ip_list; uip_node; uip_node = uip_node->next) {
	   if (!strcmp(ip, uip_node->ip_address)) {
		   if (cont<=uip_node->max_users)
			   return TRUE;
		   else
			   return FALSE;
	   }
   }
	
   return FALSE;
}

void write_one_ip_node (FILE *fp, struct unlockedip_list_element * node) 
{
  if (node) {
	  write_one_ip_node(fp, node->next);
	  fprintf(fp, "%s %d %ld %s", node->ip_address, node->max_users, (long) node->unlock_date, node->god_name);
  }
}

void write_unlockedip_list (void) 
{
  FILE *fuip;

  if (!(fuip = fopen(UNLOCKED_IP_FILE, "w"))) {
	  log("SYSERR: write_unlockedip_list");
	  return;
  }

  write_one_ip_node(fuip, unlocked_ip_list); //Scrittura ricorsiva, dalla fine all'inizio

  fclose(fuip);
}

ACMD(do_sbloccaip) {
  char site[MAX_INPUT_LENGTH], num_utenti[10], format[MAX_INPUT_LENGTH], *nextchar, *timestr;
  int number;
   //la variabile temp serve in REMOVE_FROM_LIST (vedi utils.h)
  struct unlockedip_list_element *uip_node, *temp;

  if (!*argument) {
	  if (!unlocked_ip_list) {
		  send_to_char("Nessun IP e' sbloccato.\r\n", ch);
		  return;
	  }

	  strcpy(format, "%-15.15s %-17.17s %-10.10s %-16.16s\r\n");
	  sprintf(buf, format,
	  	"IP Sbloccato",
		  "Numero Max Utenti",
		  "In Data",
		  "Sbloccato Da"
		  );
	  send_to_char(buf, ch);
	  sprintf(buf, format,
	  	"---------------------------------",
		  "---------------------------------",
		  "---------------------------------",
		  "---------------------------------"
		  );
	  send_to_char(buf, ch);

	  for (uip_node = unlocked_ip_list; uip_node; uip_node = uip_node->next) {
		  if (uip_node->unlock_date) {
		  	timestr = asctime(localtime(&(uip_node->unlock_date)));
		  	*(timestr + 10) = 0;
		  	strcpy(site, timestr);
		  } 
		  else
		  	strcpy(site, "Unknown");
		  sprintf(num_utenti, "%d", uip_node->max_users);
		  sprintf(buf, format, uip_node->ip_address, num_utenti, site, uip_node->god_name);
		  send_to_char(buf, ch);
	  }
	  return;
  }

  two_arguments(argument, num_utenti, site);
  if (!*site || !*num_utenti) {
	  send_to_char("Usage: sbloccaip {numero massimo di utenti per questo indirizzo} {indirizzo}\r\n", ch);
	  return;
  }

  if (strlen(site)!=15) {
	  send_to_char("Formato numero Ip errato, non saltare gli zero (es: 192.167.020.067).\r\n", ch);
	  return;
  }

  number = atoi(num_utenti);
  for (uip_node = unlocked_ip_list; uip_node; uip_node = uip_node->next) {
	  if (!str_cmp(uip_node->ip_address, site)) {
		  if (number==1) {
			  REMOVE_FROM_LIST(uip_node, unlocked_ip_list, next);
			  send_to_char("Indirizzo bloccato.\r\n", ch);
			  sprintf(buf, "%s ha bloccato l'indirizzo %s", GET_NAME(ch), uip_node->ip_address);
			  mudlog(buf, NRM, LVL_GOD, TRUE);
			  free(uip_node);
			  write_unlockedip_list();
			  return;
		  } 
		  else {
			  send_to_char("Questo indirizzo e' gia' sbloccato, controlla meglio.\r\n", ch);
			  return;
		  }
	  }
  }

  CREATE(uip_node, struct unlockedip_list_element, 1);
  strncpy(uip_node->ip_address, site, UNLOCKED_SITE_LENGTH);
  for (nextchar = uip_node->ip_address; *nextchar; nextchar++)
	  *nextchar = LOWER(*nextchar);
  uip_node->ip_address[UNLOCKED_SITE_LENGTH] = '\0';
  uip_node->max_users = number;
  strncpy(uip_node->god_name, GET_NAME(ch), MAX_NAME_LENGTH);
  uip_node->god_name[MAX_NAME_LENGTH] = '\0';
  uip_node->unlock_date = time(0);

  uip_node->next = unlocked_ip_list;
  unlocked_ip_list = uip_node;

  sprintf(buf, "%s ha sbloccato l'indirizzo %s per un massimo di %d giocatori.", GET_NAME(ch), site, number);
  mudlog(buf, NRM, LVL_GOD, TRUE);
  send_to_char("Indirizzo sbloccato.\r\n", ch);
  write_unlockedip_list();
}
/* Orione Fine */
