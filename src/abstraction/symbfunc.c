/*
 * symbfunc.c
 *
 *  Created on: Jun 2, 2014
 *      Author: nguyen
 */
#include "symbfunc.h"
extern Symbol symbtab[HASHSIZE];
extern Symbol symb_alloc;

void removeSymb(Symbol s)
{
	  int hv;
	  Symbol t;
	  hv = hash (s->text);
	  t = symbtab[hv];
	  //remove the symbol from hash table
	  Symbol prev=NULL;
	  while (t != NULL)
	    {
	      if (t==s)
	      {
	    	  if(prev==NULL)
	    		  symbtab[hv]=t->next;
	    	  else prev->next=t->next;
	    	  break;
	      }
	      prev=t;

	    	  t = t->next;
	    }
	  //remove the symbol from symb_alloc
	  t= symb_alloc;
	  prev=NULL;
	  while(t!=NULL)
	  {
	      if (t==s)
	      {
	    	  if(prev==NULL)
	    		  symb_alloc=t->allocnext;
	    	  else prev->allocnext=t->allocnext;
	    	  break;
	      }
	      prev=t;
		  t=t->allocnext;
	  }
	  free(s);
}
