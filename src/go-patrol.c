/********************************************************************
	Modulo Go - Patrol realizzato da Micale Adriano il 17-11-98
	Ad uso esclusivo di Dalila Mud

	Modifiche:
	Adriano Micale  21/12/98

	Release sperimentale 0.1
	Questa versione e' da considerarsi provvisoria.
**********************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "wilderness.h"
#include "db.h"
#include "interpreter.h"
#include "handler.h"
#include "clan.h"


#define NO_CAN_GO -1
#define TARGET_FOUND -2
#define MISSING_TARGET -3
#define NO_CAN_GO_N -4
#define NO_CAN_GO_E -5
#define NO_CAN_GO_S -6
#define NO_CAN_GO_W -7
#define NO_CAN_GO_U -8
#define NO_CAN_GO_D -9

#define ENTER_MODE 0
#define EXIT_MODE  1

#define MODE_IN_MINI 0
#define MODE_OUT_MINI 1

#define TOROOM(x, y) (world[(x)].dir_option[(y)]->to_room)
#define CAN_GO2_THROUGH_DOORS(roomnum, door) (EXIT2(roomnum,door) && \
			 (EXIT2(roomnum,door)->to_room != NOWHERE) && \
			 !IS_SET(EXIT2(roomnum, door)->exit_info, EX_LOCKED))


struct data_go_mode {
	 int direzione;
	 long vnum_room;
  };

struct mini_enter_exit {
  room_vnum	exit[6][2];
  };

extern struct room_data *world;
void stop_go(struct char_data * ch);
void move_struct_go_mode(struct data_go_mode **dest, struct data_go_mode **source);
long get_mini_distanza(struct mini_enter_exit *mini,room_vnum vroom,int mode);
int find_first_step(room_rnum src, sh_int target);
int is_area_ok(struct char_data * ch, int dir, long target,long zone_mini);
int parse_dir_char(char dir);
int has_boat(struct char_data *ch);
int can_fly(struct char_data *ch);
struct mini_enter_exit * get_mini_enter_exit(room_vnum vnum);
char buffer[100];


int chk_circle_dir(char *path)
{
  int cont = 0;
  int dir[6] = {0,0,0,0,0,0};

  while (cont < strlen(path)) {
	  switch (parse_dir_char(path[cont])) {
	  case 0: dir[0]++; break;
	  case 1: dir[1]++; break;
	  case 2: dir[2]++; break;
	  case 3: dir[3]++; break;
	  case 4: dir[4]++; break;
	  case 5: dir[5]++; break;
	  }
	  cont++;
  }
  if ((dir[0]==dir[2]) && (dir[1]==dir[3]) && (dir[4]==dir[5])) return 0;
  else return -1;
}


int direzioneopposta(int dir)
{
  switch (dir) {
  case 0: return 2; break;
  case 1: return 3; break;
  case 2: return 0; break;
  case 3: return 1; break;
  case 4: return 5; break;
  case 5: return 4; break;
  default: return -1;
  }
}


/*Calcola la distanza tra due room nell wild*/
long distanza(long start_room, long target)
{
  struct coord chcoord; /* Coord of start */
  struct coord trcoord; /* Coord of target */

  if (start_room==-1 || target == -1) return 0;
  wild_coord_from_virtual(start_room, &chcoord);
  wild_coord_from_virtual(target, &trcoord);
  return (abs(trcoord.y-chcoord.y) + abs(trcoord.x-chcoord.x));
}

