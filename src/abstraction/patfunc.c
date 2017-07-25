/*
 * eqfunc.c
 *
 *  Created on: Jun 10, 2014
 *      Author: nguyen
 */
#include "patfunc.h"

extern Symbol pat;
extern Symbol pat;
//term generation
Term
createVariableWithType (Symbol s, Term type)
{
  Term term = (Term) malloc (sizeof (struct term));
  term->abst = 0;
  term->type = VARIABLE;
  term->stype = termlistAdd (NULL, type);
  term->left.symb = s;
  return term;
}

Term
createNewVariableWithType (Term type)
{
  Symbol s = symbolNextFree (pat);
  return createVariableWithType (s, type);
}

Equation
createTicketHomomorphicPatForHash (Term hashfunc)
{
  Equation eq = makeEquation ();
  Term var = createNewVariableWithType (TERM_Ticket);
  var->abst = 1;
  Term left = makeTermFcall (var, hashfunc);
  eq->left = eq->right = left;
  eq->type = SYSTEM_TRIVIAL;
  return eq;
}

//this creates a term from a type
Term
createSimplePatFromType (Term type)
{
  Term t;
  if (realTermLeaf (type))
    {
      return createNewVariableWithType (type);
    }
  else if (realTermEncrypt (type))
    {
      Term v = createNewVariableWithType (TermOp (type));
      if (type->helper.fcall)
	{
	  t = makeTermFcall (v, TermKey (type));
	}
      else
	{
	  Term key = createNewVariableWithType (TermKey (type));
	  t = makeTermEncrypt (v, key);
	}
    }
  else
    {
      Term v1 = createNewVariableWithType (TermOp1 (type));
      Term v2 = createNewVariableWithType (TermOp2 (type));
      t = makeTermTuple (v1, v2);
    }
  return t;
}

Term
makeHomomorphic (Term t)
{
  if (realTermLeaf (t))
    t->abst = 1;
  else if (realTermEncrypt (t))
    {
      makeHomomorphic (TermOp (t));
      if (!t->helper.fcall)
	makeHomomorphic (TermKey (t));
    }
  else
    {
      makeHomomorphic (TermOp1 (t));
      makeHomomorphic (TermOp2 (t));
    }
  return t;
}

Term
makeHomomorphicTuple (Term t)
{
  if (realTermTuple (t))
    {
      makeHomomorphicTuple (TermOp1 (t));
      makeHomomorphicTuple (TermOp2 (t));
    }
  else
    t->abst = 1;
  return t;
}

//create an equation (homomorphic one) from a type
Equation
createHomoEquationFromType (Term type)
{
  Equation eq = makeEquation ();
  Term left = createPatFromType (type);
  left->stype = termlistAdd (NULL, type);
  eq->left = left;
  eq->right = left;
  if (realTermLeaf (left))
    eq->right->abst = 0;
  else
    eq->right = makeHomomorphic (eq->right);
  eq->type = SYSTEM_TRIVIAL;
  return eq;
}

Term
createTuplePatFromType (Term type)
{
  if (!realTermTuple (type))
    {
      return createNewVariableWithType (type);
    }
  else
    {
      Term op1 = createTuplePatFromType (TermOp1 (type));
      Term op2 = createTuplePatFromType (TermOp2 (type));
      return makeTermTuple (op1, op2);
    }
}

Term
createPatFromType (Term type)
{
  if (!realTermEncrypt (type))
    {
      return createTuplePatFromType (type);
    }
  if (type->helper.fcall)
    {
      return makeTermFcall (createTuplePatFromType (TermOp (type)),
			    TermKey (type));
    }
  return makeTermEncrypt (createTuplePatFromType (TermOp (type)),
			  createNewVariableWithType (TermKey (type)));
}


Term
createTermFromPatList (Termlist tl)
{
  if (tl == NULL)
    return NULL;
  Term t;
  if (tl->next == NULL)
    {
      t = termDuplicate (tl->term->subst);
    }
  else
    {
      t =
	makeTermTuple (termDuplicate (tl->term->subst),
		       createTermFromPatList (tl->next));
    }
  return t;
}
