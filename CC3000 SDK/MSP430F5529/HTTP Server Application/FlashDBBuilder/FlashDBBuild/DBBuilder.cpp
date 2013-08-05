/*****************************************************************************
*
*  DBBuilder.cpp
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

#include "DBBuilder.h"
#include <sys/stat.h>
#include <algorithm>
#include <Windows.h>

#define MAX_FILE_SIZE 1024*64 //64KB
#define MAX_DATABASE_SIZE 1024*1024*1024 //1 GB

DBBuilder::DBBuilder(void)
{
}


DBBuilder::~DBBuilder(void)
{
}


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

// global for string comparison function
vector<fileEntryType> * filesList;

bool fileEntryTypeCompareNoCase( const int &left, const int &right )
{
	string _left = filesList->at(left).first;
	string _right = filesList->at(right).first;
    //using stricmp for compatability with webserver
    int res = _stricmp(_left.c_str(), _right.c_str());
	return (res<0);
}

bool fileEntryTypeCompare( const int &left, const int &right )
{
	string _left = filesList->at(left).first;
	string _right = filesList->at(right).first;
    //using stricmp for compatability with webserver
    int res = strcmp(_left.c_str(), _right.c_str());
	return (res<0);
}


void DBBuilder::InitIndices(vector<int> & ind, int size)
{
	for (int i = 0; i< size; i++)
		ind.push_back(i);
}

void DBBuilder::SortIndices(vector<int> & ind, vector<fileEntryType> & files, int caseSensitive)
{
	filesList = &files;
    if (caseSensitive==1)
        sort(ind.begin(), ind.end(), fileEntryTypeCompare);
    else
        sort(ind.begin(), ind.end(), fileEntryTypeCompareNoCase);
	filesList = NULL;
}


int DBBuilder::CompressWithGzip(const string & path, int originalSize, string & result)
{
    static char fileName[] = "FlashDBtempzgipOutput.gz";
    static char lpTempPathBuffer[MAX_PATH];
    DWORD dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        return -3;
    }
    string tempfile = string(lpTempPathBuffer, dwRetVal);
    tempfile.append(fileName);

    static char buffer[MAX_FILE_SIZE];
    string cmd = "gzip -fqn9c ";
    cmd.append(path);
    cmd.append(" >");
    cmd.append(tempfile);
    int res = system(cmd.c_str());
	int newsize;
	if (res==0)
	{
	    struct stat filestatus;
        stat(tempfile.c_str(), &filestatus);
		newsize = filestatus.st_size;
	}
	else
		newsize = originalSize+1;
    if (newsize > MAX_FILE_SIZE && originalSize>MAX_FILE_SIZE)
        return DBBUILDER_ERROR_FILE_SIZE_TOO_LARGE;
    FILE *f = NULL;
    int n;
    if (newsize<originalSize)
    {
        fopen_s(&f, tempfile.c_str(), "rb");
        if (f)
        {
            n = fread(buffer, 1, MAX_FILE_SIZE, f);
            fclose(f);
            result = string(buffer, n);
            return 1;
        }
    }
    else
    {
        fopen_s(&f, path.c_str(), "rb");
        if (f)
        {
            n = fread(buffer, 1, MAX_FILE_SIZE, f);
            fclose(f);
            result = string(buffer, n);
            return 0;
        }
    }
    return DBBUILDER_ERROR_CANT_OPEN_RESOURCE_FILE;
}

int DBBuilder::GetResourceInfo(vector<string> & contentTypes, const string &type, int &index)
{
    int size = contentTypes.size();
    for (index=0; index<size; index++)
    {
        if (contentTypes.at(index).compare(type)==0)
        {
            return index;
        }
    }
    return -1;
}

int DBBuilder::ApplySettings(FlatDBSettings * settings, vector<fileEntryType> &files, vector<FlashDBContentEntry> & contentEntryList, vector<string> &content)
{
    vector<fileEntryType>::iterator filesIter = files.begin();
    vector<FlashDBContentEntry>::iterator contentEntryIter = contentEntryList.begin();
    vector<string>::iterator contentIter = content.begin();
    unsigned int i=0;
    while (i < (files.size()))
    {
        replace((files.begin()+i)->first.begin(), (files.begin()+i)->first.end(), '\\', '/');
        if (settings->ShouldExclude((files.begin()+i)->first))
        {
            files.erase(files.begin()+i);
            contentEntryList.erase(contentEntryList.begin()+i);
            content.erase(content.begin()+i);
            continue;
            // i is not advanced since now all the enries moved one back
        }
        string rename = settings->GetRename((files.begin()+i)->first);
        if (rename!="")
        {
            (files.begin()+i)->first = rename;
            i++;
            continue;
        }
        string alias = settings->GetAlias((files.begin()+i)->first);
        if (alias!="")
        {
            files.push_back(make_pair(alias, 0));
            FlashDBContentEntry entry = contentEntryList.at(i);
			entry.uResourceLength = alias.length();
            contentEntryList.push_back(entry);
            content.push_back(content.at(i));
            i++;
            continue;
        }
        i++;
    }
    return 0;
}


int	DBBuilder::PreProcessResources(vector<fileEntryType> &files, FlatDBSettings * settings, vector<FlashDBContentEntry> & contentEntryList, map<string,string> & extensionToType, vector<string> & contentTypes, vector<string> &content)
{
    string path;
    for (vector<fileEntryType>::iterator it = files.begin(); it!=files.end(); it++)
    {
        path = settings->GetRoot();
        path.append(it->first);
        string res;
        int compressed = CompressWithGzip(path, it->second, res);
        if (compressed < 0)
            return compressed;
        FlashDBContentEntry cEntry;
        int index;
        string type = extensionToType[path.substr(path.find_last_of('.'))];
        if ( GetResourceInfo(contentTypes, type, index) == -1)
            return DBBUILDER_ERROR_INTERNAL_NO_CONTENT_TYPE;
        cEntry.uContentType = index;
        cEntry.uFlags = 0;
        if (compressed==1)
            cEntry.uFlags |= FLASHDB_FLAG_COMPRESSED;
		cEntry.uResourceLength = it->first.length();
        cEntry.uResourceOffset = 0;
        cEntry.uContentLength = res.length();
        cEntry.uContentOffset = 0;
        content.push_back(res);
        contentEntryList.push_back(cEntry);
    }
	return 0;
}

int	DBBuilder::UpdateEntriesOffsets(FlatDBSettings * settings, FlashDBHeader & header, vector<fileEntryType> &files, vector<FlashDBContentEntry> & contentEntryList, vector<FlashDBContentTypeEntry> & contentTypeEntries, vector<string> & contentTypes, vector<string> &content)
{
	unsigned int offset = 0;
	int SIZE_OF_CONTENT_TYPE_ENTRY = sizeof(FlashDBContentTypeEntry);
	int SIZE_OF_CONTENT_ENTRY = sizeof(FlashDBContentEntry);
	header.uContentTypeAmount = contentTypes.size();
	header.uContentTypeOffset = sizeof(header);
	header.uContentAmount = files.size();
	header.uContentOffset = header.uContentTypeAmount * SIZE_OF_CONTENT_TYPE_ENTRY + sizeof(header);
    header.uFlags = 0;
    if (settings->GetCaseSensitive())
        header.uFlags = FLASH_DB_FLAG_CASE_SENSITIVE;
	offset += header.uContentOffset + header.uContentAmount * SIZE_OF_CONTENT_ENTRY;
	// create content type entries
	for (int i=0; i<header.uContentTypeAmount; i++)
	{
		FlashDBContentTypeEntry entry;
		entry.uValueLength = contentTypes.at(i).length();
		entry.uValueOffset = offset;
		offset += entry.uValueLength;
		contentTypeEntries.push_back(entry);
	}
	// update content name entries and content entries - the order is to puth name then the content
	for (int i = 0; i < header.uContentAmount; i++)
	{
		int index = _indices.at(i);
		contentEntryList.at(index).uResourceOffset = offset;
		offset += contentEntryList.at(index).uResourceLength;
		contentEntryList.at(index).uContentOffset = offset;
		offset += contentEntryList.at(index).uContentLength;
	}
	return 0;
}

int	DBBuilder::CreateDB(FlashDBHeader & header, vector<fileEntryType> &files, vector<FlashDBContentEntry> & contentEntryList, vector<FlashDBContentTypeEntry> & contentTypeEntries, vector<string> & contentTypes, vector<string> &content, string &res)
{
	char * buff = new char[MAX_DATABASE_SIZE];
	int offset = 0;
    memcpy(buff+offset, &header, sizeof(header));
	offset += sizeof(header);
	//content type entries
	for (int i=0; i<header.uContentTypeAmount; i++)
	{
		memcpy(buff+offset, &contentTypeEntries.at(i), sizeof(FlashDBContentTypeEntry));
		offset += sizeof(FlashDBContentTypeEntry);
	}
	//content entries
	for (int i=0; i<header.uContentAmount; i++)
	{
		int index = _indices.at(i);
		memcpy(buff+offset, &contentEntryList.at(index), sizeof(FlashDBContentEntry));
		offset += sizeof(FlashDBContentEntry);
	}
	//content type strings
	for (int i=0; i<header.uContentTypeAmount; i++)
	{
		memcpy(buff+offset, contentTypes.at(i).c_str(), contentTypes.at(i).length());
		offset += contentTypes.at(i).length();
	}

	// update content name entries and content entries - the order is to puth name then the content
	for (int i = 0; i < header.uContentAmount; i++)
	{
		int index = _indices.at(i);
		if (contentEntryList.at(index).uResourceOffset != offset)
			return DBBUILDER_ERROR_INTERNAL_OFSSETS_NOT_MATCH;
		memcpy(buff+offset, files.at(index).first.c_str(), files.at(index).first.length());
		offset += files.at(index).first.length();
		if (contentEntryList.at(index).uContentOffset != offset)
			return DBBUILDER_ERROR_INTERNAL_OFSSETS_NOT_MATCH;
		memcpy(buff+offset, content.at(index).c_str(), content.at(index).length());
		offset += content.at(index).length();
	}
    res = string(buff, offset);
    delete buff;
	return 0;
}

int DBBuilder::BuildDB(vector<fileEntryType> &files, map<string,string> & extensionToType, vector<string> & contentTypes, FlatDBSettings * settings, string &res)
{
    int result;
    FlashDBHeader header;
	vector<FlashDBContentTypeEntry> contentTypeEntries;
	vector<FlashDBContentEntry> contentEntries;
	vector<string> content;
	
	// go over the file list, first act upon the setting, then sork all files using stricmp
	// fill the content entries and save the resource content in string vector update size and GZIP flag
	// if a resource to a content type was removed - delete it from the set
    result = PreProcessResources(files, settings, contentEntries, extensionToType, contentTypes, content);
    if (result < 0)
        return result;
	
    // Go ever the files list and aplly settings if such exist
    result = ApplySettings(settings, files, contentEntries, content);
    if (result < 0)
        return result;

    // Since there are three vectors holding mutually relevent information - the sort is done on an indirection (index) vector
    // the comparison function compares the resource names and the vectors are then used ifter an indirection from the sorted _indices vector
	InitIndices(_indices, files.size());
    SortIndices(_indices, files, settings->GetCaseSensitive());

    // go over the content type vector and the sorted vectors of content and update the final offset in the entries
	result = UpdateEntriesOffsets(settings, header, files, contentEntries, contentTypeEntries, contentTypes, content);
    if (result < 0)
        return result;

    // concatenate all the information in one array
	result = CreateDB(header, files, contentEntries, contentTypeEntries, contentTypes, content, res);
    if (result < 0)
        return result;

    return 0;
}


int DBBuilder::SaveInCHfile(string path, string & res)
{
    FILE *f = NULL;
    int n;
    string filename = path;
    filename.append(".h");
    fopen_s(&f, filename.c_str(), "wt");
    if (f)
    {
        n = fprintf(f, "#ifndef _FLASH_DB_CONTENT_H_\n#define _FLASH_DB_CONTENT_H_\n\n#define FlashDBLen (%d)\nextern const char FlashDBContent[FlashDBLen];\n\n#endif\n", res.length());
        if (n<=0)
        {
            fclose(f);
            return DBBUILDER_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
        }
        fclose(f);
        fopen_s(&f, path.append(".c").c_str(), "wt");
        if (f)
        {
            n = fprintf(f, "#include \"%s\"\n\nconst char FlashDBContent[] = {", filename.c_str());
            for (unsigned int i = 0; i < (res.length()-1); i++)
            {
                if (i % 16 == 0)
                    n = fprintf(f, "\n\t");
                if (n<=0)
                {
                    fclose(f);
                    return DBBUILDER_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
                }
                n = fprintf(f, "0x%.2x, ", (unsigned char)(res.at(i)));
                if (n<=0)
                {
                    fclose(f);
                    return DBBUILDER_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
                }
            }
            fprintf(f, "0x%.2x\n};\n\n", (unsigned char)(res.at(res.length()-1)));
            fclose(f);
            return 0;
        }
    }
    return DBBUILDER_ERROR_CANT_OPEN_OUTPUT_FILE;
}

int DBBuilder::SaveInBinaryfile(string path, string & res)
{
    FILE *f = NULL;
    int n;
    fopen_s(&f, path.append(".bin").c_str(), "wb");
    if (f)
    {
        n = fwrite(res.c_str(), 1, res.length(), f);
        fclose(f);
        if (n==res.length())
            return 0;
    }
    return DBBUILDER_ERROR_CANT_OPEN_OUTPUT_FILE;
}