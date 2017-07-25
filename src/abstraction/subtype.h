#include <stdlib.h>
#include <stdio.h>
#include "../termlist.h"
#include "../specialterm.h"

int isSubtype (Term, Term);
Term subTypeInSubterm (Term sttype, Term type);
Term getTermType (Term);
Term upperCommonSubtype (Term, Term);
int isComparable (Term typ1, Term typ2);
int compareTypelist (Term type, Termlist termlist);
int compareTypeTermlist (Term type, Termlist termlist);
Term mostGeneralType (Term, Term);
