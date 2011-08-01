/* ************************************************************************
*   File: objsave.c                                     Part of CircleMUD *
*  Usage: loading/saving player objects for rent and crash-save           *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* now with auto-equip - BK */

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"
#include "spells.h"
#include "wilderness.h"

/* these factors should be unique integers */
#define RENT_FACTOR 	1
#define CRYO_FACTOR 	4

extern struct str_app_type str_app[];
extern struct room_data *world;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct descriptor_data *descriptor_list;
extern struct player_index_element *player_table;
extern int top_of_p_table;
extern int min_rent_cost;
extern int get_shopindexfromroom(room_rnum room);
extern int shop_producing(struct obj_data * item, int shop_nr);
extern bool test_can_do_event(struct char_data *ch, long int comdtype, long pulse);

int gen_majordome(struct char_data * ch, struct char_data * recep, int cmd, char *arg, int mode);
void Crash_MobEqSave(struct char_data * ch, room_vnum room);
int Crash_save_mob(struct char_data *ch,long room, struct obj_data * obj, FILE * fp, int locate);
/* Extern functions */
bool check_free_hands(struct char_data *ch,struct obj_data *obj);
//bool check_hold_obj(struct char_data *ch,struct obj_data *obj);
ACMD(do_action);
SPECIAL(receptionist);
SPECIAL(cryogenicist);
SPECIAL(majordome);

struct obj_data *Obj_from_store_to(struct obj_file_elem object, int *locate)
{
	struct obj_data *obj;
	int j, i;
	
	if (real_object(object.item_number) > -1) {
		obj = read_object(object.item_number, VIRTUAL);
		*locate = (int) object.locate;
		for (i=0; i<10; i++)
			GET_OBJ_VAL(obj, i) = object.value[i];
		GET_OBJ_TYPE(obj) = object.type_flag;
		GET_OBJ_EXTRA(obj) = object.extra_flags;
		GET_OBJ_WEIGHT(obj) = object.weight;
		GET_OBJ_TIMER(obj) = object.timer;
		GET_OBJ_CSLOTS(obj) = object.curr_slots;
		GET_OBJ_TSLOTS(obj) = object.total_slots;
		for (i = 0; i < 4; i++)
			GET_OBJ_BITVECTOR(obj,i) = object.bitvector[i];
		GET_OBJ_MAT_TYPE(obj) = object.tipo_materiale;
		GET_OBJ_MAT_NUM(obj)  = object.num_materiale;
		
		if (object.min_level != -2550)
			GET_OBJ_LEVEL(obj) = object.min_level;
		if (object.wear_flags != -2550)
			GET_OBJ_WEAR(obj) = object.wear_flags;
		if (object.cost != -2550)
			GET_OBJ_COST(obj) = object.cost;
		if (object.cost_per_day != -2550)
			GET_OBJ_RENT(obj) = object.cost_per_day;
		
		for (j = 0; j < MAX_OBJ_AFFECT; j++)
			obj->affected[j] = object.affected[j];
		
		return obj;
	} else
		return NULL;
}



/* this function used in house.c */
struct obj_data *Obj_from_store(struct obj_file_elem object)
{
	int locate;
	
	return Obj_from_store_to(object, &locate);
}



int Obj_to_store_from(struct obj_data * obj, FILE * fl, int locate)
{
	int j, i;
	struct obj_file_elem object;
	
	object.item_number = GET_OBJ_VNUM(obj);
	object.locate = (sh_int) locate; /* where worn or inventory? */
	for (i=0; i<10; i++)
		object.value[i] = GET_OBJ_VAL(obj, i);
	object.type_flag =  GET_OBJ_TYPE(obj);
	object.extra_flags = GET_OBJ_EXTRA(obj);
	object.weight = GET_OBJ_WEIGHT(obj);
	object.timer = GET_OBJ_TIMER(obj);
	object.curr_slots = GET_OBJ_CSLOTS(obj);
	object.total_slots = GET_OBJ_TSLOTS(obj);
	object.wear_flags  = GET_OBJ_WEAR(obj);
	object.cost        = GET_OBJ_COST(obj);
	object.cost_per_day= GET_OBJ_RENT(obj);
	object.min_level   = GET_OBJ_LEVEL(obj);
	for (i = 0; i < 4; i++)
		object.bitvector[i] = GET_OBJ_BITVECTOR(obj, i);
	object.tipo_materiale = GET_OBJ_MAT_TYPE(obj);
	object.num_materiale  = GET_OBJ_MAT_NUM(obj);
	for (j = 0; j < MAX_OBJ_AFFECT; j++)
		object.affected[j] = obj->affected[j];
	
	if (fwrite(&object, sizeof(struct obj_file_elem), 1, fl) < 1) {
		if (obj && obj->name) perror(obj->name);
		perror("Error writing object in Obj_to_store (FILE)");	
		sprintf(buf,"Il sistema non ha salvato l'oggetto: %s Room: %d",obj->name,world[obj->in_room].number);
		mudlog(buf, CMP, LVL_IMMORT, FALSE);
		return 0;
	}
	
	if (IS_OBJ_STAT(obj, ITEM_RESTRING))
	{
		if (fputc(strlen(obj->name), fl) < 1)
		{
			perror("Error writing object in Obj_to_store R(Name s)");
			return 0;
		}
		if (fputs(obj->name, fl) < 1)
		{
			perror("Error writing object in Obj_to_store R(Name)");
			return 0;
		}
		if (fputc(strlen(obj->description), fl) < 1)
		{
			perror("Error writing object in Obj_to_store R(desc s)");
			return 0;
		}
		if (fputs(obj->description, fl) < 1)
		{
			perror("Error writing object in Obj_to_store R(desc)");
			return 0;
		}
		if (fputc(strlen(obj->short_description), fl) < 1)
		{
			perror("Error writing object in Obj_to_store R(sH_desc s)");
			return 0;
		}
		if (fputs(obj->short_description, fl) < 1)
		{
			perror("Error writing object in Obj_to_store  R(sH_desc)");
			return 0;
		}
	 //lance
		if (!obj->action_description) obj->action_description = str_dup(" ");
		if (fputc(strlen(obj->action_description), fl) < 1)
		{
			perror("Error writing object in Obj_to_store  R(Act_desc)");
			return 0;
		}
		if (fputs(obj->action_description, fl) < 1)
		{
			perror("Error writing object in Obj_to_store  R(Act_desc s)");
			return 0;
		}
	}
	
	return 1;
}

int Ascii_Obj_to_store_from(struct obj_data * obj, FILE * fl, int locate)
{
	int j;
	
	fprintf(fl, "---------\n");
	fprintf(fl, "VNUM: %d LOCATE: %d\n", GET_OBJ_VNUM(obj), locate);
	fprintf(fl, "VALORI: %d %d %d %d %d %d %d %d %d %d\n", GET_OBJ_VAL(obj, 0), GET_OBJ_VAL(obj, 1), GET_OBJ_VAL(obj, 2), GET_OBJ_VAL(obj, 3),
		GET_OBJ_VAL(obj, 4), GET_OBJ_VAL(obj, 5), GET_OBJ_VAL(obj, 6), GET_OBJ_VAL(obj, 7),
		GET_OBJ_VAL(obj, 8), GET_OBJ_VAL(obj, 9) );
	fprintf(fl, "EXTRA FLAGS: %Ld\n", GET_OBJ_EXTRA(obj));
	fprintf(fl, "WEAR  FLAGS: %d\n", GET_OBJ_WEAR(obj));
	fprintf(fl, "WEIGHT: %d TIMER: %d CSLOTS/TSLOTS: %d/%d\n",  GET_OBJ_WEIGHT(obj), GET_OBJ_TIMER(obj), GET_OBJ_CSLOTS(obj), GET_OBJ_TSLOTS(obj));
	fprintf(fl, "COST: %d RENT: %d MINLEVEL: %d\n", GET_OBJ_COST(obj), GET_OBJ_RENT(obj), GET_OBJ_LEVEL(obj));
	for (j = 0; j < MAX_OBJ_AFFECT; j++)
		fprintf(fl, "AFFECT NUMERO %d:       LOCATION: %d MODIFIER: %d\n", j, obj->affected[j].location, obj->affected[j].modifier);
	if (IS_OBJ_STAT(obj, ITEM_RESTRING))
	{
		fprintf(fl, "RESTRING NAME: %s\n", obj->name);
		fprintf(fl, "RESTRING DESC: %s\n", obj->description);
		fprintf(fl, "RESTRING SHDE: %s\n", obj->short_description);
		fprintf(fl, "RESTRING ACDE: %s\n", obj->action_description); //lance
	}
	return 1;
}


