/*
 * attack_check.c

 *
 *  Created on: Mar 9, 2015
 *      Author: nguyen
 */
#include "attack_check.h"
extern System original;
extern Protocol INTRUDER;
int regular_runs;

Runinfo runs;
unsigned int *runmap;
List nodelist;
int maxruns;

void
initModelCheck (System sys)
{
  systemStart (sys);
  sys->traceKnow[0] = sys->know;	// store initial knowledge
  bindingInit (sys);
  arachneInit (sys);
  systemReset (sys);		// reset any globals
  systemRuns (sys);		// init runs data
}

/*
void
printRuns (const System sys)
{
  int i;
  for (i = 0; i < sys->maxruns; i++)
    {
      eprintf ("Run %d:", i);
      int j;
      for (j = 0; j < sys->runs[i].step; j++)
	{
	  Roledef rd = eventRoledef (sys, i, j);
	  roledefPrint (rd);
	}
      eprintf ("\n");
    }
}
*/

//copy a run (from abstract attack)
struct runinfo
copyRun (struct run r)
{
  struct runinfo result;
  result.protocol = r.protocol;
  result.protocol = r.protocol;
  result.role = r.role;
  Roledef rd = roledef_shift (r.start, r.step - 1);
  result.lastevent = rd->label;
  result.eventtype = rd->type;
  return result;
}

//find the index of the original event that corresponds to the abstract event index in an abstract run
int
findIndex (const System sys, int abst_run, int abst_e)
{
  Protocol p = sys->runs[abst_run].protocol;
  Protocol orgp = original->protocols;
  int orge = 0;
  while (orgp != NULL)
    {
      if (isTermEqual (orgp->nameterm, p->nameterm))
	break;
      orgp = orgp->next;
    }
  Role abstr = sys->runs[abst_run].role;
  Role orgr;
  for (orgr = orgp->roles; orgr != NULL; orgr = orgr->next)
    if (isTermEqual (orgr->nameterm, abstr->nameterm))
      {
	Roledef abst_rd = roledef_shift (sys->runs[abst_run].start, abst_e);
	Roledef org_rd;
	for (org_rd = orgr->roledef; org_rd != NULL; org_rd = org_rd->next)
	  {
	    if (org_rd->type == abst_rd->type
		&& isTermEqual (org_rd->label, abst_rd->label))
	      {
		break;
	      }
	    orge++;
	  }
      }
  return orge;
}

void
copyDependGraph (const System sys)
{
  int r1, r2, e1, e2;
  nodelist = NULL;
  for (r1 = 0; r1 < maxruns; r1++)
    for (r2 = 0; r2 != r1 && r2 < maxruns; r2++)
      {
	int l1 = sys->runs[runmap[r1]].step;
	int l2 = sys->runs[runmap[r2]].step;
	e1 = 0;
	e2 = 0;
	while (e1 < l1)
	  {
	    while (e2 < l2)
	      {
		if (isDependEvent (runmap[r1], e1, runmap[r2], e2))
		  {
		    Nodepair n = (Nodepair) malloc (sizeof (struct nodepair));
		    n->r1 = r1;
		    n->r2 = r2;
		    n->e1 = findIndex (sys, runmap[r1], e1);
		    n->e2 = findIndex (sys, runmap[r2], e2);
		    nodelist = list_add (nodelist, n);
		    break;
		  }
		e2++;
	      }
	    if (e2 == l2)
	      break;
	    e1++;
	  }
      }
}

//copy runs from an abstract attack
void
copyRuns (System sys)
{
  maxruns = 0;
  regular_runs = sys->num_regular_runs;

  runs = (Runinfo) realloc (runs, sizeof (struct runinfo) * (sys->maxruns));
  runmap =
    (unsigned int *) realloc (runmap, regular_runs * sizeof (unsigned int));

  int i;
  for (i = 0; i < sys->maxruns; i++)
    {
      if (sys->runs[i].protocol != INTRUDER)
	{
	  runs[maxruns] = copyRun (sys->runs[i]);
	  runmap[maxruns] = i;
	  maxruns++;
	}
    }
  copyDependGraph (sys);
}


void
addDependency ()
{
  while (nodelist != NULL)
    {
      Nodepair n = (Nodepair) nodelist->data;
      setDependEvent (n->r1, n->e1, n->r2, n->e2);
      List node = nodelist;
      nodelist = nodelist->next;
      free (n);
      free (node);
    }
}

//map abstract attack runs to the corresponding symbolic runs in the original model
void
mapRuns (Claimlist cl, int *newruns, int *newgoals)
{
  Protocol p;
  Role r;
  int tracelength = 0;
  original->num_intruder_runs = 0;
  original->attackid = 0;
  int i;
  for (i = 0; i < maxruns; i++)
    {
      Protocol abst_p = runs[i].protocol;
      for (p = original->protocols; p != NULL; p = p->next)
	if (isTermEqual (p->nameterm, abst_p->nameterm))
	  {
	    Role abst_r = runs[i].role;
	    for (r = p->roles; r != NULL; r = r->next)
	      if (isTermEqual (r->nameterm, abst_r->nameterm))
		{
		  roleInstance (original, p, r, NULL, NULL);
		  (*newruns)++;
		  //identify how many steps that should be executed in the original model
		  Roledef rd;
		  int step = 0;
		  for (rd = r->roledef;; rd = rd->next)
		    {
		      if (rd->type != runs[i].eventtype
			  || !isTermEqual (rd->label, runs[i].lastevent))
			step++;
		      else
			break;
		    }
		  *newgoals = add_recv_goals (i, 0, ++step);
		  tracelength += step;
		  break;
		}
	    break;
	  }
    }
  //eprintf("original runs:\n");
  //printRuns(original);
  //eprintf("\n");

  addDependency ();
  free (runmap);
  runmap = NULL;
  free (runs);
  runs = NULL;
  //change the switches
  switches.maxtracelength = tracelength;
  switches.runs = regular_runs;
}
