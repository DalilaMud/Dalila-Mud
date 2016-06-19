/* ************************************************************************
*   File: graph.c                                       Part of CircleMUD *
*  Usage: various graph algorithms                                        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


#define TRACK_THROUGH_DOORS

/* You can define or not define TRACK_THOUGH_DOORS, above, depending on
   whether or not you want track to find paths which lead through closed
   or hidden doors.
*/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"


/* Externals */
extern int top_of_world;
extern const char *dirs[];
extern struct room_data *world;
extern struct zone_data *zone_table;
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);

#define NUM_PRIGIONI 10
int prison_info[][2]={
	{125,12542},/*Deira 2*/
	{30,3192},/*Deira vecchia*/
	{31,3192},
	{54,0},/*Jirock*/
	{66,6813},/*Lamorka*/
	{280,28071},/*Lamorka*/
	{90,9160},/*Cimmura*/
	{97,9836},/*Cimmura nuova*/
	{85,8578}, /*Cimmura Jerold*/
	{212,21261},/*Chireillos*/
	{500, 50137} // ALORN
};

int find_prison(struct char_data *ch){
	struct room_data *room =&world[ch->in_room];
	int prison_zone=zone_table[room->zone].number,i;
	for(i=0;i< NUM_PRIGIONI;i++)
		if(prison_zone== prison_info[i][0])
			return prison_info[i][1];
	return 0;
}

char *itoa(int num,char *string){
	sprintf(string,"%d",num);
	return string;
}

struct bfs_queue_struct {
	room_rnum room;
	char dir;
	struct bfs_queue_struct *next;
};

static struct bfs_queue_struct *queue_head = 0, *queue_tail = 0;

/* Utility macros */
#define MARK(room) (SET_BIT(ROOM_FLAGS(room), ROOM_BFS_MARK))
#define UNMARK(room) (REMOVE_BIT(ROOM_FLAGS(room), ROOM_BFS_MARK))
#define IS_MARKED(room) (IS_SET(ROOM_FLAGS(room), ROOM_BFS_MARK))
#define TOROOM(x, y) (world[(x)].dir_option[(y)]->to_room)
#define IS_CLOSED(x, y) (IS_SET(world[(x)].dir_option[(y)]->exit_info, EX_CLOSED))

#ifdef TRACK_THROUGH_DOORS
#define VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
			  (TOROOM(x, y) != NOWHERE) &&	\
			  (!ROOM_FLAGGED(TOROOM(x, y), ROOM_NOTRACK)) && \
			  (!IS_MARKED(TOROOM(x, y))))
#else
#define VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
			  (TOROOM(x, y) != NOWHERE) &&	\
			  (!IS_CLOSED(x, y)) &&		\
			  (!ROOM_FLAGGED(TOROOM(x, y), ROOM_NOTRACK)) && \
			  (!IS_MARKED(TOROOM(x, y))))
#endif

void bfs_enqueue(room_rnum room, int dir)
{
	struct bfs_queue_struct *curr;
	
	CREATE(curr, struct bfs_queue_struct, 1);
	curr->room = room;
	curr->dir = dir;
	curr->next = 0;
	
	if (queue_tail) {
		queue_tail->next = curr;
		queue_tail = curr;
	} else
		queue_head = queue_tail = curr;
}


void bfs_dequeue(void)
{
	struct bfs_queue_struct *curr;
	
	curr = queue_head;
	
	if (!(queue_head = queue_head->next))
		queue_tail = 0;
	free(curr);
}


void bfs_clear_queue(void)
{
	while (queue_head)
		bfs_dequeue();
}


/* find_first_step: given a source room and a target room, find the first
   step on the shortest path from the source to the target.

   Intended usage: in mobile_activity, give a mob a dir to go if they're
   tracking another mob or a PC.  Or, a 'track' skill for PCs.
*/

