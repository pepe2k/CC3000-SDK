/*****************************************************************************
*
*  HttpCore.c
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
#include "Common.h"
#include "HttpHeaders.h"
#include "HttpCore.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpAuth.h"
#include "HttpDebug.h"
#include "HttpDynamic.h"
#include "HttpStatic.h"
#include <string.h>
#include "HTTPConfig.h"

// Include CC3000 bridged networking stack headers
#include "cc3000_common.h"
#include "socket.h"
#include "netapp.h"
#include "FlashDB.h"

/** 
 * @addtogroup HttpCore
 * @{
 */


/**
 * This enumeration defines all possible states for a connection
 */
enum HttpConnectionState
{
    /// Connection is inactive. The connection's socket should be INVALID_SOCKET_HANDLE
    Inactive,
    /// Waiting for packet(s) with the request method
    RequestMethod,
    /// Waiting for packet(s) with request headers
    RequestHeaders,
    /// Currently receiving a header which is too long - Drop it and continue waiting for more headers
    DropCurrentHeader,
    /// Done parsing headers, waiting for POST data pakcets
    RequestData,
    /// Request is at the hands of the content module (static and/or dynamic), waiting for response headers.
    Processing,
    /// Response headers have been sent. Sending response content.
    ResponseData,
    /// Response complete. Possibility of another request.
    ResponseComplete
};

/**
 * This enumeration defines all possible request-handler modules
 */
enum HttpHandler
{
    /// No module is going to process this request (use the default 404 error)
    None,
    /// The HttpStatic module is going to process this request
    HttpStatic,
    /// The HttpDynamic module is going to process this request
    HttpDynamic
};

/**
 * This structure holds all information for an HTTP connection
 */
#ifdef __CCS__
struct __attribute__ ((__packed__)) HttpConnectionData
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct HttpConnectionData
#endif
{
    /// The state of this connection
    enum HttpConnectionState connectionState;
    /// The data socket for this connection. Should be INVALID_SOCKET_HANDLE when in Inactive state
    SOCKET dataSocket;
    /// The current HTTP request on this connection
    struct HttpRequest request;
    /// Which handler is going to process the current request
    enum HttpHandler handler;
    /// An un-parsed chunk of header line
    uint8 headerStart[HTTP_CORE_MAX_HEADER_LINE_LENGTH];
    /// Amount of content left to be read in the request or sent in the response
    uint32 uContentLeft;
    /// If the headers will arrive in several packets the content will be buffered in the headers start buffer until a whole line is available
    uint16 uSavedBufferSize;
    /// Check weather the authentication is done or not
    uint8 HttpAuth;
};

/**
 * This structure holds all of the global state of the HTTP server
 */
#ifdef __CCS__
struct __attribute__ ((__packed__)) HttpGlobalState
#elif __IAR_SYSTEMS_ICC__
#pragma pack(1)
struct HttpGlobalState
#endif
{
    /// The listening socket
    SOCKET listenSocket;
    /// Number of active connections
    uint16 uOpenConnections;
    /// All possible connections
    struct HttpConnectionData connections[HTTP_CORE_MAX_CONNECTIONS];
    /// The packet-receive buffer
    uint8 packetRecv[HTTP_CORE_MAX_PACKET_SIZE_RECEIVED];
    /// Size of data in the packet-receive buffer
    int packetRecvSize;
    /// The packet-send buffer
    uint8 packetSend[HTTP_CORE_MAX_PACKET_SIZE_SEND];
    /// Size of data in the packet-send buffer
    uint16 packetSendSize;
};

/// The global state of the HTTP server
__no_init struct HttpGlobalState g_state;

// Forward declarations for static functions
static void HttpCore_InitWebServer();
static void HttpCore_CloseConnection(uint16 uConnection);
static int HttpCore_HandleRequestPacket(uint16 uConnection, struct HttpBlob packet);
static int HttpCore_HandleMethodLine(uint16 uConnection, struct HttpBlob line);
static int HttpCore_HandleHeaderLine(uint16 uConnection, struct HttpBlob line);
static int HttpCore_HandleRequestData(uint16 uConnection, struct HttpBlob* pData);
static void HttpCore_ProcessNotFound(uint16 uConnection);

struct HttpBlob nullBlob = {NULL, 0};

extern volatile char runSmartConfig;
extern char DevServname[];
extern unsigned char mDNSSend;
extern char CheckSocket;
extern signed char sd[];

#define MAX_SENT_DATA 912

/**
 * Detect all error conditions from a sockets API return value, such as accpet()
 * Note: This is different in Windows and CC3000
 * Returns nonzero if valid socket, or zero if invalid socket
 */
static int HttpIsValidSocket(SOCKET sock)
{
#if (defined(WIN32) && !defined(HTTP_WEB_SERVER_ON_BRIDGE))
    // The CC3000 API returns an int, and might return CC3000_INVALID_SOCKET
    if ((sock == INVALID_SOCKET) ||
        (sock == CC3000_INVALID_SOCKET))
        return 0;
#else 
    if (sock < 0)
        return 0;
#endif
    return 1;
}

void HttpCloseServer()
{
	closesocket(g_state.listenSocket);
}

