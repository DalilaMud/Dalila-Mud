/* ************************************************************************
*   File: utils.h                                       Part of CircleMUD *
*  Usage: header file: utility macros and prototypes of utility funcs     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


/* external declarations and prototypes **********************************/

extern FILE *logfile;

#undef log

/*
#ifdef CIRCLE_WINDOWS
#define log(x) basic_mud_log(x)
#endif
*/

#define log(x) log_mud(x)

/* public functions in utils.c */
char	*str_dup(const char *source);
int	str_cmp(const char *arg1, char *arg2);
int	strn_cmp(char *arg1, char *arg2, int n);
void	log_mud(char *str);
int	touch(char *path);
void	mudlog(char *str, char type, int level, byte file);
void	log_death_trap(struct char_data *ch);
int	number(int from, int to);
int	dice(int number, int size);
void	sprintbit(long long int vektor, char *names[], char *result);
void	sprinttype(int type, char *names[], char *result);
int	get_line(FILE *fl, char *buf);
int	get_filename(char *orig_name, char *filename, int mode);
struct time_info_data age(struct char_data *ch);
int	num_pc_in_room(struct room_data *room);
int     replace_str(char **string, char *pattern, char *replacement, int rep_all, int max_size);
void    format_text(char **ptr_string, int mode, struct descriptor_data *d, int maxlen);
char   *stripcr(char *dest, const char *src);
long long int atolonglong (const char * nptr);

//extern int abilita_vector[][16];

void	phanlog(char *str);
char phantomima[MAX_STRING_LENGTH];
char mamba_uno[MAX_STRING_LENGTH];
char mamba_due[MAX_STRING_LENGTH];
char mamba_tre[MAX_STRING_LENGTH];
char mamba_qua[MAX_STRING_LENGTH];
char mamba_cin[MAX_STRING_LENGTH];

/* undefine MAX and MIN so that our functions are used instead */
#ifdef MAX
#undef MAX
#endif

#ifdef MIN
#undef MIN
#endif

int MAX(int a, int b);
int MIN(int a, int b);

/* in magic.c */
bool	circle_follow(struct char_data *ch, struct char_data * victim);

/* in act.informative.c */
void	look_at_room(struct char_data *ch, int mode);

/* in act.movement.c */
int	do_simple_move(struct char_data *ch, int dir, int following, int show_room);
int	perform_move(struct char_data *ch, int dir, int following, int show_room);

/* in limits.c */
int	mana_limit(struct char_data *ch);
int	hit_limit(struct char_data *ch);
int	move_limit(struct char_data *ch);
int	mana_gain(struct char_data *ch);
int	hit_gain(struct char_data *ch);
int	move_gain(struct char_data *ch);
void	advance_level(struct char_data *ch);
void	set_title(struct char_data *ch, char *title);
int	gain_exp(struct char_data *ch, int gain);
void	gain_exp_regardless(struct char_data *ch, int gain);
void	gain_condition(struct char_data *ch, int condition, int value);
void	check_idling(struct char_data *ch);
void	point_update(void);
void    damage_update(void);
void	update_pos(struct char_data *victim);

/* in spec_procs.c */
SPECIAL(manovale);



/* various constants *****************************************************/
#define NUM_STARTROOMS 6
#define NUM_CMD_FANTASMA 57
#define NUM_CMD_NOGDR    64

/* defines for mudlog() */
#define OFF	0
#define BRF	1
#define NRM	2
#define CMP	3

/* get_filename() */
#define CRASH_FILE	0
#define ETEXT_FILE	1
#define ALIAS_FILE      2
#define MOB_FILE        3

/* breadth-first searching */
#define BFS_ERROR		-1
#define BFS_ALREADY_THERE	-2
#define BFS_NO_PATH		-3

/*mud virtual sarting year*/

#define STARTING_YEAR    1000   /*Anno da cui inizia il conteggio
				  del tempo trascorso            */

/* mud-life time */
#define SECS_PER_MUD_HOUR	75
#define SECS_PER_MUD_DAY	(24*SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH	(35*SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR	(17*SECS_PER_MUD_MONTH)

/* real-life time (remember Real Life?) */
#define SECS_PER_REAL_MIN	60
#define SECS_PER_REAL_HOUR	(60*SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY	(24*SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR	(365*SECS_PER_REAL_DAY)

/*mestieri*/
#define RANGE_CACCIATORE 3

/* string utils **********************************************************/


#define YESNO(a) ((a) ? "YES" : "NO")
#define ONOFF(a) ((a) ? "ON" : "OFF")

#define LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)   (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c) )

#define ISNEWL(ch) ((ch) == '\n' || (ch) == '\r')
#define IF_STR(st) ((st) ? (st) : "\0")
#define CAP(st)  (*(st) = UPPER(*(st)), st)