int Obj_to_store(struct obj_data * obj, FILE * fl)
{
	return Obj_to_store_from(obj, fl, 0);
}


int Crash_delete_file(char *name)
{
	char filename[50];
	FILE *fl;
	
	if (!get_filename(name, filename, CRASH_FILE))
		return 0;
	if (!(fl = fopen(filename, "rb"))) {
		if (errno != ENOENT) {	/* if it fails but NOT because of no file */
			sprintf(buf1, "SYSERR: deleting crash file %s (1)", filename);
			perror(buf1);
		}
		return 0;
	}
	fclose(fl);
	
	if (unlink(filename) < 0) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "SYSERR: deleting crash file %s (2)", filename);
			perror(buf1);
		}
	}
	return (1);
}


int Crash_delete_crashfile(struct char_data * ch)
{
	char fname[MAX_INPUT_LENGTH];
	struct rent_info rent;
	FILE *fl;
	
	if (!get_filename(GET_NAME(ch), fname, CRASH_FILE))
		return 0;
	if (!(fl = fopen(fname, "rb"))) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "SYSERR: checking for crash file %s (3)", fname);
			perror(buf1);
		}
		return 0;
	}
	if (!feof(fl))
		fread(&rent, sizeof(struct rent_info), 1, fl);
	fclose(fl);
	
	if (rent.rentcode == RENT_CRASH)
		Crash_delete_file(GET_NAME(ch));
	
	return 1;
}


int Crash_clean_file(char *name)
{
	char fname[MAX_STRING_LENGTH], filetype[20];
	struct rent_info rent;
	extern int rent_file_timeout, crash_file_timeout;
	FILE *fl;
	
	if (!get_filename(name, fname, CRASH_FILE))
		return 0;
	
  /*
   * open for write so that permission problems will be flagged now, at boot
   * time.
   */
	if (!(fl = fopen(fname, "r+b"))) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "SYSERR: OPENING OBJECT FILE %s (4)", fname);
			perror(buf1);
		}
		return 0;
	}
	if (!feof(fl))
		fread(&rent, sizeof(struct rent_info), 1, fl);
	fclose(fl);
	
	if ((rent.rentcode == RENT_CRASH) || (rent.rentcode == RENT_FORCED) || (rent.rentcode == RENT_TIMEDOUT))
	{
		if (rent.time < time(0) - (crash_file_timeout * SECS_PER_REAL_DAY))
		{
			Crash_delete_file(name);
			switch (rent.rentcode)
			{
				case RENT_CRASH:
					strcpy(filetype, "crash");
					break;
				case RENT_FORCED:
					strcpy(filetype, "forced rent");
					break;
				case RENT_TIMEDOUT:
					strcpy(filetype, "idlesave");
					break;
				default:
					strcpy(filetype, "UNKNOWN!");
					break;
			}
			sprintf(buf, "    Deleting %s's %s file.", name, filetype);
			log(buf);
			return 1;
		}
    /* Must retrieve rented items w/in 30 days */
	}
	else if (rent.rentcode == RENT_RENTED || rent.rentcode == RENT_CAMP)
		if (rent.time < time(0) - (rent_file_timeout * SECS_PER_REAL_DAY))
		{
			Crash_delete_file(name);
			sprintf(buf, "    Deleting %s's rent file.", name);
			log(buf);
			return 1;
		}
	return (0);
}


void update_obj_file(void)
{
	int i;

	for (i = 0; i <= top_of_p_table; i++)
		Crash_clean_file((player_table + i)->name);
	return;
}


void Crash_listrent(struct char_data * ch, char *name)
{
	FILE *fl;
	char fname[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	struct obj_file_elem object;
	struct obj_data *obj;
	struct rent_info rent;
	int lunghezza;
	char stringa[MAX_STRING_LENGTH];
	
	if (!get_filename(name, fname, CRASH_FILE))
		return;
	if (!(fl = fopen(fname, "rb"))) {
		sprintf(buf, "%s has no rent file.\r\n", name);
		send_to_char(buf, ch);
		return;
	}
	
	if (!feof(fl))
		fread(&rent, sizeof(struct rent_info), 1, fl);
	switch (rent.rentcode) {
		case RENT_RENTED:
			strcat(buf, "Rent\r\n");
			break;
		case RENT_CRASH:
			strcat(buf, "Crash\r\n");
			break;
		case RENT_CRYO:
			strcat(buf, "Cryo\r\n");
			break;
		case RENT_TIMEDOUT:
		case RENT_FORCED:
			strcat(buf, "TimedOut\r\n");
			break;
		case RENT_CAMP:
			strcat(buf, "Camp\r\n");
			break;
		default:
			strcat(buf, "Undef\r\n");
			break;
	}
	while (!feof(fl)) {
		fread(&object, sizeof(struct obj_file_elem), 1, fl);
		if (ferror(fl)) {
			fclose(fl);
			return;
		}
		if (!feof(fl))
		{
			if (IS_SET(object.extra_flags,ITEM_RESTRING))
			{
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
 		  //lance
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_listrent.");
					fclose(fl);
					return;
				}
			}
			
			if (real_object(object.item_number) > -1)
			{
				obj = read_object(object.item_number, VIRTUAL);
				sprintf(buf, "%s [%5d] (%5dau) <%2d> %-20s\r\n", buf,
					object.item_number, GET_OBJ_RENT(obj),
					object.locate, obj->short_description);
				extract_obj(obj);
			}
		}
	}
	send_to_char(buf, ch);
	fclose(fl);
}



int Crash_write_rentcode(struct char_data * ch, FILE * fl, struct rent_info * rent)
{
	if (fwrite(rent, sizeof(struct rent_info), 1, fl) < 1) {
		perror("Writing rent code.");
		return 0;
	}
	return 1;
}



