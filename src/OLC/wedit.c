/************************************************************************
 *  OasisOLC - wedit.c						v1.5	*
 *  Copyright 1996 Harvey Gilpin. Copyright 1998 Carlo Mocci.		*
 *  Original author: Levork						*
 ************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "utils.h"
#include "db.h"
#include "olc.h"
#include "wilderness.h"

/*------------------------------------------------------------------------*/

/*
 * External data structures.
 */
extern struct room_data *world;
extern char *room_bits[];
extern char *sector_types[];
extern char *colori[];
extern struct zone_data *zone_table;
extern int top_of_zone_table;

extern void kill_all_events(void);
/*------------------------------------------------------------------------*/

/*
 * Function Prototypes
 */
void wedit_disp_color_menu(struct descriptor_data *d);
void wedit_disp_flag_menu(struct descriptor_data *d);
void wedit_disp_sector_menu(struct descriptor_data *d);
void wedit_disp_menu(struct descriptor_data *d);
void wedit_parse(struct descriptor_data *d, char *arg);
void wedit_setup_new(struct descriptor_data *d);
void wedit_setup_existing(struct descriptor_data *d, int real_num);
void wedit_save_to_disk();
void wedit_save_internally(struct descriptor_data *d);
void free_wild(struct wild_data *wild);

/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\
  Utils and exported functions.
\*------------------------------------------------------------------------*/

void wedit_setup_new(struct descriptor_data *d)
{
  CREATE(OLC_WILD(d), struct wild_data, 1);

  OLC_WILD(d)->name = str_dup("Un nuovo tipo di regione non finito.");
  OLC_WILD(d)->description = str_dup("Sei in un nuovo tipo di regione non finito.\r\n");
  OLC_WILD(d)->symbol='N';
  OLC_WILD(d)->color=4;
  OLC_WILD(d)->owner=0;
  OLC_WILD(d)->move_cost=1;
  OLC_WILD(d)->altitudine=0;
  OLC_WILD(d)->sector_type=SECT_FIELD;
  OLC_WILD(d)->room_flags=0;
  OLC_WILD(d)->can_enter=1;
  wedit_disp_menu(d);
  OLC_VAL(d) = 0;
}

/*------------------------------------------------------------------------*/

void wedit_setup_existing(struct descriptor_data *d, int real_num)
{
  struct wild_data *wild;

  /*
   * Build a copy of the wild_element for editing.
   */
  CREATE(wild, struct wild_data, 1);

  *wild = wild_table[real_num];
  /*
   * Allocate space for all strings.
   */
  wild->name = str_dup(wild_table[real_num].name ? wild_table[real_num].name : "undefined");
  wild->description = str_dup(wild_table[real_num].description ?
			wild_table[real_num].description : "undefined\r\n");

  OLC_WILD(d) = wild;
  OLC_VAL(d) = 0;
  wedit_disp_menu(d);
}

/*------------------------------------------------------------------------*/

