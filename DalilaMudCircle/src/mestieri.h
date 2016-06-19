/**************************************************************
 *** Mesteri.h - strutture e defines per il file mestieri.c ***
 **************************************************************/

#define NUM_MATERIALI            9

#define NON_DEFINITO      0
#define PIETRE            1
#define ALBERI            2
#define GEMME             3
#define ROCCE             4
#define PELLI             5
#define NATURA            6
#define ERBE              7

#define ERBA_CAFFE	14

//Materiale Pelle
#define PEL_CARNE		0
#define PEL_PELLE		1
#define PEL_STOFFA		2
#define PEL_PELLICCIA	3
#define PEL_SCAGLIE		4

//Materiale Natura
#define NAT_FARINACEI	0
#define NAT_CARTA		1
#define NAT_PESCE		2
#define NAT_FRUTTA		3
#define NAT_VERDURA		4

//Materiale Gemme
#define GEM_DIAMANTE		0
#define GEM_TOPAZIO		1
#define GEM_SMERALDO		2
#define GEM_RUBINO		3
#define GEM_ZAFFIRO		4

#define RES_AMMAESTRABILI  7
#define RES_SELVAGGINA     6
#define RES_ERBE           5
#define RES_NATURA         4
#define RES_ROCCE          3
#define RES_GEMME          2
#define RES_ALBERI         1
#define RES_PIETRE         0 

#define NUM_MATERIALI_PIETRE  6
#define NUM_MATERIALI_ALBERI  9
#define NUM_MATERIALI_GEMME   5
#define NUM_MATERIALI_ROCCE   3
#define NUM_MATERIALI_PELLI   5
#define NUM_MATERIALI_NATURA  5
#define NUM_MATERIALI_ERBE   15

#define OBJVNUM_METALLO_GREZZO    8
#define OBJVNUM_LINGOTTO         13
#define OBJVNUM_ARMOR            16
#define OBJVNUM_TRAVE_COST       17
#define OBJVNUM_WEAPON           18
#define OBJVNUM_TRONCO           19
#define OBJVNUM_TRAVE_LEGNO      20
#define OBJVNUM_ASSE_LEGNO       21
#define OBJVNUM_PIETRA_GREZZA    25
#define OBJVNUM_PIETRA_PULITA    26
#define OBJVNUM_GEMMA            27
#define OBJVNUM_ROCCIA_GREZZA    28
#define OBJVNUM_ROCCIA_SQUADRATA 29
#define OBJVNUM_PRODOTTO         30 //Deprecated
#define OBJVNUM_RISORSA_CASUALE  31
#define OBJVNUM_ERBA_GREZZA      32
#define OBJVNUM_ERBA_PULITA      33
#define OBJVNUM_PESCE            38 //Deprecated
#define OBJVNUM_CIBO_PULITO      39 
#define OBJVNUM_CIBO_COTTO       40 //Deprecated
#define OBJVNUM_SELV_PULITA      41
#define OBJVNUM_LIMONE		 47
#define OBJVNUM_CAFFELIMONE	 48
#define OBJVNUM_BENDE_UNTE	 54


// Alchimista Lance
//Vnum iniziale delle pozioni
#define OBJVNUM_POZIONI 		12601
//numero pozioni creabili
#define NUM_POZIONI_CREABILI 	20
//numero massiono di ingredienti per pozione
#define MAX_ERBE_X_POZIONE 		5
//pesca
#define NUM_SPECIE_PESCE 		9