/* so this is gonna be the auto equip (hopefully) */
void auto_equip(struct char_data *ch, struct obj_data *obj, int *locate)
{
	extern int invalid_class(struct char_data *ch, struct obj_data *obj);
	extern int invalid_align(struct char_data *ch, struct obj_data *obj);
	extern int objlevel (struct obj_data * obj);
//  extern void check_abil_obj(struct char_data *ch,struct obj_data *obj,bool wear);
	int j;
	
	if (*locate > 0) { /* was worn */
		switch (j = *locate-1) {
			case WEAR_LIGHT:
				break;
			case WEAR_FINGER_R:
			case WEAR_FINGER_L:
				if (!CAN_WEAR(obj,ITEM_WEAR_FINGER)) /* not fitting :( */
					*locate = 0;
				break;
			case WEAR_NECK_1:
			case WEAR_NECK_2:
				if (!CAN_WEAR(obj,ITEM_WEAR_NECK))
					*locate = 0;
				break;
			case WEAR_BODY:
				if (!CAN_WEAR(obj,ITEM_WEAR_BODY))
					*locate = 0;
				break;
			case WEAR_HEAD:
				if (!CAN_WEAR(obj,ITEM_WEAR_HEAD))
					*locate = 0;
				break;
			case WEAR_LEGS:
				if (!CAN_WEAR(obj,ITEM_WEAR_LEGS))
					*locate = 0;
				break;
			case WEAR_FEET:
				if (!CAN_WEAR(obj,ITEM_WEAR_FEET))
					*locate = 0;
				break;
			case WEAR_HANDS:
				if (!CAN_WEAR(obj,ITEM_WEAR_HANDS))
					*locate = 0;
				break;
			case WEAR_ARMS:
				if (!CAN_WEAR(obj,ITEM_WEAR_ARMS))
					*locate = 0;
				break;
			case WEAR_SHIELD:
				if (!CAN_WEAR(obj,ITEM_WEAR_SHIELD))
					*locate = 0;
				break;
			case WEAR_ABOUT:
				if (!CAN_WEAR(obj,ITEM_WEAR_ABOUT))
					*locate = 0;
				break;
			case WEAR_WAIST:
				if (!CAN_WEAR(obj,ITEM_WEAR_WAIST))
					*locate = 0;
				break;
			case WEAR_WRIST_R:
			case WEAR_WRIST_L:
				if (!CAN_WEAR(obj,ITEM_WEAR_WRIST))
					*locate = 0;
				break;
			case WEAR_WIELD:
			case WEAR_WIELD_L:
				if (!CAN_WEAR(obj,ITEM_WEAR_WIELD))
					*locate = 0;
				break;
			case WEAR_HOLD:
				if (!CAN_WEAR(obj,ITEM_WEAR_HOLD) &&
					!(CAN_WEAR(obj,ITEM_WEAR_WIELD) && ((GET_OBJ_TYPE(obj) == ITEM_WEAPON) || (GET_OBJ_TYPE(obj)==ITEM_WEAPON_2HANDS))))
					*locate = 0;
				break;
			case WEAR_LOBSX:
			case WEAR_LOBDX:
				if (!CAN_WEAR(obj,ITEM_WEAR_LOBSX))
					*locate = 0;
				break;
			case WEAR_SPALLE:
				if (!CAN_WEAR(obj, ITEM_WEAR_SPALLE))
					*locate= 0;
				break;
			case WEAR_IMMOBIL:
				if (!CAN_WEAR(obj, ITEM_WEAR_IMMOBIL))
					*locate= 0;
				break;
			case WEAR_EYE:
				if (!CAN_WEAR(obj, ITEM_WEAR_EYE))
					*locate= 0;
				break;
			case WEAR_BOCCA:
				if (!CAN_WEAR(obj, ITEM_WEAR_BOCCA))
					*locate= 0;
				break;
			case WEAR_ALTRO1:
				if (!CAN_WEAR(obj, ITEM_WEAR_ALTRO1))
					*locate= 0;
				break;
			case WEAR_HANG:
				if (!CAN_WEAR(obj, ITEM_WEAR_HANG))
					*locate= 0;
				break;
			case WEAR_RELIQUIA:	
				if (!CAN_WEAR(obj,ITEM_WEAR_RELIQUIA))
					*locate= 0;
				break;
			case WEAR_VESTE:  /*Inizio Jerold */
				if (!CAN_WEAR(obj, ITEM_WEAR_VESTE))
					*locate= 0;
				break;       /*Fine Jerold*/
			default:
				*locate = 0;
		}
		if (*locate > 0){
			if (!GET_EQ(ch,j))
			{
        /* check ch's alignment to prevent $M from being zapped through auto-equip */
				if (invalid_align(ch, obj) || invalid_class(ch, obj) ||
					(GET_LEVEL(ch)<GET_OBJ_LEVEL(obj)) ||
					(GET_OBJ_LEVEL(obj)<=0 && (GET_LEVEL(ch)<objlevel(obj))) )
					*locate = 0;
				else{
	    // if(check_hold_obj(ch,obj)){
					if(check_free_hands(ch,obj)){
//	      check_abil_obj(ch,obj,TRUE);
						equip_char(ch, obj, j);
						}
					}
				}
			else  /* oops - saved player with double equipment[j]? */
				*locate = 0;
			}
		}	
	if (*locate <= 0)
		obj_to_char(obj, ch);
}



#define MAX_BAG_ROW 5
/* should be enough - who would carry a bag in a bag in a bag in a
   bag in a bag in a bag ?!? */