void wedit_save_internally(struct descriptor_data *d)
{
  int i, wild_num, found = 0;
  int x,y;
  int zone;
  room_rnum number;
  struct wild_data *new_wild_table;

  wild_num = real_wild_table(OLC_NUM(d));
  /*
   * Wild_Element exists: move contents over then free and replace it.
   */
  if (wild_num > 0)
    {
    free_wild(wild_table + wild_num);
    wild_table[wild_num] = *OLC_WILD(d);
    for ( zone = 0; zone <= top_of_zone_table; zone++)
      {
      if (zone_table[zone].wilderness == ZONE_WILDERNESS)
        {
        for ( x=WILD_RXOR; x<=WILD_RXEN; x++)
          for ( y=WILD_RYOR; y<WILD_RYEN; y++)
            if ( (number=real_room(WILD_VNUM(x,y)))>0 && world[number].wild_rnum==wild_num && world[number].wild_modif==FALSE)
              {
              world[number].name=wild_table[wild_num].name;
              world[number].description=wild_table[wild_num].description;
              world[number].sector_type=wild_table[wild_num].sector_type;
              world[number].room_flags=wild_table[wild_num].room_flags;
              }
        }
      else if (zone_table[zone].wilderness == ZONE_MINIWILD)
        {
        for ( x=MINIWILD_RXOR; x<=MINIWILD_RXEN; x++)
          for ( y=MINIWILD_RYOR; y<MINIWILD_RYEN; y++)
            if ( (number=real_room(MINIWILD_VNUM(zone,x,y)))>0 && world[number].wild_rnum == wild_num && world[number].wild_modif==FALSE)
              {
              world[number].name=wild_table[wild_num].name;
              world[number].description=wild_table[wild_num].description;
              world[number].sector_type=wild_table[wild_num].sector_type;
              world[number].room_flags=wild_table[wild_num].room_flags;
              }
        }
      }
    }
  else               /* Wild_element doesn't exist, add it. */
    {
    CREATE(new_wild_table, struct wild_data, top_of_wild_table + 2);

    /*
     * Count through wild_table.
     */
    for (i = 0; i <= top_of_wild_table; i++)
      {
      if (!found)
        {
	/*
	 * Is this the place? 
	 */
	if (wild_table[i].index > OLC_NUM(d))
          {
	  found = TRUE;
	  new_wild_table[i] = *(OLC_WILD(d));
	  new_wild_table[i].index = OLC_NUM(d);
	  wild_num = i;

	  /*
	   * Copy from world to new_world + 1.
	   */
	  new_wild_table[i + 1] = wild_table[i];

	  /*
	   * Update all wilderness rooms pointing to a new wild_rnum.
	   */
	  }
        else	/* Not yet placed, copy straight over. */
          new_wild_table[i] = wild_table[i];
        }
      else              /* Already been found. */
	new_wild_table[i + 1] = wild_table[i];
      }

    if (!found)        /* Still not found, insert at top of table. */
      {
      new_wild_table[i] = *(OLC_WILD(d));
      new_wild_table[i].index = OLC_NUM(d);
      wild_num = i;
      }

    /*
     * Update wilderness/miniwild room wild_rnum.
     */
    for ( zone =0; zone <= top_of_zone_table; zone++)
      {
      if (zone_table[zone].wilderness == ZONE_WILDERNESS)
        {
        for ( x=WILD_RXOR; x<=WILD_RXEN; x++)
          for ( y=WILD_RYOR; y<WILD_RYEN; y++)
            if ( (number=real_room(WILD_VNUM(x,y)))>0 && world[number].wild_rnum > wild_num )
              world[number].wild_rnum ++;
        }
      else if (zone_table[zone].wilderness == ZONE_MINIWILD)
        {
        for ( x=MINIWILD_RXOR; x<=MINIWILD_RXEN; x++)
          for ( y=MINIWILD_RYOR; y<MINIWILD_RYEN; y++)
            if ( (number=real_room(MINIWILD_VNUM(zone,x,y)))>0 && world[number].wild_rnum > wild_num )
              world[number].wild_rnum ++;
        }
      }
    /*
     * Copy wild table over to new one.
     */
    free(wild_table);
    wild_table = new_wild_table;
    top_of_wild_table++;
  }
  
  kill_all_events();
  olc_add_to_save_list(WILD_ZONE, OLC_SAVE_WILD); 
}

/*------------------------------------------------------------------------*/

