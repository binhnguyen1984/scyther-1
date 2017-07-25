/*
 * protfunc.c
 *
 *  Created on: May 23, 2014
 *      Author: nguyen
 */
#include "protfunc.h"

//for protocols, roles,...

void
addAgentVariables (Knowledge know, Termlist vars)
{
  Termlist tmp = vars;
  while (tmp != NULL)
    {
      if (inTermlist (tmp->term->stype, TERM_Agent))
	addTermToIK (know, tmp->term);
      tmp = tmp->next;
    }
}

void
termlistPrevDelete (Termlist tl)
{
  if (tl == NULL)
    return;
  termlistPrevDelete (tl->prev);
  free (tl);
  tl = NULL;
}

void
recoverIK (Knowledge know, Termlist basicHead, Termlist varsHead,
	   Termlist encHead)
{
  if (basicHead != NULL)
    {
      termlistPrevDelete (basicHead->prev);
      basicHead->prev = NULL;
    }
  if (varsHead != NULL)
    {
      termlistPrevDelete (varsHead->prev);
      varsHead->prev = NULL;
    }
  if (encHead != NULL)
    {
      termlistPrevDelete (encHead->prev);
      encHead->prev = NULL;
    }
  know->basic = basicHead;
  know->vars = varsHead;
  know->encrypt = encHead;
}


//role, events,...
Roledef
findMatchingRun (List running, Roledef comm)
{
  Term toRole1 = ((Termlist) turn_tuple_to_termlist (comm->message))->term;
  while (running != NULL)
    {
      Roledef rd = (Roledef) running->data;
      Term toRole2 = ((Termlist) turn_tuple_to_termlist (rd->message))->term;
      if (isTermEqual (rd->from, toRole1)
	  && isTermEqual (comm->from, toRole2))
	{
	  return rd;
	}
      running = running->next;
    }
  return NULL;
}

Roledef
findNextNonClaim (Roledef rd)
{
  Roledef tmp = rd->next;
  while (tmp != NULL)
    {
      if (tmp->type != CLAIM)
	return tmp;
      tmp = tmp->next;
    }
  return tmp;
}

//find a corresponding send/receive event to the one with given name, event type and label
Roledef
findSendRecvEvent (Role roles, Roledef roledef)
{
  Term rolename = roledef->type == SEND ? roledef->to : roledef->from;
  Term label = roledef->label;
  int type = roledef->type == SEND ? RECV : SEND;
  Role r = roles;
  while (r != NULL && !isTermEqual (r->nameterm, rolename))
    r = r->next;
  if (r != NULL)
    {
      Roledef rd = r->roledef;
      while (rd != NULL)
	{
	  if (rd->type == type && isTermEqual (rd->label, label))
	    return rd;
	  rd = rd->next;
	}
    }
  return NULL;
}

//checking
int
isSecretClaim (Claimlist cl)
{
  return cl->type == CLAIM_Secret || cl->type == CLAIM_SKR;
}

int
isCommitEvent (Claimlist cl)
{
  return cl->type == CLAIM_Commit;
}

int
isRunningEvent (Claimlist cl)
{
  return cl->type == CLAIM_Running;
}

int
isAuthStrongClaim (Claimlist cl)
{
  return cl->type == CLAIM_Nisynch || cl->type == CLAIM_Niagree;
}

int
isRunnOrCommEvent (Claimlist cl)
{
  return cl->type == CLAIM_Running || cl->type == CLAIM_Commit;
}

//adding
Claimlist
addClaim2list (Claimlist bigclaim, Claimlist cl)
{
  if (bigclaim == NULL)
    {
      return cl;
    }
  else
    {
      Claimlist cl2 = bigclaim;
      while (cl2->next != NULL)
	cl2 = cl2->next;
      cl2->next = cl;
      return bigclaim;
    }
}


//properties