ACMD(do_mobgo)
{
 struct char_data *vict;
 struct char_data *vict_target=NULL;
 struct obj_data *obj;
 long target_room=0;
 //long r;
 char target[MAX_NAME_LENGTH];
 char vict_name[MAX_NAME_LENGTH];
 char sogg[MAX_NAME_LENGTH];
 //char command[MAX_NAME_LENGTH];
 int mode, dir;

 mode = SEARCH_MODE_NOTHING;

 argument = one_argument(argument, vict_name);
 argument = one_argument(argument, sogg);
 argument = one_argument(argument, target);

 if ((!vict_name || !sogg) ||
	 (((strcmp(sogg,"mob")==0) || (strcmp(sogg,"player")==0) ||
		(strcmp(sogg,"obj")==0) || (strcmp(sogg,"path")==0)) && (!target)))
 {
  if (subcmd==SCMD_PATROL)
	 send_to_char("comando: patrol <nome mob> <player/mob/obj/room/path> <nome pg/nome npg/nomeoggetto/numroom/percorso>.\r\n", ch);
  else
	 send_to_char("comando: go <nome mob> <player/mob/obj/room/path> <nome pg/nome npg/nomeoggetto/numroom/percorso> [<comando>].\r\n", ch);
  return;
 }

 if (!(vict = get_char_room_vis(ch, vict_name)))
 {
  send_to_char("A chi lo ordini?.\r\n", ch);
  return;
 }

 if (subcmd==SCMD_PATROL) {
	  // comando patrol
	 if (!((IS_NPC(ch)) ||                        //NPG
			 (GET_LEVEL(ch)>LVL_BUILDER) ||         //BUILDER
			 (MASTER_ID(vict) == GET_IDNUM(ch)) ||  // E' assegnato a te (pg)
			 ((GET_CLAN(vict)!=-1) && (CLAN_ID(vict) == GET_CLAN(ch)) &&
			  (!IS_SET(GET_CLAN_PRIV(ch),CP_MNGMOB) //assegnato al tuo clan e lo puoi comandare
			 )
		  )))

	 {
	  sprintf(buf,"%s ti ignora.",PERS(vict,ch));
	  send_to_char(buf, ch);
	  return;
	 }
  } else {
	 // comando go
	 if (!IS_NPC(ch) && (GET_LEVEL(ch)<LVL_BUILDER))
	 {
		sprintf(buf,"%s ti ignora.",PERS(vict,ch));
		send_to_char(buf, ch);
		return;
	 }
  }

 if (subcmd == SCMD_PATROL)
	CMD_MODE(vict) = CMD_MODE_PATROL;
 else
	CMD_MODE(vict) = CMD_MODE_GO ;


 if ((strcmp(sogg,"mob")==0) || (strcmp(sogg,"player")==0))
	if ((vict_target = get_char_vis(ch, target)))
	{
	 if (strcmp(sogg,"player")==0)
		 mode = SEARCH_MODE_PG;

	 if (strcmp(sogg,"mob")==0)
		 mode = SEARCH_MODE_MOB;

	 if (subcmd == SCMD_PATROL)
		sprintf(buf,"Ordini di fare la guardia a %s.\r\n", PERS(vict_target,ch));
	 else
		sprintf(buf,"Ordini di andare da %s.\r\n", PERS(vict_target,ch));

	 send_to_char(buf,ch);

	 if (!CAN_SEE(vict, vict_target))
	 {
	  sprintf(buf,"%s non riesce a vedere %s.\r\n", PERS(vict, ch) ,PERS(vict_target,ch));
	  send_to_char(buf,ch);
	  return;
	 }
	}

 if (strcmp(sogg,"obj")==0)
	 if ((obj = get_obj_vis(ch, target)))
	 {
	  mode = SEARCH_MODE_OBJ;
	  if (subcmd==SCMD_PATROL)
		  sprintf(buf,"Ordini di sorvegliare %s.\r\n", OBJS(obj, ch));
	  else
		  sprintf(buf,"Ordini di andare dove c'e' %s.\r\n", OBJS(obj, ch));

	  send_to_char(buf,ch);
	 }

 if (strcmp(sogg, "room")==0) {
	 if (!target) target_room = world[ch->in_room].number;
	 if (is_number(target) && (real_room(atoi(target)) != -1))
		 target_room = world[real_room(atoi(target))].number;

	 if (real_room(target_room) != -1)
	  {
		mode = SEARCH_MODE_ROOM;
		if (subcmd==SCMD_PATROL)
		  sprintf(buf,"Ordini di sorvegliare la room %d.\r\n",(int) target_room);
		else
		  sprintf(buf,"Ordini di andare nella room %d.\r\n",(int) target_room);

		send_to_char(buf,ch);
	  }
	 }

if (strcmp(sogg,"path")==0)
	{
	for (dir=0;dir <= (strlen(target) -1);dir++)
	 if (parse_dir_char(target[dir]) == -1)
	 {
		send_to_char("Percorso non corretto.\r\n",ch);
		return;
	 }

	if ((subcmd==SCMD_PATROL) && (chk_circle_dir(target)==-1))
	{
	 send_to_char("Il percorso deve essere un percorso chiuso! Es. nnsseeewwwnnss\r\n",ch);
		return;
	}

	mode = SEARCH_MODE_PATH;

	if (subcmd==SCMD_PATROL)
	  sprintf(buf,"Ordini di sorvegliare in direzione %s partendo da qui.\r\n", target);
	else
	  sprintf(buf,"Ordini di seguire la direzione %s partendo da qui.\r\n", target);

	send_to_char(buf,ch);
	}

	if ((argument) && (subcmd!=SCMD_PATROL))
	{
	  if (!SEARCH_COMMAND(vict))
		CREATE(SEARCH_COMMAND(vict),char, MAX_INPUT_LENGTH+1);
	  strcpy(SEARCH_COMMAND(vict), argument + 1 );
	}

  SEARCH_MODE(vict) = mode;
  switch (mode) {
  case SEARCH_MODE_PG:
	 GO_PG_TARGET(vict) = GET_IDNUM(vict_target);
	 break;
  case SEARCH_MODE_MOB:
	 if (!GO_NAME_TARGET(vict))
	  CREATE(GO_NAME_TARGET(vict), char,MAX_NAME_LENGTH + 1);
	 strcpy(GO_NAME_TARGET(vict), target);
	 break;
  case SEARCH_MODE_OBJ:
	 if (!GO_NAME_TARGET(vict))
		CREATE(GO_NAME_TARGET(vict), char,MAX_NAME_LENGTH + 1);
	 strcpy(GO_NAME_TARGET(vict), target);
	 break;
  case SEARCH_MODE_ROOM:
	 GO_ROOM_TARGET(vict) = target_room;
	 break;
  case SEARCH_MODE_PATH:
	 if (!GO_PATH(vict))
		CREATE(GO_PATH(vict), char,MAX_INPUT_LENGTH + 1);
	 strcpy(GO_PATH(vict), target);
	 POS_PATH(vict) = 0;
	 break;
  }
  return;
}


