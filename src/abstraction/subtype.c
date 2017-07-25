/*
 * subtype.c
 *
 *  Created on: Apr 19, 2014
 *      Author: nguyen
 */
#include "subtype.h"
extern Termlist homfunc;
//check if type1 is a subtype of type2
//exp: type1 = {Agent}Ticket, type2 = {Nonce}Nonce. Note that for types it is more complicated.
//e.g., type Nonce is a constant, but a constant b is also a constant. So it could be confusing.
//e.g., the term {b}X is of type {b}Nonce, where b is a constant, not a real type. So to see whether a
//term in a type is a constant term or a constant type, we look at the stype. For a type, stype is NULL
int
isSubtype (Term type1, Term type2)
{
  if (type1 == NULL || type2 == NULL)
    return 0;
  if (realTermLeaf (type2))
    {
      //if type2 is a msg then it is trivial that type1 < type2
      if (isTermEqual (type2, TERM_Ticket))
	return 1;
      //if type1 is composed then fail
      else if (type1->type > LEAF)
	return 0;
      //otherwise, it is a real atomic type
      else if (inTermlist (type1->stype, TERM_Type))
	{
	  if (!isTermEqual (type1, type2))
	    return inTermlist (type1->stype, type2);
	  return 1;
	}
      // or a real constant or fresh values
      else
	{
	  //if type2 is a real type
	  if (inTermlist (type2->stype, TERM_Type))
	    return inTermlist (type1->stype, type2);
	  return isTermEqual (type1, type2);
	}
    }
  else
    {
      if (type1->type != type2->type)
	return 0;
      if (realTermEncrypt (type1))
	{
	  if (type1->helper.fcall != type2->helper.fcall)
	    return false;
	  if (type1->helper.fcall)
	    return isSubtype (TermOp (type1), TermOp (type2)) &&
	      isTermEqual (TermKey (type1), TermKey (type2));
	  return isSubtype (TermKey (type1), TermKey (type2))
	    && isSubtype (TermOp (type1), TermOp (type2));
	}
      else
	return isSubtype (TermOp1 (type1), TermOp1 (type2))
	  && isSubtype (TermOp2 (type1), TermOp2 (type2));
    }
}

Term
subTypeInSubterm (Term sttype, Term type)
{
  if (realTermLeaf (type))
    return NULL;
  if (isSubtype (sttype, type))
    return type;
  if (realTermEncrypt (type))
    {
      Term t1 = subTypeInSubterm (sttype, TermOp (type));
      if (t1)
	return t1;
      else if (!type->helper.fcall)
	{
	  return subTypeInSubterm (sttype, TermKey (type));
	}
    }
  else if (realTermTuple (type))
    {
      Term t1 = subTypeInSubterm (sttype, TermOp1 (type));
      if (t1)
	return t1;
      else
	return subTypeInSubterm (sttype, TermOp2 (type));
    }
  return NULL;
}

Term
makeTupleTicket (Term t)
{
  if (!realTermTuple (t))
    return TERM_Ticket;
  else
    {
      return makeTermTuple (TERM_Ticket, makeTupleTicket (TermOp2 (t)));
    }
}

//Derive a type from a given term
Term
getTermType (Term t)
{
  if (t == NULL)
    return NULL;
  Term type;
  if (realTermLeaf (t))
    {
      // the term is a constant or a fresh value
      if (t->type == GLOBAL)
	return t;
      //the first type in the type list is returned, so we do not support multiple types
      if (t->stype != NULL)
	return ((Termlist) t->stype)->term;
      return TERM_Ticket;
    }
  else if (t->stype != NULL)
    return ((Termlist) t->stype)->term;
  else if (realTermEncrypt (t))
    {
      Term opType = getTermType (TermOp (t));
      if (t->helper.fcall)
	{
	  if (inTermlist (homfunc, TermKey (t)))
	    type = makeTermFcall (makeTupleTicket (TermOp (t)), TermKey (t));
	  else
	    type = makeTermFcall (opType, TermKey (t));
	}
      else
	type = makeTermEncrypt (opType, getTermType (TermKey (t)));
    }
  else
    {
      Term op1Type = getTermType (TermOp1 (t));
      Term op2Type = getTermType (TermOp2 (t));
      type = makeTermTuple (op1Type, op2Type);;
    }
  //update type so that it can be used for future
  t->stype = termlistAdd (NULL, type);
  return type;
}

