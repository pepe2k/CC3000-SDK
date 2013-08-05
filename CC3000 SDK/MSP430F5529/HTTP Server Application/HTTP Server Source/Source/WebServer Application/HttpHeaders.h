/*****************************************************************************
*
*  HttpHeaders.h
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
#ifndef _HTTP_HEADERS_H_
#define _HTTP_HEADERS_H_

/** 
 * @defgroup HttpHeaders HTTP header strings
 * This header file predefines various HTTP request and response header strings
 *
 * @{
 */

#include "Common.h"

char    HTTP_HEADER_DELIMITER[]         =  "\r\n";

// HTTP method line strings
char    HTTP_METHOD_GET[]               =  "get";
char    HTTP_METHOD_POST[]              =  "post";
char    HTTP_VERSION_1P1[]              =  "http/1.1";
char    HTTP_VERSION_1P0[]              =  "http/1.0";

// HTTP request/response header line strings
char    HTTP_CONTENT_TYPE[]             =  "content-type";
char    HTTP_CONTENT_LENGTH[]           =  "content-length";
char    HTTP_ACCEPT_ENCODING[]          =  "accept-encoding";
char    HTTP_AUTHORIZATION[]            =  "authorization";
char    HTTP_CONNECTION_CLOSE[]         =  "connection";
char    HTTP_GZIP[]                     =  "gzip";
char    HTTP_CLOSE[]                    =  "close";
char    HTTP_LOCATION[]                 =  "location";
char    HTTP_CONTENT_ENCODING[]         =  "content-encoding";

// HTTP response status line strings
char    HTTP_STATUS_OK_STR[]                   =    "ok";
char    HTTP_STATUS_REDIRECT_PERMANENT_STR[]   =    "moved permanently";
char    HTTP_STATUS_REDIRECT_TEMPORARY_STR[]   =    "moved temporarily";
char    HTTP_STATUS_ERROR_UNAUTHORIZED_STR[]   =    "unauthorized";
char    HTTP_STATUS_ERROR_NOT_FOUND_STR[]      =    "not found";
char    HTTP_STATUS_ERROR_NOT_ACCEPTED_STR[]   =    "not accepted";
char    HTTP_STATUS_ERROR_INTERNAL_STR[]       =    "internal server error";

#endif //_HTTP_HEADERS_H_