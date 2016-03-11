
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
 *  Revision: 2.2.1                                                          *
 *  Date: 2002/07/04                                                         *
 *        Several fixes and cleanups (see comment "July 2002 fix V2.2.1:")   * 
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Init_File_Content (this file)  */

void Init_File_Content(
     void* BUF, int pos1, int pos2
     );

/* Prototype Init_File (this file)  */

int Init_File(
     struct comm_info* c_info, struct Bench* Bmark, int NP
     );

/* Prototype Free_File (this file)  */

void Free_File(
     struct comm_info * c_info);

/* Prototype Del_File (this file)  */

void Del_File(
     struct comm_info* c_info);

/* Prototype Open_File (this file)  */

int Open_File(struct comm_info* c_info);

/*** PROTOTYPES (extern)   ***/

/* Prototype str, File: strgs.c */

char* str(
     char* Bname);

/* Prototype v_alloc (file Mem_Manager.c)  */

void* v_alloc(
      int Len, char* where
     );


/* Prototype del_s_buf, File: Mem_Manager.c */

void del_s_buf(
     struct comm_info* c_info );

/* Prototype Err_Hand, File: Err_Handler.c */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/* Prototype ass_buf, File: Mem_Manager.c */

void ass_buf(
     void* buf, int rank, int pos1, 
     int pos2, int value);

/* Prototype User_Set_Info, File: User_Set_Info.c */

void User_Set_Info(
     MPI_Info* opt_info);

/* Prototype alloc_buf, File: Mem_Manager.c */

void alloc_buf(
     struct comm_info* c_info, char* where, int s_len, 
     int r_len);


/******************************/
/*       END PROTOTYPES       */



/******* Implementation Init_File_Content *******/

void Init_File_Content(void* BUF, int pos1, int pos2)
{
ass_buf( BUF, 0, pos1, pos2, 1);
}



/******* Implementation Init_File *******/

int Init_File( struct comm_info* c_info, struct Bench* Bmark, int NP)
{
int error = 0;
int fnlen;

Free_File(c_info);

c_info->fh = MPI_FILE_NULL;
c_info->etype = MPI_BYTE;
MPI_Type_size(c_info->etype,&c_info->e_size);
c_info->filetype = c_info->etype;

User_Set_Info(&c_info->info);

c_info-> disp = (MPI_Offset)0;

c_info->datarep = str("native"); 

if( Bmark->RUN_MODES[0].type == SingleTransfer )
    c_info->all_io_procs = 1;
else
    c_info->all_io_procs = c_info->num_procs;

fnlen = 1+strlen(FILENAME);
/* July 2002 fix V2.2.1: group_mode >= 0 */
if( c_info->group_mode >= 0 )
  fnlen += 4;

if( Bmark->fpointer == private )
  {

  if( c_info->rank > c_info->all_io_procs-1 || c_info->rank < 0 )
  {
  c_info->File_comm = MPI_COMM_NULL;
  c_info->File_rank = -1;
  c_info->File_num_procs = 0;
  }
  else
  {
  c_info->File_comm = MPI_COMM_SELF;
  c_info->File_rank = 0;
  c_info->File_num_procs = 1;
  }

  if( c_info->File_rank >= 0 )
  {

  fnlen += 4;

  c_info->filename = (char*) v_alloc(sizeof(char)*fnlen,"Init_File");

/* July 2002 fix V2.2.1: group_mode >= 0 */
  if( c_info->group_mode >= 0 )
  sprintf(c_info->filename,"%s_g%d_%d",FILENAME,c_info->group_no,c_info->w_rank);
  else
  sprintf(c_info->filename,"%s_%d",FILENAME,c_info->w_rank);

  c_info->amode = MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN;
  }

  }
else
  {
if( c_info->communicator == MPI_COMM_NULL ) 
  {
  c_info->File_comm = MPI_COMM_NULL;
  c_info->File_rank = -1;
  c_info->File_num_procs = 0;
  }
else
  {


  c_info->File_comm = c_info->communicator;

  c_info->File_rank = c_info->rank;
  c_info->File_num_procs = c_info->num_procs;
  }

  c_info->filename = (char*) v_alloc(sizeof(char)*fnlen,"Init_File");
/* July 2002 fix V2.2.1: group_mode >= 0 */
  if( c_info->group_mode >= 0 )
  sprintf(c_info->filename,"%s_g%d",FILENAME,c_info->group_no);
  else
  sprintf(c_info->filename,"%s",FILENAME);
  
  c_info->amode = MPI_MODE_CREATE | MPI_MODE_RDWR;
  }

  if( Bmark->access == no ) return 0;


  if( c_info -> File_rank == 0 )
   {

   int ierr, size, total;
   MPI_Status stat;

   Del_File(c_info);

   size=1<<MAXMSGLOG;
   total = max(size,OVERALL_VOL);

   if( OVERALL_VOL/size > MSGSPERSAMPLE )
       total = size*MSGSPERSAMPLE;
/* July 2002 fix V2.2.1: calculation of file sizes in "priv" case */
   if ( Bmark->fpointer == private ) 
    {
    int NP= c_info->all_io_procs;

    total = (total+NP-1)/NP;

    if ( size%NP ) total += asize*MSGSPERSAMPLE;
    }
/* July 2002 end fix */

   ierr = MPI_File_open(MPI_COMM_SELF, c_info->filename,
                        c_info->amode, c_info->info, &c_info->fh);
   Err_Hand(1,ierr);

   MPI_File_set_view(c_info->fh,(MPI_Offset)0,
       c_info->etype,c_info->etype,c_info->datarep,c_info->info);
   
/* July 2002 fix V2.2.1: size <-> total */
   ierr = MPI_File_set_size(c_info->fh, total);
   Err_Hand(1,ierr);

   if( Bmark->access == get )
   {
/* Prepare File for input */


   int el_size = 1<<20;
   int pos1, pos2;

   pos1 = 0;

   while( pos1 < total )
   {
   pos2 = min(total-1,pos1+el_size-1);
   size = ((pos2-pos1)/asize+1)*asize;


   alloc_buf(c_info, "Init_File 1 ",size, 0);
   Init_File_Content(c_info->s_buffer, pos1, pos2);

   ierr=MPI_File_write(c_info->fh,c_info->s_buffer,pos2-pos1+1,c_info->etype,&stat);
   Err_Hand(1,ierr);

   pos1 = pos2+1;
   }
   
   del_s_buf(c_info);
   }

   ierr= MPI_File_close(&c_info->fh);
   Err_Hand(1,ierr);

   }

return error;
}




