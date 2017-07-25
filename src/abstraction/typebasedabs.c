#include "typebasedabs.h"

extern int globalError;
extern Term TERM_NIL;
extern int nilfree;
extern Eqlist eql;
extern Termlist topconst;
int eql_changed;
static System sys;
void
typebasedInit (System mysys)
{
  sys = mysys;
  resetAbs (sys);
}

Eqlist
checkPatternDisjointness1 (Term typ1, Eqlist Ef)
{
  Eqlist remain = Ef;
  Eqlist l = NULL;
  while (remain != NULL)
    {
      Term typ2 = getTermType (remain->eq->left);
      int isCompr = isComparable (typ1, typ2);
      if (isCompr)
	{
	  l = equationlistAdd (l, remain->eq);
	}
      remain = remain->next;
    }
  return l;
}

int
checkPatternDisjointness ()
{
  Eqlist Ef = eql;
  while (Ef != NULL)
    {
      Eqlist remain = Ef->next;
      Term type = getTermType (Ef->eq->left);
      if (checkPatternDisjointness1 (type, remain))
	return false;
      Ef = Ef->next;
    }
  return true;
}

//check if the term is of the form <f(?), ..., f(?)>
int
homomorphicTuple (Term t)
{
  if (t->type != TUPLE)
    {
      if (!t->abst)
	return false;
      return true;
    }
  return homomorphicTuple (TermOp1 (t)) && homomorphicTuple (TermOp2 (t));
}

int
homomorphicList (Termlist tl)
{
  while (tl != NULL)
    {
      if (!tl->term->abst)
	return false;
      tl = tl->next;
    }
  return true;
}



//update types for each variable in term t so that t's type is type.
//note that we assume t and type have the same structure
void
updateTypeForTerm (Term t, Term type)
{
  if (t->type == VARIABLE)
    {
      deleteTermtype (t);
      t->stype = termlistAdd (t->stype, type);
    }
  else if (realTermEncrypt (t))
    {
      updateTypeForTerm (TermOp (t), TermOp (type));
      if (!t->helper.fcall)
	updateTypeForTerm (TermKey (t), TermKey (type));
    }
  else
    {
      updateTypeForTerm (TermOp1 (t), TermOp1 (type));
      updateTypeForTerm (TermOp2 (t), TermOp2 (type));
    }
}

void
updateTypeForPattern (Equation eq, Term type)
{
  //updateTypeForTerm(eq->left,type);
  if (!realTermLeaf (eq->left))
    deleteTermtype (eq->left);
  eq->left->stype = NULL;
  eq->left->stype = termlistAdd (NULL, type);
}


Term
mostGeneralTypeForEqlist (Eqlist eql, Term type)
{
  Term genType = type;
  while (eql != NULL)
    {
      Term u = ((Termlist) eql->eq->left->stype)->term;
      genType = mostGeneralType (genType, u);
      eql = eql->next;
    }
  return genType;
}

Equation
resolveDisjointnessConflict (Term type)
{
  Eqlist conflict = checkPatternDisjointness1 (type, eql);
  Term newtype = mostGeneralTypeForEqlist (conflict, type);
  //we remove equations that are deeper than the type
  if (conflict == NULL)
    return NULL;
  Eqlist tmp;
  for (tmp = conflict; tmp != NULL; tmp = tmp->next)
    if (isDeeper (tmp->eq->left, newtype))
      {
	eql = removeEquationFromList (eql, tmp->eq);
	eql_changed = 1;
	conflict = removeEquationFromListShallow (conflict, tmp->eq);
      }
  if (conflict == NULL)
    return NULL;
  //for other equations, we try to fix them
  Equation eq = conflict->eq;
  tmp = conflict->next;
  if (tmp != NULL)
    eql_changed = 1;
  while (tmp != NULL)
    {
      //if this is a user-defined equation, then it overwrites those created by the system
      if ((tmp->eq->type == eq->type && abstCost (eq) > abstCost (tmp->eq))
	  || (tmp->eq->type > eq->type))
	{
	  Equation eqtmp = eq;
	  eq = tmp->eq;
	  if (!eq->type)
	    {
	      globalError++;
	      eprintf
		("warning: equation at line %d has been removed due to violation of disjointness\n",
		 tmp->eq->line);
	      globalError--;
	    }
	  eql = removeEquationFromList (eql, eqtmp);
	}
      else
	eql = removeEquationFromList (eql, tmp->eq);
      tmp = tmp->next;
    }
  if (eq->type < SYSTEM_TRIVIAL)	//user-defined equation
    {
      globalError++;
      eprintf ("warning: equation's type at line %d has been generalized to ",
	       tmp->eq->line);
      printTerm (newtype);
      eprintf ("\n");
      globalError--;
    }
  updateTypeForPattern (eq, newtype);
  deleteEqlistShallow (conflict);
  return eq;
}

