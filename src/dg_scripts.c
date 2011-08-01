/**************************************************************************
*  File: scripts.c                                                        *
*  Usage: contains general functions for using scripts.                   *
*                                                                         *
*                                                                         *
*  $Author: root $
*  $Date: 2000-03-05 19:05:16+01 $
*  $Revision: 1.1 $
**************************************************************************/

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "dg_scripts.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "dg_event.h"
#include "db.h"
#include "screen.h"
#include "clan.h"
#include "spells.h"

#include "eserciti.h"

#define PULSES_PER_MUD_HOUR     (SECS_PER_MUD_HOUR*PASSES_PER_SEC)

/* external vars from db.c */
extern int top_of_trigt;
extern struct index_data **trig_index;

/* external vars from triggers.c */
extern char *trig_types[], *otrig_types[], *wtrig_types[];

/* other external vars */
extern struct room_data *world;
extern int top_of_world;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern char *item_types[];
extern char *genders[];
extern char *pc_class_types[];
extern char *exit_bits[];
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct str_app_type str_app[];
extern struct dex_app_type dex_app[];

/* external functions */
void free_varlist(struct trig_var_data *vd);
int obj_room(obj_data *obj);
int is_empty(int zone_nr);
room_rnum find_target_room(struct char_data * ch, char *rawroomstr);
trig_data *read_trigger(int nr);
struct obj_data *get_object_in_equip(struct char_data * ch, char *name);
void extract_trigger(struct trig_data *trig);
int eval_lhs_op_rhs(char *expr, char *result, void *go, struct script_data *sc,
	trig_data *trig, int type);


/* function protos from this file */
int script_driver(void *go, trig_data *trig, int type, int mode);
int trgvar_in_room(int vnum);
void script_log(char *msg);
struct cmdlist_element *find_done(struct cmdlist_element *cl);	//Orione
struct cmdlist_element *find_case(struct trig_data *trig,	      //Orione
		struct cmdlist_element *cl, void *go, struct script_data *sc, int type, char *cond); //Orione

/* local structures */
struct wait_event_data {
	trig_data *trigger;
	void *go;
	int type;
};


struct trig_data *trigger_list = NULL;  /* all attached triggers */

/* Return pointer to first occurrence in string ct in */
/* cs, or NULL if not present.  Case insensitive */
char *str_str(char *cs, char *ct)
{
	char *s, *t;
	
	if (!cs || !ct)
		return NULL;
	
	while (*cs) {
		t = ct;
		
		while (*cs && (LOWER(*cs) != LOWER(*t)))
			cs++;
		
		s = cs;
		
		while (*t && *cs && (LOWER(*cs) == LOWER(*t))) {
			t++;
			cs++;
		}
		
		if (!*t)
			return s;
	}
	
	return NULL;
}


int trgvar_in_room(int vnum) {
	int i = 0;
	char_data *ch;
	
	if (NOWHERE == real_room(vnum)) {
		script_log("people.vnum: world[vnum] does not exist");
		return (-1);
	}
	
	for (ch = world[vnum].people; ch !=NULL; ch = ch->next_in_room)
		i++;
	
	return i;
}

obj_data *get_obj_in_list(char *name, obj_data *list)
{
	obj_data *i;
	long id;
	
	if (*name == UID_CHAR)
	{
		id = atoi(name + 1);
		
		for (i = list; i; i = i->next_content)
			if (id == GET_ID(i))
				return i;
	}
	else
	{
		for (i = list; i; i = i->next_content)
			if (isname(name, i->name))
				return i;
	}
	
	return NULL;
}

obj_data *get_object_in_equip(char_data * ch, char *name)
{
	int j, n = 0, number;
	obj_data *obj;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname; 
	long id;
	
	if (*name == UID_CHAR)
	{
		id = atoi(name + 1);
		
		for (j = 0; j < NUM_WEARS; j++)
			if ((obj = GET_EQ(ch, j)))
				if (id == GET_ID(obj))
					return (obj);
	}
	else
	{
		strcpy(tmp, name);
		if (!(number = get_number(&tmp)))
			return NULL;
		
		for (j = 0; (j < NUM_WEARS) && (n <= number); j++)
			if ((obj = GET_EQ(ch, j)))
				if (isname(tmp, obj->name))
					if (++n == number)
						return (obj);
	}
	
	return NULL;
}

/************************************************************
 * search by number routines
 ************************************************************/

/* return char with UID n */
struct char_data *find_char(int n)
{
	struct char_data *ch;
	
	for (ch = character_list; ch; ch=ch->next)
	{
		if (GET_ID(ch)==n) return (ch);
	}
	
	return NULL;
}


/* return object with UID n */
obj_data *find_obj(int n)
{
	obj_data *i;
	
	for (i = object_list; i; i = i->next)
		if (n == GET_ID(i))
			return i;
		
	return NULL;
}

/* return room with UID n */
room_data *find_room(int n)
{
	n -= ROOM_ID_BASE;
	
	if ((n >= 0) && (n <= top_of_world))
		return &world[n];
	
	return NULL;
}



/************************************************************
 * generic searches based only on name
 ************************************************************/

/* search the entire world for a char, and return a pointer */
char_data *get_char(char *name)
{
	char_data *i;
	
	if (*name == UID_CHAR)
	{
		i = find_char(atoi(name + 1));
		
		if (i && !GET_INVIS_LEV(i))
			return i;
	}
	else
	{
		for (i = character_list; i; i = i->next)
			if (isname(name, i->player.name) &&
				!GET_INVIS_LEV(i))
				return i;
	}
	
	return NULL;
}


/* returns the object in the world with name name, or NULL if not found */
obj_data *get_obj(char *name)  
{
	obj_data *obj;
	long id;
	
	if (*name == UID_CHAR)
	{
		id = atoi(name + 1);
		
		for (obj = object_list; obj; obj = obj->next)
			if (id == GET_ID(obj))
				return obj;
	}      
	else
	{
		for (obj = object_list; obj; obj = obj->next)
			if (isname(name, obj->name))
				return obj;
	}
	
	return NULL;
}


/* finds room by with name.  returns NULL if not found */
room_data *get_room(char *name)
{
	int nr; 
	
	if (*name == UID_CHAR)
		return find_room(atoi(name + 1));
	else if ((nr = real_room(atoi(name))) == NOWHERE)
		return NULL;
	else
		return &world[nr];
}


/*
 * returns a pointer to the first character in world by name name,
 * or NULL if none found.  Starts searching with the person owing the object
 */
char_data *get_char_by_obj(obj_data *obj, char *name)
{
	char_data *ch;
	
	if (*name == UID_CHAR)
	{
		ch = find_char(atoi(name + 1));
		
		if (ch && !GET_INVIS_LEV(ch))
			return ch;
	}
	else
	{
		if (obj->carried_by &&
			isname(name, obj->carried_by->player.name) &&
			!GET_INVIS_LEV(obj->carried_by))
			return obj->carried_by;
		
		if (obj->worn_by &&
			isname(name, obj->worn_by->player.name) &&
			!GET_INVIS_LEV(obj->worn_by))
			return obj->worn_by;
		
		for (ch = character_list; ch; ch = ch->next)
			if (isname(name, ch->player.name) &&
				!GET_INVIS_LEV(ch))
				return ch;
	}
	
	return NULL;
}


/*
 * returns a pointer to the first character in world by name name,
 * or NULL if none found.  Starts searching in room room first
 */
char_data *get_char_by_room(room_data *room, char *name)
{    
	char_data *ch;
	
	if (*name == UID_CHAR)
	{
		ch = find_char(atoi(name + 1));
		
		if (ch && !GET_INVIS_LEV(ch))
			return ch;
	}
	else
	{
		for (ch = room->people; ch; ch = ch->next_in_room)
			if (isname(name, ch->player.name) &&
				!GET_INVIS_LEV(ch))
				return ch;
			
		for (ch = character_list; ch; ch = ch->next)
			if (isname(name, ch->player.name) &&
				!GET_INVIS_LEV(ch))
				return ch;
	}
	
	return NULL;
}


/*
 * returns the object in the world with name name, or NULL if not found
 * search based on obj
 */  
obj_data *get_obj_by_obj(obj_data *obj, char *name)
{
	obj_data *i = NULL;  
	int rm;
	long id;
	
	if (!str_cmp(name, "self") || !str_cmp(name, "me"))
		return obj;
	
	if (obj->contains && (i = get_obj_in_list(name, obj->contains)))
		return i;
	
	if (obj->in_obj) {
		if (*name == UID_CHAR) {
			id = atoi(name + 1);
			
			if (id == GET_ID(obj->in_obj))
				return obj->in_obj;
		}
		else if (isname(name, obj->in_obj->name))
			return obj->in_obj;
	}   
	
	else if (obj->worn_by && (i = get_object_in_equip(obj->worn_by, name)))
		return i;
	else if (obj->carried_by && (i = get_obj_in_list(name, obj->carried_by->carrying)))
		return i;
	else if (((rm = obj_room(obj)) != NOWHERE) && (i = get_obj_in_list(name, world[rm].contents)))
		return i;
	
	if (*name == UID_CHAR) {           
		id = atoi(name + 1);
		
		for (i = object_list; i; i = i->next)
			if (id == GET_ID(i))
				break;
	}
	
	else
	{
		for (i = object_list; i; i = i->next)
			if (isname(name, i->name))
				break;
	}
	
	return i;
}   


