/*****************************************************************************
*
*  HttpResponse.h
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
#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "Common.h"
#include "HttpString.h"

/** 
 * @defgroup HttpResponse HTTP Response modules
 * This module implements routines to allow content handler modules to build and send HTTP responses back to the client.
 * There are two layers in this module:
 * - The lower layer consists of HttpResponse_Headers() and HttpResponse_Content(). These routines allow the caller to specify all details of the response.
 * - The higher layer consists of HttpResponse_Canned*(). These routines emit canned (pre-made) responses, such as redirects and errors, which are useful in many situations.
 *
 * @{
 */

/**
 * @defgroup HttpStatus Supported HTTP status codes
 * @{
 */
#define HTTP_STATUS_OK                 200
#define HTTP_STATUS_REDIRECT_PERMANENT 301
#define HTTP_STATUS_REDIRECT_TEMPORARY 302
#define HTTP_STATUS_ERROR_UNAUTHORIZED 401
#define HTTP_STATUS_ERROR_NOT_FOUND    404
#define HTTP_STATUS_ERROR_NOT_ACCEPTED 406
#define HTTP_STATUS_ERROR_INTERNAL     500

/// @}

/// The response data is gzip-compressed. Implies the header Content-Encoding: gzip
#define HTTP_RESPONSE_FLAG_COMPRESSED   (1 << 0)

/**
 * Respond with the specified HTTP status and headers
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param uHttpStatus The HTTP status number to response with. Must be one of HTTP_STATUS_*
 * @param uFlags Flags which are manifested in the response headers. See HTTP_RESPONSE_FLAG_*
 * @param uContentLength The total length of content which will be sent via HttpResponse_Content()
 * @param contentType The content type string, or NULL to omit the content type
 * @param location A string which will be used for the Location header, or NULL to omit the Location header
 */
void HttpResponse_Headers(uint16 uConnection, uint16 uHttpStatus, uint16 uFlags, uint32 uContentLength, struct HttpBlob contentType, struct HttpBlob location);

/**
 * Retrieves the pointer and size of the packet-send buffer
 * This function should be called by content handlers that wish to use the already-allocated packet-send buffer in calls to HttpResponse_Content()
 * @param[out] pPacketSendBuffer Returns the pointer and size of the packet-send buffer
 */
void HttpResponse_GetPacketSendBuffer(struct HttpBlob* pPacketSendBuffer);

/**
 * Send response content to the client.
 * This function may be called more than once, until all the content is sent.
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param content Content blob to send to the client.
 */
void HttpResponse_Content(uint16 uConnection, struct HttpBlob content);

/**
 * Sends a canned response, with an HTTP redirect
 * This function should be called *instead* of HttpResponse_Status(), HttpResponse_Headers() and HttpResponse_Content()
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param pLocation The redirect URL
 * @param bPermanent zero for temporary redirect, nonzero for permanent redirect
 */
void HttpResponse_CannedRedirect(uint16 uConnection, struct HttpBlob location, uint16 bPermanent);

/**
 * Sends a canned response, with an error message
 * This function should be called *instead* of HttpResponse_Status(), HttpResponse_Headers() and HttpResponse_Content()
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param uHttpStatus The HTTP error status. Must be one of HTTP_STATUS_ERROR_*
 */
void HttpResponse_CannedError(uint16 uConnection, uint16 uHttpStatus);

/// @}

#endif //_HTTP_RESPONSE_H_
