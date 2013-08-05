/*****************************************************************************
*
*  HttpDynamic.h
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef _HTTP_DYNAMIC_H_
#define _HTTP_DYNAMIC_H_

#include "HttpConfig.h"

#ifdef HTTP_CORE_ENABLE_DYNAMIC

/** 
 * @defgroup HttpDynamic Dynamic request handler module
 * This module implements dynamic content processing for HTTP requests.
 * All requests are handled by C code functions, and the response contents is returned via HttpResopnse routines
 * Note this module is only compiled if HTTP_CORE_ENABLE_DYNAMIC is defined in HttpConfig.h
 *
 * @{
 */

#include "HttpRequest.h"

#define HTTP_DYNAMIC_NUM_OF_RESOURCES		2
#define HTTP_DYNAMIC_MAX_RESOURCE_LEN		14
#define HTTP_DYNAMIC_LED_NUM_LEN			4
#define HTTP_DYNAMIC_LED_ACTION_LEN			7
#define HTTP_DYNAMIC_TEMPR_UNIT_LEN			5
#define HTTP_DYNAMIC_CONF_PROTO_LEN			6
#define HTTP_DYNAMIC_CONTENT_BODY_LEN		17


enum HttpDynamicNumOfResources
{
	LED,
	WHEEL,
};

enum HttpDynamicLedNumber
{
    NOLED,
    LED_1,
    LED_2,
    LED_3,
    LED_4,
    LED_5,
    LED_6,
    LED_7,
    LED_8
};

enum HttpDynamicLedAction
{
    OFF,
    ON,
    toggle,
};

enum HttpDynamicWheelAction
{
    NONE,
   GETSTATUS
};


/* input params structure */
struct HttpDynamicLedInParam
{
	enum HttpDynamicLedNumber 	uLedNumber;
	enum HttpDynamicLedAction   	uLedAction;
};

struct HttpDynamicWheelInParam
{
	enum HttpDynamicWheelAction	uWheelAction;
};


typedef union
{
	struct HttpDynamicLedInParam		uLedParam;
	struct HttpDynamicWheelInParam	        uWheelParam;
}inputParams;

struct HttpDynamicLedOutParam
{
	uint8	uLedDummyOut;
};

struct HttpDynamicWheelOutParam
{
        uint8	uLedDummyOut;
        uint8   uWheelPosition;
	uint16	uWheelValue;
};


typedef union
{
	struct HttpDynamicLedOutParam		uLedParam;
	struct HttpDynamicWheelOutParam	sWheelParam;
}outputParams;

typedef struct 
{
	inputParams	dynamicHandlerInParam;
	outputParams	dynamicHandlerOutParam;
	enum HttpDynamicNumOfResources	resourceType;
	void (*pDynamicHandler)(inputParams,  outputParams*);
}PerConnDynamicContent;


/**
 * Initialize HttpDynamic module state for a new request, and identify the request
 * This function must examine the specified resource string and determine whether it can commit to process this request
 * Also, if the resource string includes any information that this module needs in order to process the request (such as the contents of the query string)
 * then it is the responsibility of this function to parse this information and store it in a connection-specific struct.
 * If this function returns nonzero, then the core will call HttpDynamic_ProcessRequest() with the rest of the request details.
 * @param uConnection The number of the connection. This value is guaranteed to satisfy: 0 <= uConnection < HTTP_CORE_MAX_CONNECTIONS
 * @param resource The resource part of the URL, as specified by the browser in the request, including any query string (and hash)
 *                 Note: The resource string exists ONLY during the call to this function. The string pointer should not be copied by this function.
 * @return nonzero if request is to be handled by this module. zero if not.
 */
int HttpDynamic_InitRequest(uint16 uConnection, struct HttpBlob resource);

/**
 * Process a dynamic-content HTTP request
 * This function is only be called by the core, if HttpDynamic_InitRequest() returns nonzero.
 * This function processes the specified HTTP request, and send the response on the connection specified by request->uConnection.
 * This function must call the HttpResponse_*() functions in order to send data back to the browser.
 * Please refer to HttpResponse.h for more information.
 * @param request Pointer to all data available about the request
 */
void HttpDynamic_ProcessRequest(struct HttpRequest* request);

/// @}

#endif // HTTP_CORE_ENABLE_DYNAMIC

#endif // _HTTP_DYNAMIC_H_
