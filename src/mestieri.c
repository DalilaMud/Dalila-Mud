/*****************************************************************************
***   Mestieri.c - File di implementazione dei mestieri su DalilaMUD       ***
***  Questo file e' di proprietad' esclusiva dello staff di DalilaMUD      ***
***  Autori: Carlo Mocci (Kabir), Micale Adriano (Lance / Shade)           ***
***          Chardhros & Jerold.                                           ***                
***  Versione: 1 alpha                                                     ***
******************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "spells.h"
#include "interpreter.h"
#include "db.h"
#include "wilderness.h"
#include "olc.h"
#include "events.h"
#include "mestieri.h"
#include "dg_scripts.h"
#include "house.h"


extern struct dex_app_type dex_app[];
extern int wild_mobhunter(struct char_data * ch);
ACMD(do_flee);

#define PROF_PRIMARIA 1
#define PROF_SECONDARIA 2

const int classe_mestieri[TOT_PROF_MESTIERI][2] =
{
// Mestiere, Mestiere assoc, bonus
	{1  ,PROF_PRIMARIA},   //  PROF_MONTARE
	{2  ,PROF_PRIMARIA},   //  PROF_CAVALCARE
	{3  ,PROF_PRIMARIA},   //  PROF_DOMARE
	{4  ,PROF_PRIMARIA},   //  PROF_CACCIARE
	{5  ,PROF_SECONDARIA},   //  PROF_ADDESTRATORE
	{6  ,PROF_SECONDARIA}, //  PROF_SARTO
	{7  ,PROF_PRIMARIA},   //  PROF_COLTIVARE
	{8  ,PROF_SECONDARIA}, //  PROF_CUCINARE
	{9  ,PROF_PRIMARIA},   //  PROF_MINATORE
	{10 ,PROF_SECONDARIA}, //  PROF_FABBRO
	{11 ,PROF_SECONDARIA}, //  PROF_FALEGNAMERIA
	{12 ,PROF_SECONDARIA}, //  PROF_CARPENTERIA
	{13 ,PROF_SECONDARIA}, //  PROF_GIOIELLERIA
	{14 ,PROF_SECONDARIA}, //  PROF_ARCHITETTURA
	{15 ,PROF_PRIMARIA},   //  PROF_ERBORISTERIA
	{16 ,PROF_SECONDARIA}, //  PROF_ALCHIMIA
	{17 ,PROF_SECONDARIA}    //  PROF_PESCATORE
};

const char *frutta[NUM_TIPO_FRUTTA] =
{"lamponi","fragole","mele","ciliege","more","pesche","albicocche"};

const char *pesce[NUM_SPECIE_PESCE] =
{"arborella","gatto","branzino","trota","luccio","storione","tonno","squalo", "mazzugoru"};

const char *fattura[] = {"pessima", "scarsa", "media", "buona", "ottima", "magnifica"};

const char *aggioie[] = {"un orrendo", "uno scadente","un mirabile","uno stupendo","un pregiatissimo"};

const char *qualita[MAX_LIV_MOB_SELV] = {"buona", "ottima", "eccellente", "rubusta"}; //param 4(liv>3) per i draghi

const int dadi_armi[25][2] = {
	{ 0, 0 },   // Danno medio: 0
	{ 1, 1 },
	{ 1, 3 },
	{ 1, 5 },
	{ 2, 3 },
	{ 2, 4 },   // Danno medio: 5
	{ 3, 3 },
	{ 2, 6 },
	{ 4, 3 },
	{ 3, 5 },
	{ 4, 4 },   // Danno medio: 10
	{ 4, 4 },   // Sarebbe 2d10
	{ 4, 5 },
	{ 4, 5 },   // Sarebbe 2d12
	{ 4, 6 },
	{ 5, 5 },   // Danno medio: 15
	{ 4, 7 },
	{ 4, 7 },   // Sarebbe 2d16
	{ 6, 5 },
	{ 6, 5 },   // Sarebbe 2d18
	{ 8, 4 },   // Danno medio: 20
	{ 6, 6 },
	{ 4, 10 },
	{ 4, 10 },  // Sarebbe 2d22
	{ 8, 5 }};  // Danno medio: 24

 /* materiale 
    char *nome;
    int max_ac;
    int bonus_livello;
    int max_livspell;
    int max_slot;
    int peso;
    int val_comm;
    int val_nutr;
    int n_grezzi;
    int riuscita;
*/


const char * tipi_materiale[] = { "Non Specificato", "Metallo", "Legno", "Gemma", "Roccia", "Pelle e affini", "Naturale", "Erbe medicinali"};

struct materiale pietre[NUM_MATERIALI_PIETRE] = {
	{ "ferro"   , "x", 15, -10, 60, 50, 2, 200, 0, 2, 80},
	{ "argento" , "x", 11, -15, 40, 40, 3, 400, 0, 3, 70},
	{ "rame"    , "x",  8, -20, 20, 60, 2, 150, 0, 2, 80},
	{ "stagno"  , "x", 10,   0, 40, 50, 2, 110, 0, 2, 80},
	{ "zinco"   , "x", 12, - 5, 50, 50, 2, 180, 0, 2, 85},
	{ "oro"     , "x",  9, +10, 20, 40, 3, 600, 0, 3, 60}};

struct risorse_reset pietre_reset[NUM_MATERIALI_PIETRE] = {
	{ 180, 220,  20,  50,  25,  40, 110, 210, 210, 210},
	{  90, 100,  20,  30,  15,  25,  85, 185, 185, 185},
	{ 260, 280,  20,  50,  45,  65, 125, 225, 225, 225},
	{ 280, 310,  20,  50,  30,  45, 115, 215, 215, 215},
	{ 260, 320,  20,  50,  25,  40, 130, 230, 230, 230},
	{  70,  80,  10,  30,  15,  45, 105, 205, 205, 205}};

struct materiale alberi[NUM_MATERIALI_ALBERI] = {
	{ "frassino" , "x",  5, +10, 50, 14,  4,   100, 0, 1, 85},
	{ "mogano"   , "x",  9, -15, 10, 20,  5,   130, 0, 1, 80},
	{ "quercia"  , "x", 10, -10, 20, 15,  5,   150, 0, 1, 75},
	{ "castagno" , "x",  9, -10, 30, 16,  4,   160, 0, 1, 75},
	{ "noce"     , "x",  8,   0, 20, 16,  4,   110, 0, 1, 80},
	{ "abete"    , "x",  7,   5, 60, 15,  4,   120, 0, 1, 80},
	{ "pioppo"   , "x",  6, -20, 20, 10,  4,   100, 0, 1, 80},
	{ "ciliegio" , "x",  7, -10,  0, 15,  4,   130, 0, 1, 80},
	{ "betulla"  , "x",  4, -30, 40, 10,  3,   100, 0, 1, 75} };

struct risorse_reset alberi_reset[NUM_MATERIALI_ALBERI] = {
	{  50, 60,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  40, 60,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  50, 60,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  60, 80,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  40, 60,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  50, 60,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  40, 70,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  40, 60,  20,  40,  75, 125, 160, 170, 170, 170 },
	{  40, 69,  20,  40,  75, 125, 160, 170, 170, 170 } };

struct materiale gemme[NUM_MATERIALI_GEMME] = {
	{  "&7diamante&0"   , "diamante",  8,   5, 70, 40,  1, 1800, 0, 1, 35},
	{  "&3topazio&0"    , "topazio" ,  6,   4, 50, 50,  1, 1500, 0, 1, 50},
	{  "&2&bsmeraldo&0" , "smeraldo",  5,   3, 40, 55,  1, 1200, 0, 1, 52},
	{  "&1&brubino&0"   , "rubino"  ,  4,   2, 60, 45,  1,  900, 0, 1, 55},
	{  "&4&bzaffiro&0"  , "zaffiro" ,  2,   1, 40, 60,  1,  600, 0, 1, 60}};

struct risorse_reset gemme_reset[NUM_MATERIALI_GEMME] = {
	{  10,  25,  10,  30,  30,  45, 115,  215, 215, 215 },
	{  11,  20,  20,  30,  30,  45, 115,  215, 215, 215 },
	{  12,  23,  20,  30,  30,  45, 115,  215, 215, 215 },
	{  13,  24,  20,  30,  30,  45, 115,  215, 215, 215 },
	{  15,  26,  20,  30,  30,  45, 115,  215, 215, 215 } };

struct materiale rocce[NUM_MATERIALI_ROCCE] = {
	{ "marmo"   , "x",  0,   0,  0, 50, 5,  130, 0, 1, 75},
	{ "quarzo"  , "x",  4,   0,  0, 30, 4,  110, 0, 1, 80},
	{ "granito" , "x",  0,   0,  0, 70, 4,  120, 0, 1, 85} };

struct risorse_reset rocce_reset[NUM_MATERIALI_ROCCE] = {
	{ 120, 150,  50, 100,  15,  40, 125,  225, 225, 225 },
	{ 50,  100,  40, 100,  45,  70, 155,  255, 255, 255 },
	{ 150, 175,  50, 100,  30,  55, 140,  240, 240, 240 } };

struct materiale pelli[NUM_MATERIALI_PELLI] = {
	{ "carne"             , "x",  0,   0,  0, 14,  2,   60, 15, 1, 97},
	{ "pelle"             , "x",  8, -10, 30, 20,  3,  150, 0, 1, 78},
	{ "stoffa"            , "x",  4, -20,  0, 13,  2,  120, 0, 1, 85},
	{ "pelliccia"         , "x",  9,   0,  0, 15,  2,  180, 0, 1, 72},
	{ "scaglia di drago"  , "x", 16, -10, 40, 30, 15, 5000, 0, 2, 80}};

struct materiale natura[NUM_MATERIALI_NATURA] = {
	{ "farinacei" , "x",  0,   0, 20,  5,  3,   40, 7, 1, 95},
	{ "carta"     , "x",  0,   0, 20,  5,  3,   20, 0, 1, 65},
	{ "pesce"     , "x",  0,   0, 20,  5,  2,  100, 12, 1, 85},
	{ "frutta"    , "x",  0,   0, 20,  5,  3,   30, 4, 1, 95},
	{ "verdura"   , "x",  0,   0, 20,  5,  2,   30, 4, 1, 98}};

struct risorse_reset natura_reset[NUM_MATERIALI_NATURA] = {
	{ 200, 350,  30,  60, 150, 180, 215,  225, 225, 225 },
	{   0,   0,   0,   0,   0,   0,   0,    0,   0,   0 },
	{ 600,1500,   4,   8,   0,   0,   0,    0,   0, 340 },
	{ 150, 250,  30,  60, 150, 185, 245,  255, 255, 255 },
	{ 120, 250,  30,  60, 150, 190, 240,  250, 250, 250 } };

struct materiale erbe[NUM_MATERIALI_ERBE] = {
	{ "assenzio"    , "x",  0,   0,  0, 15,  3,   150, 0, 1, 85},
	{ "bardana"     , "x",  0,   0,  0, 15,  3,   160, 0, 1, 85},
	{ "biancospino" , "x",  0,   0,  0, 15,  3,   140, 0, 1, 85},
	{ "borragine"   , "x",  0,   0,  0, 15,  3,   130, 0, 1, 85},
	{ "calendula"   , "x",  0,   0,  0, 15,  3,   150, 0, 1, 85},
	{ "celidonia"   , "x",  0,   0,  0, 15,  3,   150, 0, 1, 85},
	{ "arnica"      , "x",  0,   0,  0, 15,  3,   145, 0, 1, 85},
	{ "ortica"      , "x",  0,   0,  0, 15,  3,   145, 0, 1, 85},
	{ "alchimilla"  , "x",  0,   0,  0, 15,  3,   147, 0, 1, 85},
	{ "achillea"    , "x",  0,   0,  0, 15,  3,   160, 0, 1, 85},
	{ "enula"       , "x",  0,   0,  0, 15,  3,   139, 0, 1, 85},
	{ "ginestra"    , "x",  0,   0,  0, 15,  3,   127, 0, 1, 85},
	{ "vischio"     , "x",  0,   0,  0, 15,  3,   125, 0, 1, 85},
	{ "belladonna"  , "x",  0,   0,  0, 15,  3,   130, 0, 1, 85},
	{ "caffe'"	, "x",  0,   0,  0, 15,  3,   150, 0, 1, 85}};

struct risorse_reset erbe_reset[NUM_MATERIALI_ERBE] = {
	{  90, 100,   5,  15, 150, 180, 270, 340, 340, 340 },
	{  90, 100,   5,  15, 120, 155, 245, 295, 295, 295 },
	{  90, 120,   5,  15,  30,  45, 165, 225, 225, 225 },
	{  80,  110,   5,  15,  30,  45, 165, 225, 225, 225 },
	{  80,  110,   5,  15, 150, 165, 250, 270, 270, 270 },
	{  50,  90,   5,  15,  30,  45, 165, 225, 225, 225 },
	{  100, 130,   5,  15,  30,  80, 120, 170, 170, 170 },
	{  80, 110,   5,  15,  30,  80, 120, 170, 170, 170 },
	{  85,  102,   5,  15,  30,  80, 120, 170, 170, 170 },
	{  72,  108,   5,  15,  15,  25,  65, 165, 165, 165 },
	{  90, 110,   5,  15,  30,  55, 135, 235, 235, 235 },
	{  100, 120,   5,  15,  30,  45, 165, 225, 225, 225 },
	{  100, 130,   5,  15, 150, 175, 260, 280, 280, 280 },
	{  70,  90,   5,  15,  15,  25,  65, 165, 165, 165 },
	{  7,   12,    2,  3,   100, 200, 30, 10,  200, 200 }} ;

// Numero pozione / erba / quantita'
const int erbe_pozioni[NUM_POZIONI_CREABILI][MAX_ERBE_X_POZIONE][2] = {
	{{1,2},{2,2},{-1,-1},{-1,-1},{-1,-1}},	   //0 "assenzio"   	 Remove poison
	{{6,2},{8,1},{-1,-1},{-1,-1},{-1,-1}},	   //1 "bardana"    	 Remove paralisys
	{{6,1},{7,1},{5,1},{-1,-1},{-1,-1}},	   //2 "biancospino"	 Remove blind
	{{6,1},{2,1},{7,1},{-1,-1},{-1,-1}},	   //3 "borragine"  	 Remove curse
	{{5,2},{4,1},{-1,-1},{-1,-1},{-1,-1}},        //4 "calendula"  	 Dispel magic
	{{4,2},{7,1},{1,1},{-1,-1},{-1,-1}},	   //5 "celidonia"  	 See invisible
	{{13,2},{11,3},{0,1},{-1,-1},{-1,-1}},	   //6 "arnica"     	 Invisible
	{{0,2},{13,2},{-1,-1},{-1,-1},{-1,-1}},	   //7 "ortica"     	 Poison
	{{3,3},{12,1},{-1,-1},{-1,-1},{-1,-1}},	   //8 "alchimilla" 	 Protection from fire
	{{11,2},{4,1},{-1,-1},{-1,-1},{-1,-1}},	   //9 "achillea"   	 Barkskin
	{{11,1},{4,1},{0,1},{-1,-1},{-1,-1}},	   //10 "enula"     	 Shield
	{{12,2},{10,1},{11,1},{-1,-1},{-1,-1}},	   //11 "ginestra"  	 Refresh
	{{8,2},{12,1},{-1,-1},{-1,-1},{-1,-1}},	   //12 "vischio"   	 Cure light
	{{11,2},{9,3},{-1,-1},{-1,-1},{-1,-1}},	   //13 "belladonna"	 Cure serious
	{{11,3},{9,2},{8,1},{1,2},{-1,-1}},	   //14 "caffe"		 Cure critic
	{{10,1},{5,1},{3,1},{1,1},{-1,-1}},	   //			 Detect poison e alignment
	{{10,1},{9,2},{11,1},{-1,-1},{-1,-1}},	   //			 Infravision
	{{10,1},{4,1},{8,1},{-1,-1},{-1,-1}},	   //	  		 Strength
	{{11,1},{7,1},{-1,-1},{-1,-1},{-1,-1}},	   //			 Light
	{{9,2},{7,3},{5,1},{-1,-1},{-1,-1}}	   //			 Tisana della nonna
};

const int costruzione [4][3] = {{70,20,10},      // legno
	{15,70,15},      // pietra
	{20,30,50},      // evoluta
	{33,33,34}};    // mista

extern int check_proficienza (struct char_data * ch, int prof_num, int test, int modifier);
extern int check_proficienza_improve (struct char_data * ch, int prof_num, int prob);
extern int objlevel (struct obj_data * obj);
extern struct index_data *obj_index;

extern struct room_data *world;
extern struct str_app_type str_app[];
extern struct obj_data *obj_proto;	/* prototypes for obj		 */
extern struct house_control_rec house_control[MAX_HOUSES];
extern int num_of_houses;
extern int find_house(room_vnum vnum);
extern void hcontrol_destroy_house(struct char_data * ch, char *arg);
extern void House_save_control(void);
extern void redit_save_to_disk(int zone_num);
extern void redit_save_internally(struct descriptor_data *d);
extern int direzioneopposta(int dir);
extern int parse_dir_char(char c);
extern int parse_dir(char *st);
extern void domadragon(struct char_data *ch,struct char_data *vict);
extern void add_follower(struct char_data *ch, struct char_data *leader);

/*local function*/
void potion_list(struct char_data *ch);
void perform_mischia(struct char_data *ch, struct obj_data *strumento, char *argument);
struct obj_data * get_materiale_in_inv(struct char_data *ch, char *nome, int tipo, int materiale,
	int nummateriale, struct obj_data * list);
int chk_ingredienti(struct char_data *ch, int num, char *ingrediente);
int get_ingrediente(char *ingrediente);
int elimina_ingredienti(struct char_data *ch, int lista_ingredienti [MAX_ERBE_X_POZIONE][2]);
int is_frutta(char *arg);
void insegna_prof(struct char_data *ch, struct char_data *learner, int prof,int prob);


int what_object_conceria(char *obj,struct obj_data *target)
{
	int res=0;
	if (GET_OBJ_MAT_NUM(target)==1) {
		if (/*(GET_OBJ_MAT_NUM(target)==4) &&*/ (!str_cmp(obj, "borsa"))) res=1;
		if (!str_cmp(obj, "elmo")) res=4;
		if (!str_cmp(obj, "gambali")) res=5;
		if (!str_cmp(obj, "parabraccia")) res=6;
		if (!str_cmp(obj, "cintura")) res=3;
		if (!str_cmp(obj, "guanti")) res=8;
		if (!str_cmp(obj, "armatura")) res=7;
		if (!str_cmp(obj, "stivali")) res=10;
	}
	
	if (GET_OBJ_MAT_NUM(target)==2) {
		if (!str_cmp(obj, "vestito")) res=2;
		if (!str_cmp(obj, "mantello")) res=9;
	}
	
	if (GET_OBJ_MAT_NUM(target)==3) {
		if (!str_cmp(obj, "mantello")) res=9;
		if (!str_cmp(obj, "armatura")) res=7;
	}
	return  res;
}


#define LIV_PROF_INSEG 75
#define LIV_MAX_PROF_IMPAR 65
#define PROP_IMPAR 5

void insegna_prof(struct char_data *ch, struct char_data *learner, int prof,int prob)
{
	if (!learner || (prof>TOT_PROF_MESTIERI)) return;
	
	return; //SOSPESA
	
	if (IS_NPC(learner))
	{
		sprintf(buf,"%s, non e' in grado di apprendere nulla.\r\n", GET_NAME(learner));
		send_to_char(buf, ch);
		return;
	}
	
	if (!strcmp(ch->desc->host,learner->desc->host)){
		sprintf(buf,"Mmm, sembra quasi un multiplayer! :-).\r\n");
		send_to_char(buf, ch);
		return;
	}
	
	if (GET_PROF(ch,prof) < LIV_PROF_INSEG) {
		sprintf(buf,"Non sei in grado ancora di insegare il mestiere a %s.\r\n", GET_NAME(learner));
		send_to_char(buf, ch);
		sprintf(buf,"%s non puo' ancora insegnarti il mestiere.\r\n", GET_NAME(ch));
		send_to_char(buf, learner);
		return;
	}
	
	if (GET_PROF(learner, prof) < LIV_MAX_PROF_IMPAR) {
		sprintf(buf,"%s puo' solo imparare con l'esperienza.\r\n", GET_NAME(learner));
		send_to_char(buf, ch);
		sprintf(buf,"Puoi solo imparare con l'esperienza ora.\r\n");
		send_to_char(buf, learner);
		return;
	}
	
	sprintf(buf,"%s i tuoi insegnamenti e ti osserva lavorare attentamente.\r\n", GET_NAME(learner));
	send_to_char(buf, ch);
	sprintf(buf,"Ascolti gli insegnamenti di %s e lo osservi attentamente mentre lavora.\r\n", GET_NAME(ch));
	send_to_char(buf, learner);
	
	if (number(1,100) >= (prob + (0.5 * (GET_CHA(ch) +  GET_INT(learner) + GET_WIS(learner) - 30))))
	{
		GET_PROF(learner, prof)++;
		sprintf(buf,"%s comprende perfettamente cio' che cerchi di insegnarli.\r\n", GET_NAME(learner));
		send_to_char(buf, ch);
		sprintf(buf,"Comprendi perfettamente le nozioni di %s.\r\n", GET_NAME(ch));
		send_to_char(buf, learner);
	} else {
		sprintf(buf,"Non ti sembra che %s abbia capito come si lavora.\r\n", GET_NAME(learner));
		send_to_char(buf, ch);
		sprintf(buf,"Ti sembra complicato e molto difficile ....  bhoo!\r\n");
		send_to_char(buf, learner); 
	}  
}


//Calcola il Valore Nutrizionale dei Cibi
int calcola_val_nutrizionale(struct char_data *ch,struct obj_data *nuovo,struct obj_data *target) {
	//if ((GET_OBJ_MAT_NUM(target)==NAT_PESCE)&&(GET_OBJ_MAT_TYPE(target)==NATURA))
	//return ( ((GET_OBJ_VAL(nuovo, 0)+(GET_OBJ_WEIGHT(target))) * (GET_PROF(ch,PROF_CUCINARE)/(double) (200-GET_PROF(ch,PROF_CUCINARE))))+1);
	//else
	//return ( (GET_OBJ_VAL(nuovo, 0) * (GET_PROF(ch,PROF_CUCINARE)/(double) (200-GET_PROF(ch,PROF_CUCINARE))))+1);
	
	return ( (GET_PROF(ch,PROF_CUCINARE)/25) + (GET_OBJ_VAL(nuovo, 0)) );
}

//Calcola il Costo dei Cibi
int calcola_costo(struct char_data *ch,struct obj_data *nuovo,struct obj_data *target) {
	if ((GET_OBJ_MAT_NUM(nuovo)==NAT_PESCE)&&(GET_OBJ_MAT_TYPE(nuovo)==NATURA)){
		if (GET_OBJ_TYPE(nuovo)==ITEM_DA_CUOCERE)
			return ( GET_OBJ_WEIGHT(nuovo) * GET_OBJ_COST(nuovo) * 50);
		else if (GET_OBJ_TYPE(nuovo)==ITEM_FOOD)
			return ( ((( (GET_PROF(ch,PROF_CUCINARE)/100) + GET_OBJ_COST(nuovo) + GET_OBJ_VAL(nuovo, 0)) * 10) +  GET_PROF(ch,PROF_CUCINARE)));
	}	
	if ((GET_OBJ_MAT_NUM(nuovo)==PEL_CARNE)&&(GET_OBJ_MAT_TYPE(nuovo)==PELLI))
		if (GET_OBJ_TYPE(nuovo)!=ITEM_FOOD)
			return GET_OBJ_COST(nuovo);
	return (  (((GET_OBJ_VAL(nuovo, 0) + GET_OBJ_COST(nuovo)) * GET_PROF(ch,PROF_CUCINARE))/5) + ((100 + GET_PROF(ch,PROF_CUCINARE))/5)   );
}


ACMD(do_mescola)
{
	char arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	struct char_data *victim;
	struct obj_data *r1, *r2, *strumento, *nuovo;
	int caso;

	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	
	if (!(arg2 && arg3 && (*arg2) && (*arg3))){
		send_to_char ("Devi mescolare almeno 2 reagenti!\r\n", ch);
		return;
	}

	strumento = GET_EQ(ch, WEAR_HOLD);

	generic_find(arg2, FIND_OBJ_INV, ch, &victim, &r1);
	generic_find(arg3, FIND_OBJ_INV, ch, &victim, &r2);

	if (!(r1 && r2)){
		send_to_char ("Devi avere entrambi gli oggetti nel tuo inventario per poterli mescolare.\r\n", ch);
		return;
	}

	if (time(NULL)-ch->last_lavoro<0 && GET_LEVEL(ch)<LVL_IMMORT)
	{
		send_to_char("Hai appena smesso di lavorare... ti prendi una pausa sindacale...\r\n", ch);
		return;
	}

	/* Inserire qui tutte le possibili coppie di elementi con degli else if... */

	if (GET_OBJ_TYPE (r1) == ITEM_ERBA_PULITA && GET_OBJ_MAT_TYPE (r1) == ERBE && GET_OBJ_MAT_NUM (r1) == ERBA_CAFFE && GET_OBJ_VNUM(r2) == OBJVNUM_LIMONE){
		if (!strumento || GET_OBJ_TYPE(strumento)!=ITEM_BOCCETTA)
		{
			send_to_char("Devi avere in mano una boccetta per mescolare questi reagenti.\r\n", ch);
			return;
		}
		if (!ROOM_FLAGGED(ch->in_room, ROOM_LAB_ALCHIMIA))
		{
			send_to_char("Devi trovarti in un laboratorio di alchimia per poter eseguire questa rischiosa operazione.\r\n", ch);
			return;
		}

		if (check_proficienza(ch, PROF_ALCHIMIA, 200, 0))
		{
			nuovo = read_object(OBJVNUM_CAFFELIMONE, VIRTUAL);
			act("Mischiando i reagenti $n ricava $p.", FALSE, ch, nuovo, 0, TO_ROOM);
			act("Mischiando i reagenti ricavi $p.", FALSE, ch, nuovo, 0, TO_CHAR);
			obj_to_char(nuovo, ch);
			extract_obj(r1);
			extract_obj(r2);
			extract_obj(strumento);
		}
		else
		{
			act("Non riesci ad ottenere niente dai reagenti che hai usato, rendendoli inutilizzabili.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n non riesce ad ottenere niente dai reagenti che hai usato.", FALSE, ch, 0, 0, TO_ROOM);
			extract_obj(r1);
			extract_obj(r2);
			check_proficienza_improve(ch, PROF_ALCHIMIA, 3);
		}
		ch->last_lavoro=time(NULL);
	}

	/*Stoffe pulite e storione vivo*/
	else if (GET_OBJ_VNUM (r1) == OBJVNUM_PRODOTTO && GET_OBJ_MAT_TYPE (r1) == PELLI && GET_OBJ_MAT_NUM (r1) == 2 && GET_OBJ_VNUM (r2) == OBJVNUM_CIBO+CIBO_PESCE_FRESCO_STORIONE){
		if (!ROOM_FLAGGED(ch->in_room, ROOM_CONCERIA))
		{
			send_to_char("Devi trovarti in una conceria per poter mescolare questi ingredienti.\r\n", ch);
			return;
		}

		if (check_proficienza (ch, PROF_SARTO, 200, 0))
		{
			nuovo = read_object (OBJVNUM_BENDE_UNTE, VIRTUAL);
			act("Mischiando i reagenti $n ricava $p.", FALSE, ch, nuovo, 0, TO_ROOM);
			act("Mischiando i reagenti ricavi $p.", FALSE, ch, nuovo, 0, TO_CHAR);
			obj_to_char(nuovo, ch);
		}
		else
		{
			act("Non riesci ad ottenere niente dai reagenti che hai usato, rendendoli inutilizzabili.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n non riesce ad ottenere niente dai reagenti che hai usato.", FALSE, ch, 0, 0, TO_ROOM);
			check_proficienza_improve(ch, PROF_SARTO, 3);
		}
		extract_obj(r1);
		extract_obj(r2);
	}

	else{
		send_to_char ("Provi a mescolare i tuoi due ingredienti... chissa' che non ne esca qualcosa di buono!\r\n", ch);
		caso = number (1, 5);
		if (caso < 4){
			act ("Ehi guarda come bolle! Per Therion, bolle un po' troppo!", FALSE, ch, 0, 0, TO_CHAR);
			act ("&1La tua miscela esplode in un giubilio di luce e dolore!&0", FALSE, ch, 0, 0, TO_CHAR);
			act ("&1La miscela avventata di $n gli esplode in faccia!&0", FALSE, ch, 0, 0, TO_ROOM);
			damage(ch, ch, 40, TYPE_TRAPPOLE);
		}
		if (caso == 4){
			act ("&7Gli ingredienti iniziano ad agitarsi, frizzano, evaporano...&0", FALSE, ch, 0, 0, TO_CHAR);
			act ("&7&bTi stai intossicando con queste esalazioni!!&0", FALSE, ch, 0, 0, TO_CHAR);
			act ("&7L'incauta miscela tentata da $n inizia ad evaporare causandogli una grave intossicazione.&0", FALSE, ch, 0, 0, TO_ROOM);
			damage(ch, ch, 120, TYPE_TRAPPOLE);
		}
		if (caso == 5){
			act ("&6&bL'odore dolciastro dell'intruglio ti fa scivolare in un dolce mondo onirico...&0", FALSE, ch, 0, 0, TO_CHAR);
			act ("&3&bAvverti di essere giunto in un altro luogo, forse in un altro tempo. Al tuo risveglio ci penserai su.......&0", FALSE, ch, 0, 0, TO_CHAR);
			act ("&3&b$n scivola in un mondo onirico per svegliarsi chissa' dove...", FALSE, ch, 0, 0, TO_ROOM);
			spell_teleport (70, ch, ch, 0);
			GET_POS(ch) = POS_SLEEPING;
		}
	}

	ch->last_lavoro=time(NULL);
}


