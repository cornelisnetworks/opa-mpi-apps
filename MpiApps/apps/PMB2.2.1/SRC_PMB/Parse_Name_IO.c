
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

#include "Bnames_IO.h"
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

/* Prototype Write_Shared, File: Write.c */

void Write_Shared(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Read_Shared, File: Read.c */

void Read_Shared(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Write_Expl, File: Write.c */

void Write_Expl(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Read_Expl, File: Read.c */

void Read_Expl(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Write_Indv, File: Write.c */

void Write_Indv(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Read_Indv, File: Read.c */

void Read_Indv(
     struct comm_info* c_info, int size, int n_sample, 
     MODES RUN_MODE, double* time);

/* Prototype Open_Close, File: Open_Close.c */

void Open_Close(
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
char *tmp_name;
int md, nam_beg;
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


Bmark->reduction=0;

Bmark->Ntimes = 1;
md = 0;
nam_beg = 2;

if( Bmark->name[0] == 's' )
  {
  type = SingleTransfer;
  Bmark->name[0] = 'S';
  if( Bmark->name[2] == 'i' )
     {md = 1; nam_beg++;Bmark->name[2]='I';}
  }
if( Bmark->name[0] == 'p' )
  {
  type = ParallelTransfer;
  Bmark->name[0] = 'P';
  if( Bmark->name[2] == 'i' )
     {md = 1; nam_beg++;Bmark->name[2]='I';}
  }
if( Bmark->name[0] == 'c' )
  {
  type = Collective;
  Bmark->name[0] = 'C';
  if( Bmark->name[2] == 'i' )
     {md = 1; nam_beg++;Bmark->name[2]='I';}
  }

 if( md ) do_nonblocking=1;
 Bmark->Ntimes += md;

 Bmark->RUN_MODES[0].NONBLOCKING =md;
 Bmark->RUN_MODES[1].NONBLOCKING =md;

 tmp_name=Bmark->name+nam_beg;

 if (!strcmp(tmp_name,"write_indv"))
	{ 
          strcpy(tmp_name,"Write_Indv");
          Bmark->Benchmark = Write_Indv;
          Bmark->bench_comments = &Write_Indv_cmt[0];

          Bmark->access = put;
          Bmark->fpointer = indv_block;
	}
 else if (!strcmp(tmp_name,"write_shared"))
	{ 
          strcpy(tmp_name,"Write_Shared");
          Bmark->Benchmark = Write_Shared;
          Bmark->bench_comments = &Write_Shared_cmt[0];

          Bmark->access = put;
          Bmark->fpointer = shared;
	}
 else if (!strcmp(tmp_name,"write_priv"))
	{ 
          strcpy(tmp_name,"Write_Priv");
          Bmark->Benchmark = Write_Indv;
          Bmark->bench_comments = &Write_Priv_cmt[0];

          Bmark->access = put;
          Bmark->fpointer = private;
	}
 else if (!strcmp(tmp_name,"write_expl"))
	{ 
          strcpy(tmp_name,"Write_Expl");
          Bmark->Benchmark = Write_Expl;
          Bmark->bench_comments = &Write_Expl_cmt[0];

          Bmark->access = put;
          Bmark->fpointer = explicit;
	}
 else if (!strcmp(tmp_name,"read_indv"))
	{ 
          strcpy(tmp_name,"Read_Indv");
          Bmark->Benchmark = Read_Indv;
          Bmark->bench_comments = &Read_Indv_cmt[0];

          Bmark->access = get;
          Bmark->fpointer = indv_block;
	}
 else if (!strcmp(tmp_name,"read_shared"))
	{ 
          strcpy(tmp_name,"Read_Shared");
          Bmark->Benchmark = Read_Shared;
          Bmark->bench_comments = &Read_Shared_cmt[0];

          Bmark->access = get;
          Bmark->fpointer = shared;
	}
 else if (!strcmp(tmp_name,"read_priv"))
	{ 
          strcpy(tmp_name,"Read_Priv");
          Bmark->Benchmark = Read_Indv;
          Bmark->bench_comments = &Read_Priv_cmt[0];

          Bmark->access = get;
          Bmark->fpointer = private;
	}
 else if (!strcmp(tmp_name,"read_expl"))
	 {
          strcpy(tmp_name,"Read_Expl");
          Bmark->Benchmark = Read_Expl;
          Bmark->bench_comments = &Read_Expl_cmt[0];

          Bmark->access = get;
          Bmark->fpointer = explicit;
	}
 else if (!strcmp(Bmark->name,"open_close"))
	 {
          strcpy(Bmark->name,"Open_Close");
          Bmark->Benchmark = Open_Close;
          Bmark->bench_comments = &Open_cmt[0];

          type = Sync;
          Bmark->RUN_MODES[0].AGGREGATE=-1;

          Bmark->access = no;
          Bmark->fpointer = explicit;
	}
 else 
        {
          type = BTYPE_INVALID;
        }

if( Bmark->access == no ) Bmark->Ntimes = 1;

Bmark->RUN_MODES[0].type=type;
Bmark->RUN_MODES[1].type=type;

if( type == SingleTransfer ) Bmark->fpointer = private;

if( Bmark->access == get || Bmark->access == no  ||
    md )
   Bmark->N_Modes = 1;
else
   Bmark->N_Modes = 2;

}

