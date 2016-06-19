/*****************************************************************************
*      Abilita.c -File di implemetazione delle abilita su DalilaMud.         *
*                                                                            *
*     Written by Enrico Lazzaretto.   Basato sulle idee di Offar e Blizzard  *
*     Questo file e' proprieta esclusiva dello staff di DalilaMud            *
*****************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "spells.h"
#include "interpreter.h"
#include "db.h"
//#include "olc.h"
#include "abilita.h"
#include "house.h"
#include "screen.h"

void check_trap_danno (struct char_data *ch, struct obj_data *trap);
void check_trap_stun (struct char_data *ch, struct obj_data *trap);
void check_trap_bash (struct char_data *ch, struct obj_data *trap);
void check_trap_allarme (struct char_data *ch, struct obj_data *trap, int tracce);
void piazza_trap_danno (struct char_data *ch, struct obj_data *trappola);
void piazza_trap_stun (struct char_data *ch, struct obj_data *trappola);
void piazza_trap_bash (struct char_data *ch, struct obj_data *trappola);
void piazza_trap_allarme (struct char_data *ch, struct obj_data *trappola);
int rimuovi_trappola (struct char_data *ch, struct obj_data *trappola);

/* external vars */
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct descriptor_data *descriptor_list;
extern struct abil_info_type abil_info[];
extern struct room_data *world;
extern struct house_control_rec house_control[MAX_HOUSES];

/* external func*/
extern void xp_gain(struct char_data *ch,struct char_data *victim,int missile,struct char_data *tank);
extern void die(struct char_data * ch, struct char_data *killer);
extern int get_clan_by_id(long id);
extern void set_crime(struct char_data *ch, struct char_data *vict,char *vivomorto,char *descr, int clan_num, int flag, long taglia);
extern void	store_mail(long to, long from, char *message_pointer);
extern int apply_ac(struct char_data * ch, int eq_pos);
extern char *two_arguments(char *argument, char *first_arg, char *second_arg);
extern int find_house(room_vnum vnum);
extern int newbye(struct char_data *ch);

int check_abil_weapon(struct obj_data *weapon){
	int messag=0;//,dammod=0,liv=0;
	if (weapon !=NULL ){
		messag =GET_OBJ_VAL(weapon,3)+300;
		switch( messag  ){
			case TYPE_STING: return ABIL_PUGNALE;  /*301*/
			case TYPE_SLASH: return ABIL_ASCIA;    /*303*/
			case TYPE_BLUDGEON: return ABIL_MAZZA; /*305*/
			case TYPE_CRUSH: return ABIL_MAZZA;    /*306*/
			case TYPE_POUND: return ABIL_LANCIA;   /*307*/
			case TYPE_MAUL: return ABIL_MAZZA;     /*309*/
			case TYPE_THRASH: return ABIL_SPADA;   /*310*/
			case TYPE_PIERCE: return ABIL_LANCIA;  /*311*/
			case TYPE_BLAST: return ABIL_SPADA;    /*312*/
			case TYPE_STAB: return ABIL_PUGNALE;   /*314*/
			case TYPE_SLAY: return ABIL_ASCIA;     /*315*/
			default: return 0; break;
		}
	}
	return 0;
}

int check_abil_armor(struct obj_data *armour){
	int abil=0,ac=0,wear=0;
	if (!(GET_OBJ_TYPE(armour) == ITEM_ARMOR))
		return 0;
	ac=GET_OBJ_VAL(armour, 0);
	wear=GET_OBJ_WEAR(armour);
	if(CAN_WEAR(armour, ITEM_WEAR_SHIELD))
		abil= ABIL_SCUDO;
	if(CAN_WEAR(armour, ITEM_WEAR_BODY)){ 
		if(ac <= 14)
			abil= ABIL_ARM_MEDIA;
		else  
			abil= ABIL_ARM_PESANTE;    
		if(ac <= 7) 
			abil= ABIL_ARM_BASE;
	}
	return abil;
}  

void check_abil_obj(struct char_data *ch,struct obj_data *obj,bool wear){
	int mod=0,abil=0,liv=0;
	if(IS_NPC(ch)||(!ch)||(!obj))
		return;
	switch(GET_OBJ_TYPE(obj)){
		case ITEM_WEAPON:
		case ITEM_WEAPON_2HANDS:
			if(((liv=GET_ABIL(ch,(abil=check_abil_weapon(obj))))!=0) && CAN_LEARN_ABIL(ch, abil)){
				mod=abilita_vector[abil][liv];	  
				affect_modify(ch,APPLY_DAMROLL,mod,0,0,0,0,wear);
			}
			break;
		case ITEM_ARMOR:
			if (((liv=GET_ABIL(ch,(abil=check_abil_armor(obj))))!=0) && CAN_LEARN_ABIL(ch, abil)){
				mod=abilita_vector[abil][liv];
			}
			if(abil!=ABIL_ARM_BASE&&mod!=0)
				affect_modify(ch,APPLY_AC,mod,0,0,0,0,!wear); 
			break;
		default:break;
	}
}

int get_title_abil(int liv){
	if(liv==MASTER_LIV)
		return MASTER;
	else
	if(liv>=EXPERT_LIV)
		return EXPERT;
	else
	if(liv>0)
		return BASE;
	else 
		return 0;
}