void HttpServerInitAndRun()
{
    uint16 uConnection;
    sockaddr_in addr;
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    struct HttpBlob blob;
    fd_set readsds, errorsds;
    int ret = 0;
    SOCKET maxFD;
    timeval timeout;
    signed char curSocket;
    int optval, optlen;

    memset(&timeout, 0, sizeof(timeval));
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // Initialize the server's global state
    HttpCore_InitWebServer();

    // Create the listener socket for HTTP Server.
    g_state.listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(g_state.listenSocket == INVALID_SOCKET)
    {
        HttpDebug("Failed to create listen socket");
        return;
    }

    memset(&addr, 0, sizeof(addr));
#ifdef _WINSOCKAPI_
    addr.sin_family = AF_INET;
#else
    addr.sin_family = htons(AF_INET);
#endif
    addr.sin_port = htons(HTTP_CORE_SERVER_PORT);
    if (bind(g_state.listenSocket, (const sockaddr*)&addr, sizeof(addr)) != 0)
    {
        HttpDebug("bind() fail");
        return;
    }
    if (listen(g_state.listenSocket, 10) != 0)
    {
        HttpDebug("listen() fail");
        return;
    }
    
#ifdef HTTP_CORE_ENABLE_AUTH
    struct HttpBlob user,pass;
    user.pData = "cc3000";
    user.uLength= 6;
    pass.pData = "admin";
    pass.uLength = 5;
    int count;
    
    HttpAuth_Init(user, pass);    
#endif
    
    g_state.uOpenConnections = 0;

    // Main loop of the server. Note: This is an infinite loop
    while (1)
    {
    	// Check if button is pressed for smart config
        if(runSmartConfig == 1)
        {
          // Button is pressed for smart config. Close all active connections
          for(uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection)
          {
            if (g_state.connections[uConnection].connectionState != Inactive)
            {
              HttpCore_CloseConnection(uConnection);
            }
          }

          // Close listening socket
          HttpCloseServer();
          break;
        }
        if( mDNSSend ==1)
        {
          mdnsAdvertiser(1,DevServname, strlen(DevServname));
          mDNSSend =0;
        }
        
        // Client socket is closed, close socket
        if(CheckSocket == 1)
        {
          for(count = 0; count<9 ; count++)
          {
            if(sd[count] == 1)
            {
              HttpCore_CloseConnection(count);
              sd[count] = 0;
            }
          }
          for(uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection)
          {
            if (g_state.connections[uConnection].connectionState != Inactive)
            {
            // Check for socket timeouts
              curSocket =  getsockopt(g_state.connections[uConnection].dataSocket, SOL_SOCKET, SOCKOPT_RECV_NONBLOCK  , &optval, (socklen_t*)&optlen);
              if (curSocket == -57)
              {
                HttpCore_CloseConnection(uConnection);
              }
            }
          }
          CheckSocket = 0;
        }
        
        SOCKET newsock;

        newsock = accept(g_state.listenSocket, (sockaddr*)&clientAddr, &addrLen);

        // If new connection is returned - initialize the new connection object
        if (HttpIsValidSocket(newsock))
        {
          if (g_state.uOpenConnections >= HTTP_CORE_MAX_CONNECTIONS)
          {
            //check if sockets are are available
            for(uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection)
            {
              if (g_state.connections[uConnection].connectionState != Inactive)
              {
                //Check for Socket Timeout
            	curSocket =  getsockopt(g_state.connections[uConnection].dataSocket, SOL_SOCKET, SOCKOPT_RECV_NONBLOCK  , &optval, (socklen_t*)&optlen);
                if (curSocket == -57)
                {
                  HttpCore_CloseConnection(uConnection);
                }
              }
            }
          }
          
          if(g_state.uOpenConnections < HTTP_CORE_MAX_CONNECTIONS)
          {
            for (uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection)
              if (g_state.connections[uConnection].connectionState == Inactive)
                break;

            g_state.connections[uConnection].dataSocket = newsock;
            g_state.uOpenConnections++;
            g_state.connections[uConnection].connectionState = RequestMethod;
            HttpDebug("Accepting new connection number %d", uConnection);
          }
        }
        

        
        
        // Nothing more to do if no open connections
        if (g_state.uOpenConnections == 0)
            continue;

        // Receive data on all open connections, and handle the new data
        maxFD = (SOCKET)-1;
        // Select only the active connections
        FD_ZERO(&readsds);
        FD_ZERO(&errorsds);
        for (uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection) 
        {
            // if connection is open and in one of the receiving states, then add this socket to the set
            if (g_state.connections[uConnection].connectionState == RequestMethod ||
                g_state.connections[uConnection].connectionState == RequestHeaders ||
                g_state.connections[uConnection].connectionState == RequestData ||
                g_state.connections[uConnection].connectionState == DropCurrentHeader)
            {
                FD_SET(g_state.connections[uConnection].dataSocket, &readsds);
                FD_SET(g_state.connections[uConnection].dataSocket, &errorsds);
                // Calculate the maximum socket number
                if (maxFD <= g_state.connections[uConnection].dataSocket)
                    maxFD = g_state.connections[uConnection].dataSocket + 1;
            }
        }
        
        ret = select(maxFD, &readsds, NULL, &errorsds, &timeout);

        // Nothing more to do if no packet received
        if (ret == 0)
        {
          continue;
        }

        for (uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection)
        {
            // Skip inactive connections
            if (g_state.connections[uConnection].connectionState == Inactive)
                continue;

            // Close connections that were select()ed for error
            if (FD_ISSET(g_state.connections[uConnection].dataSocket, &errorsds))
            {
                HttpCore_CloseConnection(uConnection);
                continue;
            }

            // Skip connections that were not select()ed for reading
            if (!FD_ISSET(g_state.connections[uConnection].dataSocket, &readsds))
                continue;

            // This connection has data that can be received now

            // Receive the data into the global packet-receive buffer
            memset(g_state.packetRecv, 0, HTTP_CORE_MAX_PACKET_SIZE_RECEIVED);
            g_state.packetRecvSize = recv(g_state.connections[uConnection].dataSocket, (char *)(g_state.packetRecv), HTTP_CORE_MAX_PACKET_SIZE_RECEIVED, 0);

            // Detect and handle errors
            if (g_state.packetRecvSize <= 0)
            {
                HttpDebug("HTTP Connection recv error. connection=%d, error = %d", uConnection, g_state.packetRecvSize);
                HttpCore_CloseConnection(uConnection);
                continue;
            }

            blob.uLength = (uint16)g_state.packetRecvSize;
            blob.pData = g_state.packetRecv;
            if (!HttpCore_HandleRequestPacket(uConnection, blob))
                HttpCore_CloseConnection(uConnection);
        }
    }
}

