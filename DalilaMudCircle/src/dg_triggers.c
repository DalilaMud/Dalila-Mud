/**************************************************************************
*  File: triggers.c                                                       *
*                                                                         *
*  Usage: contains all the trigger functions for scripts.                 *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Death's Gate MUD is based on CircleMUD, Copyright (C) 1993, 94.        *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
*                                                                         *
*  $Author: galion $
*  $Date: 1996/08/05 23:32:08 $
*  $Revision: 3.9 $
**************************************************************************/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "dg_scripts.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"

extern struct room_data *world;
extern char *dirs[];



/* external functions from scripts.c */
void add_var(struct trig_var_data **var_list, char *name, char *value);
int script_driver(void *go, trig_data *trig, int type, int mode);
char *matching_quote(char *p);
char *str_str(char *cs, char *ct);


/* mob trigger types */
char *trig_types[] = {
  "Global", 
  "Random",
  "Command",
  "Speech",
  "Act",
  "Death",
  "Greet",
  "Greet-All",
  "Entry",
  "Receive",
  "Fight",
  "HitPrcnt",
  "Bribe",
  "Load",	//Orione
  "Memory", //Orione
  "\n"
};


/* obj trigger types */
char *otrig_types[] = {
  "Global",
  "Random",
  "Command",
  "UNUSED",
  "UNUSED",
  "Timer",	//Orione
  "Get",
  "Drop",
  "Give",
  "Wear",
  "UNUSED",
  "UNUSED",
  "UNUSED",
  "Load",	//Orione
  "\n"
};


/* wld trigger types */
char *wtrig_types[] = {
  "Global",
  "Random",
  "Command",
  "Speech",
  "UNUSED",
  "UNUSED",
  "Enter",
  "Drop",
  "UNUSED",
  "UNUSED",
  "UNUSED",
  "UNUSED",
  "UNUSED",
  "\n"
};


/*
 *  General functions used by several triggers
 */


/*
 * Copy first phrase into first_arg, returns rest of string
 */
char *one_phrase(char *arg, char *first_arg)
{
    skip_spaces(&arg);

    if (!*arg)
	*first_arg = '\0';

    else if (*arg == '"')
    {
	char *p, c;

	p = matching_quote(arg);
	c = *p;
	*p = '\0';
	strcpy(first_arg, arg + 1);
	if (c == '\0')
	    return p;
	else
	    return p + 1;
    }

    else
    {
	char *s, *p;

	s = first_arg;
	p = arg;
	
	while (*p && !isspace(*p) && *p != '"')
	    *s++ = *p++;

	*s = '\0';
	return p;
    }

    return arg;
}


int is_substring(char *sub, char *string)
{
    char *s;

    if ((s = str_str(string, sub)))
    {
	int len = strlen(string);
	int sublen = strlen(sub);
	
	/* check front */
	if ((s == string || isspace(*(s - 1)) || ispunct(*(s - 1))) &&

	    /* check end */
	    ((s + sublen == string + len) || isspace(s[sublen]) ||
	     ispunct(s[sublen])))
	    return 1;
    }

    return 0;
}


/*
 * return 1 if str contains a word or phrase from wordlist.
 * phrases are in double quotes (").
 * if wrdlist is NULL, then return 1, if str is NULL, return 0.
 */
int word_check(char *str, char *wordlist)
{
    char words[MAX_INPUT_LENGTH], phrase[MAX_INPUT_LENGTH], *s;

    strcpy(words, wordlist);
    
    for (s = one_phrase(words, phrase); *phrase; s = one_phrase(s, phrase))
	if (is_substring(phrase, str))
	    return 1;

    return 0;
}

	

/*
 *  mob triggers
 */

void random_mtrigger(char_data *ch)
{
  trig_data *t;

  phanlog("random_mtrigger");
  
  //if (!SCRIPT_CHECK(ch, MTRIG_RANDOM) || AFF_FLAGGED(ch, AFF_CHARM))
  if (!SCRIPT_CHECK(ch, MTRIG_RANDOM))
    return;

  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_RANDOM) && (number(1, 100) <= GET_TRIG_NARG(t))) {
      script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
      break;
    }
  }
}