int check_extra_abil(struct char_data *ch,struct char_data *vict,int obj_type){
	int liv=0,abil=0,prob=0,ac=0,valret=0;
	float deltap = 0.0;
	struct obj_data *obj=NULL, *testa=NULL;
	
	if (obj_type <= 0){
		sprintf (buf, "Attenzione: %s cercava di decapitare con un colpo privo del campo type", GET_NAME (ch));
		mudlog (buf, NRM, LVL_GOD, TRUE);
		exit;
	}
	if (!ch || IS_NPC (ch) || vict != FIGHTING (ch) || (!IS_NPC(vict) && GET_LEVEL(vict)>=LVL_QUEST_MASTER))
		exit;

	switch(obj_type){
		case ITEM_WEAPON:/*per ora solo questo*/
		case ITEM_WEAPON_2HANDS:
			prob=number(1,1000);
			if (prob > 50) /*Un pg non puo' mai avere piu' del 5% di possibilita' di decapitare*/
				return NONDECAPITATO;
			if((obj = GET_EQ (ch,WEAR_WIELD))){/*se ha un'arma in mano insomma*/
				abil=check_abil_weapon(obj);
				liv=GET_ABIL(ch,abil);
				if (CAN_LEARN_ABIL(ch, abil)) { 
					switch(abil){
						case ABIL_MAZZA:
							if(liv==MASTER_LIV)
								if(prob<=50){
									if(!AFF_FLAGGED(vict,AFF_PARALIZE)) {
										mag_affects(70,ch,vict,SPELL_PARALIZE,SAVING_PARA);
									}
								}
							return NONDECAPITATO;
							break;

						case ABIL_ASCIA:

			/***********************************************************************\
			| Decapitazione (by Spini):						|
			| La probabilita' di decapitare un pg varia tra l'1 e il 5%,		|
			| quella di decapitare un mob tra l'1 e il 4%.				|
			| Contro i pg l'affilatura dell'ascia conta con un moltiplicatore	|
			| di 0.5, la forza del pg con 2.5, la differenza tra dex dell'at-	|
			| taccante e dex del difensore con 4, l'opposto dell'ac dato		|
			| dagli oggetti al collo del difensore con 2.5. Il valore ottenuto	|
			| viene moltiplicato per 4 e aumentato di 1. Se un dado da 1 a		|
			| 1000 e' minore del risultato (che varia tra 10 e 50), allora il	|
			| pg attaccante decapita.						|
			| Contro i mob l'affilatura conta con 1, la forza con 5.5, la		|
			| destrezza con 3.5. Il valore ottenuto viene qui moltiplicato per	|
			| 3 e aumentato di 1, poi si procede come sopra.			|
			\***********************************************************************/

							if (liv == MASTER_LIV){ /*MASTER in ASCIA*/

								if (IS_NPC (vict)){
									if(!MOB_FLAGGED(vict, MOB_NODECAPITA)) {
									  if (IS_SET (GET_OBJ_EXTRA (obj), ITEM_AFFILATO))
									    deltap += 1.0;
									  if (GET_STR (ch) == 18)
									    deltap += 5.5 * (15 + (float) GET_ADD (ch)/10) / 25;
									  else
									    deltap += 5.5 * ((float)GET_STR (ch) - 3) / 25;
									  deltap += ((float)GET_DEX (ch) - 3) / 15 * 3.5;
									  deltap = deltap * 3 + 10;
									}
								}
								else{
									if (IS_SET (GET_OBJ_EXTRA (obj), ITEM_AFFILATO))
										deltap+=0.5;
									if (GET_STR (ch) == 18){
										deltap += (15 + (float)GET_ADD (ch)/10) / 10;
}
									else{
										deltap += ((float)GET_STR (ch) - 3) / 10;
}
									deltap += (((float)GET_DEX (ch) - (float)GET_DEX (vict)) / 3 + 5) * 0.45;
									if ((GET_EQ (vict, WEAR_NECK_1)))
										ac+=apply_ac(vict, WEAR_NECK_1);
									if ((GET_EQ (vict, WEAR_NECK_2)))
										ac+=apply_ac(vict, WEAR_NECK_2);
									deltap += (float) (10 - MIN (10, ac)) / 4;
									deltap = deltap * 4 + 10;
								}

								if(prob<=deltap){
log ("Iniziamo la decapitazione...");
									if (((!PRF_FLAGGED(ch,PRF_NOKILL))&&(!IS_NPC(vict)))||(IS_NPC(vict))) {
										//Se vict e' un mob entra sempre altrimenti solo se non ce' il NOKILL
										valret = DECAPITATO;
										act("&1$n con un abile colpo fa saltare la testa di $N che cade in un lago di sangue.&0", TRUE, ch, 0, vict, TO_NOTVICT);
										act("&1$n con un abile colpo ti fa saltare la testa e per pochi istanti vedi il mondo girare!&0",TRUE, ch, 0, vict, TO_VICT);
										act("&2Dimostri la tua maestria decapitando $N, ora ti guarda da una pozza di sangue.&0", TRUE, ch, 0, vict, TO_CHAR);

										//Creazione Testa
										if (!IS_NPC(vict) && ch != vict){
											act("&1&bLa testa di $N rotola a terra lasciando una macabra scia rossa.&0", TRUE, ch, 0, vict, TO_NOTVICT);
											act("&1&bLa testa di $N rotola a terra lasciando una macabra scia rossa.&0", TRUE, ch, 0, vict, TO_CHAR);
											testa = read_object (OBJVNUM_TESTA, VIRTUAL);
											sprintf(buf,  "La testa di %s giace qui.", GET_NAME (vict));
											sprintf(buf1, "la testa di %s", GET_NAME (vict));
											sprintf(buf2, "testa %s", GET_NAME (vict));
											testa->description = str_dup (buf);
											testa->short_description = str_dup (buf1);
											testa->name = str_dup (buf2);
											obj_to_room (testa, ch->in_room);
											
										}

										xp_gain(ch,vict,FALSE,FIGHTING(vict));
										if (!IS_NPC(vict)) {
											if(ch!=vict)
												sprintf(buf2, "%s e' stato ucciso da %s a %s", GET_NAME(vict), GET_NAME(ch),world[vict->in_room].name);
											mudlog(buf2, BRF, LVL_IMMORT, TRUE);


										}
										if(IS_NPC(vict))
											if (MOB_FLAGGED(vict, MOB_ASSASSIN) && HUNTING(vict))
												REMOVE_BIT(AFF_FLAGS(HUNTING(vict)), AFF_MARK);
										if (!IS_NPC(vict)&&!ROOM_FLAGGED(ch->in_room,ROOM_ARENA))
											GET_RIP_CNT(vict) += 1;

										if (!IS_NPC(ch)&&!ROOM_FLAGGED(ch->in_room,ROOM_ARENA))
											GET_KILL_CNT(ch) += 1;

										
										sprintf(buf,"(LOG) %s ha decapitato %s",GET_NAME(ch),GET_NAME(vict));
										mudlog(buf, BRF, LVL_IMMORT, TRUE);
										die(vict, ch);
									} else {
										act("&5$n colpisce $N alla nuca con il piatto dell'ascia, tramortendolo.&0", TRUE, ch, 0, vict, TO_NOTVICT);
										act("&5Un forte colpo d'ascia alla nuca ti tramortisce. $n e' stato sin troppo magnanimo...&0",TRUE, ch, 0, vict, TO_VICT);
										act("&5Eviti di uccidere $N, colpendolo alla nuca con il piatto della tua ascia. Ora dorme sonni tranquilli.&0",TRUE, ch, 0, vict, TO_CHAR);						
										if (GET_HIT (vict) > 1)
											GET_HIT (vict) = 1;
									}

								}
							}								
							break;	
							
						default:break;
					}
				} //can abil			  
			}   
	}
	return valret;
	
}

/***********************************************\
| Spada (by Spini):				|
| Funzione chiamata in fight.c. Se ritorna 1	|
| vuol dire che il pg e' armato di spada e ha	|
| l'abilita' a 15. In questo caso guadagna un	|
| attacco bonus.				|
\***********************************************/

int check_abil_spada (struct char_data *ch)
{
	int abil;
	struct obj_data *arma;

	if (! (arma = GET_EQ (ch, WEAR_WIELD)))
		return 0;

	if (GET_OBJ_TYPE(arma) != ITEM_WEAPON)
		return 0;

	abil = check_abil_weapon (arma);

	/* Solo spada da' l'attacco supplementare */
	if (abil != ABIL_SPADA)
		return 0;

	/* Bisogna essere maestri per godere di bonus */
	if (!CAN_LEARN_ABIL (ch, abil) || GET_ABIL (ch, abil) < MASTER_LIV)
		return 0;

	/* A questo punto necessariamente il pg e' maestro di spada e ne impugna una */
	return 1;
}