/* returns obj with name */
obj_data *get_obj_by_room(room_data *room, char *name)
{
	obj_data *obj;
	long id;
	
	if (*name == UID_CHAR)
	{
		id = atoi(name + 1);
		
		for (obj = room->contents; obj; obj = obj->next_content)
			if (id == GET_ID(obj)) 
				return obj;
			
		for (obj = object_list; obj; obj = obj->next)
			if (id == GET_ID(obj))
				return obj;
	}
	else
	{
		for (obj = room->contents; obj; obj = obj->next_content)
			if (isname(name, obj->name))
				return obj;
			
		for (obj = object_list; obj; obj = obj->next)
			if (isname(name, obj->name))
				return obj;
	}           
	
	return NULL;
}



/* checks every PLUSE_SCRIPT for random triggers */
void script_trigger_check(void)
{
	char_data *ch;
	obj_data *obj;
	struct room_data *room=NULL;
	int nr;
	struct script_data *sc;
	
	phanlog("script_trigger_check");
	
	for (ch = character_list; ch; ch = ch->next) {
		if (SCRIPT(ch)) {
			
			sc = SCRIPT(ch);
			
			if ((IN_ROOM(ch)!=NOWHERE) &&  IS_SET(SCRIPT_TYPES(sc), WTRIG_RANDOM) &&
				(!is_empty(world[IN_ROOM(ch)].zone) ||
					IS_SET(SCRIPT_TYPES(sc), WTRIG_GLOBAL)))
				random_mtrigger(ch);
				//phanlog("script_trigger_check - 1");
		}
	}
	
	for (obj = object_list; obj; obj = obj->next) {
		if (SCRIPT(obj)) {
			
			sc = SCRIPT(obj);
			
			if (IS_SET(SCRIPT_TYPES(sc), OTRIG_RANDOM))
				random_otrigger(obj);
		  //phanlog("script_trigger_check - 2");
		  //sprintf(phantomima," s_t_c (xobj) %d - %s", GET_OBJ_VNUM(obj), GET_OBJ_NAME(obj));
		}
	 //phanlog("script_trigger_check - 2b");
	}
	
		
	for (nr = 0; nr <= top_of_world; nr++) {
		if (SCRIPT(&world[nr])) {
			room = &world[nr];
			//sprintf(phantomima," script_trigger_check (Xroom) %d ", room->number);
			sc = SCRIPT(room);
			
			if (IS_SET(SCRIPT_TYPES(sc), WTRIG_RANDOM) && (!is_empty(room->zone) || IS_SET(SCRIPT_TYPES(sc), WTRIG_GLOBAL)))
				random_wtrigger(room);
			//phanlog("script_trigger_check - 3");
		}
	}
	phanlog("script_trigger_check - NIH");
}


EVENT(trig_wait_event)
{
	struct wait_event_data *wait_event_obj = (struct wait_event_data *)info;
	trig_data *trig;
	void *go;
	int type;
	phanlog("EVENT");	
	
	trig = wait_event_obj->trigger;
	go = wait_event_obj->go;
	type = wait_event_obj->type;
	
	free(wait_event_obj);  
	GET_TRIG_WAIT(trig) = NULL;
	
	script_driver(go, trig, type, TRIG_RESTART);
	phanlog("EVENT - NIH");
}


void do_stat_trigger(struct char_data *ch, trig_data *trig)
{
	struct cmdlist_element *cmd_list;
	char sb[MAX_STRING_LENGTH];
	
	if (!trig)
	{
		log("SYSERR: NULL trigger passed to do_stat_trigger.");
		return;
	}
	
	sprintf(sb, "Name: '%s%s%s',  VNum: [%s%5d%s], RNum: [%5d]\r\n",
		CCYEL(ch, C_NRM), GET_TRIG_NAME(trig), CCNRM(ch, C_NRM),
		CCGRN(ch, C_NRM), GET_TRIG_VNUM(trig), CCNRM(ch, C_NRM),
		GET_TRIG_RNUM(trig));
	
	if (trig->attach_type==OBJ_TRIGGER) {
		send_to_char("Trigger Intended Assignment: Objects\r\n", ch);
		sprintbit(GET_TRIG_TYPE(trig), otrig_types, buf);
	} else if (trig->attach_type==WLD_TRIGGER) {
		send_to_char("Trigger Intended Assignment: Rooms\r\n", ch);
		sprintbit(GET_TRIG_TYPE(trig), wtrig_types, buf);
	} else {
		send_to_char("Trigger Intended Assignment: Mobiles\r\n", ch);
		sprintbit(GET_TRIG_TYPE(trig), trig_types, buf);
	}
	
	sprintf(sb, "Trigger Type: %s, Numeric Arg: %d, Arg list: %s\r\n",
		buf, GET_TRIG_NARG(trig), 
		((GET_TRIG_ARG(trig) && *GET_TRIG_ARG(trig))
			? GET_TRIG_ARG(trig) : "None"));
	
	strcat(sb,"Commands:\r\n   ");
	
	cmd_list = trig->cmdlist;
	while (cmd_list)
	{
		if (cmd_list->cmd)
		{
			strcat(sb,cmd_list->cmd);
			strcat(sb,"\r\n   ");
		}
		
		cmd_list = cmd_list->next;
	}
	
	page_string(ch->desc, sb, 1);
}


/* find the name of what the uid points to */
void find_uid_name(char *uid, char *name)
{
	char_data *ch;
	obj_data *obj;
	
	if ((ch = get_char(uid)))
		strcpy(name, ch->player.name);
	else if ((obj = get_obj(uid)))
		strcpy(name, obj->name);
	else
		sprintf(name, "uid = %s, (not found)", uid + 1);
}


/* general function to display stats on script sc */
void script_stat (char_data *ch, struct script_data *sc)
{
	struct trig_var_data *tv;
	trig_data *t;
	char name[MAX_INPUT_LENGTH];
	
	sprintf(buf, "Global Variables: %s\r\n", sc->global_vars ? "" : "None");
	send_to_char(buf, ch);
	
	for (tv = sc->global_vars; tv; tv = tv->next) {
		if (*(tv->value) == UID_CHAR) {
			find_uid_name(tv->value, name);
			sprintf(buf, "    %15s:  %s\r\n", tv->name, name);
		} else 
			sprintf(buf, "    %15s:  %s\r\n", tv->name, tv->value);
		send_to_char(buf, ch);
	}
	
	for (t = TRIGGERS(sc); t; t = t->next) {
		sprintf(buf, "\r\n  Trigger: %s%s%s, VNum: [%s%5d%s], RNum: [%5d]\r\n",
			CCYEL(ch, C_NRM), GET_TRIG_NAME(t), CCNRM(ch, C_NRM),
			CCGRN(ch, C_NRM), GET_TRIG_VNUM(t), CCNRM(ch, C_NRM),
			GET_TRIG_RNUM(t));
		send_to_char(buf, ch);
		
		if (t->attach_type==OBJ_TRIGGER) {
			send_to_char("  Trigger Intended Assignment: Objects\r\n", ch);
			sprintbit(GET_TRIG_TYPE(t), otrig_types, buf1);
		} else if (t->attach_type==WLD_TRIGGER) {
			send_to_char("  Trigger Intended Assignment: Rooms\r\n", ch);
			sprintbit(GET_TRIG_TYPE(t), wtrig_types, buf1);
		} else {
			send_to_char("  Trigger Intended Assignment: Mobiles\r\n", ch);
			sprintbit(GET_TRIG_TYPE(t), trig_types, buf1);
		}
		
		sprintf(buf, "  Trigger Type: %s, Numeric Arg: %d, Arg list: %s\r\n", 
			buf1, GET_TRIG_NARG(t), 
			((GET_TRIG_ARG(t) && *GET_TRIG_ARG(t)) ? GET_TRIG_ARG(t) :
				"None"));
		send_to_char(buf, ch);
		
#if 0
		if (GET_TRIG_WAIT(t)) {
			sprintf(buf, "    Wait: %ld, Current line: %s\r\n",
				time_to_event(GET_TRIG_WAIT(t)), t->curr_state->cmd);
			send_to_char(buf, ch);
			
			sprintf(buf, "  Variables: %s\r\n", GET_TRIG_VARS(t) ? "" : "None");
			send_to_char(buf, ch);
			
			for (tv = GET_TRIG_VARS(t); tv; tv = tv->next) {
				if (*(tv->value) == UID_CHAR) {
					find_uid_name(tv->value, name);
					sprintf(buf, "    %15s:  %s\r\n", tv->name, name);
				} else 
					sprintf(buf, "    %15s:  %s\r\n", tv->name, tv->value);
				send_to_char(buf, ch);
			}
		}
#endif
	}  
}


void do_sstat_room(struct char_data * ch)
{
	struct room_data *rm = &world[ch->in_room];
	
	send_to_char("Script information:\r\n", ch);
	if (!SCRIPT(rm)) {
		send_to_char("  None.\r\n", ch);
		return;
	}
	
	script_stat(ch, SCRIPT(rm));
}


void do_sstat_object(char_data *ch, obj_data *j)
{
	send_to_char("Script information:\r\n", ch);
	if (!SCRIPT(j)) {
		send_to_char("  None.\r\n", ch);
		return;
	}
	
	script_stat(ch, SCRIPT(j));
}


