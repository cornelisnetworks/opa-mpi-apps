
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
 *        2.2 change: MPI_Win_fence in Init_Transfer for PMB-EXT             * 
 *  Revision: 2.2.1                                                          *
 *  Date: 2002/07/04                                                         *
 *        Handle case of empty file view separately (Init_Transfer, PMB-IO)  *
 *        Some implementations dont seem to like empty views                 *
 *                                                                           *
 *****************************************************************************/




#include "mpi.h"
#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Init_Transfer (this file)  */

void Init_Transfer(
     struct comm_info* c_info, struct Bench* Bmark, int size
     );

/* Prototype Close_Transfer (this file)  */

void Close_Transfer (
     struct comm_info* c_info, struct Bench* Bmark, int size
     );


/*** PROTOTYPES (extern)   ***/

/* Prototype set_buf, File: Mem_Manager.c */

void set_buf(
     struct comm_info* c_info, int selected_rank, int s_pos1, 
     int s_pos2, int r_pos1, int r_pos2
     );

/* Prototype Err_Hand, File: Err_Handler.c */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/* Prototype get_rank_portion, File: pmb_init.c */

void get_rank_portion(
     int rank, int NP, int size, 
     int unit_size, int* pos1, int* pos2
     );

/* Prototype Set_Errhand (file Err_Handler.c)  */

void Set_Errhand(
     struct comm_info* c_info);


/******************************/
/*       END PROTOTYPES       */




/******* Implementation Init_Transfer *******/

void Init_Transfer
(struct comm_info* c_info, struct Bench* Bmark, int size)
{


#ifdef MPIIO

int ne, baslen, mod;
int ierr;
int pos1, pos2;

if( c_info->File_rank < 0 || Bmark->access == no ) return;

get_rank_portion(c_info->File_rank, c_info->all_io_procs, size, asize,
                 &pos1, &pos2);
baslen = max(0,pos2-pos1+1);

if( c_info->view != MPI_DATATYPE_NULL )
    MPI_Type_free(&c_info->view);

if( Bmark->fpointer == private )
  {

  c_info->split.Locsize = baslen;
  c_info->split.Offset = 0;
  c_info->split.Totalsize = baslen;

  if( Bmark->access == put )
    set_buf(c_info, c_info->File_rank, 0, baslen-1, 1, 0);
  if( Bmark->access == get )
    set_buf(c_info, c_info->File_rank, 1, 0, 0, baslen-1);
  
  }
if( Bmark->fpointer == indv_block || Bmark->fpointer == shared ||
    Bmark->fpointer == explicit )
  {
  int bllen[3];

  MPI_Aint displ[3];
  MPI_Datatype types[3];

  bllen[0]=1; displ[0] = 0; types[0] = MPI_LB;

  bllen[1] = baslen;
  displ[1] = pos1;
  types[1] = c_info->etype;

  bllen[2] = 1;
  displ[2] = size;
  types[2] = MPI_UB;

  if( Bmark->fpointer == indv_block )
  {
/* July 2002 fix V2.2.1: handle empty view case separately */
  if( baslen>0 ){
/* end change */
  ierr=MPI_Type_struct(3,bllen,displ,types,&c_info->view);
  Err_Hand(1,ierr);
  ierr=MPI_Type_commit(&c_info->view);
  Err_Hand(1,ierr);
  c_info->filetype = c_info->view;

/* July 2002 fix V2.2.1: handle empty case */
  }
  else c_info->filetype = c_info->etype;
/* end change */

  }

  if( Bmark->access == put )
     set_buf(c_info, c_info->File_rank, 0, baslen-1, 1, 0 );

  if( Bmark->access == get )
     set_buf(c_info, c_info->File_rank, 1, 0, 0, baslen-1 );

  c_info->split.Locsize = bllen[1];
  c_info->split.Offset  = pos1;
  c_info->split.Totalsize = size;
  
  }

  ierr = Open_File(c_info);

#else

#ifdef EXT
int sz, s_size, r_size, maxlen;
int ierr;

ierr=0;

if( Bmark->reduction )
{
MPI_Type_size(c_info->red_data_type,&s_size);
r_size=s_size;
}
else
{
MPI_Type_size(c_info->s_data_type,&s_size);
MPI_Type_size(c_info->r_data_type,&r_size);
}

if( c_info -> rank >= 0 )
{
User_Set_Info(&c_info->info);

maxlen = 1<<MAXMSGLOG;
sz = max(maxlen,OVERALL_VOL);
if( OVERALL_VOL/MSGSPERSAMPLE > maxlen ) sz = maxlen*MSGSPERSAMPLE;

if( Bmark->access == put)
{
  ierr = MPI_Win_create(c_info->r_buffer,sz,r_size,c_info->info,
                        c_info->communicator, &c_info->WIN);
  MPI_ERRHAND(ierr);
  ierr = MPI_Win_fence(0, c_info->WIN);
  MPI_ERRHAND(ierr);
}
else if( Bmark->access == get)
{
  ierr = MPI_Win_create(c_info->s_buffer,sz,s_size,c_info->info,
                        c_info->communicator, &c_info->WIN);
  MPI_ERRHAND(ierr);
  ierr = MPI_Win_fence(0, c_info->WIN);
  MPI_ERRHAND(ierr);
}


}

#endif

#endif

Set_Errhand(c_info);
err_flag = 0;

}



/******* Implementation Close_Transfer *******/

void Close_Transfer (struct comm_info* c_info, struct Bench* Bmark, int size)
{
#ifdef MPIIO
if( c_info->view != MPI_DATATYPE_NULL )
    MPI_Type_free(&c_info->view);

MPI_File_close(&c_info->fh);

#else
#ifdef EXT

if( c_info->WIN != MPI_WIN_NULL )
MPI_Win_free(&c_info->WIN);

#endif

#endif
}
