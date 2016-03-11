
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



#ifndef _COMM_INFO_H
#define _COMM_INFO_H 

#include "declare.h"


#ifdef MPIIO
typedef struct { int Locsize; MPI_Offset Offset; int Totalsize;} SPLITTING;
#endif

struct comm_info
{ 
/* Communication information as for MPI-1/2 parts */

int w_num_procs;          /* number of procs in COMM_WORLD            */
int w_rank;               /* rank of actual process in COMM_WORLD     */

int NP;                   /* #processes participating in benchmarks   */

MPI_Comm communicator;    /* underlying communicator for benchmark(s) */

int num_procs;            /* number of processes in communicator      */
int rank;                 /* rank of actual process in communicator   */

MPI_Datatype s_data_type; /* data type of sent data                   */
MPI_Datatype r_data_type; /* data type of received data               */

MPI_Datatype red_data_type;/* data type of reduced data               */
MPI_Op op_type;            /* operation type                          */

int pair0, pair1;          /* process pair                            */
int select_tag;            /* 0/1 for tag selection off/on            */
int select_source;         /* 0/1 for sender selection off/on         */

void* s_buffer;            /* send    buffer                          */
assign_type* s_data;       /* assign_type equivalent of s_buffer      */
int   s_alloc;             /* #bytes allocated in s_buffer            */
void* r_buffer;            /* receive buffer                          */
assign_type* r_data;       /* assign_type equivalent of r_buffer      */
int   r_alloc;             /* #bytes allocated in r_buffer            */

int group_mode;            /* Mode of running groups (<0,0,>0)        */
int n_groups;              /* No. of independent groups               */
int group_no;              /* own group index                         */
int* g_sizes;              /* array of group sizes                    */
int* g_ranks;              /* w_ranks constituting the groups         */

int* reccnt;               /* recv count argument for global ops.     */
int* displs;               /* displacement argument for global ops.   */

MPI_Errhandler ERR;
 
#ifdef MPIIO
/*   FILE INFORMATION     */
 
char* filename;
MPI_Comm File_comm;
int      File_num_procs;
int      all_io_procs;
int      File_rank;

MPI_File fh;
MPI_Datatype etype;
Type_Size e_size;
MPI_Datatype filetype;

SPLITTING split;
 
int amode;
MPI_Info info;
 
/* View: */
MPI_Offset disp;
char* datarep;
MPI_Datatype view;
MPI_Errhandler ERRF;
#endif

#ifdef EXT
MPI_Win WIN;
MPI_Info info;
MPI_Errhandler ERRW;
#endif
 

};

#endif
