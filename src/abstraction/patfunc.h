/*
 * eqfunc.h
 *
 *  Created on: Jun 10, 2014
 *      Author: nguyen
 */

#ifndef PATFUNC_H_
#define PATFUNC_H_

#include "typebasedabs.h"
#include "../termlist.h"

Equation createTicketHomomorphicPatForHash (Term hashfunc);
Term createPatFromType (Term);
Term createTermFromPatList (Termlist);
Equation createHomoEquationFromType (Term);
Term makeHomomorphic (Term t);
Term makeHomomorphicTuple (Term t);
#endif /* PATFUNC_H_ */
