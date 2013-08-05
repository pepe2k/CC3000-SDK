/*****************************************************************************
*
*  WebServerApp.c - CC3000 Slim Driver Implementation.
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

#include "wlan.h" 
#include "evnt_handler.h"    // callback function declaration
#include "nvmem.h"
#include "socket.h"
#include "netapp.h"
#include "host_driver_version.h"
#include "cc3000.h"
#include "Common.h"
#include "demo_config.h"
#include "HTTPString.h"

#include <msp430.h>
#include "Board.h"
#include "HttpCore.h"
#include "Wheel.h"
#include "dispatcher.h"

#define FALSE 0
#define SERVERAPPVERSION "v1.10.11.6.8.1"

volatile unsigned long ulCC3000Connected,ulCC3000DHCP,OkToDoShutDown;
unsigned char obtainIpInfoFlag = FALSE;
unsigned char ConnectUsingSmartConfig;
tNetappIpconfigRetArgs *ipconfig;
char DevServname[] = {'C','C','3','0','0','0'};
volatile char runSmartConfig = 0;
unsigned char mDNSValid, skipCount, mDNSSend;

int String_utoa(int uNum, char *pString)
{
    char digits[] = "0123456789";
    char* ptr;
    int uLength;
    int uTemp = uNum;

    // value 0 is a special format
    if (uNum == 0)
    {
        uLength = 1;
        *pString = '0';
        return uLength;
    }

    // Find out the length of the number, in decimal base
    uLength = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        uLength++;
    }

    // Do the actual formatting, right to left
    uTemp = uNum;
    ptr = (char*)pString + uLength;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = digits[uTemp % 10];
        uTemp /= 10;
    }
    return uLength;
}

int iptostring(unsigned char *ip, char *ipstring)
{
  int temp,i, length, uLength;
  char *ptr;
  ip =ip +3;
  ptr = ipstring;
  uLength = 0;
  for (i=0; i<4; i++)
  {
    temp = *ip;
    length = String_utoa((unsigned long) temp, ptr);
    ptr = ptr + length;
    uLength += length;
    *ptr = '.';
    ptr++;
    uLength++;
    ip--;
  }
  return (uLength-1);  
}

//*****************************************************************************
//
//! main
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The main loop is executed here
//
//*****************************************************************************

void main(void)
{
  char ipvalue[15];
  int length;
  runSmartConfig = 0;
  obtainIpInfoFlag = 0;
  skipCount = 0;
  mDNSValid = 0;
  mDNSSend =0;

  //Stop watch dog timer, In IAR this is done in low_level_init.c file
#ifdef __CCS__
  WDTCTL = WDTPW + WDTHOLD;
#endif

    // Start CC3000 State Machine
    resetCC3000StateMachine();

    //
    //  Board Initialization start
    //
        
    initDriver();
    Wheel_init();
    Wheel_enable();

    __enable_interrupt();
	DispatcherUartSendPacket("\r\n\r\n--Initializing Application--",32);
	DispatcherUartSendPacket("\r\nWeb Server App Version=",25);
	DispatcherUartSendPacket(SERVERAPPVERSION,14);
    while(1)
    {
    	// Perform Smart Config if button pressed
    	        if(runSmartConfig == 1)
    	        {
                    mDNSValid = 0;
    	            // Turn Off PAD LED's
                    turnLedOff(LED4);
                    turnLedOff(LED5);
                    turnLedOff(LED6);
                    turnLedOff(LED7);
                    turnLedOff(LED8);

                    // Start the Smart Config Process
    	            StartSmartConfig();
    	            runSmartConfig = 0;
    	        }

    	        if(!(currentCC3000State() & CC3000_ASSOC))
    	        {
    	            
    	            // Wait until connection is finished
    	            while (!(currentCC3000State() & CC3000_ASSOC))
    	            {
    	                __delay_cycles(100);
    	                // Check if user pressed button to do Smart Config
    	                if(runSmartConfig == 1)
    	                    break;
    	            }
    	        }

    	        // Print out connection status
    	        if(currentCC3000State() & CC3000_IP_ALLOC && obtainIpInfoFlag == FALSE)
    	        {
    	        	// Set flag so we don't constantly obtain the IP info
    	            obtainIpInfoFlag = TRUE;
    	            turnLedOn(CC3000_IP_ALLOC_IND);
    	         }

    	        if(currentCC3000State() & CC3000_IP_ALLOC && obtainIpInfoFlag == TRUE)
    	        {
    	        	//If smart config was done was broadcast mdns signal
    	        	if (ConnectUsingSmartConfig==1)
    	        	{
    	        	    mdnsAdvertiser(1,DevServname, sizeof(DevServname));
    	        	    ConnectUsingSmartConfig = 0;
    	        	}
                        mDNSValid =1;
                        skipCount =0;
                        ipconfig = getCC3000Info();
                        length = iptostring(ipconfig->aucIP, ipvalue);
                        DispatcherUartSendPacket("\r\nIP = ", 7);
                        DispatcherUartSendPacket((unsigned char *)ipvalue, length);
                        //Initialize and start the HTTP server
                        HttpServerInitAndRun();
    	        }
    }
}
