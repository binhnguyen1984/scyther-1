/*
 * abstraction.c
 *
 *  Created on: May 5, 2014
 *      Author: nguyen
 */
#include "absfunc.h"
//abstraction

/*
 * return True if t's fields occurs as fields of u
 */
static int asField(Term t, Term u)
{
	if(isTermTuple(u))
	{
		if(!isTermTuple(t))
			return asField(t, TermOp1(u))||asField(t,TermOp2(u));
		else return asField(TermOp1(t),u) && asField(TermOp2(t),u);
	}
	return isTermEqual(t,u);
}
/*
 * check if term t is different from u
 * note that t is equal to u if it can be obtained from u by reordering its fields
 * and removing multiple occurrences of fields
 */
static int isEqual(Term t, Term u)
{
	if(isTermLeaf(t)||isTermLeaf(u))
		return isTermEqual(t,u);
	if(t->type!=u->type) return 0;
	if(isTermTuple(t))
	{
		return asField(TermOp1(t), u) && asField(TermOp2(t),u) &&
			   asField(TermOp1(u),t) && asField(TermOp2(u),t);
	}
	return isEqual(TermOp(t), TermOp(u)) &&
			   isEqual(TermKey(t), TermKey(u));
}
int
trivialEquationlist (Eqlist eql)
{
  Eqlist tmp;
  for (tmp = eql; tmp != NULL; tmp = tmp->next)
    {
      if (tmp->eq->type != SYSTEM_TRIVIAL)
	return 0;
    }
  return 1;
}

int
tryAbstractProt1 (int (*safecheck) (Protocol), Term (*absfunc) (Term),
		  Protocol p)
{
  if (safecheck != NULL && !safecheck (p))
    return 0;
  int succeed = 0;
  Role roles = p->roles;
  //for each role in the protocol
  while (roles != NULL)
    {
      Roledef roledef = roles->roledef;
      //for each event in the role
      while (roledef != NULL)
	{
	  if (roledef->message != NULL)
	    {
		  //eprintf(" Message of role ");
		  //printTerm(roles->nameterm);
		  //eprintf("\n");
		  //printTerm(roledef->message);
		  //eprintf("\n");

	      if (roledef->absMess == NULL)
	    	  roledef->absMess = absfunc (roledef->message);
		  eprintf(" original message ");
		  printTerm(roledef->message);
		  eprintf("\n");
		  eprintf(" abstract message ");
		  printTerm(roledef->absMess);
		  eprintf("\n");

	      if (!isEqual (roledef->message, roledef->absMess))
	    	  succeed = 1;
	    }
		  //eprintf("-->");
		  //printTerm(roledef->absMess);
		  //eprintf("\n");
	  roledef = roledef->next;
	}
      roles = roles->next;
    }
  return succeed || isHelperProtocol (p);
}

int
tryAbstractProt (int (*safecheck) (Protocol), Term (*absfunc) (Term),
		 System sys)
{
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      if (!tryAbstractProt1 (safecheck, absfunc, p))
	return 0;
      p = p->next;
    }
  return 1;
}

void
resetAbsForProtocol (Protocol p)
{
  Role roles = p->roles;
  while (roles != NULL)
    {
      Roledef roledef = roles->roledef;
      //for each event in the role
      while (roledef != NULL)
	{
	  if (roledef->message != NULL)
	    roledef->absMess = NULL;
	  roledef = roledef->next;
	}
      roles = roles->next;
    }
}

void
resetAbs (System sys)
{
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      resetAbsForProtocol (p);
      p = p->next;
    }
}

void
transferData1 (System sys)
{
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      Role roles = p->roles;
      while (roles != NULL)
	 {
 	   Roledef roledef;
	  for(roledef = roles->roledef; roledef!=NULL; roledef=roledef->next)
		  if(roledef->message != NULL && roledef->absMess != NULL)
		  {
			  int isClaim = roledef->type == CLAIM;
			  roledef->message = roledef->absMess;
			  roledef->absMess = NULL;
			  if (isClaim&&roledef->claiminfo!=NULL)
				  roledef->claiminfo->parameter = roledef->message;
		  }
	  roles = roles->next;
	}
      p = p->next;
    }
}

void
transferData (System newsys, System sys)
{
  if(sys==NULL||newsys==NULL) return;
  Protocol p = sys->protocols;
  Protocol newp = newsys->protocols;
  while (p != NULL)
    {
      Role newroles = newp->roles;
      if(newroles==NULL) return;
      Role roles = p->roles;
      while (roles != NULL)
	{
	  Roledef roledef;
	  Roledef newrd;
	  //for each event in the role
	  for (roledef=roles->roledef, newrd= newroles->roledef; roledef != NULL &&newrd!=NULL; roledef=roledef->next, newrd = newrd->next)
	    {
		  if(roledef->message != NULL&&roledef->absMess != NULL)
		  {
			  int isClaim = roledef->type == CLAIM;
			  newrd->message = roledef->absMess;
			  newrd->absMess = NULL;
			  if (isClaim&&newrd->claiminfo!=NULL)
				  newrd->claiminfo->parameter = newrd->message;
		  }
	    }
	  roles = roles->next;
	  newroles = newroles->next;
	}
      p = p->next;
      newp = newp->next;
    }
}

void
handleNilEvent (System sys)
{
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      Role roles = p->roles;
      while (roles != NULL)
	{
	  Roledef roledef = roles->roledef;
	  Roledef prevEvent = NULL;
	  //for each event in the role
	  while (roledef != NULL && roledef->message != NULL)
	    {
	      if (isTermEqual (roledef->message, TERM_NIL)
		  && roledef->type != CLAIM)
		{
		  /*if(roledef->type==CLAIM){
		     roledef->claiminfo->parameter=roles->nameterm;
		     roledef->message=roles->nameterm;
		     }
		     else */
		  {
		    int isHead = prevEvent == NULL;
		    roledef = removeRoleEvent (prevEvent, roledef);
		    if (isHead)
		      roles->roledef = roledef;
		    if (roledef == NULL)
		      break;
		    else
		      continue;
		  }
		}
	      prevEvent = roledef;
	      roledef = roledef->next;
	    }
	  roles = roles->next;
	}
      p = p->next;
    }
}

System
performAbstraction1 (System sys)
{
  transferData1 (sys);
  handleNilEvent (sys);
  return sys;
}


System
performAbstraction (System abssys, System sys)
{
  transferData (abssys, sys);
  handleNilEvent (abssys);
  return abssys;
}
