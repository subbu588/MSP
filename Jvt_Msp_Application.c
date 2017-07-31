/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Msp_Application.c" $
       Author		:	Venkatesh
       Company		:	JV Technologies
    ------------------------------------------------------------------------- */

/* **************************************************************************
	modification history 
	-------------------- 
	19sep2013,BHARATH  Modified "MspCallPsActivate" function.
	19sep2013,BHARATH  Modified "MspCallPsActivateRspCb" function. 
	27sep2013,Pavan  added "MSPactivesessionPsTransaction" function.
	27sep2013,Pavan  Modified "MSPactivesessionPsTransaction" function. 
	2oct2013, pavan added "MspCallPsSetAuthentication" function.
	10oct2013,Pavan added"MspCallPsSetAuthenticationRspCb" function.
	15oct2013,Sathish added"MspCallPsConnect" function.
	16oct2013,Sathish added"MspCallPsConnectRspCb" function.
	24nov2013,chaitanya Added  " MspCallPsDeactivate"function.
	24nov2013,chaitanya Added  "MspCallPsDeactivateRspCb"
	
	
	
	
	
   *************************************************************************** */

/*===================================
		JVT_# includes
**===================================*/
#include "Jvt_Msp_Types.h"
#include "Jvt_Msp_Application.h"
#include "Jvt_Msp_Memory_Allocation.h"
#include "Jvt_Msp_Layer2.h"
#include "Jvt_Msp_Trace.h"
/*===================================
		JVT_ Global DATA
**===================================*/

 void MspCallPsActivateRspCb( JvtUInt32 sim_type, const MspCallPsActivateRspCbParams *resp, JvtUInt32 app_id );

MspCallPsInfo * psCallInfo;

static jvt_u32 Msp_CallPs_ActiveSession[MAX_ACTIVE_SESSION];
static unsigned pdp_transaction_active_instance = MSP_INSTANCE_INVALID;
static unsigned sms_transaction_active_instance = MSP_INSTANCE_INVALID;
/*static unsigned call_transaction_active_instance = MSP_INSTANCE_INVALID;*/
static unsigned call_current_sim_transaction = MSP_INSTANCE_INVALID;
/********************************************************************* 
 	MpsCallPsCheckCid- Function Definition 
**********************************************************************/


JvtUInt32 MpsCallPsCheckCid( MspCallPsPdpId cid)
{

    if( (cid < MPS_CALL_PS_INVALID_PDP_CONTEXT_ID) || (cid > 255))
    return FALSE;
   	if( cid == MPS_CALL_PS_INVALID_PDP_CONTEXT_ID)
    return TRUE;

    return TRUE;
}
/********************************************************************* 
 	MspGetCallTransaction - Function Definition 
**********************************************************************/


jvt_u32 MspGetCallTransaction(void)
{
    return call_current_sim_transaction;
}
/********************************************************************* 
 	MspGetSMSTransaction- Function Definition 
**********************************************************************/


jvt_u32 MspGetSMSTransaction(void)
{
    return sms_transaction_active_instance;
}
/********************************************************************* 
 	MspResetPDPTransaction - Function Definition 
**********************************************************************/



void MspResetPDPTransaction (void)
{
    pdp_transaction_active_instance = MSP_INSTANCE_INVALID;
}
/********************************************************************* 
 	MspSetPDPTransaction- Function Definition 
**********************************************************************/



void MspSetPDPTransaction (unsigned sim_type)
{
    pdp_transaction_active_instance = sim_type;
}
/********************************************************************* 
 	mpsIsConflictHandlingActive- Function Definition 
**********************************************************************/