//abstraction
Term
frec (Term t)
{
  Term replacef (Term t)
  {
    if (t->abst)
      return frec (t);
    if (realTermLeaf (t))
      {
	return t;
      }
    if (realTermEncrypt (t))
      {
	TermOp (t) = replacef (TermOp (t));
	//encryption
	if (!t->helper.fcall)
	  {
	    TermKey (t) = replacef (TermKey (t));
	  }
	return t;
      }
    else
      {
	TermOp1 (t) = replacef (TermOp1 (t));
	TermOp2 (t) = replacef (TermOp2 (t));
	return t;
      }
  }
  if (isLTKey (t))
    {
      return t;
    }
  Term termtype = getTermType (t);
  if (isTermEqual (termtype, TERM_Ticket))
    {
      return t;
    }
  Equation eq = findMatchingPattern (termtype, eql);
  if (eq == NULL)
    {
      //if the type is a hash type and the outer-most constructor is in topconst list,
      //then due to the syntactic criterion for authentication, the corresponding pattern must be homomorphic
      Term key;
      if (realTermEncrypt (termtype) && termtype->helper.fcall
	  && inTermlist (topconst, key = TermKey (termtype)))
	{
	  eq = createTicketHomomorphicPatForHash (key);
	  eql = equationlistAdd (eql, eq);
	}
      else
	{
	  eq = resolveDisjointnessConflict (termtype);
	  if (eq == NULL)
	    {
	      eq = createHomoEquationFromType (termtype);
	      eql = equationlistAdd (eql, eq);
	    }
	}
    }
  patternMatching (eq->left, t);
  Term subst = substitutedTerm (eq->right);
  return normalizeTerm (replacef (subst));
}

/* this is not necessary as protocol terms are always in normal form
Term abstractTerm(Term t){
	return frec(normalize(t, TERM_NIL));
}
*/
//checking whether the function specification is a type-based one

int
checkWelldefinednessForLongtermKey (Term left, Term right, int isAsym)
{
  if (!isTermEqual (left, right))
    return false;
  Term arg = TermOp (right);
  //asymmetric keys
  if (isAsym)
    {
      if (arg->type == VARIABLE || arg->abst)
	return true;
      return false;
    }
  //shared key
  else
    return arg->type == TUPLE &&
      TermOp1 (arg)->type == VARIABLE &&
      TermOp2 (arg)->type == VARIABLE &&
      TermOp1 (arg)->abst && TermOp2 (arg)->abst;
}

int
checkWelldefinednessForTuple (Term left, Term right)
{
  Termlist tlright = split (right);
  Termlist tlleft = split (left);
  if (!isTermlistEqual (tlright, tlleft))
    return false;
  return homomorphicList (tlright);
}

int
checkWelldefinednessForHash (Term left, Term right)
{
  Termlist plaintexts = NULL;
  int processOneTerm (Term left, Term right)
  {
    if (realTermLeaf (right))
      {
	if (!right->abst)
	  {
	    if (isTermEqual (right, TERM_NIL))
	      return true;
	    {
	      eprintf ("Fail because ");
	      printTerm (right);
	      eprintf (" is not abstracted\n");
	      return false;
	    }
	  }
      }
    while (right->type == ENCRYPT && !right->abst)
      {
	if (!isTermEqual (TermKey (left), TermKey (right)))
	  {
	    eprintf ("Fail because ");
	    printTerm (TermOp (left));
	    eprintf ("  is not equal to ");
	    printTerm (TermOp (right));
	    eprintf ("\n");
	    return false;
	  }
	right = TermOp (right);
      }
    Termlist tl = split (right);
    if (!homomorphicList (tl))
      {
	eprintf ("fail because ");
	printTermlist (tl);
	eprintf (" is not homomorphic\n");
	return false;
      }
    plaintexts = termlistCheckandAppend (plaintexts, tl);
    return true;
  }
  Termlist tl = split (right);
  while (tl != NULL)
    {
      if (!processOneTerm (left, tl->term))
	return false;
      tl = tl->next;
    }
  return termlistContained (split (TermOp (left)), plaintexts);
}

