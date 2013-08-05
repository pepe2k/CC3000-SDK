/*****************************************************************************
*
*  HttpAuth.h
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
#ifndef _HTTP_AUTH_H_
#define _HTTP_AUTH_H_

#include "HttpConfig.h"
#include "HttpString.h"
#include "HttpRequest.h"
#ifdef HTTP_CORE_ENABLE_AUTH

/** 
 * @defgroup HttpAuth HTTP Authentication
 * This module implements the HTTP digest access authentication routines.
 * Note this module is only compiled if HTTP_CORE_ENABLE_AUTH is defined in HttpConfig.h
 * 
 * When a "not authorized" response is sent to the client, the WWW-Authenticate header is built using HttpAuth_ResponseAuthenticate()
 * This in turn generates new nonce and opaque values which will be used for authentication.
 * Note that since only a single nonce is kept, only one client may ever be authenticated simultaneously.
 * When another request with Authorization header is received, it is verified using HttpAuth_RequestAuthenticate()
 * If all authentication tests pass, then the appropriate flag is set in the request to indicate that.
 *
 * @{
 */

/**
 * Initialize the authentication module, so that it accepts the specified username and password
 * This function should be called during server initialization in order to set initial user credentials
 * This function may then be called at any time during the operation of the server in order to set different user credentials
 * @param username The authorized user's username 
 * @param password The authorized user's password
 */
void HttpAuth_Init(struct HttpBlob username, struct HttpBlob password);

/**
 * Builds and returns the WWW-Authenticate response header.
 * This implies generating a new nonce, etc.
 * Notes about return value:
 *     Upon entry, pWWWAuthenticate should point to the place in the packet-send buffer where the header needs to be generated, and also specify the maximum amount of bytes available for the header at that place
 *     Upon return, pWWWAuthenticate points to the same location, but specifies the actual length of the header.
 *     If the returned length is 0, this means that there was not enough room in the buffer for the header.
 *     In such a case, the core may try again with a larger buffer
 * @param pRequest All data about the request
 * @param[in,out] pWWWAuthenticate On entry specifies the memory location to build the header at, and the maximum size. On return, specifies the same location and the actual size of the header line
 */
void HttpAuth_ResponseAuthenticate(struct HttpRequest* pRequest, struct HttpBlob* pWWWAuthenticate);

/**
 * Check the authentication header in a request, and either authorize the request or deny it
 * If the authorization succeeds, then HTTP_REQUEST_FLAG_AUTHENTICATED is added to the request flags
 * @param pRequest All data about the request to authorize
 * @param authorization The full string of the Authorization header
 */
void HttpAuth_RequestAuthenticate(struct HttpRequest* pRequest, struct HttpBlob authorization);

/// @}

#endif // HTTP_CORE_ENABLE_AUTH

#endif // _HTTP_AUTH_H_