BOOL mpsIsConflictHandlingActive( unsigned sim_type, T_MSP_CONFLICT_HANDLING_TYPE conflict_handling_type )
{
	unsigned current_instance=0;

   	switch (conflict_handling_type)
   	{
		case E_MSP_CONFLICT_HANDLING_TYPE_CALL_CS:
   		case E_MSP_CONFLICT_HANDLING_TYPE_SS:
   		case E_MSP_CONFLICT_HANDLING_TYPE_SMS:
   		case E_MSP_CONFLICT_HANDLING_TYPE_CALL_PS:
   		{
   			if ( conflict_handling_type == E_MSP_CONFLICT_HANDLING_TYPE_CALL_PS )
   				{
   					if((current_instance != MSP_INSTANCE_INVALID) && (current_instance != sim_type))
   						{
   							return TRUE;
   						}
   				}

   	/* Check for CALL CS CFH, SS CFH, SMS CFH */
   			current_instance = MspGetSMSTransaction();
   			if((current_instance != MSP_INSTANCE_INVALID) && (current_instance != sim_type) )
   				{
   							return TRUE;
   				}
   					break;
   	   }
   			default:
   			break;
   	}

   	return FALSE;
}

/********************************************************************* 
 	MspCallPsActivate- Function Definition 
**********************************************************************/

JvtUInt32 MspCallPsActivate( JvtUInt32 sim_type, const MspCallPsActivateParams * params, JvtUInt32 app_id )
{
	

 	MSP_Log_Func("===Entry point of MspCallPsActivate()===",0xFF,"JVT MSP APPLICATION");

   	if(params == NULL)
	{
   		return   MPS_ERROR_INVALID_PARAM;
	}
	
   	if( !MpsCallPsCheckCid(params->cid))   
	{
   	   	return   MPS_ERROR_INVALID_PARAM;
	}

   	MPS_ENTER_CFH_MUTEX;

   	if(!mpsIsConflictHandlingActive(sim_type, E_MSP_CONFLICT_HANDLING_TYPE_CALL_PS) == TRUE )
	{
		MPS_LEAVE_CFH_MUTEX;
		return   MPS_ERROR_WRONG_STATE;
	}
    if(MSPactivesessionTransaction(params->cid))
    {
       	
         return MPS_ERROR_NOT_SUPPORTED;

	}
	
   	MspGetSMSTransaction();

   	MPS_LEAVE_CFH_MUTEX;

   	psCallInfo = (MspCallPsInfo*)JvtMemAlloc( sizeof(MspCallPsInfo));

   	if(psCallInfo == NULL)
   	{
   	   	MspResetPDPTransaction();
   	   	return MPS_ERROR_OUT_OF_MEMORY;
   	}

   	psCallInfo->pdp_context_id = params->cid;
   	psCallInfo->application_id = app_id;

#if defined( MSP_MULTI_SIM_PS_SUSPEND )
   	MspSetPDPTransaction(sim_type);
#endif


   	L2_Msp_Sync_Activate_Req( sim_type, (PVOID)psCallInfo, (MSPHANDLER)MspCallPsActivateRspCb,L2_NW_ACTIVATE_REQ);

   	MSP_Log_Func("===Exit point of MspCallPsActivate()===",0xFF,"JVT MSP APPLICATION");

   	return MSP_SUCCESS;

}


/********************************************************************* 
   	MspCallPsActivateRspCb- Function Definition 
**********************************************************************/

    void MspCallPsActivateRspCb( JvtUInt32 sim_type,const MspCallPsActivateRspCbParams  *resp, JvtUInt32 app_id )
	{
	   MSP_Log_Func("===Entry point of MspCallPsActivateRspCb()===",0xFF,"JVT MSP APPLICATION");	
	   if(resp->result==L2_NW_ACTIVATE_CNF_SUCCESS)
	   printf("*** Sessoin Activated ***\n\n");
	   else
	   printf("***session failed****\n\n");
       MSP_Log_Func("===Exit point of MspCallPsActivateRspCb()===",0xFF,"JVT MSP APPLICATION");
	}

 /********************************************************************* 
 	MSPactivesessionTransaction- Function Definition 
**********************************************************************/
 BOOL MSPactivesessionTransaction(MspCallPsPdpId cid)
{
   jvt_u32 i=0;
   static jvt_u32 CurrentSession=0;
   MSP_Log_Func("===Entry point of MSPactivesessionPsTransaction()===",0xFF,"JVT MSP APPLICATION");
   	for(i=0;i<MAX_ACTIVE_SESSION;i++)
   	{   
	   if(Msp_CallPs_ActiveSession[i]==cid)
       	return TRUE;
	}
   Msp_CallPs_ActiveSession[CurrentSession]=cid;
   CurrentSession++;	
   printf("session ID --->  %d\n",cid);
   MSP_Log_Func("===Exit point of MSPactivesessionPsTransaction()===",0xFF,"JVT MSP APPLICATION");
   return FALSE;
}
 /********************************************************************* 
 	MspCallPsSetAuthentication - Function Definition 
**********************************************************************/