/**
 * Reset open connection after finishing HTPP transaction
 */
static void HttpCore_ResetConnection(uint16 uConnection)
{
        g_state.connections[uConnection].uContentLeft = 0;
        g_state.connections[uConnection].uSavedBufferSize = 0;
        g_state.connections[uConnection].handler = None;
        g_state.connections[uConnection].request.requestContent.pData = NULL;
        g_state.connections[uConnection].request.requestContent.uLength = 0;
        g_state.connections[uConnection].request.uFlags = 0;
}

/**
 * Initialize the server's global state structure
 */
static void HttpCore_InitWebServer()
{
    uint16 uConnection;
    g_state.packetRecvSize = 0;
    g_state.packetSendSize = 0;
    g_state.uOpenConnections = 0;
    g_state.listenSocket = INVALID_SOCKET;

    for (uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS; ++uConnection)
    {
        g_state.connections[uConnection].connectionState = Inactive;
        g_state.connections[uConnection].dataSocket = INVALID_SOCKET;
        g_state.connections[uConnection].request.uConnection = uConnection;
        g_state.connections[uConnection].HttpAuth = 0;
        HttpCore_ResetConnection(uConnection);
    }

    FlashDB_Init();
}


/**
 * Close a connection and clean up its state
 */
static void HttpCore_CloseConnection(uint16 uConnection)
{
    HttpDebug("Close connection");

    closesocket(g_state.connections[uConnection].dataSocket);

    g_state.connections[uConnection].connectionState = Inactive;
    g_state.connections[uConnection].dataSocket = INVALID_SOCKET;
    g_state.connections[uConnection].HttpAuth = 0;
    HttpCore_ResetConnection(uConnection);
    if(g_state.uOpenConnections > 0)
      g_state.uOpenConnections--;
}

/**
 * Getting the next line in the HTTP headers section
 * This function is called to get the header lines one by one until an empty line is encountered which means the end of the header section
 * The input is the connection and the remaining blob of the received packet
 *
 * @return zero if the whole packet was handled, and need to wait for more data (pLine is not set to anything yet)
 *         negative if some error occurred, and the connection should be closed.
 *         positive if successful. In this case pCurrentLocation is advanced to skip the line and pLine returns the next line, or NULL and 0 if it should be discarded
 */
