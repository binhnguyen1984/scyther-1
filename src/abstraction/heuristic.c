/*
 * heuristic.c
 *
 *  Created on: May 26, 2014
 *      Author: nguyen
 */
#include "../term.h"
#include "heuristic.h"
#include "subtype.h"
#include "termfunc.h"

extern Termlist secret, homfunc;
//extern Termlist secretav;
Termlist auth;
//Termlist authav;
Termlist comp;
Termlist labeledterm;
//Termlist oraclecomp;
Termlist gentype;		// in the corresponding relation to the list above
Termlist topconst;
Termlist homeq;
Termlist forbidden;
Termlist tmpl;
Termlist unif;
extern Eqlist eql;
System sys;
Protocol main_prot;
Term term, smallterm, parentTerm, smalltype, bigterm;
int currdepth;
int hasTuple, hasEnc;
int authProp;
int typebasedRes, redRes, avRes;
int UNIFDEPTH = 5;
enum seclabeltype
{
  UNDEFINED, NOSEC, MAYBE_SEC, SEC, NOAUTH, MAYBE_AUTH, AUTH
};

enum unif
{
  FIXED, NO_FIX, NOT_NEED
};
enum
{
  NONE, KEYPOS, HASH, MAC, PK, SIG, LTSYM, SYM
} crytotype;

enum
{
  ACCESSIBLE, NOT_ACCESSIBLE
} accessLabel;

enum
{
	TMP, ORG, PROT
}labelmode;
void
heuristicInit (System mysys)
{
  sys = mysys;
  labeledterm = NULL;
  //authav=NULL;
  auth = topconst = forbidden = comp = gentype = unif = NULL;
  hasTuple = hasEnc = 0;
}

void
heuristicDone ()
{
  termlistDelete (auth);
  auth = NULL;
  //termlistDelete(authav);
  //authav=NULL;
  termlistDelete (comp);
  termlistDelete (topconst);
  termlistDelete (forbidden);
  termlistDelete (gentype);
//      termlistDelete(oraclecomp);
  termlistDelete (unif);
}

void
extractTopConstructors1 (Term t)
{
  if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	{
	  topconst = termlistAddNewTerm (topconst, TermKey (t));
	  extractTopConstructors1 (TermOp (t));
	}
      else
	{
	  hasEnc = 1;
	  extractTopConstructors1 (TermOp (t));
	  extractTopConstructors1 (TermKey (t));
	}
    }
}

void
extractTopConstructors (Term t1, Term t2)
{
  int istuple1 = realTermTuple (t1);
  int istuple2 = realTermTuple (t2);
  if (istuple1 && istuple2)
    {
      extractTopConstructors (TermOp1 (t1), TermOp1 (t2));
      extractTopConstructors (TermOp2 (t1), TermOp2 (t2));
    }
  else if (istuple1)
    {
      if (isTicketVariable (t2))
	hasTuple = 1;
    }
  else if (istuple2)
    {
      if (isTicketVariable (t1))
	hasTuple = 1;
    }
  else
    {
      int isLeaf1 = realTermLeaf (t1);
      int isLeaf2 = realTermLeaf (t2);
      if (!isLeaf1 && !isLeaf2)
	{
	  if (t1->type != t2->type)
	    return;
	  if (t1->type == ENCRYPT)
	    {
	      extractTopConstructors (TermOp (t1), TermOp (t2));
	      extractTopConstructors (TermKey (t1), TermKey (t2));
	    }
	  else if (isTermEqual (TermKey (t1), TermKey (t2)))
	    extractTopConstructors (TermOp (t1), TermOp (t2));
	}
      else if (isTicketVariable (t2))
	{
	  extractTopConstructors1 (t1);
	}
      else if (isTicketVariable (t1))
	{
	  extractTopConstructors1 (t2);
	}
    }
}

void
extractAuthTermFromSignals (List running, List commit)
{
  while (commit != NULL)
    {
      Roledef comm = (Roledef) commit->data;
      Roledef runn = findMatchingRun (running, comm);
      if (runn == NULL)
	return;
      //authav=extractAV(authav,runn->message,0);
      //authav=extractAV(authav,comm->message,0);

      auth = addField2Termlist (auth, runn->message);
      auth = addField2Termlist (auth, comm->message);

      extractTopConstructors (comm->message, runn->message);
      commit = commit->next;
    }
}


void
setMainProtocol ()
{
  Protocol prot = sys->protocols;
  while (prot != NULL)
    {
      if (!isHelperProtocol (prot))
	{
	  main_prot = prot;
	  return;
	}
      prot = prot->next;
    }
}

void
leafLabelInit (Term t)
{
  t->seclabel.prop_auth = t->seclabel.prot_auth = UNDEFINED;
  t->seclabel.prop_sec = t->seclabel.prot_sec = UNDEFINED;
  t->accessible = NOT_ACCESSIBLE;
  t->inIK = UNKNOWN;
}

void
tmpLabelInit (Term t)
{
  t->seclabel.tmp_auth = t->seclabel.tmp_sec = UNDEFINED;
}

void
labelInit (Term t, void (*initLabel) (Term))
{
  initLabel (t);
  if (realTermEncrypt (t))
    {
      labelInit (TermOp (t), initLabel);
      if (!t->helper.fcall)
	labelInit (TermKey (t), initLabel);
    }
  else if (realTermTuple (t))
    {
      labelInit (TermOp1 (t), initLabel);
      labelInit (TermOp2 (t), initLabel);
    }
}


//check if a term's type disjoint fro all types specified in equation list
int
typeDisjointFromEqlist (Term t)
{
  Term type1 = getTermType (t);
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      Term type2 = getTermType (tmp->eq->left);
      if (isComparable (type1, type2))
	return 0;
      tmp = tmp->next;
    }
  return 1;
}

Term
getMostGeneralTypeInlist (Termlist tl, Term type)
{
  while (tl != NULL)
    {
      type = mostGeneralType (type, tl->term);
      tl = tl->next;
    }
  return type;
}

/*
Term mostGenTypeOracle(Term type)
{
	Termlist tl;
	Termlist conflict= NULL;
	for(tl = oraclecomp; tl!=NULL; tl = tl->next)
	{
		int isCompr = isComparable(tl->term,type);
		if(isCompr)
			conflict =termlistAdd(conflict,tl->term);
	}
	Term newtype = getMostGeneralTypeInlist(conflict,type);
	termlistDelete(conflict);
	return newtype;
}
*/

void
updateGenType (Term type)
{
  Termlist tl;
  Termlist newgen = NULL;
  Termlist conflict = NULL;
  Term commType;
  //search for conflicts
  for (tl = gentype; tl != NULL; tl = tl->next)
    {
      commType = upperCommonSubtype (tl->term, type);
      if (commType != NULL && !isTermEqual (commType, type))
	{
	  conflict = termlistAdd (conflict, tl->term);
	}
      else
	newgen = termlistAdd (newgen, tl->term);
    }
  if (conflict == NULL)
    {
      termlistDelete (newgen);
      gentype = termlistAdd (gentype, type);
    }
  else
    {
      Term newtype = getMostGeneralTypeInlist (conflict, type);
      newgen = termlistAdd (newgen, newtype);
      termlistDelete (gentype);
      gentype = newgen;
    }
}

//not necessarily the type of the term, but the type is supposed to be assigned to the corresponding pattern
Term
getIntendedPatternType (Term t)
{
  Term type = getTermType (t);
  Termlist tl;
  for (tl = gentype; tl != NULL; tl = tl->next)
    {
      Term t = subTypeInSubterm (type, tl->term);
      if (t)
	return t;
    }
  return type;
}

//add a term of more general type or is deeper in term of structure (if the two types have common subtype)
void
addTermForAbstraction (Term t)
{
  Termlist tmp = comp;
  Term type = getTermType (t);
  Termlist conflict = NULL;

  while (tmp != NULL)
    {
      Term type1 = getTermType (tmp->term);
      Term commType = upperCommonSubtype (type, type1);
      if (commType != NULL)
	{
	  if (isTermEqual (commType, type))
	    {
	      return;
	    }
	  else
	    {
	      if (!isTermEqual (commType, type1))
		conflict = termlistAdd (conflict, type1);
	      /*
	         Termlist tmp1 = tmp->next;
	         comp=termlistDelTerm(tmp);
	         tmp = tmp1;
	         continue;
	       */
	    }
	}
      tmp = tmp->next;
    }
  t->originType = type;
  forbidden = termlistAdd (forbidden, t);
  Term newgentype = getMostGeneralTypeInlist (conflict, type);
  //newgentype = mostGenTypeOracle(newgentype);
  updateGenType (newgentype);
  termlistDelete (conflict);
  comp = termlistAdd (comp, t);
}

int
coveredByFixEquations (Term type)
{
  //check if it is covered by a fixed homomorphic equation created by the system
  if (realTermEncrypt (type) && type->helper.fcall
      && inTermlist (topconst, TermKey (type)))
    return 1;
  Eqlist tmp = eql;
  while (tmp != NULL)
    {
      //it may be covered by an user-defined equation
      if (tmp->eq->type < SYSTEM_TRIVIAL)
	{
	  Term type1 = getTermType (tmp->eq->left);
	  if (isSubtype (type, type1))
	    return 1;
	}
      tmp = tmp->next;
    }
  return 0;
}

//collect composed terms at top level, requiring that no two term-types have a common subtype.
void
getTopComposed (Term t)
{
  if (realTermEncrypt (t))
    {
      //if(!typeDisjointFromEqlist(t)) return ;
      if (coveredByFixEquations (t))
	return;
      addTermForAbstraction (t);
    }
  else if (realTermTuple (t))
    {
      getTopComposed (TermOp1 (t));
      getTopComposed (TermOp2 (t));
    }
}

