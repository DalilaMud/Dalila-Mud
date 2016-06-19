#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "clan.h"
#include "auction.h"

#define PERC_TAS_AUC 10  //10% in tasse

extern struct descriptor_data *descriptor_list;
extern struct char_data *character_list;
extern struct room_data *world;
extern struct str_app_type str_app[];

/* The storage struct itself */
struct auction_data auction;

extern int can_take_container(struct char_data * ch, struct obj_data * obj);
extern int objlevel (struct obj_data * obj);

/*
 * auction_output : takes two strings and dispenses them to everyone connected
 *             based on if they have color on or not.  Note that the buf's are
 *             commonly used *color and *black so I allocate my own buffer.
 */
void auction_output(char *color, char *black)
{
	char buffer[MAX_STRING_LENGTH];
	struct descriptor_data *d;
	
	if (!auction.auctioneer)
		auction.auctioneer = str_dup(DEFAULT_AUCTIONEER);
	
	for (d = descriptor_list; d; d = d->next)
		if (!d->connected && d->character &&
			!PLR_FLAGGED(d->character, PLR_WRITING) &&
			!PRF_FLAGGED(d->character, PRF_NOAUCT) &&
			!ROOM_FLAGGED(d->character->in_room, ROOM_SOUNDPROOF) &&
			!ROOM_FLAGGED(d->character->in_room, ROOM_PRISON)) {
			sprintf(buffer, "%s%s%s auctions, '%s%s%s'%s\r\n",
				CCMAG(d->character,C_NRM), auction.auctioneer,
				CCCYN(d->character,C_NRM), CCNRM(d->character,C_NRM),
				(COLOR_LEV(d->character) > C_NRM) ? color : black,
				CCMAG(d->character,C_NRM),CCNRM(d->character,C_NRM));
			send_to_char(buffer, d->character);
		}
}

