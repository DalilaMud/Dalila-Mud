/* ************************************************************************
*   File: mobact.c                                      Part of CircleMUD *
*  Usage: Functions for generating intelligent (?) behavior in mobiles    *
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
#include "db.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "spells.h"
#include "wilderness.h"
#include "clan.h"

/* external structs */
extern struct char_data *character_list;
extern struct index_data *mob_index;
extern struct room_data *world;
extern struct str_app_type str_app[];
extern struct dex_app_type dex_app[];

#define MOB_AGGR_TO_ALIGN (MOB_AGGR_EVIL | MOB_AGGR_NEUTRAL | MOB_AGGR_GOOD)

void appear(struct char_data *ch);
void hunt_victim(struct char_data * ch);
void chkmaster(struct char_data *ch);

extern int wild_mobhunter(struct char_data * ch);
extern int objlevel (struct obj_data * obj);
extern int find_eq_pos(struct char_data * ch, struct obj_data * obj, char *arg);
extern void wear_message(struct char_data * ch, struct obj_data * obj, int where);
extern void perform_get_from_container(struct char_data * ch, struct obj_data * obj, struct obj_data * cont, int mode);
extern int perform_go(struct char_data *ch);
extern void selvaggina_free(struct char_data *ch,int times);
extern void npc_backstab(struct char_data *ch,struct char_data *vict);
extern void add_follower(struct char_data * ch, struct char_data * leader);
SPECIAL(thief);

int npc_rescue(struct char_data * ch_hero, struct char_data * ch_victim)
{
  struct char_data *ch_bad_guy;

  for (ch_bad_guy = world[ch_hero->in_room].people;
        ch_bad_guy && (FIGHTING(ch_bad_guy) != ch_victim);
         ch_bad_guy = ch_bad_guy->next_in_room);
  if (ch_bad_guy) {
    if (ch_bad_guy == ch_hero)
      return FALSE;		/* NO WAY I'll rescue the one I'm fighting! */
    //act("Impavidamente Togli  $N dai Guai!.\r\n", FALSE, ch_hero, 0, ch_victim, TO_CHAR);
    act("Vieni Salvato da $N, Tuo servitore Fedelissimo!\r\n", FALSE, ch_victim, 0, ch_hero, TO_CHAR);
    act("$n Eroicamente Salva $N.", FALSE, ch_hero, 0, ch_victim, TO_NOTVICT);

    if (FIGHTING(ch_bad_guy))
      stop_fighting(ch_bad_guy);
    if (FIGHTING(ch_hero)!=ch_bad_guy)
      stop_fighting(ch_hero);

    set_fighting(ch_hero, ch_bad_guy);
    set_fighting(ch_bad_guy, ch_hero);
    return TRUE;
  } //End forif
  return FALSE;
}

void mobile_activity(void)
{
  register struct char_data *ch, *next_ch, *vict;
  struct char_data *min_vict = NULL;
  struct obj_data *obj, *next_obj, *best_obj, *cont;
  int door, found, max, where;
  int casual, max_abil=0, curr_abil, gold;
  memory_rec *names;
  room_rnum target_room;

  extern int no_specials;
  ACMD(do_get);

  for (ch = character_list; ch; ch = next_ch) {
    next_ch = ch->next;
    
    if (!IS_MOB(ch))
      continue;
    
    //lance ripristina il master!
 	  if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_SAVE))
 		  chkmaster(ch);

    // Examine call for special procedure
    if (MOB_FLAGGED(ch, MOB_SPEC) && !no_specials) {
      if (mob_index[GET_MOB_RNUM(ch)].func == NULL) {
	      sprintf(buf, "SYSERR: %s (#%d): Attempting to call non-existing mob func", GET_NAME(ch), GET_MOB_VNUM(ch));
	      log(buf);
	      REMOVE_BIT(MOB_FLAGS(ch), MOB_SPEC);
      }
      else {
	      //(mob_index[GET_MOB_RNUM(ch)].func) (ch, ch, 0, "");
	      if ((mob_index[GET_MOB_RNUM(ch)].func) (ch, ch, 0, ""))
	        continue;	     /*  go to next char*/
      }
    }

