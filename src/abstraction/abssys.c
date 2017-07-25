/*
 * absys.c
 *
 *  Created on: May 5, 2014
 *      Author: nguyen
 */
#include "abssys.h"
extern System original;
extern Eqlist eql;
Symbol pat;
extern Term TERM_NIL;
extern struct switchdata switches;
static List absList;
List outputClaims;
List falsifiedClaims;
List verified, falsified;
Termlist secret;
Termlist homfunc;
//Termlist secretav;
Termlist claims;
int abstcount;

void
initKnowledge ()
{
  knowledgeAddTerm (original->know, TERM_NIL);
}

void
initClaims ()
{
  claims = NULL;
  Claimlist cl;
  for (cl = original->claimlist; cl != NULL; cl = cl->next)
    {
      if (!isClaimSignal (cl) && !cl->alwaystrue)
	claims = termlistAppend (claims, cl->label);
    }
}

void
addProt2Stack (const void *data)
{
  List newnode;

  newnode = list_create (data);
  if (absList == NULL)
    {
      absList = newnode;
    }
  else
    {
      newnode->next = absList;
      absList->prev = newnode;
    }
  absList = newnode;
}

void
addSecretTerms ()
{
  Claimlist cl = original->claimlist;
  while (cl != NULL)
    {
      if (isSecretClaim (cl))
	{
	  secret = termlistAdd (secret, cl->parameter);
	}
      cl = cl->next;
    }
}

Roledef
findEventContainTerm (Roledef rd, Term t)
{
  while (rd != NULL)
    {
      if (rd->type != CLAIM && containTermInPlain (t, rd->message))
	return rd;
      rd = rd->next;
    }
  return NULL;
}

void
addSecretTerm (Termlist tl)
{
  Termlist tmp;
  for (tmp = tl; tmp != NULL; tmp = tmp->next)
    {
      if (isSubtermInTermlist (tmp->term, secret))
	{
	  if (!termInList (secret, tmp->term->subst))
	    secret = termlistAdd (secret, tmp->term->subst);
	}
      else if (isSubtermInTermlist (tmp->term->subst, secret))
	{
	  if (!termInList (secret, tmp->term))
	    secret = termlistAdd (secret, tmp->term);
	}
    }
}


void
secrecyAnalysis (Role roles, Role r, Term sec)
{
  Termlist av = extractAV (NULL, sec, 0);
  Termlist tl = av;
  Termlist subs = NULL;
  Termlist av1 = NULL;
  while (tl != NULL)
    {
      Roledef rd = findEventContainTerm (r->roledef, tl->term);
      if (rd != NULL)
	{
	  Roledef rd1 = findSendRecvEvent (roles, rd);
	  if (rd1 != NULL)
	    {
	      Termlist st = termMguTerm (rd->message, rd1->message);
	      if (st != MGUFAIL)
		{
		  subs = termlistConcat (st, subs);
		}
	    }
	  Termlist tmp = extractAV (NULL, rd->message, 0);
	  av1 = termlistConcat (tmp, av1);
	  if (termlistContained (av1, av))
	    break;
	}
      tl = tl->next;
    }
  /*
     Term secterm = buildSecretTerm(sec,subs);
     termlistSubstReset(subs);
     if(!inTermlist(secret,secterm))
     secret = termlistAdd(secret,secterm);
   */
  addSecretTerm (subs);
  termlistSubstReset (subs);
  termlistDelete (subs);
  termlistDelete (av);
}

void
extractSecrets ()
{
  addSecretTerms ();
  Role r;
  Roledef rd;
  Protocol main_prot = original->protocols;
  while (main_prot != NULL && isHelperProtocol (main_prot))
    main_prot = main_prot->next;
  for (r = main_prot->roles; r != NULL; r = r->next)
    {
      for (rd = r->roledef; rd != NULL; rd = rd->next)
	if (rd->type == CLAIM && isSecretClaim (rd->claiminfo))
	  secrecyAnalysis (main_prot->roles, r, rd->message);
    }
  //extract atoms and variables from secrets
  /*
     Termlist tl = NULL;
     Termlist tmp;
     for(tmp = secret; tmp!=NULL; tmp= tmp->next)
     tl = extractAV(tl,tmp->term,0);
     //termlistDelete(secret);
     secret = tl;
   secretav=tl;
   */
}

