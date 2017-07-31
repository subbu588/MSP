/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Mps_Critical_Section.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
    ------------------------------------------------------------------------- */
    
/* **************************************************************************
	modification history 
	-------------------- 
	25sep2013,Pavan  Modified "MpsOsCriticalSectionEnter" function.
	25sep2013,Pavan  Modified "MpsOsCriticalSectionLeave" function.
	
   *************************************************************************** */

/*===================================
		JVT_# includes
**===================================
*/

#include "Jvt_Msp_Types.h"
#include "Jvt_Msp_Critical_Section.h"

/*===============================================
*	Declaring The Global Variable
*===============================================*/

SEM_ID mySem;

JvtUInt32 MpsOsCriticalSectionEnter()
{
	MSP_Log_Func("===Entry point of MpsOsCriticalSectionEnter()===",0xFF,"JVT_MPS_CRITICAL_SECTION");

	/*Please implement mutex semaphore here*/
	mySem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
	semTake(mySem, WAIT_FOREVER);
	printf("===Taking Semaphore=== \n\n");
	MSP_Log_Func("===Exit point of MpsOsCriticalSectionEnter()===",0xFF,"JVT_MPS_CRITICAL_SECTION");

	return MPS_SUCCESS; 
}

JvtUInt32 MpsOsCriticalSectionLeave()
{
	MSP_Log_Func("===Entry point of MpsOsCriticalSectionLeave()===",0xFF,"JVT_MPS_CRITICAL_SECTION");

	/*Please implement mutex semaphore here*/
	
	semGive(mySem);
	printf("===Giving Semaphore=== \n\n");
	
	MSP_Log_Func("===Exit point of MpsOsCriticalSectionLeave()===",0xFF,"JVT_MPS_CRITICAL_SECTION");
	
	return MPS_SUCCESS;
}
