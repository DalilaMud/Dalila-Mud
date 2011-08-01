 /* ************************************************************************
*   File: act.wizard.c                                  Part of CircleMUD *
*  Usage: Player-level god commands and other goodies                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <math.h>

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "house.h"
#include "screen.h"
#include "olc.h"
#include "dg_scripts.h"
#include "arena.h"
#include "wilderness.h"
#include "clan.h"
#include "shop.h"

/*   external vars  */
extern FILE *player_fl;
extern struct room_data *world;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct descriptor_data *descriptor_list;
extern struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct int_app_type int_app[];
extern struct wis_app_type wis_app[];
extern struct zone_data *zone_table;
extern int top_of_zone_table;
extern int restrict;
extern int top_of_world;
extern int top_of_mobt;
extern int top_of_objt;
extern int top_of_p_table;
/*Lance save mob*/
//extern struct mob_index_element *mobs_table = NULL;
/* index to mobs file	 */
//extern int top_of_m_table = 0;
/* ref to top of table		 */



/* for objects */
extern char *item_types[];
extern char *wear_bits[];
extern char *extra_bits[];
extern char *container_bits[];
extern char *drinks[];
extern char * tipi_materiale[];

/* for rooms */
extern char *dirs[];
extern char *room_bits[];
extern char *exit_bits[];
extern char *sector_types[];

/* for chars */
extern char *spells[];
extern char *proficienze[];
extern char *abilita[];
extern char *equipment_types[];
extern char *affected_bits[];
extern char *affected_bits_1[];
extern char *affected_bits_2[];
extern char *affected_bits_3[];
extern char *apply_types[];
extern char *pc_class_types[];
extern char *npc_class_types[];
extern char *action_bits[];
extern char *player_bits[];
extern char *preference_bits[];
extern char *position_types[];
extern char *connected_types[];
extern struct attack_hit_type attack_hit_text[];
extern struct player_index_element *player_table;
extern int objlevel (struct obj_data * obj);
extern char *how_good(int percent);
extern const char * nome_materiale (int tipo, int num);
extern void sedit_save_to_disk(int zone);
extern void generic_unstow(struct char_data *ch);
extern int get_stipendiomob(struct char_data *ch);
extern int find_name(char *name);
extern int _parse_name(char *arg, char *name);
extern int Valid_Name(char *newname);
extern int reserved_word(char *argument);
extern struct char_data *find_char(int n);	//Orione
extern int find_skill_num(char *name);


/* Nota: questo comando deve essere immediatamente seguito da un
   reboot o i casini potrebbero essere numerosi... */
ACMD(do_pulisci_pg)
{
	extern FILE *player_fl;
	struct char_file_u st;
	FILE *player_fl_nuovo;

	argument = one_argument (argument, buf);

	rewind(player_fl);
	if (!(player_fl_nuovo = fopen ("etc/new_players", "wb"))){
		mudlog ("SYSERR: impossibile creare un nuovo file di players.", CMP, LVL_QUEST_MASTER, TRUE);
		return;
	}

	while (!feof(player_fl)){
		fread (&st, sizeof(struct char_file_u), 1, player_fl);
		if (feof(player_fl))
			break;

		if (*st.name < 'A' || *st.name > 'Z'){
			sprintf (buf2, "Eliminato il player %s. Prima lettera sbagliata.", st.name);
			send_to_char (buf2, ch);
			continue;
		}
		if (LOWER(st.name[1]) < 'a' || LOWER(st.name[1]) > 'z'){
			sprintf (buf2, "Eliminato il player %s. Seconda lettera sbagliata.", st.name);
			send_to_char (buf2, ch);
			continue;
		}
		if (LOWER(st.name[2]) < 'a' || LOWER(st.name[2]) > 'z'){
			sprintf (buf2, "Eliminato il player %s. Terza lettera sbagliata.", st.name);
			send_to_char (buf2, ch);
			continue;
		}
		if (!strcmp (buf, "religioni"))
			st.player_specials_saved.culto = 0;

		fwrite (&st, sizeof (struct char_file_u), 1, player_fl_nuovo);
	}

	fclose (player_fl);
	fclose (player_fl_nuovo);
	remove (PLAYER_FILE);
	rename ("etc/new_players", PLAYER_FILE);
	if (!(player_fl = fopen (PLAYER_FILE, "r+b"))){
		mudlog ("SYSERR: impossibile riaprire il file players.", CMP, LVL_QUEST_MASTER, TRUE);
		return;
	}
	rewind (player_fl);
}

ACMD(do_ltitolo)
{
	extern struct nomi_titolanti *pg_titolanti;
	extern int num_titolanti;

	int i, t;
	struct descriptor_data *pt;
	struct char_data *titolante;

	one_argument (argument, arg);

	if (*arg){
		if (!(titolante = get_char_vis(ch, arg))){
			send_to_char(NOPERSON, ch);
			return;
		}

		for (i = 0; i < num_titolanti && strcmp(GET_NAME(titolante), pg_titolanti[i].nome); i++);
		if (i == num_titolanti){
			send_to_char ("Ma quella persona non ha fatto richiesta del titolo!\r\n", ch);
			return;
		}

		SET_BIT(PLR_FLAGS(titolante), PLR_GO_GILDA);
		for (; i < num_titolanti-1; i++)
			pg_titolanti[i] = pg_titolanti[i+1];
		num_titolanti--;
		pg_titolanti = realloc (pg_titolanti, sizeof(struct nomi_titolanti) * num_titolanti);

		send_to_char ("Okay.\r\n", ch);
		return;
	}

	send_to_char ("Pg attualmente in lista per il titolo:\r\n", ch);
	send_to_char ("Nome                 Titolo   Classe     Giorni dalla richiesta   Online\r\n", ch);

	for (i = 0, t = 0; i < num_titolanti; i++){
		sprinttype(pg_titolanti[i].class, pc_class_types, buf2);
		for (pt = descriptor_list; pt; pt = pt->next)
			if (!pt->connected && pt->character)
				if (!strcmp (GET_NAME(pt->character), pg_titolanti[i].nome))
					t++;

		sprintf (buf, "%-23s %d     %-11s           %-15ld %s\r\n", pg_titolanti[i].nome, pg_titolanti[i].titolo, buf2, (time(NULL) - pg_titolanti[i].richiesta) / SECS_PER_REAL_DAY, t ? "si'" : "no");
		send_to_char (buf, ch);
	}

	send_to_char ("Usa ltitolo <nomepg> per rimuovere un pg dalla lista (quest compiuta o storia letta e approvata).\r\n", ch);
}

ACMD(do_ammala)
{
	struct char_data *victim;
	int spellnum;
	
	half_chop(argument, buf, buf2);

	if (! (*buf && *buf2)){
		sprintf (buf,	"&6&bUtilizzo di ammala:&0\r\n"
				"ammala nomepg nomemalattia\r\n"
				"Senza inserire la malattia tra apici.\r\n\r\n"
				"&6&bLe malattie attualmente disponibili sono:&0\r\n"
				"Raffreddore\r\n"
				"Peste\r\n"
				"Malattia di Bacco\r\n"
				"Maledizione di Shaarr\r\n"
				"Piaghe alle mani\r\n"
				"Male degl Antichi\r\n"
				"Malattia di Morfeo\r\n");
		send_to_char (buf, ch);
		return;
	}
	if (!(victim = get_char_vis(ch, buf))){
		send_to_char(NOPERSON, ch);
		return;
	}
	if (!*buf2){
		send_to_char("Dovresti anche specificare una malattia...\r\n", ch);
		return;
	}
	spellnum = find_skill_num (buf2);
	if (spellnum <= MAX_SKILLS || spellnum > TOP_SPELL_DEFINE){
		send_to_char ("Che ne dici di mettere il nome di una malattia?\r\n", ch);
		return;
	}

	if (affected_by_spell(victim, SPELLSKILL, spellnum))
		affect_from_char(victim, SPELLSKILL, spellnum);
	else{
		if (GET_LEVEL(victim) > GET_LEVEL(ch)){
			send_to_char ("Non penso che ne sarebbe contento...\r\n", ch);
			return;
		}
		mag_affects(GET_LEVEL(ch), ch, victim, spellnum, 0);
	}

	send_to_char ("Okay.\r\n", ch);
}


ACMD(do_stutto)
{
	void oedit_save_to_disk(int zone_num);
	FILE *index;
	int zn = 0;
	char stringa[100];
	
	sprintf (stringa, "%s/index", OBJ_PREFIX);

	if (!(index = fopen (stringa, "r"))){
		send_to_char ("Errore! File index non trovato.\n", ch);
		return;
	}
	for (;;){
		fscanf (index, "%s\n", stringa);
		if (*stringa == '~')
			break;
		sscanf (stringa, "%d.obj\n", &zn);
		oedit_save_to_disk(zn);
	}
	send_to_char ("Okay.\r\n", ch);
}

ACMD(do_echo)
{
	skip_spaces(&argument);
	
	if (!*argument)
		send_to_char("Yes.. but what?\r\n", ch);
	else {
		if (subcmd == SCMD_EMOTE)
			sprintf(buf, "$n %s", argument);
		else
			strcpy(buf, argument);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else
			act(buf, FALSE, ch, 0, 0, TO_CHAR);
	}
}


ACMD(do_tele)
{
	struct char_data *vict;
	
	half_chop(argument, arg, buf);
	
	if (!*arg) {
		send_to_char("Message what to who?\r\n", ch);
		return;
	}
	if (!(vict = get_char_vis(ch, arg))) {
		send_to_char(NOPERSON, ch);
		return;
	}
	send_to_char(buf, vict);
	send_to_char("\r\n", vict);
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char("Sent.\r\n", ch);
	else {
		sprintf(buf2, "You message '%s' to %s.\r\n", buf, GET_NAME(vict));
		send_to_char(buf2, ch);
	}
}



/* take a string, and return an rnum.. used for goto, at, etc.  -je 4/6/93 */
room_rnum find_target_room(struct char_data * ch, char *rawroomstr)
{
	int tmp;
	room_rnum location;
	struct char_data *target_mob;
	struct obj_data *target_obj;
	char roomstr[MAX_INPUT_LENGTH];
	
	one_argument(rawroomstr, roomstr);
	
	if (!*roomstr) {
		send_to_char("You must supply a room number or name.\r\n", ch);
		return NOWHERE;
	}
	if (isdigit(*roomstr) && !strchr(roomstr, '.')) {
		tmp = atoi(roomstr);
		if ((location = real_room(tmp)) < 0) {
			send_to_char("No room exists with that number.\r\n", ch);
			return NOWHERE;
		}
	} else if ((target_mob = get_char_vis(ch, roomstr)))
		location = target_mob->in_room;
	else if ((target_obj = get_obj_vis(ch, roomstr))) {
		if (target_obj->in_room != NOWHERE)
			location = target_obj->in_room;
		else {
			send_to_char("That object is not available.\r\n", ch);
			return NOWHERE;
		}
	} else {
		send_to_char("No such creature or object around.\r\n", ch);
		return NOWHERE;
	}
	
  /* a location has been found -- if you're < GRGOD, check restrictions. */
	if (GET_LEVEL(ch) < LVL_GRGOD) {
		if (ROOM_FLAGGED(location, ROOM_GODROOM)) {
			send_to_char("You are not godly enough to use that room!\r\n", ch);
			return NOWHERE;
		}
		if (ROOM_FLAGGED(location, ROOM_PRIVATE) &&
			world[location].people && world[location].people->next_in_room) {
			send_to_char("There's a private conversation going on in that room.\r\n", ch);
			return NOWHERE;
		}
		if (ROOM_FLAGGED(location, ROOM_HOUSE) &&
			!House_can_enter(ch, world[location].number)) {
			send_to_char("That's private property -- no trespassing!\r\n", ch);
			return NOWHERE;
		}
	}
	return location;
}



ACMD(do_at)
{
	char command[MAX_INPUT_LENGTH];
	int location, original_loc;
	
	half_chop(argument, buf, command);
	if (!*buf) {
		send_to_char("You must supply a room number or a name.\r\n", ch);
		return;
	}
	
	if (!*command) {
		send_to_char("What do you want to do there?\r\n", ch);
		return;
	}
	
	if ((location = find_target_room(ch, buf)) < 0)
		return;
	
  /* a location has been found. */
	original_loc = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, location);
	command_interpreter(ch, command);
	
  /* check if the char is still there */
	if (ch->in_room == location) {
		char_from_room(ch);
		char_to_room(ch, original_loc);
	}
}


ACMD(do_goto)
{
	room_rnum location;
	
	if ((location = find_target_room(ch, argument)) < 0)
		return;
	
	if (POOFOUT(ch))
		sprintf(buf, POOFOUT(ch));
	else
		strcpy(buf, "In un attimo $n scompare in una grande nuvola di fumo.");
	
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);
	
  /*Adriano*/
	if RIDING(ch)
	{
		char_from_room(RIDING(ch));
		char_to_room(RIDING(ch), location);
	}
	if TRANSPORTED(ch)
	{
		char_from_room(TRANSPORTED(ch));
		char_to_room(TRANSPORTED(ch), location);
	}
	
  /*Adriano*/
	
	if (POOFIN(ch))
		sprintf(buf, POOFIN(ch));
	else
		strcpy(buf, "Con un tremendo boato $n compare davanti a te.");
	
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	look_at_room(ch, 0);
}



ACMD(do_trans)
{
	struct descriptor_data *i;
	struct char_data *victim;
	
	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Chi vuoi trasferire ?\r\n", ch);
	else if (str_cmp("all", buf)) {
		if (!(victim = get_char_vis(ch, buf)))
			send_to_char(NOPERSON, ch);
		else if (victim == ch)
			send_to_char("Non ha molto senso, vero ?\r\n", ch);
		else {
			if ((GET_LEVEL(ch) < GET_LEVEL(victim)) && !IS_NPC(victim)) {
				send_to_char("Prova a trasferire qualcuno della tua taglia.\r\n", ch);
				return;
			}
			act("$n scompare in una grande nuvola di fumo.", TRUE, victim, 0, 0, TO_ROOM);
			char_from_room(victim);
			char_to_room(victim, ch->in_room);
      /*Adriano*/
			generic_unstow(ch);
			if RIDING(victim)
			{
				char_from_room(RIDING(victim));
				char_to_room(RIDING(victim), ch->in_room);
			}
			if TRANSPORTED(victim)
			{
				char_from_room(TRANSPORTED(victim));
				char_to_room(TRANSPORTED(victim), ch->in_room);
			}	
      /*Adriano*/
			act("$n arriva da una grande nuvola di fumo.", TRUE, victim, 0, 0, TO_ROOM);
			act("$n ti ha trasferito!", FALSE, ch, 0, victim, TO_VICT);
			look_at_room(victim, 0);
			if (IS_TRUST(ch)) {
				sprintf(buf, "(TRUST) %s transfer %s to room %s(#%d).", GET_NAME(ch),GET_NAME(victim),world[ch->in_room].name,world[ch->in_room].number);
				mudlog(buf, CMP, LVL_GOD, TRUE);
			}
		}
	} else {			/* Trans All */
		if (GET_LEVEL(ch) < LVL_GRGOD) {
			send_to_char("Penso di no.\r\n", ch);
			return;
		}
		
		for (i = descriptor_list; i; i = i->next)
			if (!i->connected && i->character && i->character != ch) {
				victim = i->character;
				if (GET_LEVEL(victim) >= GET_LEVEL(ch))
					continue;
				act("$n scompare in una grande nuvola di fumo.", FALSE, victim, 0, 0, TO_ROOM);
				char_from_room(victim);
				char_to_room(victim, ch->in_room);
				act("$n arriva da una grande nuvola di fumo.", FALSE, victim, 0, 0, TO_ROOM);
				act("$n ti ha trasferito!", FALSE, ch, 0, victim, TO_VICT);
				look_at_room(victim, 0);
				if (IS_TRUST(ch)) {
					sprintf(buf, "(TRUST) %s transfer %s to room %s(#%d).", GET_NAME(ch),GET_NAME(victim),world[ch->in_room].name,world[ch->in_room].number);
					mudlog(buf, CMP, LVL_GOD, TRUE);
				}
			}
		send_to_char(OK, ch);
		
		
	}
}

