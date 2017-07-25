/*
 * advfunc.c
 *
 *  Created on: Sep 18, 2014
 *      Author: nguyen
 */
#include "advfunc.h"

void
addTermToIK (Knowledge know, Term term)
{
  if (inKnowledge (know, term))
    return;
  if (isTermLeaf (term))
    {
      know->basic = termlistAdd (know->basic, term);
    }
  else if (term->type == ENCRYPT)
    {
      know->encrypt = termlistAdd (know->encrypt, term);
    }
  else
    {
      addTermToIK (know, TermOp1 (term));
      addTermToIK (know, TermOp2 (term));
    }
}