int Crash_load(struct char_data * ch)
/* return values:
	0 - successful load, keep char in rent room.
	1 - load failure or load of crash items -- put char in temple.
	2 - rented equipment lost (no $)
*/
{
	void Crash_crashsave(struct char_data * ch);
	
	FILE *fl;
	char fname[MAX_STRING_LENGTH];
	struct obj_file_elem object;
	struct rent_info rent;
	int cost, orig_rent_code;
	float num_of_days;
	struct obj_data *obj;
	int locate, j;
	struct obj_data *obj1;
	struct obj_data *cont_row[MAX_BAG_ROW];
	int lunghezza;
	char stringa[MAX_STRING_LENGTH];
	
	if (!get_filename(GET_NAME(ch), fname, CRASH_FILE))
		return 1;
	if (!(fl = fopen(fname, "r+b"))) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "SYSERR: READING OBJECT FILE %s (5)", fname);
			perror(buf1);
			send_to_char("\r\n********************* NOTICE *********************\r\n"
				"There was a problem loading your objects from disk.\r\n"
				"Contact a God for assistance.\r\n", ch);
		}
		sprintf(buf, "%s entering game with no equipment.", GET_NAME(ch));
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
		return 1;
	}
	if (!feof(fl))
		fread(&rent, sizeof(struct rent_info), 1, fl);
	
	if (rent.rentcode == RENT_RENTED || rent.rentcode == RENT_TIMEDOUT)
	{
		num_of_days = (float) (time(0) - rent.time) / SECS_PER_REAL_DAY;
		cost = (int) (rent.net_cost_per_diem * num_of_days);
		if (cost > GET_GOLD(ch) + GET_BANK_GOLD(ch))
		{
			fclose(fl);
			sprintf(buf, "%s entering game, rented equipment lost (no $).", GET_NAME(ch));
			mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			GET_BANK_GOLD(ch) = 0;
			GET_GOLD(ch) = 0;
			Crash_crashsave(ch);
			return 2;
		}
		else
		{
			GET_BANK_GOLD(ch) -= MAX(cost - GET_GOLD(ch), 0);
			GET_GOLD(ch) = MAX(GET_GOLD(ch) - cost, 0);
			save_char(ch, NOWHERE);
		}
	}
	switch (orig_rent_code = rent.rentcode)
	{
		case RENT_RENTED:
			sprintf(buf, "%s un-renting and entering game.", GET_NAME(ch));
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			break;
		case RENT_CRASH:
			sprintf(buf, "%s retrieving crash-saved items and entering game.", GET_NAME(ch));
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			break;
		case RENT_CRYO:
			sprintf(buf, "%s un-cryo'ing and entering game.", GET_NAME(ch));
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			break;
		case RENT_FORCED:
		case RENT_TIMEDOUT:
			sprintf(buf, "%s retrieving force-saved items and entering game.", GET_NAME(ch));
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			break;
		case RENT_CAMP:
			sprintf(buf, "%s un-camping and entering game.", GET_NAME(ch));
			mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			break;
		default:
			sprintf(buf, "WARNING: %s entering game with undefined rent code.", GET_NAME(ch));
			mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
			break;
	}
	
	for (j = 0;j < MAX_BAG_ROW;j++)
		cont_row[j] = NULL; /* empty all cont lists (you never know ...) */
	
	while (!feof(fl))
	{
		fread(&object, sizeof(struct obj_file_elem), 1, fl);
		if (ferror(fl))
		{
			perror("Reading crash file: Crash_load.");
			fclose(fl);
			return 1;
		}
		if (!feof(fl))
		{
			obj = Obj_from_store_to(object, &locate);
			if ((obj) && (IS_OBJ_STAT(obj, ITEM_RESTRING)))
			{
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				obj->name=strdup(stringa);
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				obj->description=strdup(stringa);
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				obj->short_description=strdup(stringa);
 		  //lance
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (str_cmp(stringa," "))
					obj->action_description=strdup(stringa);
				
			}
			
			if (obj)
			{
				auto_equip(ch, obj, &locate);
				
/*
what to do with a new loaded item:

  if there's a list with <locate> less than 1 below this:
    (equipped items are assumed to have <locate>==0 here) then its
    container has disappeared from the file   *gasp*
     -> put all the list back to ch's inventory

  if there's a list of contents with <locate> 1 below this:
    check if it's a container
    - if so: get it from ch, fill it, and give it back to ch (this way the
        container has its correct weight before modifying ch)
    - if not: the container is missing -> put all the list to ch's inventory

  for items with negative <locate>:
    if there's already a list of contents with the same <locate> put obj to it
    if not, start a new list

Confused? Well maybe you can think of some better text to be put here ...

since <locate> for contents is < 0 the list indices are switched to
non-negative
*/
				
				if (locate > 0)
				{ /* item equipped */
					for (j = MAX_BAG_ROW-1;j > 0;j--)
						if (cont_row[j])
						{ /* no container -> back to ch's inventory */
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_char(cont_row[j], ch);
							}
							cont_row[j] = NULL;
						}
					if (cont_row[0])
					{ /* content list existing */
						if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER)
						{
	      /* rem item ; fill ; equip again */
							obj = unequip_char(ch, locate-1);
							obj->contains = NULL; /* should be empty - but who knows */
							for (;cont_row[0];cont_row[0] = obj1)
							{
								obj1 = cont_row[0]->next_content;
								obj_to_obj(cont_row[0], obj);
							}
							equip_char(ch, obj, locate-1);
						}
						else
						{ /* object isn't container -> empty content list */
							for (;cont_row[0];cont_row[0] = obj1)
							{
								obj1 = cont_row[0]->next_content;
								obj_to_char(cont_row[0], ch);
							}
							cont_row[0] = NULL;
						}
					}
				}
				else
				{ /* locate <= 0 */
					for (j = MAX_BAG_ROW-1;j > -locate;j--)
						if (cont_row[j])
						{ /* no container -> back to ch's inventory */
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_char(cont_row[j], ch);
							}
							cont_row[j] = NULL;
						}
						
					if (j == -locate && cont_row[j])
					{ /* content list existing */
						if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER)
						{
	      /* take item ; fill ; give to char again */
							obj_from_char(obj);
							obj->contains = NULL;
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_obj(cont_row[j], obj);
							}
							obj_to_char(obj, ch); /* add to inv first ... */
						}
						else
						{ /* object isn't container -> empty content list */
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_char(cont_row[j], ch);
							}
							cont_row[j] = NULL;
						}
					}
					
					if (locate < 0 && locate >= -MAX_BAG_ROW)
					{
	        /* let obj be part of content list
		   but put it at the list's end thus having the items
		   in the same order as before renting */
						obj_from_char(obj);
						if ((obj1 = cont_row[-locate-1]))
						{
							while (obj1->next_content)
								obj1 = obj1->next_content;
							obj1->next_content = obj;
						}
						else cont_row[-locate-1] = obj;
					}
				}
			}
		}
	}         // Fine del ciclo while
	
  /* turn this into a crash file by re-writing the control block */
	rent.rentcode = RENT_CRASH;
	rent.time = time(0);
	rewind(fl);
	Crash_write_rentcode(ch, fl, &rent);
	
	fclose(fl);
	
	if ((orig_rent_code == RENT_RENTED) || (orig_rent_code == RENT_CRYO) || (orig_rent_code == RENT_CAMP))
		return 0;
	else
		return 1;
}



int Crash_save(struct obj_data * obj, FILE * fp, int locate)
{
	struct obj_data *tmp;
	int result;
	
	if (obj) {
		Crash_save(obj->next_content, fp, locate);
		Crash_save(obj->contains, fp, MIN(0,locate)-1);
		result = Obj_to_store_from(obj, fp, locate);
		
		for (tmp = obj->in_obj; tmp; tmp = tmp->in_obj)
			GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);
		
		if (!result)
			return 0;
	}
	return TRUE;
}


void Crash_restore_weight(struct obj_data * obj)
{
	if (obj) {
		Crash_restore_weight(obj->contains);
		Crash_restore_weight(obj->next_content);
		if (obj->in_obj)
			GET_OBJ_WEIGHT(obj->in_obj) += GET_OBJ_WEIGHT(obj);
	}
}



void Crash_extract_objs(struct obj_data * obj)
{
	if (obj) {
		Crash_extract_objs(obj->contains);
		Crash_extract_objs(obj->next_content);
		extract_obj(obj);
	}
}


int Crash_is_unrentable(struct obj_data * obj)
{
	if (!obj)
		return 0;
	
	if (IS_OBJ_STAT(obj, ITEM_NORENT) || GET_OBJ_RENT(obj) < 0 ||
		GET_OBJ_RNUM(obj) <= NOTHING || GET_OBJ_TYPE(obj) == ITEM_KEY)
		return 1;
	
	return 0;
}


void Crash_extract_norents(struct obj_data * obj)
{
	if (obj) {
		Crash_extract_norents(obj->contains);
		Crash_extract_norents(obj->next_content);
		if (Crash_is_unrentable(obj))
			extract_obj(obj);
	}
}


/* get norent items from eq to inventory and
   extract norents out of worn containers */
void Crash_extract_norents_from_equipped(struct char_data * ch)
{
	int j;
	
	for (j = 0;j < NUM_WEARS;j++) {
		if (GET_EQ(ch,j)) {
			if (IS_OBJ_STAT(GET_EQ(ch,j), ITEM_NORENT) ||
				GET_OBJ_RENT(GET_EQ(ch,j)) < 0 ||
				GET_OBJ_RNUM(GET_EQ(ch,j)) <= NOTHING ||
				GET_OBJ_TYPE(GET_EQ(ch,j)) == ITEM_KEY)
				obj_to_char(unequip_char(ch,j),ch);
			else
				Crash_extract_norents(GET_EQ(ch,j));
		}
	}
}


void Crash_extract_expensive(struct obj_data * obj)
{
	struct obj_data *tobj, *max;
	
	max = obj;
	for (tobj = obj; tobj; tobj = tobj->next_content)
		if (GET_OBJ_RENT(tobj) > GET_OBJ_RENT(max))
			max = tobj;
	extract_obj(max);
}



void Crash_calculate_rent(struct obj_data * obj, int *cost)
{
	if (obj) {
		*cost += MAX(0, GET_OBJ_RENT(obj));
		Crash_calculate_rent(obj->contains, cost);
		Crash_calculate_rent(obj->next_content, cost);
	}
}


void Crash_crashsave(struct char_data * ch)
{
	char buf[MAX_INPUT_LENGTH];
	struct rent_info rent;
	int j;
	FILE *fp;
	
	if (IS_NPC(ch))
		return;
	
	if (!get_filename(GET_NAME(ch), buf, CRASH_FILE))
		return;
	if (!(fp = fopen(buf, "wb")))
		return;
	
	rent.rentcode = RENT_CRASH;
	rent.time = time(0);
	if (!Crash_write_rentcode(ch, fp, &rent)) {
		fclose(fp);
		return;
	}
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch,j)) {
			if (!Crash_save(GET_EQ(ch,j), fp, j+1)) {
				fclose(fp);
				return;
			}
			Crash_restore_weight(GET_EQ(ch,j));
		}
		
	if (!Crash_save(ch->carrying, fp, 0)) {
		fclose(fp);
		return;
	}
	Crash_restore_weight(ch->carrying);
	
	fclose(fp);
	REMOVE_BIT(PLR_FLAGS(ch), PLR_CRASH);
}


