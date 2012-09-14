
#include "config.h"
#include "W5200\w5200.h"
#include "W5200\socket.h"
#include "APPs\dns.h"
#include "APPs\dns_parse.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8 DNS_SERVER_IP[4]={8,8,8,8};

extern __IO uint32_t Timer2_Counter;

/*
********************************************************************************
Define Part
********************************************************************************
*/
//#define DBG_DNS

/*
********************************************************************************
Local Variable Declaration Section
********************************************************************************
*/
u16 MSG_ID = 0x1122;
u8 dns_buf[MAX_DNS_BUF_SIZE];

/*
********************************************************************************
Function Implementation Part
********************************************************************************
*/

/*
********************************************************************************
*              PUT NETWORK BYTE ORDERED INT.
*
* Description : This function copies u16 to the network buffer with network byte order.
* Arguments   : s - is a pointer to the network buffer.
*               i - is a unsigned integer.
* Returns     : a pointer to the buffer.
* Note        : Internal Function
********************************************************************************
*/
u8 * put16(u8 * s, u16 i)
{
	*s++ = i >> 8;
	*s++ = i;

	return s;
}


/*
********************************************************************************
*              MAKE DNS QUERY MESSAGE
*
* Description : This function makes DNS query message.
* Arguments   : op   - Recursion desired
*               name - is a pointer to the domain name.
*               buf  - is a pointer to the buffer for DNS message.
*               len  - is the MAX. size of buffer.
* Returns     : the pointer to the DNS message.
* Note        :
********************************************************************************
*/

u16 dns_makequery(u16 op, char * name, u8 * buf, u16 len)
{
	u8 *cp;
	char *cp1;
	char sname[MAX_DNS_BUF_SIZE];
	char *dname;
	u16 p;
	u16 dlen;

	cp = buf;

	MSG_ID++;
	cp = put16(cp, MSG_ID);
	p = (op << 11) | 0x0100;			/* Recursion desired */
	cp = put16(cp, p);
	cp = put16(cp, 1);
	cp = put16(cp, 0);
	cp = put16(cp, 0);
	cp = put16(cp, 0);

	strcpy(sname, name);
	dname = sname;
	dlen = strlen(dname);
	for (;;)
	{
		/* Look for next dot */
		cp1 = strchr(dname, '.');

		if (cp1 != NULL) len = cp1 - dname;	/* More to come */
		else len = dlen;			/* Last component */

		*cp++ = len;				/* Write length of component */
		if (len == 0) break;

		/* Copy component up to (but not including) dot */
		strncpy((char *)cp, dname, len);
		cp += len;
		if (cp1 == NULL)
		{
			*cp++ = 0;			/* Last one; write null and finish */
			break;
		}
		dname += len+1;
		dlen -= len+1;
	}

	cp = put16(cp, 0x0001);				/* type */
	cp = put16(cp, 0x0001);				/* class */

        return ((u16)(cp - buf));
}

/*
********************************************************************************
*              MAKE DNS QUERY AND PARSE THE REPLY
*
* Description : This function makes DNS query message and parses the reply from DNS server.
* Arguments   : name - is a pointer to the domain name.
* Returns     : if succeeds : 1, fails : -1
* Note        :
********************************************************************************
*/

u8 dns_query(u8 s, u8 * name, u8 * pSip)
{
	struct dhdr dhp;
	u8 ip[4];
	u16 len, port;
	u16 cnt;


	//for(;socket(s, Sn_MR_UDP, 0, 0) != 1;);
        while(UDPOpen(s,0)!=1);
        while(getSn_SR(s) != SOCK_UDP);
        Delay_ms(50);

	len = dns_makequery(0, (char *)name, dns_buf, MAX_DNS_BUF_SIZE);
	cnt = UDPSend(s, dns_buf, len, DNS_SERVER_IP, IPPORT_DOMAIN);

	if (cnt == 0) return(0); // dns fail

	cnt = 0;

	while (1)
	{
		if ((len = getSn_RX_RSR(s)) > 0)
		{
			if (len > MAX_DNS_BUF_SIZE) len = MAX_DNS_BUF_SIZE;
			len = UDPRecv(s, dns_buf, len, ip, &port);

			UDPClose(s);
			while(getSn_SR(s) != SOCK_CLOSED);
			break;
		}
		Delay_ms(10);
		if (cnt++ == 100)
		{
			UDPClose(s);
			while(getSn_SR(s) != SOCK_CLOSED);
			return 0;
		}
	}


	return(parseMSG(&dhp, dns_buf, pSip));	/* Convert to local format */
}
