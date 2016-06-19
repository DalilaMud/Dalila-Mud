/***************************************************************************
 *  OasisOLC - olc.c 		                                           *
 *    				                                           *
 *  Copyright 1996 Harvey Gilpin.                                          *
 ***************************************************************************/

#define _OASIS_OLC_

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "interpreter.h"
#include "comm.h"
#include "utils.h"
#include "db.h"
#include "olc.h"
#include "screen.h"
#include "wilderness.h"
#include "shop.h"

/*
 * External data structures.
 */
extern struct obj_data *obj_proto;
extern struct char_data *mob_proto;
extern struct room_data *world;
extern int top_of_zone_table;
extern struct zone_data *zone_table;
extern struct descriptor_data *descriptor_list;

/*
 * External functions.
 */
extern void zedit_setup(struct descriptor_data *d, int room_num);
extern void zedit_save_to_disk(int zone);
extern void zedit_new_zone(struct char_data *ch, int new_zone);
extern void medit_setup_new(struct descriptor_data *d);
extern void medit_setup_existing(struct descriptor_data *d, int rmob_num);
extern void medit_save_to_disk(int zone);
extern void redit_setup_new(struct descriptor_data *d);
extern void redit_setup_existing(struct descriptor_data *d, int rroom_num);
extern void redit_save_to_disk(int zone);
extern void oedit_setup_new(struct descriptor_data *d);
extern void oedit_setup_existing(struct descriptor_data *d, int robj_num);
extern void oedit_save_to_disk(int zone);
extern void sedit_setup_new(struct descriptor_data *d);
extern void sedit_setup_existing(struct descriptor_data *d, int robj_num);
extern void sedit_setup_shedit(struct descriptor_data *d, int robj_num);
extern void sedit_save_to_disk(int zone);
extern int real_shop(int vnum);
extern void free_shop(struct shop_data *shop);
extern void free_room(struct room_data *room);
extern void free_wild(struct wild_data *wild);
extern void medit_free_mobile(struct char_data *mob);
extern void trigedit_setup_new(struct descriptor_data *d);
extern void trigedit_setup_existing(struct descriptor_data *d, int rtrg_num);
extern int real_trigger(int vnum);
extern void wedit_setup_new(struct descriptor_data *d);
extern void wedit_setup_existing(struct descriptor_data *d, int rroom_num);
extern void wedit_save_to_disk();

/*
 * Internal function prototypes.
 */
int real_zone(room_vnum number);
void olc_saveinfo(struct char_data *ch);

/*
 * Global string constants.
 */
const char *save_info_msg[8] = {"Rooms", "Objects", "Zone info",
	"Mobiles", "Shops", "SAVEINFO", "Triggers", "Wild Types"}; 

/*
 * Internal data structures.
 */
struct olc_scmd_data {
	char *text;
	int con_type;
};

struct olc_scmd_data olc_scmd_info[8] =
{
	{"room", CON_REDIT},
	{"object", CON_OEDIT},
	{"room", CON_ZEDIT},
	{"mobile", CON_MEDIT},
	{"shop", CON_SEDIT},
	{"saveinfo", CON_WEDIT},        // CARLO cosa mettere come CON qui ???
	{"trigger",   CON_TRIGEDIT},
	{"wild type", CON_WEDIT}
};

/*------------------------------------------------------------*/

/*
 * Exported ACMD do_olc function.
 *
 * This function is the OLC interface.  It deals with all the 
 * generic OLC stuff, then passes control to the sub-olc sections.
 */