int Crash_save_mob(struct char_data *ch, long room, struct obj_data * obj, FILE * fp, int locate)
{
	struct obj_data *tmp;
	int result = (FALSE);
	int shop_nr;
	
	if (obj) {
		Crash_save(obj->next_content, fp, locate);
		Crash_save(obj->contains, fp, MIN(0,locate)-1);
		if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_SHOPKEEPER)) {
			if ((shop_nr = get_shopindexfromroom(room))!=-1)
				if (shop_producing(obj, shop_nr)==(TRUE))
					return TRUE;
			result = Obj_to_store_from(obj, fp, locate);
		} else
			result = Obj_to_store_from(obj, fp, locate);
		
		for (tmp = obj->in_obj; tmp; tmp = tmp->in_obj)
			GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);
		
		if (!result)
			return 0;
	}
	return TRUE;
}


void Crash_idlesave(struct char_data * ch)
{
	char buf[MAX_INPUT_LENGTH];
	struct rent_info rent;
	int j;
	int cost, cost_eq;
	FILE *fp;
	
	if (IS_NPC(ch))
		return;
	
	if (!get_filename(GET_NAME(ch), buf, CRASH_FILE))
		return;
	if (!(fp = fopen(buf, "wb")))
		return;
	
	Crash_extract_norents_from_equipped(ch);
	
	Crash_extract_norents(ch->carrying);
	
	cost = 0;
	Crash_calculate_rent(ch->carrying, &cost);
	
	cost_eq = 0;
	for (j = 0; j < NUM_WEARS; j++)
		Crash_calculate_rent(GET_EQ(ch,j), &cost_eq);
	
	cost <<= 1;			/* forcerent cost is 2x normal rent */
	cost_eq <<= 1;
	
	if (cost+cost_eq > GET_GOLD(ch) + GET_BANK_GOLD(ch)) {
		for (j = 0; j < NUM_WEARS; j++) /* unequip player with low money */
			if (GET_EQ(ch,j))
				obj_to_char(unequip_char(ch, j), ch);
		cost += cost_eq;
		cost_eq = 0;
		
		while ((cost > GET_GOLD(ch) + GET_BANK_GOLD(ch)) && ch->carrying) {
			Crash_extract_expensive(ch->carrying);
			cost = 0;
			Crash_calculate_rent(ch->carrying, &cost);
			cost <<= 1;
		}
	}
	
	if (!ch->carrying) {
		for (j = 0; j < NUM_WEARS && !(GET_EQ(ch,j)); j++)
			;
		if (j == NUM_WEARS) { /* no eq nor inv */
			fclose(fp);
			Crash_delete_file(GET_NAME(ch));
			return;
		}
	}
	rent.net_cost_per_diem = cost;
	
	rent.rentcode = RENT_TIMEDOUT;
	rent.time = time(0);
	rent.gold = GET_GOLD(ch);
	rent.account = GET_BANK_GOLD(ch);
	if (!Crash_write_rentcode(ch, fp, &rent)) {
		fclose(fp);
		return;
	}
	
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch,j)) {
			if (!Crash_save(GET_EQ(ch,j), fp, j+1)) {
				fclose(fp);
				return;
			}
			Crash_restore_weight(GET_EQ(ch,j));
			Crash_extract_objs(GET_EQ(ch,j));
		}
	if (!Crash_save(ch->carrying, fp, 0)) {
		fclose(fp);
		return;
	}
	fclose(fp);
	
	Crash_extract_objs(ch->carrying);
}


void Crash_rentsave(struct char_data * ch, int cost)
{
	char buf[MAX_INPUT_LENGTH];
	struct rent_info rent;
	int j;
	FILE *fp;
	
	if (IS_NPC(ch))
		return;
	
	if (!get_filename(GET_NAME(ch), buf, CRASH_FILE))
		return;
	if (!(fp = fopen(buf, "wb")))
		return;
	
	Crash_extract_norents_from_equipped(ch);
	
	Crash_extract_norents(ch->carrying);
	
	rent.net_cost_per_diem = cost;
	rent.rentcode = RENT_RENTED;
	rent.time = time(0);
	rent.gold = GET_GOLD(ch);
	rent.account = GET_BANK_GOLD(ch);
	if (!Crash_write_rentcode(ch, fp, &rent)) {
		fclose(fp);
		return;
	}
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch,j)) {
			if (!Crash_save(GET_EQ(ch,j), fp, j+1)) {
				fclose(fp);
				return;
			}
			Crash_restore_weight(GET_EQ(ch,j));
			Crash_extract_objs(GET_EQ(ch,j));
		}
	if (!Crash_save(ch->carrying, fp, 0)) {
		fclose(fp);
		return;
	}
	fclose(fp);
	
	Crash_extract_objs(ch->carrying);
}


void Crash_cryosave(struct char_data * ch, int cost)
{
	char buf[MAX_INPUT_LENGTH];
	struct rent_info rent;
	int j;
	FILE *fp;
	
	if (IS_NPC(ch))
		return;
	
	if (!get_filename(GET_NAME(ch), buf, CRASH_FILE))
		return;
	if (!(fp = fopen(buf, "wb")))
		return;
	
	Crash_extract_norents_from_equipped(ch);
	
	Crash_extract_norents(ch->carrying);
	
	GET_GOLD(ch) = MAX(0, GET_GOLD(ch) - cost);
	
	rent.rentcode = RENT_CRYO;
	rent.time = time(0);
	rent.gold = GET_GOLD(ch);
	rent.account = GET_BANK_GOLD(ch);
	rent.net_cost_per_diem = 0;
	if (!Crash_write_rentcode(ch, fp, &rent)) {
		fclose(fp);
		return;
	}
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch,j)) {
			if (!Crash_save(GET_EQ(ch,j), fp, j+1)) {
				fclose(fp);
				return;
			}
			Crash_restore_weight(GET_EQ(ch,j));
			Crash_extract_objs(GET_EQ(ch,j));
		}
	if (!Crash_save(ch->carrying, fp, 0)) {
		fclose(fp);
		return;
	}
	fclose(fp);
	
	Crash_extract_objs(ch->carrying);
	SET_BIT(PLR_FLAGS(ch), PLR_CRYO);
}

void Crash_campsave(struct char_data * ch, int cost)
{
	char buf[MAX_INPUT_LENGTH];
	struct rent_info rent;
	int j;
	FILE *fp;
	
	if (IS_NPC(ch))
		return;
	
	if (!get_filename(GET_NAME(ch), buf, CRASH_FILE))
		return;
	if (!(fp = fopen(buf, "wb")))
		return;
	
	Crash_extract_norents_from_equipped(ch);
	
	Crash_extract_norents(ch->carrying);
	
	rent.net_cost_per_diem = cost;
	rent.rentcode = RENT_CAMP;
	rent.time = time(0);
	rent.gold = GET_GOLD(ch);
	rent.account = GET_BANK_GOLD(ch);
	if (!Crash_write_rentcode(ch, fp, &rent)) {
		fclose(fp);
		return;
	}
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch,j)) {
			if (!Crash_save(GET_EQ(ch,j), fp, j+1)) {
				fclose(fp);
				return;
			}
			Crash_restore_weight(GET_EQ(ch,j));
			Crash_extract_objs(GET_EQ(ch,j));
		}
	if (!Crash_save(ch->carrying, fp, 0)) {
		fclose(fp);
		return;
	}
	fclose(fp);
	
	Crash_extract_objs(ch->carrying);
}