//check if t and u have the same set of variables and atoms
int
compareAV (Term t, Term u)
{
  Termlist av = extractAV (NULL, t, 0);
  Termlist tmp;
  for (tmp = av; tmp != NULL; tmp = tmp->next)
    {
      if (!isSubterm (tmp->term, u))
	return 0;
    }
  termlistDelete (av);
  return 1;
}

//check if two list of arguments are compatible, i.e., each argument pair has the same shape and the same set of variables and atoms
int
isCompatibleArgument (Term t, Term u)
{
  if (t->type != u->type)
    return 0;
  if (realTermLeaf (t))
    return isTermEqual (t, u);
  else if (realTermEncrypt (t))
    {
      if (t->helper.fcall != u->helper.fcall)
	return 0;
      if (t->helper.fcall)
	{
	  if (!isTermEqual (TermKey (t), TermKey (u)))
	    return 0;
	  return compareAV (TermOp (t), TermOp (u));
	}
      else
	return compareAV (TermOp (t), TermOp (u)) &&
	  compareAV (TermKey (t), TermKey (u));
    }
  else
    return isCompatibleArgument (TermOp1 (t), TermOp1 (u))
      && isCompatibleArgument (TermOp2 (t), TermOp2 (u));
}

void
extractHashFunc (Term t)
{
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	homfunc = termlistAdd (homfunc, TermKey (t));
      extractHashFunc (TermOp (t));
      extractHashFunc (TermKey (t));
    }
  else if (realTermTuple (t))
    {
      extractHashFunc (TermOp1 (t));
      extractHashFunc (TermOp2 (t));
    }
}

void
extractHomFunc (Term t, Term u)
{
  if (t->type == u->type)
    {
      if (realTermEncrypt (t))
	{
	  if (t->helper.fcall && u->helper.fcall)
	    {
	      if (isTermEqual (TermKey (t), TermKey (u)))
		{
		  if (isCompatibleArgument (TermOp (t), TermOp (u)))
		    extractHomFunc (TermOp (t), TermOp (u));
		  else
		    extractHashFunc (t);	//homfunc = termlistAdd(homfunc, TermKey(t));
		}
	    }
	  else if (!t->helper.fcall && !u->helper.fcall)
	    {
	      extractHomFunc (TermKey (t), TermKey (u));
	      extractHomFunc (TermOp (t), TermOp (u));
	    }
	}
      else if (realTermTuple (t))
	{
	  extractHomFunc (TermOp1 (t), TermOp1 (u));
	  extractHomFunc (TermOp2 (t), TermOp2 (u));
	}
    }
}

//extract terms that involve equational theory and are handled by oracles
//for these terms, the transformations should be homomorphic
void
getHomoFunc ()
{
  Protocol p;
  for (p = original->protocols; p != NULL; p = p->next)
    if (isHelperProtocol (p))
      {
	Role r;
	for (r = p->roles; r != NULL; r = r->next)
	  {
	    Roledef rd = r->roledef;
	    while (rd != NULL)
	      {
		extractHomFunc (rd->message, rd->next->message);
		rd = rd->next->next;
	      }
	  }
      }
}

void
abssysInit ()
{
  outputClaims = falsifiedClaims = NULL;
  absList = NULL;
  secret = NULL;
  pat = symbolSysConst ("Pat_");
  initKnowledge ();
  initClaims ();
  extractSecrets ();
  getHomoFunc ();
  addProt2Stack (original);
}

