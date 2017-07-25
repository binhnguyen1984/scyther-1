/*
 * eqlist.c
 *
 *  Created on: Jun 27, 2014
 *      Author: nguyen
 */
#include "eqlist.h"

int
patSize (Term t)
{
  if (realTermLeaf (t))
    return 1;
  if (realTermEncrypt (t))
    return 2 * patSize (TermOp (t)) + 1;
  return patSize (TermOp1 (t)) + patSize (TermOp2 (t));
}

//compute how much an equation abstracts a term
int
abstCost (Equation eq)
{
  if (eq->type == 1)
    return 0;			//homomorphic equations do not abstract anything
  return patSize (eq->left) - patSize (eq->right);
}

//find matching pattern
Equation
findMatchingPattern (Term termtype, Eqlist eql)
{
  Eqlist templ = eql;
  while (templ != NULL)
    {
      Term left = templ->eq->left;
      Term pattype;
      if (left->stype == NULL)
	{
	  pattype = getTermType (left);
	  left->stype = termlistAdd (left->stype, pattype);
	}
      else
	pattype = ((Termlist) left->stype)->term;
      if (isSubtype (termtype, pattype))
	return templ->eq;
      templ = templ->next;
    }
  return NULL;
}

void
printEquationlist (Eqlist eql)
{
  int i = 0;
  eprintf ("\n");
  while (eql != NULL)
    {
      i++;
      eprintf ("[%d]    ", i);
      printEquation (eql->eq->left, eql->eq->right);
      eprintf (":");
      printTerm (getTermType (eql->eq->left));
      eprintf ("\n");
      eql = eql->next;
    }
}


void
deleteEquation (Equation eq)
{
  int diff = (eq->left != eq->right);
  termDestroy (eq->left);
  if (diff)
    termDelete (eq->right);
  free (eq);
}

//delete a equation list given by a pointer
void
deleteEqlist (Eqlist tmp)
{
  if (tmp == NULL)
    return;
  deleteEqlist (tmp->next);
  deleteEquation (tmp->eq);
  free (tmp);
}

void
deleteEqlistShallow (Eqlist tmp)
{
  if (tmp == NULL)
    return;
  deleteEqlistShallow (tmp->next);
  free (tmp);
}

Eqlist
removeEquationFromList (Eqlist l, Equation eq)
{
  Eqlist tmp = l;
  Eqlist prev = NULL;
  while (tmp != NULL)
    {
      if (tmp->eq == eq)
	{
	  if (prev == NULL)
	    {
	      l = tmp->next;
	    }
	  else
	    {
	      prev->next = tmp->next;
	    }
	  deleteEquation (tmp->eq);
	  free (tmp);
	  return l;
	}
      prev = tmp;
      tmp = tmp->next;
    }
  return l;
}

Eqlist
removeEquationFromListShallow (Eqlist l, Equation eq)
{
  Eqlist tmp = l;
  Eqlist prev = NULL;
  while (tmp != NULL)
    {
      if (tmp->eq == eq)
	{
	  if (prev == NULL)
	    {
	      l = tmp->next;
	    }
	  else
	    {
	      prev->next = tmp->next;
	    }
	  free (tmp);
	  return l;
	}
      prev = tmp;
      tmp = tmp->next;
    }
  return l;
}

Eqlist
deleteNonPersistentEq (Eqlist eqlist)
{
  Eqlist tmp = eqlist;
  Eqlist prev = NULL;
  while (tmp != NULL)
    {
      if (tmp->eq->type != USER_DEFINED_PERSISTENT)
	{
	  Eqlist tmp1 = tmp;
	  if (prev == NULL)
	    eqlist = tmp->next;
	  else
	    prev->next = tmp->next;
	  tmp = tmp->next;
	  deleteEquation (tmp1->eq);
	  tmp1->eq = NULL;
	  tmp1->next = NULL;
	  free (tmp1);
	  continue;
	}
      prev = tmp;
      tmp = tmp->next;
    }
  return eqlist;
}

Equation
makeEquation ()
{
  Equation eq = (Equation) malloc (sizeof (struct equation));
  eq->left = eq->right = NULL;
  return eq;
}

Eqlist
makeEquationlist ()
{
  Eqlist eqlist = (Eqlist) malloc (sizeof (struct eqlist));
  eqlist->eq = NULL;
  eqlist->next = NULL;
  return eqlist;
}

Eqlist
equationlistAdd (Eqlist eqlist, Equation eq)
{
  if (eq == NULL)
    return eqlist;
  Eqlist neweqlist = makeEquationlist ();
  neweqlist->eq = eq;
  neweqlist->next = eqlist;
  return neweqlist;
}



void
printEquation (Term left, Term right)
{
  printTerm (left);
  eprintf (" = ");
  printTerm (right);
  //eprintf("[type:");
  //printTerm(getTermType(right));
  //eprintf("]");
}