/* ************************************************************************
* Routines used for the receptionist					  *
************************************************************************* */

void Crash_rent_deadline(struct char_data * ch, struct char_data * recep,
	long cost)
{
	long rent_deadline;
	
	if (!cost)
		return;
	
	rent_deadline = ((GET_GOLD(ch) + GET_BANK_GOLD(ch)) / cost);
	sprintf(buf,
		"$n ti dice, 'Puoi rentare per %ld giorni%s con i soldi che hai\r\n"
		"in mano e in banca.'\r\n",
		rent_deadline, (rent_deadline > 1) ? "" : "");
	act(buf, FALSE, recep, 0, ch, TO_VICT);
}

int Crash_report_unrentables(struct char_data * ch, struct char_data * recep,
	struct obj_data * obj)
{
	char buf[128];
	int has_norents = 0;
	
	if (obj) {
		if (Crash_is_unrentable(obj)) {
			has_norents = 1;
			sprintf(buf, "$n ti dice, 'Non puoi rentare con %s.'", OBJS(obj, ch));
			act(buf, FALSE, recep, 0, ch, TO_VICT);
		}
		has_norents += Crash_report_unrentables(ch, recep, obj->contains);
		has_norents += Crash_report_unrentables(ch, recep, obj->next_content);
	}
	return (has_norents);
}



void Crash_report_rent(struct char_data * ch, struct char_data * recep,
	struct obj_data * obj, long *cost, long *nitems, int display, int factor)
{
	static char buf[256];
	
	if (obj) {
		if (!Crash_is_unrentable(obj)) {
			(*nitems)++;
			*cost += MAX(0, (GET_OBJ_RENT(obj) * factor));
			if (display) {
				sprintf(buf, "$n ti dice, '%5d coins per %s..'",
					(GET_OBJ_RENT(obj) * factor), OBJS(obj, ch));
				act(buf, FALSE, recep, 0, ch, TO_VICT);
			}
		}
		Crash_report_rent(ch, recep, obj->contains, cost, nitems, display, factor);
		Crash_report_rent(ch, recep, obj->next_content, cost, nitems, display, factor);
	}
}



int Crash_offer_rent(struct char_data * ch, struct char_data * receptionist,
	int display, int factor)
{
	extern int max_obj_save;	/* change in config.c */
	char buf[MAX_INPUT_LENGTH];
	int i;
	long totalcost = 0, numitems = 0, norent = 0;
	
	norent = Crash_report_unrentables(ch, receptionist, ch->carrying);
	for (i = 0; i < NUM_WEARS; i++)
		norent += Crash_report_unrentables(ch, receptionist, GET_EQ(ch,i));
	
	if (norent)
		return 0;
	
	totalcost = min_rent_cost * factor;
	
	Crash_report_rent(ch, receptionist, ch->carrying, &totalcost, &numitems, display, factor);
	
	for (i = 0; i < NUM_WEARS; i++)
		Crash_report_rent(ch, receptionist, GET_EQ(ch,i), &totalcost, &numitems, display, factor);
	
	if (!numitems) {
		act("$n ti dice, 'Ma non stai portando niente!  Quitta!'",
			FALSE, receptionist, 0, ch, TO_VICT);
		return (0);
	}
	if (numitems > max_obj_save) {
		sprintf(buf, "$n ti dice, 'Scusa, ma non ho abbastanza spazio per %d oggetti.'",
			max_obj_save);
		act(buf, FALSE, receptionist, 0, ch, TO_VICT);
		return (0);
	}
	if (display) {
		sprintf(buf, "$n ti dice, 'Piu', %d coin per la mia mancia..'",
			min_rent_cost * factor);
		act(buf, FALSE, receptionist, 0, ch, TO_VICT);
		sprintf(buf, "$n ti dice, 'Per un totale di %ld coins%s.'",
			totalcost, (factor == RENT_FACTOR ? " per giorno" : ""));
		act(buf, FALSE, receptionist, 0, ch, TO_VICT);
		if (totalcost > GET_GOLD(ch) + GET_BANK_GOLD(ch)) {
			act("$n ti dice, '...che vedo non puoi permetterti.'",
				FALSE, receptionist, 0, ch, TO_VICT);
			return (0);
		} else if (factor == RENT_FACTOR)
			Crash_rent_deadline(ch, receptionist, totalcost);
	}
	return (totalcost);
}



int gen_receptionist(struct char_data * ch, struct char_data * recep,
	int cmd, char *arg, int mode)
{
	int cost = 0;
	extern int free_rent;
	room_rnum save_room;
	struct follow_type *f;  
	char *action_table[] = {"sorride", "balla", "sospira", "arrosisce", "rutta",
		"tossisce", "emette strani suoni", "canta", "piroetta felice"};
	
	if (!ch->desc || IS_NPC(ch))
		return FALSE;
	
	if (!cmd && !number(0, 5)) {
		do_action(recep, "", find_command(action_table[number(0, 8)]), 0);
		return FALSE;
	}
	if (!CMD_IS("offer") && !CMD_IS("rent"))
		return FALSE;
	if (!AWAKE(recep)) {
		send_to_char("Non puo' parlare con te...\r\n", ch);
		return TRUE;
	}
	if (!CAN_SEE(recep, ch)) {
		act("$n dice, 'Non tratto con gente che non posso vedere...!'", FALSE, recep, 0, 0, TO_ROOM);
		return TRUE;
	}
	if (free_rent) {
		act("$n tells you, 'Il Rent e' gratis qui. Quitta, e i tuoi averi saranno salvati!'",
			FALSE, recep, 0, ch, TO_VICT);
		return 1;
	}
	if (CMD_IS("rent")) {
		
		if (!test_can_do_event(ch,LAST_TKILLER, 60 * 10))
		{
			send_to_char("Hai appena commeso un crimine non scappare codardo!\r\n", ch);
			return TRUE;
		}
		
		for (f = ch->followers; f; f = f->next)
			if ((f->follower) && IS_NPC(f->follower) && (
					(MOB_FLAGGED(f->follower, MOB_MOUNTABLE) || 
						(MOB_FLAGGED(f->follower,MOB_HARD_TAME) && AFF_FLAGGED(f->follower,AFF_CHARM)))))
			{
				sprintf(buf,"Devi prima rentare %s", GET_NAME(f->follower));
				send_to_char(buf,ch);
				return TRUE;
			}
			
		if (!(cost = Crash_offer_rent(ch, recep, FALSE, mode)))
			return TRUE;
		if (mode == RENT_FACTOR)
			sprintf(buf, "$n ti dice, 'Il rent ti costa %d gold coins per giorno.'", cost);
		else if (mode == CRYO_FACTOR)
			sprintf(buf, "$n ti dice, 'Ti costera' %d coins essere congelato.'", cost);
		act(buf, FALSE, recep, 0, ch, TO_VICT);
		if (cost > GET_GOLD(ch) + GET_BANK_GOLD(ch)) {
			act("$n ti dice, '...che, vedo, non puoi permetterti.'",
				FALSE, recep, 0, ch, TO_VICT);
			return TRUE;
		}
		if (cost && (mode == RENT_FACTOR))
			Crash_rent_deadline(ch, recep, cost);
		
		if (mode == RENT_FACTOR) {
			act("$n ripone i tuoi averi e ti accompagna nella tua stanza privata.",
				FALSE, recep, 0, ch, TO_VICT);
			Crash_rentsave(ch, cost);
			sprintf(buf, "%s ha rentato il gioco (%d/day, %d tot.)", GET_NAME(ch),
				cost, GET_GOLD(ch) + GET_BANK_GOLD(ch));
		} else {			/* cryo */
			act("$n ripone i tuoi averi e ti accompagna nella tua stanza privata.\r\n"
				"A white mist appears in the room, chilling you to the bone...\r\n"
				"You begin to lose consciousness...",
				FALSE, recep, 0, ch, TO_VICT);
			Crash_cryosave(ch, cost);
			sprintf(buf, "%s has cryo-rented.", GET_NAME(ch));
			SET_BIT(PLR_FLAGS(ch), PLR_CRYO);
		}
		
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
		act("$n helps $N into $S private chamber.", FALSE, recep, 0, ch, TO_NOTVICT);
		save_room = ch->in_room;
		extract_char(ch);
		save_char(ch, save_room);
		GET_LOADROOM(ch)=world[save_room].number;
	} else {
		Crash_offer_rent(ch, recep, TRUE, mode);
		act("$N gives $n an offer.", FALSE, ch, 0, recep, TO_ROOM);
	}
	return TRUE;
}