void do_sstat_character(char_data *ch, char_data *k)
{
	send_to_char("Script information:\r\n", ch);
	if (!SCRIPT(k)) {
		send_to_char("  None.\r\n", ch);
		return;
	}
	
	script_stat(ch, SCRIPT(k));
}


/*
 * adds the trigger t to script sc in in location loc.  loc = -1 means
 * add to the end, loc = 0 means add before all other triggers.
 */
void add_trigger(struct script_data *sc, trig_data *t, int loc)
{
	trig_data *i;
	int n;
	
	for (n = loc, i = TRIGGERS(sc); i && i->next && (n != 0); n--, i = i->next);
	
	if (!loc) {
		t->next = TRIGGERS(sc);
		TRIGGERS(sc) = t;
	} else if (!i)
		TRIGGERS(sc) = t;
	else {
		t->next = i->next;
		i->next = t;
	}
	
	SCRIPT_TYPES(sc) |= GET_TRIG_TYPE(t);
	
	t->next_in_world = trigger_list;
	trigger_list = t;
}


ACMD(do_attach) 
{
	char_data *victim;
	obj_data *object;
	trig_data *trig;
	char targ_name[MAX_INPUT_LENGTH], trig_name[MAX_INPUT_LENGTH];
	char loc_name[MAX_INPUT_LENGTH];
	int loc, tn, rn;
	room_rnum room;
	
	argument = two_arguments(argument, arg, trig_name);
	two_arguments(argument, targ_name, loc_name);
	
	if (!*arg || !*targ_name || !*trig_name) {
		send_to_char("Usage: attach { mtr | otr | wtr } { trigger } { name } [ location ]\r\n", ch);
		return;
	}
	
	tn = atoi(trig_name);
	loc = (*loc_name) ? atoi(loc_name) : -1;
	
	if (is_abbrev(arg, "mtr")) {
		if ((victim = get_char_vis(ch, targ_name))) {
			if (IS_NPC(victim))  {
				
	/* have a valid mob, now get trigger */
				rn = real_trigger(tn);
				if ((rn >= 0) && (trig = read_trigger(rn))) {
					
					if (!SCRIPT(victim))
						CREATE(SCRIPT(victim), struct script_data, 1);
					add_trigger(SCRIPT(victim), trig, loc);
					
					sprintf(buf, "Trigger %d (%s) attached to %s.\r\n",
						tn, GET_TRIG_NAME(trig), GET_SHORT(victim));
					send_to_char(buf, ch);
				} else
					send_to_char("That trigger does not exist.\r\n", ch);
			} else
				send_to_char("Players can't have scripts.\r\n", ch);
		} else
			send_to_char("That mob does not exist.\r\n", ch);
	}
	
	else if (is_abbrev(arg, "otr")) {
		if ((object = get_obj_vis(ch, targ_name))) {
			
      /* have a valid obj, now get trigger */
			rn = trig_index[tn] ? tn : -1;
			if ((rn >= 0) && (trig = read_trigger(rn))) {
				
				if (!SCRIPT(object))
					CREATE(SCRIPT(object), struct script_data, 1);
				add_trigger(SCRIPT(object), trig, loc);
				
				sprintf(buf, "Trigger %d (%s) attached to %s.\r\n",
					tn, GET_TRIG_NAME(trig), 
					(object->short_description ?
						object->short_description : object->name));
				send_to_char(buf, ch);
			} else
				send_to_char("That trigger does not exist.\r\n", ch);
		} else
			send_to_char("That object does not exist.\r\n", ch); 
	}
	
	else if (is_abbrev(arg, "wtr")) {
		if (isdigit(*targ_name) && !strchr(targ_name, '.')) {
			if ((room = find_target_room(ch, targ_name)) != NOWHERE) {
				
	/* have a valid room, now get trigger */
				rn = trig_index[tn] ? tn : -1;
				if ((rn >= 0) && (trig = read_trigger(rn))) {
					
					if (!(world[room].script))
						CREATE(world[room].script, struct script_data, 1);
					add_trigger(world[room].script, trig, loc);
					
					sprintf(buf, "Trigger %d (%s) attached to room %d.\r\n",
						tn, GET_TRIG_NAME(trig), world[room].number);
					send_to_char(buf, ch);
				} else
					send_to_char("That trigger does not exist.\r\n", ch);
			}
		} else
			send_to_char("You need to supply a room number.\r\n", ch);
	}
	
	else
		send_to_char("Please specify 'mtr', otr', or 'wtr'.\r\n", ch);
}


/* adds a variable with given name and value to trigger */
void add_var(struct trig_var_data **var_list, char *name, char *value)
{
	struct trig_var_data *vd;
	
	for (vd = *var_list; vd && str_cmp(vd->name, name); vd = vd->next);
	
	if (vd) {
		free(vd->value);
		CREATE(vd->value, char, strlen(value) + 1);
		strcpy(vd->value, value);
	}
	
	else {
		CREATE(vd, struct trig_var_data, 1);
		
		CREATE(vd->name, char, strlen(name) + 1);
		strcpy(vd->name, name);
		
		CREATE(vd->value, char, strlen(value) + 1);
		strcpy(vd->value, value);
		
		vd->next = *var_list;
		*var_list = vd;
	}
}


/*
 *  removes the trigger specified by name, and the script of o if
 *  it removes the last trigger.  name can either be a number, or
 *  a 'silly' name for the trigger, including things like 2.beggar-death.
 *  returns 0 if did not find the trigger, otherwise 1.  If it matters,
 *  you might need to check to see if all the triggers were removed after
 *  this function returns, in order to remove the script.
 */
int remove_trigger(struct script_data *sc, char *name)
{
	trig_data *i, *j;
	int num = 0, string = FALSE, n;
	char *cname;
	
	
	if (!sc)
		return 0;
	
	if ((cname = strstr(name,".")) || (!isdigit(*name)) ) {
		string = TRUE;
		if (cname) {
			*cname = '\0';
			num = atoi(name);
			name = ++cname;
		}
	} else
		num = atoi(name);
	
	for (n = 0, j = NULL, i = TRIGGERS(sc); i; j = i, i = i->next) {
		if (string) {
			if (isname(name, GET_TRIG_NAME(i)))
				if (++n >= num)
					break;
		}
		
		else if (++n >= num)
			break;
	}
	
	if (i) {
		if (j) {
			j->next = i->next;
			extract_trigger(i);
		}
		
    /* this was the first trigger */
		else {
			TRIGGERS(sc) = i->next;
			extract_trigger(i);
		}
		
    /* update the script type bitvector */
		SCRIPT_TYPES(sc) = 0;
		for (i = TRIGGERS(sc); i; i = i->next)
			SCRIPT_TYPES(sc) |= GET_TRIG_TYPE(i);
		
		return 1;
	} else
		return 0; 
}     

ACMD(do_detach)
{  
	char_data *victim = NULL;
	obj_data *object = NULL;
	struct room_data *room;
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	char *trigger = 0;   
	int tmp;
	
	argument = two_arguments(argument, arg1, arg2);
	one_argument(argument, arg3);
	
	if (!*arg1 || !*arg2) {
		send_to_char("Usage: detach [ mob | object ] { target } { trigger |"
			" 'all' }\r\n", ch);
		return;
	}
	
	if (!str_cmp(arg1, "room")) {
		room = &world[IN_ROOM(ch)];
		if (!SCRIPT(room))
			send_to_char("This room does not have any triggers.\r\n", ch);
		else if (!str_cmp(arg2, "all")) {
			extract_script(SCRIPT(room));
			SCRIPT(room) = NULL;
			send_to_char("All triggers removed from room.\r\n", ch);
		}
		
		else if (remove_trigger(SCRIPT(room), arg2)) {
			send_to_char("Trigger removed.\r\n", ch);
			if (!TRIGGERS(SCRIPT(room))) {
				extract_script(SCRIPT(room));
				SCRIPT(room) = NULL;
			}
		} else
			send_to_char("That trigger was not found.\r\n", ch);
	}
	
	else {
		if (is_abbrev(arg1, "mob")) {
			if (!(victim = get_char_vis(ch, arg2)))
				send_to_char("No such mobile around.\r\n", ch);
			else if (!*arg3)
				send_to_char("You must specify a trigger to remove.\r\n", ch);
			else
				trigger = arg3;
		}
		
		else if (is_abbrev(arg1, "object")) {
			if (!(object = get_obj_vis(ch, arg2)))
				send_to_char("No such object around.\r\n", ch);
			else if (!*arg3)
				send_to_char("You must specify a trigger to remove.\r\n", ch);
			else
				trigger = arg3;
		}
		else  {
			if ((object = get_object_in_equip_vis(ch, arg1, ch->equipment, &tmp)));
			else if ((object = get_obj_in_list_vis(ch, arg1, ch->carrying)));
			else if ((victim = get_char_room_vis(ch, arg1)));
			else if ((object = get_obj_in_list_vis(ch, arg1, world[IN_ROOM(ch)].contents)));
			else if ((victim = get_char_vis(ch, arg1)));
			else if ((object = get_obj_vis(ch, arg1)));
			else
				send_to_char("Nothing around by that name.\r\n", ch);
			
			trigger = arg2;
		}
		
		if (victim) {
			if (!IS_NPC(victim))
				send_to_char("Players don't have triggers.\r\n", ch);
			
			else if (!SCRIPT(victim))
				send_to_char("That mob doesn't have any triggers.\r\n", ch);
			else if (!str_cmp(arg2, "all")) {
				extract_script(SCRIPT(victim));
				SCRIPT(victim) = NULL;
				sprintf(buf, "All triggers removed from %s.\r\n", GET_SHORT(victim));
				send_to_char(buf, ch);
			}
			
			else if (remove_trigger(SCRIPT(victim), trigger)) {
				send_to_char("Trigger removed.\r\n", ch);
				if (!TRIGGERS(SCRIPT(victim))) {
					extract_script(SCRIPT(victim));
					SCRIPT(victim) = NULL;
				}
			} else
				send_to_char("That trigger was not found.\r\n", ch);
		}
		
		else if (object) {
			if (!SCRIPT(object))
				send_to_char("That object doesn't have any triggers.\r\n", ch);
			
			else if (!str_cmp(arg2, "all")) {
				extract_script(SCRIPT(object));
				SCRIPT(object) = NULL;
				sprintf(buf, "All triggers removed from %s.\r\n",
					object->short_description ? object->short_description :
					object->name);
				send_to_char(buf, ch);
			}
			
			else if (remove_trigger(SCRIPT(object), trigger)) {
				send_to_char("Trigger removed.\r\n", ch);
				if (!TRIGGERS(SCRIPT(object))) {
					extract_script(SCRIPT(object));
					SCRIPT(object) = NULL;
				}
			} else
				send_to_char("That trigger was not found.\r\n", ch);
		}
	}  
}    


