/**************************************************************************
 * File: regni.c     Basato su clan.c  per la gestione dei regni in       *
 *                          DalilaMud                                     *
 *  Written by Enrico Lazzaretto                                          *
 *  Questo file e' proprieta esclusiva dello staff di Dalila              *
 *                                                                        *
 **************************************************************************/

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "spells.h"
#include "handler.h"
#include "clan.h"
#include "shop.h"
#include "house.h"

int num_clan;
int num_of_clans;
struct clan_rec clan[MAX_CLANS];

extern void save_char_file_u(struct char_file_u st);
extern struct descriptor_data *descriptor_list;
extern struct char_data *is_playing(char *vict_name);
extern int top_of_p_table;
extern struct player_index_element *player_table;
extern int get_no_play(struct char_data *ch,struct char_file_u chdata,int time_out,int inplay);
extern struct room_data *world;
extern void House_save_control(void);
extern int num_of_houses;
extern struct house_control_rec house_control[MAX_HOUSES];

extern char * string(char *s1,char *buf,bool flag);
int real_zone(room_vnum number);
void sedit_save_to_disk(int zone);
void show_status_regno(struct char_data *ch);
void do_clan_set_status(struct char_data *ch,char *arg);
void do_clan_set_tasse(struct char_data *ch,char *argument);
void do_clan_assign_zone(struct char_data *ch,char *argument);
void do_clan_economy(struct char_data *ch,char *arg);
void do_clan_stipendio(struct char_data *ch,char *arg);
void do_clan_licenzia(struct char_data*ch,char *arg);
void do_clan_set_limits(struct char_data *ch,char *arg);
void do_clan_set_truppe(struct char_data *ch,char *arg);
void do_clan_set_attacco(struct char_data *ch,char *arg);
void do_clan_set_difesa(struct char_data *ch,char *arg);
void arruola_truppe(void);
void perform_shop_coins(struct char_data * ch, char *coins);
void fedina_penale(struct char_data * ch, struct char_data * vict,int regno);
void perform_assolvi(struct char_data * ch, char *arg1);
void perform_condanna(struct char_data * ch, char *arg1);
void regno_ricercati(struct char_data * ch, char *argument);
char *get_reward_flag_name(int flag,int modo);
int get_reward_flag_id(char * flag);

int find_pg(char *name,struct char_data *searched,struct char_file_u srchdata);
void do_clan_assumi(struct char_data *ch,char *argument);
void do_clan_mob(struct char_data *ch,char *argument);
void Save_mob(void);
void do_clan_secondino1 (struct char_data *ch, char *arg);
void do_clan_secondino2 (struct char_data *ch);

/*negozi*/
extern int top_shop;
extern struct shop_data *shop_index;
extern struct index_data *mob_index;
extern struct char_data *character_list;
extern struct char_data *mob_proto;
#define AUTOMATIC_CLASSES 7
#define FEE_PER_LEVEL 1000
#define NOPAYTIMEOUT 15

char *clan_privileges[]=
{
	"SETPLAN",
	"ARRUOLA.",
	"ESPELLI.",
	"PROMUOVI.",
	"DEGRADA.",
	"STIPENDI",
	"TASSE",
	"PRELEVA",
	"SET",
	"DIPLO",
	"BILANCIO",
	"ECONOMY",
	"DARE PRIV.",
	"TOGLIERE PRIV.",
	"ZECCA & EXPORT",
	"GIUDICE",
	"MNGSHOP",
	"SECONDINO",
	"\n"
};


const char *tipi_clan[]={
	"&1Regno&0",
	"&2Feudo&0",
	"&4Contea&0",
	"&5&bDucato&0",
	"&6Villaggio&0",
	"&1Citta&0",
	"&7&bTribu&0",
	"Clan",
	"&8Gilda&0"
};

const char *default_ranks[]={
	"Esiliato",
	"Popolano",
	"Cittadino",
	"Mercante",
	"Borghese",
	"Cortigiano",
	"Lord",
	"Fuorilegge",
	"Guardiano",
	"Soldato",
	"Guardia",
	"Sergente",
	"Cavaliere",
	"Sceriffo",
	"Consigliere",
	"Guardia Reale",
	"Comandante",
	"Nobile",
	"Baronetto",
	"Sindaco",
	"Generale",
	"Alto Consigliere",
	"Marchese",
	"Barone",
	"Conte",
	"Duca",
	"Consigliere del Re",
	"Principe",
	"Regnante",
	"\n"
};

const int default_tax[MAX_RANKS]=
{0,500,1000,2500,5000,12000,23000,0,1000,1000,2500,4000,6000,6000,9000,15000,
	18000,27000,30000,30000,30000,35000,35000,40000,45000,50000,60000,80000,0};


const int default_bounds[MAX_RANKS]=
{0,10000,20000,50000,100000,250000,450000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0};


#define ARTICOLO     0
#define PROPOSIZIONE 1


char *frase_regno(int id,int type){
	static char buffer[50];
	int tipo = clan[id].type;

	if (type==ARTICOLO){
		if(tipo==REGNO||tipo==FEUDO||tipo==DUCATO||tipo==VILLAGGIO||tipo==CLAN)
			sprintf(buffer,"il ");
		else
			sprintf(buffer,"la ");
	}

	if(type==PROPOSIZIONE){
		if(tipo==REGNO||tipo==FEUDO||tipo==DUCATO||tipo==VILLAGGIO||tipo==CLAN)
			sprintf(buffer,"del ");
		else
			sprintf(buffer,"della ");
	}

	strcat(buffer,tipi_clan[tipo]);
	if(tipo==REGNO||tipo==CONTEA||tipo==DUCATO||tipo==CITTA)
		strcat(buffer," di ");
	else
		strcat(buffer," ");
	
	if(id==0)
		strcat(buffer,"&3&b");
	else if(id==2)
		strcat(buffer,"&4&b");
	else if(id==3)
		strcat(buffer,"&5");
	else if(id==1)
		strcat(buffer,"&6&b");
	else if(id==5) //alorn
		strcat(buffer,"&1&8");
	
	strcat(buffer,clan[id].name);
	strcat(buffer,"&0");
	return buffer;
}


void send_clan_format(struct char_data *ch)
{
	int c,r;
	
	send_to_char("Comandi Regno che puoi usare:\n\r"
		"   Regno who  - vedi i membri del clan presenti.\r\n"
		"   Regno status\r\n"
		"   Regno info <clan> - info generali del clan\r\n",ch);
	if(GET_LEVEL(ch)>=LVL_CLAN_GOD)
		send_to_char(
		    "   Regno crea     <leader> <Nome Regno>\r\n"
			"   Regno assegna     <Regno> <vnum zona> \r\n"
			"   Regno negozi \r\n"   					
			"   Regno arruola     <player> <regno>\r\n"
			"   Regno espelli     <player> <regno>\r\n"
			"   Regno promuovi    <player> <regno>\r\n"
			"   Regno degrada     <player> <regno>\r\n"
			"   Regno preleva     <amount> <regno>\r\n"
			"   Regno deposita    <amount> <regno>\r\n"
			"   Regno set ranks   <rank>   <regno>\r\n"
			"   Regno privilegi   <privilegio> <num rango> <regno>\r\n"
			"   Regno set titoli  <nome Regno> <num rango> <titolo>\r\n"
			"   Regno set stato   <nome PG>    <num rango>\r\n"
			"   Regno set livapp  <liv>\r\n"
			"   Regno set tasse   <num rango>  <coins>  \r\n"
			"   Regno stipendi    <nome PG>    <coins>  \r\n"
			"   Regno diplo       <nome Regno> \r\n"
			"   Regno economia    <nome Regno> \r\n"
			"   Regno mob         <nome Regno> <idmob>\r\n"
			"   Regno condanna    <nome> <flag> <vivo/morto> <taglia> <regno> <reato>\r\n"
			"   Regno assolvi     <nome> <num> <regno>\r\n"
			"   Regno ricercati     \r\n"
			"   Regno libera      <nome>    \r\n"
			"   Regno aprite    \r\n"
			"   Regno set truppe  <numero truppe > <regno> \r\n"
			"   Regno set attacco <valore attacco> <regno> \r\n"
			"   regno set difesa  <valore difesa > <regno  \r\n"
			,ch);
	else {
		c=0;
		if (GET_CLAN(ch)>0)
		  c=find_clan_by_id(GET_CLAN(ch));
		r=GET_CLAN_RANK(ch);
		if(r<1)
			send_to_char("   Regno domanda      <Regno>\r\n",ch);
		if(c>0) {
			send_to_char("   Regno deposita   <coins>\r\n",ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_WITHDRAW))
				send_to_char("   Regno preleva    <coins>\r\n" ,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_ENROLL))
				send_to_char("   Regno arruola    <PG>\r\n" ,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_EXPEL))
				send_to_char("   Regno espelli    <PG>\r\n" ,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_PROMOTE))
				send_to_char("   Regno promuovi   <PG>\r\n",ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_DEMOTE))
				send_to_char("   Regno degrada    <PG>\r\n",ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_MNGSHOP))
				send_to_char("   Regno negozi               \r\n",ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_SECONDINO))
				send_to_char(
					"   Regno libera     <PG>\r\n"
					"   Regno aprite\r\n"
					,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_SET_RANK))
				send_to_char(
					"   Regno set ranghi  <num ranghi>\r\n"
					"   Regno set titoli  <num rango> <titolo>\r\n"
					,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_PRIV))
				send_to_char(
					"   Regno privilegi   <privilegio> <num rango>\r\n"
					,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_SET_TAX))
				send_to_char(
					"   Regno set stato   <nome Pg>  <num rango>\r\n"
					"   Regno set livapp  <liv>\r\n"
					"   Regno set tasse   <num rango> <coins>  \r\n"
					"   Regno limiti      <num rango> <nuovo limit>\r\n"
					,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_FEES))
				send_to_char(
					"   Regno stipendio  <nome Pg> <amount> \r\n"
					,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_DIPLO))
				send_to_char(
					"   Regno diplo       <nomeRegno> \r\n"
					,ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_ECONOMY_V)||IS_SET(GET_CLAN_PRIV(ch),CP_ECONOMY_AQ))
				send_to_char(
					"   Regno economia    \r\n",ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_ECONOMY_AQ)||IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_FEES))
				send_to_char(
					"   Regno assumi <idmob>    \r\n"
					"   Regno licenzia <idmob>  \r\n",ch);
			if(IS_SET(GET_CLAN_PRIV(ch),CP_ZECCA_EXP))
				send_to_char(
					"   conia <tot> <gold>    \r\n"
					"   trade <tot> <gems>  \r\n",ch);
			if (IS_SET(GET_CLAN_PRIV(ch),CP_GIUDICE)) {
				send_to_char("   Regno condanna <nome> <flag> <vivo/morto> <taglia> <reato>    \r\n",ch);
				send_to_char("   Regno assolvi  <nome> <num>     \r\n",ch);			    
			}
			
			send_to_char("   Regno ricercati     \r\n",ch);  
			
		}
	}
}

void do_clan_create (struct char_data *ch, char *arg)
{
	struct char_data *leader = NULL;
	char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
	int new_id=0,i;
	
	if (!*arg) {
		send_clan_format(ch);
		return;
	}
	
	if (GET_LEVEL(ch) < LVL_CLAN_GOD) {
		send_to_char("Non sei abbastanza Immortale per  creare nuovi Regni!\r\n", ch);
		return;
	}

        /* COMANDO DISABILITATO!!!! */
	send_to_char("La creazione di nuovi regni tramite questo comando e' disabilitata\r\n"
                     "a causa di possibili incasinamenti del file dei regni. Per creare nuovi regni\r\n"
                     "bisogna agire direttamente sul file binario dei clan\r\n", ch);
 	return;

	
	if(num_of_clans == MAX_CLANS) {
		send_to_char("Raggiunto il Massimo dei Regni. WOW!\r\n",ch);
		return;
	}
	
	half_chop(arg, arg1, arg2);
	
	if(!(leader=get_char_vis(ch,arg1))) {
		send_to_char("Il Leader del nuovo Regno deve essere presente.\r\n",ch);
		return;
	}
	
	if(strlen(arg2)>=32) {
		send_to_char("Nome del Regno troppo lungo! (al massimo 32 caratteri)\r\n",ch);
		return;
	}
	
	if(GET_LEVEL(leader)>=LVL_IMMORT) {
		send_to_char("Non puoi mettere un Immortale a capo di un Regno.\r\n",ch);
		return;
	}
	
	if(GET_CLAN(leader)>0 && GET_CLAN_RANK(leader)>0) {
		send_to_char("Il Leader gia appartiene ad un Regno!\r\n",ch);
		return;
	}
	
	if(find_clan(arg2)!=-1) {
		send_to_char("Questo Regno gia esiste!\r\n",ch);
		return;
	}
	
	strncpy(clan[num_of_clans].name, CAP((char *)arg2), 32);
	strcpy(clan[num_of_clans].leader,GET_NAME(leader));

	for(i=0;i<num_of_clans;i++)
		if(new_id<clan[i].id)
			new_id=clan[i].id;

	clan[num_of_clans].type=REGNO;
	clan[num_of_clans].id=new_id+1;
	clan[num_of_clans].regno=clan[num_of_clans].id;
	clan[num_of_clans].ranks =DEFAULT_RANKS;
	for(i=0;i<DEFAULT_RANKS;i++){
		strcpy(clan[num_of_clans].rank_name[i],default_ranks[i]);
		clan[num_of_clans].rank_tax[i]=default_tax[i];
	}
	clan[num_of_clans].automatic_ranks=AUTOMATIC_CLASSES;
	for(i=0;i<DEFAULT_RANKS;i++)
		clan[num_of_clans].rank_bounds[i]=default_bounds[i];
	
	clan[num_of_clans].economy.treasure = 0 ;
//	clan[num_of_clans].members = 1 ;
//	clan[num_of_clans].power = GET_LEVEL(leader) ;
	clan[num_of_clans].truppe_disponibili = 11 ;
	clan[num_of_clans].valore_attacco = 1 ;
	clan[num_of_clans].valore_difesa = 1;
	for(i=0;i< NUM_CP;i++)
		SET_BIT(GET_CLAN_PRIV(leader),( 1 << i));
	
	
	for(i=0;i<num_of_clans;i++){
		strcpy(clan[i].diplomacy[num_of_clans].name, clan[num_of_clans].name);
		clan[i].diplomacy[num_of_clans].proposta = -1;
		clan[i].diplomacy[num_of_clans].timer = -1;
		clan[num_of_clans].diplomacy[i].id=clan[i].id;
		strcpy(clan[num_of_clans].diplomacy[i].name, clan[i].name);
		clan[num_of_clans].diplomacy[i].status=PEACE;
		clan[num_of_clans].diplomacy[i].proposta=-1;
		clan[num_of_clans].diplomacy[i].timer=-1;
	}
	//reset_history(num_of_clans);
	
	num_of_clans++;
	
	save_clans();
	send_to_char("Regno creato.\r\n", ch);
	GET_CLAN(leader)=clan[num_of_clans-1].id;
	GET_CLAN_RANK(leader)=clan[num_of_clans-1].ranks;
	save_char(leader, leader->in_room);
	sprintf(buf,"(REGNI) %s ha creato il regno %s",GET_NAME(ch),arg2);
	mudlog(buf,NRM,LVL_GOD,TRUE);
	return;
}
/*
void do_clan_destroy (struct char_data *ch, char *arg)
{
	
	int i,j,k, shop_nr, zone;
	
	struct char_file_u chdata;
	struct char_data *victim=NULL;
	
	if (!*arg) {
		send_clan_format(ch);
		return; }
	
	if ((i = find_clan(arg)) < 0) {
		send_to_char("Clan Sconosciuto.\r\n", ch);
		return;
	}
	
	if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
		send_to_char("Non sei abbastanza influente per distruggere un Clan!\r\n", ch);
		return;
	}
	
  //Lance Negozi
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		if (SHOP_CLAN(shop_nr)==clan[i].id)
		{
			SHOP_CLAN(shop_nr)= -1;
			zone = real_zone(SHOP_ROOM(shop_nr,0));
			if (zone!=-1)
				sedit_save_to_disk(zone);
		}
	}
	
	for (j = 0; j <= top_of_p_table; j++){
		if((victim=is_playing((player_table +j)->name))) {
			if(GET_CLAN(victim)==clan[i].id) {
				GET_CLAN(victim)=0;
				GET_CLAN_RANK(victim)=0;
				save_char(victim, victim->in_room);
			}
		}
		else {
			load_char((player_table + j)->name, &chdata);
			if(chdata.player_specials_saved.clan==clan[i].id) {
				chdata.player_specials_saved.clan=0;
				chdata.player_specials_saved.clan_rank=0;
				save_char_file_u(chdata);
			}
		}
	}
	for(j=0;j<num_of_clans;j++){
		clan[j].diplomacy[i].timer=-1;
		clan[j].diplomacy[i].status=-1;
		clan[j].diplomacy[i].proposta=-1;
	}
	//memset(&clan[i], sizeof(struct clan_rec), 0);
	
	for (j = i; j < num_of_clans - 1; j++)
		clan[j] = clan[j + 1];
	for(j=0 ;j<num_of_clans -1;j++){
		sprintf(buf," clan %d\n",j);
		send_to_char(buf,ch);
		for(k=i;k<num_of_clans-1;k++){
			sprintf(buf," doing %d\n",k);
			send_to_char(buf,ch);
			clan[j].diplomacy[k] = clan[j].diplomacy[k+1];
		}
	}
	num_of_clans--;
	
	send_to_char("Regno eliminato.\r\n", ch);
	save_clans();
	sprintf(buf,"(REGNI) %s ha distrutto il regno %s",GET_NAME(ch),arg);
	mudlog(buf,NRM,LVL_GOD,TRUE);
	return;
}
*/
void do_clan_enroll (struct char_data *ch, char *arg){
	struct char_data *vict=NULL;
	int clan_num,immcom=0;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	
	if (!(*arg)) {
		send_clan_format(ch);
		return;
	}

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	  
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun regno!\r\n",ch);
			return;
		}
		strcpy(arg2, clan[clan_num].name);
	}
	else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return;
		}
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Regno sconosciuto.\r\n", ch);
			return;
		}
	}
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_ENROLL) && !immcom) {
		send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	if(!(vict=get_char_room_vis(ch,arg))) {
		send_to_char("La persona deve essere di fronte a te!\r\n",ch);
		return;
	}
	else {
		if(GET_CLAN(vict)!=clan[clan_num].id) {
			if(GET_CLAN_RANK(vict)>0) {
				send_to_char("Appartiene gia ad un altro Clan.\r\n",ch);
				return;
			}
			else {
				send_to_char("Non ha chiesto di entrare nel tuo clan.\r\n",ch);
				return;
			}
		}
		else
		if(GET_CLAN_RANK(vict)>0) {
			send_to_char("E' gia nel tuo clan.\r\n",ch);
			return;
		}
		if(GET_LEVEL(vict)>=LVL_IMMORT) {
			send_to_char("Non puoi arruolare Immortali nel tuo clan.\r\n",ch);
			return;
		}
		if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict)){
			send_to_char("Non puoi arruolare un fantasma nel tuo clan.\r\n",ch);
			return;
		}
		if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict)){
			send_to_char("Non sembra stia giocando al tuo stesso gioco\r\n",ch);
			return;
		}
	}
	
	GET_CLAN_RANK(vict)++;
	save_char(vict, vict->in_room);