void wedit_save_to_disk()
{
  int counter;
  FILE *fp;
  struct wild_data *wild;

  sprintf(buf, "%s/%s.new", WILD_PREFIX, WILD_TABLE_FILE);

  if (!(fp = fopen(buf, "w+")))
    {
    mudlog("SYSERR: OLC: Cannot open wild table file!", BRF, LVL_BUILDER, TRUE);
    return;
    }

  for (counter = 0; counter <= top_of_wild_table; counter++)
    {
    wild = (wild_table + counter);

    /*
     * Remove the '\r\n' sequences from description.
     */
    strcpy(buf1, wild->description ? wild->description : "Empty");
    strip_string(buf1);

    /*
     * Forget making a buffer, lets just write the thing now.
     */
    fprintf(fp, "#%d\n%s~\n%s~\n%c %d %d\n%d %d %d\n%d %d\n",
            wild_table[counter].index,
            wild_table[counter].name,
            buf1,
            wild_table[counter].symbol,
            wild_table[counter].color,
            wild_table[counter].owner,
            wild_table[counter].move_cost,
            wild_table[counter].altitudine,
            wild_table[counter].can_enter,
            wild_table[counter].sector_type,
            wild_table[counter].room_flags);
    }

  /*
   * Write final line and close.
   */
  fprintf(fp, "$~\n");
  fclose(fp);

  sprintf(buf2, "%s/%s", WILD_PREFIX, WILD_TABLE_FILE);

  /*
   * We're fubar'd if we crash between the two lines below.
   */
  remove(buf2);
  rename(buf, buf2);
  for (counter=0; counter <= top_of_zone_table; counter++)
    wild_ascii_map_save_to_disk(counter);
  olc_remove_from_save_list(WILD_ZONE, OLC_SAVE_WILD);
}

/*------------------------------------------------------------------------*/

void free_wild(struct wild_data *wild)
{
  if (wild->name)         free(wild->name);
  if (wild->description)  free(wild->description);
}

/**************************************************************************
 Menu functions 
 **************************************************************************/

/*
 * For color
 */
void wedit_disp_color_menu(struct descriptor_data *d)
{
  int counter;

#if defined(CLEAR_SCREEN)
  send_to_char("[H[J", d->character);
#endif

  for (counter = 1; counter <= NUM_WILD_COLOR; counter++)
    {
    sprintf(buf, "%s%2d%s) %s%s%s\r\n", grn, counter, nrm,
            (counter > 1 ? color_from_color(d->character, counter) : nrm),
            colori[counter], nrm);
    send_to_char(buf, d->character);
    }
  send_to_char("\r\nEnter color number : ", d->character);
  OLC_MODE(d) = WEDIT_COLOR;
}

/*
 * For room flags.
 */
void wedit_disp_flag_menu(struct descriptor_data *d)
{
  int counter, columns = 0;

  get_char_cols(d->character);
#if defined(CLEAR_SCREEN)
  send_to_char("[H[J", d->character);
#endif
  for (counter = 0; counter < NUM_ROOM_FLAGS; counter++) {
    sprintf(buf, "%s%2d%s) %-20.20s %s", grn, counter + 1, nrm,
		room_bits[counter], !(++columns % 2) ? "\r\n" : "");
    send_to_char(buf, d->character);
  }
  sprintbit(OLC_WILD(d)->room_flags, room_bits, buf1);
  sprintf(buf, "\r\nRoom flags: %s%s%s\r\n"
	  "Enter room flags, 0 to quit : ", cyn, buf1, nrm);
  send_to_char(buf, d->character);
  OLC_MODE(d) = WEDIT_FLAGS;
}

/*
 * For sector type.
 */
void wedit_disp_sector_menu(struct descriptor_data *d)
{
  int counter, columns = 0;

#if defined(CLEAR_SCREEN)
  send_to_char("[H[J", d->character);
#endif
  for (counter = 0; counter < NUM_ROOM_SECTORS; counter++)
    {
    sprintf(buf, "%s%2d%s) %-20.20s %s", grn, counter, nrm,
            sector_types[counter], !(++columns % 2) ? "\r\n" : "");
    send_to_char(buf, d->character);
    }
  send_to_char("\r\nEnter sector type : ", d->character);
  OLC_MODE(d) = WEDIT_SECTOR;
}

/*
 * The main menu.
 */
