/* ************************************************************************
*   File: interpreter.c                                 Part of CircleMUD *
*  Usage: parse user commands, search for specials, call ACMD functions   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __INTERPRETER_C__

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "spells.h"
#include "handler.h"
#include "mail.h"
#include "screen.h"
//#include "olc.h"
#include "dg_scripts.h"

extern const struct title_type titles[NUM_CLASSES][LVL_IMPL + 1];
extern char *motd;
extern char *imotd;
extern char *background;
extern char *MENU;
extern char *WELC_MESSG;
extern char *START_MESSG;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct player_index_element *player_table;
extern int top_of_p_table;
extern int restrict;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct room_data *world;
extern struct char_data *is_playing(char *vict_name);
extern struct char_data *last_ch; 
extern char last_command[MAX_STRING_LENGTH]; 
/* external functions */
void do_newbie(struct char_data *vict);
void echo_on(struct descriptor_data *d);
void echo_off(struct descriptor_data *d);
void do_start(struct char_data *ch);
void init_char(struct char_data *ch);
int create_entry(char *name);
int special(struct char_data *ch, int cmd, char *arg);
int isbanned(char *hostname);
int isunlocked(char *ip); //Orione
int Valid_Name(char *newname);
void oedit_parse(struct descriptor_data *d, char *arg);
void redit_parse(struct descriptor_data *d, char *arg);
void zedit_parse(struct descriptor_data *d, char *arg);
void medit_parse(struct descriptor_data *d, char *arg);
void sedit_parse(struct descriptor_data *d, char *arg);
void trigedit_parse(struct descriptor_data *d, char *arg);
void wedit_parse(struct descriptor_data *d, char *arg);
void read_aliases(struct char_data *ch);
void roll_real_abils(struct char_data *ch);
int is_same_ip(struct descriptor_data *dch);
extern void assign_to_regno(struct char_data *suddito,int choice);
extern void change_client(struct char_data *ch); //Orione

/* prototypes for all do_x functions. */
ACMD(do_abilita);
ACMD(do_abilset);
ACMD(do_acostruzione);
ACMD(do_action);
ACMD(do_advance);
ACMD(do_affila);
ACMD(do_agguato);
ACMD(do_ahall);
ACMD(do_alias);
ACMD(do_ammaestra);
ACMD(do_amob);
ACMD(do_arena);
ACMD(do_assist);
ACMD(do_at);
ACMD(do_awho);
ACMD(do_attributes);
ACMD(do_autoquest);
ACMD(do_auction);
ACMD(do_auctioneer);
//ACMD(do_ashop);
ACMD(do_bid);
ACMD(do_backstab);
ACMD(do_ban);
ACMD(do_bar);
ACMD(do_bash);
ACMD(do_bet);
ACMD(do_brew);
ACMD(do_caccia);
ACMD(do_camp);
ACMD(do_cast);
ACMD(do_chaos);
ACMD(do_clan);
ACMD(do_ctell);
ACMD(do_color);
ACMD(do_commands);
ACMD(do_consider);
ACMD(do_costruisci);
ACMD(do_credits);
ACMD(do_chown);
ACMD(do_date);
ACMD(do_dc);
ACMD(do_diagnose);
ACMD(do_disarm);
ACMD(do_display);
ACMD(do_doma);
ACMD(do_dragonbreath);
ACMD(do_dragonfly);
ACMD(do_drink);
ACMD(do_drop);
ACMD(do_eat);
ACMD(do_echo);
ACMD(do_enter);
//ACMD(do_elenca);
ACMD(do_equipment);
ACMD(do_evadi);
ACMD(do_examine);
ACMD(do_exit);
ACMD(do_exits);
ACMD(do_fedina);
ACMD(do_flee);
ACMD(do_fillet);
ACMD(do_follow);
ACMD(do_force);
ACMD(do_forge);
ACMD(do_freedragon);
ACMD(do_gecho);
ACMD(do_gen_comm);
ACMD(do_gen_door);
ACMD(do_gen_ps);
ACMD(do_gen_tog);
ACMD(do_gen_write);
ACMD(do_get);
ACMD(do_give);
ACMD(do_givehome);
ACMD(do_godnet);
ACMD(do_gold);
ACMD(do_gohome);
ACMD(do_gostop);
ACMD(do_goto);
ACMD(do_grab);
ACMD(do_group);
ACMD(do_gsay);
ACMD(do_gtell);
ACMD(do_hcontrol);
ACMD(do_help);
ACMD(do_hire);
ACMD(do_hide);
ACMD(do_hit);
ACMD(do_hiscore);
ACMD(do_horizon);
ACMD(do_house);
ACMD(do_incanta);
ACMD(do_info);
ACMD(do_insegna);
ACMD(do_hang);
ACMD(do_insult);
ACMD(do_inventory);
ACMD(do_invis);
ACMD(do_kick);
ACMD(do_kill);
ACMD(do_land);
ACMD(do_last);
ACMD(do_lavora);
ACMD(do_leave);
ACMD(do_levels);
ACMD(do_list_groups);
ACMD(do_listprac);
ACMD(do_listprof);
ACMD(do_load);
ACMD(do_look);
ACMD(do_mestieri);
ACMD(do_mobgo);
ACMD(do_monta);
ACMD(do_move);
ACMD(do_mlist);
ACMD(do_new_clan);
ACMD(do_not_here);
//ACMD(do_notifica);
ACMD(do_offer);
//ACMD(do_olc);
ACMD(do_order);
ACMD(do_olist);
ACMD(do_page);
ACMD(do_peep);
ACMD(do_pesca);
ACMD(do_players);
ACMD(do_poofset);
ACMD(do_pour);
ACMD(do_practice);
ACMD(do_proficienze);
ACMD(do_profset);
ACMD(do_pull);
ACMD(do_punch);
ACMD(do_purge);
ACMD(do_put);
ACMD(do_qcomm);
ACMD(do_quit);
ACMD(do_reboot);
ACMD(do_recalldragon);
ACMD(do_regni);
ACMD(do_remove);
ACMD(do_rent);
ACMD(do_reply);
ACMD(do_repair);
ACMD(do_report);
ACMD(do_rescue);
ACMD(do_rest);
ACMD(do_retreat);
ACMD(do_restore);
ACMD(do_return);
ACMD(do_rlist);
ACMD(do_save);
ACMD(do_say);
ACMD(do_sbloccaip); //Orione
ACMD(do_scan);
ACMD(do_scopriti);
ACMD(do_scarica);
ACMD(do_score);
ACMD(do_scribe);
ACMD(do_scrolla);
ACMD(do_send);
ACMD(do_search);
ACMD(do_set);
ACMD(do_sgancia);
ACMD(do_shoot);
ACMD(do_shoplist_pg);
ACMD(do_show);
//ACMD(do_shsave);
//ACMD(do_shedit);
ACMD(do_shutdown);
ACMD(do_sit);
ACMD(do_skillset);
ACMD(do_slay);
ACMD(do_sleep);
ACMD(do_smonta);
ACMD(do_sneak);
ACMD(do_snoop);
ACMD(do_spec_comm);
ACMD(do_split);
ACMD(do_spot);
ACMD(do_spy);
ACMD(do_stand);
ACMD(do_stat);
ACMD(do_steal);
ACMD(do_stop_auction);
ACMD(do_stopfight);
ACMD(do_switch);
ACMD(do_syslog);
ACMD(do_takehome);
ACMD(do_takemob);
//ACMD(do_takeshop);
ACMD(do_tele);
ACMD(do_teleport);
ACMD(do_tedit);
ACMD(do_tell);
ACMD(do_throw);
ACMD(do_time);
ACMD(do_title);
ACMD(do_toggle);
ACMD(do_track);
ACMD(do_trans);
ACMD(do_trappola);
ACMD(do_trust);
ACMD(do_unban);
ACMD(do_ungroup);
ACMD(do_use);
ACMD(do_users);
ACMD(do_visible);
ACMD(do_vnum);
ACMD(do_vstat);
ACMD(do_wake);
ACMD(do_wear);
ACMD(do_weather);
ACMD(do_where);
ACMD(do_who);
ACMD(do_wield);
ACMD(do_wildx);
ACMD(do_wildy);
ACMD(do_wimpy);
ACMD(do_wizlock);
ACMD(do_wiznet);
ACMD(do_wizutil);
ACMD(do_write);
ACMD(do_write_magic);
ACMD(do_zonexit);
ACMD(do_zreset);
/* DG Script ACMD's */
ACMD(do_attach);
ACMD(do_detach);
ACMD(do_tlist);
ACMD(do_tstat);
ACMD(do_masound);
ACMD(do_mkill);
ACMD(do_mjunk);
ACMD(do_mdoor);		//Orione
ACMD(do_mechoaround);
ACMD(do_msend);
ACMD(do_mecho);
ACMD(do_mload);
ACMD(do_mpurge);
ACMD(do_mgoto);
ACMD(do_mat);
ACMD(do_mteleport);
ACMD(do_mforce);
ACMD(do_mexp);
ACMD(do_mgold);
ACMD(do_mhunt);		//Orione
ACMD(do_mremember);	//Orione
ACMD(do_mforget);		//Orione
ACMD(do_mtransform);	//Orione
/*ACMD(do_interwho);
ACMD(do_interwiz);
ACMD(do_intertel);
ACMD(do_interpage);
ACMD(do_intermud);
ACMD(do_mudinfo);
ACMD(do_mudlist);*/
ACMD(do_free);
ACMD(do_namechange);

ACMD(do_regnopay);
ACMD(do_gildapay);
//PEPPE RELIGIONE
ACMD(do_resuscita);
//PEPPE CRITICAL
ACMD(do_rigenera);
ACMD(do_stato);
ACMD(do_rinnova); //Orione

ACMD(do_rinomina);
ACMD(do_rinomina_alias);
ACMD(do_rinomina_descr);
ACMD(do_rinomina_name);

ACMD(do_colpo_mortale);
ACMD(do_assegna);
ACMD(do_tornado);
ACMD(do_medita);


ACMD(do_catarsi);
ACMD(do_control_ooc);
ACMD(do_rigenerazione);
ACMD(do_ammala);
ACMD(do_mescola);
ACMD(do_esercito);
ACMD(do_ltitolo);
ACMD(do_religione);
ACMD(do_pulisci_pg);

/* This is the Master Command List(tm).

 * You can put new commands in, take commands out, change the order
 * they appear in, etc.  You can adjust the "priority" of commands
 * simply by changing the order they appear in the command list.
 * (For example, if you want "as" to mean "assist" instead of "ask",
 * just put "assist" above "ask" in the Master Command List(tm).
 *
 * In general, utility commands such as "at" should have high priority;
 * infrequently used and dangerously destructive commands should have low
 * priority.
 */