//	clan[clan_num].power += GET_LEVEL(vict);
//	clan[clan_num].members++;
//	clan[clan_num].fama += GET_FAMA(vict);
	send_to_char("Entri a far parte da ora del clan che hai scelto!\r\n",vict);
	send_to_char("Fatto.\r\n",ch);
	sprintf(buf,"(REGNI) %s ha arruolato %s nel regno %s",GET_NAME(ch),GET_NAME(vict),arg2);
	mudlog(buf,NRM,LVL_GOD,TRUE);
	return;
}

void do_clan_expel (struct char_data *ch, char *arg)
{
	struct char_data *vict=NULL;
	struct char_file_u chdata;
	int clan_num,immcom=0,found,j;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	
	if (!(*arg)) {
		send_clan_format(ch);
		return;
	}

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun clan!\r\n",ch);
			return;
		}
	}
	else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return;
		}
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Clan Sconosciuto.\r\n", ch);
			return;
		}
	}
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_EXPEL) && !immcom) {
		send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
		if((vict=is_playing((player_table +j)->name))) {
			if(str_cmp(arg,GET_NAME(vict))==0)
				found=1;
		}
		else{
			load_char((player_table + j)->name, &chdata);
			if(str_cmp(arg,chdata.name)==0)
				found=2;
		}
	}
	switch(found){
		case 0:
			sprintf(buf,"Non esiste nessun %s.",arg1);
			send_to_char(buf,ch);
			return;
			break;
		case 1:
			if(GET_CLAN(vict)!=clan[clan_num].id) {
				send_to_char("Non appartiene al tuo Clan.\r\n",ch);
				return;
			}
			else {
				if(GET_CLAN_RANK(vict)>=GET_CLAN_RANK(ch) && !immcom) {
					send_to_char("Non puoi buttar fuori questa persona .\r\n",ch);
					return;
				}
			}
			GET_CLAN(vict)=0;
			GET_CLAN_RANK(vict)=0;
			save_char(vict, vict->in_room);
//			clan[clan_num].members--;
//			clan[clan_num].power-=GET_LEVEL(vict);
//			clan[clan_num].fama -=GET_FAMA(vict);
			send_to_char("Sei stato cacciato dal tuo clan!\r\n",vict);
			send_to_char("Done.\r\n",ch);
			sprintf(buf,"(REGNI) %s ha espulso %s dal regno %s",GET_NAME(ch),GET_NAME(vict),clan[clan_num].name);
			mudlog(buf,NRM,LVL_GOD,TRUE);
			return;
			break;
		case 2:
			if(chdata.player_specials_saved.clan!=clan[clan_num].id){
				send_to_char("Non appartiene al tuo Clan.\r\n",ch);
				return;
			}
			else {
				if(chdata.player_specials_saved.clan_rank>=GET_CLAN_RANK(ch) && !immcom)
				{
					send_to_char("Non puoi buttar fuori questa persona .\r\n",ch);
					return;
				}
				chdata.player_specials_saved.clan=0;
				chdata.player_specials_saved.clan_rank=0;
				save_char_file_u(chdata);
//				clan[clan_num].members--;
				send_to_char("Done.\r\n",ch);
				sprintf(buf,"(REGNI) %s ha espulso %s dal regno %s",GET_NAME(ch),chdata.name,clan[clan_num].name);
				mudlog(buf,NRM,LVL_GOD,TRUE);
				return;
			}
			break;
	}
}

void do_clan_demote (struct char_data *ch, char *arg)
{
	struct char_data *vict=NULL;
	int clan_num,immcom=0;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	if (!(*arg)) {
		send_clan_format(ch);
		return;
	}

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun clan!\r\n",ch);
			return;
		}
	}
	else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return;
		}
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Clan Sconosciuto.\r\n", ch);
			return;
		}
	}
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_DEMOTE) && !immcom) {
		send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	if(!(vict=get_char_room_vis(ch,arg))) {
		send_to_char("Scusa, CHI ??\r\n",ch);
		return;
	}
	else {
		if(GET_CLAN(vict)!=clan[clan_num].id) {
			send_to_char("Non appartiene al tuo clan.\r\n",ch);
			return;
		}
		else {
			if(GET_CLAN_RANK(vict)==1) {
				send_to_char("Non puo essere degradato ulteriormente, usa espelli semmai.\r\n",ch);
				return;
			}
			if(GET_CLAN_RANK(vict)>=GET_CLAN_RANK(ch) && !immcom) {
				send_to_char("Non puoi degradare una persona di Rank superiore al tuo!\r\n",ch);
				return;
			}
		}
	}
	
	GET_CLAN_RANK(vict)--;
	save_char(vict, vict->in_room);
	send_to_char("Sei appena stato Degradato!\r\n",vict);
	send_to_char("Fatto.\r\n",ch);
	sprintf(buf,"(REGNI) %s ha degradato %s nel regno %s",GET_NAME(ch),GET_NAME(vict),clan[clan_num].name);
	mudlog(buf,NRM,LVL_GOD,TRUE);
	return;
}

void do_clan_promote (struct char_data *ch, char *arg)
{
	struct char_data *vict=NULL;
	int clan_num,immcom=0;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	
	if (!(*arg)) {
		send_clan_format(ch);
		return;
	}

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun Clan!\r\n",ch);
			return;
		}
	}
	else {
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Clan Sconosciuto.\r\n", ch);
			return;
		}
	}
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_PROMOTE) && !immcom) {
		send_to_char("Non sei abbastanza influente all interno del tuo Clan per farlo\r\n",ch);
		return;
	}
	
	if(!(vict=get_char_room_vis(ch,arg))) {
		send_to_char("Er, Chi??\r\n",ch);
		return;
	}
	else {
		if(GET_CLAN(vict)!=clan[clan_num].id) {
			send_to_char("Non appartiene al tuo clan.\r\n",ch);
			return;
		}
		else {
			if(GET_CLAN_RANK(vict)==0) {
				send_to_char("Non e' stato ancora arruolato.\r\n",ch);
				return;
			}
			if((GET_CLAN_RANK(vict)+1)>GET_CLAN_RANK(ch) && !immcom) {
				send_to_char("Non puoi promuovere questa persona oltre il tuo stesso Rank!\r\n",ch);
				return;
			}
			if(GET_CLAN_RANK(vict)==clan[clan_num].ranks) {
				send_to_char("Non puoi promuovere oltre il massimo Rank!\r\n",ch);
				return;
			}
		}
	}
	if((GET_CLAN_RANK(ch)==clan[clan_num].ranks)&&(GET_CLAN_RANK(vict)==clan[clan_num].ranks-1)) {
		send_to_char("&1&bDeve esserci un solo capo ,cioe tu!\r\n&0",ch);
		return;
	}
	GET_CLAN_RANK(vict)++;
	save_char(vict, vict->in_room);
	send_to_char("&1&bSei stato promosso all interno del tuo Clan!\r\n&0",vict);
	send_to_char("Fatto.\r\n",ch);
	sprintf(buf,"(REGNI) %s ha promosso %s nel regno %s",GET_NAME(ch),GET_NAME(vict),clan[clan_num].name);
	mudlog(buf,NRM,LVL_GOD,TRUE);
	return;
}

void do_clan_who (struct char_data *ch)
{
	extern struct player_index_element *player_table;
	extern int top_of_p_table;
	struct char_data *victim;
	//  struct char_file_u chdata;
	char line_disp[90];
	char buff[1000];
	char buff2[1000];
	char *present;
	int j,rank=0,id ;
	int tempclan,tempclanrank;
	//PAGINAZIONE
	char *s1=NULL;
	
	bzero(buff,sizeof(buff));
	bzero(buff2,sizeof(buff2));
	
	s1=string(s1,buff,FALSE);
	
	if(GET_CLAN_RANK(ch)==0 || GET_CLAN(ch)<=0 ) {
		send_to_char("Non appartieni a nessun clan!\r\n",ch);
		return;
	}
	id=find_clan_by_id(GET_CLAN(ch));
	send_to_char("\r\n         SUDDITI DEL REGNO\r\n",ch);
	send_to_char("&1---------------------------------------------&0\r\n",ch);

	for (j = 0; j <= top_of_p_table; j++){
		if((victim=is_playing((player_table +j)->name))) {
			if (victim && AFF_FLAGGED(victim,AFF_DISGUISE) && (victim->player.namedisguise))
			{
				tempclanrank = victim->player.clanrankdisguise;
				tempclan = victim->player.clandisguise;
			} else {
				tempclanrank = GET_CLAN_RANK(victim);
				tempclan = GET_CLAN(victim);  
			}	    
			present ="* ";
			if(tempclan==GET_CLAN(ch)&&tempclanrank>0)  {
				rank=tempclanrank;
				sprintf(line_disp,"&1%s&0%-15s %s\r\n",present,PERS(victim,ch),clan[id].rank_name[rank-1]);
				//send_to_char(line_disp,ch);
				s1=string(s1,line_disp,TRUE);
			}
		}
	/* else {
		present="X";
		load_char((player_table + j)->name, &chdata);
		if(chdata.player_specials_saved.clan==GET_CLAN(ch)) {
		  sprintf(line_disp,"&4%s&0%-15s %s\r\n",present,chdata.name,clan[id].rank_name[chdata.player_specials_saved.clan_rank-1]);
	strcat(buff2,line_disp);
	}
	}*/
	}
	page_string(ch->desc,s1,1);
	free(s1);
	
	send_to_char("&1---------------------------------------------&0\r\n",ch);
  /*  send_to_char("\r\n         NON COLLEGATI\r\n",ch);
  send_to_char("&1---------------------------------------------&0\r\n",ch);
  page_string(ch->desc,buff2,1);
  send_to_char("&1---------------------------------------------&0\r\n",ch);*/
	return;
}

void do_clan_status (struct char_data *ch,char *argument)
{
	char line_disp[90];
	int clan_num;
	
	if(GET_LEVEL(ch)>=LVL_IMMORT) {
		send_to_char("Tu sei un Immortale e non puoi appartenere a nessun Clan!\r\n",ch);
		return;
	}

    if(GET_CLAN(ch)<=0){
		send_to_char("Non appartieni a nessun Clan!\r\n",ch);
		return;    	
    }
	
	clan_num=find_clan_by_id(GET_CLAN(ch));
	if(!*argument){
		if(GET_CLAN_RANK(ch)==0){
			if(clan_num>=0) {
				sprintf(line_disp,"Hai fatto domanda per entrare a far parte del clan %s\r\n",clan[clan_num].name);
				send_to_char(line_disp,ch);
				return;
			}
			else {
				send_to_char("Non appartieni ad un clan!\r\n",ch);
				return;
			}
		}	
		sprintf(line_disp,"Sei %s (Rank %d) of %s (ID %d)\r\n",
			clan[clan_num].rank_name[GET_CLAN_RANK(ch)-1],GET_CLAN_RANK(ch),
			clan[clan_num].name,clan[clan_num].id);
		send_to_char(line_disp,ch);
		if(GET_CLAN_PRIV(ch)>0){
			send_to_char("&1Godi dei seguenti privilegi&0:\n",ch);
			sprintbit(GET_CLAN_PRIV(ch),clan_privileges,buf);
			send_to_char(buf,ch);
		}
		return;
	}
	if((clan_num=find_clan(argument))<0){
		send_to_char("Regno Sconosciuto.",ch);
		return;
	}
	show_status_regno(ch);
}

void do_clan_apply (struct char_data *ch, char *arg){
	int clan_num;
	
	if (!(*arg)) {
		send_clan_format(ch);
		return; }
	
	if(GET_LEVEL(ch)>=LVL_IMMORT) {
		send_to_char("Gli Dei non possono entrare in nessun Clan.\r\n",ch);
		return; }
	
	if(GET_CLAN_RANK(ch)>0) {
		send_to_char("Tu gia appartieni ad un clan!\r\n",ch);
		return; }
	else {
		if ((clan_num = find_clan(arg)) < 0) {
			send_to_char("Clan Sconosciuto.\r\n", ch);
			return; } }
	
	GET_GOLD(ch) -= 1000;
	clan[clan_num].economy.treasure += 1000;
	save_clans();
	GET_CLAN(ch)=clan[clan_num].id;
	save_char(ch, ch->in_room);
	send_to_char("Hai fatto domanda per entrare a far parte di un clan!\r\n",ch);
	sprintf(buf,"(REGNI) %s ha fatto richiesta per entrare nel regno %s",GET_NAME(ch),arg);
	mudlog(buf,NRM,LVL_GOD,TRUE);
	return;
}

void do_clan_info (struct char_data *ch, char *arg)
{
  int i=0,j;
	
  if (num_of_clans == 0) {
	send_to_char("Non ci sono ancora Clan formati.\r\n",ch);
	return;
  }
	
  if (!(*arg)) {
	sprintf(buf, "\r\n");
		
	for (i=0; i < num_of_clans; i++) {
	  sprintf(buf, "%s[%-3d][%s][ID%d] %-10s Regnante %s Armate :%3d \r\n",
	               buf,
	               i,
	               tipi_clan[clan[i].type],
	               clan[i].id,
	               clan[i].name,
	               clan[i].leader,
	               clan[i].truppe_disponibili/10 );
    }
	
	page_string(ch->desc, buf, 1);
	return;
  }
  else {
	if ((i = find_clan(arg)) < 0) {
	  send_to_char("Clan Sconosciuto.\r\n", ch);
	  return;
	}
  }
	
  sprintf(buf, "%s (ID:%d)>:\r\n",frase_regno(i,0),clan[i].id);
  send_to_char(buf, ch);
	
  for (j=0; j<clan[i].ranks; j++) {
	sprintf(buf, "%s ", clan[i].rank_name[j]);
	send_to_char(buf, ch);}
	sprintf(buf, "\r\nArmate disponibili    : %d\r\nPotenza di Attacco      : %d\t\nPotenza di Difesa       : %d\t\nTreasure   : %d\r\nRanghi: %d\r", 
	             clan[i].truppe_disponibili/10, 
	             clan[i].valore_attacco,
	             clan[i].valore_difesa,
		         clan[i].economy.treasure,
		         clan[i].ranks );	
	sprintf(buf, "%s\r\n", buf);
	send_to_char(buf, ch);
	
	sprintf(buf,"&1&bRegnante&0: %s\n", clan[i].leader);
	send_to_char(buf,ch);

	if (clan[i].truppe_disponibili%2!=0)
	  send_to_char("Puoi ancora fare la tua azione-esercito questo round\r\n",ch);
	return;
}

sh_int find_clan_by_id(int idnum)
{
  int i;
//  char bufo[128];
	
  for ( i=0; i < num_of_clans; i++)
	if (idnum==clan[i].id)
	  return i;
			
//  sprintf(bufo,"clan.c find_clan_by_id: Regno n° %d non trovato. %d Clan attivi.", idnum, num_of_clans);
//  mudlog(bufo,NRM,LVL_GOD,TRUE);

  return -1;
}

sh_int find_clan(char *name)
{
  int i;
//  char bufo[128];

  for ( i=0; i < num_of_clans; i++)
	if (strcmp(CAP(name), CAP(clan[i].name))==0)
	  return i;

//  sprintf(bufo,"clan.c find_clan: Regno %s non trovato. %d Clan attivi.", name, num_of_clans);
//  mudlog(bufo,NRM,LVL_GOD,TRUE);

  return -1;
}

void save_clans()
{

	FILE *fl;
	
	if (!(fl = fopen(CLAN_FILE, "wb"))) {
		log("SYSERR: Unable to open clan file");
		return;
	}
	
	fwrite(&num_of_clans, sizeof(int), 1, fl);
	fwrite(clan, sizeof(struct clan_rec), num_of_clans, fl);
	fclose(fl);
	return;
}