/* frees memory associated with var */
void free_var_el(struct trig_var_data *var)
{
	free(var->name);
	free(var->value);
	free(var);
}


/*
 * remove var name from var_list
 * returns 1 if found, else 0
 */
int remove_var(struct trig_var_data **var_list, char *name)
{
	struct trig_var_data *i, *j;
	
	for (j = NULL, i = *var_list; i && str_cmp(name, i->name);
		j = i, i = i->next);
	
	if (i) {
		if (j) {
			j->next = i->next;
			free_var_el(i);
		} else {
			*var_list = i->next;
			free_var_el(i);
		}
		
		return 1;      
	}
	
	return 0;
}


/*  
 *  Logs any errors caused by scripts to the system log.
 *  Will eventually allow on-line view of script errors.
 */
void script_log(char *msg)
{
	char bufi[256];
	
	sprintf(bufi,"SCRIPT ERR: %s", msg);
	mudlog(bufi, NRM, LVL_GOD, TRUE);
}


/* sets str to be the value of var.field */
void find_replacement(void *go, struct script_data *sc, trig_data *trig,
	int type, char *var, char *field, char *str)
{
	struct trig_var_data *vd=NULL; //Orione
	char_data *ch, *c = NULL, *rndm;
	obj_data *obj, *o = NULL;
	struct room_data *room, *r = NULL;
	char *name;
	int num, count;
	
	//Orione Inizio
	char *send_cmd[] = {"msend", "osend", "wsend"};
	char *echo_cmd[] = {"mecho", "oecho", "wecho"};
	char *echoaround_cmd[] = {"mechoaround", "oechoaround", "wechoaround"};
	char *door[] = {"mdoor", "odoor", "wdoor"};
	char *force[] = {"mforce", "oforce", "wforce"};
	char *load[] = {"mload", "oload", "wload"};
	char *purge[] = {"mpurge", "opurge", "wpurge"};
	char *teleport[] = {"mteleport","oteleport", "wteleport"};
	char *damage[] = {"mdamage", "odamage",	"wdamage"};									
	

	/* X.global() will have a NULL trig */
	if (trig)
		for (vd = GET_TRIG_VARS(trig); vd; vd = vd->next)
			if (!str_cmp(vd->name, var))
				break;
	
	if (!vd)
		for (vd = sc->global_vars; vd; vd = vd->next)
			if (!str_cmp(vd->name, var))
				break;
	
	if (!*field) {
		if (vd)
			strcpy(str, vd->value);
		else {
			if (!str_cmp(var, "self"))
				strcpy(str, "self");
			else if (!str_cmp(var, "door"))
				strcpy(str,door[type]);
			else if (!str_cmp(var, "force"))
				strcpy(str,force[type]);
			else if (!str_cmp(var, "load"))
			  strcpy(str,load[type]);
			else if (!str_cmp(var, "purge"))
				strcpy(str,purge[type]);
			else if (!str_cmp(var, "teleport"))
				strcpy(str,teleport[type]);
			else if (!str_cmp(var, "damage"))
				strcpy(str,damage[type]);
			else if (!str_cmp(var, "send"))
				strcpy(str,send_cmd[type]);
			else if (!str_cmp(var, "echo"))
				strcpy(str,echo_cmd[type]);
			else if (!str_cmp(var, "echoaround"))
				strcpy(str,echoaround_cmd[type]);
			else
				*str = '\0';
		}

		return;
	}
	//Orione Fine
	
	else {
		if (vd) {
			name = vd->value;
			
			switch (type) {
				case MOB_TRIGGER:
					ch = (char_data *) go;
					
					if ((o = get_object_in_equip(ch, name)));
					else if ((o = get_obj_in_list(name, ch->carrying)));
					else if ((c = get_char_room(name, IN_ROOM(ch))));
					else if ((o = get_obj_in_list(name,world[IN_ROOM(ch)].contents)));
					else if ((c = get_char(name)));
					else if ((o = get_obj(name)));
					else if ((r = get_room(name))) {}
					
					break;
				case OBJ_TRIGGER:
					obj = (obj_data *) go;
					
					if ((c = get_char_by_obj(obj, name)));
					else if ((o = get_obj_by_obj(obj, name)));
					else if ((r = get_room(name))) {}
					
					break;
				case WLD_TRIGGER:
					room = (struct room_data *) go;
					
					if ((c = get_char_by_room(room, name)));
					else if ((o = get_obj_by_room(room, name)));
					else if ((r = get_room(name))) {}
					
					break;
			}
		}
		
		else {
			if (!str_cmp(var, "self")) {
				switch (type) {
					case MOB_TRIGGER:
						c = (char_data *) go;
						break;
					case OBJ_TRIGGER:
						o = (obj_data *) go;
						break;
					case WLD_TRIGGER:
						r = (struct room_data *) go;
						break;
				}
			}
			
			else if (!str_cmp(var, "people")) {
				sprintf(str,"%d",((num = atoi(field)) > 0) ? trgvar_in_room(num) : 0);	
				return;
			}
			else if (!str_cmp(var, "random")) {
				if (!str_cmp(field, "char")) {
					rndm = NULL;
					count = 0;
					
					if (type == MOB_TRIGGER) {
						ch = (char_data *) go;
						for (c = world[IN_ROOM(ch)].people; c; c = c->next_in_room)
							if (!PRF_FLAGGED(c, PRF_NOHASSLE) && (c != ch) &&
								CAN_SEE(ch, c)) {
								if (!number(0, count))
									rndm = c;
								count++;
							}
					}
					
					else if (type == OBJ_TRIGGER) {
						for (c = world[obj_room((obj_data *) go)].people; c;
							c = c->next_in_room)
							if (!PRF_FLAGGED(c, PRF_NOHASSLE) && !GET_INVIS_LEV(c)) {
								if (!number(0, count))
									rndm = c;
								count++;
							}
					}
					
					else if (type == WLD_TRIGGER) {
						for (c = ((struct room_data *) go)->people; c;
							c = c->next_in_room)
							if (!PRF_FLAGGED(c, PRF_NOHASSLE) && !GET_INVIS_LEV(c)) {
								if (!number(0, count))
									rndm = c;
								count++;
							}
					}
					
					if (rndm)
						sprintf(str, "%c%ld", UID_CHAR, GET_ID(rndm));
					else
						*str = '\0';
				}
				
				else
					sprintf(str, "%d", ((num = atoi(field)) > 0) ? number(1, num) : 0);
				
				return;
			}
		}
		
		if (c) {
			if (!str_cmp(field, "name"))
				if (GET_SHORT(c))
					strcpy(str, GET_SHORT(c));
				else
					strcpy(str, GET_NAME(c));
			
			else if (!str_cmp(field, "alias"))
				strcpy(str, GET_NAME(c));
			
			else if (!str_cmp(field, "level"))
				sprintf(str, "%d", GET_LEVEL(c));
			
			else if (!str_cmp(field, "align"))
				sprintf(str, "%d", GET_ALIGNMENT(c));
			
			else if (!str_cmp(field, "gold"))
				sprintf(str, "%d", GET_GOLD(c));
			
			else if (!str_cmp(field, "sex"))
				strcpy(str, genders[(int)GET_SEX(c)]);
			
			else if (!str_cmp(field, "fame"))
				sprintf(str, "%d", GET_COND(c,FULL));
			
			else if (!str_cmp(field, "sete"))
				sprintf(str, "%d", GET_COND(c,THIRST));
			else if (!str_cmp(field, "canbeseen")) {
				if ((type == MOB_TRIGGER) && !CAN_SEE(((char_data *)go), c))
					strcpy(str, "0");
				else
					strcpy(str, "1");
			}
			
			else if (!str_cmp(field, "class"))
				sprinttype(GET_CLASS(c), pc_class_types, str);
			
#ifdef GET_RACE
			else if (!str_cmp(field, "race"))
				sprinttype(GET_RACE(c), race_types, str);
#endif
			
#ifdef GET_CLAN_RANK
			else if (!str_cmp(field, "rank"))
				sprintf(str, "%d", GET_CLAN_RANK(c));
#endif
#ifdef GET_CLAN
			else if (!str_cmp(field, "clan"))
				sprintf(str, "%d", GET_CLAN(c));
#endif
			
			
			else if (!str_cmp(field, "vnum"))
				sprintf(str, "%d", GET_MOB_VNUM(c));
			
			else if (!str_cmp(field, "cha"))
				sprintf(str, "%d", GET_CHA(c));
			
			else if (!str_cmp(field, "room"))
				sprintf(str, "%d", world[IN_ROOM(c)].number);
			
			else if (!str_cmp(field, "peso"))
				sprintf(str, "%d", GET_WEIGHT(c));
			
			else if (!str_cmp(field, "altezza"))
				sprintf(str, "%d", GET_HEIGHT(c));
			
			else if (!str_cmp(field, "exp"))
				sprintf(str, "%d", GET_EXP(c));
			
			else if (!str_cmp(field, "fama"))
				sprintf(str, "%d", GET_FAMA(c));
			
			else if (!str_cmp(field, "notorieta"))
				sprintf(str, "%d", GET_NOTORIETA(c));
			
			else if (!str_cmp(field, "ac"))
				sprintf(str, "%d", GET_AC(c));
			
			else if (!str_cmp(field, "hit"))
				sprintf(str, "%d", GET_HIT(c));
			
			else if (!str_cmp(field, "maxhit"))
				sprintf(str, "%d", GET_MAX_HIT(c));
			
			else if (!str_cmp(field, "mov"))
				sprintf(str, "%d", GET_MOVE(c));
			
			else if (!str_cmp(field, "maxmove"))
				sprintf(str, "%d", GET_MAX_MOVE(c));
			
			else if (!str_cmp(field, "mana"))
				sprintf(str, "%d", GET_MANA(c));			
			
			else if (!str_cmp(field, "maxmana"))
				sprintf(str, "%d", GET_MAX_MANA(c));
			
			else if (!str_cmp(field, "bankgold"))
				sprintf(str, "%d", GET_BANK_GOLD(c));
			
			else if (!str_cmp(field, "hitroll"))
				sprintf(str, "%d", (affected_by_spell(c, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(c)));
			
			else if (!str_cmp(field, "damroll"))
				sprintf(str, "%d", GET_DAMROLL(c));
			
			else if (!str_cmp(field, "can_carry_W"))
				sprintf(str, "%d", CAN_CARRY_W(c));
			
			else if (!str_cmp(field, "can_carry_n"))
				sprintf(str, "%d", CAN_CARRY_N(c));
			
			else if (!str_cmp(field, "carry_weight"))
				sprintf(str, "%d", IS_CARRYING_W(c));
			
			else if (!str_cmp(field, "carrying_n"))
				sprintf(str, "%d", IS_CARRYING_N(c));
			
			else if (!str_cmp(field, "id_num")) //id num del pg
				sprintf(str, "%ld", GET_IDNUM(c));
				
			else if (!str_cmp(field, "generale")) {
				if(GET_ESERCITO(c) == NULL) {
					sprintf(buf, "ATTENZIONE: mob %d senza la flag ESERCITO richiama un trigger di un esercito", GET_MOB_VNUM(c));
					mudlog(buf, BRF, LVL_GOD, TRUE);
					sprintf(str, "0");
				} else {
				sprintf(str, "%ld", GET_GENERALE(c));
				}
			}
			
			else {
				*str = '\0';
				sprintf(buf2, 
					"Trigger: %s, VNum %d. unknown char field: '%s'",
					GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), field);
				script_log(buf2);
			}
		}
		
		else if (o) {
			if (!str_cmp(field, "name"))
				strcpy(str, o->name);
			
			else if (!str_cmp(field, "shortdesc"))
				strcpy(str, o->short_description);
			
			else if (!str_cmp(field, "vnum"))
				sprintf(str, "%d", GET_OBJ_VNUM(o));
			
			else if (!str_cmp(field, "type"))
				sprinttype(GET_OBJ_TYPE(o), item_types, str);
			
			else if (!str_cmp(field, "timer"))        //Orione
				sprintf(str, "%d", GET_OBJ_TIMER(o));	//Orione

			else if (!str_cmp(field, "val0"))
				sprintf(str, "%d", GET_OBJ_VAL(o, 0));
			
			else if (!str_cmp(field, "val1"))
				sprintf(str, "%d", GET_OBJ_VAL(o, 1));
			
			else if (!str_cmp(field, "val2"))
				sprintf(str, "%d", GET_OBJ_VAL(o, 2));
			
			else if (!str_cmp(field, "val3"))
				sprintf(str, "%d", GET_OBJ_VAL(o, 3));
			
			else if (!str_cmp(field, "cost"))
				sprintf(str, "%d", GET_OBJ_COST(o));
			
			else if (!str_cmp(field, "peso"))
				sprintf(str, "%d", GET_OBJ_WEIGHT(o));
			
			else {
				*str = '\0';
				sprintf(buf2,
					"Trigger: %s, VNum %d, type: %d. unknown object field: '%s'",
					GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), type, field);
				script_log(buf2);
			}
		}
		
		else if (r) {
			if (!str_cmp(field, "name"))
				strcpy(str, r->name);
			
			if (!str_cmp(field, "north")) {
				if (r->dir_option[NORTH])
					sprintbit(r->dir_option[NORTH]->exit_info ,exit_bits, str);
				else
					*str = '\0';
			}
			
			if (!str_cmp(field, "east")) {
				if (r->dir_option[EAST])
					sprintbit(r->dir_option[EAST]->exit_info ,exit_bits, str);
				else
					*str = '\0';
			}
			
			if (!str_cmp(field, "south")) {
				if (r->dir_option[SOUTH])
					sprintbit(r->dir_option[SOUTH]->exit_info ,exit_bits, str);
				else
					*str = '\0';
			}
			
			if (!str_cmp(field, "west")) {
				if (r->dir_option[WEST])
					sprintbit(r->dir_option[WEST]->exit_info ,exit_bits, str);
				else
					*str = '\0';
			}
			
			if (!str_cmp(field, "up")) {
				if (r->dir_option[UP])
					sprintbit(r->dir_option[UP]->exit_info ,exit_bits, str);
				else
					*str = '\0';
			}
			
			if (!str_cmp(field, "down")) {
				if (r->dir_option[DOWN])
					sprintbit(r->dir_option[DOWN]->exit_info ,exit_bits, str);
				else
					*str = '\0';
			}
			
			/* Orione: aggiunta della var vnum*/
				if (!str_cmp(field, "vnum"))
         				sprintf(str,"%d",r->number);
			
			else {
				*str = '\0';
				sprintf(buf2,
					"Trigger: %s, VNum %d, type: %d. unknown room field: '%s'",
					GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), type, field);
				script_log(buf2);
			}
		}
		
		else
			*str = '\0';
	}
}


