/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 JV Technologies 
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to JVT.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name	:	"Jvt_Msp_Trace.c" $
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

#include "Jvt_Msp_Types.h"
#include "Jvt_Msp_Trace.h"


static const jvt_s8 *EventIdToEventString[MSP_LAST_EVENT+1] =
{
   "MSP_TRACE_CALL_PS",
   	
   "MSP_TRACE_CALL_CS"  
};

/*==========================================================================
 * 	Trace function Definition to print the data of Application parameters
 *==========================================================================*/

void MSP_Trace( MspTraceTypes type, MspTraceId id, const char * format, ... )
{     
	va_list args;

	jvt_s8 trace_buf[TRACE_BUFFER_LENGTH];

	jvt_s32 pos;

	const jvt_s8 *EventName = EventIdToEventString[id];

	/*copy Application Name*/

	pos = strlen(EventName);

	memcpy((void*)&trace_buf[0],EventName,pos);

	/*copy arguments*/

	va_start( args, format );

	va_end(args);

	trace_buf[TRACE_BUFFER_LENGTH-1]='\0';

	printf("%s",trace_buf);
	      
	}

/*=========================================
 * 	log function to print normal data
 *=========================================*/
 
void MSP_Log_Func(const char *str, int var, const char *str_id)
{
	jvt_s8 text[256];

	if (var == 0xFF)
	{
		sprintf(text, "%s  <------ [%s] \n\n", str, str_id);
	}
	else
	{
		sprintf(text, "%s %d <------ [%s] \n\n", str, var, str_id);
	}
	printf("%s" ,text);
}