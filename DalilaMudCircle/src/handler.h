/* ************************************************************************
*   File: handler.h                                     Part of CircleMUD *
*  Usage: header file: prototypes of handling and utility functions       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* handling the affected-structures */
void	affect_total(struct char_data *ch);
void	affect_modify(struct char_data * ch, byte loc, sbyte mod, long long int bitv1, long long int bitv2, long long int bitv3, long long int bitv4, bool add);
void	affect_to_char(struct char_data *ch, struct affected_type *af);
void	affect_remove(struct char_data *ch, struct affected_type *af);
void	affect_from_char(struct char_data *ch, int natura, int type);
bool	affected_by_spell(struct char_data *ch, int natura, int type);
void	affect_join(struct char_data *ch, struct affected_type *af,
        bool add_dur, bool avg_dur, bool add_mod, bool avg_mod);
void	put_two_flags_to_aff (struct affected_type *af, int bitv1, long long int flag1, int bitv2, long long int flag2);
int	char_affected_by_obj (struct char_data *ch, long long int affect, int bitv);
void	affect_remove_all (struct char_data *ch, long long int affect, int bitv);


/* utility */
char *money_desc(int amount);
struct obj_data *create_money(int amount);
int	isname(char *str, char *namelist);
char	*fname(char *namelist);
int	get_number(char **name);

/* ******** objects *********** */

void	obj_to_char(struct obj_data *object, struct char_data *ch);
void	obj_from_char(struct obj_data *object);

void	equip_char(struct char_data *ch, struct obj_data *obj, int pos);
struct obj_data *unequip_char(struct char_data *ch, int pos);

struct obj_data *get_obj_in_list(char *name, struct obj_data *list);
struct obj_data *get_obj_in_list_num(int num, struct obj_data *list);
struct obj_data *get_obj(char *name);
struct obj_data *get_obj_num(int nr);

void	obj_to_room(struct obj_data *object, int room);
void	obj_from_room(struct obj_data *object);
void	obj_to_obj(struct obj_data *obj, struct obj_data *obj_to);
void	obj_from_obj(struct obj_data *obj);
void	object_list_new_owner(struct obj_data *list, struct char_data *ch);

void	extract_obj(struct obj_data *obj);

/* ******* characters ********* */

struct char_data *get_char_room(char *name, int room);
struct char_data *get_char_num(int nr);
struct char_data *get_char(char *name);

void	char_from_room(struct char_data *ch);
void	char_to_room(struct char_data *ch, int room);
void	extract_char(struct char_data *ch);
void	dismount_char(struct char_data * ch);

/* find if character can see */
struct char_data *get_char_room_vis(struct char_data *ch, char *name);
struct char_data *get_player_vis(struct char_data *ch, char *name, int inroom);
struct char_data *get_char_vis(struct char_data *ch, char *name);
struct obj_data *get_obj_in_list_vis(struct char_data *ch, char *name, struct obj_data *list);
struct obj_data *get_obj_in_list_vis_or_light(struct char_data * ch, char *name, struct obj_data * list);
int num_materiale_in_list_vis(struct char_data * ch, struct obj_data *primo, struct obj_data * list);
struct obj_data *get_obj_vis(struct char_data *ch, char *name);
struct obj_data *get_object_in_equip_vis(struct char_data *ch,
char *arg, struct obj_data *equipment[], int *j);


/* find all dots */

int	find_all_dots(char *arg);

#define FIND_INDIV	0
#define FIND_ALL	1
#define FIND_ALLDOT	2


/* Generic Find */

int	generic_find(char *arg, int bitvector, struct char_data *ch,
                     struct char_data **tar_ch, struct obj_data **tar_obj);

#define FIND_CHAR_ROOM     1
#define FIND_CHAR_WORLD    2
#define FIND_OBJ_INV       4
#define FIND_OBJ_ROOM      8
#define FIND_OBJ_WORLD    16
#define FIND_OBJ_EQUIP    32


/* prototypes from crash save system */

int	Crash_get_filename(char *orig_name, char *filename);
int	Crash_delete_file(char *name);
int	Crash_delete_crashfile(struct char_data *ch);
int	Crash_clean_file(char *name);
void	Crash_listrent(struct char_data *ch, char *name);
int	Crash_load(struct char_data *ch);
void	Crash_crashsave(struct char_data *ch);
void	Crash_idlesave(struct char_data *ch);
void	Crash_save_all(void);
void	Reboot_save_all(void);

/* prototypes from fight.c */
void	set_fighting(struct char_data *ch, struct char_data *victim);
void	stop_fighting(struct char_data *ch);
void	stop_follower(struct char_data *ch);
void	hit(struct char_data *ch, struct char_data *victim, int type);
void	forget(struct char_data *ch, struct char_data *victim);
void	remember(struct char_data *ch, struct char_data *victim);
void	damage(struct char_data *ch, struct char_data *victim, int dam, int attacktype);
void	damage_nuovo(struct char_data *ch, struct char_data *victim, int dam, int attacktype, bool maxato, int tipo);
int	skill_message(int dam, struct char_data *ch, struct char_data *vict,
		      int attacktype);
int	controllo_volo (struct char_data *ch, struct char_data *vict);
int	controllo_scontro (struct char_data *ch, struct char_data *vict);

/* prototypes from regen.c */
void    alter_hit(struct char_data *ch, int amount);
void    alter_mana(struct char_data *ch, int amount);
void    alter_move(struct char_data *ch, int amount);
void    check_regen_rates(struct char_data *ch);

bool can_do_event(struct char_data *ch, long int comdtype, long pulse);


