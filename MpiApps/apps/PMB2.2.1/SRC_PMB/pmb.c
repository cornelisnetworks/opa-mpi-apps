
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
 *        2.2 change: function set_default called (variable initialization)  * 
 *  Revision: 2.2.1                                                          *
 *  Date: 2002/07/04                                                         *
 *        Restrict #iterations for certain cases                             *
 *        (see comment "July 2002 fix V2.2.1:")                              * 
 *                                                                           *
 *                                                                           *
 *****************************************************************************/




#include "mpi.h"

#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype main (this file)  */

int main(
     int argc, char **argv);


/*** PROTOTYPES (extern)   ***/

/* Prototype Init_Buffers, File: Mem_Manager.c */

void Init_Buffers(
     struct comm_info* c_info, struct Bench* Bmark, int size
     );

/* Prototype Err_Hand, File: Err_Handler.c */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/* Prototype set_default, File: pmb_init.c */
void set_default(struct comm_info* c_info);

/* Prototype Init_Communicator, File: pmb_init.c */

int Init_Communicator(
     struct comm_info* c_info, int NP);

/* Prototype Init_Pointers, File: Mem_Manager.c */

void Init_Pointers(
     struct comm_info *c_info );

/* Prototype valid, File: pmb_init.c */

int valid(
     struct comm_info * c_info, struct Bench* Bmark, int NP
     );

/* Prototype Output, File: Output.c */

void Output(
     struct comm_info* c_info, struct Bench* Bmark, MODES BMODE, 
     int tmp_NP, int header, int size, 
     int n_sample, double *time);

/* Prototype Basic_Input, File: pmb_init.c */

void Basic_Input(
     struct comm_info* c_info, struct Bench** P_BList, int *argc, 
     char ***argv, int* NP_min);

/* Prototype Init_File, File: Init_File.c */

int Init_File(
     struct comm_info* c_info, struct Bench* Bmark, int NP
     );

/* Prototype Init_Transfer, File: Init_Transfer.c */

void Init_Transfer(
     struct comm_info* c_info, struct Bench* Bmark, int size
     );

/* Prototype Show_Selections, File: Output.c */

void Show_Selections(
     struct comm_info* c_info, struct Bench* BList);

/* Prototype free_all, File: Mem_Manager.c */

void free_all(
     struct comm_info* c_info, struct Bench** P_BList);

/* Prototype Close_Transfer, File: Init_Transfer.c */

void Close_Transfer (
     struct comm_info* c_info, struct Bench* Bmark, int size
     );

/* Prototype Warm_Up, File: Warm_Up.c */

void Warm_Up (
     struct comm_info* c_info, struct Bench* Bmark, int iter
     );

/* Prototype CPU_Exploit, File: CPU_Exploit.c */

void CPU_Exploit(float target_secs, int initialize);

/* Prototype end_msg, File: g_info.c */

void end_msg();

/******************************/
/*       END PROTOTYPES       */


/**********************************************************************/


/******* Implementation main *******/

