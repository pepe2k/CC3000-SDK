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
#ifndef _FLASH_DB_H_
#define _FLASH_DB_H_

#include "HttpString.h"

/** 
 * @defgroup FlashDB Flash-storage database management
 * This module implements a database for static content, which is either stored on Flash or read-only memory of some kind.
 * The database is pre-compiled using external tools and is stored as a "flat file" in memory.
 * The contents of the database are:
 * - A list of content-type strings
 * - A sorted-list of static content entities. Each entity has:
 *   - URL Resource of the content (sort and lookup key)
 *   - Content type number (index into the list of content-types)
 *   - Content binary data
 *   - Additional metadata flags (Compressed, Requires-authentication)
 *
 * @{
 */

/// Denotes gzip-compressed content
#define FLASHDB_FLAG_COMPRESSED     (1 << 0)
/// Denotes authentication required
#define FLASHDB_FLAG_REQUIRE_AUTH   (1 << 1)

/**
 * A structure to hold all data about content which was found in a database lookup
 */
#ifdef __CCS__
struct __attribute__ ((__packed__)) FlashDBContent
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct FlashDBContent
#endif
{
    /// Flags. See FLASHDB_FLAG_*
    uint16 uFlags;
    /// Index of content type entry for this resource, in the content type list
    uint16 uContentType;
    /// The content blob's pointer and length
    struct HttpBlob contentBlob;
};

/**
 * Initialize the flash database module
 * @param pDatabase Pointer to the beginning of the database in flash memory
 */
void FlashDB_Init();

/**
 * Find a content-type in the database
 * @param index The index of the content type in the database
 * @param[out] pResult Returns the string and its length
 * @return nonzero if successful. zero if index out of range.
 */
int FlashDB_FindContentType(uint16 index, struct HttpBlob* pResult);

/**
 * Lookup content in the database, given a resource identifier
 * @param pResource The resource as given in the HTTP request
 * @param[out] pResult Returns the content and its metadata
 * @return nonzero if successful. zero if such a resource was not found.
 */
int FlashDB_FindContent(struct HttpBlob pResource, struct FlashDBContent* pResult);

/// @}

#endif // _FLASH_DB_H_