#define AN(string) (strchr("aeiouAEIOU", *string) ? "un" : "una")


/* memory utils **********************************************************/


#define CREATE(result, type, number)  do {\
	if (!((result) = (type *) calloc ((number), sizeof(type))))\
		{ perror("malloc failure"); abort(); } } while(0)

#define RECREATE(result,type,number) do {\
  if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
		{ perror("realloc failure"); abort(); } } while(0)

/*
 * the source previously used the same code in many places to remove an item
 * from a list: if it's the list head, change the head, else traverse the
 * list looking for the item before the one to be removed.  Now, we have a
 * macro to do this.  To use, just make sure that there is a variable 'temp'
 * declared as the same type as the list to be manipulated.  BTW, this is
 * a great application for C++ templates but, alas, this is not C++.  Maybe
 * CircleMUD 4.0 will be...
 */
#define REMOVE_FROM_LIST(item, head, next)	\
   if ((item) == (head))		\
      head = (item)->next;		\
   else {				\
      temp = head;			\
      while (temp && (temp->next != (item))) \
	 temp = temp->next;		\
      if (temp)				\
         temp->next = (item)->next;	\
   }					\


/* basic bitvector utils *************************************************/


#define ASSASSIN(ch)  ((ch)->char_specials.assassin)
#define AS_ROOM(ch)   ((ch)->player_specials->saved.as_room)
#define AS_RNUM(ch)   ((ch)->player_specials->saved.as_rnum)
#define AS_MOB_ROOM(ch)  (ASSASSIN(ch)->in_room)
#define AS_MOB_RNUM(ch)  (ASSASSIN(ch)->nr)



#define IS_SET(flag,bit)  ((flag) & ((long long int) (bit)) )
#define SET_BIT(var,bit)  ((var) |= ((long long int) (bit)) )
#define REMOVE_BIT(var,bit)  ((var) &= ~((long long int) (bit)) )
#define TOGGLE_BIT(var,bit) ((var) = (var) ^ ((long long int) (bit)) )

#define MOB_FLAGS(ch) ((ch)->char_specials.saved.act)
#define PLR_FLAGS(ch) ((ch)->char_specials.saved.act)
#define PRF_FLAGS(ch) ((ch)->player_specials->saved.pref)

/* Perche' non un'unica, bella macro AFF_FLAGS (ch, n)? Per compatibilita',
   e' ovvio! (come pure AFF_FLAGGED) */
#define AFF_FLAGS(ch) ((ch)->char_specials.saved.affected_by[0])
#define AFF_FLAGS_S(ch, n) ((ch)->char_specials.saved.affected_by[n])
#define ROOM_FLAGS(loc) (world[(loc)].room_flags)

#define IS_NPC(ch)  (IS_SET(MOB_FLAGS(ch), MOB_ISNPC))
#define IS_MOB(ch)  (IS_NPC(ch) && ((ch)->nr >-1))
#define IS_CRIMINAL(ch) (PLR_FLAGGED(ch,PLR_KILLER)||PLR_FLAGGED(ch,PLR_TKILLER)||PLR_FLAGGED(ch,PLR_WAR_KILLER)||PLR_FLAGGED(ch,PLR_THIEF)||PLR_FLAGGED(ch,PLR_WANTED))

#define IS_QUESTOR(ch)  (PLR_FLAGGED(ch, PLR_QUESTOR))

#define MOB_FLAGGED(ch, flag) (IS_NPC(ch) && IS_SET(MOB_FLAGS(ch), (flag)))
#define PLR_FLAGGED(ch, flag) (!IS_NPC(ch) && IS_SET(PLR_FLAGS(ch), (flag)))
#define AFF_FLAGGED(ch, flag) (AFFECTED ((ch), (flag), 0))
#define PRF_FLAGGED(ch, flag) (IS_SET(PRF_FLAGS(ch), (flag)))
#define ROOM_FLAGGED(loc, flag) (IS_SET(ROOM_FLAGS(loc), (flag)))
/* AFFECTED e' l'equivalente di AFF_FLAGGED ma specifica il bitvector in cui salvarlo,
   per avere 4 bitvector e quindi 256 flags */
#define AFFECTED(ch, flag, file) (IS_SET ((AFF_FLAGS_S(ch, file)), (flag)))

/* IS_AFFECTED for backwards compatibility */
#define IS_AFFECTED(ch, skill) (AFF_FLAGGED((ch), (skill)))

#define PLR_TOG_CHK(ch,flag) ((TOGGLE_BIT(PLR_FLAGS(ch), (flag))) & (flag))
#define PRF_TOG_CHK(ch,flag) ((TOGGLE_BIT(PRF_FLAGS(ch), (flag))) & (flag))


/* room utils ************************************************************/