int check_extra_dam(struct char_data *ch,struct char_data *vict,int obj_type){
	int liv=0,abil=0,prob=0;
	struct obj_data *obj=NULL;
	if(!ch||IS_NPC(ch) ||!vict || obj_type<=0)
		return 1;
	switch(obj_type){
		case ITEM_WEAPON:/*per ora solo questo*/
		case ITEM_WEAPON_2HANDS:
			prob=number(1,100);
			if((obj=GET_EQ(ch,WEAR_WIELD))!=NULL){/*se ha un arma in mano insomma*/
				abil=check_abil_weapon(obj);
				liv=GET_ABIL(ch,abil);
				if (CAN_LEARN_ABIL(ch, abil)) { 
					switch(abil){
						
						case ABIL_LANCIA:
							if(liv==MASTER_LIV)/*MASTER in LANCIA*/
								if(prob<=50){/*50 e' la % di DUPLICARE il Danno*/
									return DOUBLE_DAM;
								}
							break;
			   //case ABIL_PUGNALE:
				//if(liv==MASTER_LIV)/*MASTER in PUGNALE*/
				//  if(prob<=15){/*15 e' la % di TRIPLICARE il danno*/
				//  return TRIPLE_DAM;
				//  }
				//break;
							
						default:break;
					}
				} //can abil			  
			}   
	}
	return 1;
}

int check_mana_mod(struct char_data *ch,int mana,int obj_type){
	struct obj_data *obj=NULL;
	int abil=0,liv=0,percent=0;
	if(ch && !IS_NPC(ch))
		if (CAN_LEARN_ABIL(ch, abil)) {
			switch(obj_type){/*stessa struttura della precedente funzione*/
				case ITEM_ARMOR:
					if((obj=GET_EQ(ch,WEAR_BODY))){ 
						abil=check_abil_armor(obj);
						liv=GET_ABIL(ch,abil);
						switch((abil=check_abil_armor(obj))){
							case ABIL_ARM_BASE:
					/* if(liv!=0)*/
								percent=-abilita_vector[abil][liv];
								break;
					/*in effetti e'X mille*/
							case ABIL_ARM_MEDIA:
								percent=(15-get_title_abil(liv)*5)*10;
								break;
							case ABIL_ARM_PESANTE:
								percent=(25-get_title_abil(liv)*10)*10;
								break;		    
							default:
								percent=0;
								break;			
						}
					} 
					mana=percent*mana/1000;
					return mana; /*quantita di mana da aggiungere a quella gia spesa*/	
					break;
		/*add here new Item Case*/
				default:
					break;
			}
		} //can abil		
	return 0;
}

int check_gen_abil(struct char_data *ch,int abilita){
	if(!ch || IS_NPC(ch) ||abilita<=0||abilita>=TOP_ABILITA_DEFINE || !CAN_LEARN_ABIL(ch, abilita))
		return 0;
	else
		return (int)abilita_vector[abilita][(int)GET_ABIL(ch,abilita)];
} 

bool check_second_weapon(struct char_data *ch,struct obj_data *l_wielded){
	int abil=0;
	struct obj_data *r_wielded=NULL;
	
	if(!ch||IS_NPC(ch)||!l_wielded)
		return FALSE;
	r_wielded=GET_EQ(ch,WEAR_WIELD);
	abil=check_abil_weapon(l_wielded);
	if (CAN_LEARN_ABIL(ch, abil)){
		switch(abil){
			case ABIL_PUGNALE:
			case ABIL_SPADA:
				if(abil!=check_abil_weapon(r_wielded))
					return FALSE;
				if(GET_ABIL(ch,abil)!=MASTER_LIV)
					return FALSE;
				break;
			default:return FALSE;
		}
	}	  
	return TRUE;
}

	/*******************************************************\
	| Ritorna il valore per cui deve essere moltiplicato il |
	| danno (fisico) in caso la vittima abbia maglia o 	|
	| piastre a 15 e indossi l'armatura appropriata. Spini	|
	\*******************************************************/
float abil_armatura (struct char_data *ch, int attacktype)
{
	int abil, ac;
	struct obj_data *armour;

	if (attacktype < MAX_SPELLS || (attacktype > SPELL_IDENTIFY && attacktype < TOP_SPELL_DEFINE) || attacktype >= TYPE_HUNGER)
		return 1;

	if (! (armour = GET_EQ (ch, WEAR_BODY)))
		return 1;
	abil = check_abil_armor (armour);
	ac = GET_OBJ_VAL(armour, 0);

	if (!(GET_OBJ_TYPE(armour) == ITEM_ARMOR))
		return 1;

	/* Bisogna essere maestri per godere di bonus */
	if (!CAN_LEARN_ABIL (ch, abil) || GET_ABIL (ch, abil) < MASTER_LIV)
		return 1;

	/* Cuoio non da' alcuna protezione */
	if (abil == ABIL_ARM_BASE)
		return 1;

	/* Maglia dà il 7 + (ac - 8) % = (ac - 1) % di riduzione del danno */
	else if (abil == ABIL_ARM_MEDIA)
		return ((float) (101 - ac) / 100);

	/* Piastre dà il 19 + (ac - 15) % = (ac + 4) % di riduzione del danno */
	return ((float) (96 - ac) / 100);
}

/*******************************************************\
| Scudo: para da un minimo di 14 colpi su 100 a	un	|
| massimo di 43 colpi su 100, in base all'ac dello	|
| scudo e alla dex del difensore. Se il colpo viene	|
| parato lo scudo viene rotto con una percentuale dello	|
| 0.5 % se l'attaccante e' un mob, variabile da 0 a	|
| 1.5 % (in base a forza e tipo di arma) se questi e'	|
| pg.							|
\*******************************************************/

int abil_scudo (struct char_data *ch, struct char_data *vict)
{
	int perc, prob, wtype, ac;
	struct obj_data *scudo, *arma;

	if (!(scudo = GET_EQ (vict, WEAR_SHIELD)))
		return 0;
	
	if (GET_OBJ_TYPE (scudo) != ITEM_ARMOR)
		return 0;

	if (!CAN_LEARN_ABIL (vict, ABIL_SCUDO) || GET_ABIL (vict, ABIL_SCUDO) < MASTER_LIV)
		return 0;

	if (!(arma = GET_EQ (ch, WEAR_WIELD)))
		wtype = 0;
	else
		wtype = check_abil_weapon (arma);

	ac = GET_OBJ_VAL (scudo, 0);
	prob = 10 + GET_DEX (vict) + ac;
	perc = number (1, 100);

	if (perc <= prob){
		prob = 6;
		if (!IS_NPC (ch)){
			if (GET_STR (ch) > 14)
				prob++;
			if (GET_STR (ch) == 18 && GET_ADD (ch) > 20)
				prob++;
			if (GET_STR (ch) == 18 && GET_ADD (ch) > 60)
				prob++;
			if (GET_STR (ch) == 18 && GET_ADD (ch) == 100)
				prob++;
	
			if (wtype == 0 || wtype == ABIL_PUGNALE)
				prob = 0;
			else if (wtype == ABIL_SPADA)
				prob++;
			else if (wtype == ABIL_LANCIA || wtype == ABIL_ASCIA)
				prob += 3;
			else if (wtype == ABIL_MAZZA)
				prob += 5;
		}
		else
			prob = 5;

		perc = number (1, 1000);
		if (perc <= prob){
			act ("&7&bIl colpo di $n e' talmente potente da mandare il tuo scudo in frantumi!!&0", FALSE, ch, 0, vict, TO_VICT);
			act ("&7&bIl possente colpo di $n frantuma lo scudo di $N!&0", FALSE, ch, 0, vict, TO_NOTVICT);
			act ("&7&bCon il tuo possente colpo frantumi lo scudo di $N!&0", FALSE, ch, 0, vict, TO_CHAR);
			extract_obj (scudo);
		}
		return 1;
	}
	return 0;
}

