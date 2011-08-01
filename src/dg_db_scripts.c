/* ************************************************************************
*  File: db.script.c                             Part of Death's Gate MUD *
*                                                                         *
*  Usage: Contains routines to handle db functions for scripts and trigs  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Death's Gate MUD is based on CircleMUD, Copyright (C) 1993, 94.        *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
*                                                                         *
*  $Author: egreen $
*  $Date: 1996/09/30 21:27:54 $
*  $Revision: 3.7 $
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "dg_scripts.h"
#include "utils.h"
#include "db.h"
#include "handler.h"
#include "dg_event.h"
#include "comm.h"

void trig_data_copy(trig_data *this, const trig_data *trg);
void trig_data_free(trig_data *this);

extern struct index_data **trig_index;
extern int top_of_trigt;

extern struct index_data *mob_index;
extern struct index_data *obj_index;

extern void half_chop(char *string, char *arg1, char *arg2);
extern long long int asciiflag_conv(char *flag);

void parse_trigger(FILE *trig_f, int nr)
{
    int t[2], k, attach_type, i;
    char line[256], flags[256], greatline[2048];
//	char *cmds, *s ;
    struct cmdlist_element *cle;
    index_data *index;
    trig_data *trig;

    CREATE(trig, trig_data, 1);
    CREATE(index, index_data, 1);

    index->virtual = nr;
    index->number = 0;
    index->func = NULL;
    index->proto = trig;

    sprintf(buf2, "trig vnum %d", nr);

    trig->nr = top_of_trigt;
    trig->name = fread_string(trig_f, buf2);

    get_line(trig_f, line);
    k = sscanf(line, "%d %s %d", &attach_type, flags, t);
    trig->attach_type = (byte)attach_type;
    trig->trigger_type = asciiflag_conv(flags);
    trig->narg = (k == 3) ? t[0] : 0;

    trig->arglist = fread_string(trig_f, buf2);
/*
    cmds = s = fread_string(trig_f, buf2);

    CREATE(trig->cmdlist, struct cmdlist_element, 1);
    trig->cmdlist->cmd = str_dup(strtok(s, "\n\r"));
    cle = trig->cmdlist;

    while ((s = strtok(NULL, "\n\r"))) {
	CREATE(cle->next, struct cmdlist_element, 1);
	cle = cle->next;
	cle->cmd = str_dup(s);
    }

    free(cmds);*/

	//Lance - Shade
	 //strcpy(greatline,"");
 	 get_line(trig_f, greatline);
	 CREATE(trig->cmdlist, struct cmdlist_element, 1);
	 trig->cmdlist->cmd = str_dup(greatline);
	 cle = trig->cmdlist;
     i=0;
//	 sprintf(buf,"SYSINFO: %s", greatline);
//	 log(buf);
	 while (str_cmp(strtok(greatline, "\n\r"),"~")!=0) {
	 i++;
	 //strcpy(greatline,"");
	 get_line(trig_f, greatline);
	 if (str_cmp(strtok(greatline, "\n\r"),"~")!=0) {
	   CREATE(cle->next, struct cmdlist_element, 1);
	   cle = cle->next;
	   cle->cmd = str_dup(greatline);
	  }
	// if (i==5000) {
	//   sprintf(buf,"SYSERR: trigger corrupted %d", index->virtual);
	//   log(buf);  }

	 }
    trig_index[top_of_trigt++] = index;
}


/*
 * create a new trigger from a prototype.
 * nr is the real number of the trigger.
 */
trig_data *read_trigger(int nr)
{
    index_data *index;
    trig_data *trig;

    if (nr >= top_of_trigt) return NULL;
    if ((index = trig_index[nr]) == NULL)
	return NULL;

    CREATE(trig, trig_data, 1);
    trig_data_copy(trig, index->proto);

    index->number++;

    return trig;
}


/* release memory allocated for a variable list */
void free_varlist(struct trig_var_data *vd)
{
    struct trig_var_data *i, *j;

    for (i = vd; i;) {
	j = i;
	i = i->next;
	if (j->name)
	    free(j->name);
	if (j->value)
	    free(j->value);
	free(j);
    }
}