#define SECT(room)	(world[(room)].sector_type)

#define IS_DARK(room)  ( !world[room].light && \
                         (ROOM_FLAGGED(room, ROOM_DARK) || \
                          ( ( SECT(room) != SECT_INSIDE && \
                              SECT(room) != SECT_CITY ) && \
                            (sunlight == SUN_SET || \
			                       sunlight == SUN_DARK)) ) )

#define IS_LIGHT(room)  (!IS_DARK(room))

#define GET_ROOM_SPEC(room) ((room) >= 0 ? world[(room)].func : NULL)
#define GET_ROOM_VNUM(room) ((room)->number)
/* char utils ************************************************************/


#define IN_ROOM(ch)	((ch)->in_room)
#define GET_WAS_IN(ch)	((ch)->was_in_room)
#define GET_AGE(ch)     (age(ch).year)

#define GET_NAME(ch)     (IS_NPC(ch) ? \
			   (ch)->player.short_descr : (ch)->player.name)
#define GET_CLASS_TITLE(ch) ((ch)->player_specials->saved.class_title)
#define GET_STATO_SOCIALE(ch) ((ch)->player_specials->saved.stato_sociale)
#define GET_TITLE(ch)    ((ch)->player.title)
#define GET_LEVEL(ch)    ((ch)->player.level)
#define GET_PASSWD(ch)	 ((ch)->player.passwd)
#define POOFIN(ch)       ((ch)->player.poofin)
#define POOFOUT(ch)      ((ch)->player.poofout)
#define GET_PFILEPOS(ch) ((ch)->pfilepos)
//PEPPEDISGUISE
#define GET_DESCR(ch)    ((ch)->player.description)
#define GET_L_DESCR(ch)  ((ch)->player.long_descr)

/*
 * I wonder if this definition of GET_REAL_LEVEL should be the definition
 * of GET_LEVEL?  JE
 */
#define GET_REAL_LEVEL(ch) \
   (ch->desc && ch->desc->original ? GET_LEVEL(ch->desc->original) : \
    GET_LEVEL(ch))

#define GET_CLASS(ch)   ((ch)->player.class)
#define GET_HOME(ch)	((ch)->player.hometown)
#define GET_HEIGHT(ch)	((ch)->player.height)
#define GET_WEIGHT(ch)	((ch)->player.weight)
#define GET_SEX(ch)	((ch)->player.sex)
#define GET_MASTER(ch)  ((ch)->master)

#define GET_STR(ch)     ((ch)->aff_abils.str)
#define GET_ADD(ch)     ((ch)->aff_abils.str_add)
#define GET_DEX(ch)     ((ch)->aff_abils.dex)
#define GET_INT(ch)     ((ch)->aff_abils.intel)
#define GET_WIS(ch)     ((ch)->aff_abils.wis)
#define GET_CON(ch)     ((ch)->aff_abils.con)
#define GET_CHA(ch)     ((ch)->aff_abils.cha)

#define GET_EXP(ch)	  ((ch)->points.exp)
#define GET_FAMA(ch)      ((ch)->points.fama)
#define GET_NOTORIETA(ch) ((ch)->points.notorieta)
#define GET_AC(ch)        ((ch)->points.armor)
#define GET_HIT(ch)	  ((ch)->points.hit)
#define GET_ARENA_HIT(ch) ((ch)->player_specials->saved.arena_hit)
#define GET_MAX_HIT(ch)	  ((ch)->points.max_hit)
#define GET_MOVE(ch)	  ((ch)->points.move)
#define GET_MAX_MOVE(ch)  ((ch)->points.max_move)
#define GET_ARENA_MOVE(ch) ((ch)->player_specials->saved.arena_move)
#define GET_MANA(ch)	  ((ch)->points.mana)
#define GET_MAX_MANA(ch)  ((ch)->points.max_mana)
#define GET_ARENA_MANA(ch) ((ch)->player_specials->saved.arena_mana)
#define GET_ARENA_ROOM(ch) ((ch)->player_specials->saved.arena_room)
#define GET_GOLD(ch)	  ((ch)->points.gold)
#define GET_BANK_GOLD(ch) ((ch)->points.bank_gold)
#define GET_HITROLL(ch)	  ((ch)->points.hitroll)
#define GET_DAMROLL(ch)   ((ch)->points.damroll)
#define GET_AMT_BET(ch) ((ch)->player_specials->saved.bet_amt)
#define GET_BETTED_ON(ch) ((ch)->player_specials->saved.betted_on)

