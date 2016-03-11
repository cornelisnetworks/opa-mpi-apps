
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

#ifndef _DECLARE_H
#define _DECLARE_H

#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <malloc.h>*/
#include <stddef.h>
#include "appl_errors.h"
#include "err_check.h"

/* THERE IS INCOSISTENT DEFINITION OF MPI_Type_Size arguments */
typedef int Type_Size;          /*correct MPI standard  */
/*typedef MPI_Aint Type_Size;*/ /*on SGI, HITACHI SR2201*/

#ifdef MPIIO
#include "settings_io.h"
#else
#include "settings.h"
#endif

#include "comm_info.h"

#define nDEBUG

#ifdef DEBUG

#define DIAGNOSTICS(text,c_info,buf,loclen,totlen,j_sample,pos) {if(j_sample == 0) show((text),(c_info),(buf),(loclen),(totlen),(j_sample),(pos));}

extern FILE* dbg_file;
extern char* dbgf_name;

#define DBG_I1(text,i1) {fprintf(unit,"%s %d\n",(text),(i1));fflush(unit);}
#define DBG_I2(text,i1,i2) {fprintf(unit,"%s %d %d\n",(text),(i1),(i2));fflush(unit);}
#define DBG_I3(text,i1,i2,i3) {fprintf(unit,"%s %d %d %d\n",(text),(i1),(i2),(i3));fflush(unit);}

#define DBGF_I1(text,i1) {fprintf(dbg_file,"%s %d\n",(text),(i1));fflush(dbg_file);}
#define DBGF_I2(text,i1,i2) {fprintf(dbg_file,"%s %d %d\n",(text),(i1),(i2));fflush(dbg_file);}
#define DBGF_I3(text,i1,i2,i3) {fprintf(dbg_file,"%s %d %d\n",(text),(i1),(i2),(i3));fflush(dbg_file);}

#define DBGF_IARR(text,N,ia)\
{int i; fprintf(dbg_file,"%s ",text);for(i=0; i<N; i++) {fprintf(dbg_file,"%d ",(ia)[i]);};fprintf(dbg_file,"\n");fflush(dbg_file);}

#else

#define DIAGNOSTICS(text,c_info,buf,loclen,totlen,j_sample,pos) 

#define DBG_I1(text,i1)
#define DBG_I2(text,i1,i2)
#define DBG_I3(text,i1,i2,i3)

#define DBGF_I1(text,i1) 
#define DBGF_I2(text,i1,i2) 
#define DBGF_I3(text,i1,i2,i3) 

#define DBGF_IARR(text,N,ia)

#endif

#ifdef CHECK
#define CHK_DIFF(text,\
             c_info, RECEIVED, buf_pos,\
             Locsize, Totalsize, unit_size,\
             mode, pos,\
             n_sample, j_sample,\
             source, diff )\
        chk_diff(\
             (text),\
             (c_info), (RECEIVED), (buf_pos), \
             (Locsize), (Totalsize), (unit_size), \
             (mode), (pos),\
             (n_sample), (j_sample),\
             (source), (diff) )\


#define CHK_STOP {if(err_flag) return;}
#define CHK_BRK  {if(err_flag) {Err_Hand(0,1); break;}}

extern long   compute_crc (char* buf, int size);
extern long   r_check,s_check;
extern double defect;
extern double *all_defect;


#else
#define CHK_DIFF(text,\
             c_info, RECEIVED, buf_pos,\
             Locsize, Totalsize, unit_size,\
             mode, pos,\
             n_sample, j_sample,\
             source, diff )
#define CHK_STOP
#define CHK_BRK
#endif

/*   Filename for results output; insert empty string for stdout */
#define OUTPUT_FILENAME  ""

#define SCALE 1000000

/*   WIDTH OF OUTPUT TABLE */
#define ow_format 13
#define out_fields 8

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#ifdef BUFFERS_INT
#define A_ABS(i) abs((i))
#endif

#ifdef BUFFERS_FLOAT
#define A_ABS(x) fabs((x))
#endif

static int asize = sizeof(assign_type);
static int ierr;

#define MAX_TIMINGS 2
extern double *all_times;

/* STRING FOR OUTPUT    */
extern char aux_string[out_fields*ow_format];

/* FORMAT FOR OUTPUT    */
extern char format    [out_fields*7];

/* I/O unit                  */
extern FILE* unit;

/* Error message headers*/

extern int err_flag;

extern void* AUX;
extern int AUX_LEN;

extern int    do_nonblocking;
extern double tCPU;    /* CPU time for overlap benchmarks */
extern double MFlops;


#define TOL 1.e-2

#endif