//generate an equation from required communication, e.g., corresponding send and receive events
Equation
genEquationFromComm (Role roles, Roledef rd)
{
  Roledef rd1 = findSendRecvEvent (roles, rd);
  if (rd1 != NULL)
    {
      Equation eq = makeEquation ();
      eq->left = rd->message;
      eq->right = rd1->message;
      return eq;
    }
  return NULL;
}

List
getSecrecyProp (Protocol p, List secret)
{
  if (isHelperProtocol (p))
    return secret;
  Role r = p->roles;
  while (r != NULL)
    {
      Roledef rd = r->roledef;
      while (rd != NULL)
	{
	  if (rd->type == CLAIM && isSecretClaim (rd->claiminfo))
	    {
	      secret = list_add (secret, rd);
	    }
	  rd = rd->next;
	}
      r = r->next;
    }
  return secret;
}

Eqlist
getEqualities (Protocol p, Eqlist eql)
{
  if (isHelperProtocol (p))
    return eql;
  List commit = NULL;
  List running = NULL;
  Role r = p->roles;
  while (r != NULL)
    {
      Roledef rd = r->roledef;
      Roledef prev = NULL;
      Roledef start = NULL;
      while (rd != NULL)
	{
	  if (rd->type == CLAIM)
	    {
	      if (prev != NULL)
		{
		  if (isRunningEvent (rd->claiminfo))
		    {
		      running = list_add (running, rd);
		    }
		  else if (isCommitEvent (rd->claiminfo))
		    {
		      commit = list_add (commit, rd);
		    }
		  else if (isAuthStrongClaim (rd->claiminfo) && start != NULL)
		    {
		      //add all preceding events and the corresponding send/receive ones
		      Roledef tmp = start;
		      while (tmp != rd)
			{
			  if (tmp->type != CLAIM)
			    {
			      eql =
				equationlistAdd (eql,
						 genEquationFromComm
						 (p->roles, tmp));
			    }
			  tmp = tmp->next;
			}
		      start = NULL;
		    }
		}
	    }
	  else
	    {
	      prev = rd;
	      if (start == NULL)
		start = rd;
	    }
	  rd = rd->next;
	}
      r = r->next;
    }
  //get equations generated from running and commit signals
  while (commit != NULL)
    {
      Roledef comm = (Roledef) commit->data;
      Roledef runn = findMatchingRun (running, comm);
      if (runn != NULL)
	{
	  Equation eq = makeEquation ();
	  eq->left = runn->message;
	  eq->right = comm->message;
	  eql = equationlistAdd (eql, eq);
	}
      commit = commit->next;
    }
  return eql;
}

void
getPropertyEvents (Protocol p, List * evPhi, List * evPhiPlus)
{
  if (isHelperProtocol (p))
    return;
  Role r = p->roles;
  while (r != NULL)
    {
      Roledef rd = r->roledef;
      Roledef prev = NULL;
      Roledef start = NULL;
      while (rd != NULL)
	{
	  if (rd->type == CLAIM)
	    {
	      if (prev != NULL)
		{
		  /* for secrecy, no event is needed
		   * for non-injective agreement, one can use signals
		   if(isRunningEvent(rd->claiminfo))
		   {
		   Roledef nonClaim = findNextNonClaim(rd);
		   if(nonClaim!=NULL)
		   {
		   *evPhiPlus = list_add(*evPhiPlus,nonClaim);
		   *evPhi=list_add(*evPhi,nonClaim);
		   }
		   }
		   else if(isCommitEvent(rd->claiminfo)||isSecretClaim(rd->claiminfo))
		   {
		   if(!in_list(*evPhi,prev))
		   *evPhi=list_add(*evPhi,prev);
		   }
		   */
		  if (isAuthStrongClaim (rd->claiminfo) && start != NULL)
		    {
		      //add all preceding events and the corresponding send/receive ones
		      Roledef tmp = start;
		      while (tmp != rd)
			{
			  if (tmp->type != CLAIM)
			    {
			      if (!in_list (*evPhi, tmp))
				*evPhi = list_add (*evPhi, tmp);
			      if (!in_list (*evPhiPlus, tmp))
				*evPhiPlus = list_add (*evPhiPlus, tmp);
			    }
			  tmp = tmp->next;
			}
		      start = NULL;
		    }
		}
	    }
	  else
	    {
	      prev = rd;
	      if (start == NULL)
		start = rd;
	    }
	  rd = rd->next;
	}
      r = r->next;
    }
}

