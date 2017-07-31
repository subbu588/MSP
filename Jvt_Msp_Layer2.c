/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Mps_Layer2.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
    ------------------------------------------------------------------------- */

/* **************************************************************************
	modification history 
	-------------------- 
	19sep2013,BHARATH  Modified "L2_Msp_Sync_Activate_Req" function.
	19sep2013,BHARATH  Added "L2_Msp_Sync_Initializer" function.
	19sep2013,BHARATH  Added "L2_Msp_MQ_Send_ToNW" function.
	19sep2013,BHARATH  Added "L2_Msp_Sync_Deinitializer" function.
	30sep2013,pavan  Modified "L2_Msp_Sync_Activate_Req" function.
	30sep2013,pavan Modified "L2_Msp_Sync_Initializer" function.
	5oct 2013,pavan  modified "L2_Msp_MQ_Send_ToNW" function.
	8oct 2013,pavan  modified "L2_Msp_Sync_Deinitializer" function.
	14sep2013,Sathish  Modified "L2_Msp_Sync_Activate_Req" function.
	15sep2013,Sathish  Modified  "L2_Msp_Sync_Initializer" function.
	17oct 2013,Sathish  modified "L2_Msp_MQ_Send_ToNW" function.
	17oct 2013,Sathish  modified "L2_Msp_Sync_Deinitializer" function.
	26nov2013,Chaitanya modified " L2_Msp_Sync_Deinitializer"
	26nov2013,Chaitanya Added "Mspdeallocatememory"function.
	
   *************************************************************************** */
   

/*===================================
		JVT_# includes
**===================================*/

#include "Jvt_Msp_Types.h"
#include "Jvt_Msp_Init.h"
#include "Jvt_Msp_Memory_Allocation.h"
#include "Jvt_Msp_Layer2.h"
#include "Jvt_Msp_Application.h"
 
/*===================================
		Prototype 
**===================================*/
 static jvt_s32 Mspdeallocatememory(PL2_SIGNAL pReqHead);

/*===============================================
*	Externing The Global Variable
*===============================================*/ 

L2_SIGNAL *pReqHead=NULL;

/************************************************************************* 
 	L2_Msp_Sync_Activate_Req- Function Definition 
**************************************************************************/

JvtUInt32   L2_Msp_Sync_Activate_Req( unsigned int sim_id, PVOID pvInstance, MSPHANDLER pfnHandler,Msp_L2_Nw_Req L2_Nw_RqId )
{
   	PL2_SIGNAL signal;

	MSP_Log_Func("===Entry point of L2_Msp_Sync_Activate_Req()===",0xFF,"JVT_MPS_LAYER2");

	/*Please allocate the memory dynmaically for this structure and it needs to be implemented in DLL .*/
	signal=(L2_SIGNAL *)JvtMemAlloc(sizeof(L2_SIGNAL));
	
	if(signal==NULL)
	{
		return JVT_Memory_Failure;
	}
	else
	{
		signal->sim_id=sim_id;
		signal->pvInstance=pvInstance;
		signal->L2_Req_id=L2_Nw_RqId;
		signal->pfnHandler=pfnHandler;
		signal->pReqNext=NULL;
		signal->pReqPrev=NULL;
	
   	L2_Msp_Sync_Initializer(signal);
	}

	/*Please send the  mesage queue to network task*/

	L2_Msp_MQ_Send_ToNW(L2_Nw_RqId);
		
    /*MPS_LEAVE_CFH_MUTEX;*/
	
	MSP_Log_Func("===Exit point of L2_Msp_Sync_Activate_Req()===",0xFF,"JVT_MPS_LAYER2");
return 0;
}

/************************************************************************* 
 	L2_Msp_Sync_Initializer - Function Definition 
**************************************************************************/

void L2_Msp_Sync_Initializer(PL2_SIGNAL signal)
{
	PL2_SIGNAL pReqTemp=pReqHead;

	MSP_Log_Func("===Entry point of L2_Msp_Sync_Initializer()===",0xFF,"JVT_MPS_LAYER2");

	if(pReqHead==NULL)
	{
		signal->pReqPrev=NULL;
		pReqHead=signal;
		signal->pReqNext=NULL;
	}
		else 
		{
	 
			while(pReqTemp->pReqNext!=NULL)
			{
				pReqTemp=pReqTemp->pReqNext;
			
			}
		
			pReqTemp->pReqNext=signal;
			signal->pReqPrev=pReqTemp;
		
		}
	 
	MSP_Log_Func("===Exit point of L2_Msp_Sync_Initializer()===",0xFF,"JVT_MPS_LAYER2");

	/*return L2_NW_ACTIVATE_RSP_SUCCESS;*/
}

/************************************************************************* 
 	L2_Msp_MQ_Send_ToNW- Function Definition 
**************************************************************************/