int
checkWelldefinednessForEnc (Term left, Term right)
{
  Termlist plaintexts = NULL;
  Termlist keys = NULL;
  int processOneTerm (Term right)
  {
    if (right->type == ENCRYPT)
      {
	if (right->abst)
	  plaintexts = termlistCheckandAppend (plaintexts, split (right));
	else
	  {
	    if (homomorphicTuple (TermOp (right))
		&& homomorphicTuple (TermKey (right)))
	      {
		plaintexts =
		  termlistCheckandAppend (plaintexts, split (TermOp (right)));
		keys = termlistCheckandAppend (keys, split (TermKey (right)));
		return true;
	      }
	    return false;
	  }
      }
    if (right->type == TUPLE)
      {
	if (homomorphicTuple (right))
	  {
	    plaintexts = termlistCheckandAppend (plaintexts, split (right));
	    return true;
	  }
	return false;
      }
    if (!right->abst)
      return false;
    plaintexts = termlistCheckandAppend (plaintexts, split (right));
    return true;
  }
  Termlist tl = split (right);
  while (tl != NULL)
    {
      if (!processOneTerm (tl->term))
	return false;
      tl = tl->next;
    }

  return termlistContained (split (TermKey (left)), keys) &&
    isTermlistEqual (split (TermOp (left)), plaintexts);
}

int
checkWelldefinednessForVariable (Term left, Term right)
{
  if (right == TERM_NIL)
    {
      int result = !(inTermlist (left->stype, TERM_Agent)
		     || inTermlist (left->stype, TERM_Ticket));
      if (!result)
	eprintf
	  ("The type of variable %s should be neither Agent nor Ticket\n",
	   TermSymb (left)->text,
	   TermSymb (((Termlist) left->stype)->term)->text);
      return result;
    }
  return right == left;
}

int
checkWelldefinednessForEquation (Term left, Term right)
{
  if (realTermVariable (left))
    return checkWelldefinednessForVariable (left, right);
  if (realTermEncrypt (left))
    {
      if (left->helper.fcall)
	{
	  if (isLTSharedKeyConstructor (TermKey (left)))
	    return checkWelldefinednessForLongtermKey (left, right, 1);
	  if (isLTSharedKeyConstructor (TermKey (left)))
	    return checkWelldefinednessForLongtermKey (left, right, 0);
	  return checkWelldefinednessForHash (left, right);
	}
      else
	return checkWelldefinednessForEnc (left, right);
    }
  if (realTermTuple (left))
    return checkWelldefinednessForTuple (left, right);
  return true;
}

int
checkWelldefinednessFortypebasedabs ()
{
  Eqlist tmp = eql;
  while (tmp->eq != NULL)
    {
      if (!checkWelldefinednessForEquation (tmp->eq->left, tmp->eq->right))
	{
	  globalError++;
	  eprintf ("error: Equation ");
	  printEquation (tmp->eq->left, tmp->eq->right);
	  eprintf (" is not well-defined\n");
	  globalError--;
	  return false;
	}
      tmp = tmp->next;
    }
  return true;
}


int
HasMsgVarForNonPair (Term t)
{
  if (realTermLeaf (t))
    return t->type == VARIABLE && inTermlist (t->stype, TERM_Ticket);
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	return HasMsgVarForNonPair (TermOp (t));
      return HasMsgVarForNonPair (TermOp (t))
	|| HasMsgVarForNonPair (TermKey (t));
    }
  return false;
}

int
isMsgClearForNonPair (Term t)
{
  if (realTermEncrypt (t))
    {
      //hash
      if (t->helper.fcall)
	{
	  return !HasMsgVarForNonPair (TermOp (t));
	}
      else			//encryption
	{
	  return !HasMsgVarForNonPair (TermOp (t)) &&
	    !HasMsgVarForNonPair (TermKey (t));
	}
    }
  return true;
}

//check if a term is msg-clear
int
isMsgClear (Term t)
{
  Termlist tl = split (t);
  while (tl != NULL)
    {
      if (!isMsgClearForNonPair (tl->term))
	return false;
      tl = tl->next;
    }
  return true;
}


