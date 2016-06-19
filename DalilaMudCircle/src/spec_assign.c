/* ************************************************************************
*   File: spec_assign.c                                 Part of CircleMUD *
*  Usage: Functions to assign function pointers to objs/mobs/rooms        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"

extern struct room_data *world;
extern int top_of_world;
extern int mini_mud;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct char_data *mob_proto;	/* prototypes for mobs		 */

SPECIAL(postmaster);
SPECIAL(majordome);
SPECIAL(cityguard);
SPECIAL(receptionist);
SPECIAL(cryogenicist);
SPECIAL(guild_guard);
SPECIAL(guild);
SPECIAL(abil_guild);
SPECIAL(puff);
SPECIAL(fido);
SPECIAL(janitor);
SPECIAL(mayor);
SPECIAL(snake);
SPECIAL(thief);
SPECIAL(magic_user);
SPECIAL(warrior);
SPECIAL(archer);
SPECIAL(cleric);
SPECIAL(sund_earl);
SPECIAL(hangman);
SPECIAL(blinder);
SPECIAL(silktrader);
SPECIAL(butcher);
SPECIAL(idiot);
SPECIAL(athos);
SPECIAL(stu);
SPECIAL(dragon_fire);
SPECIAL(dragon_gas);
SPECIAL(dragon_frost);
SPECIAL(dragon_acid);
SPECIAL(dragon_lightning);
SPECIAL(dragon_guard);
SPECIAL(trainer);
SPECIAL(regina);
SPECIAL(horseman);
SPECIAL(dragonman);
SPECIAL(secondino);
SPECIAL(maestro_errante);
SPECIAL(maestro_santuario);
SPECIAL(manovale);
SPECIAL(vendor_manovale);
SPECIAL(oste);
SPECIAL(attack_questmob); //Orione
SPECIAL(vendor_armate);
SPECIAL(guardia_sentinel);
SPECIAL(guardia_stabber);
SPECIAL(untore);
SPECIAL(esercito);
void assign_kings_castle(void);

/* functions to perform assignments */

void ASSIGNMOB(int mob, SPECIAL(fname))
{
	if (real_mobile(mob) >= 0)
		mob_index[real_mobile(mob)].func = fname;
	else if (!mini_mud) {
		sprintf(buf, "SYSERR: Attempt to assign spec to non-existant mob #%d",
			mob);
		log(buf);
	}
}

void ASSIGNOBJ(int obj, SPECIAL(fname))
{
	if (real_object(obj) >= 0)
		obj_index[real_object(obj)].func = fname;
	else if (!mini_mud) {
		sprintf(buf, "SYSERR: Attempt to assign spec to non-existant obj #%d",
			obj);
		log(buf);
	}
}

void ASSIGNROOM(int room, SPECIAL(fname))
{
	if (real_room(room) >= 0)
		world[real_room(room)].func = fname;
	else if (!mini_mud) {
		sprintf(buf, "SYSERR: Attempt to assign spec to non-existant rm. #%d",
			room);
		log(buf);
	}
}

void ASSIGNABIL(int mob,int num_abil,int pos)
{
	int i;
	if(pos<0||pos>4){
		sprintf(buf, "SYSERR: Attempt to assign  abilita to non existant position: mob #%d", mob);
		log(buf);
		return;
	}
	if(num_abil<=0||num_abil>20){
		sprintf(buf, "SYSERR: Attempt to assign  non existant abilita to mob #%d", mob);
		log(buf);
		return;
	}
	if ((i=real_mobile(mob)) >= 0)
		mob_proto[i].char_specials.mob_abil[pos]=num_abil;
	else if (!mini_mud) {
		sprintf(buf, "SYSERR: Attempt to assign abilita to non-existant mob #%d",
			mob);
		log(buf);
	}
}

