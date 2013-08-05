/*****************************************************************************
*
*  HTTPString.h
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
#ifndef _HTTP_STRING_H_
#define _HTTP_STRING_H_

/** 
 * @defgroup HttpString String routines helper module
 * This module implements some string and buffer-related helper routines
 *
 * @{
 */

#include "Common.h"

/**
 * A structure to hold a string or buffer which is not null-terminated
 */
#ifdef __CCS__
struct __attribute__ ((__packed__)) HttpBlob
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct HttpBlob
#endif
{
    uint16 uLength;
    uint8* pData;
};

/**
 * Utility macro which sets an existing blob 
 */
#define HttpBlobSetConst(blob, constString) \
    {   \
        (blob).pData = (uint8*)constString; \
        (blob).uLength = sizeof(constString) - 1; \
    }

/**
 * Perform string comparison between two strings.
 * @param first Pointer to data about the first blob or string
 * @param second Pointer to data about the second blob or string
 * @return zero if equal, positive if first is later in order, negative if second is later in order
 */
int HttpString_strcmp(struct HttpBlob first, struct HttpBlob second);

/**
 * return pointer to the next token
 * @param token Pointer to data of the first token
 * @param blob Pointer to data of the search string/blob
 * @return pointer if found, otherwize NULL
 */
uint8* HttpString_nextToken(char* pToken, uint16 uTokenLength, struct HttpBlob blob);

/**
 * Parse a string representation of an unsigned decimal number
 * Stops at any non-digit character.
 * @param string The string to parse
 * @return The parsed value
 */
uint32 HttpString_atou(struct HttpBlob string);

/**
 * Format an unsigned decimal number as a string
 * @param uNum The number to format
 * @param[in,out] pString A string buffer which returns the formatted string. On entry, uLength is the maximum length of the buffer, upon return uLength is the actual length of the formatted string
 */
void HttpString_utoa(uint32 uNum, struct HttpBlob* pString);

/**
 * Format an unsigned decimal number as an hexdecimal string
 * @param uNum The number to format
 * @param bPadZero nonzero to pad with zeros up to the string length, or zero to use minimal length required to represent the number
 * @param[in,out] pString A string buffer which returns the formatted string. On entry, uLength is the maximum length of the buffer, upon return uLength is the actual length of the formatted string
 */
void HttpString_htoa(uint32 uNum, struct HttpBlob* pString, uint8 bPadZero);

/// @}

#endif // _HTTP_STRING_H_
