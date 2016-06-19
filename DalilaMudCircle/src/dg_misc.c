/**************************************************************************
*  File: dg_misc.c                                                        *
*  Usage: contains general functions for script usage.                    *
*                                                                         *
*                                                                         *
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
#include "spells.h"

/* copied from spell_parser.c: */
#define SINFO spell_info[spellnum]

/* external vars */
extern struct index_data **trig_index;
extern struct room_data *world;
extern const char *item_types[];
extern struct spell_info_type spell_info[];
extern const char *apply_types[];
extern const char *affected_bits[];
extern const char *affected_bits_1[];
extern const char *affected_bits_2[];
extern const char *affected_bits_3[];



/* cast a spell; can be called by mobiles, objects and rooms, and no   */
/* level check is required. Note that mobs should generally use the    */
/* normal 'cast' command (which must be patched to allow mobs to cast  */
/* spells) as the spell system is designed to have a character caster, */
/* and this cast routine may overlook certain issues.                  */
/* LIMITATION: a target MUST exist for the spell unless the spell is   */
/* set to TAR_IGNORE. Also, group spells are not permitted             */
/* code borrowed from do_cast() */
void do_dg_cast(void *go, struct script_data *sc, trig_data *trig,
		 int type, char *cmd)
{
  struct char_data *caster = NULL;
  struct char_data *tch = NULL;
  struct obj_data *tobj = NULL;
  struct room_data *caster_room = NULL;
  char *s, *t;
  int spellnum, target = 0;


  /* need to get the caster or the room of the temporary caster */
  switch (type) {
    case MOB_TRIGGER:
      caster = (struct char_data *)go;
      break;
    case WLD_TRIGGER:
      caster_room = (struct room_data *)go;
      break;
    case OBJ_TRIGGER:
      caster_room = dg_room_of_obj((struct obj_data *)go);
      if (!caster_room) {
        script_log("dg_do_cast: unknown room for object-caster!");
        return;
      }
      break;
    default:
      script_log("dg_do_cast: unknown trigger type!");
      return;
  }

  /* get: blank, spell name, target name */
  s = strtok(cmd, "'");
  if (s == NULL) {
    sprintf(buf2, "Trigger: %s, VNum %d. dg_cast needs spell name.",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
    script_log(buf2);
    return;
  }
  s = strtok(NULL, "'");
  if (s == NULL) {
    sprintf(buf2, "Trigger: %s, VNum %d. dg_cast needs spell name in `'s.",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
    script_log(buf2);
    return;
  }
  t = strtok(NULL, "\0");

  /* spellnum = search_block(s, spells, 0); */
  spellnum = find_skill_num(s);
  if ((spellnum < 1) || (spellnum > MAX_SPELLS)) {
    sprintf(buf2, "Trigger: %s, VNum %d. dg_cast: invalid spell name (%s)",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
    script_log(buf2);
    return;
  }

  /* Find the target */
  if (t != NULL) {
    one_argument(strcpy(arg, t), t);
    skip_spaces(&t);
  }
  if (IS_SET(SINFO.targets, TAR_IGNORE)) {
    target = TRUE;
  } else if (t != NULL && *t) {
    if (!target &&
          (IS_SET(SINFO.targets, TAR_CHAR_ROOM) ||
           IS_SET(SINFO.targets, TAR_CHAR_WORLD))) {
      if ((tch = get_char(t)) != NULL)
        target = TRUE;
    }

    if (!target &&
          (IS_SET(SINFO.targets, TAR_OBJ_INV) ||
           IS_SET(SINFO.targets, TAR_OBJ_EQUIP) ||
           IS_SET(SINFO.targets, TAR_OBJ_ROOM) ||
           IS_SET(SINFO.targets, TAR_OBJ_WORLD))) {
      if ((tobj = get_obj(t)) != NULL)
        target = TRUE;
    }

    if (!target) {
      sprintf(buf2, "Trigger: %s, VNum %d. dg_cast: target not found (%s)",
        GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
      script_log(buf2);
      return;
    }
  }

  if (IS_SET(SINFO.routines, MAG_GROUPS)) {
    sprintf(buf2,
      "Trigger: %s, VNum %d. dg_cast: group spells not permitted (%s)",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), cmd);
    script_log(buf2);
    return;
  }

  if (!caster) {
    caster = read_mobile(DG_CASTER_PROXY, VIRTUAL);
    if (!caster) {
      script_log("dg_cast: Cannot load the caster mob!");
      return;
    }
    /* set the caster's name to that of the object, or the gods.... */
    /* take select pieces from char_to_room(); */
    if (type==OBJ_TRIGGER)
      caster->player.short_descr =
        str_dup(((struct obj_data *)go)->short_description);
    else if (type==WLD_TRIGGER)
      caster->player.short_descr = str_dup("The gods");
    caster->next_in_room = caster_room->people;
    caster_room->people = caster;
    caster->in_room = real_room(caster_room->number);
    call_magic(caster, tch, tobj, spellnum, GET_LEVEL(caster), CAST_SPELL);
    extract_char(caster);
  } else
    call_magic(caster, tch, tobj, spellnum, GET_LEVEL(caster), CAST_SPELL);
}


/* modify an affection on the target. affections can be of the AFF_x  */
/* variety or APPLY_x type. APPLY_x's have an integer value for them  */
/* while AFF_x's have boolean values. In any case, the duration MUST  */
/* be non-zero.                                                       */
/* usage:  apply <target> <property> <value> <duration>               */
#define APPLY_TYPE	1
#define AFFECT_TYPE	2
void do_dg_affect(void *go, struct script_data *sc, trig_data *trig,
		  int script_type, char *cmd)
{
  struct char_data *ch = NULL;
  int value=0, duration=0;
  char junk[MAX_INPUT_LENGTH]; /* will be set to "dg_affect" */
  char charname[MAX_INPUT_LENGTH], property[MAX_INPUT_LENGTH];
  char value_p[MAX_INPUT_LENGTH], duration_p[MAX_INPUT_LENGTH];
  int index=0, type=0, salvataggio=0;
  struct affected_type af;


  half_chop(cmd, junk, cmd);
  half_chop(cmd, charname, cmd);
  half_chop(cmd, property, cmd);
  half_chop(cmd, value_p, duration_p);

  /* make sure all parameters are present */
  if (!charname || !*charname || !property || !*property ||
      !value_p || !*value_p || !duration_p || !*duration_p) {
    sprintf(buf2, "Trigger: %s, VNum %d. dg_affect usage: <target> <property> <value> <duration>",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
    script_log(buf2);
    return;
  }

  value = atoi(value_p);
  duration = atoi(duration_p);
  if (duration <= 0) {
    sprintf(buf2, "Trigger: %s, VNum %d. dg_affect: need positive duration!",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
    script_log(buf2);
    return;
  }

  /* find the property -- first search apply_types */
  index = 0;
  while (str_cmp(apply_types[index], "\n")) {
    if (!str_cmp(apply_types[index], property)) {
      //type=APPLY_TYPE;
      type = index;
      break;
    }
    index++;
  }

  if (!type) { /* search affect_types now */
    index = 0;
    while (str_cmp(affected_bits[index], "\n")) {
      if (!str_cmp(affected_bits[index], property)) {
        //type=AFFECT_TYPE;
        type = index;
        break;
      }
      index++;
    }
  }

  if (!type) { /* cerca nel secondo bitvector */
    index = 0;
    salvataggio++;
    while (str_cmp(affected_bits_1[index], "\n")) {
      if (!str_cmp(affected_bits_1[index], property)) {
        type = index;
        break;
      }
      index++;
    }
  }

  if (!type) { /* cerca nel terzo bitvector */
    index = 0;
    salvataggio++;
    while (str_cmp(affected_bits_2[index], "\n")) {
      if (!str_cmp(affected_bits_2[index], property)) {
        type = index;
        break;
      }
      index++;
    }
  }

  if (!type) { /* cerca nel quarto bitvector */
    index = 0;
    salvataggio++;
    while (str_cmp(affected_bits_3[index], "\n")) {
      if (!str_cmp(affected_bits_3[index], property)) {
        type = index;
        break;
      }
      index++;
    }
  }

  if (!type) { /* property not found */
    sprintf(buf2, "Trigger: %s, VNum %d. dg_affect: unknown property '%s'!",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), property);
    script_log(buf2);
    return;
  }


  /* locate the target */
  ch = get_char(charname);
  if (!ch) {
    sprintf(buf2, "Trigger: %s, VNum %d. dg_affect: cannot locate target!",
      GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
    script_log(buf2);
    return;
  }

  /* add the affect */
  af.type = 0;
  af.duration = duration;
  af.modifier = value;
  af.location = 0;
  af.bitvector[0] = af.bitvector[1] = af.bitvector[2] = af.bitvector[3] = 0;
  af.bitvector[salvataggio] = (1<<type);
  affect_to_char(ch, &af);
}
