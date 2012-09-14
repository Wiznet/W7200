/*
*
@file		sockutil.c
@brief	Implementation of useful function of iinChip
*
*/

#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"

#include "types.h"
#include "W5200\w5200.h"
#include "util.h"
#include "sockutil.h"


/*******************************************************************************
* Function Name  : Int2Str
* Description    : Convert an Integer to a string
* Input          : - str: The string
*                  - intnum: The intger to be converted
* Output         : None
* Return         : None
*******************************************************************************/
void Int2Str(char *str, uint32 intnum)
{
    int i, Div = 1000000000, j = 0, Status = 0;
    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;
        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*******************************************************************************
* Function Name  : Str2Int
* Description    : Convert a string to an integer
* Input 1        : - inputstr: The string to be converted
*                  - intnum: The intger value
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
uint8 Str2Int(char *inputstr, uint32 *intnum)
{
    uint8 i = 0, res = 0;
    uint32 val = 0;
    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                res = 1; /* return 1; */
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11) res = 0; /* over 8 digit hex --invalid */
    }
    else /* max 10-digit decimal input */
    {
        for (i = 0;i < 11;i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
                val = val * 10 + CONVERTDEC(inputstr[i]);
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11) res = 0; /* Over 10 digit decimal --invalid */
    }
    return res;
}

/*******************************************************************************
* Function Name  : GetIntegerInput
* Description    : Get an integer from the HyperTerminal
* Input          : - num: The inetger
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
uint8 GetIntegerInput(uint32 *num)
{
    char inputstr[16];
    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0') continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }
        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}

/*******************************************************************************
* Function Name  : SerialKeyPressed
* Description    : Test to see if a key has been pressed on the HyperTerminal
* Input          : - key: The key pressed
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
uint8 SerialKeyPressed(char *key)
{
	FlagStatus flag  ;
	/* First clear Rx buffer */
	flag = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
	if ( flag == SET)
	{
		*key = (char)USART1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : GetKey
* Description    : Get a key from the HyperTerminal
* Input          : None
* Output         : None
* Return         : The Key Pressed
*******************************************************************************/
char GetKey(void)
{
	char key = 0;
	/* Waiting for user input */
	while (1)
	{
		if (SerialKeyPressed((char*)&key)) break;
	}
	return key;
}

/*******************************************************************************
* Function Name  : SerialPutChar
* Description    : Print a character on the HyperTerminal
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutChar(char c)
{
    USART_SendData(USART1, c);
    while ((USART1->SR & USART_SR_TXE ) != USART_SR_TXE );
}

/*******************************************************************************
* Function Name  : SerialPutString
* Description    : Print a string on the HyperTerminal
* Input          : - s: The string to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutString(char *s)
{
	while (*s != '\0')
	{
		SerialPutChar(*s);
		s ++;
	}
}

/*******************************************************************************
* Function Name  : GetInputString
* Description    : Get Input string from the HyperTerminal
* Input          : - buffP: The input string
* Output         : None
* Return         : None
*******************************************************************************/
void GetInputString (char *buffP)
{
	int bytes_read = 0;
	char c = 0;
	do
	{
		c = GetKey();
		if (c == '\r')
      break;
    if (c == '\b') /* Backspace */
    {
      if (bytes_read > 0)
      {
        SerialPutString("\b \b");
        bytes_read --;
      }
      continue;
    }
    if (bytes_read >= CMD_STRING_SIZE )
    {
      SerialPutString("Command string size overflow\r\n");
      bytes_read = 0;
      continue;
    }
    if (c >= 0x20 && c <= 0x7E)
    {
      buffP[bytes_read++] = c;
      SerialPutChar(c);
		}
	}
  while (1);
  SerialPutString("\n\r");
  buffP[bytes_read] = '\0';
}


/**
@brief	CONVERT CHAR INTO HEX
@return	HEX
  
This function converts HEX(0-F) to a character
*/
char C2D(
	uint8 c	/**< is a character('0'-'F') to convert to HEX */
	)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';

	return (char)c;
}


/**
@brief	CONVERT STRING INTO INTEGER
@return	a integer number
*/
uint16 ATOI(
	char* str,	/**< is a pointer to convert */
	uint8  base	/**< is a base value (must be in the range 2 - 16) */
	)
{
        uint16 num = 0;
        while (*str !=0)
                num = num * base + C2D(*str++);
	return num;
}

/**
@brief	CONVERT STRING INTO HEX OR DECIMAL
@return	success - 1, fail - 0
*/
int ValidATOI(
	char* str, 	/**< is a pointer to string to be converted */
	int base, 	/**< is a base value (must be in the range 2 - 16) */
	int* ret		/**<  is a integer pointer to return */
	)
{
	int c;
	char* tstr = str;
	if(str == 0 || *str == '\0') return 0;
	while(*tstr != '\0')
	{
		c = C2D(*tstr);
		if( c >= 0 && c < base) tstr++;
		else    return 0;
	}
	
	*ret = ATOI(str,base);
	return 1;
}


