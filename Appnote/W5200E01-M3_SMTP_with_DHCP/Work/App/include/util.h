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
void USART2_Init(void);

void Delay_us(uint8 time_us);
void Delay_ms(uint16 time_ms);

int putchar(int ch);
int getchar(void);
int putchar2(int ch);
int getchar2(void);

#endif
