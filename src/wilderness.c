#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "wilderness.h"
#include "db.h"
#include "interpreter.h"
#include "handler.h"
#include "spells.h"
#include "house.h"
#include "dg_scripts.h"
#include "olc.h"
#include "screen.h"
#include "telnet.h" //Orione


typedef int BOOL;

extern int use_wilderness;
extern struct zone_data *zone_table;
extern struct room_data *world;
extern int top_of_world;
extern int top_of_zone_table;
extern int movement_loss[];



/* GLOBAL VARIABLES */
struct wild_data * wild_table;
int top_of_wild_table=0;

/* LOCAL FUNCTIONS */
void wild_table_load();
void parse_wild_table(FILE * fwt, int virtual_nr);
void wild_index_map_load();
void wild_index_special_load();
void wild_map_load(FILE * fm, int zone_rnum);
void wild_special_load(FILE * fl, int zone_rnum);
void wild_parse_room(FILE * fl, int virtual_nr);
int wilderness_los(struct char_data *ch, int x, int y, int cx, int cy);
//int wild_time_modify(struct char_data *ch, int radius);
int any_mob(struct char_data *ch, room_rnum room);
char * color_from_color (struct char_data * ch, int color);
void wilderness_draw_type( struct char_data *ch,BOOL graphWildON );
char *colore_esercito(struct char_data * ch, room_rnum r );

/* EXTERN FUNCTIONS */
int has_boat(struct char_data *ch);
void death_cry(struct char_data *ch);
int can_fly(struct char_data *ch);
void parse_room(FILE * fl, int virtual_nr);
long long int asciiflag_conv(char *flag);
void setup_dir(FILE * fl, int room, int dir);
char fread_letter(FILE *fp);
int test_and_improve (struct char_data * ch, int skill_num, int test, int prob, int modifier);

extern char *dirs1[];

//typedef int BOOL;

/***************************************************************************************/

void wilderness_boot()
{
  if (!use_wilderness) return;

  wild_table_load();
  wild_index_map_load();
  wild_index_special_load();
  
  /* ARDANOS - calcolo dimensione wild */
  /* Piccola procedura per il calcolo dell'occupazione in memoria della wild */
  /* decommentare in caso di necessita'

	printf("Top_world : %d, sizeof room %d\r\n", top_of_world,sizeof(struct room_data));
	printf("top_of_wild: %d\r\n", top_of_wild_table);
	int i;
	int stringhe = 0;
	
	for(i = 0; i != top_of_wild_table - 2; i++) {
		if(wild_table[i].name)
			stringhe += strlen(wild_table[i].name);
		if(wild_table[i].description)
			stringhe += strlen(wild_table[i].description);
	}
	printf("Stringe : %d\r\n", stringhe);
	printf("Wild info: %d\r\n", top_of_wild_table*sizeof(struct wild_data));
		
	printf("Totale: %d\r\n", stringhe + 
	(sizeof(struct room_data)*(200*200)+ // Numero stanze wild-cicciona * sizeof
	(17*25*34)*sizeof(struct room_data)) + //Numero stanze miniwild * numero miniwild * sizeof
	top_of_wild_table*sizeof(struct wild_data));
	
	fine calcolo dimensioni wild */
	
}

void wild_table_load()
{
  FILE * fwt;
  int nr=-1, last=0;
  int rec_count;
  char line[256];

  sprintf(buf2, "%s/%s", WILD_PREFIX, WILD_TABLE_FILE);
  if (!(fwt = fopen(buf2, "r")))
    {
    sprintf(buf1, "Error opening wilderness table file '%s'", buf2);
    perror(buf1);
    exit(1);
    }
  rec_count = count_hash_records(fwt);
  if (!rec_count)
    {
    log("SYSERR: boot error - 0 records counted in wilderness table file");
    exit(1);
    }

  //  rec_count++; C'e' in db.c ma a cosa serve ???
  CREATE(wild_table, struct wild_data, rec_count);
  rewind(fwt);

  for (;;)
    {
    if (!get_line(fwt, line))
      {
      fprintf(stderr, "Format error after wilderness type #%d\n", nr);
      exit(1);
      }
    if (*line == '$')
      break;
    if (*line == '#')
      {
      last = nr;
      if (sscanf(line, "#%d", &nr) != 1)
        {
	fprintf(stderr, "Format error after wilderness type #%d\n", last);
	exit(1);
        }
      else parse_wild_table(fwt, nr);
      }
    else
      {
      fprintf(stderr, "Format error in wilderness table file near type #%d\n", nr);
      fprintf(stderr, "Offending line: '%s'\n", line);
      exit(1);
      }
    } //FINE CICLO FOR(;;)

  fclose(fwt);
}

void parse_wild_table(FILE * fwt, int virtual_nr)
{
  static int wt_nr = 0;
  int t[3];
  char line[256];
  char symbol;

  if (virtual_nr<=0)
    {
    fprintf(stderr, "Wilderness type #%d not legal.\n", virtual_nr);
    exit(1);
    }
  if (real_wild_table(virtual_nr)!=-1)
    {
    fprintf(stderr, "Wilderness type #%d already defined.\n", virtual_nr);
    exit(1);
    }
  wild_table[wt_nr].index=virtual_nr;
  wild_table[wt_nr].name = fread_string(fwt, buf2);
  wild_table[wt_nr].description = fread_string(fwt, buf2);

  if (!get_line(fwt, line) || sscanf(line, " %c %d %d ", &symbol, t, t+1) != 3)
    {
    fprintf(stderr, "Format error in wilderness type #%d.\n", virtual_nr);
    exit(1);
    }

  wild_table[wt_nr].symbol = symbol;
  wild_table[wt_nr].color  = t[0];
  wild_table[wt_nr].owner  = t[1];

  if (!get_line(fwt, line) || sscanf(line, " %d %d %d ", t, t+1, t+2) != 3)
    {
    fprintf(stderr, "Format error in wilderness type #%d.\n", virtual_nr);
    exit(1);
    }

  wild_table[wt_nr].move_cost   = t[0];
  wild_table[wt_nr].altitudine  = t[1];
  wild_table[wt_nr].can_enter   = t[2];

  if (!get_line(fwt, line) || sscanf(line, " %d %d ", t, t+1) != 2)
    {
    fprintf(stderr, "Format error in wilderness type #%d.\n", virtual_nr);
    exit(1);
    }

  wild_table[wt_nr].sector_type = t[0];
  wild_table[wt_nr].room_flags  = t[1];

  top_of_wild_table=wt_nr++;
}

void wild_index_map_load()
{
  FILE *index, *db_file;
  int rec_count = 0;
  int numero;

  sprintf(buf2, "%s/%s", WILD_PREFIX, WILD_INDEX_MAP);
  if (!(index = fopen(buf2, "r"))) {
    sprintf(buf1, "Error opening index file '%s'", buf2);
    perror(buf1);
    exit(1);
  }

  /* first, count the number of records in the file so we can malloc */
  fscanf(index, "%s\n", buf1);
  while (*buf1 != '$')
    {
    sprintf(buf2, "%s/%s", WILD_PREFIX, buf1);
    if (!(db_file = fopen(buf2, "r")))
      {
      perror(buf2);
      log("file listed in index not found");
      exit(1);
      }
    else rec_count++;

    fclose(db_file);
    fscanf(index, "%s\n", buf1);
    }

  /* Exit if 0 records */
  if (!rec_count)
    {
    log("SYSERR: boot error - 0 records counted");
    exit(1);
    }

  rewind(index);
  fscanf(index, "%s\n", buf1);
  while (*buf1 != '$')
    {
    sprintf(buf2, "%s/%s", WILD_PREFIX, buf1);
    if (!(db_file = fopen(buf2, "r")))
      {
      perror(buf2);
      exit(1);
      }

    // I nomi delle mappe devono essere tipo 10000.map
    // Per le miniwild invece 2xx00.map

    numero = atoi(buf1);
    if ((numero=real_zone(numero*100))==-1)
      {
      log("Zona non esistente nell'indice delle mappe wilderness e miniwild");
      exit(1);
      }
    wild_map_load(db_file, numero);
    fclose(db_file);
    fscanf(index, "%s\n", buf1);
    }
}