#define GET_RES_FUOCO(ch) ((ch)->resistenze.res_fuoco)
#define GET_RES_GHIACCIO(ch) ((ch)->resistenze.res_ghiaccio)
#define GET_RES_ELETTRICITA(ch) ((ch)->resistenze.res_elettricita)
#define GET_RES_ACIDO(ch) ((ch)->resistenze.res_acido)
#define GET_RES_SHAARR(ch) ((ch)->resistenze.res_shaarr)
#define GET_RES_XHYPHYS(ch) ((ch)->resistenze.res_xhyphys)
#define GET_RES_THERION(ch) ((ch)->resistenze.res_therion)
#define GET_RES_SILUE(ch) ((ch)->resistenze.res_silue)
#define GET_RES_FISICO(ch) ((ch)->resistenze.res_fisico)

#define GET_POS(ch)	  ((ch)->char_specials.position)
#define GET_IDNUM(ch)	  ((ch)->char_specials.saved.idnum)
#define GET_ID(x)         ((x)->id)
#define IS_CARRYING_W(ch) ((ch)->char_specials.carry_weight)
#define IS_CARRYING_N(ch) ((ch)->char_specials.carry_items)
#define FIGHTING(ch)	  ((ch)->char_specials.fighting)
#define HUNTING(ch)	  ((ch)->char_specials.hunting)
#define WILDHUNT(ch)      ((ch)->char_specials.wildhunt)
#define RIDING(ch)	  ((ch)->char_specials.riding)
#define RIDDEN_BY(ch)     ((ch)->char_specials.ridden_by)
#define TRANSPORTED(ch)       ((ch)->char_specials.transported)
#define TRANSPORTED_BY(ch)       ((ch)->char_specials.transported_by)
#define GET_CLIENT(ch)	  ((ch)->char_specials.client) //Orione
#define GET_SAVE(ch, i)	  ((ch)->char_specials.saved.apply_saving_throw[i])
#define GET_ALIGNMENT(ch) ((ch)->char_specials.saved.alignment)

#define GET_TIME_CATARSI(ch) ((ch)->char_specials.catarsi)
#define GET_SENZA_DORMIRE(ch) ((ch)->char_specials.secondi_senza_dormire)

//#define GET_COND(ch, i)		((ch)->player_specials->saved.conditions[(i)])

#define GET_COND(ch,i)	(IS_NPC(ch) ? \
				     ( (ch)->mob_specials.conditions[(i)]) \
					: ((ch)->player_specials->saved.conditions[(i)]) )

#define GET_LOADROOM(ch)	((ch)->player_specials->saved.load_room)
#define GET_PRACTICES(ch)	((ch)->player_specials->saved.spells_to_learn)
#define GET_ABIL_PRACS(ch)  ((ch)->player_specials->saved.abilita_to_learn)
#define GET_GOHOME(ch)		((ch)->player_specials->saved.gohome)
#define GET_QUESTMRR(ch)	((ch)->player_specials->saved.questmrr)	//Orione
#define GET_QUESTGIVER(ch)	((ch)->player_specials->saved.questgiver)
#define GET_QUESTOBJ(ch)	((ch)->player_specials->saved.questobj)
#define GET_QUESTPOINTS(ch)	((ch)->player_specials->saved.questpoints)
#define GET_TIPOQUEST(ch)	((ch)->player_specials->saved.tipoquest) //Orione
#define GET_COUNTDOWN(ch)	((ch)->player_specials->saved.countdown)

#define GET_INVIS_LEV(ch)	((ch)->player_specials->saved.invis_level)
#define GET_WIMP_LEV(ch)	((ch)->player_specials->saved.wimp_level)
#define GET_FREEZE_LEV(ch)	((ch)->player_specials->saved.freeze_level)
#define GET_BAD_PWS(ch)		((ch)->player_specials->saved.bad_pws)
#define GET_ASHOP(ch)           ((ch)->player_specials->saved.shop)
#define GET_TALK(ch, i)		((ch)->player_specials->saved.talks[i])
#define GET_LAST_OLC_TARG(ch)	((ch)->player_specials->last_olc_targ)
#define GET_LAST_OLC_MODE(ch)	((ch)->player_specials->last_olc_mode)
#define GET_ALIASES(ch)		((ch)->player_specials->aliases)
#define GET_LAST_TELL(ch)	((ch)->player_specials->last_tell)
#define GET_DURATA_COMB(ch)     ((ch)->player_specials->durata_comb) //PEPPE COMB

#define GET_TELLS(ch, number)   ((ch)->player_specials->previous_tells[number])

#define GET_SKILL(ch, i)	((ch)->player_specials->saved.skills[i])
#define SET_SKILL(ch, i, pct)	{ (ch)->player_specials->saved.skills[i] = pct; }

#define GET_PROF(ch, i)     	((ch)->player_specials->saved.proficienze[i])
#define SET_PROF(ch, i, pct)	{ (ch)->player_specials->saved.proficienze[i] = pct; }
#define GET_ABIL(ch, i)   	((ch)->player_specials->saved.abilita[i])
#define GET_ABIL_MOD(ch ,i)     abilita_vector[i][GET_ABIL(ch,i)]

