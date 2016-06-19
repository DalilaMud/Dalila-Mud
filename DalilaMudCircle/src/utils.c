/* ************************************************************************
*   File: utils.c                                       Part of CircleMUD *
*  Usage: various internal functions of a utility nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <assert.h>

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "screen.h"
#include "spells.h"
#include "handler.h"

/* external globals */
extern struct time_data time_info;
extern struct room_data *world;
extern int sunlight;


/* restituisce x elevato alla y */
double potenza (double x, int y)
{
	int i;
	double result = 1;
	
	assert (y>=0);
	for (i=0; i<y; i++) result *= x;
	return result;
}

/* creates a random number in interval [from;to] */
int number(int from, int to)
{
  /* error checking in case people call number() incorrectly */
	if (from > to) {
		int tmp = from;
		from = to;
		to = tmp;
	}
	
	return ((random() % (to - from + 1)) + from);
}


/* simulates dice roll */
int dice(int number, int size)
{
	int sum = 0;
	
	if (size <= 0 || number <= 0)
		return 0;
	
	while (number-- > 0)
		sum += ((random() % size) + 1);
	
	return sum;
}


int MIN(int a, int b)
{
	return a < b ? a : b;
}


int MAX(int a, int b)
{
	return a > b ? a : b;
}



/* Create a duplicate of a string */
char *str_dup(const char *source)
{
	char *new_z;
	
	CREATE(new_z, char, strlen(source) + 1);
	return (strcpy(new_z, source));
}



/* str_cmp: a case-insensitive version of strcmp */
/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different or end of both                 */
int str_cmp(const char *arg1, char *arg2)
{
	int chk, i;
	
	for (i = 0; *(arg1 + i) || *(arg2 + i); i++)
		if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))){
			if (chk < 0)
				return (-1);
			else
				return (1);
		}		
	return (0);
}


/* strn_cmp: a case-insensitive version of strncmp */
/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different, end of both, or n reached     */
int strn_cmp(char *arg1, char *arg2, int n)
{
	int chk, i;
	
	for (i = 0; (*(arg1 + i) || *(arg2 + i)) && (n > 0); i++, n--)
		if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))){
			if (chk < 0)
				return (-1);
			else
				return (1);
		}
	return (0);
}


/* log a death trap hit */
void log_death_trap(struct char_data * ch)
{
	char buf[150];
	extern struct room_data *world;
	
	sprintf(buf, "%s hit death trap #%d (%s)", GET_NAME(ch),
		world[ch->in_room].number, world[ch->in_room].name);
	mudlog(buf, BRF, LVL_IMMORT, TRUE);
}


/* writes a string to the log */
void log_mud(char *str)
{
	time_t ct;
	char *tmstr;
	
	ct = time(0);
	tmstr = asctime(localtime(&ct));
	*(tmstr + strlen(tmstr) - 1) = '\0';
	fprintf(stderr, "%-19.19s :: %s\n", tmstr, str);
}


/* Phantom Log */
void phanlog(char *str)
{
	strcpy(mamba_uno, mamba_due);
	strcpy(mamba_due, mamba_tre);
	strcpy(mamba_tre, mamba_qua);
	strcpy(mamba_qua, mamba_cin);
	strcpy(mamba_cin, phantomima);
	sprintf(phantomima," %s", str);
	//fprintf(stderr, "%-19.19s :: %s\n", tmstr, str);
}

/* the "touch" command, essentially. */
int touch(char *path)
{
	FILE *fl;
	
	if (!(fl = fopen(path, "a"))) {
		perror(path);
		return -1;
	} else {
		fclose(fl);
		return 0;
	}
}


/*
 * mudlog -- log mud messages to a file & to online imm's syslogs
 * based on syslog by Fen Jul 3, 1992
 */
