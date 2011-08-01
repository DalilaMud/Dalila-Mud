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
#include "clan2.h"
#include "shop.h"

int new_num_clan;
int new_num_of_clans;
struct new_clan_rec new_clan[NEW_MAX_CLANS];
extern int get_no_play(struct char_data *ch,struct char_file_u chdata,int time_out,int inplay);
extern void save_char_file_u(struct char_file_u st);
extern struct descriptor_data *descriptor_list;
extern struct char_data *is_playing(char *vict_name);
extern int top_of_p_table;
extern struct player_index_element *player_table;
int real_zone(room_vnum number);
void sedit_save_to_disk(int zone);


void new_show_status_regno(struct char_data *ch);
void do_new_clan_set_status(struct char_data *ch,char *arg);
void do_new_clan_set_tasse(struct char_data *ch,char *argument);
void do_new_clan_assign_zone(struct char_data *ch,char *argument);
void do_new_clan_economy(struct char_data *ch,char *arg);
void do_new_clan_stipendio(struct char_data *ch,char *arg);
void do_new_clan_licenzia(struct char_data*ch,char *arg);
void do_new_clan_set_limits(struct char_data *ch,char *arg);

int find_pg(char *name,struct char_data *searched,struct char_file_u srchdata);
void do_new_clan_assumi(struct char_data *ch,char *argument);
void do_new_clan_mob(struct char_data *ch,char *argument);
void Save_mob(void);
/*negozi*/
extern int top_shop;
extern struct shop_data *shop_index;
extern struct index_data *mob_index;
extern struct char_data *character_list;
extern struct char_data *mob_proto;
#define NEW_AUTOMATIC_CLASSES 0
#define NEW_FEE_PER_LEVEL 1000
#define NOPAYTIMEOUT 15

char *new_clan_privileges[]=
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
//  "ZECCA & EXPORT",
  "\n"
};


const char *new_tipi_clan[]={
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

const char *new_default_ranks[]={
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

const int new_default_tax[NEW_MAX_RANKS]=
{0,500,1000,2500,5000,12000,23000,0,1000,1000,2500,4000,6000,6000,9000,15000,
 18000,27000,30000,30000,30000,35000,35000,40000,45000,50000,60000,80000,0};


const int new_default_bounds[NEW_MAX_RANKS]=
{0,10000,20000,50000,100000,250000,450000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0};


#define ARTICOLO     0
#define PROPOSIZIONE 1


char * new_frase_regno(int id,int type){
 static char buffer[50];
 int tipo = new_clan[id].type;
 if (type==ARTICOLO){
	if(tipo==NEW_REGNO||tipo==NEW_FEUDO||tipo==NEW_DUCATO||tipo==NEW_VILLAGGIO||tipo==NEW_CLAN)
		sprintf(buffer,"il ");
	 else
		sprintf(buffer,"la ");
  }
 if(type==PROPOSIZIONE){
	 if(tipo==NEW_REGNO||tipo==NEW_FEUDO||tipo==NEW_DUCATO||tipo==NEW_VILLAGGIO||tipo==NEW_CLAN)
		sprintf(buffer,"del ");
	 else
		sprintf(buffer,"della ");
  }
  strcat(buffer,new_tipi_clan[tipo]);
  if(tipo==NEW_REGNO||tipo==NEW_CONTEA||tipo==NEW_DUCATO||tipo==NEW_CITTA)
	 strcat(buffer," di ");
  else
	 strcat(buffer," ");

  if(id==0)
	 strcat(buffer,"&3&b");
  else if(id==2)
	 strcat(buffer,"&4&b");
  else if(id==3)
	 strcat(buffer,"&5");
  else if(id==4)
	 strcat(buffer,"&6");

  strcat(buffer,new_clan[id].name);
  strcat(buffer,"&0");
  return buffer;
}


void send_new_clan_format(struct char_data *ch)
{
int c,r;

send_to_char("Comandi Gilda che puoi usare:\n\r"
				 "   Gilda who  - vedi i membri del clan presenti.\r\n"
				 "   Gilda status\r\n"
				 "   Gilda info <Gilda> - info generali del clan\r\n",ch);
if(GET_LEVEL(ch)>=NEW_LVL_CLAN_GOD)
       send_to_char("   Gilda crea        <leader> <Nome Gilda>\r\n"
					"   Gilda distruggi   <Gilda>\r\n"
					"   Gilda assegna     <Gilda> <vnum zona> \r\n "
		//			"   Gilda negozi                      \r\n"
					"   Gilda arruola     <player> <Gilda>\r\n"
					"   Gilda espelli     <player> <Gilda>\r\n"
					"   Gilda promuovi    <player> <Gilda>\r\n"
					"   Gilda degrada     <player> <Gilda>\r\n"
					"   Gilda preleva     <amount> <Gilda>\r\n"
					"   Gilda deposita    <amount> <Gilda>\r\n"
					"   Gilda set ranks   <rank>   <Gilda>\r\n"
					"   Gilda privilegi   <privilegio> <num rango> <Gilda>\r\n"
					"   Gilda set titoli  <nome Gilda> <num rango> <titolo>\r\n"
					"   Gilda set stato   <nome PG>    <num rango>\r\n"
		  //			"   Gilda set livapp  <liv>\r\n"
					"   Gilda set tasse   <num rango>  <coins>  \r\n"
					"   Gilda stipendi    <nome PG>    <coins>  \r\n"
					"   Gilda diplo       <nome Gilda> \r\n"
					"   Gilda economia    <nome Gilda> \r\n"
					"   Gilda mob         <nome Gilda> <idmob>\r\n"
			 ,ch);
	       else {
		 c=find_new_clan_by_id(NEW_GET_CLAN(ch));
		 r=NEW_GET_CLAN_RANK(ch);
		 if(r<1)
			send_to_char("   Gilda domanda    <Gilda>\r\n",ch);
		 if(c>=0) {
			send_to_char("   Gilda deposita   <coins>\r\n",ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_WITHDRAW))
			send_to_char("   Gilda preleva    <coins>\r\n" ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ENROLL))
			send_to_char("   Gilda arruola    <PG>\r\n" ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_EXPEL))
			send_to_char("   Gilda espelli    <PG>\r\n" ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_PROMOTE))
			send_to_char("   Gilda promuovi   <PG>\r\n",ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_DEMOTE))
			send_to_char("   Gilda degrada    <PG>\r\n",ch);
		//	if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_MNGSHOP))
		//	send_to_char("   Gilda negozi               \r\n",ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_SET_RANK))
			send_to_char(
				  "   Gilda set ranghi  <num ranghi>\r\n"
				  "   Gilda set titoli  <num rango> <titolo>\r\n"
				  ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_PRIV))
			  send_to_char(
				  "   Gilda privilegi   <privilegio> <num rango>\r\n"
				  ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_SET_TAX))
			  send_to_char(
				  "   Gilda set stato   <nome Pg>  <num rango>\r\n"
			//	  "   Gilda set livapp  <liv>\r\n"
				  "   Gilda set tasse   <num rango> <coins>  \r\n"
			//	  "   Gilda limiti      <num rango> <nuovo limit>\r\n"
				  ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_FEES))
			  send_to_char(
				  "   Gilda stipendio  <nome Pg> <amount> \r\n"
				  ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_DIPLO))
			  send_to_char(
				  "   Gilda diplo       <nome Gilda> \r\n"
				  ,ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ECONOMY_V)||IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ECONOMY_AQ))
			  send_to_char(
				  "   Gilda economia    \r\n",ch);
			if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ECONOMY_AQ)||IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_FEES))
			 send_to_char(
				  "   Gilda assumi <idmob>    \r\n"
				  "   Gilda licenzia <idmob>  \r\n",ch);
		   	/*if(IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ZECCA_EXP))
			  send_to_char(
				  "   conia <tot> <gold>    \r\n"
				  "   trade <tot> <gems>  \r\n",ch); */
		      }
			 }
}

void do_new_clan_create (struct char_data *ch, char *arg)
{
  struct char_data *leader = NULL;
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  int new_id=0,i;

  if (!*arg) {
	 send_new_clan_format(ch);
    return;
  }

  if (GET_LEVEL(ch) < NEW_LVL_CLAN_GOD) {
    send_to_char("Non sei abbastanza Immortale per creare nuove Gilde!\r\n", ch);
    return;
  }

  if(new_num_of_clans == NEW_MAX_CLANS) {
    send_to_char("Raggiunto il Massimo delle Gilde. WOW!\r\n",ch);
    return;
  }

  half_chop(arg, arg1, arg2);

  if(!(leader=get_char_vis(ch,arg1))) {
	 send_to_char("Il Leader della nuova Gilda deve essere presente.\r\n",ch);
	 return;
  }

  if(strlen(arg2)>=32) {
    send_to_char("Nome della gilda troppo lungo! (al massimo 32 caratteri)\r\n",ch);
    return;
  }

  if(GET_LEVEL(leader)>=LVL_IMMORT) {
    send_to_char("Non puoi mettere un Immortale a capo di una gilda.\r\n",ch);
    return;
  }

  if(NEW_GET_CLAN(leader)!=0 && NEW_GET_CLAN_RANK(leader)!=0) {
    send_to_char("Il Leader gia appartiene ad una gilda!\r\n",ch);
	 return;
  }

  if(find_new_clan(arg2)!=-1) {
    send_to_char("Questo gilda gia esiste!\r\n",ch);
    return;
  }

  strncpy(new_clan[new_num_of_clans].name, CAP((char *)arg2), 32);
  strcpy(new_clan[new_num_of_clans].leader,GET_NAME(leader));
  for(i=0;i<new_num_of_clans;i++)
	 if(new_id<new_clan[i].id)
		new_id=new_clan[i].id;
  new_clan[new_num_of_clans].type=NEW_REGNO;
  new_clan[new_num_of_clans].id=new_id+1;
  new_clan[new_num_of_clans].regno=new_clan[new_num_of_clans].id;
  new_clan[new_num_of_clans].ranks =NEW_DEFAULT_RANKS;
  for(i=0;i<NEW_DEFAULT_RANKS;i++){
	 strcpy(new_clan[new_num_of_clans].rank_name[i],new_default_ranks[i]);
	 new_clan[new_num_of_clans].rank_tax[i]=new_default_tax[i];
  }
  new_clan[new_num_of_clans].automatic_ranks=NEW_AUTOMATIC_CLASSES;
  for(i=0;i<NEW_DEFAULT_RANKS;i++)
	 new_clan[new_num_of_clans].rank_bounds[i]=new_default_bounds[i];

  new_clan[new_num_of_clans].economy.treasure = 0 ;
  new_clan[new_num_of_clans].members = 1 ;
  new_clan[new_num_of_clans].power = GET_LEVEL(leader) ;
  for(i=0;i< NEW_NUM_CP;i++)
	 SET_BIT(NEW_GET_CLAN_PRIV(leader),( 1 << i));


  for(i=0;i<new_num_of_clans;i++){
	 strcpy(new_clan[i].diplomacy[new_num_of_clans].name, new_clan[new_num_of_clans].name);
	 new_clan[i].diplomacy[new_num_of_clans].proposta = -1;
	 new_clan[i].diplomacy[new_num_of_clans].timer = -1;
	 new_clan[new_num_of_clans].diplomacy[i].id=new_clan[i].id;
	 strcpy(new_clan[new_num_of_clans].diplomacy[i].name, new_clan[i].name);
	 new_clan[new_num_of_clans].diplomacy[i].status=NEW_PEACE;
	 new_clan[new_num_of_clans].diplomacy[i].proposta=-1;
	 new_clan[new_num_of_clans].diplomacy[i].timer=-1;
  }
  //reset_history(new_num_of_clans);

  new_num_of_clans++;

  new_save_clans();
  send_to_char("Gilda creata.\r\n", ch);
  NEW_GET_CLAN(leader)=new_clan[new_num_of_clans-1].id;
  NEW_GET_CLAN_RANK(leader)=new_clan[new_num_of_clans-1].ranks;
  save_char(leader, leader->in_room);
  return;
}

