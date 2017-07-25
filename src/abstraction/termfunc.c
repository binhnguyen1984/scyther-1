/*
 * utility.c
 *
 *  Created on: May 14, 2014
 *      Author: nguyen
 */
#include "termfunc.h"
int norm;			//
//checking
int
realTermFcall (Term t)
{
  return t->type == ENCRYPT && t->helper.fcall;
}

//long-term keys

int
isPublicKeyConstructor (Term t)
{
  return isTermEqual (t, TERM_PK);
}

int
isPrivateKeyConstructor (Term t)
{
  return isTermEqual (t, TERM_SK);
}

int
isLTSharedKeyConstructor (Term t)
{
  return isTermEqual (t, TERM_K);
}

int
isLTKeyConstructor (Term t)
{
  return isTermEqual (t, TERM_PK) || isTermEqual (t, TERM_K)
    || isTermEqual (t, TERM_SK);
}

int
isLTSharedKey (Term t)
{
  return realTermFcall (t) && isLTSharedKeyConstructor (TermKey (t));
}

int
isPublicKey (Term t)
{
  return realTermFcall (t) && isPublicKeyConstructor (TermKey (t));
}

int
isPrivateKey (Term t)
{
  return realTermFcall (t) && isPrivateKeyConstructor (TermKey (t));
}

int
isLTKey (Term t)
{
  return realTermFcall (t) && isLTKeyConstructor (TermKey (t));
}

int
isTicketVariable (Term t)
{
  return realTermVariable (t) && isTermEqual (getTermType (t), TERM_Ticket);
}

int
termEqual (Term t, Term u)
{
  return t->type == u->type && isTermEqual (t, u);
}

int
termInList (Termlist tl, const Term term)
{
  if (term == NULL)
    {
      return 0;
    }
  while (tl != NULL)
    {
      if (termEqual (tl->term, term))
	{
	  return 1;
	}
      tl = tl->next;
    }
  return 0;
}

Term
getTermInList (Termlist tl, const Term term)
{
  if (term == NULL)
    {
      return NULL;
    }
  while (tl != NULL)
    {
      if (tl->term->type == term->type && isTermEqual (tl->term, term))
	{
	  return tl->term;
	}
      tl = tl->next;
    }
  return NULL;
}

int
hasTicketVariable (Term t)
{
  if (isTicketVariable (t))
    return 1;
  if (realTermEncrypt (t))
    {
      //hashes
      if (t->helper.fcall)
	return hasTicketVariable (TermOp (t));
      else
	return hasTicketVariable (TermOp (t))
	  && hasTicketVariable (TermKey (t));
    }
  if (realTermTuple (t))
    return hasTicketVariable (TermOp1 (t)) && hasTicketVariable (TermOp2 (t));
  return false;
}


//check if term occurs in accessible positions
int
containTermInPlain (Term u, Term t)
{
  if (realTermLeaf (t))
    {
      return isTermEqual (u, t);
    }
  else if (realTermEncrypt (t))
    {
      return containTermInPlain (u, TermOp (t));
    }
  else
    return containTermInPlain (u, TermOp1 (t))
      || containTermInPlain (u, TermOp2 (t));
}

//check if an agent with different name occurs in accessible positions
int
containDiffAgent (Term t, Term agent)
{
  if (realTermLeaf (t))
    {
      if (isAgentVariable (t))
	{
	  if (agent != NULL)
	    return !isTermEqual (t, agent);
	  else
	    return 1;
	}
      else
	return 0;
    }
  else if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	return containDiffAgent (TermOp (t), agent);
      return containDiffAgent (TermOp (t), agent)
	|| containDiffAgent (TermKey (t), agent);
    }
  else
    return containDiffAgent (TermOp1 (t), agent)
      || containDiffAgent (TermOp2 (t), agent);
}

