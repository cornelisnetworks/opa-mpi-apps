
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
 *        2.2 change: function set_default added (variable initialization)   * 
 *  Revision: 2.2.1                                                          *
 *  Date: 2002/07/04                                                         *
 *        Bug fix (see comment "July 2002 fix V2.2.1:")                      * 
 *                                                                           *
 *                                                                           *
 *****************************************************************************/




#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

void set_default(struct comm_info* c_info)
{
c_info->w_num_procs=0;          
c_info->w_rank=-1;
c_info->NP=0;                   
c_info->communicator=MPI_COMM_NULL;    
c_info->num_procs =0;
c_info->rank = -1;
c_info->s_data_type =  MPI_DATATYPE_NULL;
c_info->r_data_type =  MPI_DATATYPE_NULL;
c_info->red_data_type =  MPI_DATATYPE_NULL;
c_info->op_type = MPI_OP_NULL;
c_info->pair0 = c_info->pair1 = -2;
c_info->select_tag = 0;
c_info->select_source = 0;
c_info->s_buffer = NULL;
c_info->s_data = NULL;
c_info->s_alloc = 0;
c_info->r_buffer = NULL;
c_info->r_data = NULL;
c_info->r_alloc = 0;
c_info->group_mode = 0;
c_info->n_groups = 0;
c_info->group_no = -1;
c_info->g_sizes = NULL;
c_info->g_ranks = NULL;
c_info->reccnt = NULL;
c_info->displs = NULL;

c_info->ERR=MPI_ERRHANDLER_NULL;
 
#ifdef MPIIO
/*   FILE INFORMATION     */
 
c_info->filename=NULL;
c_info->File_comm=MPI_COMM_NULL;
c_info->File_num_procs=0;
c_info->all_io_procs=0;
c_info->File_rank=-1;

c_info->fh=MPI_FILE_NULL;
c_info->etype=MPI_DATATYPE_NULL;
c_info->e_size=0;
c_info->filetype=MPI_DATATYPE_NULL;

c_info->split.Locsize=0;
c_info->split.Offset=(MPI_Offset)0;
c_info->split.Totalsize=0;
 
c_info->amode=0;
c_info->info=MPI_INFO_NULL;
 
/* View: */
c_info->disp=(MPI_Offset)0;
c_info->datarep=NULL;
c_info->view=MPI_DATATYPE_NULL;
c_info->ERRF=MPI_ERRHANDLER_NULL;
#endif

#ifdef EXT
c_info->WIN=MPI_WIN_NULL;
c_info->info=MPI_INFO_NULL;
c_info->ERRW=MPI_ERRHANDLER_NULL;
#endif
 

}


/* Prototype Basic_Input (this file)  */

void Basic_Input(
     struct comm_info* c_info, struct Bench** P_BList, int *argc, 
     char ***argv, int* NP_min);

/* Prototype get_rank_portion (this file)  */

void get_rank_portion(
     int rank, int NP, int size, 
     int unit_size, int* pos1, int* pos2
     );

/* Prototype Init_Communicator (this file)  */

int Init_Communicator(
     struct comm_info* c_info, int NP);

/* Prototype Set_Communicator (this file)  */

void Set_Communicator(
     struct comm_info *c_info );

/* Prototype valid (this file)  */

int valid(
     struct comm_info * c_info, struct Bench* Bmark, int NP
     );


/*** PROTOTYPES (extern)   ***/

/* Prototype Free_File, File: Init_File.c */

void Free_File(
     struct comm_info * c_info);

/* Prototype Construct_BList, File: BenchList.c */

void Construct_BList(
     struct Bench** P_BList, int n_args, char* name
     );

/* Prototype Err_Hand, File: Err_Handler.c */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/* Prototype sadd_header, File: Output.c */

void sadd_header (
     char* msg);

/* Prototype Str_Atoi, File: strgs.c */

int Str_Atoi(
     char s[]);

/* Prototype set_buf, File: Mem_Manager.c */

void set_buf(
     struct comm_info* c_info, int selected_rank, int s_pos1, 
     int s_pos2, int r_pos1, int r_pos2
     );

/* Prototype Destruct_BList, File: BenchList.c */

void Destruct_BList(
     struct Bench ** P_BList);

/* Prototype Set_Errhand, File: Err_Handler.c */

void Set_Errhand(
     struct comm_info* c_info);

/* Prototype i_alloc, File: Mem_Manager.c */

void i_alloc(
     int** B, int Len, char* where 
     );

/* Prototype str, File: strgs.c */

char* str(
     char* Bname);

/* Prototype Init_Errhand, File: Err_Handler.c */

void Init_Errhand(
     struct comm_info* c_info);

/* Prototype Del_Errhand, File: Err_Handler.c */

void Del_Errhand(
     struct comm_info* c_info);

