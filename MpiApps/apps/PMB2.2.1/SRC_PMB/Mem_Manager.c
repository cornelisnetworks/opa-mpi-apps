
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
 *        Use of MPI_Alloc_mem/MPI_Free_mem (PMB-EXT/PMB-IO case)            *
 *                                                                           *
 *****************************************************************************/

#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype v_alloc (this file)  */

void* v_alloc(
      int Len, char* where
     );

/* Prototype i_alloc (this file)  */

void i_alloc(
     int** B, int Len, char* where 
     );

/* Prototype alloc_buf (this file)  */

void alloc_buf(
     struct comm_info* c_info, char* where, int s_len, 
     int r_len);

/* Prototype alloc_aux (this file)  */

void alloc_aux(
     int L, char* where);

/* Prototype free_aux (this file)  */

void free_aux(
     );

/* Prototype Init_Pointers (this file)  */

void Init_Pointers(
     struct comm_info *c_info );

/* Prototype ass_buf (this file)  */

void ass_buf(
     void* buf, int rank, int pos1, 
     int pos2, int value);

/* Prototype set_buf (this file)  */

void set_buf(
     struct comm_info* c_info, int selected_rank, int s_pos1, 
     int s_pos2, int r_pos1, int r_pos2
     );

/* Prototype Init_Buffers (this file)  */

void Init_Buffers(
     struct comm_info* c_info, struct Bench* Bmark, int size
     );

/* Prototype free_all (this file)  */

void free_all(
     struct comm_info* c_info, struct Bench** P_BList);

/* Prototype del_s_buf (this file)  */

void del_s_buf(
     struct comm_info* c_info );

/* Prototype del_r_buf (this file)  */

void del_r_buf(
     struct comm_info* c_info );

/*** PROTOTYPES (extern)   ***/

/* Prototype Init_Errhand (file Err_Handler.c)  */

void Init_Errhand(
     struct comm_info* c_info);

/* Prototype Del_Errhand (file Err_Handler.c)  */

void Del_Errhand(
     struct comm_info* c_info);

/* Prototype Destruct_BList (file BenchList.c)  */

void Destruct_BList(
     struct Bench ** P_BList);



/******************************/
/*       END PROTOTYPES       */




/******* Implementation v_alloc *******/

void* v_alloc(int Len, char* where)
{
void* B;
Len=max(asize,Len);
if( (B = (void*)malloc(Len) ) == NULL )
  {
printf ("Memory allocation failed. code position: %s. tried to alloc. %d bytes\n",where,Len);
return NULL;
  }
return B;
}



/******* Implementation i_alloc *******/

void i_alloc(int** B, int Len, char* where )
{
Len=max(1,Len);
*B = (int*) v_alloc(sizeof(int)*Len, where);
}




/******* Implementation alloc_buf *******/

void alloc_buf(struct comm_info* c_info, char* where,
               int s_len, int r_len)
{
/* July 2002 V2.2.1 change: use MPI_Alloc_mem */
#if ( defined EXT || defined MPIIO )
  MPI_Aint slen = (MPI_Aint)(max(1,s_len));
  MPI_Aint rlen = (MPI_Aint)(max(1,r_len));
  int ierr;
#else
  s_len=max(1,s_len);
  r_len=max(1,r_len);
#endif
  if( c_info->s_alloc < s_len )
         {
         if( c_info->s_alloc > 0 ) 
/* July 2002 V2.2.1 change: use MPI_Alloc_mem */
#if ( defined EXT || defined MPIIO )
                { MPI_Free_mem(c_info->s_buffer); }

         ierr=MPI_Alloc_mem(slen, MPI_INFO_NULL, &c_info->s_buffer);
         MPI_ERRHAND(ierr);
#else
                { free(c_info->s_buffer); }

         c_info->s_buffer = v_alloc(s_len,where);
#endif

         c_info->s_alloc = s_len;
         c_info->s_data = (assign_type*)c_info->s_buffer;
         }
  if( c_info->r_alloc < r_len )
         {
         if( c_info->r_alloc > 0 ) 
/* July 2002 V2.2.1 change: use MPI_Alloc_mem */
#if ( defined EXT || defined MPIIO )
                { MPI_Free_mem(c_info->r_buffer); }

         ierr=MPI_Alloc_mem(rlen, MPI_INFO_NULL, &c_info->r_buffer);
         MPI_ERRHAND(ierr);
#else
                { free(c_info->r_buffer); }

         c_info->r_buffer = v_alloc(r_len,where);
#endif

         c_info->r_alloc = r_len;
         c_info->r_data = (assign_type*)c_info->r_buffer;
         }

}



