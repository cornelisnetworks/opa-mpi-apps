
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




#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "settings.h"
#include "comm_info.h"
#include "err_check.h"


#ifdef CHECK

/*  PROTOTYPES (this file)    */
/******************************/


/******************************/
/*       END PROTOTYPES       */

long   r_check,s_check;
double defect;
double *all_defect;

#endif

FILE* dbg_file;
char* dbgf_name;

double *all_times;

/*  STRINGS FOR OUTPUT   */
char aux_string[out_fields*ow_format];

/* FORMAT FOR OUTPUT    */
char format [out_fields*7];

/* ARRAY OF CASES, EITHER DEFAULT OR ARGUMENT LIST */
char **cases ;           

/* Error status  */

int err_flag;

/* I/O unit */
FILE* unit;

/* MAXIMAL MESSAGE LENGTH    */
int MAXMSG;

void * AUX;
int AUX_LEN=0;

int    do_nonblocking=0;
double tCPU = 0.;    /* CPU time for overlap benchmarks */
double MFlops = -1.;