/******* Implementation Free_File *******/

void Free_File(struct comm_info * c_info)
{
if( c_info->filename != (char*)NULL ) free(c_info->filename);
if( c_info->datarep  != (char*)NULL ) free(c_info->datarep );
if( c_info->filename != (char*)NULL )
if( c_info->view     != MPI_DATATYPE_NULL ) 
                        MPI_Type_free(&c_info->view);
if( c_info->info     != MPI_INFO_NULL ) 
                        MPI_Info_free(&c_info->info);
if( c_info->fh       != MPI_FILE_NULL )
                        MPI_File_close(& c_info->fh );
c_info->filename = (char*)NULL;
c_info->datarep  = (char*)NULL;
c_info->view     = MPI_DATATYPE_NULL;
c_info->info     = MPI_INFO_NULL;
c_info->fh       = MPI_FILE_NULL;
}



/******* Implementation Del_File *******/

void Del_File(struct comm_info* c_info)
{

if( c_info-> fh != MPI_FILE_NULL ) MPI_File_close(&c_info->fh);

if( c_info->filename != (char*)NULL )
{
if( c_info->File_rank == 0 )
  {
   int ierr;
   ierr = MPI_File_open(MPI_COMM_SELF, c_info->filename,
                        MPI_MODE_CREATE | MPI_MODE_DELETE_ON_CLOSE,
                        c_info->info, &c_info->fh);

   MPI_File_close(&c_info->fh);
  }
}
}

int Open_File(struct comm_info* c_info)
{
int ierr;
ierr = 0;
if ( c_info->File_comm != MPI_COMM_NULL )
{
ierr = MPI_File_open(c_info->File_comm, c_info->filename,
                     c_info->amode, c_info->info, &c_info->fh);
MPI_ERRHAND(ierr);

ierr = MPI_File_set_view(c_info->fh, c_info->disp, c_info->etype, 
                         c_info->filetype, c_info->datarep, c_info->info);
MPI_ERRHAND(ierr);
}
return ierr;
}