bool check_hold_obj(struct char_data *ch,struct obj_data *hold){
	
  /*routine che blocca l'uso di ogg WEAR_LIGHT WEAR_HOLD e WEAR_WIELD_L */
	
	if(CAN_WEAR(hold,ITEM_WEAR_HOLD))
		if(GET_EQ(ch,WEAR_HOLD)||GET_EQ(ch,WEAR_LIGHT)||GET_EQ(ch,WEAR_WIELD_L)){
			send_to_char("Non Puoi impugnare cosi tanti oggetti!Hai per caso tre mani?\n",ch);
			return FALSE;
		}
	if(CAN_WEAR(hold,ITEM_WEAR_SHIELD)&&GET_EQ(ch,WEAR_WIELD_L)){
		send_to_char("Non puoi usare uno scudo con 2 armi gia impugnate.\n",ch);
		return FALSE;
	}
	if(CAN_WEAR(hold,ITEM_WEAR_WIELD)&&GET_EQ(ch,WEAR_WIELD)&&(GET_EQ(ch,WEAR_SHIELD)||GET_EQ(ch,WEAR_HOLD))){
		send_to_char("Devi toglierti lo scudo o liberare una mano\n per poter impugnare una seconda arma.\n",ch);
		return FALSE;
	}
	if(CAN_WEAR(hold,ITEM_WEAR_WIELD)&&GET_EQ(ch,WEAR_HOLD)&&GET_EQ(ch,WEAR_LIGHT)){
		send_to_char("Hai gia tutte e due le mani impegnate!",ch);
		return FALSE;
	}
	return TRUE;
}


bool check_free_hands(struct char_data *ch,struct obj_data *obj){
	int used_hands=0,i;
	int vector[6]={WEAR_WIELD,WEAR_WIELD_L,WEAR_HOLD,WEAR_LIGHT,WEAR_SHIELD,WEAR_HANG};
	if(CAN_WEAR(obj,ITEM_WEAR_HOLD)||CAN_WEAR(obj,ITEM_WEAR_SHIELD)||
		CAN_WEAR(obj,ITEM_WEAR_WIELD)||CAN_WEAR(obj,ITEM_WEAR_HANG)||
		(GET_OBJ_TYPE(obj)==ITEM_LIGHT)){/*se e' un ogg che occupa le mani*/
		for(i=0;i<5;i++){
			if(GET_EQ(ch,vector[i]))
				used_hands++;
		}
		if(GET_EQ(ch,vector[5]))
			used_hands+=2;
		if(used_hands<2)
			return TRUE;
		else {
			send_to_char("Non Puoi impugnare cosi tanti oggetti!Hai per caso tre mani?\n",ch);
			return FALSE;
		} 
	}
	return TRUE;
}      

void update_abilita(struct char_data *ch,int abilita,int new_liv){
	int prec_liv,i;
	struct obj_data *obj=NULL;
	
	if (CAN_LEARN_ABIL(ch, abilita)) {
		prec_liv=GET_ABIL(ch,abilita);
		if( abilita<=5 )
			obj=GET_EQ(ch,WEAR_WIELD);
		else
		if( abilita==6)
			obj=GET_EQ(ch,WEAR_SHIELD);
		else
		if( abilita<=9)
			obj=GET_EQ(ch,WEAR_BODY);
		if(abilita <=9)
			check_abil_obj(ch,obj,FALSE);
		SET_ABIL(ch,abilita,new_liv);
		if(abilita <=9)
			check_abil_obj(ch,obj,TRUE);
		if(abilita==ABIL_MEDITAZIONE){
			if(prec_liv<=new_liv){
				for(i=prec_liv+1;i<=new_liv;i++)
					GET_MAX_MANA(ch)+=abilita_vector[abilita][i];
				send_to_char("&5Aumenti&0 la tua &1&bMana&0.\n",ch);
			}
			else{
				for(i=new_liv+1;i<=prec_liv;i++)
					GET_MAX_MANA(ch)-=abilita_vector[abilita][i];
				send_to_char("&4Diminuisci&0 la tua &1&bMana&0.\n",ch);
			}
		}
		
		if(abilita==ABIL_BODYBUILDING){
			if(prec_liv<=new_liv){
				for(i=prec_liv+1;i<=new_liv;i++)
					GET_MAX_HIT(ch)+=abilita_vector[abilita][i];
				send_to_char("&5Aumenti&0 i tuoi &1&bHP&0.\n",ch);
				if (new_liv == MASTER_LIV){
					if (ch->real_abils.str <= 16)
						ch->real_abils.str += 2;
					else if (ch->real_abils.str == 17){
						ch->real_abils.str = 18;
						ch->real_abils.str_add = 10;
					} else if (ch->real_abils.str_add <= 80)
						ch->real_abils.str_add += 20;
					else
						ch->real_abils.str_add = 100;
					send_to_char("&3&bTi senti piu' potente.&0\n", ch);
					save_char(ch, ch->in_room);
					Crash_crashsave(ch);
				}
			}
			else{
				for(i=new_liv+1;i<=prec_liv;i++)
					GET_MAX_HIT(ch)-=abilita_vector[abilita][i];
				send_to_char("&4Diminuisci&0 i tuoi &1&bHP&0.\n",ch);
				if (prec_liv == MASTER_LIV){
					if (ch->real_abils.str < 18 || ch->real_abils.str_add == 0)
						ch->real_abils.str -= 2;
					else if (ch->real_abils.str_add == 10){
						ch->real_abils.str_add = 0;
						ch->real_abils.str = 17;
					} else
						ch->real_abils.str_add -= 20;
					ch->real_abils.str = MAX (3, ch->real_abils.str);
					send_to_char("&3&bTi senti meno potente.\n", ch);
					save_char(ch, ch->in_room);
					Crash_crashsave(ch);
				}
			}
		}
	}	  
} 


/*******************************************************\
| Tornado (by Spini):					|
| Legata all'abilita' lancia. Chi impugna una lancia ed	|
| ha l'abilita' a 15 puo', ogni 5 minuti, lanciare un	|
| tornado contro l'avversario che, nell'80% dei casi,	|
| lo stordisce per 3 round.				|
\*******************************************************/

