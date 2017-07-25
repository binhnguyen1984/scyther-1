#ifndef TYPEBASEDABS_H_
#define TYPEBASEDABS_H_

#include <stdlib.h>
#include <stdio.h>
#include "../symbol.h"
#include "../termlist.h"
#include "subtype.h"
#include "../specialterm.h"
#include "../tac.h"
#include "../mgu.h"
#include "eqlist.h"
#include "safecheck.h"
#include "absfunc.h"
#include "termfunc.h"
#include "protfunc.h"
#include "patfunc.h"
void typebasedInit (System mysys);
int typebasedAbstraction ();
int checkPatternDisjointness ();
int checkWelldefinednessForEquation (Term left, Term right);
Equation resolveDisjointnessConflict (Term type);
Term frec (Term t);
#endif
