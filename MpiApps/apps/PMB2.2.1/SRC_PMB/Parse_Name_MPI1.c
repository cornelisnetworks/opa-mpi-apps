
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

#include "Bnames_MPI1.h"
#include "Comments.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Get_Def_Cases (this file)  */

void Get_Def_Cases(
     char*** defc, char*** Gcmt);

/* Prototype Set_Bmark (this file)  */

void Set_Bmark(
     struct Bench* Bmark);


/*** PROTOTYPES (extern)   ***/

/* Prototype Get_Def_Index (file BenchList.c)  */

void Get_Def_Index(  
           int* index, char* name 
                  );

/* Prototype Allreduce, File: Allreduce.c */

void Allreduce(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype PingPong, File: PingPong.c */

void PingPong(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Exchange, File: Exchange.c */

void Exchange(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Allgather, File: Allgather.c */

void Allgather(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Alltoall, File: Alltoall.c */

void Alltoall(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Barrier, File: Barrier.c */

void Barrier(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Reduce, File: Reduce.c */

void Reduce(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Allgatherv, File: Allgatherv.c */

void Allgatherv(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Sendrecv, File: Sendrecv.c */

void Sendrecv(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype PingPing, File: PingPing.c */

void PingPing(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Bcast, File: Bcast.c */

void Bcast(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Reduce_scatter, File: Reduce_scatter.c */

void Reduce_scatter(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype str, File: strgs.c */

char* str(
     char* Bname);

/* Prototype LWR, File: strgs.c */

void LWR(
     char* Bname);


/******************************/
/*       END PROTOTYPES       */

 


/******* Implementation Get_Def_Cases *******/

void Get_Def_Cases(char*** defc, char*** Gcmt)
{
*defc = &DEFC[0];
*Gcmt = &Gral_cmt[0];
}



/******* Implementation Set_Bmark *******/

void Set_Bmark(struct Bench* Bmark)
{
BTYPES type;
int index;

Get_Def_Index(&index,Bmark->name);

if( index < 0 )
{
Bmark->RUN_MODES[0].type=BTYPE_INVALID;
Bmark->RUN_MODES[1].type=BTYPE_INVALID;
return;
}

 Bmark->N_Modes = 1;
 Bmark->RUN_MODES[0].AGGREGATE=-1;
 Bmark->RUN_MODES[0].NONBLOCKING =0;

 Bmark->reduction = 0;

 Bmark->Ntimes = 1;

 if (!strcmp(Bmark->name,"pingpong"))
	{ 
          strcpy(Bmark->name,"PingPong");
          Bmark->Benchmark = PingPong;
          Bmark->bench_comments = &PingPong_cmt[0];
          type = SingleTransfer;
          Bmark->scale_time = 0.5;
	}
 else if (!strcmp(Bmark->name,"pingping"))
	{ 
          strcpy(Bmark->name,"PingPing");
          Bmark->Benchmark = PingPing;
          Bmark->bench_comments = &PingPing_cmt[0];
          type = SingleTransfer;
	}
 else if (!strcmp(Bmark->name,"sendrecv"))
	{ 
          strcpy(Bmark->name,"Sendrecv");
          Bmark->Benchmark = Sendrecv;
          Bmark->bench_comments = &Sendrecv_cmt[0];
          type = ParallelTransfer;
          Bmark->scale_time = 1.0;
          Bmark->scale_bw   = 2.0;
	}
 else if (!strcmp(Bmark->name,"exchange") )
	{ 
          strcpy(Bmark->name,"Exchange");
          Bmark->Benchmark = Exchange;
          Bmark->bench_comments = &Exchange_cmt[0];
          type = ParallelTransfer;
          Bmark->scale_time = 1.0;
          Bmark->scale_bw   = 4.0;
	}
 else if (!strcmp(Bmark->name,"allreduce"))
	{ 
          strcpy(Bmark->name,"Allreduce");
          Bmark->Benchmark = Allreduce;
          type = Collective;
          Bmark->bench_comments = &Allreduce_cmt[0];
          Bmark->reduction = 1;
	}
 else if (!strcmp(Bmark->name,"reduce"))
	{ 
          strcpy(Bmark->name,"Reduce");
          Bmark->Benchmark = Reduce;
          type = Collective;
          Bmark->bench_comments = &Reduce_cmt[0];
          Bmark->reduction = 1;
	}
 else if (!strcmp(Bmark->name,"reduce_scatter"))
	{ 
          strcpy(Bmark->name,"Reduce_scatter");
          Bmark->Benchmark = Reduce_scatter;
          type = Collective;
          Bmark->bench_comments = &Reduce_scatter_cmt[0];
          Bmark->reduction = 1;
	}
 else if (!strcmp(Bmark->name,"bcast"))
	{ 
          strcpy(Bmark->name,"Bcast");
          Bmark->Benchmark = Bcast;
          type = Collective;
          Bmark->bench_comments = &Bcast_cmt[0];
	}
 else if (!strcmp(Bmark->name,"barrier"))
	{ 
          strcpy(Bmark->name,"Barrier");
          Bmark->Benchmark = Barrier;
          type = Sync;
          Bmark->bench_comments = &Barrier_cmt[0];
	}
 else if (!strcmp(Bmark->name,"allgather"))
	{ 
          strcpy(Bmark->name,"Allgather");
          Bmark->Benchmark = Allgather;
          type = Collective;
          Bmark->bench_comments = &Allgather_cmt[0];
	}
 else if (!strcmp(Bmark->name,"alltoall"))
	{ 
          strcpy(Bmark->name,"Alltoall");
          Bmark->Benchmark = Alltoall;
          type = Collective;
          Bmark->bench_comments = &Alltoall_cmt[0];
	}
 else if (!strcmp(Bmark->name,"allgatherv"))
	{ 
          strcpy(Bmark->name,"Allgatherv");
          Bmark->Benchmark = Allgatherv;
          type = Collective;
          Bmark->bench_comments = &Allgatherv_cmt[0];
	}
 else 
        {
          type = BTYPE_INVALID;
        }
Bmark->RUN_MODES[0].type=type;
}