/* release memory allocated for a script */
void free_script(struct script_data *sc)
{
    trig_data *t1, *t2;

    for (t1 = TRIGGERS(sc); t1 ;) {
	t2 = t1;
	t1 = t1->next;
	trig_data_free(t2);
    }

    free_varlist(sc->global_vars);

    free(sc);
}

void trig_data_init(trig_data *this)
{
    this->nr = NOTHING;
    this->data_type = 0;
    this->name = NULL;
    this->trigger_type = 0;
    this->cmdlist = NULL;
    this->curr_state = NULL;
    this->narg = 0;
    this->arglist = NULL;
    this->depth = 0;
    this->wait_event = NULL;
    this->purged = FALSE;
    this->var_list = NULL;

    this->next = NULL;
}


void trig_data_copy(trig_data *this, const trig_data *trg)
{
    trig_data_init(this);

    this->nr = trg->nr;
    this->attach_type = trg->attach_type;
    this->data_type = trg->data_type;
    this->name = str_dup(trg->name);
    this->trigger_type = trg->trigger_type;
    this->cmdlist = trg->cmdlist;
    this->narg = trg->narg;
    if (trg->arglist) this->arglist = str_dup(trg->arglist);
}


void trig_data_free(trig_data *this)
{
/*    struct cmdlist_element *i, *j;*/

    free(this->name);

    /*
     * The command list is a memory leak right now!
     *
    if (cmdlist != trigg->cmdlist || this->proto)
	for (i = cmdlist; i;) {
	    j = i;
	    i = i->next;
	    free(j->cmd);
	    free(j);
	}
	*/

    free(this->arglist);

    free_varlist(this->var_list);

    if (this->wait_event)
	remove_event(this->wait_event);

    free(this);
}

/* for mobs and rooms: */
void dg_read_trigger(FILE *fp, void *proto, int type)
{
  char line[256];
  char junk[8];
  int vnum, rnum, count;
  char_data *mob;
  room_data *room;
  struct trig_proto_list *trg_proto, *new_trg;

  get_line(fp, line);
  count = sscanf(line,"%s %d",junk,&vnum);

  if (count != 2) {
    /* should do a better job of making this message */
    log("TRIGERR: Error assigning trigger!");
    return;
  }

  rnum = real_trigger(vnum);
  
  if (rnum<0) {
  	switch(type) {
  		case MOB_TRIGGER:
  		  mob = (char_data *)proto;
  		  sprintf(line,"TRIGERR: Trigger vnum #%d non esiste, evocato dal mob #%d.", vnum, GET_MOB_VNUM(mob));
        log(line);
  		  break;
  		
  		case WLD_TRIGGER:
  		  room = (room_data *)proto;
  		  sprintf(line,"TRIGERR: Trigger vnum #%d non esiste, evocato dalla room #%d.", vnum, room->number);
        log(line);
  		  break;
  		  
  		default:
  		  sprintf(line,"TRIGERR: Trigger vnum #%d tipo errato.", vnum);
        log(line);
  	}
  		
    sprintf(line,"TRIGERR: Trigger vnum #%d asked for but non-existant!", vnum);
    log(line);
    return;
  }

  switch(type) {
    case MOB_TRIGGER:
      CREATE(new_trg, struct trig_proto_list, 1);
      new_trg->vnum = vnum;
      new_trg->next = NULL;

      mob = (char_data *)proto;
      trg_proto = mob->proto_script;
      if (!trg_proto) {
        mob->proto_script = trg_proto = new_trg;
      } 
      else {
        while (trg_proto->next) trg_proto = trg_proto->next;
        trg_proto->next = new_trg;
      }
      break;
    case WLD_TRIGGER:
      CREATE(new_trg, struct trig_proto_list, 1);
      new_trg->vnum = vnum;
      new_trg->next = NULL;
      room = (room_data *)proto;
      trg_proto = room->proto_script;
      if (!trg_proto) {
        room->proto_script = trg_proto = new_trg;
      } 
      else {
        while (trg_proto->next) trg_proto = trg_proto->next;
        trg_proto->next = new_trg;
      }

      if (rnum>=0) {
        if (!(room->script))
          CREATE(room->script, struct script_data, 1);
        add_trigger(SCRIPT(room), read_trigger(rnum), -1);
        sprintf(line,"TRIGINFO: Put T#%7d to R#%8d", trg_proto->vnum, room->number);
        log(line);
      } 
      else {
        sprintf(line,"TRIGERR: ROOM not-exist fail to Put T#%7d to R#%8d", vnum, room->number);
        log(line);
      }
      break;
    default:
      sprintf(line,"TRIGERR: T#%7d assigned to non-mob/obj/room", vnum);
      log(line);
  }
}