int
isComparable (Term typ1, Term typ2)
{
  if (realTermLeaf (typ1) || realTermLeaf (typ2))
    {
      if (isSubtype (typ1, typ2))
	return 1;
      else if (isSubtype (typ2, typ1))
	return 1;
      else
	return 0;
    }
  else				//both composed
    {
      if (typ1->type != typ2->type)
	return 0;
      if (realTermEncrypt (typ1))
	{
	  if (typ1->helper.fcall != typ2->helper.fcall)
	    return 0;
	  else
	    {
	      int opLeast, keyLeast;
	      opLeast = isComparable (TermOp (typ1), TermOp (typ2));
	      if (!opLeast)
		return 0;
	      if (typ1->helper.fcall)
		{
		  keyLeast = isTermEqual (TermKey (typ1), TermKey (typ2));
		  return keyLeast;
		}
	      else
		{
		  keyLeast = isComparable (TermKey (typ1), TermKey (typ2));
		  return keyLeast;
		}
	    }
	}
      else
	{
	  int op1Least = isComparable (TermOp1 (typ1), TermOp1 (typ2));
	  if (!op1Least)
	    return 0;
	  int op2Least = isComparable (TermOp2 (typ1), TermOp2 (typ2));
	  return op2Least;
	}
    }
  return 1;
}

// compare types in a list
int
compareTypelist (Term type, Termlist typelist)
{
  Termlist tl;
  for (tl = typelist; tl != NULL; tl = tl->next)
    {
      if (isComparable (type, tl->term))
	{
	  return 1;
	}
    }
  return 0;
}

int
compareTypeTermlist (Term type, Termlist termlist)
{
  Termlist tl;
  for (tl = termlist; tl != NULL; tl = tl->next)
    {
      if (isComparable (type, getTermType (tl->term)))
	{
	  return 1;
	}
    }
  return 0;
}

//find the largest common subtype of two given types
Term
upperCommonSubtype (Term typ1, Term typ2)
{
  if (realTermLeaf (typ1) || realTermLeaf (typ2))
    {
      if (isSubtype (typ1, typ2))
	return typ1;
      else if (isSubtype (typ2, typ1))
	return typ2;
      else
	return NULL;
    }
  else				//both composed
    {
      if (typ1->type != typ2->type)
	return NULL;
      if (realTermEncrypt (typ1))
	{
	  if (typ1->helper.fcall != typ2->helper.fcall)
	    return NULL;
	  else
	    {
	      Term opLeast, keyLeast;
	      opLeast = upperCommonSubtype (TermOp (typ1), TermOp (typ2));
	      if (!opLeast)
		return NULL;
	      if (typ1->helper.fcall)
		{
		  keyLeast =
		    isTermEqual (TermKey (typ1),
				 TermKey (typ2)) ? TermKey (typ1) : NULL;
		  if (!keyLeast)
		    return NULL;
		  return makeTermFcall (opLeast, keyLeast);
		}
	      else
		{
		  keyLeast =
		    upperCommonSubtype (TermKey (typ1), TermKey (typ2));
		  if (!keyLeast)
		    return NULL;
		  return makeTermEncrypt (opLeast, keyLeast);
		}
	    }
	}
      else
	{
	  Term op1Least = upperCommonSubtype (TermOp1 (typ1), TermOp1 (typ2));
	  if (!op1Least)
	    return NULL;
	  Term op2Least = upperCommonSubtype (TermOp2 (typ1), TermOp2 (typ2));
	  if (!op2Least)
	    return NULL;
	  return makeTermTuple (op1Least, op2Least);
	}
    }
  return NULL;
}

//find the most general type of two given types
Term
mostGeneralType (Term typ1, Term typ2)
{
  if (realTermLeaf (typ1) || realTermLeaf (typ2))
    {
      if (isSubtype (typ1, typ2))
	return typ2;
      else if (isSubtype (typ2, typ1))
	return typ1;
      else
	return TERM_Ticket;
    }
  else				//both composed
    {
      if (typ1->type != typ2->type)
	return NULL;
      if (realTermEncrypt (typ1))
	{
	  if (typ1->helper.fcall != typ2->helper.fcall)
	    return TERM_Ticket;
	  else
	    {
	      Term opGen;
	      opGen = mostGeneralType (TermOp (typ1), TermOp (typ2));
	      if (typ1->helper.fcall)
		{
		  if (!isTermEqual (TermKey (typ1), TermKey (typ2)))
		    return TERM_Ticket;
		  return makeTermFcall (opGen, TermKey (typ1));
		}
	      else
		{
		  Term keyGen;
		  keyGen = mostGeneralType (TermKey (typ1), TermKey (typ2));
		  return makeTermEncrypt (opGen, keyGen);
		}
	    }
	}
      else
	{
	  Term op1Gen = mostGeneralType (TermOp1 (typ1), TermOp1 (typ2));
	  Term op2Gen = mostGeneralType (TermOp2 (typ1), TermOp2 (typ2));
	  return makeTermTuple (op1Gen, op2Gen);
	}
    }
}
