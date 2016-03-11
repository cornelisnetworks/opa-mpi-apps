
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

/* Prototype Unidir_Put (this file)  */

void Unidir_Put (
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Unidir_Get (this file)  */

void Unidir_Get (
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype OneS_Get (this file)  */

void OneS_Get(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);

/* Prototype OneS_mGet (this file)  */

void OneS_mGet(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);

/* Prototype OneS_Put (this file)  */

void OneS_Put(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);

/* Prototype OneS_mPut (this file)  */

void OneS_mPut(
     struct comm_info* c_info, int s_num, int dest, 
     int r_num, int sender, int size, 
     int n_sample, double* time);


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

/*************************************************/



/******* Implementation Unidir_Put *******/

void Unidir_Put  (struct comm_info* c_info,
                 int size,int n_sample,MODES RUN_MODE,double* time)
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
      sender = 1;
    }
  else if (c_info->rank == c_info->pair1)
    {
      dest =c_info->pair0 ;
      sender = 0;
    }
 
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

/*************************************************************************/


/******* Implementation Unidir_Get *******/

void Unidir_Get  (struct comm_info* c_info,
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
      sender = 1;
    }
  else if (c_info->rank == c_info->pair1)
    {
      dest =c_info->pair0 ;
      sender = 0;
    }
 

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




/******* Implementation OneS_Get *******/

void OneS_Get( struct comm_info* c_info, 
                int s_num, int dest, 
                int r_num, int sender,
                int size, int n_sample, 
                double* time)
{
int i, ierr;
int s_size;

#ifdef CHECK 
  defect=0;
#endif

MPI_Type_size(c_info->s_data_type,&s_size);

if( c_info-> rank < 0 )
*time = 0.;
else
{

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( sender ) 
{

for(i=0;i< n_sample;i++)
	{

/* "Send ", i.e. synchronize window */

       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

	}
}
else
{

for(i=0;i< n_sample;i++)
	{

/* "Receive" */
       ierr = MPI_Get(c_info->r_buffer, r_num, c_info->r_data_type,
                      dest, i*s_num, s_num, c_info->s_data_type, c_info->WIN);
       ierr = MPI_Win_fence(0, c_info->WIN);

       MPI_ERRHAND(ierr);

       DIAGNOSTICS("MPI_Get: ",c_info,c_info->r_buffer,r_num,r_num,i,0);

       CHK_DIFF("MPI_Get",c_info, c_info->r_buffer, i*r_num,
                 size, size, asize,
                 get, 0, n_sample, i,
                 dest, &defect);
	}

}

*time=(MPI_Wtime()-*time)/n_sample;

}
}



/******* Implementation OneS_mGet *******/

void OneS_mGet( struct comm_info* c_info, 
                int s_num, int dest, 
                int r_num, int sender,
                int size, int n_sample, 
                double* time)
{
int i, ierr;
char* recv;

#ifdef CHECK 
defect=0;
#endif

if( c_info-> rank < 0 )
*time = 0.;
else
{
recv = (char*)c_info->r_buffer;

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( !sender )
for(i=0;i< n_sample;i++)
	{
        ierr = MPI_Get((void*)(recv+i*r_num), r_num, c_info->r_data_type,
                       dest, i*s_num, s_num, c_info->s_data_type, c_info->WIN);
	}

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

*time=(MPI_Wtime()-*time)/n_sample;

#ifdef CHECK
if(!sender)
       CHK_DIFF("MPI_Get",c_info, c_info->r_buffer, 0,
                 n_sample*r_num, n_sample*r_num, asize,
                 get, 0, n_sample, i,
                 dest, &defect);
#endif

}
}



/******* Implementation OneS_Put *******/

void OneS_Put( struct comm_info* c_info, 
                int s_num, int dest, 
                int r_num, int sender,
                int size, int n_sample, 
                double* time)
{
int i, ierr, r_size;
char* recv;

#ifdef CHECK 
defect=0;
#endif

MPI_Type_size(c_info->r_data_type,&r_size);

recv = (char*)c_info->r_buffer;

if( c_info-> rank < 0 )
*time = 0.;
else
{

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( sender ) 
{

for(i=0;i< n_sample;i++)
	{

/* Send */

       ierr = MPI_Put(c_info->s_buffer, s_num, c_info->s_data_type,
                      dest, i*r_num, r_num, c_info->r_data_type, c_info->WIN);

       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

        }
}
else
{

for(i=0;i< n_sample;i++)
	{

/* "Receive", i.e. synchronize the window */
       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

       DIAGNOSTICS("MPI_Put: ",c_info,c_info->r_buffer,r_num,r_num,i,0);

       CHK_DIFF("MPI_Put",c_info, (void*)(recv+i*r_num), 0,
                 size, size, asize,
                 get, 0, n_sample, i,
                 dest, &defect);

        }
}

*time=(MPI_Wtime()-*time)/n_sample;

}
}



/******* Implementation OneS_mPut *******/

void OneS_mPut( struct comm_info* c_info, 
                int s_num, int dest, 
                int r_num, int sender,
                int size, int n_sample, 
                double* time)
{
int i, ierr;
char* send;

#ifdef CHECK 
defect=0;
#endif

if( c_info-> rank < 0 )
*time = 0.;
else
{
send = (char*)c_info->s_buffer;

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( sender )
for(i=0;i< n_sample;i++)
	{
          ierr = MPI_Put((void*)(send+i*s_num), s_num, c_info->s_data_type,
                         dest, i*r_num, r_num, c_info->r_data_type, c_info->WIN);
          MPI_ERRHAND(ierr);

	}

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

*time=(MPI_Wtime()-*time)/n_sample;

if(!sender)
       CHK_DIFF("MPI_Put",c_info, c_info->r_buffer, 0,
                 n_sample*r_num, n_sample*r_num, asize,
                 get, 0, n_sample, -1,
                 dest, &defect);

}
}