void wedit_disp_menu(struct descriptor_data *d)
{
  struct wild_data *wild;

  get_char_cols(d->character);
  wild = OLC_WILD(d);

  sprinttype(wild->color,             colori,       buf1);
  sprintbit((long)wild->room_flags,        room_bits,    buf2);
  sprintf(buf,
#if defined(CLEAR_SCREEN)
	  "[H[J"
#endif
	  "-- Wild Type number : [%s%d%s]\r\n"
	  "%s1%s) Name               : %s%s\r\n"
	  "%s2%s) Description        :\r\n%s%s"
          "%s3%s) Symbol             : %s%c\r\n"
	  "%s4%s) Color              : %s%s\r\n"
	  "%s5%s) Room flags         : %s%s\r\n",

	  cyn, OLC_NUM(d), nrm,
	  grn, nrm, yel, wild->name,
	  grn, nrm, yel, wild->description,
          grn, nrm, (wild->color > 1 ? color_from_color(d->character, wild->color) : nrm), wild->symbol,
          grn, nrm, (wild->color > 1 ? color_from_color(d->character, wild->color) : nrm), buf1,
	  grn, nrm, cyn, buf2
          );

  send_to_char(buf, d->character);

  sprinttype(wild->sector_type, sector_types, buf1);
  sprintf(buf,
	  "%s6%s) Sector type        : %s%s\r\n"
	  "%s7%s) Movement cost      : %s%d\r\n"
	  "%s8%s) Altitudine         : %s%d\r\n"
	  "%s9%s) Can enter into     : %s%s\r\n"
	  "%sA%s) Owner:             : %s%d\r\n"
	  "%sQ%s) Quit\r\n"
	  "Enter choice : ",

	  grn, nrm, cyn, buf1,
	  grn, nrm, cyn, wild->move_cost,
	  grn, nrm, cyn, wild->altitudine,
	  grn, nrm, cyn, wild->can_enter ? "YES" : "NO",
	  grn, nrm, cyn, wild->owner,
          grn, nrm
          );

  send_to_char(buf, d->character);

  OLC_MODE(d) = WEDIT_MAIN_MENU;
}

/**************************************************************************
  The main loop
 **************************************************************************/