//check if a clause f(p)=u is composite-preserving
int
isCompositePreserving1 (Equation eq)
{
  return realTermLeaf (eq->left) || !realTermLeaf (eq->right);
}

int
isCompositePreserving ()
{
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      if (!isCompositePreserving1 (tmp->eq))
	{
	  globalError++;
	  eprintf ("warning: equation ");
	  printEquation (tmp->eq->left, tmp->eq->right);
	  eprintf (" is not composite preserving");
	  eprintf ("\n");
	  globalError--;
	  return 0;
	}
      tmp = tmp->next;
    }
  return 1;
}

//if f(p) = q and q = g(q') then p = g(p')
int
isContructorExclusiveForHash1 (Equation eq, Term g)
{
  return (!realTermEncrypt (eq->right) || !eq->right->helper.fcall
	  || !isTermEqual (TermKey (eq->right), g))
    || (realTermEncrypt (eq->left) && eq->left->helper.fcall
	&& isTermEqual (TermKey (eq->left), g));
}

int
isContructorExclusiveForHash (Term g)
{
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      Equation eq = tmp->eq;
      if (!isContructorExclusiveForHash1 (eq, g))
	{
	  globalError++;
	  eprintf ("warning: equation ");
	  printEquation (eq->left, eq->right);
	  eprintf (" is not constructor exclusiveness for ");
	  printTerm (g);
	  eprintf ("\n");
	  globalError--;
	  return 0;
	}
      tmp = tmp->next;
    }
  return 1;
}

int
isContructorExclusiveForNonHash1 (Equation eq)
{
  if (realTermEncrypt (eq->right))
    return eq->right->helper.fcall || (realTermEncrypt (eq->left)
				       && !eq->left->helper.fcall);
  if (realTermTuple (eq->right))
    return realTermTuple (eq->left);
  return 1;
}

int
isConstructorExclusiveForNonHash ()
{
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      if (!isContructorExclusiveForNonHash1 (tmp->eq))
	return 0;
      tmp = tmp->next;
    }
  return 1;
}

//check if f(p)=g(f(p1),...,f(pn))
int
isHomomorphicConstructorForHash (Term g)
{
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      Term p = tmp->eq->left;
      if (realTermEncrypt (p) && p->helper.fcall
	  && isTermEqual (TermKey (p), g))
	{
	  if (!isTermEqual (p, tmp->eq->right))
	    return 0;
	  if (!isTicketVariable (TermOp (tmp->eq->right)))
	    return 0;
	}
      tmp = tmp->next;
    }
  return 1;
}


int
isHomomorphicConstructorForNonHash ()
{
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      Term p = tmp->eq->left;
      if (!isTermEqual (p, tmp->eq->right))
	return 0;
      if (realTermEncrypt (p) && !p->helper.fcall)
	{
	  Term op = TermOp (p);
	  Term key = TermKey (p);
	  if (!isTicketVariable (op) || !isTicketVariable (key))
	    return 0;
	}
      if (realTermTuple (p))
	{
	  if (!isTicketVariable (TermOp1 (tmp->eq->right)) ||
	      !isTicketVariable (TermOp2 (tmp->eq->right)))
	    return 0;
	}
      tmp = tmp->next;
    }
  return 1;
}


//condition 2 of (P,f)-safe
int
checkSafenessForSecTerm (Term t)
{
  if (!nilfree)
    return true;
  if (isMsgClear (t))
    return true;
  return false;
}

int
checkSafenessForSecClaim (List secPhi)
{
  while (secPhi != NULL)
    {
      Roledef rd = (Roledef) secPhi->data;
      if (rd->absMess == NULL)
	{
	  rd->absMess = frec (rd->message);
	  if (rd->absMess == NULL)
	    return 0;
	}
      if (!checkSafenessForSecTerm (rd->absMess))
	{
	  globalError++;
	  eprintf ("warning: the term ");
	  printTerm (rd->message);
	  eprintf (" in secrecy property is not msg-clear after abstraction");
	  globalError--;
	  return false;
	}
      secPhi = secPhi->next;
    }
  return true;
}

//condition iii &iv of (P,f)-safe
//1: condition is fulfilled
//0: condition fails or is not justified

