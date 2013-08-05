/*****************************************************************************
*
*  FlashDBBuild.cpp
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


#include "FlatDBSettings.h"
#include "DirectoryEnumeration.h"
#include "DBBuilder.h"

int main(int argc, char* argv[])
{
    int error = 0;
	string res = "";
    vector<fileEntryType> files;
    map<string, string> extensionToContentTypeMap;
    vector<string> contentTypes;
    FlatDBSettings *settings = new FlatDBSettings();
    DirectoryEnumeration *fileList = new DirectoryEnumeration();
	DBBuilder * dbBuilder = new DBBuilder();

    error = settings->Parse(argc, argv);
    if (error < 0)
    {
        if (error == PARSING_ERROR_NOT_ENOUGH_ARGUMENTS)
            printf("Not enough arguments\n");
        if (error == PARSING_ERROR_FAILED_PARSING)
            printf("Error parsing arguments\n");
        return -1;
    }
    error = fileList->Enumerate(settings->GetRoot().c_str(), files);
    if (error < 0)
    {
        printf("Error in opening root directory\n");
        return -1;
    }
    error = fileList->CreateContentTypeList(files, extensionToContentTypeMap, contentTypes);
    if (error < 0)
        return -1;
    error = dbBuilder->BuildDB(files, extensionToContentTypeMap, contentTypes, settings, res);
    if (error < 0)
    {
        switch (error){
        case DBBUILDER_ERROR_CANT_OPEN_RESOURCE_FILE:
            printf("Error building database: \"DBBUILDER_ERROR_CANT_OPEN_RESOURCE_FILE\"\n");
            break;
        case DBBUILDER_ERROR_FILE_SIZE_TOO_LARGE:
            printf("Error building database: \"DBBUILDER_ERROR_FILE_SIZE_TOO_LARGE\"\n");
            break;
        case DBBUILDER_ERROR_INTERNAL_NO_CONTENT_TYPE:
            printf("Error building database: \"DBBUILDER_ERROR_INTERNAL_NO_CONTENT_TYPE\"\n");
            break;
        case DBBUILDER_ERROR_INTERNAL_OFSSETS_NOT_MATCH:
            printf("Error building database: \"DBBUILDER_ERROR_INTERNAL_OFSSETS_NOT_MATCH\"\n");
            break;
        default:
            printf("Error building database: \"NOT_RECOGNIZED ERROR\"\n");
            break;
        }
        return -1;
    }
    string fileName = "FlashDbContent";
    error = dbBuilder->SaveInBinaryfile(fileName, res);
    if (error < 0)
    {
        if (error == DBBUILDER_ERROR_CANT_OPEN_OUTPUT_FILE)
            printf("Can not open output file (FlashDbContent...)\n");
        if (error == DBBUILDER_ERROR_CANT_WRITE_TO_OUTPUT_FILE)
            printf("Can not write to output file (FlashDbContent...)\n");
        return -1;
    }
    error = dbBuilder->SaveInCHfile(fileName, res);
    if (error<0)
    {
        if (error == DBBUILDER_ERROR_CANT_OPEN_OUTPUT_FILE)
            printf("Can not open output file (FlashDbContent...)\n");
        if (error == DBBUILDER_ERROR_CANT_WRITE_TO_OUTPUT_FILE)
            printf("Can not write to output file (FlashDbContent...)\n");
        return -1;
    }
	return 0;
}

