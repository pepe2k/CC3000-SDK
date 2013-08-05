/*****************************************************************************
*
*  DirectoryEnumeration.cpp
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

#include "DirectoryEnumeration.h"
#include <Windows.h>
#include <algorithm>
using namespace std;


DirectoryEnumeration::DirectoryEnumeration(void)
{
}


DirectoryEnumeration::~DirectoryEnumeration(void)
{
}

int IsDirectory(DWORD dwFileAttributes)
{
    return ((dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0);
}

int IsIrrelevant(WIN32_FIND_DATA FindFileData)
{
	if ((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN) != 0)
		return true;
	string tmp = ".";
	if (tmp.compare(FindFileData.cFileName)==0)
		return true;
	tmp = "..";
	if (tmp.compare(FindFileData.cFileName)==0)
		return true;
	return false;

}

bool isAlphaNum(char ch)
{
	return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>=0 && ch<=9));
}

int 
DirectoryEnumeration::Enumerate(const char * dir, vector<fileEntryType> &files)
{
	int res = Enumerate(dir, "", files);
	return res;
}

int
DirectoryEnumeration::Enumerate(const char * rootDir, const char * relativeDir, vector<fileEntryType> &files)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	string directory = rootDir;
	directory.append(relativeDir);
    directory.append("\\*");
	hFind = FindFirstFile( directory.c_str(), &FindFileData );
    
	if( hFind != INVALID_HANDLE_VALUE )
	{
        if (!IsIrrelevant(FindFileData))
        {
            if (!IsDirectory(FindFileData.dwFileAttributes))
            {
                if (FindFileData.nFileSizeHigh>0 || FindFileData.nFileSizeLow >0)
                {
                    files.push_back(pair<string,int>(FindFileData.cFileName, FindFileData.nFileSizeLow));
                    //Save the file size
                }   
            }
            else
			{
				string tmp = "\\";
				tmp.append(FindFileData.cFileName);
				return Enumerate(rootDir, tmp.c_str(), files);
			}
        }

		while( FindNextFile( hFind, &FindFileData ) )
		{
			if (!IsIrrelevant(FindFileData))
			{
				if (!IsDirectory(FindFileData.dwFileAttributes))
				{
					if (FindFileData.nFileSizeHigh>0 || FindFileData.nFileSizeLow >0)
					{
						string sRelativeDir = relativeDir;
						sRelativeDir.append("\\");
						sRelativeDir.append(FindFileData.cFileName);
                        files.push_back(pair<string,int>(sRelativeDir.c_str(), FindFileData.nFileSizeLow));
						//Save the file size
					}   
				}
				else
				{
					string tmp = "\\";
					tmp.append(FindFileData.cFileName);
					Enumerate(rootDir, tmp.c_str(), files);
				}
			}
		}
	}
    else
        return -1;
	FindClose(hFind);
    return 0;
}

int DirectoryEnumeration::GetContentType(string & ext, string & res)
{
    res = "application/octetstream";

    HKEY hKey = NULL;
    LONG result = RegOpenKeyEx(HKEY_CLASSES_ROOT, ext.c_str(), 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS)
    {
        DWORD  dwType = REG_SZ;
        char buffer[1024];
        DWORD buflen = 1024;
        DWORD type = REG_SZ;
        result = RegQueryValueEx(hKey, "Content Type", NULL, &type, (LPBYTE)buffer, &buflen); 
        if (result == ERROR_SUCCESS && buflen>0)
        {
            res = buffer;
        }
    }
    RegCloseKey(hKey);
    return result;
}



int DirectoryEnumeration::CreateContentTypeList(vector<fileEntryType> &filesList, map<string, string> &extensionToContentTypeMap, vector<std::string> & contentTypes)
{
    string contentType = "";
    set<string> contentTypesSet;
    for (vector<fileEntryType>::iterator it = filesList.begin(); it!= filesList.end(); it++)
    {
        string path = it->first;
        string extension = path.substr(path.find_last_of('.'));
        int res = GetContentType(extension, contentType);
        extensionToContentTypeMap[extension] = contentType;
        contentTypesSet.insert(contentType);
    }
    for (set<string>::iterator it = contentTypesSet.begin(); it!=contentTypesSet.end(); it++)
        contentTypes.push_back(*it);
    return 0;
}
