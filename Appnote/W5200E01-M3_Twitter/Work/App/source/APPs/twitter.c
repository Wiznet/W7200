
#include "config.h"
#include "APPS\hyperterminal.h"
#include "W5200\w5200.h"
#include "W5200\socket.h"
#include "APPS\dns.h"
#include "APPS\twitter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern CONFIG_MSG Config_Msg;

char buf[255];
u16 client_port = 2000;

u8 Twitter_Post(u8 s, u8 * msg, u8 *pSip)
{
	u16 n;

 	// make packet
 	memset(buf, '\0', sizeof(buf));
 	n = sprintf(buf, "POST http://%s/update HTTP/1.0\r\n", LIB_DOMAIN);
	n += sprintf(buf+n, "Content-Length: %d\r\n\r\n", (u16)(strlen((void *)msg)+strlen(token)+14));
	n += sprintf(buf+n, "token=%s", token);
	n += sprintf(buf+n, "&status=%s\r\n", msg);

	SerialPutString(buf);

	// connnect to server
	if (TCPClientOpen(s, client_port++, pSip, 80) == SUCCESS) {

		// send
		TCPSend(s, (void *)buf, n);

		SerialPutString("\r\nSend ok");

		TCPClose(s);
		while(getSn_SR(s) != SOCK_CLOSED);
	}else {
		SerialPutString("\r\nconnection error");

		return(0);
	}

	return(1);
}