//we consider the case that u contains message variables and t does not.
int
injectivenessForEquality1 (Term t, Term u)
{
  //if u is a message variable
  if (realTermVariable (u))
    {
      if (hasTicketVariable (t))
	return 0;
      if (realTermEncrypt (t))
	{
	  if (t->helper.fcall)
	    {
	      Term g = TermKey (t);
	      return isContructorExclusiveForHash (g) &&
		isHomomorphicConstructorForHash (g);
	    }
	  else
	    return isConstructorExclusiveForNonHash () &&
	      isHomomorphicConstructorForNonHash ();

	}
      if (realTermTuple (t))
	return isConstructorExclusiveForNonHash () &&
	  isHomomorphicConstructorForNonHash ();
    }
  //u is composed
  if (realTermEncrypt (u))
    {
      if (t->type != u->type || t->helper.fcall != u->helper.fcall)
	return 1;
      if (u->helper.fcall)
	{
	  if (!isTermEqual (TermKey (t), TermKey (u)))
	    return 1;
	  return injectivenessForEquality1 (TermOp (t), TermOp (u));
	}
      Term op = TermOp (u);
      Term key = TermKey (u);
      if (hasTicketVariable (op)
	  && !injectivenessForEquality1 (TermOp (t), op))
	return 0;
      if (hasTicketVariable (key)
	  && !injectivenessForEquality1 (TermKey (t), key))
	return 0;
    }
  if (realTermTuple (t))
    {
      if (t->type != u->type)
	return 1;
      Term op1 = TermOp1 (u);
      Term op2 = TermOp2 (u);
      if (hasTicketVariable (op1)
	  && !injectivenessForEquality1 (TermOp1 (t), op1))
	return 0;
      if (hasTicketVariable (op2)
	  && !injectivenessForEquality1 (TermOp1 (t), op2))
	return 0;
    }
  return 0;
}

int
injectivenessForEquality (Term t, Term u)
{
  int istuple1 = t->type == TUPLE;
  int istuple2 = u->type == TUPLE;
  if (istuple1 != istuple2)
    return 1;
  if (istuple1)
    return injectivenessForEquality (TermOp1 (t), TermOp1 (u)) &&
      injectivenessForEquality (TermOp2 (t), TermOp2 (u));
  if (realTermEncrypt (t))
    {
      if (realTermVariable (u) && !isTermEqual (getTermType (t), TERM_Ticket))
	return 1;
      if (realTermEncrypt (u))
	{
	  if (u->helper.fcall != t->helper.fcall)
	    return 1;
	  else if (t->helper.fcall)
	    return isTermEqual (TermKey (t), TermKey (u)) ||
	      injectivenessForEquality (TermOp (t), TermOp (u));
	  else
	    return injectivenessForEquality (TermOp (t), TermOp (u)) &&
	      injectivenessForEquality (TermKey (t), TermKey (u));
	}

    }
  //now one of t and u is not a tuple
  int hasMsgVar1 = hasTicketVariable (t);
  int hasMsgVar2 = hasTicketVariable (u);
  if (!hasMsgVar1 && !hasMsgVar2)
    return 1;
  if (!nilfree)
    {
      //one of the terms contains message variable
      if (hasMsgVar1)
	return injectivenessForEquality1 (u, t);
      if (hasMsgVar2)
	return injectivenessForEquality1 (t, u);
    }
  else
    {
      //if Ff is not nil-free then we do not allow message variable
      if (hasMsgVar1 || hasMsgVar2)
	return 0;
      else
	{
	  if (termMguTerm (t, u) == MGUFAIL)
	    return 0;
	  Term st = substitutedTerm (t);
	  Term su = substitutedTerm (u);
	  if (isTermEqual (st, su))
	    return 1;
	}
    }
  return 0;
}

//check injectiveness condition for equations generated from non-injective agreement
int
injectivenessForEquations (Eqlist eqlist)
{
  while (eqlist != NULL)
    {
      if (!injectivenessForEquality (eqlist->eq->left, eqlist->eq->right))
	{
	  globalError++;
	  eprintf ("warning: injectiveness fails for equation \n");
	  printEquation (eqlist->eq->left, eqlist->eq->right);
	  eprintf ("\n");
	  globalError--;
	  return false;
	}
      eqlist = eqlist->next;
    }
  return true;
}