#define CAN_LEARN_SKILL(ch, spell_num) (((GET_LEVEL((ch)) >= spell_info[(spell_num)].min_level[(int) GET_CLASS((ch))]))||(GET_LEVEL(ch)>=LVL_GOD))
#define CAN_LEARN_ABIL(ch, abil_num) ((abil_info[(abil_num)].class_users[(int) GET_CLASS((ch))]>=0)||(GET_LEVEL(ch)>=LVL_GOD))

#define SET_ABIL(ch, i, pct)	{ (ch)->player_specials->saved.abilita[i] = pct; }

#define GET_WILD_MAX_X_RANGE(ch)  ((ch)->player_specials->saved.wild_max_x_range)
#define GET_UNITA_ARMATE(ch)		((ch)->player_specials->saved.numero_unita_armate)
#define GET_WILD_MAX_Y_RANGE(ch)  ((ch)->player_specials->saved.wild_max_y_range)
//PEPPE RELIGIONE
#define GET_CULTO(ch)             ((ch)->player_specials->saved.culto)
#define GET_RESUSCITA(ch)         ((ch)->player_specials->saved.resuscita)
#define GET_EQ(ch, i)		  ((ch)->equipment[i])
#define GET_FIATO(ch)             ((ch)->player_specials->saved.fiato)  
#define GET_TCONCENTRATO(ch)             ((ch)->player_specials->saved.TeTconcentrazione)

#define GET_PESTE(ch)		  ((ch)->player_specials->saved.morte_peste)
#define GET_MORTI_DA_PG(ch)	  ((ch)->player_specials->saved.morti_da_pg)
#define GET_XP_GIORNALIERI(ch)	  ((ch)->player_specials->saved.xp_giornalieri)

#define GET_POINTS_EVENT(ch, i) ((ch)->points_event[i])
#define GET_DAMAGE_EVENTS(ch)   ((ch)->damage_events)
#define GET_POINTS_ACT_EVENTS(ch, i) ((ch)->points_act_event[i])

#define GET_MOB_ID(ch) ((ch)->mob_specials.idmob)
#define GET_MOB_SPEC(ch) (IS_MOB(ch) ? (mob_index[(ch->nr)].func) : NULL)
#define GET_MOB_RNUM(mob)	((mob)->nr)
#define GET_MOB_VNUM(mob)	(IS_MOB(mob) ? \
				 mob_index[GET_MOB_RNUM(mob)].virtual : -1)

#define GET_MOB_WAIT(ch)	((ch)->mob_specials.wait_state)
#define GET_DEFAULT_POS(ch)	((ch)->mob_specials.default_pos)
#define MEMORY(ch)		((ch)->mob_specials.memory)
#define GET_MOB_SAVED_MASTER(ch) ((ch)->mob_specials.savedmaster)

#define STRENGTH_APPLY_INDEX(ch) \
        ( ((GET_ADD(ch)==0) || (GET_STR(ch) != 18)) ? GET_STR(ch) :\
          (GET_ADD(ch) <= 50) ? 26 :( \
          (GET_ADD(ch) <= 75) ? 27 :( \
          (GET_ADD(ch) <= 90) ? 28 :( \
          (GET_ADD(ch) <= 99) ? 29 :  30 ) ) )                   \
        )
/*
#define CAN_CARRY_W(ch) (str_app[STRENGTH_APPLY_INDEX(ch)].carry_w)
#define CAN_CARRY_N(ch) (dex_app[GET_DEX(ch)].item_carry)
*/

#define CAN_CARRY_W(ch) ( \
	     ((GET_LEVEL(ch)==100) && (MOB_FLAGGED(ch, MOB_SHOPKEEPER))) ? 10000 : \
	     str_app[STRENGTH_APPLY_INDEX(ch)].carry_w)
#define CAN_CARRY_N(ch) ( \
	     ((GET_LEVEL(ch)==100) && (MOB_FLAGGED(ch, MOB_SHOPKEEPER))) ? 10000 : \
		 dex_app[GET_DEX(ch)].item_carry)

#define AWAKE(ch) (GET_POS(ch) > POS_SLEEPING)
#define CAN_SEE_IN_DARK(ch) \
   (AFF_FLAGGED(ch, AFF_INFRAVISION) || PRF_FLAGGED(ch, PRF_HOLYLIGHT))