void init_clans()
{
  FILE *fl;
  int i;
  	
  memset(clan, 0, sizeof(struct clan_rec)*MAX_CLANS);
  num_of_clans=0;
  i=0;
	
  if (!(fl = fopen(CLAN_FILE, "rb"))) {
	log("   Clan file does not exist. Will create a new one");
	save_clans();
	return;
  }
	
  fread(&num_of_clans, sizeof(int), 1, fl);
  fread(clan, sizeof(struct clan_rec), num_of_clans, fl);
  fclose(fl);

  return;
}

void do_clan_bank(struct char_data *ch, char *arg, int action)
{
	int clan_num,immcom=0;
	long amount=0;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	
	if (!(*arg)) {
		send_clan_format(ch);
		return; }

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun clan!\r\n",ch);
			return;
		}
	}
	else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return;
		}
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Clan sconosciuto.\r\n", ch);
			return;
		}
	}
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_WITHDRAW) && !immcom && action==CB_WITHDRAW) {
		send_to_char("Non se abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	if(!(*arg)) {
		send_to_char("Depositi quanto?\r\n",ch);
		return;
	}
	
	if(!is_number(arg)) {
		send_to_char("Depositi cosa???\r\n",ch);
		return;
	}
	
	amount=atoi(arg);
	
	if(!immcom && action==CB_DEPOSIT && GET_GOLD(ch)<amount) {
		send_to_char("Non hai tutti quei soldi!!\r\n",ch);
		return;
	}
	
	if(action==CB_WITHDRAW && clan[clan_num].economy.treasure<amount) {
		send_to_char("Il clan non e' cosi ricco!\r\n",ch);
		return;
	}
	
	switch(action) {
		case CB_WITHDRAW:
			GET_GOLD(ch)+=amount;
			clan[clan_num].economy.treasure-=amount;
			send_to_char("Prelevi dal tesoro del clan.\r\n",ch);
			sprintf(buf, "(REGNI) %s preleva %ld coins nel regno di %s.", GET_NAME(ch), amount  ,clan[clan_num].name );
			mudlog(buf, BRF, LVL_GOD, TRUE);
			break;
		case CB_DEPOSIT:
			if(!immcom) GET_GOLD(ch)-=amount;
			clan[clan_num].economy.treasure+=amount;
			send_to_char("Aumenti il tesoro del clan ,bravo ,fallo piu spesso!.\r\n",ch);
			sprintf(buf, "(REGNI) %s deposita %ld coins nel regno di %s.", GET_NAME(ch), amount  ,clan[clan_num].name );
			mudlog(buf, BRF, LVL_GOD, TRUE);
			break;
		default:
			send_to_char("Problem in command, please report.\r\n",ch);
			break;
	}  
	save_char(ch, ch->in_room);
	save_clans();
	return;
}

void do_clan_money(struct char_data *ch, char *arg, int action)
{
	int clan_num,immcom=0;
	long amount=0;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	
	if (!(*arg)) {
		send_clan_format(ch);
		return; }

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun clan!\r\n",ch);
			return;
		}
	}
	else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return; }
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Clan sconosciuto.\r\n", ch);
			return;
		}
	}
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_FEES) && !immcom) {
		send_to_char("Non se abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	if(!(*arg)) {
		send_to_char("Lo imposti a quanto??\r\n",ch);
		return;
	}
	
	if(!is_number(arg)) {
		send_to_char("Lo imposti a quanto?\r\n",ch);
		return;
	}
	
	amount=atoi(arg);
/*
switch(action) {
  case CM_APPFEE:
	clan[clan_num].app_fee=amount;
	send_to_char("Cambi la quota d'iscrizione ad clan.\r\n",ch);
	break;
  case CM_DUES:
	clan[clan_num].dues=amount;
	send_to_char("You change the monthly dues.\r\n",ch);
	break;
  default:
	send_to_char("Problem in command, please report.\r\n",ch);
	break;
  }

save_clans();*/
	return;
}

void do_clan_ranks(struct char_data *ch, char *arg)
{
	int i,j;
	int clan_num,immcom=0;
	int new_ranks;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	extern int top_of_p_table;
	extern struct player_index_element *player_table;
	struct char_file_u chdata;
	struct char_data *victim=NULL;
	
	if (!(*arg)) {
		send_clan_format(ch);
		return; }

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun clan!\r\n",ch);
			return;
		}
	}
	else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai l'autorita' per fare questo.\r\n", ch);
			return; }
		immcom=1;
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg1);
		if ((clan_num = find_clan(arg2)) < 0) {
			send_to_char("Clan sconosciuto.\r\n", ch);
			return;
		}
	}
	
	if(!(*arg)) {
		send_to_char("Quanti ranks?\r\n",ch);
		return;
	}
	
	if(!is_number(arg)) {
		send_to_char("Imposti i ranks a cosa?\r\n",ch);
		return;
	}
	
	new_ranks=atoi(arg);
	
	if(new_ranks==clan[clan_num].ranks) {
		send_to_char("Il clan ha gia' questo numero di ranks.\r\n",ch);
		return;
	}
	
	if(new_ranks<2 || new_ranks>MAX_RANKS) {
		sprintf(buf,"Il Clan puo' avere da 2 a %d ranks.\r\n",MAX_RANKS);
		send_to_char(buf,ch);
		return;
	}
	
	for (j = 0; j <= top_of_p_table; j++) {
		if((victim=is_playing((player_table +j)->name))) {
			if(GET_CLAN(victim)==clan[clan_num].id) {
				if(GET_CLAN_RANK(victim)<clan[clan_num].ranks && GET_CLAN_RANK(victim)>0)
					GET_CLAN_RANK(victim)=1;
				if(GET_CLAN_RANK(victim)==clan[clan_num].ranks)
					GET_CLAN_RANK(victim)=new_ranks;
				save_char(victim, victim->in_room);
			}
		}
		else {
			load_char((player_table + j)->name, &chdata);
			if(chdata.player_specials_saved.clan==clan[clan_num].id) {
				if(chdata.player_specials_saved.clan_rank<clan[clan_num].ranks && chdata.player_specials_saved.clan_rank>0)
					chdata.player_specials_saved.clan_rank=1;
				if(chdata.player_specials_saved.clan_rank==clan[clan_num].ranks)
					chdata.player_specials_saved.clan_rank=new_ranks;
				save_char_file_u(chdata);
			}
		}
	}
	
	clan[clan_num].ranks=new_ranks;
	for(i=0;i<clan[clan_num].ranks-1;i++)
		strcpy(clan[clan_num].rank_name[i],"Membro");
	strcpy(clan[clan_num].rank_name[clan[clan_num].ranks -1],"Leader");
	
	save_clans();
	return;
}

void do_clan_titles( struct char_data *ch, char *arg)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	int clan_num=0,rank;
	
	if (!(*arg)) {
		send_clan_format(ch);
		return; }

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun clan!\r\n",ch);
			return;
		}
	} else {
		if(GET_LEVEL(ch)<LVL_CLAN_GOD) {
			send_to_char("Non hai l'autorita' per fare questo.\r\n", ch);
			return; }
		half_chop(arg,arg1,arg2);
		strcpy(arg,arg2);
		
		if(!is_number(arg1)) {
			send_to_char("Devi specificare il numero del clan (fai Regno info).\r\n",ch);
			return;
		}
		
		if((clan_num=atoi(arg1))<0 || clan_num>=num_of_clans) {
			send_to_char("Non esiste nessun clan con quel numero.\r\n",ch);
			return;
		}
	}
	
	half_chop(arg,arg1,arg2);
	
	if(!is_number(arg1)) {
		send_to_char("Devi spacificare il numero del rango.\r\n",ch);
		return; }
	
	rank=atoi(arg1);
	
	if(rank<1 || rank>clan[clan_num].ranks) {
		send_to_char("Il clan non ha cosi tanti ranghi.\r\n",ch);
		return; }
	
	if(strlen(arg2)<1 || strlen(arg2)>20) {
		send_to_char("Puoi definire un nome di massimo 20 caratteri.\r\n",ch);
		return; }
	
	strcpy(clan[clan_num].rank_name[rank-1],arg2);
	save_clans();
	send_to_char("Fatto.\r\n",ch);
	return;
}


void display_pg_priv(struct char_data *ch,struct char_data *suddito,struct char_file_u chdata,int mode) {
	char name[128];
	char arg1[50];
	int i,privilegi=0;
	switch(mode){
		case 0:
			sprintf(buf,"PRIVILEGI DEL REGNO\n");
			for(i=0;i<NUM_CP;i++)
				sprintf(arg1,"%2d] %s\r\n",i,clan_privileges[i]);
			strcat(buf,arg1);
			send_to_char(buf,ch);
			return;
			break;
		case 1:
			strcpy(name,GET_NAME(suddito));
			privilegi=GET_CLAN_PRIV(suddito);
			break;
		case 2:
			strcpy(name,chdata.name);
			privilegi=GET_CLAN_PRIV2(chdata);
			break;
		default:
			return;
	}
	strcat(name," :");
	for(i=0;i<NUM_CP;i++)
		if(IS_SET(privilegi,1<<i)){
			strcat(name,clan_privileges[i]);
			strcat(name,"\r\n");
		}
	send_to_char(name,ch);
}

void display_privilegi(struct char_data *ch){
	int i;
	send_to_char("\n&1&bCLAN PRIVILEGES&0:\n",ch);
	for(i=0;i<NUM_CP;i++){
		sprintf(buf,"&4&b[&0%2d&4&b]&0 &2&b%s&0\n",i+1,clan_privileges[i]);
		send_to_char(buf,ch);
	}
}

void do_clan_privilegi(struct char_data *ch,char *arg){
	char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
	int j,cp=0,cl=0,rank=0,found=0,priv=0;
	bool imcom=FALSE;
	bool change=TRUE;
	struct char_file_u chdata;
	struct char_data *suddito=NULL;
	if(GET_LEVEL(ch)>=LVL_GOD)
		imcom=TRUE;
	if(!*arg){
		display_privilegi(ch);
		return;
	}
	two_arguments(arg,arg1,arg2);
	if(!*arg1){
		display_privilegi(ch);
		return;
	}
	if(!*arg2)
		change=FALSE;
	else{
		cp=atoi(arg2);
		//PEPPE PRIVILEGGI
		if(!is_number(arg2)||(cp<=0)||cp>NUM_CP){
			sprintf(buf, "Il secondo argomento deve essere un numero compreso tra 1 e %d", NUM_CP);
			send_to_char(buf, ch);
			return;
		}
	}
	//sprintf(buf,"STR:%s--CP:%d\n",arg2,cp);
	//send_to_char(buf,ch);
	
	if (!imcom && !IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_PRIV)) 
	{
		send_to_char("Non sei cosi' influente da poterlo fare!",ch); 
		return;
		
	}
	
	
	for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
		//cerco tra i pg giocanti
		if((suddito=is_playing((player_table +j)->name))) {
			
			if(str_cmp(arg1,GET_NAME(suddito))==0)
				found=1;
		}
		//cerco tra i pg non connessi
		else{
			load_char((player_table + j)->name, &chdata);
			if(str_cmp(arg1,chdata.name)==0)
				found=2;
		}
	}
	
	switch(found){
		case 0:
			sprintf(buf,"Non esiste nessun %s.",arg1);
			send_to_char(buf,ch);
			return;
			break;
		case 1:
			cl=GET_CLAN(suddito);
			rank=GET_CLAN_RANK(suddito);
			priv=GET_CLAN_PRIV(suddito);
			break;
		case 2:
			cl=GET_CLAN2(chdata);
			rank=GET_CLAN_RANK2(chdata);
			priv=GET_CLAN_PRIV2(chdata);
			break;
	}
	
	if((cl!=GET_CLAN(ch)||rank>GET_CLAN_RANK(ch))&&!imcom)
		send_to_char("NON puoi!",ch);
	else{
		if(change){
			send_to_char("Okay.\n",ch);
			if(found==1){
				TOGGLE_BIT(GET_CLAN_PRIV(suddito),1 << (cp-1));
				priv=GET_CLAN_PRIV(suddito);
				if (IS_SET(GET_CLAN_PRIV(suddito),1 << (cp-1)))
					sprintf(buf,"(REGNI) %s ha dato a %s il privilegio %s",GET_NAME(ch),GET_NAME(suddito),clan_privileges[cp-1]);
				else
					sprintf(buf,"(REGNI) %s ha tolto a %s il privilegio %s",GET_NAME(ch),GET_NAME(suddito),clan_privileges[cp-1]);					
				mudlog(buf,NRM,LVL_GOD,TRUE);
			}
			if(found==2){
				TOGGLE_BIT(GET_CLAN_PRIV2(chdata),1 << (cp-1));
				priv=GET_CLAN_PRIV2(chdata);
				if (IS_SET(GET_CLAN_PRIV2(chdata),1 << (cp-1)))
					sprintf(buf,"(REGNI) %s ha dato a %s il privilegio %s",GET_NAME(ch),chdata.name,clan_privileges[cp-1]);
				else
					sprintf(buf,"(REGNI) %s ha tolto a %s il privilegio %s",GET_NAME(ch),chdata.name,clan_privileges[cp-1]);
				mudlog(buf,NRM,LVL_GOD,TRUE);
			}
		}
		
		sprintbit(priv,clan_privileges,buf1);
		if(found==1){
			sprintf(buf,"&4&b[&0%s&4&b]&0:&1&b%s&0",GET_NAME(suddito),buf1);
			save_char(suddito,suddito->in_room);
		}
		else{
			sprintf(buf,"&4&b[&0%s&4&b]&0:&1&b%s&0",chdata.name,buf1);
			save_char_file_u(chdata);
		}
		send_to_char(buf,ch);
	}
}



void do_clan_set(struct char_data *ch, char *arg)
{
	char arg1[MAX_INPUT_LENGTH] ,arg2[MAX_INPUT_LENGTH];
	
	half_chop(arg,arg1,arg2);
	
	if (is_abbrev(arg1, "ranghi"     )) { do_clan_ranks(ch,arg2);          return ;}
	if (is_abbrev(arg1, "titoli"     )) { do_clan_titles(ch,arg2);         return ;}
//if (is_abbrev(arg1, "privilege" )) { do_clan_privilege(ch,arg2);      return ;}
//if (is_abbrev(arg1, "dues"      )) { do_clan_money(ch,arg2,CM_DUES);  return ;}
//if (is_abbrev(arg1, "appfee"    )) { do_clan_money(ch,arg2,CM_APPFEE);return ;}
//if (is_abbrev(arg1, "livapp"    )) { do_clan_application(ch,arg2);    return ;}
	if (is_abbrev(arg1, "stato"    )) { do_clan_set_status(ch,arg2);     return ;}
	if (is_abbrev(arg1, "tasse"     )) { do_clan_set_tasse(ch,arg2);      return ;}
	if (is_abbrev(arg1, "truppe"     )) { do_clan_set_truppe(ch,arg2);      return ;}
	if (is_abbrev(arg1, "attacco"     )) { do_clan_set_attacco(ch,arg2);      return ;}
	if (is_abbrev(arg1, "difesa"     )) { do_clan_set_difesa(ch,arg2);      return ;}

	send_clan_format(ch);
}

int update_diplo(int cpos,int vpos, int declaration){
	if(cpos==vpos){
	/*  sprintf(buf,"Non ha molto senso quello che vuoi fare\n");*/
		return -1;
	}
	if(cpos>num_of_clans||vpos>num_of_clans){
	/*   sprintf(buf,"C'e qualcosa di cannato qui:i clan specificati non esistono affatto!!!\n");
	send_to_char(buf,ch);*/
		return -1;
	}
	switch(declaration){
		case WAR:clan[cpos].diplomacy[vpos].status=WAR;clan[vpos].diplomacy[cpos].status=WAR;break;
		case PEACE:clan[cpos].diplomacy[vpos].status=PEACE;clan[vpos].diplomacy[cpos].status=PEACE;break;
		case ALLIANCE:clan[cpos].diplomacy[vpos].status=ALLIANCE;clan[vpos].diplomacy[cpos].status=ALLIANCE;break;
		case SURRENDER:clan[cpos].diplomacy[vpos].status=VASSALLO;clan[vpos].diplomacy[cpos].status=SURRENDER;break;
		case VASSALLO:clan[cpos].diplomacy[vpos].status=SURRENDER;clan[vpos].diplomacy[cpos].status=VASSALLO;break;
		default:return -1;
	}
  /*azzerero Proposte e timers ,e poi salvo*/
	clan[cpos].diplomacy[vpos].proposta=-1;
	clan[cpos].diplomacy[vpos].timer=-1;
	clan[vpos].diplomacy[cpos].proposta=-1;
	clan[vpos].diplomacy[cpos].timer=-1;
	save_clans();
	return clan[cpos].diplomacy[vpos].status;
}

void set_diplo_timer(int n_clan_prop,int n_clan_ric,int proposta){
	clan[n_clan_ric].diplomacy[n_clan_prop].proposta= proposta;
	clan[n_clan_ric].diplomacy[n_clan_prop].timer= 24;
}

char *status_flag(int status){
	char *str=NULL;
	switch(status){
		case PEACE:str="&3Pace&0";break;
		case WAR  :str="&1Guerra&0";break;
		case ALLIANCE  :str="&4Alleanza&0";break;
		case SURRENDER  :str="&5Vassallaggio&0";break;
		case VASSALLO  :str="&8Sudditanza&0";break;
	}
	return str;
}