static int HttpCore_GetNextLine(uint16 uConnection, struct HttpBlob* pCurrentLocation, struct HttpBlob* pLine)
{
    uint16 uLength;
    uint8* nextLocation;
    // Keep a pointer to the connection state object
    struct HttpConnectionData* pConnection = &g_state.connections[uConnection];

    // search for the line delimiter in the received data
    nextLocation = HttpString_nextToken(HTTP_HEADER_DELIMITER, sizeof(HTTP_HEADER_DELIMITER)-1, *pCurrentLocation);
    uLength = (uint16)(nextLocation - pCurrentLocation->pData);

    if (pConnection->uSavedBufferSize > 0)
    {
        // There is previous saved data for this line, so need to concatenate
        if ((pConnection->headerStart[pConnection->uSavedBufferSize - 1] == '\r') && (pCurrentLocation->pData[0] == '\n'))
        {
            // Handle special case where the headers were splitted exactly at the delimiter
            pConnection->headerStart[pConnection->uSavedBufferSize + 1] = pCurrentLocation->pData[0];
            pLine->pData = pConnection->headerStart;
            // Account for the excessive \r in the buffer
            pLine->uLength = pConnection->uSavedBufferSize - 1; 
            pConnection->uSavedBufferSize = 0;
            // Advance the current location past this line
            pCurrentLocation->pData += 1;
            pCurrentLocation->uLength -= 1;
            return 1;
        }
        else
        {
            // There is saved data, and the delimiter is not split between packets
            if (nextLocation == NULL)
            {
                // No line delimiter was found in the current packet
                if ((pConnection->uSavedBufferSize + pCurrentLocation->uLength) < HTTP_CORE_MAX_HEADER_LINE_LENGTH)
                {
                    // There is enough space to append remaining header into saved buffer
                    memcpy(pConnection->headerStart + pConnection->uSavedBufferSize, pCurrentLocation->pData, pCurrentLocation->uLength);
                    pConnection->uSavedBufferSize += pCurrentLocation->uLength;
                    return 0;
                }
                else
                // There is not enough space in the saved buffer. This header line will be discarded
                if (pConnection->connectionState == RequestMethod)
                {
                    // Connection awaits to receive the the HTTP method line
                    // The method line cannot be discarded, drop the connection
                    return -1;
                }
                else
                {
                    // Connection awaits to receive the next header line which is not the method
                    // Clean saved buffer, and drop this header line
                    pConnection->uSavedBufferSize = 0;
                    pConnection->connectionState = DropCurrentHeader;
                    return 0;
                }
            }
            else
            {
                // Header line delimiter was found in the current packet
                if ((pConnection->uSavedBufferSize + uLength) < HTTP_CORE_MAX_HEADER_LINE_LENGTH)
                {
                    // This header length is of legal size
                    // Concatenate data from the saved buffer and the current packet
                    memcpy(pConnection->headerStart + pConnection->uSavedBufferSize, pCurrentLocation->pData, uLength);
                    pConnection->uSavedBufferSize += uLength;
                    pLine->pData = pConnection->headerStart;
                    pLine->uLength = pConnection->uSavedBufferSize;
                    pConnection->uSavedBufferSize = 0;
                }
                else
                {
                    // There is not enough space in the saved buffer. This header line will be discarded
                    if (pConnection->connectionState == RequestMethod)
                    {
                        // Connection awaits to receive the the HTTP method line
                        // The method line cannot be discarded, drop the connection
                        return -1;
                    }
                    // Return an epmty line since the header is too long
                    pLine->pData = NULL;
                    pLine->uLength = 0;
                }
                // Advance the current location past this line
                pCurrentLocation->pData += uLength + sizeof(HTTP_HEADER_DELIMITER)-1;
                pCurrentLocation->uLength -= uLength + sizeof(HTTP_HEADER_DELIMITER)-1;
                return 1;

            }
        }
    }
    else
    {
        // There is no previously saved data for this line
        if (nextLocation == NULL)
        {
            // No line delimiter was found in the current packet
            if (pCurrentLocation->uLength < HTTP_CORE_MAX_HEADER_LINE_LENGTH)
            {
                // There is enough space to append remaining header into saved buffer
                memcpy(pConnection->headerStart, pCurrentLocation->pData, pCurrentLocation->uLength);
                pConnection->uSavedBufferSize = pCurrentLocation->uLength;
                return 0;
            }
            else
            // There is not enough space in the saved buffer
            // This header line will be discarded
            if (pConnection->connectionState == RequestMethod)
            {
                // Connection awaits to receive the the HTTP method line
                // The method line cannot be discarded, drop the connection
               return -1;
            }
            else
            {
                // Connection awaits to receive the next header line which is not the method
                // Clean saved buffer, and drop this header line
                pConnection->uSavedBufferSize = 0;
                pConnection->connectionState = DropCurrentHeader;
                return 0;
            }
        }
        else
        {
            // Header line delimiter was found in the current packet
            if (uLength < HTTP_CORE_MAX_HEADER_LINE_LENGTH)
            {
                // This header length is of legal size
                // The whole line is in the packet buffer
                pLine->pData = pCurrentLocation->pData;
                pLine->uLength = uLength;
            }
            else
            {
                // There is not enough space to append remaining header into saved buffer
                if (pConnection->connectionState == RequestMethod)
                {
                    // Connection awaits to receive the HTTP method line
                    // The method line cannot be discarded, drop the connection
                    return -1;
                }
                // Return an epmty line since the header is too long
                pLine->pData = NULL;
                pLine->uLength = 0;
                pConnection->connectionState = DropCurrentHeader;
            }
            // Advance the current location past this line
            pCurrentLocation->pData += uLength + sizeof(HTTP_HEADER_DELIMITER)-1;
            pCurrentLocation->uLength -= uLength + sizeof(HTTP_HEADER_DELIMITER)-1;
            return 1;
        }
    }
}


/**
 * The main state machine to handle an HTTP transaction
 * Every received data packet for a connection is passed to this function for parsing and handling
 *
 * If there is an error the connection will be closed in the end of the transaction
 * It will also be closed if "connection: close" header is received or HTTP version is 1.0
 *
 * @return zero to close the connection
 *         nonzero if packet was consumed successfully, and the connection can handle more data
 */
