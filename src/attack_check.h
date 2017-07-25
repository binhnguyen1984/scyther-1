/*
 * attack_check.h
 *
 *  Created on: Mar 10, 2015
 *      Author: nguyen
 */

#ifndef ATTACK_CHECK_H_
#define ATTACK_CHECK_H_
#include "system.h"
#include "role.h"
#include "system.h"
#include "arachne.h"
#include "error.h"
#include "binding.h"
#include "switches.h"
#include "depend.h"
#include "abstraction/termfunc.h"
#include <stdlib.h>
void initModelCheck (System);

struct runinfo
{
  Protocol protocol;		//!< Protocol of this run.
  Role role;			//!< Role of this run.
  Term lastevent;		//!< Current execution point in the run (the label of the last event)
  int eventtype;		//type of the last event
};
typedef struct runinfo *Runinfo;

struct nodepair
{
  int r1, e1, r2, e2;
};

typedef struct nodepair *Nodepair;

void copyRuns (System);
void mapRuns (Claimlist, int *, int *);
#endif /* ATTACK_CHECK_H_ */
