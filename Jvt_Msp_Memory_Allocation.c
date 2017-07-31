/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_MSP_Memory_Allocation.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
    ------------------------------------------------------------------------- */

/* **************************************************************************
	modification history 
	-------------------- 

   *************************************************************************** */
   
/*===================================
	JVT_# includes
**===================================
*/

#include "Jvt_Msp_Memory_Allocation.h"
#include<stdlib.h>
#include"Jvt_Msp_Types.h"

/*=========================================
 *	Allocating Memory Dynamically
 *=========================================
 */
 void *JvtMemFree( void* mptr);
void* JvtMemAlloc( JVTU32 size)
{
	void * mptr = NULL;

	JVTInt32 status=0;
	MSP_Log_Func("===Entry point of Dynamic Memory Allocation===",0xFF,"JvtMemAlloc");
	

	if(NULL == size)
	{
		return (void *)JVT_Memory_Failure;
	}

	mptr = (JVTU32*)malloc( size);

	if ( status != JVT_Memory_Failure)
	{
		memset(mptr,0,size);
	}
	else
	{
		mptr = NULL;
	}
	MSP_Log_Func("===Exit point of Dynamic Memory Allocation===",0xFF,"JvtMemAlloc");
	return mptr;
}

/*=================================================
 *	Deallocating Memory dynamically
 *=================================================*/

 void *JvtMemFree( void* mptr)
{
	
   free(mptr);

   mptr=NULL;

  return mptr;
}