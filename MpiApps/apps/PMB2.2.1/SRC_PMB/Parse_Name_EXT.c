
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




#include <string.h>

#include "declare.h"
#include "Benchmark.h"

#include "Bnames_EXT.h"
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

/* Prototype Bidir_Put, File: OneS_bidir.c */

void Bidir_Put(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Unidir_Put, File: OneS_unidir.c */

void Unidir_Put (
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Accumulate, File: OneS_accu.c */

void Accumulate (
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Bidir_Get, File: OneS_bidir.c */

void Bidir_Get(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Unidir_Get, File: OneS_unidir.c */

void Unidir_Get (
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Window, File: Window.c */

void Window (
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

Bmark->RUN_MODES[0].AGGREGATE=1;
Bmark->RUN_MODES[1].AGGREGATE=0;

Bmark->RUN_MODES[0].NONBLOCKING =0;
Bmark->RUN_MODES[1].NONBLOCKING =0;

Bmark->N_Modes = 2;

Bmark->reduction = 0;

Bmark->Ntimes = 1;

 if (!strcmp(Bmark->name,"unidir_get"))
	{ 
          strcpy(Bmark->name,"Unidir_Get");
          Bmark->Benchmark = Unidir_Get;
          Bmark->bench_comments = &Unidir_Get_cmt[0];
          type = SingleTransfer;
          Bmark->access = get;
	}
 else if (!strcmp(Bmark->name,"unidir_put"))
	{ 
          strcpy(Bmark->name,"Unidir_Put");
          Bmark->Benchmark = Unidir_Put;
          Bmark->bench_comments = &Unidir_Put_cmt[0];
          type = SingleTransfer;
          Bmark->access = put;
	}
 else if (!strcmp(Bmark->name,"bidir_get"))
	{ 
          strcpy(Bmark->name,"Bidir_Get");
          Bmark->Benchmark = Bidir_Get;
          Bmark->bench_comments = &Bidir_Get_cmt[0];
          type = SingleTransfer;
          Bmark->access = get;
	}
 else if (!strcmp(Bmark->name,"bidir_put"))
	{ 
          strcpy(Bmark->name,"Bidir_Put");
          Bmark->Benchmark = Bidir_Put;
          Bmark->bench_comments = &Bidir_Put_cmt[0];
          type = SingleTransfer;
          Bmark->access = put;
	}
 else if (!strcmp(Bmark->name,"accumulate"))
	{ 
          strcpy(Bmark->name,"Accumulate");
          Bmark->Benchmark = Accumulate;
          Bmark->bench_comments = &Accumulate_cmt[0];
          type = Collective;
          Bmark->access = put;
          Bmark->reduction = 1;
	}
 else if (!strcmp(Bmark->name,"window"))
	{ 
          strcpy(Bmark->name,"Window");
          Bmark->Benchmark = Window;
          Bmark->bench_comments = &Window_cmt[0];
          Bmark->RUN_MODES[0].AGGREGATE=-1;
          Bmark->N_Modes = 1;
          type = Collective;
          Bmark->access = no;
          Bmark->reduction = 1;
	}
 else 
        {
          type = BTYPE_INVALID;
        }
Bmark->RUN_MODES[0].type=type;
Bmark->RUN_MODES[1].type=type;

}