ACMD(do_olc)
{
	int number = -1, save = 0, real_num;
	struct descriptor_data *d;
	
  /*
   * No screwing around as a mobile.
   */
	if (IS_NPC(ch)) return;
	
	if (subcmd == SCMD_OLC_SAVEINFO)
	{
		olc_saveinfo(ch);
		return;
	}
	
  /*
   * Parse any arguments.
   */
	two_arguments(argument, buf1, buf2);
	if (!*buf1) 		/* No argument given. */
	{
		switch (subcmd)
		{
			case SCMD_OLC_ZEDIT:
			case SCMD_OLC_REDIT:
				number = world[IN_ROOM(ch)].number;
				break;
			case SCMD_OLC_TRIGEDIT:
			case SCMD_OLC_OEDIT:
			case SCMD_OLC_MEDIT:
			case SCMD_OLC_SEDIT:
			case SCMD_OLC_WEDIT:
				sprintf(buf, "Specify a %s VNUM to edit.\r\n", olc_scmd_info[subcmd].text);
				send_to_char(buf, ch);
				return;
		}
	}
	else if (!isdigit(*buf1))
	{
		if (strn_cmp("save", buf1, 4) == 0) 
		{
			if (subcmd == SCMD_OLC_WEDIT) 
			{
				save = 1;
				number=WILD_ORIGIN;
			}
			else if (!*buf2) 
			{
				if (GET_OLC_ZONE(ch)) 
				{
					save = 1;
					number = (GET_OLC_ZONE(ch) * 100);
				}
				else
				{
					send_to_char("Save which zone?\r\n", ch);
					return;
				}
			}
			else
			{
				save = 1;
				number = atoi(buf2) * 100;
			}
		} 
		else if (subcmd == SCMD_OLC_ZEDIT && GET_LEVEL(ch) >= LVL_GRGOD) {
			if ((strn_cmp("new", buf1, 3) == 0) && *buf2)
				zedit_new_zone(ch, atoi(buf2));
			else
				send_to_char("Specify a new zone number.\r\n", ch);
			return;
		} else {
			send_to_char("Yikes!  Stop that, someone will get hurt!\r\n", ch);
			return;
		}
	}
  /*
   * If a numeric argument was given, get it.
   */
	if (number == -1)
		number = atoi(buf1);
	
  /*
   * Check that whatever it is isn't already being edited.
   */
	for (d = descriptor_list; d; d = d->next)
		if (d->connected == olc_scmd_info[subcmd].con_type)
			if (d->olc && OLC_NUM(d) == number) {
				sprintf(buf, "That %s is currently being edited by %s.\r\n",
					olc_scmd_info[subcmd].text, GET_NAME(d->character));
				send_to_char(buf, ch);
				return;
			}
	d = ch->desc;
	
  /*
   * Give descriptor an OLC struct.
   */
	CREATE(d->olc, struct olc_data, 1);
	
  /*
   * Find the zone.
   */
	if (subcmd==SCMD_OLC_WEDIT) OLC_ZNUM(d) = real_zone (WILD_ORIGIN);
	else if ((OLC_ZNUM(d) = real_zone(number)) == -1) {
		send_to_char("Sorry, there is no zone for that number!\r\n", ch);
		free(d->olc);
		return;
	}
	
  /*
   * Everyone but IMPLs can only edit zones they have been assigned.
   * Ora e' un po' piu' differenziato ;-)
   */
	
 /* if ( (GET_LEVEL(ch) < LVL_MEDIUM_GOD) ||
     ((GET_LEVEL(ch) < LVL_GRGOD) && (subcmd==SCMD_OLC_MEDIT || subcmd==SCMD_OLC_OEDIT)) )
    if (zone_table[OLC_ZNUM(d)].number != GET_OLC_ZONE(ch))
      {
      send_to_char("You do not have permission to edit this zone.\r\n", ch);
      free(d->olc);
      return;
      }*/
	if (GET_LEVEL(ch) < LVL_GOD && !IS_SET(PLR_FLAGS(ch), PLR_ALL_OLC))
		if (zone_table[OLC_ZNUM(d)].number != GET_OLC_ZONE(ch) || GET_OLC_ZONE(ch) == 0)
		{
			send_to_char("Non hai il permesso di editare questa zona.\r\n", ch);
			free(d->olc);
			return;
		}
		
		
	if (save) {
		const char *type = NULL;
		
		switch (subcmd) {
			case SCMD_OLC_REDIT: type = "room";	break;
			case SCMD_OLC_ZEDIT: type = "zone";	break;
			case SCMD_OLC_SEDIT: type = "shop"; break;
			case SCMD_OLC_MEDIT: type = "mobile"; break;
			case SCMD_OLC_OEDIT: type = "object"; break;
			case SCMD_OLC_WEDIT: type = "wild type"; break;
		}
		if (!type) {
			send_to_char("Oops, I forgot what you wanted to save.\r\n", ch);
			return;
		}
		sprintf(buf, "Saving all %ss in zone %d.\r\n",
			type, zone_table[OLC_ZNUM(d)].number);
		send_to_char(buf, ch);
		sprintf(buf, "OLC: %s saves %s info for zone %d.", GET_NAME(ch), type,
			zone_table[OLC_ZNUM(d)].number);
		mudlog(buf, CMP, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE);
		
		switch (subcmd) {
			case SCMD_OLC_REDIT: redit_save_to_disk(OLC_ZNUM(d)); break;
			case SCMD_OLC_ZEDIT: zedit_save_to_disk(OLC_ZNUM(d)); break;
			case SCMD_OLC_OEDIT: oedit_save_to_disk(OLC_ZNUM(d)); break;
			case SCMD_OLC_MEDIT: medit_save_to_disk(OLC_ZNUM(d)); break;
			case SCMD_OLC_SEDIT: sedit_save_to_disk(OLC_ZNUM(d)); break;
			case SCMD_OLC_WEDIT: wedit_save_to_disk(); break;
		}
		free(d->olc);
		return;
	}
	OLC_NUM(d) = number;
	
  /*
   * Steal player's descriptor start up subcommands.
   */
	switch (subcmd) {
		case SCMD_OLC_TRIGEDIT:
			if ((real_num = real_trigger(number)) >= 0)
				trigedit_setup_existing(d, real_num);
			else
				trigedit_setup_new(d);
			STATE(d) = CON_TRIGEDIT;
			break;
		case SCMD_OLC_REDIT:
			if ((real_num = real_room(number)) >= 0)
				redit_setup_existing(d, real_num);
			else 
			if (zone_table[real_zone(number)].wilderness == ZONE_NORMAL_AREA)
				redit_setup_new(d);
			else
			{
				send_to_char("You can only edit existing wilderness/miniwild rooms.\r\n", ch);
				free(d->olc);
				return;
			}
			STATE(d) = CON_REDIT;
			break;
		case SCMD_OLC_ZEDIT:
			if ((real_num = real_room(number)) < 0) {
				send_to_char("That room does not exist.\r\n", ch);
				free(d->olc);
				return;
			}
			zedit_setup(d, real_num);
			STATE(d) = CON_ZEDIT;
			break;
		case SCMD_OLC_MEDIT:
			if ((real_num = real_mobile(number)) < 0)
				medit_setup_new(d);
			else
				medit_setup_existing(d, real_num);
			STATE(d) = CON_MEDIT;
			break;
		case SCMD_OLC_OEDIT:
			if ((real_num = real_object(number)) >= 0)
				oedit_setup_existing(d, real_num);
			else
				oedit_setup_new(d);
			STATE(d) = CON_OEDIT;
			break;
		case SCMD_OLC_SEDIT:
			if ((real_num = real_shop(number)) >= 0)
				sedit_setup_existing(d, real_num);
			else
				sedit_setup_new(d);
			STATE(d) = CON_SEDIT;
			break;
		case SCMD_OLC_WEDIT:
			if ((real_num = real_wild_table(number)) >= 0)
				wedit_setup_existing(d, real_num);
			else wedit_setup_new(d);
			STATE(d) = CON_WEDIT;
			break;
	}
	act("$n starts using OLC.", TRUE, d->character, 0, 0, TO_ROOM);
	SET_BIT(PLR_FLAGS(ch), PLR_WRITING);
}