//check if t contains agent variables from u
int
containDiffAgentFromTerm (Term t, Term u)
{
  if (realTermLeaf (t))
    {
      if (isAgentVariable (t))
	{
	  return isSubterm (t, u);
	}
      else
	return 0;
    }
  else if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	return containDiffAgentFromTerm (TermOp (t), u);
      return containDiffAgentFromTerm (TermOp (t), u)
	|| containDiffAgentFromTerm (TermKey (t), u);
    }
  else
    return containDiffAgentFromTerm (TermOp1 (t), u)
      || containDiffAgentFromTerm (TermOp2 (t), u);
}

int
isAgentVariable (Term t)
{
  return termInList (t->stype, TERM_Agent);
}

//check if t contains agent variables from a termlist
int
containDiffAgentlist (Term t, Termlist agents)
{
  if (realTermLeaf (t))
    {
      if (isAgentVariable (t))
	{
	  if (agents != NULL)
	    return !termInList (agents, t);
	  else
	    return 1;
	}
      else
	return 0;
    }
  else if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	return containDiffAgentlist (TermOp (t), agents);
      return containDiffAgentlist (TermOp (t), agents)
	|| containDiffAgentlist (TermKey (t), agents);
    }
  else
    return containDiffAgentlist (TermOp1 (t), agents)
      || containDiffAgentlist (TermOp2 (t), agents);
}


Term
containLTSharedKeyInPlain (Term t)
{
  if (realTermLeaf (t))
    return 0;
  else if (realTermTuple (t))
    {
      Term key = containLTSharedKeyInPlain (TermOp1 (t));
      if (key == NULL)
	key = containLTSharedKeyInPlain (TermOp2 (t));
      return key;
    }
  else
    {
      if (t->helper.fcall && isLTSharedKeyConstructor (TermKey (t)))
	return t;
      else
	return containLTSharedKeyInPlain (TermOp (t));
    }
}

//check if the set of atoms and variables of u is included in that of t
int avInclusion(Term t, Term u)
{
	if(realTermLeaf(u))
	{
		return isSubterm(u,t);
	}
	else if(realTermEncrypt(u))
	{
		if(u->helper.fcall)
			return avInclusion(t, TermOp(u));
		else return avInclusion(t, TermOp(u))&&avInclusion(t,TermKey(u));
	}
	else return avInclusion(t, TermOp1(u)) && avInclusion(t, TermOp2(u));
}

//check if the sets of atoms and variables of the two terms are overlapped
int
avOverlap (Term t, Term u)
{
  Termlist avt = extractAV (NULL, t, 0);
  Termlist avu = extractAV (NULL, u, 0);
  Termlist tl;
  int result = 0;
  for (tl = avt; tl != NULL; tl = tl->next)
    if (termInList (avu, tl->term))
      {
	result = 1;
	break;
      }
  termlistDelete (avt);
  termlistDelete (avu);
  return result;
}

int
avOverlaptermInList (Term t, Termlist tl)
{
  for (; tl != NULL; tl = tl->next)
    if (avOverlap (t, tl->term))
      return 1;
  return 0;
}

//functions for terms
Termlist
termlistAddNewTerm (Termlist tl, Term t)
{
  if (!termInList (tl, t))
    return termlistAdd (tl, t);
  return tl;
}

Termlist
termlistCheckandAppend (Termlist tl1, Termlist tl2)
{
  Termlist temp = tl2;
  while (temp != NULL)
    {
      if (!termInList (tl1, temp->term))
	tl1 = termlistAppend (tl1, temp->term);
      temp = temp->next;
    }
  return tl1;
}


//check if t is deeper than u. Note that we assume the types of t and u are comparable
int
isDeeper (Term t, Term u)
{
  if (realTermLeaf (t))
    return 0;
  else if (realTermEncrypt (t))
    {
      if (realTermLeaf (u))
	return 1;
      if (t->helper.fcall)
	return isDeeper (TermOp (t), TermOp (u));
      else
	return isDeeper (TermKey (t), TermKey (u))
	  || isDeeper (TermOp (t), TermOp (u));
    }
  else
    {
      if (realTermLeaf (u))
	return 1;
      return isDeeper (TermOp1 (t), TermOp1 (u))
	|| isDeeper (TermOp2 (t), TermOp2 (u));
    }
}