/******* Implementation alloc_aux *******/

void alloc_aux(int L, char* where) 
{
L=max(asize,L);
if( AUX_LEN < L)
  {
  if( AUX_LEN>0 ) free(AUX);

  AUX = v_alloc(L, where);
  AUX_LEN=L;
  }
}



/******* Implementation free_aux *******/

void free_aux()
{
if (AUX_LEN > 0 ) {free(AUX); AUX_LEN=0; }
}



/******* Implementation ass_buf *******/

void ass_buf( void* buf, int rank, int pos1, int pos2, int value)
{

if( pos2>= pos1 )
{
int a_pos1, a_pos2, i, j;
a_pos1 =  pos1/asize;
if( pos2>=pos1 )
a_pos2 =  pos2/asize;
else
a_pos2 =  a_pos1-1;

if( value )
for ( i=a_pos1,j=0 ; i<=a_pos2; i++,j++ )
((assign_type *)buf)[j] = BUF_VALUE(rank,i);

else
for ( i=a_pos1,j=0 ; i<=a_pos2; i++,j++ )
((assign_type *)buf)[j] = 0.;

if( a_pos1*asize != pos1 )
  {
  void* xx = (void*)(((char*)buf)+pos1-a_pos1*asize);
  memmove(buf,xx,pos2-pos1+1); 
  }
}

}



/******* Implementation set_buf *******/

void set_buf(struct comm_info* c_info, int selected_rank,
                                       int s_pos1, int s_pos2,
                                       int r_pos1, int r_pos2)
{
/*
Sets c_info->s_buffer/c_info->r_buffer int byte positions 
s_pos1..s_pos2/r_pos1..r_pos2
Values are taken for "selected_rank"
Checks right allocation.
*/
int s_len, r_len;

s_len = (max(s_pos2-s_pos1,0)/asize+1)*asize;
r_len = (max(r_pos2-r_pos1,0)/asize+1)*asize;

alloc_buf(c_info, "set_buf 1",s_len, r_len);


if( s_pos2 >= s_pos1 ) 
  ass_buf( c_info->s_buffer, selected_rank, s_pos1, s_pos2, 1);
if( r_pos2 >= r_pos1 ) 
  ass_buf( c_info->r_buffer, selected_rank, r_pos1, r_pos2, 0);

}


/******* Implementation Init_Pointers *******/

void Init_Pointers(struct comm_info *c_info )
{
/********************************************************************


---------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
---------------------------------------------------------------------
In/Out     : c_info   | struct comm_info* | see comm_info.h 
                      |                   | Pointers initialized
----------------------------------------------------------------------*/

MPI_Comm_size(MPI_COMM_WORLD,&c_info->w_num_procs);
MPI_Comm_rank(MPI_COMM_WORLD,&c_info->w_rank     );

c_info->s_data_type   = MPI_BYTE;  /* DATA TYPE of SEND    BUFFER    */ 
c_info->r_data_type   = MPI_BYTE;  /* DATA TYPE of RECEIVE BUFFER    */

c_info->op_type       = MPI_SUM;   /* OPERATION TYPE IN Allred       */
c_info->red_data_type = MPI_FLOAT; /* NOTE: NO 'CAST' CHECK IN. IBUF */

c_info -> s_buffer = c_info -> r_buffer = NULL;
c_info -> s_data   = c_info -> r_data   = NULL;
c_info -> s_alloc  = c_info -> r_alloc = 0;

c_info->communicator= MPI_COMM_NULL;

/* Auxiliary space */
i_alloc(&c_info->g_ranks,c_info->w_num_procs,"Init_Pointers 1");
i_alloc(&c_info->g_sizes,c_info->w_num_procs,"Init_Pointers 2");

i_alloc(&c_info->reccnt,c_info->w_num_procs,"Init_Pointers 3");
i_alloc(&c_info->displs,c_info->w_num_procs,"Init_Pointers 4");

#ifdef MPIIO
c_info->filename = c_info->datarep = (char*)NULL;
c_info->view = MPI_DATATYPE_NULL;
c_info->info = MPI_INFO_NULL;
c_info->fh = MPI_FILE_NULL;
#endif

all_times = NULL;
#ifdef CHECK
all_defect = NULL;
#endif

Init_Errhand(c_info);

}


