/*****************************************************************************
*
*  FlashDB.h
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

#pragma once


#define FLASHDB_FLAG_COMPRESSED     (1 << 0)
/// Denotes authentication required
#define FLASHDB_FLAG_REQUIRE_AUTH   (1 << 1)

/// This database flag indicates that all resource strings are to be matched with a case-sensitive string comparison function
#define FLASH_DB_FLAG_CASE_SENSITIVE (1 << 0)

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;

/**
 * A structure to hold a string or buffer which is not null-terminated
 */
struct HttpBlob
{
    uint16 uLength;
    uint8* pData;
};

#pragma pack(push)
#pragma pack(1)

struct FlashDBHeader
{
    /// Amount of content type entries
    uint16 uContentTypeAmount;
    /// Offset of content-type entry list
    uint16 uContentTypeOffset;
    /// Amount of content entries
    uint16 uContentAmount;
    /// Offset of content entry list
    uint16 uContentOffset;
    /// Database flags. See FLASH_DB_FLAG_*
    uint16 uFlags;
};

/**
 * The header structure for a content entity in the database
 */
struct FlashDBContentTypeEntry
{
    /// Length of the value string. Should be 255 characters at most.
    uint16 uValueLength;
    /// Offset of the value string
    uint16 uValueOffset;
};

/**
 * The header structure for a content entity in the database
 */
struct FlashDBContentEntry
{
    /// Length of the resource string. Should be 255 characters at most.
    uint16 uResourceLength;
    /// Offset of the resource string
    uint32 uResourceOffset;
    /// Flags. See FLASHDB_FLAG_*
    uint16 uFlags;
    /// Index of content type entry for this resource, in the content type list
    uint16 uContentType;
    /// Length (in bytes) of the content blob
    uint16 uContentLength;
    /// Offset of the content blob in the database
    uint32 uContentOffset;
};

#pragma pack(pop)


/**
 * The layout of the database file is as follows:
 *    Database Header
 *    Content type entry 1
 *    Content type entry 2
 *      and so on...
 *    Content entry 1
 *    Content entry 2
 *      and so on...
 *    Content type string 1
 *    Content type string 2
 *      and so on...
 *    Content blob 1
 *    Content blob 2
 *      and so on...
 *
 * Note: All offsets in the structures are absolute offsets, meaning they are relative to the database base pointer, as passed to FlashDB_Init()
 */
