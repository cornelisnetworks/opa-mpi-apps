
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




/*-----------------------------------------------------------*/

#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Accumulate (this file)  */

void Accumulate (
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);


/*** PROTOTYPES (extern)   ***/

/* Prototype chk_diff, File: chk_diff.c */

void chk_diff(
     char* text,
     struct comm_info* c_info, void* RECEIVED, int buf_pos, 
     int Locsize, int Totalsize, int unit_size, 
     DIRECTION mode, POSITIONING fpos,
     int n_sample, int j_sample,
     int source, double* diff );

/* Prototype Err_Hand (file Err_Handler.c)  */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );



/******************************/
/*       END PROTOTYPES       */


/*************************************************************************/


/******* Implementation Accumulate *******/

void Accumulate (struct comm_info* c_info,
                 int size,int n_sample,MODES RUN_MODE,double* time)
/*************************************************************************/

/*------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             size     | int               | message length in byte
	     n_sample | int               | repetition count
             RUN_MODE | MODES (typedef,   | Distinction aggregate/
                      | see Benchmark.h)  | non aggr., see docu.
                      |                   |
Output     : time     | double*           | *time: time/sample in usec
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
------------------------------------------------------------
------------------------------------------------------------
Description: see the accompanying document
-------------------------------------------------------------*/
{
  double t1, t2;
  
  Type_Size s_size,r_size;
  int s_num, r_num;
  int s_tag, r_tag;
  int dest, source, root;
  int i;
  MPI_Status stat;


#ifdef CHECK 
  defect=0;
#endif
  ierr = 0;

  /*  GET SIZE OF DATA TYPE */  
MPI_Type_size(c_info->red_data_type,&s_size);
if (s_size!=0) s_num=size/s_size;

root = (c_info-> rank == 0);


if( c_info-> rank < 0 )
*time = 0.;
else
{

if( !RUN_MODE->AGGREGATE )
{

*time = MPI_Wtime();

for(i=0;i< n_sample;i++)
	{

       ierr = MPI_Accumulate
                     (c_info->s_buffer, s_num, c_info->red_data_type,
                      0, i*s_num, s_num, c_info->red_data_type, c_info->op_type,
                      c_info->WIN );
       MPI_ERRHAND(ierr);

       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);
#ifdef CHECK
if( root ) 
{
       CHK_DIFF("Accumulate",c_info, (void*)(c_info->r_data+i*s_num), 0,
                size, size, asize, 
                put, 0, n_sample, i,
                -1, &defect);
       ass_buf(c_info->r_buffer, 0, 0, size-1, 0);
}
MPI_Barrier(c_info->communicator);
#endif

	}
*time=(MPI_Wtime()-*time)/n_sample;
}

if( RUN_MODE->AGGREGATE )
{

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

for(i=0;i< n_sample;i++)
	{


       ierr = MPI_Accumulate
                ((void*)(c_info->s_data+i*s_num), s_num, c_info->red_data_type,
                 0, i*s_num, s_num, c_info->red_data_type, c_info->op_type,
                 c_info->WIN );
       MPI_ERRHAND(ierr);

	}

       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

*time=(MPI_Wtime()-*time)/n_sample;

#ifdef CHECK
if( root ) 
{
    CHK_DIFF("Accumulate",c_info, c_info->r_buffer, 0,
             n_sample*size, n_sample*size, asize, 
             put, 0, n_sample, -1,
             -1, &defect);
}
#endif



}

}
}