void do_diplo_status(struct char_data *ch,int nclan){
	Regno *current,*avversario;
	char *status;
	char *name=NULL;
	bool found=FALSE;
	int i,time_left,tmp_regno,j,tmp_id;

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)<LVL_IMMORT)
		if(!IS_SET(GET_CLAN_PRIV(ch),CP_DIPLO)){
			sprintf(buf,"Non sei abbastanza influente per occuparti di cose cosi Importanti!!");
			send_to_char(buf,ch);
			return;
		}
	current=clan[nclan].diplomacy;
	sprintf(buf,"----------------------------\n");
	send_to_char(buf,ch);
	sprintf(buf,"|     DIPLOMACY STATUS     |\n");
	send_to_char(buf,ch);
	sprintf(buf,"----------------------------\n");
	send_to_char(buf,ch);
  /*Ciclo degli status effettivi*/
	for(i=0;i<num_of_clans;i++){
		if (i!=nclan){
			name = current[i].name;
			status=status_flag(current[i].status);
			if( clan[i].type==REGNO||current[i].status<=WAR){
				sprintf(buf,"Sei in stato di %s con %s\n",status,frase_regno(i,0));
				send_to_char(buf,ch);
			}
		}
	}
	found=FALSE;
  /*Ciclo delle prop fatte*/
	sprintf(buf,"\n&1---------------------------------------------------------&0\n");
	for(i=0;i<num_of_clans;i++){
		name=clan[i].name;
		avversario=clan[i].diplomacy;
		if(( i!=nclan) && avversario[nclan].proposta!=-1){
			status=status_flag(avversario[nclan].proposta);
			time_left=avversario[nclan].timer;
			found=TRUE;
			sprintf(buf,"Hai fatto una proposta di %s a %s. %d ore allo scadere.\n",status,frase_regno(i,0),time_left);
			send_to_char(buf,ch);
		}
	}
  /*Ciclo delle prop fatte*/
	
	for(i=0;i<num_of_clans;i++){
		if(( i!=nclan) &&(current[i].proposta != -1)){
			status=status_flag(current[i].proposta);
			name=current[i].name;
			time_left=current[i].timer+1;
			found=TRUE;
			sprintf(buf,"Hai Ricevuto una proposta di %s da %s. %d ore X rispondere.\n",status,frase_regno(i,0),time_left);
			send_to_char(buf,ch);
		}
	}
	if(found==TRUE)
		send_to_char("\n&1---------------------------------------------------------&0\n",ch);
	sprintf(buf,"\t&1&bVASSALLI&0\n");
	found=FALSE;
	for(i=0,j=0;i<num_of_clans;i++){
		if (i!=nclan){
			name = current[i].name;
			status=status_flag(current[i].status);
			if(current[i].status==VASSALLO){
				if(found==FALSE){
					send_to_char(buf,ch);
					found=TRUE;
				}
				j++;
				sprintf(buf,"[%d] %s--%s\n",j,name,tipi_clan[clan[i].type]);
				send_to_char(buf,ch);
			}
		}
	}
	if((tmp_regno=clan[nclan].regno)!=clan[nclan].id){
		sprintf(buf,"Sei vassallo di:\n");
		send_to_char(buf,ch);
		sprintf(buf,"[%d] %s--%s\n",j,name,tipi_clan[clan[tmp_regno].type]);
		send_to_char(buf,ch);
		tmp_regno=clan[tmp_regno].regno;
		while((tmp_id=clan[find_clan_by_id(tmp_regno)].type)!=REGNO||
			clan[tmp_id].regno!=tmp_regno){
			sprintf(buf,"[%d] %s--%s\n",j,name,tipi_clan[clan[tmp_id].type]);
			send_to_char(buf,ch);
			tmp_regno=clan[tmp_id].regno;
		}
	}
	
}

int diplo_permesso(int req_clan_num,int part_clan_num) {
	if((clan[req_clan_num].diplomacy[part_clan_num].timer!=-1) ||( clan[part_clan_num].diplomacy[req_clan_num].timer!=-1))
		return -1;
	else
		return 1;
}


int check_response(int responderer,int asker){
	if (clan[responderer].diplomacy[asker].proposta!= -1)
		return 1;
	else
		return -1;
}

void do_clan_diplo(struct char_data * ch, char * arg){
	char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
	int own,partner,newstatus;

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)>= LVL_IMMORT){
		if(!*arg){
			sprintf(buf,"\nSpecifica il nome del clan di cui vuoi vedere la Situazione Diplomatica.\n");
			send_to_char(buf,ch);
			return;
		}
		if((partner = find_clan(arg))==-1){
			sprintf(buf,"\nSpecifica Esattamente il nome del clan con cui vuoi interagire\n");
			send_to_char(buf,ch);
			return;
		}
		do_diplo_status(ch,partner);
		return;
	}
	own=find_clan_by_id(GET_CLAN(ch));
  /*controlli su chi ha l'autorita di fare sta roba*/
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_DIPLO)){
		sprintf(buf,"Non sei abbastanza influente per occuparti di cose cosi Importanti!!");
		send_to_char(buf,ch);
		return;
	}
	if(!*arg){
		sprintf(buf,"clan diplo status\n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo guerra <clan>\n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo alleanza <clan>\n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo pace <clan>\n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo resa <clan>\n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo vassallaggio <clan>\n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo accetta <clan> \n");
		send_to_char(buf,ch);
		sprintf(buf,"clan diplo rifiuta <clan> \n");
		send_to_char(buf,ch);
		return;
	}
	half_chop(arg,arg1,arg2);
  /*own=find_clan_by_id(GET_CLAN(ch));*/
	if( is_abbrev(arg1,"status")){
		do_diplo_status(ch,own);
		return;
	}
	partner=find_clan(arg2);
	
	if(!*arg2 || partner==-1){
		sprintf(buf,"C)Specifica Esattamente il nome del clan con cui vuoi interagire\n");
		send_to_char(buf,ch);
		return;
	}
	
	if(is_abbrev(arg1,"accetta")){
		if(check_response(own,partner)==1){
			newstatus=clan[own].diplomacy[partner].proposta;
			clan[own].diplomacy[partner].proposta=-1;
			clan[own].diplomacy[partner].timer=-1;
			update_diplo(partner,own,newstatus);
			sprintf(buf,"ok Hai accettato la Proposta di %s di %s\n",status_flag(newstatus),arg2);
			send_to_char(buf,ch);
			return;
		}
		sprintf(buf,"Cos'e che vuoi accettare?\n");
		send_to_char(buf,ch);
		return;
	}
	if(is_abbrev(arg1,"rifiuta")){
		if(check_response(own,partner)==1){
			newstatus=clan[own].diplomacy[partner].proposta;
			clan[own].diplomacy[partner].proposta=-1;
			clan[own].diplomacy[partner].timer=-1;
			save_clans();
			sprintf(buf,"ok Hai Rifiutato la Proposta di %s di %s\n",status_flag(newstatus),arg2);
			send_to_char(buf,ch);
			return;
		}
		sprintf(buf,"Cos'e che vuoi Rifiutare?\n");
		send_to_char(buf,ch);
		return;
	}
	
  /*check se posso fare una proposta al clan partner :se ne ho gia una in
    scadenza ovviamente no!*/
	if(diplo_permesso(own,partner)==1){
		if( is_abbrev(arg1, "guerra")){
			sprintf(buf,"WOW:hai appena dichiarato guerra al clan %s!!!\n",arg2);
			send_to_char(buf,ch);
			update_diplo(own,partner,WAR);
			return;
		}
		if( is_abbrev(arg1,"alleanza")){
			sprintf(buf,"Hai appena chiesto un ALLEANZA con il  clan %s!!!\n",arg2);
			send_to_char(buf,ch);
			set_diplo_timer(own,partner,ALLIANCE);
			return;
		}
		if( is_abbrev(arg1, "pace")){
			sprintf(buf,"WOW:hai appena chiesto la PACE con il clan %s!!!\n",arg2);
			send_to_char(buf,ch);
			set_diplo_timer(own,partner,PEACE);
			return;
		}
		if( is_abbrev(arg1, "resa")){
			sprintf(buf,"Ti sei appena ARRESO al clan %s!!!\n",arg2);
			send_to_char(buf,ch);
			update_diplo(own,partner,SURRENDER);
			return;
		}
		if( is_abbrev(arg1, "vassallaggio")){
			sprintf(buf,"WOW:hai appena intimato al clan %s di ARRENDERSI a te!!!\n",arg2);
			send_to_char(buf,ch);
			set_diplo_timer(own,partner,VASSALLO);
			return;
		}
		
		sprintf(buf,"Comando sconosciuto:digita clan diplo per tutti i possibili comandi.\n");
		send_to_char(buf,ch);
	}
	else{
		sprintf(buf,"Mi dispiace hai in ballo con questo clan un altra trattativa");
		send_to_char(buf,ch);
	}
	return;
}

void update_diplo_timers(){
	int i,j;
	for(i=0;i<num_of_clans;i++){
		for(j=0;j<num_of_clans;j++){
			if(clan[i].diplomacy[j].timer>0){
				clan[i].diplomacy[j].timer--;
			}
			else{
				clan[i].diplomacy[j].timer=-1;
				clan[i].diplomacy[j].proposta=-1;
			}
			save_clans();
		}
	}
}

ACMD(do_clan)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	
	half_chop(argument, arg1, arg2);
	if (is_abbrev(arg1, "assegna"   )){do_clan_assign_zone(ch,arg2); return  ;}
	if (is_abbrev(arg1, "crea"      )){ do_clan_create(ch,arg2);     return  ;}
//	if (is_abbrev(arg1, "distruggi" )){ do_clan_destroy(ch,arg2);    return  ;}
	if (is_abbrev(arg1, "negozi"    )){ list_shop_own (ch, 0);       return  ;}
	if (is_abbrev(arg1, "arruola"   )){ do_clan_enroll(ch,arg2);     return  ;}
	if (is_abbrev(arg1, "espelli"   )){ do_clan_expel(ch,arg2);      return  ;}
	if (is_abbrev(arg1, "who"       )){ do_clan_who(ch);             return  ;}
	if (is_abbrev(arg1, "status"    )){ do_clan_status(ch,arg2);     return  ;}
	if (is_abbrev(arg1, "info"      )){ do_clan_info(ch,arg2);       return  ;}
	if (is_abbrev(arg1, "domanda"   )){ do_clan_apply(ch,arg2);      return  ;}
	if (is_abbrev(arg1, "degrada"   )){ do_clan_demote(ch,arg2);     return  ;}
	if (is_abbrev(arg1, "promuovi"  )){ do_clan_promote(ch,arg2);    return  ;}
	if (is_abbrev(arg1, "set"       )){ do_clan_set(ch,arg2);        return  ;}
	if (is_abbrev(arg1, "preleva"   )){ do_clan_bank(ch,arg2,CB_WITHDRAW);return ;}
	if (is_abbrev(arg1, "deposita"  )){ do_clan_bank(ch,arg2,CB_DEPOSIT); return ;}
	if (is_abbrev(arg1, "diplomazia")){do_clan_diplo(ch,arg2);       return ;}
	if (is_abbrev(arg1, "economia"  )){do_clan_economy(ch,arg2);          return ;}
	if (is_abbrev(arg1, "privilegi" )){do_clan_privilegi(ch,arg2);   return ;}
	if (is_abbrev(arg1, "stipendi"  )){do_clan_stipendio(ch,arg2);   return ;}
	if (is_abbrev(arg1, "mobiles"   )){do_clan_mob(ch,arg2);   return ;}
	if (is_abbrev(arg1, "assumi"    )){do_clan_assumi(ch,arg2);   return ;}
	if (is_abbrev(arg1, "licenzia"  )){do_clan_licenzia(ch,arg2);   return ;}
	if (is_abbrev(arg1, "limiti"    )){do_clan_set_limits(ch,arg2);   return ;}
	if (is_abbrev(arg1, "condanna"  )){perform_condanna(ch,arg2);   return ;}
	if (is_abbrev(arg1, "assolvi"   )){perform_assolvi(ch,arg2);   return ;}  
	if (is_abbrev(arg1, "ricercati" )){regno_ricercati(ch,arg2);   return ;}    	
	if (is_abbrev(arg1, "libera"    )){do_clan_secondino1(ch,arg2); return ;}
	if (is_abbrev(arg1, "aprite"    )){do_clan_secondino2(ch);      return ;}
	send_clan_format(ch);
}

void do_clan_set_status(struct char_data *ch,char *arg){
	char arg1[30];
	char arg2[30];
	extern int top_of_p_table;
	extern struct player_index_element *player_table;
	struct char_data *vict=NULL;
	int found=0,status;
	bool ok=TRUE;
	
	struct char_file_u chdata;
	extern struct char_data *is_playing(char *vict_name);
	int clan_num=0,immcom=0,j;
	
    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun Regno!\r\n",ch);
			return;
		}
	}
	
	else
		immcom=1;
	//PEPPE CLAN
	//la riga qui sotto e' stata sotituita per permettere a tutti quelli che avessero
	//un rank superiore a 26 (27-28-29-30) di settare i ranke degli altri pg 
	//if(GET_CLAN_RANK(ch)!=(clan[clan_num].ranks -3 ) && immcom==0) {
	if(!(*arg)) {
		show_status_regno(ch);
		return;
	}
	two_arguments(arg,arg1,arg2);
	if(!is_number(arg2))
		ok=FALSE;
	if(ok==FALSE||((status=(atoi(arg2)))<=0||status>(MAX_RANKS-1))){
		sprintf(buf,"Il secondo argomento deve essere un numero compreso tra 1 e %d",MAX_RANKS-1);
		send_to_char(buf,ch);
		return;
	}
	for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
		//cerco tra i pg giocanti
		if((vict=is_playing((player_table +j)->name))) {
			if(str_cmp(arg1,GET_NAME(vict))==0)
				found=1;
		}
		//cerco tra i pg non connessi
		else{
			load_char((player_table + j)->name, &chdata);
			if(str_cmp(arg1,chdata.name)==0)
				found=2;
		}
	}
	
	switch(found){
		case 0:
			send_to_char("Questo giocatore non esiste",ch);
			return;
		case 1:
			if(find_clan_by_id(GET_CLAN(vict))!=clan_num&&immcom!=1){
				send_to_char("Non appartiene al tuo regno",ch);
				return;
			} else{
				GET_CLAN_RANK(vict)=status;
				send_to_char("Okay.",ch);
				save_char(vict, vict->in_room);
				return;
			}
			break;
		case 2:
			if(chdata.player_specials_saved.clan!=GET_CLAN(ch)&&immcom!=1){
				send_to_char("Non appartiene al tuo regno",ch);
				return;
			} else{
				chdata.player_specials_saved.clan_rank=status;
				send_to_char("Okay.",ch);
				save_char_file_u(chdata);
				return;
			}
			break;
		default:
			send_to_char("BUGGONE MEGAGALATTICO!",ch);
			return;
	}
}

void show_status_regno(struct char_data *ch){
	int i,j,clan_num;
	char line[80];
	bzero(buf2,sizeof(buf2));


    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0)
		return;
	for(i=0;i<(clan[clan_num].ranks-1);i++){
		if(i<10)
			sprintf(line," ");
		sprintf(buf,"%d]%s",i,clan[clan_num].rank_name[i]);
		strcat(line,buf);
		for(j=0;strlen(line)<=25;j++)
			strcat(line," ");
		sprintf(buf,"[&1&b%d&0]\n",clan[clan_num].rank_tax[i]);
		strcat(line,buf);
		strcat(buf2,line);
		bzero(line,80);
	}
	page_string(ch->desc, buf2, 0);
}


void do_clan_set_truppe(struct char_data *ch,char *argument){

	int numero_truppe,regno;
	char arg1[30];
	char arg2[30];
	if (!*argument) {
		send_clan_format(ch);
		return;
	}
	two_arguments(argument,arg1,arg2);
	numero_truppe=atoi(arg1);
	if((regno=find_clan(arg2))<0){
		send_to_char("Non esiste un regno con quel nome!",ch);
		return;
	}
	clan[regno].truppe_disponibili=numero_truppe*10;
}

void do_clan_set_attacco(struct char_data *ch,char *argument){

	int numero_attacco,regno;
	char arg1[30];
	char arg2[30];
	if (!*argument) {
		send_clan_format(ch);
		return;
	}
	two_arguments(argument,arg1,arg2);
	numero_attacco=atoi(arg1);
	if((regno=find_clan(arg2))<0){
		send_to_char("Non esiste un regno con quel nome!",ch);
		return;
	}
	clan[regno].valore_attacco=numero_attacco;
}


void do_clan_set_difesa(struct char_data *ch,char *argument){

	int numero_difesa,regno;
	char arg1[30];
	char arg2[30];
	if (!*argument) {
		send_clan_format(ch);
		return;
	}
	two_arguments(argument,arg1,arg2);
	numero_difesa=atoi(arg1);
	if((regno=find_clan(arg2))<0){
		send_to_char("Non esiste un regno con quel nome!",ch);
		return;
	}
	clan[regno].valore_difesa=numero_difesa;
}