void do_new_clan_destroy (struct char_data *ch, char *arg)
{

  int i,j,k, shop_nr, zone;

  struct char_file_u chdata;
  struct char_data *victim=NULL;

  if (!*arg) {
	 send_new_clan_format(ch);
	 return; }

  if ((i = find_new_clan(arg)) < 0) {
	 send_to_char("Gilda Sconosciuta.\r\n", ch);
	 return;
  }

  if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
	 send_to_char("Non sei abbastanza influente per distruggere una gilda!\r\n", ch);
	 return;
  }

  /*Lance Negozi*/
  for (shop_nr = 0; shop_nr < top_shop; shop_nr++)
	 {
	 if (SHOP_CLAN(shop_nr)==new_clan[i].id)
		{
		SHOP_CLAN(shop_nr)= -1;
		zone = real_zone(SHOP_ROOM(shop_nr,0));
		if (zone!=-1)
		  sedit_save_to_disk(zone);
		}
	 }

  for (j = 0; j <= top_of_p_table; j++){
	 if((victim=is_playing((player_table +j)->name))) {
		if(NEW_GET_CLAN(victim)==new_clan[i].id) {
	NEW_GET_CLAN(victim)=0;
	NEW_GET_CLAN_RANK(victim)=0;
	save_char(victim, victim->in_room);
		}
	 }
	 else {
		load_char((player_table + j)->name, &chdata);
		if(chdata.player_specials_saved.new_clan==new_clan[i].id) {
	chdata.player_specials_saved.new_clan=0;
	chdata.player_specials_saved.new_clan_rank=0;
	save_char_file_u(chdata);
		}
	 }
  }
  for(j=0;j<new_num_of_clans;j++){
	 new_clan[j].diplomacy[i].timer=-1;
	 new_clan[j].diplomacy[i].status=-1;
	 new_clan[j].diplomacy[i].proposta=-1;
  }
  memset(&new_clan[i], sizeof(struct new_clan_rec), 0);

  for (j = i; j < new_num_of_clans - 1; j++)
	 new_clan[j] = new_clan[j + 1];
	for(j=0 ;j<new_num_of_clans -1;j++){
	  sprintf(buf," clan %d\n",j);
	  send_to_char(buf,ch);
	  for(k=i;k<new_num_of_clans-1;k++){
		 sprintf(buf," doing %d\n",k);
		 send_to_char(buf,ch);
		new_clan[j].diplomacy[k] = new_clan[j].diplomacy[k+1];
	 }
	}
  new_num_of_clans--;

  send_to_char("Gilda eliminata.\r\n", ch);
  new_save_clans();
  return;
}

void do_new_clan_enroll (struct char_data *ch, char *arg){
  struct char_data *vict=NULL;
  int clan_num,immcom=0;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  if (!(*arg)) {
	 send_new_clan_format(ch);
	 return;
  }

  if(GET_LEVEL(ch)<LVL_IMMORT) {
	 if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
		send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
		return;
	 }
  }
  else {
	 if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
		send_to_char("Non hai i privilegi necessari.\r\n", ch);
		return;
	 }
	 immcom=1;
	 half_chop(arg,arg1,arg2);
	 strcpy(arg,arg1);
	 if ((clan_num = find_new_clan(arg2)) < 0) {
		send_to_char("Gilda sconosciuta.\r\n", ch);
		return;
	 }
  }

  if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ENROLL) && !immcom) {
	 send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
	 return;
  }

  if(!(vict=get_char_room_vis(ch,arg))) {
	 send_to_char("La persona deve essere di fronte a te!\r\n",ch);
	 return;
  }
  else {
	 if(NEW_GET_CLAN(vict)!=new_clan[clan_num].id) {
		if(NEW_GET_CLAN_RANK(vict)>0) {
	send_to_char("Appartiene gia ad un altra gilda.\r\n",ch);
	return;
		}
		else {
	send_to_char("Non ha chiesto di entrare nella tua gilda.\r\n",ch);
	return;
		}
	 }
	 else
		if(NEW_GET_CLAN_RANK(vict)>0) {
	send_to_char("E' gia nella tua gilda.\r\n",ch);
	return;
		}
	 if(GET_LEVEL(vict)>=LVL_IMMORT) {
		send_to_char("Non puoi arruolare Immortali nella tua gilda.\r\n",ch);
		return;
	 }
  }

  NEW_GET_CLAN_RANK(vict)++;
  save_char(vict, vict->in_room);
  new_clan[clan_num].power += GET_LEVEL(vict);
  new_clan[clan_num].members++;
  new_clan[clan_num].fama += GET_FAMA(vict);
  send_to_char("Entri a far parte da ora della gilda che hai scelto!\r\n",vict);
  send_to_char("Fatto.\r\n",ch);
  return;
}

void do_new_clan_expel (struct char_data *ch, char *arg)
{
  struct char_data *vict=NULL;
  struct char_file_u chdata;
  int clan_num,immcom=0,found,j;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  if (!(*arg)) {
	 send_new_clan_format(ch);
	 return;
  }
  if(GET_LEVEL(ch)<LVL_IMMORT) {
	 if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
		send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
		return;
	 }
  }
  else {
	 if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
		send_to_char("Non hai i privilegi necessari.\r\n", ch);
		return;
	 }
	 immcom=1;
	 half_chop(arg,arg1,arg2);
	 strcpy(arg,arg1);
	 if ((clan_num = find_new_clan(arg2)) < 0) {
		send_to_char("Gilda Sconosciuta.\r\n", ch);
		return;
	 }
  }

  if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_EXPEL) && !immcom) {
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
	 if(NEW_GET_CLAN(vict)!=new_clan[clan_num].id) {
		send_to_char("Non appartiene alla tua Gilda.\r\n",ch);
		return;
	 }
	 else {
		if(NEW_GET_CLAN_RANK(vict)>=NEW_GET_CLAN_RANK(ch) && !immcom) {
	send_to_char("Non puoi buttar fuori questa persona .\r\n",ch);
	return;
		}
	 }
	 NEW_GET_CLAN(vict)=0;
	 NEW_GET_CLAN_RANK(vict)=0;
	 save_char(vict, vict->in_room);
	 new_clan[clan_num].members--;
	 new_clan[clan_num].power-=GET_LEVEL(vict);
	 new_clan[clan_num].fama -=GET_FAMA(vict);
	 send_to_char("Sei stato cacciato dalla tua gilda!\r\n",vict);
	 send_to_char("Fatto. Addios!\r\n",ch);
	 return;
	 break;
  case 2:
	 if(chdata.player_specials_saved.new_clan!=new_clan[clan_num].id){
		send_to_char("Non appartiene alla tua gilda.\r\n",ch);
		return;
	 }
	 else {
	  if(chdata.player_specials_saved.new_clan_rank>=NEW_GET_CLAN_RANK(ch) && !immcom)
	  {
		 send_to_char("Non puoi buttar fuori questa persona .\r\n",ch);
		 return;
	  }
		chdata.player_specials_saved.new_clan=0;
		chdata.player_specials_saved.new_clan_rank=0;
		save_char_file_u(chdata);
		new_clan[clan_num].members--;
		send_to_char("Fatto. Addios!\r\n",ch);
		return;
	 }
	 break;
  }
}

void do_new_clan_demote (struct char_data *ch, char *arg)
{
  struct char_data *vict=NULL;
  int clan_num,immcom=0;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  if (!(*arg)) {
	 send_new_clan_format(ch);
	 return;
  }

  if(GET_LEVEL(ch)<LVL_IMMORT) {
	 if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
		send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
		return;
	 }
  }
  else {
	 if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
		send_to_char("Non hai i privilegi necessari.\r\n", ch);
		return;
	 }
	 immcom=1;
	 half_chop(arg,arg1,arg2);
	 strcpy(arg,arg1);
	 if ((clan_num = find_new_clan(arg2)) < 0) {
		send_to_char("Gilda Sconosciuto.\r\n", ch);
		return;
	 }
  }

  if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_DEMOTE) && !immcom) {
	 send_to_char("Non sei abbastanza influente per farlo!\r\n",ch);
	 return;
  }

  if(!(vict=get_char_room_vis(ch,arg))) {
	 send_to_char("Scusa, CHI ??\r\n",ch);
	 return;
  }
  else {
	 if(NEW_GET_CLAN(vict)!=new_clan[clan_num].id) {
		send_to_char("Non appartiene al tuo clan.\r\n",ch);
		return;
	 }
	 else {
		if(NEW_GET_CLAN_RANK(vict)==1) {
	send_to_char("Non puo essere degradato ulteriormente, usa espelli semmai.\r\n",ch);
	return;
		}
		if(NEW_GET_CLAN_RANK(vict)>=NEW_GET_CLAN_RANK(ch) && !immcom) {
	send_to_char("Non puoi degradare una persona di rango superiore al tuo!\r\n",ch);
	return;
		}
	 }
  }

  NEW_GET_CLAN_RANK(vict)--;
  save_char(vict, vict->in_room);
  send_to_char("Sei appena stato degradato!\r\n",vict);
  send_to_char("Fatto.\r\n",ch);
  return;
}

void do_new_clan_promote (struct char_data *ch, char *arg)
{
  struct char_data *vict=NULL;
  int clan_num,immcom=0;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  if (!(*arg)) {
	 send_new_clan_format(ch);
	 return;
  }

  if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
	 if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
		send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
		return;
	 }
  }
  else {
	 immcom=1;
	 half_chop(arg,arg1,arg2);
	 strcpy(arg,arg1);
	 if ((clan_num = find_new_clan(arg2)) < 0) {
		send_to_char("Gilda Sconosciuto.\r\n", ch);
		return;
	 }
  }

  if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_PROMOTE) && !immcom) {
	 send_to_char("Non sei abbastanza influente all interno della tua gilda per farlo\r\n",ch);
	 return;
  }

  if(!(vict=get_char_room_vis(ch,arg))) {
	 send_to_char("Er, Chi??\r\n",ch);
	 return;
  }
  else {
	 if(NEW_GET_CLAN(vict)!=new_clan[clan_num].id) {
		send_to_char("Non appartiene alla tua gilda.\r\n",ch);
	 return;
	 }
	 else {
		if(NEW_GET_CLAN_RANK(vict)==0) {
	send_to_char("Non e' stato ancora arruolato.\r\n",ch);
      return;
      }
      if((NEW_GET_CLAN_RANK(vict)+1)>NEW_GET_CLAN_RANK(ch) && !immcom) {
	send_to_char("Non puoi promuovere questa persona oltre il tuo stesso rango!\r\n",ch);
	return;
      }
      if(NEW_GET_CLAN_RANK(vict)==new_clan[clan_num].ranks) {
	send_to_char("Non puoi promuovere oltre il massimo dei raghi!\r\n",ch);
	return;
		}
	 }
  }
  if((NEW_GET_CLAN_RANK(ch)==new_clan[clan_num].ranks)&&(NEW_GET_CLAN_RANK(vict)==new_clan[clan_num].ranks-1)) {
	 send_to_char("&1&bDeve esserci un solo capo ,cioe tu!\r\n&0",ch);
	 return;
  }
  NEW_GET_CLAN_RANK(vict)++;
  save_char(vict, vict->in_room);
  send_to_char("&1&bSei stato promosso all interno della tua gilda!\r\n&0",vict);
  send_to_char("Fatto.\r\n",ch);
  return;
}

void do_new_clan_who (struct char_data *ch)
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
  bzero(buff,sizeof(buff));
  bzero(buff2,sizeof(buff2));
  if(NEW_GET_CLAN_RANK(ch)==0) {
	 send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
	 return;
  }
  id=find_new_clan_by_id(NEW_GET_CLAN(ch));
  send_to_char("\r\n         SUDDITI DEL REGNO\r\n",ch);
  send_to_char("&1---------------------------------------------&0\r\n",ch);
  for (j = 0; j <= top_of_p_table; j++){
	 if((victim=is_playing((player_table +j)->name))) {
		present ="* ";
		if(NEW_GET_CLAN(victim)==NEW_GET_CLAN(ch)&&NEW_GET_CLAN_RANK(victim)>0)  {
	rank=NEW_GET_CLAN_RANK(victim);
	sprintf(line_disp,"&1%s&0%-15s %s\r\n",present,GET_NAME(victim),new_clan[id].rank_name[rank-1]);
	//send_to_char(line_disp,ch);
	strcat(buff,line_disp);
		}
	 }
	 /* else {
		present="X";
		load_char((player_table + j)->name, &chdata);
		if(chdata.player_specials_saved.new_clan==NEW_GET_CLAN(ch)) {
		  sprintf(line_disp,"&4%s&0%-15s %s\r\n",present,chdata.name,new_clan[id].rank_name[chdata.player_specials_saved.new_clan_rank-1]);
	strcat(buff2,line_disp);
	}
	 }*/
  }
  page_string(ch->desc,buff,1);
  send_to_char("&1---------------------------------------------&0\r\n",ch);
  /*  send_to_char("\r\n         NON COLLEGATI\r\n",ch);
  send_to_char("&1---------------------------------------------&0\r\n",ch);
  page_string(ch->desc,buff2,1);
  send_to_char("&1---------------------------------------------&0\r\n",ch);*/
  return;
}

