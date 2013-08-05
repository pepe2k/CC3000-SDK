/*****************************************************************************
*
*  FlatDBSettings.cpp
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

const char RENAME[] = "--rename";
const char ALIAS[] = "--alias";
const char EXCLUDE[] = "--exclude";
const char CASE_SENSITIVE[] = "--cs";

FlatDBSettings::FlatDBSettings(void):
_root(""),_caseSensitive(0)
{
}


FlatDBSettings::~FlatDBSettings(void)
{
}

int FlatDBSettings::Parse(int num, char* args[])
{
	if (num < 2)
		return PARSING_ERROR_NOT_ENOUGH_ARGUMENTS;
	_root = args[1];
	int count = 2;
	while (count < num)
	{
		if (strcmp(RENAME, args[count])==0)
		{
			if (count + 2 <= num)
			{
				_rename[args[count+1]] = args[count+2];
				count += 3;
			}
			else
				return PARSING_ERROR_FAILED_PARSING;
		}
		else if (strcmp(ALIAS, args[count])==0)
		{
			if (count + 2 <= num)
			{
				_alias[args[count+1]] = args[count+2];
				count += 3;
			}
			else
				return PARSING_ERROR_FAILED_PARSING;
		}
		else if (strcmp(EXCLUDE, args[count])==0)
		{
			if (count + 1 <= num)
			{
				_exclude.insert(args[count+1]);
				count += 2;
			}
			else
				return PARSING_ERROR_FAILED_PARSING;
		}
		else if (strcmp(CASE_SENSITIVE, args[count])==0)
		{
			if (count <= num)
			{
				_caseSensitive = 1;
				count += 1;
			}
			else
				return PARSING_ERROR_FAILED_PARSING;
		}
		else
			return PARSING_ERROR_FAILED_PARSING;
	}
    return 0;
}

string FlatDBSettings::GetRename(const string & resource)
{
	map<string,string>::iterator it;
	it = _rename.find(resource);
	if (it == _rename.end())
		return "";
	return it->second;
}

string FlatDBSettings::GetAlias(const string & resource)
{
	map<string,string>::iterator it;
	it = _alias.find(resource);
	if (it == _alias.end())
		return "";
	return it->second;
}

bool FlatDBSettings::ShouldExclude(const string & resource)
{
	if (_exclude.find(resource) != _exclude.end())
		return true;
	return false;
}