void bribe_mtrigger(char_data *ch, char_data *actor, int amount)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("bribe_mt");
  
  //if (!SCRIPT_CHECK(ch, MTRIG_BRIBE) || AFF_FLAGGED(ch, AFF_CHARM))
  if (!SCRIPT_CHECK(ch, MTRIG_BRIBE))
    return;

  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_BRIBE) && (amount >= GET_TRIG_NARG(t))) {

      sprintf(buf, "%d", amount);
      add_var(&GET_TRIG_VARS(t), "amount", buf); 
      ADD_UID_VAR(buf, t, actor, "actor");
      script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
      break;
    }
  }
}


int greet_mtrigger(char_data *actor, int dir)
{
  trig_data *t;
  char_data *ch;
  char buf[MAX_INPUT_LENGTH];
  int rev_dir[] = { SOUTH, WEST, NORTH, EAST, DOWN, UP };
  phanlog("greet_mt");
  
  //PEPPE BUG?!?!?!
  if(dir==-1){
	//mudlog("ERRORE dir del trigger -1", NRM, LVL_IMMORT, TRUE);
	//return 1;
  dir=1;
  }
  for (ch = world[IN_ROOM(actor)].people; ch; ch = ch->next_in_room) {
    if (!SCRIPT_CHECK(ch, MTRIG_GREET | MTRIG_GREET_ALL) || 
	      !AWAKE(ch) || FIGHTING(ch) || (ch == actor)) 
	//|| AFF_FLAGGED(ch, AFF_CHARM))
      continue;
    
    for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
      if (((IS_SET(GET_TRIG_TYPE(t), MTRIG_GREET) && CAN_SEE(ch, actor)) ||
	          IS_SET(GET_TRIG_TYPE(t), MTRIG_GREET_ALL)) && 
	          !GET_TRIG_DEPTH(t) && (number(1, 100) <= GET_TRIG_NARG(t))) {
        add_var(&GET_TRIG_VARS(t), "direction", dirs[rev_dir[dir]]);
	      ADD_UID_VAR(buf, t, actor, "actor");
	      return script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
	      break;
      }
    }
  }
  phanlog("greet_mt - NIH");
  return 1;
}

//Orione Inizio
void greet_memory_mtrigger(char_data *actor)
{
	trig_data *t;
	char_data *ch;
	struct script_memory *mem;
	char buf[MAX_INPUT_LENGTH];
	int command_performed = 0;
	phanlog("greet_memory_mt");

	for (ch = world[IN_ROOM(actor)].people; ch; ch = ch->next_in_room) {
		if (!SCRIPT_MEM(ch) || !AWAKE(ch) || FIGHTING(ch) || (ch == actor) || AFF_FLAGGED(ch, AFF_CHARM))
		  continue;

		/* find memory line with command only */
		for (mem = SCRIPT_MEM(ch); mem && SCRIPT_MEM(ch); mem=mem->next) {
			if (GET_ID(actor)!=mem->id) 
			  continue;
			if (mem->cmd) {
				command_interpreter(ch, mem->cmd); /* no script */
				command_performed = 1;
				break;
			}
		}

		/* if a command was not performed execute the memory script */
		if (mem && !command_performed) {
			for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
				if (IS_SET(GET_TRIG_TYPE(t), MTRIG_MEMORY)
				     && CAN_SEE(ch, actor) 
				     && !GET_TRIG_DEPTH(t) 
				     && number(1, 100) <= GET_TRIG_NARG(t)) {
					ADD_UID_VAR(buf, t, actor, "actor");
					script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
					phanlog("greet_memory_mt < script_dr");
					break;
				} 
			}
		}
	
		/* delete the memory */
		if (mem) {
			if (SCRIPT_MEM(ch)==mem) {
				SCRIPT_MEM(ch) = mem->next;
			} 
			else {
				struct script_memory *prev;
				prev = SCRIPT_MEM(ch);
				while (prev->next != mem) prev = prev->next;
				prev->next = mem->next;
			}
			if (mem->cmd) 
			  free(mem->cmd);
			free(mem);
		}
	}
  phanlog("greet_memory_mt - NIH");
}
//Orione Fine