void do_clan_set_tasse(struct char_data *ch,char *argument){
	int class=0,new_tasse=0,clan_num=0,ranks=0;
	bool ok =TRUE;
	char arg1[30];
	char arg2[30];

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun Regno!\r\n",ch);
			return;
		}
	}
	ranks=clan[clan_num].ranks-1;
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_SET_TAX)){
		send_to_char("Non hai l'autorita' per fare questo!",ch);
		return;
	}
	if(!*argument){
		show_status_regno(ch);
		return;
	}
	
	two_arguments(argument,arg1,arg2);
	if(!is_number(arg1)||!is_number(arg2))
		ok=FALSE;
	if(ok==FALSE||(class=atoi(arg1))<0||class>ranks){
		sprintf(buf,"Il primo arg deve essere un numero compreso tra 0 e %d.\nIl secondo,l'ammmontare delle tasse.\n",ranks);
		send_to_char(buf,ch);
		return;
	}
	new_tasse=atoi(arg2);
	if(new_tasse<0){
		send_to_char("Le tasse non possono essere negative.",ch);
		return;
	}
	sprintf(buf,"Cambiate le tasse della Classe sociale &1&b%s&0 da &1&b%d&0 a &1&b%d&0.\r\n",clan[clan_num].rank_name[class],clan[clan_num].rank_tax[class],new_tasse);
	clan[clan_num].rank_tax[class]=new_tasse;
	save_clans();
	send_to_char(buf,ch);
}

void do_clan_assign_zone(struct char_data *ch,char *argument){
	extern struct zone_data *zone_table;	/* zone table   */
	extern int top_of_zone_table;
	extern void print_zone_to_buf(char *bufptr, int zone);
	char arg1[32];
	char arg2[32];
	char tmp_buf[256];
	int i,j,k;
	bzero(tmp_buf,sizeof(tmp_buf));
	if(GET_LEVEL(ch)<LVL_GOD){
		send_to_char("Solo un creatore puo assegnare una zona ad un Clan",ch);
		return;
	}
	if (!*argument) {
		send_clan_format(ch);
		return;
	}
	two_arguments(argument,arg1,arg2);
	if(!*arg2){
		send_to_char("Devi specificare anche il vnum della zona che vuoi assegnare",ch);
		return;
	}
	if((k=find_clan(arg1))<0){
		send_to_char("Non esiste un regno con quel nome!",ch);
		return;
	}
	
	if(!is_number(arg2)){
		send_to_char("Il secondo argomento deve essere un Vnum di una zona!",ch);
		return;
	}
	for (j = atoi(arg2), i = 0; zone_table[i].number != j && i <= top_of_zone_table; i++);
	
	if(i>top_of_zone_table){
		send_to_char("Non e' una zona valida.\r\n", ch);
		return;
	}
	else{
		clan[k].capitale_regno=j;
		print_zone_to_buf(tmp_buf, i);
		sprintf(buf,"Hai assegnato al clan %s la zona:\n%s\n",arg1,tmp_buf);
		send_to_char(buf,ch);
	}
	save_clans();
}


void assign_to_regno(struct char_data *suddito,int choice){
	extern int mortal_start_zone[];
	int i,zone_start,cl=0;
	bool found;
	
	if(num_of_clans==0){/*non ci sono ancora regni creati,e' inutile cercare*/
		GET_CLAN(suddito)=0;
		GET_CLAN_RANK(suddito)=0;
		return;
	}
	zone_start=mortal_start_zone[choice+1];
	
	for(i=0,found=FALSE;i<num_of_clans;i++){
		if(clan[i].capitale_regno==zone_start){
			cl=i;
			found=TRUE;
		}
	}
	
	if(found==TRUE){
		GET_CLAN(suddito)=clan[cl].id;
		GET_CLAN_RANK(suddito)=2;
//		clan[cl].members++;
//		clan[cl].power++;
		sprintf(buf,"NUOVO SUDDITO PER %s:%s",frase_regno(cl,0),GET_NAME(suddito));
		mudlog(buf, NRM, LVL_IMMORT, FALSE);
		save_clans();
	}
	else{
		GET_CLAN(suddito)=0;
		GET_CLAN_RANK(suddito)=0;
	}
	return;
}

int update_class(struct char_data *taxed,struct char_file_u chdata,bool playing){
	int i,capital,tax,old_rank,new_rank=0,cl, numcase,x;
	bool is_playing=TRUE;
	long housetax;
	
	if(playing){
		
		if(taxed!=NULL){
			if(GET_CLAN(taxed)<=0)
	          return 0;

			cl=find_clan_by_id(GET_CLAN(taxed));
			old_rank=GET_CLAN_RANK(taxed);
			capital=GET_GOLD(taxed)+GET_BANK_GOLD(taxed);
		}
		else
			return 0;
	}
	else
	if(!playing){
		is_playing=FALSE;
		cl=find_clan_by_id(GET_CLAN2(chdata));
		old_rank=chdata.player_specials_saved.clan_rank;
		capital=chdata.points.gold+chdata.points.bank_gold;
	}
	
	//Tassa sulle case
	numcase=0;
	tax=TAX_HOUSE;
	housetax = 0;
	for (x=0; x < num_of_houses; x++)
	{
		if (real_room(house_control[x].vnum) >= 0)
		{
			if(!playing){
				if (house_control[x].owner == chdata.char_specials_saved.idnum) {
					if(chdata.points.bank_gold>=tax){
						chdata.points.bank_gold-=tax;
						tax = 0;
					} else{
						i=tax - chdata.points.bank_gold;
						if (chdata.points.gold>=i){
							chdata.points.bank_gold=0;
							chdata.points.gold-=i;
							tax=0;
						}
						
					}
					numcase++;
					if (tax == 0) {
						house_control[x].last_payment = time(0);
						housetax = TAX_HOUSE * numcase;
						sprintf(buf,"(REGNI) %s paga le tasse la casa %d.",chdata.name, house_control[x].vnum);
						log(buf);
					} else {
						sprintf(buf,"(REGNI) %s non puo' pagare le tasse la casa %d.",chdata.name, house_control[x].vnum);
						log(buf);
					}
				}//owner
			}//playing
			
			if(playing) {
				if (house_control[x].owner == GET_IDNUM(taxed)) {
					if(GET_BANK_GOLD(taxed)>=tax){
						GET_BANK_GOLD(taxed)-=tax;
						tax = 0;
					} else{
						i=tax-GET_BANK_GOLD(taxed);
						if (GET_GOLD(taxed)>=i){
							GET_BANK_GOLD(taxed)=0;
							GET_GOLD(taxed)-=i;
							tax = 0;
						}
					}
					numcase++;
					if (tax == 0) {
						house_control[x].last_payment = time(0);
						housetax += TAX_HOUSE;
						sprintf(buf,"(REGNI) %s paga le tasse per la casa %d.",GET_NAME(taxed),  house_control[x].vnum);
						log(buf);
					} else {
						sprintf(buf,"(REGNI) %s non puo' pagare le tasse per la casa %d.",GET_NAME(taxed),  house_control[x].vnum);
						log(buf);
					}
				}  //owner
			}  //playing
		}//realroom
	}//for
	
	
	House_save_control();
	
	new_rank=old_rank;
	
//  if(old_rank<=AUTOMATIC_CLASSES&&old_rank>=2){
//    new_rank=1;
//    while(capital>=clan[cl].rank_bounds[new_rank + 1]&&new_rank<=AUTOMATIC_CLASSES)
//      new_rank++;
//  }
	
	tax=clan[cl].rank_tax[new_rank-1];
	
	if(playing){
//	GET_CLAN_RANK(taxed)=new_rank;
		if(GET_BANK_GOLD(taxed)>=tax)
			GET_BANK_GOLD(taxed)-=tax;
		else{
			i=tax-GET_BANK_GOLD(taxed);
			GET_BANK_GOLD(taxed)=0;
			if (GET_GOLD(taxed)>=i)
				GET_GOLD(taxed)-=i;
			else {
				tax=GET_GOLD(taxed);
				GET_GOLD(taxed)=0;
			}
		}
		sprintf(buf,"&1&bPAGHI LE TASSE&0: hai &4&b%d&0 monete in meno!\n",tax);
		send_to_char(buf,taxed);
		sprintf(buf,"delle quali &4&b%ld&0 delle %ld dovute per beni materiali.\n", housetax, (long) (TAX_HOUSE * numcase));
		send_to_char(buf,taxed);
		save_char(taxed,taxed->in_room);
	}
	else{
//	chdata.player_specials_saved.clan_rank=new_rank;
		if(chdata.points.bank_gold>=tax)
			chdata.points.bank_gold-=tax;
		else{
			i=tax-chdata.points.bank_gold;
			chdata.points.bank_gold=0;
			if (chdata.points.gold>=i)
				chdata.points.gold-=i;
			else {
				tax=chdata.points.gold;
				chdata.points.gold=0;
			}
		}
		save_char_file_u(chdata);
	}
	return tax;
}


void tax_improve(void){
	int to_clan,from_pg,i,j;
	struct char_data *taxed;
	struct char_file_u chdata;
	
	log("CALCULATING TAXES");
	for(i=0;i<num_of_clans;i++){
		sprintf(buf,"REGNO:%s",clan[i].name);
		log(buf);
		//clan[i].economy.last_tax_income=0;
		to_clan=0;
		for (j = 0; j <= top_of_p_table; j++){
			if((taxed=is_playing((player_table +j)->name))) {
				if(GET_CLAN(taxed)==clan[i].id) {
					from_pg=update_class(taxed,chdata,TRUE);
					to_clan+=from_pg;
					save_char(taxed, taxed->in_room);
					sprintf(buf,"(REGNI) %s--$:%d",GET_NAME(taxed),from_pg);
					log(buf);
				}
			} else {
				load_char((player_table + j)->name, &chdata);
				if(chdata.player_specials_saved.clan==clan[i].id) {
					if (get_no_play(NULL, chdata, NOPAYTIMEOUT, FALSE)) {
						from_pg=update_class(NULL,chdata,FALSE);
						to_clan+=from_pg;
						save_char_file_u(chdata);
						sprintf(buf,"(REGNI) %s--$:%d",chdata.name,from_pg);
						log(buf);
					}
				}
			}
			
		}
		clan[i].economy.treasure+=to_clan;
		clan[i].economy.current_entrate +=to_clan;
		//clan[i].economy.last_tax_income = clan[i].economy.tax_income;
		clan[i].economy.tax_income = to_clan;
		sprintf(buf,"(REGNI) TOTAL TAX FOR %s-$:%d",clan[i].name,to_clan);
		mudlog(buf, NRM, LVL_IMMORT, FALSE);
		log(buf);
	}
	save_clans();
}

/*------------------------------------------------------------------------*
 *questa procedura scova un pg connesso o non connesso                    *
 *Ritorna: [0] se non trova il pg                                         *
 *         [1] se lo trova ed e' connesso (ch)                            *
 *         [2] se lo trova e non e' connesso (chdata)                     *
 *Ovviamente si utlizzera poi l'argomento ch se la funzione ritorna 1     *
 *                            l'argomento chdata se torna 2               *
 *PS:ch e' un puntatore ,chdata no!                                       *
 *------------------------------------------------------------------------*/

int find_pg(char *name,struct char_data *searched,struct char_file_u srchdata){
	int j,found;
	for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
		//cerco tra i pg giocanti
		if((searched=is_playing((player_table +j)->name))) {
			if(str_cmp(name,GET_NAME(searched))==0)
				found=1;
		}
		
		//cerco tra i pg non connessi
		else{
			load_char((player_table + j)->name, &srchdata);
			if(str_cmp(name,srchdata.name)==0)
				found=2;
		}
	}
	return found;
}

struct mob_ass{
	int num;
	struct char_data *mob;
	int fee;
	int vnum;
};

void display_stipendi(struct char_data *ch,int idclan){
	
	struct char_data *suddito;
	struct char_data *mob=NULL;
	struct char_file_u chdata;
	bool imcom=FALSE,found=FALSE;
	int i,j,next_pos=0,num_parz=0,tot_parz=0,tot=0,num=0;
	struct mob_ass mob_counter[MAX_MOB_ASSOLDABILI];
	for(i=0;i<MAX_MOB_ASSOLDABILI;i++){
		mob_counter[i].num=0;
		mob_counter[i].mob=NULL;
		mob_counter[i].fee=0;
		mob_counter[i].vnum=0;
	}
	if(GET_LEVEL(ch)>=LVL_GOD)
		imcom=TRUE;
  /*else
    idclan=GET_CLAN(ch);*/
	sprintf(buf,
		"                   PG STIPENDIATI                   \n"
		"&1------------------------------------------------&0\n"
	);
	send_to_char(buf,ch);
	for (j = 0,found=FALSE; (j <= top_of_p_table ); j++){
		//cerco tra i pg giocanti
		if((suddito=is_playing((player_table +j)->name))) {
			if(GET_CLAN(suddito)==idclan||imcom){
				if(GET_STIPENDIO(suddito).has_fee&&GET_STIPENDIO(suddito).clan_fee>0){
					sprintf(buf,"&4&b[&0%s&4&b]&0 $:&4&b%d\n",GET_NAME(suddito),GET_STIPENDIO(suddito).clan_fee);
					found=TRUE;
					num_parz++;
					tot_parz+=GET_STIPENDIO(suddito).clan_fee;
					send_to_char(buf,ch);
				}
			}
		}
		//cerco tra i pg non connessi
		else{
			load_char((player_table + j)->name, &chdata);
			if(GET_CLAN2(chdata)==idclan||imcom){
				if(GET_STIPENDIO2(chdata).has_fee&&GET_STIPENDIO2(chdata).clan_fee>0){
					sprintf(buf,"&4&b[&0%s&4&b]&0 $:&4&b%d\n",chdata.name,
						GET_STIPENDIO2(chdata).clan_fee);
					found=TRUE;
					num_parz++;
					tot_parz+=GET_STIPENDIO2(chdata).clan_fee;
					send_to_char(buf,ch);
				}
			}
		}
	}
	if(found==FALSE)
		send_to_char("&4&bNESSUNO&0\n",ch);
	
	sprintf(buf,
		"&1------------------------------------------------&0\n"
		"               MOB AL SERVIZIO DEL REGNO\r\n"
		"&1------------------------------------------------&0\n"
	);
	send_to_char(buf,ch);
	for (mob = character_list; mob; mob = mob->next){
		if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&GET_STIPENDIO(mob).has_fee==TRUE){
			if(GET_STIPENDIO(mob).idclan>0&&GET_STIPENDIO(mob).clan_fee>0&&
				CLAN_ID(mob) ==idclan ){
				if( GET_STIPENDIO(mob).clan_fee!=GET_LEVEL(mob)*FEE_PER_LEVEL)
					GET_STIPENDIO(mob).clan_fee=GET_LEVEL(mob)*FEE_PER_LEVEL;
				for(i=0,found=FALSE;i<MAX_MOB_ASSOLDABILI&&(found==FALSE);i++){
					if(GET_MOB_VNUM(mob)==mob_counter[i].vnum){
						found=TRUE;
						mob_counter[i].num++;
					}
				}
				if(found==FALSE){
					mob_counter[next_pos].num++;
					mob_counter[next_pos].mob=mob;
					mob_counter[next_pos].vnum=GET_MOB_VNUM(mob);
					mob_counter[next_pos].fee=GET_STIPENDIO(mob).clan_fee;
					next_pos++;
				}
			}
		}
	}
	for(i=0;i<next_pos;i++){
		sprintf(buf,"&4&b[&0%d&4&b]&0 %s &4&bVNUM&0:%d &4&bLIV&0:%d &4&bSTIP&0:%d\n",mob_counter[i].num,
			GET_NAME(mob_counter[i].mob),mob_counter[i].vnum,GET_LEVEL(mob_counter[i].mob),mob_counter[i].fee);
		num+=mob_counter[i].num;
		tot+=mob_counter[i].num*mob_counter[i].fee;
		send_to_char(buf,ch);
	}
	
	sprintf(buf,
		"&1------------------------------------------------&0 \n"
		"&3&bTOT PG&0  :%d                  &3&bCosto&0:%d    \n"
		"&1------------------------------------------------&0 \n"
		"&3&bTOT MOB&0 :%d                  &3&bCosto&0:%d    \n"
		"&1------------------------------------------------&0 \n"
		"&3&bTOT STIP&0:%d                  &3&bCosto&0:%d    \n"
		"&1------------------------------------------------&0 \n",
		num_parz,tot_parz,  num,tot,num_parz+num,tot_parz+tot);
	send_to_char(buf,ch);
}