/* Prototype CPU_Exploit, File: CPU_Exploit.c */

void CPU_Exploit(float target_secs, int initialize);

/* Prototype Get_Def_Cases, File: Parse_Name_XX.c */

void Get_Def_Cases(
     char*** defc, char*** Gcmt);

/* Prototype Get_Def_Index (file BenchList.c)  */

void Get_Def_Index(  
           int* index, char* name 
                  );


/******************************/
/*       END PROTOTYPES       */

/********************************************************************/


/******* Implementation Basic_Input *******/

void Basic_Input(struct comm_info* c_info,
                 struct Bench** P_BList,
                 int *argc, char ***argv,int* NP_min)
/********************************************************************


---------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
---------------------------------------------------------------------
Input:       argc     | int*              | number of comm. line args
             argv     | char***           | command line arguments
             
In/Out     : c_info   | struct comm_info* | see comm_info.h 
                      |                   | (will be partially initialized)
                      |                   |
Output     : 
             P_BList  | struct Bench**    | *P_Blist = 
                      |                   | array of Benchmarks descriptors
                      |                   |
             NP_min   | int               | minimum number of processes
                      |                   |  
----------------------------------------------------------------------
----------------------------------------------------------------------
Description: Initializing options defined in command line.
             1. Selection of individual benchmarks       
	        (e.g. mpirun -np * pmb Allreduce ) 
             2. Selection of additional args:
	        (e.g. mpirun -np * pmb -npmin * -group * ) 
--------------------------------------------------------------------*/


{
  int i,n_cases,iarg;
  int deflt;
  int * ALL_INFO;
  char** DEFC, **CMT;

  MPI_Comm_rank(MPI_COMM_WORLD,&c_info->w_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&c_info->w_num_procs);
  
  unit = stdout; 
  if( c_info->w_rank == 0 && strlen(OUTPUT_FILENAME) > 0 )
    unit = fopen(OUTPUT_FILENAME,"w"); 

#ifdef DEBUG

      dbgf_name = str("DBG_   ");
      sprintf(dbgf_name+4,"%d",c_info->w_rank);

      dbg_file = fopen(dbgf_name,"w");

#endif


  deflt = 0;

  c_info->group_mode = -1;
#ifdef MPIIO
  *NP_min=1;
#else
  *NP_min=2;
#endif

  if( c_info->w_rank == 0 )
  {
/* Interpret command libe */

  if( *argc <= 1 )
    {
      /* Take default */
      deflt = 1;
    }
  else
    { 
      n_cases = *argc-1;

      iarg = 1;
       
      while( iarg <= *argc-1 )
      {
      if(!strcmp((*argv)[iarg],"-npmin"))
	{
	  *NP_min=Str_Atoi((*argv)[iarg+1]);
	  if(*NP_min>=0){n_cases -= 2;}
	  else
	    {Err_Hand(0, APPL_ERR_OPTIONS);}
          iarg++;
	}
      if(!strcmp((*argv)[iarg],"-multi"))
	{
	  c_info->group_mode=Str_Atoi((*argv)[iarg+1]);
          n_cases -= 2;
          iarg++;
	}
      if(!strcmp((*argv)[iarg],"-input"))
	{
	  FILE*t = fopen((*argv)[iarg+1],"r");
          n_cases -= 2;
          if( t )
          {
          char inp_line[72];
          while(fgets(inp_line,72,t))
          {
          if( inp_line[0] != '#' && strlen(inp_line)>1 )
          n_cases++;
          }
          fclose(t);
          }
          else fprintf(unit,"Input File %s doesnt exist\n",(*argv)[iarg+1]);
          iarg++;
	}
        iarg++;
       }

      if(n_cases <= 0 )
        deflt = 1;
      else
        {
        iarg = 1;
 
        while( iarg <= *argc-1 )
        {
        if(!strcmp((*argv)[iarg],"-npmin")) 
                iarg++;
        else if(!strcmp((*argv)[iarg],"-multi")) 
                iarg++;
        else if(!strcmp((*argv)[iarg],"-input"))
	  {
	  FILE*t = fopen((*argv)[iarg+1],"r");
          if( t )
          {
          char inp_line[72], nam[32];
          while(fgets(inp_line,72,t))
          {
          if( inp_line[0] != '#' && strlen(inp_line)-1 )
            {
            sscanf(inp_line,"%s",nam);
            Construct_BList(P_BList,n_cases,nam);
            }
          }
          fclose(t);
          }
          else fprintf(unit,"Input File %s doesnt exist\n",(*argv)[iarg+1]);
          iarg++;
          }
	else  
            {
              Construct_BList(P_BList,n_cases,(*argv)[iarg]);          
            }
        iarg ++;
        }
        }
    }

    if( deflt) {
      n_cases = 0;
      Construct_BList(P_BList,0,"ALL");
               }

    i_alloc(&ALL_INFO,4+n_cases,"Basic_Input");
	    

    if( !deflt )
    {
    
    i=0;
    n_cases = 0;

    while( (*P_BList)[i].name )
      {
      int index;
      Get_Def_Index(&index,(*P_BList)[i].name );
  
      if( index >= 0 )
                 ALL_INFO[4+n_cases++] = index;

      i++;
      }
    }

    ALL_INFO[0] = *NP_min;
    ALL_INFO[1] = c_info->group_mode;
    ALL_INFO[2] = deflt;
    ALL_INFO[3] = n_cases;

    MPI_Bcast(ALL_INFO,4,MPI_INT,0,MPI_COMM_WORLD);

    if(n_cases > 0 && !deflt) 
    MPI_Bcast(ALL_INFO+4,n_cases,MPI_INT,0,MPI_COMM_WORLD);

    free(ALL_INFO);

    }
    else  /* w_rank > 0 */
/* Receive input arguments */

    {
    int TMP[4];
    MPI_Bcast(TMP,4,MPI_INT,0,MPI_COMM_WORLD);


    *NP_min = TMP[0];
    c_info->group_mode = TMP[1];
    deflt = TMP[2];
    n_cases = TMP[3];

    if( deflt )
      Construct_BList(P_BList,0,"ALL");
    else if( n_cases>0 )
     {
     i_alloc(&ALL_INFO,n_cases,"Basic_Input");
     MPI_Bcast(ALL_INFO,n_cases,MPI_INT,0,MPI_COMM_WORLD);
     Get_Def_Cases(&DEFC,&CMT);
     for( i = 0; i<n_cases; i++ )
     Construct_BList(P_BList,n_cases,DEFC[ALL_INFO[i]]);
     free(ALL_INFO);
     }
    else
     Construct_BList(P_BList,1,"");

    }
 
#ifndef EXT
    if( do_nonblocking )
    CPU_Exploit(TARGET_CPU_SECS, 1);
#endif

}



