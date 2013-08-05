/*****************************************************************************
*
*  FlashDB.c
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
#include "Common.h"
#include "FlashDB.h"
#include "HttpString.h"
#include "string.h"


/// This database flag indicates that all resource strings are to be matched with a case-sensitive string comparison function
#define FLASH_DB_FLAG_CASE_SENSITIVE (1 << 0)

/**
 * The header structure of the flash database
 */
#ifdef __CCS__
struct __attribute__ ((__packed__)) FlashDBHeader
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct FlashDBHeader
#endif
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
#ifdef __CCS__
struct __attribute__ ((__packed__)) FlashDBContentTypeEntry
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct FlashDBContentTypeEntry
#endif
{
    /// Length of the value string. Should be 255 characters at most.
    uint16 uValueLength;
    /// Offset of the value string
    uint16 uValueOffset;
};

/**
 * The header structure for a content entity in the database
 */
#ifdef __CCS__
struct __attribute__ ((__packed__)) FlashDBContentEntry
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct FlashDBContentEntry
#endif
{
    /// Length of the resource string. Should be 255 characters at most.
    uint16 uResourceLength;
    /// Offset of the resource string
    //uint32 uResourceOffset;
    unsigned long uResourceOffset;
    /// Flags. See FLASHDB_FLAG_*
    uint16 uFlags;
    /// Index of content type entry for this resource, in the content type list
    uint16 uContentType;
    /// Length (in bytes) of the content blob
    uint16 uContentLength;
    /// Offset of the content blob in the database
    //uint32 uContentOffset;
    unsigned long uContentOffset;
};

extern const char FlashDBContentData[];

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

/* this global shows a sample how static pages/files are stored in the memory */
   
#if 0
struct StaticExamplePages
{
	struct FlashDBHeader				header;
	struct FlashDBContentTypeEntry		contentTypeList[];
	struct FlashDBContentEntry			ContentList[];
	char						contentType[][];
	char						resource[][];
	char					        content[][];
};
#endif

struct FlashDBHeader	*gFlashDB;

/**
 * Initialize the flash database module
 * @param pDatabase Pointer to the beginning of the database in flash memory
 */
void FlashDB_Init()
{
        gFlashDB = (struct FlashDBHeader *)FlashDBContentData;
}

/**
 * Find a content-type in the database
 * @param index The index of the content type in the database
 * @param[out] pResult Returns the string and its length
 * @return nonzero if successful. zero if index out of range.
 */
int FlashDB_FindContentType(uint16 index, struct HttpBlob* pResult)
{
	struct FlashDBContentTypeEntry	*pContentTypeEntryList;

	if (index > gFlashDB->uContentTypeAmount)
		return 0;
	
	pContentTypeEntryList = (struct FlashDBContentTypeEntry *)((uint8 *)gFlashDB + gFlashDB->uContentTypeOffset);
	pContentTypeEntryList = (struct FlashDBContentTypeEntry *)(pContentTypeEntryList + index);
	pResult->uLength = pContentTypeEntryList->uValueLength;
	pResult->pData =  (uint8 *)((uint8 *)gFlashDB + pContentTypeEntryList->uValueOffset);

	return 1;
}

/**
 * Lookup content in the database, given a resource identifier
 * @param pResource The resource as given in the HTTP request
 * @param[out] pResult Returns the content and its metadata
 * @return nonzero if successful. zero if such a resource was not found.
 */
int FlashDB_FindContent(struct HttpBlob pResource, struct FlashDBContent* pResult)
{
	/* it is assumed that the resource list is sorted */
	struct FlashDBContentEntry *pContentFieldStart;
	struct FlashDBContentEntry *pContentFieldMoving;
    struct HttpBlob MainPage;
	uint16	uLowSearchIndex = 0;
	uint16	uMidSearchIndex;
	uint16	uHighSearchIndex;
	int		uComparison;
	int16	found = -1;	/* not found */
	struct HttpBlob	ResourceBlob;

	uHighSearchIndex = gFlashDB->uContentAmount - 1;
	pContentFieldStart = (struct FlashDBContentEntry *)((uint8 *)gFlashDB + gFlashDB->uContentOffset);

	while (uLowSearchIndex <= uHighSearchIndex)
	{
		uMidSearchIndex = (uLowSearchIndex + uHighSearchIndex)/2;
		pContentFieldMoving = pContentFieldStart + uMidSearchIndex;
		ResourceBlob.pData = (uint8 *)((uint8 *)gFlashDB + pContentFieldMoving->uResourceOffset);
		ResourceBlob.uLength = pContentFieldMoving->uResourceLength;
                if(pResource.uLength ==1 && *(pResource.pData)=='/')
                {
                  MainPage.uLength = 11;
                  MainPage.pData = "/index.html";
                  uComparison = HttpString_strcmp(MainPage, ResourceBlob);
                }
                else
                  uComparison = HttpString_strcmp(pResource, ResourceBlob);
		/* found the exact blob */
		if (uComparison == 0)
		{
			found = 1;
			break;
		}
		/* the requested blob is "larger" than the current blob */
		else if (uComparison > 0)
		{
			uLowSearchIndex = uMidSearchIndex + 1;
		}
		/* the requested blob is "smaller" than the current blob */
		else
		{
			uHighSearchIndex = uMidSearchIndex - 1;
		}
	}

	if (found == -1)
		return 0;
	else
	{
		pResult->uContentType = pContentFieldMoving->uContentType;
		pResult->uFlags = pContentFieldMoving->uFlags;
		pResult->contentBlob.uLength = pContentFieldMoving->uContentLength;
		pResult->contentBlob.pData = (uint8 *)((uint8 *)gFlashDB + pContentFieldMoving->uContentOffset);
		return 1;
	}
}
