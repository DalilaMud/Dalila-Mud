/**************************************************************************
*  File: objcmd.c                                                         *
*  Usage: contains the command_interpreter for objects,                   *
*         object commands.                                                *
*                                                                         *
*                                                                         *
*  $Author: galion $
*  $Date: 1996/08/04 23:10:16 $
*  $Revision: 3.8 $
**************************************************************************/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "screen.h"
#include "dg_scripts.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"

extern struct room_data *world;
extern struct index_data *obj_index;
extern const char *dirs[];	//Orione

char_data *get_char_by_obj(obj_data *obj, char *name);
obj_data *get_obj_by_obj(obj_data *obj, char *name);
void sub_write(char *arg, char_data *ch, byte find_invis, int targets);
void die(struct char_data * ch, struct char_data *killer);
long asciiflag_conv(char *flag); //Orione
room_data *get_room(char *name);	//Orione

#define OCMD(name)  \
   void (name)(obj_data *obj, char *argument, int cmd, int subcmd)


struct obj_command_info {
   char *command;
   void	(*command_pointer)(obj_data *obj, char *argument, int cmd, int subcmd);
   int	subcmd;
};


/* do_osend */
#define SCMD_OSEND         0
#define SCMD_OECHOAROUND   1



/* attaches object name and vnum to msg and sends it to script_log */
void obj_log(obj_data *obj, char *msg)
{
    char buf[MAX_INPUT_LENGTH + 100];

    void script_log(char *msg);

    sprintf(buf, "Obj (%s, VNum %d): %s",
	    obj->short_description, GET_OBJ_VNUM(obj), msg);
    script_log(buf);
}


/* returns the real room number that the object or object's carrier is in */
int obj_room(obj_data *obj)
{
    if (obj->in_room != NOWHERE)
	return obj->in_room;
    else if (obj->carried_by)
	return IN_ROOM(obj->carried_by);
    else if (obj->worn_by)
	return IN_ROOM(obj->worn_by);
    else if (obj->in_obj)
	return obj_room(obj->in_obj);
    else
	return NOWHERE;
}


/* returns the real room number, or NOWHERE if not found or invalid */
room_rnum find_obj_target_room(obj_data *obj, char *rawroomstr)
{
    int tmp;
    room_rnum location;
    char_data *target_mob;
    obj_data *target_obj;
    char roomstr[MAX_INPUT_LENGTH];

    one_argument(rawroomstr, roomstr);

    if (!*roomstr)
	return NOWHERE;

    if (isdigit(*roomstr) && !strchr(roomstr, '.'))
    {
	tmp = atoi(roomstr);
	if ((location = real_room(tmp)) < 0)
	    return NOWHERE;
    }

    else if ((target_mob = get_char_by_obj(obj, roomstr)))
	location = IN_ROOM(target_mob);
    else if ((target_obj = get_obj_by_obj(obj, roomstr)))
    {
	if (target_obj->in_room != NOWHERE)
	    location = target_obj->in_room;
	else 
	    return NOWHERE;
    }
    else
	return NOWHERE;
  
    /* a room has been found.  Check for permission */
    if (ROOM_FLAGGED(location, ROOM_GODROOM) || 
#ifdef ROOM_IMPROOM
	ROOM_FLAGGED(location, ROOM_IMPROOM) ||
#endif
	ROOM_FLAGGED(location, ROOM_HOUSE))
	return NOWHERE;

    if (ROOM_FLAGGED(location, ROOM_PRIVATE) &&
	world[location].people && world[location].people->next_in_room)
	return NOWHERE;

    return location;
}



/* Object commands */

OCMD(do_oecho)
{
  int room;

  skip_spaces(&argument);
  
  if (!*argument) 
	  obj_log(obj, "oecho called with no args");

  else if ((room = obj_room(obj)) != NOWHERE) {
    if (world[room].people)
	    sub_write(argument, world[room].people, TRUE, TO_ROOM | TO_CHAR);
  }
  
  else
	  obj_log(obj, "oecho called by object in NOWHERE");
}


