
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
 *        Bug fix for case of NULL-Info in function Print_Info (PMB-IO)      *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/




#include <string.h>
#include "declare.h"
#include "Benchmark.h"

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype Output (this file)  */

void Output(
     struct comm_info* c_info, struct Bench* Bmark, MODES BMODE, 
     int tmp_NP, int header, int size, 
     int n_sample, double *time);

/* Prototype Display_Times (this file)  */

void Display_Times(
     struct Bench* Bmark, double* tlist, struct comm_info* c_info, 
     int group, int n_sample, int size, 
     int edit_type);

/* Prototype Show_Selections (this file)  */

void Show_Selections(
     struct comm_info* c_info, struct Bench* BList);

/* Prototype Show_Procids (this file)  */

void Show_Procids(
     struct comm_info* c_info);

/* Prototype Print_Info (this file)  */

void Print_Info(
     );

/* Prototype Print_Headlines (this file)  */

void Print_Headlines(
     struct comm_info* c_info, int tmp_NP, char* cases, 
     int li_len, char* h_line);

/* Prototype Edit_Format (this file)  */

void Edit_Format(
     int n_ints , int n_floats);

/* Prototype Make_Line (this file)  */

void Make_Line(
     int li_len);

/*** PROTOTYPES (extern)   ***/

/* Prototype v_alloc, File: Mem_Manager.c */

void* v_alloc(
     int Len, char* where
     );

#ifdef MPIIO
/* Prototype User_Set_Info, File: User_Set_Info.c */

void User_Set_Info(
     MPI_Info* opt_info);
#endif

/* Prototype str, File: strgs.c */

char* str(
     char* Bname);

/* Prototype str_erase, File: strgs.c */

void  str_erase(
     char* str, int Nblnc);

/* Prototype General_Info, File: g_info.c */

void General_Info(
     );

/* Prototype Print_BList, File: BenchList.c */

void Print_BList(
     struct comm_info * c_info, struct Bench *BList);

/* Prototype Err_Hand, File: Err_Handler.c */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );


/******************************/
/*       END PROTOTYPES       */


/*****************************************************************/


/******* Implementation Output *******/

void Output(struct comm_info* c_info,struct Bench* Bmark,MODES BMODE,
	    int tmp_NP, int header,int size,int n_sample,double *time)
/*****************************************************************/