// If the mob has no specproc, do the default actions
    if (FIGHTING(ch) || !AWAKE(ch))
      continue;

// Nuovo Scavenger (picking up objects) by Rusty 
    if (MOB_FLAGGED(ch, MOB_SCAVENGER))
	  if (world[ch->in_room].contents && (number(0, 10) > 5)) {
		  max = -1000;
		  best_obj = NULL;
		  for (obj = world[ch->in_room].contents; obj; obj = obj->next_content)
	  	if (CAN_GET_OBJ(ch, obj) && objlevel(obj) > max) {
			  best_obj = obj;
			  max = objlevel(obj);
			}
    
		  if (best_obj != NULL) {
			  obj_from_room(best_obj);
			  obj_to_char(best_obj, ch);
			  act("$n prende $p.", FALSE, ch, best_obj, 0, TO_ROOM);

// Orione, tolta la procedura che fa indossare l'eq raccolto ai mob scavenger
// Scavenger Plus:Mob wear the best object
/*
			  where=find_eq_pos(ch, best_obj, 0);
			  if (CAN_WEAR(best_obj, ITEM_WEAR_WIELD))
			    where=WEAR_WIELD;

			  if ( (where>0) && GET_EQ(ch,where)) {
				  // se ce l'ha gia!
				  if ((objlevel((ch)->equipment[where]))< objlevel(best_obj)) {
				    obj_to_char((obj=unequip_char(ch, where)), ch);
					  act("$n smette di usare $p.", FALSE, ch, obj, 0, TO_ROOM);
				  } 
				  else {
					  where = NUM_WEARS; // cioe' oggetto non indossabile
				  }

				  if (GET_LEVEL(ch)>=objlevel(best_obj)) {
					  if (where>=0 && where <NUM_WEARS) {
						  obj_from_char(best_obj);
						  equip_char(ch,best_obj,where);
						  wear_message(ch,best_obj,where);
					  }
				  }
			  }
*/
		  }
	  } //End ifif

// Mob BodyGuard !?       
    if (MOB_FLAGGED(ch, MOB_BGUARD) && (ch->master)) {
      if (   (FIGHTING(ch->master) && (npc_rescue(ch, ch->master) == FALSE))
	        || (ch->master && WHOKILLED(ch->master)) ) {
        act("$N Si schiera Prontamente al tuo fianco!", FALSE,ch->master, 0, ch, TO_CHAR);
	      act("$n Si schiera prontamente al fianco di $N", FALSE, ch, 0, ch->master, TO_ROOM);
	      if (WHOKILLED(ch->master))	
	        hit(ch, WHOKILLED(ch->master), TYPE_UNDEFINED);/*in ogni caso assiste*/
	      else
	        hit(ch, FIGHTING(ch->master), TYPE_UNDEFINED);/*in ogni caso assiste*/
	    }
     
      if (FIGHTING(ch)) {
	      npc_rescue(ch, ch->master);
		    if (WHOKILLED(ch->master))
		      hit(ch, WHOKILLED(ch->master), TYPE_UNDEFINED);  /*in ogni caso assiste*/
		    continue;
		  }	  
    }