ACMD (do_tornado)
{
	int abil;
	struct obj_data *arma;
	struct char_data *vict;

	if (! (arma = GET_EQ (ch, WEAR_WIELD))){
		send_to_char ("Non puoi farlo da disarmato!\r\n", ch);
		return;
	}

	if (GET_OBJ_TYPE(arma) != ITEM_WEAPON){
		send_to_char ("Attenzione - stai impugnando un oggetto che non e' un'arma, riportalo ai creatori\r\n", ch);
		return;
	}

	abil = check_abil_weapon (arma);

	/* Solo spada da' l'attacco supplementare */
	if (abil != ABIL_LANCIA){
		send_to_char ("Puoi fare questo attacco solo impugnando una lancia!\r\n", ch);
		return;
	}

	/* Bisogna essere maestri per godere di bonus */
	if (!CAN_LEARN_ABIL (ch, abil) || GET_ABIL (ch, abil) < MASTER_LIV){
		send_to_char ("Devi essere maestro di lancia per poter utilizzare questo attacco.\r\n", ch);
		return;
	}

	/* A questo punto necessariamente il pg e' maestro di lancia e ne impugna una */

	one_argument (argument, buf);
	if (!*buf){
		send_to_char ("CHI vuoi colpire con il tuo tornado?\r\n", ch);
		return;
	}

	if(AFF_FLAGGED(ch,AFF_IMMOBIL))
	{
		send_to_char("Sei legato!!\r\n",ch);
		return;
	}
	if(TRANSPORTED_BY(ch))
	{
		send_to_char("Devi prima scendere.\r\n",ch);
		return;
	}
	if (!(vict = get_char_room_vis(ch, buf))) {
		if (FIGHTING(ch)) {
			vict = FIGHTING(ch);
		} else {
			send_to_char("Non vedi nessuno con quel nome qui.\r\n", ch);
			return;
		}
	}

	if (newbye(vict)) {
		send_to_char("Non puoi far del male ad un Newbye!\r\n",ch);
		return;
	}
	if(PLR_FLAGGED(vict,PLR_FANTASMA) && !IS_NPC(vict) && GET_LEVEL(ch)<LVL_AVATAR){
		send_to_char("Non e' logico colpire un fantasma...non trovi?!?\r\n",ch);
		return;
	}
	if (vict == ch) {
		send_to_char("Ma come siamo spiritosi oggi...\r\n", ch);
		return;
	}
	if (MOB_FLAGGED(vict, MOB_SELVAGGINA) && (!MOB_FLAGGED(vict, MOB_HARD_TAME))) {
		act("Prova a cacciarlo.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}
	if (!can_do_event (ch, LAST_TORNADO, 300)){
		send_to_char ("La tua lancia ha bisogno di tempo prima di poter liberare nuovamente il suo potere\r\n", ch);
		return;
	}

	if(!IS_NPC(ch))
		if(GET_POS(ch)==POS_STANDING)
			GET_DURATA_COMB(ch)=0;

	if (controllo_scontro (ch, vict))
		return;

	if (number (1, 10) <= 8){
		act ("&7Il tornado generato dalla tua lancia investe $N impedendogli di controllare i suoi arti!&0", FALSE, ch, 0, vict, TO_CHAR);
		act ("&7$n agita la sua lancia creando un piccolo tornado che ti investe. Per un po' non puoi piu' controllare i tuoi arti.&0", FALSE, ch, 0, vict, TO_VICT);
		act ("&7$n agita la sua lancia creando un piccolo tornado che investe $N.&0", FALSE, ch, 0, vict, TO_NOTVICT);
		WAIT_STATE (vict, PULSE_VIOLENCE * 3);
	}
	else{
		act ("&7&bIl tornado generato dalla tua lancia manca miserabilmente $N!&0", FALSE, ch, 0, vict, TO_CHAR);
		act ("&7&bIl tornado generato dalla lancia di $n si allontana sollevando soltando qualche nube di polvere.&0", FALSE, ch, 0, vict, TO_ROOM);
	}
	WAIT_STATE (ch, PULSE_VIOLENCE);
}

struct obj_data *find_trappola(struct char_data *ch) {
	
	struct obj_data *i;
	extern int newbye(struct char_data *ch); 
	
	if (newbye(ch)) return NULL;
	
	
	for (i=world[ch->in_room].contents; i!=NULL ; i = i->next_content) {
		if ((GET_OBJ_TYPE(i)==ITEM_TRAPPOLA)&&(GET_TRAP_TIPO(i) != TRAP_VUOTA))
			return i;	
	}	
	return NULL;
}

#define TIMER_ALLARME 2300

int check_ladro(struct char_data *ch) {
	int h,j;
	
	if ((h = find_house(world[ch->in_room].number)) < 0)
		send_to_char("Um.. questa casa sembra non funzionare.\r\n", ch);
	else {
		if (GET_IDNUM(ch) == house_control[h].owner)
			return FALSE; // Padrone di Casa
		for (j = 0; j < MIN(house_control[h].num_of_guests, MAX_GUESTS); j++) {
			if(GET_IDNUM(ch)==(house_control[h].guests[j]))
				return FALSE; // Ospite Autorizzato
		}
	}
	return TRUE; // Ladro
}

void check_trappola(struct char_data *ch) 
{
	
	struct obj_data *list=world[ch->in_room].contents;
	struct obj_data *i;
	int perc,bonuspg;

	for (i = list; i != NULL ; i = i->next_content) {
		if (GET_OBJ_TYPE(i) == ITEM_TRAPPOLA) {
			
			if (GET_TRAP_TIPO(i) == TRAP_ALLARME && !check_ladro(ch)) 
				return; // Non e' un ladro

			perc=number(0,100);
			
 				/* Bonus per le caratteristiche */
			bonuspg=((GET_INT(ch)+GET_WIS(ch)+GET_DEX(ch))/3)-13;
			if (bonuspg>0) 
				perc+=bonuspg*10;
			
 				/* Bonus per l'abilita' */
			if (GET_ABIL(ch, ABIL_TRAPPOLE) == MASTER_LIV)
				perc+=30;
			else if (GET_ABIL(ch,ABIL_TRAPPOLE) >= EXPERT_LIV)
				perc+=20;
			else if (GET_ABIL(ch,ABIL_TRAPPOLE) > 0)
				perc+=10;
			
			if (GET_TRAP_PERC(i)>perc) {
				if (GET_TRAP_TIPO(i) == TRAP_DANNO) {
					check_trap_danno (ch, i);
					return;
				} 
				else if (GET_TRAP_TIPO(i)==TRAP_IMMOBILIZZA) {
					check_trap_stun (ch, i);
					return;
				} 

				else if (GET_TRAP_TIPO(i)==TRAP_BASH) {
					check_trap_bash (ch, i);
					return;
				} 

				else if (GET_TRAP_TIPO(i)==TRAP_ALLARME) {
					check_trap_allarme (ch, i, TRUE);
					return;
				}
			} 
			else if (GET_TRAP_MISS(i) > number (0, 100) && GET_TRAP_TIPO(i) != TRAP_SCATTATA){
				if (GET_TRAP_TIPO(i) == TRAP_ALLARME)
					check_trap_allarme (ch, i, FALSE); 
				else
					extract_obj(i); 
				
				if (ch->desc)
					act("&3Fai scattare una trappola, ma fortunatamente si inceppa!&0",TRUE,ch,0,0,TO_CHAR);
				act("&3$n fa scattare una trappola, ma fortunatamente si inceppa!&0",TRUE,ch,0,0,TO_ROOM);
				return;			
			}
			return;	
		}
	} 
}

void check_trap_danno (struct char_data *ch, struct obj_data *trap)
{
	int hit;
	hit = number(GET_TRAP_MIND(trap), GET_TRAP_MAXD(trap));
	damage(ch, ch, hit, TYPE_TRAPPOLE);
	extract_obj(trap); //Elimina la trappola
}

void check_trap_stun (struct char_data *ch, struct obj_data *trap)
{
	act("&6Una trappola si chiude sul tuo piede iniettandoti del veleno...ti senti stordito..&0",TRUE,ch,0,0,TO_CHAR);
	act("&6Una trappola si chiude sul piede di $n iniettandogli del veleno. Rimane stordito.&0",TRUE,ch,0,0,TO_ROOM);

	switch (GET_TRAP_LEVEL(trap)){
		case 8:
		case 9:
		case 10:
		case 11:
			WAIT_STATE(ch, PULSE_VIOLENCE*1);
			break;
		case 12:
		case 13:
		case 14:
			WAIT_STATE(ch, PULSE_VIOLENCE*2);
			break;
		case 15:
			WAIT_STATE(ch, PULSE_VIOLENCE*3);
			break;
	}
	extract_obj(trap); //Elimina la trappola
}

void check_trap_bash (struct char_data *ch, struct obj_data *trap)
{
	act("&1Improvvisamente metti il piedi su qualcosa di scivoloso che ti manda gambe all'aria!&0",TRUE,ch,0,0,TO_CHAR);
	act("&1$n mette il piede su qualcosa di scivoloso e fa un capitombolo finendo gambe all'aria!&0",TRUE,ch,0,0,TO_ROOM);
	dismount_char(ch);
	GET_POS(ch) = POS_SITTING;
	WAIT_STATE(ch, PULSE_VIOLENCE*2);
	extract_obj(trap); //Elimina la trappola
}

void check_trap_allarme (struct char_data *ch, struct obj_data *trap, int tracce)
{
	int h;
	
	if (tracce == TRUE){
		sprintf (buf, "Una trappola fatta scattare da %s.", GET_NAME(ch));
		sprintf (buf2, "Sono state rilevate tracce di furto con scasso nella tua casa %s,\r\n"
		"%s e' stato trovato sul luogo del furto.\r\n",world[ch->in_room].name, GET_NAME(ch));
	}
	else{
		sprintf (buf, "Una trappola fatta scattare da qualcuno.");
		sprintf (buf2, "Sono state rilevate tracce di furto con scasso nella tua casa %s.\r\n",world[ch->in_room].name);
	}
					
	//Aggiunge nome ladro alla Trappola!
	GET_OBJ_TIMER(trap) = TIMER_ALLARME;
	GET_TRAP_TIPO(trap) = TRAP_SCATTATA;
	trap->description = str_dup(buf);
	SET_BIT(GET_OBJ_EXTRA(trap),ITEM_RESTRING);
		
	//Spedisce la mail al padrone di casa
	h = find_house(world[ch->in_room].number);
	store_mail(house_control[h].owner, MAIL_GUARDIE,buf2);
}

ACMD(do_trappola) {
	struct obj_data *trappola;
	
	if(GET_ABIL(ch,ABIL_TRAPPOLE)<=0) {
		send_to_char("Non sai nulla dell'abilita' trappole\n\r",ch);
		return;
	}
	
	two_arguments(argument, arg, buf2);
	
	if (!*arg) {
		send_to_char("Comandi Trappola:\n\r",ch); 
		send_to_char("Trappola danno <livello>\n\r",ch);
		send_to_char("Trappola allarme\n\r",ch);
 		send_to_char("Trappola stordente\n\r",ch);
		send_to_char("Trappola rimuovi\n\r",ch);
		send_to_char("Trappola scivolosa\n\r",ch);
		return;
	}
	
	trappola = find_trappola(ch);
	if (trappola) {
		
		if (is_abbrev(arg,"rimuovi")) {
			rimuovi_trappola(ch, trappola);
			return;
		}
		send_to_char("C'e' gia' una trappola, puoi solo rimuoverla!\r\n",ch);
		return;
	} 
	else {
		if (is_abbrev(arg,"rimuovi")) {
			send_to_char("Non c'e' nessuna trappola da rimuovere!\r\n",ch);
			return;
		}	
	}
	
	if (is_abbrev(arg,"danno") || is_abbrev(arg,"allarme") || is_abbrev(arg,"stordente") || is_abbrev(arg,"scivolosa") ) {
		
		if (!(trappola = GET_EQ(ch, WEAR_HOLD))) {
			send_to_char("Non stai tenendo una trappola!\r\n",ch);
			return;
		}
		
		if(GET_OBJ_TYPE(trappola) != ITEM_TRAPPOLA) {
			send_to_char("Non stai tenendo una trappola!\r\n",ch);
			return;
		}
		
		if (is_abbrev(arg,"danno")) {
			piazza_trap_danno (ch, trappola);
		}

		else if (is_abbrev(arg,"stordente")) {
			piazza_trap_stun (ch, trappola);
		}

		else if (is_abbrev(arg,"scivolosa")) {
			piazza_trap_bash (ch, trappola);
		}

		else if (is_abbrev(arg,"allarme")) {
			piazza_trap_allarme (ch, trappola);
		}
	} else
		send_to_char("Tipo di trappola ancora da definire!\n\r",ch);

}

int rimuovi_trappola (struct char_data *ch, struct obj_data *trappola)
{
	int h, j, perc;

	perc=number(0,100);
	
	if(GET_LEVEL(ch) > 70) {
		store_mail(get_id_by_name("ardanos"), GET_IDNUM(ch), "Ho cercato di rimuover trappole!");
	}
	
	if (GET_TRAP_TIPO(trappola) == TRAP_VUOTA) {
		send_to_char("Non c'e' nessuna trappola da rimuovere!\r\n",ch);
		return 0;
	}
	
	if (GET_TRAP_TIPO(trappola) == TRAP_SCATTATA) {
		if ((h = find_house(world[ch->in_room].number)) < 0) {
			send_to_char("Um.. questa casa sembra non funzionare.\r\n", ch);
			return 0;
		}
		else {
			if (GET_IDNUM(ch) == house_control[h].owner) {
				act("Rimuovi la trappola.",TRUE,ch,0,0,TO_CHAR);
				sprintf(buf,"$n rimuove la trappola.");
				act(buf,TRUE,ch,0,0,TO_ROOM);
				extract_obj(trappola);
				return 1; // Padrone di Casa
			}
			for (j = 0; j < MIN(house_control[h].num_of_guests, MAX_GUESTS); j++) {
				if((GET_IDNUM(ch)==(house_control[h].guests[j]))) {
					act("Rimuovi la trappola.",TRUE,ch,0,0,TO_CHAR);
					sprintf(buf,"$n rimuove la trappola.");
					act(buf,TRUE,ch,0,0,TO_ROOM);
					extract_obj(trappola);
					return 1; // Ospite Autorizzato
				}
			}
		}
		send_to_char("Non puoi rimuovere le prove....hehehehehehe!\r\n",ch);
		return 0;
	}
			
 	/* Bonus per il livello di abilita' */
	if (GET_ABIL(ch,ABIL_TRAPPOLE)==MASTER_LIV)
		perc-=50;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>=EXPERT_LIV)
		perc-=25;
	
	if (perc<GET_TRAP_MISS(trappola)) {	// Trappola rimossa			
		act("Rimuovi la trappola.",TRUE,ch,0,0,TO_CHAR);
		sprintf(buf,"$n rimuove la trappola.");
		act(buf,TRUE,ch,0,0,TO_ROOM);
		extract_obj(trappola);
		return 1;
	} 
	else {                                // Trappola scattata
		if (GET_TRAP_TIPO(trappola)==TRAP_DANNO) {				
			check_trap_danno(ch, trappola);
			return 0;
		} else if (GET_TRAP_TIPO(trappola)==TRAP_IMMOBILIZZA) {
			check_trap_stun(ch, trappola);
			return 0;
		} else if (GET_TRAP_TIPO(trappola)==TRAP_BASH) {
			check_trap_bash(ch, trappola);
			return 0;
		} else if (GET_TRAP_TIPO(trappola)==TRAP_ALLARME) {
			check_trap_allarme(ch, trappola, TRUE);
			return 0;
		}
	}
	return -1;
}

void piazza_trap_danno (struct char_data *ch, struct obj_data *trappola)
{
	int level;
	if (!*buf2)
		level = GET_ABIL(ch, ABIL_TRAPPOLE);
	else
		level=atoi(buf2);
	
	if (level > GET_ABIL(ch, ABIL_TRAPPOLE)) {
		send_to_char("Non puoi costruire trappole cosi' potenti!\r\n",ch);
		return;
	}
	
	GET_OBJ_WEAR(trappola) = 0;
	SET_BIT(GET_OBJ_EXTRA(trappola), ITEM_RESTRING);
	GET_OBJ_WEIGHT(trappola) = 1;
	GET_OBJ_CSLOTS(trappola) = 1;
	GET_OBJ_TSLOTS(trappola) = 1;
	GET_TRAP_TIPO(trappola) = TRAP_DANNO;
	GET_TRAP_LEVEL(trappola) = GET_ABIL(ch,ABIL_TRAPPOLE);
	
	/* Percentuale di Innesco con Danno */
	GET_TRAP_PERC(trappola) = ((GET_DEX(ch)+GET_INT(ch))/5) * 8;
	
	/* Bonus Innesco con Danno */
	if (GET_ABIL(ch,ABIL_TRAPPOLE) == MASTER_LIV)
		GET_TRAP_PERC(trappola) += 30;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE) >= EXPERT_LIV)
		GET_TRAP_PERC(trappola) += 20;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>0)
		GET_TRAP_PERC(trappola) += 10;
	
	/* Percentuale di Innesco senza Danno */
	if (GET_ABIL(ch,ABIL_TRAPPOLE)==MASTER_LIV)
		GET_TRAP_MISS(trappola) = 0;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>=EXPERT_LIV)
		GET_TRAP_MISS(trappola) = 50;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>0)
		GET_TRAP_MISS(trappola) = 100;
	
	GET_TRAP_MIND(trappola) = (level*10)-9;
	GET_TRAP_MAXD(trappola) = (level*10);				
	act("$n costriusce una trappola.", TRUE, ch, 0, 0, TO_ROOM);
	act("Costruisci una trappola.", TRUE, ch, 0, 0, TO_CHAR);
	obj_to_char(unequip_char(ch, WEAR_HOLD), ch);	
	obj_from_char(trappola);
	obj_to_room(trappola, ch->in_room);
	
	if(GET_LEVEL(ch) > 70) {
		store_mail(get_id_by_name("ardanos"), GET_IDNUM(ch), "Ho cercato di metter una trappola danno, essendo WIZ!");
	}
}