static int HttpCore_HandleRequestPacket(uint16 uConnection, struct HttpBlob packet)
{
    struct HttpBlob currentLocation, line;
    int ret;

    currentLocation.pData = packet.pData;
    currentLocation.uLength = packet.uLength;

    //  when no more data is left and the HTTP transaction is not complete then return to wait for more data
    while (1)
    {
        if (g_state.connections[uConnection].connectionState == RequestMethod || 
            g_state.connections[uConnection].connectionState == RequestHeaders ||
            g_state.connections[uConnection].connectionState == DropCurrentHeader)
        {
            // Parsing HTTP headers
            int result;

            // The received blob is empty, return to wait for more data
            if (currentLocation.uLength < 1)
            {
                return 1;
            }
                
            // Get next header line if available
            result = HttpCore_GetNextLine(uConnection, &currentLocation, &line);

            // Method line is too long, or some other error
            if (result < 0)
                return 0;

            // Whole packet was consumed, and no line-break found. Wait for more data
            if (result == 0)
                return 1;
            
            // Otherwise a new and legal header line is found
        }

        switch (g_state.connections[uConnection].connectionState)
        {
            case DropCurrentHeader:
                g_state.connections[uConnection].connectionState = RequestHeaders;
                break;
            case RequestMethod:
                HttpAssert((line.pData != NULL) && (line.uLength > 0));
                // If there is an error, then return error to drop the connection
                if (!HttpCore_HandleMethodLine(uConnection, line))
                    return 0;
                break;
            case RequestHeaders:
                if (!HttpCore_HandleHeaderLine(uConnection, line))
                    return 0;
                break;
            case RequestData:
                ret = HttpCore_HandleRequestData(uConnection, &currentLocation);
                if (ret == 0)
                    return 1;
                else
                    if (ret < 0)
                        return 0;
                break;
            case Processing:
                // All the request data was received - start final processing of the headers and post data if exists
                switch (g_state.connections[uConnection].handler)
                {
#ifdef HTTP_CORE_ENABLE_STATIC
                    case HttpStatic:
                        HttpStatic_ProcessRequest(&g_state.connections[uConnection].request);
                        break;
#endif
#ifdef HTTP_CORE_ENABLE_DYNAMIC
                    case HttpDynamic:
                        HttpDynamic_ProcessRequest(&g_state.connections[uConnection].request);
                        break;
#endif
                    default:
                        HttpCore_ProcessNotFound(uConnection);
                        break;
                }
                break;
            case ResponseData:
                // This state should never be reached, it is set internally during the processing
                HttpDebug("Response data state in request handling main loop!");
                HttpAssert(0);
                break;
            case ResponseComplete:
                if ((g_state.connections[uConnection].request.uFlags & HTTP_REQUEST_FLAG_CLOSE)!=0 ||
                    (g_state.connections[uConnection].request.uFlags & HTTP_REQUEST_1_1) == 0)
                {
                    // Connection should be closed - either "Connection: close" was received or the HTTP version is 1.0
                    // Return 0 to close the connection
                    return 0;
                }
                // The current HTTP transaction is complete - reset state for the next transaction on this connection
                g_state.connections[uConnection].connectionState = RequestMethod;
                HttpCore_ResetConnection(uConnection);
                break;
            default:
                HttpDebug("Bad state in HttpCore!");
                HttpAssert(0);
                break;
        }
    }
}

/**
 * This function handles connection initial state
 * Parse the first header line as a method header
 * 
 * Method line should be in the form:
 *     GET /resource.html HTTP/1.1\r\n
 *
 * @return nonzero if success
 */
static int HttpCore_HandleMethodLine(uint16 uConnection, struct HttpBlob line)
{
    struct HttpBlob resource;
    uint8* methodLocation;
    uint8* versionLocation;
    uint16 uMethodLength;
    // Search for GET token in the input blob
    methodLocation = HttpString_nextToken(HTTP_METHOD_GET, sizeof(HTTP_METHOD_GET)-1, line);
    uMethodLength = sizeof(HTTP_METHOD_GET)-1;
    if (methodLocation == NULL)
    {
        // The method is not GET
        // Search for the POST token in the input blob
        methodLocation = HttpString_nextToken(HTTP_METHOD_POST, sizeof(HTTP_METHOD_POST)-1, line);
        uMethodLength = sizeof(HTTP_METHOD_POST)-1;
        g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_FLAG_METHOD_POST;
    }
    else
    {
        // Method is GET
        g_state.connections[uConnection].request.requestContent.uLength = 0;
        g_state.connections[uConnection].request.uFlags &= ~HTTP_REQUEST_FLAG_METHOD_POST;
    }
    if (methodLocation != line.pData)
    {
        methodLocation = HttpString_nextToken(HTTP_METHOD_POST, sizeof(HTTP_METHOD_POST)-1, line);
        uMethodLength = sizeof(HTTP_METHOD_POST)-1;
        g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_FLAG_METHOD_POST;
        if(methodLocation == NULL || methodLocation != line.pData)
        {
          // Header does not begin line with GET or POST as it should
          HttpDebug("Unsupported method");
          return 0;
        }
    }

    // Search for "HTTP/1.1" version token
    versionLocation = HttpString_nextToken(HTTP_VERSION_1P1, sizeof(HTTP_VERSION_1P1)-1, line);
    // Version is 1.1
    if (versionLocation != NULL)
        g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_1_1;
    else 
    {
        // Search for "HTTP/1.1" version token
        versionLocation = HttpString_nextToken(HTTP_VERSION_1P0, sizeof(HTTP_VERSION_1P0)-1, line);
        // Version is 1.0
        if (versionLocation != NULL)
            g_state.connections[uConnection].request.uFlags &= ~HTTP_REQUEST_1_1;
        else
        {
            HttpDebug("Bad protocol version");
            return 0;
        }
    }

    HttpDebug("method Header: %.*s", line.uLength, line.pData);
    // Find the URL part of the header 
    resource.pData = methodLocation + uMethodLength + 1;
    resource.uLength = (uint16)(versionLocation - (methodLocation +  uMethodLength + 1) - 1);

    // Determine which handler is supposed to handle this request
    // The handler functions are called according to a hardcoded priority - dynamic, static, default
    // The first handler that returns non zero will handle this request
#ifdef HTTP_CORE_ENABLE_DYNAMIC
    if (HttpDynamic_InitRequest(uConnection, resource) != 0)
        g_state.connections[uConnection].handler = HttpDynamic;
    else
#endif
#ifdef HTTP_CORE_ENABLE_STATIC
    if (HttpStatic_InitRequest(uConnection, resource) != 0)
        g_state.connections[uConnection].handler = HttpStatic;
    else
#endif
        g_state.connections[uConnection].handler = None;
        g_state.connections[uConnection].connectionState = RequestHeaders;
    return 1;
}

/**
 * Handle The HTTP headers (after method) one by one
 * If an empty header is received then the headers section is complete
 * Searches for the headers tokens. If important data is found then it is saved in the connection object
 * 
 * returns nonzero if sucessful
 */
