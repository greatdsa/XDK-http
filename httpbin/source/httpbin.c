/*
* Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licenseeï¿½s application development.
* Fitness and suitability of the example code for any use within Licenseeï¿½s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
* Licensee shall be responsible for conducting the development of its applications as well as integration of parts of the example code into such applications, taking into account the state of the art of technology and any statutory regulations and provisions applicable for such applications. Compliance with the functional system requirements and testing there of (including validation of information/data security aspects and functional safety) and release shall be solely incumbent upon Licensee. 
* For the avoidance of doubt, Licensee shall be responsible and fully liable for the applications and any distribution of such applications into the market.
* 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are 
* met:
* 
*     (1) Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer. 
* 
*     (2) Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.  
*     
*     (3)The name of the author may not be used to
*     endorse or promote products derived from this software without
*     specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*/
/*----------------------------------------------------------------------------*/
/**
* @ingroup APPS_LIST
*
* @defgroup XDK_APPLICATION_TEMPLATE XDK Application Template
* @{
*
* @brief XDK Application Template
*
* @details Empty XDK Application Template without any functionality. Should be used as a template to start new projects.
*
* @file
**/
/* module includes ********************************************************** */

/* system header files */
#include <stdio.h>
#include <Serval_HttpClient.h>
/* additional interface header files */
#include "FreeRTOS.h"
#include "timers.h"

/* own header files */
#include "httpbin.h"
#include "BCDS_CmdProcessor.h"
#include "BCDS_Assert.h"



#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#include "PAL_Initialize_ih.h"
#include "PAL_socketMonitor_ih.h"
#include "Pip.h"



/* constant definitions ***************************************************** */

/* local variables ********************************************************** */



/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

static	retcode_t	onHTTPRequestSent(Callable_T *callfunc, retcode_t status)
{
 (void) (callfunc);

 	 if (status != RC_OK) {
 		 printf("Failed to send HTTP request!\r\n");
 	 	 }
 	 return(RC_OK);
}



static retcode_t onHTTPResponseReceived(HttpSession_T *httpSession, Msg_T *msg_ptr, retcode_t status)
{
 (void) (httpSession);

 if (status == RC_OK && msg_ptr != NULL) {
	 Http_StatusCode_T statusCode = HttpMsg_getStatusCode(msg_ptr);
	 char const *contentType = HttpMsg_getContentType(msg_ptr);

	 char const *content_ptr;
	 unsigned int contentLength = 0;
	 HttpMsg_getContent(msg_ptr, &content_ptr, &contentLength);
	 char content[contentLength+1];
	 strncpy(content, content_ptr, contentLength);
	 content[contentLength] = 0;

	 printf("HTTP RESPONSE: %d [%s]\r\n", statusCode, contentType);
	 printf("%s\r\n", content);

 	 } else {
 		 printf("Failed to receive HTTP response!\r\n");
 	 }
 	 return(RC_OK);
}

/* global functions ********************************************************* */

/**
 * @brief This is a template function where the user can write his custom application.
 *
 */
void appInitSystem(void * CmdProcessorHandle, uint32_t param2)
{
    if (CmdProcessorHandle == NULL)
    {
        printf("Command processor handle is null \n\r");
        assert(false);
    }
    BCDS_UNUSED(param2);

    	WlanConnect_SSID_T connectSSID = (WlanConnect_SSID_T)	"OnePlus 5T";
    	WlanConnect_PassPhrase_T connectPassPhrase = (WlanConnect_PassPhrase_T) "password123";
        WlanConnect_Init();
        NetworkConfig_SetIpDhcp(0);
        WlanConnect_WPA(connectSSID, connectPassPhrase, NULL);
        PAL_initialize();
        PAL_socketMonitorInit();


    /* Initialize HTTP CLient*/

    HttpClient_initialize();

    /*SEt IP address and port*/

    Ip_Address_T destAddr;
    PAL_getIpaddress((uint8_t*)"httpbin.org",&destAddr);
    Ip_Port_T port = Ip_convertIntToPort(80);

	/*	Set up message structure with all information required to create http request*/

	Msg_T* msg_ptr;
    HttpClient_initRequest(&destAddr,port,&msg_ptr);
    HttpMsg_setReqMethod(msg_ptr, Http_Method_Get);
	HttpMsg_setReqUrl(msg_ptr, "/get");
    HttpMsg_setHost(msg_ptr,"httpbin.org");

    /* Send the request*/
    static Callable_T sentCallable;
    Callable_assign(&sentCallable, &onHTTPRequestSent);

    HttpClient_pushRequest(msg_ptr, &sentCallable, &onHTTPResponseReceived);



}
/**@} */
/** ************************************************************************* */