void piazza_trap_stun (struct char_data *ch, struct obj_data *trappola)
{
	int level;

	level = GET_ABIL(ch, ABIL_TRAPPOLE);
	if (level<EXPERT_LIV) {
		send_to_char("Non puoi costruire ancora questo tipo di trappola!\r\n",ch);
		return;
	}

	GET_OBJ_WEAR(trappola) = 0;
	SET_BIT(GET_OBJ_EXTRA(trappola),ITEM_RESTRING);
	GET_OBJ_WEIGHT(trappola) = 1;
	GET_OBJ_CSLOTS(trappola) = 1;
	GET_OBJ_TSLOTS(trappola) = 1;
	GET_TRAP_TIPO(trappola) = TRAP_IMMOBILIZZA;
	GET_TRAP_LEVEL(trappola) = GET_ABIL(ch,ABIL_TRAPPOLE);
	
	/* Percentuale di Innesco con Stun*/
	GET_TRAP_PERC(trappola) = ((GET_DEX(ch)+GET_INT(ch))/5)*10;
	
	/* Bonus Innesco */
	if (GET_ABIL(ch,ABIL_TRAPPOLE)==MASTER_LIV)
		GET_TRAP_PERC(trappola) += 30;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>=EXPERT_LIV)
		GET_TRAP_PERC(trappola) += 20;
	
	/* Percentuale di Innesco senza Stun */
	if (GET_ABIL(ch,ABIL_TRAPPOLE)==MASTER_LIV)
		GET_TRAP_MISS(trappola) = 0;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>=EXPERT_LIV)
		GET_TRAP_MISS(trappola) = 50;
	
	act("$n costriusce una trappola.", TRUE, ch, 0, 0, TO_ROOM);
	act("Costruisci una trappola.", TRUE, ch, 0, 0, TO_CHAR);
	obj_to_char(unequip_char(ch, WEAR_HOLD), ch);		
	obj_from_char(trappola);
	obj_to_room(trappola, ch->in_room);
	
	if(GET_LEVEL(ch) > 70) {
		store_mail(get_id_by_name("ardanos"), GET_IDNUM(ch), "Ho cercato di metter una trappola stordente, essendo WIZ!");
	}
}

