#include "stm32f10x.h"
#include "config.h"
#include "util.h"
#include "W5200\w5200.h"
#include <stdio.h>
#include <stdarg.h>


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


void Reset_W5200(void)
{

	GPIO_ResetBits(GPIOB, WIZ_RESET);
	Delay_us(8); // 
	GPIO_SetBits(GPIOB, WIZ_RESET);
	Delay_ms(50); // 

}


void LED3_onoff(uint8_t on_off)
{
#ifdef __DEF_W5200__
	GPIO_TypeDef* GPIOx = GPIOA;
#endif
#ifdef __DEF_W7200__
	GPIO_TypeDef* GPIOx = GPIOB;
#endif

	if (on_off == ON) {
		GPIO_ResetBits(GPIOx, LED3); // LED on
	}else {
		GPIO_SetBits(GPIOx, LED3); // LED off
	}
}

void LED4_onoff(uint8_t on_off)
{
#ifdef __DEF_W5200__
	GPIO_TypeDef* GPIOx = GPIOA;
#endif
#ifdef __DEF_W7200__
	GPIO_TypeDef* GPIOx = GPIOB;
#endif

	if (on_off == ON) {
		GPIO_ResetBits(GPIOx, LED4); // LED on
	}else {
		GPIO_SetBits(GPIOx, LED4); // LED off
	}
}

void USART1_Init(void)
{
USART_InitTypeDef USART_InitStructure;

/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure the USARTx */ 
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);


}


/*******************************************************************************
* Function Name  : Delay_us
* Description    : Delay per micro second.
* Input          : time_us
* Output         : None
* Return         : None
*******************************************************************************/

void Delay_us( u8 time_us )
{
  register u8 i;
  register u8 j;
  for( i=0;i<time_us;i++ )    
  {
    for( j=0;j<5;j++ )          // 25CLK
    {
      asm("nop");       //1CLK         
      asm("nop");       //1CLK         
      asm("nop");       //1CLK         
      asm("nop");       //1CLK         
      asm("nop");       //1CLK                  
    }      
  }                              // 25CLK*0.04us=1us
}


/*******************************************************************************
* Function Name  : Delay_ms
* Description    : Delay per mili second.
* Input          : time_ms
* Output         : None
* Return         : None
*******************************************************************************/

void Delay_ms( u16 time_ms )
{
  register u16 i;
  for( i=0;i<time_ms;i++ )
  {
    Delay_us(250);
    Delay_us(250);
    Delay_us(250);
    Delay_us(250);
  }
}


/*******************************************************************************
* Function Name  : PUTCHAR_PROTOTYPE
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
PUTCHAR_PROTOTYPE
{
  // Write a character to the USART
  USART_SendData(USART1, (uint8_t) ch);

  //  Loop until the end of transmission 
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }

  return ch;
}
*/

int putchar(int ch)
{
  // Write a character to the USART
  USART_SendData(USART1, (uint8_t) ch);

  //  Loop until the end of transmission 
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }

  return ch;
}

int getchar(void)
{
  int ch;

	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET){
	}

	ch = USART_ReceiveData(USART1);

  return ch;
}



#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



/**
@brief	CONVERT STRING INTO INTEGER
@return	a integer number
*/
u_int ATOI(
	char* str,	/**< is a pointer to convert */
	u_int base	/**< is a base value (must be in the range 2 - 16) */
	)
{
        unsigned int num = 0;
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

/**
@brief	CONVERT CHAR INTO HEX
@return	HEX
  
This function converts HEX(0-F) to a character
*/
char C2D(
	u_char c	/**< is a character('0'-'F') to convert to HEX */
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



