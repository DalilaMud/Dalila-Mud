/*
** dg_event.c: This file contains a simplified event system to allow
** DG Script triggers to use the "wait" command, causing a delay in the
** middle of a script.
**
** By: Mark A. Heilpern (Sammy @ eQuoria MUD   equoria.com:4000)
*/
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "dg_event.h"


/*
** define statics
*/
static struct event_info *event_list = NULL;


/*
** Add an event to the current list
*/
struct event_info *add_event(int time, EVENT(*func), void *info)
{
  struct event_info *thiso, *prev, *curr;

  CREATE(thiso, struct event_info, 1);
  thiso->time_remaining = time;
  thiso->func = func;
  thiso->info = info;

  /* sort the event into the list in next-to-fire order */
  if (event_list == NULL)
    event_list = thiso;
  else if (thiso->time_remaining <= event_list->time_remaining) {
    thiso->next = event_list;
    event_list = thiso;
  } else {
    prev = event_list;
    curr = prev->next;

    while (curr && (curr->time_remaining > thiso->time_remaining)) {
      prev = curr;
      curr = curr->next;
    }

    thiso->next = curr;
    prev->next = thiso;
  }

  return thiso;
}

void remove_event(struct event_info *event)
{
  struct event_info *curr;

  if (!event) {
    log("SYSERR:  Attempted to cancel a NULL event");
    return;
  }
  
  if (event_list == event) {
    event_list = event->next;
  } 
  else {
    curr = event_list;
    while (curr && (curr->next != event))
      curr = curr->next;
    if (!curr) 
      return; /* failed to find it */
    curr->next = curr->next->next;
  }
  free(event);
}

void process_events(void)
{
  struct event_info *e = event_list;
  struct event_info *del;

  while (e) {
    if (--(e->time_remaining) == 0) {
      e->func(e->info);
      del = e;
      e = e->next;
      remove_event(del);
    } 
    else
      e = e->next;
  }
}

void kill_all_events(void)
{
  struct event_info *e = event_list;
  struct event_info *del;

  while (e) {
    del = e;
    e = e->next;
    remove_event(del);
  }
}