int find_first_step(room_rnum src, sh_int target)
{
	int curr_dir;
	room_rnum curr_room;
	
	if (src < 0 || src > top_of_world || target < 0 || target > top_of_world) {
		log("Illegal value passed to find_first_step (graph.c)");
		return BFS_ERROR;
	}
	if (src == target)
		return BFS_ALREADY_THERE;
	
  /* clear marks first */
	for (curr_room = 0; curr_room <= top_of_world; curr_room++)
		UNMARK(curr_room);
	
	MARK(src);
	
  /* first, enqueue the first steps, saving which direction we're going. */
	for (curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++)
		if (VALID_EDGE(src, curr_dir)) {
			MARK(TOROOM(src, curr_dir));
			bfs_enqueue(TOROOM(src, curr_dir), curr_dir);
		}
  /* now, do the classic BFS. */
	while (queue_head) {
		if (queue_head->room == target) {
			curr_dir = queue_head->dir;
			bfs_clear_queue();
			return curr_dir;
		} else {
			for (curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++)
				if (VALID_EDGE(queue_head->room, curr_dir)) {
					MARK(TOROOM(queue_head->room, curr_dir));
					bfs_enqueue(TOROOM(queue_head->room, curr_dir), queue_head->dir);
				}
			bfs_dequeue();
		}
	}
	
	return BFS_NO_PATH;
}


void wild_track(struct char_data *hunter,struct char_data *target,int range)
{
	ACMD(do_move);
	int hunter_room=hunter->in_room;
	int target_room=target->in_room;
	int yhunt=(hunter_room-1400400)/1000;
	int xhunt=(hunter_room-1400400)-yhunt*1000;
	int ytarget=(target_room-1400400)/1000;
	int xtarget=(target_room-1400400)-ytarget*1000;
	int diffy=yhunt-ytarget;
	int diffx=xhunt-xtarget;
	int dirx=0,diry=0;
	char argument[10];
	if(diffy>0)
		diry=SCMD_NORTH;/*nord*/
	else{
		diffy=diffy*-1;
		diry=SCMD_SOUTH;/*sud*/
	}
	if(diffx>0)
		diry=SCMD_WEST;/*weast*/
	else{
		dirx=SCMD_EAST;/*est*/
		diffx=diffx*-1;
	}
	if(range<diffx)
		diffx=range;
	if(range<diffy)
		diffy=range;
	
	do_move(hunter,itoa(diffx,argument),0,dirx);
	do_move(hunter,itoa(diffy,argument),0,diry);
}

bool track_n_move(struct char_data *ch,int target){
	void do_doorcmd(struct char_data *ch,struct obj_data *obj,int door,int scmd);
	int dir;
	if((target=real_room(target))==-1)
		return FALSE;
	dir = find_first_step(ch->in_room, target);
	act("WOW",FALSE,0,0,0,TO_ROOM);
	switch (dir) {
		case BFS_ERROR:
			send_to_char("Hmm.. something seems to be wrong.\r\n", ch);
			break;
		case BFS_ALREADY_THERE:
      // act("$n urla 'SONO ARRIVATO A DESTINAZIONE'",FALSE,ch,0,0,TO_ROOM);
			return TRUE;
			break;
		case BFS_NO_PATH:
			act("$n urla '&1&bNON TROVO LA PRIGIONE!&0'",FALSE,ch,0,0,TO_ROOM);
			return FALSE;
			break;
		default:
			if(IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED)){
	// if(perform_move(ch, dir, 1, CAN_SHOW_ROOM)==0){
	//	if (EXIT(ch, dir)->keyword){
				do_doorcmd(ch, NULL, dir, SCMD_UNLOCK);
				do_doorcmd(ch, NULL, dir, SCMD_OPEN);
				
      // perform_move(ch, dir, 1, CAN_SHOW_ROOM);
				return track_n_move(ch,target);
			}
			perform_move(ch, dir, 1, CAN_SHOW_ROOM);
			return FALSE;
			break;
	}
	return FALSE;
}