void L2_Msp_MQ_Send_ToNW(Msp_L2_Nw_Req Msp_Nw_ReqType)
{	
	mqd_t	 mqPXId;		  

	MspCallPs_Msg_Descriptor Msp_Msg_Descriptor;
	
	MSP_Log_Func("===Entry point of L2_Msp_MQ_Send_ToNW()===",0xFF,"JVT_MPS_LAYER2");
    
    Msp_Msg_Descriptor.Msp_Nw_Req=Msp_Nw_ReqType;

	/* open message queue using default attributes */ 
	if ((mqPXId = mq_open (MSP_MQ_NAME, O_RDWR | O_CREAT, 0, NULL)) == (mqd_t) -1) 
	{ 
		printf ("Msg_open: mq_open failed\n"); 
		return; 
	}
	if (mq_send (mqPXId,(char *)&Msp_Msg_Descriptor, sizeof (Msp_Msg_Descriptor), HI_PRIO) == -1) 
	{ 
		printf ("Msg_Send: mq_send failed\n"); 
		return; 
	} 
	else
	{
		printf ("Msg_send:%d mq_send succeeded\n\n",Msp_Msg_Descriptor.Msp_Nw_Req);
	}

	MSP_Log_Func("===Exit point of L2_Msp_MQ_Send_ToNW()===",0xFF,"JVT_MPS_LAYER2");
}


/************************************************************************* 
 	L2_Msp_Sync_Deinitializer- Function Definition 
**************************************************************************/

void L2_Msp_Sync_Deinitializer(MspCallPs_Msg_Descriptor Msg_Decriptor)
{
    
	PL2_SIGNAL pReqTemp=pReqHead;
	
	MspCallPsActivateRspCbParams ActivateRspCbParams;
	
	MspCallPsSetAuthenticationRspCbParams AuthenticationRspCbParams;
	
	MspCallPsConnectRspCbParams ConnectRspCbParams;
	
	MspCallPsDeactivateRspCbParams DeactivateRspCbParams;
	
	JvtUInt32 app_id=APP_ID;
	
	ActivateRspCbParams.cid=SIM_ID;
	
	AuthenticationRspCbParams.cid=SIM_ID;
	
	ConnectRspCbParams.cid=SIM_ID;  
	
	DeactivateRspCbParams.cid=SIM_ID;
	
	ActivateRspCbParams.result=Msg_Decriptor.Msp_Nw_Resp;
	
	AuthenticationRspCbParams.result=Msg_Decriptor.Msp_Nw_Resp;
	
	ConnectRspCbParams.result=Msg_Decriptor.Msp_Nw_Resp; 
	
	DeactivateRspCbParams.result=Msg_Decriptor.Msp_Nw_Resp;   
	
	
	MSP_Log_Func("===Entry point of L2_Msp_Sync_Deinitializer()===  ",0xFF,"JVT_MSP_LAYER2");
	
			
	while(pReqTemp!= NULL)

	{
		if(pReqTemp->L2_Req_id==Msg_Decriptor.Msp_Nw_Req)
		{
			switch(Msg_Decriptor.Msp_Nw_Req)
			{
			case L2_NW_ACTIVATE_CNF_SUCCESS:
			
			case L2_NW_ACTIVATE_REJ_FAILURE:

			pReqTemp->pfnHandler((PL2_SIGNAL*)(pReqTemp->sim_id),&ActivateRspCbParams,app_id);

			break;
			
			case L2_NW_AUTHENTICATE_CNF_SUCCESS:
			
			case L2_NW_AUTHENTICATE_REJ_FAILURE:

			pReqTemp->pfnHandler((PL2_SIGNAL*)(pReqTemp->sim_id),&AuthenticationRspCbParams,app_id);

			break;
			
			case L2_NW_CONNECT_SUCCESS:
			
			case L2_NW_CONNECT_FAILURE:
					
			pReqTemp->pfnHandler((PL2_SIGNAL*)(pReqTemp->sim_id),&ConnectRspCbParams,app_id);
			break;
			
			case L2_NW_DEACTIVATE_CNF_SUCCESS:
			
			case L2_NW_DEACTIVATE_REJ_FAILURE:
					
            pReqTemp->pfnHandler((PL2_SIGNAL*)(pReqTemp->sim_id),&DeactivateRspCbParams,app_id);
			
                          if((pReqTemp->L2_Req_id==L2_NW_DEACTIVATE_CNF_SUCCESS)||(pReqTemp->L2_Req_id==L2_NW_DEACTIVATE_REJ_FAILURE))
				        {
				          Mspdeallocatememory(pReqHead);

						   
				        }
			
			
					
			break;
			
			default:
				MSP_Log_Func("=====default===",0xFF,"JVT_MSP_LAYER2");
					
				
			}	
				
		}
		pReqTemp=pReqTemp->pReqNext;
	
	}
	
	MSP_Log_Func("===Exit point of L2_Msp_Sync_Deinitializer()=== ",0xFF,"JVT_MPS_LAYER2");
}

 
 /************************************************************************* 
	 Mspdeallocatememory- Function Definition 
 **************************************************************************/

                   
  static jvt_s32 Mspdeallocatememory(PL2_SIGNAL pReqHead)
 	
	 {

                                        
	MSP_Log_Func("===Entry point of Deallocate Memory",0xFF,"JVT_MPS_LAYER2");
		  
				
		while(pReqHead->pReqNext!=NULL)
									
		{
			PL2_SIGNAL pReqtemp=pReqHead;
	        pReqHead=pReqHead->pReqNext;
				
			JvtMemFree(pReqtemp);
				
			pReqHead->pReqPrev=NULL;					
		MSP_Log_Func("****Deallocation Successfull****",0xFF,"JVT_MPS_LAYER2");		
		}
		 JvtMemFree(pReqHead);
		 
		pReqHead=NULL;
	    
		
				

	   MSP_Log_Func("===Exit point of Deallocate Memory",0xFF,"JVT_MPS_LAYER2");
				
return 0;
   
}
