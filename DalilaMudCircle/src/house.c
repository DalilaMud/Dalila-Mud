/* ************************************************************************
*   File: house.c                                       Part of CircleMUD *
*  Usage: Handling of player houses                                       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"



#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"
#include "house.h"
#include "spells.h"

extern const char *dirs[];
extern struct room_data *world;
extern int top_of_world;
extern const int rev_dir[];
extern struct index_data *obj_index;
extern struct dex_skill_type dex_app_skill[];

extern struct obj_data *Obj_from_store(struct obj_file_elem object);
extern int Obj_to_store(struct obj_data * obj, FILE * fl);
extern struct obj_data *Obj_from_store_to(struct obj_file_elem object, int *locate);
extern int Obj_to_store_from(struct obj_data * obj, FILE * fl, int locate);
extern int Ascii_Obj_to_store_from(struct obj_data * obj, FILE * fl, int locate);
extern int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);

//extern void redit_save_to_disk(int zone_num);

int House_undef_clean();
void Elimina_Cantieri();

struct house_control_rec house_control[MAX_HOUSES];
int num_of_houses = 0;


/* First, the basics: finding the filename; loading/saving objects */

/* Return a filename given a house vnum */
int House_get_filename(int vnum, char *filename)
{
  if (vnum < 0)
    return 0;

  sprintf(filename, "house/%d.house", vnum);
  return 1;
}


#define MAX_BAG_ROW 5

/* Load all objects for a house */
int House_load(room_vnum vnum)
{
  FILE *fl;
  char fname[MAX_STRING_LENGTH];
  struct obj_file_elem object;
  struct obj_data *obj;
  struct obj_data *obj1;
  struct obj_data *cont_row[MAX_BAG_ROW];
  room_rnum rnum;
  int locate, j;
  int lunghezza;
  char stringa[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if ((rnum = real_room(vnum)) == -1)
    return 0;
  if (!House_get_filename(vnum, fname))
    return 0;
  if (!(fl = fopen(fname, "r+b"))) {
    /* no file found */
    return 0;
  }

  for (j = 0; j < MAX_BAG_ROW; j++) 
    cont_row[j]=NULL;

  while (!feof(fl)) {
    fread(&object, sizeof(struct obj_file_elem), 1, fl);
    
    if (ferror(fl)) {
      perror("Reading house file: House_load.");
      fclose(fl);
      return 0;
    }
    
    if (!feof(fl)) {
      obj = Obj_from_store_to(object, &locate);
      
      if ((obj) && (IS_OBJ_STAT(obj, ITEM_RESTRING))) {
        
        if ((lunghezza=fgetc(fl)) < 1) {
          perror("(1)Reading crash file: Crash_load.");
          fclose(fl);
          return 1;
        }
        
        if (!fgets(stringa, lunghezza+1, fl)) {
          perror("(2)Reading crash file: Crash_load.");
          fclose(fl);
          return 1;
        }
        
        obj->name=strdup(stringa);
        
        if ((lunghezza=fgetc(fl)) < 1) {
          perror("(3)Reading crash file: Crash_load.");
          fclose(fl);
          return 1;
        }
        
        if (!fgets(stringa, lunghezza+1, fl)) {
          perror("(4)Reading crash file: Crash_load.");
          fclose(fl);
          return 1;
        }
        
        obj->description=strdup(stringa);
        
        if ((lunghezza=fgetc(fl)) < 1) {
          perror("(5)Reading crash file: Crash_load.");
          fclose(fl);
          return 1;
        }
        
        if (!fgets(stringa, lunghezza+1, fl)) {
          perror("(6)Reading crash file: Crash_load.");
          fclose(fl);
          return 1;
        }
        
        obj->short_description=strdup(stringa);
	      
	      if ((lunghezza=fgetc(fl)) < 1) {
 			    perror("(7)Reading crash file: Crash_load.");
 			    fclose(fl);
 			    return 1;
 			  }
 		
 		    if (!fgets(stringa, lunghezza+1, fl)) {
 			    perror("(8)Reading crash file: Crash_load.");
 			    fclose(fl);
 			    return 1;
 			  }
       
        if (str_cmp(stringa," "))
 		     obj->action_description=strdup(stringa);
      }
      
      if (obj) {
	      obj_to_room(obj, rnum);
        
        // Non dovrebbe mai arrivare qui
        if (locate>0) { 
          sprintf(buf, "Locate= %d : Room vnum: %d ", locate, vnum);
		      log("!!!!Errore in House_load - oggetto con locate > 0");
		      log(buf);
		      return 0;
          //  exit(0);
		    }

        for (j = MAX_BAG_ROW-1; j > -locate; j--) {
          // Niente contenitore - manda alla stanza
          if (cont_row[j]) {
            for(; cont_row[j]; cont_row[j] = obj1) {
              obj1= cont_row[j]->next_content;
              obj_to_room(cont_row[j], rnum);
            }
           
            cont_row[j] = NULL;
          }
        }
       
        // Esiste lista contents
        if (j == -locate && cont_row[j]) {
          if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER) {    /* prendi oggetto; riempi; metti nella stanza */
            obj_from_room(obj);
            obj->contains = NULL;
           
            for (; cont_row[j]; cont_row[j] = obj1) {
              obj1 = cont_row[j]->next_content;
              obj_to_obj(cont_row[j], obj);
            }
           
            obj_to_room(obj, rnum);
          }
         
          // Non e' un contenitore -> svuota la lista contents
          else {
            for (; cont_row[j]; cont_row[j] = obj1) {
              obj1 = cont_row[j]->next_content;
              obj_to_room(cont_row[j], rnum);
            }          
            cont_row[j]=NULL;
	        }
	      }
       
        if (locate < 0 && locate >= - MAX_BAG_ROW) {
          obj_from_room(obj);
         
          if ( (obj1 = cont_row[-locate-1]) ) {
            while (obj1->next_content) 
              obj1 = obj1->next_content;
            obj1->next_content=obj;
          }
          else 
            cont_row[-locate-1] = obj;
        }
	    }
	  }
  }

  fclose(fl);

  REMOVE_BIT(ROOM_FLAGS(rnum), ROOM_HOUSE_CRASH); 
  return 1;
}