void wild_map_load(FILE * fm, int zone_rnum)
{
  char line[2048];
  char * lread;
  int x,y;
  int r,nr;
  int i;
  int mode = (zone_table[zone_rnum].wilderness == ZONE_WILDERNESS); // 1 se wilderness, 0 se miniwild

  for (y=0; y < (mode ? WILD_YSIZE : MINIWILD_YSIZE); y++)
    {
    if (!get_line(fm, line))
      {
      fprintf(stderr, "Can't read line %d of wilderness map file (zone %d)\n", y+1, zone_table[zone_rnum].number);
      exit(1);
      }
    }
  rewind(fm);

  if (mode) RECREATE(world, struct room_data, top_of_world + 2 + WILD_XSIZE * WILD_YSIZE);
  else RECREATE(world, struct room_data, top_of_world + 2 + MINIWILD_XSIZE * MINIWILD_YSIZE);

  for (y=0; y < (mode ? WILD_YSIZE : MINIWILD_YSIZE); y++)
    {
    if (!get_line(fm, line))
      {
      fprintf(stderr, "Can't read line %d of wilderness map file\n", y+1);
      exit(1);
      }
    lread=line;
    for (x=0; x < (mode ? WILD_XSIZE : MINIWILD_XSIZE); x++)
      {
      if (!lread || sscanf(lread, " %d ", &nr) != 1)
        {
        fprintf(stderr, "Error on line %d, position %d of wilderness map file (rnum %d) - cannot read integer\n", y+1, x+1, zone_rnum);
        exit(1);
        }
      if ((r=real_wild_table(nr))==-1)
        {
        fprintf(stderr, "Error on line %d, position %d of wilderness map file (rnum %d) - wild type %d unknown\n", y+1, x+1, zone_rnum, nr);
        exit(1);
        }
      lread=strchr(lread, ' ');
      if (lread) lread++;
      /* Wilderness room put on end of world[] */

      top_of_world++;

      world[top_of_world].zone        = zone_rnum;
      if (mode) world[top_of_world].number      = WILD_VNUM(WILD_RXOR+x, WILD_RYOR+y);
      else      world[top_of_world].number      = MINIWILD_VNUM(zone_rnum, MINIWILD_RXOR+x, MINIWILD_RYOR+y);
      world[top_of_world].name        = wild_table[r].name;
      world[top_of_world].description = wild_table[r].description;

      world[top_of_world].room_flags  = wild_table[r].room_flags;
      world[top_of_world].func        = NULL;
      world[top_of_world].contents    = NULL;
      world[top_of_world].people      = NULL;
      world[top_of_world].light       = 0;	/* Zero light sources */
      for (i = 0; i < NUM_OF_DIRS; i++)
        world[top_of_world].dir_option[i] = NULL;
      world[top_of_world].ex_description = NULL;

      world[top_of_world].wild_rnum   = r;
      world[top_of_world].wild_modif  = FALSE;
      world[top_of_world].sector_type = wild_table[r].sector_type;
	  world[top_of_world].script = NULL;

      /* Come fare per i trigger ???? */

      }
    }
}

void wild_index_special_load()
{
  FILE *index, *db_file;
  int rec_count = 0;
  int numero;

  sprintf(buf2, "%s/%s", WILD_PREFIX, WILD_INDEX_WLD);
  if (!(index = fopen(buf2, "r"))) {
    sprintf(buf1, "Error opening index file '%s'", buf2);
    perror(buf1);
    exit(1);
  }

  /* first, count the number of records in the file so we can malloc */
  fscanf(index, "%s\n", buf1);
  while (*buf1 != '$')
    {
    sprintf(buf2, "%s/%s", WILD_PREFIX, buf1);
    if (!(db_file = fopen(buf2, "r")))
      {
      perror(buf2);
      log("file listed in index not found");
      exit(1);
      }
    else rec_count++;

    fclose(db_file);
    fscanf(index, "%s\n", buf1);
    }

  /* Exit if 0 records */
  if (!rec_count)
    {
    log("SYSERR: boot error - 0 records counted");
    exit(1);
    }

  rewind(index);
  fscanf(index, "%s\n", buf1);
  while (*buf1 != '$')
    {
    sprintf(buf2, "%s/%s", WILD_PREFIX, buf1);
    if (!(db_file = fopen(buf2, "r")))
      {
      perror(buf2);
      exit(1);
      }

    // I nomi delle aree stanze speciali devono essere tipo 10000.wld
    // Per le miniwild invece 2xx00.wld

    numero = atoi(buf1);
    if ((numero=real_zone(numero*100))==-1)
      {
      log("Zona non esistente nell'indice delle stanza speciali (.wld) wilderness e miniwild");
      exit(1);
      }
    wild_special_load(db_file, numero);
    fclose(db_file);
    fscanf(index, "%s\n", buf1);
    }
}

void wild_special_load (FILE * fl, int zone_rnum)
{
  int nr=-1, last=0;
  char line[256];

  for (;;)
    {
    if (!get_line(fl, line))
        {
	fprintf(stderr, "Format error after wilderness special room #%d\n", nr);
	exit(1);
        }
    if (*line == '$') break;
    if (*line == '#')
      {
      last = nr;
      if (sscanf(line, "#%d", &nr) != 1)
        {
        fprintf(stderr, "Format error after wilderness special room #%d\n", last);
        exit(1);
        }
      wild_parse_room(fl, nr);
      }
    else
      {
      fprintf(stderr, "Format error in wilderness special room file near room #%d\n", nr);
      fprintf(stderr, "Offending line: '%s'\n", line);
      exit(1);
      }
    }
}


/* load the rooms */
void wild_parse_room(FILE * fl, int virtual_nr)
{
  int t[10];
  char line[256], flags[128];
  struct extra_descr_data *new_descr;
  char letter;
  room_rnum room_nr;

  sprintf(buf2, "room #%d", virtual_nr);
  if ((room_nr=real_room(virtual_nr))==-1)
    {
    fprintf(stderr, "Room #%d in wilderness special room file doesn't exists", virtual_nr);
    exit(1);
    }
  if (!IS_WILD(room_nr))
    {
    fprintf(stderr, "Room #%d in wilderness special room file isn't a wilderness/miniwild room", virtual_nr);
    exit(1);
    }

  world[room_nr].wild_modif = TRUE;
  world[room_nr].name = fread_string(fl, buf2);
  world[room_nr].description = fread_string(fl, buf2);

  if (!get_line(fl, line) || sscanf(line, " %d %s %d ", t, flags, t + 2) != 3) {
    fprintf(stderr, "Format error in room #%d\n", virtual_nr);
    exit(1);
  }
  /* t[0] is the zone number; ignored with the zone-file system */
  world[room_nr].room_flags = asciiflag_conv(flags);
  world[room_nr].sector_type = t[2];

  sprintf(buf, "Format error in room #%d (expecting D/E/S)", virtual_nr);

  for (;;) {
    if (!get_line(fl, line)) {
      fprintf(stderr, "%s\n", buf);
      exit(1);
    }
    switch (*line) {
    case 'D':
      setup_dir(fl, room_nr, atoi(line + 1));
      break;
    case 'E':
      CREATE(new_descr, struct extra_descr_data, 1);
      new_descr->keyword = fread_string(fl, buf2);
      new_descr->description = fread_string(fl, buf2);
      new_descr->next = world[room_nr].ex_description;
      world[room_nr].ex_description = new_descr;
      break;
    case 'S':			/* end of room */
      /* DG triggers -- script is defined after the end of the room */

      /* Sistemare i trigger */

      letter = fread_letter(fl);
      ungetc(letter, fl);
      while (letter=='T') {
        dg_read_trigger(fl, &world[room_nr], WLD_TRIGGER);
        letter = fread_letter(fl);
        ungetc(letter, fl);
      }
      return;
      break;
    default:
      fprintf(stderr, "%s\n", buf);
      exit(1);
      break;
    }
  }
}

void wild_map_save_to_disk(int zone) // E' un zone_rnum
{
  FILE * fm;
  char line[2048];
  int x,y;
  char buf3[256];
  int miniwild = (zone_table[zone].wilderness == ZONE_MINIWILD);
  room_rnum r;

  if (zone_table[zone].wilderness == ZONE_NORMAL_AREA) return;

  sprintf(buf1, "%s/%d.map", WILD_PREFIX, zone_table[zone].number);
  sprintf(buf2, "%s.new", buf1);
  sprintf(buf3, "%s.old", buf1);
  if (!(fm = fopen(buf2, "w+")))
    {
    sprintf(buf1, "Error opening wilderness/miniwild map file '%s'", buf2);
    perror(buf1);
    exit(1);
    }

  for (y=0; y < MWILD_YSIZE; y++)
    {
    strcpy(line, "\0");
    for (x=0; x < MWILD_XSIZE; x++)
      {
      if (miniwild) r = real_room(MINIWILD_VNUM(zone, x+MINIWILD_RXOR, y+MINIWILD_RYOR));
      else r = real_room(WILD_VNUM(x+WILD_RXOR,y+WILD_RYOR));
      sprintf(line+strlen(line), "%d ", GET_WILD_TABLE(r).index );
      }
    strcat(line, "\0");
    fprintf(fm, "%s\n", line);
    }

  fclose(fm);
  remove(buf3);
  rename(buf1, buf3);
  rename(buf2, buf1);
}

