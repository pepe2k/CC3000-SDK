/*****************************************************************************
*
*  server.c - CC3000 Sensor Application Server functionality implementation
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

#include "cc3000.h"
#include "msp430fr5739.h"
#include "wlan.h" 
#include "evnt_handler.h"    // callback function declaration
#include "nvmem.h"
#include "socket.h"
#include "common.h"
#include "netapp.h"
#include "board.h"
#include "common.h"
#include "string.h"
#include "demo_config.h"
#include "uart.h"
#include "sensors.h"
#include "board.h"
#include "terminal.h"
#include "strlib.h"
#include "server.h"

long serverSocket;
sockaddr serverSocketAddr;

/** \brief Definition of data packet to be sent by server */
unsigned char dataPacket[] = { '\r', 0xBE, 128, 128, 128, 70, 36, 0xEF };
char requestBuffer[SERVER_RECV_BUF_SIZE];
char serverErrorCode = 0;
extern char DevServname[];
extern volatile char runSmartConfig;
extern volatile unsigned long SendmDNSAdvertisment;
unsigned int closeconnection = 0;

//*****************************************************************************
//
//! Initialize Connection Server
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Waits for a connection where we will 
//
//*****************************************************************************
void initServer(void)
{       
    short  nonBlocking = 0;
    short Status;
    char portStr[12];
    memset(portStr,0,sizeof(portStr));
    
    // Open Server Socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == -1)
    {
        serverError(SERV_ERR_SOCKET);
        wlan_stop();
        while(1)
        {
            __no_operation();
        }
    }
    
    // Open port
    int port = SERVER_PORT;
    serverSocketAddr.sa_family = AF_INET;
    
    // Set the Port Number
    serverSocketAddr.sa_data[0] = (port & 0xFF00)>> 8;
    serverSocketAddr.sa_data[1] = (port & 0x00FF);
    
    memset (&serverSocketAddr.sa_data[2], 0, 4);
    
    if (bind(serverSocket, &serverSocketAddr, sizeof(sockaddr)) != 0) 
    {
        serverError(SERV_ERR_BIND);        
        return;
    }

        
    // Start Listening
    if (listen (serverSocket, 1) != 0)
    {         
        serverError(SERV_ERR_LISTEN);
        return;
    }
    
    Status = setsockopt(serverSocket, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &nonBlocking, sizeof(nonBlocking));
    if( Status < 0 )
    {
       while(1); //error  
    } 
    setCC3000MachineState(CC3000_SERVER_INIT);
    terminalPrint("Server Initialized on port ");
    itoa(port, portStr, DECIMAL_BASE);
    terminalPrint(portStr);
    terminalPrint("\r\n");

}

//*****************************************************************************
//
//! \brief  Waits and handle a client connection
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************

