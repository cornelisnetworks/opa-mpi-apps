
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




#include "mpi.h"

#include "declare.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype User_Set_Info (this file)  */

void User_Set_Info(
     MPI_Info* opt_info);


/******************************/
/*       END PROTOTYPES       */




/******* Implementation User_Set_Info *******/

void User_Set_Info( MPI_Info* opt_info)
{

#ifdef MPIIO

*opt_info = MPI_INFO_NULL;

/*
Set info for all MPI_File_open calls
MPI_Info_create(opt_info);
MPI_Info_set(* opt_info, "striping_factor", "1");
MPI_Info_set(* opt_info, "io_node_list",
                "/u3/pallas/,/u2/pallas/,/u1/pallas/,/u0/pallas/");
*/


/*

MPI_Info_create(opt_info);
MPI_Info_set(* opt_info, "file_system", "mpifs");


MPI_Info_set(* opt_info, "striping_factor", "2");
MPI_Info_set(* opt_info, "striping_unit", "65536");

MPI_Info_set(* opt_info, "io_node_list",
                "/u2/pallas/,/u3/pallas/,/u1/pallas/,/u0/pallas/");
*/

#endif

#ifdef EXT
/*
Set info for all MPI_Win_create calls
*/

*opt_info = MPI_INFO_NULL;

#endif
}
