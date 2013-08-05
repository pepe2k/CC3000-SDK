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



#define SMART_CONFIG_SET 0xAA

 
extern unsigned char * ptrFtcAtStartup;
extern volatile unsigned long SendmDNSAdvertisment;
//*****************************************************************************
//
//! pio_init
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Initialize the board's I/O
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

#if 0
    P4OUT |= BIT1;                      // Configure pullup resistor  
    P4DIR &= ~(BIT1);                  // Direction = input
    P4REN |= BIT1;                     // Enable pullup resistor
    P4IES |= (BIT1);                   // P4.0 Hi/Lo edge interrupt  
    P4IFG = 0;                         // P4 IFG cleared
    P4IE |= BIT1;
#endif
    
    // Enable Smart Config Prefix changing jumper
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
//! \param  none
//!
//! \return none
//!
//! \brief  return wlan interrup pin
//
//*****************************************************************************

long ReadWlanInterruptPin(void)
{
    return (P2IN & BIT3);
}

//*****************************************************************************
//
//! Enable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
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
//! Disable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
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
//! \param  new val
//!
//! \return none
//!
//! \brief  void
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
//! unsolicicted_events_timer_init
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The function initializes the unsolicited events timer handler
//
//*****************************************************************************
void unsolicicted_events_timer_init(void)
{
    TA1CCTL0 &= ~CCIE; 
    TA1CTL |= MC_0;
    
    // Configure teh timer for each 500 milli to handle un-solicited events
    TA1CCR0 = 0x4000;
    
    // run the timer from ACLCK, and enable interrupt of Timer A
    TA1CTL |= (TASSEL_1 + MC_1 + TACLR);
    
    TA1CCTL0 |= CCIE;
}

void mDNS_packet_trigger_timer_enable(void)
{
    TA0CCTL0 &= ~CCIE;                         // TACCR0 interrupt enabled
		TA0CTL |= MC_0;
    // The timer clock is ~10KHz. Dividing it by 8 and than by 8 gives us clock of 156.25 Hz
    TA0EX0 = TAIDEX_7;
		 // We want to wakeup each 10 sec which is  0x1248 @ 156.25 Hz
    TA0CCR0 = 0x618;
		
  	// run the timer from ACLCK, and enable interrupt of Timer A0
		TA0CTL |= (TASSEL_1 + MC_1 + TACLR  + ID_3);
		
    TA0CCTL0 |= CCIE;
}
//*****************************************************************************
//
//! unsolicicted_events_timer_init
//!
//!  \param  None
//!
//!  \return none
//!
//!  \brief  The function initializes a CC3000 device and triggers it to start operation
//
//*****************************************************************************
void unsolicicted_events_timer_disable(void)
{
	TA1CCTL0 &= ~CCIE; 
	TA1CTL |= MC_0;
}


// Timer A1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{              

	__no_operation();                          // For debugger
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{              
	SendmDNSAdvertisment  = 1;  //Set flag to send mDNS Advertisment
                        
}


void InitPWMTimer(){
  TB1CCR0 = 0xFFFFu;
  TB1CCR1 = 0x0FFFu;
  TB1CCR2 = 0x0FFFu;
  TB1CTL = TBSSEL_2 + MC_2 +TBCLR + ID_2;// MCLK, contmode, /8
  

  TB2CCR0 = 0xFFFFu;
  TB2CCR1 = 0x0FFFu;
  TB2CCR2 = 0x0FFFu;
  TB2CTL = TBSSEL_2 + MC_2 +TBCLR + ID_2;// MCLK, contmode, /8

  P3SEL1 &= ~0xF0;
  P3SEL0 |= 0xF0;
  P3DIR |= 0xF0;

  
  //TB1EX0 = 7; 
}
void EnablePWMs(){

  //TB1CCTL1 = CCIE; // CCR1 interrupt enabled
  //TB1CCTL2 = CCIE; // CCR2 interrupt enabled
  //TB1CTL |= TBIE;
  TB1CCTL1 = OUTMOD_7;
  TB1CCTL2 = OUTMOD_7;
  TB2CCTL1 = OUTMOD_7;
  TB2CCTL2 = OUTMOD_7;

}


//*****************************************************************************
//
//! init clk
//!
//!  \param  None
//!
//!  \return none
//!
//!  \Init the device with 16 MHz DCOCLCK.
//
//*****************************************************************************
void initClk(void)
{
	 
  // SMCLCK which will source also SPI will be sourced also by DCO
    // 
    CSCTL0_H = 0xA5;
    CSCTL1 |= DCORSEL + DCOFSEL0 + DCOFSEL1;	 // Set max. DCO setting
    CSCTL2 = SELA_1 + SELS_3 + SELM_3;		// set ACLK - VLO, the rest  = MCLK = DCO
    CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0;		// set all dividers to 0

}