char* STRTOK(char* strToken, const char* strDelimit)
{
       static char* pCurrent;
       char* pDelimit;
 
       if ( strToken != NULL )
             pCurrent = strToken;
       else
             strToken = pCurrent;
 
//       if ( *pCurrent == NULL ) return NULL;
 
       while ( *pCurrent )
       {
             pDelimit = (char*)strDelimit;
             while ( *pDelimit )
             {
                    if ( *pCurrent == *pDelimit )
                    {
                           //*pCurrent = NULL;
                           *pCurrent = 0;
                           ++pCurrent;
                           return strToken;
                    }
                    ++pDelimit;
             }
             ++pCurrent;
       }
 
       return strToken;
}


char VerifyIPAddress(char* src, uint8 * ip)
{
	int i;
	int tnum;
	char tsrc[50];
	char* tok = tsrc;
	
	strcpy(tsrc,src);
	
	for(i = 0; i < 4; i++)
	{
		tok = STRTOK(tok,".");
		if ( !tok ) return 0;
		if(tok[0] == '0' && tok[1] == 'x')
		{
			if(!ValidATOI(tok+2,0x10,&tnum)) return 0;
		}
		else if(!ValidATOI(tok,10,&tnum)) return 0;

		ip[i] = tnum;
		
		if(tnum < 0 || tnum > 255) return 0;
		tok = NULL;
	}
	return 1;	
}

/**
@brief	Convert 32bit Address(Host Ordering) into Dotted Decimal Format
@return 	a char pointer to a static buffer containing the text address in standard ".'' notation. Otherwise, it returns NULL. 
*/  
char* inet_ntoa(
	unsigned long addr	/**< Pointer variable to store converted value(INPUT) */
	)
{
	static char addr_str[16];
	memset(addr_str,0,16);
	printf(addr_str,"%d.%d.%d.%d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	return addr_str;
}

char* inet_ntoa_pad(unsigned long addr)
{
	static char addr_str[16];
	memset(addr_str,0,16);
	printf(addr_str,"%03d.%03d.%03d.%03d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	return addr_str;
}

/**
@brief	Converts a string containing an (Ipv4) Internet Protocol decimal dotted address into a 32bit address 
@return 	32bit address (Host Odering)
*/  
unsigned long inet_addr(
	unsigned char* addr	/**< dotted notation address string.  */
	)
{
	char i;
	u_long inetaddr = 0;
	char taddr[30];
	char * nexttok;
	int num;
	strcpy(taddr,(char*)addr);
	
	nexttok = taddr;
	for(i = 0; i < 4 ; i++)
	{
		nexttok = strtok(nexttok,".");
		if(nexttok[0] == '0' && nexttok[1] == 'x') num = ATOI(nexttok+2,0x10);
		else num = ATOI(nexttok,10);
		inetaddr = inetaddr << 8;		
		inetaddr |= (num & 0xFF);
		nexttok = NULL;
	}
	return inetaddr;	
}	

/**
@brief	Output destination IP address of appropriate channel
@return 	32bit destination address (Host Ordering)
*/ 
unsigned long GetDestAddr(
	SOCKET s	/**< Channel number which try to get destination IP Address */
	)
{
	u_long addr=0;
	int i = 0;
	for(i=0; i < 4; i++)
	{
		addr <<=8;
		addr += IINCHIP_READ(Sn_DIPR0(s)+i);
	}
	return addr;
}

/**
@brief	Output destination port number of appropriate channel
@return 	16bit destination port number
*/ 
unsigned int GetDestPort(
	SOCKET s	/**< Channel number which try to get destination port */
	)
{
	u_int port;
	port = ((u_int) IINCHIP_READ(Sn_DPORT0(s))) & 0x00FF;
	port <<= 8;
	port += ((u_int) IINCHIP_READ(Sn_DPORT0(s)+1)) & 0x00FF;
	return port;
}

u_short swaps(u_int i)
{
	u_short ret=0;
	ret = (i & 0xFF) << 8;
	ret |= ((i >> 8)& 0xFF);
	return ret;	
}

u_long swapl(u_long l)
{
	u_long ret=0;
	ret = (l & 0xFF) << 24;
	ret |= ((l >> 8) & 0xFF) << 16;
	ret |= ((l >> 16) & 0xFF) << 8;
	ret |= ((l >> 24) & 0xFF);
	return ret;
}

/**
@brief	htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).
@return 	the value in TCP/IP network byte order
*/ 
unsigned short htons( 
	unsigned short hostshort	/**< A 16-bit number in host byte order.  */
	)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
	return swaps(hostshort);
#else
	return hostshort;
#endif		
}


/**
@brief	htonl function converts a unsigned long from host to TCP/IP network byte order (which is big-endian).
@return 	the value in TCP/IP network byte order
*/ 
unsigned long htonl(
	unsigned long hostlong		/**< hostshort  - A 32-bit number in host byte order.  */
	)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
	return swapl(hostlong);
#else
	return hostlong;
#endif	
}


