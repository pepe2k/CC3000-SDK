/*****************************************************************************
*
*  wifi_application_multithread.c - Application to demonstrate multithreading
*                                   capabilities of the host-driver
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

#include <string.h>
#include <stdbool.h>

#include <ti/sysbios/BIOS.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_ssi.h"
#include "inc/hw_gpio.h"
#include "inc/hw_gpio.h"
#include "inc/lm4f120h5qr.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/rom_map.h"

#include "utils/uartstdio.h"

#include "wlan.h"
#include "socket.h"
#include "nvmem.h"
#include "netapp.h"
#include "security.h"

#include "board.h"
#include "spi.h"
#include "spi_version.h"

#include "CC3000_common.h"

#include "host_driver_version.h"
#include "osal.h"

#define APPLICATION_VERSION_NUMBER_1  1    //version number 10
#define APPLICATION_VERSION_NUMBER_2  0

#define SL_VERSION_LENGTH                   (11)
#define PALTFORM_VERSION                    (4)

unsigned char pucUARTExampleAppString[] = {'\f', '\r','E', 'x', 'a', 'm', 'p', 'l', 'e', ' ', 'A', 'p', 'p', ':', 'd', 'r', 'i', 'v', 'e', 'r', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', ' ' };
unsigned char version_string[SL_VERSION_LENGTH];

static unsigned char CC3000ConectionState;

/* These are the CC3000 connection status states */
#define CC3000_INIT_STATE                   1
#define CC3000_NOT_CONNECTED_STATE          2
#define CC3000_CONNECTED_STATE              3
#define CC3000_CONNECTED_PORTS_OPEN_STATE   4

#define MAX_WIFI_PROFILES                   5

/* Various different CC3000 Status values returned when
   attempting to initializeand open sockets. */
enum{
      START_CONNECT = 0,
      WAIT_FOR_CONNECT,
      OPEN_SOCKET,
      UDP_CONNECT,
      UDP_COMMUNICATION
};

/* The Processor Clock frequency in Hz */
#define MCLK                                50000000
#define DHCP_EN                             1

#define DISABLE                             (0)
#define ENABLE                              (1)

unsigned char g_ThisIpAddress[4] = {192, 168, 1, 111};
unsigned char g_ThisMulticastAddress[4] = {224, 0, 0, 1};

#define TICK_RATE_1MS                       (MCLK/1000)

#if (TICK_RATE_1MS > 0x0000FFFF)
    #error "TICK_RATE_1MS > than 16-bit"
#endif

short status = 0;
int socket_state = 0;

unsigned long ulSmartConfigFinished, ulCC3000Connected, ucStopSmartConfig;
unsigned long ulCC3000DHCP, data_count = 0;
unsigned long timeout = 5 ;                         /* 5*10 milliseconds */
unsigned long count;

SemaphoreHandle         g_tx_sem;

// Simple Config Prefix
const char aucCC3000_prefix[] = {'T', 'T', 'T'};
//AES key "smartconfigAES16"
const unsigned char smartconfigkey[] = {0x73,0x6d,0x61,0x72,0x74,0x63,0x6f,0x6e,0x66,0x69,0x67,0x41,0x45,0x53,0x31,0x36};
//device name used by smart config response
char device_name[] = "home_assistant";



//*****************************************************************************
//
// The size of the SPI transmit and receive buffers.  They do not need to be
// the same size.
//
//*****************************************************************************
#define SPI_TXBUF_SIZE         32
#define SPI_RXBUF_SIZE         80

socklen_t tRxPacketLength;
bool flag = false ;

unsigned long  *p_data_count =&data_count;
unsigned        char g_ucRxBufB[SPI_RXBUF_SIZE];

GPIO_Callbacks cb_gpiob_int;
GPIO_Callbacks cb_spi_int;

extern void IntSpiGPIOHandler(void);
extern void SpiIntHandler(void);

/***** Static Function Declarations *****/
static int init_console(void);
static int init_driver(void);
static int init_wlan_device();