void interpret_sproc(int rnum,int nspec){
	if(nspec<=0||nspec>MAX_SPEC)
		return;
	if(rnum<0)
		return;
	switch(nspec){
		case 1:
			mob_index[rnum].func = postmaster;
			break;
		case 2:
			mob_index[rnum].func = majordome;
			break;
		case 3:
			mob_index[rnum].func = cityguard;
			break;
		case 4:
			mob_index[rnum].func = receptionist;
			break;
		case 5:
			mob_index[rnum].func = cryogenicist;
			break;
		case 6:
			mob_index[rnum].func = guild_guard;
			break;
		case 7:
			mob_index[rnum].func = guild;
			break;
		case 8:
			mob_index[rnum].func = abil_guild;
			break;
		case 9:
			mob_index[rnum].func = puff;
			break;
		case 10:
			mob_index[rnum].func = fido ;
			break;
		case 11:
			mob_index[rnum].func = janitor;
			break;
		case 12:
			mob_index[rnum].func = mayor;
			break;
		case 13:
			mob_index[rnum].func = snake;
			break;
		case 14:
			mob_index[rnum].func = thief;
			break;
		case 15:
			mob_index[rnum].func = magic_user;
			break;
		case 16:
			mob_index[rnum].func = warrior;
			break;
		case 17:
			mob_index[rnum].func = archer;
			break;
		case 18:
			mob_index[rnum].func = cleric;
			break;
		case 19:
			mob_index[rnum].func = sund_earl;
			break;
		case 20:
			mob_index[rnum].func = hangman;
			break;
		case 21:
			mob_index[rnum].func = blinder;
			break;
		case 22:
			mob_index[rnum].func = silktrader;
			break;
		case 23:
			mob_index[rnum].func = butcher;
			break;
		case 24:
			mob_index[rnum].func = idiot;
			break;
		case 25:
			mob_index[rnum].func = athos;
			break;
		case 26:
			mob_index[rnum].func = stu;
			break;
		case 27:
			mob_index[rnum].func = dragon_fire;
			break;
		case 28:
			mob_index[rnum].func = dragon_gas;
			break;
		case 29:
			mob_index[rnum].func = dragon_frost;
			break;
		case 30:
			mob_index[rnum].func = dragon_acid;
			break;
		case 31:
			mob_index[rnum].func = dragon_lightning;
			break;
		case 32:
			mob_index[rnum].func = dragon_guard;
			break;
		case 33:
			mob_index[rnum].func = trainer;
			break;
		case 34:
			mob_index[rnum].func = regina;
			break;
		case 35:
			mob_index[rnum].func = horseman;
			break;
		case 36:
			mob_index[rnum].func = dragonman;
			break;
		case 37:
			mob_index[rnum].func = secondino;
			break;
		case 38:
			mob_index[rnum].func = maestro_errante;
			break;
		case 39:
			mob_index[rnum].func = maestro_santuario;
			break;
		case 40:
			mob_index[rnum].func = manovale;
			break;
		case 41:
			mob_index[rnum].func = vendor_manovale;
			break;
		case 42:
			mob_index[rnum].func = oste;
			break;
		case 43:					//Orione
		        mob_index[rnum].func = attack_questmob;	//Orione
			break;					//Orione
		case 44:
			mob_index[rnum].func = vendor_armate;
			break;
		case 45:
			mob_index[rnum].func = guardia_sentinel;
			break;
		case 46:
			mob_index[rnum].func = guardia_stabber;
			break;
		case 47:
			mob_index[rnum].func = untore;
			break;
		case 48:
			mob_index[rnum].func = esercito;
			break;
		default:
			mob_index[rnum].func =NULL;
			return;
	}
}