void do_new_clan_status (struct char_data *ch,char *argument)
{
  char line_disp[90];
  int clan_num;

  if(GET_LEVEL(ch)>=LVL_IMMORT) {
	 send_to_char("Tu sei un Immortale e non puoi appartenere a nessuna gilda!\r\n",ch);
	 return;
  }

  clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch));
  if(!*argument){
	 if(NEW_GET_CLAN_RANK(ch)==0){
		if(clan_num>=0) {
		    sprintf(line_disp,"Hai fatto domanda per entrare a far parte della gilda %s\r\n",new_clan[clan_num].name);
		    send_to_char(line_disp,ch);
		    return;
		}
		else {
		    send_to_char("Non appartieni ad una gilda!\r\n",ch);
		    return;
		}
	}
	 sprintf(line_disp,"Sei %s (Rank %d) of %s (ID %d)\r\n",
		 new_clan[clan_num].rank_name[NEW_GET_CLAN_RANK(ch)-1],NEW_GET_CLAN_RANK(ch),
		 new_clan[clan_num].name,new_clan[clan_num].id);
	 send_to_char(line_disp,ch);
	 if(NEW_GET_CLAN_PRIV(ch)>0){
		send_to_char("&1Godi dei seguenti privilegi&0:\n",ch);
		sprintbit(NEW_GET_CLAN_PRIV(ch),new_clan_privileges,buf);
		send_to_char(buf,ch);
		}
	 return;
  }
  if((clan_num=find_new_clan(argument))<0){
	 send_to_char("Gilda sconosciuta.",ch);
	 return;
  }
  new_show_status_regno(ch);
}

void do_new_clan_apply (struct char_data *ch, char *arg){
int clan_num;

if (!(*arg)) {
  send_new_clan_format(ch);
  return; }

if(GET_LEVEL(ch)>=LVL_IMMORT) {
  send_to_char("Gli Dei non possono entrare in nessuna gilda.\r\n",ch);
  return; }

if(NEW_GET_CLAN_RANK(ch)>0) {
  send_to_char("Tu gia appartieni ad una gilda!\r\n",ch);
  return; }
else {
  if ((clan_num = find_new_clan(arg)) < 0) {
	 send_to_char("Gilda Sconosciuto.\r\n", ch);
	 return; } }

 GET_GOLD(ch) -= 1000;
 new_clan[clan_num].economy.treasure += 1000;
 new_save_clans();
 NEW_GET_CLAN(ch)=new_clan[clan_num].id;
 save_char(ch, ch->in_room);
 send_to_char("Hai fatto domanda per entrare a far parte di una gilda!\r\n",ch);

 return;
}

void do_new_clan_info (struct char_data *ch, char *arg)
{
  int i=0,j;
  if(new_num_of_clans == 0) {
    send_to_char("Non ci sono ancora gilda formate.\r\n",ch);
    return;
  }

  if(!(*arg)) {
    sprintf(buf, "\r");

    for(i=0; i < new_num_of_clans; i++)
		sprintf(buf, "%s[%-3d][%s] %-10s Sudditi:%3d \r\n",buf,i
	  ,new_tipi_clan[new_clan[i].type],new_clan[i].name,new_clan[i].members);
	 page_string(ch->desc,buf, 1);
	 return;
  }
  else
	 if ((i = find_new_clan(arg)) < 0) {
		send_to_char("Gilda sconosciuta.\r\n", ch);
		return;
	 }

  sprintf(buf, "%s (ID:%d)>:\r\n",new_frase_regno(i,0),new_clan[i].id);
  send_to_char(buf, ch);
  for(j=0;j<new_clan[i].ranks;j++){
	 sprintf(buf, "%s ",new_clan[i].rank_name[j]);
	 send_to_char(buf, ch);}
  sprintf(buf, "%s\r\nMembri    : %d\r\nPower      : %d\t\nFama       : %d\t\nTreasure   : %d\r",buf, new_clan[i].members, new_clan[i].power,(new_clan[i].fama/new_clan[i].members),
	new_clan[i].economy.treasure);
  sprintf(buf, "%s\r\n",buf);
  send_to_char(buf, ch);
  sprintf(buf,"&1&bLeader&0: %s\n",new_clan[i].leader);
  send_to_char(buf,ch);
  return;
}

sh_int find_new_clan_by_id(int idnum)
{
  int i;
  for( i=0; i < new_num_of_clans; i++)
    if(idnum==new_clan[i].id)
      return i;
  return -1;
}

sh_int find_new_clan(char *name)
{
  int i;
  for( i=0; i < new_num_of_clans; i++)
	 if(strcmp(CAP(name), CAP(new_clan[i].name))==0)
      return i;
  return -1;
}

void new_save_clans()
{
  FILE *fl;

  if (!(fl = fopen(NEW_CLAN_FILE, "wb"))) {
	 log("SYSERR: Unable to open clan file");
    return;
  }

  fwrite(&new_num_of_clans, sizeof(int), 1, fl);
  fwrite(new_clan, sizeof(struct new_clan_rec), new_num_of_clans, fl);
  fclose(fl);
  return;
}


void new_init_clans()
{
  FILE *fl;
  int i,j;
  extern int top_of_p_table;
  extern struct player_index_element *player_table;
  struct char_file_u chdata;

  memset(new_clan,0,sizeof(struct new_clan_rec)*NEW_MAX_CLANS);
  new_num_of_clans=0;
  i=0;

  if (!(fl = fopen(NEW_CLAN_FILE, "rb"))) {
    log("   Clan file does not exist. Will create a new one");
    new_save_clans();
    return;
  }

  fread(&new_num_of_clans, sizeof(int), 1, fl);
  fread(new_clan, sizeof(struct new_clan_rec), new_num_of_clans, fl);
  fclose(fl);

  log("   Calcolo di potenza, numero di membri e fama delle Gilde");
  for(i=0;i<new_num_of_clans;i++) {
	 new_clan[i].power=0;
	 new_clan[i].members=0;
	 new_clan[i].fama=0;
  }
  for (j = 0; j <= top_of_p_table; j++){
	 load_char((player_table + j)->name, &chdata);
	 if((i=find_new_clan_by_id(chdata.player_specials_saved.new_clan))>=0) {
		new_clan[i].power+=chdata.level;
		new_clan[i].members++;
		new_clan[i].fama += chdata.points.fama;
	 }
  }

  return;
}

void do_new_clan_bank(struct char_data *ch, char *arg, int action)
{
int clan_num,immcom=0;
long amount=0;
char arg1[MAX_INPUT_LENGTH];
char arg2[MAX_INPUT_LENGTH];

if (!(*arg)) {
  send_new_clan_format(ch);
  return; }

if(GET_LEVEL(ch)<LVL_IMMORT) {
  if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
	 send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
	 return;
  }
}
else {
  if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
	 send_to_char("Non hai i privilegi necessari.\r\n", ch);
	 return;
  }
  immcom=1;
  half_chop(arg,arg1,arg2);
  strcpy(arg,arg1);
  if ((clan_num = find_new_clan(arg2)) < 0) {
	 send_to_char("Gilda sconosciuta.\r\n", ch);
	 return;
  }
}

if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_WITHDRAW) && !immcom && action==NEW_CP_WITHDRAW) {
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

if(!immcom && action==NEW_CB_DEPOSIT && GET_GOLD(ch)<amount) {
  send_to_char("Non hai tutti quei soldi!!\r\n",ch);
  return;
  }

if(action==NEW_CB_WITHDRAW && new_clan[clan_num].economy.treasure<amount) {
  send_to_char("La gilda non e' cosi ricca!\r\n",ch);
  return;
  }

switch(action) {
  case NEW_CB_WITHDRAW:
	 GET_GOLD(ch)+=amount;
	 new_clan[clan_num].economy.treasure-=amount;
	 send_to_char("Prelevi dal tesoro della gilda.\r\n",ch);
	 break;
  case NEW_CB_DEPOSIT:
	 if(!immcom) GET_GOLD(ch)-=amount;
	 new_clan[clan_num].economy.treasure+=amount;
	 send_to_char("Aumenti il tesoro della gilda ,bravo ,fallo piu spesso!.\r\n",ch);
	 break;
  default:
	 send_to_char("Problem in command, please report.\r\n",ch);
	 break;
  }
 save_char(ch, ch->in_room);
 new_save_clans();
 return;
}

void do_new_clan_money(struct char_data *ch, char *arg, int action)
{
int clan_num,immcom=0;
long amount=0;
char arg1[MAX_INPUT_LENGTH];
char arg2[MAX_INPUT_LENGTH];

if (!(*arg)) {
  send_new_clan_format(ch);
  return; }

if(GET_LEVEL(ch)<LVL_IMMORT) {
  if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
	 send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
	 return;
  }
}
else {
  if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
	 send_to_char("Non hai i privilegi necessari.\r\n", ch);
	 return; }
  immcom=1;
  half_chop(arg,arg1,arg2);
  strcpy(arg,arg1);
  if ((clan_num = find_new_clan(arg2)) < 0) {
	 send_to_char("Gilda sconosciuta.\r\n", ch);
	 return;
  }
}
if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_FEES) && !immcom) {
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
  case NEW_CM_APPFEE:
	 new_clan[clan_num].app_fee=amount;
	 send_to_char("Cambi la quota d'iscrizione ad clan.\r\n",ch);
	 break;
  case NEW_CM_DUES:
	 new_clan[clan_num].dues=amount;
	 send_to_char("You change the monthly dues.\r\n",ch);
	 break;
  default:
	 send_to_char("Problem in command, please report.\r\n",ch);
	 break;
  }

new_save_clans();*/
return;
}

void do_new_clan_ranks(struct char_data *ch, char *arg)
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
  send_new_clan_format(ch);
  return; }

if(GET_LEVEL(ch)<LVL_IMMORT) {
  if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
	 send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
	 return;
  }
}
else {
  if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
	 send_to_char("Non hai l'autorita' per fare questo.\r\n", ch);
	 return; }
  immcom=1;
  half_chop(arg,arg1,arg2);
  strcpy(arg,arg1);
  if ((clan_num = find_new_clan(arg2)) < 0) {
	 send_to_char("Gilda sconosciuta.\r\n", ch);
    return;
  }
}

if(NEW_GET_CLAN_RANK(ch)!=new_clan[clan_num].ranks && !immcom) {
  send_to_char("Non sei abbastanza influente per fare questo!\r\n",ch);
  return;
  }

if(!(*arg)) {
  send_to_char("Quanti ranghi?\r\n",ch);
  return;
  }

if(!is_number(arg)) {
  send_to_char("Imposti i ranghi a cosa?\r\n",ch);
  return;
  }

new_ranks=atoi(arg);

if(new_ranks==new_clan[clan_num].ranks) {
  send_to_char("La gilda ha gia' questo numero di ranghi.\r\n",ch);
  return;
  }

if(new_ranks<2 || new_ranks>NEW_MAX_RANKS) {
  sprintf(buf,"La gilda puo' avere da 2 a %d ranghi.\r\n",NEW_MAX_RANKS);
  send_to_char(buf,ch);
  return;
  }

for (j = 0; j <= top_of_p_table; j++) {
  if((victim=is_playing((player_table +j)->name))) {
    if(NEW_GET_CLAN(victim)==new_clan[clan_num].id) {
      if(NEW_GET_CLAN_RANK(victim)<new_clan[new_clan_num].ranks && NEW_GET_CLAN_RANK(victim)>0)
        NEW_GET_CLAN_RANK(victim)=1;
      if(NEW_GET_CLAN_RANK(victim)==new_clan[clan_num].ranks)
        NEW_GET_CLAN_RANK(victim)=new_ranks;
      save_char(victim, victim->in_room);
	 }
  }
  else {
    load_char((player_table + j)->name, &chdata);
    if(chdata.player_specials_saved.new_clan==new_clan[clan_num].id) {
      if(chdata.player_specials_saved.new_clan_rank<new_clan[new_clan_num].ranks && chdata.player_specials_saved.new_clan_rank>0)
        chdata.player_specials_saved.new_clan_rank=1;
      if(chdata.player_specials_saved.new_clan_rank==new_clan[new_clan_num].ranks)
        chdata.player_specials_saved.new_clan_rank=new_ranks;
      save_char_file_u(chdata);
	 }
  }
}

new_clan[new_clan_num].ranks=new_ranks;
for(i=0;i<new_clan[new_clan_num].ranks-1;i++)
  strcpy(new_clan[new_clan_num].rank_name[i],"Membro");
strcpy(new_clan[new_clan_num].rank_name[new_clan[new_clan_num].ranks -1],"Leader");

new_save_clans();
return;
}

