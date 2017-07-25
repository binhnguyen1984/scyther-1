/*
 * redabst.c
 *
 *  Created on: May 18, 2014
 *      Author: nguyen
 */
#include "redabs.h"

static System sys;
static int succeed;
void
redabsInit (System mysys)
{
  sys = mysys;
  resetAbs (sys);
}

Termlist
removeTermlistRedundancy (Termlist tl, Knowledge know)
{
  if (tl == NULL)
    return NULL;
  Termlist basicHead = know->basic;
  Termlist varsHead = know->vars;
  Termlist encHead = know->encrypt;

  Termlist temp = tl;
  int redundant = 0;
  while (temp != NULL)
    {
      //add all preceding elements
      Termlist p = temp->prev;
      while (p != NULL)
	{
	  addTermToIK (know, p->term);
	  p = p->prev;
	}
      p = temp->next;
      while (p != NULL)
	{
	  addTermToIK (know, p->term);
	  p = p->next;
	}
      int isRedundant = inKnowledge (know, temp->term);
      //recover initial knowledge
      recoverIK (know, basicHead, varsHead, encHead);
      //check the result
      if (isRedundant)
	{
	  redundant = 1;
	  Termlist next = temp->next;
	  if (temp->prev != NULL)
	    temp->prev->next = temp->next;
	  if (temp->next != NULL)
	    temp->next->prev = temp->prev;
	  tl = termlistDelTerm (temp);
	  temp = next;
	  if (temp == NULL)
	    break;
	  else
	    continue;
	}
      temp = temp->next;
    }
  if (redundant)
    succeed++;
  return tl;
}

Term
removeRedundancy (Term t)
{
  Termlist tl = split (t);
  tl = removeTermlistRedundancy (tl, sys->know);
  if (tl == NULL)
    return TERM_NIL;
  Term result = termlist_to_tuple (tl);
  termlistDelete (tl);
  return result;
}

void
removeProtRedundancy (Protocol prot)
{
  Protocol p = prot;
  //we are going to change ik, so remember the heads of these lists
  Termlist basic = sys->know->basic;
  while (p != NULL)
    {
      List evPhi = NULL;
      List evPhiPlus = NULL;
      getPropertyEvents (p, &evPhi, &evPhiPlus);
      Termlist basicHead = sys->know->basic;
      Termlist varsHead = sys->know->vars;
      Termlist encHead = sys->know->encrypt;
      Role role = p->roles;
      while (role != NULL)
	{
	  addAgentVariables (sys->know, role->variables);
	  Roledef rd = role->roledef;
	  Roledef prevNonClaim = NULL;
	  while (rd != NULL)
	    {
	      if (rd->message != NULL)
		{

		  int isClaim = rd->type == CLAIM;
		  if (!isClaim)
		    {
		      rd->absMess = removeRedundancy (rd->message);
		      if (isTermEqual (rd->absMess, TERM_NIL)
			  && in_list (evPhi, rd))
			{
			  rd->absMess = smallestTermInTuple (rd->message);
			  succeed -= isTermEqual (rd->message, rd->absMess);
			}
		      prevNonClaim = rd;
		      addTermToIK (sys->know, prevNonClaim->message);
		    }
		  else
		    rd->absMess = rd->message;
		}
	      rd = rd->next;
	    }
	  //we set the endings to NULL, i.e., recover the initial knowledge
	  recoverIK (sys->know, basicHead, varsHead, encHead);
	  role = role->next;
	}
      sys->know->basic = basic;
      list_delete (evPhi);
      list_delete (evPhiPlus);
      p = p->next;
    }
}

int
redundancyAbstraction ()
{
  succeed = 0;
  removeProtRedundancy (sys->protocols);
  return succeed;
}
