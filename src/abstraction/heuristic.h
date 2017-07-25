/*
 * heuristic.h
 *
 *  Created on: May 26, 2014
 *      Author: nguyen
 */

#ifndef HEURISTIC_H_
#define HEURISTIC_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "../term.h"
#include "../termlist.h"
#include "../system.h"
#include "subtype.h"
#include "eqlist.h"
#include "typebasedabs.h"
#include "redabs.h"
#include "avrem.h"
#include "patfunc.h"
#include "advfunc.h"
#include "../bool.h"
#endif /* HEURISTIC_H_ */

struct cryptstr
{
  int type;
  Term info;			// it is the key (for MAC, encryptions) and NULL otherwise
};

struct cryptolabel
{
  int auth_crypt;
  int sec_crypt;
};
void heuristicInit (System mysys);
System abstractSystem ();
int abstractionSucceed ();
void heuristicDone ();
