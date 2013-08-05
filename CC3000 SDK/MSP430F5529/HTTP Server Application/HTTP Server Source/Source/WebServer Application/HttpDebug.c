/*****************************************************************************
*
*  HttpDebug.c
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
#include "HttpDebug.h"


#ifdef HTTP_CORE_ENABLE_DEBUG
#ifdef WIN32

#include <stdio.h>
#include <stdarg.h>

void HttpDebug(const char* pFormat, ...)
{
    char buffer[2048];
    va_list argptr;
    va_start(argptr, pFormat);

    vsprintf_s(buffer, sizeof(buffer), pFormat, argptr);

    va_end(argptr);

    HttpDebugStr(buffer);
}

void HttpDebugStr(const char* pString)
{
    printf("%s\n", pString);
}

void HttpAssert(int condition)
{
    if (condition)
        return;

    printf("ASSERTION!\n");
    while (1)
    {
    }
}

#endif
#else
void HttpDebug(const char* pFormat, ...)
{

}
void HttpDebugStr(const char* pString)
{

}
void HttpAssert(int condition)
{

}
#endif
