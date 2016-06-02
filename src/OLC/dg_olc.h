/*
** dg_olc.h: this header file is used in extending Oasis style OLC for
** dg-scripts onto a CircleMUD that already has dg-scripts (as released
** by Mark Heilpern on 1/1/98) implemented.
*/
#include "dg_scripts.h"


/* prototype exported functions from dg_olc.c */
void script_copy(void *dst, void *src, int type);
void script_save_to_disk(FILE *fp, void *item, int type);
void dg_script_menu(struct descriptor_data *d, int new_edit);
int dg_script_edit_parse(struct descriptor_data *d, char *arg);

/* define the largest set of commands for as trigger */
#define MAX_CMD_LENGTH 16384 /* 16k should be plenty and then some */


/*
 * Submodes of TRIGEDIT connectedness.
 */
#define TRIGEDIT_MAIN_MENU              0
#define TRIGEDIT_TRIGTYPE               1
#define TRIGEDIT_CONFIRM_SAVESTRING	2
#define TRIGEDIT_NAME			3
#define TRIGEDIT_INTENDED		4
#define TRIGEDIT_TYPES			5
#define TRIGEDIT_COMMANDS		6
#define TRIGEDIT_NARG			7
#define TRIGEDIT_ARGUMENT		8



#define OLC_SCRIPT_EDIT		    82766
#define SCRIPT_MAIN_MENU		0
#define SCRIPT_NEW_TRIGGER		1
#define SCRIPT_DEL_TRIGGER		2


#define OLC_SCRIPT_EDIT_MODE(d)	((d)->olc->script_mode)	/* parse input mode */
#define OLC_SCRIPT(d)           ((d)->olc->script)	/* script editing   */
#define OLC_ITEM_TYPE(d)	((d)->olc->item_type)	/* mob/obj/room     */
