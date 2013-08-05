/*****************************************************************************
*
*  FlatDBSettings.h
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
#include <string>
#include <map>
#include <set>
using namespace std;

#define PARSING_ERROR_NOT_ENOUGH_ARGUMENTS (-1)
#define PARSING_ERROR_FAILED_PARSING (-2)

class FlatDBSettings
{
public:
    FlatDBSettings(void);
    ~FlatDBSettings(void);
    // parse command line arguments
    int Parse(int num, char* args[]);
    string GetRoot() {return _root;}
    int GetCaseSensitive() {return _caseSensitive;}
    // check if the given resource should be renamed
	string GetRename(const string & resource);
    // check if the given resource should be aliased
    string GetAlias(const string & resource);
	// check if the given resource should be excluded from the DB
    bool ShouldExclude(const string & resource);

private:
    string _root;
    int _caseSensitive;
	map<string, string> _alias;
	map<string, string> _rename;
	set<string> _exclude;
};