static void Delay100ms();
static void wlan_restart(void);
static void setState(int newState);

static void StartSmartConfig(void);

void CC3000_UsynchCallback(long lEventType, char * data, unsigned char length);
char *sendDriverPatch(unsigned long *Length);
char *sendBootLoaderPatch(unsigned long *Length);
char *sendWLFWPatch(unsigned long *Length);

static sInt16 port = 9000;

/* Rx Thread */
void *rx_handler(void);
/* Tx Thread */
void *tx_handler(void);
/* Idle Function */
void idle_fun();

int main(int argc, char **argv)
{
    int ret_val = 0;

    ret_val = init_console(); /**/
    if (ESUCCESS != ret_val) return -1;

    ret_val = init_driver();  /**/
    if (ESUCCESS != ret_val) return -1;

    BIOS_start();
    return 0;
}

TaskHandle h_rx_thread;
s_thread_params rx_thrd_params;
s_thread_params *p_rx_thrd_params = &rx_thrd_params;

TaskHandle h_tx_thread;
s_thread_params tx_thrd_params;
s_thread_params *p_tx_thrd_params = &tx_thrd_params;

/* New task is spawned 'coz HWI registration happens only after BIOS_Start */
int launch_service()
{
    int ret_val = 0;

    ret_val = init_wlan_device(); /**/
    if (ESUCCESS != ret_val) return -1;

    initClk();

    socket_state = START_CONNECT; // not connected socket state
    status = START_CONNECT;       // status not connected
    ulCC3000DHCP = 0;
    CC3000ConectionState = CC3000_NOT_CONNECTED_STATE;

    wlan_disconnect();

#ifndef ENABLE_SMART_CONFIG
    wlan_connect(WLAN_SEC_UNSEC, "TP-LINK-APK", 11, NULL, NULL, 0);
    //Wait until CC3000 is connected
    while (CC3000ConectionState != CC3000_CONNECTED_STATE)
    {
        SysCtlDelay(100);
    }

#else /* ENABLE_SMART_CONFIG */
    StartSmartConfig();
#endif /* ENABLE_SMART_CONFIG */

    if (NULL == g_tx_sem)
        OS_semaphore_create(&g_tx_sem, "Tx_Sem", 0);

    /* Spawn Rx Thread - Receives data from an iperf-client */
    strncpy(p_rx_thrd_params->thread_name, "rx_thread_1", strlen("rx_thread_1"));
    p_rx_thrd_params->priority = 4;       /* Should be at higher priority than 'SelectThread' */
    p_rx_thrd_params->stack_size = 0x200; /* TBD - Optimize */
    p_rx_thrd_params->p_entry_function = &rx_handler;
    *(int *)p_rx_thrd_params->p_func_params = 0;
    p_rx_thrd_params->p_stack_start = NULL; /* TBD - Mandatory in Thread-X */
    if (e_SUCCESS != OS_thread_create(&h_rx_thread, p_rx_thrd_params)){
        /* rx_handler creation FAILED..!! */
        return -1;
    }

#ifdef MULTITHREAD_SUPPORT
    /* Spawn Rx Thread - Receives data from an iperf-client */
    strncpy(p_rx_thrd_params->thread_name, "rx_thread_2", strlen("rx_thread_2"));
    p_rx_thrd_params->priority = 4;       /* Should be at higher priority than 'SelectThread' */
    p_rx_thrd_params->stack_size = 0x200; /* TBD - Optimize */
    p_rx_thrd_params->p_entry_function = &rx_handler;
    *(int *)p_rx_thrd_params->p_func_params = 0;
    p_rx_thrd_params->p_stack_start = NULL; /* TBD - Mandatory in Thread-X */
    if (e_SUCCESS != OS_thread_create(&h_rx_thread, p_rx_thrd_params)){
        /* rx_handler creation FAILED..!! */
        return -1;
    }

    /* Spawn Tx Thread - Sends data to an iperf-server */
    strncpy(p_tx_thrd_params->thread_name, "tx_thread", strlen("tx_thread"));
    p_tx_thrd_params->priority = 4;       /* Should be at higher priority than 'SelectThread' */
    p_tx_thrd_params->stack_size = 0x200; /* TBD - Optimize */
    p_tx_thrd_params->p_entry_function = &tx_handler;
    *(int *)p_tx_thrd_params->p_func_params = 0;
    p_tx_thrd_params->p_stack_start = NULL; /* TBD - Mandatory in Thread-X */
    if (e_SUCCESS != OS_thread_create(&h_tx_thread, p_tx_thrd_params)){
        /* rx_handler creation FAILED..!! */
        return -1;
    }
#endif /* MULTITHREAD_SUPPORT */

    return 0; /* Success */
}

