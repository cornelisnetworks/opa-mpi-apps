
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



#define nSET_ERRH

#include "declare.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Err_Hand (this file)  */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/* Prototype Errors_MPI (this file)  */

void Errors_MPI(
     MPI_Comm * comm, int* ierr,...);

#ifdef EXT
/* Prototype Errors_WIN (this file)  */

void Errors_WIN(
     MPI_Win * WIN, int* ierr,...);
#endif

#ifdef MPIIO
/* Prototype Errors_IO  (this file)  */

void Errors_IO (
     MPI_File * fh  , int* ierr,...);
#endif

/* Prototype Init_Errhand (this file)  */

void Init_Errhand(
     struct comm_info* c_info);

/* Prototype Set_Errhand (this file)  */

void Set_Errhand(
     struct comm_info* c_info);

/* Prototype Del_Errhand (this file)  */

void Del_Errhand(
     struct comm_info* c_info);


/******************************/
/*       END PROTOTYPES       */

/*-------------------------------------------------------------*/


/******* Implementation Err_Hand *******/

void Err_Hand( int ERR_IS_MPI, int ERR_CODE )
/*-------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
-------------------------------------------------------------------
Input      :ERR_IS_MPI| int               | flag if MPI error occured
	    ERR_CODE  | int               | error code
                      |                   |
Output     :          |                   |
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
-------------------------------------------------------------------
-------------------------------------------------------------------
Description: Tiny error handling function
-----------------------------------------------------------------*/
{
  int i_break;
  
  if(! ERR_IS_MPI )
    {
      i_break=1;
      fprintf(stderr,"Application error code %d occurred\n",ERR_CODE);
      switch(ERR_CODE)
	{
	case APPL_ERR_INVCASE: fprintf(stderr,"INVALID benchmark name\n");
	  break;
	case APPL_ERR_MALLOC : fprintf(stderr,"Malloc failed \n");
	  break;
	case APPL_ERR_CINFO  : fprintf(stderr,"Invalid \"c_info\" setup\n");
	  break;
	}
      ERR_CODE=MPI_ERR_INTERN;
    }
  else if ( ERR_CODE != MPI_SUCCESS )
    {
      char aux_string[MPI_MAX_ERROR_STRING];
      int L;
      i_break=1;
      fprintf(stderr,"MPI error  %d occurred\n",ERR_CODE);
      MPI_Error_string(ERR_CODE,aux_string, &L);
      fprintf(stderr,"%s\n",aux_string);
    }
  else i_break=0;
  
  if(i_break )
    { 
      MPI_Abort(MPI_COMM_WORLD, ERR_CODE);
    }
}



/******* Implementation Errors_MPI *******/

void Errors_MPI(MPI_Comm * comm, int* ierr,...)
{
Err_Hand(1,*ierr);
}

#ifdef EXT
/******* Implementation Errors_WIN *******/

void Errors_WIN(MPI_Win * WIN, int* ierr,...)
{
Err_Hand(1,*ierr);
}
#endif

#ifdef MPIIO
/******* Implementation Errors_IO  *******/

void Errors_IO (MPI_File * fh, int* ierr,...)
{
Err_Hand(1,*ierr);
}
#endif



/******* Implementation Init_Errhand *******/

void Init_Errhand(struct comm_info* c_info)
{
#ifdef SET_ERRH

MPI_Errhandler_create(Errors_MPI,&c_info->ERR);

#ifdef EXT
MPI_Win_create_errhandler(Errors_WIN, &c_info->ERRW);
#endif

#ifdef MPIIO
MPI_File_create_errhandler(Errors_IO, &c_info->ERRF);
#endif

#endif
}



/******* Implementation Set_Errhand *******/

void Set_Errhand(struct comm_info* c_info)
{
#ifdef SET_ERRH

if( c_info->communicator != MPI_COMM_NULL )
MPI_Errhandler_set(c_info->communicator, c_info->ERR);

#ifdef EXT
if( c_info->WIN != MPI_WIN_NULL )
MPI_Win_set_errhandler(c_info->WIN, c_info->ERRW);
#endif

#ifdef MPIIO
if( c_info->fh != MPI_FILE_NULL )
MPI_File_set_errhandler(c_info->fh, c_info->ERRF);
#endif

#endif
}



/******* Implementation Del_Errhand *******/

void Del_Errhand(struct comm_info* c_info)
{
#ifdef SET_ERRH
MPI_Errhandler_free(&c_info->ERR);

#ifdef EXT
MPI_Errhandler_free(&c_info->ERRW);
#endif

#ifdef MPIIO
MPI_Errhandler_free(&c_info->ERRF);
#endif

#endif
}
