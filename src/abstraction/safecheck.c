/*
 * safecheck.c
 *
 *  Created on: Jun 4, 2014
 *      Author: nguyen
 */
#include "safecheck.h"
#include "termfunc.h"

extern Term TERM_NIL;

int
checkInjectivenessForEvent (Term (*absfunc) (Term), Protocol p,
			    List evPhiPlus, int error, void (*handle) (Term,
								       Term))
{
  while (evPhiPlus != NULL)
    {
      Roledef ev = (Roledef) evPhiPlus->data;
      if (ev->absMess == NULL)
	{
	  ev->absMess = absfunc (ev->message);
	}
      Role role = p->roles;
      while (role != NULL)
	{
	  Roledef events = role->roledef;
	  events->absMess = absfunc (events->message);
	  while (events != NULL)
	    {

	      if (ev->type == events->type
		  && isTermEqual (ev->absMess, events->absMess))
		if (!isTermEqual (ev->message, events->message))
		  {
		    ev->absMess = events->absMess = NULL;
		    if (error)
		      {
			globalError++;
			eprintf
			  ("warning: injectiveness for events (condition v) fails for event terms ");
			printTerm (ev->message);
			eprintf (" and ");
			printTerm (events->message);
			eprintf ("\n");
			globalError--;
		      }
		    if (handle != NULL)
		      handle (ev->message, events->message);
		    return false;
		  }
	      events = events->next;
	    }
	  role = role->next;
	}
      evPhiPlus = evPhiPlus->next;
    }
  return true;
}

int
checkNoNilEvent (Term (*absfunc) (Term), List evPhi, int error,
		 void (*handle) (Term))
{
  List tmp = evPhi;
  while (tmp != NULL)
    {
      Roledef ev = (Roledef) tmp->data;
      if (ev->absMess == NULL)
	{
	  ev->absMess = absfunc (ev->message);
	}
      //last condition
      if (isTermEqual (ev->absMess, TERM_NIL))
	{
	  ev->absMess = NULL;
	  if (error)
	    {
	      globalError++;
	      eprintf ("warning: running event term ");
	      printTerm (ev->message);
	      eprintf ("is transformed to NIL, so condition iv fails\n");
	      globalError--;
	      return false;
	    }
	  if (handle != NULL)
	    handle (ev->message);
	}
      tmp = tmp->next;
    }
  return true;
}