static int HttpCore_HandleHeaderLine(uint16 uConnection, struct HttpBlob line)
{
    struct HttpBlob blobValue;
    uint8* pFound;
    uint32 length;

    // NULL line is received when a header line is too long. 
    if (line.pData == NULL)
        return 1;

    // Length is 0, this means than End-Of-Headers marker was reached
    // State of this connection is set to RequestData
    if (line.uLength == 0)
    {
        g_state.connections[uConnection].connectionState = RequestData;
        return 1;
    }

    // If "Accept-encoding" header then set or clear HTTP_REQUEST_FLAG_ACCEPT_GZIP flag
    if (HttpString_nextToken(HTTP_ACCEPT_ENCODING, sizeof(HTTP_ACCEPT_ENCODING)-1, line))
    {
        line.pData += sizeof(HTTP_ACCEPT_ENCODING)-1 + 2;
        line.uLength -= sizeof(HTTP_ACCEPT_ENCODING)-1 + 2;
        pFound = HttpString_nextToken(HTTP_GZIP, sizeof(HTTP_GZIP)-1, line);
        if (pFound != NULL)
            g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_FLAG_ACCEPT_GZIP;
        else
            g_state.connections[uConnection].request.uFlags &= ~HTTP_REQUEST_FLAG_ACCEPT_GZIP;
        return 1;
    }

    // If "Content-Length" header then parse the lenght and set uContentLeft to it
    // GET and POST method behave the same
    if (HttpString_nextToken(HTTP_CONTENT_LENGTH, sizeof(HTTP_CONTENT_LENGTH)-1, line) == line.pData)
    {
        line.pData += sizeof(HTTP_CONTENT_LENGTH)-1 + 2;
        line.uLength -= sizeof(HTTP_CONTENT_LENGTH)-1 + 2;
        blobValue.pData = line.pData;
        blobValue.uLength = line.uLength - 2;
        length = HttpString_atou(blobValue);
        g_state.connections[uConnection].uContentLeft = length;
        // Set ignore flag
        if (g_state.connections[uConnection].uContentLeft > HTTP_CORE_MAX_HEADER_LINE_LENGTH)
            g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_CONTENT_IGNORED;
        // Prepare the request blob to buffer the content
        g_state.connections[uConnection].request.requestContent.pData = g_state.connections[uConnection].headerStart;
        g_state.connections[uConnection].request.requestContent.uLength = 0;
        return 1;
    }
    // If "Connection" header then look for "close" and set or clear HTTP_REQUEST_FLAG_CLOSE flag
    // The default behaviour for keep alive or no such header is to keep the connection open in http version 1.1
    // In http version 1.0 the default behavior is to always close the socket
    if (HttpString_nextToken(HTTP_CONNECTION_CLOSE, sizeof(HTTP_CONNECTION_CLOSE)-1, line) == line.pData)
    {
        line.pData += sizeof(HTTP_CONNECTION_CLOSE)-1 + 2;
        line.uLength -= sizeof(HTTP_CONNECTION_CLOSE)-1 + 2;
        pFound = HttpString_nextToken(HTTP_CLOSE, sizeof(HTTP_CLOSE)-1, line);
        if (pFound != 0)
            g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_FLAG_CLOSE;
        else
            g_state.connections[uConnection].request.uFlags &= ~HTTP_REQUEST_FLAG_CLOSE;
        return 1;
    }
    // If "Authorization" header the  handle authentication
    if (HttpString_nextToken(HTTP_AUTHORIZATION, sizeof(HTTP_AUTHORIZATION)-1, line) == line.pData)
    {
        line.pData += sizeof(HTTP_AUTHORIZATION)-1 + 2;
        line.uLength -= sizeof(HTTP_AUTHORIZATION)-1 + 2;
        blobValue.pData = line.pData;
        blobValue.uLength = line.uLength;
        //TODO: handle the case when we don't support authentication
#ifdef HTTP_CORE_ENABLE_AUTH
        HttpAuth_RequestAuthenticate(&g_state.connections[uConnection].request, blobValue);
#endif
        return 1;
    }
    // None of the above mentioned headers was recognized so just ignore this header
    return 1;
}

/**
 * Handles request data for this transaction
 * Behaves the same for POST and GET methods -
 * If content length header was present then read the content for further processing
 * If the content is too long then ignore it
 *
 * @return 1 if successful, pData is updated to skip the handled data
           0 if all data is consumed and need to read more data
 *         negative if an error occurs and the connection should be closed.
 */
static int HttpCore_HandleRequestData(uint16 uConnection, struct HttpBlob* pData)
{
    uint32 uLengthToHandle;

    if (g_state.connections[uConnection].uContentLeft == 0)
    {
        HttpDebug("Received content. Length=%d, content:\r\n%.*s", g_state.connections[uConnection].request.requestContent.uLength, g_state.connections[uConnection].request.requestContent.uLength, g_state.connections[uConnection].headerStart);
        g_state.connections[uConnection].connectionState = Processing;
        return 1;
    }

    // Find minimum between the content left to handle and the current blob
    uLengthToHandle = g_state.connections[uConnection].uContentLeft;
    if (uLengthToHandle > pData->uLength)
        uLengthToHandle = pData->uLength;

    // If no new data is received - return and wait for more
    if (uLengthToHandle == 0)
        return 0;

    if ( (g_state.connections[uConnection].request.uFlags & HTTP_REQUEST_CONTENT_IGNORED) != 0)
    {
        // Ignore Content
        pData->pData += uLengthToHandle;
        pData->uLength -= (uint16)uLengthToHandle;
        g_state.connections[uConnection].uContentLeft -= uLengthToHandle;
    }
    else
    {
        // Read content
        memcpy(g_state.connections[uConnection].headerStart + g_state.connections[uConnection].request.requestContent.uLength, pData->pData, uLengthToHandle);
        pData->pData += uLengthToHandle;
        pData->uLength -= (uint16)uLengthToHandle;
        g_state.connections[uConnection].uContentLeft -= uLengthToHandle;
        g_state.connections[uConnection].request.requestContent.uLength += (uint16)uLengthToHandle;
    }

    return 1;
}

