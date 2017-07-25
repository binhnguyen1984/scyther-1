/*
 * avrem.c
 *
 *  Created on: May 29, 2014
 *      Author: nguyen
 */
#include "avrem.h"

static System sys;
static int succeed;
Termlist av;
void
avremInit (System mysys)
{
  sys = mysys;
  resetAbs (sys);
}

//check if term u occurs in clear text in t
int
isClearTextinTerm (Term t, Term u)
{
  if (realTermLeaf (t))
    return 1;
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall == true)
	return !isSubterm (u, TermOp (t));
      return !isSubterm (u, TermOp (t)) && !isSubterm (u, TermKey (t));
    }
  return isClearTextinTerm (TermOp1 (t), u)
    && isClearTextinTerm (TermOp2 (t), u);
}

void
extractClearAVFromRole (Role r)
{
  Termlist tl;
  //check for variables
  for (tl = r->declaredvars; tl != NULL; tl = tl->next)
    {
      Roledef rd;
      int isclear = 1;
      for (rd = r->roledef; rd != NULL; rd = rd->next)
	{
	  if (rd->type != CLAIM && !isClearTextinTerm (rd->message, tl->term))
	    {
	      isclear = 0;
	      break;
	    }
	}
      if (isclear)
	{
	  tl->term->rolename = r->nameterm;
	  av = termlistAdd (av, tl->term);
	}
    }
  //check for constants
  for (tl = r->declaredconsts; tl != NULL; tl = tl->next)
    {
      Roledef rd;
      int isclear = 1;
      for (rd = r->roledef; rd != NULL; rd = rd->next)
	if (rd->type != CLAIM && !isClearTextinTerm (rd->message, tl->term))
	  {
	    isclear = 0;
	    break;
	  }
      if (isclear)
	av = termlistAdd (av, tl->term);
    }
}

void
extractClearAVFromProt (Protocol p)
{
  Role r = p->roles;
  while (r != NULL)
    {
      extractClearAVFromRole (r);
      r = r->next;
    }
}

//remove atom(varialbe) av from t
Term
removeAv (Term t, Term NIL)
{
  if (realTermLeaf (t))
    {
      if (isTermEqual (t, NIL) && t->rolename == NIL->rolename)
	{
	  succeed = 1;
	  return TERM_NIL;
	}
      return t;
    }
  //tuples
  else
    {
      Term op1 = removeAv (TermOp1 (t), NIL);
      Term op2 = removeAv (TermOp2 (t), NIL);
      if (isTermEqual (op1, TERM_NIL))
	return op2;
      else if (isTermEqual (op2, TERM_NIL))
	return op1;
      TermOp1 (t) = op1;
      TermOp2 (t) = op2;
      return t;
    }
}

Term
removeAvlist (Term t)
{
  if (realTermEncrypt (t))
    return t;
  Termlist tl = av;
  while (tl != NULL)
    {
      t = removeAv (t, tl->term);
      tl = tl->next;
    }
  return t;
}



void
extractAtom (Termlist at, Term t)
{
  if (realTermLeaf (t))
    {
      //only consider non-agent terms
      if (!realTermVariable (t) && !inTermlist (t->stype, TERM_Agent))
	{
	  at = termlistAddNewTerm (at, t);
	}
      return;
    }
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	extractAtom (at, TermOp (t));
      else
	{
	  extractAtom (at, TermOp (t));
	  extractAtom (at, TermKey (t));
	}
      return;
    }
  extractAtom (at, TermOp1 (t));
  extractAtom (at, TermOp2 (t));
}

//condition i: filter out clear atoms and variables for removal
Termlist
clearInProtMessages (Protocol p)
{
  Termlist tl = av;
START:
  while (tl != NULL)
    {
      Role r = p->roles;
      while (r != NULL)
	{
	  Roledef rd = r->roledef;
	  while (rd != NULL)
	    {
	      if ((rd->type == CLAIM && rd->message != NULL
		   && isSubterm (tl->term, rd->message)) || (rd->type != CLAIM
							     &&
							     !isClearTextinTerm
							     (rd->message,
							      tl->term)))
		{
		  if (tl->prev != NULL)
		    tl->prev->next = tl->next;
		  else
		    av = tl->next;
		  if (tl->next != NULL)
		    tl->next->prev = tl->prev;
		  Termlist tmp = tl;
		  tl = tl->next;
		  free (tmp);
		  goto START;
		}
	      rd = rd->next;
	    }
	  if (rd != NULL)
	    break;
	  r = r->next;
	}
      tl = tl->next;
    }
  return av;
}

