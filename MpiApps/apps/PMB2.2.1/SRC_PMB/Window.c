
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
 *  Revision: 2.2                                                            *
 *  Date: 2000/08/03                                                         *
 *        2.2 change: MPI_Win_fence on created Window                        * 
 *  Revision: 2.2.1                                                          *
 *  Date: 2002/07/04                                                         *
 *        Fix for empty window case  (see comment "July 2002 fix V2.2.1:")   * 
 *                                                                           *
 *                                                                           *
 *****************************************************************************/




#include "declare.h"
#include "Benchmark.h"


/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Window (this file)  */

void Window(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);


/* Prototype Err_Hand (file Err_Handler.c)  */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/******************************/
/*       END PROTOTYPES       */


/*************************************************************************/


/******* Implementation Window *******/

void Window(struct comm_info*  c_info, int size, int n_sample, 
            MODES RUN_MODE,double* time)
/*************************************************************************/

/*----------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
------------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             size     | int               | message length in byte
	     n_sample | int               | repetition count
             RUN_MODE | MODES (typedef,   | unused
                      | see Benchmark.h)  |
                      |                   |
Output     : time     | double*           | *time: time/sample in usec
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
------------------------------------------------------------------
------------------------------------------------------------------
Description: see the accompanying document
----------------------------------------------------------------*/
{
  double t1, t2;
  int    i, dum;

  ierr = 0;

  if(c_info->rank!=-1)
    {
      for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

      t1 = MPI_Wtime();
      for(i=0;i< n_sample;i++)
	{
          ierr = MPI_Win_create(c_info->r_buffer,size,1,MPI_INFO_NULL,
                                c_info->communicator, &c_info->WIN);
          MPI_ERRHAND(ierr);
          ierr = MPI_Win_fence(0, c_info->WIN);
          MPI_ERRHAND(ierr);
/* July 2002 fix V2.2.1, empty window case */
          if(size>0)
          {
          ierr = MPI_Put(c_info->s_buffer, 1, c_info->s_data_type,
                         c_info->rank, 0, 1, c_info->r_data_type, c_info->WIN);
          MPI_ERRHAND(ierr);
          }
          ierr = MPI_Win_free(&c_info->WIN);
          MPI_ERRHAND(ierr);
	}
      t2 = MPI_Wtime();
      *time=(t2 - t1)/(n_sample);
    }
  else
    { 
      *time = 0.; 
    }
}