#define IS_GOOD(ch)    (GET_ALIGNMENT(ch) >= 350)
#define IS_EVIL(ch)    (GET_ALIGNMENT(ch) <= -350)
#define IS_NEUTRAL(ch) (!IS_GOOD(ch) && !IS_EVIL(ch))
#define IS_TRUST(ch)   (PLR_FLAGGED(ch, PLR_TRUSTED))
#define LVL_BEF_TRUST(ch) ((ch)->player_specials->saved.trust_level)
#define IS_FLYING(ch)	(AFFECTED((ch), AFF_FLYING, BITV_FLYING) || AFFECTED((ch), AFF_LEVITATE, BITV_LEVITATE))


/* descriptor-based utils ************************************************/


#define WAIT_STATE(ch, cycle) { \
	if ((ch)->desc) (ch)->desc->wait = (cycle); \
	else if (IS_NPC(ch)) GET_MOB_WAIT(ch) = (cycle); }

#define CHECK_WAIT(ch)	(((ch)->desc) ? ((ch)->desc->wait > 1) : 0)
#define GET_WAIT_STATE(ch) (((ch)->desc) ? ( (ch)->desc->wait) : (GET_MOB_WAIT(ch)))

#define STATE(d)	((d)->connected)


/* object utils **********************************************************/

#define GET_OBJ_NAME(obj)	((obj)->name)
#define GET_OBJ_DESC(obj)     ((obj)->description)
#define GET_OBJ_TYPE(obj)	((obj)->obj_flags.type_flag)
#define GET_OBJ_COST(obj)	((obj)->obj_flags.cost)
#define GET_OBJ_RENT(obj)	((obj)->obj_flags.cost_per_day)
#define GET_OBJ_EXTRA(obj)	((obj)->obj_flags.extra_flags)
#define GET_OBJ_WEAR(obj)	((obj)->obj_flags.wear_flags)
#define GET_OBJ_VAL(obj, val)	((obj)->obj_flags.value[(val)])
#define GET_OBJ_WEIGHT(obj)	((obj)->obj_flags.weight)
#define GET_OBJ_TIMER(obj)	((obj)->obj_flags.timer)
#define GET_OBJ_RNUM(obj)	((obj)->item_number)
#define GET_OBJ_VNUM(obj)	(GET_OBJ_RNUM(obj) >= 0 ? \
				 obj_index[GET_OBJ_RNUM(obj)].virtual : -1)

#define GET_OBJ_CSLOTS(obj)     ((obj)->obj_flags.curr_slots)
#define GET_OBJ_TSLOTS(obj)     ((obj)->obj_flags.total_slots)
#define GET_OBJ_LEVEL(obj)      ((obj)->obj_flags.min_level)
#define GET_OBJ_BITVECTOR(obj, n)  ((obj)->obj_flags.bitvector[(n)])
#define GET_OBJ_MAT_TYPE(obj)   ((obj)->obj_flags.tipo_materiale)
#define GET_OBJ_MAT_NUM(obj)    ((obj)->obj_flags.num_materiale)
#define IS_OBJ_STAT(obj,stat)	(IS_SET((obj)->obj_flags.extra_flags,stat))

#define GET_OBJ_SPEC(obj) ((obj)->item_number >= 0 ? \
	(obj_index[(obj)->item_number].func) : NULL)

#define CAN_WEAR(obj, part) (IS_SET((obj)->obj_flags.wear_flags, (part)))

#define IS_PC_CORPSE(obj)	GET_OBJ_VAL (obj, 4) && IS_OBJ_STAT (obj, ITEM_ISCORPSE)

/* per contatore morti,uccisi,dethtrap **********************************/
#define GET_RIP_CNT(ch)         ((ch)->player_specials->saved.rip_cnt)
#define GET_KILL_CNT(ch)        ((ch)->player_specials->saved.kill_cnt)
#define GET_DT_CNT(ch)          ((ch)->player_specials->saved.dt_cnt)

/* compound utilities and other macros **********************************/


#define HSHR(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "":"") :"")
#define HSSH(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "" :"") : "")
#define HMHR(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "":"") : "")

#define ANA(obj) (strchr("aeiouyAEIOUY", *(obj)->name) ? "" : "")
#define SANA(obj) (strchr("aeiouyAEIOUY", *(obj)->name) ? "" : "")


/* Various macros building up to CAN_SEE */

#define LIGHT_OK(sub)	(!IS_AFFECTED(sub, AFF_BLIND) && \
   (IS_LIGHT((sub)->in_room) || IS_AFFECTED((sub), AFF_INFRAVISION)))

#define INVIS_OK(sub, obj) \
 ( (!IS_AFFECTED((obj), AFF_INVISIBLE) || IS_AFFECTED(sub, AFF_DETECT_INVIS)) \
   && (!IS_AFFECTED((obj), AFF_HIDE) || IS_AFFECTED(sub, AFF_SENSE_LIFE)) \
   && (!PRF_FLAGGED(obj, PRF_AFK)) \
   && (!PLR_FLAGGED(obj, PLR_FANTASMA)) )

