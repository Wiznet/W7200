
#include "config.h"
#include "APPS\hyperterminal.h"
#include "W5200\w5200.h"
#include "W5200\socket.h"
#include "APPs\smtp.h"
#include "APPs\base64.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
********************************************************************************
Define Part
********************************************************************************
*/
//#define DBG_SMTP

/*
********************************************************************************
Local Variable Declaration Section
********************************************************************************
*/
char buf[255];
u16 client_port = 2000;

/*
********************************************************************************
Function Implementation Part
********************************************************************************
*/

/*
********************************************************************************
*              MAKE MIME MESSAGE
*
* Description : This function makes MIME message.
* Arguments   : MIME - is a pointer to the destination.
                sender - is a pointer to the sender.
                recipient - is a pointer to the recipient.
                subject - is a pointer to the subject of mail.
                content - is a pointer to the content of mail.
* Returns     : none.
* Note        :
********************************************************************************
*/

void MakeMIME(u8 * MIME, u8 * sender, u8 * recipient, u8 * subject, u8 * content)
{
	sprintf((void *)MIME,   "From: %s\r\n"\
				"To: %s\r\n"\
				"Subject: %s\r\n"\
				"\r\n%s\r\n"\
				"\r\n."\
				, sender, recipient, subject, content);
}

/*
********************************************************************************
*              SEND MESSAGE AND RECEIVE THE REPLY
*
* Description : This function makes DNS query message and parses the reply from DNS server.
* Arguments   : s - is a socket number.
                data - is a pointer to the data for send.
                pSip - is a pointer to the server ip.
* Returns     : none.
* Note        :
********************************************************************************
*/

void send_receive(u8 s, u8 * data, u8 * pSip)
{
	s16 n;

#ifdef DBG_SMTP
	printf("Send------>\r\n%s\r\n", data);
#endif

	n = sprintf(buf, "%s\r\n", data);
	while(TCPSend(s, (void *)buf, n) <= 0);

	//recv
	while((n = TCPRecv(s, (void *)buf, 255)) <= 0);
	buf[n]='\0';

#ifdef DBG_SMTP
	printf("Receive------>\r\n%s\r\n", buf);
#endif
}


/*
********************************************************************************
*              SEND MAIL
*
* Description : This function send mail and parses the reply from mail server.
* Arguments   : s - is a socket number.
                sender - is a pointer to the sender.
                passwd - is a pointer to the password.
                recipient - is a pointer to the recipient.
                subject - is a pointer to the subject of mail.
                content - is a pointer to the content of mail.
                pSip - is a pointer to the server ip.
* Returns     : if succeeds : 1, fails : 0
* Note        :
********************************************************************************
*/

u8 send_mail(u8 s, u8 * sender, u8 * passwd, u8 * recipient, u8 * subject, u8 * content, u8 * pSip)
{
	u8 MIME[256]={'\0'}, encode[16]={'\0'}, ret=1;
	s16 n;

	// connnect to server
	if (TCPClientOpen(s, client_port++, pSip, 25) == 1) {
		do{

			//recv
			while((n = TCPRecv(s, (void *)buf, 255)) <= 0);
			buf[n]='\0';
#ifdef DBG_SMTP
			printf("Receive(%d)------>\r\n%s\r\n", n, buf);
#endif
			if(strncmp(buf, "220", 3)) {
				SerialPutString("This is not SMTP Server\r\n");
				ret = 0;
				break;
			}

			// Send HELO
			send_receive(s, "EHLO", pSip);
			if(strncmp(buf, "250", 3)) {
				SerialPutString("Fail HELO\r\n");
				ret = 0;
				break;
			}

			// Send AUTH LOGIN
			send_receive(s, "AUTH LOGIN", pSip);
			if(strncmp(buf, "334", 3)) {
				SerialPutString("Fail AUTH LOGIN\r\n");
				ret = 0;
				break;
			}

			// Send ID
			base64_encode((void *)sender, strlen((void *)sender)+1, (void *)encode);
			send_receive(s, encode, pSip);
			if(strncmp(buf, "334", 3)) {
				SerialPutString("Fail ID\r\n");
				ret = 0;
				break;
			}

			// Send PW
			base64_encode((void *)passwd, strlen((void *)passwd)+1, (void *)encode);
			send_receive(s, encode, pSip);
			if(strncmp(buf, "235", 3)) {
				SerialPutString("Fail PW\r\n");
				ret = 0;
				break;
			}

			// Send MAIL FROM
			sprintf(buf, "MAIL FROM:%s", sender);
			send_receive(s, (void *)buf, pSip);
			if(strncmp(buf, "250", 3)) {
				SerialPutString("Fail MAIL FROM\r\n");
				ret = 0;
				break;
			}

			// Send RCPT
			sprintf(buf, "RCPT TO:%s", recipient);
			send_receive(s, (void *)buf, pSip);
			if(strncmp(buf, "250", 3)) {
				SerialPutString("Fail RCPT TO\r\n");
				ret = 0;
				break;
			}

			// Send DATA
			send_receive(s, "DATA", pSip);
			if(strncmp(buf, "354", 3)) {
				SerialPutString("Fail DATA\r\n");
				ret = 0;
				break;
			}

			// Send content
			MakeMIME(MIME, sender, recipient, subject, content);
			send_receive(s, MIME, pSip);
			if(strncmp(buf, "250", 3)) {
				SerialPutString("Fail Send Content\r\n");
				ret = 0;
				break;
			}

			// Send QUIT
			send_receive(s, "QUIT", pSip);

			SerialPutString("\r\nSend ok\r\n");

		}while(0);

	}else {
		SerialPutString("\r\nconnection error");

		return(0);
	}

	TCPClose(s);
	while(getSn_SR(s) != SOCK_CLOSED);

	return(ret);
}