void
termDestroy (Term t)
{
  if (t != NULL)
    {
      if (realTermLeaf (t))
	{
	  if (t->type == VARIABLE)
	    {
	      symbolDelete (t->left.symb);
	      free (t);
	    }
	  return;
	}
      else if (realTermEncrypt (t))
	{
	  termDestroy (TermOp (t));
	  termDestroy (TermKey (t));
	}
      else
	{
	  termDestroy (TermOp1 (t));
	  termDestroy (TermOp2 (t));
	}
      free (t);
    }
}

//delete term's type
void
deleteTermtype (Term t)
{
  Termlist tl = (Termlist) t->stype;
  termlistDelete (tl);
  t->stype = NULL;
}

//delete a term from termlist at the current pointer, return the pointer pointing to the next element
Termlist
termlistRemoveTerm (Termlist tl)
{
  if (tl == NULL)
    return tl;
  Termlist tmp = tl;
  if (tl->prev == NULL)
    {
      tl = tl->next;
      if (tl)
	tl->prev = NULL;
      free (tmp);
      return tl;
    }
  else
    {
      tl->prev->next = tl->next;
      if (tl->next != NULL)
	tl->next->prev = tl->prev;
      tl = tl->next;
      free (tmp);
      return tl;
    }
}

Termlist
termlistFindAndRemove (Termlist tl, Term t)
{
  if (tl == NULL)
    return tl;
  Termlist tmp = tl;
  while (tmp != NULL)
    {
      if (isTermEqual (tmp->term, t))
	{
	  Termlist tmp1 = tmp;
	  if (tmp->prev != NULL)
	    {
	      tmp->prev->next = tmp->next;
	    }
	  else
	    {
	      tl = tmp->next;
	      if (tl)
		tl->prev = NULL;
	    }
	  if (tmp->next != NULL)
	    tmp->next->prev = tmp->prev;
	  tmp = tmp->next;
	  free (tmp1);
	  continue;
	}
      tmp = tmp->next;
    }
  return tl;
}


//extract non-agent atoms and variables in clear
Termlist
extractClearAVFromTerm (Termlist av, Term t)
{
  if (realTermLeaf (t))
    {
      return termlistAddNewTerm (av, t);
    }
  else if (realTermTuple (t))
    {
      av = extractClearAVFromTerm (av, TermOp1 (t));
      av = extractClearAVFromTerm (av, TermOp2 (t));
      return av;
    }
  return av;
}

Termlist
split (Term t)
{
  if (t->type != TUPLE)
    {
      Termlist tl = makeTermlist ();
      tl->term = t;
      tl->next = NULL;
      tl->prev = NULL;
      return tl;
    }
  else
    {
      Termlist tl1 = split (TermOp1 (t));
      Termlist tl2 = split (TermOp2 (t));
      Termlist tlbig = termlistCheckandAppend (tl1, tl2);
      return tlbig;
    }
}

Termlist
turn_tuple_to_termlist (Term t)
{
  if (t->type != TUPLE)
    {
      return termlistAdd (NULL, t);
    }
  else
    {
      Termlist tl1 = turn_tuple_to_termlist (TermOp1 (t));
      Termlist tl2 = turn_tuple_to_termlist (TermOp2 (t));
      Termlist tlbig = termlistConcat (tl1, tl2);
      return tlbig;
    }
}

Term
turn_termlist_to_tuple (Termlist tl)
{
  if (tl == NULL)
    return NULL;
  if (tl->next == NULL)
    return tl->term;
  Term newterm = makeTermTuple (tl->term, turn_termlist_to_tuple (tl->next));
  return newterm;
}

//printing