#undef MAX_BAG_ROW


/* Save all objects for a house (recursive; initial call must be followed
   by a call to House_restore_weight)  Assumes file is open already. */
int House_save(struct obj_data * obj, FILE * fp,  FILE * fpa, int locate)
{
  struct obj_data *tmp;
  int result;

  if (obj)
    {
    House_save(obj->next_content, fp, fpa, locate);
    House_save(obj->contains, fp, fpa, MIN(0,locate)-1 );
    result = Obj_to_store_from(obj, fp, locate);
    Ascii_Obj_to_store_from(obj, fpa, locate);
    for (tmp = obj->in_obj; tmp; tmp = tmp->in_obj)
      GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);
    if (!result)
      return 0;
    }
  return 1;
}


/* restore weight of containers after House_save has changed them for saving */
void House_restore_weight(struct obj_data * obj)
{
  if (obj) {
    House_restore_weight(obj->contains);
    House_restore_weight(obj->next_content);
    if (obj->in_obj)
      GET_OBJ_WEIGHT(obj->in_obj) += GET_OBJ_WEIGHT(obj);
  }
}


/* Save all objects in a house */
void House_crashsave(room_vnum vnum)
{
  int rnum;
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  FILE *fpa;

  if ((rnum = real_room(vnum)) == -1)
    return;
  if (!House_get_filename(vnum, buf))
    return;
  if (!(fp = fopen(buf, "wb"))) {
    perror("SYSERR: Error saving house file");
    return;
  }
  strcat(buf,".txt");
  if (!(fpa = fopen(buf, "w"))) {
    perror("SYSERR: Error saving house text file");
    return;
  }
  if (!House_save(world[rnum].contents, fp, fpa, 0)) {
    sprintf(buf,"Failed House crash-saving : %d", world[rnum].number);
    mudlog(buf, BRF, LVL_IMMORT, TRUE);
    fclose(fp);
    return;
  }
  fclose(fp);
  fclose(fpa);
  House_restore_weight(world[rnum].contents);
  REMOVE_BIT(ROOM_FLAGS(rnum), ROOM_HOUSE_CRASH);
}


