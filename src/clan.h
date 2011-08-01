/*****new file clan.h*****/



#define MAX_HISTORY      30

#define MAX_CLANS	 20

#define LVL_CLAN_GOD	 LVL_GOD

#define CLAN_PLAN_LENGTH 800

#define MAX_RANKS        30

#define DEFAULT_RANKS    29

#define MAX_MOB_ASSOLDABILI 30  /*tipi di mob assoldabili*/

#define GET_CLAN(ch)		((ch)->player_specials->saved.clan)

#define GET_CLAN2(chdata)       (chdata.player_specials_saved.clan)

#define GET_CLAN_RANK(ch)	((ch)->player_specials->saved.clan_rank)

#define GET_CLAN_RANK2(chdata)  (chdata.player_specials_saved.clan_rank)

#define GET_CLAN_PRIV(ch)       ((ch)->player_specials->saved.privilegi)

#define GET_CLAN_PRIV2(chdata)  (chdata.player_specials_saved.privilegi)

#define GET_STIPENDIO(ch)       ((ch)->player_specials->saved.stipendio)

#define GET_STIPENDIO2(chdata)  (chdata.player_specials_saved.stipendio)

#define GET_CLAN_DIPLO(clan1, clan2) (clan[clan1].diplomacy[clan2].status)



#define CP_SET_PLAN   (1 << 0)

#define CP_ENROLL     (1 << 1)

#define CP_EXPEL      (1 << 2)

#define CP_PROMOTE    (1 << 3)

#define CP_DEMOTE     (1 << 4)

#define CP_GIVE_FEES  (1 << 5)

#define CP_SET_TAX    (1 << 6)

#define CP_WITHDRAW   (1 << 7)

#define CP_SET_RANK   (1 << 8)

#define CP_DIPLO      (1 << 9)

#define CP_ECONOMY_V  (1 << 10)

#define CP_ECONOMY_AQ (1 << 11)

#define CP_GIVE_PRIV  (1 << 12)

#define CP_TOGG_PRIV  (1 << 13)

#define CP_ZECCA_EXP  (1 << 14)

#define CP_GIUDICE    (1 << 15)

#define CP_MNGSHOP	 (1 << 16)

#define CP_SECONDINO	 (1 << 17)

#define NUM_CP         18





#define PEACE         0

#define ALLIANCE      1

#define WAR           2

#define VASSALLO      3

#define SURRENDER     4



#define CM_DUES   1

#define CM_APPFEE 2



#define CB_DEPOSIT  1

#define CB_WITHDRAW 2

/*Lance da implementare e utilizzato per i negozi e mob*/

//#define CP_MNGSHOP CP_WITHDRAW

#define CP_MNGMOB  CP_WITHDRAW





#define REGNO     0

#define FEUDO     1

#define CONTEA    2

#define DUCATO    3

#define VILLAGGIO 4

#define CITTA     5

#define TRIBU     6

#define CLAN      7

#define GILDA     8

#define TIPI_ASSOCIAZIONE 9



/*Lance da implementare e utilizzato per i negozi*/

//#define CP_MNGSHOP CP_WITHDRAW





#define CLAN_MODE(d) ((d)->clan->mode)



void save_clans(void);

void init_clans(void);

sh_int find_clan_by_id(int clan_id);

sh_int find_clan(char *name);



extern int num_of_clans;

int clan_num;  



struct mess_history{

	int new;

	int prop;

	int esito;

	int timer;

	char name[32];

};

typedef struct mess_history HISTORY;





struct regno{

	int proposta;

	int timer;

	int id;

	int status;

	char name[32];

};



typedef struct regno Regno ;





struct clan_economy{

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





struct clan_rec {

	int	id;
	int   regno;
	int   type;/*tipo di clan*/
	int   capitale_regno;/*vnum della capitale del regno,tutti i pg 
			che nascono in questa zona automatic sono sudditi
			di questo regno*/
	char	name[32];
	char  leader[20];/*nome del leader*/
	ubyte	ranks;
	char	rank_name[MAX_RANKS][20];
	int	automatic_ranks;
	int   rank_bounds[MAX_RANKS];
	int   rank_tax[MAX_RANKS];
/*
	int	members;

	int	power;

	int   fama;
*/
	int truppe_disponibili;
	int valore_attacco;
	int valore_difesa;
	Regno diplomacy[MAX_CLANS];
	struct clan_economy economy;
	int mob_assoldabili[MAX_MOB_ASSOLDABILI];

};



extern struct clan_rec clan[MAX_CLANS];



struct clan_data{

	int mode;

	int clanid;

};















