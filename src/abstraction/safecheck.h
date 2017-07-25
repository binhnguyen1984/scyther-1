/*
 * safecheck.h
 *
 *  Created on: Jun 4, 2014
 *      Author: nguyen
 */

#ifndef SAFECHECK_H_
#define SAFECHECK_H_
#include <stdlib.h>
#include <stdio.h>
#include "../bool.h"
#include "../list.h"
#include "../system.h"

int checkInjectivenessForEvent (Term (*absfunc) (Term), Protocol p,
				List evPhiPlus, int error,
				void (*handle) (Term, Term));
int checkNoNilEvent (Term (*absfunc) (Term), List evPhi, int error,
		     void (*handle) (Term));
#endif /* SAFECHECK_H_ */
