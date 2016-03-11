
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
 *        2.2 change:  string VERSION introduce                              *
 *                     function end_msg() added                              *
 *                                                                           *
 *****************************************************************************/


char* VERSION="2.2";

#include <stdio.h>
#include <time.h>

#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype General_Info (this file)  */

void General_Info(
     );

/* Prototype make_sys_info (this file)  */

void make_sys_info(
     );

/* Prototype end_msg (this file)  */

void end_msg(
     );


/******************************/
/*       END PROTOTYPES       */

extern FILE* unit;



/******* Implementation General_Info *******/

void General_Info()
{
  void make_sys_info();
  time_t T;
  
  time(&T);
  fprintf(unit,"#---------------------------------------------------\n");

#ifdef MPI1                 
  fprintf(unit,"#    PALLAS MPI Benchmark Suite V%s, MPI-1 part    \n",VERSION);
#elif defined EXT
  fprintf(unit,"#    PALLAS MPI Benchmark Suite V%s, MPI-2 part    \n",VERSION);
#elif defined MPIIO
  fprintf(unit,"#    PALLAS MPI Benchmark Suite V%s, MPI-IO part   \n",VERSION);
#endif


  fprintf(unit,"#---------------------------------------------------\n");
  fprintf(unit,"# Date       : %s",asctime(localtime(&T)));

  make_sys_info();
  fprintf(unit,"\n");
}

#include <sys/utsname.h>


/******* Implementation make_sys_info *******/

void   make_sys_info()
{
  struct utsname info;
  uname( &info );
  
  fprintf(unit,"# Machine    : %s"  ,info.machine);
  fprintf(unit,"# System     : %s\n",info.sysname);
  fprintf(unit,"# Release    : %s\n",info.release);
  fprintf(unit,"# Version    : %s\n",info.version);
}

void end_msg( struct comm_info* c_info )
{

FILE* u;

if( c_info->w_rank == 0 ) {

for( u=stdout; u; u=(u==unit ? NULL : unit) )
{

fprintf(u,"#=====================================================\n");
fprintf(u,"#\n");
fprintf(u,"#  Thanks for using PMB%s               \n",VERSION);
fprintf(u,"#\n");
fprintf(u,"#  The Pallas team kindly requests that you \n");      
fprintf(u,"#  give us as much feedback for PMB as possible.    \n");      
fprintf(u,"#\n");
fprintf(u,"#  It would be very helpful when you sent the \n");      
fprintf(u,"#  output tables of your run(s) of PMB to \n");      
fprintf(u,"#\n");
fprintf(u,"#              #######################           \n");
fprintf(u,"#              #                     #           \n");
fprintf(u,"#              #   pmb@pallas.com    #           \n");
fprintf(u,"#              #                     #           \n");
fprintf(u,"#              #######################           \n");
fprintf(u,"#\n");
fprintf(u,"#  You might also add                             \n");      
fprintf(u,"#\n");
fprintf(u,"#  - personal information (institution, motivation\n");      
fprintf(u,"#                          for using PMB)         \n");      
fprintf(u,"#  - basic information about the machine you used \n");  
fprintf(u,"#    (number of CPUs, processor type e.t.c.)      \n");  
fprintf(u,"#\n");
fprintf(u,"#=====================================================\n");

}


}

}