/* Delete a house save file */
void House_delete_file(int vnum)
{
  char buf[MAX_INPUT_LENGTH], fname[MAX_INPUT_LENGTH];
  FILE *fl;

  if (!House_get_filename(vnum, fname))
    return;
  if (!(fl = fopen(fname, "rb"))) {
    if (errno != ENOENT) {
      sprintf(buf, "SYSERR: Error deleting house file #%d. (1)", vnum);
      perror(buf);
    }
    return;
  }
  fclose(fl);
  if (unlink(fname) < 0) {
    sprintf(buf, "SYSERR: Error deleting house file #%d. (2)", vnum);
    perror(buf);
  }
}


/* List all objects in a house file */
void House_listrent(struct char_data * ch, room_vnum vnum)
{
  FILE *fl;
  char fname[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  struct obj_file_elem object;
  struct obj_data *obj;
  int lunghezza;
  char stringa[MAX_STRING_LENGTH];


  if (!House_get_filename(vnum, fname))
    return;
  if (!(fl = fopen(fname, "rb"))) {
    sprintf(buf, "No objects on file for house #%d.\r\n", vnum);
    send_to_char(buf, ch);
    return;
  }
  *buf = '\0';
  while (!feof(fl))
    {
    fread(&object, sizeof(struct obj_file_elem), 1, fl);
    if (ferror(fl))
      {
      fclose(fl);
      return;
      }
    if (!feof(fl))
      {
      obj = Obj_from_store(object);
      if ((obj) && (IS_OBJ_STAT(obj, ITEM_RESTRING)))
        {
        if ((lunghezza=fgetc(fl)) < 1)
          {
          perror("Reading crash file: House_listrent.");
          fclose(fl);
          return;
          }
        if (!fgets(stringa, lunghezza+1, fl))
          {
          perror("Reading crash file: House_listrent.");
          fclose(fl);
          return;
          }
        if ((lunghezza=fgetc(fl)) < 1)
          {
          perror("Reading crash file: House_listrent.");
          fclose(fl);
          return;
          }
        if (!fgets(stringa, lunghezza+1, fl))
          {
          perror("Reading crash file: House_listrent.");
          fclose(fl);
          return;
          }
        if ((lunghezza=fgetc(fl)) < 1)
          {
          perror("Reading crash file: House_listrent.");
          fclose(fl);
          return;
          }
        if (!fgets(stringa, lunghezza+1, fl))
          {
          perror("Reading crash file: House_listrent.");
          fclose(fl);
          return;
          }
	    if ((lunghezza=fgetc(fl)) < 1)
 			 {
			 perror("Reading crash file: House_listrent.");
 			 fclose(fl);
 			 return;
 			 }
 		if (!fgets(stringa, lunghezza+1, fl))
 			 {
 			 perror("Reading crash file: House_listrent.");
 			 fclose(fl);
 			 return;
 			 }
        }
      if (obj)
        {
        sprintf(buf, "%s [%5d] (%5dau) %s\r\n", buf,
	      GET_OBJ_VNUM(obj), GET_OBJ_RENT(obj),
	      obj->short_description);
        free_obj(obj);
        }
      }
    }

  send_to_char(buf, ch);
  fclose(fl);
}




/******************************************************************
 *  Functions for house administration (creation, deletion, etc.  *
 *****************************************************************/

int find_house(room_vnum vnum)
{
  int i;

  for (i = 0; i < num_of_houses; i++)
    if (house_control[i].vnum == vnum)
      return i;

  return -1;
}



/* Save the house control information */
void House_save_control(void)
{
  FILE *fl;

  if (!(fl = fopen(HCONTROL_FILE, "wb"))) {
    perror("SYSERR: Unable to open house control file");
    return;
  }
  /* write all the house control recs in one fell swoop.  Pretty nifty, eh? */
  fwrite(house_control, sizeof(struct house_control_rec), num_of_houses, fl);

  fclose(fl);
}


/* call from boot_db - will load control recs, load objs, set atrium bits */
/* should do sanity checks on vnums & remove invalid records */
void House_boot(void)
{
  struct house_control_rec temp_house;
  room_rnum real_house, real_atrium;
  FILE *fl;

  memset((char *)house_control,0,sizeof(struct house_control_rec)*MAX_HOUSES);

  if (!(fl = fopen(HCONTROL_FILE, "rb"))) {
    log("House control file does not exist.");
    return;
  }
  while (!feof(fl) && num_of_houses < MAX_HOUSES) {
    fread(&temp_house, sizeof(struct house_control_rec), 1, fl);

    if (feof(fl))
      break;

    if (get_name_by_id(temp_house.owner) == NULL)
      continue;			/* owner no longer exists -- skip */

    if ((real_house = real_room(temp_house.vnum)) < 0)
      continue;			/* this vnum doesn't exist -- skip */

    if ((find_house(temp_house.vnum)) >= 0)
      continue;			/* this vnum is already a hosue -- skip */

    if ((real_atrium = real_room(temp_house.atrium)) < 0)
      continue;			/* house doesn't have an atrium -- skip */

    if (temp_house.exit_num < 0 || temp_house.exit_num >= NUM_OF_DIRS)
      continue;			/* invalid exit num -- skip */

    if (TOROOM(real_house, temp_house.exit_num) != real_atrium)
      continue;			/* exit num mismatch -- skip */

    if (temp_house.num_of_guests>MAX_GUESTS)
      {
      sprintf(buf, "SYSERR: Numero di ospiti %d > MAX_GUESTS in casa di ",
              temp_house.num_of_guests);
      strcat(buf, get_name_by_id(temp_house.owner));
      sprintf(buf+strlen(buf), "[id=%ld], vnum=%d", temp_house.owner, temp_house.vnum);

      mudlog(buf, BRF, LVL_IMPL, TRUE);
      temp_house.num_of_guests=MAX_GUESTS;
      }

    house_control[num_of_houses++] = temp_house;

    SET_BIT(ROOM_FLAGS(real_house), ROOM_HOUSE | ROOM_PRIVATE);
    SET_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);
    House_load(temp_house.vnum);
  }

  fclose(fl);
  House_undef_clean();
}



