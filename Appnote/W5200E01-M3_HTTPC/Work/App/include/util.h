/*
*
@file		util.h
@brief	
*/

#ifndef _UTIL_H
#define _UTIL_H

//#define PERIPH_BASE           ((u32)0x40000000)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define USART1_BASE           (APB2PERIPH_BASE + 0x3800)
#define	UART1_CR1		(USART1_BASE + 0x0C)
#define	XMEM(addr)		( *(vu32 *)addr)
#define	UART1_TX_ON		(XMEM(UART1_CR1) |= 0x0040)
#define	UART1_TX_OFF		(XMEM(UART1_CR1) &= ~0x0040)
#define	UART1_RX_ON		(XMEM(UART1_CR1) |= 0x0020)
#define	UART1_RX_OFF		(XMEM(UART1_CR1) &= ~0x0020)
#define	UART1_SR		(USART1_BASE + 0x00)
#define	UART1_DR		(USART1_BASE + 0x04)

#define	KEYBOARD_BACK		0x08
#define	KEYBOARD_CarryRet	0x0D
#define	KEYBOARD_SPACE	        0x20

#define	SIZE_UART1TX_BUFF			0x0100		// 256 Bytes
#define	SIZE_UART1RX_BUFF			0x0100		// 256 Bytes

extern uint8	gau8Uart1TxBuff[SIZE_UART1TX_BUFF];
extern uint8	gu8Uart1TxOperating;
extern uint16	gu16Uart1TxWrIdx; 
extern uint16	gu16Uart1TxRdIdx;

extern uint8	gau8uart1RxBuff[SIZE_UART1RX_BUFF];	
extern uint16	gu16Uart1RxWrIdx;
extern uint16	gu16Uart1RxRdIdx;


void Reset_W5200(void);


void LED3_onoff(uint8 on_off);
void LED4_onoff(uint8 on_off);


//void USART1_Init(void);
extern void USART1_Init(unsigned long u32BaudRate);

void Delay_us(uint8 time_us);
void Delay_ms(uint16 time_ms);

int putchar(int ch);
int getchar(void);


uint32 time_return(void);

extern u_int ATOI(char* str,u_int base); 			/* Convert a string to integer number */
extern int ValidATOI(char* str, int base, int* ret); 		/* Verify character string and Convert it to (hexa-)decimal. */
extern char C2D(u_char c); 					/* Convert a character to HEX */

extern u_short swaps(u_int i);
extern u_long swapl(u_long l);
extern void replacetochar(char * str, char oldchar, char newchar);
extern uint8 zScanf_s(uint8 u8UartPort, uint8* strptr);
extern uint8 Uart1Get(uint8* pu8Data);
extern void Uart1Put(uint8 u8Data);


#endif