void do_new_clan_titles( struct char_data *ch, char *arg)
{
char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
int clan_num=0,rank;

if (!(*arg)) {
  send_new_clan_format(ch);
  return; }

if(GET_LEVEL(ch)<LVL_IMMORT) {
  if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
	 send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
	 return;
  }
  if(NEW_GET_CLAN_RANK(ch)!=new_clan[clan_num].ranks) {
	 send_to_char("Non sei abbastanza influente per fare questo!\r\n",ch);
	 return;
  }
} else {
  if(GET_LEVEL(ch)<NEW_LVL_CLAN_GOD) {
	 send_to_char("Non hai l'autorita' per fare questo.\r\n", ch);
	 return; }
  half_chop(arg,arg1,arg2);
  strcpy(arg,arg2);

  if(!is_number(arg1)) {
	 send_to_char("Devi specificare il numero della gilda (fai Gilda info).\r\n",ch);
	 return;
  }

  if((clan_num=atoi(arg1))<0 || clan_num>=new_num_of_clans) {
	 send_to_char("Non esiste nessuna gilda con quel numero.\r\n",ch);
	 return;
  }
}

half_chop(arg,arg1,arg2);

if(!is_number(arg1)) {
  send_to_char("Devi spacificare il numero del rango.\r\n",ch);
  return; }

rank=atoi(arg1);

if(rank<1 || rank>new_clan[clan_num].ranks) {
  send_to_char("La gilda non ha cosi tanti ranghi.\r\n",ch);
  return; }

if(strlen(arg2)<1 || strlen(arg2)>20) {
  send_to_char("Puoi definire un nome di massimo 20 caratteri.\r\n",ch);
  return; }

strcpy(new_clan[clan_num].rank_name[rank-1],arg2);
new_save_clans();
send_to_char("Fatto.\r\n",ch);
return;
}


void new_display_pg_priv(struct char_data *ch,struct char_data *suddito,struct char_file_u chdata,int mode) {
  char name[128];
  char arg1[50];
  int i,privilegi=0;
  switch(mode){
  case 0:
    sprintf(buf,"PRIVILEGI DELLA GILDA\n");
    for(i=0;i<NEW_NUM_CP;i++)
      sprintf(arg1,"%2d] %s\r\n",i,new_clan_privileges[i]);
      strcat(buf,arg1);
      send_to_char(buf,ch);
    return;
    break;
  case 1:
    strcpy(name,GET_NAME(suddito));
	 privilegi=NEW_GET_CLAN_PRIV(suddito);
    break;
  case 2:
    strcpy(name,chdata.name);
    privilegi=NEW_GET_CLAN_PRIV2(chdata);
    break;
  default:
    return;
  }
  strcat(name," :");
  for(i=0;i<NEW_NUM_CP;i++)
    if(IS_SET(privilegi,1<<i)){
      strcat(name,new_clan_privileges[i]);
      strcat(name,"\r\n");
    }
  send_to_char(name,ch);
}

void new_display_privilegi(struct char_data *ch){
  int i;
  send_to_char("\n&1&bPRIVILEGI DELLA GILDA&0:\n",ch);
  for(i=0;i<NEW_NUM_CP;i++){
	 sprintf(buf,"&4&b[&0%2d&4&b]&0 &2&b%s&0\n",i+1,new_clan_privileges[i]);
	 send_to_char(buf,ch);
  }
}

void do_new_clan_privilegi(struct char_data *ch,char *arg){
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  int j,cp=0,cl=0,rank=0,found=0,priv=0;
  bool imcom=FALSE;
  bool change=TRUE;
  struct char_file_u chdata;
  struct char_data *suddito=NULL;
  if(GET_LEVEL(ch)>=LVL_GOD)
	 imcom=TRUE;
  if(!*arg){
	 new_display_privilegi(ch);
	 return;
  }
  two_arguments(arg,arg1,arg2);
  if(!*arg1){
	 new_display_privilegi(ch);
	 return;
  }
  if(!*arg2)
	 change=FALSE;
  else{
	 cp=atoi(arg2);
	 if(!is_number(arg2)||(cp<0)||cp>NEW_NUM_CP){
		send_to_char("Il secondo argomento deve essere un numero compreso tra 1 e 15",ch);
		return;
	 }
  }
  //sprintf(buf,"STR:%s--CP:%d\n",arg2,cp);
  //send_to_char(buf,ch);
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
	 cl=NEW_GET_CLAN(suddito);
	 rank=NEW_GET_CLAN_RANK(suddito);
	 priv=NEW_GET_CLAN_PRIV(suddito);
	 break;
  case 2:
	 cl=NEW_GET_CLAN2(chdata);
	 rank=NEW_GET_CLAN_RANK2(chdata);
	 priv=NEW_GET_CLAN_PRIV2(chdata);
	 break;
  }

  if((cl!=NEW_GET_CLAN(ch)||rank>NEW_GET_CLAN_RANK(ch))&&!imcom)
	 send_to_char("NON puoi!",ch);
  else{
	 if(change){
		send_to_char("Okay.\n",ch);
		if(found==1){
	TOGGLE_BIT(NEW_GET_CLAN_PRIV(suddito),1 << (cp-1));
	priv=NEW_GET_CLAN_PRIV(suddito);
		}
		if(found==2){
	TOGGLE_BIT(NEW_GET_CLAN_PRIV2(chdata),1 << (cp-1));
	priv=NEW_GET_CLAN_PRIV2(chdata);
		}
	 }

	 sprintbit(priv,new_clan_privileges,buf1);
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



void do_new_clan_set(struct char_data *ch, char *arg)
{
char arg1[MAX_INPUT_LENGTH] ,arg2[MAX_INPUT_LENGTH];

half_chop(arg,arg1,arg2);

if (is_abbrev(arg1, "ranghi"     )) { do_new_clan_ranks(ch,arg2);          return ;}
if (is_abbrev(arg1, "titoli"     )) { do_new_clan_titles(ch,arg2);         return ;}
//if (is_abbrev(arg1, "privilege" )) { do_new_clan_privilege(ch,arg2);      return ;}
//if (is_abbrev(arg1, "dues"      )) { do_new_clan_money(ch,arg2,NEW_CM_DUES);  return ;}
//if (is_abbrev(arg1, "appfee"    )) { do_new_clan_money(ch,arg2,NEW_CM_APPFEE);return ;}
//if (is_abbrev(arg1, "livapp"    )) { do_new_clan_application(ch,arg2);    return ;}
if (is_abbrev(arg1, "stato"    )) { do_new_clan_set_status(ch,arg2);     return ;}
if (is_abbrev(arg1, "tasse"     )) { do_new_clan_set_tasse(ch,arg2);      return ;}

send_new_clan_format(ch);
}

int new_update_diplo(int cpos,int vpos, int declaration){
  if(cpos==vpos){
	 /*  sprintf(buf,"Non ha molto senso quello che vuoi fare\n");*/
	 return -1;
  }
  if(cpos>new_num_of_clans||vpos>new_num_of_clans){
	 /*   sprintf(buf,"C'e qualcosa di cannato qui:i clan specificati non esistono affatto!!!\n");
	 send_to_char(buf,ch);*/
	 return -1;
  }
  switch(declaration){
  case NEW_WAR:new_clan[cpos].diplomacy[vpos].status=NEW_WAR;new_clan[vpos].diplomacy[cpos].status=NEW_WAR;break;
  case NEW_PEACE:new_clan[cpos].diplomacy[vpos].status=NEW_PEACE;new_clan[vpos].diplomacy[cpos].status=NEW_PEACE;break;
  case NEW_ALLIANCE:new_clan[cpos].diplomacy[vpos].status=NEW_ALLIANCE;new_clan[vpos].diplomacy[cpos].status=NEW_ALLIANCE;break;
  case NEW_SURRENDER:new_clan[cpos].diplomacy[vpos].status=NEW_VASSALLO;new_clan[vpos].diplomacy[cpos].status=NEW_SURRENDER;break;
  case NEW_VASSALLO:new_clan[cpos].diplomacy[vpos].status=NEW_SURRENDER;new_clan[vpos].diplomacy[cpos].status=NEW_VASSALLO;break;
  default:return -1;
  }
  /*azzerero Proposte e timers ,e poi salvo*/
  new_clan[cpos].diplomacy[vpos].proposta=-1;
  new_clan[cpos].diplomacy[vpos].timer=-1;
  new_clan[vpos].diplomacy[cpos].proposta=-1;
  new_clan[vpos].diplomacy[cpos].timer=-1;
  new_save_clans();
  return new_clan[cpos].diplomacy[vpos].status;
}

void new_set_diplo_timer(int n_clan_prop,int n_clan_ric,int proposta){
  new_clan[n_clan_ric].diplomacy[n_clan_prop].proposta= proposta;
  new_clan[n_clan_ric].diplomacy[n_clan_prop].timer= 24;
}

char *new_status_flag(int status){
  char *str=NULL;
  switch(status){
  case NEW_PEACE:str="&3Pace&0";break;
  case NEW_WAR  :str="&1Guerra&0";break;
  case NEW_ALLIANCE  :str="&4Alleanza&0";break;
  case NEW_SURRENDER  :str="&5Vassallaggio&0";break;
  case NEW_VASSALLO  :str="&8Sudditanza&0";break;
  }
  return str;
}

void do_new_diplo_status(struct char_data *ch,int nclan){
  New_Regno *current,*avversario;
  char *status;
  char *name=NULL;
  bool found=FALSE;
  int i,time_left,tmp_regno,j,tmp_id;
  if(GET_LEVEL(ch)<LVL_IMMORT)
    if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_DIPLO)){
      sprintf(buf,"Non sei abbastanza influente per occuparti di cose cosi Importanti!!");
      send_to_char(buf,ch);
      return;
    }
  current=new_clan[nclan].diplomacy;
  sprintf(buf,"----------------------------\n");
  send_to_char(buf,ch);
  sprintf(buf,"|     STATUS DIPLOMATICO   |\n");
  send_to_char(buf,ch);
  sprintf(buf,"----------------------------\n");
  send_to_char(buf,ch);
  /*Ciclo degli status effettivi*/
  for(i=0;i<new_num_of_clans;i++){
	 if (i!=nclan){
      name = current[i].name;
      status=new_status_flag(current[i].status);
      if( new_clan[i].type==NEW_REGNO||current[i].status<=NEW_WAR){
	sprintf(buf,"Sei in stato di %s con %s\n",status,new_frase_regno(i,0));
	send_to_char(buf,ch);
      }
    }
  }
  found=FALSE;
  /*Ciclo delle prop fatte*/
  sprintf(buf,"\n&1---------------------------------------------------------&0\n");
  for(i=0;i<new_num_of_clans;i++){
    name=new_clan[i].name;
    avversario=new_clan[i].diplomacy;
    if(( i!=nclan) && avversario[nclan].proposta!=-1){
      status=new_status_flag(avversario[nclan].proposta);
      time_left=avversario[nclan].timer;
      found=TRUE;
      sprintf(buf,"Hai fatto una proposta di %s a %s. %d ore allo scadere.\n",status,new_frase_regno(i,0),time_left);
      send_to_char(buf,ch);
    }
  }
  /*Ciclo delle prop fatte*/

  for(i=0;i<new_num_of_clans;i++){
    if(( i!=nclan) &&(current[i].proposta != -1)){
     status=new_status_flag(current[i].proposta);
     name=current[i].name;
	  time_left=current[i].timer+1;
     found=TRUE;
     sprintf(buf,"Hai Ricevuto una proposta di %s da %s. %d ore X rispondere.\n",status,new_frase_regno(i,0),time_left);
     send_to_char(buf,ch);
    }
  }
  if(found==TRUE)
   send_to_char("\n&1---------------------------------------------------------&0\n",ch);
  sprintf(buf,"\t&1&bVASSALLI&0\n");
  found=FALSE;
  for(i=0,j=0;i<new_num_of_clans;i++){
    if (i!=nclan){
		name = current[i].name;
      status=new_status_flag(current[i].status);
      if(current[i].status==NEW_VASSALLO){
	if(found==FALSE){
	  send_to_char(buf,ch);
	  found=TRUE;
	  }
	j++;
	sprintf(buf,"[%d] %s--%s\n",j,name,new_tipi_clan[new_clan[i].type]);
	send_to_char(buf,ch);
      }
    }
  }
  if((tmp_regno=new_clan[nclan].regno)!=new_clan[nclan].id){
    sprintf(buf,"Sei vassallo di:\n");
    send_to_char(buf,ch);
    sprintf(buf,"[%d] %s--%s\n",j,name,new_tipi_clan[new_clan[tmp_regno].type]);
	 send_to_char(buf,ch);
    tmp_regno=new_clan[tmp_regno].regno;
    while((tmp_id=new_clan[find_new_clan_by_id(tmp_regno)].type)!=NEW_REGNO||
	  new_clan[tmp_id].regno!=tmp_regno){
		sprintf(buf,"[%d] %s--%s\n",j,name,new_tipi_clan[new_clan[tmp_id].type]);
      send_to_char(buf,ch);
      tmp_regno=new_clan[tmp_id].regno;
      }
  }

}