/*-----------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
-------------------------------------------------------------------
Input      :  c_info  | struct comm_info* | see comm_info.h 
              Bmark   | struct Bench*     | current enchmark
              tmp_NP  | int               | number of nodes
	      header  | int               | first call or not (header flag)
              size    | int               | message length in byte
	      n_sample| int               | repetition count
	      time    | double*           | time of measurement
	              |                   |
Output     :          |                   |
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
-------------------------------------------------------------------
-------------------------------------------------------------------
Description: Output of results (header and bare data of measurement)
-----------------------------------------------------------------*/
{
  double scaled_time[MAX_TIMINGS];
  
  int DO_OUT;
  int GROUP_OUT;
  int i,i_gr;
  int li_len;
  int edit_type;
  
  ierr = 0;

  DO_OUT    = (c_info->w_rank  == 0 );
  GROUP_OUT = (c_info->group_mode > 0 );

  if (DO_OUT) 
    {
      if(!all_times)
	{
          all_times = 
  (double*)v_alloc(c_info->w_num_procs * Bmark->Ntimes * sizeof(double), 
                  "Output 1");
	}
#ifdef CHECK
      if(!all_defect)
	{
          all_defect = (double*)v_alloc(c_info->w_num_procs * sizeof(double), 
                  "Output 1");
          for(i=0; i<c_info->w_num_procs; i++) all_defect[i]=0.;
	}
#endif  	  
    }

/* Scale the timings */
  for(i=0; i<Bmark->Ntimes;  i++)
  scaled_time[i] = time[i] * SCALE * Bmark->scale_time;


/* collect all times  */
  ierr=MPI_Gather(scaled_time,Bmark->Ntimes,MPI_DOUBLE,all_times,Bmark->Ntimes,MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_ERRHAND(ierr);

#ifdef CHECK      
/* collect all defects */	      
  ierr=MPI_Gather(&defect,1,MPI_DOUBLE,all_defect,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_ERRHAND(ierr);

#endif
  if( DO_OUT )
    {
      BTYPES type= Bmark->RUN_MODES[0].type;
      if ( Bmark->RUN_MODES[0].NONBLOCKING )
           edit_type = 4;
      else if ( type == SingleTransfer && c_info->group_mode != 0 )
           edit_type=0;
      else if ( type == ParallelTransfer || type == SingleTransfer )
           edit_type=1;
      else if (type == Collective )
#ifdef MPIIO
           edit_type=1;
#else
           edit_type=2;
#endif
      else 
           edit_type=3;

      if( header )
	{
        fprintf(unit,"\n");            /* FOR GNUPLOT: CURVE SEPERATOR  */


          if( GROUP_OUT ) {strcpy(aux_string,"&Group") ; li_len=1;}
          else            {strcpy(aux_string,"");  li_len=0;}
	  if ( edit_type == 0 )
	    {
	      li_len+=4;
	      strcat(aux_string,"&#bytes&#repetitions&t[usec]&Mbytes/sec&");
	    }
	  else if ( edit_type == 1 )
	    {
	      li_len+=6;
	      strcat(aux_string,
		     "&#bytes&#repetitions&t_min[usec]&t_max[usec]&t_avg[usec]&Mbytes/sec&");
	    }
	  else if ( edit_type == 2 )
	    {
	      li_len+=5;
	      strcat(aux_string,
		     "&#bytes&#repetitions&t_min[usec]&t_max[usec]&t_avg[usec]&");
	    }
	  else if ( edit_type == 3 )
	    {
	      li_len+=4;
	      strcat(aux_string,
		     "&#repetitions&t_min[usec]&t_max[usec]&t_avg[usec]&");
	    }
          else
            {
	      li_len+=6;
	      strcat(aux_string,
		     "&#bytes&#repetitions&t_ovrl[usec]&t_pure[usec]&t_CPU[usec]& overlap[%]&");
            }
#ifdef CHECK
          if( Bmark->RUN_MODES[0].type != Sync &&
              strcmp(Bmark->name,"Window") )
          {
	  li_len+=1;
	  strcat(aux_string,"&defects&");
          }
#endif
        Make_Line(li_len);
        if( c_info->n_groups > 1) 
        fprintf(unit,"# Benchmarking Multi-%s ",Bmark->name);
        else
        fprintf(unit,"# Benchmarking %s ",Bmark->name);
        Show_Procids(c_info); 

        Make_Line(li_len);

        switch(BMODE->AGGREGATE)
          {
          case 1:
          fprintf(unit,"#\n#    MODE: AGGREGATE \n#\n");
          break;
          case 0:
          fprintf(unit,"#\n#    MODE: NON-AGGREGATE \n#\n");
          break;
          }
	  Print_Headlines(c_info,tmp_NP,Bmark->name,li_len,aux_string);
	}     



      if( GROUP_OUT )
      {


      for( i_gr=0; i_gr<c_info->n_groups; i_gr++ )
	{
	  if(i_gr == 0) fprintf(unit,"\n");

    	  Display_Times(Bmark, all_times, c_info, i_gr, n_sample, size, edit_type);
	} 
      }
      else
    	  Display_Times(Bmark, all_times, c_info,  0, n_sample, size, edit_type);
    } 
}
/*****************************************************************/


/******* Implementation Display_Times *******/

void Display_Times( struct Bench* Bmark,
                    double* tlist,struct comm_info* c_info,int group,
		    int n_sample,int size,int edit_type)
/*****************************************************************/

/*-------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
-------------------------------------------------------------------
Input      :  Bmark   | struct Bench*     | Current benchmark 
              tlist   | double*           | list of timings
	      c_info  | struct comm_info* | current comm. structure
	      group   | int               | number of group
	      n_sample| int               | repetition count
              size    | int               | message length in byte
                      |                   |
Output     :          |                   |
                      |                   |
In/Out     :  -       | -                 | -
                      |                   |  
-------------------------------------------------------------------
-------------------------------------------------------------------
Description: Output of results (header and bare data of measurement)
-----------------------------------------------------------------*/
{
  static double MEGA = 1.0/1048576.0;

  double tmax, tmin, tav, t_pure, throughput, overlap; 
#ifdef CHECK
  double defect;
#endif
  int i, ip, itim, inc;
  
  if( c_info->g_sizes[group]<= 0 ) return;

  inc = Bmark->Ntimes;

  for(itim=0; itim < Bmark->Ntimes; itim++ )
  {


  if( c_info->group_mode > 0)
  {
  
  i =0;
  ip=0;
  while( i<group )
    {
      ip+= c_info->g_sizes[i++];
    }
 
    tmin = tlist[ip*inc+itim];
    tmax = 0.;
    tav  = 0.;
#ifdef CHECK
    defect = 0.;
#endif
    for(i=0; i<c_info->g_sizes[group]; i++)
	{
	  tmax  =  max(tmax,tlist[(ip+i)*inc+itim]);
	  tmin  =  min(tmin,tlist[(ip+i)*inc+itim]);
	  tav  += tlist[(ip+i)*inc+itim];
#ifdef CHECK 
          defect = max ( defect, all_defect[c_info->g_ranks[ip+i]] );
#endif 
	}
    tav /= c_info->g_sizes[group];
    }
    else
    {
    ip = 0;
    for( i=0; i<c_info->n_groups; i++ ) ip += c_info->g_sizes[i];
    tmin = tlist[itim];
    tmax = 0.;
    tav  = 0.;
#ifdef CHECK
    defect = 0.;
#endif

    for(i=0; i<ip; i++)
	{
	  tmax  =  max(tmax,tlist[i*inc+itim]);
	  tmin  =  min(tmin,tlist[i*inc+itim]);
	  tav  += tlist[i*inc+itim];
#ifdef CHECK 
          defect = max ( defect, all_defect[i] );
#endif 
	}
    tav /= ip;
    }

    ip=0;


    if( Bmark->RUN_MODES[0].NONBLOCKING )
      if( itim == 0 )
        {
        t_pure = tmax;
        }
      else
        {
        overlap = 100.*max(0,min(1,(t_pure+tCPU-tmax)/min(t_pure,tCPU)));
        }

    }  /* for (itim .. ) */

    throughput = 0.;
    if( tmax > 0. ) throughput = (Bmark->scale_bw*SCALE*MEGA)*size/tmax;

    
    if(c_info->group_mode > 0 )
      {
	Edit_Format(1,0);
	sprintf(aux_string,format,group);
	ip=strlen(aux_string);
      }

    if( edit_type == 0 )
      { 
	Edit_Format(2,2);
	sprintf(aux_string+ip,format,size,n_sample,
		tmax,throughput);
      }
    else if ( edit_type == 1 )
      {
	Edit_Format(2,4);
	sprintf(aux_string+ip,format,size,n_sample,tmin,tmax,
		tav,throughput);
      }
    else if ( edit_type == 2 )
      {
	Edit_Format(2,3);
	sprintf(aux_string+ip,format,size,n_sample,tmin,tmax,tav);
      }
    else if ( edit_type == 3 )
      {
	Edit_Format(1,3);
	sprintf(aux_string+ip,format,n_sample,tmin,tmax,tav);
      }
    else if ( edit_type == 4 )
      {
	Edit_Format(2,4);
	sprintf(aux_string+ip,format,size,n_sample,tmax,t_pure,tCPU,overlap);
      }

#ifdef CHECK 
    if ( edit_type != 3 && strcmp(Bmark->name,"Window") )
    {
    Edit_Format(0,1);
    ip=strlen(aux_string);
    sprintf(aux_string+ip,format,defect);

    if( defect > TOL    ) Bmark->success=0;
    }
#endif
    fprintf(unit,"%s\n",aux_string);
    fflush(unit);

  
}


/************************************************************************/


/******* Implementation Show_Selections *******/

void Show_Selections(struct comm_info* c_info, struct Bench* BList)
/************************************************************************

-----------------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
-------------------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
             BList    | struct Bench*     | list of benchmarks to run
                      |                   |
Output     :     -    | -                 | - 
                      |                   |  
In/Out     :     -    | -                 | -
                      |                   |  
------------------------------------------------------------------------
------------------------------------------------------------------------
Description: Output of basic parameters
------------------------------------------------------------------------
*/
{
  if(c_info->w_rank == 0 )
    {
      General_Info();
      fprintf(unit,"#\n");
#ifndef MPIIO
      fprintf(unit,"# Minimum message length in bytes:   %d\n",0);
      fprintf(unit,"# Maximum message length in bytes:   %d\n",1<<MAXMSGLOG);
      fprintf(unit,"#\n");
      fprintf(unit,"# MPI_Datatype                   :   MPI_BYTE \n");
      fprintf(unit,"# MPI_Datatype for reductions    :   MPI_FLOAT\n");
      fprintf(unit,"# MPI_Op                         :   MPI_SUM  \n");
#else
      fprintf(unit,"# Minimum io portion in bytes:   %d\n",0);
      fprintf(unit,"# Maximum io portion in bytes:   %d\n",1<<MAXMSGLOG);
      fprintf(unit,"#\n");
      Print_Info();
#endif
      fprintf(unit,"#\n");

#ifdef PMB_OPTIONAL
      fprintf(unit,"#\n\n");
      fprintf(unit,"# !! Attention: results have been achieved in\n");
      fprintf(unit,"# !! PMB_OPTIONAL mode.\n");
      fprintf(unit,"# !! Results may differ from standard case.\n");
      fprintf(unit,"#\n");
#endif

      fprintf(unit,"#\n");

      Print_BList(c_info, BList);

      if( do_nonblocking )
      {
      fprintf(unit,"\n\n# For nonblocking benchmarks:\n\n");
      fprintf(unit,"# Function CPU_Exploit obtains an undisturbed\n");
      fprintf(unit,"# performance of %7.2f MFlops\n",MFlops);         
      }

    }
}

/****************************************************************************/


/******* Implementation Show_Procids *******/

void Show_Procids(struct comm_info* c_info)
/****************************************************************************

-----------------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
-----------------------------------------------------------------------------
Input      : c_info   | struct comm_info* | see comm_info.h 
                      |                   |
Output     :     -    | -                 | - 
                      |                   |  
In/Out     :     -    | -                 | -
                      |                   |  
---------------------------------------------------------------------------
---------------------------------------------------------------------------
Description: Output of number of processes and number of 
             active groups (incl. nodes)
---------------------------------------------------------------------------
*/
{
  int ip, i, k, idle;
  
  if( c_info->w_rank == 0 )
    {
      if(c_info->n_groups == 1)
        {
	fprintf(unit,"\n# ( #processes = %d ) \n",c_info->num_procs);
        idle = c_info->w_num_procs-c_info->num_procs;
        }
      if(c_info->n_groups != 1)
	{
	  fprintf(unit,"\n# ( %d groups of %d processes each running simultaneous ) \n",
		  c_info->n_groups,c_info->num_procs); 
	  ip = 0;
	  for(i=0; i<c_info->n_groups; i++)
	    {
	      fprintf(unit,"# Group %4d                     :",i);
	      for(k=0; k<c_info->g_sizes[i]; k++) 
		fprintf(unit," %d", c_info->g_ranks[ip+k]); 
	      ip += c_info->g_sizes[i];
	      fprintf(unit,"\n");
	    }
          idle = c_info->w_num_procs - ip;
	}
      if( idle )
        {
        if( idle == 1 )
	fprintf(unit,"# ( %d additional process waiting in MPI_Barrier)\n",idle);
        else
	fprintf(unit,"# ( %d additional processes waiting in MPI_Barrier)\n",idle);
        }
    }

}

#ifdef MPIIO

/****************************************************************************/


/******* Implementation Print_Info *******/

void Print_Info()
/****************************************************************************/
{
int nkeys,ikey,vlen,exists;
MPI_Info tmp_info;
char key[MPI_MAX_INFO_KEY], *value;
 
User_Set_Info(&tmp_info);

/* July 2002 fix V2.2.1: handle NULL case */
if( tmp_info!=MPI_INFO_NULL ) {
/* end change */

MPI_Info_get_nkeys(tmp_info, &nkeys);

if( nkeys > 0) fprintf(unit,"# Got %d Info-keys:\n\n",nkeys);
 
for( ikey=0; ikey<nkeys; ikey++ )
 {
 MPI_Info_get_nthkey(tmp_info, ikey, key);
 
 MPI_Info_get_valuelen(tmp_info, key, &vlen, &exists);
 
 value = (char*)v_alloc((vlen+1)* sizeof(char), "Print_Info");
 
 MPI_Info_get(tmp_info, key, vlen, value, &exists);
 printf("# %s = \"%s\"\n",key,value);
 
 free (value);
 }

MPI_Info_free(&tmp_info);

/* July 2002 fix V2.2.1: end if */
}
/* end change */
 
}
#endif



/*****************************************************************/


/******* Implementation Print_Headlines *******/

void Print_Headlines(struct comm_info* c_info,int tmp_NP,char* cases,
		     int li_len,char* h_line)
/*****************************************************************/
{
  char* help;
  char* token;

  help=h_line;
  while(token = strtok(help, "&") )
    {

      sprintf(format,"%%%ds",ow_format);
      fprintf(unit,format,token);
      help = NULL;
    }
  fprintf(unit,"\n");
}

/*****************************************************************/


/******* Implementation Edit_Format *******/

void Edit_Format(int n_ints , int n_floats)
/*****************************************************************/
{
  int ip,i;
  
  ip=0;
  for(i=1 ; i<=n_ints; i++)
    {    
      sprintf(&(format[ip]),"%%%dd",ow_format); 
      ip=strlen(format);
    }
  for(i=1 ; i<=n_floats ; i++)
    {
      sprintf(&(format[ip]),"%%%d.2f",ow_format);
      ip=strlen(format);
    }
}

/***************************************************************************/


/******* Implementation Make_Line *******/

void Make_Line(int li_len)
/***************************************************************************

-----------------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
----------------------------------------------------------------------------
Input      :  li_len  | int               | line length
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
Description: print a line of li_len many "-"
---------------------------------------------------------------------------*/
{
  int i;
  char* char_line = "-";
  fprintf(unit,"#");
  for( i=1;i<li_len*ow_format; i++ )
    {
      fprintf(unit,"%s",char_line);
    }
  fprintf(unit,"\n");	
}