/**
 * Returns HTTP 404 not found response
 */
static void HttpCore_ProcessNotFound(uint16 uConnection)
{
    // call HttpResponse_CannedError with 404 NOT_FOUND
    HttpResponse_CannedError(uConnection, HTTP_STATUS_ERROR_NOT_FOUND);
}

/**
 * Sends the input blob over the connection socket
 */
static int HttpCore_SendPacket(uint16 uConnection, struct HttpBlob buffer)
{
    //  Send the buffer over the data socket until all the buffer is sent
    while (buffer.uLength > 0)
    {
        int sent;
        if(buffer.uLength > MAX_SENT_DATA)
        {
          sent = send(g_state.connections[uConnection].dataSocket, (char*)buffer.pData, MAX_SENT_DATA, 0);
          if (sent <= 0)
          {
            // Connection must be closed if send has failed
            return 0;
          }
        }
        else
        { 
          sent = send(g_state.connections[uConnection].dataSocket, (char*)buffer.pData, buffer.uLength, 0);
          if (sent <= 0)
          {
            // Connection must be closed if send has failed
            return 0;
          }
        }
        buffer.pData += sent;
        buffer.uLength -= (uint16)sent;
    }
    return 1;
}

/**
 * Add char to the response buffer
 */
static void HttpResponse_AddCharToResponseHeaders(char ch)
{
    //add char
    g_state.packetSend[g_state.packetSendSize] = ch;
    g_state.packetSendSize++;
}

/**
 * Add uint32 number to the response buffer
 */
static void HttpResponse_AddNumberToResponseHeaders(uint32 num)
{
    struct HttpBlob resource;
    resource.pData = g_state.packetSend + g_state.packetSendSize;
    resource.uLength = 0;
    HttpString_utoa(num, &resource);
    g_state.packetSendSize += resource.uLength;
}


/**
 * Add a string to the response buffer
 */
static void HttpResponse_AddStringToResponseHeaders(char * str, uint16 len)
{
    memcpy(g_state.packetSend + g_state.packetSendSize, str, len);
    g_state.packetSendSize += len;
}

/**
 *  Add header line to response buffer
 *  Adds a line to the header with the provided name value pair
 *  Precondition to this function is that g_state.packetSendSize and g_state.packetSend are correct
 */
static void HttpResponse_AddHeaderLine(char * headerName, uint16 headerNameLen, char * headerValue, uint16 headerValueLen)
{   
    HttpResponse_AddStringToResponseHeaders(headerName, headerNameLen);
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders(headerValue, headerValueLen);
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER, sizeof(HTTP_HEADER_DELIMITER)-1);
}

/**
 *  Add Header line to response buffer
 *  Adds a line to the header with the provided name value pair when the value is numeric
 *  precondition to this function is that g_state.packetSendSize and g_state.packetSend are correct
 */
static void HttpResponse_AddHeaderLineNumValue(char * headerName, uint16 uHeaderNameLen, uint32 headerValue)
{   
    HttpResponse_AddStringToResponseHeaders(headerName, uHeaderNameLen);
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddNumberToResponseHeaders(headerValue);
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER, sizeof(HTTP_HEADER_DELIMITER)-1);
}

/**
 * Returns status string according to status code
 */
static void HttpStatusString(uint16 uHttpStatus, struct HttpBlob* status)
{
    switch (uHttpStatus)
    {
    case HTTP_STATUS_OK:
        HttpBlobSetConst(*status, HTTP_STATUS_OK_STR);
        break;
    case HTTP_STATUS_REDIRECT_PERMANENT:
        HttpBlobSetConst(*status, HTTP_STATUS_REDIRECT_PERMANENT_STR);
        break;
    case HTTP_STATUS_REDIRECT_TEMPORARY:
        HttpBlobSetConst(*status, HTTP_STATUS_REDIRECT_TEMPORARY_STR);
        break;
    case HTTP_STATUS_ERROR_UNAUTHORIZED:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_UNAUTHORIZED_STR);
        break;
    case HTTP_STATUS_ERROR_NOT_FOUND:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_NOT_FOUND_STR);
        break;
    case HTTP_STATUS_ERROR_NOT_ACCEPTED:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_NOT_ACCEPTED_STR);
        break;
    case HTTP_STATUS_ERROR_INTERNAL:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_INTERNAL_STR);
        break;
    default:
        HttpDebug("Unknown response status");
        HttpAssert(0);
        break;
    }
}

