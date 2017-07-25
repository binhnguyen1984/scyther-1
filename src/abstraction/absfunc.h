/*
 * abstraction.h
 *
 *  Created on: May 5, 2014
 *      Author: nguyen
 */

#ifndef ABSTRACTION_H_
#define ABSTRACTION_H_
#include "../system.h"
#include "../termlist.h"
#include "typebasedabs.h"
#include "termfunc.h"
#endif /* ABSTRACTION_H_ */
int trivialEquationlist (Eqlist eql);
int tryAbstractProt1 (int (*safecheck) (Protocol), Term (*absfunc) (Term),
		      Protocol p);
int tryAbstractProt (int (*safecheck) (Protocol), Term (*absfunc) (Term),
		     System sys);
void resetAbsForProtocol (Protocol p);
void resetAbs (System sys);
System performAbstraction1 (System sys);
System performAbstraction (System abssys, System sys);
