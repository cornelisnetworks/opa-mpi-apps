
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


/* Implementation of all PMB-IO Write - Benchmarks
   (see the accompanying document)

   Calling sequence for all:

   void Write_XXXX(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);
*/

/*------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             size     | int               | message length in byte
	     n_sample | int               | repetition count
             RUN_MODE | MODES (typedef,   | Distinction aggregate/
                      | see Benchmark.h)  | non aggr. and 
                      |                   | blocking / nonblocking modes,
                      |                   | see docu.
                      |                   |
Output     : time     | double*           | *time: time/sample in usec
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
------------------------------------------------------------
*/


#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Write_Shared (this file)  */

void Write_Shared(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Write_Indv (this file)  */

void Write_Indv(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Write_Expl (this file)  */

void Write_Expl(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Write_ij (this file)  */

void Write_ij(
     struct comm_info* c_info, int size, 
     POSITIONING pos, BTYPES type,
     int i_sample, int j_sample, int time_inner, 
     double* time);

/* Prototype IWrite_ij (this file)  */

void IWrite_ij(
     struct comm_info* c_info, int size, 
     POSITIONING pos, BTYPES type,
     int i_sample, int j_sample, int time_inner, int do_ovrlp,
     double* time);


/*** PROTOTYPES (extern)   ***/

/* Prototype CPU_Exploit, File: CPU_Exploit.c */

void CPU_Exploit(float target_secs, int initialize);


/* Prototype Open_File (file Init_File.c)  */

int Open_File(struct comm_info* c_info);

/* Prototype v_alloc (file Mem_Manager.c)  */

void* v_alloc(
      int Len, char* where
     );

/* Prototype chk_diff, File: chk_diff.c */

void chk_diff(
     char* text,
     struct comm_info* c_info, void* RECEIVED, int buf_pos, 
     int Locsize, int Totalsize, int unit_size, 
     DIRECTION mode, POSITIONING pos,
     int n_sample, int j_sample,
     int source, double* diff );

/* Prototype Err_Hand (file Err_Handler.c)  */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );


/******************************/
/*       END PROTOTYPES       */


/*************************************************************************/


/******* Implementation Write_Shared *******/

void Write_Shared(struct comm_info* c_info,int size,int n_sample,
                  MODES RUN_MODE,double* time)
/*------------------------------------------------------------
MPI_Write with shared file pointers

RUN_MODE determines "aggregate", "non blocking" and "collective" property.

In the non blocking case:
  First:  Call the corresponding blocking benchmark for comparison
  Then:   Call the non blocking version.

Further description: see the accompanying document
-------------------------------------------------------------*/
{
if( RUN_MODE->AGGREGATE )
Write_ij(c_info, size, shared, RUN_MODE->type, 1, n_sample, 1, time);
else
Write_ij(c_info, size, shared, RUN_MODE->type,  n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
Open_File(c_info);

if( RUN_MODE->AGGREGATE )
IWrite_ij(c_info, size, shared, RUN_MODE->type, 1, n_sample, 1, 1, time+1);
else
IWrite_ij(c_info, size, shared, RUN_MODE->type,  n_sample, 1, 0, 1, time+1);
}
}

/*************************************************************************/


/******* Implementation Write_Indv *******/

void Write_Indv(struct comm_info* c_info,int size,int n_sample,
                MODES RUN_MODE,double* time)
/*------------------------------------------------------------
MPI_Write with individual file pointers

RUN_MODE determines "aggregate", "non blocking" and "collective" property.

In the non blocking case:
  First:  Call the corresponding blocking benchmark for comparison
  Then:   Call the non blocking version.

Further description: see the accompanying document
-------------------------------------------------------------*/
{
if( RUN_MODE->AGGREGATE )
Write_ij(c_info, size, indv_block, RUN_MODE->type, 1, n_sample, 1, time);
else
Write_ij(c_info, size, indv_block, RUN_MODE->type,  n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
Open_File(c_info);

if( RUN_MODE->AGGREGATE )
IWrite_ij(c_info, size, indv_block, RUN_MODE->type, 1, n_sample, 1, 1, time+1);
else
IWrite_ij(c_info, size, indv_block, RUN_MODE->type,  n_sample, 1, 0, 1, time+1);
}

}

/*************************************************************************/


/******* Implementation Write_Expl *******/

void Write_Expl(struct comm_info* c_info,int size,int n_sample,
                MODES RUN_MODE,double* time)
/*------------------------------------------------------------
MPI_Write with explicit file pointers

RUN_MODE determines "aggregate", "non blocking" and "collective" property.

In the non blocking case:
  First:  Call the corresponding blocking benchmark for comparison
  Then:   Call the non blocking version.

Further description: see the accompanying document
-------------------------------------------------------------*/
{
if( RUN_MODE->AGGREGATE )
Write_ij(c_info, size, explicit, RUN_MODE->type, 1, n_sample, 1, time);
else
Write_ij(c_info, size, explicit, RUN_MODE->type,  n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
Open_File(c_info);

if( RUN_MODE->AGGREGATE )
IWrite_ij(c_info, size, explicit, RUN_MODE->type, 1, n_sample, 1, 1, time+1);
else
IWrite_ij(c_info, size, explicit, RUN_MODE->type,  n_sample, 1, 0, 1, time+1);
}
}

/******* Implementation Write_ij *******/

void Write_ij(struct comm_info* c_info,int size, 
              POSITIONING pos, BTYPES type,
              int i_sample, int j_sample, int time_inner,
              double* time)
{
/*
The proper output function, blocking cases
    pos:  file pointer type
    type: collective/non collective distinction
    i_sample/j_sample:
          1/n_sample in the aggregate, n_sample/1 in the non aggregate case
          elementary benchmarks performed inside a double loop
          for( i=0 .. i_sample-1 )
             {
             for( j=0 .. j_sample-1 )
                  Output ...
             Synchronize (!)
             }
          Thus, every j_sample sample a synchronization takes place
*/
int i, j;
int Locsize,Totalsize,Ioffs;
MPI_Status stat;
MPI_Offset Offset;

ierr = 0;

*time=0.;
if( c_info->File_rank >= 0 )
{

int (* GEN_File_write)(MPI_File fh, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);
int (* GEN_File_write_shared)
                      (MPI_File fh, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);
int (* GEN_File_write_at) 
                      (MPI_File fh, MPI_Offset offset, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);

#ifdef CHECK
int chk_mode;

if( pos == shared && type != Collective) chk_mode = -3;
else                                     chk_mode = -2;

defect = 0.;
#endif

if (type == Collective )
  {
    GEN_File_write = MPI_File_write_all;
    GEN_File_write_shared = MPI_File_write_ordered;
    GEN_File_write_at = MPI_File_write_at_all;
#ifdef DEBUG
fprintf(unit,"Collective output\n");
#endif
  }
else
  {
    GEN_File_write = MPI_File_write;
    GEN_File_write_shared = MPI_File_write_shared;
    GEN_File_write_at = MPI_File_write_at;
#ifdef DEBUG
fprintf(unit,"Non collective output\n");
#endif
  }

Locsize = c_info->split.Locsize;
Totalsize = c_info->split.Totalsize;
Offset = (MPI_Offset)c_info->split.Offset;

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
BLOCKING COLLECTIVE/NON COLLECTIVE OUTPUT CASES COMBINED
(function pointer GEN_File_write_XXX
either standard or collective MPI_File_write_XXX
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

if( !time_inner ) *time = MPI_Wtime();

for( i=0; i<i_sample; i++ )
{


if( time_inner ) *time = MPI_Wtime();

if( pos == indv_block )
for ( j=0; j<j_sample; j++ )
{

ierr=GEN_File_write(c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Write standard ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

else if ( pos == explicit )
for ( j=0; j<j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)((i+j)*Totalsize);

ierr=GEN_File_write_at
     (c_info->fh, Offset, c_info->s_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Write explicit ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

else if ( pos == shared )
for ( j=0; j<j_sample; j++ )
{

ierr=GEN_File_write_shared
     (c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Write shared ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

MPI_File_sync(c_info->fh);

if( time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

CHK_DIFF("Write_xxx",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         put, pos, j_sample, time_inner ? -1 : i,
         chk_mode, &defect);
CHK_STOP;

}

if( !time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

}  /* end if (File_comm ) */
}


/******* Implementation IWrite_ij (nonblocking cases) *******/

void IWrite_ij(struct comm_info* c_info,int size, 
               POSITIONING pos, BTYPES type,
               int i_sample, int j_sample, int time_inner, int do_ovrlp,
               double* time)
/*
The proper output function, non blocking cases
    pos:  file pointer type
    type: collective/non collective distinction
    i_sample/j_sample, non collective case:
          1/n_sample in the aggregate, n_sample/1 in the non aggregate case
          elementary benchmarks performed inside a double loop
          for( i=0 .. i_sample-1 )
             {
             for( j=0 .. j_sample-1 )
                  Non blocking Output ...

             Waitall
             Synchronize (!)
             }
          Thus, every j_sample samples a synchronization takes place

    i_sample/j_sample, collective case:
    i_sample*j_sample is the number of repetitions (since multiple requests
                                                    are not allowed)
 
*/
{
int i, j;
int Locsize,Totalsize,Ioffs;
MPI_Offset Offset;

MPI_Status*  STAT, stat;
MPI_Request* REQUESTS;

ierr = 0;

*time=0;

if( c_info->File_rank >= 0 )
{
#ifdef CHECK
int chk_mode;

if( pos == shared && type != Collective) chk_mode = -3;
else                                     chk_mode = -2;

defect = 0.;
#endif


Locsize = c_info->split.Locsize;
Totalsize = c_info->split.Totalsize;
Offset = (MPI_Offset)c_info->split.Offset;


if(type == Collective )

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON BLOCKING COLLECTIVE OUTPUT CASES
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

{
for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

*time = MPI_Wtime();

if( pos == indv_block )

for ( j=0; j<i_sample*j_sample; j++ )
{

ierr=MPI_File_write_all_begin
  (c_info->fh,c_info->s_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);
DIAGNOSTICS("IWrite coll. ",c_info,c_info->s_buffer,Locsize,Totalsize,j,pos);

if( do_ovrlp )
CPU_Exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_write_all_end
  (c_info->fh,c_info->s_buffer,&stat);
MPI_ERRHAND(ierr);

}

else if ( pos == explicit )

for ( j=0; j<i_sample*j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)(j*Totalsize);

ierr=MPI_File_write_at_all_begin
  (c_info->fh,Offset,c_info->s_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite expl coll. ",c_info,c_info->s_buffer,Locsize,Totalsize,j,pos);

if( do_ovrlp )
CPU_Exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_write_at_all_end
  (c_info->fh,c_info->s_buffer,&stat);
MPI_ERRHAND(ierr);


}

else if ( pos == shared )

for ( j=0; j<i_sample*j_sample; j++ )
{

ierr=MPI_File_write_ordered_begin
  (c_info->fh,c_info->s_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite shared coll. ",c_info,c_info->s_buffer,Locsize,Totalsize,j,pos);

if( do_ovrlp )
CPU_Exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_write_ordered_end
  (c_info->fh,c_info->s_buffer,&stat);
MPI_ERRHAND(ierr);

}

MPI_File_sync(c_info->fh);


*time = (MPI_Wtime() - *time)/(i_sample*j_sample);

CHK_DIFF("Coll. IWrite_xxx",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         put, pos, i_sample*j_sample, -1,
         chk_mode, &defect);
CHK_STOP;

}

else  /* type non Collective */

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON BLOCKING NON COLLECTIVE OUTPUT CASES
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

{

REQUESTS = (MPI_Request*)v_alloc(j_sample*sizeof(MPI_Request), "IWrite_ij");
STAT     = (MPI_Status *)v_alloc(j_sample*sizeof(MPI_Status ), "IWrite_ij");

for( j=0; j<j_sample; j++ ) REQUESTS[j]=MPI_REQUEST_NULL;

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

if( !time_inner ) *time = MPI_Wtime();

for( i=0; i<i_sample; i++ )
{


if( time_inner ) *time = MPI_Wtime();

if( pos == indv_block )
for ( j=0; j<j_sample; j++ )
{

ierr=MPI_File_iwrite(c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);
DIAGNOSTICS("IWrite standard ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

else if ( pos == explicit )
for ( j=0; j<j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)((i+j)*Totalsize);

ierr=MPI_File_iwrite_at
  (c_info->fh,Offset,c_info->s_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite expl ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);


}

else if ( pos == shared )
for ( j=0; j<j_sample; j++ )
{

ierr=MPI_File_iwrite_shared
  (c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite shared ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}


if( do_ovrlp )
for ( j=0; j<j_sample; j++ )
CPU_Exploit(TARGET_CPU_SECS,0);

if( j_sample == 1 )
MPI_Wait(REQUESTS,STAT);
else                 
MPI_Waitall(j_sample,REQUESTS,STAT);


MPI_File_sync(c_info->fh);


if( time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

CHK_DIFF("IWrite_xxx",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         put, pos, j_sample, time_inner ? -1 : i,
         chk_mode, &defect);
CHK_STOP;

}
if( !time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

free (REQUESTS);
free (STAT);    

}


}  /* end if (File_comm ) */
}