void HttpResponse_Headers(uint16 uConnection, uint16 uHttpStatus, uint16 uFlags, uint32 uContentLength, struct HttpBlob contentType, struct HttpBlob location)
{
    struct HttpBlob status;
    struct HttpBlob packet;
    HttpAssert(g_state.packetSendSize == 0);
    HttpAssert(g_state.connections[uConnection].connectionState == Processing);

    // Get status string according to uHttpStatus
    HttpStatusString(uHttpStatus, &status);

    // Build the response status line in the packet-send buffer: "HTTP/1.1 " followed by the status number as string, a space, the status string, and "\r\n"
    // For example: HTTP/1.1 200 OK\r\n

    // Add http version to sent packet according to the version that was received in the request
    if ( (g_state.connections[uConnection].request.uFlags & HTTP_REQUEST_1_1) != 0)
        HttpResponse_AddStringToResponseHeaders(HTTP_VERSION_1P1, sizeof(HTTP_VERSION_1P1)-1);
    else
        HttpResponse_AddStringToResponseHeaders(HTTP_VERSION_1P0, sizeof(HTTP_VERSION_1P0)-1);
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddNumberToResponseHeaders(uHttpStatus);
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders((char*)status.pData, status.uLength);
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER, sizeof(HTTP_HEADER_DELIMITER)-1);
 

    // Handle Authentication
    if (uHttpStatus == HTTP_STATUS_ERROR_UNAUTHORIZED)
    {
#ifdef HTTP_CORE_ENABLE_AUTH
        HttpResponse_GetPacketSendBuffer(&packet);
        packet.pData = packet.pData + packet.uLength;
        packet.uLength = HTTP_CORE_MAX_PACKET_SIZE_SEND - packet.uLength;
        HttpAuth_ResponseAuthenticate(&g_state.connections[uConnection].request, &packet);
        if (packet.uLength > 0)
            g_state.packetSendSize += packet.uLength;
#endif
    }

    // Handle content type
    //    e.g. "Content-Type: text/html\r\n"
    if ((contentType.pData != NULL) && (contentType.uLength > 0))
        HttpResponse_AddHeaderLine(HTTP_CONTENT_TYPE, sizeof(HTTP_CONTENT_TYPE)-1, (char*)contentType.pData, contentType.uLength);

    // Handle Content-length
    //    e.g. "Content-Length: 562\r\n"
    if (uContentLength > 0)
        HttpResponse_AddHeaderLineNumValue(HTTP_CONTENT_LENGTH, sizeof(HTTP_CONTENT_LENGTH)-1, uContentLength);
    g_state.connections[uConnection].uContentLeft = uContentLength;

    // Handle compression
    //    e.g. "Content-Encoding: gzip\r\n"
    if ((uFlags & HTTP_RESPONSE_FLAG_COMPRESSED) != 0)
        HttpResponse_AddHeaderLine(HTTP_CONTENT_ENCODING, sizeof(HTTP_CONTENT_ENCODING)-1, HTTP_GZIP, sizeof(HTTP_GZIP)-1);

    // Handle redirection/Location
    //    e.g. "Location: /otherpage.html\r\n"
    if ((location.pData != NULL) && (location.uLength > 0))
        HttpResponse_AddHeaderLine(HTTP_LOCATION, sizeof(HTTP_LOCATION)-1, (char*)location.pData, location.uLength);

    // Add the end of headers marker (\r\n)
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER, sizeof(HTTP_HEADER_DELIMITER)-1);

    // Send all response headers over the connection socket
    packet.pData = g_state.packetSend;
    packet.uLength = g_state.packetSendSize;
    if (!HttpCore_SendPacket(uConnection, packet))
        return;
    g_state.packetSendSize = 0;
    
    // advance state according to need to send content
    if (uContentLength > 0)
        g_state.connections[uConnection].connectionState = ResponseData;
    else
        g_state.connections[uConnection].connectionState = ResponseComplete;
    /*
        Todo: add logic to send the header packet at any point in the middle, if there is not enough room left in it.
    */
}

void HttpResponse_GetPacketSendBuffer(struct HttpBlob* pPacketSendBuffer)
{
	pPacketSendBuffer->pData = g_state.packetSend;
	pPacketSendBuffer->uLength = g_state.packetSendSize;
}

void HttpResponse_Content(uint16 uConnection, struct HttpBlob content)
{
    HttpAssert(g_state.connections[uConnection].connectionState == ResponseData);
    HttpAssert(g_state.connections[uConnection].uContentLeft >= content.uLength);

    // Send the specified content over the data socket
    HttpCore_SendPacket(uConnection, content);

    // Update uContentLeft
    g_state.connections[uConnection].uContentLeft -= content.uLength;

    // If no more content left to send then HTTP transaction is complete
    if (g_state.connections[uConnection].uContentLeft == 0)
        g_state.connections[uConnection].connectionState = ResponseComplete;
}

void HttpResponse_CannedRedirect(uint16 uConnection, struct HttpBlob location, uint16 bPermanent)
{
    struct HttpBlob status;
    HttpStatusString((bPermanent==1? HTTP_STATUS_REDIRECT_PERMANENT:HTTP_STATUS_REDIRECT_TEMPORARY), &status);
    
    HttpResponse_Headers(uConnection, (bPermanent==1? HTTP_STATUS_REDIRECT_PERMANENT:HTTP_STATUS_REDIRECT_TEMPORARY), 0, 0, nullBlob, location);
}

void HttpResponse_CannedError(uint16 uConnection, uint16 uHttpStatus)
{
    struct HttpBlob status;
    HttpStatusString(uHttpStatus, &status);

    HttpResponse_Headers(uConnection, uHttpStatus, 0, status.uLength, nullBlob, nullBlob);
    HttpResponse_Content(uConnection, status);
}


/// @}
