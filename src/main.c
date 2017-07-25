/*
 * Scyther : An automatic verifier for security protocols.
 * Copyright (C) 2007-2013 Cas Cremers
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 *@file main.c
 * \brief The main file.
 *
 * Contains the main switch handling, and passes everything to the core logic.
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 *
 * Scyther is a model checker for security protocols.
 *
 * \section install Installation
 *
 * How to install Scyther.
 *
 * \section exit Exit codes
 *
 * 0  Okay	No attack found, claims encountered
 *
 * 1  Error	Something went wrong (error) E.g. switch error, or scenario ran out.
 *
 * 2  Okay	No attack found (because) no claims encountered
 *
 * 3  Okay	Attack found
 *
 * \section coding Coding conventions
 *
 * Usually, each source file except main.c has an myfileInit() and myfileDone() function
 * available. These allow any initialization and destruction of required structures.
 *
 * GNU indent rules are used, but K&R derivatives are allowed as well. Conversion can
 * be done for any style using the GNU indent program.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "system.h"
#include "debug.h"
#include "symbol.h"
#include "pheading.h"
#include "symbol.h"
#include "tac.h"
#include "timer.h"
#include "compiler.h"
#include "binding.h"
#include "switches.h"
#include "specialterm.h"
#include "color.h"
#include "error.h"
#include "claim.h"
#include "arachne.h"
#include "xmlout.h"
#include "attack_check.h"
#include "abstraction/abssys.h"
//! The global system state pointer
System original;

//! Pointer to the tac node container
extern struct tacnode *spdltac;
//! Match mode

void scanner_cleanup (void);
void strings_cleanup (void);
int yyparse (void);

int modelCheck (const System sys);
void MC_single (const System sys);

//! The main body, as called by the environment.
int
main (int argc, char **argv)
{
  int exitcode = EXIT_NOATTACK;

  /* initialize symbols */
  termsInit ();
  termmapsInit ();
  termlistsInit ();
  knowledgeInit ();
  symbolsInit ();
  tacInit ();

  /*
   * ------------------------------------------------
   *     generate system
   * ------------------------------------------------
   */

  /* process any command-line switches */
  switchesInit (argc, argv);

  /* process colors */
  colorInit ();

  /* start system */
  original = systemInit ();
  //sys =systemInit(sys);
  /* init knowledge. Needs to go before compiler init for special term init */
  original->know = emptyKnowledge ();
  /* init compiler for this system */
  compilerInit (original);

  /* parse input */

  yyparse ();
#ifdef DEBUG
  if (DEBUGL (1))
    tacPrint (spdltac);
#endif

  /* compile */
  // Compile no runs for Arachne and preprocess
  compile (spdltac, 0);
  scanner_cleanup ();
#ifdef DEBUG
  if (DEBUGL (1))
    {
      printf ("\nCompilation yields:\n\n");
      printf ("untrusted agents: ");
      termlistPrint (original->untrusted);
      printf ("\n");
      knowledgePrint (original->know);
      locVarPrint (original->locals);
      protocolsPrint (original->protocols);

      printf ("\nInstantiated runs:\n\n");
      runsPrint (original);
    }
#endif

  /* allocate memory for traces, based on runs */
  //systemStart (sys);
  //sys->traceKnow[0] = sys->know;      // store initial knowledge

  /* add parameters to system */


  /*
   * ---------------------------------------
   *  Switches consistency checking.
   * ---------------------------------------
   */

#ifdef DEBUG
  if (DEBUGL (4))
    {
      warning ("Selected output method is %i", switches.output);
    }
#endif
  runVerification (MC_single);
  //System abssys = getAbstractSystem();
  //eprintf("abstracted protocol:\n");
  //protocolsPrint(abssys->protocols);
  //eprintf("\nEquation list=");
  //printEquationlist(eql);
  //eprintf("\n");
  //MC_single (abssys);
  //MC_single(sys);
  /*
   * ---------------------------------------
   *  Start real stuff
   * ---------------------------------------
   */

  /* xml init */
  if (switches.xml)
    xmlOutInit ();

  /* model check system */
#ifdef DEBUG
  if (DEBUGL (1))
    warning ("Start modelchecking system.");
#endif

  /*
   * ---------------------------------------
   *  After checking the system, results
   * ---------------------------------------
   */

  /* Exitcodes are *not* correct anymore */

  exitcode = EXIT_ATTACK;

  /* xml closeup */
  if (switches.xml)
    xmlOutDone ();

  /*
   * Now we clean up any memory that was allocated.
   */

  colorDone ();
  switchesDone ();
  compilerDone ();

  /* done symbols */
  tacDone ();
  symbolsDone ();
  knowledgeDone ();
  termlistsDone ();
  termmapsDone ();
  termsDone ();

  /* memory clean up? */
  strings_cleanup ();
  if (switches.exitCodes)
    {
      return exitcode;
    }
  else
    {
      return 0;
    }
}

//! Analyse the model
/**
 * Traditional handywork.
 */


void
MC_single (const System sys)
{
  /*
   * simple one-time check
   */
  initModelCheck (sys);
  arachnePrepare ();
/*
	bindingInit (sys);
  arachneInit (sys);
  arachnePrepare();
  systemReset (sys);		// reset any globals
  systemRuns (sys);		// init runs data
  */
  modelCheck (sys);

  //arachneDone ();
  //knowledgeDestroy (sys->know); //close this because atomic terms are shared among abstract models
  //knowledgeDelete(sys->know);//only shallow deletion is done because abstract systems share the basic knowledge
  //protocolDelete(sys->protocols);
  //systemDone (sys);
}


//! Model check the system, given all parameters.
/*
 * Precondition: the system was reset with the corresponding parameters.
 * Reports time and states traversed.
 * Note that the return values doubles as the number of failed claims.
 *@return True iff any claim failed, and thus an attack was found.
 */

int
modelCheck (const System sys)
{
  int claimcount;

  /* modelcheck the system */
  claimcount = arachne ();

  if (claimcount == 0)
    {
      warning ("No claims in system.");
    }

  return (sys->failed != STATES0);
}