int
checkSafenessForTypebasedAbs (Protocol p)
{
  if (isHelperProtocol (p))
    return 1;
  int check;
  List evPhi;
  List evPhiPlus;
  List secret;
  Eqlist eqs;

  evPhi = NULL;
  evPhiPlus = NULL;
  secret = getSecrecyProp (p, NULL);
  eqs = getEqualities (p, NULL);
  getPropertyEvents (p, &evPhi, &evPhiPlus);
  //check condition ii
  check = checkSafenessForSecClaim (secret);
  list_delete (secret);
  if (!check)
    {
      return 0;
    }
  //check condition iii
  check = injectivenessForEquations (eqs);
  deleteEqlistShallow (eqs);
  if (!check)
    {
      return 0;
    }
  //condition iv is not relevant for the properties of interest as nontrivial equations always occur positively
  //check condition v
  check = checkInjectivenessForEvent (frec, p, evPhiPlus, 1, NULL);

  list_delete (evPhiPlus);
  if (!check)
    return 0;
  //check condition vi
  check = checkNoNilEvent (frec, evPhi, 1, NULL);
  list_delete (evPhi);
  return check;
}

//we may add missing equations to encure totality
void
ensureTotality ()
{
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      Role roles = p->roles;
      while (roles != NULL)
	{
	  Roledef rd = roles->roledef;
	  while (rd != NULL)
	    {
	      if (rd->message != NULL)
		rd->absMess = frec (rd->message);
	      rd = rd->next;
	    }
	  roles = roles->next;
	}
      p = p->next;
    }
}


Roledef
firstAbstEventWithTerm (Roledef rd, Term t)
{
  while (rd != NULL)
    {
      if (isSubterm (t, rd->absMess))
	{
	  return rd;
	}
      rd = rd->next;
    }
  return NULL;
}

int
isWellformed ()
{
  Protocol p;
  Role r;
  Termlist vars;
  for (p = sys->protocols; p != NULL; p = p->next)
    {
      for (r = p->roles; r != NULL; r = r->next)
	{
	  for (vars = r->variables; vars != NULL; vars = vars->next)
	    {
	      if (!inTermlist (p->rolenames, vars->term))
		{
		  Roledef rd;
		  rd = firstAbstEventWithTerm (r->roledef, vars->term);
		  if (rd != NULL && rd->type != RECV)
		    return 0;
		}
	    }
	}
    }
  return 1;
}

Roledef
findAbsEventWithTerm (Roledef rd, Term t)
{
  while (rd != NULL)
    {
      if (rd->type != CLAIM)
	{
	  if (rd->absMess == NULL)
	    rd->absMess = frec (rd->message);
	  if (isSubterm (t, rd->absMess))
	    return rd;
	}
      rd = rd->next;
    }
  return NULL;
}

int
checkWellformed (List * rl, List * varl)
{
  int ok = 1;
  Protocol p;
  Role r;
  Termlist vars;
  for (p = sys->protocols; p != NULL; p = p->next)
    {
      for (r = p->roles; r != NULL; r = r->next)
	{
	  Termlist tl = NULL;
	  for (vars = r->variables; vars != NULL; vars = vars->next)
	    {
	      if (!inTermlist (p->rolenames, vars->term))
		{
		  Roledef rd;
		  rd = findAbsEventWithTerm (r->roledef, vars->term);
		  if (rd != NULL && rd->type != RECV)
		    {
		      tl = termlistAdd (tl, vars->term);
		      ok = 0;
		    }
		}
	    }
	  if (tl != NULL)
	    {
	      *rl = list_add (*rl, r);
	      *varl = list_add (*varl, tl);
	    }
	}
    }
  return ok;
}

void
extractVariablesInHashPat (Term pat, Term hashfunc, Termlist * vlin,
			   Termlist * vlout)
{
  if (realTermLeaf (pat))
    *vlout = termlistAdd (*vlout, pat);
  if (realTermEncrypt (pat) && pat->helper.fcall
      && isTermEqual (TermKey (pat), hashfunc))
    *vlin = extractVariables (*vlin, TermOp (pat));
  if (realTermTuple (pat))
    {
      extractVariablesInHashPat (TermOp1 (pat), hashfunc, vlin, vlout);
      extractVariablesInHashPat (TermOp2 (pat), hashfunc, vlin, vlout);
    }
}

