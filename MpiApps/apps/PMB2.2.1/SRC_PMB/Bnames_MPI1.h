
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


#include "Benchmark.h"

/* NAMES OF BENCHMARKS (DEFAULT CASE)*/
char *DEFC[] = {
   "PingPong" 
  ,"PingPing" 
  ,"Sendrecv"
  ,"Exchange" 
  ,"Allreduce" 
  ,"Reduce" 
  ,"Reduce_scatter" 
  ,"Allgather" 
  ,"Allgatherv" 
  ,"Alltoall"
  ,"Bcast" 
  ,"Barrier" 
  ,NULL
};