/**********************************************************************/


/******* Implementation Init_Buffers *******/

void Init_Buffers(struct comm_info* c_info, struct Bench* Bmark, int size)
/********************************************************************


             VARIABLE |       TYPE        |   MEANING
--------------------------------------------------------------------
Input      : Bmark    | struct Bench*     | Benchmark to run
             size     | int               | size of part to be initialized
Output     :          |                   |  
In/Out     : c_info   |struct comm_info*  | see comm_info.c
                      |                   | Comm. buffers initialized
---------------------------------------------------------------------*/
{
  int s_len, r_len, maxlen;
  int init_size;

maxlen = 1<<MAXMSGLOG;
#ifdef MPIIO
  init_size = size;
#endif
#ifdef EXT
  init_size = max(maxlen,OVERALL_VOL);
  if( OVERALL_VOL/MSGSPERSAMPLE > maxlen ) init_size = maxlen*MSGSPERSAMPLE;
#endif
#ifdef MPI1
  init_size = maxlen;
#endif


  if(c_info->rank < 0 ) return;

  if(!strcmp(Bmark->name,"Alltoall") )
    {
      s_len = c_info->num_procs*init_size;
      r_len = c_info->num_procs*init_size;
    }
  else if( !strcmp(Bmark->name,"Allgather") || !strcmp(Bmark->name,"Allgatherv") )
    {
      s_len = init_size;
      r_len = c_info->num_procs*init_size;
    }
  else
      s_len = r_len = init_size;

  set_buf(c_info, c_info->rank, 0, s_len-1, 0, r_len-1);

}

/********************************************************************/


/******* Implementation free_all *******/

void free_all(struct comm_info* c_info, struct Bench** P_BList)
/********************************************************************

---------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
---------------------------------------------------------------------
In/Out     : c_info   | struct comm_info* | see comm_info.h 
                      |                   | contained pointers freed
             P_BList  | struct Bench**    | cotnained pointers freed
                      |                   | 
----------------------------------------------------------------------*/
{
  del_s_buf(c_info);
  del_r_buf(c_info);

  free(c_info->g_sizes);
  free(c_info->g_ranks);

  free(c_info->reccnt); 
  free(c_info->displs);

  if( c_info->communicator != MPI_COMM_NULL && 
      c_info->communicator != MPI_COMM_SELF &&
      c_info->communicator != MPI_COMM_WORLD )
  {
  Del_Errhand(c_info);
  MPI_Comm_free(&c_info->communicator);
  }

  Destruct_BList(P_BList);

#ifdef MPIIO
  Free_File(c_info);
#endif
  if( all_times ) {free (all_times); all_times=(double*)NULL;}

#ifdef CHECK
  if( all_defect ) {free (all_defect);all_defect=(double*)NULL;}

  if( AUX_LEN > 0 ) {free(AUX); AUX_LEN = 0;}
#endif

#ifdef DEBUG
fclose(dbg_file);
#endif
}

/******* Implementation del_s_buf *******/

void del_s_buf( struct comm_info* c_info )
{
/* July 2002 V2.2.1 change: use MPI_Free_mem */
if ( c_info->s_alloc> 0)
#if (defined EXT || defined MPIIO)
 MPI_Free_mem( c_info->s_buffer );
#else
 free( c_info->s_buffer );
#endif

c_info-> s_alloc = 0;
}



/******* Implementation del_r_buf *******/

void del_r_buf( struct comm_info* c_info )
{
/* July 2002 V2.2.1 change: use MPI_Free_mem */
if ( c_info->r_alloc> 0)
#if (defined EXT || defined MPIIO)
 MPI_Free_mem( c_info->r_buffer );
#else
 free( c_info->r_buffer );
#endif

c_info-> r_alloc = 0;
}

