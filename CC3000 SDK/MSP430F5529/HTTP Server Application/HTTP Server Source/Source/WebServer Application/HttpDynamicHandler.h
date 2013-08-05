/*****************************************************************************
*
*  HttpDynamicHandler.h
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
#ifndef _HTTP_DYNAMIC_HANDLER_H_
#define _HTTP_DYNAMIC_HANDLER_H_

#include "HttpConfig.h"
#include "HttpDynamic.h"
#include <msp430.h>

#ifdef HTTP_CORE_ENABLE_DYNAMIC

/** 
 * @defgroup HttpDynamicHandler Handlers methods requested by HttpDynamic module
 * This module implements methods requested by HttpDynamic module.
 * Note this module is only compiled if HTTP_CORE_ENABLE_DYNAMIC is defined in HttpConfig.h
 *
 * @{
 */



/**
 * This function applies an operation on a LED.
 * It is assumed that the parameters are ordered as denoted below.
 * @param uLedInParams Includes the LED parameters: LED number and LED action
 * @param uLedOutParams NA. No return parameter is expected. 
 * @return no return value
 */
void HttpDynamicHandler_ActOnLED(inputParams uLedInParams, outputParams *uLedOutParams);



/**
 * This function gets a temperature reading.
 * @param uTempInParams Denotes the temerature units, Celsius or Fahrenheit
 * @param uTempOutParams Denotes the temerature value
 * @return no return value
 */
void HttpDynamicHandler_GetWheelValue(inputParams uWheelInParams, outputParams *uWheelOutParams);




/// @}

#endif // HTTP_CORE_ENABLE_DYNAMIC

#endif // _HTTP_DYNAMIC_HANDLER_H_
