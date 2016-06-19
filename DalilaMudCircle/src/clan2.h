/* ************************************************************************
*   File: clan2.h                                     Part of Dalila2mud  *
*  Usage: header file: gild manager                                       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1999 by the Micale Adriano                           *
*  Dalila2mud is based on CircleMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define NEW_MAX_HISTORY      30
#define NEW_MAX_CLANS	 20
#define NEW_LVL_CLAN_GOD	 LVL_GOD
#define NEW_CLAN_PLAN_LENGTH 800
#define NEW_MAX_RANKS        30
#define NEW_DEFAULT_RANKS    29
#define NEW_MAX_MOB_ASSOLDABILI 30  /*tipi di mob assoldabili*/
#define NEW_GET_CLAN(ch)		((ch)->player_specials->saved.new_clan)
#define NEW_GET_CLAN2(chdata)       (chdata.player_specials_saved.new_clan)
#define NEW_GET_CLAN_RANK(ch)	((ch)->player_specials->saved.new_clan_rank)
#define NEW_GET_CLAN_RANK2(chdata)  (chdata.player_specials_saved.new_clan_rank)
#define NEW_GET_CLAN_PRIV(ch)       ((ch)->player_specials->saved.new_privilegi)
#define NEW_GET_CLAN_PRIV2(chdata)  (chdata.player_specials_saved.new_privilegi)
/*
#define NEW_GET_STIPENDIO(ch)       ((ch)->player_specials->saved.new_stipendio)
#define NEW_GET_STIPENDIO2(chdata)  (chdata.player_specials_saved.new_stipendio)
*/
#define NEW_GET_CLAN_DIPLO(clan1, clan2) (new_clan[clan1].diplomacy[clan2].status)


#define NEW_GET_STIPENDIOHAS_FEE(ch)       ((ch)->player_specials->saved.new_stipendio_hasfee)
#define NEW_GET_STIPENDIOHAS_FEE2(chdata)  (chdata.player_specials_saved.new_stipendio_hasfee)
#define NEW_GET_STIPENDIOCLAN_FEE(ch)       ((ch)->player_specials->saved.new_stipendio_clanfee)
#define NEW_GET_STIPENDIOCLAN_FEE2(chdata)  (chdata.player_specials_saved.new_stipendio_clanfee)




#define NEW_CP_SET_PLAN   (1 << 0)
#define NEW_CP_ENROLL     (1 << 1)
#define NEW_CP_EXPEL      (1 << 2)
#define NEW_CP_PROMOTE    (1 << 3)
#define NEW_CP_DEMOTE     (1 << 4)
#define NEW_CP_GIVE_FEES  (1 << 5)
#define NEW_CP_SET_TAX    (1 << 6)
#define NEW_CP_WITHDRAW   (1 << 7)
#define NEW_CP_SET_RANK   (1 << 8)
#define NEW_CP_DIPLO      (1 << 9)
#define NEW_CP_ECONOMY_V  (1 << 10)
#define NEW_CP_ECONOMY_AQ (1 << 11)
#define NEW_CP_GIVE_PRIV  (1 << 12)
#define NEW_CP_TOGG_PRIV  (1 << 13)
//#define NEW_CP_ZECCA_EXP  (1 << 14)
//#define NEW_NUM_CP         15
#define NEW_NUM_CP         14

#define NEW_PEACE         0
#define NEW_ALLIANCE      1
#define NEW_WAR           2
#define NEW_VASSALLO      3
#define NEW_SURRENDER     4

#define NEW_CM_DUES   1
#define NEW_CM_APPFEE 2

#define NEW_CB_DEPOSIT  1
#define NEW_CB_WITHDRAW 2
/*Lance da implementare e utilizzato per i negozi e mob*/
#define NEW_CP_MNGSHOP NEW_CP_WITHDRAW
#define NEW_CP_MNGMOB  NEW_CP_WITHDRAW


#define NEW_REGNO     0
#define NEW_FEUDO     1
#define NEW_CONTEA    2
#define NEW_DUCATO    3
#define NEW_VILLAGGIO 4
#define NEW_CITTA     5
#define NEW_TRIBU     6
#define NEW_CLAN      7
#define NEW_GILDA     8
#define NEW_TIPI_ASSOCIAZIONE 9

/*Lance da implementare e utilizzato per i negozi*/
#define NEW_CP_MNGSHOP NEW_CP_WITHDRAW


#define NEW_CLAN_MODE(d) ((d)->new_clan->mode)

void new_save_clans(void);
void new_init_clans(void);
sh_int find_new_clan_by_id(int clan_id);
sh_int find_new_clan(char *name);

extern int new_num_of_clans;
int new_clan_num;  

struct new_mess_history{
  int new;
  int prop;
  int esito; 
  int timer;
  char name[32];
};
typedef struct new_mess_history NEWHISTORY;

struct new_regno{
int proposta;
int timer;
int id;
int status;
char name[32];
};

typedef struct new_regno New_Regno ;


struct new_clan_economy{
  int treasure;
  int last_sem_treasure;
  int last_mens_treasure;
  int last_entrate_sem;
  int last_uscite_sem;
  int last_uscite_mens;
  int last_entrate_mens;
  int current_uscite;/*uscite*/
  int current_entrate;/*ultimo mese|mud*/
  int last_tax_income;/*ogni 6 mesi*/
  int tax_income;
  int last_dogana_income;/*ultimo mese*/
  int current_dogana_income;
  int current_zecca_income;
  int last_zecca_income;
  int corrent_export_income;
  int last_export_income;
  int current_shops_balance;
  int last_stipendi;
  int stipendi;
};

  
struct new_clan_rec {
  int	id;
  int   regno;
  int   type;/*tipo di clan*/
  int   capitale_regno;/*vnum della capitale del regno,tutti i pg 
			che nascono in questa zona automatic sono sudditi
			di questo regno*/
  char	name[32];
  char  leader[20];/*nome del leader*/
  ubyte	ranks;
  char	rank_name[NEW_MAX_RANKS][20];
  int	automatic_ranks;
  int   rank_bounds[NEW_MAX_RANKS];
  int   rank_tax[NEW_MAX_RANKS];
  int	members;
  int	power;
  int   fama;
  New_Regno diplomacy[NEW_MAX_CLANS];
  struct new_clan_economy economy;
  int mob_assoldabili[NEW_MAX_MOB_ASSOLDABILI];
};

struct new_clan_data{
  int mode;
  int clanid;
};

extern struct new_clan_rec new_clan[NEW_MAX_CLANS];