void do_clan_stipendio(struct char_data *ch,char *arg){
	char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
	int j,cl=0,rank=0,found=0,stipendio=0,new_stipendio=0;
	bool imcom=FALSE;
	bool change=TRUE;
	bool has_fee=FALSE;
	struct char_file_u chdata;
	struct char_data *suddito=NULL;

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)>=LVL_GOD)
		imcom=TRUE;

	cl=find_clan_by_id(GET_CLAN(ch));
	if(!imcom&&(cl<0||!IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_FEES))){
		send_to_char("Non sei autorizzato!",ch);
		return;
	}
	
	if(!*arg){
		display_stipendi(ch,GET_CLAN(ch));
		return;
	}
	two_arguments(arg,arg1,arg2);
	if(!*arg2)
		change=FALSE;
	else{
		new_stipendio=atoi(arg2);
		if(!is_number(arg2)||(stipendio<0)){
			send_to_char("il secondo argomento deve essere un numero compreso tra 1 e 13",ch);
			return;
		}
	}
	for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
		//cerco tra i pg giocanti
		if((suddito=is_playing((player_table +j)->name))) {
			
			if(str_cmp(arg1,GET_NAME(suddito))==0)
				found=1;
		}
		//cerco tra i pg non connessi
		else{
			load_char((player_table + j)->name, &chdata);
			
			if(str_cmp(arg1,chdata.name)==0)
				found=2;
		}
	}
	switch(found){
		case 0:
			sprintf(buf,"Non esiste nessun %s.",arg1);
			send_to_char(buf,ch);
			return;
			break;
		case 1:
			has_fee=GET_STIPENDIO(suddito).has_fee;
			cl=GET_CLAN(suddito);
			stipendio=GET_STIPENDIO(suddito).clan_fee;
			break;
		case 2:
			has_fee=GET_STIPENDIO2(chdata).has_fee;
			cl=GET_CLAN2(chdata);
			stipendio=GET_STIPENDIO2(chdata).clan_fee;
			break;
	}
	if((cl!=GET_CLAN(ch)||rank>=GET_CLAN_RANK(ch))&&!imcom)
		send_to_char("NON puoi!",ch);
	else{
		if(change){
			send_to_char("Okay.\n",ch);
			if(found==1){
				GET_STIPENDIO(suddito).has_fee=TRUE;
				if(imcom){
					GET_STIPENDIO(suddito).idchar=GET_IDNUM(ch);
					GET_STIPENDIO(suddito).char_fee=new_stipendio;
				}
				else{
					GET_STIPENDIO(suddito).idclan=GET_CLAN(ch);
					GET_STIPENDIO(suddito).clan_fee=new_stipendio;
					save_char(suddito,suddito->in_room);
				}
			}
			if(found==2){
				GET_STIPENDIO2(chdata).has_fee=TRUE;
				if(imcom){
					GET_STIPENDIO2(chdata).idchar=GET_IDNUM(ch);
					GET_STIPENDIO2(chdata).char_fee=new_stipendio;
				}
				else{
					GET_STIPENDIO2(chdata).idclan=GET_CLAN(ch);
					GET_STIPENDIO2(chdata).clan_fee=new_stipendio;
					save_char_file_u(chdata);
				}
			}
		}
		if(found==1){
			sprintf(buf,"&4&b[&0%s&4&b]&0$:&4&b%d&0",GET_NAME(suddito),
				GET_STIPENDIO(suddito).clan_fee);
			save_char(suddito,suddito->in_room);
		}
		else{
			sprintf(buf,"&4&b[&0%s&4&b]&0:&1&b%d&0",chdata.name,GET_STIPENDIO2(chdata).clan_fee);
			save_char_file_u(chdata);
		}
		send_to_char(buf,ch);
	}
}

void stipendi_mensili(void){
	int i,j,cl,stipendio_char,stipendio_clan;
	int found=0,found2=0;
	bool has_fee;
	struct char_data *suddito,*pagante,*mob;
	struct char_file_u chdata,chpag;
  /*pg*/
	for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
		has_fee=FALSE;
		//cerco tra i pg giocanti
		if((suddito=is_playing((player_table +j)->name))) {
			
			if(GET_STIPENDIO(suddito).has_fee){
				if(GET_STIPENDIO(suddito).idclan>0){
					cl=find_clan_by_id(GET_STIPENDIO(suddito).idclan);
					if((stipendio_clan=GET_STIPENDIO(suddito).clan_fee)>0){
						has_fee=TRUE;
						GET_BANK_GOLD(suddito)+=stipendio_clan;
						clan[cl].economy.treasure-=stipendio_clan;
						clan[cl].economy.stipendi+=stipendio_clan;
						clan[cl].economy.current_uscite+=stipendio_clan;
					}
				}
				if(GET_STIPENDIO(suddito).idchar>0){
					if((stipendio_char=GET_STIPENDIO(suddito).char_fee)>0){
						for (i= 0,found2=0; (i <= top_of_p_table)&&found!=1; i++){
							//cerco tra i pg giocanti
							if((pagante=is_playing((player_table +i)->name))) {
								if(GET_STIPENDIO(suddito).idchar==GET_IDNUM(pagante)){
									GET_BANK_GOLD(suddito)+=stipendio_char;
									GET_BANK_GOLD(pagante)-=stipendio_char;
									save_char(pagante,pagante->in_room);
									found2=1;
									has_fee=TRUE;
								}
							} else{
								load_char((player_table + i)->name, &chpag);
								if(GET_STIPENDIO(suddito).idchar==chpag.char_specials_saved.idnum){
									if (get_no_play(NULL, chpag , NOPAYTIMEOUT, FALSE)) {
										GET_BANK_GOLD(suddito)+=stipendio_char;
										chpag.points.bank_gold-=stipendio_char;
										save_char_file_u(chpag);
										found2=1;
										has_fee=TRUE;
										sprintf(buf,"Ricevi il tuo stipendio da %s ,$%d.",chpag.name,stipendio_char);
										send_to_char(buf,suddito);
									}
								}
							}
						} //for
					}
				}
				GET_STIPENDIO(suddito).has_fee=has_fee;
				save_char(suddito,suddito->in_room);
			}
		}
		//cerco tra i pg non connessi
		else{
			load_char((player_table + j)->name, &chdata);
			if(GET_STIPENDIO2(chdata).has_fee){
				if((cl=find_clan_by_id(GET_STIPENDIO2(chdata).idclan))>0){
					if((stipendio_clan=GET_STIPENDIO2(chdata).clan_fee)>0){
						if (get_no_play(NULL, chdata , NOPAYTIMEOUT, FALSE)) {
							has_fee=TRUE;
							chdata.points.bank_gold+=stipendio_clan;
							clan[cl].economy.treasure-=stipendio_clan;
							clan[cl].economy.stipendi+=stipendio_clan;
							clan[cl].economy.current_uscite+=stipendio_clan;
						}
					}
				}
				if(GET_STIPENDIO2(chdata).idchar>0){
					if((stipendio_char=GET_STIPENDIO2(chdata).char_fee)>0){
						for (i= 0,found2=0; (i <= top_of_p_table)&&found!=1; i++){
							//cerco tra i pg giocanti
							if((pagante=is_playing((player_table +i)->name))) {
								if(GET_STIPENDIO2(chdata).idchar==GET_IDNUM(pagante)){
									if (get_no_play(NULL, chdata , NOPAYTIMEOUT, FALSE)) {
										chdata.points.bank_gold+=stipendio_char;
										GET_BANK_GOLD(pagante)-=stipendio_char;
										sprintf(buf,"Paghi lo stipendio a %s ,$%d.",chdata.name,stipendio_char);
										send_to_char(buf,pagante);
										save_char(pagante,pagante->in_room);
										found2=1;
										has_fee=TRUE;
									}
								}
							} else{
								load_char((player_table + i)->name, &chpag);
								if(GET_STIPENDIO2(chdata).idchar==chpag.char_specials_saved.idnum){
									if (get_no_play(NULL, chpag , NOPAYTIMEOUT, FALSE)) {
										chdata.points.bank_gold+=stipendio_char;
										chpag.points.bank_gold-=stipendio_char;
										save_char_file_u(chpag);
										found2=1;
										has_fee=TRUE;
									}
								}
							}/*else*/
						} /*for*/
					}/*charfee*/
				}/*idchar*/
				GET_STIPENDIO2(chdata).has_fee=has_fee;
				save_char_file_u(chdata);
			}/*has_fee*/
		}/*else*/
	}/*for*/
	for (mob = character_list; mob; mob = mob->next){
		if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&GET_STIPENDIO(mob).has_fee){
			if(GET_STIPENDIO(mob).idclan>0&&(stipendio_clan=GET_STIPENDIO(mob).clan_fee)>0&&(cl=find_clan_by_id(GET_STIPENDIO(mob).idclan))>=0){
				clan[cl].economy.treasure-=stipendio_clan;
				clan[cl].economy.stipendi+=stipendio_clan;
				clan[cl].economy.current_uscite+=stipendio_clan;
			}
			else{//a differenza di un pg un mob puo essere stipendiato 1 volta sola
				if(GET_STIPENDIO(mob).idchar>0&&
					(stipendio_char=GET_STIPENDIO(mob).char_fee)>0){
					//solito ciclo per beccare chi deve pagare tra i pg
					for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
						//cerco tra i pg giocanti
						if((suddito=is_playing((player_table +j)->name))) {
							if(GET_IDNUM(suddito)==GET_STIPENDIO(mob).idchar){
								GET_BANK_GOLD(suddito)-=stipendio_char;
								sprintf(buf,"Paghi %s :$%d",GET_NAME(mob),stipendio_char);
								send_to_char(buf,suddito);
								save_char(suddito,suddito->in_room);
							}
						}
						else{
							load_char((player_table + j)->name, &chpag);
							if(GET_STIPENDIO(mob).idchar==chpag.char_specials_saved.idnum)
							{
								chpag.points.bank_gold-=stipendio_char;
								save_char_file_u(chpag);
							}
						}
					}//for
				}//if
			}//else
		} //if
	}//for
	save_clans();
}/*end*/
#define MESE 0
#define SEMESTRE 1
#define CLAN_EC(i) clan[i].economy

void update_bilancio_regni(void){
	FILE *fd;
	time_t now=time(0);
	int i, current_uscite,current_entrate=0,dogana_income,stipendi,zecca,treasure,export,tax;
	char nomeregno[30];
	
	for(i=0;i<num_of_clans;i++){
		treasure=CLAN_EC(i).treasure;
		export=CLAN_EC(i).corrent_export_income;
		current_uscite=CLAN_EC(i).current_uscite;
		current_entrate=CLAN_EC(i).current_entrate;
		dogana_income=CLAN_EC(i).current_dogana_income;
		stipendi=CLAN_EC(i).stipendi;
		zecca=CLAN_EC(i).current_zecca_income;
		tax=CLAN_EC(i).tax_income;
		
	/*write to disk the last balance and update it*/
		
		sprintf(nomeregno,"etc/regno.%d",i);
		if((fd=fopen(nomeregno,"a+"))==NULL){
			sprintf(buf,"WARNING: ' %s ' not Found ,Creating it.....",nomeregno);
			log(buf);
			fd=fopen(nomeregno,"wb");
		}
		fd=fopen(nomeregno,"a+");
		fprintf(fd,"%s  %s",clan[i].name,ctime(&now ));
		sprintf(buf,
			"TESORO:%d - ULTIME USCITE:%d - ULTIME ENTRATE:%d\n"
			"STIPENDI:%d - TASSE:%d - DOGANA :%d - PROD. ZECCA:%d\n"
			"EXPORT:%d\n",
			CLAN_EC(i).treasure,current_uscite,current_entrate,stipendi,
			tax,dogana_income,zecca,export);
		fprintf(fd,"%s\n",buf);
		fclose(fd);
		
		CLAN_EC(i).last_sem_treasure=treasure;
		CLAN_EC(i).current_uscite        =0;
		CLAN_EC(i).current_entrate       =0;
		CLAN_EC(i).current_dogana_income =0;
		CLAN_EC(i).stipendi              =0;
		CLAN_EC(i).current_zecca_income  =0;
		CLAN_EC(i).corrent_export_income =0;
		CLAN_EC(i).tax_income = tax;
		CLAN_EC(i).last_uscite_sem=current_uscite;
		CLAN_EC(i).last_entrate_sem=current_entrate;
		CLAN_EC(i).last_dogana_income=dogana_income;
		CLAN_EC(i).last_stipendi=stipendi;
		CLAN_EC(i).last_zecca_income=zecca;
		CLAN_EC(i).last_export_income=export;
		CLAN_EC(i).last_tax_income=tax;
		
	}
	save_clans();
}

void do_clan_economy(struct char_data *ch, char *arg){
	int cl,bal_tesoro,bal_uscite,bal_entrate,bal_zecca,bal_tax,bal_export,bal_stipendi,bal_dogana;
	bool imcom=FALSE;
	extern struct time_info_data time_info;

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)>=LVL_GOD)
		imcom=TRUE;
	if(!*arg||(cl=find_clan(arg))<0)
		if(imcom){
			send_to_char("Devi specificare il nome di un Regno",ch);
			return;
		}
	if(!imcom){
		if(!IS_SET(GET_CLAN_PRIV(ch),CP_ECONOMY_V)){
			send_to_char("Queste cose sono troppo grandi per te!",ch);
			return;
		}
		if((cl=find_clan_by_id(GET_CLAN(ch)))<0){
			send_to_char("NON APPARTIENI A NESSUN CLAN CONOSCIUTO!",ch);
			return;
		}
	}
  /*bal_tesoro =CLAN_EC(cl).treasure-CLAN_EC(cl).last_sem_treasure;
  bal_uscite =CLAN_EC(cl).current_uscite-CLAN_EC(cl).last_uscite_sem;
  bal_entrate=CLAN_EC(cl).current_entrate-CLAN_EC(cl).last_entrate_sem;
  bal_zecca =CLAN_EC(cl).current_zecca_income-CLAN_EC(cl).last_zecca_income;
  bal_export=CLAN_EC(cl).corrent_export_income-CLAN_EC(cl).last_export_income;
  bal_dogana=CLAN_EC(cl).current_dogana_income-CLAN_EC(cl).last_dogana_income;
  bal_stipendi=CLAN_EC(cl).stipendi-CLAN_EC(cl).last_stipendi;
  bal_tax=CLAN_EC(cl).tax_income-CLAN_EC(cl).last_tax_income;*/
	bal_tesoro =CLAN_EC(cl).last_sem_treasure;
	bal_uscite =CLAN_EC(cl).last_uscite_sem;
	bal_entrate=CLAN_EC(cl).last_entrate_sem;
	bal_zecca =CLAN_EC(cl).last_zecca_income;
	bal_export=CLAN_EC(cl).last_export_income;
	bal_dogana=CLAN_EC(cl).last_dogana_income;
	bal_stipendi=CLAN_EC(cl).last_stipendi;
	bal_tax=CLAN_EC(cl).last_tax_income;
	
  /*view clan economy*/
	sprintf(buf,
		"  &1&bREGNO&0 DI &4&b%s&0\n"
		"&1------------------------&0\n"
		"&4&bTESORO&0   : &1&b%d&0 [&3&b%d&0]\n"
		"&4&bUSCITE  M&0: &1&b%d&0 [&3&b%d&0]\n"
		"&4&bENTRATE M&0: &1&b%d&0 [&3&b%d&0]\n"
		"&4&bDOGANA &0  : &1&b%d&0 [&3&b%d&0]\n"
		"&4&bZECCA  &0  : &1&b%d&0 [&3&b%d&0]\n"
		"&4&bTASSE  &0  : &1&b%d&0 [&3&b%d&0]\n"
		"&4&bSTIPENDI&0 : &1&b%d&0 [&3&b%d&0]\n"
		"&4&bSHOP BAL.&0: &1&b%d&0\n"
		"&4&bEXPORT  &0 : &1&b%d&0 [&3&b%d&0]\n"
		"&1------------------------&0\n"
		"Tra parentesi il guadagno della chiusura precedante.\n",
		clan[cl].name,
		clan[cl].economy.treasure,bal_tesoro,
		clan[cl].economy.current_uscite,bal_uscite,
		clan[cl].economy.current_entrate,bal_entrate,
		clan[cl].economy.current_dogana_income,bal_dogana,
		clan[cl].economy.current_zecca_income,bal_zecca,
		clan[cl].economy.tax_income,bal_tax,
		clan[cl].economy.stipendi,bal_stipendi,
		clan[cl].economy.current_shops_balance,
		clan[cl].economy.corrent_export_income,bal_export
	);
	send_to_char(buf,ch);
	sprintf(buf,"Alla chiusura del bilancio mancano: %d mesi %d giorni %d ore (tempo mud)",
		(time_info.month >= 8) ? (15 - time_info.month): (7 - time_info.month),
		(34 - time_info.day),
		(23 - time_info.hours));
	send_to_char(buf,ch);
}

void do_clan_mob(struct char_data *ch,char *arg){
	char arg1[64];
	char arg2[64];
	int  cl,i,j,vnum,rnum;
	bool found=FALSE;
	struct char_data *mob=NULL;
	if(GET_LEVEL(ch)<LVL_GOD){
		send_to_char("Huh?\n\r",ch);
		return;
	}
	if(!*arg){
		send_to_char("Devi specificare il nome di un clan e un vnum di mob.",ch);
		return;
	}
	two_arguments(arg,arg1,arg2);
	if((cl=find_clan(arg1))<0){
		send_to_char("Devi specificare il nome di un clan e un vnum di mob o 'reset'.",ch);
		return;
	}
	if(!*arg2){
		sprintf(buf,
			"    \n&4&bMOB CHE IL REGNO DI &1%s&4 PUO ASSUMERE.&0\r\n"
			"&1------------------------------------------------&0\n"
			,clan[cl].name);
		send_to_char(buf,ch);
		for(i=0,vnum=0;i<MAX_MOB_ASSOLDABILI;i++){
			if((vnum=clan[cl].mob_assoldabili[i])>0&&(j=real_mobile(vnum)>0)){
				mob=read_mobile(vnum,VIRTUAL);
				if(mob==NULL)
					return;
				char_to_room(mob,0);
				sprintf(buf,"&4&b[&0%d&4&b]&0 %s &4&bVNUM:&0%d &4&bLIV:&0%d &4&bFEE:&0%d\n",i,GET_NAME(mob),vnum,GET_LEVEL(mob),FEE_PER_LEVEL*GET_LEVEL(mob));
				
				found=TRUE;
				extract_char(mob);
				send_to_char(buf,ch);
			}
		}
		if(found==FALSE){
			send_to_char("&1&bNESSUNO.&0\r\n",ch);
			return;
		}
	}
	else{
		if(is_number(arg2)){
			if((vnum=atoi(arg2))<=0||(rnum=real_mobile(vnum))<0){
				send_to_char("Non e' un numero_valido.",ch);
				return;
			}
			mob=read_mobile(vnum,VIRTUAL);
			
			if(mob==NULL)
				return;
			char_to_room(mob,0);
			if(!MOB_FLAGGED(mob,MOB_SAVE)){
				send_to_char("Non e' un mob salvabile,inutile assegnarlo!",ch);
				extract_char(mob);
				return;
			}
			i=0;
			while(i<MAX_MOB_ASSOLDABILI&&clan[cl].mob_assoldabili[i]>0)
				i++;
			if(i>=MAX_MOB_ASSOLDABILI){
				send_to_char("ESAURITI GLI SLOT ;BISOGNA ALLUNGARE L'ARRAY",ch);
				extract_char(mob);
				return;
			}
			clan[cl].mob_assoldabili[i]=vnum;
			send_to_char("Okay.\r\n",ch);
			extract_char(mob);
			save_clans();
		}
		else{
			if(!str_cmp(arg2,"reset")){
				send_to_char("Il secondo argomento deve essere un vnum di un mob o 'reset'\r\n",ch);
				return;
			}
			for(i=0;i<MAX_MOB_ASSOLDABILI;i++)
				clan[cl].mob_assoldabili[i]=-1;
			send_to_char("Okay.\r\n",ch);
		}
	}
	save_clans();
}