//MOB_THIEF
    if (   MOB_FLAGGED(ch, MOB_CRIMINAL)  && (GET_POS(ch) == POS_RESTING)
        && IS_AFFECTED(ch, AFF_HIDE) && GET_HIT(ch) >= 2*GET_MAX_HIT(ch)/3) {
      appear(ch);
      GET_POS(ch)=POS_STANDING;
    }
    if (   MOB_FLAGGED(ch, MOB_CRIMINAL) 
        && ( GET_POS(ch) == POS_STANDING) 
        && ( (casual = number(0, 11)) < 6 ) ) {   // 50%  Rubare

      // cerca le vittime  1) Devono aver almeno 1 coin  
      //                   2) Deve aver una buona prob di farcela
      for (found=FALSE, min_vict = NULL, vict = world[ch->in_room].people; vict; vict = vict->next_in_room) {
	      if (CAN_SEE(ch, vict) && (vict != ch)) {
	        if ((min_vict == NULL) || (max_abil < 100)) {
	        //se ha il 100% tanto vale fermarsi
	        //Calcolo dell abilita:presa esattamente da D n'D

	          curr_abil = MIN(90, (10*(GET_LEVEL(ch)) - 5*GET_LEVEL(vict)));
	          curr_abil -=  dex_app[GET_DEX(vict)].reaction;
	          if (GET_POS(vict) < POS_STANDING)
	            curr_abil += 200;
	    // Se la vittima e' addormentata-stunned - incap o morta ovviamente
	    //  deruberai questa sicuramente

	          if (   (curr_abil >= 40) 
	              && (GET_GOLD(vict) != 0) 
	              && (curr_abil > max_abil) ) {
	            min_vict = vict;
	            max_abil = curr_abil;
	            found=TRUE;
	          }
	        }
	      }
      }
      // Se ha trovato la vittima
      if (found == TRUE) {
	      if ((casual = number(1, 100)) > max_abil) {
	        if (!IS_NPC(min_vict))
	          act(" $n cerca di prendere dei soldi dal tuo Borsello", FALSE, ch, 0, min_vict, TO_CHAR);
	        else {
	          act("Oops...", FALSE, ch, 0, min_vict, TO_ROOM);
	          act("$n cerca di rubare soldi a $N..Ma viene Scoperto! ", FALSE, ch, 0, min_vict, TO_ROOM);
	        }
	        
	        //Beccato ... fa 2 tentativi di fuga
	        if (((door = number(1, 7)) < NUM_OF_DIRS) && CAN_GO(ch, door) &&
	             !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	          perform_move(ch,door,1,CAN_SHOW_ROOM);
	        if ((door -= 1) < NUM_OF_DIRS && CAN_GO(ch, door) &&
	             !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	          perform_move(ch,door,1,CAN_SHOW_ROOM);
	        if ((door += 2) < NUM_OF_DIRS && CAN_GO(ch, door) &&
	             !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	          perform_move(ch,door,1,CAN_SHOW_ROOM);
	        
	        if (IS_NPC(min_vict)) {
	          act("$n Urla: $N e' uno sporco LADRO!!!!", FALSE, min_vict, 0, ch, TO_ROOM);
	          hit(min_vict, ch, TYPE_UNDEFINED);
	        }
	      }
	      // Il Bastardo ce la fa!
	      else {
	        if (GET_POS(min_vict) < 5) {
	          GET_GOLD(ch) += GET_GOLD(min_vict);
	          GET_GOLD(min_vict) = 0;    //la ripulisce completamente
	        }
	        else {
	          gold = number((GET_GOLD(min_vict) / 10), (GET_GOLD(min_vict) / 2));
	          //gold = MIN(5000, gold);
	          if (gold > 0) {
	            GET_GOLD(ch) += gold;
	            GET_GOLD(min_vict) -= gold;
	            if (GET_GOLD(min_vict) < 0)
	              GET_GOLD(min_vict) = 0;
	          }
	        }
	        // Dopo il furto si allontana
	        if (((door = number(1, 6)) < NUM_OF_DIRS) && CAN_GO(ch, door) &&
	           !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	          perform_move(ch,door,1,CAN_SHOW_ROOM);
	        if ((door -=1) < NUM_OF_DIRS && CAN_GO(ch, door) &&
	           !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	          perform_move(ch,door,1,CAN_SHOW_ROOM);
	        if ((door +=2) < NUM_OF_DIRS && CAN_GO(ch, door) &&
	           !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	          perform_move(ch,door,1,CAN_SHOW_ROOM);
	      }
      }
      else { // Nessuna vittima appetibile:se ne va!
	      
	      if (((door = number(0, 6)) < NUM_OF_DIRS) && CAN_GO(ch, door) &&
	          !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) )
	        perform_move(ch, door, 1, CAN_SHOW_ROOM);
      }
    }

// Mob Sciacalli indossano EQ    
    if (MOB_FLAGGED(ch, MOB_NECRO) && !FIGHTING(ch) && AWAKE(ch))
    if (world[ch->in_room].contents) {
	    max = -1000;
	    best_obj=NULL;
	    for (cont = world[ch->in_room].contents;cont;cont = cont->next_content)
	    if (GET_OBJ_TYPE(cont) == ITEM_CONTAINER) {
	      for (obj = cont->contains; obj; obj = next_obj) {
		      next_obj = obj->next_content;
		      if (obj != NULL) {
		        if (CAN_SEE_OBJ(ch, obj) && ( (max = objlevel(obj)) <= GET_LEVEL(ch)) ) {
		          perform_get_from_container(ch, obj, cont, FIND_OBJ_INV);
		          if (obj != NULL) {
		            where = find_eq_pos(ch, obj, 0);
		            if (CAN_WEAR(obj, ITEM_WEAR_HOLD))
			            where = WEAR_HOLD;
		            if (GET_OBJ_TYPE(obj)==ITEM_LIGHT)
			            where = WEAR_LIGHT;
		            if (CAN_WEAR(obj, ITEM_WEAR_WIELD))
			            where = WEAR_WIELD;
		            if (where < 0) {
			            sprintf(buf,"SYSERR:pos < 0 ,in cont %s obj %d %s In room %d", 
			                        cont->name, 
			                        GET_OBJ_RNUM(obj), 
			                        obj->name, 
			                        IN_ROOM(ch));
			            log(buf);
			            where = NUM_WEARS;
		            }
		            
		            if (GET_EQ(ch, where) && where < NUM_WEARS) {
			            if ((objlevel((ch)->equipment[where])) < objlevel(obj)) {
			              obj_to_char((best_obj = unequip_char(ch, where)), ch);
			              act("$n smette di usare $p.",FALSE, ch, best_obj, 0, TO_ROOM);
			            }
			            else
			              where = NUM_WEARS;
		            }
		            if (where >= 0 && where < NUM_WEARS) {
			            wear_message(ch, obj, where);
			            obj_from_char(obj);
			            equip_char(ch, obj, where);
		            }
		          }
		        }
		      }
	      }
	    } //End forif
	  } //End ifif

// Mob Movement
    if (HUNTING(ch)) 
      hunt_victim(ch);
    else {
	    if (MOB_FLAGGED(ch , MOB_SEARCHER)) {}
 	    else {
        if (ch->in_room != NOWHERE) {
          if (!IS_IN_WILD(ch)) {
            if (   !MOB_FLAGGED(ch, MOB_SENTINEL) 
                && (GET_POS(ch) == POS_STANDING) 
                && (   (door = number(0, 8)) < NUM_OF_DIRS) 
                    && CAN_GO(ch, door) 
                    && !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_NOMOB | ROOM_DEATH) 
                    && (   !MOB_FLAGGED(ch, MOB_STAY_ZONE) 
                        || (world[EXIT(ch, door)->to_room].zone == world[ch->in_room].zone))) {
              perform_move(ch, door, 1, CAN_SHOW_ROOM);
            }
          }
          else {  // E' in wilderness
            if (MOB_FLAGGED(ch, MOB_WILDHUNT)) 
              door = wild_mobhunter(ch);
            else 
              door = number(0, 8);
            
            if (door >= 0 && door < NUM_OF_DIRS) 
              target_room = wild_target_room(ch, door);
            else 
              target_room = -1;
            
            if (   !MOB_FLAGGED(ch, MOB_SENTINEL) 
                && (GET_POS(ch) == POS_STANDING) 
                && (target_room != -1) 
                && (   !MOB_FLAGGED(ch, MOB_STAY_ZONE) 
                    || (world[target_room].wild_rnum == world[ch->in_room].wild_rnum))) {
              perform_move(ch, door, 1, CAN_SHOW_ROOM);
            }
          }
        }	
      }
    }

// Aggressive Mobs
    if (MOB_FLAGGED(ch, MOB_AGGRESSIVE | MOB_AGGR_TO_ALIGN)) {
      found = FALSE;
      for (vict = world[ch->in_room].people; vict && !found; vict = vict->next_in_room) {
	      if (IS_NPC(vict) || !CAN_SEE(ch, vict) || PRF_FLAGGED(vict, PRF_NOHASSLE))
	        continue;
	      if (number(0, 3) < 2)
	        continue; // Simple simulate Random a Joke from Phantom ;P
	      if (GET_ABIL (vict, ABIL_TRATTATIVA) >= EXPERT_LIV)
	        continue; // I pg con trattativa ad esperto fanno pace con gli aggressivi (by Spini)
	      if (controllo_volo(ch, vict))
		continue;
	      if (affected_by_spell (vict, SPELLSKILL, DISEASE_PESTE))
		continue;

	      if (MOB_FLAGGED(ch, MOB_AGGR_NO_PROP) && (MASTER_ID(ch)==GET_IDNUM(vict)))
	        continue;
	      if (MOB_FLAGGED(ch, MOB_AGGR_NO_CLAN) && (CLAN_ID(ch)==GET_CLAN(vict)))
	        continue;
	      if (MOB_FLAGGED(ch, MOB_AGGR_NO_AL_CLAN) && (GET_CLAN_DIPLO(GET_CLAN(ch),GET_CLAN(vict)) == ALLIANCE))
	        continue;
	      if (MOB_FLAGGED(ch, MOB_AGGR_NO_EN_CLAN) && (GET_CLAN_DIPLO(GET_CLAN(ch),GET_CLAN(vict)) == WAR))
	        continue;
	      if (MOB_FLAGGED(ch, MOB_AGGR_NO_PC_CLAN) && (GET_CLAN_DIPLO(GET_CLAN(ch),GET_CLAN(vict)) == PEACE))
	        continue;
	      if (MOB_FLAGGED(ch, MOB_AGGR_VAS_CLAN) && (GET_CLAN_DIPLO(GET_CLAN(ch),GET_CLAN(vict)) == VASSALLO))
	  	    continue;

        if (   !MOB_FLAGGED(ch, MOB_AGGR_TO_ALIGN) 
            || (MOB_FLAGGED(ch, MOB_AGGR_EVIL) && IS_EVIL(vict)) 
            || (MOB_FLAGGED(ch, MOB_AGGR_NEUTRAL) && IS_NEUTRAL(vict)) 
            || (MOB_FLAGGED(ch, MOB_AGGR_GOOD) && IS_GOOD(vict)) ) {
	        if (GET_MOB_SPEC(ch)== thief)
	          npc_backstab(ch,vict);
	        else
	          hit(ch, vict, TYPE_UNDEFINED);
	        found = TRUE;
	      }
      }
    }

// Mob Memory
    if (MOB_FLAGGED(ch, MOB_MEMORY) && MEMORY(ch)) {
      found = FALSE;
      for (vict = world[ch->in_room].people; vict && !found; vict = vict->next_in_room) {
        if (!CAN_SEE(ch, vict) || PRF_FLAGGED(vict, PRF_NOHASSLE))
          continue;
        for (names = MEMORY(ch); names && !found; names = names->next)
        if (names->id == GET_IDNUM(vict)) {
          found = TRUE;
          act("$n esclama, 'Hey!! Tu sei il tipo che mi ha attaccato!!!'", FALSE, ch, 0, 0, TO_ROOM);
          hit(ch, vict, TYPE_UNDEFINED);
        } //End forif
      }
    }

// Helper Mobs Paladino del bene
    if (   MOB_FLAGGED(ch, MOB_HELPER)
        && !MOB_FLAGGED(ch, MOB_CRIMINALHELPER) ) {
      found = FALSE;
      for (vict = world[ch->in_room].people; vict && !found; vict = vict->next_in_room)
      if (   ch != vict 
          && IS_NPC(vict) 
          && FIGHTING(vict) 
          && !IS_NPC(FIGHTING(vict)) 
          && ch != FIGHTING(vict) ) {			
        if (   MOB_FLAGGED(vict, MOB_CRIMINAL)
            || (   (ch->master)
                && (FIGHTING(vict) == ch->master) ) ) {
	        hit(ch, vict, TYPE_UNDEFINED);
	        found = TRUE;
	      } 		
	      else { 
	        act("$n arriva in aiuto di $N!", FALSE, ch, 0, vict, TO_ROOM);
	        hit(ch, FIGHTING(vict), TYPE_UNDEFINED);
	        found = TRUE;
	      }
	    } //End forif
    }
	
// Helper Mobs Servo del male
    if (   MOB_FLAGGED(ch, MOB_CRIMINALHELPER)
        && !MOB_FLAGGED(ch, MOB_HELPER) ) {
      found = FALSE;
      for (vict = world[ch->in_room].people; vict && !found; vict = vict->next_in_room)
      if (   ch != vict 
          && IS_NPC(vict) 
          && FIGHTING(vict) 
          && !IS_NPC(FIGHTING(vict)) 
          && ch != FIGHTING(vict) ) {			
        if (   MOB_FLAGGED(vict, MOB_CRIMINAL)
            || (   (ch->master) 
                && (vict == ch->master) ) ) {
          act("$n arriva in aiuto di $N!", FALSE, ch, 0, vict, TO_ROOM);
          hit(ch, FIGHTING(vict), TYPE_UNDEFINED);
          found = TRUE;
        }
      } //End forif
    }
	
// Add new mobile actions here

  }				// end for()
}



/* Mob Memory Routines */

/* make ch remember victim */
void remember(struct char_data * ch, struct char_data * victim)
{
  memory_rec *tmp;
  bool present = FALSE;

  if (!IS_NPC(ch) || IS_NPC(victim) || (GET_LEVEL(victim) >= LVL_IMMORT))
    return;

  for (tmp = MEMORY(ch); tmp && !present; tmp = tmp->next)
    if (tmp->id == GET_IDNUM(victim))
      present = TRUE;

  if (!present) {
    CREATE(tmp, memory_rec, 1);
    tmp->next = MEMORY(ch);
    tmp->id = GET_IDNUM(victim);
    MEMORY(ch) = tmp;
  }
}


/* make ch forget victim */
void forget(struct char_data * ch, struct char_data * victim)
{
  memory_rec *curr, *prev = NULL;

  if (!(curr = MEMORY(ch)))
    return;

  while (curr && curr->id != GET_IDNUM(victim)) {
    prev = curr;
    curr = curr->next;
  }

  if (!curr)
    return;			/* person wasn't there at all. */

  if (curr == MEMORY(ch))
    MEMORY(ch) = curr->next;
  else
    prev->next = curr->next;

  free(curr);
}


/* erase ch's memory */
void clearMemory(struct char_data * ch)
{
  memory_rec *curr, *next;

  curr = MEMORY(ch);

  while (curr) {
    next = curr->next;
    free(curr);
    curr = next;
  }

  MEMORY(ch) = NULL;
}


 /*Mob Go - Patrol by Lance*/
 void go_patrol_activity(int pulse)
 {
  register struct char_data *ch, *next_ch;
  //extern int no_specials;

  for (ch = character_list; ch; ch = next_ch) {
 	 next_ch = ch->next;

 	 if (!IS_MOB(ch))
 	  continue;

 	  /* If the mob has no specproc, do the default actions */
 	 if (FIGHTING(ch) || !AWAKE(ch))
 		continue;

 	 if (MOB_FLAGGED(ch , MOB_SEARCHER)) {

 		if (MOB_FLAGGED(ch , MOB_GO_FAST)) {
 			if (!(pulse % (2 * PASSES_PER_SEC)))
 			  perform_go(ch);
 		} else {
 		  if (!(pulse % (4 * PASSES_PER_SEC)))
 			perform_go(ch);
 		}
 	}
   }//for
 }


void selvaggina_activity(void)
{
 register struct char_data *ch, *next_ch;

  for (ch = character_list; ch; ch = next_ch) {
	  next_ch = ch->next;
 	  if (   ch 
 	      && MOB_FLAGGED(ch, MOB_SELVAGGINA) 
 	      && (!MOB_FLAGGED(ch, MOB_HARD_TAME)) 
 	     )
 		  selvaggina_free(ch, 4);
  }
}


void chkmaster(struct char_data *ch)
{
  struct char_data *masterplayer, *next_ch;

  if ((!ch->master) && (GET_MOB_SAVED_MASTER(ch) != -1))
  for (masterplayer = character_list; masterplayer; masterplayer = next_ch) {
 	  next_ch = masterplayer->next;
	  if (   masterplayer 
	      && !IS_NPC(masterplayer) 
	      && (GET_MOB_SAVED_MASTER(ch)==GET_IDNUM(masterplayer)) )
 		  add_follower(ch, masterplayer);
  }
}