void
buildAbstractionList ()
{
  System abssys = systemDuplicate (original);
  abstcount = 0;
  while (abstcount < switches.maxAbstractions)
    {
      abssys = abstractSystem (abssys);
      if (abstractionSucceed ())
	{
	  addProt2Stack (abssys);
	  abstcount++;
	}
      else
	break;
    }
  eprintf ("Number of abstractions:%d\n", abstcount);
}

void
outputResult (System abssys)
{
  List claims = verified;
  while (claims != NULL)
    {
      claimStatusReport (abssys, claims->data);
      if (switches.xml)
	{
	  xmlOutClaim (abssys, claims->data);
	}
      claims = claims->next;
    }
}

void
freeClaims ()
{
  while (verified != NULL)
    {
      List tmp = verified;
      verified = verified->next;
      free (tmp);
    }

  while (falsified != NULL)
    {
      List tmp = falsified;
      falsified = falsified->next;
      free (tmp);
    }

}

void
freeSystem (System sys)
{
  knowledgeDelete (sys->know);	//only shallow deletion is done because abstract systems share the basic knowledge
  protocolDelete (sys->protocols);
  systemDestroy (sys);
}


void
list_delete_system (List l)
{
  while (l != NULL)
    {
      List tmp = l;
      l = l->next;
      freeSystem (tmp->data);
      free (tmp);
    }
}


int
isRemovedClaim (Term label)
{
  List tmp;
  for (tmp = verified; tmp != NULL; tmp = tmp->next)
    {
      Claimlist remove = (Claimlist) tmp->data;
      if (isTermEqual (remove->label, label))
	return 1;
    }

  for (tmp = falsified; tmp != NULL; tmp = tmp->next)
    {
      Claimlist remove = (Claimlist) tmp->data;
      if (isTermEqual (remove->label, label))
	return 1;
    }

  return 0;
}

void
removeClaims ()
{
  Termlist newClaims = NULL;
  Termlist labels;
  for (labels = claims; labels != NULL; labels = labels->next)
    {
      if (!isRemovedClaim (labels->term))
	newClaims = termlistAdd (newClaims, labels->term);
    }
  termlistDelete (claims);
  claims = newClaims;
}

//! Model check the system, given all parameters.
/*
 * Precondition: the system was reset with the corresponding parameters.
 * Reports time and states traversed.
 * Note that the return values doubles as the number of failed claims.
 *@return True iff any claim failed, and thus an attack was found.
 */

void MC_check(int (*arachneClaim)(),const System sys)
{
	  initModelCheck (sys);
	  arachnePrepare ();
	  arachne(arachneClaim);
}

void
runVerification (void (*MC_single) (const System))
{
  abssysInit ();
  struct timeval start, finish;
  long msec;
  gettimeofday (&start, NULL);
  buildAbstractionList ();
  gettimeofday (&finish, NULL);
  msec = timevaldiff (&start, &finish);
  eprintf ("Constructing abstractions in %d millisecond(s)\n\n", msec);
  System abssys;
  List abst = absList;
  while (claims != NULL && abstcount >= 0)
    {
      abssys = (System) abst->data;
      //systemStart(abssys);
      //abssys->traceKnow[0] = abssys->know;
      MC_single (abssys);
      if (verified != NULL || falsified != NULL)
	{
	  if (verified != NULL)
	    {
	      if (abstcount)
		{
		  //eprintf("Abstract protocol model %d:\n", abstcount);
		  //protocolsPrint(abssys->protocols);
		  //eprintf("\n");
		  eprintf ("Properties verified at the abstract model %d: \n",
			   abstcount);
		}
	      else
		eprintf ("Properties verified at the original protocol:\n");
	      outputResult (abssys);
	      eprintf("\n");
	    }
	  removeClaims ();
	}
      abstcount--;
      freeClaims ();
      verified = falsified = outputClaims = falsifiedClaims = NULL;
      abst = abst->next;
    }
  list_delete_system (absList);
  deleteEqlist (eql);
  termlistDelete (secret);
  termlistDelete (homfunc);
}
