/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Msp_Network.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
    ------------------------------------------------------------------------- */

/* **************************************************************************
	modification history 
	-------------------- 
	19 sep2013,BHARATH  Added "Network_Stack_Thread" function.
	08 oct2013,pavan Modified "Network_Stack_Thread" function.
	13 oct2013,pavan Modified "Network_Stack_Thread" function.

	15 oct2013,Sathish Modified Global Variables.
	25nov2013,chaitanya Added one Enums to"Nw_Req_Type"
	25nov2013,chaitanya Added two Enums to"Nw_Resp_Type"
	 
*************************************************************************** 
*/

/*===================================
		JVT_# includes
**===================================
*/

#include "Jvt_Msp_Types.h"
#include "Jvt_Msp_Network.h"
#include "Jvt_Msp_Layer2.h"

/*===============================================
*    Static Global Variables
*===============================================*/
   static Msp_L2_Nw_Req Nw_Req_Type[REQ_PARAMS_SIZE]={L2_NW_ACTIVATE_REQ,L2_NW_AUTHENTICATE_REQ,L2_NW_CONNECT_REQ,L2_NW_DEACTIVATE_REQ};
   static Msp_L2_Nw_Resp Nw_Resp_Type[RESP_PARAMS_SIZE]={L2_NW_ACTIVATE_CNF_SUCCESS,L2_NW_AUTHENTICATE_CNF_SUCCESS,L2_NW_CONNECT_SUCCESS,L2_NW_DEACTIVATE_CNF_SUCCESS,L2_NW_ACTIVATE_REJ_FAILURE,L2_NW_AUTHENTICATE_REJ_FAILURE,L2_NW_CONNECT_FAILURE,L2_NW_DEACTIVATE_REJ_FAILURE};
/*================================================================== 
 *		Network_Stack_Thread- Function Definition 
 *=================================================================*/
	
	void Network_Stack_Thread()
	{
	   	mqd_t					   mqPXId;
		MspCallPs_Msg_Descriptor   Msp_Msg_Descriptor;
		JvtInt32 index;
		JvtInt32 priority;
		
		MSP_Log_Func("===Entry point of Network_Stack_Thread()===",0xFF,"JVT_MSP_NETWORK");
		
		while(TRUE)
		{
			/* open msg queue; should already exist with default attributes */ 
			if ((mqPXId = mq_open (MSP_MQ_NAME, O_RDWR, 0, NULL)) == (mqd_t) -1) 
			{ 
				printf("sendTask: mq_open failed\n"); 
				return; 
			}
			/* try reading from queue */ 
			if (mq_receive (mqPXId,&Msp_Msg_Descriptor,MSG_SIZE,&priority) == -1) 
			{ 
				printf("receiveTask: mq_receive failed	nw recieve\n"); 
				return; 
			} 
			else 
			{ 
			for(index=0;index<REQ_PARAMS_SIZE;index++)
			{
				if((Msp_Msg_Descriptor.Msp_Nw_Req==Nw_Req_Type[index]))
				{
					Msp_Msg_Descriptor.Msp_Nw_Resp=Nw_Resp_Type[index];

					/* open message queue using default attributes */ 
				if ((mqPXId = mq_open (MSP_MQ_RSP, O_RDWR | O_CREAT, 0, NULL)) == (mqd_t) -1) 
				{ 
					printf ("Msg_Open: mq_open failed\n"); 
					return; 
				}
				
				if (mq_send (mqPXId,(char *)&Msp_Msg_Descriptor, sizeof (Msp_Msg_Descriptor), HI_PRIO) == -1) 
				{ 
					printf ("Msg_send: mq_send failed\n"); 
					return; 
				} 
				else
				{
					printf ("Msg_send:%d mq_send succeeded\n\n",Msp_Msg_Descriptor.Msp_Nw_Resp);
				}
				
			}
				
			}
		}
			
		}
		MSP_Log_Func("===Exit point of Network_Stack_Thread()===",0xFF,"JVT_MSP_NETWORK");
	}