// Uso: lavora <oggetto> [obiettivo]
ACMD(do_lavora)
{
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char stato[MAX_INPUT_LENGTH];
	struct obj_data *strumento, *target, *nuovo = NULL, *target2 = NULL;
	struct char_data *victim, *learner = NULL;
	//int tipofrutta;
	int bitv, wobj;
	int tempcount = 0;
	int danno_medio, ac_medio;
	int percentuale=10;
	int num_posseduto, num_necessario, num_rovinato, num_distrutto;
	int num_richiesto=0;
	int i, j;
	int vnum = 0;
	int num_prodotti = 1;
	int bonusHIT = 0;
	int bonusDAM = 0;
	int subcapacita;
  //int uno=0;
	if (RIDING(ch))	{
		send_to_char("Non puoi lavorare mentre stai cavalcando.\r\n", ch);
		return;
	}
	if(GET_MOVE(ch)<2)
	{
	    send_to_char("Sei esausto....ti concedi una pausa sindacale.\r\n",ch);
	    return;
	}
	if (subcmd == SCMD_POZIONI)
	{
		potion_list(ch);
		return;
	}
	
	
	strumento = GET_EQ(ch, WEAR_HOLD);
	if (!strumento)
	{
		send_to_char("Non mi sembra che tu stia tenendo uno strumento.\r\n", ch);
		return;
	}
	
	if (subcmd == SCMD_MISCHIA)
	{
		perform_mischia(ch, strumento, argument);
		return;
	}
	
	argument = one_argument(argument, arg2);
	if (!*arg2)
	{
		send_to_char("Lavorare che cosa?\r\n", ch);
		return;
	}
	
	bitv = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &target);
	
	if (!target)
	{
		sprintf(buf, "Non vedi %s.\r\n", arg2);
		send_to_char(buf, ch);
		return;
	}
	
	if (  (!(   GET_OBJ_TYPE(target) == ITEM_PIETRA   
	         || GET_OBJ_TYPE(target) == ITEM_ALBERO 
	         || GET_OBJ_TYPE(target) == ITEM_MINERALE 
	         || GET_OBJ_TYPE(target) == ITEM_ERBA     
	         || GET_OBJ_TYPE(target) == ITEM_NATURALE 
	         || GET_OBJ_TYPE(target) == ITEM_ROCCIA 
	         || GET_OBJ_TYPE(target) == ITEM_PELLI  
				   || (GET_OBJ_TYPE(target) == ITEM_CONTAINER && IS_OBJ_STAT(target, ITEM_ISCORPSE))
				  )) 
			&& (bitv==FIND_OBJ_ROOM) ) {
		act ("Devi avere $p nel tuo inventario per poter lavorare.\r\n", FALSE, ch, target, 0, TO_CHAR);
		return;
	}
	
  // Insegamento by Shade
	one_argument(argument, arg3);
	if (*arg3 && (!str_cmp(arg3,"con"))) {
		argument = one_argument(argument, arg3);
		one_argument(argument, arg4);
		if (!(learner = get_char_room_vis(ch, arg4))) {
			send_to_char("Non c'e' nessuno con quel nome con te.\r\n", ch);
			return;
		}
	}
	
	if (time(NULL)-ch->last_lavoro<0 && GET_LEVEL(ch)<LVL_IMMORT)
	{
		send_to_char("Hai appena smesso di lavorare... ti prendi una pausa sindacale...\r\n", ch);
		return;
	}
	
	switch (GET_OBJ_TYPE(target))
	{
		case ITEM_PIETRA:
			if (GET_OBJ_TYPE(strumento)!=ITEM_PICCA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questo giacimento e' ormai esaurito.\r\n", ch);
				return;
			}

//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			
			if (check_proficienza(ch, PROF_MINATORE, 100, 0))
			{
				if (GET_OBJ_VAL(target, 1) == 1) 
				  GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) 
				  GET_OBJ_VAL(target, 1)--;
				
				nuovo = read_object(OBJVNUM_METALLO_GREZZO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = PIETRE;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_PIETRE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, pietre[GET_OBJ_MAT_NUM(target)].max_slot * 2);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = 10;
				GET_OBJ_COST(nuovo)   = pietre[GET_OBJ_MAT_NUM(target)].val_comm * 0.10;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				
				sprintf(buf, "Una pietra grezza di %s e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "una pietra grezza di %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "pietra grezza metallo grezzo %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				manufatti_to_char(nuovo, ch, 1);
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner) 
				  insegna_prof(ch,learner,PROF_MINATORE, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_MINATORE, 2);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_MET_GREZZO:
			if (GET_OBJ_TYPE(strumento)!=ITEM_MARTELLO)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_FORGIA))
			{
				send_to_char("Devi trovarti in una forgia per poter lavorare i metalli.\r\n", ch);
				return;
			}
			
			num_richiesto = pietre[GET_OBJ_MAT_NUM(target)].n_grezzi;
			
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere un lingotto di %s ti servono almeno %d pietre grezze dello stesso materiale.\r\n",
					           pietre[GET_OBJ_MAT_NUM(target)].nome, num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_FABBRO, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, pietre[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0)
				{
					if (num_distrutto == 1) 
					  sprintf(buf, "Nel processo di creazione del lingotto, una pietra grezza si rivela inutilizzabile.\r\n");
					else 
					  sprintf(buf, "Nel processo di creazione del lingotto, %d pietre grezze si rivelano inutilizzabili.\r\n", num_distrutto);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				
				if (num_necessario > num_posseduto)
				{
					send_to_char("Poiche' non ti rimangono abbastanza pietre grezze sei costretto a fermarti.\r\n", ch);
					return;
				}
				
				nuovo = read_object(OBJVNUM_LINGOTTO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = PIETRE;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_PIETRE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, pietre[GET_OBJ_MAT_NUM(target)].max_slot * 1.3);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = pietre[GET_OBJ_MAT_NUM(target)].peso * 1.3;
				GET_OBJ_COST(nuovo)   = pietre[GET_OBJ_MAT_NUM(target)].val_comm * 0.50;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				
				sprintf(buf, "Un lingotto di %s e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "un lingotto di %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "lingotto %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				act("$n ricava $p lavorando le pietre grezze con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
				act("Ricavi $p lavorando le pietre grezze con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)
				  insegna_prof(ch,learner,PROF_FABBRO, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d pietre grezze di %s.\r\n", num_distrutto, pietre[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_FABBRO, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_LINGOTTO:
			if (GET_OBJ_TYPE(strumento)!=ITEM_MARTELLO)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_FABBRO))
			{
				send_to_char("Devi trovarti in un laboratorio da fabbro per poter lavorare i lingotti.\r\n", ch);
				return;
			}
			
			argument = one_argument(argument, arg3);
			argument = one_argument(argument, arg4);
			if (strlen(arg4)>0) 
			  percentuale = atoi(arg4);
			
			if (   (   str_cmp(arg3,"trave") 
			        && str_cmp(arg3,"scudo")
			        && str_cmp(arg3,"elmo") 
			        && str_cmp(arg3,"gambali")
			        && str_cmp(arg3,"parabraccia") 
			        && str_cmp(arg3, "armatura")
			        && str_cmp(arg3,"guanti")
			        && str_cmp(arg3, "stivali")
			        && str_cmp(arg3,"spada")
			        && str_cmp(arg3,"ascia")
			        && str_cmp(arg3, "pugnale")
			        && str_cmp(arg3, "lancia")
			        && str_cmp(arg3,"mazza")
			       ) 
			   || percentuale <=0 
			   || percentuale > 100
			  ) {
			  send_to_char("Uso: lavora <lingotto> <obiettivo> [percentuale]\r\n"
					          "Gli oggetti che si possono realizzare sono i seguenti:\r\n"
					          "-Scudo       (3) -Elmo       (3) -Gambali    (3) -Parabraccia (3)\r\n"
					          "-Armatura    (3) -Guanti     (3) -Stivali    (3)  \r\n"
					          "-Spada       (2) -Ascia      (2) -Pugnale    (1)  \r\n"
					          "-Lancia      (2) -Mazza      (2) -Trave      (4)  \r\n"
					          "Il numero tra parentesi indica il numero di lingotti necessario per ogni oggetto.\r\n"
					          "La percentuale (0-100) permette di realizzare oggetti meno potenti\r\n"
					          "ma piu' facili da usare per i principianti.\r\n"
					          "Sulle travi da costruzione la percentuale non viene presa in conto.\r\n", ch);
			  return;
			} 
			
			if (!str_cmp(arg3, "trave")) 
			  num_richiesto = 4;
			else if (!str_cmp(arg3, "pugnale")) 
			  num_richiesto=1;
			else if (   !str_cmp(arg3, "spada") 
			         || !str_cmp(arg3, "ascia") 
			         || !str_cmp(arg3, "lancia")
			         || !str_cmp(arg3, "mazza")
				      ) 
				num_richiesto = 2;
			else 
			  num_richiesto = 3;
			
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Ti servono almeno %d lingotti dello stesso materiale.\r\n", num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_FABBRO, 100, 0))
			{
				if (!str_cmp(arg3, "trave"))
				{
					nuovo = read_object(OBJVNUM_TRAVE_COST, VIRTUAL);
					GET_OBJ_TSLOTS(nuovo) = MIN(100, pietre[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = pietre[GET_OBJ_MAT_NUM(target)].peso * 1;
					GET_OBJ_COST(nuovo)   = pietre[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) GET_PROF(ch, PROF_FABBRO)*50/10000);
					sprintf(buf,  "Una trave da costruzione di %s e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf1, "una trave da costruzione di %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "trave costruzione %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
				}
				else if (   !str_cmp(arg3, "spada") // SPADA ED ALTRE ARMI
				         || !str_cmp(arg3, "ascia")
				         || !str_cmp(arg3, "mazza") 
				         || !str_cmp(arg3, "lancia")
				         || !str_cmp(arg3, "pugnale")
				        ) {
					danno_medio = (2 * pietre[GET_OBJ_MAT_NUM(target)].max_ac * GET_PROF(ch, PROF_FABBRO) * percentuale)/ 10000;
					danno_medio = MAX (0, MIN (danno_medio, 24));
					tempcount = (char) (GET_PROF(ch, PROF_FABBRO)/20);
					tempcount = MAX (0, MIN (5, tempcount));
					nuovo = read_object(OBJVNUM_WEAPON, VIRTUAL);
					GET_OBJ_TSLOTS(nuovo) = MIN(100, pietre[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = pietre[GET_OBJ_MAT_NUM(target)].peso * 1;
					GET_OBJ_COST(nuovo)   = pietre[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_FABBRO)*percentuale)/10000);
					GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_WIELD;
					GET_OBJ_VAL(nuovo, 1) = dadi_armi[danno_medio][0];
					GET_OBJ_VAL(nuovo, 2) = dadi_armi[danno_medio][1];
					
					if (!str_cmp(arg3, "spada")) {
						sprintf(buf,  "Una spada di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "una spada di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "spada %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}  
						GET_OBJ_VAL(nuovo, 3) = 10;  // Tipo danno
					}
	      // ASCIA
					if (!str_cmp(arg3, "ascia")) {
						sprintf(buf,  "Un'ascia di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un'ascia di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "ascia %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}  
						GET_OBJ_VAL(nuovo, 3) = 15;  // Tipo danno
					}
					
					if (!str_cmp(arg3, "pugnale")) {
						sprintf(buf,  "Un pugnale di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un pugnale di %s di %s fattura",pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "pugnale %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}  

						GET_OBJ_VAL(nuovo, 3) = 1;  // Tipo danno
						GET_OBJ_COST(nuovo)   = pietre[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_FABBRO)*percentuale)/10000);
					}
					
					if (!str_cmp(arg3, "mazza")) {
						sprintf(buf,  "Una mazza di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "una mazza di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "mazza %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}  
						GET_OBJ_VAL(nuovo, 3) = 9;  // Tipo danno
					}
					
					if (!str_cmp(arg3, "lancia")) {
						sprintf(buf,  "Una lancia di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "una lancia di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "lancia %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}  
						GET_OBJ_VAL(nuovo, 3) = 11;  // Tipo danno
					}
				}
				else { // ARMATURE
					ac_medio  = (pietre[GET_OBJ_MAT_NUM(target)].max_ac * GET_PROF(ch, PROF_FABBRO) * percentuale)/ 10000;
					ac_medio  = MAX (0, ac_medio);
					tempcount = (char) (GET_PROF(ch, PROF_FABBRO)/20);
					tempcount = MAX (0, MIN (5, tempcount));
					nuovo = read_object(OBJVNUM_ARMOR, VIRTUAL);
					GET_OBJ_TSLOTS(nuovo) = MIN(100, pietre[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = pietre[GET_OBJ_MAT_NUM(target)].peso * 1;
					GET_OBJ_COST(nuovo)   = pietre[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_FABBRO)*percentuale)/10000);
					GET_OBJ_VAL(nuovo, 0) = ac_medio;
					
					if (!str_cmp(arg3, "scudo")) { // SCUDO
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_SHIELD;
						sprintf(buf,  "Uno scudo di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "uno scudo di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "scudo %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
					else if (!str_cmp(arg3, "elmo")) { // ELMO
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HEAD;
						sprintf(buf,  "Un elmo di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un elmo di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "elmo %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = GET_OBJ_VAL(nuovo, 0);
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
					else if (!str_cmp(arg3, "gambali")) {  // GAMBALI	
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_LEGS;
						sprintf(buf,  "Dei gambali di %s di %s fattura sono qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "dei gambali di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "gambali %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = GET_OBJ_VAL(nuovo, 0);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
					else if (!str_cmp(arg3, "parabraccia"))	{    // PARABRACCIA
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_ARMS;
						sprintf(buf,  "Un parabraccia di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un parabraccia di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "parabraccia %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = GET_OBJ_VAL(nuovo, 0);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
					else if (!str_cmp(arg3, "armatura")) {     // ARMATURA
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_BODY;
						sprintf(buf,  "Un'armatura di %s di %s fattura sono qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "Un'armatura di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "armatura %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
					else if (!str_cmp(arg3, "guanti")) {      // GUANTI
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HANDS;
						sprintf(buf,  "Dei guanti di %s di %s fattura e' qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "dei guanti di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "guanti %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = GET_OBJ_VAL(nuovo, 0);
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
					else {   // STIVALI ED ALTRE ARMATURE INDEFINITE
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_FEET;
						sprintf(buf,  "Degli stivali di %s di %s fattura sono qui.", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "degli stivali di %s di %s fattura", pietre[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "stivali %s", pietre[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = GET_OBJ_VAL(nuovo, 0);
						
						if (GET_PROF(ch,PROF_FABBRO)==100) {
						  if (number(1,100)<15) {
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							  SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						  }
						}
					}
				}
				
				GET_OBJ_MAT_TYPE(nuovo) = PIETRE;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_PIETRE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				
				nuovo->description = str_dup(buf);
				nuovo->short_description = str_dup(buf1);
				nuovo->name = str_dup(buf2);
		/* GET_OBJ_LEVEL(nuovo) = MAX (1, objlevel(nuovo) * (100 + pietre[GET_OBJ_MAT_NUM(target)].bonus_livello) / 100);*/
				GET_OBJ_LEVEL(nuovo)=MAX(objlevel(nuovo)*((float)(3/4)),MIN(70,objlevel(nuovo)-number(1,10)));
				
				act("$n ricava $p lavorando i lingotti con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
				act("Ricavi $p lavorando i lingotti con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_richiesto);
				obj_to_char(nuovo, ch);
				//PEPPE ITEM RINOMINABILI
				if (IS_OBJ_STAT(nuovo,ITEM_RINOM_ALIAS)) {
				  send_to_char("Complimenti,hai creato un oggetto straordinario\r\nAdesso hai la possibilita' di personalizzarlo\r\n",ch);
				  send_to_char("Digita rinomina e ti sara spiegato come rinominare questa tua opera.\r\n",ch);
				} 
				check_danno_oggetto(ch, strumento, 1);
				if (learner) 
				  insegna_prof(ch,learner,PROF_FABBRO, PROP_IMPAR);
			}
			else {
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else {
					sprintf(buf, "Rovini irreparabilmente %d lingotti di %s.\r\n", num_distrutto, pietre[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_FABBRO, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_ALBERO:
			if (GET_OBJ_TYPE(strumento)!=ITEM_ASCIA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questa foresta e' ormai esaurita.\r\n", ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_FALEGNAMERIA, 100, 0)) {
				if (GET_OBJ_VAL(target, 1)==1) 
				  GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) 
				  GET_OBJ_VAL(target, 1)--;
				
				nuovo = read_object(OBJVNUM_TRONCO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = ALBERI;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_ALBERI-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 2);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 2.50;
				GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * 0.10;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				
				sprintf(buf, "Un tronco di %s e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "un tronco di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "tronco %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				manufatti_to_char(nuovo, ch, 1);
				
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner) 
				  insegna_prof(ch,learner,PROF_FALEGNAMERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_FALEGNAMERIA, 2);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_TRONCO:
			if (GET_OBJ_TYPE(strumento)!=ITEM_SEGA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_FALEGNAMERIA))
			{
				send_to_char("Devi trovarti in una falegnameria per poter lavorare i tronchi.\r\n", ch);
				return;
			}
			
			argument = one_argument(argument, arg3);
			
			if (str_cmp(arg3, "asse") && str_cmp(arg3, "trave"))
			{
				send_to_char("Uso: lavora <tronco> <obiettivo>\r\n"
					"Gli oggetti che si possono realizzare sono i seguenti:\r\n"
					"-TRAVE da costruzione\r\n"
					"-ASSE di legno da cui ricavare oggetti\r\n", ch);
				return;
			}
			
			num_richiesto = alberi[GET_OBJ_MAT_NUM(target)].n_grezzi;
			
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere una trave o un asse di %s ti servono almeno %d tronchi dello stesso legno.\r\n",
					alberi[GET_OBJ_MAT_NUM(target)].nome, num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			
			if (check_proficienza(ch, PROF_FALEGNAMERIA, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, pietre[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0) {
					if (num_distrutto == 1) 
					  sprintf(buf, "Nel processo di lavorazione, un tronco si rivela inutilizzabile.\r\n");
					else 
					  sprintf(buf, "Nel processo di lavorazione del tronco, %d tronchi si rivelano inutilizzabili.\r\n", num_distrutto);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				if (num_necessario > num_posseduto)
				{
					send_to_char("Poiche' non ti rimangono abbastanza tronchi sei costretto a fermarti.\r\n", ch);
					return;
				}
				
				if (!str_cmp(arg3, "trave"))
				{
					nuovo = read_object(OBJVNUM_TRAVE_LEGNO, VIRTUAL);
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1.30);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 1.50;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * 0.50;
					SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
					
					sprintf(buf,  "Una trave da costruzione e' qui.");
					sprintf(buf1, "una trave di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "trave %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
				}
				else                         // ASSE PER OTTENERE OGGETTI
				{
					nuovo = read_object(OBJVNUM_ASSE_LEGNO, VIRTUAL);
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1.30);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 1.30;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * 0.50;
					
					sprintf(buf,  "Un asse di %s e' qui.&0", alberi[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf1, "un asse da costruzione di %s&0", alberi[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "asse %s&0", alberi[GET_OBJ_MAT_NUM(target)].nome);
				}
				GET_OBJ_MAT_TYPE(nuovo) = ALBERI;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_ALBERI-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				
				nuovo->description = str_dup(buf);
				nuovo->short_description = str_dup(buf1);
				nuovo->name = str_dup(buf2);
				
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_FALEGNAMERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d tronchi di %s.\r\n", num_distrutto, alberi[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_FALEGNAMERIA, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
			
		case ITEM_ASSE_LEGNO:
			if (GET_OBJ_TYPE(strumento)!=ITEM_SEGA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_CARPENTERIA))
			{
				send_to_char("Devi trovarti in una carpenteria per poter lavorare il legno.\r\n", ch);
				return;
			}
			
			argument = one_argument(argument, arg3);
			argument = one_argument(argument, arg4);
			if (strlen(arg4)>0) 
			  percentuale = atoi(arg4);
			
			if (   (    str_cmp(arg3, "orecchino")   
			         && str_cmp(arg3, "amuleto") 
			         && str_cmp(arg3, "bracciale") 
			         && str_cmp(arg3, "bacchetta") 
			         && str_cmp(arg3, "anello") 
			         && str_cmp(arg3, "arco") 
			         && str_cmp(arg3, "scudo") 
			         && str_cmp(arg3, "frecce") 
			         && str_cmp(arg3, "porta") 
			         && str_cmp(arg3, "cassapanca") 
			         && str_cmp(arg3, "armadio") 
			         && str_cmp(arg3, "scrivania") 
			         && str_cmp(arg3, "tavolo") 
			         && str_cmp(arg3, "carretto") 
			         && str_cmp(arg3, "bastone")
			        ) 
			     || percentuale <=0 || percentuale > 100
			    ) {
				send_to_char("\r\n"
				  "Uso: lavora <asse> <obiettivo> [percentuale]\r\n"
					"Gli oggetti che si possono realizzare sono i seguenti:\r\n"
					"-Orecchino    x2 (1)   -Arco        (2)    -Cassapanca    (3)\r\n"
					"-Amuleto      x2 (1)   -Scudo       (2)    -Armadio       (3)\r\n"
					"-Bracciale    x2 (1)   -Frecce   x5 (1)    -Scrivania     (3)\r\n"
					"-Anello       x2 (1)   -Porta       (3)    -Tavolo        (3)\r\n"
					"-Bacchetta    x2 (1)   -Bastone  x2 (1)    -Carretto      (5)\r\n"
					"Il numero tra parentesi indica il numero dele assi necessarie per ogni oggetto.\r\n"
					"La percentuale (0-100) permette di realizzare oggetti meno potenti\r\n"
					"ma piu' facili da usare per i principianti.\r\n"
					"La dicitura x2 indica che vengono creati 2 oggetti.\r\n"
					"Sugli oggetti da costruzione la percentuale non viene presa in conto.\r\n",ch);
				
				return;
			}
			
			num_prodotti = 1;
			
			if (   !str_cmp(arg3, "orecchino") 
			    || !str_cmp(arg3, "amuleto") 
			    || !str_cmp(arg3, "bracciale") 
			    || !str_cmp(arg3, "anello") 
			    || !str_cmp(arg3, "bacchetta") 
			    || !str_cmp(arg3, "bastone")
			   ) 
			  num_prodotti = 2;
			
			if (!str_cmp(arg3, "frecce")) 
			  num_prodotti = 5;
			
			if (!str_cmp(arg3, "carretto")) 
			  num_richiesto = 5;
			
			else if (   !str_cmp(arg3, "porta") 
			         || !str_cmp(arg3, "cassapanca") 
			         || !str_cmp(arg3, "armadio")
				       || !str_cmp(arg3, "scrivania")
				       ||!str_cmp(arg3, "tavolo")
				      ) 
				num_richiesto = 3;
			
			else if (!str_cmp(arg3, "arco") || !str_cmp(arg3, "scudo")) 
			  num_richiesto = 2;
			
			else 
			  num_richiesto = 1;
			
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Ti servono almeno %d assi.\r\n", num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_CARPENTERIA, 100, 0))
			{
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				
				if (!str_cmp(arg3, "porta") || !str_cmp(arg3, "cassapanca") || !str_cmp(arg3, "armadio")
					||!str_cmp(arg3, "scrivania")||!str_cmp(arg3, "tavolo")||!str_cmp(arg3, "carretto"))
				{
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 3;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) GET_PROF(ch, PROF_CARPENTERIA)*50/10000);
					if (!str_cmp(arg3, "cassapanca") ||!str_cmp(arg3, "scrivania")) {
						GET_OBJ_TYPE(nuovo) = ITEM_CONTAINER;
						sprintf(buf,  "Una %s di %s e' qui.",arg3, alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "una %s di %s",arg3, alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "%s %s",arg3 , alberi[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = 3000;
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					} else
					if (!str_cmp(arg3, "armadio")) {
						GET_OBJ_TYPE(nuovo) = ITEM_CONTAINER;
						sprintf(buf,  "Un armadio di %s e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "un armadio di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "armadio %s" , alberi[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = 3000;
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					}
					
					if (!str_cmp(arg3, "porta")) {
						GET_OBJ_TYPE(nuovo) = ITEM_PORTA;
						sprintf(buf,  "Una porta di %s e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "una porta di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "porta %s" , alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					}
					
					if (!str_cmp(arg3, "tavolo")) {
						GET_OBJ_TYPE(nuovo) = ITEM_OTHER;
						sprintf(buf,  "Un tavolo di %s e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "un tavolo di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "tavolo %s" , alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					}
					
					if (!str_cmp(arg3, "carretto")) {
						GET_OBJ_TYPE(nuovo) = ITEM_CONTAINER;
						sprintf(buf,  "Un carretto di %s e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "un carretto di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "carretto %s" , alberi[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_WEAR(nuovo) = ITEM_WEAR_HANG;
						GET_OBJ_VAL(nuovo, 0) = 500;
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					}
					
				} else
				if (!str_cmp(arg3, "scudo")) {
					GET_OBJ_TYPE(nuovo) = ITEM_ARMOR;
					GET_OBJ_WEAR(nuovo) |= ITEM_WEAR_SHIELD;
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 0.8;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_CARPENTERIA)*percentuale)/10000);
					ac_medio    = (alberi[GET_OBJ_MAT_NUM(target)].max_ac * GET_PROF(ch, PROF_CARPENTERIA) * percentuale)/ 10000;
					ac_medio    = MAX (0, ac_medio);
					tempcount = (char) (GET_PROF(ch, PROF_CARPENTERIA)/20);
					tempcount = MAX (0, MIN (5, tempcount));
					GET_OBJ_VAL(nuovo, 0) = ac_medio;
					sprintf(buf,  "Uno scudo di %s e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf1, "uno scudo di %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "scudo %s" , alberi[GET_OBJ_MAT_NUM(target)].nome);
					GET_OBJ_LEVEL(nuovo) = MAX (1, objlevel(nuovo) * (100 + alberi[GET_OBJ_MAT_NUM(target)].bonus_livello) / 100);
					if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}				
				} else
				if (!str_cmp(arg3, "arco"))
				{
					GET_OBJ_TYPE(nuovo) = ITEM_BOW;
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 0.8;
					GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_WIELD;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_CARPENTERIA)*percentuale)/10000);
					GET_OBJ_VAL(nuovo, 0) = ((((GET_PROF(ch, PROF_CARPENTERIA)/100)+alberi[GET_OBJ_MAT_NUM(target)].peso))-3);
					
					tempcount = (char) (GET_PROF(ch, PROF_CARPENTERIA)/20);
					tempcount = MAX (0, MIN (5, tempcount));
					sprintf(buf,  "Un arco di %s di %s fattura e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
					sprintf(buf1, "un arco di %s di %s fattura", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
					sprintf(buf2, "arco %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
					GET_OBJ_LEVEL(nuovo) = MAX (1, (objlevel(nuovo) * ((90 + alberi[GET_OBJ_MAT_NUM(target)].bonus_livello+(GET_PROF(ch, PROF_CARPENTERIA))+percentuale) / 10))+alberi[GET_OBJ_MAT_NUM(target)].bonus_livello);
					if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
				} else
				if (!str_cmp(arg3, "frecce"))
				{
					GET_OBJ_TYPE(nuovo) = ITEM_ARROW;
					danno_medio = (2 * alberi[GET_OBJ_MAT_NUM(target)].max_ac * GET_PROF(ch, PROF_CARPENTERIA) * percentuale)/ 10000;
					danno_medio = MAX (0, MIN (danno_medio, 24));
					tempcount = (char) (GET_PROF(ch, PROF_CARPENTERIA)/20);
					tempcount = MAX (0, MIN (5, tempcount));
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 0.1;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * ((float) (num_richiesto/num_prodotti) + (float) (GET_PROF(ch, PROF_CARPENTERIA)*percentuale)/30000);
					GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HOLD;
					GET_OBJ_VAL(nuovo, 1) = dadi_armi[danno_medio][0];
					GET_OBJ_VAL(nuovo, 2) = dadi_armi[danno_medio][1];
					sprintf(buf,  "Una freccia di %s di %s fattura e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
					sprintf(buf1, "Una freccia  di %s di %s fattura", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
					sprintf(buf2, "freccia %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
					GET_OBJ_LEVEL(nuovo) = MAX (1, (objlevel(nuovo) * ((90 + alberi[GET_OBJ_MAT_NUM(target)].bonus_livello+(GET_PROF(ch, PROF_CARPENTERIA))+percentuale) / 10))+alberi[GET_OBJ_MAT_NUM(target)].bonus_livello);
					if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
				} else {
					GET_OBJ_TYPE(nuovo) = ITEM_TREASURE;
					GET_OBJ_TSLOTS(nuovo) = MIN(100, alberi[GET_OBJ_MAT_NUM(target)].max_slot * 1);
					GET_OBJ_WEIGHT(nuovo) = alberi[GET_OBJ_MAT_NUM(target)].peso * 0.3;
					GET_OBJ_COST(nuovo)   = alberi[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_CARPENTERIA)*percentuale)/10000);
					tempcount = (char) (GET_PROF(ch, PROF_CARPENTERIA)/20);
					tempcount = MAX (0, MIN (5, tempcount));
					if (!str_cmp(arg3, "orecchino")) {   //ORECCHINO
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_LOBSX;
						sprintf(buf,  "%s orecchino di %s e' qui.", aggioie[tempcount], alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "%s orecchino di %s", aggioie[tempcount], alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "orecchino %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					} else
					if (!str_cmp(arg3, "amuleto")) {
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_NECK;
						sprintf(buf,  "%s amuleto di %s e' qui.", aggioie[tempcount], alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "%s amuleto di %s", aggioie[tempcount], alberi[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "amuleto %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RINOM_DESCR);
						    }
						}
					} else
					if (!str_cmp(arg3, "bracciale")) {
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_WRIST;
						sprintf(buf,  "Un bracciale di %s di %s fattura e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un bracciale di %s di %s fattura", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "bracciale %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					} else 
					if (!str_cmp(arg3, "bacchetta")) {
						GET_OBJ_TYPE(nuovo) = ITEM_WAND;
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HOLD;
						GET_OBJ_VAL(nuovo, 0) = tempcount;
						sprintf(buf,  "Una bacchetta di %s di %s lavorazione e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "una bacchetta di %s di %s lavorazione", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "bacchetta %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					} else 
					if (!str_cmp(arg3, "bastone")) {
						GET_OBJ_TYPE(nuovo) = ITEM_STAFF;
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HOLD;
						GET_OBJ_VAL(nuovo, 0) = tempcount;sprintf(buf,  "Un bastone di %s di %s lavorazione e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un bastone di %s di %s lavorazione", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "bastone %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					} else 
					if (!str_cmp(arg3, "anello")) {
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_FINGER;
						sprintf(buf,  "Un anello di %s di %s fattura e' qui.", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un anello di %s di %s fattura", alberi[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "anello %s", alberi[GET_OBJ_MAT_NUM(target)].nome);
						if(GET_PROF(ch,PROF_CARPENTERIA)==100){
						    if(number(1,100)<15){
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
						    }
						}
					}
					
					GET_OBJ_LEVEL(nuovo) = MAX (1, objlevel(nuovo) * (100 + alberi[GET_OBJ_MAT_NUM(target)].bonus_livello) / 100);
				} //else freccie
				
				GET_OBJ_MAT_TYPE(nuovo) = ALBERI;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_ALBERI-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				nuovo->description = str_dup(buf);
				nuovo->short_description = str_dup(buf1);
				nuovo->name = str_dup(buf2);
				for (tempcount = 0;tempcount<num_prodotti;tempcount++){
					act("$n ricava $p lavorando le assi con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
					act("Ricavi $p lavorando le assi con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				} //Chk
				elimina_oggetti_uguali(ch, target, num_richiesto);
				manufatti_to_char(nuovo, ch, num_prodotti);
				check_danno_oggetto(ch, strumento, 1);
				//PEPPE ITEM RINOMINABILI
				if(IS_OBJ_STAT(nuovo,ITEM_RINOM_ALIAS)){
				    send_to_char("Complimenti,hai creato un oggetto straordinario\r\nAdesso hai la possibilita' di personalizzarlo\r\n",ch);
				    send_to_char("Digita rinomina e ti sara spiegato come rinominare questa tua opera.\r\n",ch);
				} 
				if (learner) 
				  insegna_prof(ch,learner,PROF_CARPENTERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d assi di %s.\r\n", num_distrutto, alberi[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_CARPENTERIA, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_MINERALE:
			if (GET_OBJ_TYPE(strumento)!=ITEM_PICCA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questo giacimento e' ormai esaurito.\r\n", ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_MINATORE, 100, 0))
			{
				if (GET_OBJ_VAL(target, 1)==1) GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) GET_OBJ_VAL(target, 1)--;
				nuovo = read_object(OBJVNUM_PIETRA_GREZZA, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = GEMME;
				GET_OBJ_MAT_NUM(nuovo) = MAX(0, MIN (NUM_MATERIALI_GEMME-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, gemme[GET_OBJ_MAT_NUM(target)].max_slot * 2);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = 10;
				GET_OBJ_COST(nuovo)   = gemme[GET_OBJ_MAT_NUM(target)].val_comm * 0.1;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Una pietra grezza di %s e' qui.", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "una pietra grezza di %s", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "pietra grezza minerale %s", gemme[GET_OBJ_MAT_NUM(target)].alias);
				nuovo->name = str_dup(buf);
				manufatti_to_char(nuovo, ch, 1);
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_MINATORE, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_MINATORE, 2);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_PIETRA_GREZ:
			if (GET_OBJ_TYPE(strumento)!=ITEM_MARTELLETTO)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (!ROOM_FLAGGED(ch->in_room, ROOM_PIETRE_PREZIOSE))
			{
				send_to_char("Devi trovarti in una stanza atta alla pulizia delle pietre preziose per poter lavorare i minerali preziosi.\r\n", ch);
				return;
			}
			num_richiesto = gemme[GET_OBJ_MAT_NUM(target)].n_grezzi;
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere una pietra pulita di %s ti servono almeno %d pietre grezze dello stesso materiale.\r\n",
					gemme[GET_OBJ_MAT_NUM(target)].nome, num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1; 
			if (check_proficienza(ch, PROF_GIOIELLERIA, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, gemme[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0)
				{
					if (num_distrutto == 1) sprintf(buf, "Nel processo di lavorazione, una pietra grezza si rivela inutilizzabile.\r\n");
					else sprintf(buf, "Nel processo di lavorazione, %d pietre grezze si rivelano inutilizzabili.\r\n", num_distrutto);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				if (num_necessario > num_posseduto)
				{
					send_to_char("Poiche' non ti rimangono abbastanza pietre grezze sei costretto a fermarti.\r\n", ch);
					return;
				}
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = GEMME;
		//GET_OBJ_TYPE(nuovo) = ITEM_PIETRA_PULI;
				GET_OBJ_TYPE(nuovo) = ITEM_GEMMA;
				GET_OBJ_MAT_NUM(nuovo) = MAX(0, MIN(NUM_MATERIALI_GEMME-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, gemme[GET_OBJ_MAT_NUM(target)].max_slot * 1.3);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = gemme[GET_OBJ_MAT_NUM(target)].peso * 2;
				GET_OBJ_COST(nuovo)   = gemme[GET_OBJ_MAT_NUM(target)].val_comm * 0.50;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Una pietra pulita di %s e' qui.", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "una pietra pulita di %s", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "pietra pulita %s", gemme[GET_OBJ_MAT_NUM(target)].alias);
				nuovo->name = str_dup(buf);
				act("$n ricava $p lavorando le pietre grezze con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
				act("Ricavi $p lavorando le pietre grezze con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner) 
				  insegna_prof(ch,learner,PROF_GIOIELLERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d pietre grezze di %s.\r\n", num_distrutto, gemme[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_GIOIELLERIA, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
//Gioielliere			
		case ITEM_GEMMA:
			if (GET_OBJ_TYPE(strumento) != ITEM_MARTELLETTO) {
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_GIOIELLERIA)) {
				send_to_char("Devi trovarti in una gioielleria per poter lavorare le pietre.\r\n", ch);
				return;
			}
			
			argument = one_argument(argument, arg3);
			argument = one_argument(argument, arg4);
			
			if (!str_cmp(arg3, "incastona")) {
				if (!*arg4) {
					send_to_char("Per incastonare la pietra nell'oggetto: lavora <pietra> incastona <oggetto>.\r\n", ch);
					return;
				}
				percentuale =1;
			} 
			else
			  if (strlen(arg4)>0) 
			    percentuale = atoi(arg4);
			
			if (   (   str_cmp(arg3, "orecchino")   
			        && str_cmp(arg3, "amuleto") 
			        && str_cmp(arg3, "bracciale") 
			        && str_cmp(arg3, "anello") 
			        && str_cmp(arg3, "pietra") 
			        && str_cmp(arg3, "incastona")
			       ) 
			    || percentuale <=0 || percentuale > 100
			   ) {
				send_to_char("\r\n"
				  "Uso: lavora <pietra> <obiettivo> [percentuale]\r\n"
					"Gli oggetti che si possono realizzare sono i seguenti:\r\n"
					"-Orecchino                (2) \r\n"
					"-Amuleto                  (2) \r\n"
					"-Bracciale                (2) \r\n"
					"-Anello                   (2) \r\n"
					"-Pietra pura              (1) \r\n"
					"-Incastona nell oggetto   (1) \r\n"
					"Per creare oggetti serve anche un lingotto."
					"Il numero tra parentesi indica il numero di pietre necessarie per ogni oggetto.\r\n"
					"La percentuale (0-100) permette di realizzare oggetti meno potenti\r\n"
					"ma piu' facili da usare per i principianti.\r\n"
					"Sulle pietre e sull'incastonamento da costruzione la percentuale non viene presa in conto.\r\n"
					"Per incastonare la pietra nell'oggetto: lavora <pietra> incastona <oggetto>", ch);
				
				return;
			}
			
			if (!str_cmp(arg3, "pietra") || !str_cmp(arg3, "incastona")) 
			  num_richiesto = 1;
			else 
			  num_richiesto = 2;
			
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Ti servono almeno %d pietre dello stesso tipo.\r\n", num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			
			if (str_cmp(arg3, "pietra") && str_cmp(arg3, "incastona")) {
				bitv = generic_find("lingotto", FIND_OBJ_INV, ch, &victim, &target2);
				if (!target2)
				{
					send_to_char("Per creare un oggetto ti serve anche un lingotto.\r\n", ch);
					return;
				}
			}
			
			if (!str_cmp(arg3, "incastona")) {
				bitv = generic_find(arg4, FIND_OBJ_INV, ch, &victim, &target2);
				if (!target2) {
					sprintf(buf,"Non hai %s.\r\n", arg4);
					send_to_char(buf, ch);
					return;
				}
				if (!CAN_WEAR(target2 ,ITEM_WEAR_WIELD)) {
					send_to_char("Non puoi incastonare la pietra su questo oggetto.\r\n", ch);
					return;
				}
				if (isname("pietrapreziosa",target2->name)) {
					send_to_char("Non puoi piu incastonare altre pietre preziose su quest'arma.\r\n", ch);
					return;
				}
			}
			//PEPPE MOVE
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_GIOIELLERIA, 100, 0)) {
				if (!str_cmp(arg3, "incastona")){
					//PEPPE MOV
//					GET_MOVE(ch)=GET_MOVE(ch)-1;
					if (check_proficienza(ch, PROF_GIOIELLERIA, 60, 0)) {
						for (i = 0; i < MAX_OBJ_AFFECT; i++) {
							if (target2->affected[i].location == APPLY_NONE || target2->affected[i].location == APPLY_HITROLL)
								break;
						} 
						for (j = 0; j < MAX_OBJ_AFFECT; j++){
							if ((target2->affected[j].location == APPLY_NONE && j!=i) || target2->affected[j].location == APPLY_DAMROLL)
								break;
						}

						switch (GET_OBJ_MAT_NUM(target)) {
							case GEM_DIAMANTE:
							case GEM_TOPAZIO: 
								bonusHIT = 1;
								bonusDAM = 0;					 
								break;
							case GEM_SMERALDO: 
							case GEM_RUBINO: 
								bonusHIT = 0;
								bonusDAM = 1;					 
								break;
							case GEM_ZAFFIRO: 
								bonusHIT = 1;
								bonusDAM = 1;					 
								break;
						}

						if ((i == MAX_OBJ_AFFECT && j == MAX_OBJ_AFFECT) || (bonusDAM == 0 && i == MAX_OBJ_AFFECT) || (bonusHIT == 0 && j == MAX_OBJ_AFFECT)){
							send_to_char("L'arma e' troppo potente perche' tu possa migliorarla.\r\n", ch);
							return;
						}

						if (bonusHIT > 0){
							if (i != MAX_OBJ_AFFECT){
								target2->affected[i].location = APPLY_HITROLL;
								target2->affected[i].modifier += bonusHIT;
								if (target2->affected[i].modifier == 0)
									target2->affected[i].location = APPLY_NONE;
							}
							else
								send_to_char ("L'arma e' troppo potente e non puo' godere dei pieni benefici della gemma.\r\n", ch);
						}
						if (bonusDAM > 0){
							if (j != MAX_OBJ_AFFECT){
								target2->affected[j].location = APPLY_DAMROLL;
								target2->affected[j].modifier += bonusDAM;
								if (target2->affected[j].modifier == 0)
									target2->affected[j].location = APPLY_NONE;
							}
							else
								send_to_char ("L'arma e' troppo potente e non puo' godere dei pieni benefici della gemma.\r\n", ch);
						}

						sprintf(buf, "%s pietrapreziosa", target2->name);
						target2->name = str_dup(buf);
						
						SET_BIT(GET_OBJ_EXTRA(target2), ITEM_RESTRING);
						GET_OBJ_COST(target2) = (GET_OBJ_COST(target2) * 2) + gemme[GET_OBJ_MAT_NUM(target)].val_comm * (0.5 + (float) GET_PROF(ch, PROF_GIOIELLERIA)*50/10000);

						act("$n incastra una pietra preziosa nell'arma.", FALSE, ch, 0, 0, TO_ROOM);
						act("Incastri la pietra preziosa nell'arma.", FALSE, ch, 0, 0, TO_CHAR);		
					} 
					else {
						act("$n rovina la gemma che stava incastonando.", FALSE, ch, 0, 0, TO_ROOM);
						act("Rovini la gemma nella fase piu' delicata del lavoro.", FALSE, ch, 0, 0, TO_CHAR);
					}
					
					elimina_oggetti_uguali(ch, target, 1);
					check_danno_oggetto(ch, strumento, 1);			 
				} 
				else {
					nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
					if (!str_cmp(arg3, "pietra"))
					{
						GET_OBJ_TYPE(nuovo) = ITEM_TREASURE;
						GET_OBJ_TSLOTS(nuovo) = MIN(100, gemme[GET_OBJ_MAT_NUM(target)].max_slot * 1);
						GET_OBJ_WEIGHT(nuovo) = gemme[GET_OBJ_MAT_NUM(target)].peso * 1;
						GET_OBJ_COST(nuovo)   = gemme[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) GET_PROF(ch, PROF_GIOIELLERIA)*50/10000);
						sprintf(buf,  "Una pietra preziosa di %s e' qui.", gemme[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf1, "una pietra preziosa di %s", gemme[GET_OBJ_MAT_NUM(target)].nome);
						sprintf(buf2, "%s pietra preziosa", gemme[GET_OBJ_MAT_NUM(target)].alias);
					}
					else
					{
						GET_OBJ_TYPE(nuovo) = ITEM_TREASURE;
						GET_OBJ_TSLOTS(nuovo) = MIN(100, gemme[GET_OBJ_MAT_NUM(target)].max_slot * 1);
						GET_OBJ_WEIGHT(nuovo) = gemme[GET_OBJ_MAT_NUM(target)].peso * 1;
						GET_OBJ_COST(nuovo)   = gemme[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_GIOIELLERIA)*percentuale)/10000);
						GET_OBJ_COST(nuovo)   += GET_OBJ_COST(target2); // valore lingotto
						ac_medio    = (gemme[GET_OBJ_MAT_NUM(target)].max_ac * GET_PROF(ch, PROF_GIOIELLERIA) * percentuale)/ 10000;
						ac_medio    = MAX (0, ac_medio);
						tempcount = (char) (GET_PROF(ch, PROF_GIOIELLERIA)/20);
						tempcount = MAX (0, MIN (5, tempcount));
						//GET_OBJ_VAL(nuovo, 0) = ac_medio;
						GET_OBJ_VAL(nuovo, 0) = 0;
						
						if (!str_cmp(arg3, "orecchino")) {   //ORECCHINO
							GET_OBJ_WEAR(nuovo) |= ITEM_WEAR_LOBSX;
							
							sprintf(buf,  "%s orecchino di %s e' qui.", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf1, "%s orecchino di %s", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf2, "orecchino %s", gemme[GET_OBJ_MAT_NUM(target)].alias);
							if(GET_PROF(ch,PROF_GIOIELLERIA)==100){
							    if(number(1,100)<15){
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
							    }	
							}
						} 
						else
						  if (!str_cmp(arg3, "amuleto")) {
							GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_NECK;
							sprintf(buf,  "%s amuleto di %s e' qui.", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf1, "%s amuleto di %s", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf2, "amuleto %s", gemme[GET_OBJ_MAT_NUM(target)].alias);
							if(GET_PROF(ch,PROF_GIOIELLERIA)==100){
							    if(number(1,100)<15){
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
							    }	
							}					
						} 
						else
						if (!str_cmp(arg3, "bracciale")) {
							GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_WRIST;
							sprintf(buf,  "%s bracciale di %s e' qui.", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf1, "%s bracciale di %s", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf2, "bracciale %s", gemme[GET_OBJ_MAT_NUM(target)].alias);
							if(GET_PROF(ch,PROF_GIOIELLERIA)==100) {
							    if(number(1,100)<15) {
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
							    }	
							}
						} 
						else {
							GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_FINGER;
							sprintf(buf,  "%s anello di %s e' qui.", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf1, "%s anello di %s", aggioie[tempcount], gemme[GET_OBJ_MAT_NUM(target)].nome);
							sprintf(buf2, "anello %s", gemme[GET_OBJ_MAT_NUM(target)].alias);
							if(GET_PROF(ch,PROF_GIOIELLERIA)==100){
							    if(number(1,100)<15){
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_ALIAS);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_NAME);
							    SET_BIT(GET_OBJ_EXTRA(nuovo),ITEM_RINOM_DESCR);
							    }	
							}
						}
						
						GET_OBJ_LEVEL(nuovo) = MAX (1, objlevel(nuovo) * (100 + gemme[GET_OBJ_MAT_NUM(target)].bonus_livello) / 100);
					}
					GET_OBJ_MAT_TYPE(nuovo) = GEMME;
					GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_GEMME-1, GET_OBJ_MAT_NUM(target)));
					GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
					GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
					SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
					nuovo->description = str_dup(buf);
					nuovo->short_description = str_dup(buf1);
					nuovo->name = str_dup(buf2);
					act("$n ricava $p lavorando le pietre preziose con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
					act("Ricavi $p lavorando le pietre preziose con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
					elimina_oggetti_uguali(ch, target, num_richiesto);
					if (target2 && str_cmp(arg3, "incastona")) elimina_oggetti_uguali(ch, target2, 1);
					obj_to_char(nuovo, ch);
					check_danno_oggetto(ch, strumento, 1);
					//PEPPE ITEM RINOMINABILI
				if(IS_OBJ_STAT(nuovo,ITEM_RINOM_ALIAS)){
				    send_to_char("Complimenti,hai creato un oggetto straordinario\r\nAdesso hai la possibilita' di personalizzarlo\r\n",ch);
				    send_to_char("Digita rinomina e ti sara spiegato come rinominare questa tua opera.\r\n",ch);
				} 
				}//incast
				if (learner) 
				  insegna_prof(ch,learner,PROF_GIOIELLERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d pietre preziose di %s.\r\n", num_distrutto, gemme[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_GIOIELLERIA, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;

	/* Nuovo passaggio By Spini:
	 * A cosa servono le pietre preziose? Ad essere polverizzate.
	 * E noi sappiamo bene come la polvere di pietre preziose sia
	 * molto utile!
	 */
		case ITEM_TREASURE:
			if (GET_OBJ_TYPE(strumento) != ITEM_MARTELLETTO) {
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_GIOIELLERIA)) {
				send_to_char("Devi trovarti in una gioielleria per poter lavorare le pietre.\r\n", ch);
				return;
			}

			if (GET_OBJ_VNUM(target) != 30){
				send_to_char ("Non puoi polverizzare un qualsiasi oggetto prezioso, deve essere una pura pietra!\r\n", ch);
				return;
			}

			argument = one_argument (argument, arg3);

			if (strcmp (arg3, "polvere")){
				send_to_char ("Uso:  lavora <pietra> polvere\r\n", ch);
				return;
			}

			if (check_proficienza (ch, PROF_GIOIELLERIA, 200, 0))
			{
				nuovo = read_object (OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_TYPE(nuovo) = ITEM_POLVERE;
				GET_OBJ_WEAR(nuovo) |= ITEM_WEAR_HOLD;
				GET_OBJ_MAT_TYPE(nuovo) = GET_OBJ_MAT_TYPE (target);
				GET_OBJ_MAT_NUM(nuovo) = GET_OBJ_MAT_NUM (target);
				GET_OBJ_TSLOTS(nuovo) = 10;
				GET_OBJ_CSLOTS(nuovo) = 10;
				GET_OBJ_WEIGHT(nuovo) = 1;
				GET_OBJ_COST(nuovo) = GET_OBJ_COST(target) / 2;
				GET_OBJ_RENT(nuovo) = GET_OBJ_RENT(target) / 2;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf (buf, "Della polvere di %s e' qui.", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "della polvere di %s", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "polvere %s", gemme[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup (buf);
				manufatti_to_char(nuovo, ch, 1);
				act("I tuoi sforzi sono ripagati con $p!", FALSE, ch, nuovo, 0, TO_CHAR);
				sprintf (buf, "Lavorando $p con gran maestria $n ottiene della bellissima polvere di %s.", gemme[GET_OBJ_MAT_NUM(nuovo)].nome);
				act(buf, FALSE, ch, target, 0, TO_ROOM);
				elimina_oggetti_uguali (ch, target, 1);
				if (learner)
				  insegna_prof(ch,learner,PROF_GIOIELLERIA, PROP_IMPAR);
			}
			else{
				act("Lavorando $p ottieni solo un inutile mucchio di rottami.", FALSE, ch, target, 0, TO_CHAR);
				act("$n distrugge $p con poca abilita' rendendolo inutile.", FALSE, ch, target, 0, TO_ROOM);
				elimina_oggetti_uguali (ch, target, 1);
				check_proficienza_improve(ch, PROF_GIOIELLERIA, 2);
				check_danno_oggetto(ch, strumento, 0.1);
			}

		break;
			
		case ITEM_ROCCIA:
			if (GET_OBJ_TYPE(strumento)!=ITEM_PICCA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questo giacimento e' ormai esaurito.\r\n", ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_MINATORE, 100, 0))
			{
				if (GET_OBJ_VAL(target, 1)==1) GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) GET_OBJ_VAL(target, 1)--;
				nuovo = read_object(OBJVNUM_ROCCIA_GREZZA, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = ROCCE;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_ROCCE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, rocce[GET_OBJ_MAT_NUM(target)].max_slot * 2);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = rocce[GET_OBJ_MAT_NUM(target)].peso * 2;
				GET_OBJ_COST(nuovo)   = rocce[GET_OBJ_MAT_NUM(target)].val_comm * 0.1;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Una pietra grezza di %s e' qui.", rocce[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "una pietra grezza di %s", rocce[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "pietra grezza roccia %s", rocce[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				manufatti_to_char(nuovo, ch, 1);
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)
				  insegna_prof(ch,learner,PROF_MINATORE, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_MINATORE, 2);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_ROCCIA_GREZ:
			if (GET_OBJ_TYPE(strumento)!=ITEM_MART_COSTR)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (!ROOM_FLAGGED(ch->in_room, ROOM_SQUADRATURA))
			{
				send_to_char("Devi trovarti in una stanza atta alla squadratura per poter lavorare i minerali da costruzione.\r\n", ch);
				return;
			}
			num_richiesto = rocce[GET_OBJ_MAT_NUM(target)].n_grezzi;
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere una pietra suqadrata di %s ti servono almeno %d pietre grezze dello stesso materiale.\r\n",
					rocce[GET_OBJ_MAT_NUM(target)].nome, num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_ARCHITETTURA, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, rocce[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0)
				{
					if (num_distrutto == 1) sprintf(buf, "Nel processo di squadratura, una pietra grezza si rivela inutilizzabile.\r\n");
					else sprintf(buf, "Nel processo di squadratura, %d pietre grezze si rivelano inutilizzabili.\r\n", num_distrutto);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				if (num_necessario > num_posseduto)
				{
					send_to_char("Poiche' non ti rimangono abbastanza pietre grezze sei costretto a fermarti.\r\n", ch);
					return;
				}
				nuovo = read_object(OBJVNUM_ROCCIA_SQUADRATA, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = ROCCE;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_ROCCE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, rocce[GET_OBJ_MAT_NUM(nuovo)].max_slot * 1.3);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = rocce[GET_OBJ_MAT_NUM(nuovo)].peso * 1.4;
				GET_OBJ_COST(nuovo)   = rocce[GET_OBJ_MAT_NUM(nuovo)].val_comm * num_richiesto;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Una pietra squadrata di %s e' qui.", rocce[GET_OBJ_MAT_NUM(nuovo)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "una pietra squadrata di %s", rocce[GET_OBJ_MAT_NUM(nuovo)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "pietra squadrataa %s", rocce[GET_OBJ_MAT_NUM(nuovo)].nome);
				nuovo->name = str_dup(buf);
				act("$n ricava $p lavorando le pietre grezze con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
				act("Ricavi $p lavorando le pietre grezze con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_ARCHITETTURA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d pietre grezze di %s.\r\n", num_distrutto, rocce[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_ARCHITETTURA, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_NATURALE:
			if ((GET_OBJ_MAT_NUM(target) == NAT_FRUTTA) && (GET_PROF(ch,PROF_COLTIVARE) == 0))
			{
				send_to_char("Non sai come raccogliere la frutta.\r\n", ch);
				return;
			}
			
			if ((GET_OBJ_MAT_NUM(target) != NAT_FRUTTA) && (GET_OBJ_TYPE(strumento)!=ITEM_FALCE))
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questa risorsa e' ormai esaurita.\r\n", ch);
				return;
			}
			
			if (GET_OBJ_MAT_NUM(target) == NAT_PESCE) //se e' di tipo pesce
			{
				send_to_char("Prova a pescarlo.\r\n", ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_COLTIVARE, 100, 0))
			{
				if (GET_OBJ_VAL(target, 1)==1) GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) GET_OBJ_VAL(target, 1)--;
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = NATURA;
				GET_OBJ_TYPE(nuovo) = ITEM_PRODOTTO;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_NATURA-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, natura[GET_OBJ_MAT_NUM(target)].max_slot * 2);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = natura[GET_OBJ_MAT_NUM(target)].peso * 2.5;
				GET_OBJ_COST(nuovo)   = natura[GET_OBJ_MAT_NUM(target)].val_comm * 0.1;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Un carico di %s da ripulire e' qui.", natura[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "una carico di %s da ripulire", natura[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "prodotto naturale carico %s ripulire", natura[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				manufatti_to_char(nuovo, ch, 1);
				act("$n raccoglie $p.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Raccogli $p.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner) 
				  insegna_prof(ch,learner,PROF_COLTIVARE, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_COLTIVARE, 2);
				if (GET_OBJ_MAT_NUM(target) != NAT_FRUTTA)
					check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		/*scuoiatore by Lance*/
		case ITEM_PELLI:
			
			//UNO
			if (GET_OBJ_TYPE(strumento)!=ITEM_CACCIA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questa risorsa e' ormai esaurita.\r\n", ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch,PROF_CACCIARE , 100, 0))
			{
				if (GET_OBJ_VAL(target, 1)==1) GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) GET_OBJ_VAL(target, 1)--;
				// ?? CHE CONTROLLO E' ?? 
				//if (MAX (0, MIN (NUM_MATERIALI_PELLI-1, GET_OBJ_MAT_NUM(target)))) {
				if (GET_OBJ_MAT_NUM(target)==PEL_CARNE){
					switch (GET_OBJ_VAL(target, 2)) {
						case 0: // Buona
							vnum = OBJVNUM_CIBO + CIBO_CARNE_FRESCA_BUONA;
							break;
						case 1: // Ottima
							vnum = OBJVNUM_CIBO + CIBO_CARNE_FRESCA_OTTIMA;
							break;
						case 2: // Eccellente
							vnum = OBJVNUM_CIBO + CIBO_CARNE_FRESCA_ECCELLENTE;
							break;
					}
					nuovo = read_object(vnum, VIRTUAL);
					if (nuovo) {
						//GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * 0.8;
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,NULL);
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						GET_OBJ_VAL(nuovo, 2) = GET_OBJ_VAL(target, 2);
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						manufatti_to_char(nuovo, ch, 1);
						act("$n scuoia $p.", FALSE, ch, nuovo, target, TO_ROOM);
						act("Scuoi $p.", FALSE, ch, nuovo, target, TO_CHAR);
						check_danno_oggetto(ch, strumento, 1);
						if (learner) 
						  insegna_prof(ch,learner,ITEM_CACCIA, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);						
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}
				}
				
				//Pelle Grezza
				
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_TYPE(nuovo) = ITEM_PELLE_GREZ;
				GET_OBJ_MAT_TYPE(nuovo) = PELLI;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_PELLI-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, pelli[GET_OBJ_MAT_NUM(target)].max_slot * 1);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * 0.8;
				GET_OBJ_COST(nuovo)   = pelli[GET_OBJ_MAT_NUM(target)].val_comm * 0.1;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				GET_OBJ_VAL(nuovo, 2)= GET_OBJ_VAL(target, 2);
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				
				if (GET_OBJ_MAT_NUM (nuovo)!=PEL_CARNE) {					
					sprintf(buf, "Una %s grezza e' qui.", pelli[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf1, "una %s grezza", pelli[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "%s grezza", pelli[GET_OBJ_MAT_NUM(target)].nome);
				}
				nuovo->description = str_dup(buf);
				nuovo->short_description = str_dup(buf1);
				nuovo->name = str_dup(buf2);
				manufatti_to_char(nuovo, ch, 1);
				act("$n scuoia $p.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Scuoi $p.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner) 
				  insegna_prof(ch,learner,ITEM_CACCIA, PROP_IMPAR);
				
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_CACCIARE, 2);
				if (GET_OBJ_MAT_NUM(target) != 3)
					check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_PELLE_GREZ:
			if (GET_OBJ_TYPE(strumento)!=ITEM_CACCIA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (!ROOM_FLAGGED(ch->in_room, ROOM_SCUOIATURA))
			{
				send_to_char("Devi trovarti in un stanza per la scuoiatura per poter trattare questo prodotto.\r\n", ch);
				return;
			}
			num_richiesto = pelli[GET_OBJ_MAT_NUM(target)].n_grezzi;
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere un carico di %s lavorata ti servono almeno %d carichi di %s fresca dello stesso tipo.\r\n",pelli[GET_OBJ_MAT_NUM(target)].nome,
					num_richiesto,pelli[GET_OBJ_MAT_NUM(target)].nome);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_CACCIARE, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, pelli[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0)
				{
					if (num_distrutto == 1) sprintf(buf, "Nella lavorazione, un carico di %s si rivela inutilizzabile.\r\n",
							pelli[GET_OBJ_MAT_NUM(target)].nome);
					else sprintf(buf, "Nella lavorazione, %d carichi di %s si rivelano inutilizzabili.\r\n", num_distrutto,
							pelli[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				if (num_necessario > num_posseduto)
				{
					sprintf(buf, "Poiche' non ti rimangono abbastanza carichi di %s sei costretto a fermarti.\r\n",
						pelli[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
					return;
				}
				
				// DUE
				if (GET_OBJ_MAT_NUM(target)==PEL_CARNE) {
					switch (GET_OBJ_VAL(target, 2)) {
						case 0: // Buona
							vnum = OBJVNUM_CIBO + CIBO_CARNE_PULITA_BUONA;
							break;
						case 1: // Ottima
							vnum = OBJVNUM_CIBO + CIBO_CARNE_PULITA_OTTIMA;
							break;
						case 2: // Eccellente
							vnum = OBJVNUM_CIBO + CIBO_CARNE_PULITA_ECCELLENTE;
							break;
					}
					nuovo = read_object(vnum, VIRTUAL);
					if (nuovo) {
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						//GET_OBJ_VAL(nuovo, 1) = pelli[GET_OBJ_MAT_NUM(target)].val_nutr * (0.50 + (GET_OBJ_VAL(target, 2)*0.05));
						GET_OBJ_VAL(nuovo, 2) = GET_OBJ_VAL(target, 2);	
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
						act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
						elimina_oggetti_uguali(ch, target, num_necessario);
						obj_to_char(nuovo, ch);
						check_danno_oggetto(ch, strumento, 1);
						if (learner)  
						  insegna_prof(ch,learner,PROF_CACCIARE, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);						
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}
				}
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = PELLI;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_PELLI-1, GET_OBJ_MAT_NUM(target)));
				
				
				
				if (GET_OBJ_MAT_NUM (nuovo)==PEL_CARNE)
					GET_OBJ_TYPE(nuovo) = ITEM_DA_CUOCERE;
				else
					GET_OBJ_TYPE(nuovo) = ITEM_PELLE_PULI;
				GET_OBJ_TSLOTS(nuovo) = MIN(100, pelli[GET_OBJ_MAT_NUM(target)].max_slot * 1);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * (0.80 + (GET_OBJ_VAL(target, 2)*0.05));
				GET_OBJ_COST(nuovo)   = pelli[GET_OBJ_MAT_NUM(target)].val_comm * (0.50 + (GET_OBJ_VAL(target, 2)*0.05));
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				GET_OBJ_VAL(nuovo, 1) = pelli[GET_OBJ_MAT_NUM(target)].val_nutr * (0.50 + (GET_OBJ_VAL(target, 2)*0.05));
				GET_OBJ_VAL(nuovo, 2)= GET_OBJ_VAL(target, 2);
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				if (GET_OBJ_MAT_NUM (nuovo)==PEL_CARNE) strcpy(stato,"da cucinare"); else
					strcpy(stato,"lavorata");
				if (GET_OBJ_MAT_NUM (nuovo)==PEL_CARNE) {
					sprintf(buf, "Un pezzo %s da cucinare e' qui.", pelli[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf1, "un pezzo di %s da cucinare", pelli[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "%s pezzo cucinare", pelli[GET_OBJ_MAT_NUM(target)].nome);
				}else{
					sprintf(buf, "Una %s lavorata e' qui.", pelli[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf1, "una %s lavorata", pelli[GET_OBJ_MAT_NUM(target)].nome);
					sprintf(buf2, "%s lavorata", pelli[GET_OBJ_MAT_NUM(target)].nome);
				}
				nuovo->description = str_dup(buf);
				nuovo->short_description = str_dup(buf1);
				nuovo->name = str_dup(buf2);
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_CACCIARE, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d carichi di %s.\r\n", num_distrutto, pelli[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_CACCIARE, 1 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
 	 /*************************  SARTO BY LANCE***********************************************/
			
		case ITEM_PELLE_PULI:
			if (GET_OBJ_TYPE(strumento)!=ITEM_STRUM_CONCERIA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (!ROOM_FLAGGED(ch->in_room, ROOM_CONCERIA))
			{
				send_to_char("Devi trovarti in una conceria per poter cucire dei vestiti, borse o armature in pelle.\r\n", ch);
				return;
			}
			argument = one_argument(argument, arg3);
 		//argument = one_argument(argument, arg4);
			half_chop(argument, argument, arg4);
			
			wobj = what_object_conceria(arg3, target);
			
			if ((wobj != 2) && (wobj != 1) && (strlen(arg3)>0)) percentuale = atoi(arg4);
			if (wobj==0 || percentuale < 0 || percentuale > 100)
			{
				send_to_char("Uso: cuci <prodotto> <obiettivo> + [percentuale]/<descrizione vestito>\r\n"
					"\r\n"
					"Esempio: cuci pelle cintura + 100\r\n"
					"oppure\r\n"
					"cuci pelle borsa + con uno stemma\r\n"
					"\r\n"
					
					"Gli oggetti che si possono realizzare sono i seguenti:\r\n",ch);
 				//PELLE E SCAGLIE
				if ((GET_OBJ_MAT_NUM(target)==1) || (GET_OBJ_MAT_NUM(target)==4))
					
					send_to_char("-Elmo        (3) -Gambali    (3) -Parabraccia (2)\r\n"
						"-Armatura    (4) -Guanti     (2) -Stivali     (3)\r\n"
						"-Cintura     (2)\r\n"
						,ch);
 			  //PELLE
				if (GET_OBJ_MAT_NUM(target)==1)
					send_to_char("-Borsa       (2)\r\n",ch);
 			  //STOFFA
				if (GET_OBJ_MAT_NUM(target)==2)
					send_to_char("-Vestito     (2) -Mantello   (3)\r\n",ch);
 			  //PELLICCIA
				if (GET_OBJ_MAT_NUM(target)==3)
					send_to_char("-Armatura    (4) -Mantello   (3)\r\n",ch);
				send_to_char("Il numero tra parentesi indica il numero di pelli necessario per ogni oggetto.\r\n"
					"La percentuale (0-100) permette di realizzare oggetti meno potenti\r\n"
					"ma piu' facili da usare per i principianti.\r\n"
					"Sugli oggetti in stoffa la percentuale non viene presa in conto ma\r\n"
					"e' sostituita dalla descrizione.\r\n", ch);
				return;
			}
			                         /*Messo da Jerold*/
			if (wobj==1) {
				percentuale = 10;
				if (!arg4 || !*arg4)
				{
					send_to_char("Metti una descrizione alla borsa. Uso: cuci <prodotto> <obiettivo> <descrizione> \r\n", ch);
					return;
				}
				if (strlen (arg4) > 180)
				{
					send_to_char("Descrizione della borsa troppo lunga.\r\n", ch);
					return;
				}
			}
			                         /*Fine Jerold */
			
			
			if (wobj==2) {
				percentuale = 10;
				if (!arg4 || !*arg4)
				{
					send_to_char("Metti una descrizione al vestito. Uso: cuci <prodotto> <obiettivo> <descrizione> \r\n", ch);
					return;
				}
				if (strlen (arg4) > 180)
				{
					send_to_char("Descrizione del vestito troppo lunga.\r\n", ch);
					return;
				}
			}
			
			if (wobj == 7)
				num_richiesto = 4;
			
			else if ((wobj == 10) || (wobj ==5) || (wobj ==4) || (wobj ==9)) num_richiesto = 3;
			else num_richiesto = 2;
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Ti servono almeno %d prodotti dello stesso materiale.\r\n", num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_SARTO, 100, 0))
			{
 			//dati comuni
				ac_medio    = pelli[GET_OBJ_MAT_NUM(target)].max_ac * GET_PROF(ch, PROF_SARTO) * percentuale / 10000;
				ac_medio    = MAX (0, ac_medio);
				tempcount = (char) (GET_PROF(ch, PROF_SARTO)/20);
				tempcount = MAX (0, MIN (5, tempcount));
				nuovo = read_object(OBJVNUM_ARMOR, VIRTUAL);
				GET_OBJ_TSLOTS(nuovo) = MIN(100, pelli[GET_OBJ_MAT_NUM(target)].max_slot * (0.9 + (GET_OBJ_VAL(target, 2)*0.05)));
				GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * (1 - (GET_OBJ_VAL(target, 2)*0.05));
				GET_OBJ_COST(nuovo)   = pelli[GET_OBJ_MAT_NUM(target)].val_comm * (num_richiesto + (float) (GET_PROF(ch, PROF_SARTO)*percentuale)/10000
						+ (GET_OBJ_VAL(target, 2)*0.05));
				GET_OBJ_VAL(nuovo, 0) = ac_medio;
				
				switch (wobj) {
					case 1:
						GET_OBJ_TYPE(nuovo) = ITEM_CONTAINER;
						GET_OBJ_WEAR(nuovo) |= ITEM_WEAR_SPALLE;
						GET_OBJ_TSLOTS(nuovo) = MIN(100, pelli[GET_OBJ_MAT_NUM(target)].max_slot * (0.9 + (GET_OBJ_VAL(target, 2)*0.05)));
						GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * 0.50;
						GET_OBJ_COST(nuovo)   = pelli[GET_OBJ_MAT_NUM(target)].val_comm * (0.1 + (float) (GET_PROF(ch, PROF_SARTO)*percentuale)/10000 + (GET_OBJ_VAL(target, 2)*0.05));
						//GET_OBJ_VAL(nuovo, 0) = 0;
					/*INIZO MESSO JEROLD */
						subcapacita = number(1, 100);
						if(GET_PROF(ch,PROF_SARTO)<50)
							GET_OBJ_VAL(nuovo, 0) =100;
						else if(GET_PROF(ch,PROF_SARTO)<75)
							GET_OBJ_VAL(nuovo, 0) =180;
						else if(GET_PROF(ch,PROF_SARTO)<99)
							GET_OBJ_VAL(nuovo, 0) =250;
						else if(subcapacita <90) {
							GET_OBJ_COST(nuovo)=(GET_OBJ_COST(nuovo)+subcapacita);
							GET_OBJ_VAL(nuovo, 0) =(350+subcapacita);}
						else if(subcapacita <99) {
							send_to_char("&2&bRiesci a sfruttare quasi al meglio tutta la stoffa!&0 \r\n", ch);
							GET_OBJ_COST(nuovo)=(GET_OBJ_COST(nuovo)+subcapacita);
							GET_OBJ_VAL(nuovo, 0) =(350+subcapacita);}
						else if(subcapacita==100){
							send_to_char("&1&bOttimo riesci a sfruttare al meglio tutta la stoffa!&0 \r\n", ch);
							GET_OBJ_COST(nuovo)=(GET_OBJ_COST(nuovo)+(2*subcapacita));
							GET_OBJ_VAL(nuovo, 0) =(350+subcapacita);}
						
						sprintf(buf,  "Una borsa %s di %s %s e di %s fattura e' qui.&0", arg4, qualita[GET_OBJ_VAL(target, 2)] ,pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "una borsa %s di %s %s e di %s fattura&0", arg4, qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome,fattura[tempcount]);
					/*FINE MESSO JEROLD */
						//sprintf(buf,  "Una borsa di %s %s e di %s fattura e' qui.", qualita[GET_OBJ_VAL(target, 2)] ,pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						//sprintf(buf1, "una borsa di %s %s e di %s fattura", qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome,fattura[tempcount]);
						sprintf(buf2, "borsa %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 2:
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_VESTE;
						sprintf(buf,  "Un vestito %s di %s %s e di %s fattura e' qui.", arg4 ,qualita[GET_OBJ_VAL(target, 2)],pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un vestito %s di %s %s e di %s fattura", arg4 ,qualita[GET_OBJ_VAL(target, 2)],pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "vestito %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_VAL(nuovo, 0) = 1;
						GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * (0.20 + (GET_OBJ_VAL(target, 2)*0.05));
						break;
					case 3:          //CINTURA
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_WAIST;
						sprintf(buf,  "Un cintura di %s %s e di %s fattura e' qui.", qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un cintura di %s %s e di %s fattura", qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "cintura %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 4:       // ELMO
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HEAD;
						sprintf(buf,  "Un elmo di %s %s e di %s fattura e' qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un elmo di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)],  pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "elmo %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 5:   // GAMBALI
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_LEGS;
						sprintf(buf,  "Dei gambali di %s %s e di %s fattura sono qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "dei gambali di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "gambali %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 6:   // PARABRACCIA
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_ARMS;
						sprintf(buf,  "Un parabraccia di %s %s e di %s fattura e' qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "un parabraccia di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)],  pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "parabraccia %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 7:      // ARMATURA
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_BODY;
						sprintf(buf,  "Un'armatura di %s %s e di %s fattura sono qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "Un'armatura di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "armatura %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 8:      // GUANTI
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_HANDS;
						sprintf(buf,  "Dei guanti di %s %s e di %s fattura e' qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "dei guanti di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "guanti %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					case 9:       // MANTELLO
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_ABOUT;
						sprintf(buf,  "Un mantello di %s %s e di %s fattura e' qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "Un mantello di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "mantello %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						GET_OBJ_WEIGHT(nuovo) = pelli[GET_OBJ_MAT_NUM(target)].peso * 0.2;
						break;
					case 10:                                     // STIVALI ED ALTRE ARMATURE INDEFINITE
						GET_OBJ_WEAR(nuovo)   |= ITEM_WEAR_FEET;
						sprintf(buf,  "Degli stivali di %s %s e di %s fattura sono qui.",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf1, "degli stivali di %s %s e di %s fattura",qualita[GET_OBJ_VAL(target, 2)], pelli[GET_OBJ_MAT_NUM(target)].nome, fattura[tempcount]);
						sprintf(buf2, "stivali %s", pelli[GET_OBJ_MAT_NUM(target)].nome);
						break;
					default:
						send_to_char("Oggetto non definito contattare un creatore.\r\n", ch);
						return;
						
				}
				GET_OBJ_MAT_TYPE(nuovo) = PELLI;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_PELLI-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				nuovo->description = str_dup(buf);
				nuovo->short_description = str_dup(buf1);
				nuovo->name = str_dup(buf2);
				GET_OBJ_LEVEL(nuovo) = MAX (1, objlevel(nuovo) * (100 + pelli[GET_OBJ_MAT_NUM(target)].bonus_livello) / 100);
				act("$n ricava $p lavorando i prodotti con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
				act("Ricavi $p lavorando i prodotti con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_richiesto);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_SARTO, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d prodotti di %s.\r\n", num_distrutto, pelli[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_SARTO, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
	/*Lance Erborista inizio*/
		case ITEM_ERBA:
			if (GET_OBJ_TYPE(strumento)!=ITEM_PICCOZZA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (GET_OBJ_VAL(target, 1) == -1)
			{
				send_to_char("Questa risorsa e' ormai esaurita.\r\n", ch);
				return;
			}
			if (check_proficienza(ch, PROF_ERBORISTERIA, 100, 0))
			{
				if (GET_OBJ_VAL(target, 1)==1) GET_OBJ_VAL(target, 1) = -1;
				else if (GET_OBJ_VAL(target, 1) !=0) GET_OBJ_VAL(target, 1)--;
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = ERBE;
				GET_OBJ_TYPE(nuovo) = ITEM_ERBA_GREZ;
				GET_OBJ_MAT_NUM(nuovo) = MAX (0, MIN (NUM_MATERIALI_ERBE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, erbe[GET_OBJ_MAT_NUM(target)].max_slot * 2);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = erbe[GET_OBJ_MAT_NUM(target)].peso * 1.5;
				GET_OBJ_COST(nuovo)   = erbe[GET_OBJ_MAT_NUM(target)].val_comm * 0.1;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Un carico di %s e' qui.", erbe[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "un carico di %s", erbe[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "prodotto erbe carico %s", erbe[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				manufatti_to_char(nuovo, ch, 1);
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_ERBORISTERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				check_proficienza_improve(ch, PROF_ERBORISTERIA, 2);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_ERBA_GREZ:
			if (GET_OBJ_TYPE(strumento)!=ITEM_TRITAERBE)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			if (!ROOM_FLAGGED(ch->in_room, ROOM_ERBORISTERIA))
			{
				send_to_char("Devi trovarti in un'erboristeria per poter ottenere delle erbe pulite.\r\n", ch);
				return;
			}
			num_richiesto = erbe[GET_OBJ_MAT_NUM(target)].n_grezzi;
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere un'erba pulita di %s ti servono almeno %d erbe grezze dello stesso tipo.\r\n",
					erbe[GET_OBJ_MAT_NUM(target)].nome, num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			if (check_proficienza(ch, PROF_ERBORISTERIA, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, erbe[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0)
				{
					if (num_distrutto == 1) sprintf(buf, "Nella pulitura, un'erba grezza si rivela inutilizzabile.\r\n");
					else sprintf(buf, "Nella pulitura, %d erbe grezze si rivelano inutilizzabili.\r\n", num_distrutto);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				if (num_necessario > num_posseduto)
				{
					send_to_char("Poiche' non ti rimangono abbastanza erbe grezze sei costretto a fermarti.\r\n", ch);
					return;
				}
				nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
				GET_OBJ_MAT_TYPE(nuovo) = ERBE;
				GET_OBJ_TYPE(nuovo) = ITEM_ERBA_PULITA;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_ERBE-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, erbe[GET_OBJ_MAT_NUM(target)].max_slot * 1.3);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = erbe[GET_OBJ_MAT_NUM(target)].peso * 1.2;
				GET_OBJ_COST(nuovo)   = erbe[GET_OBJ_MAT_NUM(target)].val_comm * 0.50;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				sprintf(buf, "Un mazzo d'erba pulita di %s e' qui.", erbe[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->description = str_dup(buf);
				sprintf(buf, "un mazzo d'erba pulita di %s", erbe[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->short_description = str_dup(buf);
				sprintf(buf, "erba pulita mazzo %s", erbe[GET_OBJ_MAT_NUM(target)].nome);
				nuovo->name = str_dup(buf);
				act("$n ricava $p lavorando $P con il suo strumento.", FALSE, ch, nuovo, target, TO_ROOM);
				act("Ricavi $p lavorando $P con il tuo strumento.", FALSE, ch, nuovo, target, TO_CHAR);
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_ERBORISTERIA, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d l'erba grezza di %s.\r\n", num_distrutto, erbe[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_ERBORISTERIA, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
									/*Lance Erborista fine */
			
	 /*Lance coltivatore e cuoco*/
		case ITEM_PRODOTTO:
			if (GET_OBJ_TYPE(strumento)!=ITEM_RIPULITURA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_MULINO_RIPUL))
			{
				send_to_char("Devi trovarti in un mulino per poter pulire gli alimenti o macinare i farinacei.\r\n", ch);
				return;
			}
			num_richiesto = natura[GET_OBJ_MAT_NUM(target)].n_grezzi;
			if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
			{
				sprintf(buf, "Per ottenere un prodotto pulito o macinato di %s ti servono almeno %d prodotti alimentari dello stesso tipo.\r\n",
					natura[GET_OBJ_MAT_NUM(target)].nome, num_richiesto);
				send_to_char(buf, ch);
				return;
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_COLTIVARE, 100, 0))
			{
				num_necessario = num_necessari(num_richiesto, natura[GET_OBJ_MAT_NUM(target)].riuscita);
				num_rovinato   = num_necessario - num_richiesto;
				num_distrutto  = MIN(num_rovinato, num_posseduto);
				if (num_distrutto >0)
				{
					if (num_distrutto == 1) sprintf(buf, "Nel processo di pulitura o macinazione del prodotto, un carico si rivela inutilizzabile.\r\n");
					else sprintf(buf, "Nel processo di pulitura o macinazione del prodotto, %d carichi si rivelano inutilizzabili.\r\n", num_distrutto);
					send_to_char(buf, ch);
					elimina_oggetti_uguali(ch, target, num_distrutto);
					return;
				}
				if (num_necessario > num_posseduto)
				{
					send_to_char("Poiche' non ti rimangono abbastanza carichi sei costretto a fermarti.\r\n", ch);
					return;
				}
				/* Fruttivendolo */
				if (GET_OBJ_MAT_NUM(target) == NAT_FRUTTA) { 
					//nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
					//GET_OBJ_TYPE(nuovo) = ITEM_FOOD;
					switch (dice(1,7)) {
						case 1: vnum = OBJVNUM_CIBO+CIBO_LAMPONE;
							break;							
						case 2: vnum = OBJVNUM_CIBO+CIBO_FRAGOLA;
							break;
						case 3: vnum = OBJVNUM_CIBO+CIBO_MELE;
							break;
						case 4: vnum = OBJVNUM_CIBO+CIBO_CILIEGIA;
							break;
						case 5: vnum = OBJVNUM_CIBO+CIBO_MORA;
							break;
						case 6: vnum = OBJVNUM_CIBO+CIBO_PESCA;
							break;
						case 7: vnum = OBJVNUM_CIBO+CIBO_ALBICOCCA;
							break;
						default: vnum = NOTHING;
					}
					nuovo = read_object(vnum, VIRTUAL);
					num_prodotti = 1;
					num_richiesto = 1;
					if (nuovo) {
						//Valore Nutritivo
						GET_OBJ_VAL(nuovo, 0) = calcola_val_nutrizionale(ch,nuovo,target);
						//Valore in Soldoni :-)
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);
						
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						for (tempcount = 0;tempcount<num_prodotti;tempcount++){
							act("$n ricava $p ripulendo il prodotto con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
							act("Ricavi $p ripulendo il prodotto con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
						}						
						manufatti_to_char(nuovo, ch, num_prodotti);
						elimina_oggetti_uguali(ch, target, num_richiesto);
						check_danno_oggetto(ch, strumento, 1);
						
						if (learner)  
						  insegna_prof(ch,learner,PROF_CUCINARE, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}			
				} else /* Vecchia Parte */{
					nuovo = read_object(OBJVNUM_PRODOTTO, VIRTUAL);
					GET_OBJ_TYPE(nuovo) = ITEM_DA_CUOCERE;
				}
				GET_OBJ_MAT_TYPE(nuovo) = NATURA;
				GET_OBJ_MAT_NUM (nuovo) = MAX (0, MIN (NUM_MATERIALI_NATURA-1, GET_OBJ_MAT_NUM(target)));
				GET_OBJ_TSLOTS(nuovo) = MIN(100, natura[GET_OBJ_MAT_NUM(target)].max_slot * 1);
				GET_OBJ_CSLOTS(nuovo) = GET_OBJ_TSLOTS(nuovo);
				GET_OBJ_WEIGHT(nuovo) = natura[GET_OBJ_MAT_NUM(target)].peso * 0.5;
				GET_OBJ_COST(nuovo)   = natura[GET_OBJ_MAT_NUM(target)].val_comm * 0.50;
				GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
				GET_OBJ_VAL(nuovo, 0) = natura[GET_OBJ_MAT_NUM(target)].val_nutr * 0.5;
				SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
				if (GET_OBJ_MAT_NUM(target)==NAT_FARINACEI) {
					sprintf(buf, "Un sacchetto di farina e' qui.");
					nuovo->description = str_dup(buf);
					sprintf(buf, "un sacchetto di farina");
					nuovo->short_description = str_dup(buf);
					sprintf(buf, "farina sacchetto");
					nuovo->name = str_dup(buf);
					act("$n ricava $p macinando un carico di farinacei.", FALSE, ch, nuovo, 0, TO_ROOM);
					act("Ricavi $p macinando un carico di farinacei.", FALSE, ch, nuovo, 0, TO_CHAR);
				} else if (GET_OBJ_MAT_NUM(target)==NAT_VERDURA) {
					sprintf(buf, "Un prodotto alimentare di %s e' qui.", natura[GET_OBJ_MAT_NUM(nuovo)].nome);
					nuovo->description = str_dup(buf);
					sprintf(buf, "un prodotto alimentare di %s", natura[GET_OBJ_MAT_NUM(nuovo)].nome);
					nuovo->short_description = str_dup(buf);
					sprintf(buf, "%s prodotto alimentare", natura[GET_OBJ_MAT_NUM(nuovo)].nome);
					nuovo->name = str_dup(buf);
					act("$n ricava $p ripulendo il prodotto con il suo strumento.", FALSE, ch, nuovo, 0, TO_ROOM);
					act("Ricavi $p ripulendo il prodotto con il tuo strumento.", FALSE, ch, nuovo, 0, TO_CHAR);
				}
				
				elimina_oggetti_uguali(ch, target, num_necessario);
				obj_to_char(nuovo, ch);
				check_danno_oggetto(ch, strumento, 1);
				if (learner)  
				  insegna_prof(ch,learner,PROF_COLTIVARE, PROP_IMPAR);
			}
			else
			{
				act("Non riesci ad ottenere niente da $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce ad ottenere niente da $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d carichi di %s.\r\n", num_distrutto, natura[GET_OBJ_MAT_NUM(target)].nome);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_COLTIVARE, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			// CUOCO
		case ITEM_DA_CUOCERE:
			if (!((GET_OBJ_MAT_TYPE(target) == NATURA) && (GET_OBJ_MAT_NUM(target)==0))
				&& (GET_OBJ_TYPE(strumento)!=ITEM_STRUM_CUCINA))
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if ((GET_OBJ_MAT_TYPE(target) == NATURA) && (GET_OBJ_MAT_NUM(target)==0) && (GET_OBJ_TYPE(strumento)!=ITEM_MATTARELLO))
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_CUCINA))
			{
				send_to_char("Devi trovarti in una cucina per poter cucinare gli alimenti.\r\n", ch);
				return;
			}
			
			if (GET_OBJ_MAT_TYPE(target)!=NATURA)
				if (!((GET_OBJ_MAT_TYPE(target)==PELLI) && (GET_OBJ_MAT_NUM(target)<3))) {
					send_to_char("Non lo puoi cucinare.\r\n", ch);
					return;
				}
				
			argument = one_argument(argument, arg3);
			
			num_prodotti = 1;
			num_richiesto = 1;
			
			
			if ((GET_OBJ_MAT_NUM(target)==NAT_FARINACEI) && (GET_OBJ_MAT_TYPE(target)==NATURA)) {
				
				if (is_abbrev(arg3, "pastella")) {num_richiesto = 3; num_prodotti = 1;}
				else if (is_abbrev(arg3, "pane")){num_richiesto = 3; num_prodotti = 1;}
				else if (is_abbrev(arg3, "pizza")){num_richiesto = 1; num_prodotti = 1;}				
				else if (is_abbrev(arg3, "pagnotta")){num_richiesto = 1; num_prodotti = 2;}				
				else if (is_abbrev(arg3, "focaccia")){num_richiesto = 1; num_prodotti = 2;}
				else if (is_abbrev(arg3, "focacciona")){num_richiesto = 2; num_prodotti = 1;}
				else {
					send_to_char("Uso: cucina <prodotto> <alimento> \r\n"
						"Gli alimenti che si possono realizzare sono i seguenti:\r\n"
						"-Pagnotta normale   x2\r\n"
						"-Focaccia           x2\r\n"
						"-Focacciona             (2)\r\n"
						"-Pizza\r\n"
						"-Pane da viaggio        (3)\r\n"
						"-Pastella               (3)\r\n"
						,ch);
					return;
				}
				
				if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
				{
					sprintf(buf, "Ti servono almeno %d sacchetti di farina.\r\n", num_richiesto);
					send_to_char(buf, ch);
					return;
				}
			}
			//PEPPE MOV
//			GET_MOVE(ch)=GET_MOVE(ch)-1;
			if (check_proficienza(ch, PROF_CUCINARE, 100, 0))
			{   
				// Inizio ProfCuoco V2
				/* FARINACEI */
				if ((GET_OBJ_MAT_NUM(target)==NAT_FARINACEI) && (GET_OBJ_MAT_TYPE(target)==NATURA)) {
					if (is_abbrev(arg3,"pagnotta")) 
						nuovo = read_object(OBJVNUM_CIBO+CIBO_PAGNOTTA, VIRTUAL);
					else if (is_abbrev(arg3,"focaccia"))
						nuovo = read_object(OBJVNUM_CIBO+CIBO_FOCACCIA, VIRTUAL);
					else if (is_abbrev(arg3,"pane"))
						nuovo = read_object(OBJVNUM_CIBO+CIBO_PANE, VIRTUAL);
					else if (is_abbrev(arg3,"pizza"))
						nuovo = read_object(OBJVNUM_CIBO+CIBO_PIZZA, VIRTUAL);
					else if (is_abbrev(arg3,"pastella"))
						nuovo = read_object(OBJVNUM_CIBO+CIBO_PASTELLA, VIRTUAL);
					else if (is_abbrev(arg3,"focacciona"))
						nuovo = read_object(OBJVNUM_CIBO+CIBO_FOCACCIONA, VIRTUAL);
					else 
						nuovo = NULL;
					
					if (nuovo) {
						
						//Valore Nutritivo
						GET_OBJ_VAL(nuovo, 0) = calcola_val_nutrizionale(ch,nuovo,target);
						//Valore in Soldoni :-)
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);						
						
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						for (tempcount = 0;tempcount<num_prodotti;tempcount++){
							act("$n cucina $p.", FALSE, ch, nuovo, 0, TO_ROOM);
							act("Cucini $p.", FALSE, ch, nuovo, 0, TO_CHAR);
						}						
						manufatti_to_char(nuovo, ch, num_prodotti);
						elimina_oggetti_uguali(ch, target, num_richiesto);
						check_danno_oggetto(ch, strumento, 1);						
						if (learner)  
						  insegna_prof(ch,learner,PROF_CUCINARE, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}
					
					
				} else
					/* PESCI */
				if ((GET_OBJ_MAT_NUM(target)==NAT_PESCE) && (GET_OBJ_MAT_TYPE(target)==NATURA)) {
					
					switch (GET_OBJ_VNUM(target)) {
						case 	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_ALBORELLA:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_ALBORELLA;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_GATTO:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_GATTO;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_BRANZINO:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_BRANZINO;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_TROTA:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_TROTA;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_LUCCIO:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_LUCCIO;
							break;				
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_STORIONE:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_STORIONE;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_TONNO:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_TONNO;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_SQUALO:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_SQUALO;
							break;
						case	OBJVNUM_CIBO+CIBO_PESCE_FRESCO_MAZZUGORU:
							vnum = OBJVNUM_CIBO+CIBO_PESCE_COTTO_MAZZUGORU;
							break;
						default: vnum = NOTHING;
					}
					nuovo = read_object(vnum, VIRTUAL);
					
					if (nuovo) {						
						//Valore Nutritivo
						GET_OBJ_VAL(nuovo, 0) = calcola_val_nutrizionale(ch,nuovo,target);
						//Valore in Soldoni :-)
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);
						
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						GET_OBJ_WEIGHT(nuovo) = GET_OBJ_WEIGHT(target);
						
						sprintf(buf1, "%s da %d kg", nuovo->short_description, GET_OBJ_WEIGHT(nuovo));
						nuovo->short_description = str_dup(buf1);
						sprintf(buf1, "%s da %d kg.", nuovo->description, GET_OBJ_WEIGHT(nuovo));
						nuovo->description = str_dup(buf1);
						
						for (tempcount = 0;tempcount<num_prodotti;tempcount++){
							act("$n cucina $p.", FALSE, ch, nuovo, 0, TO_ROOM);
							act("Cucini $p.", FALSE, ch, nuovo, 0, TO_CHAR);
						}
						elimina_oggetti_uguali(ch, target, num_richiesto);
						manufatti_to_char(nuovo, ch, num_prodotti);
						check_danno_oggetto(ch, strumento, 1);
						
						if (learner)  
						  insegna_prof(ch,learner,PROF_CUCINARE, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);						
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}
					
				} else if ((GET_OBJ_MAT_NUM(target)==NAT_VERDURA) && (GET_OBJ_MAT_TYPE(target)==NATURA)){
					
					nuovo = read_object(OBJVNUM_CIBO+CIBO_VERDURA,VIRTUAL);
					num_prodotti = 1;
					num_richiesto = 1;
					if (nuovo) {
						//Valore Nutritivo
						GET_OBJ_VAL(nuovo, 0) = calcola_val_nutrizionale(ch,nuovo,target);
						//Valore in Soldoni :-)
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);
						
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						for (tempcount = 0;tempcount<num_prodotti;tempcount++){
							act("$n cucina $p.", FALSE, ch, nuovo, 0, TO_ROOM);
							act("Cucini $p.", FALSE, ch, nuovo, 0, TO_CHAR);
						}						
						manufatti_to_char(nuovo, ch, num_prodotti);
						elimina_oggetti_uguali(ch, target, num_richiesto);
						check_danno_oggetto(ch, strumento, 1);
						
						if (learner)  
						  insegna_prof(ch,learner,PROF_CUCINARE, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}					
				} else if ((GET_OBJ_MAT_NUM(target)==PEL_CARNE) && (GET_OBJ_MAT_TYPE(target)==PELLI)){
					//TRE
					switch (GET_OBJ_VAL(target, 2)) {
						case 0: // Buona
							vnum = OBJVNUM_CIBO + CIBO_CARNE_COTTA_BUONA;
							break;
						case 1: // Ottima
							vnum = OBJVNUM_CIBO + CIBO_CARNE_COTTA_OTTIMA;
							break;
						case 2: // Eccellente
							vnum = OBJVNUM_CIBO + CIBO_CARNE_COTTA_ECCELLENTE;
							break;
					}
					nuovo = read_object(vnum, VIRTUAL);
					num_prodotti = 1;
					num_richiesto = 1;
					if (nuovo) {
						
						//Valore Nutritivo
						GET_OBJ_VAL(nuovo, 0) = calcola_val_nutrizionale(ch,nuovo,target);
						//Valore in Soldoni :-)
						GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);
						
						GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
						SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
						for (tempcount = 0;tempcount<num_prodotti;tempcount++){
							act("$n cucina $p.", FALSE, ch, nuovo, 0, TO_ROOM);
							act("Cucini $p.", FALSE, ch, nuovo, 0, TO_CHAR);
						}
						elimina_oggetti_uguali(ch, target, num_richiesto);
						manufatti_to_char(nuovo, ch, num_prodotti);
						check_danno_oggetto(ch, strumento, 1);
						
						if (learner)  
						  insegna_prof(ch,learner,PROF_CUCINARE, PROP_IMPAR);
						return;
					} else {
						mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);
						send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
						return;
					}	
				} //AVANZAVA UNA GRAFFA :-//////
			}
			else
			{
				act("Non riesci a cucinare niente con $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce a cucinare niente con $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d prodotti alimentari.\r\n", num_distrutto);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_CUCINARE, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			
		case ITEM_PASTELLA: 
			if (GET_OBJ_TYPE(strumento)!=ITEM_STRUM_CUCINA)
			{
				send_to_char("Non stai tenendo l'utensile adatto.\r\n", ch);
				return;
			}
			
			if (!ROOM_FLAGGED(ch->in_room, ROOM_CUCINA))
			{
				send_to_char("Devi trovarti in una cucina per poter cucinare dei dolci.\r\n", ch);
				return;
			}
			
			argument = one_argument(argument, arg3);
			
			if ((GET_OBJ_MAT_TYPE(target)==NATURA)&&(GET_OBJ_MAT_NUM(target)==0)) {
				if ( !is_abbrev(arg3, "crostata")   && !is_abbrev(arg3, "torta")   && !is_abbrev(arg3, "dolce")  && !is_abbrev(arg3, "galletta") && !is_abbrev(arg3, "pasticcini"))
				{
					send_to_char("Uso: cucina <prodotto> <alimento> <descrizione | frutta>\r\n"
						"Gli alimenti che si possono realizzare sono i seguenti:\r\n"
						"-Crostata alla frutta\r\n"
						"-Torta\r\n"
						"-Dolce                     x2\r\n"
						"-Galletta dolce da viaggio x4\r\n"
						"-Pasticcini                x10  (2)\r\n"
						"Le torte alla frutta possono essere create solo con la frutta.\r\n"
						"Es. facendo cucina pastella crostata frutta, ottieni una crostata di frutta.\r\n"
						"Es. facendo cucina pastella torta di Ramku, ottieni una torta di Ramku.\r\n",ch);
					return;
				}
				num_prodotti = 1;
				num_richiesto = 1;
				if ((is_abbrev(arg3, "torta")) || (is_abbrev(arg3, "crostata"))) num_prodotti = 1;
				else if (is_abbrev(arg3, "dolce"))  num_prodotti = 2;
				else if (is_abbrev(arg3, "galletta")) num_prodotti = 4;
				else if (is_abbrev(arg3, "pasticcini")) {num_prodotti = 10; num_richiesto = 2;}
				if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num_richiesto)
				{
					sprintf(buf, "Ti serve almeno un'altra pastella.\r\n");
					send_to_char(buf, ch);
					return;
				}
			}
			
			if (is_abbrev(arg3, "crostata")) {
				argument = one_argument(argument, arg4);
				if (!*arg4)
				{
					sprintf(buf, "Per creare la crostata ti serve anche della frutta.\r\n");
					send_to_char(buf, ch);
					return;
				}
				bitv = generic_find(arg4,  FIND_OBJ_INV, ch, &victim, &target2);
				if (!target2)
				{
					sprintf(buf, "Non hai %s.\r\n", arg4);
					send_to_char(buf, ch);
					return;
				}
				if ( (GET_OBJ_TYPE(target2)!=ITEM_FOOD) || (str_cmp(arg4, "frutta") && !is_frutta(arg4)) )
				{
					send_to_char("Che schifo!....\r\n", ch);
					return;
				}
			} else if (is_abbrev(arg3, "torta")) {
				if ( (!argument) || (!*argument) ) {
					send_to_char("Metti una descrizione alla Torta!\r\n",ch);
					return;
				}
			}
			GET_MOVE(ch)=GET_MOVE(ch)-2;
			if (check_proficienza(ch, PROF_CUCINARE, 100, 0))
			{
				if (is_abbrev(arg3, "torta")) nuovo = read_object(OBJVNUM_CIBO+CIBO_TORTA, VIRTUAL);
				else if (is_abbrev(arg3, "crostata")) nuovo = read_object(OBJVNUM_CIBO+CIBO_CROSTATA, VIRTUAL);
				else if (is_abbrev(arg3, "dolce")) nuovo = read_object(OBJVNUM_CIBO+CIBO_DOLCE, VIRTUAL);
				else if (is_abbrev(arg3, "galletta")) nuovo = read_object(OBJVNUM_CIBO+CIBO_GALLETTA, VIRTUAL);
				else if (is_abbrev(arg3, "pasticcini"))
					switch (dice(1,4)) {
						case 1: nuovo = read_object(OBJVNUM_CIBO+CIBO_BABBA, VIRTUAL);
							break;
						case 2: nuovo = read_object(OBJVNUM_CIBO+CIBO_CANNOLO, VIRTUAL);
							break;
						case 3: nuovo = read_object(OBJVNUM_CIBO+CIBO_BIGNE, VIRTUAL);	
							break;
						case 4: nuovo = read_object(OBJVNUM_CIBO+CIBO_SFOGLIA, VIRTUAL);	
							break;
					}
				else nuovo = NULL;
				
				if (nuovo) {
							//Valore Nutritivo
					GET_OBJ_VAL(nuovo, 0) = calcola_val_nutrizionale(ch,nuovo,target);
						      //Valore in Soldoni :-)
					GET_OBJ_COST(nuovo)   = calcola_costo(ch,nuovo,target);
					
					GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
					SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
					
					
					if (is_abbrev(arg3,"torta")) {
						skip_spaces(&argument);
						
						sprintf(buf,"%s %s&0",nuovo->short_description,argument);
						nuovo->short_description = str_dup(buf);
						
						sprintf(buf,"%s %s&0.",nuovo->description,argument);
						nuovo->description = str_dup(buf);
					}
					
					if (is_abbrev(arg3,"crostata")) {
						switch (GET_OBJ_VNUM(target2)) {
							case OBJVNUM_CIBO+CIBO_LAMPONE:
								sprintf(buf1,"di lampone");
								break;
							case OBJVNUM_CIBO+CIBO_FRAGOLA:
								sprintf(buf1,"di fragole");
								break;
							case OBJVNUM_CIBO+CIBO_MELE:
								sprintf(buf1,"di mele");
								break;
							case OBJVNUM_CIBO+CIBO_CILIEGIA:
								sprintf(buf1,"di ciliege");
								break;
							case OBJVNUM_CIBO+CIBO_MORA:
								sprintf(buf1,"di more");
								break;
							case OBJVNUM_CIBO+CIBO_PESCA:
								sprintf(buf1,"di pesche");
								break;
							case OBJVNUM_CIBO+CIBO_ALBICOCCA:
								sprintf(buf1,"di albicocche");
								break;
							default:
								sprintf(buf1,"di &BChardhros il mietitore&0");
						}
						sprintf(buf,"%s %s&0",nuovo->short_description,buf1);
						nuovo->short_description = str_dup(buf);
						
						sprintf(buf,"%s %s.",nuovo->description,buf1);
						nuovo->description = str_dup(buf);
					}
					
					elimina_oggetti_uguali(ch, target, 1);
					if (target2)
						elimina_oggetti_uguali(ch,target2, 1);
					for (tempcount = 0;tempcount<num_prodotti;tempcount++){
						act("$n cucina $p.", FALSE, ch, nuovo, 0, TO_ROOM);
						act("Cucini $p.", FALSE, ch, nuovo, 0, TO_CHAR);
					}
					manufatti_to_char(nuovo, ch, num_prodotti);
					check_danno_oggetto(ch, strumento, 1);
					if (learner)  
					  insegna_prof(ch,learner,PROF_CUCINARE, PROP_IMPAR);
					return;				
					
				} else { 
					mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);						
					send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
					return;
				}
			}
			else
			{
				act("Non riesci a cucinare niente con $P.", FALSE, ch, 0, target, TO_CHAR);
				act("$n non riesce a cucinare niente con $P.", FALSE, ch, 0, target, TO_ROOM);
				num_distrutto = num_fallimento (num_richiesto);
				if (num_distrutto == 1)
					act("Rovini irreparabilmente $P.", FALSE, ch, 0, target, TO_CHAR);
				else
				{
					sprintf(buf, "Rovini irreparabilmente %d pastelle.\r\n", num_distrutto);
					send_to_char(buf, ch);
				}
				elimina_oggetti_uguali(ch, target, num_distrutto);
				check_proficienza_improve(ch, PROF_CUCINARE, 2 * num_distrutto);
				check_danno_oggetto(ch, strumento, 0.1);
			}
			ch->last_lavoro=time(NULL);
			break;
			// Fine Cuoco V2			
		default:
			act("Non hai idea di come si possa lavorare $P.", FALSE, ch, 0, target, TO_CHAR);
			return;
	}
	
}	

/*Lance Alchimista inizio */
void perform_mischia(struct char_data *ch, struct obj_data *strumento,char *argument)
{
	int potion_vnum, potion;
	struct char_data *learner = NULL;
	struct obj_data *nuovo;
	int num_distrutto;
	int lista_ingredienti [MAX_ERBE_X_POZIONE][2] =
	{{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
	int numingrediente = 0;
	int ingrediente = -1;
	int cont,cont2;
	bool match;
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	
	if (GET_OBJ_TYPE(strumento)!=ITEM_BOCCETTA)
	{
		send_to_char("Devi avere in mano una boccetta per mischiare le erbe.\r\n", ch);
		return;
	}
	if (!ROOM_FLAGGED(ch->in_room, ROOM_LAB_ALCHIMIA))
	{
		send_to_char("Devi trovarti in un laboratorio di alchimia per poter ottenere delle pozioni.\r\n", ch);
		return;
	}
	
	if (!argument) return;
	
    // Insegamento by Shade
	one_argument(argument, arg3);
	if (*arg3 && (!str_cmp(arg3,"con"))) {
		argument = one_argument(argument, arg3);
		one_argument(argument, arg4);
		if (!(learner = get_char_room_vis(ch, arg4)))
		{
			send_to_char("Non c'e' nessuno con quel nome con te.\r\n", ch);
			return;
		}
	}
	
	skip_spaces(&argument);
	
	while (argument && (strcmp(argument,"")) && (numingrediente < MAX_ERBE_X_POZIONE ))
	{
		argument = one_argument(argument, arg2);
		argument = one_argument(argument, arg3);
		skip_spaces(&argument);
		
		if (!arg2 || !arg3 || !is_number(arg2))
		{
			send_to_char("Uso: mischia 2 ingrediente1 1 ingrediente2 3 ingrediente3 ....ect\r\n", ch);
			return;
		}
		if ((ingrediente = get_ingrediente(arg3))==-1)
		{
			sprintf(buf, "%s non e' un'erba conosciuta\r\n",arg3);
			send_to_char(buf, ch);
			return;
		}
		
		if (chk_ingredienti(ch, atoi(arg2) ,arg3) < 0) return;
		
		lista_ingredienti[numingrediente][0] = ingrediente;
		lista_ingredienti[numingrediente][1] = atoi(arg2);
		
		numingrediente +=1;
	} // while
	
	potion = -1;
	for (cont = 0;cont < NUM_POZIONI_CREABILI;cont++)
	{
		match = TRUE;
		for (cont2 = 0;cont2 < MAX_ERBE_X_POZIONE; cont2++)
			if ((erbe_pozioni[cont][cont2][0]!=lista_ingredienti[cont2][0]) ||
				(erbe_pozioni[cont][cont2][1]!=lista_ingredienti[cont2][1]))
				match = match && FALSE;
		if (match) potion = cont;
	}
	
	if (potion < 0) {
		send_to_char("BOOM...il miscuglio esplode!\r\n", ch);
		elimina_ingredienti(ch,lista_ingredienti);
		damage(ch, ch, 5, -1);
		extract_obj(strumento);
		return;
	}
	
	if (time(NULL)-ch->last_lavoro<3 && GET_LEVEL(ch)<LVL_IMMORT)
	{
		send_to_char("Hai appena smesso di lavorare... ti prendi una pausa sindacale...\r\n", ch);
		return;
	}
	
	potion_vnum = OBJVNUM_POZIONI + potion;
	//PEPPE MOV
//	GET_MOVE(ch)=GET_MOVE(ch)-1;
	if (check_proficienza(ch, PROF_ALCHIMIA, 100, 0))
	{
		nuovo = read_object(potion_vnum, VIRTUAL);
		act("Mischiando le erbe $n ricava $p.", FALSE, ch, nuovo, 0, TO_ROOM);
		act("Mischiando le erbe ricavi $p.", FALSE, ch, nuovo, 0, TO_CHAR);
		  //elimina_ingredienti(ch, potion);
		obj_to_char(nuovo, ch);
		elimina_ingredienti(ch,lista_ingredienti);
		extract_obj(strumento);
		if (learner) 
		  insegna_prof(ch,learner,PROF_ALCHIMIA, PROP_IMPAR);
	}
	else
	{
		act("Non riesci ad ottenere niente dalle erbe che hai usato.", FALSE, ch, 0, 0, TO_CHAR);
		act("$n non riesce ad ottenere niente dalle erbe che hai usato.", FALSE, ch, 0, 0, TO_ROOM);
		sprintf(buf, "Rendi unutilizzabili le erbe usate.\r\n");
		send_to_char(buf, ch);
		num_distrutto = elimina_ingredienti(ch,lista_ingredienti);
		check_proficienza_improve(ch, PROF_ALCHIMIA, 5 * num_distrutto);
	}
	ch->last_lavoro=time(NULL);
	return;
}
/*Lance Alchimista fine */


void potion_list(struct char_data *ch)
{
	if (GET_PROF(ch,PROF_ALCHIMIA )<10)
		send_to_char("Pozioni creabili dall'alchimista                                  \r\n"
			"--------------------------------------------------------------\r\n"
			"Remove poison                Shield                           \r\n"
			"Armor                        Refresh                          \r\n"
			"Remove blind                 Cure light                       \r\n"
			"Remove curse                 Cure serious                     \r\n"
			"Dispel magic                 Cure critic                      \r\n"
			"See invisible                Detect poison e alignment        \r\n"
			"Invisible                    Infravision                      \r\n"
			"Poison                       Strength                         \r\n"
			"Protection from fire         Light                            \r\n"
			"Barkskin                                                      \r\n"
			"--------------------------------------------------------------\r\n", ch);
	
	else
	if (GET_PROF(ch,PROF_ALCHIMIA )<75)
		send_to_char("Pozioni                   Ingredienti\r\n"
			"----------------------------------------------------------------------------\r\n"
			"Remove poison             (2 Bardana + 2 Biancospino)\r\n"
			"Armor                     (2 Arnica + Alchimilla)\r\n"
			"Remove blind              (Arnica + Ortica + Celidonia)\r\n"
			"Remove curse              (Arnica + Biancospino + Ortica)\r\n"
			"Cure light                (2 Alchimilla + Vischio)\r\n"
			"----------------------------------------------------------------------------\r\n"
			"Tra parentesi sono indicati gli ingredienti per creare le pozioni.\r\n", ch);
	else
		send_to_char("Pozioni                   Ingredienti\r\n"
			"----------------------------------------------------------------------------\r\n"
			"Remove poison             (2 Bardana + 2 Biancospino)\r\n"
			"Armor                     (2 Arnica + Alchimilla)\r\n"
			"Remove blind              (Arnica + Ortica + Celidonia)\r\n"
			"Remove curse              (Arnica + Biancospino + Ortica)\r\n"
			"Dispel magic              (2 Celidonia + Calendula)\r\n"
			"See invisible             (2 Calendula + Ortica + Bardana)\r\n"
			"Invisible                 (2 Belladonna + 3 Ginestra + Assenzio)\r\n"
			"Poison                    (2 Assenzio + 2 Belladonna)\r\n"
			"Protection from fire      (3 Borragine + 1 Vischio)\r\n"
			"Barkskin                  (2 Ginestra + Calendula)\r\n"
			"Shield                    (Ginestra + Calendula + Assenzio)\r\n"
			"Refresh                   (2 Vischio + Enula + Ginestra)\r\n"
			"Cure light                (2 Alchimilla + Vischio)\r\n"
			"Cure serious              (2 Ginestra + 3 Achillea)\r\n"
			"Cure critic               (3 Ginestra + 2 Achillea + Alchimilla + 2 Bardana)\r\n"
			"Detect poison e alignment (Enula + Caledonia + Borragine + Bardana)\r\n"
			"Infravision               (Enula + 2 Achillea + Ginestra)\r\n"
			"Strength                  (Enula + Calendula + Alchimilla)\r\n"
			"Light                     (Ginestra + Ortica)\r\n"
			"Tisana calda della nonna  (2 Achillea + 3 Ortica + Celidonia)\r\n"
			"\r\nMiscugli (cmd mescola)    Ingredienti\r\n"
			"----------------------------------------------------------------------------\r\n"
			"Caffe' e limone           (Caffe' + Un Limone)\r\n"
			"Garze imbevute	di grasso  (Stoffa pulita + Storione crudo)\r\n"
			"----------------------------------------------------------------------------\r\n"
			"Tra parentesi sono indicati gli ingredienti per creare le pozioni.\r\n", ch);
}


/**************************************************************/


void check_danno_oggetto (struct char_data * ch, struct obj_data * obj, float modif)
{
	int i;
	char nome[MAX_STRING_LENGTH];
	
	strcpy(nome, obj->short_description);
	if (number(0, GET_OBJ_TSLOTS(obj) * modif) == 0)
	{
		if (GET_OBJ_TSLOTS(obj) != 0)
		{
			GET_OBJ_CSLOTS(obj)--;
			GET_OBJ_COST(obj) = ( GET_OBJ_COST(obj) * GET_OBJ_CSLOTS(obj)) / (GET_OBJ_TSLOTS(obj) );
			if(GET_OBJ_TYPE(obj) == ITEM_WEAPON){
				if(IS_OBJ_STAT(obj, ITEM_AFFILATO)){
					if(GET_OBJ_VAL(obj,0)>1)
						GET_OBJ_VAL(obj,0)--;
					else{
						REMOVE_BIT(GET_OBJ_EXTRA(obj), ITEM_AFFILATO);
						for (i=0; i<MAX_OBJ_AFFECT; i++){
							if ( obj->affected[i].location==APPLY_HITROLL)
								break;
						}
						obj->affected[i].modifier--;
						send_to_char("La tua arma non e' piu affilata.\n",ch);
					}
				}
			}
			sprintf(buf, "%s si e' DANNEGGIATO!\r\n", CAP(nome));
			send_to_char(buf, ch);
		}
	}
	
	if (GET_OBJ_CSLOTS(obj)<=0 && GET_OBJ_TSLOTS(obj)!=0)
	{
		sprintf(buf, "%s si sbriciola e cade per terra!\r\n", CAP(nome));
		send_to_char(buf, ch);
		extract_obj(obj);
	}
}

int num_necessari (int obiettivo, int riuscita)
{
	int riusciti=0, usati=0;
	
	while (riusciti < obiettivo)
	{
		if (number(1,100)<=riuscita) riusciti++;
		usati++;
	}
	return usati;
}

int num_fallimento (int num_richiesto)
{
	int i, falliti=0;
	
	for (i=1; i<=num_richiesto; i++)
		if (number(0,1)) falliti++;
	falliti=MAX(1, falliti);
	return falliti;
}

void elimina_oggetti_uguali(struct char_data * ch, struct obj_data * primo, int numero)
{
	struct obj_data * i, * next;
	int eliminati = 1;
	int type     = GET_OBJ_TYPE    (primo);
	int mat_type = GET_OBJ_MAT_TYPE(primo);
	int mat_num  = GET_OBJ_MAT_NUM (primo);
	
	obj_from_char(primo);
	extract_obj(primo);
	
	for (i = ch->carrying; i && (eliminati < numero); i = next)
	{
		next = i->next_content;
		if (GET_OBJ_TYPE(i)== type && GET_OBJ_MAT_TYPE(i) == mat_type && GET_OBJ_MAT_NUM(i) == mat_num)
		{
			obj_from_char(i);
			extract_obj(i);
			eliminati++;
		}
	}
}

void manufatti_to_char(struct obj_data * object, struct char_data * ch, int numero)
{
	int oggetti, i;
	struct obj_data * nuovo;
	
	if ((IS_CARRYING_N(ch)<CAN_CARRY_N(ch)) && (IS_CARRYING_W(ch)+GET_OBJ_WEIGHT(object) < CAN_CARRY_W(ch)))
		obj_to_char(object, ch);
	else
	{
		obj_to_room(object, ch->in_room);
		act("Hai troppo peso, lasci $p per terra.", FALSE, ch, object, 0, TO_CHAR);
	}
	
	for (oggetti=2; oggetti<=numero; oggetti++)
	{
		nuovo = read_object(object->item_number, REAL);
		GET_OBJ_COST(nuovo)     = GET_OBJ_COST(object);
		GET_OBJ_RENT(nuovo)     = GET_OBJ_RENT(object);
		for (i=0; i<10; i++)
			GET_OBJ_VAL(nuovo, i) = GET_OBJ_VAL(object, i);
		GET_OBJ_TYPE(nuovo)     = GET_OBJ_TYPE(object);
		GET_OBJ_EXTRA(nuovo)    = GET_OBJ_EXTRA(object);
		GET_OBJ_WEAR(nuovo)     = GET_OBJ_WEAR(object);
		GET_OBJ_WEIGHT(nuovo)   = GET_OBJ_WEIGHT(object);
		GET_OBJ_TIMER(nuovo)    = GET_OBJ_TIMER(object);
		GET_OBJ_CSLOTS(nuovo)   = GET_OBJ_CSLOTS(object);
		GET_OBJ_TSLOTS(nuovo)   = GET_OBJ_TSLOTS(object);
		GET_OBJ_LEVEL(nuovo)    = GET_OBJ_LEVEL(object);
		for (i=0; i<4; i++)
			GET_OBJ_BITVECTOR(nuovo, i)= GET_OBJ_BITVECTOR(object, i);
		GET_OBJ_MAT_TYPE(nuovo) = GET_OBJ_MAT_TYPE(object);
		GET_OBJ_MAT_NUM(nuovo)  = GET_OBJ_MAT_NUM(object);
		nuovo->description = str_dup(object->description);
		nuovo->short_description = str_dup(object->short_description);
		nuovo->name = str_dup(object->name);
		
		if ((IS_CARRYING_N(ch)<CAN_CARRY_N(ch)) && (IS_CARRYING_W(ch)+GET_OBJ_WEIGHT(nuovo) < CAN_CARRY_W(ch)))
			obj_to_char(nuovo, ch);
		else
		{
			obj_to_room(nuovo, ch->in_room);
			act("Hai troppo peso, lasci $p per terra.", FALSE, ch, nuovo, 0, TO_CHAR);
		}
	}
}

const char non_definito[] = "Non Definito";

const char * nome_materiale (int tipo, int num)
{
	switch (tipo)
	{
		case PIETRE:
			if (num>=0 && num<NUM_MATERIALI_PIETRE) return pietre[num].nome;
			break;
		case ALBERI:
			if (num>=0 && num<NUM_MATERIALI_ALBERI) return alberi[num].nome;
			break;
		case GEMME :
			if (num>=0 && num<NUM_MATERIALI_GEMME ) return gemme[num].nome;
			break;
		case ROCCE :
			if (num>=0 && num<NUM_MATERIALI_ROCCE ) return rocce[num].nome;
			break;
		case PELLI :
			if (num>=0 && num<NUM_MATERIALI_PELLI ) return pelli[num].nome;
			break;
		case NATURA:
			if (num>=0 && num<NUM_MATERIALI_NATURA) return natura[num].nome;
			break;
		case ERBE  :
			if (num>=0 && num<NUM_MATERIALI_ERBE  ) return erbe[num].nome;
			break;
	}
	return non_definito;
}

int limite_materiali(int tipo)
{
	switch (tipo)
	{
		case PIETRE: return NUM_MATERIALI_PIETRE;
		case ALBERI: return NUM_MATERIALI_ALBERI;
		case GEMME : return NUM_MATERIALI_GEMME;
		case ROCCE : return NUM_MATERIALI_ROCCE;
		case PELLI : return NUM_MATERIALI_PELLI;
		case NATURA: return NUM_MATERIALI_NATURA;
		case ERBE  : return NUM_MATERIALI_ERBE;
			
	}
	return 1;
}

int trova_sector (struct risorse_reset * mat)
{
	int numero = number(1, mat->p_noswim);
	
	if (numero <= mat->p_field)  return SECT_FIELD;
	if (numero <= mat->p_forest) return SECT_FOREST;
	if (numero <= mat->p_hill)   return SECT_HILLS;
	if (numero <= mat->p_mount)  return SECT_MOUNTAIN;
	if (numero <= mat->p_swim)   return SECT_WATER_SWIM;
	return SECT_WATER_NOSWIM;
}

void conta_wilderness (void)
{
	extern char *sector_types[];
	int conta[NUM_ROOM_SECTORS];
	int i;
	room_rnum prima, ultima;
	room_rnum scorri;
	
	for (i=0; i<NUM_ROOM_SECTORS; i++)
		conta[i]=0;
	
	prima =  real_room(WILD_VNUM(WILD_RXOR, WILD_RYOR));
	ultima = real_room(WILD_VNUM(WILD_RXEN, WILD_RYEN));
	
	if (prima==-1 || ultima==-1)
	{
		mudlog("Non riesco a calcolare l'inizio o la fine della wilderness", NRM, LVL_GOD, TRUE);
		return ;
	}
	
	for (scorri=prima; scorri<=ultima; scorri++)
		if (SECT(scorri)>=0 && SECT(scorri)<NUM_ROOM_SECTORS)
			conta[SECT(scorri)]++;
		
	for (i=0; i<NUM_ROOM_SECTORS; i++)
	{
		sprintf(buf, "Sector type: %-20s Numero: %5d.", sector_types[i], conta[i]);
	// mudlog(buf, NRM, LVL_GOD, TRUE);
	}
}

void reset_risorse(int what)
{
	room_rnum prima, ultima;
	room_rnum scorri, inserito, tentativo;
	struct obj_data * obj, * next_obj;
	struct char_data *mob, *next_mob;
	int nris, i, j;
	int sector=0;
	int ok_object;
	long vnum;
	extern struct char_data *character_list;
	extern struct index_data *mob_index;
	
	conta_wilderness();
	
	prima =  real_room(WILD_VNUM(WILD_RXOR, WILD_RYOR));
	ultima = real_room(WILD_VNUM(WILD_RXEN, WILD_RYEN));
	
	if (prima==-1 || ultima==-1)
	{
		mudlog("Non riesco a calcolare l'inizio o la fine della wilderness", NRM, LVL_GOD, TRUE);
		return ;
	}
	
	for (scorri=prima; scorri <= ultima; scorri++)
		if (any_mob(NULL, scorri)!=2) {
			ok_object = FALSE;
			for (obj=world[scorri].contents; obj; obj=next_obj)
			{
				next_obj=obj->next_content;
				switch (what) {
					case RES_PIETRE:
						if (GET_OBJ_TYPE(obj) == ITEM_PIETRA) ok_object = TRUE;
						break;
					case RES_ALBERI: 
						if (GET_OBJ_TYPE(obj) == ITEM_ALBERO) ok_object = TRUE;
						break;
					case RES_GEMME: 
						if (GET_OBJ_TYPE(obj) == ITEM_MINERALE) ok_object = TRUE;
						break;
					case RES_ERBE: 
						if (GET_OBJ_TYPE(obj) == ITEM_ERBA) ok_object = TRUE;
						break;
					case RES_NATURA: 
						if (GET_OBJ_TYPE(obj) == ITEM_NATURALE) ok_object = TRUE;
						break;
					case RES_ROCCE: 
						if (GET_OBJ_TYPE(obj) == ITEM_ROCCIA) ok_object = TRUE;
						break;
				} 
				if (ok_object)	 
				{
					obj_from_room(obj);
					extract_obj(obj);
				}
			}
		}	  
		
	if (what == RES_SELVAGGINA) 
    /*Lance elimina selvaggina*/
		for (mob = character_list; mob; mob = next_mob) {
			next_mob = mob->next;
			if (mob && MOB_FLAGGED(mob,MOB_SELVAGGINA)) {
				if (any_mob(NULL, mob->in_room)!=2)
					extract_char(mob);	   	   
			}
		} 
		
	if (what == RES_AMMAESTRABILI)
    /*Lance elimina ammaestrabili*/
		for (mob = character_list; mob; mob = next_mob) {
			next_mob = mob->next;
			if (mob && (GET_MOB_VNUM(mob) >= START_MOB_AMMAESTR_VNUM) &&  (GET_MOB_VNUM(mob) <= END_MOB_AMMAESTR_VNUM)) {
				if (any_mob(NULL, mob->in_room)!=2)
					extract_char(mob);
			}
		} 
		
		
	switch (what) {
		
		case RES_PIETRE:
			for (i=0; i<NUM_MATERIALI_PIETRE; i++)
			{
				nris = number(pietre_reset[i].qmin, pietre_reset[i].qmax);
				sprintf (buf, "Creazione di %d giacimenti casuali di %s.", nris, pietre[i].nome);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j=0; j<nris; j++)
				{
					obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
					if (!obj)
					{
						mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
						return;
					}
					GET_OBJ_TYPE(obj) = ITEM_PIETRA;
					GET_OBJ_MAT_TYPE(obj) = PIETRE;
					GET_OBJ_MAT_NUM (obj) = i;
					GET_OBJ_TSLOTS(obj) = 0;
					GET_OBJ_CSLOTS(obj) = 0;
					GET_OBJ_WEIGHT(obj) = 10000;
					GET_OBJ_COST(obj)   = 0;
					GET_OBJ_RENT(obj)   = 0;
					SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
					sprintf(buf, "Un giacimento di %s e' qui.", pietre[i].nome);
					obj->description = str_dup(buf);
					sprintf(buf, "un giacimento di %s", pietre[i].nome);
					obj->short_description = str_dup(buf);
					sprintf(buf, "giacimento pietre %s", pietre[i].nome);
					obj->name = str_dup(buf);
					GET_OBJ_VAL(obj, 1) = number (pietre_reset[i].usimin, pietre_reset[i].usimax);
					
					sector = trova_sector(pietre_reset + i);
					inserito = 0;
					while (!inserito)
					{
						tentativo = number(prima, ultima);
						if (SECT(tentativo) == sector)
						{
							obj_to_room(obj, tentativo);
							inserito=tentativo;
						}
					}
				}
			}
			break;
			
		case RES_ALBERI:
			for (i=0; i<NUM_MATERIALI_ALBERI; i++)
			{
				nris = number(alberi_reset[i].qmin, alberi_reset[i].qmax);
				sprintf (buf, "Creazione di %d foreste casuali di %s.", nris, alberi[i].nome);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j=0; j<nris; j++)
				{
					obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
					if (!obj)
					{
						mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
						return;
					}
					GET_OBJ_TYPE(obj) = ITEM_ALBERO;
					GET_OBJ_MAT_TYPE(obj) = ALBERI;
					GET_OBJ_MAT_NUM (obj) = i;
					GET_OBJ_TSLOTS(obj) = 0;
					GET_OBJ_CSLOTS(obj) = 0;
					GET_OBJ_WEIGHT(obj) = 10000;
					GET_OBJ_COST(obj)   = 0;
					GET_OBJ_RENT(obj)   = 0;
					SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
					sprintf(buf, "Una foresta di %s e' qui.", alberi[i].nome);
					obj->description = str_dup(buf);
					sprintf(buf, "una foresta di %s", alberi[i].nome);
					obj->short_description = str_dup(buf);
					sprintf(buf, "foresta alberi %s", alberi[i].nome);
					obj->name = str_dup(buf);
					GET_OBJ_VAL(obj, 1) = number (alberi_reset[i].usimin, alberi_reset[i].usimax);
					
					sector = trova_sector(alberi_reset + i);
					inserito = 0;
					while (!inserito)
					{
						tentativo = number(prima, ultima);
						if (SECT(tentativo) == sector)
						{
							obj_to_room(obj, tentativo);
							inserito=tentativo;
						}
					}
				}
			}
			break;
			
		case RES_GEMME:
			for (i=0; i<NUM_MATERIALI_GEMME; i++)
			{
				nris = number(gemme_reset[i].qmin, gemme_reset[i].qmax);
				sprintf (buf, "Creazione di %d giacimenti casuali di %s.", nris, gemme[i].nome);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j=0; j<nris; j++)
				{
					obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
					if (!obj)
					{
						mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
						return;
					}
					GET_OBJ_TYPE(obj) = ITEM_MINERALE;
					GET_OBJ_MAT_TYPE(obj) = GEMME;
					GET_OBJ_MAT_NUM (obj) = i;
					GET_OBJ_TSLOTS(obj) = 0;
					GET_OBJ_CSLOTS(obj) = 0;
					GET_OBJ_WEIGHT(obj) = 10000;
					GET_OBJ_COST(obj)   = 0;
					GET_OBJ_RENT(obj)   = 0;
					SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
					sprintf(buf, "Un giacimento di %s e' qui.", gemme[i].nome);
					obj->description = str_dup(buf);
					sprintf(buf, "un giacimento di %s", gemme[i].nome);
					obj->short_description = str_dup(buf);
					sprintf(buf, "giacimento gemme %s", gemme[i].alias);
					obj->name = str_dup(buf);
					GET_OBJ_VAL(obj, 1) = number (gemme_reset[i].usimin, gemme_reset[i].usimax);
					
					sector = trova_sector(gemme_reset + i);
					inserito = 0;
					while (!inserito)
					{
						tentativo = number(prima, ultima);
						if (SECT(tentativo) == sector)
						{
							obj_to_room(obj, tentativo);
							inserito=tentativo;
						}
					}
				}
			}
			break;
			
		case RES_ROCCE:
			for (i=0; i<NUM_MATERIALI_ROCCE; i++)
			{
				nris = number(rocce_reset[i].qmin, rocce_reset[i].qmax);
				sprintf (buf, "Creazione di %d giacimenti casuali di %s.", nris, rocce[i].nome);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j=0; j<nris; j++)
				{
					obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
					if (!obj)
					{
						mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
						return;
					}
					GET_OBJ_TYPE(obj) = ITEM_ROCCIA;
					GET_OBJ_MAT_TYPE(obj) = ROCCE;
					GET_OBJ_MAT_NUM (obj) = i;
					GET_OBJ_TSLOTS(obj) = 0;
					GET_OBJ_CSLOTS(obj) = 0;
					GET_OBJ_WEIGHT(obj) = 10000;
					GET_OBJ_COST(obj)   = 0;
					GET_OBJ_RENT(obj)   = 0;
					SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
					sprintf(buf, "Un giacimento di %s e' qui.", rocce[i].nome);
					obj->description = str_dup(buf);
					sprintf(buf, "un giacimento di %s", rocce[i].nome);
					obj->short_description = str_dup(buf);
					sprintf(buf, "giacimento rocce %s", rocce[i].nome);
					obj->name = str_dup(buf);
					GET_OBJ_VAL(obj, 1) = number (rocce_reset[i].usimin, rocce_reset[i].usimax);
					
					sector = trova_sector(rocce_reset + i);
					inserito = 0;
					while (!inserito)
					{
						tentativo = number(prima, ultima);
						if (SECT(tentativo) == sector)
						{
							obj_to_room(obj, tentativo);
							inserito=tentativo;
						}
					}
				}
			}
			break;
			
		case RES_NATURA:
			for (i=0; i<NUM_MATERIALI_NATURA; i++)
			{
				nris = number(natura_reset[i].qmin, natura_reset[i].qmax);
				sprintf (buf, "Creazione di %d risorse casuali di %s.", nris, natura[i].nome);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j=0; j<nris; j++)
				{
					obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
					if (!obj)
					{
						mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
						return;
					}
					GET_OBJ_TYPE(obj) = ITEM_NATURALE;
					GET_OBJ_MAT_TYPE(obj) = NATURA;
					GET_OBJ_MAT_NUM (obj) = i;
					GET_OBJ_TSLOTS(obj) = 0;
					GET_OBJ_CSLOTS(obj) = 0;
					GET_OBJ_WEIGHT(obj) = 0;
					GET_OBJ_COST(obj)   = 0;
					GET_OBJ_RENT(obj)   = 0;
					SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
					sprintf(buf, "Una risorsa di %s e' qui.", natura[i].nome);
					obj->description = str_dup(buf);
					sprintf(buf, "una risorsa di %s", natura[i].nome);
					obj->short_description = str_dup(buf);
					sprintf(buf, "risorsa naturali %s", natura[i].nome);
					obj->name = str_dup(buf);
					GET_OBJ_VAL(obj, 1) = number (natura_reset[i].usimin, natura_reset[i].usimax);
					
					sector = trova_sector(natura_reset + i);
					inserito = 0;
					while (!inserito)
					{
						tentativo = number(prima, ultima);
						if (SECT(tentativo) == sector)
						{
							obj_to_room(obj, tentativo);
							inserito=tentativo;
						}
					}
				}
			}
			break;
			
		case RES_ERBE:
			for (i=0; i<NUM_MATERIALI_ERBE; i++)
			{
				nris = number(erbe_reset[i].qmin, erbe_reset[i].qmax);
				sprintf (buf, "Creazione di %d cespugli casuali di %s.", nris, erbe[i].nome);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j=0; j<nris; j++)
				{
					obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
					if (!obj)
					{
						mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
						return;
					}
					GET_OBJ_TYPE(obj) = ITEM_ERBA;
					GET_OBJ_MAT_TYPE(obj) = ERBE;
					GET_OBJ_MAT_NUM (obj) = i;
					GET_OBJ_TSLOTS(obj) = 0;
					GET_OBJ_CSLOTS(obj) = 0;
					GET_OBJ_WEIGHT(obj) = 10000;
					GET_OBJ_COST(obj)   = 0;
					GET_OBJ_RENT(obj)   = 0;
					SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
					sprintf(buf, "Un cespuglio di %s e' qui.", erbe[i].nome);
					obj->description = str_dup(buf);
					sprintf(buf, "un cespuglio di %s", erbe[i].nome);
					obj->short_description = str_dup(buf);
					sprintf(buf, "%s cespuglio erbe", erbe[i].nome); /*Lance per il locate*/
					obj->name = str_dup(buf);
					GET_OBJ_VAL(obj, 1) = number (erbe_reset[i].usimin, erbe_reset[i].usimax);
					
					sector = trova_sector(erbe_reset + i);
					inserito = 0;
					while (!inserito)
					{
						tentativo = number(prima, ultima);
						if (SECT(tentativo) == sector)
						{
							obj_to_room(obj, tentativo);
							inserito=tentativo;
						}
					}
				}
			}
			
			break;
			
		case RES_SELVAGGINA:
	/*reset selvaggina*/
			
			for (vnum = START_SELV_VNUM; vnum <= END_SELV_VNUM;vnum++)
			{
				nris = number(30, 60);
				sprintf (buf, "Creazione di %d animali selvaggina casuali [%d].", nris ,(int) vnum);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j = 0;j<nris;j++)
				{
					mob = read_mobile(vnum, VIRTUAL);
					if (mob) {
						inserito = 0;
						sector = number(2,5);
						while (!inserito)
						{
							tentativo = number(prima, ultima);
							if (SECT(tentativo) == sector)
							{
								char_to_room(mob, tentativo);
								inserito=tentativo;
							}
						}
					} //if mob
				}
			}
			break;
			
		case RES_AMMAESTRABILI:  
 	 /*reset mob ammaestrabili*/
			for (vnum = START_MOB_AMMAESTR_VNUM; vnum <= END_MOB_AMMAESTR_VNUM;vnum++)
			{
				nris = number(30, 60);
				sprintf (buf, "Creazione di %d mob ammaestrabili casuali [%d].", nris ,(int) vnum);
				mudlog(buf, NRM, LVL_GOD, TRUE);
				for (j = 0;j<nris;j++)
				{
					mob = read_mobile(vnum, VIRTUAL);
					if (mob) {
						inserito = 0;
						sector = number(2,5);
						while (!inserito)
						{
							tentativo = number(prima, ultima);
							if (SECT(tentativo) == sector)
							{
								char_to_room(mob, tentativo);
								inserito=tentativo;
							}
						}
					} //if mob
				}
			}
			
			
	} //end case
}

int chk_ingredienti(struct char_data *ch, int num, char *ingrediente)
{
	int num_posseduto;
	int bitv;
	struct char_data *victim;
	struct obj_data *target;
	if (!ingrediente) return -1;
	bitv = generic_find(ingrediente, FIND_OBJ_INV, ch, &victim, &target);
	if (target && GET_OBJ_TYPE(target)==ITEM_ERBA_PULITA) {
		if ((num_posseduto=num_materiale_in_list_vis(ch, target, ch->carrying)) < num)
		{
			sprintf(buf, "Non hai %d erb%s di %s.\r\n",
				num,(num>1 ? "e": "a"),ingrediente);
			send_to_char(buf, ch);
			return -1;
		}
		
	} else {
		sprintf(buf, "Non hai nessun erba di %s.\r\n", ingrediente);
		send_to_char(buf, ch);
		return -1;
	}
	return 0;
}

int elimina_ingredienti(struct char_data *ch, int lista_ingredienti [MAX_ERBE_X_POZIONE][2])
{
	int cont;
	struct obj_data *target;
	int num_ingr_eliminati = 0;
	
	for (cont = 0; cont<=MAX_ERBE_X_POZIONE - 1; cont++)
	{
		if (lista_ingredienti[cont][0] != -1)
		{
			target = get_materiale_in_inv(ch, erbe[lista_ingredienti[cont][0]].nome, ITEM_ERBA_PULITA,
					ERBE, get_ingrediente(erbe[lista_ingredienti[cont][0]].nome) ,ch->carrying);
			if (target) {
				elimina_oggetti_uguali(ch, target ,lista_ingredienti[cont][1]);
				num_ingr_eliminati += lista_ingredienti[cont][1];
			}
		}
	}
	return num_ingr_eliminati;
}

int get_ingrediente(char *ingrediente)
{
	int cont;
	for (cont = 0;cont < NUM_MATERIALI_ERBE;cont++)
		if (is_abbrev(ingrediente,erbe[cont].nome)) return cont;
	return -1;
}

struct obj_data * get_materiale_in_inv(struct char_data *ch, char *nome, int tipo, int materiale,
	int nummateriale, struct obj_data * list)
{
	struct obj_data * i;
	
	for (i = list; i; i = i->next_content)
		if (CAN_SEE_OBJ(ch, i) &&
			isname(nome,i->name) &&
			GET_OBJ_TYPE(i)   == tipo    &&
			GET_OBJ_MAT_TYPE(i) == materiale  &&
			GET_OBJ_MAT_NUM(i)  == nummateriale )
			return i;
		
	return NULL;
}

int is_frutta(char *arg)
{
	int res, num;
	res=0;
	for (num=0;num<NUM_TIPO_FRUTTA;num++) {
		sprintf(buf,"%s",frutta[num]);
		if (!str_cmp(arg, buf))
			res=1;
	}
	return res;
}

/*************************************** PESCA BY LANCE************************************/

#define PULSES_PER_MUD_HOUR     (SECS_PER_MUD_HOUR*PASSES_PER_SEC)

struct event_pesca_data
{
	struct char_data *ch;
	struct obj_data *target;
	int times;
};

int get_nome_pesce()
{
	int caso;
	
	caso = number(0,100);
	if (caso == 99) return 8;   //un caso solo per il mazzugoru
	if (caso > 90) return 7;
	if (caso > 80) return 6;
	if (caso > 70) return 5;
	if (caso > 60) return 4;
	if (caso > 50) return 3;
	if (caso > 40) return 2;
	if (caso > 30) return 1;
	if (caso > 15) return 0;
	return 0;
}

EVENTFUNC(pesca_event_proc)
{
	struct char_data *ch;
	struct obj_data *strumento, *target;
	struct event_pesca_data *evtime = (struct event_pesca_data *) event_obj;
	int gain;
	
	ch = evtime->ch;
	target = evtime->target;
	
	gain = 3 + (GET_PROF(ch, PROF_PESCATORE) / 40);
	
	if (evtime->times>100)
	{
		sprintf(buf, "Probabilmente il pesce ha mangiato l'esca, meglio cambiarla.\r\n");
		send_to_char(buf, ch);
		return 0;
	}
	
	if (!ch)
	{
	 /*sgancia l'evento*/
		free(event_obj);
		return 0;
	}
	
	if ((IN_ROOM(ch)!=-1) && (SECT(IN_ROOM(ch)) != SECT_WATER_NOSWIM) &&
		
		(SECT(IN_ROOM(ch)) != SECT_WATER_SWIM))
	{
		send_to_char("Non puoi pescare qui.\r\n", ch);
		GET_PESCA_STATUS(ch) = 0;
		GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT) = NULL;
		free(event_obj);
		return 0;
	}
	
	if (target->in_room != ch->in_room)
	{
		send_to_char("Ti sei spostato. Ritiri su la lenza e ti prepari ad un nuovo lancio.\r\n", ch);
		GET_PESCA_STATUS(ch) = 0;
		GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT) = NULL;
		free(event_obj);
		return 0;
	}
	
	strumento = GET_EQ(ch, WEAR_HOLD);
	if (!strumento)
	{
		send_to_char("Non hai piu' uno strumento per pescare... rinunci!.\r\n", ch);
		GET_PESCA_STATUS(ch) = 0;
		GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT) = NULL;
		free(event_obj);
		return 0;
	}
	//PEPPE MOV
//	GET_MOVE(ch)=GET_MOVE(ch)-1;
	if ((check_proficienza(ch, PROF_PESCATORE, 100, 0)) || (GET_PESCA_STATUS(ch) == 2))
	{
		act("Vedi la tua lenza tirare... forse ha abboccato.", FALSE, ch, 0, 0, TO_CHAR);
		act("La lenza di $n inizia a tirare.", FALSE, ch, 0, 0, TO_ROOM);
		GET_PESCA_STATUS(ch) = 2;
	} else
	{
		sprintf(buf, "Mmm.....oggi e' una giornata sfortunata, non abbocca nulla!.\r\n");
		//PEPPE PROVA PESCATORE
		check_proficienza_improve(ch, PROF_PESCATORE, 4);
		send_to_char(buf, ch);
		GET_PESCA_STATUS(ch) = 1;
	}
	
	evtime->times++;
	
	return (long)(PULSES_PER_MUD_HOUR / (gain ? gain : 1));
	
}

void tirasu(struct char_data *ch,struct obj_data *target,struct obj_data *strumento,struct char_data *learner)
{
	int tipopesce,vnum;
	struct obj_data *nuovo = NULL;
	
	if (GET_PESCA_STATUS(ch) == 0)
	{
		send_to_char("Devi prima buttare la lenza in acqua.\r\n", ch);
		return;
	}
	
	if (GET_PESCA_STATUS(ch) == 1)
	{
		send_to_char("Tiri su la lenza ma non trovi nulla sull'amo.\r\n", ch);
	} else {
		
		if (check_proficienza(ch, PROF_PESCATORE, 100, 0))
		{
			tipopesce = get_nome_pesce();
			sprintf(buf, "Tiri su la tua preda!.\r\n");
			send_to_char(buf, ch);
			if (GET_OBJ_VAL(target, 1)==1) GET_OBJ_VAL(target, 1) = -1;
			else if (GET_OBJ_VAL(target, 1) !=0) GET_OBJ_VAL(target, 1)--;
			switch (tipopesce) {
				case 0:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_ALBORELLA;
					break;
				case	1:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_GATTO;
					break;
				case	2:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_BRANZINO;
					break;
				case	3:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_TROTA;
					break;
				case	4:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_LUCCIO;
					break;				
				case	5:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_STORIONE;
					break;
				case	6:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_TONNO;
					break;
				case	7:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_SQUALO;
					break;
				case	8:	vnum = OBJVNUM_CIBO+CIBO_PESCE_FRESCO_MAZZUGORU;
					break;
				default: vnum = NOTHING;
			}
			if (vnum != NOTHING)
				nuovo = read_object(vnum, VIRTUAL);
			else {
				mudlog("SYSERR: Cibo inesistente!",CMP,LVL_GOD,TRUE);						
				send_to_char("Cibo Inesistente, Contata un CREATORE!\r\n",ch);
				return;	
			}
			
			GET_OBJ_WEIGHT(nuovo) = tipopesce + number(0,1) + 1;
			//if (tipopesce==8) GET_OBJ_WEIGHT(nuovo) *=2; /*mazzugoru*/
			//Valore Nutritivo (un terzo, dei pesci cotti)
			//GET_OBJ_VAL(nuovo, 0) = (( (GET_OBJ_VAL(nuovo, 0)+(GET_OBJ_WEIGHT(target))) * (GET_PROF(ch,PROF_PESCATORE)/(double) (200-GET_PROF(ch,PROF_CUCINARE))))+1)/3;
						//Valore in Soldoni :-)
			//GET_OBJ_COST(nuovo)   = (GET_OBJ_COST(nuovo) * 7 /*GET_OBJ_VAL(nuovo, 0) */ * GET_PROF(ch,PROF_PESCATORE))/10;						
			GET_OBJ_COST(nuovo) = calcola_costo(ch,nuovo,NULL);
			GET_OBJ_RENT(nuovo)   = GET_OBJ_COST(nuovo) / 10;
			SET_BIT(GET_OBJ_EXTRA(nuovo), ITEM_RESTRING);
			
			sprintf(buf, "%s da %d kg", nuovo->short_description,GET_OBJ_WEIGHT(nuovo));
			nuovo->short_description = str_dup(buf);
			sprintf(buf, "%s da %d kg.", nuovo->description,GET_OBJ_WEIGHT(nuovo));
			nuovo->description = str_dup(buf);
			
			act("$n pesca $p.", FALSE, ch, nuovo, 0, TO_ROOM);
			act("Peschi $p.", FALSE, ch, nuovo, 0, TO_CHAR);
			manufatti_to_char(nuovo, ch, 1);
			check_danno_oggetto(ch, strumento, 1);		
		} else
		{
			act("acc....ti scappa!.", FALSE, ch, 0, 0, TO_CHAR);
			act("$n si fa sfuggire la sua preda.", FALSE, ch, 0, 0, TO_ROOM);
			//check_proficienza_improve(ch, PROF_PESCATORE, 2);
		}
		
	}// else
	
	GET_PESCA_STATUS(ch) = 0;
	if (GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT))
		event_cancel(GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT));
	GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT) = NULL;
}

ACMD(do_pesca)
{
	struct obj_data *strumento, *target;
	struct event_pesca_data *evtime;
	struct char_data *victim, *learner = NULL;
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	int time,gain;
	
	strumento = GET_EQ(ch, WEAR_HOLD);
	if (!strumento)
	{
		send_to_char("Non mi sembra che tu stia tenendo uno strumento.\r\n", ch);
		return;
	}
	
	if (GET_OBJ_TYPE(strumento)!=ITEM_CANNA)
	{
		send_to_char("Devi usare una canna da pesca per poter pescare.\r\n", ch);
		return;
	}
	
	if ((SECT(IN_ROOM(ch)) != SECT_WATER_NOSWIM) && (SECT(IN_ROOM(ch)) != SECT_WATER_SWIM))
	{
		send_to_char("Non puoi pescare qui.\r\n", ch);
		return;
	}
	
	argument = one_argument(argument, arg2);
	if (!*arg2)
	{
		send_to_char("Peschi che cosa?\r\n", ch);
		return;
	}
	
	generic_find(arg2, FIND_OBJ_ROOM , ch, &victim, &target);
	if (!target)
	{
		sprintf(buf, "Non vedi %s.\r\n", arg2);
		send_to_char(buf, ch);
		return;
	}
	
	if (subcmd == SCMD_TIRASU) {
		
   // Insegnamento by Shade
		one_argument(argument, arg3);
		if (*arg3 && (!str_cmp(arg3,"con"))) {
			one_argument(argument, arg4);
			if (!(learner = get_char_room_vis(ch, arg4)))
			{
				send_to_char("Non c'e' nessuno con quel nome con te.\r\n", ch);
				return;
			}	 
		}
		
		tirasu(ch,target,strumento,learner);
		return;
	}
	
	if ((GET_OBJ_TYPE(target)!=ITEM_NATURALE) || (GET_OBJ_MAT_NUM(target)!=2))
	{
		sprintf(buf, "Non puoi pescare %s.\r\n", arg2);
		send_to_char(buf, ch);
		return;
	}
	
	if (GET_PESCA_STATUS(ch) > 0)
	{
		send_to_char("Hai gia' la lenza in acqua.\r\n", ch);
		return;
	}
	
	if (GET_OBJ_VAL(target, 1) < 0)
	{
		send_to_char("mmm, ma ! non vedi piu' pesci in questa zona.\r\n", ch);
		return;
	}
	
	if (GET_OBJ_TYPE(target)==ITEM_NATURALE)
	{
		sprintf(buf, "!!SOUND(Pescatore.wav)Butti la lenza in acqua nella speranza che qualcosa abbocchi.\r\n");
		send_to_char(buf, ch);
		act("$n butta la lenza in acqua.", FALSE, ch, 0, 0, TO_ROOM);
		gain = 3 + (GET_PROF(ch, PROF_PESCATORE) / 40);
		time = PULSES_PER_MUD_HOUR / (gain ? gain : 1);
		CREATE(evtime, struct event_pesca_data, 1);
		evtime->times = 0;
		evtime->ch = ch;
		evtime->target = target;
		GET_PESCA_STATUS(ch) = 1;
		
		if (GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT))
			event_cancel(GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT));
		GET_POINTS_ACT_EVENTS(ch, PESCA_EVENT) =
			event_create(pesca_event_proc, evtime, time);
		
	} else {
		sprintf(buf, "Non vedi nulla da pescare.\r\n");
		send_to_char(buf, ch);
	}
}


/*************************************** CACCIA BY LANCE************************************/

void selvaggina_free(struct char_data *ch, int times)
{
	struct coord chcoord;  /* Coord of character */
 //struct coord victcoord;   Coord of victim
	struct char_data *k;
	int tempcount;
	long room;
	long vch, tempx=1, tempy=1;
	int found = 0;
	int dir=0;
	room_rnum target_room;
	  
	if (!ch || (ch->in_room == -1)) 
	  return;
	
	room = (IN_ROOM(ch));
	
	vch = world[ch->in_room].number;
	
	if (IS_IN_WILD(ch)) {
		wild_coord_from_virtual(world[ch->in_room].number, &chcoord);
		for (tempx = chcoord.x - RANGE_CACCIATORE;tempx < chcoord.x + RANGE_CACCIATORE;tempx++) {
			for (tempy = chcoord.y - RANGE_CACCIATORE;tempy < chcoord.y + RANGE_CACCIATORE;tempy++) {
				if (IS_IN_MINIWILD(ch)) {
				  if ((tempx < 65) && (tempy < 57))
				    room = real_room(MINIWILD_VNUM(real_zone(vch), tempx, tempy));
				  else
				    continue;
				}
				else
				  room = real_room(WILD_VNUM(tempx, tempy));
				
				if (room != NOWHERE) {
					for (k = world[room].people; k; k = k->next_in_room) {
						if (   k 
						    && !IS_NPC(k) 
						    && (GET_LEVEL(k) < LVL_IMMORT) 
						    && (!check_proficienza(k, PROF_CACCIARE, 100, -30))
						   ) {
						  found = 1;
						  break;
						}
					}
				}
			  if ((found == 1))
			    break;
			} //ForY
			if ((found == 1))
			  break;
		} //ForX
		
		if (found == 0)
		  return;
		
		for (tempcount = 0;tempcount < times;tempcount++) {
			
			if (found > 1) {
			  if (tempx < chcoord.x) 
				  dir = 0;
			  if (tempx >= chcoord.x) 
				  dir = 2;
				found = 1;
			}
			else {
			  if (tempy < chcoord.y)
				  dir = 1;
			  if (tempy >= chcoord.y)
			    dir = 3;
			  found = 2;
			}

      if (MOB_FLAGGED(ch, MOB_WILDHUNT)) 
        dir = wild_mobhunter(ch);
                        
      if (dir >= 0 && dir < NUM_OF_DIRS && IS_IN_WILD(ch)) 
        target_room = wild_target_room(ch, dir);
      else 
        target_room = -1;
            
      if (   !MOB_FLAGGED(ch, MOB_SENTINEL) 
          && (GET_POS(ch) == POS_STANDING) 
          && (target_room != -1) 
          && (   !MOB_FLAGGED(ch, MOB_STAY_ZONE) 
              || (world[target_room].wild_rnum == world[ch->in_room].wild_rnum)
             )
         ) {
        perform_move(ch, dir, 1, CAN_SHOW_ROOM);
      }
       
		} //FleeFor
		
	} //IsinWild 
}


ACMD(do_caccia)
{
	struct char_data *vict, *learner = NULL;
	struct obj_data *strumento;
	int test;
	void raw_kill(struct char_data * ch,  struct char_data * killer);
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	if (!ch) return;
	
	argument = one_argument(argument, arg2);
	if (!*arg2)
	{
		send_to_char("Cacciare che cosa?\r\n", ch);
		return;
	}
	
    // Insegamento by Shade
	one_argument(argument, arg3);
	if (*arg3 && (!str_cmp(arg3,"con"))) {
		one_argument(argument, arg4);
		if (!(learner = get_char_room_vis(ch, arg4)))
		{
			send_to_char("Non c'e' nessuno con quel nome con te.\r\n", ch);
			return;
		}	 
	}
	
	strumento = GET_EQ(ch, WEAR_HOLD);
	if (!strumento)
	{
		send_to_char("Non hai nessun arma da caccia... rinunci!.\r\n", ch);
		return;
	}
	
	if (GET_OBJ_TYPE(strumento)!=ITEM_CACCIA)
	{
		send_to_char("Non stai tenendo l'arma adatta.\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, arg2)))
	{
		send_to_char("Non c'e' nessuno con quel nome da cacciare.\r\n", ch);
		return;
	}
	
	if (!IS_NPC(vict) || !MOB_FLAGGED(vict,MOB_SELVAGGINA) || MOB_FLAGGED(vict,MOB_HARD_TAME))
	{
		send_to_char("Non lo puoi cacciare come fosse un animale.\r\n", ch);
		return;
	}
	
   /* max 3 liv per la pelli*/
	test = number(0,100) + (GET_LEVEL(vict)*15);
	
	if ((GET_PROF(ch,PROF_CACCIARE)-test) <= -40)
	{
		act("$n si avventa su $N ma viene scaraventato a terra.", FALSE, ch, 0, vict, TO_ROOM);
		act("Intraprendi la lotta con $N che pero' ha la meglio su di te.", FALSE, ch, 0, vict, TO_CHAR);
		do_flee(vict, 0, 0, 0);
		do_flee(vict, 0, 0, 0);
		do_flee(vict, 0, 0, 0);
		do_flee(vict, 0, 0, 0);
		do_flee(vict, 0, 0, 0);
		WAIT_STATE(ch, 10 * PASSES_PER_SEC);
		extract_char(vict);
		check_proficienza_improve(ch, PROF_CACCIARE, 2);
		return;
	}
	
	if ((GET_PROF(ch,PROF_CACCIARE)-test) < 0)
	{
		act("$n si avventa su $N ma non riesce a prenderla.", FALSE, ch, 0, vict, TO_ROOM);
		act("Ti avventi su $N tua preda ma agilmente ti scappa.", FALSE, ch, 0, vict, TO_CHAR);
		do_flee(vict, 0, 0, 0);
		do_flee(vict, 0, 0, 0);
		do_flee(vict, 0, 0, 0);
		check_proficienza_improve(ch, PROF_CACCIARE, 2);
		return;
	}
	
  /*die, bastard!*/
	act("$n riesce a prendere ed ad uccidere $N.", FALSE, ch, 0, vict, TO_ROOM);
	act("La tua abilita' nel cacciare e' invidiabile. Catturi ed uccidi $N.", FALSE, ch, 0, vict, TO_CHAR);
	raw_kill(vict,ch);
}


void make_resurce(struct char_data *ch)
{
	struct obj_data *obj;
	int i = 0;
	
	if (!ch) return;
	
	obj = read_object(OBJVNUM_RISORSA_CASUALE, VIRTUAL);
	if (!obj)
	{
		mudlog("Non esiste l'oggetto OBJVNUM RISORSA CASUALE", NRM, LVL_GOD, TRUE);
		return;
	}
	GET_OBJ_TYPE(obj) = ITEM_PELLI;
	GET_OBJ_MAT_TYPE(obj) = PELLI;
	GET_OBJ_MAT_NUM (obj) = 0;
	if (MOB_FLAGGED(ch,MOB_RIS_CARNE)) {i = MIN(NUM_MATERIALI_PELLI,0); REMOVE_BIT(MOB_FLAGS(ch),MOB_RIS_CARNE); } else
	if (MOB_FLAGGED(ch,MOB_RIS_PELLE)) {i = MIN(NUM_MATERIALI_PELLI,1);  REMOVE_BIT(MOB_FLAGS(ch),MOB_RIS_PELLE); } else
	if (MOB_FLAGGED(ch,MOB_RIS_STOFFA)) {i = MIN(NUM_MATERIALI_PELLI,2); REMOVE_BIT(MOB_FLAGS(ch),MOB_RIS_STOFFA); } else
	if (MOB_FLAGGED(ch,MOB_RIS_PELLICCIA)) {i = MIN(NUM_MATERIALI_PELLI,3); REMOVE_BIT(MOB_FLAGS(ch),MOB_RIS_PELLICCIA); }else
	if (MOB_FLAGGED(ch,MOB_RIS_SCAGLIE)) {i = MIN(NUM_MATERIALI_PELLI,4); REMOVE_BIT(MOB_FLAGS(ch),MOB_RIS_SCAGLIE); }
	GET_OBJ_MAT_NUM (obj) = i;
	GET_OBJ_TSLOTS(obj) = 50;
	GET_OBJ_CSLOTS(obj) = GET_OBJ_TSLOTS(obj);
	GET_OBJ_WEIGHT(obj) = GET_WEIGHT(ch);
	GET_OBJ_COST(obj)   = 100;
	GET_OBJ_RENT(obj)   = 0;
	GET_OBJ_VAL(obj, 1) = GET_EXP(ch);
	GET_OBJ_VAL(obj, 2) = MIN(GET_LEVEL(ch) - 1,MAX_LIV_MOB_SELV); //Qualità della pelli
	SET_BIT(GET_OBJ_EXTRA(obj), ITEM_RESTRING);
	sprintf(buf, "Un pezzo di cadavere di %s e' qui.", GET_NAME(ch));
	obj->description = str_dup(buf);
	sprintf(buf, "un pezzo di cadavere di %s", GET_NAME(ch));
	obj->short_description = str_dup(buf);
	sprintf(buf, "cadavere %s", GET_NAME(ch));
	obj->name = str_dup(buf);
	obj_to_room(obj, IN_ROOM(ch));
}

void make_resurces(struct char_data *ch)
{
	if (!ch) return;
	
	if (MOB_FLAGGED(ch,MOB_RIS_CARNE)) make_resurce(ch);
	if (MOB_FLAGGED(ch,MOB_RIS_PELLE)) make_resurce(ch);
	if (MOB_FLAGGED(ch,MOB_RIS_STOFFA)) make_resurce(ch);
	if (MOB_FLAGGED(ch,MOB_RIS_PELLICCIA)) make_resurce(ch);
	if (MOB_FLAGGED(ch,MOB_RIS_SCAGLIE)) make_resurce(ch);
}



 /*******************************ARCHITETTO BY LANCE***********************************/

ACMD(do_notifica) {

  struct char_data *vict;
  struct obj_data *strumento;
  struct room_data *new_room;
  struct descriptor_data *d;
  struct house_control_rec temp_house;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  int costr_liv = 100;
  int zona,new_dir,dir,counter;
  long start_rnum,room,real_num,new_room_vnum,VNUM;
	
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);
	
  if (!*arg1) {
	send_to_char(
      "USO: \r\n"
      "NOTIFICA <Nome Funzionario> <Tipo Costruzione> <Direzione in cui costruire>\r\n", ch);
	return;
  }
	
  if (!*arg2) {
	send_to_char("Notifichi che cosa?\r\n", ch);
	return;
  }
	
  if (!*arg3) {
	send_to_char("In che direzione inizi a costruire?\r\n", ch);
	return;
  }
	
  if ((dir = parse_dir(arg3)) == -1) {
	sprintf(buf, "%s non e' una direzione valida.\r\n", arg3);
	send_to_char(buf, ch);
	return;
  }
	
  if (!(vict = get_char_room_vis(ch, arg1))) {
	send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
	return;
  }

  if (PLR_FLAGGED(vict, PLR_FANTASMA) && !IS_NPC(vict)) {
	send_to_char("Un fantasma non puo autorizzarti a iniziare i lavori.\r\n",ch);
	return;
  }

  if (!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict)){
	send_to_char("Se non gioca di ruolo come puo autorizzarti a iniziare i lavori.\r\n",ch);
	return;
  }

  strumento = GET_EQ(vict, WEAR_HOLD);

  if (!strumento) {
	sprintf(buf, "%s non ha in mano l'autorizzazione per cominciare i lavori di costruzione. Forse non e' d'accordo?!\r\n", GET_NAME(vict));
	send_to_char(buf, ch);
	return;
  }
	
  if (GET_OBJ_TYPE(strumento)!= ITEM_MANDATO) {
	sprintf(buf, "%s deve avere in mano il mandato reale per iniziare a costruire.\r\n", GET_NAME(vict));
	send_to_char(buf, ch);
	return;
  }

  if (GET_LEVEL(vict) < LVL_BUILDER) {
    if (IS_NPC(vict) || !PLR_FLAGGED(vict, PLR_NOTAIO)) {
	  sprintf(buf, "%s non ha l'autorita' per farti costruire qui.\r\n", GET_NAME(vict));
      send_to_char(buf, ch);
      return;
    }
  }

  if (str_cmp(arg2,"casa") && str_cmp(arg2,"abitazione") && str_cmp(arg2,"castello")) {
	send_to_char(
       "Uso: notifica <notaio> <costruzione> <direzione>\r\n"
		"Tipi di costruzione:                 \r\n"
		"- Casa       (composta da una stanza)\r\n"
		"- Abitazione (composta da 5 stanze)  \r\n"
		"- Castello   (stanze illimitate)     \r\n", ch);
	return;
  }
	
  if (!str_cmp(arg2,"casa"))       costr_liv = LIV_CASA;
  if (!str_cmp(arg2,"abitazione")) costr_liv = LIV_ABITAZIONI;
  if (!str_cmp(arg2,"castello"))   costr_liv = LIV_CASTELLI;
	
  if (GET_PROF(ch, PROF_ARCHITETTURA) < costr_liv) {
	sprintf(buf, "Non sei ancora in grado di costruire una costruzione di tipo %s.\r\n", arg2);
	send_to_char(buf, ch);
	return;
  }
	
  room = IN_ROOM(vict);

  if (EXIT2(room, dir))	{
	send_to_char("Non puoi costruire in questa direzione!\r\n", ch);
	return;
  }
	
  if (   ROOM_FLAGGED(room, ROOM_CONSTRUZIONE)
      && (GET_PROF(ch, PROF_ARCHITETTURA) < LIV_ABITAZIONI) ) {
	send_to_char("Non sei ancora in grado di costruire una costruzione piu' grande di una casa.\r\n", ch);
	return;
  }
	
  if (   ROOM_FLAGGED(room, ROOM_STOP_COSTR)
      && (GET_PROF(ch, PROF_ARCHITETTURA) < LIV_CASTELLI) )	{
	send_to_char("Non sei ancora in grado di costruire una costruzione piu' grande di un'abitazione.\r\n", ch);
	return;
  }
	
  if (!(ROOM_FLAGGED(room, ROOM_HOUSE) || ROOM_FLAGGED(room, ROOM_EDIFICABILE))) {
	send_to_char("Non e' una zona edificabile.\r\n", ch);
	return;
  }
	
  new_room_vnum = -1;

  for (VNUM = START_VNUM_COSTR; VNUM < END_VNUM_COSTR; VNUM++) {
	if (real_room(VNUM) == -1) {
	  new_room_vnum = VNUM;
	  break;
	}
  }

  if (new_room_vnum == -1) {
	send_to_char("Non ci sono piu' aree disponibili per costruire (contattare un creatore)\r\n", ch);
	return;
  }
	
  if (!str_cmp(arg2, "casa") && ROOM_FLAGGED(room, ROOM_CONSTRUZIONE)) {
	send_to_char("Se costruisci ancora non si considera piu' una casa ma un abitazione.\r\n", ch);
	return;
  }
	
  //Costruzione
  d = ch->desc;
  new_dir = direzioneopposta(dir);
  zona = real_zone(new_room_vnum);
	
  CREATE(new_room, struct room_data, 1);
  new_room->number = new_room_vnum;
  new_room->zone = zona;
  new_room->name = str_dup("Edificio in costruzione");
  new_room->description = str_dup("Diversi cumuli di terra sono situati ai lati di ampi fossati, all'interno dei quali\r\n"
			"un'ampia quantita' di pietre, travi di legno e travi di metallo si interseca fino a\r\n"
			"formare le complicate fondamenta dell'edificio in costruzione. In un altro punto del\r\n"
			"sito i lavori sono ormai giunti a buon punto, cosi' prendono forma i pilastri portanti\r\n"
			"della costruzione mentre i primi materiali vengono messi da parte per iniziare la\r\n"
			"costruzione della facciata laterale. In un angolo, sedie, porte, tavoli,armadi sono\r\n"
			"stati depositati pronto per essere utilizzati ad edificio ultimato.\r\n");
  new_room->wild_rnum = -1;
  new_room->room_flags = 0;
  new_room->sector_type = SECT_INSIDE;
  
  
  
   for (counter = 0; counter < NUM_OF_DIRS; counter++)
	new_room->dir_option[counter] = NULL;

  CREATE(new_room->dir_option[new_dir], struct room_direction_data, 1);
  new_room->dir_option[new_dir]->general_description = str_dup("Entrata in costruzione");
  new_room->dir_option[new_dir]->keyword = str_dup("entrata");
  new_room->dir_option[new_dir]->to_room = IN_ROOM(ch);
  new_room->dir_option[new_dir]->key =-1;
  new_room->dir_option[new_dir]->exit_info = 0;
  new_room->ex_description = NULL;
  new_room->contents = NULL;
  new_room->people = NULL;
	
  CREATE(d->olc, struct olc_data, 1);
	
  OLC_ZNUM(d) = zona;
  OLC_NUM(d)  = new_room_vnum;
  OLC_ROOM(d) = new_room;
	
  //save zone
  redit_save_internally(d);
	
  real_num = real_room(new_room_vnum);
  
  /* Setto la flag BIG_HOUSE per fargli posare tutti gli oggetti */  
  SET_BIT(ROOM_FLAGS(real_num), ROOM_BIG_HOUSE);
	
  if (!ROOM_FLAGGED(room, ROOM_CONSTRUZIONE) && !ROOM_FLAGGED(room, ROOM_STOP_COSTR))
	SET_BIT(ROOM_FLAGS(real_num), ROOM_FIRST_ROOM);
	
  SET_BIT(ROOM_FLAGS(real_num), ROOM_HOUSE | ROOM_INDOORS | ROOM_NOMOB | ROOM_HOUSE_CRASH | ROOM_PRIVATE);
	
  if (str_cmp(arg2,"casa") && !ROOM_FLAGGED(room, ROOM_CONSTRUZIONE) && !ROOM_FLAGGED(room, ROOM_STOP_COSTR))
	SET_BIT(ROOM_FLAGS(real_num),  ROOM_CONSTRUZIONE);

  if (!str_cmp(arg2,"casa") && !ROOM_FLAGGED(room,ROOM_CONSTRUZIONE))
	SET_BIT(ROOM_FLAGS(real_num),  ROOM_STOP_COSTR);
	
  if (!str_cmp(arg2,"abitazione") && ROOM_FLAGGED(room,ROOM_CONSTRUZIONE))
	SET_BIT(ROOM_FLAGS(real_num),  ROOM_STOP_COSTR);
	
  if (!str_cmp(arg2,"castello") && ROOM_FLAGGED(room, ROOM_CONSTRUZIONE))
	SET_BIT(ROOM_FLAGS(real_num),  ROOM_CONSTRUZIONE);
	
  redit_save_to_disk(real_zone(new_room_vnum));
	
  //Crea entrata
  start_rnum = IN_ROOM(ch);
  CREATE(world[start_rnum].dir_option[dir], struct room_direction_data, 1);
  world[start_rnum].dir_option[dir]->general_description = str_dup("Entrata in un edificio");
  world[start_rnum].dir_option[dir]->keyword = str_dup("entrata");
  world[start_rnum].dir_option[dir]->to_room = real_num;
  world[start_rnum].dir_option[dir]->exit_info = 0;
  world[start_rnum].dir_option[dir]->key =-1;
  SET_BIT(ROOM_FLAGS(start_rnum), ROOM_ATRIUM);
	
  redit_save_to_disk(world[start_rnum].zone);
	
  //assegnazione della house
  temp_house.mode = HOUSE_PRIVATE;
  temp_house.vnum = new_room_vnum;
  temp_house.atrium = world[start_rnum].number;
  temp_house.exit_num = new_dir;
  temp_house.built_on = time(0);
  temp_house.last_payment = 0;
  temp_house.owner = GET_IDNUM(ch);
  temp_house.num_of_guests = 0;
  temp_house.pickdoor = 98;
	
  house_control[num_of_houses++] = temp_house;
  House_crashsave(new_room_vnum);

  if (ch != vict) {
	sprintf(buf,"%s notifica il permesso di costruzione.\r\n",GET_NAME(vict));
	send_to_char(buf, ch);
	sprintf(buf,"Dai il permesso a %s di iniziare i lavori di costruzione dell'edificio.\r\n",GET_NAME(ch));
	send_to_char(buf, vict);
  }

  act("Inizi i lavori di costruzione dell'edificio.", FALSE, ch, 0, 0, TO_CHAR);
  act("$n inizia i lavori di costruzione dell'edificio.", FALSE, ch, 0, 0, TO_ROOM);

  sprintf(buf,"Una nuova room viene creata da %s (vnum:%ld --> vnum:%ld zone: %d)",GET_NAME(ch),start_rnum, new_room_vnum, zona);
  mudlog(buf, NRM, LVL_GOD, TRUE);

  House_save_control();
  extract_obj(strumento);
  free(d->olc);
}


ACMD(do_costruisci)
{
	struct obj_data *strumento;
	struct obj_data *object, *temp;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int indtipo = 0, legno = 0, metallo = 0, pietra=0, porta=0;
	int legno_necessario = 20, metallo_necessario = 20, pietra_necessaria=20;
	int costr_liv;
	long i;
	
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	
	if (IN_ROOM(ch) == -1) return;
	
	if (!*arg1 || (str_cmp(arg1,"casa") && str_cmp(arg1,"abitazione") && str_cmp(arg1,"castello")))
	{
		send_to_char("Che cosa..? (casa, abitazione, castello)\r\n", ch);
		return;
	}
	
	if (!*arg2 || (str_cmp(arg2,"legno") && str_cmp(arg2,"pietra") && str_cmp(arg2,"evoluta") && str_cmp(arg2,"mista")))
	{
		send_to_char("Di che tipo? (legno, pietra, evoluta, mista)  \r\n"
			"- legno     70% legna  20% pietra  10% metallo \r\n"
			"- pietra    15% legna  70% pietra  15% metallo \r\n"
			"- evoluta   20% legna  30% pietra  50% metallo \r\n"
			"- mista     33% legna  33% pietra  34% metallo \r\n"
			, ch);
		return;
	}
	
	costr_liv = 101;
	if (!str_cmp(arg1,"casa"))       costr_liv = LIV_CASA;
	if (!str_cmp(arg1,"abitazione")) costr_liv = LIV_ABITAZIONI;
	if (!str_cmp(arg1,"castello"))   costr_liv = LIV_CASTELLI;
	
	if (GET_PROF(ch, PROF_ARCHITETTURA) < costr_liv)
	{
		sprintf(buf, "Non sei ancora in grado di costruire una costruzione di tipo %s.\r\n", arg1);
		send_to_char(buf, ch);
		return;
	}
	
	if (!ROOM_FLAGGED(IN_ROOM(ch), ROOM_CONSTRUZIONE) && !ROOM_FLAGGED(IN_ROOM(ch), ROOM_STOP_COSTR))
	{
		send_to_char("Non stai costruendo nulla qui.\r\n", ch);
		return;
	}
	
	//conta il materiale
	for (object=world[IN_ROOM(ch)].contents; object; object=object->next_content)
	{
		if (GET_OBJ_TYPE(object) == ITEM_TRAVE_LEGNO) legno ++;
		if (GET_OBJ_TYPE(object) == ITEM_ROCCIA_SQUA) pietra ++;
		if (GET_OBJ_TYPE(object) == ITEM_TRAVE_COST) metallo ++;
		if (GET_OBJ_TYPE(object) == ITEM_PORTA) porta ++;
	}
	
	if (!str_cmp(arg2,"legno"))   indtipo = 0;
	if (!str_cmp(arg2,"pietra"))  indtipo = 1;
	if (!str_cmp(arg2,"evoluta")) indtipo = 2;
	if (!str_cmp(arg2,"mista"))   indtipo = 3;
	
	legno_necessario = (MATERIALE_X_COSTRUIRE * costruzione[indtipo][0] /100);
	pietra_necessaria = (MATERIALE_X_COSTRUIRE * costruzione[indtipo][1] /100);
	metallo_necessario = (MATERIALE_X_COSTRUIRE * costruzione[indtipo][2] /100);
	
	if ((legno < legno_necessario) || (pietra < pietra_necessaria) || (metallo < metallo_necessario) ||
		(porta < 1)) {
		sprintf(buf, "Materiale necessario per terminare i lavori:   \r\n"
			"Legno necessario    ---->  %d    \r\n"
			"Pietra necessaria   ---->  %d    \r\n"
			"Metallo necessario  ---->  %d    \r\n"
			"Porta               ---->  %d    \r\n",
			(legno_necessario - legno) < 0 ? 0 : (legno_necessario - legno),
			(pietra_necessaria - pietra) < 0 ? 0 : (pietra_necessaria - pietra),
			(metallo_necessario - metallo) < 0 ? 0 : (metallo_necessario - metallo),
			(porta < 1) ? 1 : 0);
		send_to_char(buf , ch);
		return;
	}
	
	strumento = GET_EQ(ch, WEAR_HOLD);
	if (!strumento)
	{
		send_to_char("Non hai nessun progetto approvato in mano... rinunci!.\r\n", ch);
		return;
	}
	
	if ((GET_OBJ_TYPE(strumento)!=ITEM_NOTE) || (!isname(strumento->name,"progetto")))
	{
		send_to_char("Devi avere in mano il progetto di costruzione approvato poter concludere i lavori.\r\n", ch);
		return;
	}
	
	if (!*argument)
	{
		send_to_char("E' possibile indicare il titolo che si vuole dare alla stanza. Es: Casa di Robert.\r\n"
			"Se non si vuole assegnare nessun titolo: costruisci <costruzione> <tipo> comune .\r\n", ch);
		return;
	}
	
	if (strumento->action_description && (*strumento->action_description))
		sprintf(buf2,"%s\r\n",strumento->action_description);
	else
		sprintf(buf2,"%s",
			"Ledificio emana ancora gli odori caratteristici della calce e della segatura\r\n"
			"prodottasi nella lavoratura e nalla disposizione delle travi piu esterne.\r\n"
			"Nel suo insieme, la casa mostra ora chiaramente limpronta impressa dal suo\r\n"
			"materiale di costruzione dominante, ora non resta che arredarla e personalizzarla\r\n"
			"per renderla qualcosa di piu di una semplice palazzina.\r\n");
	
	if (!str_cmp(argument," comune"))
		sprintf(buf,"Un edificio costruito da %s", GET_NAME(ch));
	else
		sprintf(buf,"%s (%s)", argument ,arg2);
	world[IN_ROOM(ch)].name = str_dup(buf);
	if (strumento->action_description && (*strumento->action_description)) 
		world[IN_ROOM(ch)].description = str_dup(buf2);
	else {
		if (indtipo == 0) sprintf(buf,
				"%sLa casa basa la sua struttura su una ampia componente di legno di svariati\r\n"
				"tipi, unita alla presenza minore in pari misura di pietra per le fondamenta e di\r\n"
				"metalli per le parti strutturali portanti dell'edificio. Esternamente in ogni caso,\r\n"
				"l'edificio sembra essere realizzato totalmente con travi di costruzione in legno,\r\n"
				"che danno al palazzo unaria alquanto grezza ma tuttavia confortevole.\r\n", buf2);
		
		if (indtipo == 1) sprintf(buf,
				"%sLa casa basa la sua struttra su massicci blocchi di marmo e granito, visibili\r\n"
				"sia dallesterno che dallinterno e componenti piu dei due terzi del materiale\r\n"
				"utilizzato per la costruzione totale delledificio. Le uniche parti realizzate in\r\n"
				"metallo sono difatti parti minori della struttura della parte superiore dellabitato\r\n"
				"mentre il legno viene per lo piu utilizzato allinterno come isolante a basso costo\r\n"
				"e per le tegole presenti sul tetto in grande quantita. La struttura esternamente ha\r\n"
				"unaria piuttosto solenne e borghese con le sue pietre perfettamente incastonate come\r\n"
				"in un complesso mosaico.\r\n", buf2);
		
		if (indtipo == 2) sprintf(buf,
				"%sQuesto e il tipo di abitato piu raffinato e ricercato, caratteristico\r\n"
				"generalmente delle alte sfere fanatiche in quanto allutilizzazione estrema dei\r\n"
				"metalli nelle loro piu svariate forme. Cosi, esternamente, sotto a sottili strati\r\n"
				"piu' che altro estetici di quarzo, marmo o legna di vario genere, sono presenti\r\n"
				"diversi spessori in ferro o altri metalli costituenti un ottimo isolante dallesterno.\r\n"
				"Internamente la struttura superiore della casa e quasi totalmente realizzata in\r\n"
				"metallo, ad esclusione del tetto in tegole di legno estremamente raffinate.\r\n", buf2);
		
		if (indtipo == 3) sprintf(buf,
				"%sLedificio e un misto ibrido di diversi generi. Esternamente esso interavalla\r\n"
				"tratti in granito ad altri composti da ampi tronchi di vario genere. Il tetto e\r\n"
				"composto esternamente da lose di media fattura, mentre allinterno sono presenti\r\n"
				"ampie strutture metalliche che danno alla casa tratti piu evoluti rispetto alle\r\n"
				"strutture prevalentemente in pietra o legna.\r\n", buf2);
		
		world[IN_ROOM(ch)].description = str_dup(buf);
	}
	
	/* Rimuovo la flag BIG_HOUSE */
	
	REMOVE_BIT(ROOM_FLAGS(IN_ROOM(ch)), ROOM_BIG_HOUSE);
	
	if (!ROOM_FLAGGED(IN_ROOM(ch) , ROOM_FIRST_ROOM)) {
		if (ROOM_FLAGGED(IN_ROOM(ch) , ROOM_PRIVATE))
			REMOVE_BIT(ROOM_FLAGS(IN_ROOM(ch)), ROOM_PRIVATE);
		if (ROOM_FLAGGED(IN_ROOM(ch) , ROOM_HOUSE_CRASH))
			REMOVE_BIT(ROOM_FLAGS(IN_ROOM(ch)), ROOM_HOUSE_CRASH);
		if (ROOM_FLAGGED(IN_ROOM(ch) , ROOM_HOUSE))
			REMOVE_BIT(ROOM_FLAGS(IN_ROOM(ch)), ROOM_HOUSE);
		
		if (find_house(world[IN_ROOM(ch)].number) != -1) {
			sprintf(buf,"%d",world[IN_ROOM(ch)].number);
			hcontrol_destroy_house(0, buf);
			sprintf(buf," (GC) La house %d e' stata distrutta da %s",world[IN_ROOM(ch)].number,GET_NAME(ch));
			send_to_char("Richiedi al Regnante di concederti l'abitabilita' (house)!.\r\n", ch);
			mudlog(buf, NRM, LVL_GOD, TRUE);
			log(buf);
		}
	}
	
	if ((i = find_house(world[IN_ROOM(ch)].number)) != -1) {
		switch (indtipo){
			case 0:	    
				house_control[i].pickdoor = 92;
			case 1:	   
				house_control[i].pickdoor = 94;	   
			case 2:
				house_control[i].pickdoor = 98;
			case 3:	
				house_control[i].pickdoor = 96;
			default: 
				house_control[i].pickdoor = 80;	     
		}	
		House_save_control();	  
	}
	
	//save zone
	redit_save_to_disk(world[IN_ROOM(ch)].zone);
	
	// elimina materiale legno
	for (object=world[IN_ROOM(ch)].contents; (legno_necessario > 0); object = temp)
		if (!object) break; else {
			temp = object->next_content;
			if (GET_OBJ_TYPE(object) == ITEM_TRAVE_LEGNO){
				extract_obj(object); legno_necessario --;}}
	
	// elimina materiale pietra
	for (object=world[IN_ROOM(ch)].contents; (pietra_necessaria > 0); object = temp)
		if (!object) break; else  {
			temp = object->next_content;
			if (GET_OBJ_TYPE(object) == ITEM_ROCCIA_SQUA){
				extract_obj(object); pietra_necessaria --;}}
	
	 // elimina materiale metallo
	for (object=world[IN_ROOM(ch)].contents; (metallo_necessario > 0); object=temp)
		if (!object) break; else {
			temp = object->next_content;
			if (GET_OBJ_TYPE(object) == ITEM_TRAVE_COST){
				extract_obj(object); metallo_necessario --;}}
	
	 // elimina materiale porta
	for (object=world[IN_ROOM(ch)].contents; (porta > 0); object=temp)
		if (!object) break; else {
			temp = object->next_content;
			if (GET_OBJ_TYPE(object) == ITEM_PORTA) {
				extract_obj(object); break;}}
	
	send_to_char("Concludi i lavori di costruzione dell'edificio.\r\nSei veramente soddisfatto del tuo lavoro!\r\n", ch);	
	act("$n conclude i lavori di costruzione dell'edificio.", FALSE, ch, 0, 0, TO_ROOM);
	GET_PROF(ch,PROF_ARCHITETTURA) = MIN(100, GET_PROF(ch,PROF_ARCHITETTURA) + 2);
	extract_obj(strumento);
}


ACMD(do_acostruzione)
{
	long room, amount, house;
	char *timestr;
	struct obj_data *strumento;
	struct char_data *vict;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	time_t datavendita;
	
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	
	room = IN_ROOM(ch);
	
	if (room==-1) {
		send_to_char("Non puoi farlo qui.\r\n", ch);
		return;
	}
	
	if (!*arg1)
	{
		send_to_char("A chi la vendi?\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, arg1)))
	{
		send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict)){
	send_to_char("Non puoi vendere una casa ad un fantasma \r\n",ch);
	return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict)){
	send_to_char("Non puoi vendere una casa ad un persona che non gioca di ruolo\r\n",ch);
	return;
	}

	if (!*arg2)
	{
		send_to_char("A quanto la vendi?\r\n", ch);
		return;
	}
	
	if (!is_number(arg2)) {
		send_to_char("A quanto la vendi!!?... metti il prezzo.\r\n", ch);
		return;
	} else amount = atoi(arg2);
	
	if ((house = find_house(world[room].number))==-1)
	{
		send_to_char("Non puoi vendere questa casa.\r\n", ch);
		return;
	}
	
	if (house_control[house].owner != GET_IDNUM(ch))
	{
		send_to_char("Non puoi vendere questa casa, non e' tua!\r\n", ch);
		return;
	}
	
	if (GET_GOLD(vict)< amount)
	{
		sprintf(buf,"Noti che %s non possiede tutti questi soldi in contanti.\r\n",  GET_NAME(vict) );
		send_to_char(buf, ch);
		return;
	}
	
	strumento = GET_EQ(vict, WEAR_HOLD);
	if (!strumento)
	{
		sprintf(buf,"%s deve avere in mano un contratto da firmare.\r\n",  GET_NAME(vict) );
		send_to_char(buf, ch);
		sprintf(buf,"Devi avere in mano un contratto da firmare se vuoi comprare lìedificio da %s.\r\n",GET_NAME(ch));
		send_to_char(buf, vict);
		return;
	}
	
	if (GET_OBJ_TYPE(strumento)!=ITEM_CONTRATTO)
	{
		sprintf(buf,"Quello che %s ha in mano non e' un contratto.\r\n",  GET_NAME(vict));
		send_to_char(buf, ch);
		sprintf(buf,"Quello che hai in mano non e' un contratto, %s non puo' venderti l'edificio.\r\n",  GET_NAME(vict));
		send_to_char(buf, vict);
		return;
	}
	
	datavendita = time(0);
	timestr = asctime(localtime(&(datavendita)));
	*(timestr + 10) = 0;
	
	sprintf(buf,"%sContratto di vendita stipulato il %s da %s a favore di %s.\r\n"
		"Si notifica la vendita dell'edificio %s per un ammontare di %ld coins.\r\n",
		strumento->action_description ? strumento->action_description : "",
		timestr, GET_NAME(ch), GET_NAME(vict),world[room].name,amount);
	strumento->action_description = str_dup(buf);
	house_control[house].owner = GET_IDNUM(vict);
	House_save_control();
	GET_GOLD(ch) +=amount;
	GET_GOLD(vict) -=amount;
	sprintf(buf,"Vendi l'edificio a %s per %ld coins", GET_NAME(vict) ,amount);
	send_to_char(buf, ch);
	sprintf(buf,"$n ti vende questo edificio per %ld coins.", amount);
	act(buf, FALSE, ch, 0, vict, TO_VICT);
	act("$n vende questo edificio a $N.", FALSE, ch, 0, vict, TO_NOTVICT);
	
}


 /*******************************FINE ARCHITETTO****************************************************/


  /*******************************ADDESTRATORE BY LANCE**********************************************/

ACMD(do_insegna)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	struct char_data *vict;
	
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	
	if (!*arg1)
	{
		send_to_char("Cosa insegni?\r\n", ch);
		return;
	}
	
	if (!*arg2)
	{
		send_to_char("A chi lo insegni?\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, arg1)))
	{
		send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict)){
		send_to_char("Non riusciresti ad insegnare niente ad un fantasma.\r\n",ch);
		return;
	}
	if(!PRF_FLAGGED(vict,PRF_RPG) && !IS_NPC(vict)){
		send_to_char("Non ha senso insegnare a qualcuno che non gioca di ruolo.\r\n",ch);
		return;
	}
	
	
	if (str_cmp(arg2,"cavalcare") && str_cmp(arg2,"domare") && str_cmp(arg2,"montare"))
	{
		send_to_char("Puoi insegnare a cavalcare, domare e montare una cavalcatura.\r\n", ch);
		return;
	}
	
	if (!str_cmp(arg2,"domare")) {
		if (GET_PROF(ch, PROF_DOMARE) < 50)
		{
			send_to_char("Non sei ancora in grado di poter insegnare a domare, ti serve piu' esercizio.\r\n", ch);
			return;
		}
		
		if (GET_PROF(vict, PROF_DOMARE) > 50)
		{
			sprintf(buf,"%s, sa gia' tutto ormai su come si doma una cavalcatura.\r\n",  GET_NAME(vict));
			send_to_char(buf, ch);
			return;
		}
		
		if (GET_PROF(vict, PROF_DOMARE) >= GET_PROF(ch, PROF_DOMARE))
		{
			sprintf(buf,"%s, sa domare bene quanto te.\r\n",  GET_NAME(vict));
			send_to_char(buf, ch);
			return;
		}
		
		if (check_proficienza(ch, PROF_ADDESTRATORE, 100, 0)) {
			if (GET_PROF(vict, PROF_DOMARE) >= 20) {
				GET_PROF(vict, PROF_DOMARE) += 5;
				sprintf(buf,"Ascolti attentamente i consigli di %s ", GET_NAME(ch));
				send_to_char(buf, vict);
				act("Insegni a $N come si doma una cavalcatura.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n ti insegna come si doma una cavalcatura.", FALSE, ch, 0, vict, TO_VICT);
				act("$n insegna a $N come si doma una cavalcatura.", FALSE, ch, 0, vict, TO_NOTVICT);
				return;
			} else {
				act("Insegni a $N come si doma una cavalcatura.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n ti insegna come si doma una cavalcatura.", FALSE, ch, 0, vict, TO_VICT);
				act("$n insegna a $N come si doma una cavalcatura.", FALSE, ch, 0, vict, TO_NOTVICT);
				GET_PROF(vict, PROF_DOMARE) = 20;
			}
		} else {
			act("$N non capisce nulla di cio' che gli spieghi.", FALSE, ch, 0, vict, TO_CHAR);
			act("Non conosci nulla di nuovo dopo aver preso lezioni da $n.", FALSE, ch, 0, vict, TO_VICT);
			act("$n cerca di insegnare a $N come si doma una cavalcatura senza successo.", FALSE, ch, 0, vict, TO_NOTVICT);
		}
		return;
	}
	
	if (!str_cmp(arg2,"cavalcare")) {
		if (GET_PROF(ch, PROF_CAVALCARE) < 50)
		{
			send_to_char("Non sei ancora in grado di poter insegnare a cavalcare, ti serve piu' esercizio.\r\n", ch);
			return;
		}
		
		if (GET_PROF(vict, PROF_CAVALCARE) > 50)
		{
			sprintf(buf,"%s, sa gia' tutto ormai su come si cavalca una cavalcatura.\r\n",  GET_NAME(vict));
			send_to_char(buf, ch);
			return;
		}
		
		if (GET_PROF(vict, PROF_CAVALCARE) >= GET_PROF(ch, PROF_CAVALCARE))
		{
			sprintf(buf,"%s, sa cavalcare bene quanto te.\r\n",  GET_NAME(vict));
			send_to_char(buf, ch);
			return;
		}
		
		if (check_proficienza(ch, PROF_ADDESTRATORE, 100, 0)) {
			if (GET_PROF(vict, PROF_CAVALCARE) >= 20) {
				GET_PROF(vict, PROF_CAVALCARE) += 5;
				sprintf(buf,"Ascolti attentamente i consigli di %s ",  GET_NAME(ch));
				send_to_char(buf, vict);
				act("Insegni a $N come si cavalca una cavalcatura.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n ti insegna come si cavalca una cavalcatura.", FALSE, ch, 0, vict, TO_VICT);
				act("$n insegna a $N come si cavalca una cavalcatura.", FALSE, ch, 0, vict, TO_NOTVICT);
				return;
			} else {
				act("Insegni a $N come si cavalca una cavalcatura.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n ti insegna come si cavalca una cavalcatura.", FALSE, ch, 0, vict, TO_VICT);
				act("$n insegna a $N come si cavalca una cavalcatura.", FALSE, ch, 0, vict, TO_NOTVICT);
				GET_PROF(vict, PROF_CAVALCARE) = 20;
			}
		} else {
			act("$N non capisce nulla di cio' che gli spieghi.", FALSE, ch, 0, vict, TO_CHAR);
			act("Non conosci nulla di nuovo dopo aver preso lezioni da $n.", FALSE, ch, 0, vict, TO_VICT);
			act("$n cerca di insegnare a $N come si cavalca una cavalcatura senza successo.", FALSE, ch, 0, vict, TO_NOTVICT);
		}
		return;
	}
	
	if (!str_cmp(arg2,"montare")) {
		if (GET_PROF(ch, PROF_MONTARE) < 50)
		{
			send_to_char("Non sei ancora in grado di poter insegnare a montare, ti serve piu' esercizio.\r\n", ch);
			return;
		}
		
		if (GET_PROF(vict, PROF_MONTARE) > 50)
		{
			sprintf(buf,"%s, sa gia' tutto ormai su come si monta una cavalcatura.\r\n",  GET_NAME(vict));
			send_to_char(buf, ch);
			return;
		}
		
		if (GET_PROF(vict, PROF_MONTARE) >= GET_PROF(ch, PROF_MONTARE))
		{
			sprintf(buf,"%s, sa montare bene quanto te.\r\n",  GET_NAME(vict));
			send_to_char(buf, ch);
			return;
		}
		
		if (check_proficienza(ch, PROF_ADDESTRATORE, 100, 0)) {
			if (GET_PROF(vict, PROF_MONTARE) >= 20){
				GET_PROF(vict, PROF_MONTARE) += 5;
				sprintf(buf,"Ascolti attentamente i consigli di %s.\r\n",  GET_NAME(ch));
				send_to_char(buf, vict);
				act("Insegni a $N come si monta una cavalcatura.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n ti insegna come si monta una cavalcatura.", FALSE, ch, 0, vict, TO_VICT);
				act("$n insegna a $N come si monta una cavalcatura.", FALSE, ch, 0, vict, TO_NOTVICT);
				return;
			} else {
				act("Insegni a $N come si monta una cavalcatura.", FALSE, ch, 0, vict, TO_CHAR);
				act("$n ti insegna come si monta una cavalcatura.", FALSE, ch, 0, vict, TO_VICT);
				act("$n insegna a $N come si monta una cavalcatura.", FALSE, ch, 0, vict, TO_NOTVICT);
				GET_PROF(vict, PROF_MONTARE) = 20;
			}
		} else {
			act("$N non capisce nulla di cio' che gli spieghi.", FALSE, ch, 0, vict, TO_CHAR);
			act("Non conosci nulla di nuovo dopo aver preso lezioni da $n.", FALSE, ch, 0, vict, TO_VICT);
			act("$n cerca di insegnare a $N come si monta una cavalcatura senza successo.", FALSE, ch, 0, vict, TO_NOTVICT);
		}
		return;
	}
	
}


ACMD(do_ammaestra)
{
	int dam;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	struct char_data *vict, *learner = NULL;
	
	argument = one_argument(argument, arg1);
	
	if (!*arg1)
	{
		send_to_char("Ammaestri chi?.\r\n", ch);
		return;
	}
	
	if (!(vict = get_char_room_vis(ch, arg1)))
	{
		send_to_char("Non c'e' nessuno con quel nome.\r\n", ch);
		return;
	}
/*	
	if(ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)){
		send_to_char("Non vuoi rovinare questa sensazione di pace cercando di sottomettere un animale.\r\n",ch);
		return;
	}
*/
   // Insegamento by Shade
	one_argument(argument, arg2);
	if (*arg2 && (!str_cmp(arg2,"con"))) {
		one_argument(argument, arg3);
		if (!(learner = get_char_room_vis(ch, arg3)))
		{
			send_to_char("Non c'e' nessuno con quel nome con te.\r\n", ch);
			return;
		}	 
	}
	
	if (!IS_NPC(vict) || (!MOB_FLAGGED(vict, MOB_AMMAESTRABILE) && !MOB_FLAGGED(vict, MOB_HARD_TAME)))
	{
		sprintf(buf, "Non puoi ammaestrare %s\r\n" , GET_NAME(vict));
		send_to_char(buf, ch);
		return;
	}
	
	if (FIGHTING(vict)) {
		send_to_char("Non credo che tu ci riesca ora che sta combattendo.\r\n", ch);
		return;
	}
	
	if (vict->master)
	{
		send_to_char("Non ne vuole sapere di ascoltare te, sembra gia' ammaestrato.\r\n", ch);
		return;
	}
	
	act("Ti avvicini cautamente a $N........", FALSE, ch, 0, vict, TO_CHAR);
	act("$n si avvicina cautamente a $N.......", FALSE, ch, 0, vict, TO_ROOM);
	
	if (MOB_FLAGGED(vict, MOB_HARD_TAME)) {   
		if (GET_LEVEL(ch) < 50) {
			send_to_char("Non si fara' mai domare da te!!!\r\n", ch);
			return;	  
		}
		
		if (((GET_PROF(ch, PROF_ADDESTRATORE)/5) + GET_WIS(ch) - 9) >= number(0, 100)) {
			act("&7Riesci a domare $N!&0", FALSE, ch, 0, vict, TO_CHAR);
			act("&7$n riesce a domare $N con la sua maestria nell'ammaestrare le creature.&0", FALSE, ch, 0, vict, TO_ROOM);
			domadragon(ch, vict);
		} 
		else {
			if ((GET_PROF(ch, PROF_ADDESTRATORE) + GET_WIS(ch)- 9 ) < number(0,100)) {
				act("&1$N s'inferocisce contro di te e ti soffia addosso!&0", FALSE, ch, 0, vict, TO_CHAR);
				act("&1$N s'inferocisce contro $n e gli soffia addosso!&0", FALSE, ch, 0, vict, TO_ROOM);
				dam = dice(GET_LEVEL(vict), 10);
				damage(ch, vict, MAX(dam, 1), -1);
			} else {
				act("$N sembra contrariato ... in non ci riproverei una seconda volta.", FALSE, ch, 0, vict, TO_CHAR);
				act("$N sembra infastidito dall'atteggiamento di $n.", FALSE, ch, 0, vict, TO_ROOM);
				check_proficienza_improve(ch, PROF_ADDESTRATORE, 15);
			}
		}
		return;
	}
	
	if (check_proficienza(ch, PROF_ADDESTRATORE, 100, - (int) GET_LEVEL(vict)*1.5)) {
		
		REMOVE_BIT(MOB_FLAGS(vict), MOB_AGGRESSIVE);
		REMOVE_BIT(MOB_FLAGS(vict), MOB_SPEC);
		
		SET_BIT(MOB_FLAGS(vict), MOB_SENTINEL);
		
		if (vict->master)
			stop_follower(vict);
		add_follower(vict, ch);
		
		if (!MOB_FLAGGED(vict, MOB_MOUNTABLE))
			SET_BIT(AFF_FLAGS(vict), AFF_CHARM);
		
		act("Riesci a domare e ad ammaestrare $N.", FALSE, ch, 0, vict, TO_CHAR);
		act("$n riesce a domare e ad ammaestrare $N.", FALSE, ch, 0, vict, TO_ROOM);
	} 
	else {
		if (GET_PROF(ch, PROF_ADDESTRATORE) < number(0,100)) {
			act("&1$N s'inferocisce contro di te e ti attacca!&0", FALSE, ch, 0, vict, TO_CHAR);
			act("&1$N s'inferocisce contro $n e lo attacca!&0", FALSE, ch, 0, vict, TO_ROOM);
			dam = dice(GET_LEVEL(vict), 10);
			damage(ch, vict, MAX(dam, 1), -1);
			check_proficienza_improve(ch, PROF_ADDESTRATORE, 10);
		} 
		else {
			act("$N sembra contrariato .... poi diventa indifferente.", FALSE, ch, 0, vict, TO_CHAR);
			act("$N sembra infastidito dall'atteggiamento di $n.", FALSE, ch, 0, vict, TO_ROOM);
			check_proficienza_improve(ch, PROF_ADDESTRATORE, 5);
		}
	}
	
}



   /*******************************ADDESTRATORE BY LANCE FINE**********************************************/