const struct command_info cmd_info[] = {
	{ "RESERVED", 0, 0, 0, 0 },	/* this must be first -- for specprocs */
	
  /* directions must come before other commands but after RESERVED */
	{ "north"    , POS_STANDING, do_move     , 0, SCMD_NORTH },
	{ "east"     , POS_STANDING, do_move     , 0, SCMD_EAST },
	{ "south"    , POS_STANDING, do_move     , 0, SCMD_SOUTH },
	{ "west"     , POS_STANDING, do_move     , 0, SCMD_WEST },
	{ "up"       , POS_STANDING, do_move     , 0, SCMD_UP },
	{ "down"     , POS_STANDING, do_move     , 0, SCMD_DOWN },
	{ "nord"     , POS_STANDING, do_move     , 0, SCMD_NORTH },
	{ "est"      , POS_STANDING, do_move     , 0, SCMD_EAST },
	{ "sud"      , POS_STANDING, do_move     , 0, SCMD_SOUTH },
	{ "ovest"    , POS_STANDING, do_move     , 0, SCMD_WEST },
	{ "alto"     , POS_STANDING, do_move     , 0, SCMD_UP },
	{ "basso"    , POS_STANDING, do_move     , 0, SCMD_DOWN },
	
	
  /* now, the main list */
	{ "'"        , POS_RESTING , do_say      , 0, 0 },
	{ ":"        , POS_RESTING , do_echo     , 1, SCMD_EMOTE },
	{ ";"        , POS_DEAD    , do_wiznet   , LVL_BUILDER, 0 },
	{ "<"        , POS_DEAD    , do_godnet   , LVL_AVATAR, 0 },
	
 /* lettera A */
	
	{ "alias"         , POS_DEAD    , do_alias    , 0, 0 },
	{ "abbandonare"   , POS_DEAD    , do_quit     , 0, SCMD_QUIT },
	{ "abilita"       , POS_DEAD    , do_abilita  , 0, 0 },
	{ "abilset"       , POS_SLEEPING, do_abilset  , LVL_BUILDER, 0 },
	{ "abbraccia"     , POS_RESTING , do_action   , 0, 0 },
	{ "accampati"     , POS_STANDING, do_camp     , 1, 0 },
	{ "accarezza"     , POS_RESTING , do_action   , 0, 0 },
	{ "accigliati"    , POS_RESTING , do_action   , 0, 0 },
	{ "acclama"       , POS_RESTING , do_action   , 0, 0 },
	{ "accogli"	  , POS_STANDING, do_religione, 0, 0 },
	{ "accoccolati"   , POS_RESTING , do_action   , 0, 0 },
	{ "accuse"        , POS_SITTING , do_action   , 0, 0 },
	{ "accusa"        , POS_SITTING , do_action   , 0, 0 },
	{ "achi"          , POS_DEAD    , do_awho     , 0, 0 },
	{ "adirati"       , POS_RESTING , do_action   , 0, 0 },
	{ "adora"         , POS_RESTING , do_action   , 0, 0 },
	{ "advance"       , POS_DEAD    , do_advance  , LVL_IMPL, 0 },
	{ "affila"        , POS_STANDING, do_affila   , 0, 0 },
	{ "afk"           , POS_DEAD    , do_gen_tog  , 0, SCMD_AFK},
	{ "afferra"       , POS_RESTING , do_grab     , 0, 0 },
	{ "affitta"       , POS_STANDING, do_not_here , 1, 0 },
	{ "aggancia"      , POS_STANDING, do_hang     , 1, 0 },
	{ "agguato"       , POS_STANDING, do_agguato  , 1, 0 },
	{ "ahall"         , POS_DEAD    , do_ahall    , 0, 0 },
	{ "aiuto"         , POS_DEAD    , do_help     , 0, 0 },
	{ "alzati"        , POS_RESTING , do_stand    , 0, 0 },
	{ "amici"         , POS_DEAD    , do_gen_ps   , 0, SCMD_WHOAMI },
	{ "ammaestra"     , POS_STANDING , do_ammaestra   , 0, 0 },
	{ "ammala"	  , POS_RESTING , do_ammala   , LVL_QUEST_MASTER, 0 },
	{ "ammicca"       , POS_RESTING , do_action   , 0, 0 },
	{ "ama"           , POS_RESTING , do_action   , 0, 0 },
	{ "amob"          , POS_STANDING, do_amob    , LVL_BUILDER, 0 },
	{ "annuisci"      , POS_RESTING , do_action   , 0, 0 },
	{ "ancheggia"     , POS_STANDING, do_action   , 0, 0 },
	{ "ansima"        , POS_RESTING , do_action   , 0, 0 },
	{ "apri"          , POS_SITTING , do_gen_door , 0, SCMD_OPEN },
	{ "applaudi"      , POS_RESTING , do_action   , 0, 0 },
	{ "arena"         , POS_STANDING, do_arena    , 0, 0 },
	{ "arinfo"        , POS_SLEEPING, do_gen_tog  , 0, SCMD_ARENA },
	{ "arresta"       , POS_DEAD  , do_stopfight  , 0, 0 },
	{ "arrossisci"    , POS_RESTING , do_action   , 0, 0 },
	{ "arruffa"       , POS_STANDING, do_action   , 0, 0 },
	{ "assegna"         , POS_RESTING , do_not_here , 0, 0 },	
	{ "assisti"       , POS_FIGHTING, do_assist   , 1, 0 },
//	{ "ashop"         , POS_STANDING, do_ashop    , LVL_BUILDER, 0 },
	{ "ask"           , POS_RESTING , do_spec_comm, 0, SCMD_ASK },
	{ "assaggia"      , POS_RESTING , do_eat      , 0, SCMD_TASTE },
	{ "assassina"     , POS_FIGHTING, do_hit      , 0, SCMD_MURDER },
	{ "asta"          , POS_SLEEPING, do_auction  , LVL_QUEST_MASTER, 0 },
	{ "at"            , POS_DEAD    , do_at       , LVL_QUEST_MASTER, 0 },
	{ "attributes"    , POS_RESTING , do_attributes,0, 0 },
	{ "attributi"     , POS_RESTING , do_attributes,0, 0 },
	{ "atterra"       , POS_STANDING, do_land     , 0, 0 },
	{ "auction"       , POS_SLEEPING, do_auction  , LVL_QUEST_MASTER, 0 },
	{ "auctalk"       , POS_SLEEPING, do_gen_comm , 0, SCMD_AUCTION },
	{ "auctioner"     , POS_DEAD    , do_auctioneer, LVL_BUILDER, 0 },
	{ "autodir"       , POS_DEAD    , do_gen_tog  , 0, SCMD_AUTODIR },
	{ "autoexit"      , POS_DEAD    , do_gen_tog  , 0, SCMD_AUTOEXIT },
	{ "autoquest"     , POS_DEAD    , do_autoquest, 10, 0 },
	{ "avanza"        , POS_DEAD    , do_advance  , LVL_IMPL, 0 },
	{ "awho"          , POS_DEAD    , do_awho     , 0, 0 },
	{ "azzanna"       , POS_RESTING , do_action   , 0, 0 },
	
	
  /* lettera B */
	
	{ "backstab"      , POS_STANDING, do_backstab , 1, 0 },
	{ "bacia"         , POS_RESTING , do_action   , 0, 0 },
	{ "baco"          , POS_DEAD    , do_gen_write, 0, SCMD_BUG },
	{ "bilancio"       , POS_STANDING, do_not_here , 1, 0 },
	{ "ban"           , POS_DEAD    , do_ban      , LVL_BUILDER, 0 },
	{ "bar"           , POS_DEAD    , do_bar      , 0, 0 },
	{ "barra"         , POS_DEAD    , do_bar      , 0, 0 },
	{ "bash"          , POS_FIGHTING, do_bash     , 1, 0 },
	{ "beg"           , POS_RESTING , do_action   , 0, 0 },
	{ "bet"           , POS_DEAD    , do_bet	   , 0, 0 },
	{ "bevi"          , POS_RESTING , do_drink    , 0, SCMD_DRINK },
	{ "bid"           , POS_SLEEPING, do_bid	   , 0, 0 },
	{ "bilancio"      , POS_STANDING, do_not_here , 1, 0 },
	{ "bleed"         , POS_RESTING , do_action   , 0, 0 },
	{ "blocca"        , POS_SITTING , do_gen_door , 0, SCMD_LOCK },
	{ "blush"         , POS_RESTING , do_action   , 0, 0 },
	{ "bollisci"      , POS_RESTING , do_action   , 0, 0 },
	{ "bonifico"      , POS_STANDING, do_not_here , 0, 0 },
	{ "bonk"          , POS_STANDING, do_action   , 0, 0 },
	{ "botta"         , POS_STANDING, do_action   , 0, 0 },
	{ "bounce"        , POS_STANDING, do_action   , 0, 0 },
	{ "bow"           , POS_STANDING, do_action   , 0, 0 },
	{ "brancola"      , POS_RESTING , do_action   , 0, 0 },
	{ "brb"           , POS_RESTING , do_action   , 0, 0 },
	{ "breve"         , POS_DEAD    , do_gen_tog  , 0, SCMD_BRIEF },
  //{ "brew"          , POS_STANDING, do_brew     , 0, 0 },
	{ "brief"         , POS_DEAD    , do_gen_tog  , 0, SCMD_BRIEF },
	{ "broncio"       , POS_RESTING , do_action   , 0, 0 },
	{ "buffetto"      , POS_STANDING, do_action   , 0, 0 },
	{ "bug"           , POS_DEAD    , do_gen_write, 0, SCMD_BUG },
	{ "burp"          , POS_RESTING , do_action   , 0, 0 },
	{ "buy"           , POS_STANDING, do_not_here , 0, 0 },
	
  /* lettera C */
	
	{ "cast"          , POS_FIGHTING , do_cast     , 1, 0 },
	{ "caccia"        , POS_STANDING , do_caccia   , 0, 0 },
	{ "cackle"        , POS_RESTING , do_action   , 0, 0 },
	{ "calcio"        , POS_FIGHTING, do_kick     , 1, 0 },
	{ "camp"          , POS_STANDING, do_camp     , 1, 0 },
	{ "cancella"      , POS_DEAD    , do_gen_ps   , 0, SCMD_CLEAR },
	{ "canta"         , POS_RESTING , do_action   , 0, 0 },
	{ "caos"          , POS_STANDING, do_chaos    , LVL_QUEST_MASTER, 0 },
	{ "carica"        , POS_STANDING, do_give     , 1, SCMD_LOAD},
	{ "casa"          , POS_STANDING, do_house    , 1, 0 },
	//{ "cassa"         , POS_STANDING, do_not_here , 0, 0 },
  { "rdici"         , POS_SLEEPING, do_ctell    , 0, 0 },
        { "catarsi"       , POS_RESTING, do_catarsi  , 1 , 0 },
	{ "cerca"         , POS_STANDING, do_search   , 0, 0,},
	{ "chaos"         , POS_STANDING, do_chaos    , LVL_QUEST_MASTER, 0 },
	{ "check"         , POS_STANDING, do_not_here , 1, 0 },
	{ "chi"           , POS_DEAD    , do_who      , 0, 0 },
	{ "chiedi"        , POS_RESTING , do_spec_comm, 0, SCMD_ASK },
	{ "chioccia"      , POS_RESTING , do_action   , 0, 0 },
	{ "chiudi"        , POS_SITTING , do_gen_door , 0, SCMD_CLOSE },
	{ "chown"         , POS_STANDING, do_chown    , LVL_QUEST_MASTER, 0 },
	{ "chuckle"       , POS_RESTING , do_action   , 0, 0 },
	{ "ciao"          , POS_RESTING , do_action   , 0, 0 },
	{ "civetta"       , POS_RESTING , do_action   , 0, 0 },
	{ "clap"          , POS_RESTING , do_action   , 0, 0 },
	{ "clear"         , POS_DEAD    , do_gen_ps   , 0, SCMD_CLEAR },
//	{ "client"	      , POS_DEAD	, change_client, 0, 0 },
	{ "close"         , POS_SITTING , do_gen_door , 0, SCMD_CLOSE },
	{ "cls"           , POS_DEAD    , do_gen_ps   , 0, SCMD_CLEAR },
	{ "coccola"       , POS_RESTING , do_action   , 0, 0 },
	{ "colore"        , POS_DEAD    , do_color    , 0, 0 },
	{ "colpisci"      , POS_FIGHTING, do_hit      , 0, SCMD_HIT },
	{ "colpetto"      , POS_RESTING , do_action   , 0, 0 },
	{ "comandi"       , POS_DEAD    , do_commands , 0, SCMD_COMMANDS },
	{ "comb"          , POS_RESTING , do_action   , 0, 0 },
	{ "comfort"       , POS_RESTING , do_action   , 0, 0 },
	{ "commands"      , POS_DEAD    , do_commands , 0, SCMD_COMMANDS },
	{ "compact"       , POS_DEAD    , do_gen_tog  , 0, SCMD_COMPACT },
	{ "compatto"      , POS_DEAD    , do_gen_tog  , 0, SCMD_COMPACT },
	//{ "componi"       , POS_STANDING, do_scribe   , 0, 0 },
	{ "compra"        , POS_STANDING, do_not_here , 0, 0 },
	{ "conia"         , POS_STANDING, do_not_here , 0, 0 },
	//PEPPE EVADI
	{ "consegna" 	  , POS_STANDING, do_not_here , 1, 0 },
	{ "considera"     , POS_RESTING , do_consider , 0, 0 },
	{ "conforto"      , POS_RESTING , do_action   , 0, 0 },
	{ "congelo"       , POS_DEAD    , do_wizutil  , LVL_IMPLEMENTOR, SCMD_FREEZE },
	{ "contrasto"     , POS_RESTING , do_action   , 0, 0 },
	{ "controlla"     , POS_STANDING, do_not_here , 1, 0 },
	{ "costruisci"    , POS_STANDING, do_costruisci , 1, 0 },
	{ "cough"         , POS_RESTING , do_action   , 0, 0 },
	{ "crediti"       , POS_DEAD    , do_gen_ps   , 0, SCMD_CREDITS },
	{ "credits"       , POS_DEAD    , do_gen_ps   , 0, SCMD_CREDITS },
	{ "cringe"        , POS_RESTING , do_action   , 0, 0 },
	{ "cry"           , POS_RESTING , do_action   , 0, 0 },
  { "rtell"         , POS_SLEEPING, do_ctell    , 0, 0 },
	{ "cuci"          , POS_STANDING, do_lavora   , 0, 0 },
	{ "cucina"        , POS_STANDING, do_lavora   , 0, 0 },
	{ "cuddle"        , POS_RESTING , do_action   , 0, 0 },
	{ "curse"         , POS_RESTING , do_action   , 0, 0 },
	{ "curtsey"       , POS_STANDING, do_action   , 0, 0 },
	
  /* lettera D */
	
	{ "dai"           , POS_RESTING , do_give     , 0, 0 },
	{ "dance"         , POS_STANDING, do_action   , 0, 0 },
	{ "danza"         , POS_STANDING, do_action   , 0, 0 },
	{ "date"          , POS_DEAD    , do_date     , LVL_IMPLEMENTOR, SCMD_DATE },
	{ "daydream"      , POS_SLEEPING, do_action   , 0, 0 },
	{ "dc"            , POS_DEAD    , do_dc       , LVL_BUILDER, 0 },
	{ "deathblow"     , POS_FIGHTING, do_colpo_mortale , 1, 0 },	

	{ "deposita"      , POS_STANDING, do_not_here , 1, 0 },
	{ "dici"          , POS_DEAD    , do_tell     , 0, 0 },
	{ "diagnose"      , POS_RESTING , do_diagnose , 0, 0 },
	{ "diagnosi"      , POS_RESTING , do_diagnose , 0, 0 },
	{ "disarma"       , POS_FIGHTING, do_disarm   , 0, 0 },
	{ "display"       , POS_DEAD    , do_display  , 0, 0 },
	{ "disprezza"     , POS_RESTING , do_action   , 0, 0 },
//	{ "distruggi"     , POS_RESTING , do_drop     , 0, SCMD_JUNK },
	{ "dividi"        , POS_SITTING , do_split    , 1, 0 },
	{ "doma"          , POS_FIGHTING, do_doma     , 0, 0 },
	{ "doppiostab"    , POS_STANDING, do_backstab, 0, SCMD_DOPPIO_STAB},
//	{ "dona"          , POS_RESTING , do_drop     , 0, SCMD_DONATE },
//	{ "donate"        , POS_RESTING , do_drop     , 0, SCMD_DONATE },
	{ "dormi"         , POS_SLEEPING, do_sleep    , 0, 0 },
	{ "dove"          , POS_RESTING , do_where    , 1, 0 },
	{ "drink"         , POS_RESTING , do_drink    , 0, SCMD_DRINK },
	{ "drool"         , POS_RESTING , do_action   , 0, 0 },
	{ "drop"          , POS_RESTING , do_drop     , 0, SCMD_DROP },
	
	
  /* lettera E */
	
	{ "eat"           , POS_RESTING , do_eat      , 0, SCMD_EAT },
	{ "echo"          , POS_SLEEPING, do_echo     , LVL_AVATAR, SCMD_ECHO },
	{ "eco"           , POS_SLEEPING, do_echo     , LVL_AVATAR, SCMD_ECHO },
//	{ "elenca"        , POS_STANDING, do_elenca   , LVL_QUEST_MASTER  , 0 },
	{ "embrace"       , POS_STANDING, do_action   , 0, 0 },
	{ "emote"         , POS_RESTING , do_echo     , 1, SCMD_EMOTE },
	{ "emozione"      , POS_RESTING , do_echo     , 1, SCMD_EMOTE },
	{ "enter"         , POS_STANDING, do_enter    , 0, 0 },
	{ "entra"         , POS_STANDING, do_enter    , 0, 0 },

	{ "equipaggiamento", POS_SLEEPING, do_equipment, 0, 0 },
	{ "equipment"     , POS_SLEEPING, do_equipment, 0, 0 },
	{ "eroi"          , POS_DEAD    , do_hiscore  , 0, 0 },
	{ "errore"        , POS_DEAD    , do_gen_write, 0, SCMD_TYPO },
	{ "esercito"      , POS_STANDING, do_esercito , 0, 0 },
	{ "evadi"         , POS_STANDING, do_evadi    , 0, 0 },
	{ "esamina"       , POS_SITTING , do_examine  , 0, 0 },
	{ "examine"       , POS_SITTING , do_examine  , 0, 0 },
	{ "exits"         , POS_RESTING , do_exits    , 0, 0 },
	
  /* lettera F */
	
	{ "fart"          , POS_RESTING , do_action   , 0, 0 },
	{ "fedina"        , POS_RESTING , do_fedina  , 0, 0 },
  /*{ "filetta"       , POS_STANDING, do_fillet   , 1, 0 },*/
	{ "fill"          , POS_STANDING, do_pour     , 0, SCMD_FILL },
  /*{ "fillet"        , POS_STANDING, do_fillet   , 1, 0 },*/
	{ "fischia"       , POS_RESTING , do_action   , 0, 0 },
	{ "fissa"         , POS_RESTING , do_action   , 0, 0 },
	{ "flee"          , POS_FIGHTING, do_flee     , 1, 0 },
	{ "flip"          , POS_STANDING, do_action   , 0, 0 },
	{ "flirt"         , POS_RESTING , do_action   , 0, 0 },
	{ "follow"        , POS_RESTING , do_follow   , 0, 0 },
	{ "fondle"        , POS_RESTING , do_action   , 0, 0 },
	{ "force"         , POS_SLEEPING, do_force    , LVL_BUILDER, 0 },
	{ "forge"         , POS_STANDING, do_forge    , 0, 0 },
	{ "forgia"        , POS_STANDING, do_forge    , 0, 0 },
	{ "freeze"        , POS_DEAD    , do_wizutil  , LVL_IMPLEMENTOR, SCMD_FREEZE },
	{ "french"        , POS_RESTING , do_action   , 0, 0 },
	{ "frigna"        , POS_RESTING , do_action   , 0, 0 },
	{ "frown"         , POS_RESTING , do_action   , 0, 0 },
	{ "fume"          , POS_RESTING , do_action   , 0, 0 },
	{ "furtivo"       , POS_STANDING, do_sneak    , 1, 0 },
	{ "fusa"          , POS_RESTING , do_action   , 0, 0 },
	
	
  /* lettera G */
	
	{ "get"           , POS_RESTING , do_get      , 0, 0 },
	{ "guarda"        , POS_RESTING , do_look     , 0, SCMD_LOOK },
	{ "gasp"          , POS_RESTING , do_action   , 0, 0 },
	{ "gchi"          , POS_SITTING , do_list_groups , 0, 0 },
	{ "gdici"         , POS_SLEEPING, do_gsay     , 0, 0 },
	{ "gdparla"       , POS_SLEEPING, do_gtell    , 1, 0 },
	{ "gdsay"         , POS_SLEEPING, do_gtell    , 1, 0 },
	{ "gdtell"        , POS_SLEEPING, do_gtell    , 1, 0 },
//	{ "gdr"           , POS_SLEEPING, do_gen_tog  , 0, SCMD_RPG },
	{ "gecho"         , POS_DEAD    , do_gecho    , LVL_MINIBUILD, 0 },
	{ "gemi"          , POS_RESTING , do_action   , 0, 0 },
	{ "getta"         , POS_STANDING, do_throw    , 0, 0 },
	{ "giggle"        , POS_RESTING , do_action   , 0, 0 },
	{ "gilda"         , POS_SLEEPING, do_new_clan , 1, 0 },  
	{ "ginocchiata"   , POS_DEAD    , do_action   , 0, 0 },
	{ "giocherella"   , POS_RESTING , do_action   , 0, 0 },
	{ "give"          , POS_RESTING , do_give     , 0, 0 },
	//{ "givehome"      , POS_DEAD    , do_givehome , LVL_IMPLEMENTOR, 0 },
	{ "glare"         , POS_RESTING , do_action   , 0, 0 },
	{ "go"            , POS_STANDING, do_mobgo , 0, 0 },
	{ "godnet"        , POS_DEAD    , do_godnet   , LVL_AVATAR, 0 },
	//{ "gohome"        , POS_STANDING, do_gohome   , 1, 0 },
	{ "gold"          , POS_RESTING , do_gold     , 0, 0 },
	{ "gomitata"      , POS_STANDING, do_action    , 0, 0 },
	{ "gossip"        , POS_SLEEPING, do_gen_comm , LVL_IMMORT, SCMD_GOSSIP },
	{ "gostop"        , POS_STANDING, do_gostop , 0, 0 },
	{ "goto"          , POS_SLEEPING, do_goto     , LVL_IMMORT, 0 },
	{ "gparla"        , POS_SLEEPING, do_gsay     , 0, 0 },
	{ "gpray"         , POS_DEAD    , do_gen_write, 0, SCMD_PRAY},
	{ "grab"          , POS_RESTING , do_grab     , 0, 0 },
	{ "grats"         , POS_SLEEPING, do_gen_comm , 1, SCMD_GRATZ },
	{ "grazie"        , POS_RESTING , do_action   , 0, 0 },
	{ "greet"         , POS_RESTING , do_action   , 0, 0 },
	{ "grida"         , POS_RESTING , do_gen_comm , 1, SCMD_HOLLER },
	{ "grin"          , POS_RESTING , do_action   , 0, 0 },
	{ "groan"         , POS_RESTING , do_action   , 0, 0 },
	{ "grope"         , POS_RESTING , do_action   , 0, 0 },
	{ "group"         , POS_RESTING , do_group    , 1, 0 },
	{ "grovel"        , POS_RESTING , do_action   , 0, 0 },
	{ "growl"         , POS_RESTING , do_action   , 0, 0 },
	{ "gruppo"        , POS_RESTING , do_group    , 1, 0 },
	{ "gsay"          , POS_SLEEPING, do_gsay     , 0, 0 },
	{ "gtell"         , POS_SLEEPING, do_gsay     , 0, 0 },
	{ "guadagna"      , POS_STANDING, do_not_here , 0, 0 },
	{ "gwho"          , POS_SITTING , do_list_groups , 0, 0 },
	
  /* lettera H */
	
	{ "handbook"      , POS_DEAD    , do_gen_ps   , 0, SCMD_HANDBOOK },
	{ "hcontrol"      , POS_DEAD    , do_hcontrol , LVL_GOD, 0 },
	{ "hang"          , POS_STANDING, do_hang     , 1, 0 },
	{ "help"          , POS_DEAD    , do_help     , 0, 0 },
	{ "hiccup"        , POS_RESTING , do_action   , 0, 0 },
	{ "hide"          , POS_RESTING , do_hide     , 1, 0 },
	{ "hire"          , POS_RESTING , do_not_here , 1, 0 },
	{ "hit"           , POS_FIGHTING, do_hit      , 0, SCMD_HIT },
	{ "hold"          , POS_RESTING , do_grab     , 1, 0 },
	{ "holler"        , POS_RESTING , do_gen_comm , 1, SCMD_HOLLER },
	{ "holylight"     , POS_DEAD    , do_gen_tog  , LVL_AVATAR, SCMD_HOLYLIGHT },
	{ "hop"           , POS_RESTING , do_action   , 0, 0 },
	{ "horizon"       , POS_STANDING, do_horizon  , 1, 0 },
	{ "house"         , POS_RESTING , do_house    , 0, 0 },
	{ "hug"           , POS_RESTING , do_action   , 0, 0 },
	
	
  /* lettera I */
	
	{ "idea"          , POS_DEAD    , do_gen_write, 0, SCMD_IDEA },
	{ "immlist"       , POS_DEAD    , do_gen_ps   , 0, SCMD_IMMLIST },
	{ "imotd"         , POS_DEAD    , do_gen_ps   , LVL_QUEST_MASTER, SCMD_IMOTD },
	{ "impara"        , POS_RESTING , do_not_here , 0, 0 },
	{ "impettirsi"    , POS_STANDING, do_action   , 0, 0 },
	{ "implora"       , POS_RESTING , do_action   , 0, 0 },
	{ "impugna"       , POS_RESTING , do_wield    , 0, 0 },
	{ "incantesimo"   , POS_SITTING , do_cast     , 1, 0 },
	{ "incanta"       , POS_SITTING , do_incanta  , 1, 0 },
	{ "incassa"       , POS_STANDING, do_not_here , 0, 0 },
	{ "inchinati"     , POS_STANDING, do_action   , 0, 0 },
	{ "indica"        , POS_RESTING , do_action   , 0, 0 },
	{ "individua"     , POS_STANDING, do_spot     , 1, 0 },
	{ "indossa"       , POS_RESTING , do_wear     , 0, 0 },
	{ "info"          , POS_SLEEPING, do_gen_ps   , 0, SCMD_INFO },
  //{ "infusione"     , POS_STANDING, do_brew     , 0, 0 },
	{ "iniziazione"   , POS_STANDING, do_not_here , 0, 0 },
	{ "insegna"       , POS_RESTING , do_insegna   , 0, 0 },
	{ "insult"        , POS_RESTING , do_insult   , 0, 0 },
	{ "insulta"       , POS_RESTING , do_insult   , 0, 0 },
	{ "inventario"    , POS_DEAD    , do_inventory, 0, 0 },
	{ "inventory"     , POS_DEAD    , do_inventory, 0, 0 },
	{ "invis"         , POS_DEAD    , do_invis    , LVL_MINIBUILD, 0 },
	{ "ira"           , POS_RESTING , do_action   , 0, 0 },
	
  /* lettera J */
	
	//{ "junk"          , POS_RESTING , do_drop     , 0, SCMD_JUNK },
	
  /* lettera K */
	
	{ "kick"          , POS_FIGHTING, do_kick     , 1, 0 },
	{ "kill"          , POS_FIGHTING, do_kill     , 0, 0 },
	{ "kiss"          , POS_RESTING , do_action   , 0, 0 },
	
  /* lettera L */
	
	{ "look"          , POS_RESTING , do_look     , 0, SCMD_LOOK },
	{ "ladro"         , POS_STANDING, do_not_here , 1, 0 },
	{ "lancia"        , POS_STANDING, do_shoot    , 0, 0},
	{ "land"          , POS_STANDING, do_land     , 0, 0 },
	{ "lascia"        , POS_STANDING, do_leave    , 0, 0 },
	{ "last"          , POS_DEAD    , do_last     , LVL_GOD, 0 },
	{ "laugh"         , POS_RESTING , do_action   , 0, 0 },
	{ "lavora"        , POS_STANDING, do_lavora   , 1, 0 },
	{ "learn"         , POS_STANDING, do_not_here , 0, 0 },
	{ "leave"         , POS_STANDING, do_leave    , 0, 0 },
	{ "lecca"         , POS_RESTING , do_action   , 0, 0 },
	{ "leggi"         , POS_RESTING , do_look     , 0, SCMD_READ },
//	{ "levels"        , POS_DEAD    , do_levels   , 0, 0 },
	{ "liberadrago"   , POS_FIGHTING, do_freedragon, 0, 0 },
	{ "lick"          , POS_RESTING , do_action   , 0, 0 },
	{ "limona"        , POS_RESTING , do_action   , 0, 0 },
	{ "liscia"        , POS_RESTING , do_action   , 0, 0 },
	{ "lista"         , POS_STANDING, do_not_here , 0, 0 },
	{ "listprac"      , POS_DEAD    , do_listprac , LVL_QUEST_MASTER, 0 },
	{ "listprof"      , POS_DEAD    , do_listprof , LVL_QUEST_MASTER, 0 },
//	{ "livelli"       , POS_DEAD    , do_levels   , 0, 0 },
	{ "load"          , POS_DEAD    , do_load     , LVL_QUEST_MASTER, 0 },
	{ "lock"          , POS_SITTING , do_gen_door , 0, SCMD_LOCK },
	{ "love"          , POS_RESTING , do_action   , 0, 0 },
	{ "ltitolo"	  , POS_SITTING , do_ltitolo  , LVL_QUEST_MASTER, 0 },
	
  /* lettera M */
	
	{ "macina"         , POS_STANDING, do_lavora   , 0, 0 },
	{ "mah"           , POS_RESTING , do_action   , 0, 0 },
	{ "mail"          , POS_STANDING, do_not_here , 1, 0 },
	{ "maledici"      , POS_RESTING , do_action   , 0, 0 },
	{ "malumore"      , POS_RESTING , do_action   , 0, 0 },
	{ "manda"         , POS_SLEEPING, do_send     , 0, 0 },
	{ "mangia"        , POS_RESTING , do_eat      , 0, SCMD_EAT },
	{ "manovale"      , POS_STANDING, do_not_here , 1, 0 },
	{ "manuale"       , POS_DEAD    , do_gen_ps   , 0, SCMD_HANDBOOK },
	{ "massage"       , POS_RESTING , do_action   , 0, 0 },
	{ "massaggia"     , POS_RESTING , do_action   , 0, 0 },
//	{ "medit"         , POS_DEAD    , do_olc      , LVL_QUEST_MASTER, SCMD_OLC_MEDIT},
	{ "mescola"	  , POS_STANDING, do_mescola  , 0, 0 },
	{ "mestieri"      , POS_RESTING , do_mestieri , 1, 0 },
	{ "metti"         , POS_RESTING , do_put      , 0, 0 },
	{ "mischia"       , POS_STANDING, do_lavora   , 1, SCMD_MISCHIA},
	{ "mlist"         , POS_DEAD    , do_mlist    , LVL_QUEST_MASTER, 0},
	{ "moan"          , POS_RESTING , do_action   , 0, 0 },
	{ "monta"         , POS_STANDING, do_monta    , 0, 0 },
	{ "mordicchia"    , POS_RESTING , do_action   , 0, 0 },
	{ "mostra"        , POS_DEAD    , do_display  , 0, 0 },
	{ "motd"          , POS_DEAD    , do_gen_ps   , 0, SCMD_MOTD },
	{ "murder"        , POS_FIGHTING, do_hit      , 0, SCMD_MURDER },
	{ "mute"          , POS_DEAD    , do_wizutil  , LVL_GOD, SCMD_SQUELCH },
	
  /* lettera N */
	
	{ "namechange"    , POS_DEAD , do_namechange  , LVL_QUEST_MASTER, 0 },
	{ "nasconditi"    , POS_RESTING , do_hide     , 1, 0 },
	{ "news"          , POS_SLEEPING, do_gen_ps   , 0, SCMD_NEWS },
	{ "negozi"        , POS_RESTING , do_shoplist_pg, 0, 0 },
	{ "nibble"        , POS_RESTING , do_action   , 0, 0 },
	{ "noasta"        , POS_DEAD    , do_gen_tog  , 0, SCMD_NOAUCTION },
	{ "noauction"     , POS_DEAD    , do_gen_tog  , 0, SCMD_NOAUCTION },
	{ "nocongratulazioni", POS_DEAD    , do_gen_tog  , 0, SCMD_NOGRATZ },
	{ "nod"           , POS_RESTING , do_action   , 0, 0 },
	{ "nodici"        , POS_DEAD    , do_gen_tog  , 1, SCMD_NOTELL },
	{ "noeroe"        , POS_DEAD    , do_gen_tog  , LVL_GOD, SCMD_NOEROE },
	{ "noevoca"       , POS_DEAD    , do_gen_tog  , 1, SCMD_NOSUMMON },
	{ "nogossip"      , POS_DEAD    , do_gen_tog  , 0, SCMD_NOGOSSIP },
	{ "nograts"       , POS_DEAD    , do_gen_tog  , 0, SCMD_NOGRATZ },
	{ "nohassle"      , POS_DEAD    , do_gen_tog  , LVL_QUEST_MASTER, SCMD_NOHASSLE },
	{ "nokill"        , POS_DEAD    , do_gen_tog  , 0, SCMD_NOKILL},
	{ "noleggia"      , POS_RESTING , do_not_here , 1, 0 },
	{ "noooc"	        , POS_DEAD    , do_gen_tog  , 0, SCMD_NOOOC },
	{ "nonripetere"   , POS_DEAD    , do_gen_tog  , 0, SCMD_NOREPEAT },
//	{ "noregno"       , POS_DEAD    , do_gen_tog  , 0, SCMD_NOCTELL },
	{ "norepeat"      , POS_DEAD    , do_gen_tog  , 0, SCMD_NOREPEAT },
	{ "noshout"       , POS_SLEEPING, do_gen_tog  , 1, SCMD_DEAF },
	{ "nosummon"      , POS_DEAD    , do_gen_tog  , 1, SCMD_NOSUMMON },
	{ "notell"        , POS_DEAD    , do_gen_tog  , 1, SCMD_NOTELL },
//	{ "notifica"      , POS_STANDING, do_notifica , 1, 0 },
	{ "notitle"       , POS_DEAD    , do_wizutil  , LVL_GOD, SCMD_NOTITLE },
	{ "notitolo"      , POS_DEAD    , do_wizutil  , LVL_GOD, SCMD_NOTITLE },
	{ "notizie"       , POS_SLEEPING, do_gen_ps   , 0, SCMD_NEWS },
	{ "nourla"        , POS_SLEEPING, do_gen_tog  , 1, SCMD_DEAF },
	{ "nowiz"         , POS_DEAD    , do_gen_tog  , LVL_BUILDER, SCMD_NOWIZ },
	{ "nudge"         , POS_RESTING , do_action   , 0, 0 },
	{ "nuzzle"        , POS_RESTING , do_action   , 0, 0 },
	
  /* lettera O */
	
//	{ "oedit"         , POS_DEAD    , do_olc      , LVL_QUEST_MASTER, SCMD_OLC_OEDIT},
	{ "offer"         , POS_STANDING, do_not_here , 1, 0 },
	{ "offri"         , POS_STANDING, do_not_here , 1, 0 },
//	{ "olc"           , POS_DEAD    , do_olc      , LVL_QUEST_MASTER, SCMD_OLC_SAVEINFO },
	{ "olist"         , POS_DEAD    , do_olist    , LVL_QUEST_MASTER, 0},
	{ "ooc"           , POS_SLEEPING, do_gen_comm , 1, SCMD_OOC },
	{ "ooc_control"   , POS_DEAD    , do_gen_comm, LVL_QUEST_MASTER, SCMD_CONTROL_OOC},
	{ "open"          , POS_SITTING , do_gen_door , 0, SCMD_OPEN },
	{ "ora"           , POS_DEAD    , do_time     , 0, 0 },
	{ "order"         , POS_RESTING , do_order    , 1, 0 },
	{ "ordina"        , POS_RESTING , do_order    , 1, 0 },
	{ "orizzonte"     , POS_STANDING, do_horizon  , 1, 0 },
	{ "oro"           , POS_RESTING , do_gold     , 0, 0 },
	
	
	
  /* lettera P */
	
	{ "pat"           , POS_RESTING , do_action   , 0, 0 },
	{ "patrol"        , POS_STANDING, do_mobgo , 0, SCMD_PATROL },
	{ "patrolstop"    , POS_STANDING, do_gostop , 0, SCMD_PATROLSTOP },
	{ "page"          , POS_DEAD    , do_page     , LVL_QUEST_MASTER, 0 },
	{ "pardon"        , POS_DEAD    , do_wizutil  , LVL_BUILDER, SCMD_PARDON },
	{ "parla"         , POS_RESTING , do_say      , 0, 0 },
	{ "pavone"        , POS_RESTING , do_action   , 0, 0 },
	{ "peep"          , POS_STANDING, do_peep     , 0, 0 },
	{ "pedina"        , POS_STANDING, do_track    , 0, 0 },
	{ "pensa"         , POS_RESTING , do_action   , 0, 0 },
	{ "pesca"         , POS_RESTING , do_pesca   , 0, 0 },
	{ "pettina"       , POS_RESTING , do_action   , 0, 0 },
	{ "piagnucola"    , POS_RESTING , do_action   , 0, 0 },
	{ "piangi"        , POS_RESTING , do_action   , 0, 0 },
	{ "pick"          , POS_STANDING, do_gen_door , 1, SCMD_PICK },
	{ "placca"        , POS_RESTING , do_action     , 0, 0 },
	{ "players"       , POS_DEAD    , do_players  , LVL_IMPL, 0 },
	{ "point"         , POS_RESTING , do_action   , 0, 0 },
	{ "poke"          , POS_RESTING , do_action   , 0, 0 },
	{ "policy"        , POS_DEAD    , do_gen_ps   , 0, SCMD_POLICIES },
	{ "politica"      , POS_DEAD    , do_gen_ps   , 0, SCMD_POLICIES },
	{ "pondera"       , POS_RESTING , do_action   , 0, 0 },
	{ "poofin"        , POS_DEAD    , do_poofset  , LVL_IMMORT, SCMD_POOFIN },
	{ "poofout"       , POS_DEAD    , do_poofset  , LVL_IMMORT, SCMD_POOFOUT },
	{ "posa"          , POS_RESTING , do_drop     , 0, SCMD_DROP },
	{ "pour"          , POS_STANDING, do_pour     , 0, SCMD_POUR },
	{ "pout"          , POS_RESTING , do_action   , 0, 0 },
	{ "pozioni"       , POS_STANDING, do_lavora   , 1, SCMD_POZIONI},
	{ "practice"      , POS_RESTING , do_practice , 1, 0 },
	{ "pratica"       , POS_RESTING , do_practice , 1, 0 },
	{ "pray"          , POS_RESTING , do_action   , 0, 0 },
	{ "prega"         , POS_RESTING , do_action   , 0, 0 },
	{ "preghiera"     , POS_DEAD    , do_gen_write, 0, SCMD_PRAY},
	{ "preleva"       , POS_STANDING, do_not_here , 1, 0 },
	{ "prendi"        , POS_RESTING , do_get      , 0, 0 },
	{ "prigioniero"   , POS_STANDING, do_not_here , 1, 0 },
	{ "prodotti"      , POS_STANDING, do_not_here , 0, 0 },
	{ "proficienze"   , POS_RESTING , do_proficienze, 1, 0 },
	{ "profilo"       , POS_DEAD    , do_score    , 0, 0 },
	{ "profset"       , POS_SLEEPING, do_profset  , LVL_BUILDER, 0 },
	{ "prompt"        , POS_DEAD    , do_display  , 0, 0 },
	{ "pugnala"       , POS_STANDING, do_backstab , 1, 0 },
	{ "pugno"         , POS_FIGHTING, do_punch    , 0, 0 },
	{ "puke"          , POS_RESTING , do_action   , 0, 0 },
	{ "pulisci_pg"	  , POS_RESTING , do_pulisci_pg, LVL_QUEST_MASTER, 0 },
	{ "pull"          , POS_STANDING, do_not_here , 1, 0 },
	{ "punch"         , POS_FIGHTING, do_punch    , 0, 0 },
	{ "purge"         , POS_DEAD    , do_purge    , LVL_QUEST_MASTER, 0 },
	{ "purr"          , POS_RESTING , do_action   , 0, 0 },
	{ "put"           , POS_RESTING , do_put      , 0, 0 },
	
	
  /* lettera Q */
	
	{ "qecho"         , POS_DEAD    , do_qcomm    , LVL_QUEST_MASTER, SCMD_QECHO },
	{ "qparla"        , POS_RESTING , do_qcomm    , 0, SCMD_QSAY },
	{ "qsay"          , POS_RESTING , do_qcomm    , 0, SCMD_QSAY },
	{ "quaff"         , POS_RESTING , do_use      , 0, SCMD_QUAFF },
	{ "quest"         , POS_DEAD    , do_gen_tog  , 0, SCMD_QUEST },
	{ "qui"           , POS_DEAD    , do_quit     , 0, 0 },
	{ "quit"          , POS_DEAD    , do_quit     , 0, SCMD_QUIT },
	
  /* lettera R */
	
	{ "rabbrividisci" , POS_RESTING , do_action   , 0, 0 },
	{ "raccogli"      , POS_STANDING, do_lavora   , 0, 0 },
	{ "raggrinzisci"  , POS_RESTING , do_action   , 0, 0 },
	{ "rattristati"   , POS_RESTING , do_action   , 0, 0 },
	{ "rapporto"      , POS_RESTING , do_report   , 0, 0 },
	{ "read"          , POS_RESTING , do_look     , 0, SCMD_READ },
	{ "readpray"      , POS_DEAD    , do_gen_ps   , LVL_GOD, SCMD_READPRAY },
	{ "receive"       , POS_STANDING, do_not_here , 1, 0 },
	{ "recite"        , POS_RESTING , do_use      , 0, SCMD_RECITE },
	{ "recita"        , POS_RESTING , do_use      , 0, SCMD_RECITE },
	
	
	
	
	{ "recupera"        , POS_STANDING, do_pesca , 1, SCMD_TIRASU },
//	{ "redit"         , POS_DEAD    , do_olc      , LVL_MINIBUILD, SCMD_OLC_REDIT},
	{ "regni"          , POS_SLEEPING, do_gen_ps   , 0, SCMD_REGNI },
	{ "regno"         , POS_SLEEPING, do_clan     , 1, 0 },
	{ "reload"        , POS_DEAD    , do_reboot   , LVL_IMPLEMENTOR, 0 },
	{ "remove"        , POS_RESTING , do_remove   , 0, 0 },
	{ "rent"          , POS_STANDING, do_not_here , 1, 0 },
	{ "repair"        , POS_STANDING, do_repair   , 0, 0 },
	{ "replica"       , POS_SLEEPING, do_reply    , 0, 0 },
	{ "reply"         , POS_SLEEPING, do_reply    , 0, 0 },
	{ "report"        , POS_RESTING , do_report   , 0, 0 },
	{ "reroll"        , POS_DEAD    , do_wizutil  , LVL_IMPLEMENTOR, SCMD_REROLL },
	{ "rescue"        , POS_FIGHTING, do_rescue   , 1, 0 },

	{ "rest"          , POS_RESTING , do_rest     , 0, 0 },
	{ "restore"       , POS_DEAD    , do_restore  , LVL_QUEST_MASTER, 0 },
	//PEPPE RELIGIONE
	{ "resuscita"     , POS_RESTING , do_resuscita, 0, 0,},
	{ "retreat"       , POS_FIGHTING, do_retreat  , 0, 0 },
	{ "return"        , POS_DEAD    , do_return   , 0, 0 },
	{ "ricevi"        , POS_STANDING, do_not_here , 1, 0 },
	{ "richiamadrago" , POS_FIGHTING, do_recalldragon, 0, 0 },
	{ "richiesta"     , POS_DEAD    , do_gen_tog  , 0, SCMD_QUEST },
	{ "ridacchia"     , POS_RESTING , do_action   , 0, 0 },
	{ "ridi"          , POS_RESTING , do_action   , 0, 0 },
	{ "riempi"        , POS_STANDING, do_pour     , 0, SCMD_FILL },
	//PEPPE CRITICAL
	{ "rigenera"      , POS_STANDING, do_rigenera , 0, 0 },
	{ "rigenerazione" , POS_RESTING , do_rigenerazione, 0, 0},
	{ "rilancia"      , POS_SLEEPING, do_bid	   , 0, 0 },
	{ "rimbalza"      , POS_STANDING, do_action   , 0, 0 },
	{ "rimuovi"       , POS_RESTING , do_remove   , 0, 0 },
	{ "rinfaccia"     , POS_RESTING , do_action   , 0, 0 },
	{ "ringhia"       , POS_RESTING , do_action   , 0, 0 },
	//PEPPE ITEM RINOMINABILI
	{ "rinomina"      , POS_STANDING, do_rinomina , 0, 0 },
//	{ "rinomina_alias" , POS_STANDING, do_rinomina_alias , 0, 0 },
	{ "rinomina_name"  , POS_STANDING, do_rinomina_name , 0, 0 },
	{ "rinomina_descr" , POS_STANDING, do_rinomina_descr , 0, 0 },
	{ "rinnova"       , POS_STANDING, do_rinnova  , LVL_BUILDER, 0 },
	{ "ripara"        , POS_STANDING, do_repair   , 0, 0 },
	{ "riposa"        , POS_RESTING , do_rest     , 0, 0 },
	{ "ritirata"      , POS_FIGHTING, do_retreat  , 0, 0 },
	{ "riverenza"     , POS_STANDING, do_action   , 0, 0 },
	{ "rlist"         , POS_DEAD    , do_rlist    , LVL_MINIBUILD, 0},
	{ "roll"          , POS_RESTING , do_action   , 0, 0 },
	{ "roomflags"     , POS_DEAD    , do_gen_tog  , LVL_MINIBUILD, SCMD_ROOMFLAGS },
	{ "rotea"         , POS_RESTING , do_action   , 0, 0 },
//	{ "rpg"           , POS_SLEEPING, do_gen_tog  , 0, SCMD_RPG },
	{ "rtitolo"	  , POS_STANDING, do_not_here , 1, 0 },
	{ "ruba"          , POS_STANDING, do_steal    , 1, 0 },
	{ "ruffle"        , POS_STANDING, do_action   , 0, 0 },
	{ "russa"         , POS_SLEEPING, do_action   , 0, 0 },
	{ "rutta"         , POS_RESTING , do_action   , 0, 0 },
	
	//solo per debug
	{ "regnopay"         , POS_RESTING , do_regnopay   , LVL_BUILDER, 0 },
	{ "gildapay"         , POS_RESTING , do_gildapay   , LVL_BUILDER, 0 },
	
	
  /* lettera S */
	
	{ "saldo"         , POS_STANDING, do_not_here , 0, 0 },
	{ "saltella"      , POS_RESTING , do_action   , 0, 0 },
	{ "saluta"        , POS_RESTING , do_action   , 0, 0 },
	{ "salva"         , POS_SLEEPING, do_save     , 0, 0 },
	{ "sanguina"      , POS_RESTING , do_action   , 0, 0 },
	{ "save"          , POS_SLEEPING, do_save     , 0, 0 },
	{ "say"           , POS_RESTING , do_say      , 0, 0 },
	{ "sbadiglia"     , POS_RESTING , do_action   , 0, 0 },
	{ "sbava"         , POS_RESTING , do_action   , 0, 0 },
	{ "sbircia"       , POS_STANDING, do_peep     , 0, 0 },
	{ "sblocca"       , POS_SITTING , do_gen_door , 0, SCMD_UNLOCK },
	{ "sbloccaip"	    , POS_STANDING, do_sbloccaip, LVL_IMPL, 0 }, //Orione
	{ "scan"          , POS_STANDING, do_scan     , 1, 0 },
	{ "scansione"     , POS_STANDING, do_scan     , 1, 0 },
	{ "scappa"        , POS_FIGHTING, do_flee     , 1, 0 },
	{ "scarica"       , POS_STANDING, do_scarica  , 1, 0 },
	{ "scassina"      , POS_STANDING, do_gen_door , 1, SCMD_PICK },
	{ "schiaffeggia"  , POS_RESTING , do_action   , 0, 0 },
	{ "schizza"       , POS_RESTING , do_action   , 0, 0 },
	{ "scommetti"     , POS_DEAD    , do_bet	   , 0, 0 },
	{ "scopriti"      , POS_STANDING, do_scopriti	, 0, 0 }, 
	{ "score"         , POS_DEAD    , do_score    , 0, 0 },
	{ "scorreggia"    , POS_RESTING , do_action   , 0, 0 },
	{ "scream"        , POS_RESTING , do_action   , 0, 0 },
	//{ "scribe"        , POS_STANDING, do_scribe   , 0, 0 },
	{ "scrivi"        , POS_STANDING, do_not_here , 1, 0 },
	{ "scrivimagia"   , POS_STANDING, do_write_magic , 1, 0 },
	{ "scrocchia"     , POS_RESTING , do_action   , 0, 0 },
	{ "scrolla"       , POS_STANDING, do_scrolla  , 0, 0 },
	{ "sculaccia"     , POS_RESTING , do_action   , 0, 0 },
	{ "scuoia"        , POS_STANDING , do_lavora   , 0, 0 },
	{ "scuoti"        , POS_RESTING , do_action   , 0, 0 },
	{ "search"        , POS_STANDING, do_search   , 0, 0,},
//	{ "sedit"         , POS_DEAD    , do_olc      , LVL_QUEST_MASTER, SCMD_OLC_SEDIT},
	{ "segui"         , POS_RESTING , do_follow   , 0, 0 },
	{ "sell"          , POS_STANDING, do_not_here , 0, 0 },
	{ "send"          , POS_SLEEPING, do_send     , 0, 0 },
	{ "set"           , POS_DEAD    , do_set      , LVL_GOD, 0 },
	//PROVA SFAMA

	{ "sfama"         , POS_RESTING , do_not_here , 0, 0 },	

	{ "sfolgora"      , POS_RESTING , do_action   , 0, 0 },
	{ "sfonda"        , POS_SITTING , do_gen_door , 0, SCMD_BASH_LOCK}, 
	{ "sgancia"       , POS_STANDING, do_sgancia  , 0, 0 },
	{ "sgrida"        , POS_STANDING, do_action   , 0, 0 },
	{ "sgruppa"       , POS_DEAD    , do_ungroup  , 0, 0 },
	{ "shake"         , POS_RESTING , do_action   , 0, 0 },
//	{ "shedit"        , POS_STANDING, do_shedit   , 0, 0 },
	{ "shiver"        , POS_RESTING , do_action   , 0, 0 },
	{ "shoot"         , POS_STANDING, do_shoot    , 0, 0},
	{ "shout"         , POS_RESTING , do_gen_comm , 1, SCMD_SHOUT },
	{ "show"          , POS_DEAD    , do_show     , 40, 0 },
	{ "shrug"         , POS_RESTING , do_action   , 0, 0 },
//	{ "shsave"        , POS_STANDING, do_shsave   , 0, 0 },
	{ "shutdow"       , POS_DEAD    , do_shutdown , LVL_BUILDER, 0 },
	{ "shutdown"      , POS_DEAD    , do_shutdown , LVL_BUILDER, SCMD_SHUTDOWN },
	{ "siediti"       , POS_RESTING , do_sit      , 0, 0 },
	{ "sigh"          , POS_RESTING , do_action   , 0, 0 },
	{ "sing"          , POS_RESTING , do_action   , 0, 0 },
	{ "singhiozza"    , POS_RESTING , do_action   , 0, 0 },
	{ "sip"           , POS_RESTING , do_drink    , 0, SCMD_SIP },
	{ "sit"           , POS_RESTING , do_sit      , 0, 0 },
	{ "skillset"      , POS_SLEEPING, do_skillset , LVL_BUILDER, 0 },
	{ "slap"          , POS_RESTING , do_action   , 0, 0 },
	{ "slay"          , POS_FIGHTING, do_slay     , LVL_IMPL, 0 },
	{ "slegati"       , POS_RESTING , do_free     ,0 , 0 },
	{ "sleep"         , POS_SLEEPING, do_sleep    , 0, 0 },
	{ "slowns"        , POS_DEAD    , do_gen_tog  , LVL_IMPL, SCMD_SLOWNS },
	{ "smile"         , POS_RESTING , do_action   , 0, 0 },
	{ "smirk"         , POS_RESTING , do_action   , 0, 0 },
	{ "smonta"        , POS_SITTING , do_smonta   , 0, 0 },
	{ "snap"          , POS_RESTING , do_action   , 0, 0 },
	{ "snarl"         , POS_RESTING , do_action   , 0, 0 },
	{ "sneak"         , POS_STANDING, do_sneak    , 1, 0 },
	{ "sneakview"     , POS_DEAD    , do_gen_tog  , LVL_AVATAR, SCMD_HIDE_SNEAK },
	{ "sneeze"        , POS_RESTING , do_action   , 0, 0 },
	{ "snicker"       , POS_RESTING , do_action   , 0, 0 },
	{ "sniff"         , POS_RESTING , do_action   , 0, 0 },
	{ "snoop"         , POS_DEAD    , do_snoop    , LVL_BUILDER, 0 },
	{ "snore"         , POS_SLEEPING, do_action   , 0, 0 },
	{ "snowball"      , POS_STANDING, do_action   , LVL_MINIBUILD, 0 },
	{ "snuggle"       , POS_RESTING , do_action   , 0, 0 },
	{ "soccorri"      , POS_FIGHTING, do_rescue   , 1, 0 },
	{ "socials"       , POS_DEAD    , do_commands , 0, SCMD_SOCIALS },
	{ "soffia"        , POS_FIGHTING, do_dragonbreath, 0, 0 },
	{ "sogghigna"     , POS_RESTING , do_action   , 0, 0 },
	{ "sogna"         , POS_SLEEPING, do_action   , 0, 0 },
	{ "solletica"     , POS_RESTING , do_action   , 0, 0 },
	{ "sorridi"       , POS_RESTING , do_action   , 0, 0 },
	{ "sorrisetto"    , POS_RESTING , do_action   , 0, 0 },
	{ "sorseggia"     , POS_RESTING , do_drink    , 0, SCMD_SIP },
	{ "sottometti"    , POS_STANDING, do_doma, LVL_QUEST_MASTER, SCMD_SUBMITDRAGON },
	{ "spank"         , POS_RESTING , do_action   , 0, 0 },
	{ "spingi"        , POS_RESTING , do_action   , 0, 0 },
	{ "spia"          , POS_STANDING, do_spy      , 0, 0 },
	{ "spit"          , POS_STANDING, do_action   , 0, 0 },
	{ "split"         , POS_SITTING , do_split    , 1, 0 },
	{ "spot"          , POS_STANDING, do_spot     , 1, 0 },
	{ "sputa"         , POS_STANDING, do_action   , 0, 0 },
	{ "spy"           , POS_STANDING, do_spy      , 0, 0 },
	{ "squeeze"       , POS_RESTING , do_action   , 0, 0 },
	{ "stand"         , POS_RESTING , do_stand    , 0, 0 },
	{ "stai"          , POS_RESTING , do_action   , 0, 0 },
	{ "starnutisci"   , POS_RESTING , do_action   , 0, 0 },
	{ "stat"          , POS_DEAD    , do_stat     , LVL_QUEST_MASTER, 0 },
	//PEPPE CRITICAL
	{ "stato"         , POS_RESTING, do_stato     , 0, 0 },
	{ "steal"         , POS_STANDING, do_steal    , 1, 0 },
	{ "steam"         , POS_RESTING , do_action   , 0, 0 },
	{ "stopfight"     , POS_DEAD  , do_stopfight  , 0, 0 },
	{ "stow"          , POS_DEAD  , do_give  , 0, SCMD_STOW },
	{ "strilla"       , POS_RESTING , do_action   , 0, 0 },
	{ "stringi"       , POS_STANDING, do_action   , 0, 0 },
	{ "striscia"      , POS_RESTING , do_action   , 0, 0 },
	{ "strizza"       , POS_RESTING , do_action   , 0, 0 },
	{ "strofina"      , POS_RESTING , do_action   , 0, 0 },
	{ "stroke"        , POS_RESTING , do_action   , 0, 0 },
	{ "strut"         , POS_STANDING, do_action   , 0, 0 },
	{ "sulk"          , POS_RESTING , do_action   , 0, 0 },
	{ "sussurra"      , POS_RESTING , do_spec_comm, 0, SCMD_WHISPER },
	{ "sveglia"       , POS_SLEEPING, do_wake     , 0, 0 },
	{ "svuota"      , POS_STANDING, do_pour     , 0, SCMD_POUR },
	{ "switch"        , POS_DEAD    , do_switch   , LVL_QUEST_MASTER, 0 },
	{ "syslog"        , POS_DEAD    , do_syslog   , LVL_QUEST_MASTER, 0 },
	
  /* lettera T */
	
	{ "tackle"        , POS_RESTING , do_action   , 0, 0 },
	{ "take"          , POS_RESTING , do_get      , 0, 0 },
	{ "takehome"      , POS_DEAD    , do_takehome , LVL_BUILDER, 0 },
	{ "takemob"       , POS_STANDING, do_takemob  , LVL_BUILDER, 0 },
//	{ "takeshop"      , POS_STANDING, do_takeshop , LVL_BUILDER, 0 },
	{ "tango"         , POS_STANDING, do_action   , 0, 0 },
	{ "taste"         , POS_RESTING , do_eat      , 0, SCMD_TASTE },
	{ "taunt"         , POS_RESTING , do_action   , 0, 0 },
	{ "tell"          , POS_DEAD    , do_tell     , 0, 0 },
	{ "tedit"         , POS_DEAD    , do_tedit    , LVL_BUILDER, 0 },
	{ "tele"          , POS_SLEEPING, do_tele     , LVL_GOD, 0 },
	{ "teleport"      , POS_DEAD    , do_teleport , LVL_GOD, 0 },
	{ "tempo"         , POS_RESTING , do_weather  , 0, 0 },
	{ "thank"         , POS_RESTING , do_action   , 0, 0 },
	{ "thaw"          , POS_DEAD    , do_wizutil  , LVL_IMPLEMENTOR, SCMD_THAW },
	{ "think"         , POS_RESTING , do_action   , 0, 0 },
	{ "throw"         , POS_STANDING, do_throw    , 0, 0 },
	{ "tickle"        , POS_RESTING , do_action   , 0, 0 },
	{ "tieni"         , POS_RESTING , do_grab     , 1, 0 },
	{ "time"          , POS_DEAD    , do_time     , 0, 0 },
	{ "tira"          , POS_STANDING, do_not_here , 1, 0 },
	{ "title"         , POS_DEAD    , do_title    , 0, 0 },
	{ "titolo"        , POS_DEAD    , do_title    , 0, 0 },
	{ "toggle"        , POS_DEAD    , do_toggle   , 0, 0 },
	{ "torna"         , POS_DEAD    , do_return   , 0, 0 },
	{ "tornado"	  , POS_FIGHTING, do_tornado  , 0, 0 },
	{ "tossisci"      , POS_RESTING , do_action   , 0, 0 },
	{ "track"         , POS_STANDING, do_track    , 0, 0 },
	{ "trade"         , POS_STANDING, do_not_here , 1, 0 },
	{ "trangugia"     , POS_RESTING , do_use      , 0, SCMD_QUAFF },
	{ "transfer"      , POS_SLEEPING, do_trans    , LVL_QUEST_MASTER, 0 },
	{ "trappola"      , POS_STANDING, do_trappola , 0, 0 },
//	{ "trigedit"      , POS_DEAD    , do_olc      , LVL_GOD, SCMD_OLC_TRIGEDIT},
	{ "triste"        , POS_STANDING, do_action   , 0, 0 },
	{ "trust"		, POS_DEAD	  , do_trust    , LVL_BUILDER,0 },
	{ "twiddle"       , POS_RESTING , do_action   , 0, 0 },
	{ "typo"          , POS_DEAD    , do_gen_write, 0, SCMD_TYPO },
	
  /* lettera U */
	
	{ "urta"          , POS_FIGHTING, do_bash     , 1, 0 },
	{ "uccidi"        , POS_FIGHTING, do_kill     , 0, 0 },
	{ "unaffect"      , POS_DEAD    , do_wizutil  , LVL_QUEST_MASTER, SCMD_UNAFFECT },
	{ "unban"         , POS_DEAD    , do_unban    , LVL_BUILDER, 0 },
	{ "ungroup"       , POS_DEAD    , do_ungroup  , 0, 0 },
	{ "unlock"        , POS_SITTING , do_gen_door , 0, SCMD_UNLOCK },
	{ "unstow"        , POS_DEAD    , do_scarica  , 0, SCMD_UNSTOW },
	{ "uptime"        , POS_DEAD    , do_date     , LVL_BUILDER, SCMD_UPTIME },
	{ "urla"          , POS_RESTING , do_gen_comm , 1, SCMD_SHOUT },
	{ "urta"          , POS_RESTING , do_action   , 0, 0 },
	{ "uscita"        , POS_RESTING , do_exits    , 0, 0 },
	{ "use"           , POS_SITTING , do_use      , 1, SCMD_USE },
	{ "users"         , POS_DEAD    , do_users    , LVL_GOD, 0 },
	{ "usa"           , POS_SITTING , do_use      , 1, SCMD_USE },
	
  /* lettera V */
	
	{ "vai"           , POS_SLEEPING, do_goto     , LVL_IMMORT, 0 },
	{ "valore"        , POS_STANDING, do_not_here , 0, 0 },
	{ "value"         , POS_STANDING, do_not_here , 0, 0 },
	{ "valuta"        , POS_RESTING , do_action   , 0, 0 },
	{ "vendi"         , POS_STANDING, do_not_here , 0, 0 },
	{ "vendicasa"     , POS_STANDING, do_acostruzione , 0, 0 },
	{ "version"       , POS_DEAD    , do_gen_ps   , 0, SCMD_VERSION },
	{ "versione"      , POS_DEAD    , do_gen_ps   , 0, SCMD_VERSION },
	{ "vigliacco"     , POS_DEAD    , do_wimpy    , 0, 0 },
	{ "visibile"      , POS_RESTING , do_visible  , 1, 0 },
	{ "visible"       , POS_RESTING , do_visible  , 1, 0 },
	{ "vnum"          , POS_DEAD    , do_vnum     , LVL_QUEST_MASTER, 0 },
	{ "vola"          , POS_STANDING, do_dragonfly, 0, 0},
	{ "vomita"        , POS_RESTING , do_action   , 0, 0 },
	{ "vstat"         , POS_DEAD    , do_vstat    , LVL_QUEST_MASTER, 0 },
	
  /* lettera W */
	
	{ "wake"          , POS_SLEEPING, do_wake     , 0, 0 },
	{ "wave"          , POS_RESTING , do_action   , 0, 0 },
	{ "wear"          , POS_RESTING , do_wear     , 0, 0 },
	{ "weather"       , POS_RESTING , do_weather  , 0, 0 },
//	{ "wedit"         , POS_DEAD    , do_olc      , LVL_BUILDER, SCMD_OLC_WEDIT},
	{ "where"         , POS_RESTING , do_where    , 1, 0 },
	{ "whine"         , POS_RESTING , do_action   , 0, 0 },
	{ "whisper"       , POS_RESTING , do_spec_comm, 0, SCMD_WHISPER },
	{ "whistle"       , POS_RESTING , do_action   , 0, 0 },
	{ "who"           , POS_DEAD    , do_who      , 0, 0 },
	{ "whoami"        , POS_DEAD    , do_gen_ps   , 0, SCMD_WHOAMI },
	{ "wield"         , POS_RESTING , do_wield    , 0, 0 },
	{ "wiggle"        , POS_STANDING, do_action   , 0, 0 },
	{ "wildx"         , POS_DEAD    , do_wildx    , 0, 0 },
	{ "wildy"         , POS_DEAD    , do_wildy    , 0, 0 },
	{ "wimpy"         , POS_DEAD    , do_wimpy    , 0, 0 },
	{ "wink"          , POS_RESTING , do_action   , 0, 0 },
	{ "wiznet"        , POS_DEAD    , do_wiznet   , LVL_BUILDER, 0 },
	{ "wizhelp"       , POS_SLEEPING, do_commands , LVL_IMMORT, SCMD_WIZHELP },
	{ "wizlist"       , POS_DEAD    , do_gen_ps   , 0, SCMD_WIZLIST },
	{ "wizlock"       , POS_DEAD    , do_wizlock  , LVL_BUILDER, 0 },
	{ "worship"       , POS_RESTING , do_action   , 0, 0 },
	{ "write"         , POS_STANDING, do_write    , 1, 0 },
	{ "writemagic"    , POS_STANDING, do_write_magic , 1, 0 },
	
  /* lettera Y */
	
	{ "yawn"          , POS_RESTING , do_action   , 0, 0 },
	{ "yodel"         , POS_RESTING , do_action   , 0, 0 },
	
  /* lettera Z */
	
//	{ "zedit"         , POS_DEAD    , do_olc      , LVL_QUEST_MASTER, SCMD_OLC_ZEDIT},
	{ "zonexit"       , POS_DEAD    , do_zonexit  , LVL_GOD, 0},
	{ "zreset"        , POS_DEAD    , do_zreset   , LVL_QUEST_MASTER, 0 },
	
	
  /* DG trigger commands */
	{ "attach"        , POS_DEAD    , do_attach   , LVL_BUILDER, 0 },
	{ "detach"        , POS_DEAD    , do_detach   , LVL_BUILDER, 0 },
	{ "tlist"         , POS_DEAD    , do_tlist    , LVL_QUEST_MASTER, 0 },
	{ "tstat"         , POS_DEAD    , do_tstat    , LVL_QUEST_MASTER, 0 },
	{ "masound"       , POS_DEAD    , do_masound  , 0, 0 },
	{ "mkill"         , POS_STANDING, do_mkill    , 0, 0 },
	{ "mjunk"         , POS_SITTING , do_mjunk    , 0, 0 },
	{ "mdoor"    	  , POS_DEAD    , do_mdoor    , 0, 0 },	//Orione
	{ "mecho"         , POS_DEAD    , do_mecho    , 0, 0 },
	{ "mechoaround"   , POS_DEAD    , do_mechoaround    , 0, 0 },
	{ "msend"         , POS_DEAD    , do_msend    , 0, 0 },
	{ "mload"         , POS_DEAD    , do_mload    , 0, 0 },
	{ "mpurge"        , POS_DEAD    , do_mpurge    , 0, 0 },
	{ "mgoto"         , POS_DEAD    , do_mgoto    , 0, 0 },
	{ "mat"           , POS_DEAD    , do_mat      , 0, 0 },
	{ "mteleport"     , POS_DEAD    , do_mteleport, 0, 0 },
	{ "mforce"        , POS_DEAD    , do_mforce   , 0, 0 },
	{ "mexp"          , POS_DEAD    , do_mexp     , 0, 0 },
	{ "mhunt"    	  , POS_DEAD    , do_mhunt    , 0, 0 },	//Orione
	{ "mremember"	  , POS_DEAD    , do_mremember, 0, 0 },	//Orione
	{ "mforget"  	  , POS_DEAD    , do_mforget  , 0, 0 }, //Orione
	{ "mtransform"	  ,POS_DEAD    , do_mtransform, 0, 0 }, //Orione
	{ "stopauction"   , POS_SITTING , do_stop_auction  , LVL_GOD, 0 },
	{ "mspsound"      , POS_DEAD    , do_gen_tog  , 0, SCMD_SOUNDS },
	/*{ "interwho"      , POS_RESTING, do_interwho, 1, 0 },
	{ "interwiz"      , POS_RESTING, do_interwiz, LVL_GOD, 0 },
	{ "intertell"     , POS_RESTING, do_intertel, 1, 0},
	{ "interpage"     , POS_RESTING, do_interpage, LVL_BUILDER, 0},
	{ "intermud"      , POS_RESTING, do_intermud, LVL_BUILDER, 0 },
	{ "mudinfo"       , POS_RESTING, do_mudinfo, 1, 0},
	{ "mudlist"       , POS_RESTING, do_mudlist, 1, 0},*/
	{ "\n", 0, 0, 0, 0 } };	/* this must be last */


