/*****************************************************************************
*
*  smtp.h - Definitions for SMTP client implementation
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


// In Helo Command the host (us) identifies itself
const char smtp_helo[] = {'H','E','L','O','\0'};

// Mail from command which starts the e-mail itself
// The following commands do not have <CRLF> because we need
// to add data (such as addresses) after them.
const char smtp_mail_from[] = {'M','A','I','L',' ','F','R','O','M',':',' ','\0'};
const char smtp_rcpt[] = {'R','C','P','T',' ','T','O',':',' ','\0'};



const char smtp_data[] = "DATA";
const char smtp_crlf[] = "\r\n";
const char smtp_dcrlf[] = "\r\n\r\n";

////////////////////////////////////////////////////////////////////////
// These commands run after the Data command has been sent
const char smtp_subject[] = "Subject: ";

const char smtp_to[] = "To: ";

const char smtp_from[] = "From: ";
const char smtp_cc[] = "Cc: ";
const char smtp_data_end[] = {'\r','\n','.','\r','\n','\0'}; // "<CRLF>.<CRLF> Terminates the data portion"


////////////////////////////////////////////////////////////////////////

// "QUIT" Command finishes our SMTP session
const char smtp_quit[] = {'Q','U','I','T','\r','\n','\0'};


// Return Codes
// Ready - "220"
const char smtp_code_ready[] = {'2','2','0','\0'};
const char smtp_code_ok_reply[] = {'2','5','0','\0'};

// Intermediate Reply Code indicates all text after the command
// will be treated as part of the e-mail's text
const char smtp_code_intermed_reply[] = {'3','5','4','\0'};