void mudlog(char *str, char type, int level, byte file)
{
	char buf[MAX_STRING_LENGTH];
	extern struct descriptor_data *descriptor_list;
	struct descriptor_data *i;
	char *tmp, tp;
	time_t ct;
	
	ct = time(0);
	tmp = asctime(localtime(&ct));
	
	if (file)
		fprintf(stderr, "%-19.19s :: %s\n", tmp, str);
	if (level < 0)
		return;
	
	sprintf(buf, "[ %s ]\r\n", str);
	
	for (i = descriptor_list; i; i = i->next)
		if (!i->connected && !PLR_FLAGGED(i->character, PLR_WRITING)) {
			tp = ((PRF_FLAGGED(i->character, PRF_LOG1) ? 1 : 0) +
					(PRF_FLAGGED(i->character, PRF_LOG2) ? 2 : 0));
			
			if ((GET_LEVEL(i->character) >= level) && (tp >= type)) {
				send_to_char(CCGRN(i->character, C_NRM), i->character);
				send_to_char(buf, i->character);
				send_to_char(CCNRM(i->character, C_NRM), i->character);
			}
		}
}



void sprintbit(long long int bitvector, char *names[], char *result)
{
	long long int nr;
	
	*result = '\0';
	
	if (bitvector < 0) {
		strcpy(result, "<INVALID BITVECTOR>");
		return;
	}
	for (nr = 0; bitvector; bitvector >>= 1) {
		if (IS_SET(bitvector, 1)) {
			if (*names[nr] != '\n') {
				strcat(result, names[nr]);
				strcat(result, " ");
			} else
				strcat(result, "UNDEFINED ");
		}
		if (*names[nr] != '\n')
			nr++;
	}
	
	if (!*result)
		strcpy(result, "NOBITS ");
}



void sprinttype(int type, char *names[], char *result)
{
	int nr = 0;
	
	while (type && *names[nr] != '\n') {
		type--;
		nr++;
	}
	
	if (*names[nr] != '\n')
		strcpy(result, names[nr]);
	else
		strcpy(result, "UNDEFINED");
}


/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct time_info_data real_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;
	
	secs = (long) (t2 - t1);
	
	now.hours = (secs / SECS_PER_REAL_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_REAL_HOUR * now.hours;
	
	now.day = (secs / SECS_PER_REAL_DAY);	/* 0..34 days  */
	secs -= SECS_PER_REAL_DAY * now.day;
	
	now.month = -1;
	now.year = -1;
	
	return now;
}



/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data mud_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;
	
	secs = (long) (t2 - t1);
	
	now.hours = (secs / SECS_PER_MUD_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_MUD_HOUR * now.hours;
	
	now.day = (secs / SECS_PER_MUD_DAY) % 35;	/* 0..34 days  */
	secs -= SECS_PER_MUD_DAY * now.day;
	
	now.month = (secs / SECS_PER_MUD_MONTH) % 17;	/* 0..16 months */
	secs -= SECS_PER_MUD_MONTH * now.month;
	
	now.year = (secs / SECS_PER_MUD_YEAR);	/* 0..XX? years */
	
	return now;
}



struct time_info_data age(struct char_data * ch)
{
	struct time_info_data player_age;
	
	player_age = mud_time_passed(time(0), ch->player.time.birth);
	
	player_age.year += 17;	/* All players start at 17 */
	
	return player_age;
}


/* Check if making CH follow VICTIM will create an illegal */
/* Follow "Loop/circle"                                    */
bool circle_follow(struct char_data * ch, struct char_data * victim)
{
	struct char_data *k;
	
	for (k = victim; k; k = k->master) {
		if (k == ch)
			return TRUE;
	}
	
	return FALSE;
}