ACMD(do_teleport)
{
	struct char_data *victim;
	room_rnum target;
	
	two_arguments(argument, buf, buf2);
	
	if (!*buf)
		send_to_char("Chi desideri teletrasportare ?\r\n", ch);
	else if (!(victim = get_char_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else if (victim == ch)
		send_to_char("Usa 'goto' per teletrasportarti.\r\n", ch);
	else if (GET_LEVEL(victim) >= GET_LEVEL(ch))
		send_to_char("Forse non lo dovresti fare.\r\n", ch);
	else if (!*buf2)
		send_to_char("Dove vuoi mandare questa persona ?\r\n", ch);
	else if ((target = find_target_room(ch, buf2)) >= 0) {
		send_to_char(OK, ch);
		act("$n scompare in una nuvola di fumo.", FALSE, victim, 0, 0, TO_ROOM);
		char_from_room(victim);
		char_to_room(victim, target);
		generic_unstow(ch);
		if RIDING(victim)
		{
			char_from_room(RIDING(victim));
			char_to_room(RIDING(victim), target);
		}
		if TRANSPORTED(victim)
		{
			char_from_room(TRANSPORTED(victim));
			char_to_room(TRANSPORTED(victim), target);
		}  
		act("$n arriva da una nuvola di fumo.", FALSE, victim, 0, 0, TO_ROOM);
		act("$n ti ha teletrasportato!", FALSE, ch, 0, (char *) victim, TO_VICT);
		look_at_room(victim, 0);
	}
}



ACMD(do_vnum)
{
	two_arguments(argument, buf, buf2);
	
	if (!*buf || !*buf2 || (!is_abbrev(buf, "mob") && !is_abbrev(buf, "obj"))) {
		send_to_char("Usage: vnum { obj | mob } <name>\r\n", ch);
		return;
	}
	if (is_abbrev(buf, "mob"))
		if (!vnum_mobile(buf2, ch))
			send_to_char("No mobiles by that name.\r\n", ch);
		
	if (is_abbrev(buf, "obj"))
		if (!vnum_object(buf2, ch))
			send_to_char("No objects by that name.\r\n", ch);
}



void do_stat_room(struct char_data * ch)
{
	int is_tempestosa (room_rnum rm);
	struct extra_descr_data *desc;
	struct room_data *rm = &world[ch->in_room];
	int i, found = 0;
	struct obj_data *j = 0;
	struct char_data *k = 0;
	
	sprintf(buf, "Room name: %s%s%s\r\n", CCCYN(ch, C_NRM), rm->name,
		CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
	
	sprinttype(rm->sector_type, sector_types, buf2);
	sprintf(buf, "Zone: [%3d], VNum: [%s%5d%s], RNum: [%5d], Type: %s\r\n",
		rm->zone, CCGRN(ch, C_NRM), rm->number, CCNRM(ch, C_NRM), ch->in_room, buf2);
	send_to_char(buf, ch);
	
	sprintbit((long long int) rm->room_flags, room_bits, buf2);
	sprintf(buf, "SpecProc: %s, Flags: %s\r\n",
		(rm->func == NULL) ? "None" : "Exists", buf2);
	send_to_char(buf, ch);
	
	send_to_char("Description:\r\n", ch);
	if (rm->description)
		send_to_char(rm->description, ch);
	else
		send_to_char("  None.\r\n", ch);
	
	if (rm->ex_description) {
		sprintf(buf, "Extra descs:%s", CCCYN(ch, C_NRM));
		for (desc = rm->ex_description; desc; desc = desc->next) {
			strcat(buf, " ");
			strcat(buf, desc->keyword);
		}
		strcat(buf, CCNRM(ch, C_NRM));
		send_to_char(strcat(buf, "\r\n"), ch);
	}
	sprintf(buf, "Chars present:%s", CCYEL(ch, C_NRM));
	for (found = 0, k = rm->people; k; k = k->next_in_room) {
		if (!CAN_SEE(ch, k))
			continue;
		sprintf(buf2, "%s %s(%s)", found++ ? "," : "", GET_NAME(k),
			(!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")));
		strcat(buf, buf2);
		if (strlen(buf) >= 62) {
			if (k->next_in_room)
				send_to_char(strcat(buf, ",\r\n"), ch);
			else
				send_to_char(strcat(buf, "\r\n"), ch);
			*buf = found = 0;
		}
	}
	
	if (*buf)
		send_to_char(strcat(buf, "\r\n"), ch);
	send_to_char(CCNRM(ch, C_NRM), ch);
	
	if (rm->contents) {
		sprintf(buf, "Contents:%s", CCGRN(ch, C_NRM));
		for (found = 0, j = rm->contents; j; j = j->next_content) {
			if (!CAN_SEE_OBJ(ch, j))
				continue;
			sprintf(buf2, "%s %s", found++ ? "," : "", j->short_description);
			strcat(buf, buf2);
			if (strlen(buf) >= 62) {
				if (j->next_content)
					send_to_char(strcat(buf, ",\r\n"), ch);
				else
					send_to_char(strcat(buf, "\r\n"), ch);
				*buf = found = 0;
			}
		}
		
		if (*buf)
			send_to_char(strcat(buf, "\r\n"), ch);
		send_to_char(CCNRM(ch, C_NRM), ch);
	}
	for (i = 0; i < NUM_OF_DIRS; i++) {
		if (rm->dir_option[i]) {
			if (rm->dir_option[i]->to_room == NOWHERE)
				sprintf(buf1, " %sNONE%s", CCCYN(ch, C_NRM), CCNRM(ch, C_NRM));
			else
				sprintf(buf1, "%s%5d%s", CCCYN(ch, C_NRM),
					world[rm->dir_option[i]->to_room].number, CCNRM(ch, C_NRM));
			sprintbit(rm->dir_option[i]->exit_info, exit_bits, buf2);
			sprintf(buf, "Exit %s%-5s%s:  To: [%s], Key: [%5d], Keywrd: %s, Type: %s\r\n ",
				CCCYN(ch, C_NRM), dirs[i], CCNRM(ch, C_NRM), buf1, rm->dir_option[i]->key,
				rm->dir_option[i]->keyword ? rm->dir_option[i]->keyword : "None",
				buf2);
			send_to_char(buf, ch);
			if (rm->dir_option[i]->general_description)
				strcpy(buf, rm->dir_option[i]->general_description);
			else
				strcpy(buf, "  No exit description.\r\n");
			send_to_char(buf, ch);
		}
	}
	
  /* check the room for a script */
	do_sstat_room(ch);
	
	if (IS_IN_WILD(ch))
	{
		sprintf(buf, "Wilderness Type: [%s%d%s] [%s%c%s] [%s%s%s]\r\n",
			CCYEL(ch,C_NRM), rm->wild_rnum != -1 ?
			wild_table[rm->wild_rnum].index : -1, CCNRM(ch,C_NRM),
			rm->wild_rnum != -1 ? color_from_color(ch, wild_table[rm->wild_rnum].color) : CCNRM(ch,C_NRM),
			rm->wild_rnum != -1 ? wild_table[rm->wild_rnum].symbol : ' ',
			CCNRM(ch,C_NRM), CCYEL(ch,C_NRM), rm->wild_rnum != -1 ?
			wild_table[rm->wild_rnum].name : "NO WILDERNESS", CCNRM(ch,C_NRM));
		sprintf(buf+strlen(buf), "Wilderness Unique Room: %s%s%s\r\n", CCYEL(ch,C_NRM),
			rm->wild_modif ? "TRUE" : "FALSE",CCNRM(ch,C_NRM));
		if (is_tempestosa(ch->in_room))
			sprintf(buf+strlen(buf), "Qui e' in corso una tempesta!\r\n");
		send_to_char(buf, ch);
	}
}

void do_stat_wild(struct char_data * ch, struct wild_data *w)
{
	sprintf(buf, "Wild Type : [%s%c%s] - %s%s%s\r\n",
		color_from_color(ch,w->color), w->symbol, CCNRM(ch,C_NRM),
		CCCYN(ch, C_NRM), w->name, CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
	
	sprinttype(w->sector_type, sector_types, buf2);
	sprintf(buf, "VNum: [%s%5d%s], RNum: [%5d], Type: %s\r\n",
		CCGRN(ch, C_NRM), w->index, CCNRM(ch, C_NRM), real_wild_table(w->index), buf2);
	send_to_char(buf, ch);
	
	sprintf(buf, "Altitudine: %d   Move Cost: %d   Can Enter: %s\r\n", w->altitudine,
		w->move_cost, w->can_enter ? "TRUE" : "FALSE");
	send_to_char(buf, ch);
	
	sprintf(buf, "Owner: %d (not yet implemented)\r\n", w->owner);
	send_to_char(buf, ch);
	
	sprintbit((long long int) w->room_flags, room_bits, buf2);
	sprintf(buf, "Flags: %s\r\n", buf2);
	send_to_char(buf, ch);
	
	send_to_char("Description:\r\n", ch);
	if (w->description)
		send_to_char(w->description, ch);
	else
		send_to_char("  None.\r\n", ch);
}

void do_stat_object(struct char_data * ch, struct obj_data * j)
{
	int i, virtual, found;
	struct obj_data *j2;
	struct extra_descr_data *desc;
	
	virtual = GET_OBJ_VNUM(j);
	sprintf(buf, "Name: '%s%s%s', Aliases: %s\r\n", CCYEL(ch, C_NRM),
		((j->short_description) ? j->short_description : "<None>"),
		CCNRM(ch, C_NRM), j->name);
	send_to_char(buf, ch);
	sprinttype(GET_OBJ_TYPE(j), item_types, buf1);
	if (GET_OBJ_RNUM(j) >= 0)
		strcpy(buf2, (obj_index[GET_OBJ_RNUM(j)].func ? "Exists" : "None"));
	else
		strcpy(buf2, "None");
	sprintf(buf, "VNum: [%s%5d%s], RNum: [%5d], Type: %s, SpecProc: %s\r\n",
		CCGRN(ch, C_NRM), virtual, CCNRM(ch, C_NRM), GET_OBJ_RNUM(j), buf1, buf2);
	send_to_char(buf, ch);
	sprintf(buf, "L-Des: %s\r\n", ((j->description) ? j->description : "None"));
	send_to_char(buf, ch);
	
	if (j->ex_description) {
		sprintf(buf, "Extra descs:%s", CCCYN(ch, C_NRM));
		for (desc = j->ex_description; desc; desc = desc->next) {
			strcat(buf, " ");
			strcat(buf, desc->keyword);
		}
		strcat(buf, CCNRM(ch, C_NRM));
		send_to_char(strcat(buf, "\r\n"), ch);
	}
	
	sprintf(buf, "Livello minimo di utilizzo : %4d  --  Livello minimo consigliato: %4d\r\n",
		GET_OBJ_LEVEL(j), objlevel(j));
	send_to_char(buf,ch);
	
	sprintf(buf, "Tipo Materiale : %-15s Materiale : %-20s\r\n",
		tipi_materiale[GET_OBJ_MAT_TYPE(j)],
		nome_materiale(GET_OBJ_MAT_TYPE(j), GET_OBJ_MAT_NUM(j)));
	send_to_char(buf, ch);
	
	send_to_char("Can be worn on: ", ch);
	sprintbit(j->obj_flags.wear_flags, wear_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	
	send_to_char("Set char bits : ", ch);
	sprintbit(j->obj_flags.bitvector[0], affected_bits, buf);
	sprintbit(j->obj_flags.bitvector[1], affected_bits_1, buf1);
	if (strcmp(buf1, "NOBITS ")){
		if (strcmp(buf, "NOBITS "))
			strcat(buf, buf1);
		else
			sprintf (buf, "%s", buf1);
	}
	sprintbit(j->obj_flags.bitvector[2], affected_bits_2, buf1);
	if (strcmp(buf1, "NOBITS ")){
		if (strcmp(buf, "NOBITS "))
			strcat(buf, buf1);
		else
			sprintf (buf, "%s", buf1);
	}
	sprintbit(j->obj_flags.bitvector[3], affected_bits_3, buf1);
	if (strcmp(buf1, "NOBITS ")){
		if (strcmp(buf, "NOBITS "))
			strcat(buf, buf1);
		else
			sprintf (buf, "%s", buf1);
	}
	
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	
	send_to_char("Extra flags   : ", ch);
	sprintbit(GET_OBJ_EXTRA(j), extra_bits, buf);
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	
	sprintf(buf, "Weight: %d, Value: %d, Cost/day: %d, Timer: %d\r\n",
		GET_OBJ_WEIGHT(j), GET_OBJ_COST(j), GET_OBJ_RENT(j), GET_OBJ_TIMER(j));
	send_to_char(buf, ch);
	
	sprintf(buf, "Condizione [CSLOTS/TSLOTS]: [%d/%d]\r\n", GET_OBJ_CSLOTS(j), GET_OBJ_TSLOTS(j));
	send_to_char(buf, ch);
	
	strcpy(buf, "In room: ");
	if (j->in_room == NOWHERE)
		strcat(buf, "Nowhere");
	else {
		sprintf(buf2, "%d", world[j->in_room].number);
		strcat(buf, buf2);
	}
	strcat(buf, ", In object: ");
	strcat(buf, j->in_obj ? j->in_obj->short_description : "None");
	strcat(buf, ", Carried by: ");
	strcat(buf, j->carried_by ? GET_NAME(j->carried_by) : "Nobody");
	strcat(buf, ", Worn by: ");
	strcat(buf, j->worn_by ? GET_NAME(j->worn_by) : "Nobody");
	strcat(buf, "\r\n");
	send_to_char(buf, ch);
	
	switch (GET_OBJ_TYPE(j)) {
		case ITEM_LIGHT:
			if (GET_OBJ_VAL(j, 2) == -1)
				strcpy(buf, "Hours left: Infinite");
			else
				sprintf(buf, "Hours left: [%d]", GET_OBJ_VAL(j, 2));
			break;
		case ITEM_SCROLL:
		case ITEM_POTION:
			sprintf(buf, "Spells: (Level %d) %s, %s, %s", GET_OBJ_VAL(j, 0),
				skill_name(GET_OBJ_VAL(j, 1)), skill_name(GET_OBJ_VAL(j, 2)),
				skill_name(GET_OBJ_VAL(j, 3)));
			break;
		case ITEM_WAND:
		case ITEM_STAFF:
			sprintf(buf, "Spell: %s at level %d, %d (of %d) charges remaining",
				skill_name(GET_OBJ_VAL(j, 3)), GET_OBJ_VAL(j, 0),
				GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 1));
			break;
		case ITEM_THROW:
		case ITEM_ROCK:
		case ITEM_BOLT:
		case ITEM_ARROW:
			sprintf(buf, "Numbero di dadi: %d Danno per dado: %d",
				GET_OBJ_VAL(j, 1), GET_OBJ_VAL(j, 2));
			break;
		case ITEM_GRENADE:
			sprintf(buf, "Timer: %d Num dam dice: %d Size dam dice: %d",
				GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1), GET_OBJ_VAL(j, 2));
			break;
		case ITEM_BOW:
		case ITEM_CROSSBOW:
		case ITEM_SLING:
			sprintf(buf, "Raggio: %d", GET_OBJ_VAL(j, 0));
			break;
		case ITEM_WEAPON:
			sprintf(buf, "Todam: %dd%d, Message type: %d,Arma di tipo:%s",
				GET_OBJ_VAL(j, 1), GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 3)
				,attack_hit_text[GET_OBJ_VAL(j, 3)].weapon);
			break;
		case ITEM_ARMOR:
			sprintf(buf, "AC-apply: [%d]", GET_OBJ_VAL(j, 0));
			break;
		case ITEM_TRAP:
			sprintf(buf, "Spell: %d, - Hitpoints: %d",
				GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1));
			break;
		case ITEM_CONTAINER:
			sprintbit(GET_OBJ_VAL(j, 1), container_bits, buf2);
			sprintf(buf, "Weight capacity: %d, Lock Type: %s, Key Num: %d, Corpse: %s",
				GET_OBJ_VAL(j, 0), buf2, GET_OBJ_VAL(j, 2),
				YESNO(GET_OBJ_VAL(j, 3)));
			if (IS_OBJ_STAT (j, ITEM_ISCORPSE))
				sprintf (buf, "%s\nFile relativo al cadavere: %d.crps", buf, GET_OBJ_VAL (j, 4));
			break;
		case ITEM_DRINKCON:
		case ITEM_FOUNTAIN:
			sprinttype(GET_OBJ_VAL(j, 2), drinks, buf2);
			sprintf(buf, "Capacity: %d, Contains: %d, Poisoned: %s, Liquid: %s",
				GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1), YESNO(GET_OBJ_VAL(j, 3)),
				buf2);
			break;
		case ITEM_NOTE:
			sprintf(buf, "Tongue: %d", GET_OBJ_VAL(j, 0));
			break;
		case ITEM_KEY:
			strcpy(buf, "");
			break;
		case ITEM_FOOD:
			sprintf(buf, "Makes full: %d, Poisoned: %s", GET_OBJ_VAL(j, 0),
				YESNO(GET_OBJ_VAL(j, 3)));
			break;
		case ITEM_MONEY:
			sprintf(buf, "Coins: %d", GET_OBJ_VAL(j, 0));
			break;
		case ITEM_TRAPPOLA:			
			switch (GET_TRAP_TIPO(j)) {
				case TRAP_VUOTA:
					sprintf(buf,"Tipo Trappola: VUOTA");
					break;
				case TRAP_SCATTATA:
					sprintf(buf,"Tipo Trappola: SCATTATA");
					break;
				case TRAP_DANNO:
					sprintf(buf,"Tipo Trappola: DANNO, %% Attivazione: %d, %% Missing: %d, Danno: %d-%d",GET_TRAP_PERC(j),GET_TRAP_MISS(j),GET_TRAP_MIND(j),GET_TRAP_MAXD(j));
					break;
				case TRAP_ALLARME:
					sprintf(buf,"Tipo Trappola: ALLARME, %% Attivazione: %d, %% Missing: %d",GET_TRAP_PERC(j),GET_TRAP_MISS(j));
					break;
				case TRAP_BASH:
					sprintf(buf,"Tipo Trappola: BASH, %% Attivazione: %d, %% Missing: %d",GET_TRAP_PERC(j),GET_TRAP_MISS(j)); 
					break;
				case TRAP_IMMOBILIZZA:
					sprintf(buf,"Tipo Trappola: STUN, %% Attivazione: %d, %% Missing: %d",GET_TRAP_PERC(j),GET_TRAP_MISS(j));
					break;
			}
			break;
		case ITEM_LIBRO_MAGICO_W:
		case ITEM_LIBRO_MAGICO_S:
			sprintf(buf, "Incantesimo: %s di classe %d",
				skill_name(GET_OBJ_VAL(j, 3)), GET_OBJ_VAL(j, 2));
			break;
		default:
			sprintf(buf, "Values 0-9: [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d]",
				GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1),
				GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 3),
				GET_OBJ_VAL(j, 4), GET_OBJ_VAL(j, 5),
				GET_OBJ_VAL(j, 6), GET_OBJ_VAL(j, 7),
				GET_OBJ_VAL(j, 8), GET_OBJ_VAL(j, 9));
			break;
	}
	send_to_char(strcat(buf, "\r\n"), ch);
	
  /*
   * I deleted the "equipment status" code from here because it seemed
   * more or less useless and just takes up valuable screen space.
   */
	
	if (j->contains) {
		sprintf(buf, "\r\nContents:%s", CCGRN(ch, C_NRM));
		for (found = 0, j2 = j->contains; j2; j2 = j2->next_content) {
			sprintf(buf2, "%s %s", found++ ? "," : "", j2->short_description);
			strcat(buf, buf2);
			if (strlen(buf) >= 62) {
				if (j2->next_content)
					send_to_char(strcat(buf, ",\r\n"), ch);
				else
					send_to_char(strcat(buf, "\r\n"), ch);
				*buf = found = 0;
			}
		}
		
		if (*buf)
			send_to_char(strcat(buf, "\r\n"), ch);
		send_to_char(CCNRM(ch, C_NRM), ch);
	}
	found = 0;
	send_to_char("Affections:", ch);
	for (i = 0; i < MAX_OBJ_AFFECT; i++)
		if (j->affected[i].modifier) {
			sprinttype(j->affected[i].location, apply_types, buf2);
			sprintf(buf, "%s %+d to %s", found++ ? "," : "",
				j->affected[i].modifier, buf2);
			send_to_char(buf, ch);
		}
	if (!found)
		send_to_char(" None", ch);
	
	send_to_char("\r\n", ch);
	
  /* check the object for a script */
	do_sstat_object(ch, j);
}


