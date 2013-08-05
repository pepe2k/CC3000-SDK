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
#include "board.h"
#include "string.h"
#include "dispatcher.h"
#include "version.h"
#include "spi_version.h"
#include "host_driver_version.h"
#include "smtp.h"
#include "spi.h"

extern int uart_have_cmd;
extern volatile unsigned long ulSmartConfigFinished;
extern volatile unsigned char ucStopSmartConfig;
extern long ulSocket;

#define UART_COMMAND_CC3000_SIMPLE_CONFIG_START                 (0x31)
#define UART_COMMAND_CC3000_EMAIL_CONFIG						(0x32)
#define UART_COMMAND_CC3000_EMAIL_HEADER						(0x33)
#define UART_COMMAND_CC3000_EMAIL_MESSAGE						(0x34)
#define UART_COMMAND_CC3000_EMAIL_SEND							(0x35)
#define CC3000_APP_BUFFER_SIZE									(5)
#define CC3000_RX_BUFFER_OVERHEAD_SIZE							(20)
#define NETAPP_IPCONFIG_MAC_OFFSET								(20)
#define PALTFORM_VERSION										(1)
#define APPLICATION_VERSION										(2)
#define DISABLE													(0)
#define ENABLE													(1)

unsigned char printOnce = 1;
//device name used by smart config response
const char device_name[] = "home_assistant";

// UART Strings for Menu and acknowledge
const unsigned char putUARTOKString[] = {'\f', '\r', 'O', 'K','\f', '\r'};
const unsigned char putUARTErrorString[] = {'\f', '\r', 'E', 'R', 'R', 'O', 'R','\f', '\r'};
const unsigned char pucUARTCommandDoneString[] = {'\f', '\r', 'D', 'O', 'N', 'E', '\f', '\r'};
const unsigned char pucUARTExampleAppString[] = {'\f', '\r','E', 'm', 'a', 'i', 'l', ' ', 'A', 'p', 'p', ' ', 'V', 'e', 'r', 's', 'i', 'o', 'n', ' ' };
const unsigned char pucUARTNoDataString[] = {'\f', '\r', 'N', 'o', ' ', 'd', 'a', 't', 'a', ' ', 'r', 'e', 'c','e', 'i', 'v', 'e', 'd', '\f', '\r'};
const unsigned char pucUARTIllegalCommandString[] = {'\f', '\r', 'I', 'l', 'l', 'e', 'g', 'a', 'l', ' ', 'c', 'o','m', 'm', 'a', 'n', 'd', '\f', '\r'};
const unsigned char putUARTIntroString[] = {'\f', '\r', 'C', 'O', 'M', 'M', 'A', 'N', 'D', ' ', 'M', 'E', 'N', 'U'};
const unsigned char putUARTIntroString1[] = {'\f','\r','0','1',',', 't','o',' ','s','t','a','r','t',' ','S','m','a','r','t','c','o','n','f','i','g'};
const unsigned char putUARTIntroString2[] = {'\f','\r','0','2',',','<','s','e','r','v','e','r','>',',','<','p','o','r','t','>',',','<','u','s','e','r','>'};
const unsigned char putUARTIntroString3[] = {'\f','\r','0','3',',','<','t','o','>',',','<','s','u','b','j','e','c','t','>'};
const unsigned char putUARTIntroString4[] = {'\f','\r','0','4',',','<','m', 'e', 's', 's', 'a', 'g', 'e', '>',};
const unsigned char putUARTIntroString5[] = {'\f','\r','0','5',' ','t', 'o', ' ', 's', 'e', 'n', 'd', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e','\f', '\r'};
const unsigned char putUARTFinishString[] = {'\f', '\r','M','e','s','s','a','g','e',' ','s','e','n','t','!','\f','\r'};

// UART-defined config variables for Email Application
char serverloc[25];
char *ofserverloc;
char serverport[5];
char *ofserverport;
char serveruser[25];
char *ofserveruser;
char emailto[25];
char *ofemailto;
char emailsubject[25];
char *ofemailsubject;
char emailmessage[64];
char *ofemailmessage;

volatile unsigned long SendmDNSAdvertisment;
volatile unsigned long ulCC3000Connected,ulCC3000DHCP,
OkToDoShutDown, ulCC3000DHCP_configured;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//__no_init is used to prevent the buffer initialization in order to prevent hardware WDT expiration    ///
// before entering to 'main()'.                                                                         ///
//for every IDE, different syntax exists :          1.   __CCS__ for CCS v5                             ///
//                                                  2.  __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench  ///
// *CCS does not initialize variables - therefore, __no_init is not needed.                             ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __CCS__

	unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];

#elif __IAR_SYSTEMS_ICC__

	__no_init unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];