/* Called when stop following persons, or stopping charm */
/* This will NOT do if a character quits/dies!!          */
void stop_follower(struct char_data * ch)
{
	struct follow_type *j, *k;	
	assert(ch->master);
	/*if ((AFF_FLAGGED(ch,AFF_CHARM) || MOB_FLAGGED(ch, MOB_MOUNTABLE)) && IS_NPC (ch) && IS_SET (MOB_FLAGS (ch), MOB_AMMAESTRABILE) && !MOB_FLAGGED (ch, MOB_HARD_TAME)){
		act ("$n smette di avere fiducia in te e scappa via.", FALSE, ch, 0, ch->master, TO_VICT);
		act ("$n smette di avere fiducia in $N e scappa via.", FALSE, ch, 0, ch->master, TO_NOTVICT);
		dismount_char(ch);
		extract_char (ch);
		return;
	}
	else */ if (IS_AFFECTED(ch, AFF_CHARM) && !MOB_FLAGGED(ch, MOB_HARD_TAME)) {
		act("Realizzi che $N non e' affidabile!", FALSE, ch, 0, ch->master, TO_CHAR);
		act("$n realizza che $N non e' affidabile!", FALSE, ch, 0, ch->master, TO_NOTVICT);
		act("$n ti odia!", FALSE, ch, 0, ch->master, TO_VICT);
		if (affected_by_spell(ch, SPELLSKILL, SPELL_CHARM))
			affect_from_char(ch, SPELLSKILL, SPELL_CHARM);
	}
	else {
		act("Smetti di seguire $N.", FALSE, ch, 0, ch->master, TO_CHAR);
		if(GET_LEVEL(ch)< LVL_IMMORT) {
			act("$n smette di seguire $N.", TRUE, ch, 0, ch->master, TO_NOTVICT);
			act("$n smette di seguirti.", TRUE, ch, 0, ch->master, TO_VICT);
		}
	}
	
	if (ch->master->followers->follower == ch) {	/* Head of follower-list? */
		k = ch->master->followers;
		ch->master->followers = k->next;
		free(k);
	} else {			/* locate follower who is not head of list */
		for (k = ch->master->followers; k->next->follower != ch; k = k->next);
		
		j = k->next;
		k->next = j->next;
		free(j);
	}
	
	if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_SAVE))
		GET_MOB_SAVED_MASTER(ch) = -1;
	ch->master = NULL;
	REMOVE_BIT(AFF_FLAGS(ch), AFF_GROUP | AFF_CHARM);
	
}



/* Called when a character that follows/is followed dies */
void die_follower(struct char_data * ch)
{
	struct follow_type *j, *k;
	
	if (ch->master)
		stop_follower(ch);
	
	for (k = ch->followers; k; k = j) {
		j = k->next;
		stop_follower(k->follower);
	}
}



/* Do NOT call this before having checked if a circle of followers */
/* will arise. CH will follow leader                               */
void add_follower(struct char_data * ch, struct char_data * leader)
{
	struct follow_type *k;
	
	assert(!ch->master);
	
	ch->master = leader;
	
	CREATE(k, struct follow_type, 1);
	
	k->follower = ch;
	k->next = leader->followers;
	leader->followers = k;
	
	act("Stai seguendo $N.", FALSE, ch, 0, leader, TO_CHAR);
	if (CAN_SEE(leader, ch))
		act("$n inizia a seguirti.", TRUE, ch, 0, leader, TO_VICT);
	act("$n inizia a seguire $N.", TRUE, ch, 0, leader, TO_NOTVICT);
}

/*
 * get_line reads the next non-blank line off of the input stream.
 * The newline character is removed from the input.  Lines which begin
 * with '*' are considered to be comments.
 *
 * Returns the number of lines advanced in the file.
 */
int get_line(FILE * fl, char *buf)
{
	char temp[2048];
	int lines = 0;
	
	do {
		lines++;
		fgets(temp, 2048, fl);
		if (*temp)
			temp[strlen(temp) - 1] = '\0';
	} while (!feof(fl) && (*temp == '*' || !*temp));
	
	if (feof(fl))
		return 0;
	else {
		strcpy(buf, temp);
		return lines;
	}
}


int get_filename(char *orig_name, char *filename, int mode)
{
	char *prefix, *middle, *suffix, *ptr, name[64];
	
	switch (mode) {
		case CRASH_FILE:
			prefix = "plrobjs";
			suffix = "objs";
			break;
		case ALIAS_FILE :
			prefix = "plralias";
			suffix = "alias";
			break;
		case ETEXT_FILE:
			prefix = "plrtext";
			suffix = "text";
			break;
		case MOB_FILE:
			prefix = "mobobjs";
			suffix = "objs";
			break;
		default:
			return 0;
			break;
	}
	
	if (!*orig_name)
		return 0;
	
	strcpy(name, orig_name);
	for (ptr = name; *ptr; ptr++)
		*ptr = LOWER(*ptr);
	
	switch (LOWER(*name)) {
		case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
			middle = "A-E";
			break;
		case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
			middle = "F-J";
			break;
		case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
			middle = "K-O";
			break;
		case 'p':  case 'q':  case 'r':  case 's':  case 't':
			middle = "P-T";
			break;
		case 'u':  case 'v':  case 'w':  case 'x':  case 'y':  case 'z':
			middle = "U-Z";
			break;
		default:
			middle = "ZZZ";
			break;
	}
	
	sprintf(filename, "%s/%s/%s.%s", prefix, middle, name, suffix);
	return 1;
}


