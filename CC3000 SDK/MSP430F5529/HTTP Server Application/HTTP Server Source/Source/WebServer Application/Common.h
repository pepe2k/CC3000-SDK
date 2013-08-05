/*****************************************************************************
*
*  Common.h
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
#ifndef _TINY_WEB_SERVER_COMMON_H_
#define _TINY_WEB_SERVER_COMMON_H_

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned long uint32;

//#define HTTP_WEB_SERVER_ON_BRIDGE

#ifndef NULL
#define NULL 0
#endif

#ifndef __IAR_SYSTEMS_ICC__
#define __no_init
#endif

#ifdef WIN32
typedef int socklen_t;
#endif //WIN32

#if !defined(WIN32) || defined(HTTP_WEB_SERVER_ON_BRIDGE)
#define _TIME_T_DEFINED
#define SOCKET long
#define INVALID_SOCKET (-1)
#endif //WIN32

#define CC3000_INVALID_SOCKET ((SOCKET)-2)

#define FALSE 0
#define TRUE  1


#define ESUCCESS        0
#define SMART_CONFIG_SET 0xAA


#endif //_TINY_WEB_SERVER_COMMON_H_