/* "House Control" functions */

char *HCONTROL_FORMAT =
"Uso :  hcontrol costruisci <num. casa> <direzione uscita> <nome giocatore>\r\n"
"       hcontrol distruggi <num. casa>\r\n"
"       hcontrol paghi <num. casa>\r\n"
"       hcontrol mostra <vnum start> <vnum finish>\r\n"
"       hcontrol pulisci\r\n"
"       hcontrol proteggi <num. casa> <%>\r\n";

#define NAME(x) ((temp = get_name_by_id(x)) == NULL ? "<UNDEF>" : temp)

void hcontrol_list_houses(struct char_data * ch, char *argument)
{
  int i, j ,s ,f;
  char *timestr, *temp;
  char built_on[128], last_pay[128], own_name[128];
  char start[MAX_INPUT_LENGTH];
  char finish[MAX_INPUT_LENGTH];
  
  argument = one_argument(argument, start);
  one_argument(argument, finish);   

  if ((!*start) || (!*finish) || !is_number(start) || !is_number(finish))
  {
    send_to_char("Uses: hcontrol mostra <vnum start> <vnum finish>\r\n", ch);
    return;
  }
  
  if (atoi(start) > atoi(finish))
    {
      send_to_char("Intervallo non consentito.\r\n", ch);
      return;
  }
  
  if (!num_of_houses) {
    send_to_char("Nessuna dimora definta ancora.\r\n", ch);
    return;
  }


   s = atoi(start);
   f = atoi(finish);
   strcpy(buf, "Thor sitting on his reversed Hammer\r\n");
   strcat(buf, " ------- ------- -------------- -------- -------------- ----------- ----- \r\n");
   strcat(buf, "| House | Atrio | Data costruz | Ospiti | Proprietario | UltimoPag | Lck |\r\n");
   strcat(buf, " ------- ------- -------------- -------- -------------- ----------- ----- \r\n");
   // send_to_char(buf, ch);
  
  for (i = 0; i < num_of_houses; i++) {
   if ((house_control[i].vnum >= s) && (house_control[i].vnum <= f)){
    if (house_control[i].built_on) {
      timestr = asctime(localtime(&(house_control[i].built_on)));
      *(timestr + 10) = 0;
      strcpy(built_on, timestr);
    } else
      strcpy(built_on, "Sconosciuto");

    if (house_control[i].last_payment) {
      timestr = asctime(localtime(&(house_control[i].last_payment)));
      *(timestr + 10) = 0;
      strcpy(last_pay, timestr);
    } else
      strcpy(last_pay, "Nessuno");

    strcpy(own_name, NAME(house_control[i].owner));

 	 sprintf(buf, "%s%7d %7d  %-10s    %2d    %-12s %s %ld\r\n", buf,
  		 house_control[i].vnum, house_control[i].atrium, built_on,
 		 house_control[i].num_of_guests, CAP(own_name), last_pay,
 		 house_control[i].pickdoor);


    if (house_control[i].num_of_guests) {
      strcat(buf, "     Ospiti: ");
      for (j = 0; j < MIN(house_control[i].num_of_guests,MAX_GUESTS); j++) {
	sprintf(buf2, "%s ", NAME(house_control[i].guests[j]));
	strcat(buf, CAP(buf2));
      }
      strcat(buf, "\r\n");
    }
   }//atoi
  }
  send_to_char(buf, ch);
}



