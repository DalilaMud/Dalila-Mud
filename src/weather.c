/* ************************************************************************
*   File: weather.c                                     Part of CircleMUD *
*  Usage: functions handling time and the weather                         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"

extern struct time_info_data time_info;

extern struct char_data *character_list; /* lightning */
extern struct room_data *world;
void die(struct char_data * ch);
void weather_and_time(int mode);
void another_hour(int mode);
void weather_change(void);
void tax_improve(void);
void arruola_truppe(void);
void stipendi_mensili(void);
void update_bilancio_regni(void);
void weather_and_time(int mode)
{
  another_hour(mode);
  if (mode)
    weather_change();
}


void another_hour(int mode)
{
  time_info.hours++;

  if (mode) {
    switch (time_info.hours) {
    case 5:
      sunlight = SUN_RISE;
      send_to_outdoor("!!SOUND(gallo.wav L=1 P=1)&1&bIl sole lentamente sorge ad est.&0\r\n");
      break;
    case 7:
      sunlight = SUN_LIGHT;
      send_to_outdoor("&4La luce di un nuovo giorno illumina la Daresia.&0\r\n");
      break;
    case 20:
      sunlight = SUN_SET;
      send_to_outdoor("&1&bIl sole lentamente tramonta ad ovest.&0\r\n");
      break;
    case 22:
      sunlight = SUN_DARK;
      send_to_outdoor("!!SOUND(gufo.wav L=1 P=1)&4&bLa notte inizia.&0\r\n");
      break;
    default:
      break;
    }
  }

  if (time_info.hours == 8 || time_info.hours == 16 || time_info.hours == 24){
		arruola_truppe();
	}
  if (time_info.hours > 23) {	/* Changed by HHS due to bug ??? */
    time_info.hours -= 24;
    time_info.day++;

    if (time_info.day > 34) {
      time_info.day = 0;
      time_info.month++;

      if(time_info.month==8||time_info.month==16) {
	//stipendi_mensili();
	//tax_improve();
	update_bilancio_regni();

      }
      if (time_info.month > 16) {
	time_info.month = 0;
	time_info.year++;
      }
    }
  }
}


void weather_change(void)
{

int diff, change= 0, i;
struct descriptor_data *j;
extern int top_of_zone_table;
extern struct zone_data *zone_table;
extern struct descriptor_data *descriptor_list;
extern struct room_data *world;

  for (i = 0; i <= top_of_zone_table; i++) {

    if ((time_info.month >= 9) && (time_info.month <= 16))
      diff = (zone_table[i].pressure > 985 ? -2 : 2);
    else
      diff = (zone_table[i].pressure > 1015 ? -2 : 2);

    zone_table[i].change += (dice(1, 4) * diff + dice(2, 6) - dice(2, 6));
    zone_table[i].change = MIN(zone_table[i].change, 12);
    zone_table[i].change = MAX(zone_table[i].change, -12);

    zone_table[i].pressure += zone_table[i].change;
    zone_table[i].pressure = MIN(zone_table[i].pressure, 1040);
    zone_table[i].pressure = MAX(zone_table[i].pressure, 960);



  switch (zone_table[i].sky) {
    case SKY_CLOUDLESS:
      if (zone_table[i].pressure < 990)
        change = 1;
      else if (zone_table[i].pressure < 1010)
        if (dice(1, 4) == 1)
  	  change = 1;
      break;
    case SKY_CLOUDY:
      if (zone_table[i].pressure < 970)
        change = 2;
      else if (zone_table[i].pressure < 990)
        if (dice(1, 4) == 1)
	  change = 2;
        else
  	  change = 0;
      else if (zone_table[i].pressure > 1030)
        if (dice(1, 4) == 1)
  	  change = 3;

      break;
    case SKY_RAINING:
      if (zone_table[i].pressure < 970)
        if (dice(1, 4) == 1)
	  change = 4;
        else
  	  change = 0;
      else if (zone_table[i].pressure > 1030)
        change = 5;
      else if (zone_table[i].pressure > 1010)
        if (dice(1, 4) == 1)
	  change = 5;

      break;
    case SKY_LIGHTNING:
      if (zone_table[i].pressure > 1010)
        change = 6;
      else if (zone_table[i].pressure > 990)
        if (dice(1, 4) == 1)
          change = 6;

      break;
    default:
      change = 0;
      zone_table[i].sky = SKY_CLOUDLESS;
      break;
    }

    switch (change) {
    case 0:
      break;
    case 1:
      sprintf(buf, "Il cielo si annuvola.\r\n");
      zone_table[i].sky = SKY_CLOUDY;
      break;
    case 2:
      sprintf(buf, "&4!!SOUND(rain2.wav L=1 P=1)Inizia a piovere.&0\r\n");
      zone_table[i].sky = SKY_RAINING;
      break;
    case 3:
      sprintf(buf, "&3Le nuvole si diradano.&0\r\n");
      zone_table[i].sky = SKY_CLOUDLESS;
      break;
    case 4:
      sprintf(buf, "!!SOUND(thundera.wav L=1 P=1)&4&bI lampi illuminano il cielo a giorno.&0\r\n");
      zone_table[i].sky = SKY_LIGHTNING;
      break;
    case 5:
      sprintf(buf, "&8Smette di piovere.&0\r\n");
      zone_table[i].sky = SKY_CLOUDY;
      break;
    case 6:
      sprintf(buf, "&2I lampi cessano.&0\r\n");
      zone_table[i].sky = SKY_RAINING;
      break;
    default:
      break;
    }

    if ((change >= 1) && (change <= 6)) {
      for (j = descriptor_list; j; j = j->next)
        if (!j->connected && j->character && AWAKE(j->character) &&
            OUTSIDE(j->character) &&
            (zone_table[world[j->character->in_room].zone].number == i))
          SEND_TO_Q(buf, j);
    }

  }
}