/*********/
/* CUOCO */
/*********/
#define OBJVNUM_CIBO 		60000 //Vnum iniziale Cibi finiti
//Farinacei
#define CIBO_PROD_FARINACEI			50
#define CIBO_PAGNOTTA			0
#define CIBO_FOCACCIA			1
#define CIBO_PIZZA				2
#define CIBO_PANE				3
#define CIBO_PASTELLA			4
#define CIBO_FOCACCIONA			35 
//Frutta
#define CIBO_LAMPONE				5  
#define CIBO_FRAGOLA				6
#define CIBO_MELE				7
#define CIBO_CILIEGIA			8
#define CIBO_MORA				9
#define CIBO_PESCA				10
#define CIBO_ALBICOCCA			11
//Pesce Fresco
#define CIBO_PESCE_FRESCO_ALBORELLA	12 
#define CIBO_PESCE_FRESCO_GATTO		13
#define CIBO_PESCE_FRESCO_BRANZINO		14
#define CIBO_PESCE_FRESCO_TROTA		15
#define CIBO_PESCE_FRESCO_LUCCIO		16
#define CIBO_PESCE_FRESCO_STORIONE		17
#define CIBO_PESCE_FRESCO_TONNO		18
#define CIBO_PESCE_FRESCO_SQUALO		19
#define CIBO_PESCE_FRESCO_MAZZUGORU	20
//Pesce Cotto
#define CIBO_PESCE_COTTO_ALBORELLA		21
#define CIBO_PESCE_COTTO_GATTO		22
#define CIBO_PESCE_COTTO_BRANZINO		23
#define CIBO_PESCE_COTTO_TROTA		24
#define CIBO_PESCE_COTTO_LUCCIO		25
#define CIBO_PESCE_COTTO_STORIONE		26
#define CIBO_PESCE_COTTO_TONNO		27
#define CIBO_PESCE_COTTO_SQUALO		28
#define CIBO_PESCE_COTTO_MAZZUGORU		29

#define CIBO_VERDURA				30
#define CIBO_PROD_VERDURA			49
//Dolci
#define CIBO_CROSTATA			31
#define CIBO_DOLCE				32
#define CIBO_TORTA				33
#define CIBO_GALLETTA			34
//Pasticcini
#define CIBO_BABBA				36 
#define CIBO_CANNOLO				37 
#define CIBO_BIGNE				38 
#define CIBO_SFOGLIA				39 
//Carne Cotta
#define CIBO_CARNE_COTTA_BUONA		40
#define CIBO_CARNE_COTTA_OTTIMA		41
#define CIBO_CARNE_COTTA_ECCELLENTE	42
//Carne Cruda
#define CIBO_CARNE_FRESCA_BUONA		43
#define CIBO_CARNE_FRESCA_OTTIMA		44
#define CIBO_CARNE_FRESCA_ECCELLENTE	45
//Carne Fresca	
#define CIBO_CARNE_PULITA_BUONA		46
#define CIBO_CARNE_PULITA_OTTIMA		47
#define CIBO_CARNE_PULITA_ECCELLENTE	48

#define NUM_TIPO_FRUTTA 		7





// cacciatore
#define START_SELV_VNUM  		80
#define END_SELV_VNUM    		95
#define MAX_LIV_MOB_SELV 		4
// Architetto
#define START_VNUM_COSTR  		40001
#define END_VNUM_COSTR    		44999
#define MATERIALE_X_COSTRUIRE 	300
#define LIV_CASA        		30
#define LIV_ABITAZIONI  		50
#define LIV_CASTELLI    		80
//Addestratore
#define START_MOB_AMMAESTR_VNUM 	96
#define END_MOB_AMMAESTR_VNUM 	99


struct materiale {
	char *nome;
	char *alias;
	int max_ac;
	int bonus_livello;
	int max_livspell;
	int max_slot;
	int peso;
	int val_comm;
	int val_nutr;
	int n_grezzi;
	int riuscita;
};

struct risorse_reset {
	int qmin;
	int qmax;
	int usimin;
	int usimax;
	int p_field;
	int p_forest;
	int p_hill;
	int p_mount;
	int p_swim;
	int p_noswim;
};

void check_danno_oggetto (struct char_data * ch, struct obj_data * obj, float modif);
int num_necessari (int obiettivo, int riuscita);
int num_fallimento (int num_richiesto);
void elimina_oggetti_uguali(struct char_data * ch, struct obj_data * primo, int numero);
void manufatti_to_char(struct obj_data * object, struct char_data * ch, int numero);
const char * nome_materiale (int tipo, int num);
int limite_materiali(int tipo);

extern const  char * tipi_materiale[];
extern struct materiale pietre[];
extern struct materiale alberi[];
extern struct materiale gemme[];
extern struct materiale rocce[];
extern struct materiale pelli[];
extern struct materiale natura[];
extern struct materiale erbe[];
