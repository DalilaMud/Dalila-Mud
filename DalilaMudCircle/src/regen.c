/* ************************************************************************
*  File: regen.c                                                          *
*                                                                         *
*  Usage: Contains routines to handle event based point regeneration      *
*                                                                         *
*  Written by Eric Green (ejg3@cornell.edu)                               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "spells.h"
#include "handler.h"
#include "events.h"


/* Player point types for events */
#define REGEN_HIT      0
#define REGEN_MANA     1
#define REGEN_MOVE     2

#define PULSES_PER_MUD_HOUR     (SECS_PER_MUD_HOUR*PASSES_PER_SEC)

int check_gen_abil(struct char_data *ch,int abilita);
int can_fly(struct char_data *ch);

/* event object structure for point regen */
struct regen_event_obj {
  struct char_data *ch;		/* character regening */
  int type;			/* HIT, MOVE, or MANA */
};


EVENTFUNC(points_event)
{
  bool is_good_char(struct char_data *ch);
  struct regen_event_obj *regen = (struct regen_event_obj *) event_obj;
  struct char_data *ch;
  int type, gain,reg=1,gained=1;
  char logbuf[100];
  
  ch = regen->ch;
  type = regen->type;
  if(!ch){
    log("SYSERR :Bad event_obj in queue.Removing event");
    free(event_obj);
    return 0;
  }

  /*  if ((GET_POS(ch) >= POS_STUNNED &&!AFF_FLAGGED(ch,AFF_TRAMORTITO))||(GET_COND(ch,FULL)<=24&&GET_POS(ch)>=POS_MORTALLYW)) {*/
if ((GET_POS(ch) >= POS_STUNNED &&!AFF_FLAGGED(ch,AFF_TRAMORTITO))){
    /* no help for the dying */

    /*
     * Increment type of points by one.
     * If not at max, reenqueue the event.
     */
      switch (type) {
    case REGEN_HIT:
    if((gain=hit_gain(ch))<0){
	gained=-1;
	gain*=-1;
      }
      if(gain==0)
	gained=0;
       if(!FIGHTING(ch))
	reg=check_gen_abil(ch,ABIL_REGENERATION);
      
      GET_HIT(ch) = MIN(GET_HIT(ch)+gained +reg, GET_MAX_HIT(ch));
      
      if (GET_POS(ch) <= POS_STUNNED||GET_HIT(ch)<=0)
	update_pos(ch);
      if (GET_HIT(ch) < GET_MAX_HIT(ch)) {
	/* reenqueue the event */
	return (PULSES_PER_MUD_HOUR / (gain ? gain : 1));
      }
      break;

    case REGEN_MANA:

      if((gain=mana_gain(ch))<0){
	gained=-1;
	gain*=-1;
      }
      if(gain==0)
	gained=0;
       if(!FIGHTING(ch))
	reg=check_gen_abil(ch,ABIL_REGENERATION);
      GET_MANA(ch) =MAX(0, MIN(GET_MANA(ch) +gained+ reg, GET_MAX_MANA(ch)));
      
      if (GET_MANA(ch) < GET_MAX_MANA(ch)) {
	/* reenqueue the event */
	return (PULSES_PER_MUD_HOUR / (gain ? gain : 1));
      }
      break;

    case REGEN_MOVE:

      if((gain=move_gain(ch))<0){
	gained=-1;
	gain*=-1;
      }
      if(gain==0)
	gained=0;
      GET_MOVE(ch) = MIN(GET_MOVE(ch) +gained, GET_MAX_MOVE(ch));
      
      if (GET_MOVE(ch) < GET_MAX_MOVE(ch)) {
	/* reenqueue the event */
	return (PULSES_PER_MUD_HOUR / (gain ? gain : 1));
      }
      break;

    default:
      sprintf(logbuf, "SYSERR:  Unknown points event type %d", type);
      log(logbuf);
      free(event_obj);
      return 0;
    }
  }

  /* kill this event */
  GET_POINTS_EVENT(ch, type) = NULL;
  free(event_obj);
  return 0;
}


/*
 * subtracts amount of hitpoints from ch's current and starts points event
 */
void alter_hit(struct char_data *ch, int amount)
{
  struct regen_event_obj *regen;
  long time;
  int gain;

  GET_HIT(ch) = MIN(GET_HIT(ch) - amount, GET_MAX_HIT(ch));

  if (GET_HIT(ch) <= HIT_INCAP||IS_AFFECTED(ch,AFF_TRAMORTITO))
    return;
  if (PLR_FLAGGED(ch, PLR_FANTASMA) && amount > 0)
    return;

  if (GET_HIT(ch) < GET_MAX_HIT(ch) && !GET_POINTS_EVENT(ch, REGEN_HIT)) {
    CREATE(regen, struct regen_event_obj, 1);	
    regen->ch = ch;
    regen->type = REGEN_HIT;
    gain = hit_gain(ch);
    time = PULSES_PER_MUD_HOUR / (gain ? gain : 1);
    GET_POINTS_EVENT(ch, REGEN_HIT) = event_create(points_event, regen, time);

    if (amount >= 0) {
	
      /*
       * if the character gained hp, update position and
       * restart mana and move regeneration if needed.
       */
      update_pos(ch);
      alter_mana(ch, 0);
      alter_move(ch, 0);
    }
  }
}


