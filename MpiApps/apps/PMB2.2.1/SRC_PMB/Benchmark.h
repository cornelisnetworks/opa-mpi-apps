
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


#ifndef __Bmark_h__
#define __Bmark_h__



/* Classification of benchmarks */

typedef enum { BTYPE_INVALID=-1,
               SingleTransfer,
               ParallelTransfer,
               Collective,
               Sync
             } BTYPES;


typedef struct cmode
          {
          int AGGREGATE ;   /* -1/0/1 for default/NON AGGREGATE/AGGREGATE */
          int NONBLOCKING  ;   /* 0/1 for no/yes */
          BTYPES type;      
          }
        *MODES;

#define X_MODES 2

typedef enum { put, get, no } DIRECTION;

#ifdef MPIIO

typedef enum { nothing=-1,private, explicit, indv_block, indv_cyclic, shared }
               POSITIONING;
#else

typedef int POSITIONING;

#endif

/* Descriptor for benchmarks */

struct Bench
  {
  char*  name;
  char** bench_comments;       /* Comments from header => Comments.h */

  DIRECTION access;

  int reduction;               /* reduction-type y/n */

  int N_Modes;
  struct cmode RUN_MODES[X_MODES];

  void (*Benchmark)(struct comm_info* c_info,int size,
                    int n_sample,MODES RUN_MODE,double* time);
                               /* Pointer to function runnning the benchmark */

  double scale_time, scale_bw; /* Scaling of timings and bandwidth */
  int Ntimes;

  int success;

#ifdef MPIIO
  POSITIONING fpointer;
#endif
  };

#endif