int entry_mtrigger(char_data *ch)
{
  trig_data *t;
  phanlog("entry_mt");
  
  //if (!SCRIPT_CHECK(ch, MTRIG_ENTRY) || AFF_FLAGGED(ch, AFF_CHARM))
    if (!SCRIPT_CHECK(ch, MTRIG_ENTRY))
    return 1;
  
  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_ENTRY) && (number(1, 100) <= GET_TRIG_NARG(t))){
      return script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
      break;
    }
  }
  return 1;
}

//Orione Inizio
void entry_memory_mtrigger(char_data *ch)
{
	trig_data *t;
	char_data *actor;
	struct script_memory *mem;
	char buf[MAX_INPUT_LENGTH];
  phanlog("entry_memory_mt");
  
	if (!SCRIPT_MEM(ch) || AFF_FLAGGED(ch, AFF_CHARM))
		return;

	for (actor = world[IN_ROOM(ch)].people; actor && SCRIPT_MEM(ch); actor = actor->next_in_room) {
		if (actor!=ch && SCRIPT_MEM(ch)) {
			for (mem = SCRIPT_MEM(ch); mem && SCRIPT_MEM(ch); mem = mem->next) {
				if (GET_ID(actor)==mem->id) {
					struct script_memory *prev;
					if (mem->cmd) command_interpreter(ch, mem->cmd);
					else {
						for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
							if (TRIGGER_CHECK(t, MTRIG_MEMORY) && (number(1, 100) <= GET_TRIG_NARG(t))){
								ADD_UID_VAR(buf, t, actor, "actor");
								script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
								phanlog("entry_memory_mt < script_dr");
								break;
							}
						}
					}
					
					/* delete the memory */
					if (SCRIPT_MEM(ch)==mem) {
						SCRIPT_MEM(ch) = mem->next;
					} else {
						prev = SCRIPT_MEM(ch);
						while (prev->next != mem) prev = prev->next;
						prev->next = mem->next;
					}
					if (mem->cmd) free(mem->cmd);
					free(mem);
				}
			} /* for (mem =..... */
		}
	}
}
//Orione Fine

int command_mtrigger(char_data *actor, char *cmd, char *argument)
{
  char_data *ch, *ch_next;
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("command_mt");
  
  for (ch = world[IN_ROOM(actor)].people; ch; ch = ch_next) {
    ch_next = ch->next_in_room;

    //if (SCRIPT_CHECK(ch, MTRIG_COMMAND) && !AFF_FLAGGED(ch, AFF_CHARM)) {
	if (SCRIPT_CHECK(ch, MTRIG_COMMAND)) {
      for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
	if (TRIGGER_CHECK(t, MTRIG_COMMAND) &&
	    !strn_cmp(GET_TRIG_ARG(t), cmd, strlen(cmd))) {
	  
	  ADD_UID_VAR(buf, t, actor, "actor");
	  skip_spaces(&argument);
	  add_var(&GET_TRIG_VARS(t), "arg", argument);
	  
	  if (script_driver(ch, t, MOB_TRIGGER, TRIG_NEW))
	    return 1;
	}
      }
    }
  }

  return 0;
}
  

void speech_mtrigger(char_data *actor, char *str)
{
  char_data *ch, *ch_next;
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("speech_mt");
  
  for (ch = world[IN_ROOM(actor)].people; ch; ch = ch_next)
  {
    ch_next = ch->next_in_room;

    if (SCRIPT_CHECK(ch, MTRIG_SPEECH) && AWAKE(ch))
	//&& !AFF_FLAGGED(ch, AFF_CHARM))
      for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) 
        if (TRIGGER_CHECK(t, MTRIG_SPEECH) &&
            ((GET_TRIG_NARG(t) && word_check(str, GET_TRIG_ARG(t))) ||
             (!GET_TRIG_NARG(t) && is_substring(GET_TRIG_ARG(t), str)))) {
          ADD_UID_VAR(buf, t, actor, "actor");
          add_var(&GET_TRIG_VARS(t), "speech", str);
          script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
          break;
        }
  }
}


