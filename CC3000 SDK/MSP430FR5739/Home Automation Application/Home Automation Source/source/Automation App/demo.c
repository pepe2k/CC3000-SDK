/*****************************************************************************
*
*  demo.c - CC3000 Main Demo Application
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

#include <msp430.h>
#include "wlan.h" 
#include "evnt_handler.h"    // callback function declaration
#include "nvmem.h"
#include "socket.h"
#include "common.h"
#include "netapp.h"
#include "cc3000.h"
#include "sensors.h"
#include "board.h"
#include "string.h"
#include "uart.h"
#include "demo_config.h"
#include "terminal.h"
#include "server.h"
#include "version.h"
#include "spi.h"



/** \brief Flag indicating whether user requested to perform Smart Config */
volatile char runSmartConfig = 0;
//Flag to indicate Smart Config needs to be performed
unsigned char * ptrFtcAtStartup = (unsigned char *)0x1830;
//Flag to indicate Smart Config was performed in the past and CC3000 has a stored profile
unsigned char * SmartConfigProfilestored = (unsigned char *)0x1800;
unsigned char ConnectUsingSmartConfig = 0; 
/** \brief Flag indicating whether to print CC3000 Connection info */
unsigned char obtainIpInfoFlag = FALSE;

//Device name - used for Smart config in order to stop the Smart phone configuration process
char DevServname[] = {'C', 'C', '3', '0', '0', '0', '\0'};

volatile unsigned long ulCC3000Connected;
volatile unsigned long SendmDNSAdvertisment;
//*****************************************************************************
//
//!  main
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief   The main loop is executed here
//
//*****************************************************************************

void main(void)
{
    ulCC3000Connected = 0;
    SendmDNSAdvertisment = 0;
		
  // Stop WDT 
#ifdef __CCS__
    WDTCTL = WDTPW + WDTHOLD;
#elif __IAR_SYSTEMS_ICC__
	 asm("mov.w   #0x5a80,&0x015C; ");
#endif
    
    // Start CC3000 State Machine
    resetCC3000StateMachine();
    
    // Initialize Board and CC3000
    initDriver();  

    // Initialize Terminal Functionality
    initTerminal(); 
    
    // Initialize CC3000 Unsolicited Events Timer
    unsolicicted_events_timer_init();

    // Enable interrupts for UART
    __enable_interrupt();
    terminalPrint("\r\n\r\n-------Press ENTER for terminal prompt--------\r\n");
#ifdef HOME_AUTOMATION_APP_VERSION
    terminalPrint("Home Automation App version: ");
    terminalPrint(HOME_AUTOMATION_APP_VERSION);
    terminalPrint("\r\n");
#endif
    // Main Loop
    while (1)
    {
			
			
        // Perform Smart Config if button pressed in current run or if flag set in FRAM
        // from previous MSP430 Run.
        if(runSmartConfig == 1 || *ptrFtcAtStartup == SMART_CONFIG_SET)
        {
            // Clear flag
             ClearFTCflag();
   
            wlan_ioctl_set_connection_policy(0, 0, 0);
            wlan_disconnect();
            unsetCC3000MachineState(CC3000_ASSOC);
            
            // Start the Smart Config Process
            StartSmartConfig();
            runSmartConfig = 0;
        }
        
        if(!(currentCC3000State() & CC3000_ASSOC))
        {
            //
            // Check whether Smart Config was run previously. If it was, we
            // use it to connect to an access point. Otherwise, we connect to the
            // default.
            //
            
            if((isFTCSet() == 0)&&(ConnectUsingSmartConfig==0)&&(*SmartConfigProfilestored != SMART_CONFIG_SET))
            {
                // Smart Config not set, check whether we have an SSID
                // from the assoc terminal command. If not, use fixed SSID.
               
                if(isAssocSSIDValid() == 1)
                {                   
                    ConnectUsingSSID((char *)getassocSSID());
                }
                else
                {
                    ConnectUsingSSID(DEFAULT_OUT_OF_BOX_SSID);
                }
            }
            
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
            unsolicicted_events_timer_disable();
#ifndef CC3000_TINY_DRIVER        
             printConnectionInfo(getCC3000Info());
#endif               
            // Set flag so we don't constantly obtain the IP info
            obtainIpInfoFlag = TRUE;
            turnLedOn(CC3000_IP_ALLOC_IND);
            unsolicicted_events_timer_init();
        }
        
        if(currentCC3000State() & CC3000_IP_ALLOC && obtainIpInfoFlag == TRUE)
        {

            unsolicicted_events_timer_disable();
						//If Smart Config was performed, we need to send complete notification to the configure (Smart Phone App)
						if (ConnectUsingSmartConfig==1)
            {
                                                       mdnsAdvertiser(1,DevServname, strlen(DevServname));
							ConnectUsingSmartConfig = 0;
							*SmartConfigProfilestored = SMART_CONFIG_SET;
            }
						//Start mDNS timer in order to send mDNS Advertisement every 30 seconds
            mDNS_packet_trigger_timer_enable();			
            // Attempt to start data server
            terminalPrint("Starting Server...\r\n");
            initServer();
            if(currentCC3000State() & CC3000_SERVER_INIT)
            {
                waitForConnection();
            }
            else
            {
                __delay_cycles(100000);			//this should wait a second
            }
            unsolicicted_events_timer_init();
        }
    }
}


#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{    
    switch(__even_in_range(P4IV,P4IV_P4IFG1))
    {        
        case P4IV_P4IFG0:
            StartDebounceTimer();          //
            // disable switch interrupt
            DissableSwitch();
            break;
        default:
            break;
    }  
    P4IFG = 0;
}



// Timer B0 interrupt service routine
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B (void)
{
    // Check whether button is still pressed. If so, Smart Config
    // should be performed.
    
    if(!(switchIsPressed()))
    {
    	// Button is still pressed, so Smart Config should be done        
        runSmartConfig = 1;
        
        if(currentCC3000State() & CC3000_SERVER_INIT)
        {
            // Since accept and the server is blocking,
            // we will indicate in non-volatile FRAM that
            // Smart Config should be run at startup.
              SetFTCflag();
							// Clear Smart Config profile stored flag until connection established. To use the default SSID for connection, press S1 and then the reset button.
							*SmartConfigProfilestored = 0xFF;      
            restartMSP430();
        }
//		}
		
    }
    
    // Restore Switch Interrupt
        RestoreSwitch();
    
    StopDebounceTimer();
}