Term
abstractAndFixWellFormed (Term t, Termlist vars)
{
  Term replacef (Term t)
  {
    if (t->abst)
      return abstractAndFixWellFormed (t, vars);
    if (realTermLeaf (t))
      {
	return t;
      }
    if (realTermEncrypt (t))
      {
	TermOp (t) = replacef (TermOp (t));
	//encryption
	if (!t->helper.fcall)
	  {
	    TermKey (t) = replacef (TermKey (t));
	  }
	return t;
      }
    else
      {
	TermOp1 (t) = replacef (TermOp1 (t));
	TermOp2 (t) = replacef (TermOp2 (t));
	return t;
      }
  }
  if (isLTKey (t))
    {
      return t;
    }
  Term termtype = getTermType (t);
  if (isTermEqual (termtype, TERM_Ticket))
    {
      return t;
    }
  Equation eq = findMatchingPattern (termtype, eql);
  if (eq == NULL)
    {
      if (realTermEncrypt (t))
	{
	  TermOp (t) = abstractAndFixWellFormed (TermOp (t), vars);
	  if (!t->helper.fcall)
	    TermKey (t) = abstractAndFixWellFormed (TermKey (t), vars);
	}
      else if (realTermTuple (t))
	{
	  TermOp1 (t) = abstractAndFixWellFormed (TermOp1 (t), vars);
	  TermOp2 (t) = abstractAndFixWellFormed (TermOp2 (t), vars);
	}
      return t;
    }
  patternMatching (eq->left, t);
  if (eq->type == SYSTEM_REMOVE_FIELD)
    {
      Term key = TermKey (eq->left);
      Termlist vleft = extractVariables (NULL, eq->left);
      Termlist vlin = NULL;
      Termlist vlout = NULL;
      extractVariablesInHashPat (eq->right, key, &vlin, &vlout);
      Termlist tl;
      for (tl = vleft; tl != NULL; tl = tl->next)
	{
	  if (!inTermlist (vlin, tl->term) && !inTermlist (vlout, tl->term))
	    {
	      if (isSomeTermlistSubterm (tl->term->subst, vars))
		{
		  tl->term->abst = 1;
		  vlin = termlistAdd (vlin, tl->term);
		}
	    }
	}
      termDelete (eq->right);
      Termlist vright = termlistConcat (vlin, vlout);
      if (termlistContained (vright, vleft))
	{
	  eq->right = makeHomomorphic (eq->left);
	  eq->type = SYSTEM_TRIVIAL;
	}
      else
	{
	  Term hashterm = makeTermFcall (turn_termlist_to_tuple (vlin), key);
	  vlout = termlistAppend (vlout, hashterm);
	  Term newright = turn_termlist_to_tuple (vlout);
	  eq->right = newright;
	}
      termlistDelete (vleft);
      termlistDelete (vright);
    }
  return normalizeTerm (replacef (substitutedTerm (eq->right)));

}

//we adapt an equation if it creates a protocol which is not well-formed
void
ensureWellFormedness ()
{
  List rl = NULL;
  List varl = NULL;
  if (!checkWellformed (&rl, &varl))
    {
      eql_changed = 1;
      List l1 = rl;
      List l2 = varl;
      while (l1 != NULL)
	{
	  Role r = (Role) l1->data;
	  Termlist vars = (Termlist) l2->data;
	  Roledef rd = r->roledef;
	  while (rd != NULL)
	    {
	      if (rd->type == RECV)
		{
		  abstractAndFixWellFormed (rd->message, vars);
		  Termlist recovered = extractVariables (NULL, rd->message);
		  vars = termlistMinusTermlist (vars, recovered);
		  termlistDelete (recovered);
		  if (vars == NULL)
		    break;
		}
	      rd = rd->next;
	    }
	  l1 = l1->next;
	  l2 = l2->next;
	}
      list_delete (rl);
      list_delete (varl);
    }
}


int
typebasedAbstraction ()
{
  eql_changed = 0;
  ensureTotality ();

  //if(!isWellformed()) return 0;
  if (eql_changed)
    resetAbs (sys);
  eql_changed = 0;
  ensureWellFormedness ();

  if (eql_changed)
    resetAbs (sys);
  if (trivialEquationlist (eql))
    return 0;

  int result = tryAbstractProt (checkSafenessForTypebasedAbs, frec, sys);
  return result;
}