void act_mtrigger(char_data *ch, char *str, char_data *actor, 
		  char_data *victim, obj_data *object,
		  obj_data *target, char *arg)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("act_mt");
  
  //if (SCRIPT_CHECK(ch, MTRIG_ACT) && !AFF_FLAGGED(ch, AFF_CHARM))
   if (SCRIPT_CHECK(ch, MTRIG_ACT))
    for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) 
      if (TRIGGER_CHECK(t, MTRIG_ACT) &&
          ((GET_TRIG_NARG(t) && word_check(str, GET_TRIG_ARG(t))) ||
           (!GET_TRIG_NARG(t) && is_substring(GET_TRIG_ARG(t), str)))) {
	
	if (actor)
	  ADD_UID_VAR(buf, t, actor, "actor");
	if (victim)
	  ADD_UID_VAR(buf, t, victim, "victim");
	if (object)
	  ADD_UID_VAR(buf, t, object, "object");
	if (target)
	  ADD_UID_VAR(buf, t, target, "target");
	//if (arg) {
	  //skip_spaces(&arg);
	  //add_var(&GET_TRIG_VARS(t), "arg", arg);
	  skip_spaces(&str);
	  add_var(&GET_TRIG_VARS(t), "arg", str);
	//}	  
	script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
	break;
      }	
}


void fight_mtrigger(char_data *ch)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("fight_mt");
  
  if (!SCRIPT_CHECK(ch, MTRIG_FIGHT) || !FIGHTING(ch))
  //|| AFF_FLAGGED(ch, AFF_CHARM))
    return;
  
  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_FIGHT) &&
	(number(1, 100) <= GET_TRIG_NARG(t))){

      ADD_UID_VAR(buf, t, FIGHTING(ch), "actor")
      script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
      break;
    }
  }
}


void hitprcnt_mtrigger(char_data *ch)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("hitprcnt_mt");
  
  if (!SCRIPT_CHECK(ch, MTRIG_HITPRCNT) || !FIGHTING(ch) )
  //|| AFF_FLAGGED(ch, AFF_CHARM))
    return;
  
  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_HITPRCNT) && GET_MAX_HIT(ch) &&
	(((GET_HIT(ch) * 100) / GET_MAX_HIT(ch)) <= GET_TRIG_NARG(t))) {

      ADD_UID_VAR(buf, t, FIGHTING(ch), "actor")
      script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
      break;
    }
  }
}


int receive_mtrigger(char_data *ch, char_data *actor, obj_data *obj)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("receive_mt");

  //if (!SCRIPT_CHECK(ch, MTRIG_RECEIVE) || AFF_FLAGGED(ch, AFF_CHARM))
    if (!SCRIPT_CHECK(ch, MTRIG_RECEIVE))
    return 1;
  
  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_RECEIVE) &&
	(number(1, 100) <= GET_TRIG_NARG(t))){

      ADD_UID_VAR(buf, t, actor, "actor");
      ADD_UID_VAR(buf, t, obj, "object");
      return script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}


int death_mtrigger(char_data *ch, char_data *actor)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("death_mt");
  
  //if (!SCRIPT_CHECK(ch, MTRIG_DEATH) || AFF_FLAGGED(ch, AFF_CHARM))
  if (!SCRIPT_CHECK(ch, MTRIG_DEATH))
    return 1;
  
  for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
    if (TRIGGER_CHECK(t, MTRIG_DEATH)) {

      if (actor)
	ADD_UID_VAR(buf, t, actor, "actor");
      return script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}

/* Orione Inizio */
void load_mtrigger(char_data *ch) 
{
	trig_data *t;
	phanlog("load_mt");

	if (!SCRIPT_CHECK(ch, MTRIG_LOAD))
		return;

	for (t = TRIGGERS(SCRIPT(ch)); t; t = t->next) {
		if (TRIGGER_CHECK(t, MTRIG_LOAD) && (number(1, 100)<=GET_TRIG_NARG(t))) {
			script_driver(ch, t, MOB_TRIGGER, TRIG_NEW);
			break;
		}
	}
}
/* Orione Fine */

/*
 *  object triggers
 */

void random_otrigger(obj_data *obj)
{
  trig_data *t;

  phanlog("random_otrigger");

  if (!SCRIPT_CHECK(obj, OTRIG_RANDOM))
    return;

  for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
    if (TRIGGER_CHECK(t, OTRIG_RANDOM) && (number(1, 100) <= GET_TRIG_NARG(t))) {
      script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
      break;
    }
  }
  phanlog("random_ot - NIH");
}