/**
@brief	ntohs function converts a unsigned short from TCP/IP network byte order to host byte order (which is little-endian on Intel processors).
@return 	a 16-bit number in host byte order
*/ 
unsigned long ntohs(
	unsigned short netshort	/**< netshort - network odering 16bit value */
	)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )	
	return htons(netshort);
#else
	return netshort;
#endif		
}


/**
@brief	converts a unsigned long from TCP/IP network byte order to host byte order (which is little-endian on Intel processors).
@return 	a 16-bit number in host byte order
*/ 
unsigned long ntohl(unsigned long netlong)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
	return htonl(netlong);
#else
	return netlong;
#endif		
}

// destip : BigEndian
u_char CheckDestInLocal(u_long destip)
{
	int i = 0;
	u_char * pdestip = (u_char*)&destip;
	for(i =0; i < 4; i++)
	{
		if((pdestip[i] & IINCHIP_READ(SUBR0+i)) != (IINCHIP_READ(SIPR0+i) & IINCHIP_READ(SUBR0+i)))
			return 1;	// Remote
	}
	return 0;
}


/**
@brief	Get handle of socket which status is same to 'status'
@return 	socket number
*/ 
SOCKET getSocket(
	unsigned char status, 	/**< socket's status to be found */
	SOCKET start			/**< base of socket to be found */
	)
{
	SOCKET i;
	if(start > 3) start = 0;

	for(i = start; i < MAX_SOCK_NUM ; i++) if( getSn_SR(i)==status ) return i;
	return MAX_SOCK_NUM;	
}


/**
@brief	Calculate checksum of a stream
@return 	checksum
*/ 
unsigned short checksum(
	unsigned char * src, 	/**< pointer to stream  */
	unsigned int len		/**< size of stream */
	)
{
	u_int sum, tsum, i, j;
	u_long lsum;

	j = len >> 1;

	lsum = 0;

	for (i = 0; i < j; i++) 
	{
		tsum = src[i * 2];
		tsum = tsum << 8;
		tsum += src[i * 2 + 1];
		lsum += tsum;
	}

	if (len % 2) 
	{
		tsum = src[i * 2];
		lsum += (tsum << 8);
	}


	sum = lsum;
	sum = ~(sum + (lsum >> 16));
	return (u_short) sum;	
}



#ifndef NO_USE_SOCKUTIL_FUNC
/**
@brief	Get Source IP Address of iinChip.
@return 	Source IP Address(32bit Address-Host Ordering)
*/ 
u_long GetIPAddress(void)
{
	u_long ip=0;
	int i;
	for(i=0; i < 4; i++)
	{
		ip <<= 8;
		ip += (char)IINCHIP_READ(SIPR0+i);
	}
	return ip;
}


/**
@brief	Get Gateway IP Address of iinChip.
@return 	Gateway IP Address(32bit Address-Host Ordering)
*/ 
u_long GetGWAddress(void)
{
	u_long ip=0;
	int i;
	for(i=0; i < 4; i++)
	{
		ip <<= 8;
		ip += (char)IINCHIP_READ(GAR0+i);
	}
	return ip;	
}


/**
@brief	Get Subnet mask of iinChip.
@return 	Subnet Mask(32bit Address-Host Ordering)
*/ 
u_long GetSubMask(void)
{
	u_long ip=0;
	int i;
	for(i=0; i < 4; i++)
	{
		ip <<= 8;
		ip += (char)IINCHIP_READ(SUBR0+i);
	}
	return ip;	
}


/**
@brief	Get Mac Address of iinChip.
@return 	Subnet Mask(32bit Address-Host Ordering)
*/ 
void GetMacAddress(
	unsigned char* mac	/**< Pointer to store Mac Address(48bit Address)(INPUT, OUTPUT) */
	)
{
	int i = 0;
	for(i=0; i<6;i++)*mac++ = IINCHIP_READ(SHAR0+i);
}

void GetDestMacAddr(SOCKET s, u_char* mac)
{
	int i = 0;
	for(i=0; i<6;i++)*mac++ = IINCHIP_READ(Sn_DHAR0(s)+i);
}


/**
@brief	Read established network information(G/W, IP, S/N, Mac) of iinChip and Output that through Serial.
		Mac Address is output into format of Dotted HexaDecimal.Others are output into format of Dotted Decimal Format.
*/ 
void GetNetConfig(void)
{
	u_char addr[6];
	u_long iaddr;
	printf("\r\n================================================\r\n");
	printf("       Net Config Information\r\n");
	printf("================================================\r\n");
	GetMacAddress(addr);
	printf("MAC ADDRESS      : 0x%02X.0x%02X.0x%02X.0x%02X.0x%02X.0x%02X\r\n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
	
	iaddr = GetSubMask();
	printf("SUBNET MASK      : %s\r\n",inet_ntoa(iaddr));

	iaddr = GetGWAddress();
	printf("G/W IP ADDRESS   : %s\r\n",inet_ntoa(iaddr));

	iaddr = GetIPAddress();
	printf("LOCAL IP ADDRESS : %s\r\n",inet_ntoa(iaddr));
	printf("================================================\r\n");		
}

#endif