void wedit_parse(struct descriptor_data *d, char *arg)
{
  int number  ;

  switch (OLC_MODE(d))
    {
    case WEDIT_CONFIRM_SAVESTRING:
      switch (*arg)
        {
        case 'y':
        case 'Y':
          wedit_save_internally(d);
          sprintf(buf, "OLC: %s edits wild element %d.", GET_NAME(d->character), OLC_NUM(d));
          mudlog(buf, CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE);
          /*
           * Do NOT free strings! Just the wild element structure. 
           */
          cleanup_olc(d, CLEANUP_STRUCTS);
          send_to_char("Wild element saved to memory.\r\n", d->character);
          break;
        case 'n':
        case 'N':
          /*
           * Free everything up, including strings, etc.
           */
          cleanup_olc(d, CLEANUP_ALL);
          break;
        default:
          send_to_char("Invalid choice!\r\nDo you wish to save this wild element internally? : ", d->character);
          break;
        }
      return;

    case WEDIT_MAIN_MENU:
      switch (*arg)
        {
        case 'q':
        case 'Q':
          if (OLC_VAL(d))    /* Something has been modified. */
            {
	    send_to_char("Do you wish to save this wild element internally? : ", d->character);
	    OLC_MODE(d) = WEDIT_CONFIRM_SAVESTRING;
            }
          else cleanup_olc(d, CLEANUP_ALL);
          return;
        case '1':
          send_to_char("Enter wild type name:-\r\n] ", d->character);
          OLC_MODE(d) = WEDIT_NAME;
          break;
        case '2':
          OLC_MODE(d) = WEDIT_DESC;
#if defined(CLEAR_SCREEN)
          SEND_TO_Q("\x1B[H\x1B[J", d);
#endif
          SEND_TO_Q("Enter wild type description: (/s saves /h for help)\r\n\r\n", d);
          d->backstr = NULL;
          if (OLC_WILD(d)->description)
            {
	    SEND_TO_Q(OLC_WILD(d)->description, d);
	    d->backstr = str_dup(OLC_WILD(d)->description);
            }
          d->str = &OLC_WILD(d)->description;
          d->max_str = MAX_ROOM_DESC;
          d->mail_to = 0;
          OLC_VAL(d)=1;
          break;
        case '3':
          send_to_char("Enter wild type symbol: \r\n", d->character);
          OLC_MODE(d) = WEDIT_SYMBOL;
          break;
        case '4':
          wedit_disp_color_menu(d);
          break;
        case '5':
          wedit_disp_flag_menu(d);
          break;
        case '6':
          wedit_disp_sector_menu(d);
          break;
        case '7':
          send_to_char("Enter wild type movement cost: \r\n", d->character);
          OLC_MODE(d) = WEDIT_MOVE_COST;
          break;
        case '8':
          send_to_char("Enter wild type altitudine: \r\n", d->character);
          OLC_MODE(d) = WEDIT_ALTITUDINE;
          break;
        case '9':
          if (OLC_WILD(d)->can_enter==TRUE) OLC_WILD(d)->can_enter=FALSE;
          else OLC_WILD(d)->can_enter=TRUE;
          OLC_VAL(d) = 1;
          wedit_disp_menu(d);
          break;
        case 'a':
        case 'A':
          send_to_char("This hasn't been implemented yet\r\n", d->character);
          wedit_disp_menu(d);
          break;
        default:
          send_to_char("Invalid choice!\r\n", d->character);
          wedit_disp_menu(d);
          break;
        }
      return;

    case WEDIT_NAME:
      if (OLC_WILD(d)->name)
      free(OLC_WILD(d)->name);
      if (strlen(arg) > MAX_ROOM_NAME)
        arg[MAX_ROOM_NAME - 1] = '\0';
      OLC_WILD(d)->name = str_dup((arg && *arg) ? arg : "undefined");
      break;

    case WEDIT_DESC:
      /*
       * We will NEVER get here, we hope.
       */
      mudlog("SYSERR: Reached WEDIT_DESC case in parse_wedit", BRF, LVL_BUILDER, TRUE);
      break;

    case WEDIT_SYMBOL:
      if (!*arg)
        {
        send_to_char("Invalid choice!", d->character);
        wedit_disp_menu(d);
        return;
        }
      else OLC_WILD(d)->symbol = *arg;
      break;

    case WEDIT_COLOR:
      number = atoi(arg);
      if (number < 1 || number > NUM_WILD_COLOR)
        {
        send_to_char("Invalid choice!", d->character);
        wedit_disp_color_menu(d);
        return;
        }
      else OLC_WILD(d)->color = number;
      break;

    case WEDIT_FLAGS:
      number = atoi(arg);
      if ((number < 0) || (number > NUM_ROOM_FLAGS))
        {
        send_to_char("That is not a valid choice!\r\n", d->character);
        wedit_disp_flag_menu(d);
        }
      else if (number == 0) break;
      else 
        {
        /*
         * Toggle the bit.
         */
        TOGGLE_BIT(OLC_WILD(d)->room_flags, 1 << (number - 1));
        wedit_disp_flag_menu(d);
        }
      return;

    case WEDIT_SECTOR:
      number = atoi(arg);
      if (number < 0 || number >= NUM_ROOM_SECTORS)
        {
        send_to_char("Invalid choice!", d->character);
        wedit_disp_sector_menu(d);
        return;
        }
      else OLC_WILD(d)->sector_type = number;
      break;

    case WEDIT_MOVE_COST:
      number = atoi(arg);
      if (number < 1 || number > MAX_WILD_MOVE_COST)
        {
        send_to_char("Invalid choice!", d->character);
        wedit_disp_menu(d);
        return;
        }
      else OLC_WILD(d)->move_cost = number;
      break;

    case WEDIT_ALTITUDINE:
      number = atoi(arg);
      if (number < 1 || number > MAX_WILD_ALTITUDINE)
        {
        send_to_char("Invalid choice!", d->character);
        wedit_disp_menu(d);
        return;
        }
      else OLC_WILD(d)->altitudine = number;
      break;

    default:
      /*
       * We should never get here.
       */
      mudlog("SYSERR: Reached default case in parse_wedit", BRF, LVL_BUILDER, TRUE);
      break;
    }
  
  /*
   * If we get this far, something has been changed.
   */
  OLC_VAL(d) = 1;
  wedit_disp_menu(d);
}