void piazza_trap_bash (struct char_data *ch, struct obj_data *trappola)
{
	int level;

	level = GET_ABIL(ch, ABIL_TRAPPOLE);
	if (level<MASTER_LIV) {
		send_to_char("Non puoi costruire ancora questo tipo di trappola!\r\n",ch);
		return;
	}

	GET_OBJ_WEAR(trappola) = 0;
	SET_BIT(GET_OBJ_EXTRA(trappola),ITEM_RESTRING);
	GET_OBJ_WEIGHT(trappola) = 1;
	GET_OBJ_CSLOTS(trappola) = 1;
	GET_OBJ_TSLOTS(trappola) = 1;
	GET_TRAP_TIPO(trappola) = TRAP_BASH;
	GET_TRAP_LEVEL(trappola) = GET_ABIL(ch,ABIL_TRAPPOLE);
			
	/* Percentuale di Innesco con Scivolo*/
	GET_TRAP_PERC(trappola) = ((GET_DEX(ch)+GET_INT(ch))/5)*10;
	
	/* Bonus Innesco */
	GET_TRAP_PERC(trappola) += 30;
	
	/* Percentuale di Innesco senza Scivolo */
	GET_TRAP_MISS(trappola) = 0;
	
	act("$n costriusce una trappola.", TRUE, ch, 0, 0, TO_ROOM);
	act("Costruisci una trappola.", TRUE, ch, 0, 0, TO_CHAR);
	obj_to_char(unequip_char(ch, WEAR_HOLD), ch);		
	obj_from_char(trappola);
	obj_to_room(trappola, ch->in_room);
	
	if(GET_LEVEL(ch) > 70) {
		store_mail(get_id_by_name("ardanos"), GET_IDNUM(ch), "Ho cercato di metter una trappola bash, essendo WIZ!");
	}
}