int new_diplo_permesso(int req_clan_num,int part_clan_num) {
  if((new_clan[req_clan_num].diplomacy[part_clan_num].timer!=-1) ||( new_clan[part_clan_num].diplomacy[req_clan_num].timer!=-1))
    return -1;
  else
    return 1;
}


int new_check_response(int responderer,int asker){
  if (new_clan[responderer].diplomacy[asker].proposta!= -1)
    return 1;
  else
    return -1;
}

void do_new_clan_diplo(struct char_data * ch, char * arg){
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  int own,partner,newstatus;
  if(GET_LEVEL(ch)>= LVL_IMMORT){
    if(!*arg){
      sprintf(buf,"\nSpecifica il nome della gilda di cui vuoi vedere la Situazione Diplomatica.\n");
      send_to_char(buf,ch);
      return;
    }
    if((partner = find_new_clan(arg))==-1){
      sprintf(buf,"\nSpecifica Esattamente il nome della gilda con cui vuoi interagire\n");
      send_to_char(buf,ch);
      return;
    }
	 do_new_diplo_status(ch,partner);
    return;
  }
  own=find_new_clan_by_id(NEW_GET_CLAN(ch));
  /*controlli su chi ha l'autorita di fare sta roba*/
  if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_DIPLO)){
    sprintf(buf,"Non sei abbastanza influente per occuparti di cose cosi Importanti!!");
    send_to_char(buf,ch);
    return;
  }
  if(!*arg){
    sprintf(buf,"gilda diplo status\n");
    send_to_char(buf,ch);
    sprintf(buf,"gilda diplo guerra <gilda>\n");
    send_to_char(buf,ch);
    sprintf(buf,"gilda diplo alleanza <gilda>\n");
    send_to_char(buf,ch);
	 sprintf(buf,"gilda diplo pace <gilda>\n");
    send_to_char(buf,ch);
    sprintf(buf,"gilda diplo resa <gilda>\n");
    send_to_char(buf,ch);
    sprintf(buf,"gilda diplo vassallaggio <gilda>\n");
    send_to_char(buf,ch);
    sprintf(buf,"gilda diplo accetta <gilda> \n");
    send_to_char(buf,ch);
    sprintf(buf,"gilda diplo rifiuta <gilda> \n");
    send_to_char(buf,ch);
    return;
  }
  half_chop(arg,arg1,arg2);
  /*own=find_new_clan_by_id(NEW_GET_CLAN(ch));*/
  if( is_abbrev(arg1,"status")){
    do_new_diplo_status(ch,own);
    return;
  }
  partner=find_new_clan(arg2);

  if(!*arg2 || partner==-1){
	 sprintf(buf,"C)Specifica Esattamente il nome della gilda con cui vuoi interagire\n");
	 send_to_char(buf,ch);
	 return;
  }

  if(is_abbrev(arg1,"accetta")){
    if(new_check_response(own,partner)==1){
      newstatus=new_clan[own].diplomacy[partner].proposta;
		new_clan[own].diplomacy[partner].proposta=-1;
		new_clan[own].diplomacy[partner].timer=-1;
      new_update_diplo(partner,own,newstatus);
      sprintf(buf,"ok Hai accettato la Proposta di %s di %s\n",new_status_flag(newstatus),arg2);
      send_to_char(buf,ch);
      return;
    }
    sprintf(buf,"Cos'e che vuoi accettare?\n");
    send_to_char(buf,ch);
    return;
  }
  if(is_abbrev(arg1,"rifiuta")){
	 if(new_check_response(own,partner)==1){
      newstatus=new_clan[own].diplomacy[partner].proposta;
      new_clan[own].diplomacy[partner].proposta=-1;
      new_clan[own].diplomacy[partner].timer=-1;
      new_save_clans();
      sprintf(buf,"ok Hai Rifiutato la Proposta di %s di %s\n",new_status_flag(newstatus),arg2);
      send_to_char(buf,ch);
      return;
    }
    sprintf(buf,"Cos'e che vuoi Rifiutare?\n");
    send_to_char(buf,ch);
	 return;
  }

  /*check se posso fare una proposta al clan partner :se ne ho gia una in
    scadenza ovviamente no!*/
  if(new_diplo_permesso(own,partner)==1){
	 if( is_abbrev(arg1, "guerra")){
      sprintf(buf,"WOW:hai appena dichiarato guerra alla gilda %s!!!\n",arg2);
      send_to_char(buf,ch);
      new_update_diplo(own,partner,NEW_WAR);
      return;
    }
    if( is_abbrev(arg1,"alleanza")){
      sprintf(buf,"Hai appena chiesto un ALLEANZA con la gilda %s!!!\n",arg2);
      send_to_char(buf,ch);
      new_set_diplo_timer(own,partner,NEW_ALLIANCE);
      return;
    }
	 if( is_abbrev(arg1, "pace")){
      sprintf(buf,"WOW:hai appena chiesto la PACE con il clan %s!!!\n",arg2);
      send_to_char(buf,ch);
      new_set_diplo_timer(own,partner,NEW_PEACE);
      return;
    }
    if( is_abbrev(arg1, "resa")){
      sprintf(buf,"Ti sei appena ARRESO alla gilda %s!!!\n",arg2);
      send_to_char(buf,ch);
      new_update_diplo(own,partner,NEW_SURRENDER);
      return;
	 }
	 if( is_abbrev(arg1, "vassallaggio")){
		sprintf(buf,"WOW:hai appena intimato alla gilda %s di ARRENDERSI a te!!!\n",arg2);
		send_to_char(buf,ch);
		new_set_diplo_timer(own,partner,NEW_VASSALLO);
		return;
	 }

    sprintf(buf,"Comando sconosciuto:digita gilda diplo per tutti i possibili comandi.\n");
    send_to_char(buf,ch);
  }
  else{
    sprintf(buf,"Mi dispiace hai in ballo con questo clan un altra trattativa");
    send_to_char(buf,ch);
  }
  return;
}

void new_update_diplo_timers(){
int i,j;
 for(i=0;i<new_num_of_clans;i++){
   for(j=0;j<new_num_of_clans;j++){
     if(new_clan[i].diplomacy[j].timer>0){
       new_clan[i].diplomacy[j].timer--;
     }
     else{
       new_clan[i].diplomacy[j].timer=-1;
       new_clan[i].diplomacy[j].proposta=-1;
     }
     new_save_clans();
   }
 }
}

ACMD(do_new_clan)
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

  half_chop(argument, arg1, arg2);
  if (is_abbrev(arg1, "assegna"   )){do_new_clan_assign_zone(ch,arg2); return  ;}
  if (is_abbrev(arg1, "crea"      )){ do_new_clan_create(ch,arg2);     return  ;}
  if (is_abbrev(arg1, "distruggi" )){ do_new_clan_destroy(ch,arg2);    return  ;}
 // if (is_abbrev(arg1, "negozi"    )){ list_shop_own (ch, 0);       return  ;}
  if (is_abbrev(arg1, "arruola"   )){ do_new_clan_enroll(ch,arg2);     return  ;}
  if (is_abbrev(arg1, "espelli"   )){ do_new_clan_expel(ch,arg2);      return  ;}
  if (is_abbrev(arg1, "who"       )){ do_new_clan_who(ch);             return  ;}
  if (is_abbrev(arg1, "status"    )){ do_new_clan_status(ch,arg2);     return  ;}
  if (is_abbrev(arg1, "info"      )){ do_new_clan_info(ch,arg2);       return  ;}
  if (is_abbrev(arg1, "domanda"   )){ do_new_clan_apply(ch,arg2);      return  ;}
  if (is_abbrev(arg1, "degrada"   )){ do_new_clan_demote(ch,arg2);     return  ;}
  if (is_abbrev(arg1, "promuovi"  )){ do_new_clan_promote(ch,arg2);    return  ;}
  if (is_abbrev(arg1, "set"       )){ do_new_clan_set(ch,arg2);        return  ;}
  if (is_abbrev(arg1, "preleva" )){ do_new_clan_bank(ch,arg2,NEW_CB_WITHDRAW);return ;}
  if (is_abbrev(arg1, "deposita")){ do_new_clan_bank(ch,arg2,NEW_CB_DEPOSIT); return ;}
  if (is_abbrev(arg1, "diplomazia")){do_new_clan_diplo(ch,arg2);       return ;}
  if (is_abbrev(arg1, "economia"  )){do_new_clan_economy(ch,arg2);          return ;}
  if (is_abbrev(arg1, "privilegi" )){do_new_clan_privilegi(ch,arg2);   return ;}
  if (is_abbrev(arg1, "stipendi"  )){do_new_clan_stipendio(ch,arg2);   return ;}
  if (is_abbrev(arg1, "mobiles"  )){do_new_clan_mob(ch,arg2);   return ;}
  if (is_abbrev(arg1, "assumi"  )){do_new_clan_assumi(ch,arg2);   return ;}
  if (is_abbrev(arg1, "licenzia"  )){do_new_clan_licenzia(ch,arg2);   return ;}
  if (is_abbrev(arg1, "limiti"  )){do_new_clan_set_limits(ch,arg2);   return ;}
  send_new_clan_format(ch);
}

