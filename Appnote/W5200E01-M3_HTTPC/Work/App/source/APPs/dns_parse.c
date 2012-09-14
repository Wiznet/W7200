#include "config.h"
#include "W5200\socket.h"
#include "APPS\dns.h"
#include "APPS\dns_parse.h"
#include <string.h>
#include <stdio.h>



/*
********************************************************************************
*              GET HOST BYTE ORDERED INT.
*
* Description : This function converts u16 from network buffer to a host byte order integer.
* Arguments   : s - is a pointer to the network buffer
* Returns     : host byte order unsigned integer.
* Note        : Internal Function
********************************************************************************
*/
u16 get16(u8 * s)
{
	u16 i;

	i = *s++ << 8;
	i = i + *s;

	return i;
}

/*
********************************************************************************
*              CONVERT A DOMAIN NAME TO THE HUMAN-READABLE FORM
*
* Description : This function converts a compressed domain name to the human-readable form
* Arguments   : msg        - is a pointer to the reply message
*               compressed - is a pointer to the domain name in reply message.
*               buf        - is a pointer to the buffer for the human-readable form name.
*               len        - is the MAX. size of buffer.
* Returns     : the length of compressed message
* Note        :
********************************************************************************
*/
int parse_name(u8 * msg, u8 * compressed, char * buf, s16 len)
{
	u16 slen;		/* Length of current segment */
	u8 * cp;
	int clen = 0;		/* Total length of compressed name */
	int indirect = 0;	/* Set if indirection encountered */
	int nseg = 0;		/* Total number of segments in name */

	cp = compressed;

	for (;;)
	{
		slen = *cp++;	/* Length of this segment */

		if (!indirect) clen++;

		if ((slen & 0xc0) == 0xc0)
		{
			if (!indirect)
				clen++;
			indirect = 1;
			/* Follow indirection */
			cp = &msg[((slen & 0x3f)<<8) + *cp];
			slen = *cp++;
		}

		if (slen == 0)	/* zero length == all done */
			break;

		len -= slen + 1;

		if (len < 0) return -1;

		if (!indirect) clen += slen;

		while (slen-- != 0) *buf++ = (char)*cp++;
		*buf++ = '.';
		nseg++;
	}

	if (nseg == 0)
	{
		/* Root name; represent as single dot */
		*buf++ = '.';
		len--;
	}

	*buf++ = '\0';
	len--;

	return clen;	/* Length of compressed message */
}



/*
********************************************************************************
*              PARSE QUESTION SECTION
*
* Description : This function parses the qeustion record of the reply message.
* Arguments   : msg - is a pointer to the reply message
*               cp  - is a pointer to the qeustion record.
* Returns     : a pointer the to next record.
* Note        :
********************************************************************************
*/
u8 * dns_question(u8 * msg, u8 * cp)
{
	int len;
	char name[MAX_DNS_BUF_SIZE];

	len = parse_name(msg, cp, name, MAX_DNS_BUF_SIZE);

#ifdef	DBG_DNS
	//myprintf("dns_question : %s\r\n", name);
#endif

	if (len == -1) return 0;

	cp += len;
	cp += 2;		/* type */
	cp += 2;		/* class */

	return cp;
}