/*------------------------------------------------------------*\
 Internal utilities 
\*------------------------------------------------------------*/

void olc_saveinfo(struct char_data *ch)
{
	struct olc_save_info *entry;
	
	if (olc_save_list)
		send_to_char("The following OLC components need saving:-\r\n", ch);
	else
		send_to_char("The database is up to date.\r\n", ch);
	
	for (entry = olc_save_list; entry; entry = entry->next) {
		sprintf(buf, " - %s for zone %d.\r\n",
			save_info_msg[(int)entry->type], entry->zone);
		send_to_char(buf, ch);
	}
}

int real_zone(room_vnum number)
{
	int counter;
	
	for (counter = 0; counter <= top_of_zone_table; counter++)
		if ((number >= (zone_table[counter].number * 100)) &&
			(number <= (zone_table[counter].top)))
			return counter;
		
	return -1;
}

/*------------------------------------------------------------*\
 Exported utilities 
\*------------------------------------------------------------*/

/*
 * Add an entry to the 'to be saved' list.
 */

void olc_add_to_save_list(int zone, byte type)
{
	struct olc_save_info *new;
	
  /*
   * Return if it's already in the list.
   */
	for (new = olc_save_list; new; new = new->next)
		if ((new->zone == zone) && (new->type == type))
			return;
		
	CREATE(new, struct olc_save_info, 1);
	new->zone = zone;
	new->type = type;
	new->next = olc_save_list;
	olc_save_list = new;
}