/******* Implementation get_rank_portion *******/

void get_rank_portion(int rank, int NP, int size, int unit_size, 
                      int* pos1, int* pos2)
{

int ne, baslen, mod;

ne = (size+unit_size-1)/unit_size;
baslen = ne/NP;
mod    = ne%NP;

if( rank < mod )
  {
  *pos1 = rank*(baslen+1)*unit_size;
  *pos2 = *pos1-1+(baslen+1)*unit_size;
  }
  else
  {
  *pos1 = (rank*baslen + mod)*unit_size;
  *pos2 = *pos1-1 + baslen*unit_size;
  }

*pos2 = min(*pos2,size-1);

}

/********************************************************************/

/******* Implementation Init_Communicator *******/

int Init_Communicator(struct comm_info* c_info, int NP)
/**********************************************************************/

/*-------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
---------------------------------------------------------------------
                      |                   |
Input      : NP       | int               | number of processes to
                      |                   | run the benchmark (may
                      |                   | be less than size of
                      |                   | MPI_COMM_WORLD)
                      |                   |
In/Out     : c_info   | struct comm_info* | see comm_info.h 
                      |                   | communicator initialized
                      |                   | group management set up 
----------------------------------------------------------------------*/

{
int i,snd,cnt,proc,*aux_ptr;

MPI_Group group, w_group;
MPI_Status stat;

c_info->NP=NP;                     /* NUMBER OF OVERALL PROCESSES    */          
Set_Communicator( c_info );        /* GROUP MANAGEMENT               */

/* INITIALIZATION  WITHIN THE ACTUAL COMMUNICATOR */
if( c_info->communicator != MPI_COMM_NULL )
  {
    MPI_Comm_size(c_info->communicator,&(c_info ->num_procs));
    MPI_Comm_rank(c_info->communicator,&(c_info ->rank));
    
    c_info->pair0 = 0;
    c_info->pair1 = c_info ->num_procs-1;
    
    c_info->select_tag = 0;
    c_info->select_source = 0;
  }
else
  {
    c_info -> rank = -1;
  }

if( c_info->communicator == MPI_COMM_WORLD ) 
  {
    c_info->n_groups = 1;
    c_info->g_sizes[0] = c_info->w_num_procs;
    for(i=0; i<c_info->w_num_procs; i++) c_info->g_ranks[i]=i;
  }
else
  {
    /* Collect global group information */
    if( c_info->rank == 0 )
      {
	/* group leaders provide group ranks */
	MPI_Comm_group(MPI_COMM_WORLD,&w_group);
	MPI_Comm_group(c_info->communicator,&group);
	for (i=0; i<c_info->num_procs; i++) c_info->g_sizes[i] = i;

	/* TRANSLATION OF RANKS */
	MPI_Group_translate_ranks( group, c_info->num_procs, 
				   c_info->g_sizes,w_group,
				   c_info->g_ranks );
	snd = c_info->num_procs;
      }
    else
      {
	*c_info->g_ranks = -1;
	snd = 1;
      }
    /* w_rank 0 collects in g_ranks ranks of single groups */
    if( c_info->w_rank == 0 ) 
      {
        if( c_info->rank == 0 )
	  {
            c_info->n_groups = 1;
            c_info->g_sizes[c_info->n_groups-1] = c_info->num_procs;
            aux_ptr = c_info->g_ranks+c_info->num_procs;
	  }
	else
	  {
            c_info->n_groups = 0;
            aux_ptr=c_info->g_ranks;
	  }
	for( proc=1; proc<c_info->w_num_procs; proc++ )
	  {
	    /* Recv group ranks or -1  */
	    cnt = (int)(c_info->g_ranks+c_info->w_num_procs-aux_ptr);
/* July 2002 fix V2.2.1 (wrong logistics), next 23 lines */
	    if( cnt <= 0 )
	      /* all leaders have sent, recv dummies (-1) from others! */
	      {
		cnt=1;
	        MPI_Recv(&i,cnt,MPI_INT,proc,1000,MPI_COMM_WORLD,&stat);
	      }
	    else
              {

	        MPI_Recv(aux_ptr,cnt,MPI_INT,proc,1000,MPI_COMM_WORLD,&stat);
	    
	        if( *aux_ptr >= 0 ) 
	        {
		/* Message was from a group leader  */
		c_info->n_groups ++;
		MPI_Get_count( &stat, MPI_INT,
			       &c_info->g_sizes[c_info->n_groups-1]);
		aux_ptr += c_info->g_sizes[c_info->n_groups-1];
	        }

	      }
/* end fix V2.2.1 */
	  }
      }
    else  /* w_rank != 0 */
      {
	MPI_Send(c_info->g_ranks,snd,MPI_INT,0,1000,MPI_COMM_WORLD);
      }
  }
/* End collection of group information */   

Set_Errhand(c_info);

return 0;
}