OCMD(do_oforce)
{
  char_data *ch, *next_ch;
  int room;
  char arg1[MAX_INPUT_LENGTH];
  
  //Phantom oforce non eseguiva il comando completo. Eliminata arg2[MAX_INPUT_LENGTH]
  //two_arguments(argument, arg1, arg2);
  argument = one_argument(argument, arg1);
    
  if (!*arg1 || !argument) {
	  obj_log(obj, "oforce called with too few args");
	  return;
  }
  
  if (!str_cmp(arg1, "all")) {
	  if ((room = obj_room(obj)) == NOWHERE) 
	    obj_log(obj, "oforce called by object in NOWHERE");
	  else {
	    for (ch = world[room].people; ch; ch = next_ch) {
		    next_ch = ch->next_in_room;
	
		    if (GET_LEVEL(ch)<LVL_IMMORT) {
		      command_interpreter(ch, argument);
		    }
	    }
	  }      
  }
  else {
	  if ((ch = get_char_by_obj(obj, arg1))) {
	    if (GET_LEVEL(ch)<LVL_IMMORT) {
		    command_interpreter(ch, argument);
	    }
	  }
    else
	    obj_log(obj, "oforce: no target found");
  }
}


OCMD(do_osend)
{
    char buf[MAX_INPUT_LENGTH], *msg;
    char_data *ch;
  
    msg = any_one_arg(argument, buf);

    if (!*buf)
    {
	obj_log(obj, "osend called with no args");
	return;
    }

    skip_spaces(&msg);

    if (!*msg)
    {
	obj_log(obj, "osend called without a message");
	return;
    }

    if ((ch = get_char_by_obj(obj, buf)))
    {
	if (subcmd == SCMD_OSEND)
	    sub_write(msg, ch, TRUE, TO_CHAR);
	else if (subcmd == SCMD_OECHOAROUND)
	    sub_write(msg, ch, TRUE, TO_ROOM);
    }

    else
	obj_log(obj, "no target found for osend");
}

/* increases the target's exp */
OCMD(do_oexp)
{
    char_data *ch;
    char name[MAX_INPUT_LENGTH], amount[MAX_INPUT_LENGTH];

    two_arguments(argument, name, amount);

    if (!*name || !*amount)
    {
	obj_log(obj, "oexp: too few arguments");
	return;
    }
    
    if ((ch = get_char_by_obj(obj, name))) 
	gain_exp(ch, atoi(amount));
    else
    {
	obj_log(obj, "oexp: target not found");
	return;
    }
}

//Orione Inizio
/* set the object's timer value */
OCMD(do_otimer)
{
	char arg[MAX_INPUT_LENGTH];
	
	one_argument(argument, arg);

	if (!*arg)
		obj_log(obj, "otimer: missing argument");
	else if (!isdigit(*arg))
		obj_log(obj, "otimer: bad argument");
	else
		GET_OBJ_TIMER(obj) = atoi(arg);
}

/* transform into a different object */
/* note: this shouldn't be used with containers unless both objects */
/* are containers! */
OCMD(do_otransform)
{
	char arg[MAX_INPUT_LENGTH];
	obj_data *o, tmpobj;
	struct char_data *wearer=NULL;
	int pos=-1;
	
	one_argument(argument, arg);
							
	if (!*arg)
		obj_log(obj, "otransform: missing argument");
	else if (!isdigit(*arg))
		obj_log(obj, "otransform: bad argument");
	else {
		o = read_object(atoi(arg), VIRTUAL);
		if (o==NULL) {
			obj_log(obj, "otransform: bad object vnum");
			return;
		}
	
		if (obj->worn_by) {
			pos = obj->worn_on;
			wearer = obj->worn_by;
			unequip_char(obj->worn_by, pos);
		}
		
		/* move new obj info over to old object and delete new obj */
		memcpy(&tmpobj, o, sizeof(*o));
		tmpobj.in_room = obj->in_room;
		tmpobj.carried_by = obj->carried_by;
		tmpobj.worn_by = obj->worn_by;
		tmpobj.worn_on = obj->worn_on;
		tmpobj.in_obj = obj->in_obj;
		tmpobj.contains = obj->contains;
		tmpobj.id = obj->id;
		tmpobj.proto_script = obj->proto_script;
		tmpobj.script = obj->script;
		tmpobj.next_content = obj->next_content;
		tmpobj.next = obj->next;
		memcpy(obj, &tmpobj, sizeof(*obj));
		
		if (wearer) {
			equip_char(wearer, obj, pos);
		}

		extract_obj(o);
	}
}
//Orione Fine