void do_clan_assumi(struct char_data *ch,char *arg){
	int cl,vnum,rnum,i,j,fee,idclan;
	struct char_data *mob=NULL;
	bool found=FALSE;
	if((idclan=GET_CLAN(ch))<=0||!IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_FEES)){
		send_to_char("Non sei in grado di assumere Nessuno!",ch);
		return;
	}
	cl=find_clan_by_id(idclan);
	if(!*arg){
		sprintf(buf,
			"\n  &1MOB CHE IL REGNO DI&4&b%s&0 &1PUO ASSUMERE&0\r\n"
			"&1--------------------------------------------------&0\r1\n"
			,clan[cl].name);
		send_to_char(buf,ch);
		for(i=0,vnum=0;i<MAX_MOB_ASSOLDABILI;i++){
			if((vnum=clan[cl].mob_assoldabili[i])>0&&(j=real_mobile(vnum)>0)){
				mob=read_mobile(vnum,VIRTUAL);
				if(mob==NULL)
					return;
				char_to_room(mob,0);
				sprintf(buf,"&4&b[&0%d&4&b]&0 %s &4&bVNUM:&4&b%d &4&bLIV:&0%d &4&bFEE:&0%d\n",i,GET_NAME(mob),vnum,GET_LEVEL(mob),FEE_PER_LEVEL*GET_LEVEL(mob));
				found=TRUE;
				extract_char(mob);
				send_to_char(buf,ch);
			}
		}
		if(found==FALSE)
			send_to_char("NESSUNO.\r\n",ch);
		return;
	}
	if(!is_number(arg)||(i=atoi(arg))<0||i>MAX_MOB_ASSOLDABILI){
		send_to_char("L'argomento dev'essere un numero compreso tra 0 e 30",ch);
		return;
	}
	if((vnum=clan[cl].mob_assoldabili[i])<=0){
		send_to_char("Il regno non ha a disposizione questo mob",ch);
		return;
	}
	if((rnum=real_mobile(vnum))<0){
		send_to_char("Errore!! Questo mob non esiste.Contattare un creatore.",ch);
		return;
	}
	mob = read_mobile(rnum, REAL);
	if(mob==NULL)
		return;
	char_to_room(mob,0);
	CLAN_ID(mob) =idclan;
	fee=GET_LEVEL(mob)*FEE_PER_LEVEL;
	if(clan[cl].economy.treasure<fee){
		send_to_char("IL REGNO NON PUO PERMETTERSI DI STIPENDIARLO!",ch);
		extract_char(mob);
		return;
	}
	CLAN_EC(cl).treasure-=fee;
	CLAN_EC(cl).stipendi+=fee;
	CLAN_EC(cl).current_uscite+=fee;
	GET_STIPENDIO(mob).has_fee=TRUE;
	GET_STIPENDIO(mob).idclan=idclan;
	GET_STIPENDIO(mob).clan_fee=fee;
	Save_mob();
	sprintf(buf, "\nHai assunto &4&b%s&0 [PAGA: &4&b%d&0] \r\n", GET_NAME(mob),fee);
	send_to_char(buf, ch);
	char_to_room(mob, ch->in_room);
	save_clans();
}

void do_clan_licenzia(struct char_data*ch,char *arg){
	int cl=-1,vnum,idclan;
	struct char_data *mob=NULL;
	//bool found=FALSE;
	if((idclan=GET_CLAN(ch))<=0||!IS_SET(GET_CLAN_PRIV(ch),CP_GIVE_FEES)){
		send_to_char("Non sei in grado di assumere Nessuno!",ch);
		return;
	}
	cl=find_clan_by_id(idclan);
	if(!*arg){
		sprintf(buf,
			"\n           NPG DEL CLAN  &4&b%s&0                     \n"
			"&1--------------------------------------------&0\n"
			" ID           NOME         LIV.          STIP.  \n"
			,clan[cl].name);
		send_to_char(buf,ch);
		
		for (mob = character_list; mob; mob = mob->next){
			if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&
				GET_STIPENDIO(mob).has_fee==TRUE){
				if(CLAN_ID(mob)==idclan&&GET_STIPENDIO(mob).clan_fee>0){
					sprintf(buf,"[%d] %s        [%d]       [%d]  [%d] \n"
						,GET_MOB_VNUM(mob),GET_NAME(mob),GET_LEVEL(mob),GET_STIPENDIO(mob).clan_fee,GET_LEVEL(mob)*FEE_PER_LEVEL);
					send_to_char(buf,ch);
				}
			}
		}
		send_to_char("&1--------------------------------------------&0\n",ch);
		return;
	}
	else{
		if(!is_number(arg)||(vnum=atoi(arg))<=0){
			send_to_char("Devi specificare il vnum di un mob della lista",ch);
			return;
		}
		for (mob = character_list;mob; mob = mob->next){
			if(IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE)&&
				( CLAN_ID(mob) ==idclan)&&(GET_MOB_VNUM(mob)==vnum)){
				extract_char(mob);
				send_to_char("Fatto. Hai ora un dipendente in meno.\n\r",ch);
				return;
			}
		}
		send_to_char("Non esiste nessun tuo mob con quel codice.\n\r",ch);
		return;
	}
}

void display_bounds(struct char_data *ch,int cl){
	int i,aut_class;
	sprintf(buf,"&4&bCLASSI AUTOMATICHE   TASSE      LIMITE&0\r\n");
	strcat(buf,"&1--------------------------------------&0\r\n");
	if(cl<0||cl>=num_of_clans)
		return;
	aut_class=clan[cl].automatic_ranks;
	for(i=1;i<aut_class;i++){
		sprintf(buf2 ,"[&4&b%2d&0][&1%-12s&0]: [&3&b%-7d&0&1-&0&4&b%7d&0]\r\n",i,clan[cl].rank_name[i],clan[cl].rank_tax[i],clan[cl].rank_bounds[i]);
		strcat(buf,buf2);
	}
	strcat(buf,"&1--------------------------------------&0\r\n");
	send_to_char(buf,ch);
	return;
}

void do_clan_set_limits(struct char_data *ch,char *arg){
	char arg1[20],arg2[20],arg3[20];
	bool imcom=(GET_LEVEL(ch)>=LVL_GOD ? TRUE :FALSE);
	int cl,bound,rank;

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(imcom){
		if(!*arg){
			send_to_char("Specifica il nome di un regno.\r\n",ch);
			return;
		}
		two_arguments(arg,arg3,arg1);
		if((cl=find_clan(arg3))==-1){
			send_to_char("Non esiste nessun Regno con questo nome\r\n",ch);
			return;
		}
		two_arguments(arg1,arg1,arg2);
	}

	if(!imcom){
		if((cl=find_clan_by_id(GET_CLAN(ch)))==-1){
			send_to_char("Non appartieni a nessun Regno\r\n",ch);
			return;
		}
		if(!IS_SET(GET_CLAN_PRIV(ch),CP_SET_TAX)){
			send_to_char("Sono cose piu grandi di te, fila via!\r\n",ch);
			return;
		}
		two_arguments(arg,arg1,arg2);
	}
	if(!*arg1||!is_number(arg1)||(rank=atoi(arg1))<=0||rank>clan[cl].automatic_ranks){
		display_bounds(ch,cl);
		return;
	}
	if(!*arg2||!is_number(arg2)||(bound=atoi(arg2))<=0){
		sprintf(buf,"[%-2d] [%-12s] :[%d]\r\n\r\n",rank,clan[cl].rank_name[rank],clan[cl].rank_bounds[rank]);
		send_to_char(buf,ch);
		return;
	}
	sprintf(buf,"Limite economico per lo stato %s cambiato da %d a %d\r\n",clan[cl].rank_name[rank],clan[cl].rank_bounds[rank],bound);
	clan[cl].rank_bounds[rank]=bound;
	save_clans();
	send_to_char(buf,ch);
}


void perform_shop_coins(struct char_data * ch, char *coins)
{
	int shop_nr;
	int cl,shopcoins;
	struct char_data *vict;

    if(GET_CLAN(ch)<=0)
	  return;
	
	if ((!*coins) || (!is_number(coins)))
	{
		send_to_char("Quanti soldi voi dare ad ogni negozio?\r\n"
			"Uso: regno saldonegozi 1000"
			,ch);
		
		return;
	}
	shopcoins = atoi(coins);
	for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	{
		if ((GET_CLAN(ch)!=-1) && (SHOP_CLAN(shop_nr) == GET_CLAN(ch)))
		{
			if ((shop_nr < top_shop) && (SHOP_KEEPER(shop_nr)!=-1))
			{
				vict = get_char_num(SHOP_KEEPER(shop_nr));
				if (vict && MOB_FLAGGED(vict,MOB_SHOPKEEPER))  {
					cl = find_clan_by_id(GET_CLAN(ch));
					clan[cl].economy.current_shops_balance += (GET_GOLD(vict) + SHOP_BANK(shop_nr));
					clan[cl].economy.treasure += (GET_GOLD(vict) + SHOP_BANK(shop_nr));
					GET_GOLD(vict) = 0;
					if (clan[cl].economy.treasure < shopcoins){
						if (clan[cl].economy.treasure<0) shopcoins = 0;
						else shopcoins = clan[cl].economy.treasure;
					}
					sprintf(buf,"Rifornisci il negozio di %s con %d coins\r\n",
						GET_NAME(&mob_proto[SHOP_KEEPER(shop_nr)]),
						shopcoins);
					SHOP_BANK(shop_nr) =  shopcoins;
					clan[cl].economy.current_shops_balance -= shopcoins;
					clan[cl].economy.treasure -= shopcoins;
					clan[cl].economy.current_uscite += shopcoins;
					send_to_char(buf,ch);
				}
			}
		}
	} //for
}



void perform_condanna(struct char_data *ch, char *arg1)
{
	int clan_num=-1,immcom=0;
	char arg2[255];
	char arg3[255];
	char arg4[255];
	char arg5[255];
	char arg6[255];
	char arg7[255];
	struct char_data *vict = NULL;
	long flag;
	extern void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia);

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun Regno!\r\n",ch);
			return;
		}
	}
	else
		immcom=1;
	
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_GIUDICE) && immcom==0){
		send_to_char("Sono cose piu grandi di te, fila via!\r\n",ch);
		return;
	}
	
	arg1 = one_argument(arg1,arg2);
	
	if (!strcmp(arg2,"")) {    
		fedina_penale(ch,ch,clan_num);
		return;
	}
	else
	{
		if(!(vict = get_player_vis(ch, arg2, 0))) {
			send_to_char("Non c'e' nessuno con questo nome.\r\n",ch);
			return;
		}
	}
	
	if ((GET_LEVEL(vict)>LVL_IMMORT) && (GET_LEVEL(ch)<LVL_IMMORT))
	{
		send_to_char("Io non mi permetterei se fossi in te.\r\n",ch);
		return;
	}
	
	arg1 = one_argument(arg1,arg3);
	
	if ((flag = get_reward_flag_id(arg3)) < 0){
		send_to_char("Non e' un flag accettabile.\r\n",ch);
		send_to_char("Usa: attentato, tradimento, cospirazione, omicidio, furto,\r\n",ch );
		send_to_char("     strage, corruzione, evasione.\r\n",ch);
		return;
	}
	
	arg1 = one_argument(arg1,arg4);
	
	if (strcmp(arg4,"vivo") && strcmp(arg4,"morto")){
		send_to_char("Devi indicare se lo vuoi vivo o morto!\r\n",ch);
		return;
	}
	
	arg1 = one_argument(arg1,arg7);
	if (!is_number(arg7)){
		send_to_char("Devi mettere una taglia o il valore della multa!\r\n",ch);
		return;
	}
	
	if (immcom) {
		half_chop(arg1,arg6,arg5);
		
	//arg1 = one_argument(arg1,arg6);
		if (find_clan(arg6)!=-1)
			clan_num=find_clan(arg6);
		else {
			send_to_char("Il regno non esiste!\r\n",ch);
			return;
		}
		
		if (!strcmp(arg5,"")) {
			send_to_char("Descrivi la condanna. Es Omicidio di .....\r\n",ch);
			return;
		}
		
	} else {
		if (!strcmp(arg1,"")) {
			send_to_char("Descrivi la condanna. Es Omicidio di .....\r\n",ch);
			return;
		}
		strcpy(arg5,arg1);
	}
	
	set_crime(vict, NULL, arg4 , arg5,clan_num, flag, atoi(arg7));
	sprintf(buf,"Sei stato condannato per %s , sei ora ricercato %s nel regno di %s\r\n", get_reward_flag_name(flag, FALSE), arg4 , clan[clan_num].name );
	send_to_char(buf,vict);
	sprintf(buf,"%s e' stato condannato per %s, ora e' ricercato %s nel regno di %s\r\n",GET_NAME(vict), get_reward_flag_name(flag, FALSE), arg4 , clan[clan_num].name );
	send_to_char(buf,ch);
	sprintf(buf,"(REGNI) %s ha condannato %s nel regno %s per %s",GET_NAME(ch),GET_NAME(vict),clan[clan_num].name,get_reward_flag_name(flag, FALSE));
	mudlog(buf,NRM,LVL_GOD,TRUE);
}

void perform_assolvi(struct char_data * ch, char *arg1)
{
	int clan_num = -1, num=0, immcom = 0;
	char arg2[255];
	char arg3[255];
	char arg4[255];
	struct char_data *vict = NULL;
	extern int del_crime(struct char_data *vict, int num);
	extern struct reward *find_crime_by_num(struct char_data *vict, int num);	
	struct reward *rew;	
	int flag = 0;

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun Regno!\r\n",ch);
			return;
		}
	}
	else
		immcom=1;
	
	if(!IS_SET(GET_CLAN_PRIV(ch),CP_GIUDICE) && immcom==0){
		send_to_char("Sono cose piu grandi di te, fila via non sei un giudice!\r\n",ch);
		return;
	}
	
	arg1 = one_argument(arg1,arg2);
	
	if (!strcmp(arg2,"")) {
		fedina_penale(ch,ch,clan_num);
		return;
	}
	else
	{
		if(!(vict = get_player_vis(ch, arg2, 0))) {
			send_to_char("Non c'e' nessuno con questo nome.\r\n",ch);
			return;
		}
	}
	
	if ((GET_LEVEL(vict)>LVL_IMMORT) && (GET_LEVEL(ch)<LVL_IMMORT))
	{
		send_to_char("Io non mi permetterei se fossi in te.\r\n",ch);
		return;
	}
	
	arg1 = one_argument(arg1,arg3);
	if (is_number(arg3)) num = atoi(arg3);
	else {
		send_to_char("Devi inserire il numero della condanna che vuoi togliere.\r\n",ch);
		return;
	}
	
	if (immcom==1) {
		arg1 = one_argument(arg1,arg4);
		//PEPPE REGNO ASSOLVI

		if(str_cmp(arg4,"nessuno"))

			clan_num=0;

		else if (find_clan(arg4)>=0)
			clan_num=find_clan(arg4);

		else {
			send_to_char("Il regno non esiste!\r\n",ch);
			return;
		}
	}
	
	if ((rew = find_crime_by_num(vict, num)))
	{
		if ((rew->regno) != clan_num)
		{
			send_to_char("Non puoi assolverlo, non e' una condanna emanata dal tuo clan!\r\n",ch);
			return;
		}
	} else {
		send_to_char("Nessun crimine corrisponde al numero indicato!\r\n",ch);
		return;
	}
	
	flag = del_crime(vict,num);
	if (flag!=-1) {
		sprintf(buf,"Sei stato assolto dall'accusa di %s , non sei piu' ricercato nel regno di %s\r\n", get_reward_flag_name(flag, FALSE) , clan[clan_num].name);
		send_to_char(buf,vict);
		sprintf(buf,"%s e' stato assolto dall'accusa di %s, ora non e' piu' ricercato nel regno di %s\r\n",GET_NAME(vict), get_reward_flag_name(flag, FALSE) , clan[clan_num].name);
		send_to_char(buf,ch);
		sprintf(buf,"(REGNI) %s ha assolto %s nel regno %s per %s",GET_NAME(ch),GET_NAME(vict),clan[clan_num].name,get_reward_flag_name(flag,TRUE));
		mudlog(buf,NRM,LVL_GOD,TRUE);
	}	
	
}