const char *fill[] =
{
	"in",
	"from",
	"with",
	"the",
	"on",
	"at",
	"to",
	"\n"
};

const char *reserved[] =
{
	"self",
	"me",
	"all",
	"room",
	"someone",
	"something",
	"\n"
};

char *comandi_fantasma[]=		/*--------------------------------------*\
					|* Fondamentale (by Spini):		*|
					|* quando si aggiunge o si toglie un	*|
					|* comando dalla lista, modificare il	*|
					|* valore di NUM_CMD_FANTASMA in	*|
					|* utils.h, deve essere sempre uguale	*|
					|* al numero di comandi elencati	*|
					\*--------------------------------------*/
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"nord",
	"sud",
	"est",
	"ovest",
	"alto",
	"basso",
	"alias",
	"accampati",
	"afk",
	"alzati",
	"attributes",
	"attributi",
	"autodir",
	"autoexit",
	"breve",
	"bug",
	"camp",
	"chi",
	"dici",
	"equipaggiamento",
	"equipment",
	"follow",
	"guarda",
	"help",
	"idea",
	"info",
	"inventario",
	"inventory",
	"look",
	"leggi",
	"manuale",
	"motd",
	"news",
	"notell",
	"profilo",
	"prompt",
	"quit",
	"read",
	"replica",
	"reply",
	"resuscita",
	"salva",
	"save",
	"score",
	"segui",
	"stand",
	"tell",
	"title",
	"titolo",
	"who",
	"\n"	
};	