JvtUInt32 MspCallPsSetAuthentication( JvtUInt32 sim_type, const MspCallPsSetAuthenticationParams * params, JvtUInt32 app_id )
   	{
    	MSP_Log_Func("===Entry point of MspCallPsSetAuthentication()===",0xFF,"JVT MSP APPLICATION");
	
	
   	if((strlen(params->authentication.username)>MSP_CALL_PS_MAX_USERNAME_LENGTH)||(strlen(params->authentication.password)>MSP_CALL_PS_MAX_PASSWORD_LENGTH))
    {
		return MPS_ERROR_INSUFFICIENT_PERMISSIONS;
	}
	 if(!strcmp(params->authentication.auth_ppp_frame,NETWORK_TYPE))
	{
	switch(params->authentication.type)
	 {
	case MSP_CALL_PS_AUTHENTICATION_TYPE_NONE:
	case MSP_CALL_PS_AUTHENTICATION_TYPE_CHAP:
	case MSP_CALL_PS_AUTHENTICATION_TYPE_PAP:
	{
	if((strcmp(params->authentication.username,"JVT")&&strcmp(params->authentication.password,"JVT123")))
    return MPS_ERROR_INSUFFICIENT_PERMISSIONS;
	}
	break;
	default :
	printf("please give the proper authentication type");
	break;
	}	
	
	L2_Msp_Sync_Activate_Req( sim_type, (PVOID)psCallInfo, (MSPHANDLER)MspCallPsSetAuthenticationRspCb,L2_NW_AUTHENTICATE_REQ);
	
	}
	MSP_Log_Func("===Exit point of MspCallPsSetAuthentication()===",0xFF,"JVT MSP APPLICATION");
          return 0;
	}	

/************************************************************************* 
 	MspCallPsSetAuthenticationRspCb- Function Definition 
**************************************************************************/

      void MspCallPsSetAuthenticationRspCb( JvtUInt32 sim_type, const MspCallPsSetAuthenticationRspCbParams  *resp, JvtUInt32 app_id )
	{
	  MSP_Log_Func("===Entry point of MspCallPsSetAuthenticationRsponseParams()===",0xFF,"JVT MSP APPLICATION");
	
		if(resp->result==L2_NW_AUTHENTICATE_CNF_SUCCESS)
		   printf("\n*****authentication succeeded****\n\n");
		else 
		   printf("*** authentication failed****\n\n");
	  MSP_Log_Func("===Exit point of MspCallPsSetAuthenticationRsponseParams()===\n\n",0xFF,"JVT MSP APPLICATION");
	}
	
/************************************************************************* 
		MspCallPsConnect- Function Definition 
**************************************************************************/

	JvtUInt32 MspCallPsConnect( JvtUInt32 sim_type, const MspCallPsConnectReqParams * params, JvtUInt32 app_id )
	
	{

	   	MSP_Log_Func("===Entry point of MspCallPsConnect()===",0xFF,"JVT MSP APPLICATION");
			
	    if ( params == NULL )
	    {
		   return   MPS_ERROR_INVALID_PARAM;
	    }
	
	    if( !MpsCallPsCheckCid(params->cid))   
	    {
		return   MPS_ERROR_INVALID_PARAM;
	    }

	    MPS_ENTER_CFH_MUTEX;

	    if (!mpsIsConflictHandlingActive(sim_type, E_MSP_CONFLICT_HANDLING_TYPE_CALL_PS) == TRUE )
	    {
		MPS_LEAVE_CFH_MUTEX;
		return   MPS_ERROR_WRONG_STATE;
	    }
	
        if((JvtUInt32)MSPactivesessionTransaction== psCallInfo->pdp_context_id)
	    {
       	return MPS_ERROR_NOT_SUPPORTED;
	    }
	    L2_Msp_Sync_Activate_Req( sim_type, (PVOID)psCallInfo, (MSPHANDLER)MspCallPsConnectRspCb,L2_NW_CONNECT_REQ);

	    MSP_Log_Func("===Exit point of MspCallPsConnect()===",0xFF,"JVT MSP APPLICATION");
   return 0;
    }
	