//duplicate stuff

Claimlist
claimDuplicate (Claimlist cl)
{
  if (cl == NULL)
    return cl;
  Claimlist newcl = (Claimlist) malloc (sizeof (struct claimlist));
  memcpy (newcl, cl, sizeof (struct claimlist));
  newcl->parameter = termDuplicate (cl->parameter);
  newcl->next = NULL;
  return newcl;
}

Claimlist
claimlistDuplicate (Claimlist cl)
{
  if (cl == NULL)
    return cl;
  Claimlist newcl = (Claimlist) malloc (sizeof (struct claimlist));
  newcl = claimDuplicate (cl);
  newcl->next = claimlistDuplicate (cl->next);
  return newcl;
}

Roledef
eventDuplicate1 (Roledef roledef)
{
  Roledef newevent = (Roledef) malloc (sizeof (struct roledef));
  memcpy (newevent, roledef, sizeof (struct roledef));
  newevent->message = termDuplicate (roledef->message);
  //newevent->absMess=termDuplicate(roledef->absMess);
  if (newevent->type == CLAIM)
    newevent->claiminfo = claimDuplicate (roledef->claiminfo);
  newevent->next = NULL;
  return newevent;
}

Roledef
eventDuplicate (Roledef roledef)
{
  if (roledef == NULL)
    return roledef;
  Roledef newevent = (Roledef) malloc (sizeof (struct roledef));
  newevent = eventDuplicate1 (roledef);
  newevent->next = eventDuplicate (roledef->next);
  return newevent;
}

Role
roleDuplicate1 (Role role)
{
  Role newrole = (Role) malloc (sizeof (struct role));
  memcpy (newrole, role, sizeof (struct role));
  newrole->roledef = eventDuplicate (role->roledef);
  newrole->next = NULL;
  return newrole;
}

Role
roleDuplicate (Role role)
{
  if (role == NULL)
    return role;
  Role newrole = (Role) malloc (sizeof (struct role));
  newrole = roleDuplicate1 (role);
  newrole->next = roleDuplicate (role->next);
  return newrole;
}

Protocol
protocolDuplicate1 (Protocol prot)
{
  Protocol newprot = (Protocol) malloc (sizeof (struct protocol));
  memcpy (newprot, prot, sizeof (struct protocol));
  newprot->roles = roleDuplicate (prot->roles);
  newprot->next = NULL;
  return newprot;
}

Protocol
protocolDuplicate (Protocol prot)
{
  if (prot == NULL)
    return prot;
  Protocol newprot = (Protocol) malloc (sizeof (struct protocol));
  newprot = protocolDuplicate1 (prot);
  newprot->next = protocolDuplicate (prot->next);
  return newprot;
}

void
linkProtandClaim (System sys)
{
  Protocol p = sys->protocols;
  while (p != NULL)
    {
      Role role = p->roles;
      while (role != NULL)
	{
	  Roledef event = role->roledef;
	  while (event != NULL)
	    {
	      if (event->type == CLAIM)
		{
		  event->claiminfo->protocol = p;
		  event->claiminfo->role = role;
		  event->claiminfo->rolename = role->nameterm;
		  event->claiminfo->roledef = event;
		  event->claiminfo->parameter = event->message;
		  sys->claimlist =
		    addClaim2list (sys->claimlist, event->claiminfo);
		}
	      event = event->next;
	    }
	  role = role->next;
	}
      p = p->next;
    }
}