void hcontrol_build_house(struct char_data * ch, char *arg)
{
  char arg1[MAX_INPUT_LENGTH];
  struct house_control_rec temp_house;
  room_vnum virt_house, virt_atrium;
  room_rnum real_house, real_atrium;
  sh_int exit_num;
  long owner;

  if (num_of_houses >= MAX_HOUSES) {
    send_to_char("Max houses already defined.\r\n", ch);
    return;
  }

  /* first arg: house's vnum */
  arg = one_argument(arg, arg1);
  if (!*arg1) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  virt_house = atoi(arg1);
  if ((real_house = real_room(virt_house)) < 0) {
    send_to_char("La stanza non esiste.\r\n", ch);
    return;
  }
  if ((find_house(virt_house)) >= 0) {
    send_to_char("La casa gia' esiste.\r\n", ch);
    return;
  }

  /* second arg: direction of house's exit */
  arg = one_argument(arg, arg1);
  if (!*arg1) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  if ((exit_num = search_block(arg1, dirs, FALSE)) < 0) {
    sprintf(buf, "'%s' non e' una direzione valida.\r\n", arg1);
    send_to_char(buf, ch);
    return;
  }
  if (TOROOM(real_house, exit_num) == NOWHERE) {
    sprintf(buf, "Non c'e' nessuna uscita %s dalla stanza %d.\r\n", dirs[exit_num],
	    virt_house);
    send_to_char(buf, ch);
    return;
  }

  real_atrium = TOROOM(real_house, exit_num);
  virt_atrium = world[real_atrium].number;

  if (TOROOM(real_atrium, rev_dir[exit_num]) != real_house) {
    send_to_char("L'uscita della casa deve avere 2 porte.\r\n", ch);
    return;
  }

  /* third arg: player's name */
  arg = one_argument(arg, arg1);
  if (!*arg1) {
    send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  if ((owner = get_id_by_name(arg1)) < 0) {
    sprintf(buf, "Giocatore sconosciuto '%s'.\r\n", arg1);
    send_to_char(buf, ch);
    return;
  }

  temp_house.mode = HOUSE_PRIVATE;
  temp_house.vnum = virt_house;
  temp_house.atrium = virt_atrium;
  temp_house.exit_num = exit_num;
  temp_house.built_on = time(0);
  temp_house.last_payment = 0;
  temp_house.owner = owner;
  temp_house.num_of_guests = 0;
  temp_house.pickdoor = 100;

  house_control[num_of_houses++] = temp_house;

  SET_BIT(ROOM_FLAGS(real_house), ROOM_HOUSE | ROOM_PRIVATE);
  SET_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);
  House_crashsave(virt_house);

  send_to_char("La casa e' costruita. Che lavoraccio!\r\n", ch);
  House_save_control();
}


 void hcontrol_pick_house(struct char_data * ch, char *arg)
 {
   int i;
   char arg1[MAX_INPUT_LENGTH];
   char pick[MAX_INPUT_LENGTH];

   arg = one_argument(arg, arg1);
   arg = one_argument(arg, pick);

   if (!*arg1)
 	 send_to_char(HCONTROL_FORMAT, ch);
   else if ((i = find_house(atoi(arg1))) < 0)
 	 send_to_char("Casa sconosciuta.\r\n", ch);
   else if (!*pick || !is_number(pick))
 	 send_to_char("A che valore lo setti?.\r\n", ch);
   else {
 	 sprintf(buf, "Il livello di pickdoor della casa %s e' ora %s.\r\n", arg1, pick);
 	 mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
 	 house_control[i].pickdoor = atoi(pick);
 	 House_save_control();
 	 sprintf(buf, "Il livello di pickdoor della casa %s e' ora %s.\r\n", arg1, pick);
 	 send_to_char(buf,ch);
 	 }
 }

