/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : hyperterminal.c
* Author             : MCD Application Team
* Date First Issued  : 10/25/2004
* Description        : This file provides all the Hyperterminal driver functions.
********************************************************************************
* History:
*  09/15/2006 : Hyperterminal V1.00
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "APPs\hyperterminal.h"
#include "stm32f10x.h"

#include "config.h"
#include "util.h"
#include "W5200\w5200.h"
#include "W5200\socket.h"
#include "APPs\dns.h"
#include "APPs\smtp.h"
#include "APPs\loopback.h"
#include "APPs\base64.h"
#include "sockutil.h"
#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Main_Menu
* Description    : Display/Manage a Menu on HyperTerminal Window
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Main_Menu(void)
{
	static char choice[3];
	static char subject[32], msg[256], sender[32], passwd[32], recipient[32], encodedText[256];
	static bool bTreat;
	static u8 Sip[4];
	static char key = 0;
	static wiz_NetInfo netinfo;
	
	
	while (1)
	{
		/* Display Menu on HyperTerminal Window */
		bTreat = (bool)RESET ;
   	        SerialPutString("\r\n====================== STM32-Discovery ===================\r\n");
		SerialPutString("This Application is basic example of UART interface with\r\n");
		SerialPutString("Windows Hyper Terminal. \r\n");
		SerialPutString("\r\n==========================================================\r\n");
		SerialPutString("                          APPLICATION MENU :\r\n");
		SerialPutString("\r\n==========================================================\r\n\n");
		SerialPutString(" 1 - Set LD1 on \r\n");
		SerialPutString(" 2 - Set LD1 off \r\n");
		SerialPutString(" 3 - Show network setting\r\n");
		SerialPutString(" 4 - Set  network setting\r\n");
		SerialPutString(" 5 - Run TCP Loopback\r\n");
		SerialPutString(" 6 - Run UDP Loopback\r\n");
		SerialPutString(" 7 - DNS test\r\n");
		SerialPutString(" 8 - BASE64 test\r\n");
		SerialPutString(" 9 - Send Mail\r\n");
		
		SerialPutString("Enter your choice : ");
		GetInputString(choice);
		/* Set LD1 on */
		if (strcmp(choice,"1")== 0)
		{
			bTreat = (bool)SET;
			LED3_onoff(ON);
			LED4_onoff(ON);
		}
		/* Set LD1 off */
		if ((strcmp(choice,"2") == 0))
		{
			bTreat = (bool)SET;
			LED3_onoff(OFF);
			LED4_onoff(OFF);
		}
		if (strcmp(choice,"3") == 0)
		{
			bTreat = (bool)SET;
			GetNetInfo(&netinfo);
			printf("\r\nIP : %d.%d.%d.%d", netinfo.IP[0],netinfo.IP[1],netinfo.IP[2],netinfo.IP[3]);
			printf("\r\nSN : %d.%d.%d.%d", netinfo.Subnet[0],netinfo.Subnet[1],netinfo.Subnet[2],netinfo.Subnet[3]);
			printf("\r\nGW : %d.%d.%d.%d", netinfo.Gateway[0],netinfo.Gateway[1],netinfo.Gateway[2],netinfo.Gateway[3]);
			printf("\r\nDNS server : %d.%d.%d.%d", netinfo.DNSServerIP[0],netinfo.DNSServerIP[1],netinfo.DNSServerIP[2],netinfo.DNSServerIP[3]);

		}

		if (strcmp(choice,"4") == 0)
		{
			bTreat = (bool)SET;
			// IP address
			SerialPutString("\r\nIP address : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, netinfo.IP))
			{
				SerialPutString("\aInvalid.");
			}

			// Subnet mask
			SerialPutString("\r\nSubnet mask : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, netinfo.Subnet))
			{
				SerialPutString("\aInvalid.");
			}
			
			// gateway address
			SerialPutString("\r\nGateway address : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, netinfo.Gateway))
			{
				SerialPutString("\aInvalid.");
			}

			// DNS address
			SerialPutString("\r\nDNS address : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, netinfo.DNSServerIP))
			{
				SerialPutString("\aInvalid.");
			}

			printf("\r\nIP : %d.%d.%d.%d", netinfo.IP[0],netinfo.IP[1],netinfo.IP[2],netinfo.IP[3]);
			printf("\r\nSN : %d.%d.%d.%d", netinfo.Subnet[0],netinfo.Subnet[1],netinfo.Subnet[2],netinfo.Subnet[3]);
			printf("\r\nGW : %d.%d.%d.%d", netinfo.Gateway[0],netinfo.Gateway[1],netinfo.Gateway[2],netinfo.Gateway[3]);
			printf("\r\nDNS server : %d.%d.%d.%d", netinfo.DNSServerIP[0],netinfo.DNSServerIP[1],netinfo.DNSServerIP[2],netinfo.DNSServerIP[3]);

			SetNetInfo(&netinfo);
		}
		

		if (strcmp(choice,"5") == 0)
		{
			bTreat = (bool)SET;
		  
			SerialPutString("\r\nRun TCP loopback");
			printf("\r\nRun TCP loopback, port number [%d] is listened", (u16)TCP_LISTEN_PORT);
			SerialPutString("\r\nTo Exit, press [Q]");
			
		  while(1) {

			if ((SerialKeyPressed((char*)&key) == 1) && (key == 'Q')) {
				SerialPutString("\r\n Stop ");
				
				break;
			}

			loopback_tcps(7, (u16)TCP_LISTEN_PORT);
		  }
		  

		}


		if (strcmp(choice,"6") == 0)
		{
			bTreat = (bool)SET;
		  
			SerialPutString("\r\nRun UDP loopback");
			printf("\r\nRun UDP loopback, port number [%d] is listened", (u16)UDP_LISTEN_PORT);
			SerialPutString("\r\nTo Exit, press [Q]");
			
			while(1) {

				if ((SerialKeyPressed((char*)&key) == 1) && (key == 'Q')) {
					SerialPutString("\r\n Stop ");
				
					break;

				}

				loopback_udp(7, (u16)UDP_LISTEN_PORT);
			}

		}


		if (strcmp(choice,"7")== 0)
		{
		  	bTreat = (bool)SET;
			
			SerialPutString("\r\nServer address : ");
			GetInputString(msg);

			SerialPutString("URL = ");
			SerialPutString(msg);
			  	
			if (dns_query(SOCK_DNS, (void *)msg, Sip) == 1) {

				printf("\r\nSIP : %d.%d.%d.%d", (u16)Sip[0],(u16)Sip[1],(u16)Sip[2],(u16)Sip[3]);

			}else {
				SerialPutString("\n\r DNS fail");

			}

			
		}


		if (strcmp(choice,"8")== 0)
		{
			bTreat = (bool)SET;
			
			memset(encodedText, '\0', 256);

			SerialPutString("\r\n");
			SerialPutString(" 1 - BASE64 Encode \r\n");
			SerialPutString(" 2 - BASE64 Decode \r\n");
			SerialPutString("Enter your choice : ");
			GetInputString(choice);

			if (strcmp(choice,"1")== 0) {

				SerialPutString("Type Plain Text\r\n");
				GetInputString(msg);
				base64_encode(msg, strlen(msg)+1, encodedText);
				SerialPutString("Encoded Text\r\n");
				printf("%s\r\n", encodedText);

			}else if(strcmp(choice,"2")== 0){

				SerialPutString("Type Encoded Text\r\n");
				GetInputString(msg);
				base64_decode(msg, (void *)encodedText, strlen(msg));
				SerialPutString("Decoded Text\r\n");
				printf("%s\r\n", encodedText);

			}

		}


		if (strcmp(choice,"9")== 0) {
		  	bTreat = (bool)SET;

			SerialPutString("\r\nServer address : ");
			GetInputString(msg);

			SerialPutString("URL = ");
			SerialPutString(msg);

			// DNS
			if (dns_query(SOCK_DNS, (void *)msg, Sip) == 1) {
				printf("\r\nSIP : %d.%d.%d.%d", (u16)Sip[0],(u16)Sip[1],(u16)Sip[2],(u16)Sip[3]);

				while(1) {

					SerialPutString("\r\nType a Sender: ");
					GetInputString(sender);

					SerialPutString("Type a Password: ");
					GetInputString(passwd);

					SerialPutString("Type a Recipient: ");
					GetInputString(recipient);

					SerialPutString("Type a Subject: ");
					GetInputString(subject);

					SerialPutString("Type a message: ");
					GetInputString(msg);

					send_mail(SOCK_SMTP, (void *)sender, (void *) passwd, (void *)recipient, (void *)subject, (void *)msg, Sip);

					SerialPutString("\r\nIf you want send another message? [YES]: any key, [NO]: Q");
					key = GetKey();

					if (key == 'Q') {
						SerialPutString("\r\n Stop ");

						break;

					}

				}

			}else {
				SerialPutString("\r\nDNS error");

			}
		}


		/* OTHERS CHOICE*/
		if (bTreat == (bool)RESET)
		{
			SerialPutString(" wrong choice  \r\n");
		}			
	} /* While(1)*/
}/* Main_Menu */

/*******************(C)COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


