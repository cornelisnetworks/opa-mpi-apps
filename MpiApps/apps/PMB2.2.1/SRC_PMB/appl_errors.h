
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



#ifndef _APPL_ERRORS_H

#define _APPL_ERRORS_H

#include "mpi.h"

/* Define errors of the application */
#define APPL_ERR_MALLOC   10
#define APPL_ERR_INVCASE  20
#define APPL_ERR_OPTIONS  30
#define APPL_ERR_CINFO    40

/* Err-handler*/
void Errors_mpi(MPI_Comm *, int*,...);

#define MAX_ERR_LINES 32
struct ERR_HEADER
{
int n_header;
char* Lines[MAX_ERR_LINES];
int err_flag;
};

#endif