void wild_ascii_map_save_to_disk(int zone) // E' un zone_rnum
{
  FILE * fm;
  char line[2048];
  int x,y;
  char buf3[256];
  int miniwild = (zone_table[zone].wilderness == ZONE_MINIWILD);
  room_rnum r;

  if (zone_table[zone].wilderness == ZONE_NORMAL_AREA) return;

  sprintf(buf1, "%s/%d.asc", WILD_PREFIX, zone_table[zone].number);
  sprintf(buf2, "%s.new", buf1);
  sprintf(buf3, "%s.old", buf1);
  if (!(fm = fopen(buf2, "w+")))
	 {
    sprintf(buf1, "Error opening wilderness/miniwild map file '%s'", buf2);
    perror(buf1);
    exit(1);
    }

  for (y=0; y < MWILD_YSIZE; y++)
    {
    strcpy(line, "\0");
    for (x=0; x < MWILD_XSIZE; x++)
      {
      if (miniwild) r = real_room(MINIWILD_VNUM(zone, x+MINIWILD_RXOR, y+MINIWILD_RYOR));
      else r = real_room(WILD_VNUM(x+WILD_RXOR,y+WILD_RYOR));
      sprintf(line+strlen(line), "%c", GET_WILD_TABLE(r).symbol );
      }
    strcat(line, "\0");
    fprintf(fm, "%s\n", line);
    }

  fclose(fm);
  remove(buf3);
  rename(buf1, buf3);
  rename(buf2, buf1);
}


/************************************************************************************/
#define WILD_TYPE_ROAD     0x000F
#define WILD_TYPE_WALL     0x001F
#define WILD_TYPE_RIVER    0x002F

#define WILD_TYPE_DESE01  0x003F // DESERTO .
#define WILD_TYPE_PIAN01  0x0051 // PIANURA p
#define WILD_TYPE_PIAN02  0x0052 // PIANURA P
#define WILD_TYPE_FORE01  0x0053 // FORESTA f
#define WILD_TYPE_FORE02  0x0054 // FORESTA F
#define WILD_TYPE_COLL01  0x0055 // COLLINA c
#define WILD_TYPE_COLL02  0x0056 // COLLINA C
#define WILD_TYPE_MONT01  0x0057 // MONTAGNA m
#define WILD_TYPE_MONT02  0x0058 // MONTAGNA M
#define WILD_TYPE_ALTO01  0x0059 // ALTOPIANO a
#define WILD_TYPE_PIAN03  0x005A // PIANORO I
#define WILD_TYPE_VALL01  0x005B // VALLE v
#define WILD_TYPE_VALL02  0x005C // VALLE V
#define WILD_TYPE_LAGO01  0x005D // LAGO ~
#define WILD_TYPE_SPIA01  0x005E // SPIAGGIA s
#define WILD_TYPE_AREN01  0x0060 // SPALTI ARENA o
#define WILD_TYPE_PONT01  0x0061 // PONTE
#define WILD_TYPE_PONT02  0x0062 // PONTE
#define WILD_TYPE_LOCA01  0x0063 // LOCAZIONE
#define WILD_TYPE_MACR01  0x0064 // MACRORISORSA

#define WILD_TYPE_MISSING   0xFFFE
#define WILD_TYPE_NULL     0xFFFF

// Struct to make conversion from server WORD to bitmap BYTE.
typedef struct sCellConverter
{
 WORD   cellSrvrCode;
 WORD   cellBackBitmap;
 WORD   cellForeBitmap;
} cellConverter;