void hcontrol_destroy_house(struct char_data * ch, char *arg)
{
  int i, j;
  room_rnum real_atrium, real_house;

  if (!*arg) {
    if (ch!=0) send_to_char(HCONTROL_FORMAT, ch);
    return;
  }
  if ((i = find_house(atoi(arg))) < 0) {
    if (ch!=0) send_to_char("Casa sconosciuta.\r\n", ch);
    return;
  }
  if ((real_atrium = real_room(house_control[i].atrium)) < 0)
    log("SYSERR: House had invalid atrium!");
  else
    REMOVE_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);

  if ((real_house = real_room(house_control[i].vnum)) < 0)
    log("SYSERR: House had invalid vnum!");
  else
    REMOVE_BIT(ROOM_FLAGS(real_house),
	       ROOM_HOUSE | ROOM_PRIVATE | ROOM_HOUSE_CRASH);

  House_delete_file(house_control[i].vnum);

  for (j = i; j < num_of_houses - 1; j++)
    house_control[j] = house_control[j + 1];

  num_of_houses--;

  if (ch!=0) send_to_char("House deleted.\r\n", ch);
  House_save_control();

  /*
   * Now, reset the ROOM_ATRIUM flag on all existing houses' atriums,
   * just in case the house we just deleted shared an atrium with another
   * house.  --JE 9/19/94
   */
  for (i = 0; i < num_of_houses; i++)
    if ((real_atrium = real_room(house_control[i].atrium)) >= 0)
      SET_BIT(ROOM_FLAGS(real_atrium), ROOM_ATRIUM);
}


void hcontrol_pay_house(struct char_data * ch, char *arg)
{
  int i;

  if (!*arg)
    send_to_char(HCONTROL_FORMAT, ch);
  else if ((i = find_house(atoi(arg))) < 0)
    send_to_char("Casa sconosciuta.\r\n", ch);
  else {
    sprintf(buf, "Pagamento per la casa %s dovuto a %s.", arg, GET_NAME(ch));
    mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);

    house_control[i].last_payment = time(0);
    House_save_control();
    send_to_char("Pagamento registrato.\r\n", ch);
  }
}

void hcontrol_clean_house(struct char_data * ch)
{
  int i;

  if ((i=House_undef_clean()))
    {
    sprintf(buf, "Pulita %d <UNDEF> da case.\r\n", i);
    send_to_char(buf, ch);
    }
  else send_to_char("Niente da pulire.\r\n", ch);
}