/* substitutes any variables into line and returns it as buf */
void var_subst(void *go, struct script_data *sc, trig_data *trig,
	int type, char *line, char *bufe)
{
	char tmp[MAX_INPUT_LENGTH], repl_str[MAX_INPUT_LENGTH], *var, *field, *p;
	int left, len;
	int paren_count = 0;
	
	if (!strchr(line, '%')) {
		strcpy(bufe, line);
		return;
	}
	
	p = strcpy(tmp, line);
	
	left = MAX_INPUT_LENGTH - 1;
	
	while (*p && (left > 0)) {
		
		while (*p && (*p != '%') && (left > 0)) {
			*(bufe++) = *(p++);
			left--;
		}
		
		*bufe = '\0';
		
    /* double % */
		if (*p && (*(++p) == '%') && (left > 0)) {
			*(bufe++) = *(p++);
			*bufe = '\0';
			left--;
			continue;
		}
		
		else if (*p && (left > 0)) {
			
			for (var = p; *p && (*p != '%') && (*p != '.'); p++);
			
			field = p;
			if (*p == '.') {
				*(p++) = '\0';
				for (field = p; *p && ((*p != '%')||(paren_count)); p++) {
					if (*p=='(') paren_count++;
					else if (*p==')') paren_count--;
				}
			}
			
			*(p++) = '\0';
			
			find_replacement(go, sc, trig, type, var, field, repl_str);
			
			strncat(bufe, repl_str, left);
			len = strlen(repl_str);
			bufe += len;
			left -= len;
		}
	}  
}


/* returns 1 if string is all digits, else 0 */
int is_num(char *num)
{
	while (*num && isdigit(*num))
		num++;
	
	if (!*num || isspace(*num))
		return 1;
	else
		return 0;
}


/* evaluates 'lhs op rhs', and copies to result */
void eval_op(char *op, char *lhs, char *rhs, char *result, void *go,
	struct script_data *sc, trig_data *trig)
{
	char *p;
	int n;
	
  /* strip off extra spaces at begin and end */
	while (*lhs && isspace(*lhs)) 
		lhs++;
	while (*rhs && isspace(*rhs))
		rhs++;
	