void piazza_trap_allarme (struct char_data *ch, struct obj_data *trappola)
{
	if (find_house(world[ch->in_room].number) < 0) {
		send_to_char("Lo puoi fare solo in una casa.\r\n", ch);
		return;
	}		
	GET_OBJ_WEAR(trappola) = 0;
	SET_BIT(GET_OBJ_EXTRA(trappola),ITEM_RESTRING);
	GET_OBJ_WEIGHT(trappola) = 1;
	GET_OBJ_CSLOTS(trappola) = 1;
	GET_OBJ_TSLOTS(trappola) = 1;
	GET_TRAP_TIPO(trappola) = TRAP_ALLARME;
	GET_TRAP_LEVEL(trappola) = GET_ABIL(ch,ABIL_TRAPPOLE);
	
 	/* Percentuale di Innesco con Allarme*/
	GET_TRAP_PERC(trappola) = ((GET_DEX(ch)+GET_INT(ch))/5)*10;
		
 	/* Bonus Innesco */
	if (GET_ABIL(ch,ABIL_TRAPPOLE)==MASTER_LIV)
		GET_TRAP_PERC(trappola) += 30;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>=EXPERT_LIV)
		GET_TRAP_PERC(trappola) += 20;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>0)
		GET_TRAP_PERC(trappola) += 10;
			
 	/* Percentuale di Innesco senza Allarme */
	if (GET_ABIL(ch,ABIL_TRAPPOLE)==MASTER_LIV)
		GET_TRAP_MISS(trappola) = 0;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>=EXPERT_LIV)
		GET_TRAP_MISS(trappola) = 50;
	else if (GET_ABIL(ch,ABIL_TRAPPOLE)>0)
		GET_TRAP_MISS(trappola) = 100;
	
	act("$n costriusce una trappola.", TRUE, ch, 0, 0, TO_ROOM);
	act("Costruisci una trappola.", TRUE, ch, 0, 0, TO_CHAR);
	obj_to_char(unequip_char(ch, WEAR_HOLD), ch);		
	obj_from_char(trappola);
	obj_to_room(trappola, ch->in_room);
	
	if(GET_LEVEL(ch) > 70) {
		store_mail(get_id_by_name("ardanos"), GET_IDNUM(ch), "Ho cercato di metter una trappola allarme, essendo WIZ!");
	}
}

ACMD(do_catarsi) { // Ardanos

  if(IS_NPC(ch)) {
    send_to_char("Solo i pg possono meditare\r\n", ch);
    return;
  }
  
  if (GET_ABIL(ch,ABIL_MEDITAZIONE)!=MASTER_LIV) {
    send_to_char("Solo i maestri di meditazione possono entrare in catarsi\r\n", ch);
    return;
  }
  
  GET_TIME_CATARSI(ch) = time(0);
  GET_POS(ch) = POS_RESTING;
  send_to_char("La mente si distacca dal tuo corpo\r\n", ch);
  act("$n entra in meditazione.", TRUE, ch, 0, 0, TO_ROOM);
  SET_BIT(PLR_FLAGS(ch), PLR_CATARSI);
}
  

void check_catarsi () { //Ardanos
struct char_data * ch;

  for (ch = character_list; ch; ch = ch->next) {
    if(PLR_FLAGGED(ch, PLR_CATARSI) && (time(0) - GET_TIME_CATARSI(ch) > 65)) {
      send_to_char("La tua mente si ricongiunge al corpo.\r\n", ch);
      act("$n esce dal suo profondo stato meditativo.", TRUE, ch, 0, 0, TO_ROOM);
      REMOVE_BIT(PLR_FLAGS(ch), PLR_CATARSI);
      GET_MANA(ch) = GET_MAX_MANA(ch);
    }
  }
      
}

ACMD(do_rigenerazione) { //Ardanos

  if(IS_NPC(ch) && IS_SET(AFF_FLAGS(ch), AFF_CHARM)) { // se il mob e' charmato, allora non eseguo la rigenerazione
    return;
  }

  if (affected_by_spell (ch, SPELLSKILL, DISEASE_SHAARR)){
    send_to_char ("&1Shaarr ti ha maledetto. Questi mezzi mortali non ti redimeranno alla sua vista!\r\n", ch);
    return;
  }
  if(!IS_NPC(ch)) {
    if (GET_ABIL(ch,ABIL_REGENERATION)!=MASTER_LIV) {
    send_to_char("Solo i maestri di rigenerazione possono curarsi le ferite\r\n", ch);
    return;
    }
  
    if(GET_MANA(ch) != GET_MAX_MANA(ch)) { // ci vuole il 100% di mana per rigenerare
      send_to_char("Hai bisogno di piu' energia per curarti le ferite\r\n",  ch);
      return;
    }
  
    if(GET_POS(ch) == POS_FIGHTING) {
      send_to_char("Non puoi rigenerarti le ferite mentre combatti!\r\n", ch);
     return;
    }
  
    if(!IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_1) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_2) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_3) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_1) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_2) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_3) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_1) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_2) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_3) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_1) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_2) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_3) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_1) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_2) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_3) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_1) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_2) &&
    !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_3)) {
      send_to_char("Non hai nessuna ferita sul tuo corpo\r\n", ch);
      return;
    }

  }
  
  // A questo punto il comando va a buon fine ....
  
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_FERITO);
  
  if(!IS_NPC(ch)) {
    send_to_char("Chiudi gli occhi e pronunciando arcane parole rigeneri le tue ferite\r\n", ch);
    act("$n rigenera le sue ferite.", TRUE, ch, 0, 0, TO_ROOM);
  
    GET_MANA(ch) = 1;
  }
  affect_total(ch); // faccio ripartire il regain



/*
  if(IS_NPC(ch)) {
    send_to_char("Solo i pg possono rigenerare le ferite\r\n", ch);
    return;
  }
  
  if (GET_ABIL(ch,ABIL_REGENERATION)!=MASTER_LIV) {
    send_to_char("Solo i maestri di rigenerazione possono curarsi le ferite\r\n", ch);
    return;
  }
  
  if(GET_MANA(ch) != GET_MAX_MANA(ch)) { // ci vuole il 100% di mana per rigenerare
    send_to_char("Hai bisogno di piu' energia per curarti le ferite\r\n",  ch);
    return;
  }
  
  if(GET_POS(ch) == POS_FIGHTING) {
    send_to_char("Non puoi rigenerarti le ferite mentre combatti!\r\n", ch);
    return;
  }
  
  if(!IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_1) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_2) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_3) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_1) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_2) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_3) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_1) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_2) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_3) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_1) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_2) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_3) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_1) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_2) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_3) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_1) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_2) &&
  !IS_SET(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_3)) {
    send_to_char("Non hai nessuna ferita sul tuo corpo\r\n", ch);
    return;
  }
  
  
  
  // A questo punto il comando va a buon fine ....
  
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TESTA_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_D_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_BRACCIO_S_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_TORSO_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_D_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_1);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_2);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_CRITICAL_GAMBA_S_3);
  REMOVE_BIT(AFF_FLAGS(ch),AFF_FERITO);
  
  send_to_char("Chiudi gli occhi e pronunciando arcane parole rigeneri le tue ferite\r\n", ch);
  act("$n rigenera le sue ferite.", TRUE, ch, 0, 0, TO_ROOM);
  
  GET_MANA(ch) = 1;
  affect_total(ch); // faccio ripartire il regain
  */
}