#define MORT_CAN_SEE(sub, obj) (LIGHT_OK(sub) && INVIS_OK(sub, obj))

#define IMM_CAN_SEE(sub, obj) \
   (MORT_CAN_SEE(sub, obj) || PRF_FLAGGED(sub, PRF_HOLYLIGHT))

#define SELF(sub, obj)  ((sub) == (obj))

/* Can subject see character "obj"? */
#define CAN_SEE(sub, obj) (SELF(sub, obj) || \
   ((GET_REAL_LEVEL(sub) >= GET_INVIS_LEV(obj)) && IMM_CAN_SEE(sub, obj)))

/* End of CAN_SEE */


#define INVIS_OK_OBJ(sub, obj) \
  (!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || IS_AFFECTED((sub), AFF_DETECT_INVIS))

#define MORT_CAN_SEE_OBJ(sub, obj) (LIGHT_OK(sub) && INVIS_OK_OBJ(sub, obj))

#define CAN_SEE_OBJ(sub, obj) \
   (MORT_CAN_SEE_OBJ(sub, obj) || PRF_FLAGGED((sub), PRF_HOLYLIGHT))

#define MORT_CAN_SEE_OBJ_DARK(sub, obj) (((GET_OBJ_TYPE(obj) == ITEM_BENDA) || !IS_AFFECTED(sub, AFF_BLIND)) && INVIS_OK_OBJ(sub, obj))

#define CAN_SEE_OBJ_DARK(sub, obj) (MORT_CAN_SEE_OBJ_DARK(sub, obj) || PRF_FLAGGED((sub), PRF_HOLYLIGHT))

#define CAN_CARRY_OBJ(ch,obj)  \
   (((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) <= CAN_CARRY_W(ch)) &&   \
    ((IS_CARRYING_N(ch) + 1) <= CAN_CARRY_N(ch)))

#define CAN_GET_OBJ(ch, obj)   \
   (CAN_WEAR((obj), ITEM_WEAR_TAKE) && CAN_CARRY_OBJ((ch),(obj)) && \
    CAN_SEE_OBJ((ch),(obj)))

#define PERS(ch, vict)   (CAN_SEE(vict, ch) ? PERSCAMUF(ch) : "qualcuno")

#define PERSCAMUF(ch) (((AFF_FLAGGED(ch,AFF_DISGUISE) && ((ch)->player.namedisguise)) ? (ch)->player.namedisguise : GET_NAME(ch)))


#define OBJS(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? \
	(obj)->short_description  : "qualcosa")

#define OBJN(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? \
	fname((obj)->name) : "qualcosa")


#define EXIT(ch, door)  (world[(ch)->in_room].dir_option[door])
#define EXIT2(roomnum, door)   (world[(roomnum)].dir_option[door])

#define CAN_GO(ch, door) (EXIT(ch,door) && \
			 (EXIT(ch,door)->to_room != NOWHERE) && \
			 !IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
#define CAN_GO2(roomnum, door) (EXIT2(roomnum,door) && \
			 (EXIT2(roomnum,door)->to_room != NOWHERE) && \
			 !IS_SET(EXIT2(roomnum, door)->exit_info, EX_CLOSED))


#define CLASS_ABBR(ch) (IS_NPC(ch) ? "--" : class_abbrevs[(int)GET_CLASS(ch)])

#define IS_MAGIC_USER(ch)	(!IS_NPC(ch) && \
				(GET_CLASS(ch) == CLASS_MAGIC_USER))
#define IS_CLERIC(ch)		(!IS_NPC(ch) && \
				(GET_CLASS(ch) == CLASS_CLERIC))
#define IS_THIEF(ch)		(!IS_NPC(ch) && \
				(GET_CLASS(ch) == CLASS_THIEF))
#define IS_WARRIOR(ch)		(!IS_NPC(ch) && \
				(GET_CLASS(ch) == CLASS_WARRIOR))
#define IS_PELOI(ch)            (!IS_NPC(ch) && \
                                (GET_CLASS(ch) == CLASS_PELOI))
#define IS_DARESIANO(ch)        (!IS_NPC(ch) && (GET_CLASS(ch) == CLASS_DARESIANO))

#define OUTSIDE(ch) (!ROOM_FLAGGED((ch)->in_room, ROOM_INDOORS))

#define IS_MANOVALE(ch)		((GET_MOB_SPEC(ch))==manovale)

#define MANOVALE_OF(mano,ch)	(MASTER_ID(mano) == GET_IDNUM(ch))

#define GET_STIPENDIOMOB(ch)	((ch)->mob_specials.stipendiomob)
#define GET_PAGA(ch)		((ch)->mob_specials.paga)	
char *temp;
#define NAME(x) ((temp = get_name_by_id(x)) == NULL ? "<UNDEF>" : temp)