/**********************************************************************/


/******* Implementation Set_Communicator *******/

void Set_Communicator( struct comm_info *c_info )
/**********************************************************************

----------------------------------------------------------------------
             VARIABLE              |       TYPE        |   MEANING
----------------------------------------------------------------------
Input      :                       |                   |  
Output     :                       |                   |
                                   |                   |  
In/Out     :  c_info               | struct comm_info* | see comm_info.h
                                   |                   | Communicator set up
-----------------------------------------------------------------------

-----------------------------------------------------------------------
Description: Initialization of communicators by group coloring
---------------------------------------------------------------------*/
{
  int color,key,i;
  
  /* insert choice for communicator here;
     NOTE   :  globally more than one communicator is allowed   
     Example: grouping of pairs of processes: 
     0 0 1 1 2 2  .. (if even),  UNDEF 0 0 1 1 2 2  .. (if odd) 
     */

  if( c_info->communicator != MPI_COMM_NULL &&  
      c_info->communicator != MPI_COMM_SELF &&
      c_info->communicator != MPI_COMM_WORLD)
  {
  i=MPI_Comm_free(&c_info->communicator);
  Err_Hand(1,i);
  }

  if(c_info->group_mode >= 0)
    {
      i=c_info->w_rank;
      color = i/c_info->NP;
      c_info->group_no = color;
      key = 0;
      if(color >= c_info->w_num_procs/c_info->NP) color=MPI_UNDEFINED;
      MPI_Comm_split(MPI_COMM_WORLD, color, key, &c_info->communicator);
    }
  /* Default choice and Group definition.  */
  else
    {
      if(c_info->w_rank < c_info->NP) color=0;
      else color=MPI_UNDEFINED;   
      c_info->group_no = 0;
      key=0;
      MPI_Comm_split(MPI_COMM_WORLD, color, key, &c_info->communicator); 
    }
}



/******* Implementation valid *******/

int valid(struct comm_info * c_info, struct Bench* Bmark, int NP)
{
/* Checks validity of Bmark for NP processes */
/* Erroneous cases: */
int invalid, skip;

invalid = 0; skip = 0;
#ifndef MPIIO
invalid = Bmark->RUN_MODES[0].type == SingleTransfer && NP <=1;
skip    = Bmark->RUN_MODES[0].type == SingleTransfer && NP > 2;
#endif

if ( invalid )
  {
  if( c_info->w_rank == 0 )
  {
  if( NP == 1 )
  fprintf(unit,"\n# !! Benchmark %s invalid for %d process   !! \n\n",Bmark->name,NP);
  else
  fprintf(unit,"\n# !! Benchmark %s invalid for %d processes !! \n\n",Bmark->name,NP);
  }
 
  return 0;
  }

/* Cases to skip: */
if ( skip ) return 0;

return 1;
}


/********************************************************************/

