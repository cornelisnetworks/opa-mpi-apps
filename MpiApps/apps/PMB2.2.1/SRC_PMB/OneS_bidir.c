
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

/* Prototype Bidir_Get (this file)  */

void Bidir_Get(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Bidir_Put (this file)  */

void Bidir_Put(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);


/*** PROTOTYPES (extern)   ***/

/* Prototype OneS_mGet, File: OneS_unidir.c */

void OneS_mGet(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);

/* Prototype OneS_mPut, File: OneS_unidir.c */

void OneS_mPut(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);

/* Prototype OneS_Get, File: OneS_unidir.c */

void OneS_Get(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);

/* Prototype OneS_Put, File: OneS_unidir.c */

void OneS_Put(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);


/******************************/
/*       END PROTOTYPES       */

/*************************************************************************/


/******* Implementation Bidir_Get *******/

void Bidir_Get(struct comm_info* c_info,
               int size,int n_sample,MODES RUN_MODE, double* time)
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
  int dest, source,sender;
  MPI_Status stat;

  ierr = 0;
  /*  GET SIZE OF DATA TYPE */  
  MPI_Type_size(c_info->s_data_type,&s_size);
  MPI_Type_size(c_info->r_data_type,&r_size);
  if ((s_size!=0) && (r_size!=0))
    {
      s_num=size/s_size;
      r_num=size/r_size;
    } 

  if (c_info->rank == c_info->pair0)
    {
      dest = c_info->pair1;
    }
  else if (c_info->rank == c_info->pair1)
    {
      dest =c_info->pair0 ;
    }
 
  sender=0;

  if( !RUN_MODE->AGGREGATE )
     OneS_Get(  c_info,
                s_num, dest, 
                r_num, sender,
                size, n_sample, 
                time);
  if( RUN_MODE->AGGREGATE )
     OneS_mGet( c_info,
                s_num, dest, 
                r_num, sender,
                size, n_sample, 
                time);

}



/******* Implementation Bidir_Put *******/

void Bidir_Put(struct comm_info* c_info,
               int size,int n_sample,MODES RUN_MODE, double* time)
/*************************************************************************/

/*------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             size     | int               | message length in byte
	     n_sample | int               | repetition count
                      |                   |
Output     : time     | double*           | *time: time/sample in usec
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
------------------------------------------------------------
------------------------------------------------------------
Description: 
            Two processes take part. A message is transferred
	    by onesided communication
	    
-------------------------------------------------------------*/
{
  double t1, t2;
  
  Type_Size s_size,r_size;
  int s_num, r_num;
  int dest, source,sender;
  int ierr;
  MPI_Status stat;

  /*  GET SIZE OF DATA TYPE */  
  MPI_Type_size(c_info->s_data_type,&s_size);
  MPI_Type_size(c_info->r_data_type,&r_size);
  if ((s_size!=0) && (r_size!=0))
    {
      s_num=size/s_size;
      r_num=size/r_size;
    } 

  if (c_info->rank == c_info->pair0)
    {
      dest = c_info->pair1;
    }
  else if (c_info->rank == c_info->pair1)
    {
      dest =c_info->pair0 ;
    }
 
  
  sender=1;

  if( !RUN_MODE->AGGREGATE )
     OneS_Put(  c_info,
                s_num, dest, 
                r_num, sender,
                size, n_sample, 
                time);
  
  if( RUN_MODE->AGGREGATE )
     OneS_mPut( c_info,
                s_num, dest, 
                r_num, sender,
                size, n_sample, 
                time);

}