void do_new_clan_set_status(struct char_data *ch,char *arg){
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
  if(GET_LEVEL(ch)<LVL_IMMORT) {
	 if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
      send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
      return;
    }
  }

  else
    immcom=1;

  if(NEW_GET_CLAN_RANK(ch)!=new_clan[clan_num].ranks && immcom==0) {
    send_to_char("Non sei abbastanza influente per occuparti di cose cosi Importanti!!\r\n",ch);
    return;
  }
  if(!(*arg)) {
    new_show_status_regno(ch);
    return;
  }
  two_arguments(arg,arg1,arg2);
  if(!is_number(arg2))
    ok=FALSE;
  if(ok==FALSE||((status=(atoi(arg2)))<=0||status>(NEW_MAX_RANKS-1))){
    sprintf(buf,"Il secondo argomento deve essere un numero compreso tra 1 e %d",NEW_MAX_RANKS-1);
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
	 if(find_new_clan_by_id(NEW_GET_CLAN(vict))!=clan_num&&immcom!=1){
		send_to_char("Non appartiene alla tua gilda",ch);
		return;
	 } else{
		NEW_GET_CLAN_RANK(vict)=status;
		send_to_char("Okay.",ch);
		save_char(vict, vict->in_room);
		return;
	 }
	 break;
  case 2:
	 if(chdata.player_specials_saved.new_clan!=NEW_GET_CLAN(ch)&&immcom!=1){
		send_to_char("Non appartiene alla tua gilda",ch);
		return;
	 } else{
		chdata.player_specials_saved.new_clan_rank=status;
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

void new_show_status_regno(struct char_data *ch){
  int i,j,clan_num;
  char line[80];
  bzero(buf2,sizeof(buf2));
  if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0)
	 return;
  for(i=0;i<(new_clan[clan_num].ranks-1);i++){
	 if(i<10)
		sprintf(line," ");
	 sprintf(buf,"%d]%s",i,new_clan[clan_num].rank_name[i]);
	 strcat(line,buf);
	 for(j=0;strlen(line)<=25;j++)
		strcat(line," ");
	 sprintf(buf,"[&1&b%d&0]\n",new_clan[clan_num].rank_tax[i]);
	 strcat(line,buf);
	 strcat(buf2,line);
	 bzero(line,80);
  }
  page_string(ch->desc, buf2, 0);
}


void do_new_clan_set_tasse(struct char_data *ch,char *argument){
  int class=0,new_tasse=0,clan_num=0,ranks=0;
  bool ok =TRUE;
  char arg1[30];
  char arg2[30];
  if(GET_LEVEL(ch)<LVL_IMMORT) {
	 if((clan_num=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0) {
		send_to_char("Non appartieni a nessuna gilda!\r\n",ch);
		return;
	 }
  }
	ranks=new_clan[clan_num].ranks-1;
  if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_SET_TAX)){
	 send_to_char("Non hai l'autorita' per fare questo!",ch);
	 return;
  }
  if(!*argument){
	 new_show_status_regno(ch);
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
  sprintf(buf,"Cambiate le tasse della Classe sociale &1&b%s&0 da &1&b%d&0 a &1&b%d&0.\r\n",new_clan[clan_num].rank_name[class],new_clan[clan_num].rank_tax[class],new_tasse);
  new_clan[clan_num].rank_tax[class]=new_tasse;
  new_save_clans();
  send_to_char(buf,ch);
}

void do_new_clan_assign_zone(struct char_data *ch,char *argument){
  extern struct zone_data *zone_table;	/* zone table   */
  extern int top_of_zone_table;
  extern void print_zone_to_buf(char *bufptr, int zone);
  char arg1[32];
  char arg2[32];
  char tmp_buf[256];
  int i,j,k;
  bzero(tmp_buf,sizeof(tmp_buf));
  if(GET_LEVEL(ch)<LVL_GOD){
    send_to_char("Solo un creatore puo assegnare una zona ad una gilda",ch);
    return;
  }
  if (!*argument) {
    send_new_clan_format(ch);
    return;
  }
  two_arguments(argument,arg1,arg2);
  if(!*arg2){
    send_to_char("Devi specificare anche il vnum della zona che vuoi assegnare",ch);
  return;
  }
  if((k=find_new_clan(arg1))<0){
    send_to_char("Non esiste una gilda con quel nome!",ch);
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
	new_clan[k].capitale_regno=j;
	print_zone_to_buf(tmp_buf, i);
	sprintf(buf,"Hai assegnato alla gilda %s la zona:\n%s\n",arg1,tmp_buf);
	send_to_char(buf,ch);
      }
      new_save_clans();
}


void new_assign_to_regno(struct char_data *suddito,int choice){
  extern int mortal_start_zone[];
  int i,zone_start,cl=0;
  bool found;
  if(new_num_of_clans==0){/*non ci sono ancora regni creati,e' inutile cercare*/
    NEW_GET_CLAN(suddito)=0;
    NEW_GET_CLAN_RANK(suddito)=0;
    return;
  }
  zone_start=mortal_start_zone[choice+1];
  for(i=0,found=FALSE;i<new_num_of_clans;i++){
    if(new_clan[i].capitale_regno==zone_start){
      cl=i;
      found=TRUE;
    }
  }

  if(found==TRUE){
	 NEW_GET_CLAN(suddito)=new_clan[cl].id;
	 NEW_GET_CLAN_RANK(suddito)=1;
	 new_clan[cl].members++;
	 new_clan[cl].power++;
	 sprintf(buf,"NUOVO SUDDITO PER LA GILDA %s:%s",new_frase_regno(cl,0),GET_NAME(suddito));
	 mudlog(buf, NRM, LVL_IMMORT, FALSE);
	 new_save_clans();
  }
  else{
	 NEW_GET_CLAN(suddito)=0;
	 NEW_GET_CLAN_RANK(suddito)=0;
  }
  return;
}

int new_update_class(struct char_data *taxed,struct char_file_u chdata,bool playing){
  int i,capital,tax,old_rank,new_rank=0,cl;
  bool is_playing=TRUE;

  if(playing){

	 if(taxed!=NULL){
		cl = find_new_clan_by_id(NEW_GET_CLAN(taxed));
		old_rank = NEW_GET_CLAN_RANK(taxed);
		capital = GET_GOLD(taxed)+GET_BANK_GOLD(taxed);
	 }
	 else
		return 0;
  }
  else
	 if(!playing){
		is_playing=FALSE;
		cl=find_new_clan_by_id(NEW_GET_CLAN2(chdata));
		old_rank=chdata.player_specials_saved.new_clan_rank;
		capital=chdata.points.gold+chdata.points.bank_gold;
	 }

  new_rank=old_rank;

  if(old_rank<=NEW_AUTOMATIC_CLASSES&&old_rank>=2){
    new_rank=1;
    while(capital>=new_clan[cl].rank_bounds[new_rank+1]&&new_rank<=NEW_AUTOMATIC_CLASSES)
      new_rank++;
  }

  tax=new_clan[cl].rank_tax[new_rank-1];

  if(playing){
	 NEW_GET_CLAN_RANK(taxed)=new_rank;
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
	 sprintf(buf,"&1&bPAGHI LE TASSE&0: hai &4&b%d&0 monete in meno!",tax);
	 send_to_char(buf,taxed);
	 save_char(taxed,taxed->in_room);
  }
  else{
	 chdata.player_specials_saved.new_clan_rank=new_rank;
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

void new_tax_improve(void){
  int to_clan,from_pg,i,j;
  struct char_data *taxed;
  struct char_file_u chdata;

  log("CALCULATING TAXES");
  for(i=0;i<new_num_of_clans;i++){
	 sprintf(buf,"GILDA:%s",new_clan[i].name);
	 log(buf);
	 new_clan[i].economy.last_tax_income=0;
	 to_clan=0;
	 for (j = 0; j <= top_of_p_table; j++){
		if((taxed=is_playing((player_table +j)->name))) {
			if(NEW_GET_CLAN(taxed)==new_clan[i].id) {
			  from_pg=new_update_class(taxed,chdata,TRUE);
			  to_clan+=from_pg;
			  save_char(taxed, taxed->in_room);
			  sprintf(buf,"%s--$:%d",GET_NAME(taxed),from_pg);
			  log(buf);
			}
		} else {
			load_char((player_table + j)->name, &chdata);
			if(chdata.player_specials_saved.new_clan==new_clan[i].id) {
			  from_pg=new_update_class(NULL,chdata,FALSE);
			  to_clan+=from_pg;
			  save_char_file_u(chdata);
			  sprintf(buf,"%s--$:%d",chdata.name,from_pg);
			  log(buf);
			}
		}

	}
	 new_clan[i].economy.treasure+=to_clan;
	 new_clan[i].economy.last_entrate_sem+=to_clan;
	 new_clan[i].economy.last_tax_income=to_clan;
	 new_clan[i].economy.current_entrate=0;
	 //   new_clan[i].economy.current_entrate=0;
	 new_clan[i].economy.current_dogana_income=0;
	 new_clan[i].economy.current_zecca_income=0;
	 sprintf(buf,"TOTAL TAX FOR %s-$:%d",new_clan[i].name,to_clan);
	 mudlog(buf, NRM, LVL_IMMORT, FALSE);
	 log(buf);
  }
  new_save_clans();
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
/*
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
*/

struct mob_ass{
  int num;
  struct char_data *mob;
  int fee;
  int vnum;
};

void new_display_stipendi(struct char_data *ch,int idclan){

  struct char_data *suddito;
  struct char_data *mob=NULL;
  struct char_file_u chdata;
  bool imcom=FALSE,found=FALSE;
  int i,j,next_pos=0,num_parz=0,tot_parz=0,tot=0,num=0;
  struct mob_ass mob_counter[NEW_MAX_MOB_ASSOLDABILI];
  for(i=0;i<NEW_MAX_MOB_ASSOLDABILI;i++){
    mob_counter[i].num=0;
    mob_counter[i].mob=NULL;
    mob_counter[i].fee=0;
    mob_counter[i].vnum=0;
  }
  if(GET_LEVEL(ch)>=LVL_GOD)
    imcom=TRUE;
  /*else
    idclan=NEW_GET_CLAN(ch);*/
  sprintf(buf,
		 "                   PG STIPENDIATI                   \n"
	    "&1------------------------------------------------&0\n"
	  );
  send_to_char(buf,ch);
  for (j = 0,found=FALSE; (j <= top_of_p_table ); j++){
    //cerco tra i pg giocanti
	 if((suddito=is_playing((player_table +j)->name))) {
      if(NEW_GET_CLAN(suddito)==idclan||imcom){
	if(NEW_GET_STIPENDIOHAS_FEE(suddito)&&NEW_GET_STIPENDIOCLAN_FEE(suddito)>0){
	  sprintf(buf,"&4&b[&0%s&4&b]&0 $:&4&b%ld\n",GET_NAME(suddito),NEW_GET_STIPENDIOCLAN_FEE(suddito));
	  found=TRUE;
	  num_parz++;
	  tot_parz+=NEW_GET_STIPENDIOCLAN_FEE(suddito);
	  send_to_char(buf,ch);
	}
      }
    }
    //cerco tra i pg non connessi
    else{
      load_char((player_table + j)->name, &chdata);
      if(NEW_GET_CLAN2(chdata)==idclan||imcom){
	if(NEW_GET_STIPENDIOHAS_FEE2(chdata)&&NEW_GET_STIPENDIOCLAN_FEE2(chdata)>0){
	  sprintf(buf,"&4&b[&0%s&4&b]&0 $:&4&b%ld\n",chdata.name,
		  NEW_GET_STIPENDIOCLAN_FEE2(chdata));
	  found=TRUE;
	  num_parz++;
	  tot_parz+=NEW_GET_STIPENDIOCLAN_FEE2(chdata);
	  send_to_char(buf,ch);
	}
		}
    }
  }
  if(found==FALSE)
    send_to_char("&4&bNESSUNO&0\n",ch);

  sprintf(buf,
	  "&1------------------------------------------------&0\n"
	  "               MOB AL SERVIZIO DELLA GILDA\r\n"
	  "&1------------------------------------------------&0\n"
	       );
  send_to_char(buf,ch);
  for (mob = character_list; mob; mob = mob->next){
    if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&NEW_GET_STIPENDIOHAS_FEE(mob)==TRUE){
      if(/*NEW_GET_STIPENDIO(mob).idclan>0&&*/NEW_GET_STIPENDIOCLAN_FEE(mob)>0&&
	NEW_CLAN_ID(mob) ==idclan ){
	if( NEW_GET_STIPENDIOCLAN_FEE(mob)!=GET_LEVEL(mob)*NEW_FEE_PER_LEVEL)
	  NEW_GET_STIPENDIOCLAN_FEE(mob)=GET_LEVEL(mob)*NEW_FEE_PER_LEVEL;
	for(i=0,found=FALSE;i<NEW_MAX_MOB_ASSOLDABILI&&(found==FALSE);i++){
	  if(GET_MOB_VNUM(mob)==mob_counter[i].vnum){
	    found=TRUE;
	    mob_counter[i].num++;
	  }
	}
	if(found==FALSE){
	  mob_counter[next_pos].num++;
	  mob_counter[next_pos].mob=mob;
	  mob_counter[next_pos].vnum=GET_MOB_VNUM(mob);
	  mob_counter[next_pos].fee=NEW_GET_STIPENDIOCLAN_FEE(mob);
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
	  "&3&bTOT PG&0  :%d                              %d    \n"
	  "&1------------------------------------------------&0 \n"
	  "&3&bTOT MOB&0 :%d                              %d    \n"
	  "&1------------------------------------------------&0 \n"
	  "&3&bTOT STIP&0:%d                              %d    \n"
	  "&1------------------------------------------------&0 \n",
	  num_parz,tot_parz,  num,tot,num_parz+num,tot_parz+tot);
  send_to_char(buf,ch);
}

void do_new_clan_stipendio(struct char_data *ch,char *arg){
 char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  int j,cl=0,rank=0,found=0,stipendio=0,new_stipendio=0;
  bool imcom=FALSE;
  bool change=TRUE;
  bool has_fee=FALSE;
  struct char_file_u chdata;
  struct char_data *suddito=NULL;

  if(GET_LEVEL(ch)>=LVL_GOD)
    imcom=TRUE;
  cl=find_new_clan_by_id(NEW_GET_CLAN(ch));
  if(!imcom&&(cl<0||!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_FEES))){
    send_to_char("Non sei autorizzato!",ch);
	 return;
  }

  if(!*arg){
    new_display_stipendi(ch,NEW_GET_CLAN(ch));
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
    has_fee=NEW_GET_STIPENDIOHAS_FEE(suddito);
    cl=NEW_GET_CLAN(suddito);
    stipendio=NEW_GET_STIPENDIOCLAN_FEE(suddito);
    break;
  case 2:
    has_fee=NEW_GET_STIPENDIOHAS_FEE2(chdata);
    cl=NEW_GET_CLAN2(chdata);
    stipendio=NEW_GET_STIPENDIOCLAN_FEE2(chdata);
    break;
  }
  if((cl!=NEW_GET_CLAN(ch)||rank>=NEW_GET_CLAN_RANK(ch))&&!imcom)
    send_to_char("NON puoi!",ch);
  else{
	 if(change){
      send_to_char("Okay.\n",ch);
      if(found==1){
	NEW_GET_STIPENDIOHAS_FEE(suddito)=TRUE;
	if(imcom){
	  //NEW_GET_STIPENDIO(suddito).idchar=GET_IDNUM(ch);
	  //NEW_GET_STIPENDIO(suddito).char_fee=new_stipendio;
	}
	else{
	  //NEW_GET_STIPENDIO(suddito).idclan=NEW_GET_CLAN(ch);
	  NEW_GET_STIPENDIOCLAN_FEE(suddito)=new_stipendio;
	  save_char(suddito,suddito->in_room);
	}
      }
      if(found==2){
	NEW_GET_STIPENDIOHAS_FEE2(chdata)=TRUE;
	if(imcom){
	  //NEW_GET_STIPENDIO2(chdata).idchar=GET_IDNUM(ch);
	  //NEW_GET_STIPENDIO2(chdata).char_fee=new_stipendio;
	}
	else{
	  //NEW_GET_STIPENDIO2(chdata).idclan=NEW_GET_CLAN(ch);
	  NEW_GET_STIPENDIOCLAN_FEE2(chdata)=new_stipendio;
	  save_char_file_u(chdata);
	}
      }
    }
    if(found==1){
      sprintf(buf,"&4&b[&0%s&4&b]&0$:&4&b%ld&0",GET_NAME(suddito),
			NEW_GET_STIPENDIOCLAN_FEE(suddito));
      save_char(suddito,suddito->in_room);
    }
	 else{
      sprintf(buf,"&4&b[&0%s&4&b]&0:&1&b%ld&0",chdata.name,NEW_GET_STIPENDIOCLAN_FEE2(chdata));
      save_char_file_u(chdata);
    }
    send_to_char(buf,ch);
  }
}

void new_stipendi_mensili(void){
  int j,cl,stipendio_clan;
  //int stipendio_charn,found2=0,i;
  int found=0;
  bool has_fee;
  //struct char_data *pagante;
  struct char_data *suddito,*mob;
  struct char_file_u chdata;
  //struct char_file_u chpag;

  /*pg*/
  for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
    has_fee=FALSE;
    //cerco tra i pg giocanti
    if((suddito=is_playing((player_table +j)->name))) {
  	  if(NEW_GET_STIPENDIOHAS_FEE(suddito)){
	    if(NEW_GET_CLAN(suddito)>0){
	       cl=find_new_clan_by_id(NEW_GET_CLAN(suddito));
	        if((stipendio_clan=NEW_GET_STIPENDIOCLAN_FEE(suddito))>0){
	          has_fee=TRUE;
	          GET_BANK_GOLD(suddito)+=stipendio_clan;
		      new_clan[cl].economy.treasure-=stipendio_clan;
	          new_clan[cl].economy.stipendi+=stipendio_clan;
	          new_clan[cl].economy.current_uscite+=stipendio_clan;
	        }
	    }
      /*  if(NEW_GET_STIPENDIO(suddito).idchar>0){
	  if((stipendio_char=NEW_GET_STIPENDIO(suddito).char_fee)>0){
	    for (i= 0,found2=0; (i <= top_of_p_table)&&found!=1; i++){
	      //cerco tra i pg giocanti
	      if((pagante=is_playing((player_table +i)->name))) {
				if(NEW_GET_STIPENDIO(suddito).idchar==GET_IDNUM(pagante)){
				  GET_BANK_GOLD(suddito)+=stipendio_char;
				  GET_BANK_GOLD(pagante)-=stipendio_char;
				  save_char(pagante,pagante->in_room);
				  found2=1;
				  has_fee=TRUE;
				}
			} else{
				load_char((player_table + i)->name, &chpag);
				if(NEW_GET_STIPENDIO(suddito).idchar==chpag.char_specials_saved.idnum){
				  GET_BANK_GOLD(suddito)+=stipendio_char;
				  chpag.points.bank_gold-=stipendio_char;
				  save_char_file_u(chpag);
				  found2=1;
				  has_fee=TRUE;
				  sprintf(buf,"Ricevi il tuo stipendio da %s ,$%d.",chpag.name,stipendio_char);
				  send_to_char(buf,suddito);
				}
			} //else
		 } //for
	  }
		  }*/
	NEW_GET_STIPENDIOHAS_FEE(suddito)=has_fee;
	save_char(suddito,suddito->in_room);
		}
	 }
	 //cerco tra i pg non connessi
	 else{
		load_char((player_table + j)->name, &chdata);
		if(NEW_GET_STIPENDIOHAS_FEE2(chdata)){
	if((cl=find_new_clan_by_id(NEW_GET_CLAN2(chdata)))>0){
	  if((stipendio_clan=NEW_GET_STIPENDIOCLAN_FEE2(chdata))>0){
        if (get_no_play(NULL, chdata , NOPAYTIMEOUT, FALSE)) {
		 has_fee=TRUE;
		 chdata.points.bank_gold+=stipendio_clan;
		 new_clan[cl].economy.treasure-=stipendio_clan;
		 new_clan[cl].economy.stipendi+=stipendio_clan;
		 new_clan[cl].economy.current_uscite+=stipendio_clan;
        }
	  }
	}

	/*
	if(NEW_GET_STIPENDIO2(chdata).idchar>0){
	  if((stipendio_char=NEW_GET_STIPENDIO2(chdata).char_fee)>0){
		 for (i= 0,found2=0; (i <= top_of_p_table)&&found!=1; i++){
	      //cerco tra i pg giocanti
	      if((pagante=is_playing((player_table +i)->name))) {
		if(NEW_GET_STIPENDIO2(chdata).idchar==GET_IDNUM(pagante)){
		  chdata.points.bank_gold+=stipendio_char;
		  GET_BANK_GOLD(pagante)-=stipendio_char;
		  sprintf(buf,"Paghi lo stipendio a %s ,$%d.",chdata.name,stipendio_char);
		  send_to_char(buf,pagante);
		  save_char(pagante,pagante->in_room);
		  found2=1;
		  has_fee=TRUE;
		}
	      }
              else{
		load_char((player_table + i)->name, &chpag);
		if(NEW_GET_STIPENDIO2(chdata).idchar==chpag.char_specials_saved.idnum){
		  chdata.points.bank_gold+=stipendio_char;
		  chpag.points.bank_gold-=stipendio_char;
		  save_char_file_u(chpag);
		  found2=1;
		  has_fee=TRUE;
                }
	      }//else
	    } //for
	  }//charfee
	}//idchar

	*/
	NEW_GET_STIPENDIOHAS_FEE2(chdata)=has_fee;
	save_char_file_u(chdata);
      }/*has_fee*/
	 }/*else*/
 }/*for*/


 for (mob = character_list; mob; mob = mob->next){
   if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&NEW_GET_STIPENDIOHAS_FEE(mob)){
     if(NEW_CLAN_ID(mob)>0&&(stipendio_clan=NEW_GET_STIPENDIOCLAN_FEE(mob))>0&&(cl=find_new_clan_by_id(NEW_CLAN_ID(mob)))>=0){
       new_clan[cl].economy.treasure-=stipendio_clan;
		 new_clan[cl].economy.stipendi+=stipendio_clan;
       new_clan[cl].economy.current_uscite+=stipendio_clan;
     }
    /* else{//a differenza di un pg un mob puo essere stipendiato 1 volta sola
		 if(NEW_MASTER_ID(mob)>0&&
	  (stipendio_char=NEW_GET_STIPENDIOCLAN_FEE(mob))>0){
	 //solito ciclo per beccare chi deve pagare tra i pg
	 for (j = 0,found=0; (j <= top_of_p_table &&found==0); j++){
	       //cerco tra i pg giocanti
	   if((suddito=is_playing((player_table +j)->name))) {
	     if(GET_IDNUM(suddito)==NEW_MASTER_ID(mob)){
	       GET_BANK_GOLD(suddito)-=stipendio_char;
	       sprintf(buf,"Paghi %s :$%d",GET_NAME(mob),stipendio_char);
	       send_to_char(buf,suddito);
	       save_char(suddito,suddito->in_room);
	     }
	   }
	   else{
	     load_char((player_table + j)->name, &chpag);
	     if(NEW_MASTER_ID(mob)==chpag.char_specials_saved.idnum)
	       {
		 chpag.points.bank_gold-=stipendio_char;
		 save_char_file_u(chpag);
			 }
	   }
	 }//for
       }//if
     }//else  */
   } //if
 }//for
 new_save_clans();
}/*end*/
#define MESE 0
#define SEMESTRE 1
#define NEW_CLAN_EC(i) new_clan[i].economy

void new_update_bilancio_regni(void){
  FILE *fd;
  time_t now=time(0);

  int i,current_uscite,current_entrate=0,dogana_income,stipendi,zecca,treasure,export;
  char nomeregno[30];
  for(i=0;i<new_num_of_clans;i++){
	 treasure=NEW_CLAN_EC(i).treasure;
	 export=NEW_CLAN_EC(i).corrent_export_income;
	 current_uscite=NEW_CLAN_EC(i).current_uscite;
	 current_entrate=NEW_CLAN_EC(i).current_entrate;
	 dogana_income=NEW_CLAN_EC(i).current_dogana_income;
	 stipendi=NEW_CLAN_EC(i).stipendi;
	 zecca=NEW_CLAN_EC(i).current_zecca_income;

	 /*write to disk the last balance and update it*/

	 sprintf(nomeregno,"etc/gilda.%d",i);
	 if((fd=fopen(nomeregno,"a+"))==NULL){
		sprintf(buf,"WARNING: ' %s ' not Found ,Creating it.....",nomeregno);
		log(buf);
		fd=fopen(nomeregno,"wb");
	 }
	 fd=fopen(nomeregno,"a+");
	 fprintf(fd,"%s  %s",new_clan[i].name,ctime(&now ));
	 sprintf(buf,
		 "TESORO:%d - ULTIME USCITE:%d - ULTIME ENTRATE:%d\n"
		 "STIPENDI:%d - TASSE:%d - DOGANA :%d - PROD. ZECCA:%d\n"
		 "EXPORT:%d\n",
		 NEW_CLAN_EC(i).treasure,current_uscite,current_entrate,stipendi,
		 NEW_CLAN_EC(i).last_tax_income,dogana_income,zecca,export);
	 fprintf(fd,"%s\n",buf);
	 fclose(fd);

		NEW_CLAN_EC(i).last_sem_treasure=treasure;
		NEW_CLAN_EC(i).current_uscite        =0;
		NEW_CLAN_EC(i).current_entrate       =0;
		NEW_CLAN_EC(i).current_dogana_income =0;
		NEW_CLAN_EC(i).stipendi              =0;
		NEW_CLAN_EC(i).current_zecca_income  =0;
		NEW_CLAN_EC(i).corrent_export_income =0;
		NEW_CLAN_EC(i).last_uscite_sem=current_uscite;
		NEW_CLAN_EC(i).last_entrate_sem=current_entrate;
		NEW_CLAN_EC(i).last_dogana_income=dogana_income;
		NEW_CLAN_EC(i).last_stipendi=stipendi;
		NEW_CLAN_EC(i).last_zecca_income=zecca;
		NEW_CLAN_EC(i).last_export_income=export;

  }
  new_save_clans();
}

void do_new_clan_economy(struct char_data *ch, char *arg){
  int cl,bal_tesoro,bal_uscite,bal_entrate,bal_zecca,bal_tax,bal_export,bal_stipendi,bal_dogana;
  bool imcom=FALSE;
  extern struct time_info_data time_info;

  if(GET_LEVEL(ch)>=LVL_GOD)
    imcom=TRUE;
  if(!*arg||(cl=find_new_clan(arg))<0)
    if(imcom){
      send_to_char("Devi specificare il nome di una gilda",ch);
      return;
    }
  if(!imcom){
    if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_ECONOMY_V)){
		send_to_char("Queste cose sono troppo grandi per te!",ch);
      return;
    }
    if((cl=find_new_clan_by_id(NEW_GET_CLAN(ch)))<0){
      send_to_char("NON APPARTIENI A NESSUNA GILDA CONOSCIUTA!",ch);
      return;
    }
  }
  bal_tesoro =NEW_CLAN_EC(cl).treasure-NEW_CLAN_EC(cl).last_sem_treasure;
  bal_uscite =NEW_CLAN_EC(cl).current_uscite-NEW_CLAN_EC(cl).last_uscite_sem;
  bal_entrate=NEW_CLAN_EC(cl).current_entrate-NEW_CLAN_EC(cl).last_entrate_sem;
  bal_zecca =NEW_CLAN_EC(cl).current_zecca_income-NEW_CLAN_EC(cl).last_zecca_income;
  bal_export=NEW_CLAN_EC(cl).corrent_export_income-NEW_CLAN_EC(cl).last_export_income;
  bal_dogana=NEW_CLAN_EC(cl).current_dogana_income-NEW_CLAN_EC(cl).last_dogana_income;
  bal_stipendi=NEW_CLAN_EC(cl).stipendi-NEW_CLAN_EC(cl).last_stipendi;
  bal_tax=NEW_CLAN_EC(cl).tax_income-NEW_CLAN_EC(cl).last_tax_income;
  /*view clan economy*/
  sprintf(buf,
	  "  &1&bGILDA&0 &4&b%s&0\n"
	  "&1------------------------&0\n"
	  "&4&bTESORO&0   : &1&b%d&0 [&3&b%d&0]\n"
	  "&4&bUSCITE  M&0: &1&b%d&0 [&3&b%d&0]\n"
	  "&4&bENTRATE M&0: &1&b%d&0 [&3&b%d&0]\n"
	  //"&4&bDOGANA &0  : &1&b%d&0 [&3&b%d&0]\n"
	  //"&4&bZECCA  &0  : &1&b%d&0 [&3&b%d&0]\n"
	  "&4&bTASSE  &0  : &1&b%d&0 [&3&b%d&0]\n"
	  "&4&bSTIPENDI&0 : &1&b%d&0 [&3&b%d&0]\n"
	  //"&4&bSHOP BAL.&0: &1&b%d&0\n"
	  //"&4&bEXPORT  &0 : &1&b%d&0 [&3&b%d&0]\n"
	  "&1------------------------&0\n",
	  new_clan[cl].name,
	  new_clan[cl].economy.treasure,bal_tesoro,
	  new_clan[cl].economy.current_uscite,bal_uscite,
	  new_clan[cl].economy.current_entrate,bal_entrate,
	  //new_clan[cl].economy.current_dogana_income,bal_dogana,
	  //new_clan[cl].economy.current_zecca_income,bal_zecca,
	  new_clan[cl].economy.last_tax_income,bal_tax,
	  new_clan[cl].economy.last_stipendi,bal_stipendi
	  //new_clan[cl].economy.current_shops_balance,
	  //new_clan[cl].economy.corrent_export_income,bal_export
	  );
  send_to_char(buf,ch);
  sprintf(buf,"Alla chiusura del bilancio mancano: %d mesi %d giorni %d ore (tempo mud)",
  (time_info.month >= 8) ? (15 - time_info.month): (7 - time_info.month),
  (34 - time_info.day),
  (23 - time_info.hours));


  send_to_char(buf,ch);



}

void do_new_clan_mob(struct char_data *ch,char *arg){
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
    send_to_char("Devi specificare il nome di una gilda e un vnum di mob.",ch);
    return;
  }
  two_arguments(arg,arg1,arg2);
  if((cl=find_new_clan(arg1))<0){
    send_to_char("Devi specificare il nome di una gilda e un vnum di mob o 'reset'.",ch);
    return;
  }
  if(!*arg2){
    sprintf(buf,
	    "    \n&4&bMOB CHE LA GILDA &1%s&4 PUO ASSUMERE.&0\r\n"
	    "&1------------------------------------------------&0\n"
	    ,new_clan[cl].name);
	 send_to_char(buf,ch);
    for(i=0,vnum=0;i<NEW_MAX_MOB_ASSOLDABILI;i++){
      if((vnum=new_clan[cl].mob_assoldabili[i])>0&&(j=real_mobile(vnum)>0)){
	mob=read_mobile(vnum,VIRTUAL);
	if(mob==NULL)
	  return;
	char_to_room(mob,0);
	sprintf(buf,"&4&b[&0%d&4&b]&0 %s &4&bVNUM:&0%d &4&bLIV:&0%d &4&bFEE:&0%d\n",i,GET_NAME(mob),vnum,GET_LEVEL(mob),NEW_FEE_PER_LEVEL*GET_LEVEL(mob));

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
	while(i<NEW_MAX_MOB_ASSOLDABILI&&new_clan[cl].mob_assoldabili[i]>0)
	  i++;
	if(i>=NEW_MAX_MOB_ASSOLDABILI){
	  send_to_char("ESAURITI GLI SLOT ;BISOGNA ALLUNGARE L'ARRAY",ch);
	  extract_char(mob);
	  return;
	}
	new_clan[cl].mob_assoldabili[i]=vnum;
	send_to_char("Okay.\r\n",ch);
	extract_char(mob);
	new_save_clans();
    }
    else{
      if(!str_cmp(arg2,"reset")){
	send_to_char("Il secondo argomento deve essere un vnum di un mob o 'reset'\r\n",ch);
	  return;
      }
	for(i=0;i<NEW_MAX_MOB_ASSOLDABILI;i++)
	  new_clan[cl].mob_assoldabili[i]=-1;
	send_to_char("Okay.\r\n",ch);
    }
  }
  new_save_clans();
}

void do_new_clan_assumi(struct char_data *ch,char *arg){
  int cl,vnum,rnum,i,j,fee,idclan;
  struct char_data *mob=NULL;
  bool found=FALSE;
  if((idclan=NEW_GET_CLAN(ch))<=0||!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_FEES)){
    send_to_char("Non sei in grado di assumere Nessuno!",ch);
    return;
  }
  cl=find_new_clan_by_id(idclan);
  if(!*arg){
    sprintf(buf,
	    "\n  &1MOB CHE LA GILDA &4&b%s&0 &1PUO ASSUMERE&0\r\n"
	    "&1--------------------------------------------------&0\r1\n"
	    ,new_clan[cl].name);
    send_to_char(buf,ch);
    for(i=0,vnum=0;i<NEW_MAX_MOB_ASSOLDABILI;i++){
      if((vnum=new_clan[cl].mob_assoldabili[i])>0&&(j=real_mobile(vnum)>0)){
	mob=read_mobile(vnum,VIRTUAL);
	if(mob==NULL)
	  return;
	char_to_room(mob,0);
	sprintf(buf,"&4&b[&0%d&4&b]&0 %s &4&bVNUM:&4&b%d &4&bLIV:&0%d &4&bFEE:&0%d\n",i,GET_NAME(mob),vnum,GET_LEVEL(mob),NEW_FEE_PER_LEVEL*GET_LEVEL(mob));
	found=TRUE;
	extract_char(mob);
	send_to_char(buf,ch);
      }
	 }
    if(found==FALSE)
      send_to_char("NESSUNO.\r\n",ch);
    return;
  }
  if(!is_number(arg)||(i=atoi(arg))<0||i>NEW_MAX_MOB_ASSOLDABILI){
    send_to_char("L'argomento dev'essere un numero compreso tra 0 e 30",ch);
    return;
  }
  if((vnum=new_clan[cl].mob_assoldabili[i])<=0){
	 send_to_char("Il regno non ha a disposizione questo mob",ch);
     return;
  }
  if((rnum=real_mobile(vnum))<0){
    send_to_char("Errore!! Questo mob non esiste. Contattare un creatore.",ch);
    return;
  }
  mob = read_mobile(rnum, REAL);
  if(mob==NULL)
    return;
  char_to_room(mob,0);
  NEW_CLAN_ID(mob) = idclan;
  fee=GET_LEVEL(mob)*NEW_FEE_PER_LEVEL;
  if(new_clan[cl].economy.treasure<fee){
    send_to_char("LA GILDA NON PUO PERMETTERSI DI STIPENDIARLO!",ch);
    extract_char(mob);
    return;
  }
  NEW_CLAN_EC(cl).treasure-=fee;
  //CLAN_EC(cl).stipendi+=fee;
  NEW_CLAN_EC(cl).current_uscite+=fee;
  //NEW_GET_STIPENDIO(mob).has_fee=TRUE;
  //NEW_GET_STIPENDIO(mob).idclan=idclan;
  //NEW_GET_STIPENDIO(mob).clan_fee=fee;
  NEW_GET_STIPENDIOHAS_FEE(mob)=TRUE;
  NEW_GET_STIPENDIOCLAN_FEE(mob)=fee;

  Save_mob();
  sprintf(buf, "\nHai assunto &4&b%s&0 [PAGA: &4&b%d&0] \r\n", GET_NAME(mob),fee);
  send_to_char(buf, ch);
  char_to_room(mob, ch->in_room);
  new_save_clans();
}