void hcontrol_propietario_house(struct char_data *ch, char *arg)
{
  long room, house;
  struct char_data *vict;
  
  room = IN_ROOM(ch);
	
  if (room == -1) {
	send_to_char("Non puoi farlo qui.\r\n", ch);
	return;
  }
	
  if (!*arg) {
	send_to_char("A chi la trasferisci la proprieta' di questa casa?\r\n", ch);
	return;
  }

  if (!(vict = get_char_room_vis(ch, arg))) {
	send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
	return;
  }

  if ((house = find_house(world[room].number)) == -1) {
 	send_to_char("Questa non e' una casa.\r\n", ch);
	return;
  }

 
 house_control[house].owner = GET_IDNUM(vict);
 House_save_control();
 
 send_to_char("Proprietario cambiato.\r\n", ch);
	
}

/* The hcontrol command itself, used by imms to create/destroy houses */
ACMD(do_hcontrol)
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

  half_chop(argument, arg1, arg2);

  if (is_abbrev(arg1, "costruisci"))
    hcontrol_build_house(ch, arg2);
  else if (is_abbrev(arg1, "distruggi"))
    hcontrol_destroy_house(ch, arg2);
  else if (is_abbrev(arg1, "paghi"))
    hcontrol_pay_house(ch, arg2);
  else if (is_abbrev(arg1, "mostra"))
    hcontrol_list_houses(ch, arg2);
  else if (is_abbrev(arg1, "pulisci"))
    hcontrol_clean_house(ch);
  else if (is_abbrev(arg1, "proteggi"))
 	 hcontrol_pick_house(ch, arg2);
  else if (is_abbrev(arg1, "proprietario"))
 	 hcontrol_propietario_house(ch, arg2);
  else
    send_to_char(HCONTROL_FORMAT, ch);
}



/* The house command, used by mortal house owners to assign guests */
ACMD(do_house)
{
  int i, j, id;
  char *temp;
  char arg1[MAX_INPUT_LENGTH];

  one_argument(argument, arg1);

  if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_HOUSE))
    send_to_char("Devi essere nella tua casa per accettare ospiti.\r\n", ch);
  else if ((i = find_house(world[ch->in_room].number)) < 0)
    send_to_char("Um.. questa casa sembra non funzionare.\r\n", ch);
  else if (GET_IDNUM(ch) != house_control[i].owner)
    send_to_char("Solo il proprietario puo' decidere gli ospiti.\r\n", ch);
  else if (!*arg1) {
    send_to_char("Ospiti della Casa:\r\n", ch);
    if (house_control[i].num_of_guests == 0)
      send_to_char("  Nessuno.\r\n", ch);
    else
      for (j = 0; j < MIN(house_control[i].num_of_guests, MAX_GUESTS); j++) {
	strcpy(buf, NAME(house_control[i].guests[j]));
	send_to_char(strcat(CAP(buf), "\r\n"), ch);
      }
  } else if ((id = get_id_by_name(arg1)) < 0) {
    sprintf(buf, "Nessun giocatore con il nome %s", arg1);
    send_to_char(buf, ch);
  } else {
    for (j = 0; j < MIN(house_control[i].num_of_guests, MAX_GUESTS); j++)
      if (house_control[i].guests[j] == id) {
	for (; j < MIN(house_control[i].num_of_guests, MAX_GUESTS); j++)
	  house_control[i].guests[j] = house_control[i].guests[j + 1];
	house_control[i].num_of_guests--;
	House_save_control();
	send_to_char("Ospite rimosso.\r\n", ch);
	return;
      }
    if (house_control[i].num_of_guests < MAX_GUESTS)
      {
      j = house_control[i].num_of_guests++;
      house_control[i].guests[j] = id;
      House_save_control();
      send_to_char("Ospite aggiunto.\r\n", ch);
      }
    else
      {
      sprintf(buf, "Mi spiace, hai raggiunto il limite di ospiti per la tua casa, attualmente pari a %d\r\n", MAX_GUESTS);
      send_to_char(buf, ch);
      }
  }
}



/* Misc. administrative functions */


/* crash-save all the houses */
void House_save_all(void)
{
  int i;
  room_rnum real_house;

  for (i = 0; i < num_of_houses; i++)
    if ((real_house = real_room(house_control[i].vnum)) != NOWHERE)
      if (IS_SET(ROOM_FLAGS(real_house), ROOM_HOUSE_CRASH))
	House_crashsave(house_control[i].vnum);
}