//*****************************************************************************
//
//! \brief  Starts timer that handles switch debouncing
//!
//! \param  none
//!
//! \return none
//!
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
//! \brief  Stops timer that handles switch debouncing
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void StopDebounceTimer()
{  
    TB0CCTL0 &= ~CCIE;                          // TACCR0 interrupt enabled
}

//*****************************************************************************
//
//! Initialize LEDs
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Initializes LED Ports and Pins
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
//! Turn LED On
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
//
//*****************************************************************************
unsigned char getLedStatus()
{
  unsigned char status = 0;
  if(PJOUT & BIT0)
  	status |= (1<<0);
  if(PJOUT & BIT1)
  	status |= (1<<1);
  if(PJOUT & BIT2)
  	status |= (1<<2);
  if(PJOUT & BIT3)
  	status |= (1<<3);
  if(P3OUT & BIT4)
  	status |= (1<<4);
  if(P3OUT & BIT5)
  	status |= (1<<5);
  if(P3OUT & BIT6)
  	status |= (1<<6);
  if(P3OUT & BIT7)
  	status |= (1<<7);

  return status;
}

//*****************************************************************************
//
//! Turn LED On
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
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
//! Turn LED Off
//!
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Turns a specific LED Off
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
//! \param  ledNum is the LED Number
//!
//! \return none
//!
//! \brief  Toggles a board LED
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
//! \brief  check if Smart Config flag was set
//!
//! \param  none
//!
//! \return indication if flag is set
//!
//
//*****************************************************************************
long IsFTCflagSet()
{  

  return (P4OUT&BIT0) ;                                //  check if Smart Config flag is configure

}

//*****************************************************************************
//
//! \brief  set Smart Config flag when S2 was pressed
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void SetFTCflag()
{  
   *ptrFtcAtStartup = SMART_CONFIG_SET;                              //  set Smart Config flag  
}

//*****************************************************************************
//
//! \brief  clear Smart Config flag when S2 was pressed
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void ClearFTCflag()
{  
  *ptrFtcAtStartup = 0xFF;                             //  clear Smart Config flag  
}

  
//*****************************************************************************
//
//! \brief  Dissable S2 switch interrupt
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void DissableSwitch()
{  
            // disable switch interrupt
            
            
            P4IFG &= ~BIT0;                // Clear P4.0 IFG
            P4IE &= ~(BIT0);               // P4.0 interrupt disabled
            P4IFG &= ~BIT0;					// Clear P4.0 IFG
#if 0           
			P4IFG &= ~BIT1;
            P4IE &= ~(BIT1);
			P4IFG &= ~BIT1;
#endif
            
                P4IFG = 0;
}

//*****************************************************************************
//
//! \brief  Restore S2 switch interrupt
//!
//! \param  none
//!
//! \return none
//!
//
//*****************************************************************************
void RestoreSwitch()
{  
          
            
    // Restore Switch Interrupt
    P4IFG &= ~BIT0;                 // Clear P4.0 IFG
    P4IE |= BIT0;                   // P4.0 interrupt enabled
    P4IFG &= ~BIT0;                 // Clear P4.0 IFG
#if 0
    P4IFG &= ~BIT1;
    P4IE |= BIT1;
	P4IFG &= ~BIT1;
#endif
}

 //*****************************************************************************
//
//! \brief  Indication if the switch is still pressed
//!
//! \param  none
//!
//! \return none
//!
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
//! \brief  Restarts the MSP430
//!
//! Restarts the MSP430 completely. One must be careful
//!
//! \return never
//!
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
#pragma vector=PORT1_VECTOR, WDT_VECTOR, TIMER1_A1_VECTOR, ADC10_VECTOR, UNMI_VECTOR,COMP_D_VECTOR,	DMA_VECTOR, PORT3_VECTOR, RTC_VECTOR, TIMER0_B1_VECTOR, TIMER1_B0_VECTOR, TIMER1_B1_VECTOR, TIMER2_B0_VECTOR, TIMER2_B1_VECTOR,SYSNMI_VECTOR, USCI_A1_VECTOR, USCI_B0_VECTOR
__interrupt void Trap_ISR(void)
{
  while(1);
}

#endif