/* purge all objects an npcs in room, or specified object or mob */
OCMD(do_opurge)
{
  char arg[MAX_INPUT_LENGTH];
  char_data *ch, *next_ch;
  obj_data *o, *next_obj;
  int rm;

  one_argument(argument, arg);
  
  if (!*arg) {
	  if ((rm = obj_room(obj)) != NOWHERE) {
	    for (ch = world[rm].people; ch; ch = next_ch ) {
		    next_ch = ch->next_in_room;
		    if (IS_NPC(ch))
		      extract_char(ch);
	    }
    
	    for (o = world[rm].contents; o; o = next_obj ) {
		    next_obj = o->next_content;
		    if (o != obj)
		      extract_obj(o);
	    }
	  }  
	  return;
  }
  
  if (!(ch = get_char_by_obj(obj, arg))) {
	  if ((o = get_obj_by_obj(obj, arg))) {
	  	if (o != obj)
	      extract_obj(o);
	    else
	      obj_log(obj, "opurge: Obj self purged - Opxione inattiva provoca Crash ");
	  }
	  else 
	    obj_log(obj, "opurge: bad argument");
    
	  return;
  }
  
  if (!IS_NPC(ch)) {
	  obj_log(obj, "opurge: purging a PC");
	  return;
  }
  
  extract_char(ch);
}


OCMD(do_oteleport)
{
    char_data *ch, *next_ch;
    room_rnum target, rm;
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

    two_arguments(argument, arg1, arg2);
  
    if (!*arg1 || !*arg2)
    {
	obj_log(obj, "oteleport called with too few args");
	return;
    }

    target = find_obj_target_room(obj, arg2);
  
    if (target == NOWHERE) 
	obj_log(obj, "oteleport target is an invalid room");
  
    else if (!str_cmp(arg1, "all"))
    {
	rm = obj_room(obj);
	if (target == rm)
	    obj_log(obj, "oteleport target is itself");

	for (ch = world[rm].people; ch; ch = next_ch)
	{
	    next_ch = ch->next_in_room;
	    
	    char_from_room(ch);
	    char_to_room(ch, target);
	}
    }
  
    else
    {
	if ((ch = get_char_by_obj(obj, arg1)))
	{
	    char_from_room(ch);
	    char_to_room(ch, target);
	}
    
	else
	    obj_log(obj, "oteleport: no target found");
    }
}


OCMD(do_dgoload)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    int number = 0, room;
    char_data *mob;
    obj_data *object;

    two_arguments(argument, arg1, arg2);

    if (!*arg1 || !*arg2 || !is_number(arg2) || ((number = atoi(arg2)) < 0))
    {
        obj_log(obj, "oload: bad syntax");
        return;
    }
 
    if ((room = obj_room(obj)) == NOWHERE)
    {
        obj_log(obj, "oload: object in NOWHERE trying to load");
        return;
    }
    
    if (is_abbrev(arg1, "mob"))
    {
        if ((mob = read_mobile(number, VIRTUAL)) == NULL)
        {
            obj_log(obj, "oload: bad mob vnum");
            return;
        }
        char_to_room(mob, room);
	load_mtrigger(mob); //Orione
    }
     
    else if (is_abbrev(arg1, "obj"))
    {
        if ((object = read_object(number, VIRTUAL)) == NULL)
        {
            obj_log(obj, "oload: bad object vnum");
            return;
        }

        obj_to_room(object, room);
	load_otrigger(obj); //Orione
    }
         
    else
        obj_log(obj, "oload: bad type");

}