System
systemDuplicate (System sys)
{
  System newsys = (System) malloc (sizeof (struct system));
  memcpy (newsys, sys, sizeof (struct system));
  newsys->protocols = protocolDuplicate (sys->protocols);
  newsys->claimlist = NULL;
  newsys->know = knowledgeDuplicate (sys->know);
  //addition
  newsys->secrets = NULL;
  newsys->proofstate = NULL;
  newsys->bindings = NULL;
  newsys->runs = NULL;
  newsys->traceEvent = NULL;
  newsys->traceRun = NULL;
  newsys->traceKnow = NULL;
  newsys->traceNode = NULL;
  linkProtandClaim (newsys);
  return newsys;
}

//compare

/*

int isClaimEqual(Claimlist cl1, Claimlist cl2){
	if(cl1==NULL) return cl2==NULL;
	if(cl2==NULL) return false;
	Claimlist tmp1 = cl1; Claimlist tmp2 = cl2;
	while(tmp1!=NULL)
	{
		if(tmp2==NULL) return 0;
		if(!isTermEqual(tmp1->label, tmp2->label)||tmp1->count!=tmp2->count)
			return 0;
		tmp1=tmp1->next;
		tmp2=tmp2->next;
	}
	return 1;
}
int eventEqual(Roledef r1, Roledef r2){
	int type_chk =r1->type==r2->type;
	int bound_chk=r1->bound==r2->bound;
	int label_chk=	isTermEqual(r1->label,r2->label);
	int mess_chk =isTermEqual(r1->message,r2->message);
	int from_chk = isTermEqual(r1->from,r2->from);
	int to_chk=	isTermEqual(r1->to,r2->to);
	if(!type_chk){
		eprintf("Types are not equal\n");
		return false;
	}
	else if(!bound_chk){
		eprintf("Bounds are not equal\n");
		return false;
	}
	else if(!label_chk){
		eprintf("Labels are not equal\n");
		return false;
	}
	else if(!mess_chk){
		eprintf("Messages are not equal\n");
		return false;
	}
	else if(!from_chk){
		eprintf("from-terms are not equal\n");
		return false;
	}
	else if(!to_chk){
		eprintf("to-terms are not equal\n");
		return false;
	}
	return true;
}

int eventsEqual(Roledef r1, Roledef r2){
	while(r1!=NULL&&r2!=NULL){
		if(!eventEqual(r1,r2)) return false;
		r1=r1->next; r2=r2->next;
	}
	return (r1==NULL)&&(r2==NULL);
}
*/
//remove stuff

void
freeRoledef (Roledef rd)
{
  if (rd == NULL)
    return;
  termDelete (rd->message);
  free (rd);
}

//remove an event from a roledef
Roledef
removeRoleEvent (Roledef prevEvent, Roledef removeEvent)
{
  //we need to update event index for claims declared after the removeEvent
  Roledef ev = removeEvent->next;
  while (ev != NULL)
    {
      if (ev->type == CLAIM)
	ev->claiminfo->ev--;
      ev = ev->next;
    }
  if (prevEvent == NULL)
    {
      Roledef newrole = removeEvent->next;
      freeRoledef (removeEvent);
      return newrole;
    }
  prevEvent->next = removeEvent->next;
  freeRoledef (removeEvent);
  return prevEvent->next;
}

//deletion

void
eventDelete (Roledef rd)
{
  if (rd == NULL)
    return;
  eventDelete (rd->next);
  //termDelete (rd->message);
  free (rd);
}

void
roleDelete (Role r)
{
  if (r == NULL)
    return;
  roleDelete (r->next);
  eventDelete (r->roledef);
  free (r);
}

void
protocolDelete (Protocol p)
{
  if (p == NULL)
    return;
  protocolDelete (p->next);
  roleDelete (p->roles);
  free (p);
}
