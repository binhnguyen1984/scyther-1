/*
 * termfunc.h
 *
 *  Created on: May 14, 2014
 *      Author: nguyen
 */

#ifndef TERMFUNC_H_
#define TERMFUNC_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macro.h"
#include "../termlist.h"
#include "subtype.h"
#include "../symbol.h"
#include "../specialterm.h"
#include "eqlist.h"

int size_of_term (Term t);
int depth (Term t);
void printTermlist (Termlist tl);
int isPublicKeyConstructor (Term t);
int isPublicKey (Term t);
int isPrivateKey (Term t);
int isLTSharedKeyConstructor (Term t);
Term smallestTermInTuple (Term t);
Term smallestTermInPatVars (Termlist tl);
int termEqual (Term t, Term u);
int termInList (Termlist, const Term);
Term getTermInList (Termlist tl, const Term term);
int isLTKeyConstructor (Term t);
int isLTSharedKey (Term t);
int isLTKey (Term t);
int isSubterm (Term t, Term u);
int isTicketVariable (Term t);
int isSubtermInTermlist (Term t, Termlist tl);
Termlist addField2Termlist (Termlist tl, Term t);
int isTermlistSubterm (Term t, Termlist tl);
int isSomeTermlistSubterm (Term t, Termlist tl);
int conjuntNonEmpty (Termlist tl1, Termlist tl2);
int appearInAccessible (Term t, Term u);
int appearInTuple (Term t, Term u);
int hasTicketVariable (Term t);
Termlist termlistCheckandAppend (Termlist tl1, Termlist tl2);
Termlist termlistAddNewTerm (Termlist tl, Term t);
int isDeeper (Term t, Term u);
void termDestroy (Term t);
Termlist extractClearAVFromTerm (Termlist av, Term t);
Termlist split (Term t);
Termlist turn_tuple_to_termlist (Term t);
Term turn_termlist_to_tuple (Termlist tl);
//printing
void printTerm (Term t);

int patternMatching (Term pat, Term term);
Term substitutedTerm (Term t);
Term normalizeTerm (Term t);
void deleteTermtype (Term t);
Termlist termlistRemoveTerm (Termlist);
Termlist termlistFindAndRemove (Termlist tl, Term t);
int containTermInPlain (Term, Term);
int isAgentVariable (Term t);
int containDiffAgent (Term, Term);
int containDiffAgentFromTerm (Term t, Term u);
int containDiffAgentlist (Term t, Termlist agents);
Term containLTSharedKeyInPlain (Term);
int avInclusion(Term t, Term u);
int avOverlapInTermlist (Term t, Termlist tl);
int plaintextAppearInTermlist (Term t, Termlist tl);
int plaintextAppearInTermlistPat (Term t, Termlist tl);
int plaintextTermlistInTerm (Term t, Termlist tl);
int containNewAVInPlain (Term t, Termlist tl);
Termlist termlistNotInPlaintext (Term t, Termlist tl);
Termlist extractAV (Termlist vars, Term t, int agent_only);
Termlist extractVariables (Termlist vars, Term t);
#endif /* TERMFUNC_H_ */