ACMD(do_gostop)
{
 char vict_name[MAX_NAME_LENGTH];
 struct char_data * vict;

 argument = one_argument(argument, vict_name);

 if (!(vict = get_char_room_vis(ch, vict_name)))
 {
  send_to_char("A chi lo ordini?.\r\n", ch);
  return;
 }

 if (subcmd==SCMD_PATROLSTOP) {
	  // comando patrol
	 if (!((IS_NPC(ch)) ||                        //NPG
			 (GET_LEVEL(ch)>LVL_BUILDER) ||         //BUILDER
			 (MASTER_ID(vict) == GET_IDNUM(ch)) ||  // E' assegnato a te (pg)
			 ((GET_CLAN(vict)!=-1) && (CLAN_ID(vict) == GET_CLAN(ch)) &&
			  (!IS_SET(GET_CLAN_PRIV(ch),CP_MNGMOB) //assegnato al tuo clan e lo puoi comandare
			 )
		  )))

	 {
	  sprintf(buf,"%s ti ignora.",PERS(vict,ch));
	  send_to_char(buf, ch);
	  return;
	 }
  } else {
	 // comando go
	 if (!IS_NPC(ch) && (GET_LEVEL(ch)<LVL_BUILDER))
	 {
		sprintf(buf,"%s ti ignora.",PERS(vict,ch));
		send_to_char(buf, ch);
		return;
	 }
  }

  stop_go(vict);
  sprintf(buf,"Okay, %s obbedisce e interrompe il suo compito.\r\n", PERS(vict, ch));
  send_to_char(buf, ch);
}

void stop_go(struct char_data * ch)
{
  if (SEARCH_COMMAND(ch) && (SEARCH_COMMAND(ch)!= ""))
	  strcpy(SEARCH_COMMAND(ch), "");
  SEARCH_MODE(ch) = SEARCH_MODE_NOTHING;
  GO_PG_TARGET(ch) = 0;
  if (GO_NAME_TARGET(ch) && (GO_NAME_TARGET(ch)!= ""))
	 strcpy(GO_NAME_TARGET(ch), "");
  GO_ROOM_TARGET(ch) = -1;
  if (GO_PATH(ch) && (GO_PATH(ch)!= ""))
	 strcpy(GO_PATH(ch), "");
  POS_PATH(ch) = 0;

  /*free GO info*/
  if (ch->mob_specials.go_nametarget){
		free(ch->mob_specials.go_nametarget);
		ch->mob_specials.go_nametarget = NULL;
		 }
  if (ch->mob_specials.go_path){
		free(ch->mob_specials.go_path);
		ch->mob_specials.go_path = NULL;
		 }
  if (ch->mob_specials.searchcommand){
		free(ch->mob_specials.searchcommand);
		ch->mob_specials.searchcommand = NULL;
		 }
}


struct mini_enter_exit *get_mini_enter_exit(room_vnum vnum)
{
  room_vnum temproom;
  int curr_dir,oppdir;
  struct mini_enter_exit *result = NULL;

  if ((real_room(vnum)!=-1) && (IS_MINIWILD(real_room(vnum))))
  {
	 CREATE(result, struct mini_enter_exit ,1);
	 for (curr_dir = 0; curr_dir < 4; curr_dir++) {
		oppdir = direzioneopposta(curr_dir);
		temproom = zone_table[world[real_room(vnum)].zone].miniwild_exit[curr_dir];
		if (real_room(temproom)!=-1){
			result->exit[curr_dir][0] = temproom;
			if ((world[real_room(temproom)].dir_option[oppdir]) && (world[real_room(temproom)].dir_option[oppdir]->to_room!=-1))
				result->exit[curr_dir][1] = world[world[real_room(temproom)].dir_option[oppdir]->to_room].number;
			else
				result->exit[curr_dir][1] = -1;
		} else {
			result->exit[curr_dir][0] = -1;
			result->exit[curr_dir][1] = -1;
			}
		}
	 return result;
  }
  return NULL;
}

