/*****************************************************************************
*
*  HttpDynamic.c
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
#include "HttpDynamic.h"
#include "HttpDynamicHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpCore.h"
#include "string.h"

#ifdef HTTP_CORE_ENABLE_DYNAMIC

/** 
 * @defgroup HttpDynamic Dynamic request handler module
 * This module implements dynamic content processing for HTTP requests.
 * All requests are handled by C code functions, and the response contents is returned via HttpResopnse routines
 * Note this module is only compiled if HTTP_CORE_ENABLE_DYNAMIC is defined in HttpConfig.h
 *
 * @{
 */

char dynamicResourceName[HTTP_DYNAMIC_NUM_OF_RESOURCES][HTTP_DYNAMIC_MAX_RESOURCE_LEN] = {{"led"},
											 {"wheel"},
											 };
											

char dynamicLedParam1[5] = {"num="};
char dynamicLedParam2[7] = {"action="};

/* text/html is currently the blob for content type */
uint8 uDynamicContentType[10] =  "text/html";

uint8 uDynamicContentBody[HTTP_DYNAMIC_NUM_OF_RESOURCES][HTTP_DYNAMIC_CONTENT_BODY_LEN];

PerConnDynamicContent dynamicContent[HTTP_CORE_MAX_CONNECTIONS];


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
int HttpDynamic_InitRequest(uint16 uConnection, struct HttpBlob resource)
{
/*	Resource parsing follows the below assumptions:
	1. resource name is expected prior to query sign (just like a static page)
	2. query sign is expected. If not present, 0 is returned
	3. parameter name followed by a '=' sign is searched for. If not fully present, 0 is returned.
	4. parameters may arrive unordered 	*/
	
	uint16 	uLoopCounter;
	uint8	*cStr;
	struct HttpBlob subBlob;

	/* look for known resource names according to dynamicResourceName[][]*/
	for (uLoopCounter = 0; uLoopCounter < HTTP_DYNAMIC_NUM_OF_RESOURCES; uLoopCounter++)
	{
		if (HttpString_nextToken(dynamicResourceName[uLoopCounter],  strlen(dynamicResourceName[uLoopCounter]), resource) != NULL)
			break;
	}

	/* dynamic resource not found */
	if (uLoopCounter == HTTP_DYNAMIC_NUM_OF_RESOURCES)
		return 0;

	/* if got here, resource name is found on uLoopCounter index */
	/* find "query" sign */
	if (NULL == HttpString_nextToken("?", 1, resource))
		return 0;

	/* find "=" signs preceded by the parameter name */
	switch ((enum HttpDynamicNumOfResources)uLoopCounter)
	{
		case LED:
		{
			/* search the LED number */
			if ((cStr = HttpString_nextToken(dynamicLedParam1, HTTP_DYNAMIC_LED_NUM_LEN, resource)) != NULL)
			{
				dynamicContent[uConnection].dynamicHandlerInParam.uLedParam.uLedNumber = (enum HttpDynamicLedNumber)(*(cStr + HTTP_DYNAMIC_LED_NUM_LEN) - '0');
			}
			else
			{
				return 0;
			}

			/* search the LED action */
			if ((cStr	= HttpString_nextToken(dynamicLedParam2, HTTP_DYNAMIC_LED_ACTION_LEN, resource)) != NULL)
			{
				/* try to match LED action to the expected value exactly! */
				subBlob.pData = cStr + HTTP_DYNAMIC_LED_ACTION_LEN;
				subBlob.uLength = 6;
				if (HttpString_nextToken("toggle", 6, subBlob) != NULL)
					dynamicContent[uConnection].dynamicHandlerInParam.uLedParam.uLedAction = toggle;
				else
				{
                                    subBlob.uLength = 2;
                                    if (HttpString_nextToken("on", 2, subBlob) != NULL)
						dynamicContent[uConnection].dynamicHandlerInParam.uLedParam.uLedAction = ON;
                                    else
                                    {
						subBlob.uLength = 3;
						if (HttpString_nextToken("off", 3, subBlob) != NULL)
							dynamicContent[uConnection].dynamicHandlerInParam.uLedParam.uLedAction = OFF;
						else
							return 0;
                                    }
                                }
			}
			else
			{
				return 0;
			}
				
			dynamicContent[uConnection].pDynamicHandler = HttpDynamicHandler_ActOnLED;
			dynamicContent[uConnection].resourceType = LED;
			break;
		}
                
        case WHEEL:
        {
                  if ((cStr	= HttpString_nextToken(dynamicLedParam2, HTTP_DYNAMIC_LED_ACTION_LEN, resource)) != NULL)
                  {
                    subBlob.pData = cStr + HTTP_DYNAMIC_LED_ACTION_LEN;
                    subBlob.uLength = 9;
                    if (HttpString_nextToken("getstatus", 9 , subBlob) == NULL)
                    {
                      return 0;
                    }
                  }
                  else
                      return 0;
                  
                  dynamicContent[uConnection].pDynamicHandler = HttpDynamicHandler_GetWheelValue;
                  dynamicContent[uConnection].resourceType = WHEEL;
                  break;
        	}
		}

	return 1;
}