OCMD(do_odamage) {
  char name[MAX_INPUT_LENGTH], amount[MAX_INPUT_LENGTH];
  char_data *ch, *next_ch;
  int dam = 0;
  int room;
  
  two_arguments(argument, name, amount);

  if (!*name || !*amount || !isdigit(*amount)) {
	  obj_log(obj, "odamage: bad syntax");
	  return;
  }

  dam = atoi(amount);

  if (!str_cmp(name, "all")) {
	  if ((room = obj_room(obj)) == NOWHERE) 
	    obj_log(obj, "odamage called by object in NOWHERE");
	  else {
	    for (ch = world[room].people; ch; ch = next_ch) {
		    next_ch = ch->next_in_room;
//Phantom Sequenxa comandi comune a tutte le funxioni %damage% da isolare.
	      if (GET_LEVEL(ch) < LVL_IMMORT) {
	        GET_HIT(ch) -= dam;
	      }
	  
	  	  update_pos(ch);
	      switch (GET_POS(ch)) {
	        case POS_MORTALLYW:
 	          act("$n is mortally wounded, and will die soon, if not aided.", TRUE, ch, 0, 0, TO_ROOM);
 	          send_to_char("You are mortally wounded, and will die soon, if not aided.\r\n", ch);
	          break;
	        case POS_INCAP:
 	          act("$n is incapacitated and will slowly die, if not aided.", TRUE, ch, 0, 0, TO_ROOM);
 	          send_to_char("You are incapacitated an will slowly die, if not aided.\r\n", ch);
    	      break;
  	      case POS_STUNNED:
   	        act("$n is stunned, but will probably regain consciousness again.", TRUE, ch, 0, 0, TO_ROOM);
    	      send_to_char("You're stunned, but will probably regain consciousness again.\r\n", ch);
    	      break;
  	      case POS_DEAD:
    	      act("$n is dead!  R.I.P.", FALSE, ch, 0, 0, TO_ROOM);
    	      send_to_char("You are dead!  Sorry...\r\n", ch);
    	      break;

  	      default:			/* >= POSITION SLEEPING */
    	      if (dam > (GET_MAX_HIT(ch) >> 2))
              act("That really did HURT!", FALSE, ch, 0, 0, TO_CHAR);
 	     
 	          if (GET_HIT(ch) < (GET_MAX_HIT(ch) >> 2)) {
        	    sprintf(buf2, "%sYou wish that your wounds would stop BLEEDING so much!%s\r\n",
	                    CCRED(ch, C_SPR), CCNRM(ch, C_SPR));
        	    send_to_char(buf2, ch);
            }
	      }
	  
	      if (GET_POS(ch) == POS_DEAD) {
	        if (!IS_NPC(ch)) {
		        sprintf(buf2, "%s killed by a trap at %s", GET_NAME(ch),
	          		    world[ch->in_room].name);
      	    mudlog(buf2, BRF, 0, TRUE);
	        }
    	    die(ch, NULL);
	      }  
	    }
	  }      
  }
  
  else if ((ch = get_char_by_obj(obj, name))) {
	  if (GET_LEVEL(ch)>=LVL_IMMORT) {
	    send_to_char("Being the cool immortal you are, you sidestep a trap, obviously placed to kill you.", ch);
	    return;
	  }
	  
	  GET_HIT(ch) -= dam;
	  
	  update_pos(ch);
	  switch (GET_POS(ch)) {
	    case POS_MORTALLYW:
 	      act("$n is mortally wounded, and will die soon, if not aided.", TRUE, ch, 0, 0, TO_ROOM);
 	      send_to_char("You are mortally wounded, and will die soon, if not aided.\r\n", ch);
	      break;
	    case POS_INCAP:
 	      act("$n is incapacitated and will slowly die, if not aided.", TRUE, ch, 0, 0, TO_ROOM);
 	      send_to_char("You are incapacitated an will slowly die, if not aided.\r\n", ch);
    	  break;
  	  case POS_STUNNED:
   	    act("$n is stunned, but will probably regain consciousness again.", TRUE, ch, 0, 0, TO_ROOM);
    	  send_to_char("You're stunned, but will probably regain consciousness again.\r\n", ch);
    	  break;
  	  case POS_DEAD:
    	  act("$n is dead!  R.I.P.", FALSE, ch, 0, 0, TO_ROOM);
    	  send_to_char("You are dead!  Sorry...\r\n", ch);
    	  break;

  	  default:			/* >= POSITION SLEEPING */
    	  if (dam > (GET_MAX_HIT(ch) >> 2))
          act("That really did HURT!", FALSE, ch, 0, 0, TO_CHAR);
 	     
 	      if (GET_HIT(ch) < (GET_MAX_HIT(ch) >> 2)) {
        	sprintf(buf2, "%sYou wish that your wounds would stop BLEEDING so much!%s\r\n",
	                CCRED(ch, C_SPR), CCNRM(ch, C_SPR));
        	send_to_char(buf2, ch);
        }
	  }
	  
	  if (GET_POS(ch) == POS_DEAD) {
	    if (!IS_NPC(ch)) {
		    sprintf(buf2, "%s killed by a trap at %s", GET_NAME(ch),
	          		world[ch->in_room].name);
      	mudlog(buf2, BRF, 0, TRUE);
	    }
    	die(ch, NULL);
	  }
  }
  else
	  obj_log(obj, "odamage: target not found");
}