void auction_update(void)
{
	long clanid, tax;
	
	if (auction.ticks == AUC_NONE) /* No auction */
		return;
	
  /* Seller left! */
	if (!get_ch_by_id_desc(auction.seller) && !get_ch_by_id(auction.seller)) {
		if (auction.obj)
			extract_obj(auction.obj);
		auction_reset();
		return;
	}
	
  /* If there is an auction but it's not sold yet */
	if (auction.ticks >= AUC_BID && auction.ticks <= AUC_SOLD) {
		struct char_data *bidder = get_ch_by_id(auction.bidder);
		struct char_data *seller = get_ch_by_id(auction.seller);
    /* If there is a bidder and it's not sold yet */
		if (bidder && (auction.ticks < AUC_SOLD)) {
      /* Non colored message */
			sprintf(buf, "%s all'asta %s%s%s a %s per %ld coin%s.",
				auction.obj->short_description,
				auction.ticks == AUC_BID ? "uno" : "",
				auction.ticks == AUC_ONCE ? "due" : "",
				auction.ticks == AUC_TWICE ? "ultima chiamata" : "",
				GET_NAME(bidder), auction.bid, auction.bid != 1 ? "" : " ");
      /* Colored message */
			sprintf(buf2, "\x1B[1;37m%s\x1B[35m all'asta \x1B[1;37m%s%s%s\x1B[35m a \x1B[1;37m%s\x1B[35m per \x1B[1;37m%ld\x1B[35m coin%s.",
				auction.obj->short_description,
				auction.ticks == AUC_BID ? "uno" : "",
				auction.ticks == AUC_ONCE ? "due" : "",
				auction.ticks == AUC_TWICE ? "ultima chiamata" : "",
				GET_NAME(bidder), auction.bid, auction.bid != 1 ? "" : " ");
      /* send the output */
			auction_output(buf2, buf);
      /* Increment timer */
			auction.ticks++;
			return;
		}
		
    /* If there is no bidder and we ARE in the sold state */
		if (!bidder && (auction.ticks == AUC_SOLD)) {
      /* Colored message */
			sprintf(buf2, "\x1B[1;37m%s\x1B[35m VENDUTO\x1B[35m a \x1B[1;37mnessuno\x1B[35m per \x1B[1;37m%ld\x1B[35m coin%s.",
				auction.obj->short_description,
				auction.bid, auction.bid != 1 ? " " : " ");
      /* No color message */
			sprintf(buf, "%s e' VENDUTO a nessuno per %ld coin%s.",
				auction.obj->short_description,
				auction.bid,
				auction.bid != 1 ? "s" : " ");
      /* Send the output away */
			auction_output(buf2, buf);
      /* Give the poor fellow his unsold goods back */
			if (seller)
				if ((IS_CARRYING_W(seller) + GET_OBJ_WEIGHT(auction.obj)) <= CAN_CARRY_W(seller)) {
					act("Nessuno ha comprato $p.", FALSE, seller, auction.obj, 0, TO_CHAR);					
					obj_to_char(auction.obj, seller);
				} else {
					/* Cosi' si evitano le aste per diminuire il peso */
					act("Nessuno ha comprato $p, ma hai troppo peso e lo devi lasciare per terra.", FALSE, seller, auction.obj, 0, TO_CHAR);
					obj_to_room(auction.obj, IN_ROOM(seller));
				}    
     /* He's not around to get it back, destroy the object */
			else
				extract_obj(auction.obj);
     /* Reset the auction for next time */
			auction_reset();
			return;
		}
		
   /* If there is no bidder and we are not in the sold state */
		if (!bidder && (auction.ticks < AUC_SOLD)) {
     /* Colored output message */
			sprintf(buf2, "\x1B[1;37m%s\x1B[35m all'asta \x1B[1;37m%s%s%s\x1B[35m a \x1B[1;37mnessuno\x1B[35m per \x1B[1;37m%ld\x1B[35m coin%s.",
				auction.obj->short_description,
				auction.ticks == AUC_BID ? "uno" : "",
				auction.ticks == AUC_ONCE ? "due" : "",
				auction.ticks == AUC_TWICE ? "ultima chiamata" : "",
				auction.bid, auction.bid != 1 ? "s" : "");
     /* No color output message */
			sprintf(buf, "%s all'asta %s%s%s a nessuno per %ld coin%s.",
				auction.obj->short_description,
				auction.ticks == AUC_BID ? "uno" : "",
				auction.ticks == AUC_ONCE ? "due" : "",
				auction.ticks == AUC_TWICE ? "ultima chiamata" : "",
				auction.bid, auction.bid != 1 ? "s" : "");
     /* Send output away */
			auction_output(buf2, buf);
     /* Increment timer */
			auction.ticks++;
			return;
		}
		
   /* Sold */
		if (bidder && (auction.ticks >= AUC_SOLD)) {
     /* Colored output */
			sprintf(buf2, "\x1B[1;37m%s\x1B[35m VENDUTO\x1B[35m a \x1B[1;37m%s\x1B[35m per \x1B[1;37m%ld\x1B[35m coin%s.",
				auction.obj->short_description ? auction.obj->short_description : "qualcosa",
				bidder->player.name ? bidder->player.name : "qualcuno",
				auction.bid, auction.bid != 1 ? " " : "");
     /* Non color output */
			sprintf(buf, "%s VENDUTO a %s per %ld coin%s.",
				auction.obj->short_description ? auction.obj->short_description : "something",
				bidder->player.name ? bidder->player.name : "someone",
				auction.bid, auction.bid != 1 ? "s" : "");
     /* Send the output */
			auction_output(buf2, buf);
			
     /* If the seller is still around we give him the money */
			if (seller) {
				act("Congrats! Hai venduto $p!", FALSE, seller, auction.obj, 0, TO_CHAR);
				if ((clanid = find_clan_by_id(GET_CLAN(seller)))>=0) {
					tax = auction.bid * PERC_TAS_AUC/100;
					GET_GOLD(seller) += MAX(0,auction.bid - (auction.bid*PERC_TAS_AUC/100));
					clan[clanid].economy.treasure += MAX(0,(auction.bid*PERC_TAS_AUC/100));
					clan[clanid].economy.current_entrate += MAX(0,(auction.bid*PERC_TAS_AUC/100));
					sprintf(buf,"Paghi %ld di tasse al tuo regno per la vendita.", tax);
					act(buf, FALSE, seller, auction.obj, 0, TO_CHAR);
				} else GET_GOLD(seller) += auction.bid;
			}
			
     /* If the bidder is here he gets the object */
			if (bidder) {
				obj_to_char(auction.obj, bidder);
				act("Congrats! Ora hai $p!", FALSE, bidder, auction.obj, 0, TO_CHAR);
			}
     /* Restore the status of the auction */
			auction_reset();
			return;
		}
	}
	return;
}

/*
 * do_bid : user interface to place a bid.
 */
