
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
 *  Revision: 2.2.1                                                          *
 *           PMB_OPTIONAL branch deactivated                                 *
 *                                                                           *
 *****************************************************************************/



#ifndef _SETTINGS_H
#define _SETTINGS_H

/*
PMB settings definitions

There are two scenarios to run PMB

PMB_STANDARD:
"Mandatory" settings

PMB_OPTIONAL:
The user can change the settings, in order to extend the table of benchmark
results. Changeable are only a few crucial parameters for PMB.

Overall, at most 8 preprocessor varibales can bet set in order to control
PMB. These are

- PMB_OPTIONAL (has to be set when user optional settings are to be activated)

- MINMSGLOG    (second smallest data transfer size is 2^MINMSGLOG (the smallest
                always being 0))

- MAXMSGLOG    (largest data size is 2^MAXMSGLOG)

- MSGSPERSAMPLE (max. repetition count)
- MSGS_NONAGGR  (       "           for non aggregate benchmarks)

- OVERALL_VOL   (for all sizes < OVERALL_VOL, the repetition count is eventually
                reduced so that not more than OVERALL_VOL bytes overall are 
                processed.
                This avoids unnecessary repetitions for large message sizes.

                Finally, the real repetition count for message size X is

                MSGSPERSAMPLE (X=0),

                min(MSGPERSAMPLE,max(1,OVERALL_VOL/X))    (X>0)

                NOTE: OVERALL_VOL does NOT restrict the size of the max. 
                data transfer. 2^MAXMSGLOG is the largest size, independent
                of OVERALL_VOL.
               )


- N_BARR        Number of MPI_Barrier for synchronization

- TARGET_CPU_SECS
                CPU seconds (as float) to run concurrent with nonblocking
                benchmarks

In any case the user has to select the variable
FILENAME
which specifies the name of input/output file ("recycled" for all benchmarks;
if one separate file is opened on each process, an index _<rank> will be
appended to the filename)

*/ 

#define FILENAME "pmb_out"

#ifdef PMB_OPTIONAL
#else

/*
DON'T change anything below here !!
*/


#define MINMSGLOG 0
#define MAXMSGLOG 24

#define MSGSPERSAMPLE 50
#define MSGS_NONAGGR  10
#define OVERALL_VOL 16*1048576


#define TARGET_CPU_SECS 0.1 /* unit seconds */

#define N_BARR   2


#endif

#define BUFFERS_INT

/* How to set communications buffers for process rank, index i */
#ifdef BUFFERS_INT

typedef int assign_type ;
#define BUF_VALUE(rank,i)  10000000*(1+rank)+i%10000000

#endif

#ifdef BUFFERS_FLOAT

typedef float assign_type ;
#define BUF_VALUE(rank,i)  (0.1*((rank)+1)+(float)((i)))

#endif

#endif
