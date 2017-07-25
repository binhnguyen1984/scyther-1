/*
 * equation.h
 *
 *  Created on: Jun 27, 2014
 *      Author: nguyen
 */
#include <stdlib.h>
#include <stdio.h>
#include "termfunc.h"

#ifndef EQLIST_H_
#define EQLIST_H_

enum
{
  USER_DEFINED_LINEAR,
  USER_DEFINED_PERSISTENT,
  SYSTEM_TRIVIAL,
  SYSTEM_PULL_OUT,
  SYSTEM_REMOVE_FIELD
//      SYSTEM_REORDER
} eqtype;

struct equation
{
  int line;
  Term left;
  Term right;
  int type;			//1(2): homomorphical equations(non-trivial equation) created not by the system and 0 otherwise
} equation;

typedef struct equation *Equation;

struct eqlist
{
  Equation eq;
  struct eqlist *next;
} eqlist;
typedef struct eqlist *Eqlist;
int abstCost (Equation eq);

Equation findMatchingPattern (Term termtype, Eqlist eql);
void printEquationlist (Eqlist);
void printEquation (Term left, Term right);
Equation makeEquation ();
Eqlist makeEquationlist ();
Eqlist equationlistAdd ();
Eqlist removeEquationFromList (Eqlist, Equation);
Eqlist removeEquationFromListShallow (Eqlist l, Equation eq);
void deleteEquation (Equation);
void deleteEqlist (Eqlist);
void deleteEqlistShallow (Eqlist);
Eqlist deleteNonPersistentEq (Eqlist eqlist);
#endif /* EQUATION_H_ */