char *comandi_nogdr[]=
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"nord",
	"sud",
	"est",
	"ovest",
	"alto",
	"basso",
	"accampati",
	"afk",
	"alzati",
	"autodir",
	"autoexit",
	"breve",
	"bug",
	"camp",
	"chi",
//	"ctell",
	"dici",
	"follow",
//	"gdr",
	"guarda",
	"help",
	"holler",
	"idea",
	"info",
	"look",
	"leggi",
	"manuale",
	"motd",
	"news",
	"nodici",
//	"noooc",
//	"noregno",
	"noshout",
	"notell",
	"nourla",
//	"ooc",
	"parla",
	"prompt",
	"quit",
	"read",
	"replica",
	"reply",
	"salva",
	"save",
	"say",
	"segui",
	"shout",
	"siediti",
	"sit",
	"stand",
	"stai",
	"tell",
	"urla",
	"who",
	"emote",
	"'",
	"\n"	
};	

/*
 * This is the actual command interpreter called from game_loop() in comm.c
 * It makes sure you are the proper level and position to execute the command,
 * then calls the appropriate function.
 */
void command_interpreter(struct char_data *ch, char *argument)
{
	int cmd, length,lista_fantasma,trovato=0;
	extern int no_specials;
	struct last_same_com *ev;
	char *line;
	char *memorizza;	
	
	REMOVE_BIT(AFF_FLAGS(ch), AFF_HIDE);
	
	if (!PRF_FLAGGED(ch, PRF_RPG))
    SET_BIT(PRF_FLAGS(ch), PRF_RPG);
  
  if (PRF_FLAGGED(ch, PRF_REGNOTELL))
    REMOVE_BIT(PRF_FLAGS(ch), PRF_REGNOTELL);
  
  if (PRF_FLAGGED(ch, PRF_AFK)) {
    REMOVE_BIT(PRF_FLAGS(ch), PRF_AFK);
    for (ev = ch->last_same_command; ev; ev = ev->next)
      if (ev->type_event == LAST_AFK)
        ev->times = time(NULL);
    act("$n (AFK) Rientra in gioco.", TRUE, ch, 0, 0, TO_ROOM);
    send_to_char("\033[2J \r\n\n***AFK**** Ritorni in gioco.", ch);	
  }
  
  if(PLR_FLAGGED(ch,PLR_CATARSI)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_CATARSI);
    act("$n si deconcentra ed esce dalla catarsi", TRUE, ch, 0, 0, TO_ROOM);
    send_to_char("Improvvisamente perdi la concentrazione ed esci dalla catarsi.\r\n", ch);
    return;
  }
  
	
  /* just drop to next line for hitting CR */
	skip_spaces(&argument);
	if (!*argument)
		return;
	
	memorizza = strdup(argument);
	
  /*
   * special case to handle one-character, non-alphanumeric commands;
   * requested by many people so "'hi" or ";godnet test" is possible.
   * Patch sent by Eric Green and Stefan Wasilewski.
   */
	if (!isalpha(*argument) && !isdigit(*argument)) {
		arg[0] = argument[0];
		arg[1] = '\0';
		line = argument + 1;
	} 
	else
		line = any_one_arg(argument, arg);
	
  /*
   *  se qualcuno ha scritto un numero, allora mette questo numero
   *  nella argument list e ci mete 'goto' davanti
   */
	if (GET_LEVEL(ch) >= LVL_QUEST_MASTER && isdigit(*arg)) {
		strcpy(line, arg);
		strcpy(arg, "goto");
	}
	
  /* otherwise, find the command */
	
	if (   (GET_LEVEL(ch) < LVL_QUEST_MASTER)
        && (   command_wtrigger(ch, arg, line)
            || command_mtrigger(ch, arg, line)
            || command_otrigger(ch, arg, line)
            )
        )
	  return; /* command trigger took over */
	
	for (length = strlen(arg), cmd = 0; *cmd_info[cmd].command != '\n'; cmd++)
		if (!strncmp(cmd_info[cmd].command, arg, length))
			if (GET_LEVEL(ch) >= cmd_info[cmd].minimum_level)
				break;
	
	//PEPPE COMANDI FANTASMA
	if (PLR_FLAGGED(ch,PLR_FANTASMA) && !IS_NPC(ch) && GET_LEVEL(ch)<LVL_QUEST_MASTER) {
		if (*cmd_info[cmd].command == '\n'){
			send_to_char("Huh?!?\r\n", ch);
			return;
		}
		    
		if (cmd>=7 && cmd <=12) cmd-=6; // Converte il comando direzione italiano in quello in inglese
	
		lista_fantasma=0;
		while(lista_fantasma != NUM_CMD_FANTASMA) {
		  if (!strcmp(cmd_info[cmd].command,comandi_fantasma[lista_fantasma])) {
			  trovato=1;
			}
		  lista_fantasma++;
		}
		
		if (trovato==0) {
			send_to_char("Non riesci a fare niente del genere in forma eterea\r\n",ch);
			return;
		}
	}

	//PEPPE COMANDI NOGDR