long get_mini_distanza(struct mini_enter_exit *mini, room_vnum vroom, int mode)
{
  int curr_dir;
  long dist = -1;
  long tempdist = 0;

  if (real_room(vroom)!=-1)
  {
	  for (curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++) {
		if (mode==MODE_OUT_MINI) {
		  if (((tempdist = distanza(mini->exit[curr_dir][0],vroom)) < tempdist) || (dist==-1))
			 dist = tempdist;
		} else {
		  if (((tempdist = distanza(mini->exit[curr_dir][1],vroom)) < tempdist) || (dist==-1))
			 dist = tempdist;
		  }
	  }
	  return dist;
  }
  return 0;
}

struct data_go_mode *find_in_out_area(room_vnum in, room_vnum out, int mode)
{
  long real_in_room = real_room(in);
  long real_out_room = real_room(out);
  long in_zone,out_zone;
  struct data_go_mode *result = NULL;
  room_vnum current_room, nextroom;
  long distanza_curr_exit = 0;
  long disprec = -1;
  int curr_dir;

  if (real_in_room==-1) return NULL;

  CREATE(result,struct data_go_mode,1);

  in_zone = world[real_in_room].zone;
  out_zone = world[real_out_room].zone;

	  for (current_room = (zone_table[in_zone].number * 100); current_room <= zone_table[in_zone].top; current_room++)
		{
		 for (curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++)
		  {
		  if ((real_room(current_room)!=-1) && (CAN_GO2_THROUGH_DOORS(real_room(current_room),curr_dir)))
		  {
		  nextroom = world[real_room(current_room)].dir_option[curr_dir]->to_room;
		  if (IS_WILDERNESS(nextroom) || IS_MINIWILD(nextroom))
			 {
				if (IS_MINIWILD(nextroom) && (mode == EXIT_MODE))
						distanza_curr_exit = get_mini_distanza(get_mini_enter_exit(world[nextroom].number),
													world[nextroom].number, MODE_IN_MINI);
					 else
						distanza_curr_exit = distanza(out, world[nextroom].number);

				if ((distanza_curr_exit==0) || (disprec == -1) || (disprec > distanza_curr_exit)) {
				  disprec = distanza_curr_exit;
				  if ( mode == ENTER_MODE) {
					 result->vnum_room = world[nextroom].number;
					 result->direzione = direzioneopposta(curr_dir);
					 //sprintf(buffer,"GO - Enter mode - vnum->%d dir->%d dis->%d",result->vnum_room,result->direzione,distanza_curr_exit);
					 //mudlog(buffer, CMP, LVL_GOD, FALSE);
					 }
				  else {
					 result->vnum_room = current_room;
					 result->direzione = curr_dir;
					 //sprintf(buffer,"GO - Exit mode - vnum->%d dir->%d dis->%d",result->vnum_room,result->direzione,distanza_curr_exit);
					 //mudlog(buffer, CMP, LVL_GOD, FALSE);
					 }
				}
			 }
		  }
		 } //for
		}// for

  return result;
}


struct data_go_mode * find_in_out_mini(room_vnum in, room_vnum out, int mode)
{
  long real_in_room = real_room(in);
  long real_out_room = real_room(out);
  long in_zone,out_zone;
  struct data_go_mode *result = NULL;
  room_vnum current_room;// nextroom;
  long distanza_curr_exit = 0;
  long disprec = -1;
  int curr_dir=0;
  struct coord chcoord; /* Coord of character*/
  //struct coord trcoord;  Coord of target

  if (real_in_room==-1) return NULL;

  in_zone = world[real_in_room].zone;
  out_zone = world[real_out_room].zone;

  CREATE(result,struct data_go_mode,1);

