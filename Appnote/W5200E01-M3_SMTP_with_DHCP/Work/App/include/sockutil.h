/*
*
@file		sockutil.h
@brief	Implementation of useful function of iinChip
*
*/

#ifndef __SOCKUTIL_H
#define __SOCKUTIL_H


#define NO_USE_SOCKUTIL_FUNC

/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE     128
/* Exported macro ------------------------------------------------------------*/
#define IS_AF(c)	((c >= 'A') && (c <= 'F'))
#define IS_af(c)	((c >= 'a') && (c <= 'f'))
#define IS_09(c)	((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)	IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)	IS_09(c)
#define CONVERTDEC(c)	(c - '0')
#define CONVERTHEX_alpha(c)	(IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))
/* Exported functions ------------------------------------------------------- */
extern void Int2Str(char *str ,uint32 intnum);
extern uint8 Str2Int(char *inputstr,uint32 *intnum);
extern uint8 GetIntegerInput(uint32 *num);
extern uint8 SerialKeyPressed(char *key);
extern char GetKey(void);
extern void SerialPutChar(char c);
extern void SerialPutString(char *s);
extern void GetInputString(char * buffP);
extern char VerifyIPAddress(char* src, uint8 * ip);

extern char* inet_ntoa(unsigned long addr);			/* Convert 32bit Address into Dotted Decimal Format */
extern char* inet_ntoa_pad(unsigned long addr);

extern unsigned long inet_addr(unsigned char* addr);		/* Converts a string containing an (Ipv4) Internet Protocol decimal dotted address into a 32bit address */

extern unsigned long GetDestAddr(SOCKET s);			/* Output destination IP address of appropriate channel */

extern unsigned int GetDestPort(SOCKET s);			/* Output destination port number of appropriate channel */

extern unsigned short htons( unsigned short hostshort);	/* htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).*/

extern unsigned long htonl(unsigned long hostlong);		/* htonl function converts a unsigned long from host to TCP/IP network byte order (which is big-endian). */

extern unsigned long ntohs(unsigned short netshort);		/* ntohs function converts a unsigned short from TCP/IP network byte order to host byte order (which is little-endian on Intel processors). */

extern unsigned long ntohl(unsigned long netlong);		/* ntohl function converts a u_long from TCP/IP network order to host byte order (which is little-endian on Intel processors). */

extern u_char CheckDestInLocal(u_long destip);			/* Check Destination in local or remote */

extern SOCKET getSocket(unsigned char status, SOCKET start); 	/* Get handle of socket which status is same to 'status' */

extern unsigned short checksum(unsigned char * src, unsigned int len);		/* Calculate checksum of a stream */

#ifndef NO_USE_SOCKUTIL_FUNC

extern u_long GetIPAddress(void);					/* Get Source IP Address of iinChip. */

extern u_long GetGWAddress(void);					/* Get Source IP Address of iinChip. */

extern u_long GetSubMask(void);					/* Get Source Subnet mask of iinChip. */

extern void GetMacAddress(unsigned char* mac);		/* Get Mac address of iinChip. */

extern void GetDestMacAddr(SOCKET s, u_char* mac);

extern void GetNetConfig(void);				/* Read established network information(G/W, IP, S/N, Mac) of iinChip and Output that through Serial.*/

extern void dump_iinchip(void);					/* dump the 4 channel status of iinChip */

#endif

#endif