void waitForConnection(void)
{
    sockaddr clientaddr;  
    socklen_t addrlen;
    volatile int bytesRecvd = 0;
    volatile long bytesSent = 0;
    int clientDescriptor = -1;
    char clientIP[4];
    InitPWMTimer();
    EnablePWMs();
    
    fd_set readsds;
    long maxFD;
    int ret;
    timeval timeout;
    memset(&timeout, 0, sizeof(timeval));
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;//50*100;
    signed char curSocket;
    int optval, optlen;
  
    
    // Check whether the server functionality is successfully initialized
    if(currentCC3000State() & CC3000_SERVER_INIT)
    {
        // Begin waiting for client and handle the connection
        while(1)
        {
            //Turn off LED 5,6,7,8
            TB1CCR1 =((unsigned short)0x00)<<8;
            TB2CCR2 =((unsigned short)0x00)<<8;
            TB2CCR1 =((unsigned short)0x00)<<8;
            TB1CCR2 =((unsigned short)0x00)<<8;
                   
            
            addrlen = sizeof(clientaddr);
            
            terminalPrint("Waiting for Clients\r\n");
            // accept blocks until we receive a connection
            while ( (clientDescriptor == -1) || (clientDescriptor == -2) )
            {
              clientDescriptor = accept(serverSocket, (sockaddr *) &clientaddr, &addrlen);
              if(SendmDNSAdvertisment == 1)
              {
              	mdnsAdvertiser(1,DevServname,strlen(DevServname));
				SendmDNSAdvertisment = 0;
              }
			  __delay_cycles(6000000);
            }
            

            // Call user specified Clietn Accepted Event Handler            
            
            if(clientDescriptor >= 0)
            {
                setCC3000MachineState(CC3000_CLIENT_CONNECTED);
                terminalPrint("Client ");
                // Read IP and print
                clientIP[0] = clientaddr.sa_data[2];
                clientIP[1] = clientaddr.sa_data[3];
                clientIP[2] = clientaddr.sa_data[4];
                clientIP[3] = clientaddr.sa_data[5];

                printIpAddr(clientIP);
                terminalPrint(" Connected\r\n");
                
                //Add client socket ID to to the read set
                FD_SET(clientDescriptor, &readsds);
                maxFD = clientDescriptor + 1;
                
                while(currentCC3000State() & CC3000_CLIENT_CONNECTED)
                {
                        //Turn LED5 on to indicate that a client is connected
                        TB1CCR1 =((unsigned short)0xFF)<<8;
                        
                        ret = select(maxFD, &readsds, NULL, NULL, &timeout);
                        if(ret > 0)
                        {  
                          memset(requestBuffer, 0, 8);
                          bytesRecvd = recv(clientDescriptor, requestBuffer, sizeof(requestBuffer), 0);
                          int temp = bytesRecvd;
                          if(bytesRecvd >0)
                          {
                            TB1CCR2 =((unsigned short)requestBuffer[3])<<8;
                            if(requestBuffer[7] == 1)
                            {
                              TB2CCR2 = ((unsigned short) 0xFF)<<8;
                              TB2CCR1 = ((unsigned short) 0x00)<<8;
                            }
                            else if(requestBuffer[7] == 2)
                            {
                              TB2CCR2 = ((unsigned short) 0x00)<<8;
                              TB2CCR1 = ((unsigned short) 0xFF)<<8;
                            }
                            else
                            {
                              TB2CCR2 = ((unsigned short) 0x00)<<8;
                              TB2CCR1 = ((unsigned short) 0x00)<<8;
                            }
                          }
                        }
                        
                        
                        SetupAccel();
                        SetupThermistor();
                        
                        SetupAccelXXX();
                        TakeADCMeas(X_AXIS_MEAS);      // Take 1 ADC Sample for X-AXIS
                        
                        
                        SetupAccelYYY();
                        TakeADCMeas(Y_AXIS_MEAS);      // Take 1 ADC Sample for Y-AXIS
                        
                        
                        SetupAccelZZZ();
                        TakeADCMeas(Z_AXIS_MEAS);      // Take 1 ADC Sample for Y-AXIS
                        
                        
                        SetupThermistorADC();  // One time setup and calibration   
                        TakeADCMeas(TEMP_MEAS);         // Take 1 ADC Sample FOR TEMP
                        
                        SetupVcc();
                        TakeADCMeas(VCC_MEAS);

                        __no_operation();
                        __delay_cycles(10000);
                        
                        ret = select(maxFD, NULL, &readsds, NULL, &timeout);
                        if(ret >0)
                        {                  
                          bytesSent = send(clientDescriptor, (unsigned char *)dataPacket, sizeof(dataPacket), 0);					
                          if (bytesSent != sizeof(dataPacket))
                          {
                            bytesSent = send(clientDescriptor, (unsigned char *)dataPacket, sizeof(dataPacket), 0);
                            if (bytesSent != sizeof(dataPacket))
                            {
                              // Check if connection is closed
                              if(closeconnection == 1)
                              {
                                closeconnection = 0;
                                closesocket(clientDescriptor);
                                terminalPrint("Client Disconnected\r\n");
                                clientDescriptor = -1;
                                unsetCC3000MachineState(CC3000_CLIENT_CONNECTED);
                                break;
                              }
                            }
                          }
                        }
                        else
                        {
                          // Check if connection is closed
                          if(closeconnection == 1)
                          {
                            closeconnection = 0;
                            closesocket(clientDescriptor);
                            terminalPrint("Client Disconnected\r\n");
                            clientDescriptor = -1;
                            unsetCC3000MachineState(CC3000_CLIENT_CONNECTED);
                            break;
                          }
                          // Check if connection is timed out
                          curSocket =  getsockopt(clientDescriptor, SOL_SOCKET, SOCKOPT_RECV_NONBLOCK  , &optval, (socklen_t*)&optlen);
                          if (curSocket == -57)
                          {
                            closeconnection = 0;
                            closesocket(clientDescriptor);
                            terminalPrint("Client Disconnected\r\n");
                            clientDescriptor = -1;
                            unsetCC3000MachineState(CC3000_CLIENT_CONNECTED);
                            break;
                          }
                        }
                          
                    }
                __delay_cycles(1000);
            }
            else if(clientDescriptor == SOCKET_INACTIVE_ERR)
            {
                terminalPrint("Socket Server Timeout. Restarting Server\r\n");
		clientDescriptor = -1;
                // Reinitialize the server
                shutdownServer();
                initServer();
            }
        }
    }
}

//*****************************************************************************
//
//! Shut down server sockets
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Waits for a connection where we will 
//
//*****************************************************************************
void shutdownServer()
{
     // Close the Server's Socket
    closesocket(serverSocket);
    serverSocket = 0xFFFFFFFF;    
	unsetCC3000MachineState(CC3000_SERVER_INIT);
}

//*****************************************************************************
//
//! \brief  Waits for a connection where we will 
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void serverError(char err)
{     
     switch(err)
     {
        case SERV_ERR_SOCKET:
            serverErrorCode = SERV_ERR_SOCKET;
         break;
        case SERV_ERR_BIND:
            serverErrorCode = SERV_ERR_BIND;
         break;
        case SERV_ERR_LISTEN:
            serverErrorCode = SERV_ERR_LISTEN;
            break;
     }
      terminalPrint("Server Error\r\n");
	 
     while(1)
     {
         __no_operation();
     }
}