int num_pc_in_room(struct room_data *room)
{
	int i = 0;
	struct char_data *ch;
	
	for (ch = room->people; ch != NULL; ch = ch->next_in_room)
		if (!IS_NPC(ch))
			i++;
		
	return i;
}


/* string manipulation fucntion originally by Darren Wilson */
/* (wilson@shark.cc.cc.ca.us) improved and bug fixed by Chris (zero@cnw.com) */
/* completely re-written again by M. Scott 10/15/96 (scottm@workcommn.net), */
/* substitute appearances of 'pattern' with 'replacement' in string */
/* and return the # of replacements */
int replace_str(char **string, char *pattern, char *replacement, int rep_all,
int max_size) {
	char *replace_buffer = NULL;
	char *flow, *jetsam, temp;
	int len, i;
	
	if ((strlen(*string) - strlen(pattern)) + strlen(replacement) > max_size)
		return -1;
	
	CREATE(replace_buffer, char, max_size);
	i = 0;
	jetsam = *string;
	flow = *string;
	*replace_buffer = '\0';
	if (rep_all) {
		while ((flow = (char *)strstr(flow, pattern)) != NULL) {
			i++;
			temp = *flow;
			*flow = '\0';
			if ((strlen(replace_buffer) + strlen(jetsam) + strlen(replacement)) > max_size) {
				i = -1;
				break;
			}
			strcat(replace_buffer, jetsam);
			strcat(replace_buffer, replacement);
			*flow = temp;
			flow += strlen(pattern);
			jetsam = flow;
		}
		strcat(replace_buffer, jetsam);
	}
	else {
		if ((flow = (char *)strstr(*string, pattern)) != NULL) {
			i++;
			flow += strlen(pattern);
			len = ((char *)flow - (char *)*string) - strlen(pattern);
			
			strncpy(replace_buffer, *string, len);
			strcat(replace_buffer, replacement);
			strcat(replace_buffer, flow);
		}
	}
	if (i == 0) return 0;
	if (i > 0) {
		RECREATE(*string, char, strlen(replace_buffer) + 3);
		strcpy(*string, replace_buffer);
	}
	free(replace_buffer);
	return i;
}


/* re-formats message type formatted char * */
/* (for strings edited with d->str) (mostly olc and mail)     */
void format_text(char **ptr_string, int mode, struct descriptor_data *d, int maxlen) {
	int total_chars, cap_next = TRUE, cap_next_next = FALSE;
	char *flow, *start = NULL, temp;
   /* warning: do not edit messages with max_str's of over this value */
	char formated[MAX_STRING_LENGTH];
	
	flow   = *ptr_string;
	if (!flow) return;
	
	if (IS_SET(mode, FORMAT_INDENT)) {
		strcpy(formated, "   ");
		total_chars = 3;
	}
	else {
		*formated = '\0';
		total_chars = 0;
	}
	
	while (*flow != '\0') {
		while ((*flow == '\n') ||
			(*flow == '\r') ||
			(*flow == '\f') ||
			(*flow == '\t') ||
			(*flow == '\v') ||
			(*flow == ' ')) flow++;
		
		if (*flow != '\0') {
			
			start = flow++;
			while ((*flow != '\0') &&
				(*flow != '\n') &&
				(*flow != '\r') &&
				(*flow != '\f') &&
				(*flow != '\t') &&
				(*flow != '\v') &&
				(*flow != ' ') &&
				(*flow != '.') &&
				(*flow != '?') &&
				(*flow != '!')) flow++;
			
			if (cap_next_next) {
				cap_next_next = FALSE;
				cap_next = TRUE;
			}
			
	 /* this is so that if we stopped on a sentance .. we move off the sentance delim. */
			while ((*flow == '.') || (*flow == '!') || (*flow == '?')) {
				cap_next_next = TRUE;
				flow++;
			}
			
			temp = *flow;
			*flow = '\0';
			
			if ((total_chars + strlen(start) + 1) > 79) {
				strcat(formated, "\r\n");
				total_chars = 0;
			}
			
			if (!cap_next) {
				if (total_chars > 0) {
					strcat(formated, " ");
					total_chars++;
				}
			}
			else {
				cap_next = FALSE;
				*start = UPPER(*start);
			}
			
			total_chars += strlen(start);
			strcat(formated, start);
			
			*flow = temp;
		}
		
		if (cap_next_next) {
			if ((total_chars + 3) > 79) {
				strcat(formated, "\r\n");
				total_chars = 0;
			}
			else {
				strcat(formated, "  ");
				total_chars += 2;
			}
		}
	}
	strcat(formated, "\r\n");
	
	if (strlen(formated) > maxlen) formated[maxlen] = '\0';
	RECREATE(*ptr_string, char, MIN(maxlen, strlen(formated)+3));
	strcpy(*ptr_string, formated);
}
 /* strips \r's from line */