	for (p = lhs; *p; p++);
	for (--p; isspace(*p) && (p > lhs); *p-- = '\0');
	for (p = rhs; *p; p++);
	for (--p; isspace(*p) && (p > rhs); *p-- = '\0');  
	
	
  /* find the op, and figure out the value */
	if (!strcmp("||", op)) {
		if ((!*lhs || (*lhs == '0')) && (!*rhs || (*rhs == '0')))
			strcpy(result, "0");
		else
			strcpy(result, "1");
	}
	
	else if (!strcmp("&&", op)) {
		if (!*lhs || (*lhs == '0') || !*rhs || (*rhs == '0'))
			strcpy (result, "0");
		else
			strcpy (result, "1");
	}
	
	else if (!strcmp("==", op)) {
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) == atoi(rhs));
		else
			sprintf(result, "%d", !str_cmp(lhs, rhs));
	}   
	
	else if (!strcmp("!=", op)) {
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) != atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs));
	}   
	
	else if (!strcmp("<=", op)) {
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) <= atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) <= 0);
	}
	
	else if (!strcmp(">=", op)) {
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) >= atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) <= 0);
	}
	
	else if (!strcmp("<", op)) {
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) < atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) < 0);
	}
	
	else if (!strcmp(">", op)) {
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) > atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) > 0);
	}
	
	else if (!strcmp("/=", op))
		sprintf(result, "%c", str_str(lhs, rhs) ? '1' : '0');
	
	else if (!strcmp("*", op))
		sprintf(result, "%d", atoi(lhs) * atoi(rhs));
	
	else if (!strcmp("/", op))
		sprintf(result, "%d", (n = atoi(rhs)) ? (atoi(lhs) / n) : 0);
	
	else if (!strcmp("+", op)) 
		sprintf(result, "%d", atoi(lhs) + atoi(rhs));
	
	else if (!strcmp("-", op))
		sprintf(result, "%d", atoi(lhs) - atoi(rhs));
	
	else if (!strcmp("!", op)) {
		if (is_num(rhs))
			sprintf(result, "%d", !atoi(rhs));
		else
			sprintf(result, "%d", !*rhs);
	}
}


/*
 * p points to the first quote, returns the matching
 * end quote, or the last non-null char in p.
*/
char *matching_quote(char *p)
{
	for (p++; *p && (*p != '"'); p++) {
		if (*p == '\\')
			p++;
	}
	
	if (!*p)
		p--;
	
	return p;
}

/*
 * p points to the first paren.  returns a pointer to the
 * matching closing paren, or the last non-null char in p.
 */
char *matching_paren(char *p)
{
	int i;
	
	for (p++, i = 1; *p && i; p++) {
		if (*p == '(')
			i++;
		else if (*p == ')')
			i--;
		else if (*p == '"')
			p = matching_quote(p);
	}
	
	return --p;
}


/* evaluates line, and returns answer in result */
void eval_expr(char *line, char *result, void *go, struct script_data *sc,
	trig_data *trig, int type)
{
	char expr[MAX_INPUT_LENGTH], *p;
	//phanlog("eval_expr");
	
	while (*line && isspace(*line))
		line++;
	
	if (eval_lhs_op_rhs(line, result, go, sc, trig, type));
	
	else if (*line == '(') {
		p = strcpy(expr, line);
		p = matching_paren(expr);
		*p = '\0';
		eval_expr(expr + 1, result, go, sc, trig, type);
	}
	
	else
		var_subst(go, sc, trig, type, line, result);
}


/*
 * evaluates expr if it is in the form lhs op rhs, and copies
 * answer in result.  returns 1 if expr is evaluated, else 0
 */
int eval_lhs_op_rhs(char *expr, char *result, void *go, struct script_data *sc,
	trig_data *trig, int type)
{
	char *p, *tokens[MAX_INPUT_LENGTH];
	char line[MAX_INPUT_LENGTH], lhr[MAX_INPUT_LENGTH], rhr[MAX_INPUT_LENGTH];
	int i, j;
	
	/*
   * valid operands, in order of priority
   * each must also be defined in eval_op()
   */
	static char *ops[] = {
		"||",
		"&&",
		"==",
		"!=",
		"<=",
		">=",
		"<",
		">",
		"/=",
		"-",
		"+",
		"/",
		"*",
		"!",
		"\n"
	};
	
	//phanlog("eval_lhs_op_rhs");
	
	p = strcpy(line, expr);
	
  /*
   * initialize tokens, an array of pointers to locations
   * in line where the ops could possibly occur.
   */
	for (j = 0; *p; j++) {
		tokens[j] = p;
		if (*p == '(')
			p = matching_paren(p) + 1;
		else if (*p == '"')
			p = matching_quote(p) + 1;
		else if (isalnum(*p))
			for (p++; *p && (isalnum(*p) || isspace(*p)); p++);
		else
			p++;
	}
	tokens[j] = NULL;
	
	for (i = 0; *ops[i] != '\n'; i++)
		for (j = 0; tokens[j]; j++)
			if (!strn_cmp(ops[i], tokens[j], strlen(ops[i]))) {
				*tokens[j] = '\0';
				p = tokens[j] + strlen(ops[i]);
				
				eval_expr(line, lhr, go, sc, trig, type);
				eval_expr(p, rhr, go, sc, trig, type);
				eval_op(ops[i], lhr, rhr, result, go, sc, trig);
				
				return 1;
			}
			
	return 0;
}



/* returns 1 if cond is true, else 0 */
int process_if(char *cond, void *go, struct script_data *sc,
	trig_data *trig, int type)
{
	char result[MAX_INPUT_LENGTH], *p;
	//phanlog("int process_if");
	
	eval_expr(cond, result, go, sc, trig, type);
	
	p = result;
	skip_spaces(&p);
	
	if (!*p || *p == '0')
		return 0;
	else
		return 1;
}


/*
 * scans for end of if-block.
 * returns the line containg 'end', or the last
 * line of the trigger if not found.
 */
struct cmdlist_element *find_end(struct cmdlist_element *cl)
{
	struct cmdlist_element *c;
	char *p;
	phanlog("struct *find_end");
	
	if (!(cl->next))
		return cl;
	
	for (c = cl->next; c->next; c = c->next) {
		for (p = c->cmd; *p && isspace(*p); p++);
		
		if (!strn_cmp("if ", p, 3))
			c = find_end(c);
		else if (!strn_cmp("end", p, 3))
			return c;
	}
	
	return c;
}


/*
 * searches for valid elseif, else, or end to continue execution at.
 * returns line of elseif, else, or end if found, or last line of trigger.
 */
struct cmdlist_element *find_else_end(trig_data *trig,
	struct cmdlist_element *cl, void *go,
	struct script_data *sc, int type)
{
	struct cmdlist_element *c;
	char *p;
	phanlog("struct *find_else_end");
	
	if (!(cl->next))
		return cl;
	
	for (c = cl->next; c->next; c = c->next) {
		for (p = c->cmd; *p && isspace(*p); p++);
		
		if (!strn_cmp("if ", p, 3))
			c = find_end(c);
		
		else if (!strn_cmp("elseif ", p, 7)) {
			if (process_if(p + 7, go, sc, trig, type)) {
				GET_TRIG_DEPTH(trig)++;
				return c;
			}
		}
		
		else if (!strn_cmp("else", p, 4)) {
			GET_TRIG_DEPTH(trig)++;
			return c;
		}
		
		else if (!strn_cmp("end", p, 3))
			return c;
	}
	
	return c;
}


/* processes any 'wait' commands in a trigger */
void process_wait(void *go, trig_data *trig, int type, char *cmd,
	struct cmdlist_element *cl)
{
	char bufa[MAX_INPUT_LENGTH], *arg;
	struct wait_event_data *wait_event_obj;
	long time, hr, min, ntime;
	char c;
	
	extern struct time_info_data time_info;
	extern long dg_global_pulse;
	phanlog("process_wait");
	
	arg = any_one_arg(cmd, bufa);
	skip_spaces(&arg);
	
	if (!*arg) {
		sprintf(buf2, "Trigger: %s, VNum %d. wait w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cl->cmd);
		script_log(buf2);
	}
	
	else if (!strn_cmp(arg, "until ", 6)) {
		
    /* valid forms of time are 14:30 and 1430 */
		if (sscanf(arg, "until %ld:%ld", &hr, &min) == 2)
			min += (hr * 60);
		else
			min = (hr % 100) + ((hr / 100) * 60);
		
    /* calculate the pulse of the day of "until" time */
		ntime = (min * SECS_PER_MUD_HOUR * PASSES_PER_SEC) / 60;
		
    /* calculate pulse of day of current time */
		time = (dg_global_pulse % (SECS_PER_MUD_HOUR * PASSES_PER_SEC)) +
			(time_info.hours * SECS_PER_MUD_HOUR * PASSES_PER_SEC);
		
		if (time >= ntime) /* adjust for next day */
			time = (SECS_PER_MUD_DAY * PASSES_PER_SEC) - time + ntime;
		else
			time = ntime - time;
	}
	
	else {
		if (sscanf(arg, "%ld %c", &time, &c) == 2) {
			if (c == 't')
				time *= PULSES_PER_MUD_HOUR;
			else if (c == 's')
				time *= PASSES_PER_SEC;
		}
	}
	
	CREATE(wait_event_obj, struct wait_event_data, 1);
	wait_event_obj->trigger = trig;
	wait_event_obj->go = go;
	wait_event_obj->type = type;
	
	GET_TRIG_WAIT(trig) = add_event(time, trig_wait_event, wait_event_obj);
	trig->curr_state = cl->next;
	phanlog("process_wait - NIH");
}


/* processes a script set command */
void process_set(struct script_data *sc, trig_data *trig, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH], *value;
	phanlog("process_set");
	