void *rx_handler(void)
{
    sockaddr tSocketAddr;

    int data_received = 0;
    int clientSocket = -1;
    int pkts_rcvd = 0;
    int err = -1;

    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0){
        setState(START_CONNECT);
        status = 0 ;                // status not connected
        ulCC3000DHCP = 0;
        CC3000ConectionState = 2;
        return NULL;
    }

    memset(&tSocketAddr, 0, sizeof(tSocketAddr));
    tSocketAddr.sa_family = AF_INET;

    /* Setting the port number and IP address */
    tSocketAddr.sa_data[0]= (port & 0xFF00) >> 8;
    tSocketAddr.sa_data[1]= (port++ & 0x00FF);          /* For Testing - Port is a static variable */
    tSocketAddr.sa_data[2]= 0x00;
    tSocketAddr.sa_data[3]= 0x00;
    tSocketAddr.sa_data[4]= 0x00;
    tSocketAddr.sa_data[5]= 0x00;

    err = bind(clientSocket, &tSocketAddr, sizeof(sockaddr));
    if  (err == -1 ){
        setState(START_CONNECT);
        status = 0 ;                // status not connected
        ulCC3000DHCP = 0;
        CC3000ConectionState = 2;
        return NULL;
    }

    setState(UDP_COMMUNICATION);
    //err= setsockopt(clientSocket,SOL_SOCKET,SOCKOPT_RECV_TIMEOUT,&timeout,sizeof(timeout));
    //if (ESUCCESS != err) return NULL;

    while(data_received == 0)
    {
        data_received = recvfrom(clientSocket, g_ucRxBufB, sizeof(g_ucRxBufB),\
                                 0, &tSocketAddr, &tRxPacketLength);
        if( data_received > 0)
        {
            turnLedOn(2);
            UARTprintf(" Data Recieved %d\n", data_received);

            System_printf("rx_handler: Received [%d] bytes on socket-id [%d]\n", data_received, clientSocket);
            System_printf("rx_handler: Data [%s] \n", g_ucRxBufB);
            System_flush();

            memset(g_ucRxBufB, 0, sizeof(g_ucRxBufB));
            data_received = 0;

            pkts_rcvd++;
            if (pkts_rcvd > 200){
                pkts_rcvd = 0;
                OS_semaphore_post(g_tx_sem);
            }
        }
    }

    return NULL;
}

#define HOST_NAME "192.168.1.103"
#define BYTE_N(x,n) (((x) >> n*8) & 0x000000FF)