SPECIAL(majordome)
{
	return (gen_majordome(ch, me, cmd, argument, RENT_FACTOR));
}


SPECIAL(receptionist)
{
	return (gen_receptionist(ch, me, cmd, argument, RENT_FACTOR));
}


SPECIAL(cryogenicist)
{
	return (gen_receptionist(ch, me, cmd, argument, CRYO_FACTOR));
}


void Crash_save_all(void)
{
	struct descriptor_data *d;
	for (d = descriptor_list; d; d = d->next) {
		if ((d->connected == CON_PLAYING) && !IS_NPC(d->character)) {
			if (PLR_FLAGGED(d->character, PLR_CRASH)) {
				Crash_crashsave(d->character);
				save_char(d->character, d->character->in_room);
				REMOVE_BIT(PLR_FLAGS(d->character), PLR_CRASH);
			}
		}
	}
}

/*
 * By Spini:
 * uguale a Crash_save_all ma annulla gli xp giornalieri...
 */
void Reboot_save_all(void)
{
	struct descriptor_data *d;
	for (d = descriptor_list; d; d = d->next) {
		if ((d->connected == CON_PLAYING) && !IS_NPC(d->character)) {
			if (PLR_FLAGGED(d->character, PLR_CRASH)) {
				GET_XP_GIORNALIERI(d->character) = 0;
				Crash_crashsave(d->character);
				save_char(d->character, d->character->in_room);
				REMOVE_BIT(PLR_FLAGS(d->character), PLR_CRASH);
			}
		}
	}
}

int gen_majordome(struct char_data * ch, struct char_data * recep,
	int cmd, char *arg, int mode)
{
	int cost = 0;
	
	room_rnum save_room;
	char *action_table[] = {"sorride", "danza", "tira su col naso", "bacia", "singhiozza",
		"tossisce", "si tocca i capelli", "ramazza", "lava"};
	
	if (!ch->desc || IS_NPC(ch))
		return FALSE;
	
	if (!cmd && !number(0, 5)) {
		do_action(recep, "", find_command(action_table[number(0, 8)]), 0);
		return FALSE;
	}
	if (!CMD_IS("rent"))
		return FALSE;
	if (!AWAKE(recep)) {
		send_to_char("Non puo' parlare con te...\r\n", ch);
		return TRUE;
	}
	if (!CAN_SEE(recep, ch)) {
		act("$n dice, 'Non tratto con gente che non posso vedere...!'", FALSE, recep, 0, 0, TO_ROOM);
		return TRUE;
	}
	
	Crash_rentsave(ch, cost);
	save_room = ch->in_room;
	extract_char(ch);
	save_char(ch, save_room);
	GET_LOADROOM(ch)=world[save_room].number;
	
	return TRUE;
	
}

ACMD(do_camp)
{
	room_rnum save_room, to_room;
	struct follow_type *f;
	struct char_data *next;
	
	if (!ch->desc || IS_NPC(ch))
	{
		send_to_char("I mob non possono accamparsi !\r\n", ch);
		return;
	}
	
	
	if (!IS_IN_WILD(ch) && !ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON))
	{
		send_to_char("Ti puoi accampare solo nella wilderness (o in cella) !\r\n", ch);
		return;
	}
	
	if (FIGHTING(ch))
	{
		send_to_char("Non puoi accamparti mentre stai combattendo.\r\n", ch);
		return;
	}
	
	if ((time(NULL)-ch->last_flee)<180)
	{
		send_to_char("Devono passare almeno tre minuti dall'ultima volta che sei fuggito prima di poterti accampare.\r\n", ch);
		return;
	}
	if ((time(NULL)-ch->last_gdr)<180)
	{
		send_to_char("Hai appena cambiato modalita' di gioco dai il tempo agli altri\r\ndi accorgersene potrai accamparti tra tre minuti\r\n",ch);
		return;
	}	
	
	if (!ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON) && ((world[ch->in_room].people != ch) || (ch->next_in_room)))
	{
		
		for (next = ch->next_in_room;next!=NULL;next=next->next_in_room) {
			if (CAN_SEE (ch, next) && GET_LEVEL(next) < LVL_IMMORT) {
				send_to_char("C'e' troppa gente qui.\r\n", ch);
				return;
			}
		}
	}
	
	if (!test_can_do_event(ch,LAST_TKILLER, 60 * 10))
	{
		send_to_char("Hai appena commeso un crimine non scappare codardo!\r\n", ch);
		return;
	}
	
	for (f = ch->followers; f; f = f->next){
		if ((f->follower) && IS_NPC(f->follower) && AFF_FLAGGED(f->follower,AFF_CHARM) && !IS_SET (MOB_FLAGS (f->follower), MOB_AMMAESTRABILE))
		{
			REMOVE_BIT (AFF_FLAGS (f->follower), AFF_CHARM);
			affect_from_char(f->follower, SPELLSKILL, SPELL_CHARM);
		}
		else if ((f->follower) && IS_NPC (f->follower) && (AFF_FLAGGED(f->follower,AFF_CHARM) || MOB_FLAGGED(f->follower, MOB_MOUNTABLE)) && IS_NPC (f->follower) && IS_SET (MOB_FLAGS (f->follower), MOB_AMMAESTRABILE) && !MOB_FLAGGED (f->follower, MOB_HARD_TAME)){
			act ("$n smette di avere fiducia in te e scappa via.", FALSE, f->follower, 0, ch, TO_VICT);
			act ("$n smette di avere fiducia in $N e scappa via.", FALSE, f->follower, 0, ch, TO_NOTVICT);
			do {
				to_room = number(0, top_of_world);
			} while (world[to_room].number < 1400400 || world[to_room].number > 1599599 || !(strcmp(world[to_room].name, "Mare di Daresia")));
			dismount_char(f->follower);
			char_from_room (f->follower);
			char_to_room (f->follower, to_room);
		}
	}

	//reliquiacamp
	if(GET_EQ(ch,WEAR_RELIQUIA)){
		obj_to_room(unequip_char(ch,WEAR_RELIQUIA),ch->in_room);
		send_to_char("Posi a terra una reliquia,durante il sonno qualcuno potrebba anche rubartela\n\r",ch);
	}
	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PRISON)) { //ardanos
		send_to_char("Ti siedi per terra, sulla paglia, e ti metti a dormire...\r\n", ch);
	}
	else
		send_to_char("Ti accampi per passare la notte all'aperto... un po' scomodo...\r\n", ch);
	
	GET_HIT(ch) = MAX(GET_HIT(ch),1);
	GET_MOVE(ch) = MAX (GET_MOVE(ch),1);
	SET_BIT(AFF_FLAGS(ch), AFF_ACCAMPATO);
	
	Crash_campsave(ch, 0);
	save_room = ch->in_room;
	extract_char(ch);
	save_char(ch, save_room);
	GET_LOADROOM(ch)=world[save_room].number;
}

