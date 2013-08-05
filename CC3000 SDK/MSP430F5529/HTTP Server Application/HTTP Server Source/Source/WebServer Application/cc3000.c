/*****************************************************************************
*
*  cc3000.c - CC3000 Functions Implementation
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
#include <msp430.h>
#include "wlan.h" 
#include "evnt_handler.h"    // callback function declaration
#include "nvmem.h"
#include "socket.h"
#include "common.h"
#include "netapp.h"
#include "common.h"
#include "demo_config.h"
#include "spi.h"
#include "board.h"
#include "dispatcher.h"
#include "spi_version.h"
#include "application_version.h"
#include "host_driver_version.h"
#include "security.h"

#define PALTFORM_VERSION						(6)


// Variable to indicate whether the Smart Config Process has finished
volatile unsigned long ulSmartConfigFinished = 0;

unsigned char pucIP_Addr[4];
unsigned char pucIP_DefaultGWAddr[4];
unsigned char pucSubnetMask[4];
unsigned char pucDNS[4];

tNetappIpconfigRetArgs ipinfo;

// Smart Config Prefix - Texas Instruments
char aucCC3000_prefix[3] = {'T', 'T', 'T'};
extern char digits[];

const unsigned char smartconfigkey[] = {0x73,0x6d,0x61,0x72,0x74,0x63,0x6f,0x6e,0x66,0x69,0x67,0x41,0x45,0x53,0x31,0x36};

const unsigned char pucUARTExampleAppString[] = {'\f', '\r','E', 'x', 'a', 'm', 'p', 'l', 'e', ' ', 'A', 'p', 'p', ':', 'd', 'r', 'i', 'v', 'e', 'r', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', ' ' };

char cc3000state = CC3000_UNINIT;

extern unsigned char ConnectUsingSmartConfig; 
extern volatile unsigned long ulCC3000Connected;
char CheckSocket = 0;
signed char sd[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};

//*****************************************************************************
//
//! ConnectUsingSSID
//!
//! \param  ssidName is a string of the AP's SSID
//!
//! \return none
//!
//! \brief  Connect to an Access Point using the specified SSID
//
//*****************************************************************************
int ConnectUsingSSID(char * ssidName)
{
                    
    unsetCC3000MachineState(CC3000_ASSOC);
    
    // Disable Profiles and Fast Connect
    wlan_ioctl_set_connection_policy(0, 0, 0);
    
    wlan_disconnect();
    
    __delay_cycles(10000);
    
    // This triggers the CC3000 to connect to specific AP with certain parameters
    //sends a request to connect (does not necessarily connect - callback checks that for me)
#ifndef CC3000_TINY_DRIVER
    wlan_connect(0, ssidName, strlen(ssidName), NULL, NULL, 0);   
#else
    wlan_connect(ssidName, strlen(ssidName));
#endif
    // We don't wait for connection. This is handled somewhere else (in the main
    // loop for example).
    
    return 0;      
}


//*****************************************************************************
//
//! itoa
//!
//! @param[in]  integer number to convert
//!
//! @param[in/out]  output string
//!
//! @return number of ASCII parameters
//!
//! @brief  Convert integer to ASCII in decimal base
//
//*****************************************************************************
unsigned short itoa(char cNum, char *cString)
{
	char* ptr;
	char uTemp = cNum;
	unsigned short length;
	
	// value 0 is a special case
	if (cNum == 0)
	{
		length = 1;
		*cString = '0';
		
		return length;
	}
	
	// Find out the length of the number, in decimal base
	length = 0;
	while (uTemp > 0)
	{
		uTemp /= 10;
		length++;
	}
	
	// Do the actual formatting, right to left
	uTemp = cNum;
	ptr = cString + length;
	while (uTemp > 0)
	{
		--ptr;
		*ptr = digits[uTemp % 10];
		uTemp /= 10;
	}
	
	return length;
}


//*****************************************************************************
//
//! sendDriverPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the driver patch: since there is no patch yet - 
//!				it returns 0
//
//*****************************************************************************
char *sendDriverPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}


//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the boot loader patch: since there is no patch yet -
//!				it returns 0
//
//*****************************************************************************
char *sendBootLoaderPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! \param  pointer to the length
//!
//! \return none
//!
//! \brief  The function returns a pointer to the FW patch: since there is no patch yet - it returns 0
//
//*****************************************************************************

char *sendWLFWPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}


//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! \param  Event type
//!
//! \return none
//!
//! \brief  The function handles asynchronous events that come from CC3000 device 
//!		  and operates a LED4 to have an on-board indication
//
//*****************************************************************************

void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{
    if (lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
    {
        ulSmartConfigFinished = 1;      
    }
    
    if (lEventType == HCI_EVNT_WLAN_UNSOL_INIT)
    {
        setCC3000MachineState(CC3000_INIT);
    }
    if (lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
    {
			  ulCC3000Connected = 1;
        setCC3000MachineState(CC3000_ASSOC);
        
    }
    
    if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
    {
		ulCC3000Connected = 0;
        //restartMSP430();
        unsetCC3000MachineState(CC3000_ASSOC);
        
    }
    if (lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
    {
        setCC3000MachineState(CC3000_IP_ALLOC);        
    }
    
    // This Event is gengerated when the TCP connection is Half closed
    if (lEventType == HCI_EVNT_BSD_TCP_CLOSE_WAIT)
    {
      sd[data[0]] = 1;
      CheckSocket = 1;
    }
}

//*****************************************************************************
//
//! initDriver
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The function initializes a CC3000 device and triggers it to start operation
//
//*****************************************************************************
int
  initDriver(void)
{
        // Init GPIO's
	pio_init();
	
	// Init SPI
	init_spi();
        
    DispatcherUARTConfigure();
        
    // Globally enable interrupts
	__enable_interrupt();
        
    //
    // WLAN On API Implementation
    //
    wlan_init( CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);
    
    //
    // Trigger a WLAN device
    //
    wlan_start(0);
    
    //
    // Mask out all non-required events from CC3000
    //

    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_ASYNC_PING_REPORT|HCI_EVNT_WLAN_UNSOL_INIT);

    // Generate event to CLI: send a version string
    char cc3000IP[50];
	char *ccPtr;
	unsigned short ccLen;
		
	DispatcherUartSendPacket((unsigned char*)pucUARTExampleAppString, sizeof(pucUARTExampleAppString));
		
	ccPtr = &cc3000IP[0];
	ccLen = itoa(PALTFORM_VERSION, ccPtr);
	ccPtr += ccLen;
	*ccPtr++ = '.';
	ccLen = itoa(APPLICATION_VERSION, ccPtr);
	ccPtr += ccLen;
	*ccPtr++ = '.';
	ccLen = itoa(SPI_VERSION_NUMBER, ccPtr);
	ccPtr += ccLen;
	*ccPtr++ = '.';
	ccLen = itoa(DRIVER_VERSION_NUMBER, ccPtr);
	ccPtr += ccLen;
	*ccPtr++ = '\r';
        *ccPtr++ = '\n';
	*ccPtr++ = '\0';
        ccLen = strlen(cc3000IP);
		
	DispatcherUartSendPacket((unsigned char*)cc3000IP, strlen(cc3000IP));
    
    // CC3000 has been initialized
    setCC3000MachineState(CC3000_INIT);
    
    unsigned long aucDHCP, aucARP, aucKeepalive, aucInactivity;
    
    aucDHCP = 14400;
    aucARP = 3600;      
    aucKeepalive = 10;
    aucInactivity = 50;
    
    if(netapp_timeout_values(&(aucDHCP), &(aucARP), &(aucKeepalive), &(aucInactivity)) != 0)
    {
      while(1);
    }
    
    return(0);
}


//*****************************************************************************
//
//!  \brief  Return the highest state which we're in
//!
//!  \param  None
//!
//!  \return none
//!
//
//*****************************************************************************
char highestCC3000State()
{
    // We start at the highest state and go down, checking if the state
    // is set.
    char mask = 0x80;
    while(!(cc3000state & mask))
    {
        mask = mask >> 1;
    }
        
    return mask;
}

//*****************************************************************************
//
//!  \brief  Return the current state bits
//!
//!  \param  None
//!
//!  \return none
//!
//
//*****************************************************************************
char currentCC3000State()
{
    return cc3000state;
}

void setCC3000MachineState(char stat)
{	    
    char bitmask = stat;
    cc3000state |= bitmask;
    
    int i = FIRST_STATE_LED_NUM;
    
    // Find LED number which needs to be set
    while(bitmask < 0x80)
    {      
        bitmask  = bitmask << 1;
        i++;
    }    
    turnLedOn(NUM_STATES-i+2);

}


//*****************************************************************************
//
//!  \brief  Unsets a state from the state machine
//!  Also handles LEDs
//!  
//!  \param  None
//!
//!  \return none
//!  
//
//*****************************************************************************
void unsetCC3000MachineState(char stat)
{
    char bitmask = stat;
    cc3000state &= ~bitmask;
    
    int i = FIRST_STATE_LED_NUM;
    int k = NUM_STATES; // Set to last element in state list
    
    // Set all upper bits to 0 as well since state machine cannot have
    // those states.
    while(bitmask < 0x80)
    {
        cc3000state &= ~bitmask;
        bitmask = bitmask << 1;
        i++;
    }
    
    // Turn off all upper state LEDs
    for(; i > FIRST_STATE_LED_NUM; i--)
    {
        turnLedOff(k);
        k--;
    }    
}

//*****************************************************************************
//
//!  \brief  Resets the State Machine
//!  
//!  \param  None
//!
//!  \return none
//!  
//
//*****************************************************************************
void resetCC3000StateMachine()
{
    cc3000state = CC3000_UNINIT;

    // Turn off all Board LEDs

    turnLedOff(CC3000_ON_IND);
    turnLedOff(CC3000_ASSOCIATED_IND);
    turnLedOff(CC3000_IP_ALLOC_IND);
    turnLedOff(CC3000_SERVER_INIT_IND);
    turnLedOff(CC3000_CLIENT_CONNECTED_IND);
    turnLedOff(CC3000_SENDING_DATA_IND);
    turnLedOff(CC3000_UNUSED1_IND);
    turnLedOff(CC3000_FTC_IND);
}

//*****************************************************************************
//
//!  \brief  Obtains the CC3000 Connection Information from the CC3000
//!  
//!  \param  None
//!
//!  \return none
//!  
//
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER
tNetappIpconfigRetArgs * getCC3000Info()
{
    netapp_ipconfig(&ipinfo);
    return (&ipinfo);
}
#endif

//*****************************************************************************
//
//! StartSmartConfig
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The function triggers a smart configuration process on CC3000.
//!			it exists upon completion of the process
//
//*****************************************************************************

void StartSmartConfig(void)
{

	// Reset all the previous configuration
	//
	wlan_ioctl_set_connection_policy(0, 0, 0);	
        wlan_ioctl_del_profile(255);
	
	//Wait until CC3000 is dissconected
	while (ulCC3000Connected == 1)
	{
	  __delay_cycles(100);
    }

	// Start blinking LED6 during Smart Configuration process
	turnLedOn(LED6);
	wlan_smart_config_set_prefix(aucCC3000_prefix);
	//wlan_first_time_config_set_prefix(aucCC3000_prefix);
	turnLedOff(LED6);
	
    // Start the SmartConfig start process
	wlan_smart_config_start(1);
    turnLedOn(LED6);
	
	//
	// Wait for Smart config finished
	// 
	while (ulSmartConfigFinished == 0)
	{
		__delay_cycles(6000000);
                                                                             
		turnLedOn(LED6);

		__delay_cycles(6000000);
                
		turnLedOff(LED6);
	}

	turnLedOff(LED6);

    // create new entry for AES encryption key
	nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);
	
	// write AES key to NVMEM
	aes_write_key((unsigned char *)(&smartconfigkey[0]));
	
	// Decrypt configuration information and add profile
	wlan_smart_config_process();
        
	//
	// Configure to connect automatically to the AP retrieved in the 
	// Smart config process
	//
	wlan_ioctl_set_connection_policy(0, 0, 1);
	
	//
	// reset the CC3000
	// 
	wlan_stop();
	__delay_cycles(600000);
	wlan_start(0);

	ConnectUsingSmartConfig = 1; 
        ulSmartConfigFinished = 0;
	//
	// Mask out all non-required events
	//
	wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}