void do_stat_character(struct char_data * ch, struct char_data * k)
{
	int i, i2, found = 0, GETAC=0;
	struct obj_data *j;
	struct follow_type *fol;
	struct affected_type *aff;
	struct char_file_u chdata;
	
	extern struct attack_hit_type attack_hit_text[];
	extern const char *fama_mess[];
	extern const char *notorieta_mess[];
	extern char *nomi_religioni[];
	
	switch (GET_SEX(k)) {
		case SEX_NEUTRAL:    strcpy(buf, "NEUTRAL-SEX");   break;
		case SEX_MALE:       strcpy(buf, "MALE");          break;
		case SEX_FEMALE:     strcpy(buf, "FEMALE");        break;
		default:             strcpy(buf, "ILLEGAL-SEX!!"); break;
	}
	
	sprintf(buf2, " %s '%s'  IDNum: [%5ld], In room [%5d], IDMob [%5ld]\r\n",
		(!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
		GET_NAME(k), GET_IDNUM(k), k->in_room != NOWHERE ?
		world[k->in_room].number : NOWHERE , !IS_NPC(k) ? -1 : GET_MOB_ID(k));
	
	send_to_char(strcat(buf, buf2), ch);
	if (IS_MOB(k)) {
		sprintf(buf, "Alias: %s, VNum: [%5d], RNum: [%5d]\r\n",
			k->player.name, GET_MOB_VNUM(k), GET_MOB_RNUM(k));
		send_to_char(buf, ch);
	}
	
	sprintf(buf, "Title: %s\r\n", (k->player.title ? k->player.title : "<None>"));
	send_to_char(buf, ch);
	
	sprintf(buf, "L-Des: %s", (k->player.long_descr ? k->player.long_descr : "<None>\r\n"));
	send_to_char(buf, ch);
	
	if (IS_NPC(k)) {
		strcpy(buf, "Monster Class: ");
		sprinttype(k->player.class, npc_class_types, buf2);
	} else {
		strcpy(buf, "Class: ");
		sprinttype(k->player.class, pc_class_types, buf2);
	}
	strcat(buf, buf2);
	
	sprintf(buf2, ", Lev: [%s%2d%s], XP: [%s%7d%s], Align: [%4d]\r\n",
		CCYEL(ch, C_NRM), GET_LEVEL(k), CCNRM(ch, C_NRM),
		CCYEL(ch, C_NRM), GET_EXP(k), CCNRM(ch, C_NRM),
		GET_ALIGNMENT(k));
	strcat(buf, buf2);
	send_to_char(buf, ch);
	
	if (!IS_NPC(k)) {
		strcpy(buf1, (char *) asctime(localtime(&(k->player.time.birth))));
		if (load_char(GET_NAME(k), &chdata) > -1)
			strcpy(buf2, (char *) asctime(localtime(&chdata.last_logon)));
		else
			strcpy(buf2, (char *) asctime(localtime(&(k->player.time.logon))));
		buf1[10] = buf2[10] = '\0';
		
		sprintf(buf, "Created: [%s], Last Logon: [%s], Played [%dh %dm], Age [%d]\r\n",
			buf1, buf2, k->player.time.played / 3600,
			((k->player.time.played / 3600) % 60), age(k).year);
		send_to_char(buf, ch);
		
		sprintf(buf, "Hometown: [%d], Speaks: [%d/%d/%d], (STL[%d]/per[%d]/BONUS[%d])",
			k->player.hometown, GET_TALK(k, 0), GET_TALK(k, 1), GET_TALK(k, 2),
			GET_PRACTICES(k), int_app[GET_INT(k)].learn,
			wis_app[GET_WIS(k)].bonus);
    /*. Display OLC zone for immorts .*/
		if(GET_LEVEL(k) >= LVL_MINIBUILD)
			sprintf(buf, "%s, OLC[%d]", buf, GET_OLC_ZONE(k));
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
	}
	sprintf(buf, "Str: [%s%d/%d%s]  Int: [%s%d%s]  Wis: [%s%d%s]  "
		"Dex: [%s%d%s]  Con: [%s%d%s]  Cha: [%s%d%s]\r\n",
		CCCYN(ch, C_NRM), GET_STR(k), GET_ADD(k), CCNRM(ch, C_NRM),
		CCCYN(ch, C_NRM), GET_INT(k), CCNRM(ch, C_NRM),
		CCCYN(ch, C_NRM), GET_WIS(k), CCNRM(ch, C_NRM),
		CCCYN(ch, C_NRM), GET_DEX(k), CCNRM(ch, C_NRM),
		CCCYN(ch, C_NRM), GET_CON(k), CCNRM(ch, C_NRM),
		CCCYN(ch, C_NRM), GET_CHA(k), CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
	
	sprintf(buf, "Hit p.:[%s%d/%d+%d%s]  Mana p.:[%s%d/%d+%d%s]  Move p.:[%s%d/%d+%d%s]\r\n",
		CCGRN(ch, C_NRM), GET_HIT(k), GET_MAX_HIT(k), hit_gain(k), CCNRM(ch, C_NRM),
		CCGRN(ch, C_NRM), GET_MANA(k), GET_MAX_MANA(k), mana_gain(k), CCNRM(ch, C_NRM),
		CCGRN(ch, C_NRM), GET_MOVE(k), GET_MAX_MOVE(k), move_gain(k), CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
	
	sprintf(buf, "Coins: [%9d], Bank: [%9d] (Total: %d)\r\n",
		GET_GOLD(k), GET_BANK_GOLD(k), GET_GOLD(k) + GET_BANK_GOLD(k));
	send_to_char(buf, ch);
	if (GET_AC(k)<-100) GETAC=-100;
	else GETAC=GET_AC(k);
	sprintf(buf, "AC: [%d/10], Hitroll: [%2d], Damroll: [%2d], Saving throws: [%d/%d/%d/%d/%d]\r\n",
		GETAC, (affected_by_spell(k, SPELLSKILL, DISEASE_BACCO) ? 0 : GET_HITROLL(k)), k->points.damroll, GET_SAVE(k, 0),
		GET_SAVE(k, 1), GET_SAVE(k, 2), GET_SAVE(k, 3), GET_SAVE(k, 4));
	send_to_char(buf, ch);

	sprintf(buf, "Res-fuoco %d%% / Res-ghiaccio %d%% / Res-elettricita %d%% /\r\n"
		     "Res-acido %d%% / Res-Oscurita %d%% / Res-Caos %d%% /\r\n"
			 "Res-Ordine %d%% / Res-Luce %d%% / Res-danno fisico %d%%\r\n",
		MIN(100,GET_RES_FUOCO(k)), MIN(100,GET_RES_GHIACCIO(k)), MIN(100,GET_RES_ELETTRICITA(k)),
		MIN(100,GET_RES_ACIDO(k)), MIN(100,GET_RES_SHAARR(k)), MIN(100,GET_RES_XHYPHYS(k)),
		MIN(100,GET_RES_THERION(k)), MIN(100,GET_RES_SILUE(k)), MIN(100,GET_RES_FISICO(k)));
	send_to_char(buf, ch);
	
	if (!IS_NPC(k))
	{
		i = (char) (GET_FAMA(k)/100);
		i = MAX(0, MIN(i, 10));
		sprintf(buf, "Notorieta': [%4d] Ranking: %s\r\n", GET_FAMA(k), fama_mess[i]);
		send_to_char(buf, ch);
		i = (char) (GET_NOTORIETA(k)/100);
		i = MAX(0, MIN(i, 9));
		sprintf(buf, "Fama: [%4d] Ranking: %s\r\n", GET_NOTORIETA(k), notorieta_mess[i]);
		send_to_char(buf, ch);
		sprintf(buf, "Punti Quest: [%4d]   Religione: %s\r\n", GET_QUESTPOINTS(k), nomi_religioni[GET_CULTO(k)]); //Orione
		send_to_char(buf, ch);
			}
	
	sprinttype(GET_POS(k), position_types, buf2);
	sprintf(buf, "Pos: %s, Fighting: %s", buf2,
		(FIGHTING(k) ? GET_NAME(FIGHTING(k)) : "Nobody"));
	
	if (IS_NPC(k)) {
		strcat(buf, ", Attack type: ");
		strcat(buf, attack_hit_text[k->mob_specials.attack_type].singular);
	}
	if (k->desc) {
		sprinttype(k->desc->connected, connected_types, buf2);
		strcat(buf, ", Connected: ");
		strcat(buf, buf2);
	}
	send_to_char(strcat(buf, "\r\n"), ch);
	
	strcpy(buf, "Default position: ");
	sprinttype((k->mob_specials.default_pos), position_types, buf2);
	strcat(buf, buf2);
	
	sprintf(buf2, ", Idle Timer (in tics) [%d]\r\n", k->char_specials.timer);
	strcat(buf, buf2);
	send_to_char(buf, ch);
	
	if (IS_NPC(k)) {
		sprintbit(MOB_FLAGS(k), action_bits, buf2);
		sprintf(buf, "NPC flags: %s%s%s\r\n", CCCYN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	} else {
		sprintbit(PLR_FLAGS(k), player_bits, buf2);
		sprintf(buf, "PLR: %s%s%s\r\n", CCCYN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
		sprintbit(PRF_FLAGS(k), preference_bits, buf2);
		sprintf(buf, "PRF: %s%s%s\r\n", CCGRN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	}
	
	if (IS_MOB(k)) {
		sprintf(buf, "Mob Spec-Proc: %s, NPC Bare Hand Dam: %dd%d\r\n",
			(mob_index[GET_MOB_RNUM(k)].func ? "Exists" : "None"),
			k->mob_specials.damnodice, k->mob_specials.damsizedice);
		send_to_char(buf, ch);
	}
	sprintf(buf, "Carried: weight: %d, items: %d; ",
		IS_CARRYING_W(k), IS_CARRYING_N(k));
	
	for (i = 0, j = k->carrying; j; j = j->next_content, i++);
	sprintf(buf, "%sItems in: inventory: %d, ", buf, i);
	
	for (i = 0, i2 = 0; i < NUM_WEARS; i++)
		if (GET_EQ(k, i))
			i2++;
	sprintf(buf2, "eq: %d\r\n", i2);
	strcat(buf, buf2);
	send_to_char(buf, ch);
	
	sprintf(buf, "Hunger: %d, Thirst: %d, Drunk: %d\r\n",
		GET_COND(k, FULL), GET_COND(k, THIRST), GET_COND(k, DRUNK));
	send_to_char(buf, ch);

	sprintf(buf, "Stanchezza: %ld\r\n", GET_SENZA_DORMIRE(k));
	send_to_char(buf, ch);
	
	sprintf(buf, "Master is: %s, Followers are:",
		((k->master) ? GET_NAME(k->master) : "<none>"));
	for (fol = k->followers; fol; fol = fol->next) {
		sprintf(buf2, "%s %s", found++ ? "," : "", PERS(fol->follower, ch));
		strcat(buf, buf2);
		if (strlen(buf) >= 62) {
			if (fol->next)
				send_to_char(strcat(buf, ",\r\n"), ch);
			else
				send_to_char(strcat(buf, "\r\n"), ch);
			*buf = found = 0;
		}
	}
	
	if (*buf)
		send_to_char(strcat(buf, "\r\n"), ch);
	
	if (!IS_NPC(k))
	{
		sprintf(buf, "Wilderness Maximum Range    X: %2d     Y: %2d\r\n", GET_WILD_MAX_X_RANGE(k),
			GET_WILD_MAX_Y_RANGE(k));
		send_to_char(buf, ch);
	}
	if (IS_NPC(k) && MOB_FLAGGED(k, MOB_WILDHUNT))
	{
		sprintf(buf, "Wilderness Hunting: %s\r\n", WILDHUNT(k) ? GET_NAME(WILDHUNT(k)) : "Nessuno");
		send_to_char(buf, ch);
	}
	
  /* Showing the bitvector */
	sprintbit(AFF_FLAGS_S(k, 0), affected_bits, buf2);
	sprintbit(AFF_FLAGS_S(k, 1), affected_bits_1, buf1);
	if (strcmp(buf1, "NOBITS ")){
		if (strcmp(buf2, "NOBITS "))
			strcat(buf2, buf1);
		else
			sprintf (buf2, "%s", buf1);
	}
	sprintbit(AFF_FLAGS_S(k, 2), affected_bits_2, buf1);
	if (strcmp(buf1, "NOBITS ")){
		if (strcmp(buf2, "NOBITS "))
			strcat(buf2, buf1);
		else
			sprintf (buf2, "%s", buf1);
	}
	sprintbit(AFF_FLAGS_S(k, 3), affected_bits_3, buf1);
	if (strcmp(buf1, "NOBITS ")){
		if (strcmp(buf2, "NOBITS "))
			strcat(buf2, buf1);
		else
			sprintf (buf2, "%s", buf1);
	}
	sprintf(buf, "AFF: %s%s%s\r\n", CCYEL(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
  /* rip/kill/dt counter */
	if (!IS_MOB(k))
	{
		sprintf(buf, "Rip: [%d]  Kills: [%d]  DeathTraps: [%d]\r\n",GET_RIP_CNT(k), GET_KILL_CNT(k), GET_DT_CNT(k));
		sprintf(buf+strlen(buf), "Morti in questo livello: %d\r\n", k->player_specials->saved.morti_per_livello);
		sprintf(buf+strlen(buf), "Morti per mano di pg: %ld\r\n", GET_MORTI_DA_PG(k));
		send_to_char(buf, ch);
		sprintf(buf, "&8&3Ore che mancano per resuscitare:&0 [&8&7%4d&0]\r\n(-1 indica che il pg non e' morto oppure non ha ancora chiesto di resuscitare)\r\n",GET_RESUSCITA(k)-1);
		send_to_char(buf,ch);

	}

   /* Peste */

	if (!IS_NPC(k) && affected_by_spell(k, SPELLSKILL, DISEASE_PESTE)){
		sprintf (buf, "Appestato: %ld, minuti da vivere: %ld\r\n", GET_PESTE(k), (240 - GET_PESTE(k)) / 12 + 1);
		send_to_char (buf, ch);
	}

   /* Mob Stipendiati */
	if (MASTER_ID(k)>0) {
		sprintf(buf,"Padrone: &1%s&0\r\nStipendio: [%ld]  Tassa Equip: [%ld] Pagare: [%ld]  Ritrovo: [%d]\r\n",NAME(MASTER_ID(k)),GET_STIPENDIOMOB(k),get_stipendiomob(k)-GET_STIPENDIOMOB(k),GET_PAGA(k),world[(GET_HOME(k))].number);
		send_to_char(buf, ch);
	}
  /* Routine to show what spells a char is affected by */
	if (k->affected) {
		for (aff = k->affected; aff; aff = aff->next) {
			*buf2 = '\0';
			sprintf(buf, "SPL: (%3dhr) %s%-21s%s ", aff->duration + 1, CCCYN(ch, C_NRM), (aff->natura == SPELLSKILL ? spells[aff->type] : (aff->natura == PROFICIENZE ? proficienze[aff->type] : (aff->type != -1 ? abilita[aff->type] : "(none)"))), CCNRM(ch, C_NRM));
			if (aff->modifier) {
				sprintf(buf2, "%+d to %s", aff->modifier, apply_types[(int) aff->location]);
				strcat(buf, buf2);
			}
			if (aff->bitvector[0]) {
				if (*buf2)
					strcat(buf, ", sets ");
				else
					strcat(buf, "sets ");
				sprintbit(aff->bitvector[0], affected_bits, buf2);
				strcat(buf, buf2);
			}
			if (aff->bitvector[1]) {
				if (*buf2)
					strcat(buf, ", sets ");
				else
					strcat(buf, "sets ");
				sprintbit(aff->bitvector[1], affected_bits_1, buf2);
				strcat(buf, buf2);
			}
			if (aff->bitvector[2]) {
				if (*buf2)
					strcat(buf, ", sets ");
				else
					strcat(buf, "sets ");
				sprintbit(aff->bitvector[2], affected_bits_2, buf2);
				strcat(buf, buf2);
			}
			if (aff->bitvector[3]) {
				if (*buf2)
					strcat(buf, ", sets ");
				else
					strcat(buf, "sets ");
				sprintbit(aff->bitvector[3], affected_bits_3, buf2);
				strcat(buf, buf2);
			}
			send_to_char(strcat(buf, "\r\n"), ch);
		}
	}
	
	//Orione Inizio
	/* check mobiles for a script */
	if (IS_NPC(k)) {
		do_sstat_character(ch, k);
		if (SCRIPT_MEM(k)) {
			struct script_memory *mem = SCRIPT_MEM(k);
			send_to_char("Script memory:\r\n  Remember             Command\r\n", ch);
			while (mem) {
				struct char_data *mc = find_char(mem->id);
				if (!mc) send_to_char("  ** Corrupted!\r\n", ch);
				else {
					if (mem->cmd) sprintf(buf,"  %-20.20s%s\r\n",GET_NAME(mc),mem->cmd);
					else sprintf(buf,"  %-20.20s <default>\r\n",GET_NAME(mc));
					send_to_char(buf, ch);
				}
				mem = mem->next;
			}
		}
	} else {
		/* this is a PC, display their global variables */
		if (k->script && k->script->global_vars) {
			struct trig_var_data *tv;
			char name[MAX_INPUT_LENGTH];
			void find_uid_name(char *uid, char *name);
			
			send_to_char("Global Variables:\r\n", ch);

			/* currently, variable context for players is always 0, so it is */
			/* not displayed here. in the future, this might change */
			for (tv = k->script->global_vars; tv; tv = tv->next) {
				if (*(tv->value) == UID_CHAR) {
					find_uid_name(tv->value, name);
					sprintf(buf, "    %10s:  [UID]: %s\r\n", tv->name, name);
				} else
					sprintf(buf, "    %10s:  %s\r\n", tv->name, tv->value);
				send_to_char(buf, ch);
			}
		}
	}
	//Orione Fine
}


ACMD(do_stat)
{
	struct char_data *victim = 0;
	struct obj_data *object = 0;
	struct char_file_u tmp_store;
	int tmp;
	
	half_chop(argument, buf1, buf2);
	
	if (!*buf1) {
		send_to_char("Stats on who or what?\r\n", ch);
		return;
	} else if (is_abbrev(buf1, "room")) {
		do_stat_room(ch);
	} else if (is_abbrev(buf1, "wild")) {
		if (!*buf2)
			send_to_char("Stats on which wild element?\r\n", ch);
		else
		{
			if ((tmp = real_wild_table (atoi(buf2))) >= 0)
				do_stat_wild(ch, wild_table+tmp);
			else
				send_to_char("No such wild element around.\r\n", ch);
		}
	} else if (is_abbrev(buf1, "mob")) {
		if (!*buf2)
			send_to_char("Stats on which mobile?\r\n", ch);
		else {
			if ((victim = get_char_vis(ch, buf2)))
				do_stat_character(ch, victim);
			else
				send_to_char("No such mobile around.\r\n", ch);
		}
	} else if (is_abbrev(buf1, "player")) {
		if (!*buf2) {
			send_to_char("Stats on which player?\r\n", ch);
		} else {
			if ((victim = get_player_vis(ch, buf2, 0)))
				do_stat_character(ch, victim);
			else
				send_to_char("No such player around.\r\n", ch);
		}
	} else if (is_abbrev(buf1, "file")) {
		if (!*buf2) {
			send_to_char("Stats on which player?\r\n", ch);
		} else {
			CREATE(victim, struct char_data, 1);
			clear_char(victim);
			if (load_char(buf2, &tmp_store) > -1) {
				store_to_char(&tmp_store, victim);
				char_to_room(victim, 0);
				if (GET_LEVEL(victim) > GET_LEVEL(ch))
					send_to_char("Sorry, you can't do that.\r\n", ch);
				else
					do_stat_character(ch, victim);
				extract_char(victim);
			} else {
				send_to_char("There is no such player.\r\n", ch);
				free(victim);
			}
		}
	} else if (is_abbrev(buf1, "object")) {
		if (!*buf2)
			send_to_char("Stats on which object?\r\n", ch);
		else {
			if ((object = get_obj_vis(ch, buf2)))
				do_stat_object(ch, object);
			else
				send_to_char("No such object around.\r\n", ch);
		}
	} else {
		if ((object = get_object_in_equip_vis(ch, buf1, ch->equipment, &tmp)))
			do_stat_object(ch, object);
		else if ((object = get_obj_in_list_vis(ch, buf1, ch->carrying)))
			do_stat_object(ch, object);
		else if ((victim = get_char_room_vis(ch, buf1)))
			do_stat_character(ch, victim);
		else if ((object = get_obj_in_list_vis(ch, buf1, world[ch->in_room].contents)))
			do_stat_object(ch, object);
		else if ((victim = get_char_vis(ch, buf1)))
			do_stat_character(ch, victim);
		else if ((object = get_obj_vis(ch, buf1)))
			do_stat_object(ch, object);
		else
			send_to_char("Nothing around by that name.\r\n", ch);
	}
}


ACMD(do_shutdown)
{
	extern int circle_shutdown, circle_reboot;
	
	if (subcmd != SCMD_SHUTDOWN) {
		send_to_char("If you want to shut something down, say so!\r\n", ch);
		return;
	}
	one_argument(argument, arg);
	
	if (!*arg) {
		sprintf(buf, "(GC) Shutdown by %s.", GET_NAME(ch));
		log(buf);
		send_to_all("Connessione chiusa per manutenzione.\r\n");
		circle_shutdown = 1;
	} else if (!str_cmp(arg, "reboot")) {
		sprintf(buf, "(GC) Reboot by %s.", GET_NAME(ch));
		log(buf);
		send_to_all("Il MUD ripartira' tra qualche minuto. Grazie della pazienza!!\r\n");
		touch("../.fastboot");
		circle_shutdown = circle_reboot = 1;
	} else if (!str_cmp(arg, "now")) {
		sprintf(buf, "(GC) Shutdown NOW by %s.", GET_NAME(ch));
		log(buf);
		send_to_all("Il MUD ripartira' tra qualche minuto. Grazie della pazienza!!\r\n");
		circle_shutdown = 1;
		circle_reboot = 2;
	} else if (!str_cmp(arg, "die")) {
		sprintf(buf, "(GC) Shutdown by %s.", GET_NAME(ch));
		log(buf);
		send_to_all("Connessione chiusa per manutenzione.\r\n");
		touch("../.killscript");
		circle_shutdown = 1;
	} else if (!str_cmp(arg, "pause")) {
		sprintf(buf, "(GC) Shutdown by %s.", GET_NAME(ch));
		log(buf);
		send_to_all("Connessione chiusa per manutezione.\r\n");
		touch("../pause");
		circle_shutdown = 1;
	} else
		send_to_char("Unknown shutdown option.\r\n", ch);
}


void stop_snooping(struct char_data * ch)
{
	if (!ch->desc->snooping)
		send_to_char("You aren't snooping anyone.\r\n", ch);
	else {
		send_to_char("You stop snooping.\r\n", ch);
		ch->desc->snooping->snoop_by = NULL;
		ch->desc->snooping = NULL;
	}
}


ACMD(do_snoop)
{
	struct char_data *victim, *tch;
	
	if (!ch->desc)
		return;
	
	one_argument(argument, arg);
	
	if (!*arg)
		stop_snooping(ch);
	else if (!(victim = get_char_vis(ch, arg)))
		send_to_char("No such person around.\r\n", ch);
	else if (!victim->desc)
		send_to_char("There's no link.. nothing to snoop.\r\n", ch);
	else if (victim == ch)
		stop_snooping(ch);
	else if (victim->desc->snoop_by)
		send_to_char("Busy already. \r\n", ch);
	else if (victim->desc->snooping == ch->desc)
		send_to_char("Don't be stupid.\r\n", ch);
	else {
		if (victim->desc->original)
			tch = victim->desc->original;
		else
			tch = victim;
		
		if (GET_LEVEL(tch) >= GET_LEVEL(ch)) {
			send_to_char("You can't.\r\n", ch);
			return;
		}
		send_to_char(OK, ch);
		
		if (ch->desc->snooping)
			ch->desc->snooping->snoop_by = NULL;
		
		ch->desc->snooping = victim->desc;
		victim->desc->snoop_by = ch->desc;
	}
}



ACMD(do_switch)
{
	struct char_data *victim;
	
	one_argument(argument, arg);
	
	if (ch->desc->original)
		send_to_char("Sei gia' switchato.\r\n", ch);
	else if (!*arg)
		send_to_char("Switchare dentro chi?\r\n", ch);
	else if (!(victim = get_char_vis(ch, arg)))
		send_to_char("Personaggio non trovato.\r\n", ch);
	else if (ch == victim)
		send_to_char("Hehehehe....Veramente simpatico! :)\r\n", ch);
	else if (victim->desc)
		send_to_char("Non puoi farlo, qualcuno lo sta gia' usando!\r\n", ch);
	else if ((GET_LEVEL(ch) < LVL_IMPL) && !IS_NPC(victim))
		send_to_char("Non sei abbastanza Dio per switchare in un corpo mortale.\r\n", ch);
	else {
		send_to_char(OK, ch);
		
		ch->desc->character = victim;
		ch->desc->original = ch;
		
		victim->desc = ch->desc;
		ch->desc = NULL;
		if (IS_TRUST(ch)) {
			sprintf(buf, "(TRUST) %s switch in %s.", GET_NAME(ch),GET_NAME(victim));
			mudlog(buf, CMP, LVL_GOD, TRUE);
		}
		if (IS_NPC(victim))
			PRF_FLAGS(victim)=PRF_FLAGS(ch); //Copia delle Preferenze nel MOB
	}
}


ACMD(do_return)
{
	if (ch->desc && ch->desc->original) {
		send_to_char("You return to your original body.\r\n", ch);
		
		if (IS_TRUST(ch->desc->original)) {
			sprintf(buf, "(TRUST) %s return in his body.", GET_NAME(ch->desc->original));
			mudlog(buf, CMP, LVL_GOD, TRUE);
		}
    /* JE 2/22/95 */
    /* if someone switched into your original body, disconnect them */
		if (ch->desc->original->desc)
			close_socket(ch->desc->original->desc);
		
		ch->desc->character = ch->desc->original;
		ch->desc->original = NULL;
		
		ch->desc->character->desc = ch->desc;
		ch->desc = NULL;
		
		
	}
}



ACMD(do_load)
{
	struct char_data *mob;
	struct obj_data *obj;
	int number, r_num;
	
	two_arguments(argument, buf, buf2);
	
	if (!*buf || !*buf2 || !isdigit(*buf2)) {
		send_to_char("Usage: load { obj | mob } <number>\r\n", ch);
		return;
	}
	if ((number = atoi(buf2)) < 0) {
		send_to_char("A NEGATIVE number??\r\n", ch);
		return;
	}
	if (is_abbrev(buf, "mob")) {
		if ((r_num = real_mobile(number)) < 0) {
			send_to_char("There is no monster with that number.\r\n", ch);
			return;
		}
		mob = read_mobile(r_num, REAL);
		char_to_room(mob, ch->in_room);
		SET_BIT(MOB_FLAGS(mob), MOB_CRASH);
		act("$n compie un gesto con la mano.", TRUE, ch,
			0, 0, TO_ROOM);
		act("$n ha creato $N!", FALSE, ch, 0, mob, TO_ROOM);
		act("Crei $N.", FALSE, ch, 0, mob, TO_CHAR);
		if (IS_TRUST(ch)) {
			sprintf(buf, "(TRUST) %s carica il MOB %s (#%d).", GET_NAME(ch),GET_NAME(mob),number);
			mudlog(buf, CMP, LVL_GOD, TRUE);
		}
		load_mtrigger(mob); //Orione
		
	} else if (is_abbrev(buf, "obj")) {
		if ((r_num = real_object(number)) < 0) {
			send_to_char("There is no object with that number.\r\n", ch);
			return;
		}
		obj = read_object(r_num, REAL);
		
		act("$n compie una strano gesto magico.", TRUE, ch, 0, 0, TO_ROOM);
		act("$n ha creato $p!", FALSE, ch, obj, 0, TO_ROOM);
		act("Crei $p.", FALSE, ch, obj, 0, TO_CHAR);
		obj_to_room(obj, ch->in_room);
		
		if (IS_TRUST(ch)) {
			sprintf(buf, "(TRUST) %s carica l'OBJ %s (#%d).", GET_NAME(ch),GET_OBJ_NAME(obj),number);
			mudlog(buf, CMP, LVL_GOD, TRUE);
		}
		load_otrigger(obj); //Orione
		
	} else
		send_to_char("That'll have to be either 'obj' or 'mob'.\r\n", ch);
}



ACMD(do_vstat)
{
	struct char_data *mob;
	struct obj_data *obj;
	int number, r_num;
	
	two_arguments(argument, buf, buf2);
	
	if (!*buf || !*buf2 || !isdigit(*buf2)) {
		send_to_char("Usage: vstat { obj | mob } <number>\r\n", ch);
		return;
	}
	if ((number = atoi(buf2)) < 0) {
		send_to_char("A NEGATIVE number??\r\n", ch);
		return;
	}
	if (is_abbrev(buf, "mob")) {
		if ((r_num = real_mobile(number)) < 0) {
			send_to_char("There is no monster with that number.\r\n", ch);
			return;
		}
		mob = read_mobile(r_num, REAL);
		char_to_room(mob, 0);
		do_stat_character(ch, mob);
		extract_char(mob);
	} else if (is_abbrev(buf, "obj")) {
		if ((r_num = real_object(number)) < 0) {
			send_to_char("There is no object with that number.\r\n", ch);
			return;
		}
		obj = read_object(r_num, REAL);
		do_stat_object(ch, obj);
		extract_obj(obj);
	} else
		send_to_char("That'll have to be either 'obj' or 'mob'.\r\n", ch);
}




/* clean a room of all mobiles and objects */
ACMD(do_purge)
{
	struct char_data *vict, *next_v;
	struct obj_data *obj, *next_o;
	
	one_argument(argument, buf);
	
	if (*buf) {			/* argument supplied. destroy single object
				 * or char */
		if ((vict = get_char_room_vis(ch, buf))) {
			if (!IS_NPC(vict) && (GET_LEVEL(ch) <= GET_LEVEL(vict))) {
				send_to_char("Fuuuuuuuuu!\r\n", ch);
				return;
			}
			act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);
			
			if (!IS_NPC(vict)) {
				sprintf(buf, "(GC) %s has purged %s.", GET_NAME(ch), GET_NAME(vict));
				mudlog(buf, BRF, LVL_GOD, TRUE);
				if (vict->desc) {
					close_socket(vict->desc);
					vict->desc = NULL;
				}
			}
			extract_char(vict);
		} else if ((obj = get_obj_in_list_vis(ch, buf, world[ch->in_room].contents))) {
			act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
			extract_obj(obj);
		} else {
			send_to_char("Nothing here by that name.\r\n", ch);
			return;
		}
		
		send_to_char(OK, ch);
	} else {			/* no argument. clean out the room */
		act("$n gestures... You are surrounded by scorching flames!",
			FALSE, ch, 0, 0, TO_ROOM);
		send_to_room("The world seems a little cleaner.\r\n", ch->in_room);
		
		for (vict = world[ch->in_room].people; vict; vict = next_v) {
			next_v = vict->next_in_room;
			if (IS_NPC(vict))
				extract_char(vict);
		}
		
		for (obj = world[ch->in_room].contents; obj; obj = next_o) {
			next_o = obj->next_content;
			extract_obj(obj);
		}
	}
}



ACMD(do_advance)
{
	struct char_data *victim;
	char *name = arg, *level = buf2;
	int newlevel, oldlevel;
	void do_start(struct char_data *ch);
	int gain_exp(struct char_data * ch, int gain);
	two_arguments(argument, name, level);
	
	if (*name) {
		if (!(victim = get_char_vis(ch, name))) {
			send_to_char("That player is not here.\r\n", ch);
			return;
		}
	} else {
		send_to_char("Advance who?\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) <= GET_LEVEL(victim)) {
		send_to_char("Maybe that's not such a great idea.\r\n", ch);
		return;
	}
	if (IS_NPC(victim)) {
		send_to_char("NO!  Not on NPC's.\r\n", ch);
		return;
	}
	if (!*level || (newlevel = atoi(level)) <= 0) {
		send_to_char("That's not a level!\r\n", ch);
		return;
	}
	if (newlevel > LVL_IMPL) {
		sprintf(buf, "%d is the highest possible level.\r\n", LVL_IMPL);
		send_to_char(buf, ch);
		return;
	}
	if (newlevel > GET_LEVEL(ch)) {
		send_to_char("Yeah, right.\r\n", ch);
		return;
	}
	if (newlevel == GET_LEVEL(victim)) {
		send_to_char("They are already at that level.\r\n", ch);
		return;
	}
	oldlevel = GET_LEVEL(victim);
	if (newlevel < GET_LEVEL(victim)) {
		do_start(victim);
		GET_LEVEL(victim) = newlevel;
		send_to_char("You are momentarily enveloped by darkness!\r\n"
			"You feel somewhat diminished.\r\n", victim);
	} else {
		act("$n makes some strange gestures.\r\n"
			"A strange feeling comes upon you,\r\n"
			"Like a giant hand, light comes down\r\n"
			"from above, grabbing your body, that\r\n"
			"begins to pulse with colored lights\r\n"
			"from inside.\r\n\r\n"
			"Your head seems to be filled with demons\r\n"
			"from another plane as your body dissolves\r\n"
			"to the elements of time and space itself.\r\n"
			"Suddenly a silent explosion of light\r\n"
			"snaps you back to reality.\r\n\r\n"
			"You feel slightly different.", FALSE, ch, 0, victim, TO_VICT);
	}
	
	send_to_char(OK, ch);
	
	sprintf(buf, "(GC) %s has advanced %s to level %d (from %d)",
		GET_NAME(ch), GET_NAME(victim), newlevel, oldlevel);
	log(buf);
	gain_exp_regardless(victim, (titles[(int) GET_CLASS(victim)][newlevel].exp) - GET_EXP(victim));
	save_char(victim, NOWHERE);
}



ACMD(do_restore)
{
	struct char_data *vict;
	int i;
	
	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Whom do you wish to restore?\r\n", ch);
	else if (!(vict = get_char_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else {
		
		if (IS_TRUST(ch)) {
			sprintf(buf, "(TRUST) %s restore %s.", GET_NAME(ch),GET_NAME(vict));
			mudlog(buf, CMP, LVL_GOD, TRUE);
		}
		
		GET_HIT(vict) = GET_MAX_HIT(vict);
		GET_MANA(vict) = GET_MAX_MANA(vict);
		GET_MOVE(vict) = GET_MAX_MOVE(vict);
		
		if ((GET_LEVEL(ch) >= LVL_GRGOD) && (GET_LEVEL(vict) >= LVL_IMMORT) && (!IS_TRUST(vict))) {
			for (i = 1; i <= MAX_SKILLS; i++)
				SET_SKILL(vict, i, 100);
			
			if (GET_LEVEL(vict) >= LVL_GRGOD) {
				vict->real_abils.str_add = 100;
				vict->real_abils.intel = 25;
				vict->real_abils.wis = 25;
				vict->real_abils.dex = 25;
				vict->real_abils.str = 25;
				vict->real_abils.con = 25;
				vict->real_abils.cha = 25;
			}
			vict->aff_abils = vict->real_abils;
		}
		update_pos(vict);
		send_to_char(OK, ch);
		act("Una mano divina ti guarisce!", FALSE, vict, 0, ch, TO_CHAR);
	}
}


void perform_immort_vis(struct char_data *ch)
{
	void appear(struct char_data *ch);
	
	if (GET_INVIS_LEV(ch) == 0 && !IS_AFFECTED(ch, AFF_HIDE | AFF_INVISIBLE)) {
		send_to_char("You are already fully visible.\r\n", ch);
		return;
	}
	
	GET_INVIS_LEV(ch) = 0;
	appear(ch);
	send_to_char("You are now fully visible.\r\n", ch);
}


void perform_immort_invis(struct char_data *ch, int level)
{
	struct char_data *tch;
	
	if (IS_NPC(ch))
		return;
	
	for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (tch == ch)
			continue;
		if (GET_LEVEL(tch) >= GET_INVIS_LEV(ch) && GET_LEVEL(tch) < level)
			act("In un istante realizzi che $n non e' piu' qui.", FALSE, ch, 0,
				tch, TO_VICT);
		if (GET_LEVEL(tch) < GET_INVIS_LEV(ch) && GET_LEVEL(tch) >= level)
			act("In un istante realizzi che $n e' qui.", FALSE, ch, 0,
				tch, TO_VICT);
	}
	
	GET_INVIS_LEV(ch) = level;
	sprintf(buf, "Your invisibility level is %d.\r\n", level);
	send_to_char(buf, ch);
}


ACMD(do_invis)
{
	int level;
	
	if (IS_NPC(ch)) {
		send_to_char("You can't do that!\r\n", ch);
		return;
	}
	
	if (IS_TRUST(ch)) {
		send_to_char("Non puoi renderti visibile!\r\n",ch);
		return;
	}
	
	one_argument(argument, arg);
	if (!*arg) {
		if (GET_INVIS_LEV(ch) > 0)
			perform_immort_vis(ch);
		else
			perform_immort_invis(ch, GET_LEVEL(ch));
	} else {
		level = atoi(arg);
		if (level > GET_LEVEL(ch))
			send_to_char("You can't go invisible above your own level.\r\n", ch);
		else if (level < 1)
			perform_immort_vis(ch);
		else
			perform_immort_invis(ch, level);
	}
}


ACMD(do_gecho)
{
	struct descriptor_data *pt;
	
	skip_spaces(&argument);
	
	if (!*argument)
		send_to_char("That must be a mistake...\r\n", ch);
	else {
		sprintf(buf, "%s\r\n", argument);
		for (pt = descriptor_list; pt; pt = pt->next)
			if (!pt->connected && pt->character && pt->character != ch)
				send_to_char(buf, pt->character);
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else
			send_to_char(buf, ch);
	}
}


ACMD(do_poofset)
{
	char **msg;
	
	skip_spaces(&argument);
	
	if (IS_NPC(ch)) send_to_char("I tuoi messaggi poof vanno bene... vattene via.\r\n", ch);
	else if (strlen(argument) > MAX_POOF_LENGTH)
	{
		sprintf(buf, "I messaggi poofs non possono essere piu' lunghi di %d caratteri.\r\n", MAX_POOF_LENGTH);
		send_to_char(buf,ch);
	}
	
	else {
		switch (subcmd)
		{
			case SCMD_POOFIN:    msg = &(POOFIN(ch));    break;
			case SCMD_POOFOUT:   msg = &(POOFOUT(ch));   break;
			default:    return;    break;
		}
		if (*msg)   free(*msg);
		
		if (!*argument)   *msg = NULL;
		else *msg = str_dup(argument);
		
		send_to_char(OK, ch);
	}
}



ACMD(do_dc)
{
	struct descriptor_data *d;
	int num_to_dc;
	
	one_argument(argument, arg);
	if (!(num_to_dc = atoi(arg))) {
		send_to_char("Usage: DC <connection number> (type USERS for a list)\r\n", ch);
		return;
	}
	for (d = descriptor_list; d && d->desc_num != num_to_dc; d = d->next);
	
	if (!d) {
		send_to_char("No such connection.\r\n", ch);
		return;
	}
	if (d->character && GET_LEVEL(d->character) >= GET_LEVEL(ch)) {
		send_to_char("Umm.. maybe that's not such a good idea...\r\n", ch);
		return;
	}
	close_socket(d);
	sprintf(buf, "Connection #%d closed.\r\n", num_to_dc);
	send_to_char(buf, ch);
	sprintf(buf, "(GC) Connection closed by %s.", GET_NAME(ch));
	log(buf);
}



ACMD(do_wizlock)
{
	int value;
	char *when;
	
	one_argument(argument, arg);
	if (*arg) {
		value = atoi(arg);
		if (value < 0 || value > GET_LEVEL(ch)) {
			send_to_char("Invalid wizlock value.\r\n", ch);
			return;
		}
		restrict = value;
		when = "now";
	} else
		when = "currently";
	
	switch (restrict) {
		case 0:
			sprintf(buf, "The game is %s completely open.\r\n", when);
			break;
		case 1:
			sprintf(buf, "The game is %s closed to new players.\r\n", when);
			break;
		default:
			sprintf(buf, "Only level %d and above may enter the game %s.\r\n",
				restrict, when);
			break;
	}
	send_to_char(buf, ch);
}


ACMD(do_date)
{
	char *tmstr;
	time_t mytime;
	int d, h, m;
	extern time_t boot_time;
	
	if (subcmd == SCMD_DATE)
		mytime = time(0);
	else
		mytime = boot_time;
	
	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';
	
	if (subcmd == SCMD_DATE)
		sprintf(buf, "Current machine time: %s\r\n", tmstr);
	else {
		mytime = time(0) - boot_time;
		d = mytime / 86400;
		h = (mytime / 3600) % 24;
		m = (mytime / 60) % 60;
		
		sprintf(buf, "Up since %s: %d day%s, %d:%02d\r\n", tmstr, d,
			((d == 1) ? "" : "s"), h, m);
	}
	
	send_to_char(buf, ch);
}



ACMD(do_last)
{
	struct char_file_u chdata;
	extern char *class_abbrevs[];
	
	one_argument(argument, arg);
	if (!*arg) {
		send_to_char("For whom do you wish to search?\r\n", ch);
		return;
	}
	if (load_char(arg, &chdata) < 0) {
		send_to_char("There is no such player.\r\n", ch);
		return;
	}
	if ((chdata.level > GET_LEVEL(ch)) && (GET_LEVEL(ch) < LVL_IMPL)) {
		send_to_char("You are not sufficiently godly for that!\r\n", ch);
		return;
	}
	sprintf(buf, "[%5ld] [%2d %s] %-12s : %-18s : %-20s\r\n",
		chdata.char_specials_saved.idnum, (int) chdata.level,
		class_abbrevs[(int) chdata.class], chdata.name, chdata.host,
		ctime(&chdata.last_logon));
	send_to_char(buf, ch);
}


ACMD(do_force)
{
	struct descriptor_data *i, *next_desc;
	struct char_data *vict, *next_force;
	char to_force[MAX_INPUT_LENGTH + 2];
	
	half_chop(argument, arg, to_force);
	
	sprintf(buf1, "Una mano divina ti guida a '%s'.", to_force);
	
	if (!*arg || !*to_force)
		send_to_char("Whom do you wish to force do what?\r\n", ch);
	else if ((GET_LEVEL(ch) < LVL_GRGOD) || (str_cmp("all", arg) && str_cmp("room", arg))) {
		if (!(vict = get_char_vis(ch, arg)))
			send_to_char(NOPERSON, ch);
		else if (GET_LEVEL(ch) <= GET_LEVEL(vict))
			send_to_char("No, no, no!\r\n", ch);
		else {
			send_to_char(OK, ch);
			act(buf1, TRUE, ch, NULL, vict, TO_VICT);
			sprintf(buf, "(GC) %s forced %s to %s", GET_NAME(ch), GET_NAME(vict), to_force);
			mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
			command_interpreter(vict, to_force);
		}
	} else if (!str_cmp("room", arg)) {
		send_to_char(OK, ch);
		sprintf(buf, "(GC) %s forced room %d to %s", GET_NAME(ch), world[ch->in_room].number, to_force);
		mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
		
		for (vict = world[ch->in_room].people; vict; vict = next_force) {
			next_force = vict->next_in_room;
			if (GET_LEVEL(vict) >= GET_LEVEL(ch))
				continue;
			act(buf1, TRUE, ch, NULL, vict, TO_VICT);
			command_interpreter(vict, to_force);
		}
	} else { /* force all */
		send_to_char(OK, ch);
		sprintf(buf, "(GC) %s forced all to %s", GET_NAME(ch), to_force);
		mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
		
		for (i = descriptor_list; i; i = next_desc) {
			next_desc = i->next;
			
			if (i->connected || !(vict = i->character) || GET_LEVEL(vict) >= GET_LEVEL(ch))
				continue;
			act(buf1, TRUE, ch, NULL, vict, TO_VICT);
			command_interpreter(vict, to_force);
		}
	}
}



ACMD(do_wiznet)
{
	struct descriptor_data *d;
	char emote = FALSE;
	char any = FALSE;
	int level = LVL_BUILDER;
	
	skip_spaces(&argument);
	delete_doubledollar(argument);
	
	if (!*argument) {
		send_to_char("Usage: wiznet <text> | #<level> <text> | *<emotetext> |\r\n"
			"       wiznet @<level> *<emotetext> | wiz @\r\n", ch);
		return;
	}
	switch (*argument) {
		case '*':
			emote = TRUE;
		case '#':
			one_argument(argument + 1, buf1);
			if (is_number(buf1)) {
				half_chop(argument+1, buf1, argument);
				level = MAX(atoi(buf1), LVL_IMMORT);
				if (level > GET_LEVEL(ch)) {
					send_to_char("You can't wizline above your own level.\r\n", ch);
					return;
				}
			} else if (emote)
				argument++;
			break;
		case '@':
			for (d = descriptor_list; d; d = d->next) {
				if (!d->connected && GET_LEVEL(d->character) >= LVL_BUILDER &&
					!PRF_FLAGGED(d->character, PRF_NOWIZ) &&
					(CAN_SEE(ch, d->character) || GET_LEVEL(ch) == LVL_IMPL)) {
					if (!any) {
						sprintf(buf1, "Gods online:\r\n");
						any = TRUE;
					}
					sprintf(buf1, "%s  %s", buf1, GET_NAME(d->character));
					if (PLR_FLAGGED(d->character, PLR_WRITING))
						sprintf(buf1, "%s (Writing)\r\n", buf1);
					else if (PLR_FLAGGED(d->character, PLR_MAILING))
						sprintf(buf1, "%s (Writing mail)\r\n", buf1);
					else
						sprintf(buf1, "%s\r\n", buf1);
					
				}
			}
			any = FALSE;
			for (d = descriptor_list; d; d = d->next) {
				if (!d->connected && GET_LEVEL(d->character) >= LVL_BUILDER &&
					PRF_FLAGGED(d->character, PRF_NOWIZ) &&
					CAN_SEE(ch, d->character)) {
					if (!any) {
						sprintf(buf1, "%sGods offline:\r\n", buf1);
						any = TRUE;
					}
					sprintf(buf1, "%s  %s\r\n", buf1, GET_NAME(d->character));
				}
			}
			send_to_char(buf1, ch);
			return;
			break;
		case '\\':
			++argument;
			break;
		default:
			break;
	}
	if (PRF_FLAGGED(ch, PRF_NOWIZ)) {
		send_to_char("You are offline!\r\n", ch);
		return;
	}
	skip_spaces(&argument);
	
	if (!*argument) {
		send_to_char("Don't bother the gods like that!\r\n", ch);
		return;
	}
	if (level > LVL_BUILDER) {
		sprintf(buf1, "%s: <%d> %s%s\r\n", GET_NAME(ch), level,
			emote ? "<--- " : "", argument);
		sprintf(buf2, "Someone: <%d> %s%s\r\n", level, emote ? "<--- " : "",
			argument);
	} else {
		sprintf(buf1, "%s: %s%s\r\n", GET_NAME(ch), emote ? "<--- " : "",
			argument);
		sprintf(buf2, "Someone: %s%s\r\n", emote ? "<--- " : "", argument);
	}
	
	for (d = descriptor_list; d; d = d->next) {
		if ((!d->connected) && (GET_LEVEL(d->character) >= level) &&
			(!PRF_FLAGGED(d->character, PRF_NOWIZ)) &&
			(!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
			&& (d != ch->desc || !(PRF_FLAGGED(d->character, PRF_NOREPEAT)))) {
			send_to_char(CCCYN(d->character, C_NRM), d->character);
			if (CAN_SEE(d->character, ch))
				send_to_char(buf1, d->character);
			else
				send_to_char(buf2, d->character);
			send_to_char(CCNRM(d->character, C_NRM), d->character);
		}
	}
	
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
}




ACMD(do_godnet)
{
	struct descriptor_data *d;
	char emote = FALSE;
	char any = FALSE;
	int level = LVL_IMMORT;
	
	skip_spaces(&argument);
	delete_doubledollar(argument);
	
	if (!*argument) {
		send_to_char("Utilizzo: godnet <testo> | #<livello> <testo> | *<emote-testo> |\r\n"
			"          godnet @<livello> *<emote-testo> | god @\r\n", ch);
		return;
	}
	switch (*argument) {
		case '*':
			emote = TRUE;
		case '#':
			one_argument(argument + 1, buf1);
			if (is_number(buf1)) {
				half_chop(argument+1, buf1, argument);
				level = MAX(atoi(buf1), LVL_AVATAR);
				if (level > GET_LEVEL(ch)) {
					send_to_char("Non puoi parlare su Radio Dio al di sopra del tuo livello.\r\n", ch);
					return;
				}
			} else if (emote)
				argument++;
			break;
		case '@':
			for (d = descriptor_list; d; d = d->next) {
				if (!d->connected && GET_LEVEL(d->character) >= LVL_AVATAR &&
					!PRF_FLAGGED(d->character, PRF_NOWIZ) &&                       // Lascio qui nowiz, non e' il caso di sbattersi a fare un NOGOD
					(CAN_SEE(ch, d->character) || GET_LEVEL(ch) == LVL_IMPL)) {
					if (!any) {
						sprintf(buf1, "Dei in ascolto a Radio Dio:\r\n");
						any = TRUE;
						
					}
					sprintf(buf1+strlen(buf1), "  %s", GET_NAME(d->character));
					if (PLR_FLAGGED(d->character, PLR_WRITING))
						strcat(buf1, "(Writing)\r\n");
					else if (PLR_FLAGGED(d->character, PLR_MAILING))
						strcat(buf1, "(Writing mail)\r\n");
					else
						strcat(buf1, "\r\n");
					
				}
			}
			any = FALSE;
			for (d = descriptor_list; d; d = d->next) {
				if (!d->connected && GET_LEVEL(d->character) >= LVL_AVATAR &&
					PRF_FLAGGED(d->character, PRF_NOWIZ) &&
					CAN_SEE(ch, d->character)) {
					if (!any) {
						strcat(buf1, "Dei non in ascolto a Radio Dio:\r\n");
						any = TRUE;
					}
					sprintf(buf1+strlen(buf1), "  %s\r\n", GET_NAME(d->character));
				}
			}
			send_to_char(buf1, ch);
			return;
			break;
		case '\\':
			++argument;
			break;
		default:
			break;
	}
	if (PRF_FLAGGED(ch, PRF_NOWIZ)) {
		send_to_char("Non sei in ascolto a Radio Dio!\r\n", ch);
		return;
	}
	skip_spaces(&argument);
	
	if (!*argument) {
		send_to_char("Non disturbare gli dei in questo modo!\r\n", ch);
		return;
	}
	if (level > LVL_AVATAR) {
		sprintf(buf1, "%s: <%d> %s%s\r\n", GET_NAME(ch), level,
			emote ? "<--- " : "", argument);
		sprintf(buf2, "Qualcuno: <%d> %s%s\r\n", level, emote ? "<--- " : "",
			argument);
	} else {
		sprintf(buf1, "%s: %s%s\r\n", GET_NAME(ch), emote ? "<--- " : "",
			argument);
		sprintf(buf2, "Qualcuno: %s%s\r\n", emote ? "<--- " : "", argument);
	}
	
	for (d = descriptor_list; d; d = d->next) {
		if ((!d->connected) && (GET_LEVEL(d->character) >= level) &&
			(!PRF_FLAGGED(d->character, PRF_NOWIZ)) &&
			(!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
			&& (d != ch->desc || !(PRF_FLAGGED(d->character, PRF_NOREPEAT)))) {
			send_to_char(CCYEL(d->character, C_NRM), d->character);
			send_to_char(CCBLD(d->character, C_NRM), d->character);
			if (CAN_SEE(d->character, ch))
				send_to_char(buf1, d->character);
			else
				send_to_char(buf2, d->character);
			send_to_char(CCNRM(d->character, C_NRM), d->character);
		}
	}
	
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
}


ACMD(do_zreset)
{
	void reset_zone(int zone);
	
	int i, j;
	
	one_argument(argument, arg);
	if (!*arg) {
		send_to_char("You must specify a zone.\r\n", ch);
		return;
	}
	if (*arg == '*') {
		for (i = 0; i <= top_of_zone_table; i++)
			reset_zone(i);
		send_to_char("Reset world.\r\n", ch);
		sprintf(buf, "(GC) %s reset entire world.", GET_NAME(ch));
		mudlog(buf, NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE);
		return;
	} else if (*arg == '.')
		i = world[ch->in_room].zone;
	else {
		j = atoi(arg);
		for (i = 0; i <= top_of_zone_table; i++)
			if (zone_table[i].number == j)
				break;
	}
	if (i >= 0 && i <= top_of_zone_table) {
		reset_zone(i);
		sprintf(buf, "Reset zone %d (#%d): %s.\r\n", i, zone_table[i].number,
			zone_table[i].name);
		send_to_char(buf, ch);
		sprintf(buf, "(GC) %s reset zone %d (%s)", GET_NAME(ch), i, zone_table[i].name);
		mudlog(buf, NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE);
	} else
		send_to_char("Invalid zone number.\r\n", ch);
}


/*
 *  General fn for wizcommands of the sort: cmd <player>
 */

ACMD(do_wizutil)
{
	struct char_data *vict;
	struct affected_type *af, *next;
	long result;
	void roll_real_abils(struct char_data *ch);
	
	one_argument(argument, arg);
	
	if (!*arg)
		send_to_char("Yes, but for whom?!?\r\n", ch);
	else if (!(vict = get_char_vis(ch, arg)))
		send_to_char("There is no such player.\r\n", ch);
	else if (IS_NPC(vict))
		send_to_char("You can't do that to a mob!\r\n", ch);
	else if (GET_LEVEL(vict) > GET_LEVEL(ch))
		send_to_char("Hmmm...you'd better not.\r\n", ch);
	else {
		switch (subcmd) {
			case SCMD_REROLL:
				send_to_char("Rerolled...\r\n", ch);
				roll_real_abils(vict);
				sprintf(buf, "(GC) %s has rerolled %s.", GET_NAME(ch), GET_NAME(vict));
				log(buf);
				sprintf(buf, "New stats: Str %d/%d, Int %d, Wis %d, Dex %d, Con %d, Cha %d\r\n",
					GET_STR(vict), GET_ADD(vict), GET_INT(vict), GET_WIS(vict),
					GET_DEX(vict), GET_CON(vict), GET_CHA(vict));
				send_to_char(buf, ch);
				break;
			case SCMD_PARDON:
				if (!PLR_FLAGGED(vict, PLR_THIEF | PLR_KILLER| PLR_TKILLER
						|PLR_WAR_KILLER| PLR_WANTED)) {
					send_to_char("Your victim is not flagged.\r\n", ch);
					return;
				}
				REMOVE_BIT(PLR_FLAGS(vict), PLR_THIEF | PLR_KILLER | PLR_TKILLER
					| PLR_WAR_KILLER|PLR_WANTED);
				vict->player_specials->saved.num_thief=0;
				vict->player_specials->saved.num_tkills=0;
				vict->player_specials->saved.num_wanted=0;
				vict->player_specials->saved.num_warkills=0;
				vict->player_specials->saved.num_kills=0;
				send_to_char("Pardoned.\r\n", ch);
				send_to_char("You have been pardoned by the Gods!\r\n", vict);
				sprintf(buf, "(GC) %s pardoned by %s", GET_NAME(vict), GET_NAME(ch));
				mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
				break;
			case SCMD_NOTITLE:
				result = PLR_TOG_CHK(vict, PLR_NOTITLE);
				sprintf(buf, "(GC) Notitle %s for %s by %s.", ONOFF(result),
					GET_NAME(vict), GET_NAME(ch));
				mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
				strcat(buf, "\r\n");
				send_to_char(buf, ch);
				break;
			case SCMD_SQUELCH:
				result = PLR_TOG_CHK(vict, PLR_NOSHOUT);
				sprintf(buf, "(GC) Squelch %s for %s by %s.", ONOFF(result),
					GET_NAME(vict), GET_NAME(ch));
				mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
				strcat(buf, "\r\n");
				send_to_char(buf, ch);
				break;
			case SCMD_FREEZE: // comando disabilitato - Ardanos
				send_to_char("Il comando freeze e' disabilitato, per evitare perdite di eq.\r\n", ch);
				sprintf(buf,"Devi FAR CAMPARE il pg e poi usare il comando : 'set file %s freeze on'\r\n", arg);
				send_to_char(buf, ch);
				return;
				
				if (ch == vict) {
					send_to_char("Oh, yeah, THAT'S real smart...\r\n", ch);
					return;
				}
				if (PLR_FLAGGED(vict, PLR_FROZEN)) {
					send_to_char("Your victim is already pretty cold.\r\n", ch);
					return;
				}
				if (GET_LEVEL(vict) > GET_LEVEL(ch))
				  return;
				  
				SET_BIT(PLR_FLAGS(vict), PLR_FROZEN);
				GET_FREEZE_LEV(vict) = GET_LEVEL(ch);
				send_to_char("A bitter wind suddenly rises and drains every erg of heat from your body!\r\nYou feel frozen!\r\n", vict);
				send_to_char("Frozen.\r\n", ch);
				act("A sudden cold wind conjured from nowhere freezes $n!", FALSE, vict, 0, 0, TO_ROOM);
				sprintf(buf, "(GC) %s frozen by %s.", GET_NAME(vict), GET_NAME(ch));
				mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
				break;
			case SCMD_THAW:
				if (!PLR_FLAGGED(vict, PLR_FROZEN)) {
					send_to_char("Sorry, your victim is not morbidly encased in ice at the moment.\r\n", ch);
					return;
				}
				if (GET_FREEZE_LEV(vict) > GET_LEVEL(ch)) {
					sprintf(buf, "Sorry, a level %d God froze %s... you can't unfreeze %s.\r\n",
						GET_FREEZE_LEV(vict), GET_NAME(vict), HMHR(vict));
					send_to_char(buf, ch);
					return;
				}
				sprintf(buf, "(GC) %s un-frozen by %s.", GET_NAME(vict), GET_NAME(ch));
				mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
				REMOVE_BIT(PLR_FLAGS(vict), PLR_FROZEN);
				send_to_char("A fireball suddenly explodes in front of you, melting the ice!\r\nYou feel thawed.\r\n", vict);
				send_to_char("Thawed.\r\n", ch);
				act("A sudden fireball conjured from nowhere thaws $n!", FALSE, vict, 0, 0, TO_ROOM);
				break;
			case SCMD_UNAFFECT:
				if (vict->affected) {
					for (af = vict->affected; af; af = next){
						next = af->next;
						if (af->type <= MAX_SKILLS || af->type > TOP_SPELL_DEFINE)
							affect_remove(vict, af);
					}
					send_to_char("There is a brief flash of light!\r\n"
						"You feel slightly different.\r\n", vict);
					send_to_char("All spells removed.\r\n", ch);
					check_regen_rates(vict);
					if (IS_TRUST(ch)) {
						sprintf(buf, "(TRUST) %s unaffect %s.", GET_NAME(ch),GET_NAME(vict));
						mudlog(buf, CMP, LVL_GOD, TRUE);
					}
				} else {
					send_to_char("Your victim does not have any affections!\r\n", ch);
					return;
				}
				break;
			default:
				log("SYSERR: Unknown subcmd passed to do_wizutil (act.wizard.c)");
				break;
		}
		save_char(vict, NOWHERE);
	}
}


/* single zone printing fn used by "show zone" so it's not repeated in the
   code 3 times ... -je, 4/6/93 */

void print_zone_to_buf(char *bufptr, int zone)
{
	sprintf(bufptr, "%s%5d %-30.30s Age: %3d; Reset: %3d (%1d); Top: %5d\r\n",
		bufptr, zone_table[zone].number, zone_table[zone].name,
		zone_table[zone].age, zone_table[zone].lifespan,
		zone_table[zone].reset_mode, zone_table[zone].top);
}


ACMD(do_show)
{
	struct char_file_u vbuf;
	int i, j, k, l, con;
	char self = 0;
	struct char_data *vict;
	struct obj_data *obj;
	char field[MAX_INPUT_LENGTH], value[MAX_INPUT_LENGTH], birth[80];
	extern char *class_abbrevs[];
	extern char *genders[];
	extern int buf_switches, buf_largecount, buf_overflows;
	void show_shops(struct char_data * ch, char *value);
	void hcontrol_list_houses(struct char_data *ch);
	char buf[50000];
	
	struct show_struct {
		char *cmd;
		char level;
	} fields[] = {
		{ "nothing",	0  },				/* 0 */
		{ "zones",		LVL_IMMORT },			/* 1 */
		{ "player",		LVL_GOD },
		{ "rent",		  LVL_GOD },
		{ "stats",		LVL_IMMORT },
		{ "errors",		LVL_IMPL },			/* 5 */
		{ "death",		LVL_GOD },
		{ "godrooms",	LVL_GOD },
		{ "shops",		LVL_GOD },
		{ "houses",		LVL_GOD },
		{ "arena", 		LVL_IMMORT },
		{ "savemob", 	LVL_GOD },
		{ "\n", 0 }
	};
	
	skip_spaces(&argument);
	
	if (!*argument) {
		strcpy(buf, "Show options:\r\n");
		for (j = 0, i = 1; fields[i].level; i++)
			if (fields[i].level <= GET_LEVEL(ch))
				sprintf(buf, "%s%-15s%s", buf, fields[i].cmd, (!(++j % 5) ? "\r\n" : ""));
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
		return;
	}
	
	strcpy(arg, two_arguments(argument, field, value));
	
	for (l = 0; *(fields[l].cmd) != '\n'; l++)
		if (!strncmp(field, fields[l].cmd, strlen(field)))
			break;
		
	if (GET_LEVEL(ch) < fields[l].level) {
		send_to_char("You are not godly enough for that!\r\n", ch);
		return;
	}
	if (!strcmp(value, "."))
		self = 1;
	buf[0] = '\0';
	switch (l) {
		case 1:			/* zone */
    /* tightened up by JE 4/6/93 */
			if (self)
				print_zone_to_buf(buf, world[ch->in_room].zone);
			else if (*value && is_number(value)) {
				for (j = atoi(value), i = 0; zone_table[i].number != j && i <= top_of_zone_table; i++);
				if (i <= top_of_zone_table)
					print_zone_to_buf(buf, i);
				else {
					send_to_char("Non e' una zona valida.\r\n", ch);
					return;
				}
			} else
				for (i = 0; i <= top_of_zone_table; i++)
					print_zone_to_buf(buf, i);
			page_string(ch->desc,buf,1);
			//send_to_char(buf, ch);
			break;
		case 2:			/* player */
			if (!*value) {
				send_to_char("A name would help.\r\n", ch);
				return;
			}
			
			if (load_char(value, &vbuf) < 0) {
				send_to_char("There is no such player.\r\n", ch);
				return;
			}
			sprintf(buf, "Player: %-12s (%s) [%2d %s]\r\n", vbuf.name,
				genders[(int) vbuf.sex], vbuf.level, class_abbrevs[(int) vbuf.class]);
			sprintf(buf,
				"%sAu: %-8d  Bal: %-8d  Exp: %-8d  Align: %-5d  Lessons: %-3d\r\n",
				buf, vbuf.points.gold, vbuf.points.bank_gold, vbuf.points.exp,
				vbuf.char_specials_saved.alignment,
				vbuf.player_specials_saved.spells_to_learn);
			strcpy(birth, ctime(&vbuf.birth));
			sprintf(buf,
				"%sStarted: %-20.16s  Last: %-20.16s  Played: %3dh %2dm\r\n",
				buf, birth, ctime(&vbuf.last_logon), (int) (vbuf.played / 3600),
				(int) (vbuf.played / 60 % 60));
			send_to_char(buf, ch);
			break;
		case 3:
			Crash_listrent(ch, value);
			break;
		case 4:
			i = 0;
			j = 0;
			k = 0;
			con = 0;
			for (vict = character_list; vict; vict = vict->next) {
				if (IS_NPC(vict))
					j++;
				else if (CAN_SEE(ch, vict)) {
					i++;
					if (vict->desc)
						con++;
				}
			}
			for (obj = object_list; obj; obj = obj->next)
				k++;
			sprintf(buf, "Current stats:\r\n");
			sprintf(buf, "%s  %5d players in game  %5d connected\r\n", buf, i, con);
			sprintf(buf, "%s  %5d registered\r\n", buf, top_of_p_table + 1);
			sprintf(buf, "%s  %5d mobiles          %5d prototypes\r\n",
				buf, j, top_of_mobt + 1);
			sprintf(buf, "%s  %5d objects          %5d prototypes\r\n",
				buf, k, top_of_objt + 1);
			sprintf(buf, "%s  %5d rooms            %5d zones\r\n",
				buf, top_of_world + 1, top_of_zone_table + 1);
			sprintf(buf, "%s  %5d large bufs\r\n", buf, buf_largecount);
			sprintf(buf, "%s  %5d buf switches     %5d overflows\r\n", buf,
				buf_switches, buf_overflows);
			send_to_char(buf, ch);
			break;
		case 5:
			strcpy(buf, "Errant Rooms\r\n------------\r\n");
			for (i = 0, k = 0; i <= top_of_world; i++)
				for (j = 0; j < NUM_OF_DIRS; j++)
					if (world[i].dir_option[j] && world[i].dir_option[j]->to_room == 0)
						sprintf(buf, "%s%2d: [%5d] %s\r\n", buf, ++k, world[i].number,
							world[i].name);
			send_to_char(buf, ch);
			break;
		case 6:
			strcpy(buf, "Death Traps\r\n-----------\r\n");
			for (i = 0, j = 0; i <= top_of_world; i++)
				if (IS_SET(ROOM_FLAGS(i), ROOM_DEATH))
					sprintf(buf, "%s%2d: [%5d] %s\r\n", buf, ++j,
						world[i].number, world[i].name);
			send_to_char(buf, ch);
			break;
		case 7:
#define GOD_ROOMS_ZONE 2
			strcpy(buf, "Godrooms\r\n--------------------------\r\n");
			for (i = 0, j = 0; i < top_of_world; i++)
				if (world[i].zone == GOD_ROOMS_ZONE)
					sprintf(buf, "%s%2d: [%5d] %s\r\n", buf, j++, world[i].number,
						world[i].name);
			send_to_char(buf, ch);
			break;
		case 8:
			show_shops(ch, value);
			break;
		case 9:
			hcontrol_list_houses(ch);
			break;
		case 10:      /* arena */
			if (in_arena == ARENA_OFF) {
				sprintf(buf, "The Arena is closed right now.\r\n");
			} else if (in_arena == ARENA_START) {
				sprintf(buf, "Arena will start in %d hour(s)\r\n", time_to_start);
				sprintf(buf, "%sIt will last for %d hour(s)\r\n", buf, game_length);
			} else if (in_arena == ARENA_RUNNING) {
				sprintf(buf, "Arena will end in %d hour(s)\r\n", time_left_in_game);
			}
			send_to_char(buf, ch);
			break;
		case 11:
			sprintf(buf, " ID   Level    Vnum  Save inv. Sav Del  Eq   Nome                   \r\n");
			send_to_char(buf, ch);
			sprintf(buf, "-----------------------------------------------------------\r\n");
			send_to_char(buf, ch);
			for (vict = character_list; vict; vict =  vict->next) {
				if ((vict) && IS_MOB(vict) && MOB_FLAGGED(vict, MOB_SAVE))
				{
					sprintf(buf,"%3d    %2d  %8d  %2s        %2s  %2s   %2s  %s   \r\n",
						GET_MOB_ID(vict)==0 ? -1 : (int)GET_MOB_ID(vict),
						(int)  GET_LEVEL(vict),(int)GET_MOB_VNUM(vict),
						(int) MOB_FLAGGED(vict,MOB_SAVEINV) ? "SI":"NO",(int) MOB_FLAGGED(vict,MOB_CRASH) ? "SI":"NO",
						(int) MOB_FLAGGED(vict,MOB_SAVE_DELETE) ? "SI":"NO", (int) MOB_FLAGGED(vict,MOB_CRASH_EQ) ? "SI":"NO", GET_NAME(vict));
					send_to_char(buf, ch);
				}
			}
			break;
		default:
			send_to_char("Sorry, I don't understand that.\r\n", ch);
			break;
	}
}


#define PC   1
#define NPC  2
#define BOTH 3

#define MISC	0
#define BINARY	1
#define NUMBER	2

#define SET_OR_REMOVE(flagset, flags) { \
	if (on) SET_BIT(flagset, flags); \
	else if (off) REMOVE_BIT(flagset, flags); }

#define RANGE(low, high) ( value = MAX((low), MIN((high), (value))) )


ACMD(do_set)
{
	int i, l;
	
	struct char_data *vict = NULL, *cbuf = NULL, *vict2 = NULL;
	struct char_file_u tmp_store;
	char field[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH], val_arg[MAX_INPUT_LENGTH];
	int on = 0, off = 0, value = 0;
	char is_file = 0, is_mob = 0, is_player = 0, is_id = 0;
	int player_i = 0;
	int parse_class(char arg);
	char *oldname;
	int j;
	//char arg2[MAX_STRING_LENGTH];
	char tmp_name[MAX_STRING_LENGTH];
	
	struct set_struct {
		char *cmd;
		char level;
		char pcnpc;
		char type;
	}          fields[] = {
		{ "brief",		LVL_GOD, 	PC, 	BINARY },  /* 0 */
		{ "invstart", 	LVL_GOD, 	PC, 	BINARY },  /* 1 */
		{ "title",		LVL_GOD, 	PC, 	MISC },
		{ "nosummon", 	LVL_GRGOD, 	PC, 	BINARY },
		{ "maxhit",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "maxmana", 	LVL_GRGOD, 	BOTH, 	NUMBER },  /* 5 */
		{ "maxmove", 	LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "hit", 		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "mana",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "move",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "align",		LVL_GOD, 	BOTH, 	NUMBER },  /* 10 */
		{ "str",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "stradd",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "int", 		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "wis", 		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "dex", 		LVL_GRGOD, 	BOTH, 	NUMBER },  /* 15 */
		{ "con", 		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "sex", 		LVL_GRGOD, 	BOTH, 	MISC },
		{ "ac", 		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "gold",		LVL_GOD, 	BOTH, 	NUMBER },
		{ "bank",		LVL_GOD, 	BOTH, 	NUMBER },  /* 20 */
		{ "exp", 		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "hitroll", 	LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "damroll", 	LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "invis",		LVL_IMPL, 	PC, 	NUMBER },
		{ "nohassle", 	LVL_GRGOD, 	PC, 	BINARY },  /* 25 */
		{ "frozen",		LVL_FREEZE, 	PC, 	BINARY },
		{ "practices", 	LVL_GRGOD, 	PC, 	NUMBER },
		{ "lessons", 	LVL_GRGOD, 	PC, 	NUMBER },
		{ "drunk",		LVL_GRGOD, 	BOTH, 	MISC },
		{ "hunger",		LVL_GRGOD, 	BOTH, 	MISC },    /* 30 */
		{ "thirst",		LVL_GRGOD, 	BOTH, 	MISC },
		{ "killer",		LVL_GOD, 	PC, 	BINARY },
		{ "thief",		LVL_GOD, 	PC, 	BINARY },
		{ "level",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "room",		LVL_IMPL, 	BOTH, 	NUMBER },  /* 35 */
		{ "roomflag", 	LVL_GRGOD, 	PC, 	BINARY },
		{ "siteok",		LVL_GRGOD, 	PC, 	BINARY },
		{ "deleted", 	LVL_GRGOD, 	PC, 	BINARY },
		{ "class",		LVL_GRGOD, 	BOTH, 	MISC },
		{ "nowizlist", 	LVL_GOD, 	PC, 	BINARY },  /* 40 */
		{ "quest",		LVL_GOD, 	PC, 	BINARY },
		{ "loadroom", 		LVL_GRGOD, 	BOTH, 	MISC },
		{ "color",		LVL_GOD, 	PC, 	BINARY },
		{ "idnum",		LVL_IMPL, 	PC, 	NUMBER },
		{ "passwd",		LVL_GRGOD, 	PC, 	MISC },    /* 45 */
		{ "nodelete", 	LVL_GOD, 	PC, 	BINARY },
		{ "cha",		LVL_GRGOD, 	BOTH, 	NUMBER },
		{ "olc",		LVL_GOD, 	PC, 	NUMBER },
		{ "questor",		LVL_GOD, 	PC, 	BINARY },
		{ "hometown", 		LVL_GRGOD, 	BOTH, 	NUMBER },  /* 50 */
		{ "wildxrange",      	LVL_GOD,        PC,     NUMBER },
		{ "wildyrange",      	LVL_GOD,        PC,     NUMBER },
		{ "notorieta",          LVL_GRGOD,      PC,     NUMBER },
		{ "noeroe",          	LVL_GOD,        PC,     BINARY },
		{ "gdr",             	LVL_IMMORT,     PC,     BINARY },  /* 55 */
		{ "fama",       	LVL_GRGOD,      PC,     NUMBER },
		{ "abilpracs", 		LVL_GRGOD, 	PC, 	NUMBER },
		{ "classtitle",      	LVL_QUEST_MASTER,        PC,     NUMBER },
		{ "statosociale",    	LVL_GOD,        PC,     NUMBER },
		{ "ministro",       	LVL_GOD,    	PC,     BINARY },  /* 60 */
		{ "multiplayer",       	LVL_GRGOD,      	PC,     BINARY },
		{ "stipendio",       	LVL_GOD,      	NPC,    NUMBER },
		{ "paga",       	LVL_GOD,      	NPC,    NUMBER },
		{ "padrone",       	LVL_GOD,      	NPC,    MISC },
		{ "mobsave",       	LVL_GOD,      	NPC,    BINARY },  /* 65 */
		{ "mobsaveinventario", 	LVL_GOD,      	NPC,    BINARY },
		{ "name",		LVL_IMPL, 	PC, 	MISC }, 
		{ "sneakview",		LVL_GOD,	PC,	BINARY },
		{ "holylight",		LVL_GOD,	PC,	BINARY },
		{ "position",		LVL_GOD,	PC,	NUMBER },  /* 70 */
		{ "questpoints",       	LVL_GRGOD,      PC,     NUMBER }, //Orione
		{ "fantasma",           LVL_QUEST_MASTER,PC,    BINARY },
		{ "resuscita",          LVL_GOD,        PC,     NUMBER },
		{ "morti",		LVL_IMPL,	PC,	NUMBER }, //Orione
		{ "mute",		LVL_IMPL,	PC,	BINARY },
		{ "regno",		LVL_IMPL,	PC,	NUMBER},
		{ "rank",		LVL_IMPL,	PC,	NUMBER},
		{ "olccompleto",	LVL_GOD,	PC,	BINARY },
		{ "peste",		LVL_GOD,	PC,	NUMBER},
		{ "assassinato",	LVL_GOD,	PC,	NUMBER},
		{ "\n", 		0, 	      	BOTH, 	MISC }
	};
	
	half_chop(argument, name, buf);
	if (!strcmp(name, "file")) {
		is_file = 1;
		half_chop(buf, name, buf);
	} else if (!str_cmp(name, "player")) {
		is_player = 1;
		half_chop(buf, name, buf);
	} else if (!str_cmp(name, "mob")) {
		is_mob = 1;
		half_chop(buf, name, buf);
	} else if (!str_cmp(name, "id")) {
		is_id = 1;
		half_chop(buf, name, buf);
	}
	half_chop(buf, field, buf);
	strcpy(val_arg, buf);
	
	if (!*name || !*field) {
		send_to_char("Usage: set <vittima> <caratteristica> <valore>\r\n", ch);
		return;
	}
	
	if (!is_file) {
		
		if (is_player) {
			if (!(vict = get_player_vis(ch, name, 0))) {
				send_to_char("Non c'e' questo giocatore.\r\n", ch);
				return;
			}
		}  
		
		else if (is_id) {
			
			value = atoi(name);
			//strcpy(name,get_name_by_id(value));
			if (get_name_by_id(value)==NULL) {
				send_to_char("Non c'e' questo giocatore.\r\n", ch);
				return;
			}
			CREATE(cbuf, struct char_data, 1);
			clear_char(cbuf);
			if ((player_i = load_char(get_name_by_id(value), &tmp_store)) > -1) {
				store_to_char(&tmp_store, cbuf);
				if (GET_LEVEL(cbuf) >= GET_LEVEL(ch) && strcmp(GET_NAME(ch), "Phantom")) {
					free_char(cbuf);
					send_to_char("Non puoi farlo.\r\n", ch);
					return;
				}
				vict = cbuf;
			} 
			else {
				free(cbuf);
				send_to_char("Non c'e' questo giocatore.\r\n", ch);
				return;
			}	
		} else {
			if (!(vict = get_char_vis(ch, name))) {
				send_to_char("Non c'e' questa creatura.\r\n", ch);
				return;
			}
		}
	} 
	
	else if (is_file) {
		CREATE(cbuf, struct char_data, 1);
		clear_char(cbuf);
		
		if ((player_i = load_char(name, &tmp_store)) > -1) {
			store_to_char(&tmp_store, cbuf);
			if (GET_LEVEL(cbuf) >= GET_LEVEL(ch)) {
				free_char(cbuf);
				send_to_char("Non puoi farlo.\r\n", ch);
				return;
			}
			vict = cbuf;
		} 
		else {
			free(cbuf);
			send_to_char("Non c'e' questo giocatore.\r\n", ch);
			return;
		}
	}
	if (GET_LEVEL(ch) != LVL_IMPL) {
		if (!IS_NPC(vict) && GET_LEVEL(ch) <= GET_LEVEL(vict) && vict != ch) {
			send_to_char("Non e' una bella idea...\r\n", ch);
			return;
		}
	}
	for (l = 0; *(fields[l].cmd) != '\n'; l++)
		if (!strncmp(field, fields[l].cmd, strlen(field)))
			break;
		
	if (GET_LEVEL(ch) < fields[l].level) {
		send_to_char("Non sei un Dio abbastanza potente per fare cio'!\r\n", ch);
		return;
	}
/* Orione: aggiunto il controllo sull'opzione specificata
 * (solo le 3 seguenti: questpoints, notorieta', fama)
 * i valori 53,56 e 71 si riferiscono al numero di elemento nella set_struct
*/   	if (IS_NPC(ch)) {
		if (!(l == 53 || l == 56 || l == 71)) {
	 		send_to_char("Non puoi impostare quel valore!\r\n", ch);
  			return;
		}
  	}
	
	if (IS_NPC(vict) && !(fields[l].pcnpc & NPC)) {
		send_to_char("Non puoi farlo su un MOB!\r\n", ch);
		return;
	} else if (!IS_NPC(vict) && !(fields[l].pcnpc & PC)) {
		send_to_char("Puoi farlo solo su un MOB!\r\n", ch);
		return;
	}
	if (fields[l].type == BINARY) {
		if (!strcmp(val_arg, "on") || !strcmp(val_arg, "yes") || !strcmp(val_arg, "si"))
			on = 1;
		else if (!strcmp(val_arg, "off") || !strcmp(val_arg, "no"))
			off = 1;
		if (!(on || off)) {
			send_to_char("Valore deve essere ON o OFF.\r\n", ch);
			return;
		}
	} 
	else if (fields[l].type == NUMBER) {
	  if (!is_number(val_arg)) {
	    send_to_char("Valore deve essere un numero.\r\n", ch);
			return;
	  }
	    
		value = atoi(val_arg);
	}
	
	strcpy(buf, "Okay.");  /* can't use OK macro here 'cause of \r\n */
	switch (l) {
		case 0:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_BRIEF);
			break;
		case 1:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_INVSTART);
			break;
		case 2:
			set_title(vict, val_arg);
			sprintf(buf, "%s's title is now: %s", GET_NAME(vict), GET_TITLE(vict));
			break;
		case 3:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_SUMMONABLE);
			on = !on;			/* so output will be correct */
			break;
		case 4:
			vict->points.max_hit = RANGE(1, 5000);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 5:
			vict->points.max_mana = RANGE(1, 5000);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 6:
			vict->points.max_move = RANGE(1, 5000);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 7:
			vict->points.hit = RANGE(-9, vict->points.max_hit);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 8:
			vict->points.mana = RANGE(0, vict->points.max_mana);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 9:
			vict->points.move = RANGE(0, vict->points.max_move);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 10:
			GET_ALIGNMENT(vict) = RANGE(-1000, 1000);
			affect_total(vict);		/* affect_total() handles regen updates */
			break;
		case 11:
			if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
				RANGE(3, 25);
			else
				RANGE(3, 18);
			vict->real_abils.str = value;
			vict->real_abils.str_add = 0;
			affect_total(vict);
			break;
		case 12:
			vict->real_abils.str_add = RANGE(0, 100);
			if (value > 0)
				vict->real_abils.str = 18;
			affect_total(vict);
			break;
		case 13:
			if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
				RANGE(3, 25);
			else
				RANGE(3, 18);
			vict->real_abils.intel = value;
			affect_total(vict);
			break;
		case 14:
			if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
				RANGE(3, 25);
			else
				RANGE(3, 18);
			vict->real_abils.wis = value;
			affect_total(vict);
			break;
		case 15:
			if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
				RANGE(3, 25);
			else
				RANGE(3, 18);
			vict->real_abils.dex = value;
			affect_total(vict);
			break;
		case 16:
			if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
				RANGE(3, 25);
			else
				RANGE(3, 18);
			vict->real_abils.con = value;
			affect_total(vict);
			break;
		case 17:
			if (!str_cmp(val_arg, "male"))
				vict->player.sex = SEX_MALE;
			else if (!str_cmp(val_arg, "female"))
				vict->player.sex = SEX_FEMALE;
			else if (!str_cmp(val_arg, "neutral"))
				vict->player.sex = SEX_NEUTRAL;
			else {
				send_to_char("Must be 'male', 'female', or 'neutral'.\r\n", ch);
				return;
			}
			break;
		case 18:
			vict->points.armor = RANGE(-100, 100);
			affect_total(vict);
			break;
		case 19:
			GET_GOLD(vict) = RANGE(0, 100000000);
			break;
		case 20:
			GET_BANK_GOLD(vict) = RANGE(0, 100000000);
			break;
		case 21:
			vict->points.exp = RANGE(0, 999999999);
			break;
		case 22:
			vict->points.hitroll = RANGE(-20, 20);
			affect_total(vict);
			break;
		case 23:
			vict->points.damroll = RANGE(-20, 20);
			affect_total(vict);
			break;
		case 24:
			if (GET_LEVEL(ch) < LVL_IMPL && ch != vict) {
				send_to_char("You aren't godly enough for that!\r\n", ch);
				return;
			}
			GET_INVIS_LEV(vict) = RANGE(0, GET_LEVEL(vict));
			break;
		case 25:
			if (GET_LEVEL(ch) < LVL_IMPL && ch != vict) {
				send_to_char("You aren't godly enough for that!\r\n", ch);
				return;
			}
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_NOHASSLE);
			break;
		case 26:
			
			if (ch == vict) {
				send_to_char("Better not -- could be a long winter!\r\n", ch);
				return;
			}
			if(get_char_vis(ch, name) && on) { // Ardanos
				send_to_char("Il pg da freezare e' online o e' disconnesso. Bisogna farlo campare - &1Prevenzione anti perdita eq&0\r\n", ch);
				return;
			}
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_FROZEN);
			break;
		case 27:
		case 28:
			GET_PRACTICES(vict) = RANGE(0, 100);
			break;
		case 29:
		case 30:
		case 31:
			if (!str_cmp(val_arg, "off")) {
				GET_COND(vict, (l - 29)) = (char) -1;
				sprintf(buf, "%s's %s now off.", GET_NAME(vict), fields[l].cmd);
			} else if (is_number(val_arg)) {
				value = atoi(val_arg);
				RANGE(0, 96);
				GET_COND(vict, (l - 29)) = (char) value;
				sprintf(buf, "%s's %s set to %d.", GET_NAME(vict), fields[l].cmd,
					value);
			} else {
				send_to_char("Must be 'off' or a value from 0 to 96.\r\n", ch);
				return;
			}
			check_regen_rates(vict);
			break;
		case 32:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_KILLER);
			break;
		case 33:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_THIEF);
			break;
		case 34:
			if (value > GET_LEVEL(ch) || (value > LVL_IMPL && value != 500)) {
				send_to_char("You can't do that.\r\n", ch);
				return;
			}
			if (value == 500) value = 92;
			else RANGE(0, LVL_IMPL);
			vict->player.level = (byte) value;
			break;
		case 35:
			if ((i = real_room(value)) < 0) {
				send_to_char("No room exists with that number.\r\n", ch);
				return;
			}
			char_from_room(vict);
			char_to_room(vict, i);
			break;
		case 36:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_ROOMFLAGS);
			break;
		case 37:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_SITEOK);
			break;
		case 38:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_DELETED);
			break;
		case 39:
			if ((i = parse_class(*val_arg)) == CLASS_UNDEFINED) {
				send_to_char("That is not a class.\r\n", ch);
				return;
			}
			GET_CLASS(vict) = i;
			check_regen_rates(vict);
			break;
		case 40:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NOWIZLIST);
			break;
		case 41:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_QUEST);
			break;
		case 42:
			if (!str_cmp(val_arg, "off"))
				REMOVE_BIT(PLR_FLAGS(vict), PLR_LOADROOM);
			else if (is_number(val_arg)) {
				value = atoi(val_arg);
				if (real_room(value) != NOWHERE) {
					SET_BIT(PLR_FLAGS(vict), PLR_LOADROOM);
					GET_LOADROOM(vict) = value;
					sprintf(buf, "%s will enter at room #%d.", GET_NAME(vict),
						GET_LOADROOM(vict));
				} else {
					sprintf(buf, "That room does not exist!");
				}
			} else {
				strcpy(buf, "Must be 'off' or a room's virtual number.\r\n");
			}
			break;
		case 43:
			SET_OR_REMOVE(PRF_FLAGS(vict), (PRF_COLOR_1 | PRF_COLOR_2));
			break;
		case 44:
			if (GET_IDNUM(ch) != 1 || !IS_NPC(vict))
				return;
			GET_IDNUM(vict) = value;
			break;
		case 45:
			if (!is_file)
				return;
			/*if (GET_IDNUM(ch) > 1) {
				send_to_char("Please don't use this command, yet.\r\n", ch);
				return;
			}*/
			if (GET_LEVEL(vict) >= LVL_GRGOD) {
				send_to_char("You cannot change that.\r\n", ch);
				return;
			}
			strncpy(tmp_store.pwd, CRYPT(val_arg, tmp_store.name), MAX_PWD_LENGTH);
			tmp_store.pwd[MAX_PWD_LENGTH] = '\0';
			sprintf(buf, "Password changed to '%s'.", val_arg);
			break;
		case 46:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NODELETE);
			break;
		case 47:
			if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
				RANGE(3, 25);
			else
				RANGE(3, 18);
			vict->real_abils.cha = value;
			affect_total(vict);
			break;
		case 48:
			GET_OLC_ZONE(vict) = value;
			break;
		case 49:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_QUESTOR);
			break;
		case 50:
			if (!IS_NPC(vict)) {
				RANGE(0, NUM_STARTROOMS);
				GET_HOME(vict) = value;
				send_to_char("NUM   ROOM     ZONE\r\n", ch);
				send_to_char(" 1)   3001     Deira\r\n", ch);
				send_to_char(" 2)   5401     Jiroch\r\n", ch);
				send_to_char(" 3)   9700     Cimmura\r\n", ch);
				send_to_char(" 4)   21208    Cyrellos\r\n", ch);
				send_to_char(" 5)   6601     Sundhaven\r\n", ch);
				send_to_char(" 6)   50044    Alorn\r\n", ch);
			} else {
				if ((real_room(value)) < 0) {
					send_to_char("No room exists with that number.\r\n", ch);
					return;
				}
				GET_HOME(vict) = real_room(value);
			}
			break;
		case 51:
			RANGE(1, 20);
			GET_WILD_MAX_X_RANGE(vict)=value;
			break;
		case 52:
			RANGE(1, 20);
			GET_WILD_MAX_Y_RANGE(vict)=value;
			break;
		case 53:
			RANGE(0, 1000);
			GET_FAMA(vict) = value;
			break;
		case 54:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_NO_EROE);
			break;
		case 55:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_RPG);
			break;
		case 56:
			RANGE(0, 1000);
			GET_NOTORIETA(vict) = value;
			break;
		case 57:
			GET_ABIL_PRACS(vict) = RANGE(0, 100);
			break;
		case 58:
			GET_CLASS_TITLE(vict) = RANGE(1,3)-1; // DI FATTO VARIA TRA 0 E 2
			break;
		case 59:
			GET_STATO_SOCIALE(vict) = RANGE(1,20)-1; // DI FATTO VARIA TRA 0 E 19
			break;
		case 60:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NOTAIO);
			break;
		case 61:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_MULTIPLAYER);
			break;	
		case 62:
			if (value<0) {
				send_to_char("Lo stipendio non puo' essere negativo!\r\n",ch);
				return;
			}
			GET_STIPENDIOMOB(vict) = value;
			if (value>0)
				SET_BIT(MOB_FLAGS(vict),MOB_STIPENDIO);
			else
				REMOVE_BIT(MOB_FLAGS(vict),MOB_STIPENDIO);
			break;
		case 63:
			if (value< (-LICENZIAMENTO_MANOVALE)) {
				sprintf(buf,"L'ora paga non puo' essere minore di %d!\r\n",(-LICENZIAMENTO_MANOVALE));
				send_to_char(buf,ch);
				return;
			}
			GET_PAGA(vict) = value;								
			break;
		case 64:
			if (!strcmp(val_arg,"nessuno")) {
				MASTER_ID(vict) = 0;
				break;
			}
			vict2 = get_char_room_vis(ch, val_arg);
			
			if (!vict2) {
				send_to_char("Non lo vedi.\r\n",ch);
				return;
			}
			
			
			MASTER_ID(vict)=GET_IDNUM(vict2);
			CLAN_ID(vict)=GET_CLAN(vict2);
			sprintf(buf1,"Diventi il padrone di %s.\r\n",GET_NAME(vict));
			send_to_char(buf1,vict2);
			sprintf(buf1,"%s diventa il padrone di %s.\r\n",GET_NAME(vict2),GET_NAME(vict));
			send_to_char(buf1,ch);
			break;
		case 65:
			SET_OR_REMOVE(MOB_FLAGS(vict), MOB_SAVE);
			break;
		case 66:
			SET_OR_REMOVE(MOB_FLAGS(vict), MOB_SAVEINV);
			break;
		case 67:
			
			if (find_name(val_arg)>-1)  {
				send_to_char("There's already somebody with this name.\r\n",ch);
				return;
			}
			
			if ((_parse_name(val_arg, tmp_name))||strlen(tmp_name) < 2 ||
				strlen(tmp_name) > MAX_NAME_LENGTH || !Valid_Name(tmp_name) ||
				fill_word(tmp_name) || reserved_word(tmp_name)) {
				send_to_char("Illegal name, retry.\r\n", ch);
				return;
			}
			
			for (i = 0; i <= top_of_p_table; i++)
				if (player_table[i].id == GET_IDNUM(vict))
					break;
				
			if (player_table[i].id != GET_IDNUM(vict)){
				send_to_char("BUG: error in player table!!!!",ch);
				sprintf(buf,"BUG: error in player table for %s with idnum %ld",
					GET_NAME(ch),GET_IDNUM(ch));
				log(buf);
				return;
			}
			
			oldname = strdup(GET_NAME(vict));
			
    //player_table[i].name = strdup(arg2);
			for (j = 0; (*(player_table[i].name + j) = LOWER(*(val_arg + j))); j++);
			
			strcpy(vict->player.name, CAP(val_arg));  
			
			break;
		case 68:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_HIDE_SNEAK);
			break;
		case 69:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_HOLYLIGHT);
			break;
		case 70:
			if ((value >= 0)&&(value <= POS_STANDING)) {
				GET_POS(vict) = value;
			} else {
				send_to_char("NUM   POSITION\r\n", ch);
				send_to_char(" 0)	Morto\r\n", ch);
				send_to_char(" 1)	Ferito Mortalmente\r\n", ch);
				send_to_char(" 2)	Incapacitato\r\n", ch);
				send_to_char(" 3)	Stordito\r\n", ch);
				send_to_char(" 4)	Addormentato\r\n", ch);
				send_to_char(" 5)	Riposo\r\n", ch);
				send_to_char(" 6)	Seduto\r\n", ch);
				send_to_char(" 7)	In Combattimento\r\n", ch);
				send_to_char(" 8)	Inpiedi\r\n", ch);
			}
			break;
		case 71: //Orione
			RANGE(0, 1000);
			GET_QUESTPOINTS(vict) = value;
			break;
		case 72://PEPPE FANTASMA
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_FANTASMA);
			if(PLR_FLAGGED(vict,PLR_FANTASMA)){
			    SET_BIT(PRF_FLAGS(vict),PRF_NOHASSLE);
				SET_BIT(PRF_FLAGS(vict),PRF_HOLYLIGHT);
			    GET_RESUSCITA(vict)=0;
			    }
			else{
			    if(GET_LEVEL(vict)<LVL_IMMORT){
				REMOVE_BIT(PRF_FLAGS(vict),PRF_NOHASSLE);
				REMOVE_BIT(PRF_FLAGS(vict),PRF_HOLYLIGHT);
			    }
			    GET_RESUSCITA(vict)=0;
			    }    
			break;	
		case 73:
			RANGE(0,1000);
			GET_RESUSCITA(vict)=value+1;
			break;
		case 74:
			GET_RIP_CNT(vict) = value;
			vict->player_specials->saved.morti_per_livello = value;
			break;
		case 75:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NOSHOUT);
			break;
		case 76:
			RANGE(1,15);
			GET_CLAN(vict)=value;
			GET_CLAN_RANK(vict)=1;
			break;
		case 77:
			RANGE(1,30);
			GET_CLAN_RANK(vict)=value;
			break;
		case 78:
			SET_OR_REMOVE(PLR_FLAGS(vict), PLR_ALL_OLC);
			break;
		case 79:
			GET_PESTE(vict) = RANGE(0, 239);
			break;
		case 80:
			GET_MORTI_DA_PG(vict) = RANGE(0, 4);
			break;
		default:
			sprintf(buf, "Non puoi settare quello !");
			return;
	}
	
	if (IS_NPC(vict))
		SET_BIT(MOB_FLAGS(vict),MOB_CRASH);
	
	if (fields[l].type == BINARY) {
		sprintf(buf, "%s %s for %s.\r\n", fields[l].cmd, ONOFF(on),
			GET_NAME(vict));
		CAP(buf);
	} else if (fields[l].type == NUMBER) {
		sprintf(buf, "%s's %s set to %d.\r\n", GET_NAME(vict),
			fields[l].cmd, value);
	} else
		strcat(buf, "\r\n");
	send_to_char(CAP(buf), ch);
	
	//Log del Set by Chardhros
	if (on==1) {
		sprintf(buf1,"(GC) %s ha settato %s di %s a ON",GET_NAME(ch),fields[l].cmd,GET_NAME(vict));	
	} else
	if (off==1) {
		sprintf(buf1,"(GC) %s ha settato %s di %s a OFF",GET_NAME(ch),fields[l].cmd,GET_NAME(vict));	
	} else if (fields[l].type==NUMBER)
		sprintf(buf1,"(GC) %s ha settato %s di %s a %d",GET_NAME(ch),fields[l].cmd,GET_NAME(vict),value);
	else if (fields[l].type==MISC)
		sprintf(buf1,"(GC) %s ha settato %s di %s a %s",GET_NAME(ch),fields[l].cmd,GET_NAME(vict),val_arg);
	mudlog(buf1,NRM,LVL_GOD,TRUE);
	
	if (!is_file && !IS_NPC(vict))
		save_char(vict, NOWHERE);
	
	if (is_file||is_id) {
		char_to_store(vict, &tmp_store);
		fseek(player_fl, (player_i) * sizeof(struct char_file_u), SEEK_SET);
		fwrite(&tmp_store, sizeof(struct char_file_u), 1, player_fl);
		free_char(cbuf);
		send_to_char("Saved in file.\r\n", ch);		
	}
}


