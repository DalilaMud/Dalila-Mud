#ifndef _ARENA_H_
#define _ARENA_H_

#define ARENA_OFF	0
#define ARENA_START	1
#define ARENA_RUNNING	2
#define PULSE_ARENA	(15 RL_SEC)

#define SAME_ROOM 1

#define ARENA_ZONE   37  /* zone number */
#define ARENA_PREP_START  3700   /* vnum of first prep room	*/
#define ARENA_PREP_END    3703   /* vnum of last prep room	*/
#define ARENA_START_ROOM  3704   /* vnum of first real arena room	*/
#define ARENA_END_ROOM    3713   /* vnum of last real arena room	*/
#define HALL_FAME_FILE "etc/hallfame" /* the arena hall of fame */

#define MAX_BET	    1000000 /* max betable */
#define MIN_ARENA_COST	0	/* minimum cost per level */

struct hall_of_fame_element {
   char name[MAX_NAME_LENGTH+1];
   char lastname[MAX_NAME_LENGTH+1];
   time_t date;
   long award;
   struct  hall_of_fame_element *next;
};
/* in arena.c */
void start_arena();
void show_jack_pot();
void do_game();
int num_in_arena();
void find_game_winner();
void do_end_game();
void start_game();
void silent_end();
void write_fame_list(void);
void write_one_fame_node(FILE * fp, struct hall_of_fame_element * node);
void load_hall_of_fame(void);
void find_bet_winners(struct char_data *winner);

extern int in_arena;
extern int game_length;
extern int time_to_start;
extern int time_left_in_game;
extern int start_time;

#endif /* _ARENA_H_ */