// Function used to make cell conversion in order to get cell bitmap.
cellConverter * MakeConversion( WORD cellCode )
{
	int idx;
 static cellConverter cellConv[] = {
 {     1,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - X - Vetta di Montagna
 {     2,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 5 - ~ - Mare di Daresia
 {     3,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - | - Muro Nord-Sud
 {     4,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - - - Muro Est-Ovest
 {     5,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 6 - = - Ponte
 {     6,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - @ - Area
 {     7,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - O - Torre d'angolo
 {     8,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 8 - ^ - Vette innevate
 {     9,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - G - Abitazioni
 {    10,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Largo sentiero battuto
 {    11,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - @ - Area transitabile
 {    12,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada tra Deira e Lamorka
 {    13,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada tra Deira e Cimmura
 {    14,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada tra Cimmura e Chyrellos
 {    15,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada da Lamorka a Chyrellos
 {    16,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada tra Cimmura e Jiroch
 {    17,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso Roma
 {    18,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - a - Mini area
 {    19,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 6 - O - Castello
 {    20,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - X - Vette montuose
 {    21,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa alimentare
 {    26,  WILD_TYPE_PIAN02    , WILD_TYPE_NULL      },  //  - 2 - X - Terreno roccioso impraticabile
 {    27,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - x - Scarpata
 {    28,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - x - Montagne rocciose
 {    29,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - x - Colline rocciose
 {    37,  WILD_TYPE_AREN01    , WILD_TYPE_NULL      },  //  - 6 - o - Spalti dell'arena,  area Regale
 {    38,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - . - Nell'arena
 {    39,  WILD_TYPE_AREN01    , WILD_TYPE_NULL      },  //  - 3 - o - Spalti dell'arena
 {    40,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - x - Colline non passabili
 {    41,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - a - Borgo esterno di Deira
 {    42,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa mineraria.
 {    43,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa di erbe.
 {    44,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa di pietre preziose.
 {    45,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa metallurgica.
 {    46,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - S - Un santuario
 {    47,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa boschiva
 {    48,  WILD_TYPE_MACR01    , WILD_TYPE_NULL      },  //  - 6 - M - Una macrorisorsa di selvaggina
 {    90,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - S - Santuario
 {   100,  WILD_TYPE_PIAN02    , WILD_TYPE_RIVER     },  //  - 3 - u - Palude-aquitrino di Magras (Sendaria)
 {   101,  WILD_TYPE_ALTO01    , WILD_TYPE_NULL      },  //  - 8 - a - Altopiano di Cryion (Sendaria)
 {   102,  WILD_TYPE_PIAN03    , WILD_TYPE_NULL      },  //  - 8 - I - Il Pianoro di Deira (Aldaria)
 {   103,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - La Valle dell'Aldur (aldaria)
 {   104,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 3 - v - La Valle dell'Argath (aldaria)
 {   105,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianoro dell'Anduin (Sendaria)
 {   106,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - La Vallata di Lamorka (sendaria)
 {   107,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - s - Spiaggia con sabbia fine
 {   108,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 3 - v - La valle di Dacrud (Daconia)
 {   109,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Brughiere tra Daconia e Sendaria
 {   110,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 3 - ; - Il Delta del Dakrud (Daconia)
 {   111,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 8 - x - Rocce a dirupo sul mare
 {   112,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianoro di Irdan(Nyissa)
 {   113,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 3 - v - Valle del Nedrane (Tolnedra)
 {   114,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 3 - v - Valle del fiume Kerofonte (Tolnedra)
 {   115,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Trinidad (Tolnedra)
 {   116,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianoro nella Tolnedra Meridionale
 {   117,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Terre brulle nella Tolnedra Settentrionale
 {   118,  WILD_TYPE_PIAN02    , WILD_TYPE_NULL      },  //  - 3 - u - Palude della morte (Nyissa)
 {   119,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - s - Ampie savane ad erba secca e giallastra in Arendia
 {   120,  WILD_TYPE_ALTO01    , WILD_TYPE_NULL      },  //  - 8 - a - Altopiano di Turak
 {   121,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Spoglio,  secco Pianoro di Dansir (Drasnia)
 {   122,  WILD_TYPE_DESE01    , WILD_TYPE_NULL      },  //  - 4 - . - Il Grande Deserto di Polvere
 {   123,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - s - Savane Cysneghiane
 {   124,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - g - La gola dello Cthol
 {   125,  WILD_TYPE_ALTO01    , WILD_TYPE_NULL      },  //  - 8 - a - Altopiano di Krift
 {   126,  WILD_TYPE_ALTO01    , WILD_TYPE_NULL      },  //  - 8 - a - Altopiano di Astrart
 {   200,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Ripide Colline Aldariane e Sendariane (Aldaria-Sendaria)
 {   201,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Promontorio dell'Asturia (Aldaria)
 {   202,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline dell'Asturia (Aldaria)
 {   203,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Collina di Agrav (Aldaria)
 {   204,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Promontorio dell'Agrav (Aldaria)
 {   205,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Ascesa verso le Colline dell'Argentario (sendaria)
 {   206,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline dell'Argentario(Sendaria)
 {   207,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Sistema collinoso di Dridan (Sendaria)
 {   208,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline della Daconia
 {   209,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Promontorio di Zedar (Nyissa)
 {   210,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Ripida zona discendente dai Monti Khalist
 {   211,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Zona discendente dai Monti Khalist verso la Tolnedra Settentrionale
 {   212,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 4 - p - Una Radura (Tolnedra)
 {   213,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Marak (Arendia)
 {   214,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline discendenti dai monti Camaar
 {   215,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline discendenti dai monti Agraba
 {   216,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 2 - C - Promontorio di Caergoth
 {   217,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Terreno friabile discendente nel Canyon
 {   218,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Grandi colline di Obros
 {   219,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Alte colline di Nardan
 {   220,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Promontorio di Cimmos
 {   221,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 3 - v - Vallata scoscesa di Erdanos
 {   222,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Sdurnan (Sendaria)
 {   223,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Dornendil
 {   224,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline di Dornendil,  parte alta
 {   225,  WILD_TYPE_COLL01    , WILD_TYPE_ROAD      },  //  - 4 - : - Stradina collinosa nelle colline di Dornendil
 {   300,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Massiccio montuoso di Agraba,  parte inferiore.
 {   301,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Massiccio montuoso di Agraba,  parte superiore.
 {   302,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Catena Monti Khalist,  parte inferiore
 {   303,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Catena Monti Khalist,  parte superiore
 {   304,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Monti Hordil,  parte inferiore
 {   305,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Monti Hordil,  parte superiore
 {   306,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Monti Mordor,  parte inferiore
 {   307,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Monti Mordor,  parte superiore
 {   308,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Catena montuosa di Camaar,  parte inferiore
 {   309,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Catena montuosa di Camaar,  parte superiore
 {   400,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Hoan-Dor (Aldaria)
 {   401,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta di Lanowar (Aldaria)
 {   402,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta di Magras (Sendaria)
 {   403,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Gola di accesso alla Valle dell'Anduin(Sendaria)
 {   404,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta di Sandir (Sendaria)
 {   405,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta di Oros (Sendaria)
 {   406,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Scribb (Aldaria)
 {   407,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Salici piangenti vicino al lago (Daconia)
 {   408,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Vordue (Tolnedra)
 {   409,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - T - Rigogliosissima foresta tropicale di Atun (giungla)
 {   410,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - T - Giungla di Cysna
 {   411,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Wayreth
 {   412,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta di Kyrm
 {   500,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Aldur (Aldaria,  Sendaria)
 {   501,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Argath (Aldaria)
 {   502,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Anduin (Sendaria)
 {   503,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - lago sotterraneo Argento (Sendaria)
 {   504,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Dacrud (Daconia)
 {   505,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Lago Crysamilis (Daconia)
 {   506,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Nedrane (Tolnedra)
 {   507,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Kerofonte (Tolnedra)
 {   508,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Torbor (Tolnedra)
 {   509,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Cthol
 {   510,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il lago Rheon,  privo di emissario
 {   511,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Lago Mistral
 {   512,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Mishra
 {   513,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Arend
 {   514,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Fiume Seldon
 {   515,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Lago di Haon-Dor
 {   516,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il Fiume Dard
 {   600,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di HighCastle
 {   601,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Collinette di Gildel
 {   602,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Gildanren
 {   603,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco di Doen
 {   604,  WILD_TYPE_FORE01    , WILD_TYPE_ROAD      },  //  - 6 - : - &7Strada verso il Monastero di Fizban&0
 {   605,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di Bas-Tyra
 {   606,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Brandel
 {   607,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Tyran
 {   608,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco di Basir
 {   609,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello di Bas-Tyra
 {   610,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Montagne Tornan
 {   611,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Vallata del Tyran
 {   612,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di Carthmoor
 {   613,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco di Carnan
 {   614,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Krynn
 {   615,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Gramdel
 {   616,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello di Carthmoor
 {   617,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline Khalow
 {   618,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Vallata del fiume Krynn
 {   619,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Karnynn
 {   620,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di Agriland
 {   621,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco di Andor
 {   622,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Levian
 {   623,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Balin
 {   624,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello dell'Armonia
 {   625,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Leviatan
 {   626,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di Arger
 {   627,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco di Elvandar
 {   628,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Argon
 {   629,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Brindad
 {   630,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello dei Divini
 {   631,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Xenen
 {   632,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Valle dell'Argon
 {   633,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Bosco di Elvandar,  parte fitta
 {   634,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Appendice dei monti Hordil
 {   635,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di Darkland
 {   636,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Boscaglie di Dendern
 {   637,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - N - Colline brulle
 {   638,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Tortuose Colline di Bernal
 {   639,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello di Sethanon
 {   640,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 4 - N - Un nuovo tipo di regione non finito.
 {   641,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Scoscesa Valle di Brundal
 {   642,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura di Sarland
 {   643,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco di Sarfel
 {   644,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Alte colline di Sarth
 {   645,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello di Sarth
 {   646,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Scoscesa Valle del fiume Sirion
 {   647,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Sirion
 {   648,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Soren
 {   649,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Piano di Corwin
 {   650,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Boschi di Gundam
 {   651,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Tith
 {   652,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello di Armengar
 {   653,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Verde vallata del Gwynn
 {   654,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Gwynn
 {   655,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Gwyain
 {   656,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Waren
 {   657,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Boschi di Turen
 {   658,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Isnar
 {   659,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso il Castello di Gilead
 {   660,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Warlake
 {   661,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline di Isnar,  parte alta
 {   662,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Gundar
 {   663,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Boschi di Feyer
 {   664,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Sturnil
 {   665,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline di Sturnil,  parte alta
 {   666,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Il Lago Graundal
 {   667,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Pianeggiante valle del Bundar
 {   668,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura Briceland
 {   669,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Acquitrino
 {   670,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - . - Passerella instabile
 {   671,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - § - Portale mistico
 {   672,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Dard
 {   673,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Boschi di Deira
 {   674,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Dornendil
 {   675,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 8 - v - Valle del fiume Dard
 {   676,  WILD_TYPE_PIAN03    , WILD_TYPE_NULL      },  //  - 8 - I - Pianoro di Deira
 {   677,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso Deira,  la Capitale del Regno d'Aldaria
 {   678,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Il fiume Cimmor
 {   679,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Boschi di Crydeel
 {   680,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Cimmor
 {   681,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada verso Cimmura,  Capitale del Regno di Tolnedra
 {   682,  WILD_TYPE_PIAN03    , WILD_TYPE_NULL      },  //  - 8 - i - Pianoro di Cimmura
 {   683,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - La strada nel Borgo
 {   684,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Una strada ben tenuta
 {   685,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - s - Sobborgo di Cimmura
 {   698,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - X - Sito di costruzione generico
 {   699,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - A - Capitale
 {   700,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Lamorka
 {   701,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Pineta
 {   702,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - s - Dolce sorgente
 {   703,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 5 - ~ - Golfo di Lamorka
 {   704,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Parete rocciosa
 {   705,  WILD_TYPE_VALL01    , WILD_TYPE_NULL      },  //  - 2 - x - Dirupo
 {   706,  WILD_TYPE_ALTO01    , WILD_TYPE_NULL      },  //  - 8 - a - Altopiano di Lamorka
 {   707,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 2 - : - Strada della Capitale
 {   708,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 2 - : - Via del Porto
 {   709,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 2 - : - Via del Castello
 {   710,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di Chyrellos
 {   711,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - a - Pianura di Chyrellos
 {   712,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 5 - ~ - Golfo di Chyrellos
 {   713,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 2 - : - Strada verso Chyrellos,  la Capitale del Regno di Elenia
 {   714,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Colline di Chyrellos
 {  1006,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Colline morte
 {  1016,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Bassa collina
 {  1023,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Gran Massiccio dell'Er
 {  1026,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Picchi dei monti serpente
 {  1033,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Monti speranza
 {  1036,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Monte serpente
 {  1042,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - n - Pianura del freddo
 {  1044,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Terreno fangoso
 {  1058,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 7 - ~ - Palude della disperazione
 {  1067,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 6 - @ - Torre abbandonata
 {  1076,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - a - Un pozzo
 {  1077,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - a - Una casetta
 {  1093,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 4 - = - Ponte sabbioso
 {  1096,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 4 - = - Ponte in legno
 {  1097,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 3 - = - Ponte viscido
 {  1103,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada erbosa
 {  1107,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada ciottolata
 {  1116,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - . - Sabbie mobili
 {  1122,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 8 - x - Picchi dell'Er
 {  1134,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta della disperazione
 {  1136,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta di pietra
 {  1144,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta di Cedri
 {  1152,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 8 - X - L'isola inaccessibile
 {  1346,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 3 - C - Collina Dolce
 {  1356,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Bassa collina
 {  1363,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Gran Massiccio dell'Er
 {  1373,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Monte
 {  1398,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 3 - ~ - Lago Melmoso
 {  1403,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - @ - una cittadella arroccata sui monti
 {  1407,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - @ - La Fornace
 {  1416,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - a - La villa del custone
 {  1427,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 7 - S - una fonte luminosa di luce
 {  1433,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 1 - = - Il ponte del pentimento
 {  1443,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada ciottolata
 {  1446,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Sentiero
 {  1447,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada battuta
 {  1456,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 4 - . - Il sentiero dell'oracolo
 {  1462,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 8 - x - Picchi dell'Er
 {  1474,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - La forseta botanica
 {  1476,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 4 - F - Foresta Morta
 {  1478,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta dei giganti
 {  1484,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Foresta bassa
 {  1488,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Pioppi
 {  1489,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 8 - = - un ponte in pietra
 {  1522,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - o - Una grotta
 {  1523,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 2 - o - La grotta
 {  1607,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - G - una casa
 {  1636,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 4 - T - Tundra
 {  1646,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 4 - t - Tundra
 {  1722,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Terre brulle nella Tolnedra Settentrionale
 {  1738,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 5 - ~ - Mare
 {  1786,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 4 - : - Strada lastricata
 {  1787,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada ciottolata
 {  1795,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 5 - . - Tra i bastioni
 {  1796,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - . - Sabbia finissima
 {  1802,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 8 - X - Scoglio
 {  1838,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 7 - @ - La citta
 {  1846,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - s - Spiaggia con sabbia fine
 {  1853,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - O - Torre portone
 {  1854,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 3 - O - Torretta
 {  1856,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - O - Torre di guardia
 {  1863,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - o - Torre Piccola
 {  1873,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - - - Bastione
 {  1883,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - | - Bastione
 {  1886,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - | - Bastioni Principali
 {  1913,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - + - Incrocio di mura
 {  1964,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - V - Vegetazione
 {  2036,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Collinetta
 {  2058,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 3 - ~ - Fango acido
 {  2074,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 3 - ~ - Fossato
 {  2087,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - S - Santuario
 {  2093,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 2 - = - Ponte delle sfide
 {  2106,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - S - il &3&bSantuario&0 del cimitero di Cimmura
 {  2107,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 8 - : - Sentiero dei Santuari
 {  2112,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 1 - . - Campo di lotta
 {  2113,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 2 - = - Ponte levatoio
 {  2122,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 8 - : - Viale dei funzionari
 {  2127,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 8 - : - Viale esterno
 {  2164,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Querce
 {  2173,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - O - Torre
 {  2174,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 3 - O - Torre
 {  2176,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - O - Torre
 {  2183,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - o - Torre
 {  2193,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - - - Muro
 {  2198,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 7 - O - Castello del Re
 {  2203,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - | - Muro
 {  2208,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 7 - o - Torre del castello
 {  2218,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 7 - - - Mura del castello
 {  2228,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 7 - | - Mura del castello
 {  2253,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - + - Recinto del cimitero
 {  2273,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - o - Tetto
 {  2284,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 3 - G - Le gilde e l'arena
 {  2404,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 3 - p - Prato verde e soffice
 {  5006,  WILD_TYPE_COLL02    , WILD_TYPE_NULL      },  //  - 4 - C - Collina
 {  5016,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Collinetta
 {  5023,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Montagna alta
 {  5033,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Montagna
 {  5042,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 8 - p - Pianura
 {  5058,  WILD_TYPE_PIAN01    , WILD_TYPE_RIVER     },  //  - 7 - ~ - Fiume
 {  5065,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 5 - @ - Locazione
 {  5067,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - @ - Locazione
 {  5068,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 7 - @ - Locazione
 {  5097,  WILD_TYPE_RIVER     , WILD_TYPE_PONT02    },  //  - 6 - = - Ponte
 {  5107,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada
 {  5116,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - . - Sabbia
 {  5134,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta fitta
 {  5144,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco
 {  5166,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - s - Sabbie mobili
 {  5173,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - O - Torre grande.
 {  5183,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - o - Torre piccola
 {  5193,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - - - Muro
 {  5203,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - | - Muro.
 {  5213,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - \ - Muro
 {  5223,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - / - Muro
 {  5267,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - G - Cancello
 {  6002,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - C - Locazione.
 {  6016,  WILD_TYPE_COLL01    , WILD_TYPE_NULL      },  //  - 4 - c - Collinetta.
 {  6023,  WILD_TYPE_MONT02    , WILD_TYPE_NULL      },  //  - 2 - M - Alte montagne di Kor.
 {  6033,  WILD_TYPE_MONT01    , WILD_TYPE_NULL      },  //  - 2 - m - Montagne di Kor.
 {  6052,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - ~ - Muro.
 {  6055,  WILD_TYPE_LAGO01    , WILD_TYPE_NULL      },  //  - 5 - ~ - Golfo di Lamorka.
 {  6062,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - @ - Locazione.
 {  6066,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 4 - @ - Locazione.
 {  6067,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - @ - Locazione.
 {  6068,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 7 - @ - Locazione.
 {  6072,  WILD_TYPE_ALTO01    , WILD_TYPE_NULL      },  //  - 8 - a - Altopiano di Lamorka.
 {  6096,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 4 - = - Pontile.
 {  6097,  WILD_TYPE_RIVER     , WILD_TYPE_PONT01    },  //  - 6 - = - Ponte.
 {  6107,  WILD_TYPE_PIAN01    , WILD_TYPE_ROAD      },  //  - 6 - : - Strada del Porto.
 {  6112,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - . - Muro.
 {  6113,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - . - Muro.
 {  6114,  WILD_TYPE_PIAN01    , WILD_TYPE_NULL      },  //  - 3 - . - Prato.
 {  6116,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 4 - . - Sabbia.
 {  6118,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 7 - . - Facciata.
 {  6123,  WILD_TYPE_SPIA01    , WILD_TYPE_NULL      },  //  - 2 - x - Scogli.
 {  6132,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - F - Forca.
 {  6134,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - F - Foresta.
 {  6144,  WILD_TYPE_FORE01    , WILD_TYPE_NULL      },  //  - 3 - f - Bosco.
 {  6152,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - n - Entrata.
 {  6182,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - o - Muro.
 {  6183,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - o - Muro.
 {  6186,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - o - Molo.
 {  6192,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - - - Muro.
 {  6193,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - - - Muro.
 {  6198,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 7 - - - Muro.
 {  6202,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - | - Muro.
 {  6203,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - | - Muro.
 {  6206,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - | - Muro.
 {  6212,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - \ - Muro.
 {  6213,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - \ - Muro.
 {  6216,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - \ - Muro.
 {  6222,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - / - Muro.
 {  6223,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - / - Muro.
 {  6226,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 4 - / - Muro.
 {  6228,  WILD_TYPE_LOCA01    , WILD_TYPE_WALL      },  //  - 7 - / - Entrata.
 {  6243,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - + - Muro.
 {  6252,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 8 - ^ - Muro.
 {  6253,  WILD_TYPE_PIAN01    , WILD_TYPE_WALL      },  //  - 2 - ^ - Tetto.
 {  6272,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - V - Entrata.
 {  6284,  WILD_TYPE_FORE02    , WILD_TYPE_NULL      },  //  - 3 - v - Vegetazione.
 {  6292,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 8 - T - Taverna.
 {  6317,  WILD_TYPE_LOCA01    , WILD_TYPE_NULL      },  //  - 6 - @ - Locazione.
 { 65535,  WILD_TYPE_NULL      , WILD_TYPE_NULL      },  // Null cell.
 {     0,  WILD_TYPE_MISSING   , WILD_TYPE_MISSING   }   // End of the list.
};

 WORD cCode;
 for (idx = 0; ( cCode = cellConv[ idx ].cellSrvrCode ) != 0 ; idx++ ) // Simple search (slower).
	  {
		if  ( cCode == cellCode ) return &cellConv[ idx ];
		if  ( cCode  > cellCode ) break;
	  };

 return NULL;
};

/************************************************************************************/
void wilderness_draw( struct char_data *ch )
{
 wilderness_draw_type( ch, FALSE );

 // If graphic wild is active...
 if  ( ( ! IS_NPC( ch ) ) && ( GET_CLIENT( ch ) == 1 ) )
	  wilderness_draw_type( ch, TRUE );
};

void wilderness_draw_type( struct char_data *ch, BOOL graphWildON )
{
 int centerx,centery;
 int x,y;
 room_vnum vch;         /* Virtual number of room char is in */
 struct coord chcoord;  /* Coord of character */
 char c;
 int color;
 int lastcolor = -1;
 room_rnum r;
 int mobs;
 int miniwild = IS_IN_MINIWILD( ch ); // Vale 1 se e' miniwild, 0 se e' wilderness.

 char *localWildBuffer = buf; // Buffer used to save wild informations.
 WORD  bufLen = 0;                    					// For graphic client.
 cellConverter *cellPtr;		// Used to convers cell types.

 strcpy( localWildBuffer, "\0" ); // Reset wild buffer.

 if  ( IS_NPC( ch ) )
	  {
		centerx = 8;
		centery = 6;
	  }
 else
	  {
		centerx = wild_time_modify( ch, GET_WILD_MAX_X_RANGE( ch ) );
		centery = wild_time_modify( ch, GET_WILD_MAX_Y_RANGE( ch ) );
	  };

 vch = world[ ch->in_room ].number;
 wild_coord_from_virtual( vch, &chcoord );

 // If graphic wild is active...
 if  ( graphWildON != FALSE )
	  {
		localWildBuffer[ 0 ] = 0x1F;   // Command begin.
		localWildBuffer[ 1 ] = 0x00;	 // Length of command (lower  part).
		localWildBuffer[ 2 ] = 0x00;   // Length of command (higher part).
		localWildBuffer[ 3 ] = 0x01;   // Command number.
		localWildBuffer[ 4 ] = (BYTE)( ( 2 * centery ) + 1 );
		localWildBuffer[ 5 ] = (BYTE)( ( 2 * centerx ) + 1 );
      bufLen = 6;
	  };

 for ( y = chcoord.y - centery; y <= chcoord.y + centery; y++ )
	  {
		for ( x = chcoord.x - centerx; x <= chcoord.x + centerx; x++ )
			 {
			  // If out of the edges...
			  if  ( ( x < MWILD_RXOR ) ||
					  ( x > MWILD_RXEN ) ||
					  ( y < MWILD_RYOR ) ||
					  ( y > MWILD_RYEN ) )
					{
					 // If graphic wild is active...
                if  ( graphWildON != FALSE )
                    {
							localWildBuffer[ bufLen ] = 0x01;	bufLen++;
							localWildBuffer[ bufLen ] = 0xFF;	bufLen++;
							localWildBuffer[ bufLen ] = 0xFF;	bufLen++;
						  }
                else
						  {
							if  ( lastcolor != -1 ) strcat( localWildBuffer, QNRM );
							lastcolor = -1;
							strcat( localWildBuffer, " " );
						  };
					}
			  else

			  // If player center...
			  if  ( ( x == chcoord.x ) &&
					  ( y == chcoord.y ) )
					{
					 // If graphic wild is active...
					 if  ( graphWildON != FALSE )
						  {
							if  ( miniwild ) r = real_room( MINIWILD_VNUM( real_zone( vch ), x , y ) );
							else             r = real_room( WILD_VNUM( x, y ) );

							cellPtr = MakeConversion( GET_WILD_TABLE( r ).index );
							if  ( cellPtr == NULL )
								 {
								  localWildBuffer[ bufLen ] = 0x01;	bufLen++;
								  localWildBuffer[ bufLen ] = 0xFE;	bufLen++;
								  localWildBuffer[ bufLen ] = 0xFF;	bufLen++;
								 }
							else
								 {
								  localWildBuffer[ bufLen ] = ( cellPtr->cellBackBitmap != WILD_TYPE_NULL ? 1 : 0 ) + ( cellPtr->cellForeBitmap != WILD_TYPE_NULL ? 1 : 0 );	bufLen++;
								  if  ( cellPtr->cellBackBitmap != WILD_TYPE_NULL ) { localWildBuffer[ bufLen ] = cellPtr->cellBackBitmap & 0xFF; bufLen++;
																										localWildBuffer[ bufLen ] = cellPtr->cellBackBitmap >>   8; bufLen++; };
								  if  ( cellPtr->cellForeBitmap != WILD_TYPE_NULL ) { localWildBuffer[ bufLen ] = cellPtr->cellForeBitmap & 0xFF; bufLen++;
																										localWildBuffer[ bufLen ] = cellPtr->cellForeBitmap >>   8; bufLen++; };
								 };
						  }
					 else
						  {
							if ( lastcolor != -1 ) strcat( localWildBuffer, QNRM );
							lastcolor = -1;
							strcat( localWildBuffer, "#" );
						  };
					}
			  else

			  // If simple cell...
					{
					 // If graphic wild is active...
					 if  ( graphWildON != FALSE )
						  {
							if  ( miniwild ) r = real_room( MINIWILD_VNUM( real_zone( vch ), x, y ) );
							else             r = real_room( WILD_VNUM( x, y ) );

							cellPtr = MakeConversion( GET_WILD_TABLE( r ).index );
							if  ( cellPtr == NULL )
								 {
								  localWildBuffer[ bufLen ] = 0x01;	bufLen++;
								  localWildBuffer[ bufLen ] = 0xFE;	bufLen++;
								  localWildBuffer[ bufLen ] = 0xFF;	bufLen++;
								 }
							else
								 {
								  localWildBuffer[ bufLen ] = ( cellPtr->cellBackBitmap != WILD_TYPE_NULL ? 1 : 0 ) + ( cellPtr->cellForeBitmap != WILD_TYPE_NULL ? 1 : 0 );	bufLen++;
								  if  ( cellPtr->cellBackBitmap != WILD_TYPE_NULL ) { localWildBuffer[ bufLen ] = cellPtr->cellBackBitmap & 0xFF; bufLen++;
																										localWildBuffer[ bufLen ] = cellPtr->cellBackBitmap >>   8; bufLen++; };
								  if  ( cellPtr->cellForeBitmap != WILD_TYPE_NULL ) { localWildBuffer[ bufLen ] = cellPtr->cellForeBitmap & 0xFF; bufLen++;
																										localWildBuffer[ bufLen ] = cellPtr->cellForeBitmap >>   8; bufLen++; };
								 };
						  }
					 else
						  {
//							if  ( ! miniwild && wilderness_los( ch, x, y, chcoord.x, chcoord.y ) ) // PER ORA NIENTE LOS
							if  ( 1 )
								 {
								  if  ( miniwild ) r = real_room( MINIWILD_VNUM( real_zone( vch ), x, y ) );
								  else             r = real_room( WILD_VNUM( x, y ) );

								  if  ( ROOM_FLAGGED( r, ROOM_NO_DRAW ) ) mobs = 0;
								  else                                    mobs = any_mob( ch, r );

								  switch ( mobs )
									 {
									  case 2 : c = '#'; color = 6; /*Magenta*/ break;
									  case 1 : c = '#'; color = 2; /*Rosso*/   break;
									  /* Eserciti. assegnamento fuffa. Poi i colori
									   * li assegno nella funzione colore_esercito */
									  case 3 : c = '*'; color = 1; break; // Eserciti
									  break;
									  default: c     = GET_WILD_TABLE( r ).symbol;
												  color = GET_WILD_TABLE( r ).color;
									  break;
									 };
								 }
							else
								 {
								  c = ' ';
								  color = -1;
								 };

							if  ( color != lastcolor )
								 {
								  lastcolor = color;
								  strcat( localWildBuffer, color_from_color( ch, color ) );
								 };
							if (mobs == 3) {// se devo visualizzare un esercito
								 sprintf( localWildBuffer + strlen ( localWildBuffer),
								 "%s", 
								 colore_esercito(ch, r));
								
							} else
								sprintf( localWildBuffer + strlen( localWildBuffer ), "%c", c );
						  };
					};
			 };

		// If graphic wild is NOT active...
		if  ( graphWildON == FALSE )
			 {
			  strcat( localWildBuffer, QNRM );
			  lastcolor = -1;
			  strcat( localWildBuffer, "\r\n" );
			 };
	  };

 // If graphic wild is active...
 if  ( graphWildON != FALSE ) { 
	 localWildBuffer[ 1 ] = ( bufLen - 1 ) & 0xFF; 
	 localWildBuffer[ 2 ] = bufLen >> 8;   
//	 send_bin_to_char( (BYTE *)localWildBuffer, bufLen, ch ); 
 }
 else
   send_to_char( localWildBuffer, ch );
};



// CHIAMATA SOLO SE WILDERNESS NORMALE, NON CON LE MINIWILD
int wilderness_los(struct char_data *ch, int x, int y, int cx, int cy)
{
room_rnum rch = ch->in_room;
int h = GET_WILD_TABLE(rch).altitudine;
int H = GET_WILD_TABLE(real_room(WILD_VNUM(x,y))).altitudine;
int dx,dy,sx,sy;
int i,err;

  dx=abs(cx-x);
  dy=abs(cy-y);
  sx = cx >= x ? 1 : -1;
  sy = cy >= y ? 1 : -1;

  if (dx>dy)
	 {
	 err=2*dy-dx;
	 for (i=1; i<dx; i++)
		{
		while (err>=0)
		  {
        rch -= WILD_XSIZE * sy;
        err -= 2*dx;
        }
      rch -= sx;
      err += 2*dy;
      if (GET_WILD_TABLE(rch).altitudine > h+(H-h)*((float)i/dx) ) return 0;
      }
    }
  else
    {
    err=2*dx-dy;
    for (i=1; i<dy; i++)
      {
      while (err>=0)
        {
        rch -= sx;
        err -= 2*dy;
        }
      rch -= WILD_XSIZE * sy;
      err += 2*dx;
      if (GET_WILD_TABLE(rch).altitudine > h+(H-h)*((float)i/dy) ) return 0;
      }
    }
  return 1;
}


int wild_time_modify(struct char_data *ch, int radius)
{
  extern struct time_info_data time_info;
  float temp;

  if (GET_LEVEL(ch)>=LVL_QUEST_MASTER) return radius;
  if (time_info.hours<5 || time_info.hours>=22) temp = 0.3 * radius;
  else if (time_info.hours>=7 && time_info.hours<20) temp = radius;
  else temp = 0.6 * radius;

  if ((temp - (int)temp)>0) return (int) (temp+1);
  else return (int) temp;
}


/*cacciatore by lance*/
 int is_selvaggina(struct char_data *ch, struct char_data *vict, room_rnum room)
 {
  struct coord chcoord;  /* Coord of character */
  struct coord victcoord;  /* Coord of victim */

  if (!ch) return 0;
  wild_coord_from_virtual(world[ch->in_room].number, &chcoord);
  wild_coord_from_virtual(world[room].number, &victcoord);

  if (vict && MOB_FLAGGED(vict,MOB_SELVAGGINA)) {
 	if (((abs(victcoord.x-chcoord.x) < RANGE_CACCIATORE) && (abs(victcoord.y-chcoord.y) < RANGE_CACCIATORE)) &&
 		 (GET_PROF(ch, PROF_CACCIARE)>0)) {
 		return 1;
 	} else return 0;
  } else return 1;

 }

/* Restituisce 2 se c'e' almeno un player, 1 se c'e' almeno un mob, zero altrimenti */
/* Restituisce 3 se c'e' un esercito */

int any_mob(struct char_data *ch, room_rnum room)
{
struct char_data *k;
int found=0;

  for (k=world[room].people; k; k=k->next_in_room)
    if ((!ch) || CAN_SEE(ch, k)) // Se ch e' null, allora controlla sempre
      {
      found=is_selvaggina(ch,k,room);
      //se e' disguisato esce il # rosso
      if(!IS_NPC(k) && AFF_FLAGGED(k,AFF_DISGUISE)) return 1;
      if (!IS_NPC(k)) return 2;
      if (IS_NPC(k) && MOB_FLAGGED(k ,MOB_ESERCITO)) return 3;
      }
  return found;
}


room_rnum room_wild_target_room (room_rnum room, int dir)
{
struct coord chcoord;
room_rnum target_room = room;
int miniwild = IS_MINIWILD(room);

  if (   world[room].wild_modif == TRUE 
      && EXIT2(room, dir) 
      && EXIT2(room, dir)->to_room != NOWHERE 
      && !IS_SET(EXIT2(room, dir)->exit_info, EX_CLOSED))
    return EXIT2(room, dir)->to_room;

  if ((dir<0) || (dir >= (NUM_OF_DIRS-2))) 
    return -1;

  wild_coord_from_virtual (world[room].number, &chcoord);
  
  if (  ((dir==0) && (chcoord.y==MWILD_RYOR)) ||
        ((dir==1) && (chcoord.x==MWILD_RXEN)) ||
        ((dir==2) && (chcoord.y==MWILD_RYEN)) ||
        ((dir==3) && (chcoord.x==MWILD_RXOR)) )
    {

    if (miniwild) 
      return real_room(zone_table[world[room].zone].miniwild_exit[dir]);
    else 
      return -1;
    }

  switch (dir)
    {
    case 0: target_room -= MWILD_XSIZE; break;
    case 2: target_room += MWILD_XSIZE; break;
    case 1: target_room += 1; break;
    case 3: target_room -= 1; break;
    default: return 0;
    }
  
  return target_room;
}

room_rnum wild_target_room (struct char_data *ch, int dir)
{
room_rnum target_room = ch->in_room;

  if (   world[ch->in_room].wild_modif == TRUE 
    && EXIT(ch, dir) 
    && EXIT(ch, dir)->to_room != NOWHERE 
    && !IS_SET(EXIT(ch, dir)->exit_info, EX_CLOSED) )
  return EXIT(ch, dir)->to_room;

  if ((target_room = room_wild_target_room (ch->in_room, dir)) < 0)
    return -1;

  if (!RIDING(ch) || !MOB_FLAGGED(RIDING(ch), MOB_HARD_TAME))
    if (!GET_WILD_TABLE(target_room).can_enter) 
      return -1;

  return target_room;
}

ACMD(do_horizon)
{
int view_range = 16;
int x,y;
struct coord chcoord; /* Coord of character */
room_rnum r;
float rapporto;
int distanza;
byte trovato=0;
int miniwild = IS_IN_MINIWILD(ch);

  if (!IS_IN_WILD(ch))
    {
    send_to_char("Puoi scrutare l'orizzonte solo quando ti trovi nella wilderness o in una miniwild.\r\n", ch);
    return;
    }
  send_to_char("Scruti l'orizzonte in cerca di un punto di riferimento.\r\n", ch);
  wild_coord_from_virtual(world[ch->in_room].number, &chcoord);
  for (y= MAX(chcoord.y-view_range,MWILD_RYOR); y <= MIN(chcoord.y+view_range,MWILD_RYEN); y++)
    {
    for(x= MAX(chcoord.x-view_range,MWILD_RXOR); x <= MIN(chcoord.x+view_range,MWILD_RXEN); x++)
      {
      if (miniwild) r = real_room(MINIWILD_VNUM(real_zone(world[ch->in_room].number),x,y));
      else r=real_room(WILD_VNUM(x,y));
      if (ROOM_FLAGGED(r, ROOM_LANDMARK) && r!=ch->in_room)
        {
        trovato = TRUE;
        distanza = MAX(abs(y-chcoord.y),abs(x-chcoord.x));
        if (distanza<=4) strcpy(buf1, "Molto vicino verso ");
        else if (distanza<=8) strcpy(buf1, "Vicino verso ");
        else if (distanza<=12) strcpy(buf1, "Lontano verso ");
        else strcpy(buf1, "Molto lontano verso ");
        if (x-chcoord.x==0)
          {
          if (chcoord.y-y>0) strcat(buf1, "Nord vedi ");
          else strcat(buf1, "Sud vedi ");
          }
        else if (x-chcoord.x>0)
          {
          rapporto = (float) (chcoord.y-y)/(x-chcoord.x);
          if (rapporto > 2)         strcat(buf1, "Nord vedi ");
          else if (rapporto >= 0.5) strcat(buf1, "Nord-Est vedi ");
          else if (rapporto > -0.5) strcat(buf1, "Est vedi ");
          else if (rapporto > -2)   strcat(buf1, "Sud-Est vedi ");
          else                      strcat(buf1, "Sud vedi ");
          }
        else
          {
          rapporto = - (float) (chcoord.y-y)/(x-chcoord.x);
          if (rapporto > 2)         strcat(buf1, "Nord vedi ");
          else if (rapporto >= 0.5) strcat(buf1, "Nord-Ovest vedi ");
          else if (rapporto > -0.5) strcat(buf1, "Ovest vedi ");
          else if (rapporto > -2)   strcat(buf1, "Sud-Ovest vedi ");
          else                      strcat(buf1, "Sud vedi ");
          }
        sprintf(buf, "%s%s.\r\n", buf1, world[r].name);
        send_to_char(buf, ch);
        }
      }
    }
  if (!trovato) send_to_char("Non riesci a scorgere niente all'orizzonte.\r\n", ch);
}

ACMD(do_spot)
{
int x_view_range = wild_time_modify(ch,GET_WILD_MAX_X_RANGE(ch));

int y_view_range = wild_time_modify(ch,GET_WILD_MAX_Y_RANGE(ch));
int x,y;
struct coord chcoord; /* Coord of character */
room_rnum r;
float rapporto;
int distanza;
byte trovato=0;
struct char_data * k;
room_vnum vch = world[ch->in_room].number;
int miniwild = IS_IN_MINIWILD(ch);
extern struct spell_info_type spell_info[];

  if (!CAN_LEARN_SKILL(ch, SKILL_SPOT) || (GET_SKILL(ch, SKILL_SPOT)<=0))
    {
    send_to_char("Non conosci niente della skill spot.\r\n", ch);
    return;
    }

  if (!IS_IN_WILD(ch))
    {
    send_to_char("Puoi avvistare gli altri solo quando ti trovi nella wilderness o nelle miniwild.\r\n", ch);
    return;
    }

  send_to_char("Cerchi di avvistare qualcuno vicino a te.\r\n", ch);

  if (!test_and_improve(ch, SKILL_SPOT, 100, 15, 0))
    {
    send_to_char("Non riesci ad avvistare nessuno.\r\n", ch);
    return;
    }

  wild_coord_from_virtual(world[ch->in_room].number, &chcoord);
  for (y= MAX(chcoord.y-y_view_range,MWILD_RYOR); y <= MIN(chcoord.y+y_view_range,MWILD_RYEN); y++)
    {
    for(x= MAX(chcoord.x-x_view_range,MWILD_RXOR); x <= MIN(chcoord.x+x_view_range,MWILD_RXEN); x++)
      {
      if (miniwild) r = real_room(MINIWILD_VNUM(real_zone(vch), x, y));
      else r=real_room(WILD_VNUM(x,y));
      if ((world[r].people) && r!=ch->in_room)
        {
        distanza = MAX(abs(y-chcoord.y),abs(x-chcoord.x));
        if (distanza<=4) strcpy(buf1, "Molto vicino verso ");
        else if (distanza<=8) strcpy(buf1, "Vicino verso ");
        else if (distanza<=12) strcpy(buf1, "Lontano verso ");
        else strcpy(buf1, "Molto lontano verso ");
        if (x-chcoord.x==0)
          {
          if (chcoord.y-y>0) strcat(buf1, "Nord vedi ");
          else strcat(buf1, "Sud vedi ");
          }
        else if (x-chcoord.x>0)
          {
          rapporto = (float) (chcoord.y-y)/(x-chcoord.x);
          if (rapporto > 2)         strcat(buf1, "Nord vedi ");
          else if (rapporto >= 0.5) strcat(buf1, "Nord-Est vedi ");
          else if (rapporto > -0.5) strcat(buf1, "Est vedi ");
          else if (rapporto > -2)   strcat(buf1, "Sud-Est vedi ");
          else                      strcat(buf1, "Sud vedi ");
          }
        else
          {
          rapporto = - (float) (chcoord.y-y)/(x-chcoord.x);
          if (rapporto > 2)         strcat(buf1, "Nord vedi ");
          else if (rapporto >= 0.5) strcat(buf1, "Nord-Ovest vedi ");
          else if (rapporto > -0.5) strcat(buf1, "Ovest vedi ");
          else if (rapporto > -2)   strcat(buf1, "Sud-Ovest vedi ");
          else                      strcat(buf1, "Sud vedi ");
          }
        for (k=world[r].people; k; k=k->next_in_room)
          if (CAN_SEE(ch, k))
            {
            trovato=TRUE;
            sprintf(buf, "%s%s.\r\n", buf1, GET_NAME(k));
            send_to_char(buf, ch);
            }
        }
      }
    }
  if (!trovato) send_to_char("Non riesci ad avvistare nessuno.\r\n", ch);
}

int wild_mobhunter(struct char_data * ch)
{
  int range=7; // Raggio di caccia
  int x,y;
  struct coord chcoord; /* Coord of character */
  room_rnum r;
  int distanza_minima=range+1;
  struct char_data * k;
  int vch = world[ch->in_room].number;
  int miniwild = IS_IN_MINIWILD(ch);

  if (WILDHUNT(ch) == ch) WILDHUNT(ch) = NULL;
  wild_coord_from_virtual(world[ch->in_room].number, &chcoord);

  if (WILDHUNT(ch))
    {
    for (k=world[ch->in_room].people; k; k=k->next_in_room)
      if (CAN_SEE(ch, k) && WILDHUNT(ch)==k)
        {
        hit (ch, k, TYPE_UNDEFINED);
        return -1;
        }
    for (y= MAX(chcoord.y-range,MWILD_RYOR); y <= MIN(chcoord.y+range,MWILD_RYEN); y++)
      {
      for(x= MAX(chcoord.x-range,MWILD_RXOR); x <= MIN(chcoord.x+range,MWILD_RXEN); x++)
        {
        if (miniwild) r = real_room(MINIWILD_VNUM(real_zone(vch), x, y));
        else r=real_room(WILD_VNUM(x,y));
        for (k=world[r].people; k; k=k->next_in_room)
          if (CAN_SEE(ch, k) && WILDHUNT(ch)==k)
            {
            if (abs(x-chcoord.x)>abs(y-chcoord.y))
              {
              if (x-chcoord.x > 0) return 1;
              else return 3;
              }
            else
              {
              if (y-chcoord.y > 0) return 2;
              else return 0;
              }
            }
        }
      }
    WILDHUNT(ch)=NULL; // Se arriviamo qui, la vittima non e' piu' in raggio
    }

  for (k=world[ch->in_room].people; k; k=k->next_in_room)
    if (ch!=k && !IS_NPC(k) && CAN_SEE(ch, k) && !PRF_FLAGGED(k, PRF_NOHASSLE))
      {
      WILDHUNT(ch)=k;
      hit (ch, k, TYPE_UNDEFINED);
      return -1;
      }

  for (y= MAX(chcoord.y-range,MWILD_RYOR); y <= MIN(chcoord.y+range,MWILD_RYEN); y++)
    {
    for(x= MAX(chcoord.x-range,MWILD_RXOR); x <= MIN(chcoord.x+range,MWILD_RXEN); x++)
      {
      if (miniwild) r = real_room(MINIWILD_VNUM(real_zone(vch), x, y));
      else r=real_room(WILD_VNUM(x,y));
      for (k=world[r].people; k; k=k->next_in_room)
        if (!IS_NPC(k) && CAN_SEE(ch, k) && !PRF_FLAGGED(k, PRF_NOHASSLE))
          distanza_minima=MIN(distanza_minima, MAX (abs(x-chcoord.x), abs(y-chcoord.y)));
      }
    }

  if (distanza_minima>range) return number(0,11);

  for (y= MAX(chcoord.y-range,MWILD_RYOR); y <= MIN(chcoord.y+range,MWILD_RYEN); y++)
    {
    for(x= MAX(chcoord.x-range,MWILD_RXOR); x <= MIN(chcoord.x+range,MWILD_RXEN); x++)
      {
      if (miniwild) r = real_room(MINIWILD_VNUM(real_zone(vch), x, y));
      else r=real_room(WILD_VNUM(x,y));
      for (k=world[r].people; k; k=k->next_in_room)
      if (ch!= k && !IS_NPC(k) && CAN_SEE(ch, k) && !PRF_FLAGGED(k, PRF_NOHASSLE) &&
          (distanza_minima==MAX(abs(x-chcoord.x), abs(y-chcoord.y))) )
        {
        WILDHUNT(ch)=k;
        if (abs(x-chcoord.x)>abs(y-chcoord.y))
          {
          if (x-chcoord.x > 0) return 1;
          else return 3;
          }
        else
          {
          if (y-chcoord.y > 0) return 2;
          else return 0;
          }
        }
      }
    }
  return number(0,11);
}

/***************************** FUNZIONI UTILI *******************************/

/* returns real_number of wild_table element from index */
int real_wild_table(int virtual_nr)
{
  int i;

  for (i=0; i<=top_of_wild_table; i++)
    if (wild_table[i].index==virtual_nr) return i;

  return -1;
}

/* returns real_number of wild_table element from symbol */
int real_wild_table_from_symbol (char c)
{
  int i;

  for (i=0; i<=top_of_wild_table; i++)
    if (wild_table[i].symbol==c) return i;

  return -1;
}

void wild_coord_from_virtual (int virtual_nr, struct coord * toret)
{

  if (!toret) CREATE(toret, struct coord, 1);

  if (virtual_nr >= MINIWILD_ORIGIN)
    {
    virtual_nr = virtual_nr % (MINIWILD_XMEMSIZE * MINIWILD_YMEMSIZE);
    toret->x=(virtual_nr % MINIWILD_XMEMSIZE);
    toret->y=(virtual_nr / MINIWILD_XMEMSIZE);
    return;
    }

  virtual_nr-=WILD_ORIGIN;
  toret->x=(virtual_nr % WILD_XMEMSIZE);
  toret->y=(virtual_nr / WILD_XMEMSIZE);
  return;
}

char * color_from_color (struct char_data * ch, int color)
{
  switch (color)
    {
    case 1: return QBLK;     // Nero usare solo per cio' che non si deve vedere
    case 2: return QRED;     // Rosso
    case 3: return QGRN;     // Verdino
    case 4: return QYEL;     // Giallo
    case 5: return QBLU;     // Blu scuro
    case 6: return QMAG;     // Magenta
    case 7: return QCYN;     // Azzurrino
    case 8: return QWHT;     // Grigetto-bianco
    case 9: return QBLD;
    case 10:return QBLN;
    case 11:return QCLR;
    case 12:return QUND;
    case 13:return QDAR;
    case 14:return QBBLK;
    case 15:return QBRED;
    case 16:return QBGRN;
    case 17:return QBYEL;
    case 18:return QBBLU;
    case 19:return QBMAG;
    case 20:return QBCYN;
    case 21:return QBWHT;
    }
  return QNRM;
}


/* Passa tutti i mob nella stanza. In base al clan dell'esercito mi spara fuori un colore */
/* E' identica alla any_mob, perche' devo prendere come punto di riferimento il mob che mi trova quella funzione */

char * colore_esercito(struct char_data * ch, room_rnum room ) {
struct char_data *k;

	for (k=world[room].people; k; k=k->next_in_room) {
		if ((!ch) || CAN_SEE(ch, k)) { // Se ch e' null, allora controlla sempre
//			found=is_selvaggina(ch,k,room);
			//se e' disguisato esce il # rosso
			if(!IS_NPC(k) && AFF_FLAGGED(k,AFF_DISGUISE))	
				return "*";
			if (!IS_NPC(k))
				return "*";
			if(IS_NPC(k) && MOB_FLAGGED(k, MOB_ESERCITO)) {
				switch (CLAN_ID(k)) {
				case 2: return "&3&8*&0"; // Elenia. Giallo brillante
				case 3: return "&6&8*&0"; // Tolnedra. Blu
				case 5: return "&4&8*&0"; // Aldaria. Blu brillante
				case 7: return "&5*&0";  // Necropoli. Magenta
				case 8: return "&0*&0";  // sendaria. Normal
				case 9: return "&1&8*&0"; // Alorn. Rosso acceso
				}
			}
		}
	}
	return "*";
}