/* OS compatibility ******************************************************/


/* there could be some strange OS which doesn't have NULL... */
#ifndef NULL
#define NULL (void *)0
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(TRUE)
#define TRUE  (!FALSE)
#endif

/* defines for fseek */
#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

#if defined(NOCRYPT) || !defined(HAVE_CRYPT)
#define CRYPT(a,b) (a)
#else
#define CRYPT(a,b) ((char *) crypt((a),(b)))
#endif
#define SENDOK(ch) ((IS_NPC(ch) || (ch)->desc) && (AWAKE(ch) || sleep) && \
                    !PLR_FLAGGED((ch), PLR_WRITING))

/*Lance*/
#define SEARCH_MODE_NOTHING 0
#define SEARCH_MODE_PG  1
#define SEARCH_MODE_OBJ  2
#define SEARCH_MODE_ROOM  3
#define SEARCH_MODE_PATH  4
#define SEARCH_MODE_MOB 5

#define CMD_MODE_GO 0
#define CMD_MODE_PATROL 1

#define CMD_MODE(ch)  ((ch)->mob_specials.cmd_mode)
#define SEARCH_MODE(ch)  ((ch)->mob_specials.search_mode)
#define GO_PG_TARGET(ch) ((ch)->mob_specials.go_chtarget)
#define GO_NAME_TARGET(ch) ((ch)->mob_specials.go_nametarget)
#define GO_ROOM_TARGET(ch) ((ch)->mob_specials.go_room)
#define GO_PATH(ch) ((ch)->mob_specials.go_path)
#define POS_PATH(ch) ((ch)->mob_specials.pospath)
#define SEARCH_COMMAND(ch) ((ch)->mob_specials.searchcommand)
#define MASTER_ID(ch) ((ch)->mob_specials.masterid)
#define CLAN_ID(ch) ((ch)->mob_specials.clanid)
#define GET_PESCA_STATUS(ch) ((ch)->char_specials.pesca_stat)


#define GROUPNAME(ch) ((ch)->char_specials.groupname)

#define NEW_CLAN_ID(ch) ((ch)->mob_specials.newclanid)

#define WHOKILLED(ch) ((ch)->char_specials.whokilled)


 /* Chardhros: Abilita' Trappole */

#define TRAP_VUOTA	 0
#define TRAP_SCATTATA    1
#define TRAP_DANNO 	 2
#define TRAP_ALLARME	 3
#define TRAP_IMMOBILIZZA 4
#define TRAP_BASH	 5

#define GET_TRAP_TIPO(obj)    GET_OBJ_VAL((obj),0)
#define GET_TRAP_PERC(obj)    GET_OBJ_VAL((obj),1)
#define GET_TRAP_MISS(obj)    GET_OBJ_VAL((obj),2)
#define GET_TRAP_MIND(obj)    GET_OBJ_VAL((obj),3)
#define GET_TRAP_MAXD(obj)    GET_OBJ_VAL((obj),4)
#define GET_TRAP_LEVEL(obj)   GET_OBJ_VAL((obj),5)

#define IS_A_CRIMINAL_CLAN(ch, clan) \
         (((find_flag(ch, clan, CLAN_KILLER)>-1) \
              || (find_flag(ch, clan, CLAN_THIEF)>-1) \
	      || (find_flag(ch, clan, CLAN_WANTED)>-1) \
              || (find_flag(ch, clan, CLAN_TKILLER)>-1) \
              || (find_flag(ch, clan, CLAN_WKILLER)>-1) \
              || (find_flag(ch, clan, CLAN_STRAGE)>-1) \
              || (find_flag(ch, clan, CLAN_CORROTTO)>-1) \
              || (find_flag(ch, clan, CLAN_MULTA)>-1) \
         ))

#define FLAGGED_CRIME(ch, clan, flag) ((find_flag(ch, clan, flag)>-1))

/* Per i messaggi automatici  ai pg -Chardhros- */
#define MAIL_SINDACATO 	-2
#define MAIL_TASSE     	-3
#define MAIL_GUARDIE   	-4
#define MAIL_GILDA      -5
#define MAIL_NOBODY    	-6
/* Manovali -Chardhros- */


#define ORE_PAGA		168
#define STIPENDI		1000
#define LICENZIAMENTO_MANOVALE	36
#define PERCENTUALE_EQ		20

/* AutoQuest - Orione */
#define QUEST_NO 0
#define QUEST_MOBKILL 1
#define QUEST_GETITEM 2
#define QUEST_GIVITEM 3
#define QUEST_GOTROOM 4
#define QUEST_ARTICOL 5
#define NUM_QUEST 5 //Ultimo tipo di quest generabile