void *tx_handler(void)
{
    sockaddr srvr_addr;
    char data[] = "Received 200 Bytes";

    uInt32 server_ip = 0;
    sInt32 dns_response = -1;
    sInt32 srvr_sckt = -1;
    sInt16 bytes_sent = 0;
    sInt32 ret_val = -1;
    uInt16 pkts_sent = 0;

    srvr_sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (srvr_sckt < 0) return NULL;

    memset(&srvr_addr, 0, sizeof(srvr_addr));

    srvr_addr.sa_family = AF_INET;

    /* Setting the port number and IP address */
    srvr_addr.sa_data[0]= (port & 0xFF00) >> 8;
    srvr_addr.sa_data[1]= (port++ & 0x00FF);

    dns_response = gethostbyname(HOST_NAME, strlen(HOST_NAME), &server_ip);
    if(dns_response > 0){
        srvr_addr.sa_data[2] = BYTE_N(server_ip,3);  /* First Octet of destination IP */
        srvr_addr.sa_data[3] = BYTE_N(server_ip,2);  /* Second Octet of destination IP */
        srvr_addr.sa_data[4] = BYTE_N(server_ip,1);  /* Third Octet of destination IP */
        srvr_addr.sa_data[5] = BYTE_N(server_ip,0);  /* Fourth octet of destination IP */
    }else{
        /* Error - Using some default */
        srvr_addr.sa_data[2] = 192;  /* First Octet of destination IP */
        srvr_addr.sa_data[3] = 168;  /* Second Octet of destination IP */
        srvr_addr.sa_data[4] = 1;    /* Third Octet of destination IP */
        srvr_addr.sa_data[5] = 103;  /* Fourth Octet of destination IP */
    }

    ret_val = connect(srvr_sckt, &srvr_addr, sizeof(srvr_addr));
    if (ESUCCESS != ret_val) return NULL;

    //strncpy(data, "Hello World..!!", strlen("Hello World..!!"));
    while(0 == bytes_sent)
    {
        OS_semaphore_pend(g_tx_sem, e_WAIT_FOREVER);

        while(50 != pkts_sent){
            bytes_sent = sendto(srvr_sckt, data, strlen(data), 0, &srvr_addr, sizeof(srvr_addr));
            if (bytes_sent < 0)
            {
                System_printf("tx_handler: Error[%d] when sending data.\n", bytes_sent);
                break;
            }

            if( bytes_sent > 0)
            {
                turnLedOn(3);

                UARTprintf(" Bytes Sent %d\n", bytes_sent);

                System_printf("tx_handler: Sent [%d] bytes on socket-id [%d]\n", bytes_sent, srvr_sckt);
                System_printf("tx_handler: Data sent is [%s] \n", data);

                bytes_sent = 0;
                pkts_sent++;
                System_flush();
            }

            /* Note: bytes_sent = 0 not handled since 'sendto' is synchronous here */
        }

        pkts_sent = 0;
    }

    return NULL;
}

//*****************************************************************************
//
//! StartSmartConfig
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function triggers a smart configuration process on CC3000.
//!         it exists upon completion of the process
//
//*****************************************************************************

static void StartSmartConfig(void)
{
    ulSmartConfigFinished = 0;
    ulCC3000Connected = 0;
    ulCC3000DHCP = 0;

    // Reset all the previous configuration
    wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
    wlan_ioctl_del_profile(255);

    //Wait until CC3000 is disconnected
    while (CC3000ConectionState == CC3000_CONNECTED_STATE)
    {
        SysCtlDelay(100);
    }

    // Start blinking LED1 during Smart Configuration process
    turnLedOn(1);

    wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
    turnLedOff(1);

    // Start the SmartConfig start process
    wlan_smart_config_start(0);
    turnLedOn(1);

    // Wait for Smart config to finish
    while (ulSmartConfigFinished == 0)
    {
        turnLedOff(1);
        SysCtlDelay(16500000);
        turnLedOn(1);
        SysCtlDelay(16500000);
    }
    turnLedOn(1);

#if 0
    // create new entry for AES encryption key
    nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);

    // write AES key to NVMEM
    aes_write_key((unsigned char *)(&smartconfigkey[0]));

    // Decrypt configuration information and add profile
    wlan_smart_config_process();
#endif //
    // Configure to connect automatically to the AP retrieved in the
    // Smart config process
    wlan_ioctl_set_connection_policy(DISABLE, DISABLE, ENABLE);


    // reset the CC3000
    wlan_stop();

    SysCtlDelay(6000000);
    wlan_start(0);
    SysCtlDelay(6000000);

    // Mask out all non-required events
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|
                                            HCI_EVNT_WLAN_ASYNC_PING_REPORT);

    while (CC3000ConectionState != CC3000_CONNECTED_STATE)
    {
        SysCtlDelay(100);
    }


    // complete smart config process:
    // 1. if smart config is done
    // 2. CC3000 established AP connection
    // 3. DHCP IP is configured
    // then send mDNS packet to stop external SmartConfig application
    if ((ucStopSmartConfig == 1) && (ulCC3000DHCP == 1))
    {
        unsigned char loop_index = 0;

        while (loop_index < 3)
        {
            mdnsAdvertiser(1,device_name,strlen(device_name));
            loop_index++;
        }

        ucStopSmartConfig = 0;
    }
}

