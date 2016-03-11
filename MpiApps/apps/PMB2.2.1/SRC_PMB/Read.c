
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



/* Implementation of all PMB-IO Read - Benchmarks
   (see the accompanying document)

   Calling sequence for all:

   void Read_XXXX(
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

/* Prototype Read_Indv (this file)  */

void Read_Indv(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Read_Shared (this file)  */

void Read_Shared(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Read_Expl (this file)  */

void Read_Expl(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Read_ij (this file)  */

void Read_ij(
     struct comm_info* c_info, int size, 
     POSITIONING pos, BTYPES type,
     int i_sample, int j_sample, int time_inner, 
     double* time);

/* Prototype IRead_ij (this file)  */

void IRead_ij(
     struct comm_info* c_info, int size, 
     POSITIONING pos, BTYPES type,
     int i_sample, int j_sample, int time_inner, int do_ovrlp,
     double* time);


/*** PROTOTYPES (extern)   ***/

/* Prototype Open_File (file Init_File.c)  */

int Open_File(struct comm_info* c_info);

/* Prototype CPU_Exploit, File: CPU_Exploit.c */

void CPU_Exploit(float target_secs, int initialize);

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


/******* Implementation Read_Shared *******/

void Read_Shared(struct comm_info* c_info,int size,int n_sample,
                 MODES RUN_MODE,double* time)
/*************************************************************************/
/*------------------------------------------------------------
MPI_Read with shared file pointers

RUN_MODE determines "aggregate", "non blocking" and "collective" property.

In the non blocking case:
  First:  Call the corresponding blocking benchmark for comparison
  Then:   Call the non blocking version.

Further description: see the accompanying document
-------------------------------------------------------------*/


{
if( RUN_MODE->AGGREGATE )
Read_ij(c_info, size, shared, RUN_MODE->type, 1, n_sample, 1, time);
else
Read_ij(c_info, size, shared, RUN_MODE->type, n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
Open_File(c_info);

if( RUN_MODE->AGGREGATE )
IRead_ij(c_info, size, shared, RUN_MODE->type, 1, n_sample, 1, 1, time+1);
else
IRead_ij(c_info, size, shared, RUN_MODE->type,  n_sample, 1, 0, 1, time+1);
}

}
 
/*************************************************************************/

/******* Implementation Read_Indv *******/

void Read_Indv(struct comm_info* c_info,int size,int n_sample,
                MODES RUN_MODE,double* time)
/*************************************************************************/
/*------------------------------------------------------------
MPI_Read with individual file pointers

RUN_MODE determines "aggregate", "non blocking" and "collective" property.

In the non blocking case:
  First:  Call the corresponding blocking benchmark for comparison
  Then:   Call the non blocking version.

Further description: see the accompanying document
-------------------------------------------------------------*/

{

if( RUN_MODE->AGGREGATE )
Read_ij(c_info, size, indv_block, RUN_MODE->type, 1, n_sample, 1, time);
else
Read_ij(c_info, size, indv_block, RUN_MODE->type, n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
Open_File(c_info);

if( RUN_MODE->AGGREGATE )
IRead_ij(c_info, size, indv_block, RUN_MODE->type, 1, n_sample, 1, 1, time+1);
else
IRead_ij(c_info, size, indv_block, RUN_MODE->type,  n_sample, 1, 0, 1, time+1);

}


}


/******* Implementation Read_Expl *******/

void Read_Expl(struct comm_info* c_info,int size,int n_sample,
              MODES RUN_MODE,double* time)
/*************************************************************************/
/*------------------------------------------------------------
MPI_Read with explicit file pointers

RUN_MODE determines "aggregate", "non blocking" and "collective" property.

In the non blocking case:
  First:  Call the corresponding blocking benchmark for comparison
  Then:   Call the non blocking version.

Further description: see the accompanying document
-------------------------------------------------------------*/

{
if( RUN_MODE->AGGREGATE )
Read_ij(c_info, size, explicit, RUN_MODE->type, 1, n_sample, 1, time);
else
Read_ij(c_info, size, explicit, RUN_MODE->type, n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
Open_File(c_info);

if( RUN_MODE->AGGREGATE )
IRead_ij(c_info, size, explicit, RUN_MODE->type, 1, n_sample, 1, 1, time+1);
else
IRead_ij(c_info, size, explicit, RUN_MODE->type,  n_sample, 1, 0, 1, time+1);
}

}



/******* Implementation Read_ij *******/

void Read_ij(struct comm_info* c_info,int size, 
             POSITIONING pos, BTYPES type,
             int i_sample, int j_sample, int time_inner,
             double* time)
/*
The proper input function, blocking cases
    pos:  file pointer type
    type: collective/non collective distinction
    i_sample/j_sample:
          1/n_sample in the aggregate, n_sample/1 in the non aggregate case
          elementary benchmarks performed inside a double loop
          for( i=0 .. i_sample-1 )
             {
             for( j=0 .. j_sample-1 )
                  input ...
             Synchronize (!)
             }
          Thus, every j_sample sample a synchronization takes place
*/

{
int i, j;
int Locsize, Totalsize;
MPI_Status stat;
MPI_Offset Offset;


int (* GEN_File_read)(MPI_File fh, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);
int (* GEN_File_read_shared)
                      (MPI_File fh, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);
int (* GEN_File_read_at) 
                      (MPI_File fh, MPI_Offset offset, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);

#ifdef CHECK
defect = 0.;
#endif
ierr = 0;

*time = 0.;

if( c_info->File_rank >= 0 )
{

if (type == Collective )
  {
    GEN_File_read = MPI_File_read_all;
    GEN_File_read_shared = MPI_File_read_ordered;
    GEN_File_read_at = MPI_File_read_at_all;
#ifdef DEBUG
fprintf(unit,"Collective input\n");
#endif
  }
else
  {
    GEN_File_read = MPI_File_read;
    GEN_File_read_shared = MPI_File_read_shared;
    GEN_File_read_at = MPI_File_read_at;
#ifdef DEBUG
fprintf(unit,"Non collective input\n");
#endif
  }


Locsize = c_info->split.Locsize;
Totalsize = c_info->split.Totalsize;

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
BLOCKING COLLECTIVE/NON COLLECTIVE INPUT CASES COMBINED
(function pointer GEN_File_read_XXX
either standard or collective MPI_File_read_XXX
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

if( !time_inner ) *time = MPI_Wtime();

for ( i=0; i<i_sample; i++ )
{

if( time_inner ) {


for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

*time = MPI_Wtime();
}

if( pos == indv_block )
{

for( j=0; j<j_sample; j++ )
{

ierr=GEN_File_read(c_info->fh,c_info->r_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Read standard ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);

CHK_DIFF("Read_indv",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         get, pos, i_sample*j_sample, j,
         -2, &defect);

}
}

else if( pos == explicit )
{

for( j=0; j<j_sample; j++ )
{
Offset = c_info->split.Offset+(MPI_Offset)(j*Totalsize);

ierr=GEN_File_read_at
   (c_info->fh, Offset, c_info->r_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Read explicit ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);


CHK_DIFF("Read_expl",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         get, pos, i_sample*j_sample, j,
         -2, &defect);

}
}

else if( pos == shared )
{
for( j=0; j<j_sample; j++ )
{

ierr=GEN_File_read_shared
   (c_info->fh,c_info->r_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Read shared ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);


#ifdef CHECK
chk_diff("Read_shared",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         get, pos, i_sample*j_sample, j,
         -3, &defect);

MPI_Barrier(c_info->File_comm);
#endif

}

}

if( time_inner ) *time = (MPI_Wtime()-*time)/(i_sample*j_sample);

}

if( !time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

} /* end if FIle_rank >= 0 */

}


/******* Implementation IRead_ij (nonblocking benchmarks) *******/

void IRead_ij(struct comm_info* c_info,int size, 
              POSITIONING pos, BTYPES type,
              int i_sample, int j_sample, int time_inner,int do_ovrlp,
              double* time)
/*
The proper input function, non blocking cases
    pos:  file pointer type
    type: collective/non collective distinction
    i_sample/j_sample, non collective case:
          1/n_sample in the aggregate, n_sample/1 in the non aggregate case
          elementary benchmarks performed inside a double loop
          for( i=0 .. i_sample-1 )
             {
             for( j=0 .. j_sample-1 )
                  Non blocking input ...

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
int Locsize, Totalsize;
MPI_Status *STAT, stat;
MPI_Request *REQUESTS;
MPI_Offset Offset;

#ifdef CHECK
defect = 0.;
#endif
ierr = 0;

*time = 0.;

if( c_info->File_rank >= 0 )
{

Locsize = c_info->split.Locsize;
Totalsize = c_info->split.Totalsize;


if( type == Collective )
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON BLOCKING COLLECTIVE INPUT CASES
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

{
for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

*time = MPI_Wtime();

if( pos == indv_block )

for ( j=0; j<i_sample*j_sample; j++ )
{

ierr=MPI_File_read_all_begin
  (c_info->fh,c_info->r_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);

if( do_ovrlp )
CPU_Exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_read_all_end
  (c_info->fh,c_info->r_buffer,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IRead coll. ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);

CHK_DIFF("Coll. IRead_indv",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize,
         get, pos, i_sample*j_sample, j,
         -2, &defect);

}

else if ( pos == explicit )

for ( j=0; j<i_sample*j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)(j*Totalsize);

ierr=MPI_File_read_at_all_begin
  (c_info->fh,Offset,c_info->r_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);


if( do_ovrlp )
CPU_Exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_read_at_all_end
  (c_info->fh,c_info->r_buffer,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IRead expl coll. ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);
CHK_DIFF("Coll. IRead_expl",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize,
         get, pos, i_sample*j_sample, j,
         -2, &defect);


}

else if ( pos == shared )

for ( j=0; j<i_sample*j_sample; j++ )
{

ierr=MPI_File_read_ordered_begin
  (c_info->fh,c_info->r_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);


if( do_ovrlp )
CPU_Exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_read_ordered_end
  (c_info->fh,c_info->r_buffer,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IRead shared coll. ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);
CHK_DIFF("Coll. IRead_shared",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize,
         get, pos, i_sample*j_sample, j,
         -3, &defect);

}


*time = (MPI_Wtime() - *time)/(i_sample*j_sample);

}

else  /* type non-Collective */
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON BLOCKING NON COLLECTIVE INPUT CASES
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/


{

REQUESTS = (MPI_Request*)v_alloc(j_sample*sizeof(MPI_Request), "IRead_ij");
STAT     = (MPI_Status *)v_alloc(j_sample*sizeof(MPI_Status ), "IRead_ij");

for( j=0; j<j_sample; j++ ) REQUESTS[j]=MPI_REQUEST_NULL;

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

if( !time_inner ) *time = MPI_Wtime();

for ( i=0; i<i_sample; i++ )
{

if( time_inner ) {

MPI_Barrier(c_info->File_comm);

*time = MPI_Wtime();
}

if( pos == indv_block )
{

for( j=0; j<j_sample; j++ )
{

ierr=MPI_File_iread
   (c_info->fh,c_info->r_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);


#if (defined CHECK || defined DEBUG)
MPI_Wait(REQUESTS+j,STAT);
DIAGNOSTICS("IRead standard ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);

CHK_DIFF("IRead_indv",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         get, pos, i_sample*j_sample, j,
         -2, &defect);
#endif

}

}

else if( pos == explicit )
{

for( j=0; j<j_sample; j++ )
{
Offset = c_info->split.Offset+(MPI_Offset)(j*Totalsize);

ierr=MPI_File_iread_at
  (c_info->fh,Offset,c_info->r_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);



#if (defined CHECK || defined DEBUG)
MPI_Wait(REQUESTS+j,STAT);
DIAGNOSTICS("IRead expl ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);

CHK_DIFF("IRead_expl",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         get, pos, i_sample*j_sample, j,
         -2, &defect);
#endif

}
}

else if( pos == shared )
{
for( j=0; j<j_sample; j++ )
{

ierr=MPI_File_iread_shared
  (c_info->fh,c_info->r_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);


#if (defined CHECK || defined DEBUG)
MPI_Wait(REQUESTS+j,STAT);
DIAGNOSTICS("IRead shared ",c_info,c_info->r_buffer,Locsize,Totalsize,j,pos);

CHK_DIFF("IRead_shared",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         get, pos, i_sample*j_sample, j,
         -3, &defect);

MPI_Barrier(c_info->File_comm);
#endif

}

}

if( do_ovrlp )
for ( j=0; j<j_sample; j++ )
CPU_Exploit(TARGET_CPU_SECS,0);

if( j_sample == 1 )
MPI_Wait(REQUESTS,STAT);
else                 
MPI_Waitall(j_sample,REQUESTS,STAT);


if( time_inner ) *time = (MPI_Wtime()-*time)/(i_sample*j_sample);

}

if( !time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

free (REQUESTS);
free (STAT);    

} /* end if type */

} /* end if FIle_rank >= 0 */

}
