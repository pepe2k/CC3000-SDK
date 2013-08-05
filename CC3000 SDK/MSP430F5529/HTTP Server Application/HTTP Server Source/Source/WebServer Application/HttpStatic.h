/*****************************************************************************
*
*  HttpStatic.h
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
#ifndef _HTTP_STATIC_H_
#define _HTTP_STATIC_H_

#include "HttpConfig.h"

#ifdef HTTP_CORE_ENABLE_STATIC

/** 
 * @defgroup HttpStatic Static request handler module
 * This module implements static content processing for HTTP requests.
 * All requests are handled by looking up the URL's resource in the flash database, and returning the content in the response.
 * Note this module is only compiled if HTTP_CORE_ENABLE_STATIC is defined in HttpConfig.h
 *
 * @{
 */

#include "HttpRequest.h"

/**
 * Initialize HttpStatic module state for a new request, and identify the request
 * This function examines the specified resource string, and looks it up in the Flash Database.
 * Note: During FlashDB lookup, ignore the query part (?) and anchor part (#) of URL
 * If found, it commits to process this request by returning nonzero. Otherwise it returns zero.
 * @param uConnection The number of the connection. This value is guaranteed to satisfy: 0 <= uConnection < HTTP_CORE_MAX_CONNECTIONS
 * @param resource The resource part of the URL, as specified by the browser in the request, including any query string (and hash). 
 *                 Note: The resource string exists ONLY during the call to this function. The string pointer should not be copied by this function.
 * @return nonzero if request is to be handled by this module. zero if not.
 */
int HttpStatic_InitRequest(uint16 uConnection, struct HttpBlob resource);

/**
 * Process a static-content HTTP request
 * This function is called after a request was already initialized, and a FlashDB content entry was identified during a call to HttpStatic_InitRequest()
 * This function calls HttpResponse_*() to send the content data to the browser.
 * @param request Pointer to all data available about the request
 * @return nonzero if request was handled. zero if not.
 */
void HttpStatic_ProcessRequest(struct HttpRequest* request);

/// @}

#endif // HTTP_CORE_ENABLE_STATIC

#endif // _HTTP_STATIC_H_