	 if (mode == EXIT_MODE) {
	 for (current_room = (zone_table[in_zone].number * 100); current_room <= zone_table[in_zone].top; current_room++)
		{
		 if (real_room(current_room)!=-1) {
			wild_coord_from_virtual (current_room, &chcoord);
			if ((chcoord.y==MINIWILD_RYOR) || (chcoord.x==MINIWILD_RXEN) ||
				 (chcoord.y==MINIWILD_RYEN) || (chcoord.x==MINIWILD_RXOR))
			{
			  if (chcoord.y==MINIWILD_RYOR) curr_dir = 0;
			  if (chcoord.x==MINIWILD_RXEN) curr_dir = 1;
			  if (chcoord.y==MINIWILD_RYEN) curr_dir = 2;
			  if (chcoord.x==MINIWILD_RXOR) curr_dir = 3;

			  distanza_curr_exit = distanza(in, current_room);
			  //sprintf(buffer,"GO - MINI EXITMODE - vnum->%d dir->%d dis->%d",current_room,curr_dir,distanza_curr_exit);
			  //mudlog(buffer, CMP, LVL_GOD, FALSE);
			  if ((distanza_curr_exit==0) || (disprec == -1) || (disprec > distanza_curr_exit)) {
				  disprec = distanza_curr_exit;
				  result->vnum_room = current_room;
				  result->direzione = curr_dir;
				}
			}
		 } //real_room
		}//for
	 } //exit mode
	 else {
		for (curr_dir = 0; curr_dir < 4; curr_dir++)
		  if ((current_room = real_room(zone_table[in_zone].miniwild_exit[curr_dir]))!=-1){

			  distanza_curr_exit = distanza(out, current_room);
			  //sprintf(buffer,"GO - MINI ENTERMODE - vnum->%d dir->%d dis->%d",zone_table[in_zone].miniwild_exit[curr_dir]
			  //			,direzioneopposta(curr_dir),distanza_curr_exit);
			  //mudlog(buffer, CMP, LVL_GOD, FALSE);

			  if ((distanza_curr_exit==0) || (disprec == -1) || (disprec > distanza_curr_exit)) {
				  disprec = distanza_curr_exit;
				  result->vnum_room = zone_table[in_zone].miniwild_exit[curr_dir];
				  result->direzione = direzioneopposta(curr_dir);
			  }
			}
	 } // enter mode

 return result;
}


// Fai un passo per entrare/uscire nell'area o mini
int step_in_out(struct char_data *ch, struct data_go_mode * current_target, room_vnum real_target)
{

  if ((current_target) &&
		(current_target->vnum_room == world[ch->in_room].number) &&
		(real_target != world[ch->in_room].number))
  {
	 return current_target->direzione;
  }

  return -1;
}

// copia la stuttura e libera il sorgente
void move_struct_go_mode(struct data_go_mode **dest, struct data_go_mode **source)
{
  if (!*source) *dest = NULL;
  else {
	  if (!*dest)
		 CREATE(*dest,struct data_go_mode,1);
	  (*dest)->vnum_room = (*source)->vnum_room;
	  (*dest)->direzione = (*source)->direzione;
	  free((*source));
	  (*source)=NULL;
  }
}


struct data_go_mode * get_current_target(struct char_data * ch, room_vnum real_target,long *zone_mini)
{
  struct data_go_mode *current_target = NULL;
  struct data_go_mode *trgwild = NULL;
  struct data_go_mode *trgmini = NULL;
  long rroom;

  CREATE(trgwild,struct data_go_mode,1);
  trgwild->vnum_room = -1;
  trgwild->direzione = -1;

  if ((ch->in_room == NOWHERE) || (world[ch->in_room].number == NOWHERE)) return trgwild;
  if ((real_target == NOWHERE) || (real_room(real_target) == NOWHERE)) return trgwild;

  trgwild->vnum_room = real_target;

  CREATE(trgmini,struct data_go_mode,1);
  trgmini->vnum_room = -1;
  trgmini->direzione = -1;