/*     
	if(!PRF_FLAGGED(ch,PRF_RPG) && !IS_NPC(ch) && GET_LEVEL(ch)<LVL_IMMORT ){
		    if (*cmd_info[cmd].command == '\n'){
			send_to_char("Huh?!?\r\n", ch);
			return;
			}
		    
		    if (cmd>=7 && cmd <=12) cmd-=6; // Converte il comando direzione italiano in quello in inglese
	
		    lista_nogdr=0;
		while(lista_nogdr!=NUM_CMD_NOGDR){
		    if(!strcmp(cmd_info[cmd].command,comandi_nogdr[lista_nogdr])){
			trovato=1;
			}
		    lista_nogdr++;
		    }
		    if(trovato==0){
			send_to_char("Non puoi fare queste cose se non scegli di interpretare il tuo personaggio.\r\n",ch);
			return;
		}
	}
*/
	if (cmd>=7 && cmd <=12) cmd-=6; // Converte il comando direzione italiano in quello in inglese
	
	if (*cmd_info[cmd].command == '\n')
		send_to_char("Huh?!?\r\n", ch);
		
	else if (PLR_FLAGGED(ch, PLR_FROZEN) && GET_LEVEL(ch) <= LVL_IMPL)
		send_to_char("Provi, ma una brivido freddo soprannaturale ti blocca...\r\n", ch);
	else if (cmd_info[cmd].command_pointer == NULL)
		send_to_char("Mi spiace, comando non ancora implementato.\r\n", ch);
	else if (AFF_FLAGGED(ch,AFF_TRAMORTITO))
		send_to_char("Mmmm..Tutto quello che puoi far ora e' dormire!.\r\n",ch);
  /*else if (AFF_FLAGGED(ch, AFF_IMMOBIL))
    send_to_char("Non riesci a fare niente, ti hanno legato come un salame!.\r\n", ch);*/
	else if ((AFF_FLAGGED(ch, AFF_PARALIZE)) && (GET_LEVEL(ch)<LVL_AVATAR))
		send_to_char("Non riesci a fare niente, sei paralizzato.\r\n", ch);