/*
void getOracleComposedType(Term t)
{
	if(realTermEncrypt(t))
	{
		Term type = getTermType(t);
		oraclecomp = termlistAdd(oraclecomp,type);
	}
	else if(realTermTuple(t))
	{
		getOracleComposedType(TermOp1(t));
		getOracleComposedType(TermOp2(t));
	}
}
*/


int
plaintextAuthentic(int mode, Term t)
{
  if (realTermLeaf (t))
  {
	  if(mode==TMP)
	  	  return t->seclabel.tmp_auth == AUTH;// && t->seclabel.tmp_sec == SEC;
	  else if(mode==PROT)
		  return t->seclabel.prot_auth==AUTH;
	  else return t->seclabel.org_auth==AUTH;
  }
  if (realTermEncrypt (t))
    return plaintextAuthentic (mode,TermOp (t));
  return plaintextAuthentic (mode,TermOp1 (t)) || plaintextAuthentic (mode,TermOp2 (t));
}

int
isInverseKey (Term t)
{
  Termlist tl = sys->know->inversekeys;
  Termlist prev = NULL;
  while (tl != NULL)
    {
      if (isTermEqual (tl->term, t))
	{
	  if (prev == NULL)
	    return 1;
	  else if (prev->term->left.symb->text[0] == '_')
	    return 0;
	  else
	    return 1;
	}
      prev = tl;
      tl = tl->next;
    }
  return 0;
}


struct cryptolabel
getCryptoLabel (Term t)
{
  struct cryptolabel lb;
  lb.auth_crypt = NOAUTH;
  lb.sec_crypt = NOSEC;
  if (realTermEncrypt (t))
    {
      Term op = TermOp (t);
      if (t->helper.fcall)
	{
	  if (isInverseKey (TermKey (t)))
	    {
	      lb.sec_crypt = NOSEC;
	      lb.auth_crypt = NOAUTH;
	    }
	  else if (containLTSharedKeyInPlain (op) != NULL)	//it is a MAC
	    {
	      lb.sec_crypt = SEC;
	      lb.auth_crypt = AUTH;
	    }
	  else
	    {
	      lb.sec_crypt = SEC;
	    }
	}
      else
	{
	  Term key = TermKey (t);
	  if (isPublicKey (key))	//if it is public-key encryption
	    {
	      //check if some agent identifiers are there
	      if (containDiffAgent (op, TermOp (key)))
		{
		  lb.sec_crypt = SEC;
		  lb.auth_crypt = AUTH;
		}
	      else
		{
		  lb.sec_crypt = SEC;
		}
	    }
	  else if (isPrivateKey (key))	// if it is a signature
	    {
	      lb.auth_crypt = AUTH;
	    }
	  else if (isLTSharedKey (key))
	    {
	      lb.sec_crypt = SEC;
	      lb.auth_crypt = AUTH;
	    }
	  else			//symmetric encryption with a session key
	    {
	      lb.sec_crypt =
		key->accessible == NOT_ACCESSIBLE ? SEC : MAYBE_SEC;
	      //if (containDiffAgent (op, NULL))
	      lb.auth_crypt = MAYBE_AUTH;
	    }
	}
    }
  return lb;
}

void
computeProtLabel (Term t, int seclabel, int authlabel, int access)
{
  if (access == ACCESSIBLE)
    t->accessible = access;
  if (seclabel != NOSEC || authlabel != NOAUTH)
    {
      if (t->inIK == YES
	  || (t->inIK == UNKNOWN && inKnowledge (sys->know, t)))
	{
	  t->seclabel.prot_sec = NOSEC;
	  t->seclabel.prot_auth = NOAUTH;
	  t->inIK = YES;
	  return;
	}
    }
  if (t->seclabel.prot_sec == UNDEFINED)
    t->seclabel.prot_sec = seclabel;
  else
    t->seclabel.prot_sec = min (t->seclabel.prot_sec, seclabel);
  if (t->seclabel.prot_auth == UNDEFINED)
    t->seclabel.prot_auth = authlabel;
  else
    t->seclabel.prot_auth = max (t->seclabel.prot_auth, authlabel);

  //update by other occurrences of t
  if (!realTermLeaf (t))
    {
      Term u = getTermInList (labeledterm, t);
      if (u == NULL)
	labeledterm = termlistAdd (labeledterm, t);
      else
	{
	  t->seclabel.prot_auth =
	    max (t->seclabel.prot_auth, u->seclabel.prot_auth);
	  t->seclabel.prot_sec =
	    min (t->seclabel.prot_sec, u->seclabel.prot_sec);
	}
    }

  if (realTermEncrypt (t))
    {
      Term op = TermOp (t);
      if (t->helper.fcall)
	{
	  if (isInverseKey (TermKey (t)))
		  computeProtLabel (op, seclabel, authlabel, access);
	  else if (containLTSharedKeyInPlain (op) != NULL)	//it is a MAC
		  computeProtLabel (op, SEC, AUTH, NOT_ACCESSIBLE);
	  else
	    {
	      if (plaintextAuthentic (PROT,op))
	    	  computeProtLabel (op, SEC, AUTH, NOT_ACCESSIBLE);
	      else //computeProtLabel (op, SEC, authlabel, NOT_ACCESSIBLE);	//otherwise, a normal hash
	    	  computeProtLabel (op, SEC, max(MAYBE_AUTH,authlabel), NOT_ACCESSIBLE);
	    }
	}
      else
	{
	  Term key = TermKey (t);
	  if (isPublicKey (key))	//if it is public-key encryption
	    {
	      //check if some agent identifiers are there
	      if (containDiffAgent (op, TermOp (key)))
		{
	    	  computeProtLabel (op, SEC, AUTH, access);
		}
	      else
		{
		  if (plaintextAuthentic (PROT,op))
			  computeProtLabel (op, SEC, AUTH, access);
		  else
			  computeProtLabel (op, SEC, authlabel, access);
		}
	    }
	  else if (isPrivateKey (key))	// if it is a signature
	    {
		  computeProtLabel (op, seclabel, AUTH, access);
	    }
	  else if (isLTSharedKey (key))
	    {
		  computeProtLabel (op, SEC, AUTH, access);
	    }
	  else			//symmetric encryption with a session key
	    {
	      int newsec =
		key->accessible == NOT_ACCESSIBLE ? SEC : MAYBE_SEC;
	      int newauth;
	      if(plaintextAuthentic (PROT,key)&& key->seclabel.prop_sec==SEC)
	    	  newauth = AUTH;
	      else if(containDiffAgent (op, NULL))
	    	  newauth = max (authlabel,MAYBE_AUTH);
	      else newauth = authlabel;
	      computeProtLabel (op, newsec, newauth, access);
	    }
	}
    }
  else if (realTermTuple (t))
    {
      computeProtLabel (TermOp1 (t), seclabel, authlabel, access);
      computeProtLabel (TermOp2 (t), seclabel, authlabel, access);
    }
}


int comparePlainAndKey(Term plain, Term key)
{
	struct cryptolabel plainlb = getCryptoLabel(plain);
	struct cryptolabel keylb = getCryptoLabel(key);
	if(plainlb.sec_crypt==SEC)
	{
		return keylb.auth_crypt<= plainlb.auth_crypt&&avInclusion(plain,key);
	}
	return 0;
}

void
computeTmpLabel (Term t, int seclabel, int authlabel, int access)
{
  if (access == ACCESSIBLE)
    t->accessible = access;
  if (seclabel != NOSEC || authlabel != NOAUTH)
    {
      if (t->inIK == YES
	  || (t->inIK == UNKNOWN && inKnowledge (sys->know, t)))
	{
	  t->seclabel.tmp_sec = NOSEC;
	  t->seclabel.tmp_auth = NOAUTH;
	}
    }
  if (t->seclabel.tmp_sec == UNDEFINED)
    t->seclabel.tmp_sec = seclabel;
  else
    t->seclabel.tmp_sec = min (t->seclabel.tmp_sec, seclabel);
  if (t->seclabel.tmp_auth == UNDEFINED)
    t->seclabel.tmp_auth = authlabel;
  else
    t->seclabel.tmp_auth = max (t->seclabel.tmp_auth, authlabel);
  if (realTermEncrypt (t))
    {
      Term op = TermOp (t);
      if (t->helper.fcall)
	{
	  if (isInverseKey (TermKey (t)))
	    computeTmpLabel (op, seclabel, authlabel, access);
	  else if (containLTSharedKeyInPlain (op) != NULL)	//it is a MAC
	    computeTmpLabel (op, SEC, AUTH, NOT_ACCESSIBLE);
	  else
	    {
	      if (plaintextAuthentic (TMP,op))
		{
		  //t->auth=AUTH; //new
		  computeTmpLabel (op, SEC, AUTH, NOT_ACCESSIBLE);
		}
	      else
		computeTmpLabel (op, SEC, max(authlabel, MAYBE_AUTH), NOT_ACCESSIBLE);	//otherwise, a normal hash
	    }
	}
      else
	{
	  Term key = TermKey (t);
	  if (isPublicKey (key))	//if it is public-key encryption
	    {
	      //check if some agent identifiers are there
	      if (containDiffAgent (op, TermOp (key)))
		{
		  computeTmpLabel (op, SEC, AUTH, access);
		}
	      else
		{
		  if (plaintextAuthentic (TMP,op))
		    computeTmpLabel (op, SEC, AUTH, access);
		  else
		    computeTmpLabel (op, SEC, authlabel, access);
		}
	    }
	  else if (isPrivateKey (key))	// if it is a signature
	    {
	      computeTmpLabel (op, seclabel, AUTH, access);
	    }
	  else if (isLTSharedKey (key))
	    {
	      computeTmpLabel (op, SEC, AUTH, access);
	    }
	  else			//symmetric encryption with a session key
	    {
	      int newsec =
		key->accessible == NOT_ACCESSIBLE ? SEC : MAYBE_SEC;
	      int newauth;
	      if(plaintextAuthentic (TMP,key)&& key->seclabel.tmp_sec==SEC)
	    	  newauth = AUTH;
	      else if(containDiffAgent (op, NULL))
	    	  newauth = max (authlabel,MAYBE_AUTH);
	      else newauth = authlabel;
	      computeTmpLabel (op, newsec, newauth, access);
	    }
	}
    }
  else if (realTermTuple (t))
    {
      computeTmpLabel (TermOp1 (t), seclabel, authlabel, access);
      computeTmpLabel (TermOp2 (t), seclabel, authlabel, access);
    }
}