  if (world[ch->in_room].zone != world[real_room(real_target)].zone) {

	  // Calcola prima le zone (area, mini e wild) per poter calcolare la distanza ed
	  // avere il riferimento della mini in cui nasce la citta' nella quale si trova il target.
	  rroom = real_room(real_target);

	  if (!IS_WILDERNESS(rroom) && !IS_MINIWILD(rroom)){
		 current_target = find_in_out_area(real_target, -1, ENTER_MODE);
		 if (current_target) {
			rroom = real_room(current_target->vnum_room);
			move_struct_go_mode(&trgwild,&current_target);
			}
		 }

	  if (!IS_WILDERNESS(rroom) && IS_MINIWILD(rroom)){
		 trgmini->vnum_room=trgwild->vnum_room;
		 trgmini->direzione=trgwild->direzione;
		 (*zone_mini) = world[real_room(trgmini->vnum_room)].zone;
		 current_target = find_in_out_mini(trgwild->vnum_room, trgmini->vnum_room, ENTER_MODE);
		 if (current_target)
			move_struct_go_mode(&trgwild,&current_target);
		 }

	  if ((!IS_WILDERNESS(ch->in_room)) &&
			((trgmini->vnum_room==-1) || (world[ch->in_room].zone != world[real_room(trgmini->vnum_room)].zone)) &&
			(world[ch->in_room].zone != world[real_room(real_target)].zone))
	  {

	  //(ESCE DALLA MINI / AREA)
	  rroom = ch->in_room;
	  // ch e' in una zona
	  if (!IS_WILDERNESS(rroom) && !IS_MINIWILD(rroom)){
			current_target = find_in_out_area(world[rroom].number, trgwild->vnum_room, EXIT_MODE);
		  move_struct_go_mode(&trgwild,&current_target);
		}
	  // ch e' in una mini
	  if (!IS_WILDERNESS(rroom) && IS_MINIWILD(rroom)){
			 current_target = find_in_out_mini(world[rroom].number, trgwild->vnum_room, EXIT_MODE);
		  move_struct_go_mode(&trgwild,&current_target);
		 }

	  } else {

		//(ENTRA NELLA MINI / AREA)
		rroom = real_room(real_target);
		//il target e' in una mini
		if (!IS_WILDERNESS(rroom) && IS_MINIWILD(rroom)){
		 current_target = find_in_out_mini(real_target, world[ch->in_room].number, ENTER_MODE);
		 move_struct_go_mode(&trgwild,&current_target);
		}

		//il target e' in un' area...
		if (!IS_WILDERNESS(rroom) && !IS_MINIWILD(rroom)){
		 current_target = find_in_out_area(real_target, world[ch->in_room].number, ENTER_MODE);
		 if (current_target) {
			rroom = real_room(current_target->vnum_room);
			move_struct_go_mode(&trgwild,&current_target);
			}
		 //...e ha anche la mini
		 if (IS_WILDERNESS(ch->in_room) && !IS_WILDERNESS(rroom) && IS_MINIWILD(rroom)){
		  current_target = find_in_out_mini(trgwild->vnum_room, world[ch->in_room].number, ENTER_MODE);
		  if (current_target)
			move_struct_go_mode(&trgwild,&current_target);
		 }
		}

	  }//else

	  move_struct_go_mode(&current_target, &trgwild);
  }//zone test
  else {
	  current_target = trgwild;
	  //mudlog("GO - Stessa zona.", CMP, LVL_GOD, FALSE);
		 }
  //sprintf(buffer,"GO - Mob->%ld Target->%ld dir->%ld",world[ch->in_room].number,current_target->vnum_room,current_target->direzione);
  //mudlog(buffer, CMP, LVL_GOD, FALSE);
  return current_target;
}

// leggi il target da cercare
long get_real_target(struct char_data * ch, long idvict, char *targetname,
					long goroom, char *gopath, int MODE)
{
  struct obj_data * obj;
  struct char_data * vict;
  long target_room=0;
  char *nome;
  //int direzione;

  switch (MODE) {
  case SEARCH_MODE_PG:
	 if (!(nome = get_name_by_id(idvict))) return MISSING_TARGET;
	 if (!(vict = get_char_vis(ch, nome ))) return MISSING_TARGET;
	 target_room = vict->in_room;
	 break;
  case SEARCH_MODE_MOB:
	 if (!(vict = get_char_vis(ch, targetname))) return MISSING_TARGET;
	 target_room = vict->in_room;
	 break;
  case SEARCH_MODE_OBJ:
	 if (!(obj = get_obj_vis(ch , targetname))) return MISSING_TARGET;
	 target_room = obj->in_room;
	 break;
  case SEARCH_MODE_ROOM:
	 if (goroom  == -1) return MISSING_TARGET;
	 target_room = real_room(goroom);
	 break;
  case SEARCH_MODE_PATH:
	 break;
  default:
	 return MISSING_TARGET;
  }
  return target_room;
}