void
printTerm (Term t)
{
  if (t == NULL)
    return;
  if (realTermLeaf (t))
    eprintf ("%s", t->left.symb->text);
  else if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	{
	  eprintf ("%s", t->right.key->left.symb->text);
	  if (!realTermTuple (TermOp (t)))
	    {
	      eprintf ("(");
	      printTerm (TermOp (t));
	      eprintf (")");
	    }
	  else
	    printTerm (TermOp (t));
	}
      else
	{
	  eprintf ("{");
	  printTerm (TermOp (t));
	  eprintf ("}");
	  printTerm (TermKey (t));
	}
    }
  else
    {
      eprintf ("(");
      Term op1 = TermOp1 (t);
      Term op2 = TermOp2 (t);
      if (realTermTuple (op1))
	{
	  eprintf ("(");
	  printTerm (op1);
	  eprintf (")");
	}
      else
	printTerm (op1);
      eprintf (",");
      printTerm (op2);
      eprintf (")");
    }
}

void
printTermlist (Termlist tl)
{
  while (tl != NULL)
    {
      printTerm (tl->term);
      eprintf (", ");
      tl = tl->next;
    }
  printf ("\n");
}

// matching: check if the term matches the pattern. If so, update the subst of each variable in the pattern.
//note that this is specific to patterns in the abstraction as we use a number of their properties, e.g.,
//linear patterns, disjoint variable sets.
int
patternMatching (Term pat, Term term)
{
  if (realTermLeaf (pat))
    {
      if (pat->type == VARIABLE)
	{
	  pat->subst = term;
	  return true;
	}
      return isTermEqual (pat, term);
    }
  else if (realTermEncrypt (pat))
    {
      //the pattern is a hash, we obmit the hash function
      if (pat->helper.fcall)
	return patternMatching (TermOp (pat), TermOp (term));
      //the pattern is an encryption
      return patternMatching (TermOp (pat), TermOp (term)) &&
	patternMatching (TermKey (pat), TermKey (term));
    }
  else
    return patternMatching (TermOp1 (pat), TermOp1 (term)) &&
      patternMatching (TermOp2 (pat), TermOp2 (term));
  return false;
}

//check if t is a subterm of u
int
isSubterm (Term t, Term u)
{
  if (u == NULL || t == NULL)
    return 0;
  if (t->type == u->type && isTermEqual (t, u))
    return 1;
  if (realTermLeaf (u))
    return 0;
  if (realTermEncrypt (u))
    {
      if (u->helper.fcall)
	return isSubterm (t, TermOp (u));
      else
	return isSubterm (t, TermOp (u)) || isSubterm (t, TermKey (u));
    }
  return isSubterm (t, TermOp1 (u)) || isSubterm (t, TermOp2 (u));
}

//check if t is a subterm of some in tl
int
isSubtermInTermlist (Term t, Termlist tl)
{
  while (tl != NULL)
    {
      if (isSubterm (t, tl->term))
	return 1;
      tl = tl->next;
    }
  return 0;
}

//add fields from term to list
Termlist
addField2Termlist (Termlist tl, Term t)
{
  if (realTermTuple (t))
    {
      tl = addField2Termlist (tl, TermOp1 (t));
      tl = addField2Termlist (tl, TermOp2 (t));
    }
  else
    tl = termlistAdd (tl, t);
  return tl;
}

//check if all terms in list are subterms of t
int
isTermlistSubterm (Term t, Termlist tl)
{
  if (tl == NULL || tl->term == NULL)
    return 0;
  while (tl != NULL)
    {
      if (!isSubterm (tl->term, t))
	return 0;
      tl = tl->next;
    }
  return 1;
}

//check if some term in list are subterms of t
int
isSomeTermlistSubterm (Term t, Termlist tl)
{
  while (tl != NULL)
    {
      if (isSubterm (tl->term, t))
	return 1;
      tl = tl->next;
    }
  return 0;
}


int
conjuntNonEmpty (Termlist tl1, Termlist tl2)
{
  while (tl1 != NULL)
    {
      if (termInList (tl2, tl1->term))
	return 1;
      tl1 = tl1->next;
    }
  return 0;
}

//check if t occurs in accessible position in u
int
appearInAccessible (Term t, Term bigterm)
{
  if (bigterm == NULL || t == NULL)
    return 0;
  if (isTermEqual (t, bigterm))
    return 1;
  if (realTermLeaf (bigterm))
    return 0;
  if (realTermEncrypt (bigterm))
    {
      return appearInAccessible (t, TermOp (bigterm));
    }
  return appearInAccessible (t, TermOp1 (bigterm))
    || appearInAccessible (t, TermOp2 (bigterm));
}

