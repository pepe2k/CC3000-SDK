/*****************************************************************************
*
*  DBBuilder.h
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

#include <map>
#include <string>
#include <set>
#include <vector>
#include "DirectoryEnumeration.h"
#include "FlatDBSettings.h"
#include "FlashDB.h"

#define DBBUILDER_ERROR_CANT_OPEN_RESOURCE_FILE (-1)
#define DBBUILDER_ERROR_FILE_SIZE_TOO_LARGE (-2)
#define DBBUILDER_ERROR_INTERNAL_NO_CONTENT_TYPE (-3)
#define DBBUILDER_ERROR_INTERNAL_OFSSETS_NOT_MATCH (-4)
#define DBBUILDER_ERROR_CANT_OPEN_OUTPUT_FILE (-5)
#define DBBUILDER_ERROR_CANT_WRITE_TO_OUTPUT_FILE (-6)

class DBBuilder
{
public:
    DBBuilder(void);
    ~DBBuilder(void);
    // Create DB given a list of files, settings and content types
	int BuildDB(vector<fileEntryType> &files, map<string,string> & extensionToType, vector<string> & contentTypes, FlatDBSettings * settings, string &res);
    // Save the output in a .c and .h file to be used by the WebServer
    int SaveInCHfile(string path, string & res);
    // Save in binary file
    int SaveInBinaryfile(string path, string & res);
private:
    // Create entries for FlashDBContent, gzip if it saves space, vase the content and set the lengths of the resources
	int	PreProcessResources(vector<fileEntryType> &files, FlatDBSettings * settings, vector<FlashDBContentEntry> & contentEntryList, map<string,string> & extensionToType, vector<string> & contentTypes, vector<string> &content);
    // Go over the entries and calculate the correct offsets for each type of data, set the header fileds
    int	UpdateEntriesOffsets(FlatDBSettings * settings, FlashDBHeader & header, vector<fileEntryType> &files, vector<FlashDBContentEntry> & contentEntryList, vector<FlashDBContentTypeEntry> & contentTypeEntries, vector<string> & contentTypes, vector<string> &content);
    // Concatenate ready data to a "flat" string
    int CreateDB(FlashDBHeader & header, vector<fileEntryType> &files, vector<FlashDBContentEntry> & contentEntryList, vector<FlashDBContentTypeEntry> &contentTypeEntries, vector<string> & contentTypes, vector<string> &content, string &res);
    // Try to compress with gzip, if size is reduced use the compressed file
    int CompressWithGzip(const string & path, int originalSize, string & result);
    // Get the index of a resource type
    int GetResourceInfo(vector<string> & contentTypes, const string &type, int &index);
    // apply the settings on the content entries
    int ApplySettings(FlatDBSettings * settings, vector<fileEntryType> &files, vector<FlashDBContentEntry> & contentEntryList, vector<string> &content);

    // Since there are three mutually linked vectors - an indirection vector is used for sorting the the usage of the other vectors is done through indirection from the indices vector
    // Create a vector of approppriate length
	void InitIndices(vector<int> & ind, int size);
    // Sort the indices vector taking the files vector as the input for compare function
	void SortIndices(vector<int> & ind, vector<fileEntryType> & files, int caseSensitive);
	vector<int> _indices;
};