/* Orione: aggiunto al if sottostante l'opzione per esegure azioni da
immortale (solo "set") quando switchati in mob di livello 100*/
	else if (IS_NPC(ch) && cmd_info[cmd].minimum_level >= LVL_IMMORT && !(GET_LEVEL(ch) == 100 && !strcmp(cmd_info[cmd].command,"set")))
		send_to_char("Non puoi usare comandi per immortali mentre sei 'switchato'.\r\n", ch);
	else if (GET_POS(ch) < cmd_info[cmd].minimum_position)
		switch (GET_POS(ch)) {
			case POS_DEAD:
				send_to_char("Sei MORTO!!! :-(\r\n", ch);
				break;
			case POS_INCAP:
			case POS_MORTALLYW:
				send_to_char("Sei in una brutta posizione e non riesci a fare niente!\r\n", ch);
				break;
			case POS_STUNNED:
				send_to_char("Tutto quello che puoi fare ora e guardare le stelle!\r\n", ch);
				break;
			case POS_SLEEPING:
				send_to_char("Nei tuoi sogni, o dove?\r\n", ch);
				break;
			case POS_RESTING:
				send_to_char("Nah... Sei troppo rilassato ora.\r\n", ch);
				break;
			case POS_SITTING:
				send_to_char("Forse dovresti essere in piedi prima?\r\n", ch);
				break;
			case POS_FIGHTING:
				send_to_char("Niente da fare! Stai combattendo per la tua vita.\r\n", ch);
				break;
		} else if (no_specials || !special(ch, cmd, line)) { 
		
		last_ch = ch; 
		sprintf(last_command,"%s",argument); 
		
		((*cmd_info[cmd].command_pointer) (ch, line, cmd, cmd_info[cmd].subcmd)); 
	} 
}

/**************************************************************************
 * Routines to handle aliasing                                             *
  **************************************************************************/


struct alias *find_alias(struct alias *alias_list, char *str)
{
	while (alias_list != NULL) {
		if (*str == *alias_list->alias)	/* hey, every little bit counts :-) */
			if (!strcmp(str, alias_list->alias))
				return alias_list;
			
		alias_list = alias_list->next;
	}
	
	return NULL;
}


void free_alias(struct alias *a)
{
	if (a->alias)
		free(a->alias);
	if (a->replacement)
		free(a->replacement);
	free(a);
}


/* The interface to the outside world: do_alias */
ACMD(do_alias)
{
	char *repl;
	struct alias *a, *temp;
	
	if (IS_NPC(ch))
		return;
	
	repl = any_one_arg(argument, arg);
	
	if (!*arg) {			/* no argument specified -- list currently defined aliases */
		send_to_char("Currently defined aliases:\r\n", ch);
		if ((a = GET_ALIASES(ch)) == NULL)
			send_to_char(" None.\r\n", ch);
		else {
			while (a != NULL) {
				sprintf(buf, "%-15s %s\r\n", a->alias, a->replacement);
				send_to_char(buf, ch);
				a = a->next;
			}
		}
	} else {			/* otherwise, add or remove aliases */
    /* is this an alias we've already defined? */
		if ((a = find_alias(GET_ALIASES(ch), arg)) != NULL) {
			REMOVE_FROM_LIST(a, GET_ALIASES(ch), next);
			free_alias(a);
		}
    /* if no replacement string is specified, assume we want to delete */
		if (!*repl) {
			if (a == NULL)
				send_to_char("No such alias.\r\n", ch);
			else
				send_to_char("Alias deleted.\r\n", ch);
		} else {			/* otherwise, either add or redefine an alias */
			if (!str_cmp(arg, "alias")) {
				send_to_char("You can't alias 'alias'.\r\n", ch);
				return;
			}
			CREATE(a, struct alias, 1);
			a->alias = str_dup(arg);
			delete_doubledollar(repl);
			a->replacement = str_dup(repl);
			if (strchr(repl, ALIAS_SEP_CHAR) || strchr(repl, ALIAS_VAR_CHAR))
				a->type = ALIAS_COMPLEX;
			else
				a->type = ALIAS_SIMPLE;
			a->next = GET_ALIASES(ch);
			GET_ALIASES(ch) = a;
			send_to_char("Alias added.\r\n", ch);
		}
	}
}

/*
 * Valid numeric replacements are only $1 .. $9 (makes parsing a little
 * easier, and it's not that much of a limitation anyway.)  Also valid
 * is "$*", which stands for the entire original line after the alias.
 * ";" is used to delimit commands.
 */
#define NUM_TOKENS       9

void perform_complex_alias(struct txt_q *input_q, char *orig, struct alias *a)
{
	struct txt_q temp_queue;
	char *tokens[NUM_TOKENS], *temp, *write_point;
	int num_of_tokens = 0, num;
	
  /* First, parse the original string */
	temp = strtok(strcpy(buf2, orig), " ");
	while (temp != NULL && num_of_tokens < NUM_TOKENS) {
		tokens[num_of_tokens++] = temp;
		temp = strtok(NULL, " ");
	}
	
  /* initialize */
	write_point = buf;
	temp_queue.head = temp_queue.tail = NULL;
	
  /* now parse the alias */
	for (temp = a->replacement; *temp; temp++) {
		if (*temp == ALIAS_SEP_CHAR) {
			*write_point = '\0';
			buf[MAX_INPUT_LENGTH - 1] = '\0';
			write_to_q(buf, &temp_queue, 1);
			write_point = buf;
		} else if (*temp == ALIAS_VAR_CHAR) {
			temp++;
			if ((num = *temp - '1') < num_of_tokens && num >= 0) {
				strcpy(write_point, tokens[num]);
				write_point += strlen(tokens[num]);
			} else if (*temp == ALIAS_GLOB_CHAR) {
				strcpy(write_point, orig);
				write_point += strlen(orig);
			} else if ((*(write_point++) = *temp) == '$')	/* redouble $ for act safety */
				*(write_point++) = '$';
		} else
			*(write_point++) = *temp;
	}
	
	*write_point = '\0';
	buf[MAX_INPUT_LENGTH - 1] = '\0';
	write_to_q(buf, &temp_queue, 1);
	
  /* push our temp_queue on to the _front_ of the input queue */
	if (input_q->head == NULL)
		*input_q = temp_queue;
	else {
		temp_queue.tail->next = input_q->head;
		input_q->head = temp_queue.head;
	}
}


/*
 * Given a character and a string, perform alias replacement on it.
 *
 * Return values:
 *   0: String was modified in place; call command_interpreter immediately.
 *   1: String was _not_ modified in place; rather, the expanded aliases
 *      have been placed at the front of the character's input queue.
 */
int perform_alias(struct descriptor_data *d, char *orig)
{
	char first_arg[MAX_INPUT_LENGTH], *ptr;
	struct alias *a, *tmp;
	
  /* bail out immediately if the guy doesn't have any aliases */
	if ((tmp = GET_ALIASES(d->character)) == NULL)
		return 0;
	
  /* find the alias we're supposed to match */
	ptr = any_one_arg(orig, first_arg);
	
  /* bail out if it's null */
	if (!*first_arg)
		return 0;
	
  /* if the first arg is not an alias, return without doing anything */
	if ((a = find_alias(tmp, first_arg)) == NULL)
		return 0;
	
	if (a->type == ALIAS_SIMPLE) {
		strcpy(orig, a->replacement);
		return 0;
	} else {
		perform_complex_alias(&d->input, ptr, a);
		return 1;
	}
}



/***************************************************************************
 * Various other parsing utilities                                         *
 **************************************************************************/

/*
 * searches an array of strings for a target string.  "exact" can be
 * 0 or non-0, depending on whether or not the match must be exact for
 * it to be returned.  Returns -1 if not found; 0..n otherwise.  Array
 * must be terminated with a '\n' so it knows to stop searching.
 */
int search_block(char *arg, const char **list, bool exact) {
  register int i, l;
	
  /* Make into lower case, and get length of string */
	for (l = 0; *(arg + l); l++)
	  *(arg + l) = LOWER(*(arg + l));
	
	if (exact) {
	  for (i = 0; **(list + i) != '\n'; i++)
		if (!strcmp(arg, *(list + i)))
		  return (i);
	} 
	else {
	  if (!l)
		l = 1;			/* Avoid "" to match the first available string */
	  for (i = 0; **(list + i) != '\n'; i++)
		if (!strncmp(arg, *(list + i), l))
	      return (i);
	}
	
	return -1;
}


int is_number(char *str)
{
	while (*str)
		if (!isdigit(*(str++)))
			return 0;
		
	return 1;
}


void skip_spaces(char **string)
{
	for (; **string && isspace(**string); (*string)++);
}


char *delete_doubledollar(char *string)
{
	char *read, *write;
	
	if ((write = strchr(string, '$')) == NULL)
		return string;
	
	read = write;
	
	while (*read)
		if ((*(write++) = *(read++)) == '$')
			if (*read == '$')
				read++;
			
	*write = '\0';
	
	return string;
}


int fill_word(char *argument)
{
	return (search_block(argument, fill, TRUE) >= 0);
}


int reserved_word(char *argument)
{
	return (search_block(argument, reserved, TRUE) >= 0);
}


/*
 * copy the first non-fill-word, space-delimited argument of 'argument'
 * to 'first_arg'; return a pointer to the remainder of the string.
 */
char *one_argument(char *argument, char *first_arg)
{
	char *begin = first_arg;
	
	do {
		skip_spaces(&argument);
		
		first_arg = begin;
		while (*argument && !isspace(*argument)) {
			*(first_arg++) = LOWER(*argument);
			argument++;
		}
		
		*first_arg = '\0';
	} while (fill_word(begin));
	
	return argument;
}


/*
 * one_word is like one_argument, except that words in quotes ("") are
 * considered one word.
 */
char *one_word(char *argument, char *first_arg)
{
	char *begin = first_arg;
	
	do {
		skip_spaces(&argument);
		
		first_arg = begin;
		
		if (*argument == '\"') {
			argument++;
			while (*argument && *argument != '\"') {
				*(first_arg++) = LOWER(*argument);
				argument++;
			}
			argument++;
		} else {
			while (*argument && !isspace(*argument)) {
				*(first_arg++) = LOWER(*argument);
				argument++;
			}
		}
		
		*first_arg = '\0';
	} while (fill_word(begin));
	
	return argument;
}


/* same as one_argument except that it doesn't ignore fill words */
char *any_one_arg(char *argument, char *first_arg)
{
	skip_spaces(&argument);
	
	while (*argument && !isspace(*argument)) {
		*(first_arg++) = LOWER(*argument);
		argument++;
	}
	
	*first_arg = '\0';
	
	return argument;
}


/*
 * Same as one_argument except that it takes two args and returns the rest;
 * ignores fill words
 */
char *two_arguments(char *argument, char *first_arg, char *second_arg)
{
	return one_argument(one_argument(argument, first_arg), second_arg); /* :-) */
}



/*
 * determine if a given string is an abbreviation of another
 * (now works symmetrically -- JE 7/25/94)
 *
 * that was dumb.  it shouldn't be symmetrical.  JE 5/1/95
 *
 * returnss 1 if arg1 is an abbreviation of arg2
 */
int is_abbrev(char *arg1, char *arg2)
{
	if (!*arg1)
		return 0;
	
	for (; *arg1 && *arg2; arg1++, arg2++)
		if (LOWER(*arg1) != LOWER(*arg2))
			return 0;
		
	if (!*arg1)
		return 1;
	else
		return 0;
}



/* return first space-delimited token in arg1; remainder of string in arg2 */
void half_chop(char *string, char *arg1, char *arg2)
{
	char *temp;
	
	temp = any_one_arg(string, arg1);
	skip_spaces(&temp);
	strcpy(arg2, temp);
}



/* Used in specprocs, mostly.  (Exactly) matches "command" to cmd number */
int find_command(char *command)
{
	int cmd;
	
	for (cmd = 0; *cmd_info[cmd].command != '\n'; cmd++)
		if (!strcmp(cmd_info[cmd].command, command))
			return cmd;
		
	return -1;
}


int special(struct char_data *ch, int cmd, char *arg)
{
	register struct obj_data *i;
	register struct char_data *k;
	int j;
	
  /* special in room? */
	if (GET_ROOM_SPEC(ch->in_room) != NULL)
		if (GET_ROOM_SPEC(ch->in_room) (ch, world + ch->in_room, cmd, arg))
			return 1;
		
  /* special in equipment list? */
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch, j) && GET_OBJ_SPEC(GET_EQ(ch, j)) != NULL)
			if (GET_OBJ_SPEC(GET_EQ(ch, j)) (ch, GET_EQ(ch, j), cmd, arg))
				return 1;
			
  /* special in inventory? */
	for (i = ch->carrying; i; i = i->next_content)
		if (GET_OBJ_SPEC(i) != NULL)
			if (GET_OBJ_SPEC(i) (ch, i, cmd, arg))
				return 1;
			
  /* special in mobile present? */
	for (k = world[ch->in_room].people; k; k = k->next_in_room)
		if (GET_MOB_SPEC(k) != NULL)
			if (GET_MOB_SPEC(k) (ch, k, cmd, arg))
				return 1;
			
  /* special in object present? */
	for (i = world[ch->in_room].contents; i; i = i->next_content)
		if (GET_OBJ_SPEC(i) != NULL)
			if (GET_OBJ_SPEC(i) (ch, i, cmd, arg))
				return 1;
			
	return 0;
}



/* *************************************************************************
*  Stuff for controlling the non-playing sockets (get name, pwd etc)       *
************************************************************************* */


/* locate entry in p_table with entry->name == name. -1 mrks failed search */
int find_name(char *name)
{
	int i;
	
	for (i = 0; i <= top_of_p_table; i++) {
		if (!str_cmp((player_table + i)->name, name))
			return i;
	}
	
	return -1;
}


int _parse_name(char *arg, char *name)
{
	int i;
	
  /* skip whitespaces */
	for (; isspace(*arg); arg++);
	
	for (i = 0; (*name = *arg); arg++, i++, name++)
		if (!isalpha(*arg))
			return 1;
		
	if (!i)
		return 1;
	
	return 0;
}


#define RECON		1
#define USURP		2
#define UNSWITCH	3