//check if t occurs as a component of u
int
appearInTuple (Term t, Term u)
{
  if (isTermEqual (t, u))
    return 1;
  if (realTermTuple (u))
    return appearInTuple (t, TermOp1 (u)) || appearInTuple (t, TermOp2 (u));
  return 0;
}

//check if t occurs in plaintext of some term in tl
int
plaintextAppearInTermlist (Term t, Termlist tl)
{
  while (tl != NULL)
    {
      if (appearInAccessible (t, tl->term))
	return 1;
      tl = tl->next;
    }
  return 0;
}

//check if some term in tl occurs in plaintext of t
int
plaintextTermlistInTerm (Term t, Termlist tl)
{
  while (tl != NULL)
    {
      if (appearInAccessible (tl->term, t))
	return 1;
      tl = tl->next;
    }
  return 0;
}

//check if t occurs in plaintext of some term in tl (pattern)
int
plaintextAppearInTermlistPat (Term t, Termlist tl)
{
  while (tl != NULL)
    {
      if (appearInAccessible (t, tl->term->subst))
	return 1;
      tl = tl->next;
    }
  return 0;
}


//check if a term contains a new atom(variable) in plaintext which is not in the given list
int
containNewAVInPlain (Term t, Termlist tl)
{
  if (realTermLeaf (t))
    {
      if (!termInList (tl, t))
	return 1;
      return 0;
    }
  else if (realTermEncrypt (t))
    return containNewAVInPlain (TermOp (t), tl);
  return containNewAVInPlain (TermOp1 (t), tl)
    || containNewAVInPlain (TermOp2 (t), tl);
}

//return a list of terms that do not occur in t in plaintext positions
Termlist
termlistNotInPlaintext (Term t, Termlist tl)
{
  Termlist tmp = NULL;
  while (tl != NULL)
    {
      if (!appearInAccessible (tl->term, t))
	tmp = termlistAdd (tmp, tl->term);
      tl = tl->next;
    }
  return tmp;
}

//return t\sigma. where \sigma is the substitution defined in the subst field
//note that non-variable term can get substituted, if the subst is defined for that term
Term
substitutedTerm (Term t)
{
  if (realTermLeaf (t))
    {
      if (t->type == VARIABLE && t->subst != NULL)
	{
	  {
	    t->subst->abst = t->abst;
	    return t->subst;
	  }
	}
      return t;
    }
  else if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	{
	  Term op = substitutedTerm (TermOp (t));
	  return makeTermFcall (op, TermKey (t));
	}
      else
	{
	  Term op = substitutedTerm (TermOp (t));
	  Term key = substitutedTerm (TermKey (t));
	  return makeTermEncrypt (op, key);
	}
    }
  else
    {
      Term op1 = substitutedTerm (TermOp1 (t));
      Term op2 = substitutedTerm (TermOp2 (t));
      return makeTermTuple (op1, op2);
    }
}

//get rid of NIL term in a term
Term
normalizeTerm (Term t)
{
  if (t == NULL)
    return NULL;
  if (realTermLeaf (t))
    {
      return t;
    }
  if (realTermEncrypt (t))
    {
      //hashes
      if (t->helper.fcall)
	{
	  Term plain = normalizeTerm (TermOp (t));
	  if (isTermEqual (plain, TERM_NIL))
	    return TERM_NIL;
	  else
	    {
	      TermOp (t) = plain;
	      return t;
	    }
	}
      //encryptions
      else
	{
	  Term plain = normalizeTerm (TermOp (t));
	  Term key = normalizeTerm (TermKey (t));
	  if (isTermEqual (plain, TERM_NIL))
	    return TERM_NIL;
	  else if (isTermEqual (key, TERM_NIL))
	    return plain;
	  else
	    {
	      TermOp (t) = plain;
	      TermKey (t) = key;
	      return t;
	    }
	}
    }
  //tuples
  else
    {
      Term op1 = normalizeTerm (TermOp1 (t));
      Term op2 = normalizeTerm (TermOp2 (t));
      if (isTermEqual (op1, TERM_NIL))
	return op2;
      else if (isTermEqual (op2, TERM_NIL))
	return op1;
      TermOp1 (t) = op1;
      TermOp2 (t) = op2;
      return t;
    }
}