#endif



//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType   Event type
//! @param  data
//! @param  length
//!
//! @return none
//!
//! @brief  The function handles asynchronous events that come from CC3000
//!		      device and operates a LED1 to have an on-board indication
//
//*****************************************************************************

void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length)
{

	if (lEventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE)
	{
		ulSmartConfigFinished = 1;
		ucStopSmartConfig     = 1;
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_CONNECT)
	{
		ulCC3000Connected = 1;

		// Turn on the LED7
		turnLedOn(2);
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
	{
		ulCC3000Connected = 0;
		ulCC3000DHCP      = 0;
		ulCC3000DHCP_configured = 0;
		printOnce = 1;

		// Turn off the LED7
		turnLedOff(2);

		// Turn off LED5
		turnLedOff(3);
	}

	if (lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
	{
		// Notes:
		// 1) IP config parameters are received swapped
		// 2) IP config parameters are valid only if status is OK, i.e. ulCC3000DHCP becomes 1

		// only if status is OK, the flag is set to 1 and the addresses are valid
		if ( *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0)
		{
			{
				char *ccPtr;
				unsigned short ccLen;

				pucCC3000_Rx_Buffer[0] = 'I';
				pucCC3000_Rx_Buffer[1] = 'P';
				pucCC3000_Rx_Buffer[2] = ':';

				ccPtr = (char*)&pucCC3000_Rx_Buffer[3];

				ccLen = itoa(data[3], ccPtr);
				ccPtr += ccLen;
				*ccPtr++ = '.';
				ccLen = itoa(data[2], ccPtr);
				ccPtr += ccLen;
				*ccPtr++ = '.';
				ccLen = itoa(data[1], ccPtr);
				ccPtr += ccLen;
				*ccPtr++ = '.';
				ccLen = itoa(data[0], ccPtr);
				ccPtr += ccLen;
				*ccPtr++ = '\f';
				*ccPtr++ = '\r';
				*ccPtr++ = '\0';

				ulCC3000DHCP = 1;

				turnLedOn(3);
			}
		}
		else
		{
			ulCC3000DHCP = 0;

			turnLedOff(3);
		}
	}

	if (lEventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN)
	{
		OkToDoShutDown = 1;
	}
}

//*****************************************************************************
//
//! initDriver
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function initializes a CC3000 device and triggers it to
//!          start operation
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

	// WLAN On API Implementation
	wlan_init( CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch, sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

	// Trigger a WLAN device
	wlan_start(0);

	// Turn on the LED 5 to indicate that we are active and initiated WLAN successfully
	turnLedOn(1);

	// Mask out all non-required events from CC3000
	wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);

	// Generate event to CLI: send a version string
	{
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
		*ccPtr++ = '\f';
		*ccPtr++ = '\r';
		*ccPtr++ = '\0';


		DispatcherUartSendPacket((unsigned char*)cc3000IP, strlen(cc3000IP));
	}
	DispatcherUartSendPacket((unsigned char*)putUARTIntroString1, sizeof(putUARTIntroString1));
	DispatcherUartSendPacket((unsigned char*)putUARTIntroString2, sizeof(putUARTIntroString2));
	DispatcherUartSendPacket((unsigned char*)putUARTIntroString3, sizeof(putUARTIntroString3));
	DispatcherUartSendPacket((unsigned char*)putUARTIntroString4, sizeof(putUARTIntroString4));
	DispatcherUartSendPacket((unsigned char*)putUARTIntroString5, sizeof(putUARTIntroString5));

	wakeup_timer_init();

	ucStopSmartConfig   = 0;

	return(0);
}


//*****************************************************************************
//
//! DemoHandleUartCommand
//!
//!  @param  usBuffer
//!
//!  @return none
//!
//!  @brief  The function handles commands arrived from CLI
//
//*****************************************************************************
void DemoHandleUartCommand(unsigned char *usBuffer)
{
	int index = 0;
	int paramcount = 0;

	switch(usBuffer[1])
	{
	//**********************************************
	// Case 01: Start a smart configuration process
	//**********************************************
	case UART_COMMAND_CC3000_SIMPLE_CONFIG_START:

		StartSmartConfig();
		break;

	//**********************************************
	// Case 02: Configure server name, port, and user
	//**********************************************
	case UART_COMMAND_CC3000_EMAIL_CONFIG:

		ofserverport = &serverport[0];
		ofserveruser = &serveruser[0];

		index = 2;
		while ((int)usBuffer[index] != 0x0D)
		{

			//look for comma ',' for separation of params
			if((int)usBuffer[index] == 44){
				paramcount++;
			}
			else{
				if(paramcount==1){
					//Enter smtp server host
					*ofserverloc++ = usBuffer[index];

				}
				if(paramcount==2){
					//Enter server port (i.e. 2525)
					*ofserverport++ = usBuffer[index];

				}
				if(paramcount==3){
					//Enter server user
					*ofserveruser++ = usBuffer[index];
				}
			}
			index++;
		}

		*ofserverport++= '\0';
		*ofserveruser++= '\0';
		DispatcherUartSendPacket((unsigned char*)putUARTOKString, sizeof(putUARTOKString));
		break;

	//**********************************************
	// Case 03: Configure Email 'To' and 'Subject'
	//**********************************************
	case UART_COMMAND_CC3000_EMAIL_HEADER:

		ofemailto = &emailto[0];
		ofemailsubject = &emailsubject[0];

		index = 2;
		while ((int)usBuffer[index] != 0x0D)
		{
			//look for comma ',' for separation of params
			if((int)usBuffer[index] == 44){
				paramcount++;
			}
			else{
				if(paramcount==1){
					*ofemailto++ = usBuffer[index];
				}
				if(paramcount==2){
					*ofemailsubject++ = usBuffer[index];
				}
			}
			index++;
		}
		*ofemailto++= '\0';
		*ofemailsubject++= '\0';
		DispatcherUartSendPacket((unsigned char*)putUARTOKString, sizeof(putUARTOKString));
		break;

	//**********************************************
	// Case 04: Record email message
	//**********************************************
	case UART_COMMAND_CC3000_EMAIL_MESSAGE:

		ofemailmessage = &emailmessage[0];

		//Enter "Message"
		index =3;
		while ((int)usBuffer[index] != 0x0D)
		{

			if((int)usBuffer[index] == 62){
				paramcount++;
			}
			else{
				if(paramcount==0){
					*ofemailmessage++ = usBuffer[index];
				}
			}
			index++;
		}
		*ofemailmessage= '\0';
		DispatcherUartSendPacket((unsigned char*)putUARTOKString, sizeof(putUARTOKString));
		break;

	//**********************************************
	// Case 05: Send email message using configurations
	//**********************************************
	case UART_COMMAND_CC3000_EMAIL_SEND:
		// Attempt to start data server
		smtpConnect((char *)serverloc, atoi(serverport));
		//Send Email using configured parameters
		smtpSend((char *)serveruser, (char *)emailto, (char *)emailsubject, (char *)emailmessage);


		for(index=0 ;index<5 ;index++){
			__delay_cycles(6000000);
			turnLedOn(7);
			__delay_cycles(6000000);
			turnLedOff(7);
		}
		DispatcherUartSendPacket((unsigned char*)putUARTFinishString, sizeof(putUARTFinishString));
		break;
	default:
		DispatcherUartSendPacket((unsigned char*)putUARTErrorString, sizeof(putUARTErrorString));
	}

}




//*****************************************************************************
//
//! main
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The main loop is executed here
//
//*****************************************************************************

main(void)
{
	ulCC3000DHCP = 0;
	ulCC3000Connected = 0;
	ulSocket = 0;
	ulSmartConfigFinished=0;

	WDTCTL = WDTPW + WDTHOLD;

	//  Board Initialization start
	initDriver();

	// Initialize the UART RX Buffer
	memset(g_ucUARTBuffer, 0xFF, UART_IF_BUFFER);
	uart_have_cmd =0;


	// Loop forever waiting  for commands from PC...
	while (1)
	{
		__bis_SR_register(LPM2_bits + GIE);
		__no_operation();

		// If command received over UART, process and parse
		if (uart_have_cmd)
		{
			wakeup_timer_disable();

			//Process the command in RX buffer
			DemoHandleUartCommand(g_ucUARTBuffer);
			uart_have_cmd = 0;
			memset(g_ucUARTBuffer, 0xFF, UART_IF_BUFFER);
			wakeup_timer_init();
		}

		// Complete smart config process:
		// 1. if smart config is done
		// 2. CC3000 established AP connection
		// 3. DHCP IP is configured
		// then send mDNS packet to stop external SmartConfig application
		if ((ucStopSmartConfig == 1) && (ulCC3000DHCP == 1) && (ulCC3000Connected == 1))
		{
			unsigned char loop_index = 0;

			while (loop_index < 3)
			{
				mdnsAdvertiser(1,(char *)device_name,strlen(device_name));
				loop_index++;
			}

			ucStopSmartConfig = 0;
		}

		if( (ulCC3000DHCP == 1) && (ulCC3000Connected == 1)  && (printOnce == 1) )
		{
			printOnce = 0;
			DispatcherUartSendPacket((unsigned char*)pucCC3000_Rx_Buffer, strlen((char const*)pucCC3000_Rx_Buffer));
		}

	}
}