const char *logtypes[] = {
	"off", "brief", "normal", "complete", "\n"};

ACMD(do_syslog)
{
	int tp;
	
	one_argument(argument, arg);
	
	if (!*arg) {
		tp = ((PRF_FLAGGED(ch, PRF_LOG1) ? 1 : 0) +
				(PRF_FLAGGED(ch, PRF_LOG2) ? 2 : 0));
		sprintf(buf, "Your syslog is currently %s.\r\n", logtypes[tp]);
		send_to_char(buf, ch);
		return;
	}
	if (((tp = search_block(arg, logtypes, FALSE)) == -1)) {
		send_to_char("Usage: syslog { Off | Brief | Normal | Complete }\r\n", ch);
		return;
	}
	REMOVE_BIT(PRF_FLAGS(ch), PRF_LOG1 | PRF_LOG2);
	SET_BIT(PRF_FLAGS(ch), (PRF_LOG1 * (tp & 1)) | (PRF_LOG2 * (tp & 2) >> 1));
	
	sprintf(buf, "Your syslog is now %s.\r\n", logtypes[tp]);
	send_to_char(buf, ch);
}

ACMD(do_tedit) {
	int l, i;
	char field[MAX_INPUT_LENGTH];
	extern char *credits;
	extern char *news;
	extern char *motd;
	extern char *imotd;
	extern char *help;
	extern char *info;
	extern char *background;
	extern char *handbook;
	extern char *policies;
	extern char *pray;
	extern char *regni;
	
#ifndef CIRCLE_WINDOWS          //CARLO CIRCLE WINDOWS
	
	struct editor_struct {
		char *cmd;
		char level;
		char *buffer;
		int  size;
		char *filename;
	} fields[] = {
       /* edit the lvls to your own needs */
		{ "credits",	LVL_IMPL,	credits,	2400,	CREDITS_FILE},
		{ "news",	LVL_GRGOD,	news,		8192,	NEWS_FILE},
		{ "motd",	LVL_GRGOD,	motd,		8192,	MOTD_FILE},
		{ "imotd",	LVL_GRGOD,	imotd,		2400,	IMOTD_FILE},
		{ "help",       LVL_GRGOD,	help,		2400,	HELP_PAGE_FILE},
		{ "info",	LVL_GRGOD,	info,		8192,	INFO_FILE},
		{ "background",	LVL_IMPL,	background,	8192,	BACKGROUND_FILE},
		{ "handbook",   LVL_IMPL,	handbook,	8192,   HANDBOOK_FILE},
		{ "policies",	LVL_IMPL,	policies,	8192,	POLICIES_FILE},
		{ "pray",	LVL_GRGOD,	pray,		8192,	PRAY_FILE},
		{ "regni",	LVL_GRGOD,	regni,		8192,	REGNI_FILE},
		{ "\n",		0,		NULL,		0,	NULL }
	};
	
#else     //CARLO CIRCLE WINDOWS
	
	struct editor_struct {
		char *cmd;
		char level;
		char *buffer;
		int  size;
		char *filename;
	} fields[] = {
       /* edit the lvls to your own needs */
		{ "credits",	LVL_IMPL,	NULL,	2400,	CREDITS_FILE},
		{ "news",	LVL_GRGOD,	NULL,		8192,	NEWS_FILE},
		{ "motd",	LVL_GRGOD,	NULL,		8192,	MOTD_FILE},
		{ "imotd",	LVL_IMPL,	NULL,		2400,	IMOTD_FILE},
		{ "help",       LVL_GRGOD,	NULL,		2400,	HELP_PAGE_FILE},
		{ "info",	LVL_GRGOD,	NULL,		8192,	INFO_FILE},
		{ "background",	LVL_IMPL,	NULL,	8192,	BACKGROUND_FILE},
		{ "handbook",   LVL_IMPL,	NULL,	8192,   HANDBOOK_FILE},
		{ "policies",	LVL_IMPL,	NULL,	8192,	POLICIES_FILE},
		{ "pray",	LVL_GRGOD,	NULL,		8192,	PRAY_FILE},
		{ "regni",	LVL_GRGOD,	NULL,		8192,	REGNI_FILE},
		{ "\n",		0,		NULL,		0,	NULL }
	};
	
	fields[0].buffer=credits;
	fields[1].buffer=news;
	fields[2].buffer=motd;
	fields[3].buffer=imotd;
	fields[4].buffer=help;
	fields[5].buffer=info;
	fields[6].buffer=background;
	fields[7].buffer=handbook;
	fields[8].buffer=policies;
	fields[9].buffer=pray;
	fields[10].buffer=regni;
	
#endif    //CARLO CIRCLE WINDOWS
	
	
	
	if (ch->desc == NULL) {
		send_to_char("Get outta here you linkdead head!\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) < LVL_GRGOD) {
		send_to_char("You do not have text editor permissions.\r\n", ch);
		return;
	}
	
	half_chop(argument, field, buf);
	
	if (!*field) {
		strcpy(buf, "Files available to be edited:\r\n");
		i = 1;
		for (l = 0; *fields[l].cmd != '\n'; l++) {
			if (GET_LEVEL(ch) >= fields[l].level) {
				sprintf(buf, "%s%-11.11s", buf, fields[l].cmd);
				if (!(i % 7)) strcat(buf, "\r\n");
				i++;
			}
		}
		if (--i % 7) strcat(buf, "\r\n");
		if (i == 0) strcat(buf, "None.\r\n");
		send_to_char(buf, ch);
		return;
	}
	for (l = 0; *(fields[l].cmd) != '\n'; l++)
		if (!strncmp(field, fields[l].cmd, strlen(field)))
			break;
		
	if (*fields[l].cmd == '\n') {
		send_to_char("Invalid text editor option.\r\n", ch);
		return;
	}
	
	if (GET_LEVEL(ch) < fields[l].level) {
		send_to_char("You are not godly enough for that!\r\n", ch);
		return;
	}
	
	switch (l) {
		case 0: ch->desc->str = &credits; break;
		case 1: ch->desc->str = &news; break;
		case 2: ch->desc->str = &motd; break;
		case 3: ch->desc->str = &imotd; break;
		case 4: ch->desc->str = &help; break;
		case 5: ch->desc->str = &info; break;
		case 6: ch->desc->str = &background; break;
		case 7: ch->desc->str = &handbook; break;
		case 8: ch->desc->str = &policies; break;
		case 9: ch->desc->str = &pray; break;
		case 10: ch->desc->str = &regni; break;
		default:
			send_to_char("Invalid text editor option.\r\n", ch);
			return;
	}
	
    /* set up editor stats */
	send_to_char("\x1B[H\x1B[J", ch);
	send_to_char("Edit file below: (/s saves /h for help)\r\n", ch);
	ch->desc->backstr = NULL;
	if (fields[l].buffer) {
		send_to_char(fields[l].buffer, ch);
		ch->desc->backstr = str_dup(fields[l].buffer);
	}
	ch->desc->max_str = fields[l].size;
	ch->desc->mail_to = 0;
	ch->desc->storage = str_dup(fields[l].filename);
	act("$n begins editing a scroll.", TRUE, ch, 0, 0, TO_ROOM);
	SET_BIT(PLR_FLAGS(ch), PLR_WRITING);
	STATE(ch->desc) = CON_TEXTED;
}
void do_newbie(struct char_data *vict)
{
	struct obj_data *obj;
	int give_obj[] = {200, 201, 202, 204, 205, -1};
/* replace the 4 digit numbers on the line above with your basic eq -1 HAS
 * to be the end field
 */
	
	int i;
	for (i = 0; give_obj[i] != -1; i++) {
		obj = read_object(give_obj[i], VIRTUAL);
		obj_to_char(obj, vict);
	}
}

ACMD(do_rlist)
{
	extern struct room_data *world;
	extern int top_of_world;
	
	int first, last, nr, found = 0;
	
	two_arguments(argument, buf, buf2);
	
	if (!*buf || !*buf2) {
		send_to_char("Usage: rlist <begining number> <ending number>\r\n", ch);
		return;
	}
	
	first = atoi(buf);
	last = atoi(buf2);
	//PEPPEXLIST
	if ((first < 0) || (first > 9999999) || (last < 0) || (last > 9999999)) {
		send_to_char("Values must be between 0 and 9999999.\n\r", ch);
		return;
	}
	
	if (first >= last) {
		send_to_char("Second value must be greater than first.\n\r", ch);
		return;
	}
	
	for (nr = 0; nr <= top_of_world && (world[nr].number <= last); nr++) {
		if (world[nr].number >= first) {
			sprintf(buf, "%5d. [%5d] (%3d) %s\r\n", ++found,
				world[nr].number, world[nr].zone,
				world[nr].name);
			send_to_char(buf, ch);
		}
	}
	
	if (!found)
		send_to_char("No rooms were found in those parameters.\n\r", ch);
}


ACMD(do_mlist)
{
	extern struct index_data *mob_index;
	extern struct char_data *mob_proto;
	extern int top_of_mobt;
	
	int first, last, nr, found = 0;
	two_arguments(argument, buf, buf2);
	
	if (!*buf || !*buf2) {
		send_to_char("Usage: mlist <begining number> <ending number>\r\n", ch);
		return;
	}
	
	first = atoi(buf);
	last = atoi(buf2);
	
	if ((first < 0) || (first > 9999999) || (last < 0) || (last > 9999999)) {
		send_to_char("Values must be between 0 and 9999999.\n\r", ch);
		return;
	}
	
	if (first >= last) {
		send_to_char("Second value must be greater than first.\n\r", ch);
		return;
	}
	
	for (nr = 0; nr <= top_of_mobt && (mob_index[nr].virtual <= last); nr++) {
		if (mob_index[nr].virtual >= first) {
			sprintf(buf, "%5d. [%5d] [LEV:%2d] %s\r\n", ++found,
				mob_index[nr].virtual,
				mob_proto[nr].player.level,
				mob_proto[nr].player.short_descr);
			send_to_char(buf, ch);
		}
	}
	
	if (!found)
		send_to_char("No mobiles were found in those parameters.\n\r", ch);
}


ACMD(do_olist)
{
	extern struct index_data *obj_index;
	extern struct obj_data *obj_proto;
	extern int top_of_objt;
	
	int first, last, nr, found = 0;
	
	two_arguments(argument, buf, buf2);
	
	if (!*buf || !*buf2) {
		send_to_char("Usage: olist <begining number> <ending number>\r\n", ch);
		return;
	}
	first = atoi(buf);
	last = atoi(buf2);
	
	if ((first < 0) || (first > 9999999) || (last < 0) || (last > 9999999)) {
		send_to_char("Values must be between 0 and 9999999.\n\r", ch);
		return;
	}
	
	if (first >= last) {
		send_to_char("Second value must be greater than first.\n\r", ch);
		return;
	}
	
	for (nr = 0; nr <= top_of_objt && (obj_index[nr].virtual <= last); nr++) {
		if (obj_index[nr].virtual >= first) {
			sprintf(buf, "%5d. [%5d] [MINLEV:%4d] [SUGLEV:%4d] %s\r\n", ++found,
				obj_index[nr].virtual,
				obj_proto[nr].obj_flags.min_level,
				objlevel(obj_proto+nr),
				obj_proto[nr].short_description);
			send_to_char(buf, ch);
		}
	}
	
	if (!found)
		send_to_char("No objects were found in those parameters.\n\r", ch);
}

ACMD(do_chown)
{
	struct char_data *victim;
	struct obj_data *obj;
	char buf2[80];
	char buf3[80];
	int i, k = 0;
	
	two_arguments(argument, buf2, buf3);
	
	if (!*buf2)
		send_to_char("Syntax: chown <object> <character>.\r\n", ch);
	else if (!(victim = get_char_vis(ch, buf3)))
		send_to_char("No one by that name here.\r\n", ch);
	else if (victim == ch)
		send_to_char("Are you sure you're feeling ok?\r\n", ch);
	else if (GET_LEVEL(victim) >= GET_LEVEL(ch))
		send_to_char("That's really not such a good idea.\r\n", ch);
	else if (!*buf3)
		send_to_char("Syntax: chown <object> <character>.\r\n", ch);
	else {
		for (i = 0; i < NUM_WEARS; i++) {
			if (GET_EQ(victim, i) && CAN_SEE_OBJ(ch, GET_EQ(victim, i)) &&
				isname(buf2, GET_EQ(victim, i)->name)) {
				obj_to_char(unequip_char(victim, i), victim);
				k = 1;
			}
		}
		
		if (!(obj = get_obj_in_list_vis(victim, buf2, victim->carrying))) {
			if (!k && !(obj = get_obj_in_list_vis(victim, buf2, victim->carrying))) {
				sprintf(buf, "%s does not appear to have the %s.\r\n", GET_NAME(victim), buf2);
				send_to_char(buf, ch);
				return;
			}
		}
		
		if (GET_INVIS_LEV(ch) < GET_LEVEL(victim)) {
			act("&n$n makes a magical gesture and $p&n flies from $N to $m.",FALSE,ch,obj,
				victim,TO_NOTVICT);
			act("&n$n makes a magical gesture and $p&n flies away from you to $m.",FALSE,ch,
				obj,victim,TO_VICT);
		}
		act("&nYou make a magical gesture and $p&n flies away from $N to you.",FALSE,ch,
			obj, victim,TO_CHAR);
		
		obj_from_char(obj);
		obj_to_char(obj, ch);
		save_char(ch, NOWHERE);
		save_char(victim, NOWHERE);
	}
}


ACMD(do_listprac)
{
	struct char_data *vict;
	extern char *spells[];
	extern struct spell_info_type spell_info[];
	int i, sortpos;
	extern int spell_sort_info[MAX_SKILLS+1];
	
	one_argument(argument, buf);
	
	if (!*buf)
		send_to_char("Di chi vuoi elencare le abilita' ?\r\n", ch);
	else if (!(vict = get_char_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else
	{
		if (!GET_PRACTICES(vict)) sprintf(buf, "%s non ha nessuna pratica.\r\n", GET_NAME(vict));
		else sprintf(buf, "%s ha %d sessioni di pratica.\r\n", GET_NAME(vict), GET_PRACTICES(vict));
		sprintf(buf+strlen(buf), "%s conosce i seguenti spells e skills:\r\n", GET_NAME(vict));
		strcpy(buf2, buf);
		for (sortpos = 1; sortpos < MAX_SKILLS; sortpos++)
		{
			i = spell_sort_info[sortpos];
			if (strlen(buf2) >= MAX_STRING_LENGTH - 32)
			{
				strcat(buf2, "**OVERFLOW**\r\n");
				break;
			}
			if (GET_LEVEL(vict) >= spell_info[i].min_level[(int) GET_CLASS(vict)])
			{
				sprintf(buf, "[%3d] %-20s [%3d/100] %s\r\n", i, spells[i], GET_SKILL(vict,i), how_good(GET_SKILL(vict, i)));
				strcat(buf2, buf);
			}
		}
		page_string(ch->desc, buf2, 1);
	}
}

ACMD(do_listprof)
{
	struct char_data *vict;
	extern char *proficienze[];
	int i, sortpos;
	extern int proficienze_sort_info[MAX_PROFICIENZE+1];
	
	one_argument(argument, buf);
	
	if (!*buf)
		send_to_char("Di chi vuoi elencare le abilita' ?\r\n", ch);
	else if (!(vict = get_char_vis(ch, buf)))
		send_to_char(NOPERSON, ch);
	else
	{
		sprintf(buf2, "%s e' addestrato nelle seguenti proficienze:\r\n", GET_NAME(vict));
		for (sortpos = 1; sortpos < MAX_PROFICIENZE; sortpos++)
		{
			i = proficienze_sort_info[sortpos];
			if (strlen(buf2) >= MAX_STRING_LENGTH - 32)
			{
				strcat(buf2, "**OVERFLOW**\r\n");
				break;
			}
			if (strcmp(proficienze[i], "!NONUSATO!"))
			{
				sprintf(buf, "[%3d] %-20s [%3d/100] %s\r\n", i, proficienze[i], GET_PROF(vict,i), how_good(GET_PROF(vict, i)));
				strcat(buf2, buf);
			}
		}
		page_string(ch->desc, buf2, 1);
	}
}

/* Comando zonexit aggiunto da rusty */

ACMD(do_zonexit)
{                                      /*1*/
	int vmin,rmin,rmax,i,n,j,rzone,vzone,zonexit,st,two,exitzone,rxtzone;
	struct room_data *current;
	two_arguments(argument,buf,buf1);
	two=TRUE;
	exitzone=rxtzone=0;
	if (!*buf) {
		send_to_char("Uso: zonexit <num zona> <num zona uscita>.\r\n", ch);
		return;
	}
	if(!*buf1)
		two=FALSE;
	else
		exitzone=atoi(buf1);
	vzone=atoi(buf);
	vmin=vzone*100;
	
  /*cerco il real zone number all interno della zonetable */
	for (j = vzone, rzone = 0; zone_table[rzone].number != j && rzone <= top_of_zone_table; rzone++);
	if (rzone > top_of_zone_table){
		send_to_char("Non e' una zona valida.\r\n", ch);
		return;
	}
	if(two==TRUE){/*idem per la zona di uscita*/
		for (j = exitzone, rxtzone = 0; zone_table[rxtzone].number != j && rxtzone <= top_of_zone_table; rxtzone++);
		if (rxtzone > top_of_zone_table){
			send_to_char("La zona di uscita Non e' una zona valida.\r\n", ch);
			return;
		}
	}
	
	zonexit=0;
	
  /*nel caso che la zona non vesse zone*100 come prima stanza*/
	while((rmin=real_room(vmin))==-1)
		vmin++;
	
	i=rmin;
  /*Cerco rmax */
	while(world[i].zone==rzone)
		i++;
	rmax=i-1;
	
	if(two==FALSE){
		sprintf(buf, "\r\n**********************************************************");
		sprintf(buf1,"\r\n   Stanze della Zona %d che hanno uscite in altre zone", vzone);
		sprintf(buf2,"\r\n**********************************************************");
		send_to_char(buf,ch);
		send_to_char(buf1,ch);
		send_to_char(buf2,ch);
	}
	
	if(two==TRUE){
		sprintf(buf, "\r\n**********************************************************");
		sprintf(buf1,"\r\n   Stanze della Zona %d che hanno uscite nella zona %d", vzone,exitzone);
		sprintf(buf2,"\r\n**********************************************************\n");
		send_to_char(buf,ch);
		send_to_char(buf1,ch);
		send_to_char(buf2,ch);
	}
	
  /*primo ciclo for:scansione delle stanze della zona*/
	
	for(n=rmin,st=0;n<=rmax;n++){
		current=&world[n];
		zonexit=0;
		
		
    /*Secondo Ciclo for:scansione delle dir della stanza*/
		
		for (i = 0; i < NUM_OF_DIRS; i++) {
			if (current->dir_option[i]) {
				if (current->dir_option[i]->to_room != NOWHERE){/*se la stanza esiste*/
					if ( (world[current->dir_option[i]->to_room].zone != rzone) &&((two==FALSE)||((two==TRUE)&&world[current->dir_option[i]->to_room].zone == rxtzone)))
					{/*6*/
	    /*se effettivamente non appartengono alla stessa zona*/
						if(zonexit==0){ /*prima uscita cercata della stanza*/
							zonexit=1;
							st++;
							sprintf(buf1, "\n[%d]-STANZA: %s \r\n",st, current->name);
							send_to_char(buf1, ch);
							sprinttype(current->sector_type, sector_types, buf1);
							sprintf(buf, "Zona: [%3d], VNum: [%5d], RNum: [%5d], Type: %s\r\n",zone_table[current->zone].number, current->number, ch->in_room, buf1);
							send_to_char(buf, ch);
							
						}
						sprintf(buf1, "%5d",world[current->dir_option[i]->to_room].number);
						sprintbit(current->dir_option[i]->exit_info, exit_bits, buf2);
						sprintf(buf , "Exit[ %-5s]:  To: [%s], Type: %s - %s\r\n ",dirs[i],
							buf1, buf2,world[current->dir_option[i]->to_room].name);
						send_to_char(buf, ch);
					}
				}
			}
		}
	}
	if(st==0){
		sprintf(buf,"\n\r         Nessuna uscita Trovata");
		send_to_char(buf,ch);
	}
}

ACMD(do_ashop)
{
	struct char_data *vict;
	int shop;
	int i, zone;
	
	half_chop(argument, arg, buf);
	
	if (!*arg || !*buf || !isdigit(*buf))
	{
		send_to_char("Uso: ashop <giocatore/clan> <numero-negozio>\r\nUsa il comando NEGOZI per avere una lista dei negozi.\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_vis(ch, arg)) && (find_clan(arg)<0))
	{
		send_to_char("Clan o giocatore sconosciuto.\r\n", ch);
		return;
	}
	
	shop = atoi(buf);
	i = get_shopindexfromnum(buf);
	if (i==-1)
	{
		send_to_char("Il negozio non esiste.\r\n", ch);
		return;
	}
	
	if (SHOP_PROPRIETARIO(i) != 0 || SHOP_CLAN(i) != -1)
	{
		send_to_char("Il negozio e' gia' assegnato.\r\n", ch);
		return;
	}
	
	if (vict)
	{
		if (IS_NPC(vict))
		{
			sprintf(buf1,"Non puoi assegnare il negozio a %s, e' un mob.\r\n", GET_NAME(vict));
			send_to_char(buf1, ch);
			return;
		}
		
		SHOP_PROPRIETARIO(i) = (GET_IDNUM(vict));
		sprintf(buf1, "Assegni a %s un negozio!\r\n", GET_NAME(vict));
		send_to_char(buf1, ch);
		sprintf(buf1, "%s ti assegna la gestione di un negozio!\r\n", GET_NAME(ch));
		send_to_char(buf1, vict);
	}
	else
	{
		SHOP_CLAN(i) = clan[find_clan(arg)].id;
		sprintf(buf1, "Assegni al clan %s un negozio!\r\n", arg);
		send_to_char(buf1, ch);
	}
	
	zone = real_zone(SHOP_ROOM(i,0));
	if (zone!=-1)
		sedit_save_to_disk(zone);
}

ACMD(do_takeshop)
{
	int i,zone;
	
	one_argument(argument, arg);
	
	if (!arg)
	{
		send_to_char("Uso: takeshop  <numero negozio>", ch);
		return;
	}
	
	i = get_shopindexfromnum(arg);
	if (i==-1)
	{
		send_to_char("Il negozio non esiste.\r\n", ch);
		return;
	}
	
	if (SHOP_PROPRIETARIO(i) == 0 && SHOP_CLAN(i) == -1)
	{
		send_to_char("Il negozio non e' assegnato a nessuno.\r\n", ch);
		return;
	}
	
	SHOP_PROPRIETARIO(i) = 0;
	SHOP_CLAN(i) = -1;
	sprintf(buf1, "Rimuovi l'assegnazione del negozio %s!\r\n", arg);
	send_to_char(buf1, ch);
	zone = real_zone(SHOP_ROOM(i,0));
	if (zone!=-1)
		sedit_save_to_disk(zone);
}

ACMD(do_amob)
{
	struct char_data *vict;
	struct char_data *mob=NULL;
	
	argument = one_argument(argument, arg);
	
	if (!*arg || !*argument)
	{
		send_to_char("Uso: amob <mob> <pg/clan>\r\n", ch);
		return;
	}
	
	skip_spaces(&argument);
	
	if (!(mob = get_char_room_vis(ch, arg)))
	{
		send_to_char("Non vedi nessun mob con quel nome nella stanza.\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, argument)) && (find_clan(argument)<0))
	{
		send_to_char("Clan o giocatore sconosciuto.\r\n", ch);
		return;
	}
	
	if (MASTER_ID(mob) != 0 || CLAN_ID(mob) != -1)
	{
		send_to_char("Il mob e' gia' assegnato.\r\n", ch);
		return;
	}
	
	if (vict)
	{
		
		if (!IS_NPC(mob))
		{
			sprintf(buf1,"Non puoi assegnare il mob %s a %s, non e' un mob.\r\n", GET_NAME(mob), GET_NAME(vict));
			send_to_char(buf1, ch);
			return;
		}
		
		if (IS_NPC(vict))
		{
			sprintf(buf1,"Non puoi assegnare il mob a %s, e' un mob.\r\n", GET_NAME(vict));
			send_to_char(buf1, ch);
			return;
		}
		
		MASTER_ID(mob) = (GET_IDNUM(vict));
		sprintf(buf1, "Assegni a %s un il mob %s!\r\n", GET_NAME(vict),GET_NAME(mob));
		send_to_char(buf1, ch);
		sprintf(buf1, "%s ti assegna la gestione di %s!\r\n", GET_NAME(ch), GET_NAME(mob));
		send_to_char(buf1, vict);
	}
	else
	{
		CLAN_ID(mob) = clan[find_clan(argument)].id;
		sprintf(buf1, "Assegni al clan %s il mob %s!\r\n", argument, GET_NAME(mob));
		send_to_char(buf1, ch);
	}
}



ACMD(do_takemob)
{
	struct char_data *mob;
	
	one_argument(argument, arg);
	
	if (!arg)
	{
		send_to_char("Uso: takemob  <mob>", ch);
		return;
	}
	
	if (!(mob = get_char_room_vis(ch, arg)))
	{
		send_to_char("Non vedi nessun mob con quel nome nella stanza.\r\n", ch);
		return;
	}
	
	if (MASTER_ID(mob) == 0 && CLAN_ID(mob) == -1)
	{
		send_to_char("Il mob non e' assegnato a nessuno.\r\n", ch);
		return;
	}
	
	MASTER_ID(mob) = 0;
	CLAN_ID(mob) = -1;
	sprintf(buf1, "Rimuovi l'assegnazione del mob %s!\r\n", arg);
	send_to_char(buf1, ch);
}



ACMD(do_elenca)
{
	extern struct index_data *obj_index;
	extern struct obj_data *obj_proto;
	extern int top_of_objt;
	extern void oedit_disp_charaff_menu(struct descriptor_data *d);
	extern void oedit_disp_apply_menu(struct descriptor_data *d);
	int nr, found = 0, i, j;
	long first, last;
	int counter, columns = 0;
	
	argument = one_argument(argument, buf);
	argument = one_argument(argument, buf2);
	
	if (!*buf || !*buf2) {
		send_to_char("Uso: elenca <begining number> <ending number> <T(ype)/A(affections)/P(roperties)> \r\n", ch);
		return;
	}
	first = atoi(buf);
	last = atoi(buf2);
	
	if ((first ==0) && (last ==0))
	{
		first = 0;
		last = 2999999;
	}
	
	if ((first < 0) || (first > 2999999) || (last < 0) || (last > 2999999)) {
		send_to_char("Values must be between 0 and 2999999.\n\r", ch);
		return;
	}
	
	argument = one_argument(argument, arg);
	
	if (!*arg)
	{
		send_to_char("Uso: elenca <begining number> <ending number> <caratteristica>|<a/p> \r\n", ch);
		return;
	}
	
	if (!str_cmp(arg,"a"))
	{
		get_char_cols(ch);
#if defined(CLEAR_SCREEN)
		send_to_char("[H[J", ch);
#endif
		for (counter = 0; counter < NUM_AFF_FLAGS; counter++) {
			sprintf(buf, "%s%2d%s) %-20.20s %s", grn, counter + 1, nrm,
				affected_bits[counter], !(++columns % 3) ? "\r\n" : "");
			send_to_char(buf, ch);
		}
		return;
	}
	
	if (!str_cmp(arg,"p"))
	{
		get_char_cols(ch);
#if defined(CLEAR_SCREEN)
		send_to_char("[H[J", ch);
#endif
		for (counter = 0; counter < NUM_APPLIES; counter++) {
			sprintf(buf, "%s%2d%s) %-20.20s %s", grn, counter, nrm,
				apply_types[counter], !(++columns % 2) ? "\r\n" : "");
			send_to_char(buf, ch);
		}
		return;
	}
	
	
	if (!str_cmp(arg,"t")){
		get_char_cols(ch);
#if defined(CLEAR_SCREEN)
		send_to_char("[H[J", ch);
#endif
		for (counter = 0; counter < NUM_ITEM_TYPES; counter++) {
			sprintf(buf, "%s%2d%s) %-24.24s %s", grn, counter, nrm,
				item_types[counter], !(++columns % 3) ? "\r\n" : "");
			send_to_char(buf, ch);
		}
	}
	
	for (i = 0; i < NUM_AFF_FLAGS; i++) {
		if (!str_cmp(arg, affected_bits[i])) {
			found = TRUE;
			break;
		}
	}
	
	if (found) {
		for (nr = 0; nr <= top_of_objt && (obj_index[nr].virtual <= last); nr++) {
			if ((IS_SET(obj_proto[nr].obj_flags.bitvector[0] , (1 << i))) && (obj_index[nr].virtual >= first)){
				sprintf(buf, "%5d. [%5d] [MINLEV:%4d] [SUGLEV:%4d] %s\r\n", ++found,
					obj_index[nr].virtual,
					obj_proto[nr].obj_flags.min_level,
					objlevel(obj_proto+nr),
					obj_proto[nr].short_description);
				send_to_char(buf, ch);
			}
		}
		return;
	} else {
		
		for (i = 0; i < NUM_APPLIES; i++) {
			if (!str_cmp(arg, apply_types[i])) {
				found = TRUE;
				break;
			}
		}
		
		if (found) {
			for (nr = 0; nr <= top_of_objt && (obj_index[nr].virtual <= last); nr++) {
				for (j = 0; j < MAX_OBJ_AFFECT; j++) {
					if ((IS_SET(obj_proto[nr].affected[j].location, i)) && (obj_index[nr].virtual >= first)) {
						sprintf(buf, "%5d. [%5d] [MINLEV:%4d] [SUGLEV:%4d] [MOD: %s %4d] %s\r\n", ++found,
							obj_index[nr].virtual,
							obj_proto[nr].obj_flags.min_level,
							objlevel(obj_proto+nr),
							apply_types[i],
							obj_proto[nr].affected[j].modifier,
							obj_proto[nr].short_description);
						send_to_char(buf, ch);
						break;
					}
				}
			}
			return;
		} else {
			for (i = 0; i < NUM_APPLIES; i++) {
				if (!str_cmp(arg, apply_types[i])) {
					found = TRUE;
					break;
				}
			}
			if (found) {
				for (nr = 0; nr <= top_of_objt && (obj_index[nr].virtual <= last); nr++) {
					for (j = 0; j < MAX_OBJ_AFFECT; j++) {
						if ((IS_SET(obj_proto[nr].affected[j].location, i)) && (obj_index[nr].virtual >= first)) {
							sprintf(buf, "%5d. [%5d] [MINLEV:%4d] [SUGLEV:%4d] [MOD: %s %4d] %s\r\n", ++found,
								obj_index[nr].virtual,
								obj_proto[nr].obj_flags.min_level,
								objlevel(obj_proto+nr),
								apply_types[i],
								obj_proto[nr].affected[j].modifier,
								obj_proto[nr].short_description);
							send_to_char(buf, ch);
							break;
						}
					}
				}
				return;
			} else {
				
				for (i = 0; i < NUM_ITEM_TYPES; i++) {
					if (!str_cmp(arg, item_types[i])) {
						found = TRUE;
						break;
					}
				}
				
				if (found) {
					for (nr = 0; nr <= top_of_objt && (obj_index[nr].virtual <= last); nr++) {
						if ((obj_proto[nr].obj_flags.type_flag == i) && (obj_index[nr].virtual >= first)) {
							sprintf(buf, "%5d. [%5d] [MINLEV:%4d] [SUGLEV:%4d] [TYPE: %s] %s\r\n", ++found,
								obj_index[nr].virtual,
								obj_proto[nr].obj_flags.min_level,
								objlevel(obj_proto+nr),
								item_types[i],
								obj_proto[nr].short_description);
							send_to_char(buf, ch);
						}
						
					}
					return;
					
					
				} else {
					
       //Altre riceche
				}
			}
		}
	}
	if (!found)
		send_to_char("No objects were found in those parameters.\n\r", ch);
}

#define LVL_TRUST LVL_QUEST_MASTER

ACMD(do_trust) {
	
	char name[MAX_INPUT_LENGTH];
	struct char_data *vict, *cbuf = NULL;
	char is_file = 0;
	struct char_file_u tmp_store;
	int player_i = 0;
	int parse_class(char arg);
	
	half_chop(argument,name, buf);
	
	if (!(vict = get_player_vis(ch, name, 0))) {
		//send_to_char("There is no such player.\r\n", ch);
		is_file=1;
	}		
	if (is_file) {
		CREATE(cbuf, struct char_data, 1);
		clear_char(cbuf);
		if ((player_i = load_char(name, &tmp_store)) > -1) {
			store_to_char(&tmp_store, cbuf);			
			vict = cbuf;
		} else {
			free(cbuf);
			send_to_char("There is no such player.\r\n", ch);
			return;
		}
	}
	
	if ((GET_LEVEL(vict)>=LVL_TRUST)&&(!IS_TRUST(vict))) {
		send_to_char("Non ne ha bisogno!\r\n",ch);
		return;
	}
	
	if (!IS_TRUST(vict)) {
		sprintf(buf, "(TRUST) %s START trust to %s.", GET_NAME(ch), GET_NAME(vict));
		mudlog(buf, BRF, LVL_GOD, TRUE);
		LVL_BEF_TRUST(vict)=GET_LEVEL(vict); //Memorizza il vecchio LVL
		GET_LEVEL(vict)=LVL_TRUST; //Assegna il livello per i TRUST 
		SET_BIT(PLR_FLAGS(vict),PLR_TRUSTED); //IMPORTANTE!!!!!
		SET_BIT(PRF_FLAGS(vict),PRF_NOHASSLE); 
		SET_BIT(PRF_FLAGS(vict),PRF_HOLYLIGHT);
		SET_BIT(PRF_FLAGS(vict),PRF_ROOMFLAGS); 
		SET_BIT(PRF_FLAGS(vict),PRF_HIDE_SNEAK);
		SET_BIT(PRF_FLAGS(vict),PRF_NO_EROE);
		SET_BIT(PLR_FLAGS(vict), PLR_NOWIZLIST);
		GET_OLC_ZONE(vict) = -1;
		if (!is_file)
			perform_immort_invis(vict,LVL_IMMORT); // Non puo' Essere Visibile
		else 
			GET_INVIS_LEV(vict) = GET_LEVEL(vict);
		
	} else {
		sprintf(buf, "(TRUST) %s END trust to %s.", GET_NAME(ch), GET_NAME(vict));
		mudlog(buf, BRF, LVL_GOD, TRUE);
		GET_LEVEL(vict)=LVL_BEF_TRUST(vict);
		REMOVE_BIT(PLR_FLAGS(vict),PLR_TRUSTED);
		REMOVE_BIT(PLR_FLAGS(vict),PLR_TRUSTED); //IMPORTANTE!!!!!
		REMOVE_BIT(PRF_FLAGS(vict),PRF_NOHASSLE); 
		REMOVE_BIT(PRF_FLAGS(vict),PRF_HOLYLIGHT);
		REMOVE_BIT(PRF_FLAGS(vict),PRF_ROOMFLAGS); 
		REMOVE_BIT(PRF_FLAGS(vict),PRF_HIDE_SNEAK);
		REMOVE_BIT(PRF_FLAGS(vict),PRF_NO_EROE);
		REMOVE_BIT(PLR_FLAGS(vict), PLR_NOWIZLIST);
		GET_OLC_ZONE(vict) = -1;
		if (!is_file)
			perform_immort_vis(vict);
		else 
			GET_INVIS_LEV(vict) = 0;
		
	}
	
	if (!is_file)
		save_char(vict, NOWHERE);
	
	if (is_file) {
		char_to_store(vict, &tmp_store);
		fseek(player_fl, (player_i) * sizeof(struct char_file_u), SEEK_SET);
		fwrite(&tmp_store, sizeof(struct char_file_u), 1, player_fl);
		free_char(cbuf);
		send_to_char("Saved in file.\r\n", ch);		
	}
}


ACMD(do_namechange){
	struct char_data *vict;  char *oldname;
	int i,j;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char tmp_name[MAX_STRING_LENGTH];
	
	half_chop(argument, arg1, argument);
	half_chop(argument, arg2, argument);
	
	if ((!*arg1) || (!*arg2)) {
		send_to_char("Usage: namechange <oldname> <newname> \r\n", ch);    return;
	}
	
	if (!(vict = get_player_vis(ch, arg1, 0))) {
		send_to_char("They aren't here.\r\n", ch);
		return;
	}
	
/*	if (GET_LEVEL(vict) > 5) {
		send_to_char("You can't change the name of someone over level 5.\r\n",ch);
		return;
	}
*/
	if (find_name(arg2)>-1)  {
		send_to_char("There's already somebody with this name.\r\n",ch);
		return;
	}

	if ((_parse_name(arg2, tmp_name))||strlen(tmp_name) < 2 ||
		strlen(tmp_name) > MAX_NAME_LENGTH || !Valid_Name(tmp_name) ||
		fill_word(tmp_name) || reserved_word(tmp_name)) {
		send_to_char("Illegal name, retry.\r\n", ch);
		return;
	}
	
	for (i = 0; i <= top_of_p_table; i++)
		if (player_table[i].id == GET_IDNUM(vict))
			break;
		
	if (player_table[i].id != GET_IDNUM(vict)){
		send_to_char("BUG: error in player table!!!!",ch);
		sprintf(buf,"BUG: error in player table for %s with idnum %ld",
			GET_NAME(ch),GET_IDNUM(ch));
		log(buf);
		return;
	}
	
	oldname = strdup(GET_NAME(vict));
	
    //player_table[i].name = strdup(arg2);
	for (j = 0; (*(player_table[i].name + j) = LOWER(*(arg2 + j))); j++);
	
	strcpy(vict->player.name, CAP(arg2));  
	
	sprintf(buf, "(GC) %s has changed the name of %s to %s.", GET_NAME(ch),oldname, arg2);
	mudlog(buf, BRF, LVL_IMMORT, TRUE);
	
	save_char(vict, vict->in_room);
	
	send_to_char("Ok.\r\n",ch);
	send_to_char("Your name has changed.\r\n",vict);
}

ACMD(do_rinnova) {
	int i, trovato = 0;
	char nome_pg[MAX_STRING_LENGTH];
	FILE *old_player_fl, *ren_player_fl;
	struct char_file_u fch;

	one_argument(argument, nome_pg);
	if (!*nome_pg) {
		send_to_char("Usage: rinnova <nome pg>\r\n", ch);
		return;
	}

	for (i = 0; i<=top_of_p_table; i++) {
		if (find_name(nome_pg)>-1) {
			send_to_char("There's already somebody with this name.\r\n", ch);
			return;
		}
	}

	i = 0;
	old_player_fl = fopen(OLD_PLAYER_FILE, "r+");
	for (; !feof(old_player_fl); i++) {
		fread(&fch, sizeof(struct char_file_u), 1, old_player_fl);
		if (!str_cmp(fch.name, nome_pg)) {
			if (IS_SET(fch.char_specials_saved.act, PLR_DELETED)) {
				sprintf(buf, "Il pg antico di nome %s e' gia' stato riesumato", nome_pg);
				send_to_char(buf, ch);
			}
			else {
				ren_player_fl = fopen(REN_PLAYER_FILE, "a");
				fwrite(&fch, sizeof(struct char_file_u), 1, ren_player_fl);
				fclose(ren_player_fl);
				SET_BIT(fch.char_specials_saved.act, PLR_DELETED);
				fseek(old_player_fl, i * sizeof(struct char_file_u), SEEK_SET);
				fwrite(&fch, sizeof(struct char_file_u), 1, old_player_fl);
			}
			trovato = 1;
			break;
		}
	}

	if (!trovato) {
		sprintf (buf, "Nessun pg antico di nome %s", nome_pg);
		send_to_char(buf, ch);
	}
}

