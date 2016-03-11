
/*****************************************************************************
 *                                                                           *
 *  PMB source code                                       ---/---            *
 *                                                          / /              *
 *  Copyright (c) Pallas GmbH 1998                         / / /             *
 *                                                        / / / /            *
 *                                                         / / /             *
 *  Author : Hans-Joachim Plum                              / / pallas       *
 *  Revision: 2.1                                         ---/---            *
 *  Date: 1998/09/03                                                         *
 *                                                                           *
 *****************************************************************************/




#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Warm_Up (this file)  */

void Warm_Up (
     struct comm_info* c_info, struct Bench* Bmark, int iter
     );


/******************************/
/*       END PROTOTYPES       */

/**********************************************************************/


/******* Implementation Warm_Up *******/

void Warm_Up  (struct comm_info* c_info,
               struct Bench* Bmark,
               int iter)
/**********************************************************************

----------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
----------------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             Bmark    | struct Bench*     | Benchmark to run
             iter     | int               | number of iteration in pmb
Output     :          |                   |
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
----------------------------------------------------------------------
----------------------------------------------------------------------
Description: "Warm up" of benchmark: 
             Benchmark is run N_WARMUP times (see settinhs.h ) 
             with maximum message length
             N_BARR (see settings.h) Barriers follow
--------------------------------------------------------------------*/
{
  struct cmode MD;

  MD.AGGREGATE = 1;

  if( c_info->rank >= 0 )
    {
#ifndef MPIIO
      if( iter == 0 )
	{
	  int size = 1<<MAXMSGLOG;
	  double t;
	  int n_sample = N_WARMUP;

	  Bmark->Benchmark(c_info, size, n_sample, &MD, &t);
	}
#endif
    }
}