void Crash_MobEqSave(struct char_data * ch, room_vnum save_room)
{
	char filenamepath[MAX_INPUT_LENGTH];
	struct rent_info rent;
	int j;
	FILE *fp;
	char filename[255];
	char numroom[255];
	
	if (!ch) return;
  /*------crash save ------*/
	sprintf(numroom, "%d", save_room);
	strcpy(filename, GET_NAME(ch));
	strcat(filename, numroom);
	strcat(filenamepath, "");
	if (!get_filename(filename, filenamepath, MOB_FILE))
	{
		sprintf(buf, "Errore nel filename! (file: %s).", filenamepath);
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
		return;}
	if (!(fp = fopen(filenamepath, "wb")))
	{
		sprintf(buf, "%s non salva il suo equipaggiamento (file: %s).", GET_NAME(ch), filenamepath);
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
		return;
	}
	rent.rentcode = RENT_CRASH;
	rent.time = time(0);
	if (!Crash_write_rentcode(ch, fp, &rent))
	{
		fclose(fp);
		sprintf(buf, "No rent info. (%s)", GET_NAME(ch) );
		mudlog(buf, NRM, MAX(LVL_IMMORT, 0), TRUE);
		
		return;
	}
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch,j))
		{
			if (!Crash_save_mob(ch, save_room, GET_EQ(ch,j), fp, j+1))
			{
				fclose(fp);
				sprintf(buf, "Don't save obj in eq. (%s)", GET_NAME(ch) );
				mudlog(buf, NRM, MAX(LVL_IMMORT, 0), TRUE);
				return;
			}
			Crash_restore_weight(GET_EQ(ch,j));
		}
		
	if (!Crash_save_mob(ch, save_room, ch->carrying, fp, 0))
	{
		fclose(fp);
		sprintf(buf, "Don't save obj carrying. (%s)", GET_NAME(ch) );
		mudlog(buf, NRM, MAX(LVL_IMMORT, 0), TRUE);
		return;
	}
	Crash_restore_weight(ch->carrying);
	fclose(fp);
  /*------crash save ------*/
}

int Crash_MobEqLoad(struct char_data * ch, room_vnum save_room)
{
	FILE *fl;
	char fname[MAX_STRING_LENGTH];
	struct obj_file_elem object;
	struct rent_info rent;
	struct obj_data *obj;
	int locate, j;
	struct obj_data *obj1;
	struct obj_data *cont_row[MAX_BAG_ROW];
	int lunghezza;
	char stringa[MAX_STRING_LENGTH];
	char filename[255];
	char numroom[255];
	
	if (!ch) return 1;
	sprintf(numroom, "%d", save_room);
	strcpy(filename, GET_NAME(ch));
	strcat(filename, numroom);
	
	if (!get_filename(filename, fname, MOB_FILE))
		return 1;
	if (!(fl = fopen(fname, "r+b"))) {
		if (errno != ENOENT) {	/* if it fails, NOT because of no file */
			sprintf(buf1, "SYSERR: READING OBJECT FILE %s (5)", fname);
			perror(buf1);
			send_to_char("\r\n********************* NOTICE *********************\r\n"
				"There was a problem loading your objects from disk.\r\n"
				"Contact a God for assistance.\r\n", ch);
		}
		sprintf(buf, "%s loading in game with no equipment.", GET_NAME(ch));
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
		return 1;
	}
	if (!feof(fl))
		fread(&rent, sizeof(struct rent_info), 1, fl);
	
	for (j = 0;j < MAX_BAG_ROW;j++)
		cont_row[j] = NULL; /* empty all cont lists (you never know ...) */
	
	while (!feof(fl))
	{
		fread(&object, sizeof(struct obj_file_elem), 1, fl);
		if (ferror(fl))
		{
			perror("Reading crash file: Crash_load.");
			fclose(fl);
			return 1;
		}
		if (!feof(fl))
		{
			obj = Obj_from_store_to(object, &locate);
			if ((obj) && (IS_OBJ_STAT(obj, ITEM_RESTRING)))
			{
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				obj->name=strdup(stringa);
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				obj->description=strdup(stringa);
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				obj->short_description=strdup(stringa);
				if ((lunghezza=fgetc(fl)) < 1)
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (!fgets(stringa, lunghezza+1, fl))
				{
					perror("Reading crash file: Crash_load.");
					fclose(fl);
					return 1;
				}
				if (str_cmp(stringa," "))
					obj->action_description=strdup(stringa);
				
			}
			
			if (obj)
			{
				auto_equip(ch, obj, &locate);
				
/*
what to do with a new loaded item:

  if there's a list with <locate> less than 1 below this:
	 (equipped items are assumed to have <locate>==0 here) then its
	 container has disappeared from the file   *gasp*
	  -> put all the list back to ch's inventory

  if there's a list of contents with <locate> 1 below this:
	 check if it's a container
	 - if so: get it from ch, fill it, and give it back to ch (this way the
		  container has its correct weight before modifying ch)
	 - if not: the container is missing -> put all the list to ch's inventory

  for items with negative <locate>:
	 if there's already a list of contents with the same <locate> put obj to it
	 if not, start a new list

Confused? Well maybe you can think of some better text to be put here ...

since <locate> for contents is < 0 the list indices are switched to
non-negative
*/
				
				if (locate > 0)
				{ /* item equipped */
					for (j = MAX_BAG_ROW-1;j > 0;j--)
						if (cont_row[j])
						{ /* no container -> back to ch's inventory */
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_char(cont_row[j], ch);
							}
							cont_row[j] = NULL;
						}
					if (cont_row[0])
					{ /* content list existing */
						if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER)
						{
			/* rem item ; fill ; equip again */
							obj = unequip_char(ch, locate-1);
							obj->contains = NULL; /* should be empty - but who knows */
							for (;cont_row[0];cont_row[0] = obj1)
							{
								obj1 = cont_row[0]->next_content;
								obj_to_obj(cont_row[0], obj);
							}
							equip_char(ch, obj, locate-1);
						}
						else
						{ /* object isn't container -> empty content list */
							for (;cont_row[0];cont_row[0] = obj1)
							{
								obj1 = cont_row[0]->next_content;
								obj_to_char(cont_row[0], ch);
							}
							cont_row[0] = NULL;
						}
					}
				}
				else
				{ /* locate <= 0 */
					for (j = MAX_BAG_ROW-1;j > -locate;j--)
						if (cont_row[j])
						{ /* no container -> back to ch's inventory */
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_char(cont_row[j], ch);
							}
							cont_row[j] = NULL;
						}
						
					if (j == -locate && cont_row[j])
					{ /* content list existing */
						if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER)
						{
			/* take item ; fill ; give to char again */
							obj_from_char(obj);
							obj->contains = NULL;
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_obj(cont_row[j], obj);
							}
							obj_to_char(obj, ch); /* add to inv first ... */
						}
						else
						{ /* object isn't container -> empty content list */
							for (;cont_row[j];cont_row[j] = obj1)
							{
								obj1 = cont_row[j]->next_content;
								obj_to_char(cont_row[j], ch);
							}
							cont_row[j] = NULL;
						}
					}
					
					if (locate < 0 && locate >= -MAX_BAG_ROW)
					{
			  /* let obj be part of content list
			but put it at the list's end thus having the items
			in the same order as before renting */
						obj_from_char(obj);
						if ((obj1 = cont_row[-locate-1]))
						{
							while (obj1->next_content)
								obj1 = obj1->next_content;
							obj1->next_content = obj;
						}
						else cont_row[-locate-1] = obj;
					}
				}
			}
		}
	}         // Fine del ciclo while
	
  /* turn this into a crash file by re-writing the control block */
	rent.rentcode = RENT_CRASH;
	rent.time = time(0);
	rewind(fl);
	Crash_write_rentcode(ch, fl, &rent);
	
	fclose(fl);
	return 0;
}
