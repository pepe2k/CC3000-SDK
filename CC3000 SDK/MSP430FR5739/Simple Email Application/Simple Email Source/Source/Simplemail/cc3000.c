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
#include "spi_version.h"
#include "host_driver_version.h"
#include "dispatcher.h"
#include "board.h"
#include "spi.h"
#include "security.h"

#define PALTFORM_VERSION						(1)
#define APPLICATION_VERSION						(2)
#define DISABLE									(0)
#define ENABLE									(1)

long ulSocket;

/** \brief Indicates whether the Smart Config Process has finished */
volatile unsigned long ulSmartConfigFinished = 0;

unsigned char pucIP_Addr[4];
unsigned char pucIP_DefaultGWAddr[4];
unsigned char pucSubnetMask[4];
unsigned char pucDNS[4];
volatile unsigned char ucStopSmartConfig;
sockaddr tSocketAddr;

unsigned char prefixChangeFlag = 0;
unsigned char prefixFromUser[3] = {0};
char * ftcPrefixptr;

tNetappIpconfigRetArgs ipinfo;

char debugi = 0;
// Smart Config Prefix - Texas Instruments
char aucCC3000_prefix[3] = {'T', 'T', 'T'};

#define FRAM_SC_INFO_ADDRESS       0x1800
#define CC3000_APP_BUFFER_SIZE									(5)
#define CC3000_RX_BUFFER_OVERHEAD_SIZE							(20)

unsigned char *FRAM_SMART_CONFIG_WRITTEN_ptr = (unsigned char *)FRAM_SC_INFO_ADDRESS;   
extern char digits[] = "0123456789";
char cc3000state = CC3000_UNINIT;

//AES key "smartconfigAES16"
const unsigned char smartconfigkey[] = {0x73,0x6d,0x61,0x72,0x74,0x63,0x6f,0x6e,0x66,0x69,0x67,0x41,0x45,0x53,0x31,0x36};

extern volatile unsigned long ulCC3000Connected;
extern volatile unsigned long SendmDNSAdvertisment;
extern unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];
extern unsigned char printOnce;
extern volatile unsigned long ulCC3000DHCP,
OkToDoShutDown, ulCC3000DHCP_configured;

const unsigned char pucUARTCommandSmartConfigDoneString[] = {'\f', '\r','S', 'm', 'a', 'r', 't',' ', 'c', 'o', 'n', 'f', 'i', 'g', ' ',  'D', 'O', 'N', 'E', '\f', '\r'};



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
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch:
//!         since there is no patch in the host - it returns 0
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
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the boot loader patch:
//!         since there is no patch in the host - it returns 0
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
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the FW patch:
//!         since there is no patch in the host - it returns 0
//
//*****************************************************************************

char *sendWLFWPatch(unsigned long *Length)
{
	*Length = 0;
	return NULL;
}


//*****************************************************************************
//
//! Disconnect All
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  Gracefully disconnects us completely
//
//*****************************************************************************
void disconnectAll()
{

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
    if(!(currentCC3000State() & CC3000_SERVER_INIT))
    {
        // If we're not blocked by accept or others, obtain the latest
        netapp_ipconfig(&ipinfo);
    }    
    return (&ipinfo);
}
#endif
//*****************************************************************************
//
//!  \brief  Checks whether Smart Config information is set in the CC3000
//!  
//!  \param  None
//!
//!  \return none
//!  \warning This function assumes that the CC3000 module was previously
//!           run with this same board. If not, it may incorrectly assume that
//!           the CC3000 was previously configured for FTC since it stores the
//!           flag in the FRAM.
//!  
//
//*****************************************************************************
char isFTCSet()
{
    if(*FRAM_SMART_CONFIG_WRITTEN_ptr == SMART_CONFIG_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//*****************************************************************************
//
//!  \brief  Sets the flag indicating Smart Config information is set in the 
//!          CC3000
//!  
//!  \param  None
//!
//!  \return none
//!  \warning This function assumes that the CC3000 module was previously
//!           run with this same board. If not, it may incorrectly assume that
//!           the CC3000 was previously configured for Smart Config since it stores the
//!           flag in the FRAM.
//!  
//
//*****************************************************************************
void  setFTCFlag()
{
    *FRAM_SMART_CONFIG_WRITTEN_ptr = SMART_CONFIG_SET;
}


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

	ulSmartConfigFinished = 0;
	ulCC3000Connected = 0;
	ulCC3000DHCP = 0;
	OkToDoShutDown=0;

	// Reset all the previous configuration
	wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
	wlan_ioctl_del_profile(255);
	
	//Wait until CC3000 is disconnected
	while (ulCC3000Connected == 1)
	{
		__delay_cycles(1000);

	}

	// Start blinking LED6 during Smart Configuration process
	turnLedOn(6);
	wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
	turnLedOff(6);

	// Start the SmartConfig start process
	wlan_smart_config_start(0);
	
	turnLedOn(6);
	
	// Wait for Smartconfig process complete
	while (ulSmartConfigFinished == 0)
	{

		__delay_cycles(6000000);

		turnLedOff(6);

		__delay_cycles(6000000);

		turnLedOn(6);

	}

	turnLedOn(6);
#ifndef CC3000_UNENCRYPTED_SMART_CONFIG
	// create new entry for AES encryption key
	nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);

	// write AES key to NVMEM
	aes_write_key((unsigned char *)(&smartconfigkey[0]));

	// Decrypt configuration information and add profile
	wlan_smart_config_process();
#endif

	// Configure to connect automatically to the AP retrieved in the 
	// Smart config process
	wlan_ioctl_set_connection_policy(DISABLE, DISABLE, ENABLE);
	
	// reset the CC3000
	wlan_stop();

	__delay_cycles(6000000);

	DispatcherUartSendPacket((unsigned char*)pucUARTCommandSmartConfigDoneString, sizeof(pucUARTCommandSmartConfigDoneString));

	wlan_start(0);

	// Mask out all non-required events
	wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}