int get_spec(struct char_data *mob){
	if(GET_MOB_SPEC(mob)== postmaster)
		return 1;
	else if(GET_MOB_SPEC(mob)== majordome)
		return 2;
	else if(GET_MOB_SPEC(mob)== cityguard)
		return 3;
	else if(GET_MOB_SPEC(mob)== receptionist)
		return 4;
	else if(GET_MOB_SPEC(mob)== cryogenicist)
		return 5;
	else if(GET_MOB_SPEC(mob)== guild_guard)
		return 6;
	else if(GET_MOB_SPEC(mob)== guild)
		return 7;
	else if(GET_MOB_SPEC(mob)== abil_guild)
		return 8;
	else if(GET_MOB_SPEC(mob)== puff)
		return 9;
	else if(GET_MOB_SPEC(mob)== fido)
		return 10;
	else if(GET_MOB_SPEC(mob)== janitor)
		return 11;
	else if(GET_MOB_SPEC(mob)== mayor)
		return 12;
	else if(GET_MOB_SPEC(mob)== snake)
		return 13;
	else if(GET_MOB_SPEC(mob)== thief)
		return 14;
	else if(GET_MOB_SPEC(mob)== magic_user)
		return 15;
	else if(GET_MOB_SPEC(mob)== warrior)
		return 16;
	else if(GET_MOB_SPEC(mob)== archer)
		return 17;
	else if(GET_MOB_SPEC(mob)== cleric)
		return 18;
	else if(GET_MOB_SPEC(mob)== sund_earl)
		return 19;
	else if(GET_MOB_SPEC(mob)== hangman)
		return 20;
	else if(GET_MOB_SPEC(mob)== blinder)
		return 21;
	else if(GET_MOB_SPEC(mob)== silktrader)
		return 22;
	else if(GET_MOB_SPEC(mob)== butcher)
		return 23;
	else if(GET_MOB_SPEC(mob)== idiot)
		return 24;
	else if(GET_MOB_SPEC(mob)== athos)
		return 25;
	else if(GET_MOB_SPEC(mob)== stu)
		return 26;
	else if(GET_MOB_SPEC(mob)== dragon_fire)
		return 27;
	else if(GET_MOB_SPEC(mob)== dragon_gas)
		return 28;
	else if(GET_MOB_SPEC(mob)== dragon_frost)
		return 29;
	else if(GET_MOB_SPEC(mob)== dragon_acid)
		return 30;
	else if(GET_MOB_SPEC(mob)== dragon_lightning)
		return 31;
	else if(GET_MOB_SPEC(mob)== dragon_guard)
		return 32;
	else if(GET_MOB_SPEC(mob)== trainer)
		return 33;
	else if(GET_MOB_SPEC(mob)== regina)
		return 34;
	else if(GET_MOB_SPEC(mob)== horseman)
		return 35;
	else if(GET_MOB_SPEC(mob)== dragonman)
		return 36;
	else if(GET_MOB_SPEC(mob)== secondino)
		return 37;
	else if(GET_MOB_SPEC(mob)== maestro_errante)
		return 38;
	else if(GET_MOB_SPEC(mob)== maestro_santuario)
		return 39;
	else if(GET_MOB_SPEC(mob)== manovale)
		return 40;
	else if(GET_MOB_SPEC(mob)== vendor_manovale)
		return 41;
	else if(GET_MOB_SPEC(mob)== oste)
		return 42;
	else if(GET_MOB_SPEC(mob)== attack_questmob)	//Orione
		return 43;					//Orione
	else if(GET_MOB_SPEC(mob)== vendor_armate)
		return 44;
	else if(GET_MOB_SPEC(mob)== guardia_sentinel)
		return 45;
	else if(GET_MOB_SPEC(mob)== guardia_stabber)
		return 46;
	else if(GET_MOB_SPEC(mob)== untore)
		return 47;
	else if(GET_MOB_SPEC(mob)== esercito)
		return 48;
	else
    /*GET_MOB_SPEC(mob)=NULL;*/
		return 0;
}

/* ********************************************************************
*  Assignments                                                        *
******************************************************************** */