//Orione Inizio
OCMD(do_odoor)
{
	char target[MAX_INPUT_LENGTH], direction[MAX_INPUT_LENGTH];
	char field[MAX_INPUT_LENGTH], *value;
	room_data *rm;
	struct room_direction_data *exit;
	int dir, fd, to_room;

	const char *door_field[] = {
	  "purge",
	  "description",
	  "flags",
	  "key",
	  "name",
	  "room",
	  "\n"
	};

	argument = two_arguments(argument, target, direction);
	value = one_argument(argument, field);
	skip_spaces(&value);

	if (!*target || !*direction || !*field) {
		obj_log(obj, "odoor called with too few args");
		return;
	}

	if ((rm = get_room(target)) == NULL) {
		obj_log(obj, "odoor: invalid target");
		return;
	}

	if ((dir = search_block(direction, dirs, FALSE)) == -1) {
		obj_log(obj, "odoor: invalid direction");
		return;
	}

	if ((fd = search_block(field, door_field, FALSE)) == -1) {
		obj_log(obj, "odoor: invalid field");
		return;
	}

	exit = rm->dir_option[dir];

	/* purge exit */
	if (fd == 0) {
		if (exit) {
			if (exit->general_description)
				free(exit->general_description);
			if (exit->keyword)
				free(exit->keyword);
			free(exit);
			rm->dir_option[dir] = NULL;
		}
	}
	else {
		if (!exit) {
			CREATE(exit, struct room_direction_data, 1);
			rm->dir_option[dir] = exit;
		}
		
		switch (fd) {
			case 1:  /* description */
				if (exit->general_description)
					free(exit->general_description);
				CREATE(exit->general_description, char, strlen(value) + 3);
				strcpy(exit->general_description, value);
				strcat(exit->general_description, "\r\n");
				break;
			case 2:  /* flags       */
				exit->exit_info = (sh_int)asciiflag_conv(value);
				break;
			case 3:  /* key         */
				exit->key = atoi(value);
				break;
			case 4:  /* name        */
				if (exit->keyword)
					free(exit->keyword);
				CREATE(exit->keyword, char, strlen(value) + 1);
				strcpy(exit->keyword, value);
				break;
			case 5:  /* room        */
				if ((to_room = real_room(atoi(value))) != NOWHERE)
					exit->to_room = to_room;
				else
					obj_log(obj, "odoor: invalid door target");
				break;
		}
	}
}

OCMD(do_osetval)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	int position, new_value;

	two_arguments(argument, arg1, arg2);
	if (!arg1 || !*arg1 || !arg2 || !*arg2 ||
		!is_number(arg1) || !is_number(arg2)) {
		 obj_log(obj, "osetval: bad syntax");
		 return;
	}

	position = atoi(arg1);
	new_value = atoi(arg2);
	if (position>=0 && position<NUM_OBJ_VAL_POSITIONS)
		GET_OBJ_VAL(obj, position) = new_value;
	else
		obj_log(obj, "osetval: position out of bounds!");
}
//Orione Fine
	

const struct obj_command_info obj_cmd_info[] = {
    { "RESERVED", 0, 0 },/* this must be first -- for specprocs */

    { "oecho"      , do_oecho    , 0 },
    { "oechoaround", do_osend    , SCMD_OECHOAROUND },
    { "oexp"       , do_oexp     , 0 },
    { "oforce"     , do_oforce   , 0 },
    { "oload"	     , do_dgoload	 , 0 },
    { "opurge"     , do_opurge   , 0 },
    { "osend"      , do_osend    , SCMD_OSEND },
    { "osetval"    , do_osetval  , 0 },	 //Orione
	  { "oteleport"  , do_oteleport, 0 },
    { "odamage"    , do_odamage,   0 },
    { "otimer"     , do_otimer   , 0 },	 //Orione
	  { "otransform" , do_otransform, 0 }, //Orione
	  { "odoor"      , do_odoor    , 0 },  //Orione
	 
    { "\n", 0, 0 }	/* this must be last */
};



/*
 *  This is the command interpreter used by objects, called by script_driver.
 */
void obj_command_interpreter(obj_data *obj, char *argument)
{
  int cmd, length;
  char *line, arg[MAX_INPUT_LENGTH];
  
  phanlog("obj_command_int");
  skip_spaces(&argument);
  
  /* just drop to next line for hitting CR */
  if (!*argument)
	  return;

  line = any_one_arg(argument, arg);


  /* find the command */
  for (length = strlen(arg),cmd = 0; *obj_cmd_info[cmd].command != '\n'; cmd++)
	  if (!strncmp(obj_cmd_info[cmd].command, arg, length))
	    break;
  
  if (*obj_cmd_info[cmd].command == '\n') {
	  sprintf(buf2, "Unknown object cmd: '%s'", argument);
	  obj_log(obj, buf2);
  }
  else
	  ((*obj_cmd_info[cmd].command_pointer) (obj, line, cmd, obj_cmd_info[cmd].subcmd));
}