int main(int argc, char **argv)
/**********************************************************************/
{
struct comm_info C_INFO;   /* BASIC SETTINGS STRUCTURE    */
struct Bench* BList;       /* List of Benchmarks          */

struct Bench* Bmark;

#ifdef CHECK
int NFAIL, NSUCCESS;
#endif

char *p;
int  i,j,iter,imod,ierr;
int  NP,NP_min;
int  do_it  ; 

int   header;              
int     size;             
int   MAXMSG;
int x_sample,n_sample;            
MODES BMODE;            
double  time[MAX_TIMINGS]; 

Type_Size unit_size;      

if( (ierr=MPI_Init(&argc,&argv))!=MPI_SUCCESS) Err_Hand(1, ierr);

set_default(&C_INFO);

Init_Pointers(&C_INFO);

Basic_Input(&C_INFO,&BList,&argc,&argv,&NP_min); 

Show_Selections(&C_INFO,BList);

/* LOOP OVER INDIVIDUAL BENCHMARKS */
j=0;
while( (p=BList[j].name) )
  {
 Bmark = BList+j;
 if( Bmark->RUN_MODES[0].type != BTYPE_INVALID )
  {

    NP=max(1,min(C_INFO.w_num_procs,NP_min));
    if( Bmark->RUN_MODES[0].type == SingleTransfer )
     {
#ifndef MPIIO
      NP = (min(2,C_INFO.w_num_procs));
#else
      NP = 1;
#endif
     }

    do_it = 1;
/* LOOP OVER PROCESS NUMBERS */
    while ( do_it )       
      {
        if ( valid(&C_INFO,Bmark,NP) )
        {

	if(Init_Communicator(&C_INFO,NP)!=0) Err_Hand(0,-1);

#ifdef MPIIO
	if(Init_File(&C_INFO,Bmark,NP)!=0) Err_Hand(0,-1);
#endif

	/* MINIMAL OUTPUT IF UNIT IS GIVEN */
	if (C_INFO.w_rank==0)
	  {
	    if(unit != stdout) 
	      printf
              ("# Running %s; see file \"%s\" for results\n",p,OUTPUT_FILENAME);
	  };
	header=1;

	MAXMSG=1<<MAXMSGLOG;

#ifdef EXT
        MPI_Type_size(C_INFO.red_data_type,&unit_size);
#else
        
	if (Bmark->reduction)
               MPI_Type_size(C_INFO.red_data_type,&unit_size);
	else   MPI_Type_size(C_INFO.s_data_type,&unit_size);
#endif

	MAXMSG=(1<<MAXMSGLOG)/unit_size* unit_size;
	

        for( imod=0; imod < Bmark->N_Modes; imod++ )
        {

        BMODE=&Bmark->RUN_MODES[imod];

        x_sample = BMODE->AGGREGATE ? MSGSPERSAMPLE : MSGS_NONAGGR;
/* July 2002 fix V2.2.1: */
#if (defined EXT || defined MPIIO)
        if( Bmark->access==no ) x_sample=MSGS_NONAGGR;
#endif

	header=header | (imod > 0);
	
        iter=0;
	size =0;


/* LOOP OVER MESSAGE LENGTHS */
        while (size < MAXMSG)
	      {  
		if( iter == 0 ) 
		  {
		    size     = 0;
		    n_sample = x_sample ;
		  }
		else if ( iter == 1 ) 
		  {  
		    size = ((1<<MINMSGLOG) + unit_size - 1)/unit_size*unit_size;
#ifdef EXT
                    if( size < asize ) size = asize;
#endif
		    n_sample =max(1,min(OVERALL_VOL/size,x_sample));
		  }
		else
		  { 
		    size     = min(MAXMSG,size+size);
		    n_sample = max(1,min(OVERALL_VOL/size,x_sample));
		  }

                if( Bmark->RUN_MODES[0].type == Sync ) size=MAXMSG;

                for(i=0; i<MAX_TIMINGS; i++) time[i] = 0.;

                Init_Buffers(&C_INFO, Bmark, size);

	        Init_Transfer(&C_INFO, Bmark, size);

		Warm_Up  (&C_INFO,Bmark,iter);                    

		Bmark->Benchmark(&C_INFO,size,n_sample,BMODE,time);

                /* Synchronization, in particular for idle processes
                   which have to wait in a well defined manner */
                MPI_Barrier(MPI_COMM_WORLD);
                     
	  	Output   (&C_INFO,Bmark,BMODE,NP,header,size,n_sample,time);

                Close_Transfer(&C_INFO, Bmark, size);

                CHK_BRK;
		
		header = 0;
		iter++;
	      }

        if( !Bmark->success && C_INFO.w_rank == 0 )
          fprintf(unit,"\n\n!!! Benchmark unsuccessful !!!\n\n");

        CHK_BRK;
        }

        CHK_BRK;
        }

	/* CALCULATE THE NUMBER OF PROCESSES FOR NEXT STEP */
	if( NP == C_INFO.w_num_procs  ) {do_it=0;}
	else
	  {
	    NP=min(NP+NP,C_INFO.w_num_procs);
	  }	  
#ifdef MPIIO
          if( Bmark->RUN_MODES[0].type == SingleTransfer ) do_it = 0;
#endif
	
      CHK_BRK;
      }

  }
  CHK_BRK;
  j++;
  }
#ifdef CHECK

if( C_INFO.w_rank == 0 )
{
j=0;
NFAIL=0;
NSUCCESS=0;
while( (p=BList[j].name) )
{
 Bmark = BList+j;
 if( Bmark->RUN_MODES[0].type != BTYPE_INVALID )
  {
   Bmark = BList+j;
   if( !Bmark->success ) NFAIL ++;
   else                  NSUCCESS++;
  } 
j++;
}
    
if( NFAIL == 0 && NSUCCESS>0 )
{
fprintf(unit,"\n\n!!!!  ALL BENCHMARKS SUCCESSFUL !!!! \n\n");
}
else if( NSUCCESS>0 )
{
if( NFAIL == 1 )
fprintf(unit,"\n\n!!!!  %d  BENCHMARK FAILED     !!!! \n\n",NFAIL);
else
fprintf(unit,"\n\n!!!!  %d  BENCHMARKS FAILED     !!!! \n\n",NFAIL);

j=0;
while( (p=BList[j].name) )
{
 Bmark = BList+j;
 if( Bmark->RUN_MODES[0].type != BTYPE_INVALID )
  {
   if( Bmark->success ) fprintf(unit,"%s    : Successful\n",p);
   else                 fprintf(unit,"%s    : FAILED !! \n",p);
  } 
j++;
}
}
}

#endif

end_msg(&C_INFO);

free_all(&C_INFO, &BList);


MPI_Finalize();

return 0;
}