int
depth (Term t)
{
  if (realTermLeaf (t))
    return 1;
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	return 1 + depth (TermOp (t));
      return 1 + max (depth (TermOp (t)), depth (TermKey (t)));
    }
  return 1 + max (depth (TermOp1 (t)), depth (TermOp2 (t)));
}

int
size_of_term (Term t)
{
  if (realTermLeaf (t))
    return 1;
  if (realTermEncrypt (t))
    return size_of_term (TermOp (t)) + 1 + size_of_term (TermKey (t));
  return size_of_term (TermOp1 (t)) + size_of_term (TermOp2 (t));
}

int
size_of_type (Term t)
{
  if (realTermLeaf (t))
    {
      if (t->type == GLOBAL)
	return 1;
      else
	{
	  Term type = getTermType (t);
	  if (isTermEqual (type, TERM_Ticket))
	    return 3;
	  else
	    return 2;
	}
    }
  if (realTermEncrypt (t))
    return size_of_type (TermOp (t)) * size_of_type (TermKey (t));
  return size_of_type (TermOp1 (t)) * size_of_type (TermOp2 (t));

}

Term
smallestTermInTuple (Term t)
{
  Termlist tl = tuple_to_termlist (t);
  int term_s, type_s;
  Term smallest = tl->term;
  term_s = size_of_term (tl->term);
  type_s = size_of_type (tl->term);
  tl = tl->next;
  while (tl != NULL)
    {
      if (term_s > 1 || realTermLeaf (tl->term))
	{
	  int te_s = size_of_term (tl->term);
	  int ty_s = size_of_type (tl->term);
	  if (te_s < term_s || ty_s < type_s)
	    {
	      smallest = tl->term;
	      term_s = te_s;
	      type_s = ty_s;
	    }
	}
      tl = tl->next;
    }
  return smallest;
}

Term
smallestTermInPatVars (Termlist tl)
{
  int term_s, type_s;
  Term smallest = tl->term;
  term_s = size_of_term (tl->term->subst);
  type_s = size_of_type (tl->term->subst);
  tl = tl->next;
  while (tl != NULL)
    {
      if (term_s > 1 || realTermLeaf (tl->term->subst))
	{
	  int te_s = size_of_term (tl->term->subst);
	  int ty_s = size_of_type (tl->term->subst);
	  if (te_s < term_s || ty_s < type_s)
	    {
	      smallest = tl->term;
	      term_s = te_s;
	      type_s = ty_s;
	    }
	}
      tl = tl->next;
    }
  return smallest;
}

Termlist
extractAV (Termlist av, Term t, int agent_only)
{
  if (realTermLeaf (t))
    {
      if (agent_only == isAgentVariable (t))
	{
	  return termlistAddNewTerm (av, t);
	}
      return av;
    }
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	{
	  av = extractAV (av, TermOp (t), agent_only);
	}
      else
	{
	  av = extractAV (av, TermOp (t), agent_only);
	  av = extractAV (av, TermKey (t), agent_only);
	}
      return av;
    }
  av = extractAV (av, TermOp1 (t), agent_only);
  av = extractAV (av, TermOp2 (t), agent_only);
  return av;
}

Termlist
extractVariables (Termlist vars, Term t)
{
  if (realTermLeaf (t))
    {
      if (t->type == VARIABLE)
	{
	  return termlistAddNewTerm (vars, t);
	}
      return vars;
    }
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	{
	  vars = extractVariables (vars, TermOp (t));
	}
      else
	{
	  vars = extractVariables (vars, TermOp (t));
	  vars = extractVariables (vars, TermKey (t));
	}
      return vars;
    }
  vars = extractVariables (vars, TermOp1 (t));
  vars = extractVariables (vars, TermOp2 (t));
  return vars;
}
