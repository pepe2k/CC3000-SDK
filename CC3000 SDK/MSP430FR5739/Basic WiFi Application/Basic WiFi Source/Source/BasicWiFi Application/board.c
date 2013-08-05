/*****************************************************************************
*
*  board.c - FRAM board functions
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
#include "netapp.h"
#include "board.h"


#define FRAM_FORCED_RES_ADDRESS       0x1840
#define FIRST_TIME_CONFIG_SET 0xAA

unsigned char * msp430_forced_restart_ptr = (unsigned char *)FRAM_FORCED_RES_ADDRESS;  
#ifdef __GNUC__
unsigned char * ptrFtcAtStartup = 0x1830;
#else
extern unsigned char * ptrFtcAtStartup;
#endif
//*****************************************************************************
//
//! pio_init
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Initialize the board's I/O
//
//*****************************************************************************    

void pio_init()
{
	// Init the device with 24MHz DCOCLCK.
	initClk();
	
	// Enable switches
	// P4.0 and P4.1 are configured as switches
	// Port 4 has only two pins    
	P4OUT |= BIT0;                      // Configure pullup resistor  
	P4DIR &= ~(BIT0);                  // Direction = input
	P4REN |= BIT0;                     // Enable pullup resistor
	P4IES |= (BIT0);                   // P4.0 Hi/Lo edge interrupt  
	P4IFG = 0;                         // P4 IFG cleared
	P4IE = BIT0;                       // P4.0 interrupt enabled
	
	// Enable First Time Config Prefix changing jumper
	// To detect if pulled high by VCC
	
	// P3.3 is our High signal
	P3DIR |= BIT3;
	P3OUT |= BIT3;
	
	// P3.2 Configure pulled low and will detect a hi/low transition
	
	P3DIR &= ~(BIT2);             // P3.2 As Input
	P3OUT &= ~(BIT2);             // P3.2 With Pulldown
	P3REN |= BIT2;                // P3.2 Enable Pulldown
	P3IES &= ~(BIT2);             // P3.2 Lo/Hi edge interrupt
	P3IFG &= ~(BIT2);             // P3.2 IFG cleared
	P3IE |= BIT2;                 // P3.2 interrupt enabled
	
	// P4.1 - WLAN enable full DS
 	WLAN_EN_PORT_OUT &= ~WLAN_EN_PIN;
	WLAN_EN_PORT_DIR |= WLAN_EN_PIN;
	WLAN_EN_PORT_SEL2 &= ~WLAN_EN_PIN; 
	WLAN_EN_PORT_SEL &= ~WLAN_EN_PIN;
	
	
	// Configure SPI IRQ line on P2.3
	SPI_IRQ_PORT_DIR  &= (~SPI_IRQ_PIN);	
	SPI_IRQ_PORT_SEL2 &= ~SPI_IRQ_PIN; 
	SPI_IRQ_PORT_SEL &= ~SPI_IRQ_PIN;
	
	
	// Configure the SPI CS to be on P1.3
	SPI_CS_PORT_OUT |= SPI_CS_PIN;
	SPI_CS_PORT_DIR |= SPI_CS_PIN;
	SPI_CS_PORT_SEL2 &= ~SPI_CS_PIN; 
	SPI_CS_PORT_SEL &= ~SPI_CS_PIN;
	
	__delay_cycles(12000000);
	
	initLEDs();
}
//*****************************************************************************
//
//! ReadWlanInterruptPin
//!
//! @param  none
//!
//! @return none
//!
//! @brief  return wlan interrup pin
//
//*****************************************************************************

long ReadWlanInterruptPin(void)
{
	return (P2IN & BIT3);
}

//*****************************************************************************
//
//! WlanInterruptEnable
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Enable waln IrQ pin
//
//*****************************************************************************

void WlanInterruptEnable()
{
	__bis_SR_register(GIE);
	P2IES |= BIT3;
	P2IE |= BIT3;
}

//*****************************************************************************
//
//! WlanInterruptDisable
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Disable waln IrQ pin
//
//*****************************************************************************

void WlanInterruptDisable()
{
	P2IE &= ~BIT3;
}

//*****************************************************************************
//
//! WriteWlanPin
//!
//! @param  val value to write to wlan pin
//!
//! @return none
//!
//! @brief  write value to wlan pin
//
//*****************************************************************************

void WriteWlanPin( unsigned char val )
{
	if (val)
	{
		P4OUT |= BIT1;	
	}
	else
	{
		P4OUT &= ~BIT1;
	}
}

//*****************************************************************************
//
//! wakeup_timer_init
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function initializes the timer
//
//*****************************************************************************
void wakeup_timer_init(void)
{
	TA1CCTL0 &= ~CCIE; 
	TA1CTL |= MC_0;
	
	// Configure teh timer for each 500 milli to handle un-solicited events
	TA1CCR0 = 0x4000;
	
	// run the timer from ACLCK, and enable interrupt of Timer A
	TA1CTL |= (TASSEL_1 + MC_1 + TACLR);
	
	TA1CCTL0 |= CCIE;
}


//*****************************************************************************
//
//! wakeup_timer_disable
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  Stop the timer
//
//*****************************************************************************
void wakeup_timer_disable(void)
{
	TA1CCTL0 &= ~CCIE; 
	TA1CTL |= MC_0;
}

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{              
	__bic_SR_register_on_exit(LPM3_bits);
	__no_operation();                          // For debugger
}


//*****************************************************************************
//
//! initClk
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  Init the device with 16 MHz DCOCLCK.
//
//*****************************************************************************
void initClk(void)
{
	
  // SMCLCK which will source also SPI will be sourced also by DCO
	CSCTL0_H = 0xA5;
	CSCTL1 |= DCORSEL + DCOFSEL0 + DCOFSEL1;	 // Set max. DCO setting
	CSCTL2 = SELA_1 + SELS_3 + SELM_3;		// set ACLK - VLO, the rest  = MCLK = DCO
	CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0;		// set all dividers to 0
	
}

//*****************************************************************************
//
//! StartDebounceTimer
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Starts timer that handles switch debouncing
//
//*****************************************************************************
void StartDebounceTimer()
{  
	// default delay = 0
	// Debounce time = 1500* 1/8000 = ~200ms
	TB0CCTL0 = CCIE;                          // TACCR0 interrupt enabled
	TB0CCR0 = 3000;
	TB0CTL = TBSSEL_1 + MC_1;                 // SMCLK, continuous mode
}

//*****************************************************************************
//
//! StopDebounceTimer
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Stops timer that handles switch debouncing
//
//*****************************************************************************
void StopDebounceTimer()
{  
	// TACCR0 interrupt enabled
	TB0CCTL0 &= ~CCIE;                          
}

//*****************************************************************************
//
//! initLEDs
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Initializes LED Ports and Pins
//
//*****************************************************************************
void initLEDs()
{
	PJOUT &= ~(BIT0 + BIT1 + BIT2 + BIT3);
	P3OUT &= ~(BIT4 + BIT5 + BIT6 + BIT7);
	
	PJDIR |= (BIT0 + BIT1 + BIT2 + BIT3);
	P3DIR |= (BIT4 + BIT5 + BIT6 + BIT7);
}

//*****************************************************************************
//
//!  turnLedOn
//!
//! @param  ledNum is the LED Number
//!
//! @return none
//!
//! @brief  Turns a specific LED on
//
//*****************************************************************************
void turnLedOn(char ledNum)
{
	switch(ledNum)
	{
	case LED1:
		PJOUT |= (BIT0); 
		break;
	case LED2:
		PJOUT |= (BIT1); 
		break;
	case LED3:
		PJOUT |= (BIT2); 
		break;
	case LED4:
		PJOUT |= (BIT3);
		break;
	case LED5:
		P3OUT |= (BIT4);
		break;
	case LED6:
		P3OUT |= (BIT5);
		break;
	case LED7:
		P3OUT |= (BIT6);
		break;
	case LED8:
		P3OUT |= (BIT7);
		break;
	}
}

//*****************************************************************************
//
//! turnLedOff
//!
//! @param  ledNum is the LED Number
//!
//! @return none
//!
//! @brief  Turns a specific LED Off
//
//*****************************************************************************    
void turnLedOff(char ledNum)
{                     
	switch(ledNum)
	{
	case LED1:
		PJOUT &= ~(BIT0); 
		break;
	case LED2:
		PJOUT &= ~(BIT1); 
		break;
	case LED3:
		PJOUT &= ~(BIT2); 
		break;
	case LED4:
		PJOUT &= ~(BIT3);
		break;
	case LED5:
		P3OUT &= ~(BIT4);
		break;
	case LED6:
		P3OUT &= ~(BIT5);
		break;
	case LED7:
		P3OUT &= ~(BIT6);
		break;
	case LED8:
		P3OUT &= ~(BIT7);
		break;
	}
}

//*****************************************************************************
//
//! toggleLed
//!
//! @param  ledNum is the LED Number
//!
//! @return none
//!
//! @brief  Toggles a board LED
//
//*****************************************************************************    

void toggleLed(char ledNum)
{
	switch(ledNum)
	{
	case LED1:
		PJOUT ^= (BIT0); 
		break;
	case LED2:
		PJOUT ^= (BIT1); 
		break;
	case LED3:
		PJOUT ^= (BIT2); 
		break;
	case LED4:
		PJOUT ^= (BIT3);
		break;
	case LED5:
		P3OUT ^= (BIT4);
		break;
	case LED6:
		P3OUT ^= (BIT5);
		break;
	case LED7:
		P3OUT ^= (BIT6);
		break;
	case LED8:
		P3OUT ^= (BIT7);
		break;
	}
}

//*****************************************************************************
//
//! IsFTCflagSet
//!
//! @brief  check if FTC flag was set
//!
//! @param  none
//!
//! @return indication if flag is set
//!
//
//*****************************************************************************
long IsFTCflagSet()
{  
	//  check if FTC flag is configure
  return (P4OUT&BIT0) ;                                
}

//*****************************************************************************
//
//! SetFTCflag
//!
//! @param  none
//!
//! @return none
//!
//! @brief  set FTC flag when S2 was pressed
//
//*****************************************************************************
void SetFTCflag()
{  
	//  set FTC flag
	*ptrFtcAtStartup = FIRST_TIME_CONFIG_SET;         
}

//*****************************************************************************
//
//! ClearFTCflag
//!
//! @param  none
//!
//! @return none
//!
//! @brief  clear FTC flag when S2 was pressed
//
//*****************************************************************************
void ClearFTCflag()
{  
	// clear FTC flag  
  *ptrFtcAtStartup = 0xFF;                             
}

  
//*****************************************************************************
//
//! DissableSwitch
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Dissable S2 switch interrupt
//
//*****************************************************************************
void DissableSwitch()
{  
	// disable switch interrupt
	
	P4IFG &= ~BIT0;                // Clear P4.0 IFG
	P4IE &= ~(BIT0);               // P4.0 interrupt disabled
	P4IFG &= ~BIT0;                // Clear P4.0 IFG
	P4IFG = 0;
}

//*****************************************************************************
//
//! RestoreSwitch
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Restore S2 switch interrupt
//
//*****************************************************************************
void RestoreSwitch()
{                     
	// Restore Switch Interrupt
	P4IFG &= ~BIT0;                 // Clear P4.0 IFG
	P4IE |= BIT0;                   // P4.0 interrupt enabled
	P4IFG &= ~BIT0;                 // Clear P4.0 IFG
}

//*****************************************************************************
//
//! switchIsPressed
//!
//! @param  none
//!
//! @return none
//!
//! @brief  Indication if the switch is still pressed
//
//*****************************************************************************
long switchIsPressed()
{  	
	if(!(P1IN & BIT3))
		return 1;
	return 0;
}

//*****************************************************************************
//
//! restartMSP430
//!
//! @param  none
//!
//! @return never
//!
//! @brief   Restarts the MSP430 completely. One must be careful
//
//*****************************************************************************    
void restartMSP430()
{	
	PMMCTL0 |= PMMSWPOR;
	// This function will never exit since it forces a complete
	// restart of the MSP430.    
}

//Catch interrupt vectors that are not initialized.

#ifdef __CCS__
#pragma vector=PORT1_VECTOR, WDT_VECTOR, TIMER1_A1_VECTOR, TIMER0_A1_VECTOR, TIMER0_A0_VECTOR, ADC10_VECTOR, UNMI_VECTOR,COMP_D_VECTOR,	DMA_VECTOR, PORT3_VECTOR, PORT4_VECTOR, RTC_VECTOR, TIMER0_B0_VECTOR, TIMER0_B1_VECTOR, TIMER1_B0_VECTOR, TIMER1_B1_VECTOR, TIMER2_B0_VECTOR, TIMER2_B1_VECTOR,SYSNMI_VECTOR, USCI_A1_VECTOR, USCI_B0_VECTOR
__interrupt void Trap_ISR(void)
{
  while(1);
}

#endif