/* note: arg passed must be house vnum, so there. */
int House_can_enter(struct char_data * ch, room_vnum house)
{
  int i, j;
  extern struct spell_info_type spell_info[];

  if (GET_LEVEL(ch) >= LVL_IMMORT || (i = find_house(house)) < 0)
    return 1;

  switch (house_control[i].mode) {
  case HOUSE_PRIVATE:
    if (GET_IDNUM(ch) == house_control[i].owner)
      return 1;
    for (j = 0; j < MIN(house_control[i].num_of_guests,MAX_GUESTS); j++)
      if (GET_IDNUM(ch) == house_control[i].guests[j])
	    return 1;
 		//pick p_locks
    if (CAN_LEARN_SKILL(ch, SKILL_PICK_LOCK) && GET_SKILL(ch, SKILL_PICK_LOCK)>70) {
      if (!can_do_event(ch, LAST_PICK, 60)) 
        return 0;
	    send_to_char("Tenti di entrare nell'edificio.....\r\n", ch);
	    if (number(1, GET_SKILL(ch, SKILL_PICK_LOCK) + (dex_app_skill[GET_DEX(ch)].p_locks - 10))
			    >= house_control[i].pickdoor) {
		    send_to_char("Riesci ad entrare nell' edificio grazie alla tua abilita di scassinatore.\r\n\r\n", ch);
		    return 1;
 		  } 
 		  else {
			  send_to_char("....nulla da fare, ogni tuo tentativo e' vano.\r\n", ch);
			  //test_and_improve(ch, SKILL_PICK_LOCK, 100, 15, 0);
			  test_and_improve(ch, SKILL_PICK_LOCK, 100, 10, 0);
			}

    }
    return 0;
    break;
  }

  return 0;
}

int House_undef_clean()
{
  int i,j,k,toret=0;

  for (i=0; i < num_of_houses; i++)
    {
    j=0;
    while (j < MIN(house_control[i].num_of_guests, MAX_GUESTS))
      {
      if (!get_name_by_id(house_control[i].guests[j]))
        {
        for (k=j; k < MIN(house_control[i].num_of_guests, MAX_GUESTS)-1; k++)
          house_control[i].guests[k] = house_control[i].guests[k+1];
	house_control[i].num_of_guests--;
        sprintf(buf, "SYSLOG: Rimozione UNDEF in casa di %s[id=%ld]",
                get_name_by_id(house_control[i].owner), house_control[i].owner);
        mudlog(buf, BRF, LVL_IMPL, TRUE);
        toret++;
        }
      else j++;
      }
    }
  House_save_control();
  return toret;
}
void Elimina_Cantieri()
{
  int i;
  char vnum[20];
  long real_house;
  float diff_time;

  *vnum = '\0';
  for (i=0; i < num_of_houses; i++)
  {

	diff_time = (float) (time(0) - house_control[i].built_on) / SECS_PER_REAL_DAY;

    if (diff_time > 60) {
	
	  diff_time = (float) (time(0) - house_control[i].last_payment) / SECS_PER_REAL_DAY;
	    		
  	  if ((real_house = real_room(house_control[i].vnum)) < 0)
	     {
	     log("SYSERR: House had invalid vnum!");
		 return;
		 }

	  if (!strcmp(world[real_house].name,"Edificio in costruzione"))// || (diff_time > 61))
	  {
		world[real_house].name = strdup("Un cantiere abbandonato");
		world[real_house].description = strdup("Questo cantiere e' qui da cosi' tanto tempo ormai che e' caduto in disuso.\r\n"
		"Cumuli di terra e qualche trave testimoniano tristemente un'opera iniziata\r\n"
		"e mai terminata. Speriamo che l'architetto non sia scappato con i soldi!\r\n");
		//redit_save_to_disk(world[real_house].zone);
		sprintf(vnum,"%d",house_control[i].vnum);
        	hcontrol_destroy_house(0, vnum);
        	sprintf(buf,"House %d distrutta. Limite di tempo di costruzione o pagamento non effettuato.",house_control[i].vnum);
        	log(buf);
	  }
	}
  }
}
