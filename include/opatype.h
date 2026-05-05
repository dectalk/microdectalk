/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/*  Include File: opatype.h                                           */
/*  Author: Bill Hallahan                                             */
/*  Date: November 15, 1995                                           */
/*                                                                    */
/*  Abstract:                                                         */
/*                                                                    */
/*          This file includes the type definition for a type that    */
/*    is large enough to contain an address (pointer).                */
/*                                                                    */
/**********************************************************************/
/*
 *  001	MGS		05/09/2001		Some VxWorks porting BATS#972
 *  002	MGS		06/19/2001		Solaris Port BATS#972
 *
 ********************************************************************/

#ifndef _OPATYPE_H_
#define _OPATYPE_H_

/**********************************************************************/
/*  Digital UNIX. and Linux                                           */
/**********************************************************************/
typedef long	 ATYPE_T;
typedef ATYPE_T* LPATYPE_T;

#endif