int
essentialForAuth (Term t)
{
  if (t->e_auth != UNKNOWN)
    return t->e_auth;
  if (t->inIK)
    {
      t->e_auth = NO;
    }
  else
    t->e_auth = isSubtermInTermlist (t, auth) ? YES : NO;
  return t->e_auth;
}

int
essentialForSec (Term t)
{
  if (t->e_sec != UNKNOWN)
    return t->e_sec;
  if (t->inIK)
    {
      t->e_sec = NO;
    }
  else
    t->e_sec = isSubtermInTermlist (t, secret) ? YES : NO;
  return t->e_sec;
}

/*
void computeGlobalLabel(Term t)
{
	if(t->seclabel.gb_auth==UNDEFINED)
	{
		if(t->seclabel.prop_auth==UNDEFINED)
		{
			if(isSubtermInTermlist(t,auth))
				t->seclabel.prop_auth = AUTH;
			else t->seclabel.prop_auth = NOAUTH;
		}
		t->seclabel.gb_auth = min(t->seclabel.prot_auth,t->seclabel.prop_auth);
	}
	if(t->seclabel.gb_sec==UNDEFINED)
	{
		if(t->seclabel.prop_sec==UNDEFINED)
		{
			if(isSubtermInTermlist(t,secret))
				t->seclabel.prop_sec = SEC;
			else t->seclabel.prop_sec = NOSEC;
		}
		t->seclabel.gb_sec = min(t->seclabel.prot_sec,t->seclabel.prop_sec);
	}
}
*/

int
isLabelPreserving (Term t)
{
  //if t is essential then we must ensure that t's label is preserved
  if (realTermTuple (t))
    return isLabelPreserving (TermOp1 (t)) && isLabelPreserving (TermOp2 (t));
  if (essentialForAuth (t) == YES || essentialForSec (t) == YES)
    {
      //computeGlobalLabel(t);
      int pres = (t->seclabel.tmp_auth >= t->seclabel.org_auth
		  || t->seclabel.tmp_auth >= t->seclabel.prot_auth)
	&& (t->seclabel.tmp_sec >= t->seclabel.org_sec
	    || t->seclabel.tmp_sec >= t->seclabel.prot_sec);
      if (!pres)
	return 0;
    }
  if (realTermEncrypt (t))
    {
      return isLabelPreserving (TermOp (t));
    }
  return 1;
}


int
checkLabelPreservation (Term t, int seclabel, int authlabel, int access)
{
  labelInit (t, tmpLabelInit);
  computeTmpLabel (t, seclabel, authlabel, access);
  return isLabelPreserving (t);
}

/*
void labelingSessionKey(Term t)
{
	if(t->type==ENCRYPT)
	{
		if(t->helper.fcall==false&&isLTKey(TermKey(t)))
		{
			TermKey(t)->e_auth=t->e_sec = 1;
			TermKey(t)->seclabel.gb_sec=SEC;
		}
		labelingSessionKey(TermOp(t));
	}
	else if(t->type==TUPLE)
	{
		labelingSessionKey(TermOp1(t));
		labelingSessionKey(TermOp2(t));
	}
}

void labelingSessionKeys()
{
	Role r;
	Roledef rd;
	for(r = main_prot->roles; r!=NULL;r=r->next)
		for(rd = r->roledef;rd!=NULL;rd=rd->next)
		{
			if(rd->type!=CLAIM)
			{
				labelingSessionKey(rd->message);
			}
		}
}
*/
void
extractComposedTerms ()
{
  Role r;
  Roledef rd;
  //Protocol p;
  /*
     for(p=sys->protocols;p!=NULL;p=p->next)
     if(isHelperProtocol(p))
     {
     for(r = p->roles; r!=NULL;r=r->next)
     for(rd = r->roledef;rd!=NULL;rd=rd->next)
     getOracleComposedType(rd->message);
     }
   */

  for (r = main_prot->roles; r != NULL; r = r->next)
    for (rd = r->roledef; rd != NULL; rd = rd->next)
      {
		if (rd->type != CLAIM)
		  getTopComposed (rd->message);
      }
}

void
labelInitForMessages ()
{
  Role r;
  Roledef rd;
  for (r = main_prot->roles; r != NULL; r = r->next)
    {
      for (rd = r->roledef; rd != NULL; rd = rd->next)
	if (rd->type != CLAIM)
	  {
	    labelInit (rd->message, leafLabelInit);
	  }
    }
}

/*
void assignSecToSubterm(Term t)
{
	t->essential=1;
	t->sec = max(t->sec, SEC);
	if(realTermEncrypt(t))
	{
		assignSecToSubterm(TermOp(t));
	}
	else if(realTermTuple(t))
	{
		assignSecToSubterm(TermOp1(t));
		assignSecToSubterm(TermOp1(t));
	}
}

void assignAuthToSubterm(Term t)
{
	t->essential=1;
	t->auth = max(t->auth, AUTH);
	if(realTermEncrypt(t))
	{
		assignAuthToSubterm(TermOp(t));
	}
	else if(realTermTuple(t))
	{
		assignAuthToSubterm(TermOp1(t));
		assignSecToSubterm(TermOp1(t));
	}
}
*/
void
labelingMessages ()
{
  Role r;
  Roledef rd;
  Termlist basicHead = sys->know->basic;
  Termlist varsHead = sys->know->vars;
  Termlist encHead = sys->know->encrypt;
  for (r = main_prot->roles; r != NULL; r = r->next)
    {
      addAgentVariables (sys->know, r->variables);
      for (rd = r->roledef; rd != NULL; rd = rd->next)
	{
	  if (rd->type != CLAIM)
	    {
	      computeProtLabel (rd->message, NOSEC, NOAUTH, ACCESSIBLE);
	    }
	}
      recoverIK (sys->know, basicHead, varsHead, encHead);
    }
  // take the property into account
  /*
     Termlist tl ;
     for(tl=secret; tl!=NULL; tl=tl->next)
     {
     assignSecToSubterm(tl->term);
     }
     for(tl=auth; tl!=NULL; tl=tl->next)
     {
     assignAuthToSubterm(tl->term);
     }
   */
  termlistDelete (labeledterm);
}


void
processProperties ()
{
  Role r = main_prot->roles;
  //authProp=0;
  List comm = NULL;
  List runn = NULL;
  while (r != NULL)
    {
      term = r->nameterm;
      Roledef rd = r->roledef;
      while (rd != NULL)
	{
	  if (rd->type == CLAIM)
	    {
	      Claimlist cl = rd->claiminfo;
	      //if(isAuthStrongClaim(cl))
	      //      authProp=1;
	      //else
	      {
		//authProp=-1;
		if (isRunningEvent (cl))
		  {
		    runn = list_add (runn, rd);
		  }
		else if (isCommitEvent (cl))
		  {
		    //auth = termlistAdd(auth, rd->message);
		    //authav = extractAV(authav,rd->message,0);
		    comm = list_add (comm, rd);
		  }
	      }
	    }
	  rd = rd->next;
	}
      r = r->next;
    }
  extractAuthTermFromSignals (runn, comm);
}

void
labeling ()
{
  labelInitForMessages ();
  labelingMessages ();
  //labelingSessionKeys();
}

void
protocolAnalysis ()
{
  if(main_prot==NULL) return;
  labeling ();
  extractComposedTerms ();
  processProperties ();
}

void
initialAnalysis ()
{
  setMainProtocol ();
  protocolAnalysis ();
}

/*
int secrecyNotRequired(Term t)
{
	if(realTermTuple(t))
		return secrecyNotRequired(TermOp1(t))&&secrecyNotRequired(TermOp2(t));
	if(t->seclabel.prot_sec==NOSEC) return 1; //originally it is not secret, so no need to require secrecy for it
	if(realTermEncrypt(t))
	{
		int secSubt = isTermlistSubterm(t,secret);
		if(!secSubt) return 0;
		if(t->helper.fcall) return 1; //secret is protected by hashes
		Term key = TermKey(t);
		if(isPublicKey(key)||isLTSharedKey(key)) return 1; //secret is protected by long-term key encryptions
		return 0;
	}
	return 0;
}
*/
int
isAuthProvider (Term t)
{
  if (realTermEncrypt (t))
    {
      //int authSubt = isSomeTermlistSubterm(t,auth);
      //if(!authSubt) return 0;

      if (t->helper.fcall)
	{
	  return containLTSharedKeyInPlain (TermOp (t)) != NULL;	//authentication provided by a MAC
	}
      return isPrivateKey (TermKey (t));	//authentication provided by signatures
    }
  return 0;
}

//compute global label (note that protocol labels have been previously computed)
/*
void computeGlobalLabel(Term t)
{
	if(t->seclabel.gb_auth==UNDEFINED)
	{
		if(t->essential||isSomeTermlistSubterm(t,authav))
			t->seclabel.gb_auth=t->seclabel.prot_auth;
		else
		{
			if(t->seclabel.prop_auth==UNDEFINED)
			{
				if(isSubtermInTermlist(t,auth))
					t->seclabel.prop_auth = AUTH;
				else t->seclabel.prop_auth = NOAUTH;
			}
			t->seclabel.gb_auth = min(t->seclabel.prot_auth,t->seclabel.prop_auth);
		}
	}
	if(t->seclabel.gb_sec==UNDEFINED)
	{
		if(t->essential||isSomeTermlistSubterm(t,secretav)||isSomeTermlistSubterm(t,authav))
			t->seclabel.gb_sec = t->seclabel.prot_sec;
		else
		{
			if(t->seclabel.prop_sec==UNDEFINED)
			{
				if(isSubtermInTermlist(t,secret))
					t->seclabel.prop_sec = SEC;
				else t->seclabel.prop_sec = NOSEC;
			}
			t->seclabel.gb_sec = min(t->seclabel.prot_sec,t->seclabel.prop_sec);
		}
	}
}
*/

