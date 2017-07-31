 /*-------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Msp_Thread.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
 ------------------------------------------------------------------------- 
 */

/***************************************************************************
	modification history 
	-------------------- 
	19sep2013,BHARATH  Modifeid "MSP_Thread_init" function.
	19sep2013,BHARATH  Added "MSP_Thread" function.
	19sep2013,BHARATH  Added "Layer2_Thread" function.
	1Oct 2013,mahesh 	  modified "MSP_Thread" function.
	1Oct 2013,mahesh   Modified  "Layer2_Thread" function.
	17Oct 2013,Sathish  Modified  "MSP_Thread" function.
	24nov2013,chaitanya Added "MspCallPsDeactivate" function.
	
 *************************************************************************** 
*/

/*===================================
		JVT_# includes
**===================================
*/

#include "Jvt_Msp_Types.h"
#include "Jvt_Msp_Layer2.h"
#include "Jvt_Msp_Trace.h"
#include "Jvt_Msp_Application.h"
#include "Jvt_Msp_Thread_Create.h"
/*===============================================
*   Static Global Function
*===============================================
*/

 void MSP_Thread(); 
 void Layer2_Thread(); 
extern void Network_Stack_Thread();

/********************************************************************* 
 *	MSP_thread_init for creating threads - Function Definition 
 *********************************************************************
 */

void MSP_Thread_init()
{
   	/* Local variable declaration to store threads ID*/

   	/*jvt_s32 MSP_Thread_Id =0; 

   	jvt_s32 Layer2_Thread_Id =0;

   	jvt_s32 Network_Stack_Thread_Id =0;

   	jvt_s32 Application_Thread_Id =0;*/

   	MSP_Log_Func("===Entry point of MSP_Thread_init()===",0xFF,"JVT THREAD");


	 void Msp_OS_Thread_Create();
	MSP_Log_Func("===Exit point of MSP_Thread_init()===",0xFF,"JVT THREAD");

}


/********************************************************************* 
 	EM_Thread to create message queue- Function Definition 
**********************************************************************/

 void MSP_Thread()
{
	MspCallPsActivateParams Session_id;
	
	MspCallPsSetAuthenticationParams MSPAuth_Param;

	MspCallPsConnectReqParams MSPConnect_param; 
	
	MspCallPsDeactivateReqParams MSPDeactivate_param;

	Session_id.cid=SIM_ID;
	
	MSPAuth_Param.cid=SIM_ID;
	
	MSPConnect_param.cid=SIM_ID;
	
	MSPDeactivate_param.cid=SIM_ID;
	
	MSPConnect_param.l2_protocol=MSP_CALL_PS_LAYER2_PROTOCOL_OPT_PPP;
	
	strcpy(MSPAuth_Param.authentication.username,"JVT");
	
	strcpy(MSPAuth_Param.authentication.password,"JVT123");
	
    MSPAuth_Param.authentication.type=MSP_CALL_PS_AUTHENTICATION_TYPE_CHAP;
	
	strcpy(MSPAuth_Param.authentication.auth_ppp_frame,NETWORK_TYPE);

	MSP_Log_Func("===Entry point of MSP_Thread()===",0xFF,"JVT THREAD");
	
	MspCallPsActivate(SIM_ID,&Session_id,APP_ID);

	MspCallPsSetAuthentication(SIM_ID, &MSPAuth_Param,APP_ID);

	MspCallPsConnect(SIM_ID,&MSPConnect_param,APP_ID); 
	
	MspCallPsDeactivate(SIM_ID,&MSPDeactivate_param,APP_ID);

    
	

	MSP_Log_Func("===Exit point of MSP_Thread()===",0xFF,"JVT THREAD");
}


/********************************************************************* 
 	Layer2_Thread - Function Definition 
**********************************************************************/

 void Layer2_Thread()
{
    mqd_t                      mqPXId; /* msg queue file descriptor */
    MspCallPs_Msg_Descriptor   Msp_Msg_Descriptor;/* msg queue descriptor */  
	JvtInt32                        priority;     	/* priority of message */
	
	MSP_Log_Func("===Entry point of Layer2_Thread()===",0xFF,"JVT_Layer2_Thread");
	
	while(TRUE)
	{
		/* open msg queue; should already exist with default attributes */ 
		if ((mqPXId = mq_open (MSP_MQ_RSP, O_RDWR | O_CREAT, 0, NULL)) == (mqd_t) -1) 
		{ 
			printf("Msg_Open : mq_open failed\n"); 
			return; 
		}
		/* try reading from queue */ 
		if (mq_receive (mqPXId,&Msp_Msg_Descriptor, MSG_SIZE, &priority) == -1) 
		{ 
		   	printf("Msg_Receive: mq_receive failed\n"); 
		   	return; 
		} 
		else 
		{ 
		   printf("L2_Thread_receiveTask: Messsage of priority %d received: %d\n", priority, Msp_Msg_Descriptor.Msp_Nw_Resp);
		   L2_Msp_Sync_Deinitializer(Msp_Msg_Descriptor);	
		}
	}
	    MSP_Log_Func("===Exit point of L2_Msp_Sync_Initializer()===",0xFF,"JVT_MPS_LAYER2");

	/*return L2_NW_ACTIVATE_RSP_SUCCESS;*/
}	