void regno_ricercati(struct char_data * ch, char *argument)
{
	int clan_num;
	char arg1[255];
	
    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if(GET_LEVEL(ch)<LVL_IMMORT) {
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0) {
			send_to_char("Non appartieni a nessun Regno!\r\n",ch);
			return;
		}
	}
	else
	{
		one_argument(argument,arg1);
		if (find_clan(arg1)>=0)
			clan_num=find_clan(arg1);
		else {
			send_to_char("Il regno non esiste!\r\n",ch);
			return;
		}
	}
	
	fedina_penale(ch,ch,clan_num);
	return;
	
}


char *get_reward_flag_name(int flag,int modo)
{
	if (modo==TRUE) {
		if (flag == CLAN_THIEF)
			return  str_dup("Ladro");
		
		if (flag == CLAN_TKILLER)
			return  strdup("Attentatore");
		
		if (flag == CLAN_WKILLER)
			return  strdup("Traditore");
		
		if (flag == CLAN_WANTED)
			return  strdup("Cospiratore");
		
		if (flag == CLAN_KILLER)
			return  strdup("Assassino");
		
		if (flag == CLAN_STRAGE)
			return  strdup("Stragista");
		
		if (flag == CLAN_CORROTTO)
			return  strdup("Corruttore");
		
		if (flag == CLAN_MULTA)
			return  strdup("Evasore");
	} else {
		if (flag == CLAN_THIEF)
			return  str_dup("Furto");
		
		if (flag == CLAN_TKILLER)
			return  strdup("Attentato");
		
		if (flag == CLAN_WKILLER)
			return  strdup("Tradimento");
		
		if (flag == CLAN_WANTED)
			return  strdup("Cospirazione");
		
		if (flag == CLAN_KILLER)
			return  strdup("Omicidio");
		
		if (flag == CLAN_STRAGE)
			return  strdup("Strage");
		
		if (flag == CLAN_CORROTTO)
			return  strdup("Corruzione");
		
		if (flag == CLAN_MULTA)
			return  strdup("Evasione");
	}		
	
	return strdup("Indefinito");
}


int get_reward_flag_id(char * flag)
{
	
	if (!strcmp(flag,"attentato"))
		return CLAN_TKILLER;
	
	if (!strcmp(flag,"tradimento"))
		return CLAN_WKILLER;
	
	if (!strcmp(flag,"cospirazione"))
		return CLAN_WANTED;
	
	if (!strcmp(flag,"omicidio"))
		return CLAN_KILLER;
	
	if (!strcmp(flag,"furto"))
		return CLAN_THIEF;
	
	if (!strcmp(flag,"strage"))
		return CLAN_STRAGE;
	
	if (!strcmp(flag,"corruzione"))
		return CLAN_CORROTTO;
	
	if (!strcmp(flag,"evasione"))
		return CLAN_MULTA;
	
	return -1;
}


void fedina_penale(struct char_data * ch, struct char_data * vict,int regno)
{
	struct reward *rew;
	struct char_data *tvict;
	int i=0;
	extern struct char_data *get_ch_by_id(long idnum);
	extern int crime_reward(struct char_data *ch, int clan);
	
	if (!ch || !vict) return;
	
	if (regno >= 0) {
		send_to_char("-----------------&5LISTA DEI RICERCATI DAL REGNO&0--------------------\r\n",ch);	
		for (tvict = character_list; tvict; tvict = tvict->next) {
			if (AFF_FLAGGED(tvict, AFF_DISGUISE) && (tvict->player.namedisguise))
			  continue;
			i = 0;
			for (rew = tvict->ch_reward; rew; rew = rew->next) {
				if (rew->regno == regno) {
					i++;
					sprintf(buf,"Reato %3d PG: &7%-15s&0 Stato: &1%-12s&0 Da ricercarsi: &4%s&0 Taglia: %ld coins \r\n Taglia Secondino: %d     Accusa: %s\r\n",i, GET_NAME(tvict),get_reward_flag_name(rew->flag,TRUE) ,(rew->vivo_morto == 0) ? "vivo " : "vivo o morto",rew->taglia,crime_reward(tvict,rew->regno), rew->descr);
					send_to_char(buf,ch);
				}
			}
		}	
		return;	  
	} //CLAN  
	
	if (ch == vict) {
		send_to_char("-----------------&5LA TUA FEDINA PENALE ATTUALE&0--------------------\r\n",ch);
		for (rew = ch->ch_reward; rew; rew = rew->next) {
			sprintf(buf,"REGNO: &3%-15s&0 Stato: &1%-12s&0 Da ricercarsi: &4%s&0 Taglia: %ld coins \r\n Taglia Secondino: %d     Accusa: %s\r\n",clan[rew->regno].name,get_reward_flag_name(rew->flag, TRUE) ,(rew->vivo_morto == 0) ? "vivo " : "vivo o morto",rew->taglia, crime_reward(ch,rew->regno) ,rew->descr);
			send_to_char(buf,ch);
		}	
		return;	
	} else {
		sprintf(buf,"-----------------&5LA FEDINA PENALE DI %s&0--------------------\r\n",GET_NAME(vict));
		send_to_char(buf,ch);	      
		for (rew = vict->ch_reward; rew; rew = rew->next) {
			sprintf(buf,"REGNO: &3%-15s&0 Stato: &1%-12s&0 Da ricercarsi: &4%s&0 Taglia: %ld coins \r\n Taglia Secondino: %d     Accusa: %s\r\n",clan[rew->regno].name,get_reward_flag_name(rew->flag, TRUE) ,(rew->vivo_morto == 0) ? "vivo " : "vivo o morto",rew->taglia,crime_reward(vict,rew->regno), rew->descr);
			send_to_char(buf,ch);
		}
	}//else	
	
	return;
}

void do_clan_secondino1 (struct char_data *ch, char *arg)
{
	struct char_data *vict=NULL;
	int clan_num,fuga,immcom=0;
	long cella;
	int regnoid1 = 0;
	
	
	if (!(*arg))
	{
		send_clan_format(ch);
		return;
	}

    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;

	if (GET_LEVEL(ch)<LVL_IMMORT)
	{
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0)
		{
			send_to_char("Non appartieni a nessun regno!\r\n",ch);
			return;
		}
	}
	else
	{
		if(GET_LEVEL(ch)<LVL_CLAN_GOD)
		{
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return;
		}
	}
	
	if (!IS_SET(GET_CLAN_PRIV(ch),CP_SECONDINO) && !immcom)
	{
		send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	if (!(vict=get_char_room_vis(ch,arg)))
	{
		send_to_char("La persona deve essere di fronte a te!\r\n",ch);
		return;
	}
	if (!ROOM_FLAGGED(ch->in_room, ROOM_PRISON))
	{
		send_to_char("Puoi fare questo solo in una cella!\r\n",ch);
		return;
	}
	
	else
	{{
			if ((cella = world[ch->in_room].number) == 6813)
			{
				fuga = 6812;
				regnoid1 = 8;
				
			}
			else
			if ((cella = world[ch->in_room].number) == 8578)
			{
				fuga = 8579;
				regnoid1 = 3;
				
			}
			else
			if ((cella = world[ch->in_room].number) == 9836)
			{
				fuga = 9835;
				regnoid1 = 3;
				
			}
			else
			if ((cella = world[ch->in_room].number) == 12542)
			{
				fuga = 12680;
				regnoid1 = 5;
				
			}
			else
			if ((cella = world[ch->in_room].number) == 21261)
			{
				fuga = 21260;
				regnoid1 = 2;
				
			}
			else
			if ((cella = world[ch->in_room].number) == 50137) // Aldeeran
			{
				fuga = 50135;
				regnoid1 = 9;
			}
			else
				fuga = 0;
		}}
	
	
	
	if ((GET_CLAN(ch) == regnoid1) && (fuga != 0))
	{
		//send_to_room("&1&bUn robusto guardiano arriva e porta fuori il detenuto.&0\r\n", ch->in_room);
		act("&1&bUn robusto guardiano arriva e porta fuori $n.&0",TRUE,vict,0,0,TO_NOTVICT);
		char_from_room(vict);
		char_to_room(vict, real_room(fuga));
		look_at_room(vict, 0);
		save_char(vict, vict->in_room);
		send_to_char("&2&bVieni portato fuori dalla cella da un grosso guardiano!&0\r\n",vict);
		return;
	}
	else
	{
		send_to_char("&1&bNon sei nelle celle del tuo regno.&0\r\n", ch);
		return;
	}
}
void do_clan_secondino2 (struct char_data *ch)
{
	
	int clan_num,fuga,immcom=0;
	long cella;
	int regnoid1 = 0;
	
    if(GET_CLAN(ch)<=0 && GET_LEVEL(ch)<LVL_IMMORT)
	  return;
	
	if (GET_LEVEL(ch)<LVL_IMMORT)
	{
		if((clan_num=find_clan_by_id(GET_CLAN(ch)))<0)
		{
			send_to_char("Non appartieni a nessun regno!\r\n",ch);
			return;
		}
	}
	else
	{
		if(GET_LEVEL(ch)<LVL_CLAN_GOD)
		{
			send_to_char("Non hai i privilegi necessari.\r\n", ch);
			return;
		}
	}
	
	if (!IS_SET(GET_CLAN_PRIV(ch),CP_SECONDINO) && !immcom)
	{
		send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
		return;
	}
	
	if (!ROOM_FLAGGED(ch->in_room, ROOM_PRISON))
	{
		send_to_char("Puoi fare questo solo in una cella!\r\n",ch);
		return;
	}
	
	else
	{
		if ((cella = world[ch->in_room].number) == 6813)
		{
			fuga = 6812;
			regnoid1 = 8;
			
		}
		else
		if ((cella = world[ch->in_room].number) == 8578)
		{
			fuga = 8579;
			regnoid1 = 3;
			
		}
		else
		if ((cella = world[ch->in_room].number) == 9836)
		{
			fuga = 9835;
			regnoid1 = 3;
			
		}
		else
		if ((cella = world[ch->in_room].number) == 12542)
		{
			fuga = 12680;
			regnoid1 = 5;
			
		}
		else
		if ((cella = world[ch->in_room].number) == 21261)
		{
			fuga = 21260;
			regnoid1 = 2;
			
		}
		else
		if ((cella = world[ch->in_room].number) == 50137) // Aldeeran
		{
			fuga = 50135;
			regnoid1 = 9;
		}
		else
			fuga = 0;
	}
	
	if ((GET_CLAN(ch) == regnoid1) && (fuga != 0)) {
		
		//send_to_room("&1&bUn robusto guardiano arriva e scorta fuori il funzionario.&0\r\n", ch->in_room);
		act("&1&bUn robusto guardiano arriva e scorta $n fuori dalla cella.&0",TRUE,ch,0,0,TO_NOTVICT);
		char_from_room(ch);
		char_to_room(ch, real_room(fuga));
		look_at_room(ch, 0);
		save_char(ch, ch->in_room);
		send_to_char("&2&bVieni scortato fuori dalla cella da un grosso guardiano!&0\r\n",ch);
		return;
	}
	else {
		send_to_char("&1&bNon sei nelle celle del tuo regno.&0\r\n", ch);
		return;
	}
	
}





ACMD(do_rinomina)
{
    
  send_to_char("Usa il comando 'rinomina_name <nomeobj> <descrizione>' dove al posto di <nomeobj>\r\n",ch);
  send_to_char("metterai il nome dell'oggetto da modificare e al posto di <descrizione> la descrizione breve dell'oggetto in questione\r\n",ch);
  send_to_char("Per aggiungere la descrizione che si vede quando il pg impugna o indossa l'oggetto (es. Una spada di magnifica fattura).\r\n\r\n",ch);
    
  send_to_char("Usa il comando 'rinomina_descr <nomeobj> <descrizione>' dove al posto di <nomeobj>\r\n",ch);
  send_to_char("metterai il nome dell'oggetto da modificare e al posto di <descrizione> la stringa che vuoi vedere quando trovi l'oggetto a terra\r\n",ch);
  send_to_char("Per aggiungere la descrizione che si vede quando il pg passa nella locazione dove e' stato dimenticato o lasciato l'oggetto\r\n(es. Una bellissima spada e' stata abbandonata qui.).\r\n\r\n",ch);
    
  send_to_char("potrai usare anche i colori all'interno delle tue descrizioni.\r\n",ch);
  
  return;
}


ACMD(do_rinomina_alias)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], buffer[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	
	half_chop(argument, arg1, arg2);
	
	if (!*arg1) {
	    send_to_char("uso: rinomina_alias <nomeobj> <nuovoalias> \r\n",ch);
	    return;
	} 
	
	if (!(obj = get_obj_in_list_vis(ch,arg1, ch->carrying))) {
	    send_to_char("Se non hai questo oggetto come fai a rinominarlo?!?!?\r\n",ch);
	    return ;
	}
	
	if (!IS_OBJ_STAT(obj, ITEM_RINOM_ALIAS)) {
	    send_to_char("Non puoi rinominare questo oggetto.\r\n",ch); 
	    return;
	}
	   
	if (!*arg2) {
	    send_to_char("devi specificare che alias vuoi inserire.\r\n",ch);
	    return;
	}
	
	if (strlen(arg2) > 21) {
	    send_to_char("L'alias che vuoi inserire e' troppo lungo.\r\n",ch);
	    return;
	}
	
	//nel caso crashasse sostituire le 2 linee sottostanti con la linea commentata
	//=strcat(arg2," ");
	//sprintf(buffer1,"%s","_CHECK ");

	//buffer1=strcat(buffer1,obj->name);

	sprintf(buffer, "%s", arg2);
	
	//buffer1=strcat(buffer1,buffer);
	//buffer1=strcat(buffer1,"&0\0");
	obj->name = strcat(obj->name, buffer);
	obj->name = strcat(obj->name, " _CHECK ");
	sprintf(buffer, "%s",obj->name);

	obj->name = str_dup(buffer);

	

	REMOVE_BIT(GET_OBJ_EXTRA(obj), ITEM_RINOM_ALIAS);
	send_to_char("Alias Aggiunto.\r\n",ch);
	return;	
}    
	    



ACMD(do_rinomina_name)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], buffer[MAX_INPUT_LENGTH];
	struct obj_data *obj;
	
	half_chop(argument,arg1,arg2);
	
	if (!*arg1) {
	    send_to_char("uso: rinomina_name <nomeobj> <nuovadescrizione> \r\n",ch);
	    return;
	} 
	
	if (!(obj=get_obj_in_list_vis(ch,arg1,ch->carrying))){
	    send_to_char("Se non hai questo oggetto come fai a rinominarlo?!?!?\r\n",ch);
	    return ;
	}
	
	if (!IS_OBJ_STAT(obj, ITEM_RINOM_NAME)){
	    send_to_char("Non puoi rinominare questo oggetto.\r\n",ch); 
	    return;
	}   
	
	if (!*arg2) {
	    send_to_char("devi specificare la descrizione che vuoi inserire.\r\n",ch);
	    return;
	}
	
	if (strlen(arg2) > 46) {
	    send_to_char("La descrizione che vuoi inserire e' troppo lunga.\r\n",ch);
	    return;
	}
	
	sprintf(buffer, "%s", arg2);

  sprintf(buf,"SYSINF: RENAME %s to %s ", obj->short_description, buffer);
	mudlog(buf, NRM, LVL_IMMORT, FALSE);

	obj->short_description = str_dup(buffer);

	REMOVE_BIT(GET_OBJ_EXTRA(obj), ITEM_RINOM_NAME);
	send_to_char("Nuovo nome Aggiornato.\r\n",ch);
	
	return;	
}    


ACMD(do_rinomina_descr)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], buffer[MAX_INPUT_LENGTH];
	struct obj_data *obj;

	half_chop(argument, arg1, arg2);
	
	if (!*arg1) {
	    send_to_char("uso: rinomina_descr <nomeobj> <nuovadescrizione> \r\n",ch);
	    return;
	} 
	
	if (!(obj=get_obj_in_list_vis(ch,arg1,ch->carrying))) {
	    send_to_char("Se non hai questo oggetto come fai a rinominarlo?!?!?\r\n",ch);
	    return ;
	}
	
	if (!IS_OBJ_STAT(obj,ITEM_RINOM_DESCR)) {
	    send_to_char("Non puoi rinominare questo oggetto.\r\n",ch); 
	    return;
	}   
	
	if (!*arg2){
	    send_to_char("devi specificare la descrizione che vuoi inserire.\r\n",ch);
	    return;
	}
	
	if (strlen(arg2) > 36) {
	    send_to_char("La descrizione che vuoi inserire e' troppo lunga.\r\n",ch);
	    return;
	}

	sprintf(buffer, "%s", arg2);

  sprintf(buf,"SYSINF: REDESC %s to %s ", obj->description, buffer);
	mudlog(buf, NRM, LVL_IMMORT, FALSE);

	obj->description = str_dup(buffer);

	REMOVE_BIT(GET_OBJ_EXTRA(obj), ITEM_RINOM_DESCR);
	send_to_char("Nuova Descrizione inserita.\r\n",ch);
	return;	

}


void arruola_truppe(void){
	int i,nuove_truppe;
	for(i=0;i<num_of_clans;i++){
		nuove_truppe=number(0,2);
		nuove_truppe=nuove_truppe*10;
		clan[i].truppe_disponibili=clan[i].truppe_disponibili+nuove_truppe;
		if(clan[i].truppe_disponibili%2==0)
			clan[i].truppe_disponibili++;
		sprintf(buf,"%d nuove truppe per %s",nuove_truppe/10,frase_regno(i,0));
		mudlog(buf, NRM, LVL_IMMORT, FALSE);
	}
}
