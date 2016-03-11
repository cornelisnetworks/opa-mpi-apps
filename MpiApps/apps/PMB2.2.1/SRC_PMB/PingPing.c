
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

/* Prototype PingPing (this file)  */

void PingPing(
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



/******* Implementation PingPing *******/

void PingPing(struct comm_info* c_info,int size,int n_sample,
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
------------------------------------------------------------------*/
{
  double t1,t2;
  int    i;
  
  Type_Size s_size, r_size;
  int s_num, r_num;
  int s_tag, r_tag;
  int dest, source;
  MPI_Status stat;
  MPI_Request request;

#ifdef CHECK 
  defect=0;
#endif
  ierr = 0;

  MPI_Type_size(c_info->s_data_type,&s_size);
  MPI_Type_size(c_info->r_data_type,&r_size);

  if ((s_size!=0) && (r_size!=0))
   {
      s_num=size/s_size;
      r_num=size/r_size;
    } 
  s_tag = 1;
  r_tag = c_info->select_tag ? s_tag : MPI_ANY_TAG;
  
  dest = -1;
  if (c_info->rank == c_info->pair0)
      dest = c_info->pair1;
  else if (c_info->rank == c_info->pair1)
      dest = c_info->pair0;

  source = c_info->select_source ? dest : MPI_ANY_SOURCE;
      
  if( dest != -1 )
    {
      for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

      t1 = MPI_Wtime();
      for(i=0;i< n_sample;i++)
	{
	  ierr= MPI_Isend(c_info->s_buffer,s_num,
			  c_info->s_data_type,dest,s_tag,
			  c_info->communicator,&request);
	  MPI_ERRHAND(ierr);
	  ierr = MPI_Recv(c_info->r_buffer,r_num,c_info->r_data_type,source,
			  r_tag,c_info->communicator,&stat);
	  MPI_ERRHAND(ierr);

	  ierr = MPI_Wait(&request, &stat);
	  MPI_ERRHAND(ierr);

          CHK_DIFF("PingPing",c_info, c_info->r_buffer, 0,
                    size, size, asize,
                    put, 0, n_sample, i,
                    dest, &defect);
	}
      t2 = MPI_Wtime();
      
      *time=(t2 - t1)/n_sample;
    }
  else
    { 
      *time = 0.; 
    }
}