/*
********************************************************************************
*              PARSE ANSER SECTION
*
* Description : This function parses the answer record of the reply message.
* Arguments   : msg - is a pointer to the reply message
*               cp  - is a pointer to the answer record.
* Returns     : a pointer the to next record.
* Note        :
********************************************************************************
*/
u8 * dns_answer(u8 * msg, u8 * cp, u8 * pSip)
{
	int len, type;
	char name[MAX_DNS_BUF_SIZE];

	len = parse_name(msg, cp, name, MAX_DNS_BUF_SIZE);

	if (len == -1) return 0;

	cp += len;
	type = get16(cp);
	cp += 2;		/* type */
	cp += 2;		/* class */
	cp += 4;		/* ttl */
	cp += 2;		/* len */

#ifdef	DBG_DNS
	//myprintf("dns_answer : %s : %bu : %.4x\r\n", name, len, type);
#endif

	switch (type)
	{
	case TYPE_A:
		/* Just read the address directly into the structure */
		//Server_IP_Addr[0] = *cp++;
		//Server_IP_Addr[1] = *cp++;
		//Server_IP_Addr[2] = *cp++;
		//Server_IP_Addr[3] = *cp++;
		//Config_Msg.Sip[0] = *cp++;
		//Config_Msg.Sip[1] = *cp++;
		//Config_Msg.Sip[2] = *cp++;
		//Config_Msg.Sip[3] = *cp++;
		pSip[0] = *cp++;
		pSip[1] = *cp++;
		pSip[2] = *cp++;
		pSip[3] = *cp++;
		break;
	case TYPE_CNAME:
	case TYPE_MB:
	case TYPE_MG:
	case TYPE_MR:
	case TYPE_NS:
	case TYPE_PTR:
		/* These types all consist of a single domain name */
		/* convert it to ascii format */
		len = parse_name(msg, cp, name, MAX_DNS_BUF_SIZE);
		if (len == -1) return 0;

		cp += len;
		break;
	case TYPE_HINFO:
		len = *cp++;
		cp += len;

		len = *cp++;
		cp += len;
		break;
	case TYPE_MX:
		cp += 2;
		/* Get domain name of exchanger */
		len = parse_name(msg, cp, name, MAX_DNS_BUF_SIZE);
		if (len == -1) return 0;

		cp += len;
		break;
	case TYPE_SOA:
		/* Get domain name of name server */
		len = parse_name(msg, cp, name, MAX_DNS_BUF_SIZE);
		if (len == -1) return 0;

		cp += len;

		/* Get domain name of responsible person */
		len = parse_name(msg, cp, name, MAX_DNS_BUF_SIZE);
		if (len == -1) return 0;

		cp += len;

		cp += 4;
		cp += 4;
		cp += 4;
		cp += 4;
		cp += 4;
		break;
	case TYPE_TXT:
		/* Just stash */
		break;
	default:
		/* Ignore */
		break;
	}

	return cp;
}

/*
********************************************************************************
*              PARSE THE DNS REPLY
*
* Description : This function parses the reply message from DNS server.
* Arguments   : dhdr - is a pointer to the header for DNS message
*               buf  - is a pointer to the reply message.
* Returns     : None
* Note        :
********************************************************************************
*/
u8 parseMSG(struct dhdr * pdhdr, u8 * pbuf, u8 * pSip)
{
	u16 tmp;
	u16 i;
	u8 * msg;
	u8 * cp;

	msg = pbuf;
	memset(pdhdr, 0, sizeof(pdhdr));

	pdhdr->id = get16(&msg[0]);
	tmp = get16(&msg[2]);
	if (tmp & 0x8000) pdhdr->qr = 1;

	pdhdr->opcode = (tmp >> 11) & 0xf;

	if (tmp & 0x0400) pdhdr->aa = 1;
	if (tmp & 0x0200) pdhdr->tc = 1;
	if (tmp & 0x0100) pdhdr->rd = 1;
	if (tmp & 0x0080) pdhdr->ra = 1;

	pdhdr->rcode = tmp & 0xf;
	pdhdr->qdcount = get16(&msg[4]);
	pdhdr->ancount = get16(&msg[6]);
	pdhdr->nscount = get16(&msg[8]);
	pdhdr->arcount = get16(&msg[10]);

#ifdef	DBG_DNS
	printf("dhdr->qdcount : %x\r\n", (u16)dhdr->qdcount);
	printf("dhdr->ancount : %x\r\n", (u16)dhdr->ancount);
	printf("dhdr->nscount : %x\r\n", (u16)dhdr->nscount);
	printf("dhdr->arcount : %x\r\n", (u16)dhdr->arcount);
#endif

	/* Now parse the variable length sections */
	cp = &msg[12];

	/* Question section */
	for (i = 0; i < pdhdr->qdcount; i++)
	{
		cp = dns_question(msg, cp);
	}

	/* Answer section */
	for (i = 0; i < pdhdr->ancount; i++)
	{
		cp = dns_answer(msg, cp, pSip);
	}

	/* Name server (authority) section */
	for (i = 0; i < pdhdr->nscount; i++)
	{
		;
	}

	/* Additional section */
	for (i = 0; i < pdhdr->arcount; i++)
	{
		;
	}

	if(pdhdr->rcode == 0) return 1;		// No error
	else return 0;
}