/* Orione Inizio */
void timer_otrigger(struct obj_data *obj) 
{
	trig_data *t;
  phanlog("timer_mt");
  
	if (!SCRIPT_CHECK(obj, OTRIG_TIMER))
		return;
	
	for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
		if (TRIGGER_CHECK(t, OTRIG_TIMER)) {
			script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
			break;
		}
	}

	return;
}
/* Orione Fine */ 

int get_otrigger(obj_data *obj, char_data *actor)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("get_ot");
  
  if (!SCRIPT_CHECK(obj, OTRIG_GET))
    return 1;

  for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
    if (TRIGGER_CHECK(t, OTRIG_GET) && (number(1, 100) <= GET_TRIG_NARG(t))) {
      ADD_UID_VAR(buf, t, actor, "actor");
      return script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}


/* checks for command trigger on specific object. assumes obj has cmd trig */
int cmd_otrig(obj_data *obj, char_data *actor, char *cmd,
	      char *argument, int type)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("cmd_ot");
  
  if (obj && SCRIPT_CHECK(obj, OTRIG_COMMAND))
    for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
      if (TRIGGER_CHECK(t, OTRIG_COMMAND) && IS_SET(GET_TRIG_NARG(t), type) &&
	  !strn_cmp(GET_TRIG_ARG(t), cmd, strlen(GET_TRIG_ARG(t)))) {	  
	
	ADD_UID_VAR(buf, t, actor, "actor");
	skip_spaces(&argument);
	add_var(&GET_TRIG_VARS(t), "arg", argument);
	
	if (script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW))
	  return 1;
      }
    }
  phanlog("cmd_ot - NIH");
  return 0;
}


int command_otrigger(char_data *actor, char *cmd, char *argument)
{
  obj_data *obj;
  int i;

  for (i = 0; i < NUM_WEARS; i++)
    if (cmd_otrig(GET_EQ(actor, i), actor, cmd, argument, OCMD_EQUIP))
      return 1;
  
  for (obj = actor->carrying; obj; obj = obj->next_content)
    if (cmd_otrig(obj, actor, cmd, argument, OCMD_INVEN))
      return 1;

  for (obj = world[IN_ROOM(actor)].contents; obj; obj = obj->next_content)
    if (cmd_otrig(obj, actor, cmd, argument, OCMD_ROOM))
      return 1;

  return 0;
}


int wear_otrigger(obj_data *obj, char_data *actor, int where)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("wear_ot");
  
  if (!SCRIPT_CHECK(obj, OTRIG_WEAR))
    return 1;

  for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
    if (TRIGGER_CHECK(t, OTRIG_WEAR)) {
      ADD_UID_VAR(buf, t, actor, "actor");
      return script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}


int drop_otrigger(obj_data *obj, char_data *actor)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("drop_ot");
  
  if (!SCRIPT_CHECK(obj, OTRIG_DROP))
    return 1;

  for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
    if (TRIGGER_CHECK(t, OTRIG_DROP) && (number(1, 100) <= GET_TRIG_NARG(t))) {
      ADD_UID_VAR(buf, t, actor, "actor");
      return script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}