void dg_obj_trigger(char *line, struct obj_data *obj)
{
  char junk[8];
  int vnum, rnum, count;
  struct trig_proto_list *trg_proto, *new_trg;

  count = sscanf(line,"%s %d",junk,&vnum);

  if (count != 2) {
    /* should do a better job of making this message */
    log("SYSERR: Error assigning trigger!");
    return;
  }

  rnum = real_trigger(vnum);
  if (rnum<0) {
    sprintf(line,"SYSERR: Trigger vnum #%d asked (#%d Obj) for but non-existant!", vnum, GET_OBJ_VNUM(obj));
    log(line);
    return;
  }

  CREATE(new_trg, struct trig_proto_list, 1);
  new_trg->vnum = vnum;
  new_trg->next = NULL;

  trg_proto = obj->proto_script;
  if (!trg_proto) {
    obj->proto_script = trg_proto = new_trg;
  } 
  else {
    while (trg_proto->next) trg_proto = trg_proto->next;
    trg_proto->next = new_trg;
  }
}

void assign_triggers(void *i, int type)
{
  char_data *mob;
  obj_data *obj;
  struct room_data *room;
  int rnum;
  char buf[256];
  struct trig_proto_list *trg_proto;

  switch (type)
  {
    case MOB_TRIGGER:
      mob = (char_data *)i;
      trg_proto = mob->proto_script;
      while (trg_proto) {
        rnum = real_trigger(trg_proto->vnum);
        if (rnum==-1) {
          sprintf(buf,"TRIGERR: T#%7d non-existant, for M#%8d", trg_proto->vnum, mob_index[mob->nr].virtual);
          log(buf);
        } else {
          if (!SCRIPT(mob))
            CREATE(SCRIPT(mob), struct script_data, 1);
          add_trigger(SCRIPT(mob), read_trigger(rnum), -1);
          sprintf(buf,"TRIGINFO: Put T#%7d to M#%8d", trg_proto->vnum, mob_index[mob->nr].virtual);
          log(buf);
        }
        trg_proto = trg_proto->next;
      }
      break;
    case OBJ_TRIGGER:
      obj = (obj_data *)i;
      trg_proto = obj->proto_script;
      while (trg_proto) {
        rnum = real_trigger(trg_proto->vnum);
        if (rnum==-1) {
          sprintf(buf,"TRIGERR: T#%7d non-existant for O#%8d", trg_proto->vnum, obj_index[obj->item_number].virtual);
          log(buf);
        } else {
          if (!SCRIPT(obj))
            CREATE(SCRIPT(obj), struct script_data, 1);
          add_trigger(SCRIPT(obj), read_trigger(rnum), -1);
          sprintf(buf,"TRIGINFO: Put T#%7d to O#%7d", trg_proto->vnum, obj_index[obj->item_number].virtual);
          log(buf);
        }
        trg_proto = trg_proto->next;
      }
      break;
    case WLD_TRIGGER:
      room = (struct room_data *)i;
      trg_proto = room->proto_script;
      while (trg_proto) {
        rnum = real_trigger(trg_proto->vnum);
        if (rnum==-1) {
          sprintf(buf,"TRIGERR: T#%7d non-exist for R#%8d", trg_proto->vnum, room->number);
          log(buf);
        } else {
          if (!SCRIPT(room))
            CREATE(SCRIPT(room), struct script_data, 1);
          add_trigger(SCRIPT(room), read_trigger(rnum), -1);
          sprintf(buf,"TRIGINFO: Put T#%7d to R#%8d", trg_proto->vnum, room->number);
          log(buf);
        }
        trg_proto = trg_proto->next;
      }
      break;
    default:
      log("TRIGERR: unknown type for assign_triggers()");
      break;
  }
}