/************************************************************************* 
		MspCallPsConnectRspCb- Function Definition 
**************************************************************************/

		
	void MspCallPsConnectRspCb( JvtUInt32 sim_type, const MspCallPsConnectRspCbParams *param, JvtUInt32 app_id )
   	{

	   	MSP_Log_Func("===Entry point of MspCallPsConnectRsponseParams()=== ",0xFF,"JVT MSP APPLICATION");
        if(param->result==L2_NW_CONNECT_SUCCESS)
		printf("\n***** connection succeeded****\n\n");
		else 
		printf("*** connection failed****\n\n"); 
        MSP_Log_Func("===Exit point of MspCallPsConnectRsponseParams()=== ",0xFF,"JVT MSP APPLICATION");
	
	}


	/************************************************************************* 
			MspCallPsDeactivate- Function Definition 
	**************************************************************************/

	JvtUInt32 MspCallPsDeactivate( JvtUInt32 sim_type, const MspCallPsDeactivateReqParams * params, JvtUInt32 app_id  )
		{
		   
	          MSP_Log_Func("===Entry point of MspCallPsDeactivate()===",0xFF,"JVT MSP APPLICATION");
					   
		      if ( params == NULL )
		         {
	               return   MPS_ERROR_INVALID_PARAM;
		         }
		      if(!MpsCallPsCheckCid(params->cid))   
	             {
		           return   MPS_ERROR_INVALID_PARAM;
	              }

	          MPS_ENTER_CFH_MUTEX;

	          if (!mpsIsConflictHandlingActive(sim_type, E_MSP_CONFLICT_HANDLING_TYPE_CALL_PS) == TRUE )
	             {
		        	MPS_LEAVE_CFH_MUTEX;
					return   MPS_ERROR_WRONG_STATE;
	    		 }
			  if((JvtUInt32)MSPactivesessionTransaction== psCallInfo->pdp_context_id)
	             {
       	          return MPS_ERROR_NOT_SUPPORTED;
	             }
			  MPS_ENTER_CFH_MUTEX;
	
             
			   
				   
			  L2_Msp_Sync_Activate_Req( sim_type, (PVOID)psCallInfo, (MSPHANDLER)MspCallPsDeactivateRspCb,L2_NW_DEACTIVATE_REQ);
		               
			  MSP_Log_Func("===Exit point of MspCallPsDeactivate()===",0xFF,"JVT MSP APPLICATION");
				   
	            return 0;			
				   
		}

	
	/************************************************************************* 
				MspCallPsDeactivateRspCb- Function Definition 
		**************************************************************************/
		
	void MspCallPsDeactivateRspCb( JvtUInt32 sim_type, const MspCallPsDeactivateRspCbParams * params, JvtUInt32 app_id  )

		{
		MSP_Log_Func("===Entry point of MspCallPsDeactivateRsponseParams()=== ",0xFF,"JVT MSP APPLICATION");
                  if(params->result==L2_NW_DEACTIVATE_CNF_SUCCESS)
                   
		MSP_Log_Func("===***Deactivation connection succeeded***=== ",0xFF,"JVT MSP APPLICATION");
		/*printf("\n***** Deactivation connection succeeded****\n\n");*/
                 
                      
		else 
		MSP_Log_Func("===***Deactivation connection failed***=== ",0xFF,"JVT MSP APPLICATION"); 
		JvtMemFree(psCallInfo);
                   MSP_Log_Func("===Exit point of MspCallPsDeactivateRsponseParams()=== ",0xFF,"JVT MSP APPLICATION");
                    
					/*psCallInfo=NULL;*/
		}
	