int give_otrigger(obj_data *obj, char_data *actor, char_data *victim)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("give_ot");
  
  if (!SCRIPT_CHECK(obj, OTRIG_GIVE))
    return 1;

  for (t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
    if (TRIGGER_CHECK(t, OTRIG_GIVE) && (number(1, 100) <= GET_TRIG_NARG(t))) {
      ADD_UID_VAR(buf, t, actor, "actor");
      ADD_UID_VAR(buf, t, victim, "victim");
      return script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}

/* Orione Inizio */
void load_otrigger(obj_data *obj) 
{
	trig_data *t;
  phanlog("load_ot");
  
	if (!SCRIPT_CHECK(obj, OTRIG_LOAD));
		return;

	for(t = TRIGGERS(SCRIPT(obj)); t; t = t->next) {
		if (TRIGGER_CHECK(t, OTRIG_LOAD) && (number(1, 100)<=GET_TRIG_NARG(t))) {
			script_driver(obj, t, OBJ_TRIGGER, TRIG_NEW);
			break;
		}
	}
}
/* Orione Fine */

/*
 *  world triggers
 */

void random_wtrigger(struct room_data *room)
{
  trig_data *t;

  phanlog("random_wtrigger");

  if (!SCRIPT_CHECK(room, WTRIG_RANDOM))
    return;

  for (t = TRIGGERS(SCRIPT(room)); t; t = t->next) {
    if (TRIGGER_CHECK(t, WTRIG_RANDOM) &&
	(number(1, 100) <= GET_TRIG_NARG(t))) {
      script_driver(room, t, WLD_TRIGGER, TRIG_NEW);
      break;
    }
  }
}


int enter_wtrigger(struct room_data *room, char_data *actor, int dir)
{
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  int rev_dir[] = { SOUTH, WEST, NORTH, EAST, DOWN, UP };
  phanlog("enter_wt");
  
  if (!SCRIPT_CHECK(room, WTRIG_ENTER))
    return 1;

  for (t = TRIGGERS(SCRIPT(room)); t; t = t->next) {
    if (TRIGGER_CHECK(t, WTRIG_ENTER) && 
	(number(1, 100) <= GET_TRIG_NARG(t))) {
      add_var(&GET_TRIG_VARS(t), "direction", dirs[rev_dir[dir]]);
      ADD_UID_VAR(buf, t, actor, "actor");
      return script_driver(room, t, WLD_TRIGGER, TRIG_NEW);
    }
  }

  return 1;
}


int command_wtrigger(char_data *actor, char *cmd, char *argument)
{
  struct room_data *room;
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("command_wt");
  
  if (!actor || !SCRIPT_CHECK(&world[IN_ROOM(actor)], WTRIG_COMMAND))
    return 0;

  room = &world[IN_ROOM(actor)];
  for (t = TRIGGERS(SCRIPT(room)); t; t = t->next) {
    if (TRIGGER_CHECK(t, WTRIG_COMMAND) && 
	!strn_cmp(GET_TRIG_ARG(t), cmd, strlen(cmd))) {

      ADD_UID_VAR(buf, t, actor, "actor");
      skip_spaces(&argument);
      add_var(&GET_TRIG_VARS(t), "arg", argument);
      
      return script_driver(room, t, WLD_TRIGGER, TRIG_NEW);
    }
  }
  
  return 0;
}


void speech_wtrigger(char_data *actor, char *str)
{
  struct room_data *room;
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("speech_wt");
  
  if (!actor || !SCRIPT_CHECK(&world[IN_ROOM(actor)], WTRIG_SPEECH))
    return;

  room = &world[IN_ROOM(actor)];
  for (t = TRIGGERS(SCRIPT(room)); t; t = t->next) 
    if (TRIGGER_CHECK(t, WTRIG_SPEECH) &&
	((GET_TRIG_NARG(t) && word_check(str, GET_TRIG_ARG(t))) ||
         (!GET_TRIG_NARG(t) && is_substring(GET_TRIG_ARG(t), str)))) {

      ADD_UID_VAR(buf, t, actor, "actor");
      add_var(&GET_TRIG_VARS(t), "speech", str);
      script_driver(room, t, WLD_TRIGGER, TRIG_NEW);
      break;
    }	
}

int drop_wtrigger(obj_data *obj, char_data *actor)
{
  struct room_data *room;
  trig_data *t;
  char buf[MAX_INPUT_LENGTH];
  phanlog("drop_wt");
  
  if (!actor || !SCRIPT_CHECK(&world[IN_ROOM(actor)], WTRIG_DROP))
    return 1;

  room = &world[IN_ROOM(actor)];
  for (t = TRIGGERS(SCRIPT(room)); t; t = t->next) 
    if (TRIGGER_CHECK(t, WTRIG_DROP) &&
	(number(1, 100) <= GET_TRIG_NARG(t))) {	
      
      ADD_UID_VAR(buf, t, actor, "actor");
      ADD_UID_VAR(buf, t, obj, "object");
      return script_driver(room, t, WLD_TRIGGER, TRIG_NEW);
      break;
    }	

  return 1;
}