/*
 * subtracts amount of mana from ch's current and starts points event
 */
void alter_mana(struct char_data *ch, int amount)
{
  struct regen_event_obj *regen;
  long time;
  int gain;

  GET_MANA(ch) = MIN(GET_MANA(ch) - amount, GET_MAX_MANA(ch));
 
  if (!GET_POINTS_EVENT(ch, REGEN_MANA) && (GET_MANA(ch) < GET_MAX_MANA(ch))) {

    /* make sure the character isn't dying */
    if (GET_POS(ch) >= POS_STUNNED) {
      CREATE(regen, struct regen_event_obj, 1);
      regen->ch = ch;
      regen->type = REGEN_MANA;
      gain = mana_gain(ch);
      time = PULSES_PER_MUD_HOUR / (gain ? gain : 1);
      GET_POINTS_EVENT(ch, REGEN_MANA) =
	event_create(points_event, regen, time);
    }
  }
}


/*
 * subtracts amount of moves from ch's current and starts points event
 */
void alter_move(struct char_data *ch, int amount)
{
  struct regen_event_obj *regen;
  long time;
  int gain;
    
//  FIATO
//  GET_FIATO(ch)=GET_FIATO(ch)-1;
//  send_to_char("ti tolgo un punticino piccolino piccolino\r\n",ch);
//  FIATO
//	if(GET_FIATO(ch)==0){
//	send_to_char("Devi riprendere fiato\r\n",ch);
//	return 0;
//	}
//	if(!IS_MOB(ch)){ 
//     if(GET_MOB_VNUM(ch)!=9743)
//			GET_FIATO(ch)=GET_FIATO(ch)-1;
//	}
/* else */
  if(!IS_MOB(ch))
	if(!(PLR_FLAGGED(ch,PLR_FANTASMA)))
				if(!RIDING(ch))
					if(!can_fly(ch))
						GET_FIATO(ch)=GET_FIATO(ch)-(amount/2);
  
	GET_MOVE(ch) = MIN(GET_MOVE(ch) - amount, GET_MAX_MOVE(ch));
 
  if (!GET_POINTS_EVENT(ch, REGEN_MOVE) && (GET_MOVE(ch) < GET_MAX_MOVE(ch))) {

    /* make sure the character isn't dying */
    if (GET_POS(ch) >= POS_STUNNED) {
      CREATE(regen, struct regen_event_obj, 1);
      regen->ch = ch;
      regen->type = REGEN_MOVE;
      gain = move_gain(ch);
      time = PULSES_PER_MUD_HOUR / (gain ? gain : 1);
      GET_POINTS_EVENT(ch, REGEN_MOVE) =
	event_create(points_event, regen, time);
    }
  }
}


/* updates regen rates.  Use when big regen rate changes are made */
void check_regen_rates(struct char_data *ch)
{
  struct regen_event_obj *regen;
  int type, gain = 0;
  long time;

  if (GET_HIT(ch) <= HIT_INCAP|| IS_AFFECTED(ch,AFF_TRAMORTITO))
    return;
  for (type = REGEN_HIT; type <= REGEN_MOVE; type++) {
    
    switch (type) {
    case REGEN_HIT:
      if (GET_HIT(ch) >= GET_MAX_HIT(ch))
	continue;
      gain = hit_gain(ch);
      break;

    case REGEN_MANA:
      if (GET_MANA(ch) >= GET_MAX_MANA(ch))
	continue;
      gain = mana_gain(ch);
      break;

    case REGEN_MOVE:
      if (GET_MOVE(ch) >= GET_MAX_MOVE(ch))
	continue;
      gain = move_gain(ch);
      break;
    }

    time = PULSES_PER_MUD_HOUR / (gain ? gain : 1);

    if (!GET_POINTS_EVENT(ch, type) ||
	(time < event_time(GET_POINTS_EVENT(ch, type)))) {

      /* take off old event, create updated event */
      if (GET_POINTS_EVENT(ch, type))
	event_cancel(GET_POINTS_EVENT(ch, type));
	  GET_POINTS_EVENT(ch, type) = NULL;
      CREATE(regen, struct regen_event_obj, 1);
      regen->ch = ch;
      regen->type = type;
      GET_POINTS_EVENT(ch, type) = event_create(points_event, regen, time);
    }
  }
}

/*
bool is_good_char(struct char_data *ch){
  extern int top_of_world;
  char logbuf[100];
  if(ch)
    if(!GET_NAME(ch)||GET_SEX(ch)>2||GET_LEVEL(ch)>200||IN_ROOM(ch)>(top_of_world+1)){
	  sprintf(logbuf,"SYSERR :%s - Corrupted event NPC / PC! (%d,%d,%d)" ,GET_NAME(ch) ? GET_NAME(ch) : "nothing", 
	   GET_SEX(ch), GET_LEVEL(ch), IN_ROOM(ch));
	  log(logbuf);
      return FALSE;
	}  
  if(!ch){
     log("SYSERR :Not ch!");
    return FALSE;	
	}
  return TRUE;
}

*/




