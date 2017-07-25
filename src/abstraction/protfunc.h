/*
 * protfunc.h
 *
 *  Created on: May 23, 2014
 *      Author: nguyen
 */

#ifndef PROTFUNC_H_
#define PROTFUNC_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../termlist.h"
#include "../symbol.h"
#include "typebasedabs.h"
#include "advfunc.h"

Roledef findMatchingRun (List running, Roledef comm);
Roledef findSendRecvEvent (Role roles, Roledef roledef);
void addAgentVariables (Knowledge know, Termlist vars);
void recoverIK (Knowledge know, Termlist basicHead, Termlist varsHead,
		Termlist encHead);
Roledef findNextNonClaim (Roledef rd);
List getSecrecyProp (Protocol p, List secret);
Eqlist getEqualities (Protocol p, Eqlist eql);
void getPropertyEvents (Protocol p, List * evPhi, List * evPhiPlus);
int isSecretClaim (Claimlist cl);
int isCommitEvent (Claimlist cl);

int isRunningEvent (Claimlist cl);

int isAuthStrongClaim (Claimlist cl);
int isRunnOrCommEvent (Claimlist cl);
Roledef removeRoleEvent (Roledef prevEvent, Roledef removeEvent);
System systemDuplicate (System sys);
void protocolDelete (Protocol p);
#endif /* PROTFUNC_H_ */