void do_new_clan_licenzia(struct char_data*ch,char *arg){
  int cl=-1,vnum,idclan;
  struct char_data *mob=NULL;
  //bool found=FALSE;
  if((idclan=NEW_GET_CLAN(ch))<=0||!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_GIVE_FEES)){
    send_to_char("Non sei in grado di assumere Nessuno!",ch);
    return;
  }
  cl=find_new_clan_by_id(idclan);
  if(!*arg){
	 sprintf(buf,
	    "\n           NPG DELLA GILDA  &4&b%s&0          \n"
	    "&1--------------------------------------------&0\n"
	    " ID           NOME         LIV.          STIP.  \n"
	    ,new_clan[cl].name);
    send_to_char(buf,ch);

	  for (mob = character_list; mob; mob = mob->next){
       if((IS_NPC(mob)&&MOB_FLAGGED(mob,MOB_SAVE))&&
	  NEW_GET_STIPENDIOHAS_FEE(mob)==TRUE){
	 if(NEW_CLAN_ID(mob)==idclan&&NEW_GET_STIPENDIOCLAN_FEE(mob)>0){
	   sprintf(buf,"[%d] %s        [%d]       [%ld]  [%d] \n"
		   ,GET_MOB_VNUM(mob),GET_NAME(mob),GET_LEVEL(mob),NEW_GET_STIPENDIOCLAN_FEE(mob),GET_LEVEL(mob)*NEW_FEE_PER_LEVEL);
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
	( NEW_CLAN_ID(mob) ==idclan)&&(GET_MOB_VNUM(mob)==vnum)){
	extract_char(mob);
	send_to_char("Fatto. Hai ora un dipendente in meno.\n\r",ch);
	return;
      }
    }
    send_to_char("Non esiste nessun tuo mob con quel codice.\n\r",ch);
    return;
  }
}

