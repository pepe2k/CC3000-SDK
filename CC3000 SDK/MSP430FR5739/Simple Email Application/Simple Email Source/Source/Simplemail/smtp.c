/*****************************************************************************
*
*  smtp.c - Definitions for SMTP client implementation
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

// CC3000 Specific Headers
#include <msp430.h>
#include "cc3000.h"
#include "wlan.h" 
#include "evnt_handler.h"    // callback function declaration
#include "nvmem.h"
#include "socket.h"
#include "common.h"
#include "netapp.h"
#include "board.h"
#include "common.h"
#include "string.h"
#include "smtp.h"
#include "smtp_gen.h"


long smtpSocket;
extern sockaddr tSocketAddr;
unsigned char smtp_state;

int smtpServerPort = 25;
unsigned long smtpServerIP = 0;
signed char smtpServerFlag = 0;


#ifdef __CCS__
#pragma DATA_SECTION(cmdBuf, ".FRAM_DATA")
char cmdBuf[SMTP_BUF_LEN];

#elif __IAR_SYSTEMS_ICC__
#pragma location = "FRAM_DATA"
__no_init char cmdBuf[SMTP_BUF_LEN];
#endif

#ifdef __CCS__
#pragma DATA_SECTION(buf, ".FRAM_DATA")
char buf[SMTP_BUF_LEN];

#elif __IAR_SYSTEMS_ICC__
#pragma location = "FRAM_DATA"
__no_init char buf[SMTP_BUF_LEN];
#endif

//*****************************************************************************
//
//! \brief Sends an E-mail using SMTP
//!
//!  Sets up the TX Destination and RX Source and configures
//!  some settings.
//!
//! \param from is a string with the from address
//! \param to is a string with the to address
//! \param subject is a string with the subject
//! \param msg is the e-mail message body
//!
//! \return None.
//
//*****************************************************************************
char smtpSend(char * from, char * to, char * subject, char * msg)
{

    memset(buf,0,100);
    
    // Check for 220
    recv(smtpSocket,buf,100,0) ;
    
    if(buf[0] == smtp_code_ready[0] && buf[1] == smtp_code_ready[1] && 
       buf[2] == smtp_code_ready[2])
    {
        sendSMTPCommand(smtpSocket, "HELO", NULL, buf);
        
        if(buf[0] == smtp_code_ok_reply[0] && buf[1] == smtp_code_ok_reply[1] && 
           buf[2] == smtp_code_ok_reply[2])
        {
            // Start E-mail and indicate from (our address)
            sendSMTPCommand(smtpSocket, (char *)smtp_mail_from, from, buf);
            
            if(buf[0] == smtp_code_ok_reply[0] && buf[1] == smtp_code_ok_reply[1] && 
               buf[2] == smtp_code_ok_reply[2])
            {
                // Indicate Recepient
                sendSMTPCommand(smtpSocket, (char *)smtp_rcpt, to, buf);
                
                if(buf[0] == smtp_code_ok_reply[0] && buf[1] == smtp_code_ok_reply[1] && 
                   buf[2] == smtp_code_ok_reply[2])
                {
                    // Send Data Start
                    sendSMTPCommand(smtpSocket, (char *)smtp_data, NULL, buf);
                    
                    if(buf[0] == smtp_code_intermed_reply[0] && buf[1] == smtp_code_intermed_reply[1] && 
                       buf[2] == smtp_code_intermed_reply[2])
                    {
                        // Send actual Message, preceded by FROM, TO and Subject
                        sendSMTPCommand(smtpSocket, (char *)smtp_subject, subject, NULL);

                        sendSMTPCommand(smtpSocket, (char *)smtp_to, to, NULL);
                        
                        // Add E-mails "From:" Field
                        sendSMTPCommand(smtpSocket, (char *)smtp_from, from, NULL);
                        
                        // Send CRLF
                        send(smtpSocket,smtp_crlf,strlen(smtp_crlf),0);
                        
                        // Send message
                        sendSMTPCommand(smtpSocket, msg, NULL, NULL);

                        // End Message
                        sendSMTPCommand(smtpSocket,(char *)smtp_data_end,NULL,buf);
                        
                        if(buf[0] == smtp_code_ok_reply[0] && buf[1] == smtp_code_ok_reply[1] && 
                           buf[2] == smtp_code_ok_reply[2])
                        {
                            // Disconnect from server by sending QUIT command
                            send(smtpSocket,smtp_quit,strlen(smtp_quit),0);
                            smtpDisconnect();
                        }
                        else
                        {
                            smtpErrorHandler(buf);
                        }
                    }
                    else
                    {
                        smtpErrorHandler(buf);
                    }                    
                }                
                else
                {
                    smtpErrorHandler(buf);
                }
            }
            else
            {
               smtpErrorHandler(buf);
            }
        }
        else
        {
            smtpErrorHandler(buf);
        }
    }
    else
    {
        smtpErrorHandler(buf);
    }
    
    // Message will be sent. Quit
    send(smtpSocket,smtp_quit,strlen(smtp_quit),0) ;
    
    return 0;
}


//*****************************************************************************
//
//! \brief Sends an E-mail using SMTP
//!
//!  Sets up the TX Destination and RX Source and configures
//!  some settings.
//!
//! \param from is a string with the from address
//!
//! \return None.
//
//*****************************************************************************
signed char smtpConnect(char * hname, int port)
{
    smtpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (smtpSocket == -1)
    {
        wlan_stop();
        return -1;
    }

    smtpServerPort = port;

    tSocketAddr.sa_family = AF_INET;

    tSocketAddr.sa_data[0] = (port & 0xFF00) >> 8;
    tSocketAddr.sa_data[1] = (port & 0x00FF);


    if(gethostbyname(hname, strlen(hname), &smtpServerIP) > 0)
    {
      //correcting the endianess
        tSocketAddr.sa_data[5] = BYTE_N(smtpServerIP,0);  // First octet of destination IP
        tSocketAddr.sa_data[4] = BYTE_N(smtpServerIP,1);   // Second Octet of destination IP
        tSocketAddr.sa_data[3] = BYTE_N(smtpServerIP,2);  // Third Octet of destination IP
        tSocketAddr.sa_data[2] = BYTE_N(smtpServerIP,3);  // Fourth Octet of destination IP

        smtpServerFlag = connect(smtpSocket, &tSocketAddr, sizeof(tSocketAddr));

        if (smtpServerFlag < 0)
        {
            // Unable to connect
            return -1;
        }
    }
    else
    {
        // Error with DNS. Use default IP of e-mail
        tSocketAddr.sa_data[2] = 207;  // First octet of destination IP
        tSocketAddr.sa_data[3] = 58;   // Second Octet of destination IP
        tSocketAddr.sa_data[4] = 147;  // Third Octet of destination IP
        tSocketAddr.sa_data[5] = 66;  // Fourth Octet of destination IP

        smtpServerFlag = connect(smtpSocket, &tSocketAddr, sizeof(tSocketAddr));

        if (smtpServerFlag < 0)
        {
            // Unable to connect
            return -1;
        }
        else
        {
            smtpServerFlag = 1;
            turnLedOn(7);
        }
    }

    // Success
    return 0;
}



//*****************************************************************************
//
//! \brief  Disconnect SMTP and close the socket
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void smtpDisconnect(void)
{
    closesocket(smtpSocket);
    smtpSocket = 0xFFFFFFFF;
}

//*****************************************************************************
//
//! \brief Sends an SMTP command and provides the server's response
//!
//! If cmd or cmd param are NULL, it only ends up sending a <CR><LF>
//! If respBuf is NULL, it will not attempt to receive response
//!
//!  Sets up the TX Destination and RX Source and configures
//!  some settings.
//!
//! \param socket is the socket descriptor to be used for sending
//! \param cmd is the string of the command
//! \param cmdpara is the command parameter
//! \param respBuf is a pointer to the SMTP response from the server
//!
//! \return None.
//
//*****************************************************************************
void sendSMTPCommand(long socket, char * cmd, char * cmdparam, char * respBuf)
{

    int sendLen = 0;
    memset(cmdBuf,0,sizeof(cmdBuf));
    
    if(cmd != NULL)
    {        
        sendLen = strlen(cmd);
        memcpy(cmdBuf,cmd,strlen(cmd));
    }
    
    if(cmdparam != NULL)
    {        
        memcpy(&cmdBuf[sendLen],cmdparam, strlen(cmdparam));
        sendLen += strlen(cmdparam);
    }
    
    memcpy(&cmdBuf[sendLen],smtp_crlf,strlen(smtp_crlf));
    sendLen += strlen(smtp_crlf);
    send(socket, cmdBuf,sendLen,0);
    

    if(respBuf != NULL)
    {
        memset(respBuf,0,SMTP_BUF_LEN);
        recv(socket, respBuf,SMTP_BUF_LEN,0);
    }
}

//*****************************************************************************
//
//! \brief Handles SMTP Server Errors

//! \param servResp is the server response
//!
//! \return None.
//
//*****************************************************************************
void smtpErrorHandler(char * servResp)
{
    // Currently we don't handle the errors
    while(1)
    {
        __no_operation();
    }
}

