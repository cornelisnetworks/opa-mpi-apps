
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

/*  PROTOTYPES (this file)    */
/******************************/

/* Prototype str (this file)  */

char* str(
     char* Bname);

/* Prototype str_erase (this file)  */

void  str_erase(
     char* str, int Nblnc);

/* Prototype LWR (this file)  */

void LWR(
     char* Bname);

/* Prototype Str_Atoi (this file)  */

int Str_Atoi(
     char s[]);


/*** PROTOTYPES (extern)   ***/

/* Prototype Err_Hand, File: Err_Handler.c */

void Err_Hand(
     int ERR_IS_MPI, int ERR_CODE );

/* Prototype v_alloc (file Mem_Manager.c)  */

void* v_alloc(
      int Len, char* where
     );


/******************************/
/*       END PROTOTYPES       */




/******* Implementation str *******/

char* str(char* Bname)
{
char* strg = (char*) v_alloc( 1+strlen(Bname),"str" );
if(strg)
return strcpy(strg,Bname);
else return NULL;
}



/******* Implementation LWR *******/

void LWR(char* Bname)
{
int i;
for(i=0; i<strlen(Bname); i++)
   if ( Bname[i] >= 'A' && Bname[i] <= 'Z' )
        Bname[i] = Bname[i] - ('A'-'a');
}



/***************************************************************************/


/******* Implementation Str_Atoi *******/

int Str_Atoi(char s[])
/***************************************************************************

-----------------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
----------------------------------------------------------------------------
Input      :     s    | char              | asci string
                      |                   |
Return     :          | int               | numerical value of digits in string
                      |                   |  
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
Description: Converts ascii string to int up to char symbol ' '
---------------------------------------------------------------------------*/
{
  int i,n,len ;
  n=0;
  len=strlen(s);
  for(i=0; s[i] >= '0' && s[i] <= '9' || s[i]==' '; ++i)
    {
      if (s[i]!=' ') 
	{
	  n=10*n+(s[i]-'0');
	}
    }
  if (i<len)
    {
      n=-1;
    }
  return n;
}


/* str_erase   */

void  str_erase(
     char* str, int Nblnc)
{
if( Nblnc > 0 )
{
int i;
for(i=0;i<Nblnc; i++ ) str[i]=' ';
i=max(0,Nblnc);
str[i]='\0';
}
}

