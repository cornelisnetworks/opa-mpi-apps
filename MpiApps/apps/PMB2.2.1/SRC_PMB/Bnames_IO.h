
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


#include "Benchmark.h"

/* NAMES OF BENCHMARKS (DEFAULT CASE)*/
char *DEFC[] = {
   "S_Write_Indv",
   "S_IWrite_Indv",
   "S_Write_Expl",
   "S_IWrite_Expl",
   "P_Write_Indv",
   "P_IWrite_Indv",
   "P_Write_Shared",
   "P_IWrite_Shared",
   "P_Write_Priv",
   "P_IWrite_Priv",
   "P_Write_Expl",
   "P_IWrite_Expl",
   "C_Write_Indv",
   "C_IWrite_Indv",
   "C_Write_Shared",
   "C_IWrite_Shared",
   "C_Write_Expl",
   "C_IWrite_Expl",
   "S_Read_Indv",
   "S_IRead_Indv",
   "S_Read_Expl",
   "S_IRead_Expl",
   "P_Read_Indv",
   "P_IRead_Indv",
   "P_Read_Shared",
   "P_IRead_Shared",
   "P_Read_Priv",
   "P_IRead_Priv",
   "P_Read_Expl",
   "P_IRead_Expl",
   "C_Read_Indv",
   "C_IRead_Indv",
   "C_Read_Shared",
   "C_IRead_Shared",
   "C_Read_Expl",
   "C_IRead_Expl",
   "Open_Close",
   NULL
};

