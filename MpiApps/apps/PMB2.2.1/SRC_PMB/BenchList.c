
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




#include <stdio.h>

#include "declare.h"

#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype List_Names (this file)  */

void List_Names(
     char* Bname, int** List
     );

/* Prototype Get_Def_Index (this file)  */

void Get_Def_Index(  
           int* index, char* name 
                  );

/* Prototype Construct_BList (this file)  */

void Construct_BList(
     struct Bench** P_BList, int n_args, char* name
     );

/* Prototype Destruct_BList (this file)  */

void Destruct_BList(
     struct Bench ** P_BList);

/* Prototype Print_BList (this file)  */

void Print_BList(
     struct comm_info * c_info, struct Bench *BList);


/*** PROTOTYPES (extern)   ***/

/* Prototype Get_Def_Cases (file Parse_Name_xx.c)  */

void Get_Def_Cases(
     char*** defc, char*** Gcmt);


/* Prototype Set_Bmark, File: Parse_Name_MPI1.c */

void Set_Bmark(
     struct Bench* Bmark);

/* Prototype v_alloc, File: Mem_Manager.c */

void* v_alloc(
     int Len, char* where
     );

/* Prototype i_alloc, File: Mem_Manager.c */

void i_alloc(
     int** B, int Len, char* where 
     );

/* Prototype str, File: strgs.c */

char* str(
     char* Bname);

/* Prototype Get_Def_Cases, File: Parse_Name_MPI1.c */

void Get_Def_Cases(
     char*** defc, char*** Gcmt);

/* Prototype LWR, File: strgs.c */

void LWR(
     char* Bname);


/******************************/
/*       END PROTOTYPES       */


char * NIL_COMMENT[] ={NULL};



/******* Implementation List_Names *******/

void List_Names(char* Bname, int** List)
{
char** def_cases, **General_cmt;

Get_Def_Cases(&def_cases,&General_cmt);

if( !strcmp(Bname,"all") )
  {
  int Ndeflt=-1, n;
  while ( def_cases[++Ndeflt] );

  i_alloc(List, Ndeflt+1,"List_Names");
  for ( n=0; n<Ndeflt; n++ )
     (*List)[n] = n;
  (*List)[Ndeflt] = -1;
  }

else
  {
  i_alloc(List, 2, "List_Names");
  Get_Def_Index(*List, Bname);
  (*List)[1]=-1;
  }

}


void Get_Def_Index(int* index, char* name)
{
char** def_cases, **General_cmt;
*index = 0;

Get_Def_Cases(&def_cases,&General_cmt);

while( def_cases[*index] )
   {
   char* TMP1 = str(def_cases[*index]), *TMP2 = str(name);
   LWR(TMP1); LWR(TMP2);
   if(!strcmp(TMP1,TMP2)) 
     {free(TMP1); free(TMP2); break;}
   (*index)++;
   free(TMP1); free(TMP2);
   }

if( ! def_cases[*index] ) *index=-1;
}
      

/******* Implementation Construct_BList *******/

void Construct_BList(struct Bench** P_BList, int n_args, char* name)
 
/**************************************************************************
             VARIABLE |       TYPE        |   MEANING
------------------------------------------------------------
Input      : n_args   | int               | final length of benchmark list
                      |                   | stored in -> *P_Blist
             name     | char*             | name of benchmark to insert
                      |                   | ="ALL": take all default benchmarks
                      |                   |
Output     : P_BList  | struct Bench**    | Pointer to linear list of 
                      |                   | benchmark objects
                      |                   |
                      |                   | First call:
                      |                   | *P_Blist is allocated
                      |                   | current benchmark is inserted
                      |                   |
                      |                   | Following calls:
                      |                   | current benchmark is inserted
                      |                   |
------------------------------------------------------------
**************************************************************************/
 
{
 
 static int Ndeflt = -1 ;
 static int n_cases;
 
 struct Bench* Bmark;
 char* Bname;

 char** def_cases, **General_cmt;

 int* List;
 int plc;
 
 Get_Def_Cases(&def_cases, &General_cmt);
 
 if( Ndeflt < 0 )
   {
  Ndeflt=-1;
  while( def_cases[++Ndeflt] ) ;

  *P_BList = (struct Bench*)v_alloc((1+n_args+Ndeflt)*sizeof(struct Bench), "Construct_Blist 1");
 
  n_cases=0;
   }

 Bname = str(name);
 LWR(Bname);
 
 List_Names(Bname, &List);

 for( plc=0; List[plc]>=0 ; plc++ )
 {
 Bmark = (*P_BList)+n_cases;

 Bmark->name = str(def_cases[List[plc]]);
 LWR(Bmark->name);

 Bmark->bench_comments = &NIL_COMMENT[0];
 Bmark->scale_time = 1.0;
 Bmark->scale_bw   = 1.0;
 Bmark->success    = 1;
 
 Set_Bmark(Bmark);

 if( Bmark->RUN_MODES[0].type == BTYPE_INVALID ) strcpy(Bmark->name,name);
 n_cases++;

 }
 (*P_BList)[n_cases].name=NULL;

 free(Bname);
 free (List);

}




/******* Implementation Destruct_BList *******/

void Destruct_BList(struct Bench ** P_BList)
{
/****************************************************************
Freeing of the Benchmark list
*****************************************************************/
int i;
i=0;
while( (*P_BList)[i].name )
  {
  free ((*P_BList)[i++].name);
  }
free(*P_BList);
}



/******* Implementation Print_BList *******/

void Print_BList(struct comm_info * c_info, struct Bench *BList)
/****************************************************************
Display of the Benchmark list
*****************************************************************/
{
int j, ninvalid;
char*nn,*cmt;
char** def_cases, **General_cmt;

Get_Def_Cases(&def_cases, &General_cmt);

if( General_cmt[0] != NULL )
        {
        fprintf(unit,"# Remarks on the current Version:\n\n");
        j=0;
        while ( (nn=General_cmt[j++]) )
           fprintf(unit,"# %s\n",nn);
        }

j=0; ninvalid=0;

while( BList[j].name )
  {
 if( BList[j].RUN_MODES[0].type == BTYPE_INVALID )
     {
     ninvalid++;
     if( ninvalid==1 )
          fprintf(unit,"\n# Attention, invalid benchmark name(s):\n");
     
          fprintf(unit,"# %s\n",BList[j].name);
          free (BList[j].name);
          BList[j].name = str("");
     }
  j++;
  }


if( ninvalid < j)
{

fprintf(unit,"\n# List of Benchmarks to run:\n\n");

j=0;
while((nn=BList[j].name))
  {
 if( BList[j].RUN_MODES[0].type != BTYPE_INVALID )
  {
  if( c_info->group_mode >= 0 )
  fprintf(unit,"# (Multi-)%s\n",nn);
  else
  fprintf(unit,"# %s\n",nn);

  if    ( *(BList[j].bench_comments) )
      fprintf(unit,"#     Comments on this Benchmark:\n");
  while ( (cmt = *(BList[j].bench_comments++)) )
      fprintf(unit,"#     %s\n",cmt);

  }
  j++;
  }
}

}