ACMD(do_bid)
{
	long bid;
	
  /* NPC's can not bid or auction due to lack of idnum */
	if (IS_NPC(ch))
	{
		send_to_char("Non sei abastanza 'unico' per poter fare un'offerta...\r\n", ch);
		return;
	}
	
  /* There isn't an auction */
	if (auction.ticks == AUC_NONE)
	{
		send_to_char("Niente in vendita.\r\n", ch);
		return;
	}
	
	if (ROOM_FLAGGED(ch->in_room, ROOM_PRISON))
	{
		send_to_char("Non puoi farlo dalla prigione!\r\n", ch);
		return;
	}
	
	if (((GET_LEVEL(ch)<GET_OBJ_LEVEL(auction.obj)) || ( GET_OBJ_LEVEL(auction.obj)<=0 && (GET_LEVEL(ch)<objlevel(auction.obj))) ) )
	{
		act("Sei troppo inesperto per poter partecipare all'asta di $p, rinunci.", FALSE, ch, auction.obj, 0, TO_CHAR);
		return;
	}
	
	if ((GET_OBJ_TYPE(auction.obj) == ITEM_CONTAINER) && (!can_take_container(ch,auction.obj)))
	{
		act("All'interno di $p si trova qualcosa di troppo potente per te, rinunci.", FALSE, ch, auction.obj, 0, TO_CHAR);
		return;
	}
	
	one_argument(argument, buf);
	bid = atoi(buf);
	
 /* They didn't type anything else */
	if (!*buf)
	{
		sprintf(buf2, "Offerta : %ld monet%c.\r\n", auction.bid, auction.bid != 1 ? 'e' : 'a');
		send_to_char(buf2, ch);
 /* The current bidder is this person */
	}
	else if (ch == get_ch_by_id_desc(auction.bidder))
		send_to_char("Stai tentando di fare un'offerta a te stesso.\r\n", ch);
 /* The seller is the person who tried to bid */
	else if (ch == get_ch_by_id_desc(auction.seller))
		send_to_char("Non puoi fare offerte su un oggetto tuo.\r\n", ch);
 /* Tried to auction below the minimum */
	else if ((bid < auction.bid) && auction.bidder < 0) {
		sprintf(buf2, "L'offerta minima e' ora %ld coins.\r\n", auction.bid);
		send_to_char(buf2, ch);
 /* Tried to bid below the minimum where there is a bid, 5% increases */
	} else if ((bid < (auction.bid * 1.05) && auction.bidder >= 0) || bid == 0) {
		sprintf(buf2, "Prova ad offrire almeno il 5%% dell'offerta di %ld. (%.0f coins).\r\n",
			auction.bid, auction.bid * 1.05 + 1);
		send_to_char(buf2, ch);
 /* Not enough gold on hand! */
	} else if (GET_GOLD(ch) < bid) {
		sprintf(buf2, "Hai solo %d coins in mano.\r\n", GET_GOLD(ch));
		send_to_char(buf2, ch);
 /* it's an ok bid */
	} else {
   /* Give last bidder money back if he's around! */
		if (auction.bidder >= 0 && get_ch_by_id(auction.bidder))
			GET_GOLD(get_ch_by_id(auction.bidder)) += auction.bid;
   /* This is the bid value */
		auction.bid = bid;
   /* The bidder is this guy */
		auction.bidder = GET_IDNUM(ch);
   /* This resets the auction to first chance bid */
		auction.ticks = AUC_BID;
   /* Get money from new bidder. */
		GET_GOLD(ch) -= auction.bid;
   /* Prepare colored message */
		sprintf(buf2, "\x1B[1;37m%s\x1B[35m offre \x1B[1;37m%ld\x1B[35m coin%s per \x1B[1;37m%s\x1B[35m.",
			GET_NAME(ch), auction.bid, auction.bid!=1 ? " " :"",
			auction.obj->short_description);
   /* Prepare non-colored message */
		sprintf(buf, "%s bids %ld coin%s on %s.",
			GET_NAME(ch), auction.bid, auction.bid!=1 ? " " :"",
			auction.obj->short_description);
   /* Send the output away */
		auction_output(buf2, buf);
	}
}

