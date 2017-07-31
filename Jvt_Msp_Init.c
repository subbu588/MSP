
/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Msp_Init.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
    ------------------------------------------------------------------------- */
    
/* **************************************************************************
	modification history 
	-------------------- 
	19sep2013,BHARATH  Modified "L2_Msp_Async_Activate_Req" function.
	19sep2013,BHARATH  Added "L2_Msp_Async_Initializer" function.
	
   *************************************************************************** */
   
/*===================================
		JVT_# includes
**===================================
*/


#include"Jvt_Msp_Types.h"
#include"Jvt_Msp_Init.h"
#include"Jvt_Msp_Application_Interface.h"
#include"Jvt_Msp_Memory_Allocation.h"
#include"Jvt_Msp_Layer2.h"

 MSP_UI_HOOK   L2ActivateStatusHook;

MSP_UI_HOOK *pRqHead;

/*============================================================= 
 *MspCallPsInitialize - Function Definition 
 *=============================================================*/
 
/* Description:
   This function initializes the List of Asynchronus informations which will be indicated by network based on session conncetivity
   */

void MspCallPsInitialize(void)
{
	/*jvt_s32 index = 0;*/

	MSP_Log_Func("===Entry point to MspCallPsInitialize()===",0xFF,"JVT TRACE");
	
	L2_Msp_Async_Activate_Req(L2_UI_PCH_ACTIVATE_IND,MSP_PROC_ID_L2,&L2ActivateStatusHook,NULL,(ASYNC_HANDLER)MspCallPsActivateStatusIndCb);
	
	MSP_Log_Func("===Exit point to MspCallPsInitialize()===",0xFF,"JVT TRACE");
   
}


JvtUInt32 L2_Msp_Async_Activate_Req( MSP_PCH_UI_CODE uicode, MSP_PROC_IDS id ,MSP_UI_HOOK1 pHook, PVOID pvInstance, ASYNC_HANDLER pfnHandler )
{

	/*create structure and need to dynamically allocate memory */
	MSP_UI_HOOK1 signal;
	signal=(MSP_UI_HOOK *)JvtMemAlloc(sizeof(MSP_UI_HOOK));
	if(signal==NULL)
	{
		return JVT_Memory_Failure;
	}
	else
	{
	   	signal->pvInstance=pvInstance;
		signal->pfnHandler=pfnHandler;
		return L2_Msp_Async_Initializer(signal);
	}
		MPS_ENTER_CFH_MUTEX;

		MPS_LEAVE_CFH_MUTEX;
		
	return 0;
}

JvtUInt32 L2_Msp_Async_Initializer(MSP_UI_HOOK1 signal)
{
	MSP_UI_HOOK1 pReqTemp;
	signal->pReqNext=NULL;
	if(pRqHead==NULL)
	{
		signal->pReqPrev=NULL;
		pRqHead=signal;
	}
	else
	{
		pReqTemp=pRqHead;
		while(pReqTemp->pReqNext!=NULL)
		{
			pReqTemp=(MSP_UI_HOOK*)(pReqTemp->pReqNext);
		}
		(MSP_UI_HOOK*)(pReqTemp->pReqNext)=signal;
		(MSP_UI_HOOK*)(signal->pReqPrev)=pReqTemp;
	}
	return L2_NW_ACTIVATE_CNF_SUCCESS;
}

void MspCallPsActivateStatusIndCb( JvtInt32 sim_type, MspCallPsActivateStatusIndCbParams *params )
{
}