char *stripcr(char *dest, const char *src) {
	int i, length;
	char *temp;
	
	if (!dest || !src) return NULL;
	temp = &dest[0];
	length = strlen(src);
	for (i = 0; *src && (i < length); i++, src++)
		if (*src != '\r') *(temp++) = *src;
	*temp = '\0';
	return dest;
	
}

long long int atolonglong (const char * nptr)
{
	long long int numero;
	int i=0;
	
	numero = (long long int) 0;
	while (nptr[i])
	{
		numero = numero * (long long int) 10;
		numero += (long long int) (nptr[i]-'0');
		i++;
	}
	return numero;
}


int factorizer(int num,int vector[]){
	int k=0,j=2,tmp=0,resto=0;
	tmp=resto=num;
	while(j<=resto){
		tmp=resto/j;
		if(tmp*j==resto){
			vector[k]=j;
			k++;
			resto=tmp;
		}else
			j++;
	}
	if(k==0){
		vector[k]=resto;
		return 1;
	}
	return (int) k;
}

int med_dam(int num,int vector[]){
	int i=0,j=0,k=0,dice1=0,dice2=0;
	
	i=factorizer(num,vector);
	k=i;
  //printf("\n%d e' divisibile per %d fattori\n",num,i);
  //printf("Fattori:\n");
  //for(j=0;j<k;j++)
  //printf("%d\n",vector[j]);
	if(k==0){
		return 0;
	}
	if(k==1){
		return med_dam(num+1,vector);
	}
	if(k==2){
		dice1=vector[0];
	}
	else
	if(k==3){
		dice1=vector[0];
		if(vector[2]+1>=2*vector[1])
			dice1=dice1*vector[1];
		else
			dice1=dice1*vector[2];
	}
	else
	if(k>3){
		j=number(1,k);
		dice1=vector[j-1];
		vector[j-1]=0;
		while(vector[j-1]==0)
			j=number(1,k);
		dice1=dice1*vector[j-1];
	}
  //printf("Dice2!\n");
  //printf("2*num=%d ,dice1=%d\n",2*num,dice1);
	dice2=(2*num)/dice1-1;
  //printf("\nDice1#Dice2=%d\n",dice1*100+dice2);
	return (dice1*100+dice2);
}

int newbye(struct char_data *ch) {	
	if (!ch)
		return 0;
	
	if (((ch->player.time.played / 3600)<10) && !IS_NPC(ch) && (GET_LEVEL(ch)<5) && (ch->in_room!=NOWHERE) && (!ROOM_FLAGGED(ch->in_room,ROOM_ARENA))) {
		return 1;
	} else 
		return 0;
}

char * string(char *s1,char *buf,bool flag){ // by Amanu (Corretta da Chardhros)
	int i,j;
	char *s2;
	
	if(!flag){
		s1= (char *) malloc(1);
		s1[0]= '\0';            
		return(s1);             
	}       
	i= strlen(buf);         
	j= strlen(s1)+i+1;
	s2= (char *) malloc(strlen(s1)+1);      
	strcpy(s2,s1);
	free(s1);
/* Se si alloca nuova memoria quella vecchia puntata da s1 rimane allocata in
questo modo colla free sopra si evita sprechi esponenziali di memoria */  
	s1= (char *) malloc(j);              
	strcpy(s1,s2);
	strcat(s1,buf);         
	free(s2); // Ero invena dissalocatrice :) 
/* A parte gli scherzi la strcpy alloca memoria senza dissallocarla.....famolo
:)*/     return(s1);     
}
