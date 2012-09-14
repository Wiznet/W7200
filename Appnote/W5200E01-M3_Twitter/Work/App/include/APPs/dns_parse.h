
#ifndef	_DNS_PARSE_H_
#define	_DNSPARSE_H_


#define	MAX_DNS_BUF_SIZE	512		/* maximum size of DNS buffer. */


u8 parseMSG(struct dhdr * dhdr, u8 * buf, u8 * pSip);

#endif