// Ottiene la prossima direzione da seguire
int wild_area_mob_go(struct char_data * ch, long idvict, char *targetname,
					long goroom, char *gopath, int MODE)
{
  struct coord chcoord; /* Coord of character */
  struct coord trcoord; /* Coord of target */
  struct data_go_mode *current_targer = NULL;
  room_vnum target_room, real_target;
  int direzione;// curr_dir;
  long zone_mini = -1;

  if (MODE == SEARCH_MODE_PATH){
	 if (gopath  == "") return MISSING_TARGET;
	 if (POS_PATH(ch) == strlen(gopath)) return TARGET_FOUND;
	 direzione =  parse_dir_char(gopath[POS_PATH(ch)]);
	 POS_PATH(ch) += 1;
	 return direzione;
  }

  target_room = get_real_target(ch, idvict,targetname, goroom, gopath, MODE);

  if ((target_room == -1) || (target_room == MISSING_TARGET)) return MISSING_TARGET;

  real_target = world[target_room].number;

  if (real_target == -1) return MISSING_TARGET;

  current_targer = get_current_target(ch, real_target,&zone_mini);

  if ((!current_targer) || (current_targer->vnum_room==-1) ||
		(real_room(current_targer->vnum_room)==-1)|| (current_targer->vnum_room==0))
		  return MISSING_TARGET;

  //TROVATO!
  if (world[ch->in_room].number == real_target)
	 return TARGET_FOUND;

	//  PROSSIMA DIREZIONE
	if ((direzione = step_in_out(ch, current_targer, real_target)) >= 0)
	 return direzione;


	if (!IS_IN_WILDERNESS(ch) && !IS_IN_MINIWILD(ch))
	{
	  direzione = find_first_step(ch->in_room,real_room(current_targer->vnum_room));
	  if (direzione == BFS_NO_PATH)
	  {
		 act("$n sembra non orientarsi in questo posto.", FALSE, ch, 0, 0, TO_ROOM);
		 return MISSING_TARGET;
	  }  else return direzione;
	}
	 else
	{
	wild_coord_from_virtual(world[ch->in_room].number, &chcoord);
	wild_coord_from_virtual(current_targer->vnum_room, &trcoord);

	if (abs(chcoord.x-trcoord.x)>abs(chcoord.y-trcoord.y))
	  {
	  if (chcoord.x-trcoord.x > 0) direzione = 3;
	  else direzione = 1;
	  }
	else
	  {
	  if (chcoord.y-trcoord.y > 0) direzione = 0;
	  else direzione = 2;
	  }

	return is_area_ok(ch,direzione,real_target,zone_mini);
	}
}

int get_can_go_err(int dir)
{
  if (dir==0) return NO_CAN_GO_N;
  if (dir==1) return NO_CAN_GO_E;
  if (dir==2) return NO_CAN_GO_S;
  if (dir==3) return NO_CAN_GO_W;
  if (dir==4) return NO_CAN_GO_U;
  if (dir==5) return NO_CAN_GO_D;
  return -1;/*Blizzard*/
}

/*Controlla se l'area in cui sta entrando e' l'area dove c'e' il target*/
int is_area_ok(struct char_data * ch, int dir, long target,long zone_mini)
{
	// non e' l'area giusta
	if (CAN_GO2_THROUGH_DOORS(ch->in_room, dir)){
	 if (!IS_WILD(TOROOM(ch->in_room, dir)))
	 {
	  if (world[real_room(target)].zone != world[TOROOM(ch->in_room, dir)].zone)
		  return get_can_go_err(dir);
	 }	else {
	  if (wild_target_room(ch,dir)!=-1)
		 if ((world[real_room(target)].zone != world[TOROOM(ch->in_room, dir)].zone) &&
			  ((zone_mini==-1) ||
			  (world[real_room(target)].zone != world[real_room(zone_mini)].zone)))
			return get_can_go_err(dir);
	 }
	} 
	return dir;
}

void open_door_not_locked(struct char_data * ch, int dir)
{
	//apre le porte non locked
	if ((EXIT(ch, dir)) && (IS_SET(EXIT(ch, dir)->exit_info, EX_ISDOOR)) && (IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED))
		 && (!IS_SET(EXIT(ch, dir)->exit_info, EX_LOCKED)))
	  REMOVE_BIT(EXIT(ch, dir)->exit_info, EX_CLOSED);
}

/*Controlla se il mob si puo' muovere verso dir (apre le porte)*/
int mob_can_go(struct char_data * ch, int dir)
{
	room_rnum  target_room = -1;
	//struct obj_data *obj = NULL;
	//struct char_data *victim = NULL;

	open_door_not_locked(ch, dir);

	if (IS_IN_WILD(ch))
	  target_room = wild_target_room(ch, dir);
	else
	  if (EXIT(ch, dir) && EXIT(ch, dir)->to_room!=NOWHERE)
		 target_room = EXIT(ch, dir)->to_room;

	if (target_room < 0) return -1;

	if (((SECT(ch->in_room) == SECT_WATER_NOSWIM) ||
		(SECT(target_room) == SECT_WATER_NOSWIM)) && GET_LEVEL(ch)<LVL_IMMORT)
	 if ((RIDING(ch) && !MOB_FLAGGED(RIDING(ch), MOB_HARD_TAME) && !has_boat(RIDING(ch))) || (!has_boat(ch) && !can_fly(ch)))
		 return -1;

	return dir;
}

int correggidir(int dir)
{
  if (dir > 3) return 0;
  if (dir < 0) return 3;
  return dir;/*Blizzard*/
}