/**
 * Process a dynamic-content HTTP request
 * This function is only be called by the core, if HttpDynamic_InitRequest() returns nonzero.
 * This function processes the specified HTTP request, and send the response on the connection specified by request->uConnection.
 * This function must call the HttpResponse_*() functions in order to send data back to the browser.
 * Please refer to HttpResponse.h for more information.
 * @param request Pointer to all data available about the request
 */
void HttpDynamic_ProcessRequest(struct HttpRequest* request)
{
	struct HttpBlob contentType, nullBlob = {0, NULL};
	struct HttpBlob contentBody;
	int count;

	
	/* if HTTP_REQUEST_FLAG_AUTHENTICATED is not set and FLASHDB_FLAG_REQUIRE_AUTH is set then 
				call HttpResponse_CannedError() with status HTTP_STATUS_ERROR_UNAUTHORIZED	*/
	if (((request->uFlags & HTTP_REQUEST_FLAG_AUTHENTICATED) == 0))
	{
	 	/* call HttpResponse_CannedError with 500 ERROR_INTERNAL  */
		HttpResponse_CannedError(request->uConnection, HTTP_STATUS_ERROR_UNAUTHORIZED);
		return;
	}

	/* intialize the content body */
	contentBody.pData =uDynamicContentBody[request->uConnection];
    	contentBody.uLength = 0;

	contentType.pData = uDynamicContentType;
	contentType.uLength = 9;

	/* 1. call the dynamic handler as parsed in the init phase */
	dynamicContent[request->uConnection].pDynamicHandler(dynamicContent[request->uConnection].dynamicHandlerInParam, &dynamicContent[request->uConnection].dynamicHandlerOutParam);
	/* config the ContentLength according to the resource type */
	switch (dynamicContent[request->uConnection].resourceType)
	{
		case LED:
		{
                          contentBody.pData = "Done";
                          contentBody.uLength = 4;
                     
                          
			break;
		}
		case WHEEL:
		{
                        char tempdata[6];
                        struct HttpBlob tempContent;
                        tempContent.pData = (uint8 *)tempdata;
                        HttpString_utoa((uint32)dynamicContent[request->uConnection].dynamicHandlerOutParam.sWheelParam.uWheelPosition, &contentBody);
                        *(contentBody.pData + contentBody.uLength) = ':';
                        contentBody.uLength +=1;
                        HttpString_utoa((uint32)dynamicContent[request->uConnection].dynamicHandlerOutParam.sWheelParam.uWheelValue, &tempContent);
                        for(count=0; count<tempContent.uLength; count++)
                        {
                          *(contentBody.pData + (count+contentBody.uLength)) = *tempContent.pData;
                          tempContent.pData++;
                        }
                        contentBody.uLength += tempContent.uLength;
                        *(contentBody.pData + contentBody.uLength) = ':';
                        *(contentBody.pData + contentBody.uLength + 1) = dynamicContent[request->uConnection].dynamicHandlerOutParam.sWheelParam.uLedDummyOut;
                        contentBody.uLength += 2;
			break;
		}
	}
				
	/* 2. fill the header response (HTTP_RESPONSE_FLAG_COMPRESSED is not set for dynamic) */
	HttpResponse_Headers(request->uConnection, HTTP_STATUS_OK, 0, contentBody.uLength, contentType, nullBlob);

	/* 3. fill the content response (if exists) */
	if (contentBody.uLength != 0)
	{
		HttpResponse_Content(request->uConnection, contentBody);
	}

}


/// @}

#endif // HTTP_CORE_ENABLE_DYNAMIC