/************************************************************************
*  Functions and Commands which use the above fns		        *
************************************************************************/

ACMD(do_track)
{
	struct char_data *vict;
	int dir, num;
	extern struct spell_info_type spell_info[];
	
	if (!CAN_LEARN_SKILL(ch, SKILL_TRACK) || !GET_SKILL(ch, SKILL_TRACK)) {
		send_to_char("Non hai idea di come farlo.\r\n", ch);
		return;
	}
	one_argument(argument, arg);
	if (!*arg) {
		send_to_char("Chi vorresti cercare?\r\n", ch);
		return;
	}
	if (!(vict = get_char_vis(ch, arg))) {
		send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
		return;
	}
	//PEPPE VISIBILITA
	if (PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non puoi farlo finche e' un fantasma.\r\n",ch);
		return;
	}
	if (!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_IMMORT){
		send_to_char("Non sembra stia giocando al tuo stesso gioco.\r\n",ch);
		return;
	}
	if (IS_AFFECTED(vict, AFF_NOTRACK)) {
		send_to_char("Non trovi nessuna traccia.\r\n", ch);
		return;
	}
	dir = find_first_step(ch->in_room, vict->in_room);
	
	switch (dir) {
		case BFS_ERROR:
			send_to_char("Hmm.. something seems to be wrong.\r\n", ch);
			break;
		case BFS_ALREADY_THERE:
			send_to_char("Guarda bene, sei nella stessa stanza!!\r\n", ch);
			break;
		case BFS_NO_PATH:
			sprintf(buf, "Non trovi nessuna traccia di %s qui.\r\n",
				HMHR(vict));
			send_to_char(buf, ch);
			break;
		default:
			num = number(0, 101);	/* 101% is a complete failure */
			if (!test_and_improve(ch, SKILL_TRACK, 100, 15, 0))
				do {
					dir = number(0, NUM_OF_DIRS - 1);
				}
				while (!CAN_GO(ch, dir));
			sprintf(buf, "Le tracce vanno a %s!\r\n", dirs[dir]);
			send_to_char(buf, ch);
			break;
	}
}

void hunt_victim(struct char_data * ch)
{
	ACMD(do_say);
	extern struct char_data *character_list;
	
	int dir;
	byte found;
	struct char_data *tmp;
	
	if (!ch || !HUNTING(ch) || FIGHTING(ch))
		return;
	if (GET_POS(ch) <= POS_FIGHTING)
		return;
	
  /* make sure the char still exists */
	for (found = 0, tmp = character_list; tmp && !found; tmp = tmp->next)
		if (HUNTING(ch) == tmp)
			found = 1;
		
	if (!found) {
		do_say(ch, "Damn!  My prey is gone!!", 0, 0);
		HUNTING(ch) = 0;
		return;
	}
	dir = find_first_step(ch->in_room, HUNTING(ch)->in_room);
	if ((dir < 0) && (ch->in_room != HUNTING(ch)->in_room)) {
		act("$n says, 'Damn!  I lost $N!!'", FALSE, ch, 0, (HUNTING(ch)), TO_ROOM);
		HUNTING(ch) = 0;
		return;
	} else if (ch->in_room != HUNTING(ch)->in_room) {
		perform_move(ch, dir, 1, CAN_SHOW_ROOM);
		if (ch->in_room == HUNTING(ch)->in_room) {
			if (!ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)) {
				do_say(ch, "Sei un codardo!  Ti uccidero'!!", 0, 0);
				act("$n lunges at $N!!", FALSE, ch, 0, (HUNTING(ch)), TO_ROOM);
				hit(ch, HUNTING(ch), TYPE_UNDEFINED);
			} else {
				if (!MOB_FLAGGED(ch, MOB_ASSASSIN))
					act("$n says, 'Codardo!  Esci da li' e combatti $N!'", FALSE, ch, 0, (HUNTING(ch)), TO_ROOM);
			}
		}
		return;
	}
}





