/*
 * Remove an entry from the 'to be saved' list.
 */

void olc_remove_from_save_list(int zone, byte type)
{
	struct olc_save_info **entry;
	struct olc_save_info *temp;
	
	for (entry = &olc_save_list; *entry; entry = &(*entry)->next)
		if (((*entry)->zone == zone) && ((*entry)->type == type)) {
			temp = *entry;
			*entry = temp->next;
			free(temp);
			return;
		}
}

/*
 * Set the colour string pointers for that which this char will
 * see at color level NRM.  Changing the entries here will change 
 * the colour scheme throughout the OLC.
 */

void get_char_cols(struct char_data *ch)
{
	nrm = CCNRM(ch, C_NRM);
	grn = CCGRN(ch, C_NRM);
	cyn = CCCYN(ch, C_NRM);
	yel = CCYEL(ch, C_NRM);
}

/*
 * This procedure removes the '\r\n' from a string so that it may be
 * saved to a file.  Use it only on buffers, not on the original
 * strings.
 */
void strip_string(char *buffer)
{
	register char *ptr, *str;
	
	ptr = buffer;
	str = ptr;
	
	while ((*str = *ptr)) {
		str++;
		ptr++;
		if (*ptr == '\r')
			ptr++;
	}
}

/*
 * This procdure frees up the strings and/or the structures
 * attatched to a descriptor, sets all flags back to how they
 * should be.
 */

void cleanup_olc(struct descriptor_data *d, byte cleanup_type)
{
	if (d->olc) {
    /*
     * Check for a room.
     */
		if (OLC_ROOM(d)) {
      /*
       * free_room doesn't perform sanity checks, must be careful here.
       */
			switch (cleanup_type) {
				case CLEANUP_ALL:	free_room(OLC_ROOM(d));	break;
				case CLEANUP_STRUCTS:  free(OLC_ROOM(d));	break;
				default: /* The caller has screwed up. */	break;
			}
		}
    /*
     * Check for a wild type.
     */
		if (OLC_WILD(d)) {
      /*
       * free_wild doesn't perform sanity checks, must be careful here.
       */
			switch (cleanup_type) {
				case CLEANUP_ALL:	free_wild(OLC_WILD(d));	break;
				case CLEANUP_STRUCTS:  free(OLC_WILD(d));	break;
				default: /* The caller has screwed up. */	break;
			}
		}
    /*
     * Check for an object.
     */
		if (OLC_OBJ(d))
      /*
       * free_obj() makes sure strings aern't part of the prototype.
       */
			free_obj(OLC_OBJ(d));
		
    /*
     * Check for a mob.
     */
		if (OLC_MOB(d))
      /*
       * medit_free_mobile() makes sure strings are not in the prototype.
       */
			medit_free_mobile(OLC_MOB(d));
		
    /*
     * Check for a zone.
     */
		if (OLC_ZONE(d)) {
      /*
       * cleanup_type is irrelevant here, free() everything.
       */
			free(OLC_ZONE(d)->name);
			free(OLC_ZONE(d)->cmd);
			free(OLC_ZONE(d));
		}
		
    /*
     * Check for a shop.
     */
		if (OLC_SHOP(d)) {
      /*
       * free_shop doesn't perform sanity checks, we must be careful here.
       */
			switch (cleanup_type) {
				case CLEANUP_ALL: free_shop(OLC_SHOP(d));	break;
				case CLEANUP_STRUCTS:  free(OLC_SHOP(d));	break;
				default: /* The caller has screwed up. */	break;
			}
		}
		
    /*
     * Restore descriptor playing status.
     */
		if (d->character) {
			REMOVE_BIT(PLR_FLAGS(d->character), PLR_WRITING);
			STATE(d) = CON_PLAYING;
			act("$n stops using OLC.", TRUE, d->character, 0, 0, TO_ROOM);
		}
		free(d->olc);
	}
}