//check condition ii: At is disjoint from IK0. We also remove atoms those creates a violation
Termlist
disjointFromFreshIK ()
{
  return termlistMinusTermlist (av, sys->know->basic);
}

void
fixNilEvent (Term t)
{
  Term atom = smallestTermInTuple (t);
  av = termlistFindAndRemove (av, atom);
}

void
fixIdenticalEvents (Term t, Term u)
{
  Termlist tl1 = extractClearAVFromTerm (NULL, t);
  Termlist tl2 = extractClearAVFromTerm (NULL, u);
  Termlist tl = av;
  while (tl != NULL)
    {
      int in1 = inTermlist (tl1, tl->term);
      int in2 = inTermlist (tl2, tl->term);
      if ((in1 && !in2) || (in2 && !in1))
	{
	  if (tl->prev != NULL)
	    tl->prev->next = tl->next;
	  else
	    av = tl->next;
	  if (tl->next != NULL)
	    tl->next->prev = tl->prev;
	  Termlist tmp = tl;
	  tl = tl->next;
	  free (tmp);
	  return;
	}
      tl = tl->next;
    }
  if (tl == NULL)
    {
      termlistDelete (av);
      av = NULL;
    }
}

int
isSubtermOfSecTerm (Term t, List tl)
{
  while (tl != NULL)
    {
      Roledef rd = (Roledef) tl->data;
      if (isSubterm (t, rd->message))
	return 1;
      tl = tl->next;
    }
  return 0;
}

int
isSubtermOfEqTerm (Term t, Eqlist l)
{
  while (l != NULL)
    {
      if (isSubterm (t, l->eq->left) || isSubterm (t, l->eq->right))
	return 1;
      l = l->next;
    }
  return 0;
}

void
disjointFromProperties (List secret, Eqlist eqs)
{
  Termlist tl = av;
  while (tl != NULL)
    {
      if (isSubtermOfSecTerm (tl->term, secret)
	  || isSubtermOfEqTerm (tl->term, eqs))
	{
	  if (tl->prev != NULL)
	    tl->prev->next = tl->next;
	  else
	    av = tl->next;
	  if (tl->next != NULL)
	    tl->next->prev = tl->prev;
	  Termlist tmp = tl;
	  tl = tl->next;
	  free (tmp);
	  continue;
	}
      tl = tl->next;
    }
}

void
fixSafenessForAVRemoval (Protocol p)
{
  List evPhi = NULL;
  List evPhiPlus = NULL;
  List secret = getSecrecyProp (p, NULL);
  Eqlist eqs = getEqualities (p, NULL);
  getPropertyEvents (p, &evPhi, &evPhiPlus);
  //condition i
  //clearInProtMessages(p); //not necessary as we extract only clear atoms in the first place
  //condition ii
  disjointFromFreshIK ();
  //condition iii: (P,avrem)-safe
  //subcondition i
  disjointFromProperties (secret, eqs);
  //subcondition ii
  checkNoNilEvent (removeAvlist, evPhi, 0, fixNilEvent);
  //subcondition iii
  checkInjectivenessForEvent (removeAvlist, p, evPhiPlus, 0,
			      fixIdenticalEvents);
  list_delete (evPhi);
  list_delete (evPhiPlus);
  list_delete (secret);
  deleteEqlistShallow (eqs);
}


//remove atoms that are in clear in role terms
//note that we do not check if the atoms are not in the property,
//as this check is assumed to be done before invoking this abstraction
int
avRemoval1 (Protocol p)
{
  succeed = 0;
  return tryAbstractProt1 (NULL, removeAvlist, p) && succeed;
}


int
avRemoval ()
{
  int succeed = 0;
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      av = NULL;
      extractClearAVFromProt (p);
      fixSafenessForAVRemoval (p);
      if (av != NULL)
	{
	  if (avRemoval1 (p))
	    succeed = 1;
	}
      termlistDelete (av);
      p = p->next;
    }
  return succeed;
}