static int init_console(void)
{
    /* Enable GPIO port A which is used for UART0 pins.
       TODO: change this to whichever GPIO port you are using. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Enable and configure the peripherals used by the uart. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Configure the pin muxing for UART0 functions on port A0 and A1.
       This step is not necessary if your part does not support pin muxing.
       TODO: change this to select the port pin you are using. */
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    /* Select the alternate (UART) function for these pins.
       TODO: change this to select the port/pin you are using. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Initialize the UART for console I/O.*/
    UARTStdioInit(0);
    return 0;
}

static int init_driver(void)
{
    GPIO_Callbacks *p_cb_gpiob_int = &cb_gpiob_int;
    GPIO_Callbacks *p_cb_spi_int = &cb_spi_int;
    //GPIO_Callbacks *p_cb_gpiob_int = &cb_gpioe_int;

    int index = 0;

    /* Init GPIO's */
    pio_init();

    p_cb_gpiob_int->port = SPI_GPIO_IRQ_BASE;
    p_cb_gpiob_int->intNum = INT_GPIO_SPI;
    for (index = 0; index < 8; index++){ /* TDB - Check - Registration on 8 pins */
        p_cb_gpiob_int->callbackFxn[index] = IntSpiGPIOHandler;
    }
    GPIO_setupCallbacks(p_cb_gpiob_int);

    /* Init SPI */
    init_spi();

    p_cb_spi_int->port = INT_GPIO_SPI;
    p_cb_spi_int->intNum = INT_SPI;
    for (index = 0; index < 8; index++){ /* TDB - Check - Registration on 8 pins */
        p_cb_spi_int->callbackFxn[index] = SpiIntHandler;
    }
    GPIO_setupCallbacks(p_cb_spi_int);

#if 0
    p_cb_spi_int->port = SPI_CS_PORT;
    p_cb_spi_int->intNum = INT_GPIO_SPI;
    p_cb_spi_int->callbackFxn[SPI_CS_PIN] = IntSpiGPIOHandler;
    GPIO_setupCallbacks(p_cb_spi_int);
#endif

    /* Enable processor interrupts */
    //enabledMasterInterrupt();
    return 0;
}

static int init_wlan_device()
{
    /* Initializes WLAN */
    wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch,
              sendBootLoaderPatch, ReadWlanInterruptPin,
              WlanInterruptEnable, WlanInterruptDisable, WriteWlanPin);

    CC3000ConectionState = CC3000_NOT_CONNECTED_STATE;

    /* Start WLAN */
    wlan_start(false);  /* Patch not available */
    turnLedOn(1);
    SysCtlDelay(1000000);

    /* Generate teh event to CLI: send a version string
       DispatcherUartSendPacket(pucUARTExampleAppString, sizeof(pucUARTExampleAppString)); */
    UARTprintf(" %s" , pucUARTExampleAppString);

    version_string[0]= 0x30 + PALTFORM_VERSION;
    version_string[1]= '.';
    version_string[2]= 0x30 + APPLICATION_VERSION_NUMBER_1;
    version_string[3]= 0x30 + APPLICATION_VERSION_NUMBER_2;
    version_string[4]= '.';
    version_string[5] = 0x30 + SPI_VERSION_NUMBER;
    version_string[6]= '.';
    version_string[7]= 0x30 + DRIVER_VERSION_NUMBER;
    version_string[8]= 0x30 + DRIVER_VERSION_NUMBER ;
    version_string[9]=    '\n';
    version_string[10]=    '\r';
    UARTprintf("%s", version_string);

    /* Configure SysTick to occur X times per second, to use as a time
       reference.  Enable SysTick to generate interrupts. */
    InitSysTick();