int perform_dupe_check(struct descriptor_data *d)
{
	struct descriptor_data *k, *next_k;
	struct char_data *target = NULL, *ch, *next_ch;
	int mode = 0;
	
	int id = GET_IDNUM(d->character);
	
  /*
   * Now that this descriptor has successfully logged in, disconnect all
   * other descriptors controlling a character with the same ID number.
   */
	
	for (k = descriptor_list; k; k = next_k) {
		next_k = k->next;
		
		if (k == d)
			continue;
		
		if (k->original && (GET_IDNUM(k->original) == id)) {    /* switched char */
			SEND_TO_Q("\r\nMultiple login detected -- disconnecting.\r\n", k);
			STATE(k) = CON_CLOSE;
			if (!target) {
				target = k->original;
				mode = UNSWITCH;
			}
			if (k->character)
				k->character->desc = NULL;
			k->character = NULL;
			k->original = NULL;
		} 
		
		else if (k->character && (GET_IDNUM(k->character) == id)) {
			if (!target && STATE(k) == CON_PLAYING) {
				SEND_TO_Q("\r\nThis body has been usurped!\r\n", k);
				target = k->character;
				mode = USURP;
			}
			k->character->desc = NULL;
			k->character = NULL;
			k->original = NULL;
			SEND_TO_Q("\r\nMultiple login detected -- disconnecting.\r\n", k);
			STATE(k) = CON_CLOSE;
		}
	}
	
 /*
  * now, go through the character list, deleting all characters that
  * are not already marked for deletion from the above step (i.e., in the
  * CON_HANGUP state), and have not already been selected as a target for
  * switching into.  In addition, if we haven't already found a target,
  * choose one if one is available (while still deleting the other
  * duplicates, though theoretically none should be able to exist).
  */
	
	for (ch = character_list; ch; ch = next_ch) {
		next_ch = ch->next;
		
		if (IS_NPC(ch))
			continue;
		
		if (GET_IDNUM(ch) != id)
			continue;
		
    /* ignore chars with descriptors (already handled by above step) */
		if (ch->desc)
			continue;
		
    /* don't extract the target char we've found one already */
		if (ch == target)
			continue;
		
    /* we don't already have a target and found a candidate for switching */
		if (!target) {
			target = ch;
			mode = RECON;
			continue;
		}
		
    /* we've found a duplicate - blow him away, dumping his eq in limbo. */
		if (ch->in_room != NOWHERE)
			char_from_room(ch);
		char_to_room(ch, 1);
		extract_char(ch);
	}
	
  /* no target for swicthing into was found - allow login to continue */
	if (!target)
		return 0;
	
  /* Okay, we've found a target.  Connect d to target. */
	free_char(d->character); /* get rid of the old char */
	d->character = target;
	d->character->desc = d;
	d->original = NULL;
	d->character->char_specials.timer = 0;
	REMOVE_BIT(PLR_FLAGS(d->character), PLR_MAILING | PLR_WRITING);
	STATE(d) = CON_PLAYING;
	
	switch (mode) {
		case RECON:
			SEND_TO_Q("Reconnecting.\r\n", d);
			act("$n rinviene.", TRUE, d->character, 0, 0, TO_ROOM);
			sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
			break;
		case USURP:
			SEND_TO_Q("You take over your own body, already in use!\r\n", d);
			act("$n suddenly keels over in pain, surrounded by a white aura...\r\n"
				"$n's body has been taken over by a new spirit!",
				TRUE, d->character, 0, 0, TO_ROOM);
			sprintf(buf, "%s has re-logged in ... disconnecting old socket.",
				GET_NAME(d->character));
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
			break;
		case UNSWITCH:
			SEND_TO_Q("Reconnecting to unswitched char.", d);
			sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
			break;
	}
	
	return 1;
}


void roll_simple_abils(struct char_data * ch);
void accetta_roll(struct char_data * ch);

/* deal with newcomers and other non-playing sockets */
void nanny(struct descriptor_data *d, char *arg)
{
	char buf[2048];
	int player_i, load_result;
	int to_learn, conta, abil,i;
	char tmp_name[MAX_INPUT_LENGTH];
	struct char_file_u tmp_store, dummy; //Orione - dummy
	FILE *old_player_fl; //Orione
	extern room_rnum r_mortal_start_room[NUM_STARTROOMS +1];
	extern room_rnum r_immort_start_room;
	extern room_rnum r_frozen_start_room;
	extern const char *class_menu;
	extern const char *hometown_menu;
	extern const char *roll_stat_menu;
	extern int max_bad_pws;
	extern const int max_incremento[][6];
	extern struct abil_info_type abil_info[];
	
	room_rnum load_room;
	struct char_data *as;
	
	void menu_iniziazione(struct descriptor_data * e);
	void menu_mestieri(struct descriptor_data * e);
	void menu_scelta_abilita(struct descriptor_data * e);
	int attributo (struct char_data *ch, int num);
	int load_char(char *name, struct char_file_u *char_element);
	int parse_class(char arg);
	int parse_hometown(char arg);
	int parse_abils(char arg);
	
	
	skip_spaces(&arg);
	
	switch (STATE(d)) {
		
  /*. OLC states .
		case CON_OEDIT:
			oedit_parse(d, arg);
			break;
		case CON_REDIT:
			redit_parse(d, arg);
			break;
		case CON_ZEDIT:
			zedit_parse(d, arg);
			break;
		case CON_MEDIT:
			medit_parse(d, arg);
			break;
		case CON_SEDIT:
			sedit_parse(d, arg);
			break;
		case CON_TRIGEDIT:
			trigedit_parse(d, arg);
			break;
		case CON_WEDIT:
			wedit_parse(d, arg);
			break;
  . End of OLC states .*/
			
		case CON_GET_NAME:		/* wait for input of name */
		  
			if (d->character == NULL) {
				CREATE(d->character, struct char_data, 1);
				clear_char(d->character);
				CREATE(d->character->player_specials, struct player_special_data, 1);
				d->character->desc = d;
			}
			
			if (!*arg)
				close_socket(d);
			else {
				if ((_parse_name(arg, tmp_name)) || strlen(tmp_name) < 2 ||
					strlen(tmp_name) > MAX_NAME_LENGTH ||
					fill_word(strcpy(buf, tmp_name)) || reserved_word(buf)) {
					SEND_TO_Q("Nome invalido, prova un altro.\r\n"
						"Nome: ", d);
					return;
				}
				if ((player_i = load_char(tmp_name, &tmp_store)) > -1) {
					store_to_char(&tmp_store, d->character);
					GET_PFILEPOS(d->character) = player_i;
					
					if (PLR_FLAGGED(d->character, PLR_DELETED)) {
						free_char(d->character);
						CREATE(d->character, struct char_data, 1);
						clear_char(d->character);
						CREATE(d->character->player_specials, struct player_special_data, 1);
						d->character->desc = d;
						CREATE(d->character->player.name, char, strlen(tmp_name) + 1);
						strcpy(d->character->player.name, CAP(tmp_name));
						GET_PFILEPOS(d->character) = player_i;
						sprintf(buf, "Vuoi essere veramente, %s (Y/N)? ", tmp_name);
						SEND_TO_Q(buf, d);
						STATE(d) = CON_NAME_CNFRM;
					} 
					else {
	  /* undo it just in case they are set */
						REMOVE_BIT(PLR_FLAGS(d->character),
							PLR_WRITING | PLR_MAILING | PLR_CRYO);
						
						SEND_TO_Q("Password: ", d);
						echo_off(d);
						d->idle_tics = 0;
						STATE(d) = CON_PASSWORD;
					}
				} 
				else {
	/* player unknown -- make new character */

					if (!Valid_Name(tmp_name)) {
						SEND_TO_Q("Nome invalido, prova un altro.\r\n", d);
						SEND_TO_Q("Nome: ", d);
						return;
					}
					
					// Orione - verifico se esiste nel file old_player
					if (!(old_player_fl = fopen(OLD_PLAYER_FILE, "r"))){
						SEND_TO_Q("Errore di caricamento - Impossibile creare nuovi pg\r\n", d);
						sprintf (buf, "ATTENZIONE: impossibile aprire il file %s. Non si possono creare nuovi pg per permettere al mud di caricarsi.", OLD_PLAYER_FILE);
						mudlog (buf, OFF, LVL_GOD, TRUE);
						close_socket(d);
						return;
					}
					for (; !feof(old_player_fl); ) {
						fread(&dummy, sizeof(struct char_file_u), 1, old_player_fl);
						if (!str_cmp(dummy.name, tmp_name)) {
							SEND_TO_Q("Nome dell'antichita', contatta i creatori per riattivarlo.\r\n", d);
							fclose(old_player_fl);
							STATE(d) = CON_CLOSE;
							return;
						}
					}
					fclose(old_player_fl);
					
					CREATE(d->character->player.name, char, strlen(tmp_name) + 1);
					strcpy(d->character->player.name, CAP(tmp_name));

					sprintf(buf, "Vuoi essere veramente, %s (Y/N)? ", tmp_name);
					SEND_TO_Q(buf, d);
					STATE(d) = CON_NAME_CNFRM;
				}
			}
			break;
		case CON_NAME_CNFRM:		/* wait for conf. of new name    */
			if (UPPER(*arg) == 'Y') {
				if (isbanned(d->host) >= BAN_NEW) {
					sprintf(buf, "Request for new char %s denied from [%s] (siteban)",
						GET_NAME(d->character), d->host);
					mudlog(buf, NRM, LVL_GOD, TRUE);
					SEND_TO_Q("Sorry, new characters are not allowed from your site!\r\n", d);
					STATE(d) = CON_CLOSE;
					return;
				}
				
				if (is_same_ip(d)) {
					if (isunlocked(d->host)) {
						sprintf(buf, "IP [%s] sbloccato per %s", d->host, GET_NAME(d->character));
						mudlog(buf, NRM, LVL_GOD, TRUE);
					} else {
						SEND_TO_Q("Attenzione! Un'alto personaggio e' collegato con lo stesso IP.\r\n", d);
						STATE(d) = CON_CLOSE;
						sprintf(buf, "Multiplayer negato a %s IP: %s",
							GET_NAME(d->character), d->host);
						mudlog(buf, NRM, LVL_GOD, TRUE);
						return;
					}
				}	  
				if (restrict) {
					SEND_TO_Q("Al momento non e' possibile creare nuovi personaggi.\r\n", d);
					sprintf(buf, "Request for new char %s denied from %s (wizlock)",
						GET_NAME(d->character), d->host);
					mudlog(buf, NRM, LVL_GOD, TRUE);
					STATE(d) = CON_CLOSE;
					return;
				}
				SEND_TO_Q("Nuovo personaggio.\r\n", d);
				sprintf(buf, "Introduci la password per %s: ", GET_NAME(d->character));
				SEND_TO_Q(buf, d);
				echo_off(d);
				STATE(d) = CON_NEWPASSWD;
			} else if (*arg == 'n' || *arg == 'N') {
				SEND_TO_Q("Ok, quale' allora? ", d);
				free(d->character->player.name);
				d->character->player.name = NULL;
				STATE(d) = CON_GET_NAME;
			} else {
				SEND_TO_Q("Prego rispondere Yes o No: ", d);
			}
			break;
		case CON_PASSWORD:		/* get pwd for known player      */
    /*
     * To really prevent duping correctly, the player's record should
     * be reloaded from disk at this point (after the password has been
     * typed).  However I'm afraid that trying to load a character over
     * an already loaded character is going to cause some problem down the
     * road that I can't see at the moment.  So to compensate, I'm going to
     * (1) add a 15 or 20-second time limit for entering a password, and (2)
     * re-add the code to cut off duplicates when a player quits.  JE 6 Feb 96
     */
			
			echo_on(d);    /* turn echo back on */
			
			if (!*arg)
				close_socket(d);
			else {
#ifndef CIRCLE_WINDOWS
				if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character), MAX_PWD_LENGTH)) {
					sprintf(buf, "PW non valida: %s [%s]", GET_NAME(d->character), d->host);
					mudlog(buf, BRF, LVL_GOD, TRUE);
					GET_BAD_PWS(d->character)++;
					save_char(d->character, NOWHERE);
					if (++(d->bad_pws) >= max_bad_pws) {	/* 3 strikes and you're out. */
						SEND_TO_Q("Password sbagliata... disconessione.\r\n", d);
						STATE(d) = CON_CLOSE;
					} else {
						SEND_TO_Q("Password sbagliata.\r\nPassword: ", d);
						echo_off(d);
					}
					return;
				}
