#define WILD_TABLE_FILE "wild_table"
#define WILD_MAP_FILE   "wild_map"
#define WILD_WLD        "wild.wld"
#define WILD_INDEX_MAP  "index.map"
#define WILD_INDEX_WLD  "index.wld"
#define WILD_PREFIX     "world/wild"

#define WILD_ZONE     10000
#define WILD_ORIGIN   1000000
#define MINIWILD_ORIGIN 2000000
#define WILD_XMEMSIZE 1000
#define WILD_YMEMSIZE 1000
#define WILD_XSIZE    200
#define WILD_YSIZE    200
#define WILD_RXOR     400
#define WILD_RYOR     400
#define WILD_RXEN     (WILD_RXOR + WILD_XSIZE - 1)
#define WILD_RYEN     (WILD_RYOR + WILD_YSIZE - 1)
#define WILD_VNUM(x,y)     (WILD_ORIGIN + (WILD_YMEMSIZE * (y)) + (x))

#define MINIWILD_XMEMSIZE   100
#define MINIWILD_YMEMSIZE   100
#define MINIWILD_XSIZE      25
#define MINIWILD_YSIZE      17
#define MINIWILD_RXOR       40
#define MINIWILD_RYOR       40
#define MINIWILD_RXEN       (MINIWILD_RXOR + MINIWILD_XSIZE -1)
#define MINIWILD_RYEN       (MINIWILD_RYOR + MINIWILD_YSIZE -1)
#define MINIWILD_VNUM(zrnum,x,y)    ((zone_table[(zrnum)].number * 100)  + (MINIWILD_YMEMSIZE * (y)) + (x))

#define MWILD_RXOR          ((miniwild) ? MINIWILD_RXOR : WILD_RXOR)
#define MWILD_RYOR          ((miniwild) ? MINIWILD_RYOR : WILD_RYOR)
#define MWILD_RXEN          ((miniwild) ? MINIWILD_RXEN : WILD_RXEN)
#define MWILD_RYEN          ((miniwild) ? MINIWILD_RYEN : WILD_RYEN)
#define MWILD_XSIZE         ((miniwild) ? MINIWILD_XSIZE : WILD_XSIZE)
#define MWILD_YSIZE         ((miniwild) ? MINIWILD_YSIZE : WILD_YSIZE)


#define ZONE_NORMAL_AREA 0
#define ZONE_WILDERNESS  1
#define ZONE_MINIWILD    2

extern struct zone_data *zone_table;                     /* in db.c                */
extern struct wild_data *wild_table;
extern int top_of_world;
extern int top_of_wild_table;

#define IS_WILD(room_realnum) ((room_realnum <= top_of_world) &&   \
                               (zone_table[world[(room_realnum)].zone].wilderness != ZONE_NORMAL_AREA))
#define IS_WILDERNESS(room_realnum) ((room_realnum <= top_of_world) &&   \
                               (zone_table[world[(room_realnum)].zone].wilderness == ZONE_WILDERNESS))
#define IS_MINIWILD(room_realnum) ((room_realnum <= top_of_world) &&   \
                               (zone_table[world[(room_realnum)].zone].wilderness == ZONE_MINIWILD))
#define IS_IN_WILD(ch) (IS_WILD(ch->in_room))
#define IS_IN_WILDERNESS(ch) (IS_WILDERNESS(ch->in_room))
#define IS_IN_MINIWILD(ch) (IS_MINIWILD(ch->in_room))


#define GET_WILD_TABLE(room_realnum)  (wild_table[world[(room_realnum)].wild_rnum])

struct wild_data {
                 int index;
                 char * name;
                 char * description;
                 char symbol;
                 int color;
                 int owner;
                 int move_cost;
                 int altitudine;
                 int sector_type;
                 int can_enter;
                 int room_flags;
};

struct coord {
             int x;
             int y;
};

void wilderness_boot();
void wild_map_save_to_disk(int zone);        // Zone e' rnum
void wild_ascii_map_save_to_disk(int zone);  // Zone e' rnum
void wilderness_draw(struct char_data * ch);
int wild_target_room(struct char_data * ch, int dir);
int wilderness_move(struct char_data *ch, int dir, int need_specials_check);
int wild_time_modify(struct char_data *ch, int radius);

int real_wild_table(int virtual_nr);
int real_wild_table_from_symbol (char c);
void wild_coord_from_virtual (int virtual_nr, struct coord * toret);
char * color_from_color (struct char_data * ch, int color);
int any_mob(struct char_data *ch, room_rnum room);



