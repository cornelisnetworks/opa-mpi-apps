
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

/* Prototype Barrier (this file)  */

void Barrier(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/*** PROTOTYPES (extern)   ***/

/* Prototype Err_Hand (file Err_Handler.c)  */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );


/******************************/
/*       END PROTOTYPES       */


/*************************************************************************/


/******* Implementation Barrier *******/

void Barrier(struct comm_info*  c_info, int size, int n_sample, 
                    MODES RUN_MODE,double* time)
/*************************************************************************/

/*----------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
------------------------------------------------------------------
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
------------------------------------------------------------------
------------------------------------------------------------------
Description: see the accompanying document
----------------------------------------------------------------*/
{
  double t1, t2;
  int    i;

  ierr = 0;

  if(c_info->rank!=-1)
    {
      for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

      t1 = MPI_Wtime();
      for(i=0;i< n_sample;i++)
	{
	  ierr= MPI_Barrier(c_info->communicator);
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