	value = two_arguments(cmd, arg, name);
	
	skip_spaces(&value);
	
	if (!*name) {
		sprintf(buf2, "Trigger: %s, VNum %d. set w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	
	add_var(&GET_TRIG_VARS(trig), name, value);    
	
}

/* processes a script eval command */
void process_eval(void *go, struct script_data *sc, trig_data *trig,
	int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *expr;
	phanlog("process_eval");
	
	expr = two_arguments(cmd, arg, name);
	
	skip_spaces(&expr);
	
	if (!*name) {
		sprintf(buf2, "Trigger: %s, VNum %d. eval w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	
	eval_expr(expr, result, go, sc, trig, type);
	add_var(&GET_TRIG_VARS(trig), name, result);    
}

//Orione Inizio
/* script attaching a trigger to something */
void process_attach(void *go, struct script_data *sc, trig_data *trig, 
	int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], trignum_s[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *id_p;
	trig_data *newtrig;
	char_data *c=NULL;
	obj_data *o=NULL;
	room_data *r=NULL;
	long trignum, id;
  phanlog("process_attach");
  
	id_p = two_arguments(cmd, arg, trignum_s);
	skip_spaces(&id_p);

	if (!*trignum_s) {
		sprintf(buf2, "Trigger: %s, VNum %d. attach w/o an arg: '%s'", 
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}

	if (!id_p || !*id_p || atoi(id_p)==0) {
		sprintf(buf2, "Trigger: %s, VNum %d. attach invalid id arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}

	/* parse and locate the id specified */
	eval_expr(id_p, result, go, sc, trig, type);
	if (!(id = atoi(result))) {
		sprintf(buf2, "Trigger: %s, VNum %d. attach invalid id arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	c = find_char(id);
	if (!c) {
		o = find_obj(id);
		if (!o) {
			r = find_room(id);
			if (!r) {
				sprintf(buf2, "Trigger: %s, VNum %d. attach invalid id arg: '%s'",
					GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
				script_log(buf2);
				return;
			}
		}
	}

	/* locate and load the trigger specified */
	trignum = real_trigger(atoi(trignum_s));
	if (trignum<0 || !(newtrig=read_trigger(trignum))) {
		sprintf(buf2, "Trigger: %s, VNum %d. attach invalid trigger: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), trignum_s);
		script_log(buf2);
		return;
	}
	
	if (c) {
		if (!SCRIPT(c))
			CREATE(SCRIPT(c), struct script_data, 1);
		add_trigger(SCRIPT(c), newtrig, -1);
		return;
	}

	if (o) {
		if (!SCRIPT(o))
			CREATE(SCRIPT(o), struct script_data, 1);
		add_trigger(SCRIPT(o), newtrig, -1);
		return;
	}

	if (r) {
		if (!SCRIPT(r))
			CREATE(SCRIPT(r), struct script_data, 1);
		add_trigger(SCRIPT(r), newtrig, -1);
		return;
	}
	
}

/* script detaching a trigger from something */
void process_detach(void *go, struct script_data *sc, trig_data *trig,
	int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], trignum_s[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *id_p;
	char_data *c=NULL;
	obj_data *o=NULL;
	room_data *r=NULL;
	long id;
	phanlog("process_detach");
	
	id_p = two_arguments(cmd, arg, trignum_s);
	skip_spaces(&id_p);

	if (!*trignum_s) {
		sprintf(buf2, "Trigger: %s, VNum %d. detach w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	
	if (!id_p || !*id_p || atoi(id_p)==0) {
		sprintf(buf2, "Trigger: %s, VNum %d. detach invalid id arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	
	/* parse and locate the id specified */
	eval_expr(id_p, result, go, sc, trig, type);
	if (!(id = atoi(result))) {
		sprintf(buf2, "Trigger: %s, VNum %d. detach invalid id arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	c = find_char(id);
	if (!c) {
		o = find_obj(id);
		if (!o) {
			r = find_room(id);
			if (!r) {
				sprintf(buf2, "Trigger: %s, VNum %d. detach invalid id arg: '%s'",
					GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
				script_log(buf2);
				return;
			}
		}
	}
	
	if (c && SCRIPT(c)) {
		if (remove_trigger(SCRIPT(c), trignum_s)) {
			if (!TRIGGERS(SCRIPT(c))) {
				extract_script(SCRIPT(c));
				SCRIPT(c) = NULL;
			}
		}
		return;
	}
	
	if (o && SCRIPT(o)) {
		if (remove_trigger(SCRIPT(o), trignum_s)) {
			if (!TRIGGERS(SCRIPT(o))) {
				extract_script(SCRIPT(o));
				SCRIPT(o) = NULL;
			}
		}
		return;
	}

	if (r && SCRIPT(r)) {
		if (remove_trigger(SCRIPT(r), trignum_s)) {
			if (!TRIGGERS(SCRIPT(r))) {
				extract_script(SCRIPT(r));
				SCRIPT(r) = NULL;
			}
		}
		return;
	}
}

struct room_data *dg_room_of_obj(struct obj_data *obj) {						
		if (obj->in_room > NOWHERE) return &world[obj->in_room];
		if (obj->carried_by)        return &world[obj->carried_by->in_room];
		if (obj->worn_by)           return &world[obj->worn_by->in_room];
		if (obj->in_obj)            return (dg_room_of_obj(obj->in_obj));
		return NULL;
}
//Orione Fine

/*
 * processes a script return command.
 * returns the new value for the script to return.
 */
int process_return(trig_data *trig, char *cmd)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	phanlog("process_return");
	
	two_arguments(cmd, arg1, arg2);
	
	if (!*arg2) {
		sprintf(buf2, "Trigger: %s, VNum %d. return w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return 1;
	}
	
	return atoi(arg2);
}


/*
 * removes a variable from the global vars of sc,
 * or the local vars of trig if not found in global list.
 */
void process_unset(struct script_data *sc, trig_data *trig, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], *var;
	phanlog("process_unset");
	
	var = any_one_arg(cmd, arg);
	
	skip_spaces(&var);
	
	if (!*var) {
		sprintf(buf2, "Trigger: %s, VNum %d. unset w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	
	if (!remove_var(&(sc->global_vars), var))
		remove_var(&GET_TRIG_VARS(trig), var);
}


/*
 * makes a local variable into a global variable
 */
void process_global(struct script_data *sc, trig_data *trig, char *cmd)
{
	struct trig_var_data *vd;
	char arg[MAX_INPUT_LENGTH], *var;
	phanlog("process_global");
	
	var = any_one_arg(cmd, arg);
	
	skip_spaces(&var);
	
	if (!*var) {
		sprintf(buf2, "Trigger: %s, VNum %d. global w/o an arg: '%s'",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
		script_log(buf2);
		return;
	}
	
	for (vd = GET_TRIG_VARS(trig); vd; vd = vd->next)
		if (!str_cmp(vd->name, var))
			break;
		
	if (!vd) {
		sprintf(buf2, "Trigger: %s, VNum %d. local var '%s' not found in global call",
			GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), var);
		script_log(buf2);
		return;
	}    
	
	add_var(&(sc->global_vars), vd->name, vd->value);
	remove_var(&GET_TRIG_VARS(trig), vd->name);
}




/*  This is the core driver for scripts. */
int script_driver(void *go, trig_data *trig, int type, int mode)
{
	static int depth;
	int ret_val = 1;
	struct cmdlist_element *cl;
	char cmd[MAX_INPUT_LENGTH], *p;
	struct script_data *sc = 0;
	struct cmdlist_element *temp;	//Orione
	unsigned long loops = 0;		  //Orione
	
	void obj_command_interpreter(obj_data *obj, char *argument);
	void wld_command_interpreter(struct room_data *room, char *argument);
	
  //sprintf(phantomima," script_driver T %d type %d mode %d depth %d", GET_TRIG_VNUM(trig), type, mode, depth);
	phanlog("dg_script.c: script_driver");
	
	if (depth > MAX_SCRIPT_DEPTH) {
		script_log("Triggers recursed beyond maximum allowed depth.");
		return ret_val;
	}
	
	depth++;
	
	switch (type) {
		case MOB_TRIGGER:
		  if (SCRIPT((char_data *) go)) {
		  	//sprintf(phantomima," Mob is %s", GET_NAME((char_data *) go));
	      //phanlog(phantomima);
			  sc = SCRIPT((char_data *) go);
			}
			else
			  return (0);
			break;
		
		case OBJ_TRIGGER:
		  if (SCRIPT((obj_data *) go)) {
		  	//sprintf(phantomima," Obj is %s", GET_OBJ_NAME((obj_data *) go));
	      //phanlog(phantomima);
			  sc = SCRIPT((obj_data *) go);
			}
			else
			  return (0);
			break;
		
		case WLD_TRIGGER:
		  if (SCRIPT((room_data *) go)) {
		  	//sprintf(phantomima," Room is %d", GET_ROOM_VNUM((room_data *) go));
	      //phanlog(phantomima);
			  sc = SCRIPT((room_data *) go);
			}
			else
			  return (0);
			break;
	}
	
	if (mode == TRIG_NEW) {		    //Orione
		GET_TRIG_DEPTH(trig) = 1;   //Orione
		GET_TRIG_LOOPS(trig) = 0;   //Orione
	}                             //Orione
	
	for (cl = (mode == TRIG_NEW) ? trig->cmdlist : trig->curr_state;
		   cl && GET_TRIG_DEPTH(trig); cl = cl->next) {

		phanlog("Cacciatore-Cucco inside script_driver");		
		for (p = cl->cmd; *p && isspace(*p); p++);
		
		if (*p == '*')
			continue;

		else if (!strn_cmp(p, "if ", 3)) {
			if (process_if(p + 3, go, sc, trig, type))
				GET_TRIG_DEPTH(trig)++;
			else
				cl = find_else_end(trig, cl, go, sc, type);
		}
		
		else if (!strn_cmp("elseif ", p, 7) ||
			       !strn_cmp("else", p, 4)) {
			cl = find_end(cl);
			GET_TRIG_DEPTH(trig)--;
		}
		
		//Orione Inizio
		else if (!strn_cmp("while ", p, 6)) {
			temp = find_done(cl);
			if (process_if(p + 6, go, sc, trig, type)) {
				temp->original = cl;
			} else {
				cl = temp;
				loops = 0;
			}
		}	
		
		else if (!strn_cmp("switch ", p, 7))
			cl = find_case(trig, cl, go, sc, type, p + 7);
		//Orione Fine

		else if (!strn_cmp("end", p, 3)) 
			GET_TRIG_DEPTH(trig)--;
		
		//Orione Inizio
		else if (!strn_cmp("done", p, 4)) {
			char *orig_cmd = cl->original->cmd;
			while (*orig_cmd && isspace(*orig_cmd)) orig_cmd++;
			if (cl->original && process_if(orig_cmd + 6, go, sc, trig, type)) {
				cl = cl->original;
				loops++;
				GET_TRIG_LOOPS(trig)++;
				if (loops == 30) {
					process_wait(go, trig, type, "wait 1", cl);
					depth--;
					return ret_val;
				}
				if (GET_TRIG_LOOPS(trig) == 100) {
					char *bufo = (char*)malloc(MAX_STRING_LENGTH);
					sprintf(bufo,"SCRIPTERR: Trigger VNum %d has looped 100 times!!!", GET_TRIG_VNUM(trig));
					mudlog(bufo, NRM, LVL_GOD, TRUE);
					free(bufo);
				}
			}
		}	
		
		else if (!strn_cmp("break", p, 5))
			cl = find_done(cl);
			
		else if (!strn_cmp("case", p, 4)) {
			/* Do nothing, this allows multiple cases to a single instance */
		}
		//Orione Fine
		
		else {
			
			var_subst(go, sc, trig, type, p, cmd);
			
			if (!strn_cmp(cmd, "eval ", 5))
				process_eval(go, sc, trig, type, cmd);
			
			else if (!strn_cmp(cmd, "halt", 4))
				break;
			
			else if (!strn_cmp(cmd, "dg_cast ", 8))		//Orione
				do_dg_cast(go, sc, trig, type, cmd);	  //Orione

			else if (!strn_cmp(cmd, "dg_affect ", 10))	//Orione
				do_dg_affect(go, sc, trig, type, cmd);	  //Orione

			else if (!strn_cmp(cmd, "global ", 7))
				process_global(sc, trig, cmd);
			
			else if (!strn_cmp(cmd, "return ", 7))
				ret_val = process_return(trig, cmd);
			
			else if (!strn_cmp(cmd, "set ", 4))
				process_set(sc, trig, cmd);
			
			else if (!strn_cmp(cmd, "unset ", 6))
				process_unset(sc, trig, cmd);
			
			else if (!strn_cmp(cmd, "wait ", 5)) {
				process_wait(go, trig, type, cmd, cl);
				depth--;
				return ret_val;
			}
			
			else if (!strn_cmp(cmd, "attach ", 7))				//Orione
				process_attach(go, sc, trig, type, cmd);        //Orione

			else if (!strn_cmp(cmd, "detach ", 7))				//Orione
				process_detach(go, sc, trig, type, cmd);        //Orione
			
			else if (!strn_cmp(cmd, "version", 7))				//Orione
				mudlog(DG_SCRIPT_VERSION, NRM, LVL_GOD, TRUE);	//Orione
			
			else
				switch (type) {
					case MOB_TRIGGER:
						command_interpreter((char_data *) go, cmd);
						break;
					case OBJ_TRIGGER:
						obj_command_interpreter((obj_data *) go, cmd);
						break;
					case WLD_TRIGGER:
						wld_command_interpreter((struct room_data *) go, cmd);
						break;
				}
		}
	}
	
	free_varlist(GET_TRIG_VARS(trig));
	GET_TRIG_VARS(trig) = NULL;
	GET_TRIG_DEPTH(trig) = 0;
	
	depth--;
	phanlog("script_driver - NIH");
	return ret_val;
}

ACMD(do_tlist)
{
	
	int first, last, nr, found = 0;
	char pagebuf[65536];
	
	strcpy(pagebuf,"");
	
	two_arguments(argument, buf, buf2);
	
	if (!*buf) {
		send_to_char("Usage: tlist <begining number or zone> [<ending number>]\r\n", ch);
		return;
	}
	
	first = atoi(buf);
	if (*buf2) last = atoi(buf2);
	else {
		first *= 100;
		last = first+99;
	}
	//PEPPEXLIST
	if ((first < 0) || (first > 9999999) || (last < 0) || (last > 9999999)) {
		send_to_char("Values must be between 0 and 9999999.\n\r", ch);
		return;
	}
	
	if (first >= last) {
		send_to_char("Second value must be greater than first.\n\r", ch);
		return;
	}
	
	for (nr = 0; nr < top_of_trigt && (trig_index[nr]->virtual <= last); nr++)
	{
		if (trig_index[nr]->virtual >= first) {
			sprintf(buf, "%5d. [%5d] %s\r\n", ++found,
				trig_index[nr]->virtual,
				trig_index[nr]->proto->name);
			strcat(pagebuf, buf);
		}
	}
	
	if (!found)
		send_to_char("No triggers were found in those parameters.\n\r", ch);
	else page_string(ch->desc, pagebuf, TRUE);
}

int real_trigger(int vnum)
{
	int rnum;
	
	for (rnum=0; rnum < top_of_trigt; rnum++)
	{
		if (trig_index[rnum]->virtual==vnum) break;
	}
	
	if (rnum==top_of_trigt) rnum = -1;
	return (rnum);
}

ACMD(do_tstat)
{
	int vnum, rnum;
	char str[MAX_INPUT_LENGTH];
	
	half_chop(argument, str, argument);
	if (*str) {
		vnum = atoi(str);
		rnum = real_trigger(vnum);
		if (rnum<0) {
			send_to_char("That vnum does not exist.\r\n", ch);
			return;
		}
		
		do_stat_trigger(ch, trig_index[rnum]->proto);
	} else send_to_char("Usage: tstat <vnum>\r\n", ch);
}

//Orione Inizio
/*
 * scans for a case/default instance
 * returns the line containg the correct case instance, or the last
 * line of the trigger if not found.
 */
struct cmdlist_element *
find_case(struct trig_data *trig, struct cmdlist_element *cl,
				          void *go, struct script_data *sc, int type, char *cond)
{
	char result[MAX_INPUT_LENGTH];
	struct cmdlist_element *c;
	char *p, *bufo;
	phanlog("find_case");
	
	eval_expr(cond, result, go, sc, trig, type);
	
	if (!(cl->next))
		return cl;
	
	for (c = cl->next; c->next; c = c->next) {
		for (p = c->cmd; *p && isspace(*p); p++);
		
		if (!strn_cmp("while ", p, 6) || !strn_cmp("switch", p, 6))
			c = find_done(c);
		else if (!strn_cmp("case ", p, 5)) {
			bufo = (char*)malloc(MAX_STRING_LENGTH);
			/* new! improved! bug fixed! */
			eval_op("==", result, p + 5, bufo, go, sc, trig);
			if (*bufo && *bufo!='0') {
			/* end new! */
				free(bufo);
				return c;
			}
			free(bufo);
		} else if (!strn_cmp("default", p, 7))
			return c;
		else if (!strn_cmp("done", p, 3))
			return c;
	}
	return c;
}

/*
 * * scans for end of while/switch-blocks.
 * * returns the line containg 'end', or the last
 * * line of the trigger if not found.
 * */
struct cmdlist_element *
find_done(struct cmdlist_element *cl)
{
	struct cmdlist_element *c;
	char *p;
  phanlog("find_done");
	if (!(cl->next))
		return cl;

	for (c = cl->next; c->next; c = c->next) {
		for (p = c->cmd; *p && isspace(*p); p++);
		
		if (!strn_cmp("while ", p, 6) || !strn_cmp("switch ", p, 7))
			c = find_done(c);
		else if (!strn_cmp("done", p, 3))
			return c;
	}

	return c;
}
//Orione Fine