Termlist
removePatternRedundancy (Termlist tl)
{
  if (tl == NULL)
    return NULL;
  Termlist basicHead = sys->know->basic;
  Termlist varsHead = sys->know->vars;
  Termlist encHead = sys->know->encrypt;

  Termlist temp = tl;
  while (temp != NULL)
    {
      //add all preceding elements
      Termlist p = temp->prev;
      while (p != NULL)
	{
	  addTermToIK (sys->know, p->term->subst);
	  p = p->prev;
	}
      p = temp->next;
      while (p != NULL)
	{
	  addTermToIK (sys->know, p->term->subst);
	  p = p->next;
	}
      int isRedundant = inKnowledge (sys->know, temp->term->subst);
      //recover initial knowledge
      recoverIK (sys->know, basicHead, varsHead, encHead);
      //check the result
      if (isRedundant)
	{
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
  return tl;
}

int
getAuthFromCrypto (struct cryptstr crypto)
{
  if (crypto.type == MAC || crypto.type == SIG || crypto.type == LTSYM
      || crypto.type == KEYPOS)
    return AUTH;
  else
    return NOAUTH;
}

int
getSecFromCrypto (struct cryptstr crypto)
{
  if (crypto.type == HASH || crypto.type == MAC || crypto.type == LTSYM
      || crypto.type == PK || crypto.type == KEYPOS)
    return SEC;
  else
    return NOSEC;
}

int
isMoreSecure (int a1, int s1, int a2, int s2)
{
  return (a2 >= a1 && s2 >= s1);
}



//check if a term's type fall into the comp type list. Note that t's type is either encryption or hash
Term
typeOverlapped (Term t)
{
  if (t == NULL)
    return NULL;
  if (realTermLeaf (t))
    return NULL;
  Term type = getTermType (t);
  Termlist tl = forbidden;
  while (tl != NULL)
    {

      if (!isComparable (tl->term->originType, term))	//only consider terms whose types are originally disjoint
      {
    	  Term type1 = getTermType (tl->term);
    	  if (isComparable (type, type1))
    	  {
    		  return type1;
    	  }
	  }
      tl = tl->next;
    }
  return NULL;
}

//return true if there is a list of terms whose non-composed subterms with bounded depth cover those in av
/*
int termlistAVCovered(Termlist av, Termlist tl, int dep, int size)
{
	Termlist tmp;
	Termlist av1=NULL;
	//check first half of the list
	for(tmp = tl->prev; tmp!=NULL; tmp = tmp->prev)
	{
		int d = depth(tmp->term->subst);
		if(d<=dep&&isSomeTermlistSubterm(tmp->term->subst,av))
		{
			Termlist tl1 = extractAV(NULL,tmp->term->subst,0);
			int isContained = termlistContained(tl1,av);
			if(d==dep&&isContained)
				return size>size_of_term(tmp->term->subst);
			if(isContained) return 1;
			av1 = termlistConcat(av1,tl1);
			if(termlistContained(av1,av)) return 1;
		}
	}

	//check second half of the list
	for(tmp = tl->next; tmp!=NULL; tmp = tmp->next)
	{
		int d = depth(tmp->term);
		if(d<=depth&&isSomeTermlistSubterm(tmp->term->subst,av)!=NULL)
		{
			Termlist tl1 = extractAV(NULL,tmp->term->subst,0);
			int isContained = termlistContained(tl1,av);
			if(d==depth&&isContained)
				return size>size_of_term(tmp->term->subst);
			if(isContained) return 1;
			av1 = termlistConcat(av1,tl1);
			if(termlistContained(av1,av)) return 1;
		}
	}

	termlistDelete(av1);
	return 0;
}

*/
int
isReplicated (Termlist tl)
{
  Termlist tmp;
  for (tmp = tl->prev; tmp != NULL; tmp = tmp->prev)
    if (isTermEqual (tl->term, tmp->term))
      return 1;
  for (tmp = tl->next; tmp != NULL; tmp = tmp->next)
    if (isTermEqual (tl->term, tmp->term))
      return 1;
  return 0;
}


int
removeReplicateInHash (Termlist * keep)
{
  Termlist tl = *keep;
  int removed = false;
  while (tl != NULL)
    {
      if (isReplicated (tl))
	{
	  Termlist tmp = tl->next;
	  *keep = termlistDelTerm (tl);
	  tl = tmp;
	  removed = true;
	  continue;
	}
      tl = tl->next;
    }
  return removed;
}

/*
int removeOverProtection(Termlist *keep)
{
	Termlist tl;
	for(tl=*keep;tl!=NULL;tl=tl->next)
	{
		Term t = tl->term->subst;
		if(realTermEncrypt(t)&&!isSubtermInTermlist(t,auth))
		{
			Termlist av = extractAV(NULL,t,0);
			Termlist comm = termlistConjunct(av,authav);
			if(termlistAVCovered(comm,tl,depth(t), size_of_term(t)))
			{
				*keep = termlistDelTerm(tl);
				return 1;
			}
		}
	}
	return 0;
}

int removeRedundancyInHash(Termlist *keep)
{
	int replicated = removeReplicateInHash(keep);
	int overprotected = removeOverProtection(keep);
	return replicated||overprotected;
}
*/
//we have to be sure that an unexpected function symbol should not pop-up
//this is due to the exclusiveness of the syntactic criterion
void
UnexpectedFuncSymbForHash (Termlist * keep, Termlist * pull)
{
  if (hasTuple && *pull != NULL)
    {
      if (*keep != NULL || (*pull)->next != NULL)
	{
	  *keep = termlistConcat (*keep, *pull);
	  *pull = NULL;
	}
    }
  if (*keep == NULL && *pull != NULL && (*pull)->next == NULL)
    {
      Term term = (*pull)->term->subst;
      if (realTermEncrypt (term))
	{
	  if (!term->helper.fcall || inTermlist (topconst, TermKey (term)))
	    {
	      *keep = termlistAdd (*keep, (*pull)->term);
	      free (*pull);
	      *pull = NULL;
	    }
	}
    }
}

void
UnexpectedFuncSymbForEnc (Termlist * keep, Termlist * pull)
{
  if (hasTuple && *pull != NULL)
    {
      if (*keep != NULL || (*pull)->next != NULL)
	{
	  *keep = termlistConcat (*keep, *pull);
	  *pull = NULL;
	}
    }
  if (*keep == NULL && *pull != NULL && (*pull)->next == NULL)
    {
      Term term = (*pull)->term->subst;
      if (realTermEncrypt (term))
	{
	  if (term->helper.fcall && inTermlist (topconst, TermKey (term)))
	    {
	      *keep = termlistAdd (*keep, (*pull)->term);
	      free (*pull);
	      *pull = NULL;
	    }
	}
    }
}

/*
int notSecureByOracle(Term type)
{
	return compareTypelist(type,oraclecomp);
}
*/
//check if t occurs in some tuple whose depth is greater than the term's depth

int
secondOccurrenceInTuple (Term bigterm, int depth)
{
  if (depth > currdepth || isComparable (getTermType (bigterm), smalltype))
    return 0;
  if (isTermEqual (smallterm, bigterm))
    return 1;
  if (realTermEncrypt (bigterm))
    {
      int found = secondOccurrenceInTuple (TermOp (bigterm), depth + 1);
      if (found)
	return 1;
      else if (!bigterm->helper.fcall)
	return secondOccurrenceInTuple (TermKey (bigterm), depth + 1);
    }
  if (realTermTuple (bigterm))
    return secondOccurrenceInTuple (TermOp1 (bigterm), depth) ||
      secondOccurrenceInTuple (TermOp2 (bigterm), depth);
  return 0;
}

Termlist
removePatVar (Termlist tmp, Termlist * keep)
{
  Termlist removed = tmp;
  if (tmp == *keep)
    {
      *keep = tmp->next;
      if (*keep)
	(*keep)->prev = NULL;
    }
  else
    {
      tmp->prev->next = tmp->next;
      if (tmp->next)
	tmp->next->prev = tmp->prev;
    }
  tmp = tmp->next;
  free (removed);
  return tmp;
}

/*
int composedTermSecure(Termlist pt)
{
	Termlist tl;
	for(tl=pt; tl!=NULL;tl=tl->next)
	{
		Term t = tl->term->subst;
		if(!realTermLeaf(t)&&t->seclabel.gb_sec> NOSEC)
		{
			return 1;
		}
	}
	return 0;
}
*/

Term
createTmpTerm (int isHash, Termlist pull, Termlist keep, Term k)
{
  Term op = createTermFromPatList (keep);
  Term u = createTermFromPatList (pull);
  Term tmp;
  if (op == NULL)
    tmp = NULL;
  else
    {
      if (isHash)
	{
	  tmp = makeTermEncrypt (op, k);
	  tmp->helper.fcall = true;
	}
      else
	{
	  tmp = makeTermEncrypt (op, termDuplicate (k));
	}
    }
  return makeTermTuple (u, tmp);
}

//check if t contain essential term
int
containEssentialTerm (Term t)
{
  if (t->e_contain == UNKNOWN)
    {
      if (essentialForAuth (t) == YES || essentialForSec (t) == YES)
	{
	  t->e_contain = YES;
	}
      else if (realTermEncrypt (t))
	{
	  //if(t->helper.fcall==true)
	  t->e_contain = containEssentialTerm (TermOp (t));
	  //t->e_contain= containEssentialTerm(TermOp(t))||containEssentialTerm(TermKey(t));
	}
      else if (realTermTuple (t))
	{
	  t->e_contain = containEssentialTerm (TermOp1 (t))
	    || containEssentialTerm (TermOp2 (t));
	}
    }
  return t->e_contain;
}

int
isEssentialTerm (Term t)
{
  return t->e_auth == YES || t->e_sec == YES;
}

//check if all essential subterms of t are contained in another term u
int
allEssentialTermContained(Term t, Term u)
{
	if(realTermLeaf(t))
	{
		if(isEssentialTerm(t))
		{
			if(isSubterm(t,u)) return 1;
			return 0;
		}
		return 1;
	}
	else if(realTermEncrypt(t))
	{
		if(t->helper.fcall)
			return allEssentialTermContained(TermOp(t),u);
		else return allEssentialTermContained(TermOp(t),u)&&
				    allEssentialTermContained(TermKey(t),u);
	}
	else return allEssentialTermContained(TermOp1(t),u)&&
			    allEssentialTermContained(TermOp2(t),u);
}
//return true if all essential subterms of t are contained as subterm of terms in a termlist
int
AllEssentialTermContainedInTermlist (Term t, Termlist tl)
{
    if (isEssentialTerm (t))
	{
	  Termlist tmp = tl;
	  while (tmp != NULL)
	    {
	      if (isSubterm (t, tmp->term->subst))
	    	  return 1;
	      tmp = tmp->next;
	    }
	  return 0;
	}
    else if (realTermLeaf (t))
    {
    	return 0;
 	}
    else if (realTermEncrypt (t))
    {
      if (t->helper.fcall)
	return AllEssentialTermContainedInTermlist (TermOp (t), tl);
      else
	return AllEssentialTermContainedInTermlist (TermOp (t), tl) &&
	       AllEssentialTermContainedInTermlist (TermKey (t), tl);
    }
  else
    return AllEssentialTermContainedInTermlist (TermOp1 (t), tl) &&
           AllEssentialTermContainedInTermlist (TermOp2 (t), tl);
}

//check if t occurs in some other field
int
occurSomeWhere (Term t, Termlist keep)
{
  Termlist tl;
  for (tl = keep; tl != NULL; tl = tl->next)
    {
      if (!isTermEqual(t, tl->term->subst)&&isSubterm (t, tl->term->subst))
	return 1;
    }
  return 0;
}

//handle hashes
int
buildPatternForHash (Term plain, int secretOverlap, Termlist * pull, Termlist * keep,
		     int sec_outer, int auth_outer, int sec_inner,
		     int auth_inner, Term hashfunc)
{
  Termlist tmp = *keep;
  int eqtype = SYSTEM_TRIVIAL;
  while (tmp != NULL)
    {
      Term t = tmp->term->subst;
      //try to remove
      Term ttmp;
      Termlist newpull, newkeep;
      Termlist prev = tmp->prev;
      int isSafe;
      newpull = *pull;
      if (prev == NULL)
	newkeep = tmp->next;
      else
	{
	  prev->next = tmp->next;
	  newkeep = *keep;
	}
      if(t->e_auth==YES&&auth_inner==AUTH&&occurSomeWhere(t,newkeep))
      {
    	  if (prev != NULL)
    	  	prev->next = tmp;
    	        tmp = tmp->next;
    	  continue;
      }
      ttmp = createTmpTerm (1, newpull, newkeep, hashfunc);
      if(plain!=NULL)
    	  ttmp = makeTermEncrypt(plain,ttmp);
      //eprintf("check for removing:");
      //printTerm(ttmp);
      //eprintf("\n");
      //but we only remove t if t needs not be secure or there is another occurrence of t in the outer constructor
      //and we do not remove the hash completely
      int not_complete_removal = newkeep != NULL || newpull != NULL;
      smallterm = t;
      smalltype = getTermType (parentTerm);
      int redundancy = secondOccurrenceInTuple (bigterm, 0);
      int need_not_secure = !isLTKey (t) && !containEssentialTerm (t);

      isSafe = not_complete_removal && (redundancy || need_not_secure ||
					(((t->e_auth==YES&& isSubterm(t,plain))||AllEssentialTermContainedInTermlist(t, newkeep))
					 && checkLabelPreservation (ttmp,sec_outer,auth_outer,ACCESSIBLE)));
      /*
      isSafe = not_complete_removal && (redundancy || need_not_secure ||
					(AllEssentialTermContainedInTermlist(t, newkeep)
					 && checkLabelPreservation (ttmp,
								    sec_outer,
								    auth_outer,
								    ACCESSIBLE)));
	  */
      if(plain==NULL)
    	  termDelete (ttmp);
      else
      {
    	  termDelete(TermKey(ttmp));
    	  free(ttmp);
      }
      if (isSafe)
	{
	  if (prev != NULL)
	    prev->next = tmp;
	  eqtype = SYSTEM_REMOVE_FIELD;
	  tmp = removePatVar (tmp, keep);
	  continue;
	}
      //try to pull it out
      else if (!secretOverlap)
	{
	  newpull = makeTermlist ();
	  newpull->term = tmp->term;
	  newpull->next = *pull;
	  //computeGlobalLabel(t);
	  Term ttmp = createTmpTerm (1, newpull, newkeep, hashfunc);
	  if(plain!=NULL)
		  ttmp = makeTermEncrypt(plain,ttmp);
	  if (prev != NULL)
	    prev->next = tmp;
	  //eprintf("check for pulling:");
	  //printTerm(ttmp);
	  //eprintf("\n");
	  isSafe = (sec_outer >= sec_inner && auth_outer >= auth_inner)
	    || checkLabelPreservation (ttmp, sec_outer, auth_outer,
				       ACCESSIBLE);
      if(plain==NULL)
    	  termDelete (ttmp);
      else
      {
    	  termDelete(TermKey(ttmp));
    	  free(ttmp);
      }
	  free (newpull);
	  newpull = NULL;
	  //int need_not_secure = t->seclabel.gb_auth == NOAUTH && t->seclabel.gb_sec==NOSEC;
	  if (isSafe)		//&&(need_not_secure||secureOracle||composedTermSecure(newkeep)))
	    {
	      //if(outercrypto.type!=NONE||typeOverlapped(t)==NULL)
	      if (sec_outer != NOSEC || auth_outer != NOAUTH||typeOverlapped (t) == NULL)
		{
		  *pull = termlistAdd (*pull, tmp->term);
		  if (eqtype != SYSTEM_REMOVE_FIELD)
		    eqtype = SYSTEM_PULL_OUT;
		  tmp = removePatVar (tmp, keep);
		  continue;
		}
	    }
	}
      else if (prev != NULL)
	prev->next = tmp;
      tmp = tmp->next;
    }
  //check if some term in hash could be removed
  /*
     if(removeRedundancyInHash(keep))
     eqtype=SYSTEM_REMOVE_FIELD;
   */
  if (removeReplicateInHash (keep))
    eqtype = SYSTEM_REMOVE_FIELD;

  UnexpectedFuncSymbForHash (keep, pull);

  return eqtype;
}

int
isSubtermInPat (Termlist tl)
{
  Term t = tl->term->subst;
  Termlist tmp = tl->prev;
  while (tmp != NULL)
    {
      if (isSubterm (t, tmp->term->subst))
	return 1;
      tmp = tmp->prev;
    }
  tmp = tl->next;
  while (tmp != NULL)
    {
      if (isSubterm (t, tmp->term->subst))
	return 1;
      tmp = tmp->next;
    }
  return 0;
}

int
containOnlyAgent (Term t)
{
  if (realTermLeaf (t))
    {
      if (inTermlist (t->stype, TERM_Agent))
	return 1;
      return 0;
    }
  else if (realTermEncrypt (t))
    {
      int res1 = containOnlyAgent (TermOp (t));
      if (!res1)
	return 0;
      if (t->helper.fcall)
	return inTermlist (sys->know->basic, TermKey (t));
      else
	return containOnlyAgent (TermKey (t));
    }
  else
    return containOnlyAgent (TermOp1 (t)) && containOnlyAgent (TermOp2 (t));
}

// pull out agent names if they already occur somewhere else in the plaintext
void
pull_out_agent (Termlist * keep, Termlist * pull)
{
  Termlist tl = *keep;
  while (tl != NULL)
    {
      if (containOnlyAgent (tl->term->subst))
	{
	  Termlist ag = extractAV (NULL, tl->term->subst, 1);
	  int found = false;
	  Termlist tmp;
	  for (tmp = tl->prev; tmp != NULL; tmp = tmp->prev)
	    if (isTermlistSubterm (tmp->term->subst, ag))
	      {
		found = true;
		break;
	      }
	  if (!found)
	    for (tmp = tl->next; tmp != NULL; tmp = tmp->next)
	      if (isTermlistSubterm (tmp->term->subst, ag))
		{
		  found = true;
		  break;
		}
	  termlistDelete (ag);
	  if (found)
	    {
	      *pull = termlistAdd (*pull, tl->term);
	      tmp = tl;
	      if (tl == *keep)
		{
		  *keep = tl->next;
		  if (*keep)
		    (*keep)->prev = NULL;
		}
	      else
		{
		  tl->prev->next = tl->next;
		  if (tl->next)
		    tl->next->prev = tl->prev;
		}
	      tl = tl->next;
	      free (tmp);
	      continue;
	    }
	}
      tl = tl->next;
    }
}

/*
int containOnlyAgentInPatList(Termlist tl)
{
	Termlist tmp;
	for(tmp=tl;tmp!=NULL;tmp=tmp->next)
		if(!containOnlyAgent(tmp->term->subst)) return 0;
	return 1;
}
*/
//check if the session-key is weaker than the protection provided by term t
int
sessionKeyWeaker (Term t, Term key)
{
  if (realTermLeaf (t))
    return 0;
  if (realTermEncrypt (t))
    {
      //check if t contains all information of key, i.e., all atoms and variables in key
      Termlist av = extractAV (NULL, key, 0);
      Termlist tmp;
      for (tmp = av; tmp != NULL; tmp = tmp->next)
	if (!isSubterm (tmp->term, t))
	  return 0;
      termlistDelete (av);
      if (t->helper.fcall)
	return 1;
      else
	{
	  Term k = TermKey (t);
	  if (isPublicKey (k) || isLTSharedKey (k))
	    return 1;
	  if (isPrivateKey (k))
	    return 0;
	  return isTermEqual (key, k);
	}
    }
  return 0;
}

//check if some term required to be authenticated occurs in plaintext of some term in a termlist (pattern)
int
authenticTermInPlaintext (Termlist tl1, Termlist tl2)
{
  Termlist tmp;
  for (tmp = tl1; tmp != NULL; tmp = tmp->next)
    {
      if (plaintextAppearInTermlistPat (tmp->term, tl2))
	return 1;
    }
  return 0;
}

int
containSomeEssential (Termlist keep)
{
  Termlist tl;
  for (tl = keep; tl != NULL; tl = tl->next)
    {
      if (containEssentialTerm (tl->term->subst))
	return 1;
    }
  return 0;
}

int
getCryptoType (Term t)
{
  int type = NONE;
  if (t->helper.fcall)
    {
      Term sharedkey = containLTSharedKeyInPlain (TermOp (t));
      if (sharedkey != NULL)
	{
	  type = MAC;
	}
      else
	type = HASH;
    }
  else if (realTermEncrypt (t))
    {
      Term key = TermKey (t);
      if (isLTSharedKey (key))
	{
	  type = LTSYM;
	}
      else if (isPublicKey (key))
	{
	  type = PK;
	}
      else if (isPrivateKey (key))
	{
	  type = SIG;
	}
      else
	type = SYM;
    }
  return type;
}

//handle encryptions
int
buildPatternForEnc (Term plain, int secretOverlap, Term type, Termlist * pull,
		    Termlist * keep, int sec_outer, int auth_outer,
		    int sec_inner, int auth_inner, int cryptotype, Term key)
{
  //pull_out_agent(keep, pull);
  if (!containSomeEssential (*keep))
    {
      *pull = *keep;
      *keep = NULL;
      return SYSTEM_PULL_OUT;
    }
  Termlist tmp = *keep;
  while (tmp != NULL)
    {
      Term t = tmp->term->subst;
      if (!secretOverlap)
	{
	  //computeGlobalLabel(t);
	  tmp->term->abst = 1;
	  //check if the key is useful to protect the term
	  //int good_key=cryptotype!=SYM||!sessionKeyWeaker(t,key);
	  //pulling creates additional unifiability
	  int create_unif = sec_outer == NOSEC && auth_outer == NOAUTH &&typeOverlapped (t) != NULL;
	  //pulling leaks secret to the intruder though oracles
	  int leak_secret = isSubtermInTermlist (t, secret);
	  //term may contain sensitive agent identities

	  /*
	     int agent_sensitive = !isAuthProvider(t)&&
	     ((innercrypto.type!=SYM&&containDiffAgent(t,TermOp(innercrypto.info)))||
	     (innercrypto.type==SYM&&containDiffAgent(t,NULL)));
	   */
	  int crypto = getCryptoType(t);
	  int containAgent =
	    ((cryptotype != SYM && containDiffAgent (t, key))
	     || (cryptotype == SYM && (crypto==SIG||containDiffAgent (t, NULL))))
	    && crypto != MAC;

	  //pull out if
	  if (!leak_secret && !create_unif)
	    {
	      Termlist newpull = makeTermlist ();
	      Termlist newkeep;
	      newpull->prev = NULL;
	      newpull->term = tmp->term;
	      newpull->next = *pull;

	      Termlist prev = tmp->prev;
	      Term ttmp;
	      if (prev == NULL)
		newkeep = tmp->next;
	      else
		{
		  prev->next = tmp->next;
		  newkeep = *keep;
		}
	      ttmp = createTmpTerm (0, newpull, newkeep, key);
	      if(plain!=NULL)
	    	  ttmp = makeTermEncrypt(plain,ttmp);
	      //eprintf("Try to pull:");
	      //printTerm(ttmp);
	      //eprintf("\n");

	      int isSafe = (sec_outer >= sec_inner
			    && auth_outer >= auth_inner)
			    || !containEssentialTerm(t) || (t->seclabel.prot_sec==NOSEC && isSubterm(t,key))
		|| checkLabelPreservation (ttmp, sec_outer, auth_outer,
					   ACCESSIBLE);
	      if(plain==NULL)
	    	  termDelete (ttmp);
	      else
	      {
	    	  termDelete(TermKey(ttmp));
	    	  free(ttmp);
	      }
	      free (newpull);
	      newpull = NULL;
	      int occur = occurSomeWhere (t, newkeep);
	      int crypto = getCryptoType(t);
	      //hashes that contain agent names and stay alone (single field) are not considered agent-sensitive (except signatures)
	      int agent_sensitive = containAgent&&(crypto==SIG||newkeep!=NULL);
	      if (prev != NULL)
		prev->next = tmp;
	      if (isSafe&& ((agent_sensitive&&occur) || (!agent_sensitive &&(!occur||auth_inner!=AUTH))))
		{
		  *pull = termlistAdd (*pull, tmp->term);
		  //remove from keep list
		  tmp = removePatVar (tmp, keep);
		  continue;
		}
	    }
	}
      //cannot pull out
      tmp = tmp->next;
    }
  UnexpectedFuncSymbForEnc (keep, pull);
  /* agent names encrypted with long-term shared keys may still be essential ==> must be kept
     if(containOnlyAgentInPatList(*keep))
     {
     eqtype=SYSTEM_PULL_OUT;
     *pull = termlistConcat(*pull,*keep);
     *keep=NULL;
     }
   */
  return *pull != NULL ? SYSTEM_PULL_OUT : SYSTEM_TRIVIAL;
}

//check if the type violate disjointness condition
int
isPatternDisjointness (Term typ1)
{
  Eqlist eqlist = eql;
  while (eqlist != NULL)
    {
      Term typ2 = getTermType (eqlist->eq->left);
      int isCompr = isComparable (typ1, typ2);
      if (isCompr)
	{
	  return 0;
	}
      eqlist = eqlist->next;
    }
  return 1;
}


void
addForbiddenTypes (Term pattype, Term term)
{
  if (realTermTuple (pattype))
    {
      addForbiddenTypes (TermOp1 (pattype), term);
      addForbiddenTypes (TermOp2 (pattype), term);
    }
  else //if (realTermEncrypt (pattype))
    {
      Term t = substitutedTerm (pattype);
      if(realTermEncrypt(t))
      {
		  t->originType = term;
		  forbidden = termlistAdd (forbidden, t);
      }
    }
}

int
findPositionForOrdering (Termlist * pattern, Termlist type)
{
  Termlist tmp2 = type;
  int pos = 1;
  while (tmp2 != NULL)
    {
      if (!isTermEqual (tmp2->term, TERM_Ticket))
	{
	  Termlist tmp1 = *pattern;
	  while (tmp1 != NULL)
	    {
	      Term type1 = getTermType (tmp1->term->subst);
	      if (!isComparable (tmp2->term, type1))
		{
		  //we found a type that does not match, so keep it at pos
		  tmpl = tmp1;
		  return pos;
		}
	      tmp1 = tmp1->next;
	    }
	}
      tmp2 = tmp2->next;
      pos++;
    }
  return 0;
}

int
findPositionForRefinement (Termlist * pattern, Termlist type)
{
  Termlist tmp2 = type;
  int pos = 1;
  int good_pos = 0;
  int min_size = INT_MAX;
  while (tmp2 != NULL)
    {
      if (!isTermEqual (tmp2->term, TERM_Ticket))
	{
	  Termlist tmp1 = *pattern;
	  while (tmp1 != NULL)
	    {
	      Term type1 = getTermType (tmp1->term->subst);
	      if (!isComparable (tmp2->term, type1))
		{
		  //we save the term with smallest size
		  int size = size_of_term (tmp1->term->subst);
		  if (size < min_size)
		    {
		      tmpl = tmp1;
		      min_size = size;
		      good_pos = pos;
		    }
		}
	      tmp1 = tmp1->next;
	    }
	}
      tmp2 = tmp2->next;
      pos++;
    }
  return good_pos;
}

int
adaptKeeplistByRefinement (Termlist * keep, Termlist * pull, Termlist type)
{
  if (*pull == NULL)
    return 0;
  int pos = findPositionForRefinement (pull, type);
  if (!pos)
    return 0;
  int i = 1;
  Termlist tmp = *keep;
  while (i != pos)
    {
      tmp = tmp->next;
      i++;
    }
  Term t = tmpl->term;
  Termlist tl = makeTermlist ();
  tl->term = t;
  tl->prev = tmp->prev;
  tl->next = tmp;
  if (tmp->prev != NULL)
    tmp->prev->next = tl;
  tmp->prev = tl;
  if (i == 1)
    *keep = tl;
  (*pull) = termlistDelTerm (tmpl);
  return 1;
}

int
adaptKeeplistByOrdering (Termlist * keep, Termlist type)
{
  int pos = findPositionForOrdering (keep, type);
  if (!pos)
    return 0;
  Termlist tmp1;
  int i = 1;
  tmp1 = *keep;
  while (i != pos)
    {
      tmp1 = tmp1->next;
      i++;
    }
  //swap tmp1 and tmpl
  Term tmp = tmp1->term;
  tmp1->term = tmpl->term;
  tmpl->term = tmp;
  return 1;
}

void
findSmallestPatVar (Term pat, Term * var, int *min_size)
{
  if (realTermLeaf (pat))
    {
      int size = size_of_term (pat->subst);
      if (size < *min_size)
	{
	  *min_size = size;
	  *var = pat;
	}
    }
  else if (realTermTuple (pat))
    {
      findSmallestPatVar (TermOp1 (pat), var, min_size);
      findSmallestPatVar (TermOp2 (pat), var, min_size);
    }
}

int
fixUnifIssue (const int isHash, Term key, Termlist * keep, Termlist * pull,
	      int bound)
{
  if (bound > UNIFDEPTH)
    return 0;
  int result;
  Term u;
  if (isHash)
    {
      u = makeTermFcall (turn_termlist_to_tuple (*keep), key);
    }
  else
    {
      u = makeTermEncrypt (turn_termlist_to_tuple (*keep), key);
    }
  if (!inTermlist (unif, u))
    unif = termlistAdd (unif, u);
  else
    {
      result = NO_FIX;
    }
  //check if a pair of messages become unifiable

  Term subst = substitutedTerm (u);
  //only check for unifiability if the term contains some authenticated elements in plaintext
  //if(plaintextTermlistInTerm(subst,authav))
  if (containEssentialTerm (subst))
    {
      Term type = typeOverlapped (subst);
      if (type != NULL)
	{
	  Termlist typel = turn_tuple_to_termlist (TermOp (type));
	  if (adaptKeeplistByOrdering (keep, typel)
	      || adaptKeeplistByRefinement (keep, pull, typel))
	    {
	      result = fixUnifIssue (isHash, key, keep, pull, bound + 1);
	      if (result == NOT_NEED)
		result = FIXED;
	    }
	  else
	    {
	      result = NO_FIX;
	    }
	  termlistDelete (typel);
	}
      else
	result = NOT_NEED;
    }
  termlistDestroy (unif);
  unif = NULL;
  return result;
}

void
constructKeepPull (Term right, Termlist * keep, Termlist * pull)
{
  if (realTermLeaf (right))
    *pull = termlistAdd (*pull, right);
  else if (realTermEncrypt (right))
    *keep = turn_tuple_to_termlist (TermOp (right));
  else
    {
      constructKeepPull (TermOp1 (right), keep, pull);
      constructKeepPull (TermOp2 (right), keep, pull);
    }
}

/*
Equation adaptAndAddEquation(Term t, int eqtype, Equation eq, Term pattype,Termlist *keep, Termlist *pull)
{
	Term key = TermKey(t);
	if(eqtype>SYSTEM_TRIVIAL)
	{
		eq->type=eqtype;
		if(*keep==NULL)
		{
			if(*pull==NULL)
			{
				//should be a nil-free abstraction, so we keep the smallest term
				int min = INT_MAX;
				Term right;
				findSmallestPatVar(TermOp(eq->left),&right,&min);
				eq->right=right;
			}
			else eq->right=termlist_to_tuple(*pull);
		}
		else
		{
			//adapt the equation type
			if(*pull==NULL)
			{
				if(t->helper.fcall)
				{
					if(eqtype!=SYSTEM_REMOVE_FIELD)
						//eq->type=SYSTEM_REORDER;
						eq->type=SYSTEM_TRIVIAL;
				}
				else eq->type=SYSTEM_TRIVIAL;

			}
			Term u ;
			if(eq->left->helper.fcall)
				u = makeTermFcall(turn_termlist_to_tuple(*keep), key);
			else
				u=makeTermEncrypt(turn_termlist_to_tuple(*keep), key);
			if(*pull==NULL) eq->right=u;
			else eq->right=termlist_to_tuple(termlistAdd(*pull,u));

			Equation neweq=resolveDisjointnessConflict(pattype);
			if(neweq!=NULL)
			{
				if(abstCost(eq)<abstCost(neweq))
				{
					eql = removeEquationFromList(eql,neweq);
				}
				else
				{
					eprintf("remove eq:");
					printEquation(eq->left, eq->right);
					eprintf("\n");
					deleteEquation(eq);
					termlistDelete(*keep);
					termlistDelete(*pull);
					*keep = *pull = NULL;
					eq= neweq;
					eprintf("new eq:");
					printEquation(eq->left, eq->right);
					eprintf("\n");

					patternMatching(eq->left,t);
					constructKeepPull(eq->right,keep,pull);
				}
			}
			int result = fixUnifIssue(t->helper.fcall, key, keep, pull,0);
			if(result!=NO_FIX)
			{
				termDelete(eq->right);
				Term u ;
				if(eq->left->helper.fcall)
					u = makeTermFcall(turn_termlist_to_tuple(*keep), key);
				else
					u=makeTermEncrypt(turn_termlist_to_tuple(*keep), key);
				if(*pull==NULL) eq->right=u;
				else eq->right=termlist_to_tuple(termlistAdd(*pull,u));
			}
			else
			{
				//cannot fix this way, so keep the trivial equation
				eq->right = eq->left;
				eq->type=SYSTEM_TRIVIAL;
			}
		}
	}
	else
	{
		eq->right = eq->left;
		eq->type=SYSTEM_TRIVIAL;
	}
	makeHomomorphic(eq->right);
	eql=equationlistAdd(eql,eq);
	//eprintf("equation created:");
	//printEquation(eq->left,eq->right);
	//eprintf("\n");
	return eq;
}
*/

int
termlistCompare (Termlist tl1, Termlist tl2)
{
  while (tl1 != NULL)
    {
      if (tl2 == NULL)
	return 0;
      if (!isTermEqual (tl1->term, tl2->term))
	return 0;
      tl1 = tl1->next;
      tl2 = tl2->next;
    }
  return 1;
}

Equation
createEquationFromKeepPull(Equation eq, Term key, Termlist keep, Termlist pull)
{
    Term u;
    if (eq->left->helper.fcall)
	u = makeTermFcall (turn_termlist_to_tuple (keep), key);
    else
	u = makeTermEncrypt (turn_termlist_to_tuple (keep), key);
    if (pull == NULL)
	{
	  eq->right = u;
	  Termlist tmp = turn_tuple_to_termlist (TermOp (eq->left));
	  int equal = termlistCompare (tmp, keep);
	  if (equal)
	    eq->type = SYSTEM_TRIVIAL;
	  else
	    eq->type = SYSTEM_REMOVE_FIELD;
	  termlistDelete (tmp);
	}
    else
	{
 	  eq->right = makeTermTuple(termlist_to_tuple (pull),u);
	  eq->type = SYSTEM_PULL_OUT;
	}
	return eq;
}

Equation
resolveTypeConflict(Equation eq, Term pattype)
{
	Equation neweq=resolveDisjointnessConflict(pattype);
	if(neweq!=NULL)
	{
		if(abstCost(eq)<abstCost(neweq))
		{
			eql = removeEquationFromList(eql,neweq);
			return eq;
		}
		else return neweq;
	}
	return eq;
}

Equation
adaptAndAddEquation (Term t, int eqtype, Equation eq, Term pattype,
		     Termlist * keep, Termlist * pull)
{
  Term key = TermKey (eq->left);
  eq->type = eqtype;
  if (eqtype > SYSTEM_TRIVIAL)
    {
      if (*keep == NULL)
      {
		  if (*pull == NULL)
			{
			  //should be a nil-free abstraction, so we keep the smallest term
			  int min = INT_MAX;
			  Term right;
			  findSmallestPatVar (TermOp (eq->left), &right, &min);
			  eq->right = right;
			}
		  else
			eq->right = termlist_to_tuple (*pull);
      }
      else eq = createEquationFromKeepPull(eq,key,*keep,*pull);
    }
  else eq->right = eq->left;

  //first we resolve any type-conflict
	Equation neweq=resolveTypeConflict(eq,pattype);
	if(neweq!=eq)
	{
		deleteEquation(eq);
		termlistDelete(*keep);
		termlistDelete(*pull);
		*keep = *pull = NULL;
		eq= neweq;
		patternMatching(eq->left,t);
		constructKeepPull(eq->right,keep,pull);
		key = TermKey(neweq->left);
	}
	else eql = equationlistAdd (eql, neweq);

  if(*keep!=NULL)
  {
  	  //we now check for unifiability of terms
	  int result = fixUnifIssue (t->helper.fcall, key, keep, pull, 0);
	  if (result == FIXED)
	    {
	      if(eq->right!=eq->left) termDelete (eq->right);
	      eq = createEquationFromKeepPull(eq,key,*keep,*pull);
	    }
	  else if(result==NO_FIX)
	    {
	      //cannot fix this way, so keep the trivial equation
	      eq->right = eq->left;
	      eq->type = SYSTEM_TRIVIAL;
	    }
  }
  makeHomomorphic (eq->right);
  //eprintf("equation created:");
  //printEquation(eq->left,eq->right);
  //eprintf("\n");
  return eq;
}

//check if a term's type comparable with some secret's type
int
overlapSecret (Term type)
{
  Termlist tl = secret;
  while (tl != NULL)
    {
      Term sectype = getTermType (tl->term);
      if (isComparable (type, sectype))
	return 1;
      tl = tl->next;
    }
  return 0;
}

//create an equation that abstracts a protocol term
int
createEquationForTerm (Term plain, Term t, int sec_outer, int auth_outer)
{
  int success = false;
  int eqtype;
  if (realTermEncrypt (t))
    {
      //check if the term's secrecy label are not less than what the outercrypto provides
      if (isLTKey (t))
	return 0;
      Term type = getIntendedPatternType (t);
      //eprintf("create equation for term:");
      //printTerm(t);
      //eprintf(":");
      //printTerm(type);
      //printf("\n");
      if (coveredByFixEquations (type))
	return 0;
      int cryptotype = getCryptoType (t);
      struct cryptolabel clb = getCryptoLabel (t);
      Term key = TermKey (t);
      if (t->helper.fcall
	  && (inTermlist (topconst, key) || inTermlist (homfunc, key)))
	{
	  //in order to use the syntactic criterion, the equation for this hash function needs to be homomorphic
	  if (!inTermlist (homeq, key))
	    {
	      eql =
		equationlistAdd (eql,
				 createTicketHomomorphicPatForHash (key));
	      homeq = termlistAdd (homeq, key);
	    }
	  currdepth++;
	  return createEquationForTerm (plain, TermOp (t),
					max (sec_outer, clb.sec_crypt),
					max (auth_outer, clb.auth_crypt));
	}

      Equation eq = makeEquation ();
      eq->type = SYSTEM_TRIVIAL;
      eq->left = createPatFromType (type);
      eq->left->stype = termlistAdd (NULL, type);
      patternMatching (eq->left, t);
      Termlist op = turn_tuple_to_termlist (TermOp (eq->left));
      Termlist pull = NULL;

      int secretOverlap = overlapSecret (type);
      //hashes
      if (t->helper.fcall)
	{
	  parentTerm = t;
	  eqtype =
	    buildPatternForHash (plain, secretOverlap, &pull, &op, sec_outer,
				 auth_outer, clb.sec_crypt, clb.auth_crypt,
				 key);
	  success = eqtype > SYSTEM_TRIVIAL;
	}
      //encryptions
      else
	{
	  if (cryptotype == SYM)
	    {
    	  eqtype = buildPatternForEnc (plain,secretOverlap, type, &pull, &op,
			    sec_outer, auth_outer, clb.sec_crypt,
			    clb.auth_crypt, cryptotype, key);

	      if (eqtype==SYSTEM_TRIVIAL&&!plaintextAppearInTermlist (key, comp))
	    	  success = createEquationForTerm (TermOp(t), key, SEC, AUTH);

	    }
	  else
	    {
	      eqtype =
		buildPatternForEnc (plain,secretOverlap, type, &pull, &op,
				    sec_outer, auth_outer, clb.sec_crypt,
				    clb.auth_crypt, cryptotype, key);
	      success = eqtype > SYSTEM_TRIVIAL;
	    }
	}

      Equation neweq = adaptAndAddEquation (t, eqtype, eq, type, &op, &pull);
      success = neweq->type > SYSTEM_TRIVIAL;
      //if(outercrypto.type==NONE)
      if (sec_outer == NOSEC && auth_outer == NOAUTH)
    	  addForbiddenTypes (neweq->right, term);
      termlistDelete (pull);
      termlistDelete (op);
      op = pull = NULL;
      if (!success)
	//if(trivialEquationlist(eql))
	{
	  currdepth++;
	  return createEquationForTerm (plain, TermOp (t),
					max (sec_outer, clb.sec_crypt),
					max (auth_outer, clb.auth_crypt));
	}
    }
  else if (realTermTuple (t))
    {
      int result1 =
	createEquationForTerm (plain, TermOp1 (t), sec_outer, auth_outer);
      int result2 =
	createEquationForTerm (plain, TermOp2 (t), sec_outer, auth_outer);
      return result1 || result2;
    }
  return success;
}


void
OriginalLabelInit (Term t)
{
  t->seclabel.org_auth = t->seclabel.org_sec = UNDEFINED;
}

void
computeOriginalLabel (Term t, int seclabel, int authlabel, int access)
{
  if (access == ACCESSIBLE)
    t->accessible = access;
  if (seclabel != NOSEC || authlabel != NOAUTH)
    {
      if (t->inIK == YES
	  || (t->inIK == UNKNOWN && inKnowledge (sys->know, t)))
	{
	  t->seclabel.org_sec = NOSEC;
	  t->seclabel.org_auth = NOAUTH;
	}
    }
  if (t->seclabel.org_sec == UNDEFINED)
    t->seclabel.org_sec = seclabel;
  else
    t->seclabel.org_sec = min (t->seclabel.org_sec, seclabel);
  if (t->seclabel.org_auth == UNDEFINED)
    t->seclabel.org_auth = authlabel;
  else
    t->seclabel.org_auth = max (t->seclabel.org_auth, authlabel);
  if (realTermEncrypt (t))
    {
      Term op = TermOp (t);
      if (t->helper.fcall)
	{
	  if (isInverseKey (TermKey (t)))
	    computeOriginalLabel (op, seclabel, authlabel, access);
	  else if (containLTSharedKeyInPlain (op) != NULL)	//it is a MAC
	    computeOriginalLabel (op, SEC, AUTH, NOT_ACCESSIBLE);
	  else
	    {
	      if (plaintextAuthentic (ORG,op))
		{
		  computeOriginalLabel (op, SEC, AUTH, NOT_ACCESSIBLE);
		}
	      else
		computeOriginalLabel (op, SEC, max(authlabel, MAYBE_AUTH), NOT_ACCESSIBLE);	//otherwise, a normal hash
	    }
	}
      else
	{
	  Term key = TermKey (t);
	  if (isPublicKey (key))	//if it is public-key encryption
	    {
	      //check if some agent identifiers are there
	      if (containDiffAgent (op, TermOp (key)))
		{
		  computeOriginalLabel (op, SEC, AUTH, access);
		}
	      else
		{
		  if (plaintextAuthentic (ORG,op))
		    computeOriginalLabel (op, SEC, AUTH, access);
		  else
		    computeOriginalLabel (op, SEC, authlabel, access);
		}
	    }
	  else if (isPrivateKey (key))	// if it is a signature
	    {
	      computeOriginalLabel (op, seclabel, AUTH, access);
	    }
	  else if (isLTSharedKey (key))
	    {
	      computeOriginalLabel (op, SEC, AUTH, access);
	    }
	  else			//symmetric encryption with a session key
	    {
	      int newsec =
		key->accessible == NOT_ACCESSIBLE ? SEC : MAYBE_SEC;
	      int newauth;
	      if(comparePlainAndKey(op,key))
	    	  newauth = authlabel;
	      else newauth  = max (authlabel, MAYBE_AUTH);
	      computeOriginalLabel (op, newsec, newauth, access);
	    }
	}
    }
  else if (realTermTuple (t))
    {
      computeOriginalLabel (TermOp1 (t), seclabel, authlabel, access);
      computeOriginalLabel (TermOp2 (t), seclabel, authlabel, access);
    }
}


void
createEquations ()
{
  homeq = NULL;
  Termlist tl = comp;
  while (tl != NULL)
    {
      term = getIntendedPatternType (tl->term);
      bigterm = tl->term;
      currdepth = 0;
      labelInit (tl->term, OriginalLabelInit);
      computeOriginalLabel (tl->term, NOSEC, NOAUTH, ACCESSIBLE);
      createEquationForTerm (NULL,tl->term, NOSEC, NOAUTH);
      tl = tl->next;
    }
  termlistDelete (homeq);
}

void
abstractSecrets ()
{
  Termlist tl;
  for (tl = secret; tl != NULL; tl = tl->next)
    tl->term = frec (tl->term);
}

int
containNoSendEvent (Role r)
{
  Roledef rd;
  for (rd = r->roledef; rd != NULL; rd = rd->next)
    if (rd->type == SEND)
      return 0;
  return 1;
}

void
removeEventInRole (Role * r)
{
  Roledef rd = (*r)->roledef;
  Roledef prev = NULL;
  while (rd != NULL)
    {
      if (rd->type != CLAIM)
	{
	  Roledef tmp = rd;
	  if (prev == NULL)
	    {
	      (*r)->roledef = rd->next;
	    }
	  else
	    {
	      prev->next = rd->next;
	    }
	  rd = removeRoleEvent (prev, tmp);
	  continue;
	}
      prev = rd;
      rd = rd->next;
    }
}

System
removeUselessEvents (System sys)
{
  Protocol p;
  for (p = sys->protocols; p != NULL; p = p->next)
    {
      //it seems removing such redundancies in the main protocol make things worse, so we do only for oracles
      if (isHelperProtocol (p))
	{
	  Role r = p->roles;
	  while (r != NULL)
	    {
	      if (containNoSendEvent (r))
		{
		  removeEventInRole (&r);
		}
	      r = r->next;
	    }
	}
    }
  return sys;
}

System
abstractSystem (System sys)
{
  heuristicInit (sys);
  initialAnalysis ();
  createEquations ();
  //eprintf("equations:\n");
  //printEquationlist(eql);
  //eprintf("\n");
  //apply several abstractions
  typebasedInit (sys);
  System abssys;
  typebasedRes = typebasedAbstraction ();
  if (typebasedRes)
    {
      abstractSecrets ();

      //create new system
      abssys = systemDuplicate (sys);
      //apply type-based abstraction
      abssys = performAbstraction (abssys, sys);

      //do a redundancy abstraction
      redabsInit (abssys);
      redRes = redundancyAbstraction ();
      if (redRes)
	abssys = performAbstraction1 (abssys);

      //do an avrem abstraction
      avremInit (abssys);
      avRes = avRemoval ();
      if (avRes)
	abssys = performAbstraction1 (abssys);
    }
  else
    {
      resetAbs (sys);
      redabsInit (sys);
      redRes = redundancyAbstraction ();
      if (redRes)
	{
	  abssys = systemDuplicate (sys);
	  //do a redundancy abstraction
	  abssys = performAbstraction (abssys, sys);

	  //do an avrem abstraction
	  avremInit (abssys);
	  avRes = avRemoval ();
	  if (avRes)
	    abssys = performAbstraction1 (abssys);
	}
      else
	{
	  resetAbs (sys);
	  avremInit (sys);
	  avRes = avRemoval ();
	  if (avRes)
	    {
	      abssys = systemDuplicate (sys);
	      abssys = performAbstraction (abssys, sys);
	    }
	  else
	    abssys = sys;
	}
    }
  abssys = removeUselessEvents (abssys);
  eql = deleteNonPersistentEq (eql);
  heuristicDone ();
  return abssys;
}

int
abstractionSucceed ()
{
  return typebasedRes || redRes || avRes;
}

