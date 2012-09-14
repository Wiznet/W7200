/*
*
@file		util.h
@brief	
*/

#ifndef _UTIL_H
#define _UTIL_H

void Set_network();
void Reset_W5200(void);

void LED3_onoff(uint8 on_off);
void LED4_onoff(uint8 on_off);

void USART1_Init(void);

void Delay_us(uint8 time_us);
void Delay_ms(uint16 time_ms);

int putchar(int ch);
int getchar(void);

extern u_int ATOI(char* str,u_int base); 			/* Convert a string to integer number */
extern int ValidATOI(char* str, int base, int* ret); 		/* Verify character string and Convert it to (hexa-)decimal. */
extern char C2D(u_char c); 					/* Convert a character to HEX */

extern u_short swaps(u_int i);
extern u_long swapl(u_long l);

#endif