#ifdef DHCP_EN
    /* Set DHCP */
    unsigned char pucSubnetMask[4], pucIP_Addr[4];
    unsigned char pucIP_DefaultGWAddr[4], pucDNS[4];

    memset(pucSubnetMask,       0, sizeof(pucSubnetMask));
    memset(pucIP_Addr,          0, sizeof(pucIP_Addr));
    memset(pucIP_DefaultGWAddr, 0, sizeof(pucIP_DefaultGWAddr));
    memset(pucDNS,              0, sizeof(pucDNS));

    netapp_dhcp((unsigned long *) pucIP_Addr, (unsigned long *) pucSubnetMask,
                (unsigned long *) pucIP_DefaultGWAddr, (unsigned long *) pucDNS);

    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE     |
                        HCI_EVNT_WLAN_UNSOL_INIT    |
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT);
#else
    /* Set DHCP */
    unsigned char pucSubnetMask[4], pucIP_Addr[4];
    unsigned char pucIP_DefaultGWAddr[4], pucDNS[4];

    memset(pucSubnetMask, 0xFF, sizeof(pucSubnetMask));
    memcpy(pucIP_Addr, g_ThisIpAddress, sizeof(g_ThisIpAddress));

    pucIP_DefaultGWAddr[0] = g_ThisIpAddress[0];
    pucIP_DefaultGWAddr[1] = g_ThisIpAddress[1];
    pucIP_DefaultGWAddr[2] = g_ThisIpAddress[2];
    pucIP_DefaultGWAddr[3] = 1;
    memset(pucDNS, 0, sizeof(pucDNS));
    netapp_dhcp((unsigned long *) pucIP_Addr, (unsigned long *) pucSubnetMask,
                (unsigned long *) pucIP_DefaultGWAddr, (unsigned long *) pucDNS);
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE  |
                        HCI_EVNT_WLAN_UNSOL_INIT |
                        HCI_EVNT_WLAN_UNSOL_DHCP |
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT);
#endif

    /* Mask out all non-required events from CC3000 */
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE |
                        HCI_EVNT_WLAN_UNSOL_INIT|
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT);
    netapp_set_debug_level(0x1ff);
    ucStopSmartConfig = 0;
    return(0);
}

/* Performs a long delay of 100ms based on MCLK */
static void Delay100ms()
{
    SysCtlDelay(100000);
}

static void wlan_restart(void)
{
    int err = -1;

    CC3000ConectionState = CC3000_NOT_CONNECTED_STATE;
    ulCC3000DHCP = 0;

    err = wlan_ioctl_set_connection_policy(0, 0, 0);
    if (ESUCCESS != err) return;

    wlan_stop();

    SysCtlDelay(6000000);

    wlan_start(false);
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE |
                        HCI_EVNT_WLAN_UNSOL_INIT|
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}

static void setState(int newState)
{
    socket_state = newState;
    count = 0;
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
//!          and operates a LED7 to have an on-board indication
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
        CC3000ConectionState = CC3000_CONNECTED_STATE;
        ulCC3000Connected = 1;
    }

    if (lEventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT)
    {
        CC3000ConectionState = CC3000_NOT_CONNECTED_STATE;
        ulCC3000DHCP=0;
        turnLedOff(2);
        turnLedOff(3);
    }

    if(lEventType == HCI_EVNT_WLAN_UNSOL_DHCP)
    {
        ulCC3000DHCP=1;

        /* Get our assigned address from the passed data */
        g_ThisIpAddress[0] = data[3];
        g_ThisIpAddress[1] = data[2];
        g_ThisIpAddress[2] = data[1];
        g_ThisIpAddress[3] = data[0];
    }
}

char *sendDriverPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}

char *sendBootLoaderPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}

char *sendWLFWPatch(unsigned long *Length)
{
    *Length = 0;
    return NULL;
}

void idle_fun()
{
    System_printf("Idle function running \n", g_ucRxBufB);
    System_flush();
}