#endif
				load_result = GET_BAD_PWS(d->character);
				GET_BAD_PWS(d->character) = 0;
				
				if (isbanned(d->host) == BAN_SELECT &&
					!PLR_FLAGGED(d->character, PLR_SITEOK)) {
					SEND_TO_Q("Sorry, this char has not been cleared for login from your site!\r\n", d);
					STATE(d) = CON_CLOSE;
					sprintf(buf, "Connection attempt for %s denied from %s",
						GET_NAME(d->character), d->host);
					mudlog(buf, NRM, LVL_GOD, TRUE);
					return;
				}
				
				// i pg freezati non si connettono, cosi' non perdono eq - Ardanos
				if(PLR_FLAGGED(d->character, PLR_FROZEN)) {
					SEND_TO_Q("Il tuo pg e' Freezato. Ritenta appena uscira' dal freezer\r\n", d);
				        STATE(d) = CON_CLOSE;
					return;                             
				}
				
				if (is_same_ip(d)) {
					if (isunlocked(d->host)) {
						sprintf(buf, "IP [%s] sbloccato per %s", d->host, GET_NAME(d->character));
						mudlog(buf, NRM, LVL_GOD, TRUE);
					} else {
						SEND_TO_Q("Attenzione! Un'alto personaggio e' collegato con lo stesso IP.\r\n", d);
						STATE(d) = CON_CLOSE;
						sprintf(buf, "Multiplayer negato a %s IP: %s",
							GET_NAME(d->character), d->host);
						mudlog(buf, NRM, LVL_GOD, TRUE);
						return;
					}
				}
				
				if (GET_LEVEL(d->character) < restrict) {
					SEND_TO_Q("Momentaneamente siamo chiusi per alcune modifiche...riprova tra qualche giorno.\r\n", d);
					SEND_TO_Q(motd, d);
					STATE(d) = CON_CLOSE;
					sprintf(buf, "Request for login denied for %s [%s] (wizlock)",
						GET_NAME(d->character), d->host);
					mudlog(buf, NRM, LVL_GOD, TRUE);
					return;
				}
      
      /* check and make sure no other copies of this player are logged in */
				if (perform_dupe_check(d))
					return;
				
				if (GET_LEVEL(d->character) >= LVL_IMMORT)
					SEND_TO_Q(imotd, d);
				else
					SEND_TO_Q(motd, d);
				
				sprintf(buf, "%s [%s] has connected.", GET_NAME(d->character), d->host);
				mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
				
				if (load_result) {
					sprintf(buf, "\r\n\r\n\007\007\007"
						"%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\r\n",
						CCRED(d->character, C_SPR), load_result,
						(load_result > 1) ? "S" : "", CCNRM(d->character, C_SPR));
					SEND_TO_Q(buf, d);
					GET_BAD_PWS(d->character) = 0;
				}
				SEND_TO_Q("\r\n\n*** PRESS RETURN: ", d);
				STATE(d) = CON_RMOTD;
			}
			break;
			
		case CON_NEWPASSWD:
		case CON_CHPWD_GETNEW:
			if (!*arg || strlen(arg) > MAX_PWD_LENGTH || strlen(arg) < 3 ||
				!str_cmp(arg, GET_NAME(d->character))) {
				SEND_TO_Q("\r\nPassword non ammessa.\r\n", d);
				SEND_TO_Q("Password: ", d);
				return;
			}
			strncpy(GET_PASSWD(d->character), CRYPT(arg, GET_NAME(d->character)), MAX_PWD_LENGTH);
			*(GET_PASSWD(d->character) + MAX_PWD_LENGTH) = '\0';
			
			SEND_TO_Q("\r\nRidigita la password: ", d);
			if (STATE(d) == CON_NEWPASSWD)
				STATE(d) = CON_CNFPASSWD;
			else
				STATE(d) = CON_CHPWD_VRFY;
			
			break;
			
		case CON_CNFPASSWD:
		case CON_CHPWD_VRFY:
			if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character),
					MAX_PWD_LENGTH)) {
				SEND_TO_Q("\r\n Le Passwords non coincidono... ricomincia.\r\n", d);
				SEND_TO_Q("Password: ", d);
				if (STATE(d) == CON_CNFPASSWD)
					STATE(d) = CON_NEWPASSWD;
				else
					STATE(d) = CON_CHPWD_GETNEW;
				return;
			}
			echo_on(d);
			
			if (STATE(d) == CON_CNFPASSWD) {
				SEND_TO_Q("Quale' il tuo sesso (M/F)? ", d);
				STATE(d) = CON_QSEX;
			} else {
				save_char(d->character, NOWHERE);
				echo_on(d);
				SEND_TO_Q("\r\nFatto.\n\r", d);
				SEND_TO_Q(MENU, d);
				STATE(d) = CON_MENU;
			}
			
			break;
			
		case CON_QSEX:		/* query sex of new user         */
			switch (*arg) {
				case 'm':
				case 'M':
					d->character->player.sex = SEX_MALE;
					break;
				case 'f':
				case 'F':
					d->character->player.sex = SEX_FEMALE;
					break;
				default:
					SEND_TO_Q("That is not a sex..\r\n"
						"What IS your sex? ", d);
					return;
					break;
			}
			
			SEND_TO_Q(class_menu, d);
			STATE(d) = CON_QCLASS;
			d->character->aff_abils.str_add = -4; // Numero di tentativi per il rollaggio
			break;
			
		case CON_QCLASS:
			GET_CLASS(d->character) = CLASS_DARESIANO;
			
			roll_simple_abils(d->character);
			
			SEND_TO_Q(roll_stat_menu, d);
			
			sprintf(buf, "  Forza                   %2d\r\n", d->character->aff_abils.str);
			sprintf(buf+strlen(buf), "  Intelligenza            %2d\r\n", d->character->aff_abils.intel);
			sprintf(buf+strlen(buf), "  Saggezza                %2d\r\n", d->character->aff_abils.wis);
			sprintf(buf+strlen(buf), "  Destrezza               %2d\r\n", d->character->aff_abils.dex);
			sprintf(buf+strlen(buf), "  Costituzione            %2d\r\n", d->character->aff_abils.con);
			sprintf(buf+strlen(buf), "  Carisma                 %2d\r\n\r\n", d->character->aff_abils.cha);
			
			if (d->character->aff_abils.str_add < 0)
			{
				if (d->character->aff_abils.str_add == -1)
					strcat(buf, "Vuoi accettare questi attributi (Ti resta ancora un ultimo tentativo) ? ");
				else
					sprintf(buf+strlen(buf), "Vuoi accettare questi attributi (Ti restano ancora %d tentativi) ? ", (-(d->character->aff_abils.str_add)));
				STATE(d) = CON_QACCETTAROLL;
			}
			else
			{
				accetta_roll(d->character);
				strcat(buf, "Premi un tasto per continuare\r\n");
				STATE(d) = CON_QMESTIERI;
			}
			SEND_TO_Q(buf, d);
			break;
			
		case CON_QACCETTAROLL:
			if (!arg || (strcmp(arg, "si") && strcmp(arg, "no")))
				SEND_TO_Q("Per favore digita si o no: ", d);
			else if (!strcmp(arg, "si"))
			{
				accetta_roll(d->character);
				menu_mestieri(d);
				STATE(d) = CON_QMESTIERI;
			}
			else
			{
				d->character->aff_abils.str_add++;
				STATE(d) = CON_QCLASS;
				nanny(d, arg); // Per evitare l'attesa del tasto
			}
			break;
			
		case CON_QMESTIERI:
			load_result = atoi(arg);
			if (load_result<1 || load_result>(TOT_PROF_MESTIERI-3) || d->character->player_specials->saved.prof_choice[0]==load_result+3)
			{
				SEND_TO_Q("Scelta non valida !\r\n", d);
				menu_mestieri(d);
				return;
			}
			if (d->character->player_specials->saved.prof_choice[0]==0)
			{
				d->character->player_specials->saved.prof_choice[0]=load_result+3;
				GET_PROF(d->character, load_result+3) = 40;
				if (load_result+3 == PROF_ADDESTRATORE)
				{
					GET_PROF(d->character, PROF_MONTARE) = 50;
					GET_PROF(d->character, PROF_CAVALCARE) = 50;
					GET_PROF(d->character, PROF_DOMARE) = 50;
				}
				menu_mestieri(d);
			}
			else
			{
				d->character->player_specials->saved.prof_choice[1]=load_result+3;
				GET_PROF(d->character, load_result+3) = 30;
				if (load_result+3 == PROF_ADDESTRATORE)
				{
					GET_PROF(d->character, PROF_MONTARE) = 40;
					GET_PROF(d->character, PROF_CAVALCARE) = 40;
					GET_PROF(d->character, PROF_DOMARE) = 40;
				}
				SEND_TO_Q("\r\n\r\n\r\n", d);
				SEND_TO_Q(hometown_menu, d);
				SEND_TO_Q("\r\nCitta': ", d);
				STATE(d) = CON_QCITTA;
			}
			break;
			
		case CON_QCITTA:
			load_result = parse_hometown(*arg);
			if (load_result==1)
			{
				SEND_TO_Q("\r\n"
					"Si narra che in passato gloriosi cavalieri venissero dalla citta' di Jiroch\r\n"
					"ma al giorno d'oggi nessuno ha piu' il tempo di ascoltare queste storie...\r\n", d);
				SEND_TO_Q(hometown_menu, d);
				SEND_TO_Q("\r\nCitta': ", d);
				return;
			}
			
			GET_HOME(d->character) = load_result+1;
			assign_to_regno(d->character,load_result);
			load_room=r_mortal_start_room[load_result +1];
			if (GET_PFILEPOS(d->character) < 0)
				GET_PFILEPOS(d->character) = create_entry(GET_NAME(d->character));
			init_char(d->character);
			save_char(d->character, NOWHERE);
			SEND_TO_Q(motd, d);
			SEND_TO_Q("\r\n\n*** PRESS RETURN: ", d);
			STATE(d) = CON_RMOTD;
			
			sprintf(buf, "%s [%s] new player.", GET_NAME(d->character), d->host);
			mudlog(buf, NRM, LVL_IMMORT, TRUE);
			break;
			
		case CON_RMOTD:		/* read CR after printing motd   */
			SEND_TO_Q(MENU, d);
			STATE(d) = CON_MENU;
			break;
		case CON_CONF_INIZIAZ:        /* Conferma si-no dell'iniziazione */
			if (!arg || (strcmp(arg, "si") && strcmp(arg, "no")))
				SEND_TO_Q("Per favore digita si o no: ", d);
			else if (!strcmp(arg, "si"))
			{
				STATE(d) = CON_AGPT_INIZIAZ;
				menu_iniziazione(d);
			}
			else
				STATE(d) = CON_PLAYING;
			break;
			
		case CON_AGPT_INIZIAZ:        /* Gestisce la modifica punti nell'iniziazione */
			load_result = parse_abils(*arg);
			if (load_result == -1)
			{
				SEND_TO_Q("Non e' un attributo, riprova: ", d);
				return;
			}
			if (attributo(d->character, load_result)+d->character->iniz_ptagg[load_result]>=16)
			{
				SEND_TO_Q("Quell'attributo e' gia' a 16 o piu', non puo' essere superiore, riprova: ", d);
				return;
			}
			if (d->character->iniz_ptagg[load_result] >= max_incremento[d->character->iniz_classe][load_result])
			{
				SEND_TO_Q("Quell'attributo non puo' essere incrementato oltre l'incremento attuale, riprova: ", d);
				return;
			}
			d->character->iniz_ptagg[load_result]++;
			d->character->iniz_da_aggiungere--;
			menu_iniziazione(d);
			break;
			
		case CON_PREMI_TASTO:
			menu_scelta_abilita(d);
			STATE(d) = CON_SCELTA_ABIL;
			break;
			
		case CON_SCELTA_ABIL: // Scelta delle abilita' in iniziazione
			load_result = atoi(arg);
			to_learn=0;
			for (i=0; i<3; i++)
				if (d->character->player_specials->saved.abil_choice[i]==0) to_learn++;
			conta=1;
			abil=-1;
			for (i=1; i<TOP_ABILITA_DEFINE; i++)
				if (abil_info[i].start_abilita[(int)GET_CLASS(d->character)] &&
					d->character->player_specials->saved.abil_choice[0]!=i   &&
					d->character->player_specials->saved.abil_choice[1]!=i   &&
					d->character->player_specials->saved.abil_choice[2]!=i )
				{
					if (conta==load_result) abil=i;
					conta++;
				}
			if (abil==-1)
			{
				SEND_TO_Q("\r\n\r\nScelta non valida !!!!", d);
				menu_scelta_abilita(d);
				return;
			}
			if (to_learn>0 && to_learn<=3)
				d->character->player_specials->saved.abil_choice[3-to_learn]=abil;
			if (to_learn>1)
				menu_scelta_abilita(d);
			else
			{
				SEND_TO_Q("\r\n\r\nBenvenuto nel nostro ordine !!! Ora sei veramente uno di noi,\r\n"
					"non esitare a chiedere aiuto ai membri del nostro ordine in\r\n"
					"qualsiasi momento tu abbia bisogno...\r\n"
					"Il nostro precettore ti attende all'interno della gilda.\r\n", d);
				GET_GOLD(d->character)-=40000;
				STATE(d) = CON_PLAYING;
				if(GET_CLASS(d->character)==CLASS_MAGIC_USER||GET_CLASS(d->character)==CLASS_CLERIC)
					GET_SKILL(d->character,SKILL_READ_MAGIC)=60;
				save_char(d->character, NOWHERE);
			}
			break;
			
		case CON_MENU:		/* get selection from main menu  */
			switch (*arg) {
				case '0':
					close_socket(d);
					break;
					
				case '1':
					reset_char(d->character);
					if (PLR_FLAGGED(d->character, PLR_INVSTART))
						GET_INVIS_LEV(d->character) = GET_LEVEL(d->character);
					if ((load_result = Crash_load(d->character)))
						d->character->in_room = real_room(r_mortal_start_room[GET_HOME(d->character)]);
      /* with the copyover patch, this next line goes in enter_player_game() */
					GET_ID(d->character) = GET_IDNUM(d->character);
					//save_char(d->character, real_room(r_mortal_start_room[GET_HOME(d->character)]));
					send_to_char(WELC_MESSG, d->character);
					d->character->next = character_list;
					character_list = d->character;
					
					if ((load_room = GET_LOADROOM(d->character)) != NOWHERE)
						load_room = real_room(load_room);
					
      /* If char was saved with NOWHERE, or real_room above failed... */
					if (load_room == NOWHERE) {
						if (GET_LEVEL(d->character) >= LVL_QUEST_MASTER) {
							load_room = r_immort_start_room;
						} else {
							load_room = r_mortal_start_room[GET_HOME(d->character)];
						}
					}
					
					if (PLR_FLAGGED(d->character, PLR_FROZEN))
						load_room = r_frozen_start_room;
					if (IS_AFFECTED(d->character, AFF_GROUP))
						REMOVE_BIT(AFF_FLAGS(d->character), AFF_GROUP);
					if (IS_AFFECTED(d->character, AFF_MARK)) {
						as = read_mobile(AS_RNUM(d->character), REAL);
						char_to_room(as, AS_ROOM(d->character));
						SET_BIT(MOB_FLAGS(as), MOB_ASSASSIN);
						SET_BIT(MOB_FLAGS(as), MOB_HUNTER);
						remember(as, d->character);
						HUNTING(as) = d->character;
						ASSASSIN(d->character) = as;
					}
					save_char(d->character, load_room);
					char_to_room(d->character, load_room);
					act("$n e' entrato in gioco.", TRUE, d->character, 0, 0, TO_ROOM);
					
					greet_mtrigger(d->character, -1);      //Orione
					greet_memory_mtrigger(d->character);	//Orione

					STATE(d) = CON_PLAYING;
					if (!GET_LEVEL(d->character)) {
						do_start(d->character);
						send_to_char(START_MESSG, d->character);
						do_newbie(d->character);
					}
					look_at_room(d->character, 0);
					if (has_mail(GET_IDNUM(d->character)))
						send_to_char("Hai ricevuto una lettera.\r\n", d->character);
					if (load_result == 2) {	/* rented items lost */
						send_to_char("\r\n\007Non avevi abbastanza soldi per il rent!\r\n"
							"I tuoi averi sono stati donati ai bisognosi!\r\n",
							d->character);
					}
					if (GET_LEVEL(d->character) > 5 && GET_LEVEL(d->character) < LVL_QUEST_MASTER)
						if (!affected_by_spell(d->character, SPELLSKILL, DISEASE_XHYPHYS) && !number(0, 700))
							mag_affects (70, d->character, d->character, DISEASE_XHYPHYS, 0);
					if (AFFECTED(d->character, AFF_SLEEP, BITV_SLEEP))
						GET_POS(d->character) = POS_SLEEPING;
					d->prompt_mode = 1;
					read_aliases(d->character);
					break;
					
				case '2':
					sprintf(buf,"SYSLOG: %s entra nella descrizione del personaggio ",d->character->player.name);
					mudlog(buf,CMP,LVL_GOD, TRUE);
					SEND_TO_Q("Scrivi qui il testo che vuoi che vedano gli altri quando ti guardano.\r\n", d);
					SEND_TO_Q("(/s per salvare /h per aiuto\r\n", d);
					if (d->character->player.description) {
						SEND_TO_Q("Current description:\r\n", d);
						SEND_TO_Q(d->character->player.description, d);
	/* don't free this now... so that the old description gets loaded */
	/* as the current buffer in the editor */
	/* free(d->character->player.description); */
	/* d->character->player.description = NULL; */
	/* BUT, do setup the ABORT buffer here */
						d->backstr = str_dup(d->character->player.description);
					}
					d->str = &d->character->player.description;
					d->max_str = EXDSCR_LENGTH;
					STATE(d) = CON_EXDESC;
					break;
					
				case '3':
					page_string(d, background, 0);
					STATE(d) = CON_RMOTD;
					break;
					
				case '4':
					SEND_TO_Q("\r\nEnter your old password: ", d);
					echo_off(d);
					STATE(d) = CON_CHPWD_GETOLD;
					break;
					
				case '5':
					SEND_TO_Q("\r\nEnter your password for verification: ", d);
					echo_off(d);
					STATE(d) = CON_DELCNF1;
					break;
					
				default:
					SEND_TO_Q("\r\nThat's not a menu choice!\r\n", d);
					SEND_TO_Q(MENU, d);
					break;
			}
			
			break;
			
		case CON_CHPWD_GETOLD:
			if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character), MAX_PWD_LENGTH)) {
				echo_on(d);
				SEND_TO_Q("\r\nIncorrect password.\r\n", d);
				SEND_TO_Q(MENU, d);
				STATE(d) = CON_MENU;
				return;
			} else {
				SEND_TO_Q("\r\nEnter a new password: ", d);
				STATE(d) = CON_CHPWD_GETNEW;
				return;
			}
			break;
			
		case CON_DELCNF1:
			echo_on(d);
			if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character), MAX_PWD_LENGTH)) {
				SEND_TO_Q("\r\nIncorrect password.\r\n", d);
				SEND_TO_Q(MENU, d);
				STATE(d) = CON_MENU;
			} else {
				SEND_TO_Q("\r\nYOU ARE ABOUT TO DELETE THIS CHARACTER PERMANENTLY.\r\n"
					"ARE YOU ABSOLUTELY SURE?\r\n\r\n"
					"Please type \"yes\" to confirm: ", d);
				STATE(d) = CON_DELCNF2;
			}
			break;
			
		case CON_DELCNF2:
			if (!strcmp(arg, "yes") || !strcmp(arg, "YES")) {
				if (PLR_FLAGGED(d->character, PLR_NODELETE)) {
					SEND_TO_Q("Il personaggio non e' cancellabile, mi spiace\r\n", d);
					STATE(d) = CON_CLOSE;
					return;
				}
				if (PLR_FLAGGED(d->character, PLR_FROZEN)) {
					SEND_TO_Q("You try to kill yourself, but the ice stops you.\r\n", d);
					SEND_TO_Q("Character not deleted.\r\n\r\n", d);
					STATE(d) = CON_CLOSE;
					return;
				}
				if (GET_LEVEL(d->character) < LVL_GRGOD)
					SET_BIT(PLR_FLAGS(d->character), PLR_DELETED);
				d->character->player_specials->saved.clan=0;
				d->character->player_specials->saved.clan_rank=0;
				d->character->player_specials->saved.privilegi=0;
				save_char(d->character, NOWHERE);
				Crash_delete_file(GET_NAME(d->character));
				sprintf(buf, "Character '%s' deleted!\r\n"
					"Goodbye.\r\n", GET_NAME(d->character));
				SEND_TO_Q(buf, d);
				sprintf(buf, "%s (lev %d) has self-deleted.", GET_NAME(d->character),
					GET_LEVEL(d->character));
				mudlog(buf, NRM, LVL_GOD, TRUE);
				STATE(d) = CON_CLOSE;
				return;
			} else {
				SEND_TO_Q("\r\nCharacter not deleted.\r\n", d);
				SEND_TO_Q(MENU, d);
				STATE(d) = CON_MENU;
			}
			break;
			
		case CON_CLOSE:
			close_socket(d);
			break;
			
		default:
			log("SYSERR: Nanny: illegal state of con'ness; closing connection");
			close_socket(d);
			break;
	}
}

struct char_data *is_playing(char *vict_name)
{
	extern struct descriptor_data *descriptor_list;
	struct descriptor_data *i, *next_i;
	
	for (i = descriptor_list; i; i = next_i) {
		next_i = i->next;
		if(i->connected == CON_PLAYING && (str_cmp(i->character->player.name,vict_name)==0))
			return i->character;
	}
	return NULL;
}

int is_same_ip(struct descriptor_data *dch)
{
	struct descriptor_data *d;
	extern struct descriptor_data *descriptor_list;
	int cont=0;
	
	if (GET_LEVEL(dch->character)>=LVL_QUEST_MASTER) return FALSE;      
	
	for (d = descriptor_list; d; d = d->next){
		if ((d->character) && (str_cmp(dch->host, d->host)==0) &&
			(GET_LEVEL(d->character)<LVL_QUEST_MASTER) && (GET_NAME(d->character)!=NULL) &&
			(GET_NAME(dch->character)!=NULL) &&
			(str_cmp(GET_NAME(dch->character), GET_NAME(d->character))!=0)
			){
			if (PLR_FLAGGED(dch->character, PLR_MULTIPLAYER)) 
				if (cont>=1) return TRUE;
				else cont++;			
			else
				return TRUE;
		}
	}
	return FALSE;
}

ACMD(do_regnopay)
{
	//extern void tax_improve(void);
	extern void stipendi_mensili(void);
	extern void update_bilancio_regni(void);
	
	stipendi_mensili();
//tax_improve();
	update_bilancio_regni();
}

ACMD(do_gildapay)
{
	extern void new_tax_improve(void);
	extern void new_stipendi_mensili(void);
	extern void new_update_bilancio_regni(void);
	
	new_stipendi_mensili();
	new_tax_improve();
	new_update_bilancio_regni();
}


ACMD(do_fedina)
{
	char arg2[255];
	extern void fedina_penale(struct char_data * ch, struct char_data * vict,int regno);
	struct char_data *vict;
	
	argument = one_argument(argument, arg2); 
	
	if (strcmp(arg2,"")) {    
		if(!(vict = get_player_vis(ch, arg2, 0))) {
			send_to_char("Non c'e' nessuno con questo nome.\r\n", ch);
			return;
		}
		if (AFF_FLAGGED(vict, AFF_DISGUISE) && GET_LEVEL(ch) < LVL_IMMORT) {
			send_to_char("Non c'e' nessuno con questo nome.\r\n", ch);
			return;
		}
		fedina_penale(ch, vict, -1);
	}
	else {  
		fedina_penale(ch, ch, -1);
	}	
}
