
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

/* Prototype Exchange (this file)  */

void Exchange(
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


/******* Implementation Exchange *******/

void Exchange(struct comm_info* c_info,int size,int n_sample,
                    MODES RUN_MODE,double* time)
/*************************************************************************/

/*-----------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
-------------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             size     | int               | message length in byte
	     n_sample | int               | repetition count
             RUN_MODE | MODES (typedef,   | unused (MPI-2 only)
                      | see Benchmark.h)  |
                      |                   |
Output     : time     | double*           | *time: time/sample in usec
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
-------------------------------------------------------------------
-------------------------------------------------------------------
Description: see the accompanying document
-----------------------------------------------------------------*/
{
  double t1, t2;
  int  i;
  
  Type_Size s_size, r_size;
  int s_num, r_num;
  int s_tag, r_tag;
  int left, right;
  MPI_Status  stat[2];
  MPI_Request request[2];
  
#ifdef CHECK 
  defect=0;
#endif
  ierr = 0;

  /*GET SIZE OF DATA TYPE's in s_size and r_size*/
  MPI_Type_size(c_info->s_data_type,&s_size);
  MPI_Type_size(c_info->r_data_type,&r_size);
  if ((s_size!=0) && (r_size!=0))
    {
      s_num=size/s_size;
      r_num=size/r_size;
    } 
  s_tag = 1;
  r_tag = c_info->select_tag ? s_tag : MPI_ANY_TAG;
  
  if(c_info->rank != -1)
    {
      if(c_info->rank < c_info->num_procs-1)   right  = c_info->rank+1;
      if(c_info->rank > 0)                     left   = c_info->rank-1;
      
      if(c_info->rank == c_info->num_procs-1)  right  = 0;
      if(c_info->rank == 0)                    left   = c_info->num_procs-1 ;
      
      if((c_info->rank >= 0) && (c_info->rank <= c_info->num_procs-1))
	{
          for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

	  t1 = MPI_Wtime();
	  for(i=0;i< n_sample;i++)
	    { 
	      ierr= MPI_Isend(c_info->s_buffer,s_num,c_info->s_data_type,
			      right,s_tag,c_info->communicator,&request[0]);
	      MPI_ERRHAND(ierr);
	      ierr= MPI_Isend(c_info->s_buffer,s_num,c_info->s_data_type,
			      left ,s_tag,c_info->communicator,&request[1]);
	      MPI_ERRHAND(ierr);
      
	      ierr= MPI_Recv( c_info->r_buffer,r_num,c_info->r_data_type,
			      left ,r_tag,c_info->communicator,stat);
	      MPI_ERRHAND(ierr);

              CHK_DIFF("Exchange",c_info, c_info->r_buffer, 0,
                       size, size, 1, 
                       put, 0, n_sample, i,
                       left, &defect);

	      ierr= MPI_Recv( c_info->r_buffer,r_num,c_info->r_data_type,
			      right,r_tag,c_info->communicator,stat);
	      MPI_ERRHAND(ierr);
	      
              CHK_DIFF("Exchange",c_info, c_info->r_buffer, 0,
                       size, size, 1, 
                       put, 0, n_sample, i,
                       right, &defect);

	      ierr= MPI_Waitall(2,request,stat);
	      MPI_ERRHAND(ierr);   
	    }
	  t2 = MPI_Wtime();
	  *time=(t2 - t1)/n_sample;
	}
    }
  else
    { 
      *time = 0.; 
    }
}