/*
* do_auction : user interface for placing an item up for sale
*/
ACMD(do_auction)
{
	struct obj_data *obj;
	struct char_data *seller;
	
 /* NPC's can not bid or auction due to lack of idnum */
	if (IS_NPC(ch)) {
		send_to_char("You're not unique enough to auction.\r\n", ch);
		return;
	}
	
	two_arguments(argument, buf1, buf2);
	
 /* There is nothing they typed */
	if (!*buf1)
		send_to_char("Auction cosa e che offerta minima?\r\n", ch);
 /* Hrm...logic error? */
	else if (auction.ticks != AUC_NONE) {
   /* If seller is no longer present, auction continues with no seller */
		if ((seller = get_ch_by_id(auction.seller)))
			sprintf(buf2, "%s sta facendo un asta su %s per %ld coins.\r\n",
				GET_NAME(seller), auction.obj->short_description, auction.bid);
		else
			sprintf(buf2, "Nessuno sta facendo un asta su %s per %ld coins.\r\n",
				auction.obj->short_description, auction.bid);
		send_to_char(buf2, ch);
 /* Person doesn't have that item */
	} else if ((obj = get_obj_in_list_vis(ch, buf1, ch->carrying)) == NULL)
		send_to_char("Non sembra tu abbia quello da vendere.\r\n", ch);
 /* Can not auction corpses because they may decompose */
	else if ((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && (GET_OBJ_VAL(obj, 3)))
		send_to_char("Non puoi vendere corpi.\n\r", ch);	
 /* It's valid */
	else {
   /* First bid attempt */
		auction.ticks = AUC_BID;
   /* This guy is selling it */
		auction.seller = GET_IDNUM(ch);
   /* Can not make the minimum less than 0 --KR */
		auction.bid = (atoi(buf2) > 0 ? atoi(buf2) : 1);
   /* Pointer to object */
		auction.obj = obj;
   /* Get the object from the character, so they cannot drop it! */
		obj_from_char(auction.obj);
   /* Prepare color message for those with it */
		sprintf(buf2, "\x1B[1;37m%s\x1B[35m mette \x1B[1;37m%s\x1B[35m in vendita, offerta minima \x1B[1;37m%ld\x1B[35m coin%s",
			GET_NAME(ch), auction.obj->short_description, auction.bid,
			auction.bid != 1 ? "." : ".");
   /* Make a message sans-color for those whole have it off */
		sprintf(buf, "%s mette %s in vendita, offerta minima %ld coin%s",
			GET_NAME(ch), auction.obj->short_description, auction.bid,
			auction.bid != 1 ? "s." : ".");
   /* send out the messages */
		auction_output(buf2, buf);
	}
}

/*
 * auction_reset : returns the auction structure to a non-bidding state
 */
void auction_reset(void)
{
	auction.bidder = -1;
	auction.seller = -1;
	auction.obj = NULL;
	auction.ticks = AUC_NONE;
	auction.bid = 0;
}

/*
 * get_ch_by_id_desc : given an ID number, searches every descriptor for a
 *             character with that number and returns a pointer to it.
 */
struct char_data *get_ch_by_id_desc(long idnum)
{
	struct descriptor_data *d;
	for (d = descriptor_list; d; d = d->next)
		if (d && d->character && GET_IDNUM(d->character) == idnum)
			return (d->character);
		
	return NULL;
}

/*
 * get_ch_by_id: searches the character list for a pc
 */
struct char_data *get_ch_by_id(long idnum)
{
	struct char_data *tch;
	for (tch = character_list; tch; tch = tch->next)
		if (tch && !IS_NPC(tch) && GET_IDNUM(tch) == idnum)
			return (tch);
		
	return NULL;
}

/*
* do_auctioneer: Changes the name used on the auction channel.
*/
ACMD(do_auctioneer)
{
	skip_spaces(&argument);
	
	if (!argument || !*argument)
		send_to_char("Deve avere un nome!\r\n", ch);
	else {
		if (auction.auctioneer)
			free(auction.auctioneer);
		auction.auctioneer = str_dup(argument);
		send_to_char(OK, ch);
	}
}


ACMD(do_stop_auction)
{
	struct char_data *tch;
	
	if (auction.obj) {
		if ((tch = get_ch_by_id(auction.seller)))
			obj_to_char(auction.obj, tch);
		else
			extract_obj(auction.obj);
	}
	
	if (auction.bid)
		if ((tch = get_ch_by_id(auction.bidder)))
			GET_GOLD(tch) += auction.bid;
		
	auction_reset();
	send_to_char(OK, ch);
}



