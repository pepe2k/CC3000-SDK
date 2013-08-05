/*****************************************************************************
*
*  HttpString.c
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
#include "string.h"
#include <stdlib.h>
#include "HttpString.h"

/** 
 * @addtogroup HttpString
 * @{
 */

char digits[] = "0123456789";
char hexDigits[] = "0123456789abcdef";

int HttpString_strcmp(struct HttpBlob first, struct HttpBlob second)
{
    int min,res;
    if (first.uLength > second.uLength)
        min = second.uLength;
    else
        min = first.uLength;

    // Compare a common length which might be equal
    res = memcmp(first.pData, second.pData, min);
    if (res != 0)
        return res;

    // Common length is equal, so the longer blob is "larger"
    if (first.uLength > second.uLength)
        return 1;
    else if (first.uLength < second.uLength)
        return -1;
    else
        return 0;
}

static void ToLower(char * str, uint16 len)
{
    int i;
    for (i=0 ; i<=len; i++)
    {
        if (str[i]>='A' && str[i]<='Z')
            str[i] = str[i] + 32; 
    }
}

uint8* HttpString_nextToken(char* pToken, uint16 uTokenLength, struct HttpBlob blob)
{
    uint8* pch = blob.pData;
    struct HttpBlob partialBlob;
    struct HttpBlob token;
    token.pData = (uint8*)pToken;
    token.uLength = uTokenLength;
    ToLower((char *)blob.pData, blob.uLength);
    while (pch < blob.pData + blob.uLength)
    {
        // Calculate how many blob bytes we should search
        int nMaxCount = blob.uLength - (pch - blob.pData) - uTokenLength + 1;
        if (nMaxCount < 1)
            return NULL;

        // Search for the first character of the token
        pch = (uint8*)memchr(pch, pToken[0], nMaxCount);
        if (pch==NULL)
            return NULL;

        // Found first character, now compare the rest
        partialBlob.pData = pch;
        partialBlob.uLength = uTokenLength;
        if (HttpString_strcmp(token, partialBlob)==0)
            return pch;

        // Skip this byte, and look for the token in the rest of the blob
        ++pch;
    }
    return NULL;
}

uint32 HttpString_atou(struct HttpBlob string)
{
    return atoi((const char*)string.pData);
}

void HttpString_utoa(uint32 uNum, struct HttpBlob* pString)
{
    char* ptr;
    uint32 uTemp = uNum;

    // value 0 is a special format
    if (uNum == 0)
    {
        pString->uLength = 1;
        *pString->pData = '0';
        return;
    }

    // Find out the length of the number, in decimal base
    pString->uLength = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        pString->uLength++;
    }

    // Do the actual formatting, right to left
    uTemp = uNum;
    ptr = (char*)pString->pData + pString->uLength;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = digits[uTemp % 10];
        uTemp /= 10;
    }
}

void HttpString_htoa(uint32 uNum, struct HttpBlob* pString, uint8 bPadZero)
{
    uint8* ptr;
    uint32 uTemp = uNum;

    if (!bPadZero)
    {
        // value 0 is a special format
        if (uNum == 0)
        {
            pString->uLength = 1;
            *pString->pData = '0';
            return;
        }

        // Find out the length of the number, in hexadecimal base
        pString->uLength = 0;
        while (uTemp > 0)
        {
            uTemp /= 16;
            pString->uLength++;
        }
    }

    // Do the actual formatting, right to left
    uTemp = uNum;
    ptr = pString->pData + pString->uLength;
    while (ptr > pString->pData)
    {
        --ptr;
        *ptr = (uint8)hexDigits[uTemp % 16];
        uTemp /= 16;
    }
}
