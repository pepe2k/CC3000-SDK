/*****************************************************************************
*
*  HttpCore.h
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
#ifndef _HTTP_CORE_H_
#define _HTTP_CORE_H_

/** 
 * @defgroup HttpCore HTTP Server core
 * This module implements the HTTP server core
 *
 * @{
 */

/// The TCP port to listen on.
#define HTTP_CORE_SERVER_PORT 80

/// Maximum number of concurrent client connections. This should be one less than the CC3000 maximum number of sockets
#define HTTP_CORE_MAX_CONNECTIONS 3

/// Maximum size for a received or sent packet. Two buffers of this size must be allocated at all times.
#define HTTP_CORE_MAX_PACKET_SIZE_RECEIVED 912

/// Maximum size for a received or sent packet. Two buffers of this size must be allocated at all times.
#define HTTP_CORE_MAX_PACKET_SIZE_SEND 912

/// Maximum length of header line which might be buffered, per connection, if a line is broken between packets
#define HTTP_CORE_MAX_HEADER_LINE_LENGTH 320

/**
 * Initialize and start the HTTP server.
 * The Wifi interface of the CC3000 chip should be initialized by now, and connected to the network
 */
void HttpServerInitAndRun();

/// @}

#endif //_HTTP_CORE_H_