/* assign special procedures to mobiles */
void assign_mobiles(void){
	assign_kings_castle();
  /*Maestri abilita dei santuari*/
	ASSIGNMOB(40,maestro_santuario);
	ASSIGNMOB(41,maestro_santuario);
	ASSIGNMOB(42,maestro_santuario);
	ASSIGNMOB(43,maestro_santuario);
	ASSIGNMOB(44,maestro_santuario);
	ASSIGNMOB(45,maestro_santuario);
	ASSIGNMOB(46,maestro_santuario);
	ASSIGNMOB(47,maestro_santuario);
	ASSIGNMOB(48,maestro_santuario);
	ASSIGNMOB(49,maestro_santuario);
	ASSIGNMOB(50,maestro_santuario);
	ASSIGNMOB(51,maestro_santuario);
	ASSIGNMOB(52,maestro_santuario);
	ASSIGNMOB(53,maestro_santuario);
	ASSIGNMOB(54,maestro_santuario);
	ASSIGNMOB(55,maestro_santuario);
	ASSIGNMOB(56,maestro_santuario);
  //sintassi:VNUM, N_ABIL,N_SLOT (da 0 a 5,per maestro_santuario usato slot 0)
	ASSIGNABIL(40,1,0);
	ASSIGNABIL(41,5,0);
	ASSIGNABIL(42,2,0);
	ASSIGNABIL(43,3,0);
	ASSIGNABIL(44,4,0);
	ASSIGNABIL(45,6,0);
	ASSIGNABIL(46,7,0);
	ASSIGNABIL(47,8,0);
	ASSIGNABIL(48,9,0);
	ASSIGNABIL(49,10,0);
	ASSIGNABIL(50,11,0);
	ASSIGNABIL(51,12,0);
	ASSIGNABIL(52,13,0);
	ASSIGNABIL(53,15,0);
	ASSIGNABIL(54,18,0);
	ASSIGNABIL(55,19,0);
	ASSIGNABIL(56,14,0);
}



/* assign special procedures to objects */
void assign_objects(void)
{
	SPECIAL(bank);
  /* SPECIAL(kriss); */
	SPECIAL(gen_board);
	SPECIAL(crime_board);
	SPECIAL(tokens);
	SPECIAL(portal);
	SPECIAL(marbles);
	SPECIAL(slot);
  /*ASSIGNOBJ(21011, kriss);*/
	
	ASSIGNOBJ(1249,crime_board);
	
	
	ASSIGNOBJ(73, portal);
	
	ASSIGNOBJ(3096, gen_board);	/* social board */
	ASSIGNOBJ(3098, gen_board);	/* freeze board */
	ASSIGNOBJ(9096, gen_board);   /* la bacheca sociale di cimmura */
	ASSIGNOBJ(9095, crime_board);
	ASSIGNOBJ(12556, gen_board);
	ASSIGNOBJ(12557, gen_board);
	ASSIGNOBJ(12559, gen_board);
	ASSIGNOBJ(12560, gen_board);
	ASSIGNOBJ(12561, gen_board);
	ASSIGNOBJ(12570, crime_board);
	ASSIGNOBJ(6647, marbles);
	ASSIGNOBJ(6709, marbles);
	ASSIGNOBJ(21623, gen_board);
	ASSIGNOBJ(21626, gen_board);
	ASSIGNOBJ(21627, gen_board);
	ASSIGNOBJ(21624, gen_board);
	ASSIGNOBJ(12558, bank);	/* atm */
  //ASSIGNOBJ(3036, bank);	/* cashcard */
	ASSIGNOBJ(6738, bank);
	ASSIGNOBJ(9732, crime_board);
	ASSIGNOBJ(9733, gen_board); 
	ASSIGNOBJ(8584, bank);
	ASSIGNOBJ(50045, gen_board);
}



/* assign special procedures to rooms */
void assign_rooms(void)
{
	extern int dts_are_dumps;
	int i;
	SPECIAL(dump);
	SPECIAL(pet_shops);
	SPECIAL(pray_for_items);
	SPECIAL(assassin);
	

	ASSIGNROOM(3030, dump);
	ASSIGNROOM(28072, dump);
	ASSIGNROOM(3188,pet_shops);
	ASSIGNROOM(6810,pet_shops);
	ASSIGNROOM(12581, pet_shops);
	ASSIGNROOM(21116, pet_shops);
	ASSIGNROOM(21651, pet_shops);
	ASSIGNROOM(9102, pet_shops);
	ASSIGNROOM(9830, pet_shops);
	ASSIGNROOM(50021, pet_shops); // Aldeeran 
	if (dts_are_dumps)
		for (i = 0; i < top_of_world; i++)
			if (IS_SET(ROOM_FLAGS(i), ROOM_DEATH))
				world[i].func = dump;
}