int intellimove(struct char_data * ch, int godir)
{
	int direction, piumeno, d;
	int missing = -1;

	if (godir<MISSING_TARGET)
	  {
		missing = godir;
		godir = (godir + 4)* -1;
	  }
	direction = godir;


	// se puo' mantiene la dir precedente
	if (ch->mob_specials.lastdir != -1)
	{
	  if ((mob_can_go(ch, ch->mob_specials.lastdir) > -1) &&
			((missing == -1) || (
			 (missing != -1) && (ch->mob_specials.lastdir != direction)
			)))
		  return ch->mob_specials.lastdir;
	}

	piumeno = 1;
	d = 0;
	if (ch->mob_specials.lastdir != -1)
	{
	 d = godir + (1 * piumeno);
	 d = correggidir(d);
	 if (ch->mob_specials.lastdir == direzioneopposta(d));
		piumeno = piumeno * (-1);
	}

		godir = direction;
		godir += (1 * piumeno);
		godir = correggidir(godir);
		if (mob_can_go(ch, godir) > -1) return godir;

		if (piumeno== -1) {
		godir = direction;
		godir += 2;
		godir = correggidir(godir);
		if (mob_can_go(ch, godir) > -1) return godir;
		}

		godir = direction;
		godir += (1 * piumeno * (-1));
		godir = correggidir(godir);
		if (mob_can_go(ch, godir) > -1) return godir;

		//up
		if (direction!=4) {
		godir = 4;
		if (mob_can_go(ch, godir) > -1) return godir;
		}

		//down
		if (direction!=5) {
		godir = 5;
		if (mob_can_go(ch, godir) > -1) return godir;
		}

		if (piumeno== 1) {
		godir = direction;
		godir += 2;
		godir = correggidir(godir);}

	return godir;
}



/*
int intellimove(struct char_data * ch, int godir)
{
	int direction, piumeno, d;

	direction = godir;
	//if (mob_can_go(ch, godir) > -1) return godir;

	if (ch->mob_specials.lastdir != -1)
	{
	  if ((mob_can_go(ch, ch->mob_specials.lastdir) > -1) &&
		  (godir != NO_CAN_GO))
		  return ch->mob_specials.lastdir;
	}


	piumeno = 1;
	d=0;
	if (ch->mob_specials.lastdir != -1)
	{
	 if ((d = godir + (1 * piumeno)) > 3) d = 0;
	 if (ch->mob_specials.lastdir == d);
		piumeno = piumeno * (-1);
	}

		godir = direction;
		if ((godir += (1 * piumeno)) > 3) godir = 0;
		if (mob_can_go(ch, godir) > -1) return godir;

		godir = direction;
		if ((godir += (1 * piumeno * (-1))) < 0) godir = 3;
		if (mob_can_go(ch, godir) > -1) return godir;

		godir = direction;
		if ((godir += 2) > 3) godir = 0;

	return godir;
}
*/

int perform_go(struct char_data * ch)
{
  int godir, okmove=0;

  if ((ch) && IS_NPC(ch) && SEARCH_MODE(ch)!=SEARCH_MODE_NOTHING)
  {

	godir = wild_area_mob_go(ch,GO_PG_TARGET(ch),GO_NAME_TARGET(ch),GO_ROOM_TARGET(ch),GO_PATH(ch),SEARCH_MODE(ch));

	if ((godir == TARGET_FOUND) && (CMD_MODE(ch) == CMD_MODE_PATROL))
	{
	  // se e' patrol rifa' il percorso da capo.
	  if (SEARCH_MODE(ch)==SEARCH_MODE_PATH) POS_PATH(ch) = 0;
	}

	if ((godir == TARGET_FOUND) && (CMD_MODE(ch) == CMD_MODE_GO) && (SEARCH_COMMAND(ch)) && (SEARCH_COMMAND(ch) != "")) {
	  command_interpreter(ch, SEARCH_COMMAND(ch));
	  strcpy(SEARCH_COMMAND(ch), "");
	 }

	if ((godir == TARGET_FOUND) && (MOB_FLAGGED(ch, MOB_SEARCHER)))
	  return 0;

	if (godir > NO_CAN_GO){
		// apre la porta se non e' bloccata.
		open_door_not_locked(ch, godir);
		okmove = perform_move(ch, godir, 1, CAN_SHOW_ROOM);
		}

	if (((okmove == 0) || (godir < MISSING_TARGET)) && (SEARCH_MODE(ch) != SEARCH_MODE_PATH))
	  {
		godir = intellimove(ch, godir);
		perform_move(ch, godir, 1, CAN_SHOW_ROOM);

	  }
  if (godir > NO_CAN_GO)
	ch->mob_specials.lastdir = godir;
  }
  return -1;
}