ACMD(do_shsave)
{
	int number = -1,room = -1;
	struct descriptor_data *d;
	int i;
	
	one_argument(argument, arg);
	
	if (!*arg)
		i = get_shopindexfromroom(world[ch->in_room].number);
	else
		i = get_shopindexfromnum(arg);
	
	if (i==-1)
	{
		send_to_char("Il negozio non esiste.\r\n", ch);
		return;
	}
	
	if (world[ch->in_room].number != SHOP_ROOM(i,0))
	{	
		send_to_char("Devi essere nel negozio.\r\n", ch);
		return;
	}
	
	if (SHOP_PROPRIETARIO(i) == 0 && SHOP_CLAN(i) == -1)
	{
		send_to_char("Il negozio non e' assegnato a nessuno.\r\n", ch);
		return;
	}
	
	if (!IsYourShop(ch,i,1))
		return;
	
  /*
   * No screwing around as a mobile.
   */
	if (IS_NPC(ch))
		return;
	
	if (subcmd == SCMD_OLC_SAVEINFO)
	{
		olc_saveinfo(ch);
		return;
	}
	
	number = atoi(arg);
	room = world[ch->in_room].number;
	
	for (d = descriptor_list; d; d = d->next)
		if (d->connected == olc_scmd_info[subcmd].con_type)
			if (d->olc && OLC_NUM(d) == number) {
				sprintf(buf, "That %s is currently being edited by %s.\r\n",
					olc_scmd_info[subcmd].text, GET_NAME(d->character));
				send_to_char(buf, ch);
				return;
			}
	d = ch->desc;
	
  /*
   * Give descriptor an OLC struct.
   */
	CREATE(d->olc, struct olc_data, 1);
	
  /*
   * Find the zone.
   */
	
	OLC_ZNUM(d)= real_zone(room);    
	
	sprintf(buf, "Saving all %ss in zone %d.\r\n",
		"shop", zone_table[OLC_ZNUM(d)].number);
	send_to_char(buf, ch);
	sprintf(buf, "OLC: %s saves %s info for zone %d.", GET_NAME(ch), "shop",
		zone_table[OLC_ZNUM(d)].number);
	mudlog(buf, CMP, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE);
	
	sedit_save_to_disk(OLC_ZNUM(d));
	
	free(d->olc);
}

ACMD(do_shedit)
{
	int number = -1, real_num, room= -1;
	struct descriptor_data *d;
	int i;
	
	one_argument(argument, arg);
	
	if (!*arg)
		i = get_shopindexfromroom(world[ch->in_room].number);
	else
		i = get_shopindexfromnum(arg);
	
	if (i==-1)
	{
		send_to_char("Il negozio non esiste.\r\n", ch);
		return;
	}
	if (world[ch->in_room].number != SHOP_ROOM(i,0))
	{
		send_to_char("Devi essere nel negozio.\r\n", ch);
		return;
	}
	if (SHOP_PROPRIETARIO(i) == 0 && SHOP_CLAN(i) == -1)
	{
		send_to_char("Il negozio non e' assegnato a nessuno.\r\n", ch);
		return;
	}
	if (!IsYourShop(ch,i,1))
		return;
	
  /*
   * No screwing around as a mobile.
   */
	if (IS_NPC(ch))
		return;
	
	if (subcmd == SCMD_OLC_SAVEINFO)
	{
		olc_saveinfo(ch);
		return;
	}
	
  /*
   * If a numeric argument was given, get it.
   */
	
	number = SHOP_NUM(i);
	room = SHOP_ROOM(i,0);
	
  /*
   * Check that whatever it is isn't already being edited.
   */
	
	for (d = descriptor_list; d; d = d->next)
		if (d->connected == olc_scmd_info[subcmd].con_type)
			if (d->olc && OLC_NUM(d) == number) {
				sprintf(buf, "That %s is currently being edited by %s.\r\n",
					olc_scmd_info[subcmd].text, GET_NAME(d->character));
				send_to_char(buf, ch);
				return;
			}
	d = ch->desc;
	
  /*
   * Give descriptor an OLC struct.
   */
	CREATE(d->olc, struct olc_data, 1);
	
  /*
   * Find the zone.
   */
	if ((OLC_ZNUM(d) = real_zone(room)) == -1) {
		send_to_char("Sorry, there is no zone for that number!\r\n", ch);
		free(d->olc);
		return;
	}
  /*
   * Everyone but IMPLs can only edit zones they have been assigned.
   */
	
	OLC_NUM(d) = number;
	
  /*
   * Steal player's descriptor start up subcommands.
   */
	
	if ((real_num = real_shop(number)) >= 0)
		sedit_setup_shedit(d, real_num);
	
	STATE(d) = CON_SEDIT;
	
	
	act("$n starts using OLC.", TRUE, d->character, 0, 0, TO_ROOM);
	SET_BIT(PLR_FLAGS(ch), PLR_WRITING);
}