void new_display_bounds(struct char_data *ch,int cl){
  int i,aut_class;
  sprintf(buf,"&4&bCLASSI AUTOMATICHE   TASSE      LIMITE&0\r\n");
  strcat(buf,"&1--------------------------------------&0\r\n");
  if(cl<0||cl>=new_num_of_clans)
    return;
  aut_class=new_clan[cl].automatic_ranks;
  for(i=1;i<aut_class;i++){
    sprintf(buf2,"[&4&b%2d&0][&1%-12s&0]: [&3&b%-7d&0&1-&0&4&b%7d&0]\r\n",i,new_clan[cl].rank_name[i],new_clan[cl].rank_tax[i],new_clan[cl].rank_bounds[i]);
	 strcat(buf,buf2);
  }
  strcat(buf,"&1--------------------------------------&0\r\n");
  send_to_char(buf,ch);
  return;
}

void do_new_clan_set_limits(struct char_data *ch,char *arg){
  char arg1[20],arg2[20],arg3[20];
  bool imcom=(GET_LEVEL(ch)>=LVL_GOD ? TRUE :FALSE);
  int cl,bound,rank;
  if(imcom){
	 if(!*arg){
		send_to_char("Specifica il nome di una gilda.\r\n",ch);
		return;
	 }
	 two_arguments(arg,arg3,arg1);
	 if((cl=find_new_clan(arg3))==-1){
		send_to_char("Non esiste nessuna gilda con questo nome\r\n",ch);
		return;
	 }
	 two_arguments(arg1,arg1,arg2);
  }
  if(!imcom){
	 if((cl=find_new_clan_by_id(NEW_GET_CLAN(ch)))==-1){
		send_to_char("Non appartieni a nessuna gilda\r\n",ch);
		return;
	 }
	 if(!IS_SET(NEW_GET_CLAN_PRIV(ch),NEW_CP_SET_TAX)){
		send_to_char("Sono cose piu grandi di te, fila via!\r\n",ch);
		return;
	 }
	 two_arguments(arg,arg1,arg2);
  }
  if(!*arg1||!is_number(arg1)||(rank=atoi(arg1))<=0||rank>new_clan[cl].automatic_ranks){
	 new_display_bounds(ch,cl);
	 return;
  }
  if(!*arg2||!is_number(arg2)||(bound=atoi(arg2))<=0){
	 sprintf(buf,"[%-2d] [%-12s] :[%d]\r\n\r\n",rank,new_clan[cl].rank_name[rank],new_clan[cl].rank_bounds[rank]);
	 send_to_char(buf,ch);
	 return;
  }
  sprintf(buf,"Limite economico per lo stato %s cambiato da %d a %d\r\n",new_clan[cl].rank_name[rank],new_clan[cl].rank_bounds[rank],bound);
  new_clan[cl].rank_bounds[rank]=bound;
  new_save_clans();
  send_to_char(buf,ch);
}


