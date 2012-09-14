/*
 * (c)COPYRIGHT
 * ALL RIGHT RESERVED
 *
 * FileName : w5200.c
  * -----------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "W5200\spi1.h"
#include "W5200\w5200.h"
#include "W5200\socket.h"

static uint8 I_STATUS[MAX_SOCK_NUM];
uint16 SMASK[MAX_SOCK_NUM]; /**< Variable for Tx buffer MASK in each channel */
uint16 RMASK[MAX_SOCK_NUM]; /**< Variable for Rx buffer MASK in each channel */
uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */
uint16 SBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Tx buffer base address by each channel */
uint16 RBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Rx buffer base address by each channel */

uint8 windowfull_retry_cnt[MAX_SOCK_NUM];

uint8 incr_windowfull_retry_cnt(uint8 s)
{
  return windowfull_retry_cnt[s]++;
}

void init_windowfull_retry_cnt(uint8 s)
{
  windowfull_retry_cnt[s] = 0;
}

uint16 pre_sent_ptr, sent_ptr;

uint8 getISR(uint8 s)
{
  return I_STATUS[s];
}
void putISR(uint8 s, uint8 val)
{
   I_STATUS[s] = val;
}
uint16 getIINCHIP_RxMAX(uint8 s)
{
   return RSIZE[s];
}
uint16 getIINCHIP_TxMAX(uint8 s)
{
   return SSIZE[s];
}
uint16 getIINCHIP_RxMASK(uint8 s)
{
   return RMASK[s];
}
uint16 getIINCHIP_TxMASK(uint8 s)
{
   return SMASK[s];
}
uint16 getIINCHIP_RxBASE(uint8 s)
{
   return RBUFBASEADDRESS[s];
}
uint16 getIINCHIP_TxBASE(uint8 s)
{
   return SBUFBASEADDRESS[s];
}
void IINCHIP_CSoff(void)
{
  WIZ_CS(LOW);
}
void IINCHIP_CSon(void)
{
  WIZ_CS(HIGH);
}
u8  IINCHIP_SpiSendData(uint8 dat)
{
  return(SPI1_SendByte(dat));
}


 /**
@brief  This function writes the data into W5200 registers.
*/

uint8 IINCHIP_WRITE(uint16 addr,uint8 data)
{
  IINCHIP_ISR_DISABLE();                      // Interrupt Service Routine Disable

  //SPI MODE I/F
  IINCHIP_CSoff();                            // CS=0, SPI start

  IINCHIP_SpiSendData((addr & 0xFF00) >> 8);  // Address byte 1
  IINCHIP_SpiSendData(addr & 0x00FF);         // Address byte 2
  IINCHIP_SpiSendData(0x80);                  // Data write command and Write data length 1
  IINCHIP_SpiSendData(0x01);                  // Write data length 2
  IINCHIP_SpiSendData(data);                  // Data write (write 1byte data)

  IINCHIP_CSon();                             // CS=1,  SPI end

  IINCHIP_ISR_ENABLE();                       // Interrupt Service Routine Enable
  return 1;
}
/**
@brief  This function reads the value from W5200 registers.
*/
uint8 IINCHIP_READ(uint16 addr)
{
  uint8 data;
        
  IINCHIP_ISR_DISABLE();                       // Interrupt Service Routine Disable
  
  IINCHIP_CSoff();                             // CS=0, SPI start
  
  IINCHIP_SpiSendData((addr & 0xFF00) >> 8);   // Address byte 1
  IINCHIP_SpiSendData(addr & 0x00FF);          // Address byte 2
  IINCHIP_SpiSendData(0x00);                   // Data read command and Read data length 1
  IINCHIP_SpiSendData(0x01);                   // Read data length 2    
  data = IINCHIP_SpiSendData(0x00);            // Data read (read 1byte data)
  
  IINCHIP_CSon();                              // CS=1,  SPI end
  
  IINCHIP_ISR_ENABLE();                        // Interrupt Service Routine Enable
  return data;
}

/**
@brief  This function writes into W5200 memory(Buffer)
*/ 
uint16 IINCHIP_WRITE_BLOCK(uint16 addr,uint8* buf,uint16 len)
{
  uint16 idx = 0;

  if(len == 0)
    return 0;

  IINCHIP_ISR_DISABLE();

  //SPI MODE I/F
  IINCHIP_CSoff();                                        // CS=0, SPI start 
  
  IINCHIP_SpiSendData(((addr+idx) & 0xFF00) >> 8);        // Address byte 1
  IINCHIP_SpiSendData((addr+idx) & 0x00FF);               // Address byte 2
  IINCHIP_SpiSendData((0x80 | ((len & 0x7F00) >> 8)));    // Data write command and Write data length 1
  IINCHIP_SpiSendData((len & 0x00FF));                    // Write data length 2
  for(idx = 0; idx < len; idx++)                          // Write data in loop
  {   
    IINCHIP_SpiSendData(buf[idx]);
  }
  
  IINCHIP_CSon();                                         // CS=1, SPI end 
        
  IINCHIP_ISR_ENABLE();                                   // Interrupt Service Routine Enable        
  return len;
}


/**
@brief  This function reads into W5200 memory(Buffer)
*/ 
uint16 IINCHIP_READ_BLOCK(uint16 addr, uint8* buf,uint16 len)
{
  uint16 idx = 0;
        
  IINCHIP_ISR_DISABLE();                                  // Interrupt Service Routine Disable
        
  IINCHIP_CSoff();                                        // CS=0, SPI start 
        
  IINCHIP_SpiSendData(((addr+idx) & 0xFF00) >> 8);        // Address byte 1
  IINCHIP_SpiSendData((addr+idx) & 0x00FF);               // Address byte 2
  IINCHIP_SpiSendData((0x00 | ((len & 0x7F00) >> 8)));    // Data read command
  IINCHIP_SpiSendData((len & 0x00FF));            

  for(idx = 0; idx < len; idx++)                          // Read data in loop
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
    
  }
        
  IINCHIP_CSon();                                         // CS=0, SPI end      
        
  IINCHIP_ISR_ENABLE();                                   // Interrupt Service Routine Enable
  return len;
}


/**
@brief  This function sets up gateway IP address.
*/ 
void setGAR(
  uint8 * addr  /**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
  )
{
  IINCHIP_WRITE((GAR0 + 0),addr[0]);
  IINCHIP_WRITE((GAR0 + 1),addr[1]);
  IINCHIP_WRITE((GAR0 + 2),addr[2]);
  IINCHIP_WRITE((GAR0 + 3),addr[3]);
}

/*
void getGWIP(uint8 * addr)
{
  addr[0] = IINCHIP_READ((GAR0 + 0));
  addr[1] = IINCHIP_READ((GAR0 + 1));
  addr[2] = IINCHIP_READ((GAR0 + 2));
  addr[3] = IINCHIP_READ((GAR0 + 3));
}
*/

/**
@brief  It sets up SubnetMask address
*/ 
void setSUBR(
  uint8 * addr  /**< a pointer to a 4 -byte array responsible to set the SubnetMask address */
  )
{
  IINCHIP_WRITE((SUBR0 + 0),addr[0]);
  IINCHIP_WRITE((SUBR0 + 1),addr[1]);
  IINCHIP_WRITE((SUBR0 + 2),addr[2]);
  IINCHIP_WRITE((SUBR0 + 3),addr[3]);
}


/**
@brief  This function sets up MAC address.
*/ 
void setSHAR(
  uint8 * addr  /**< a pointer to a 6 -byte array responsible to set the MAC address. */
  )
{
  IINCHIP_WRITE((SHAR0 + 0),addr[0]);
  IINCHIP_WRITE((SHAR0 + 1),addr[1]);
  IINCHIP_WRITE((SHAR0 + 2),addr[2]);
  IINCHIP_WRITE((SHAR0 + 3),addr[3]);
  IINCHIP_WRITE((SHAR0 + 4),addr[4]);
  IINCHIP_WRITE((SHAR0 + 5),addr[5]);
}

/**
@brief  This function sets up Source IP address.
*/
void setSIPR(
  uint8 * addr  /**< a pointer to a 4 -byte array responsible to set the Source IP address. */
  )
{
  IINCHIP_WRITE((SIPR0 + 0),addr[0]);
  IINCHIP_WRITE((SIPR0 + 1),addr[1]);
  IINCHIP_WRITE((SIPR0 + 2),addr[2]);
  IINCHIP_WRITE((SIPR0 + 3),addr[3]);
}

/**
@brief  This function sets up Source IP address.
*/
void getGAR(uint8 * addr)
{
  addr[0] = IINCHIP_READ(GAR0);
  addr[1] = IINCHIP_READ(GAR0+1);
  addr[2] = IINCHIP_READ(GAR0+2);
  addr[3] = IINCHIP_READ(GAR0+3);
}
void getSUBR(uint8 * addr)
{
  addr[0] = IINCHIP_READ(SUBR0);
  addr[1] = IINCHIP_READ(SUBR0+1);
  addr[2] = IINCHIP_READ(SUBR0+2);
  addr[3] = IINCHIP_READ(SUBR0+3);
}
void getSHAR(uint8 * addr)
{
  addr[0] = IINCHIP_READ(SHAR0);
  addr[1] = IINCHIP_READ(SHAR0+1);
  addr[2] = IINCHIP_READ(SHAR0+2);
  addr[3] = IINCHIP_READ(SHAR0+3);
  addr[4] = IINCHIP_READ(SHAR0+4);
  addr[5] = IINCHIP_READ(SHAR0+5);
}
void getSIPR(uint8 * addr)
{
  addr[0] = IINCHIP_READ(SIPR0);
  addr[1] = IINCHIP_READ(SIPR0+1);
  addr[2] = IINCHIP_READ(SIPR0+2);
  addr[3] = IINCHIP_READ(SIPR0+3);
}

void setMR(uint8 val)
{
  IINCHIP_WRITE(MR,val);
}

/**
@brief  This function gets Interrupt register in common register.
 */
uint8 getIR( void )
{
   return IINCHIP_READ(IR);
}


/**
 Retransmittion 
 **/
 
/**
@brief  This function sets up Retransmission time.

If there is no response from the peer or delay in response then retransmission 
will be there as per RTR (Retry Time-value Register)setting
*/
void setRTR(uint16 timeout)
{
  IINCHIP_WRITE(RTR,(uint8)((timeout & 0xff00) >> 8));
  IINCHIP_WRITE((RTR + 1),(uint8)(timeout & 0x00ff));
}

/**
@brief  This function set the number of Retransmission.

If there is no response from the peer or delay in response then recorded time 
as per RTR & RCR register seeting then time out will occur.
*/
void setRCR(uint8 retry)
{
  IINCHIP_WRITE(RCR,retry);
}




/**
@brief  This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
set in IR register.
*/
void setIMR(uint8 mask)
{
  IINCHIP_WRITE(IMR,mask); // must be setted 0x10.
}

/**
@brief  This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*/
void setSn_MSS(SOCKET s, uint16 Sn_MSSR0)
{
  IINCHIP_WRITE(Sn_MSSR0(s),(uint8)((Sn_MSSR0 & 0xff00) >> 8));
  IINCHIP_WRITE((Sn_MSSR0(s) + 1),(uint8)(Sn_MSSR0 & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8 ttl)
{
   IINCHIP_WRITE(Sn_TTL(s), ttl);
}


/**
@brief  These below function is used to setup the Protocol Field of IP Header when
    executing the IP Layer RAW mode.
*/
void setSn_PROTO(SOCKET s, uint8 proto)
{
  IINCHIP_WRITE(Sn_PROTO(s),proto);
}


/**
@brief  get socket interrupt status

These below functions are used to read the Interrupt & Soket Status register
*/
uint8 getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}


/**
@brief   get socket status
*/
uint8 getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief  get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16 getSn_TX_FSR(SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_TX_FSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
      if (val1 != 0)
    {
        val = IINCHIP_READ(Sn_TX_FSR0(s));
        val = (val << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
    }
  } while (val != val1);
   return val;
}


/**
@brief   get socket RX recv buf size

This gives size of received data in receive buffer. 
*/
uint16 getSn_RX_RSR(SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_RX_RSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
      if(val1 != 0)
    {
        val = IINCHIP_READ(Sn_RX_RSR0(s));
        val = (val << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
    }
  } while (val != val1);
   return val;
}


/**
@brief  This function is being called by send() and sendto() function also. for copy the data form application buffer to Transmite buffer of the chip.

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
And this function is being used for copy the data form application buffer to Transmite
buffer of the chip. It calculate the actual physical address where one has to write
the data in transmite buffer. Here also take care of the condition while it exceed
the Tx memory uper-bound of socket.

*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len)
{
  
  uint16 ptr;
  uint16 size;
  uint16 dst_mask;
  uint8 * dst_ptr;

  ptr = IINCHIP_READ(Sn_TX_WR0(s));
  ptr = (ptr << 8) + IINCHIP_READ(Sn_TX_WR0(s) + 1);

  dst_mask = (uint32)ptr & getIINCHIP_TxMASK(s);
  dst_ptr = (uint8 *)(getIINCHIP_TxBASE(s) + dst_mask);
  
  if (dst_mask + len > getIINCHIP_TxMAX(s)) 
  {
    size = getIINCHIP_TxMAX(s) - dst_mask;
    IINCHIP_WRITE_BLOCK((uint32)dst_ptr, (uint8*)data, size);
    data += size;
    size = len - size;
    dst_ptr = (uint8 *)(getIINCHIP_TxBASE(s));
    IINCHIP_WRITE_BLOCK((uint32)dst_ptr, (uint8*)data, size);
  } 
  else
  {
    IINCHIP_WRITE_BLOCK((uint32)dst_ptr, (uint8*)data, len);
  }

  ptr += len;

  IINCHIP_WRITE(Sn_TX_WR0(s),(uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE((Sn_TX_WR0(s) + 1),(uint8)(ptr & 0x00ff));
  
}


/**
@brief  This function is being called by recv() also. This function is being used for copy the data form Receive buffer of the chip to application buffer.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
It calculate the actual physical address where one has to read
the data from Receive buffer. Here also take care of the condition while it exceed
the Rx memory uper-bound of socket.
*/
void recv_data_processing(SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr;
  uint16 size;
  uint16 src_mask;
  uint8 * src_ptr;

  ptr = IINCHIP_READ(Sn_RX_RD0(s));
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(s) + 1);
  
#ifdef __DEF_IINCHIP_DBG__
  printf(" ISR_RX: rd_ptr : %.4x\r\n", ptr);
#endif

  src_mask = (uint32)ptr & getIINCHIP_RxMASK(s);
  src_ptr = (uint8 *)(getIINCHIP_RxBASE(s) + src_mask);
  
  if( (src_mask + len) > getIINCHIP_RxMAX(s) ) 
  {
    size = getIINCHIP_RxMAX(s) - src_mask;
    IINCHIP_READ_BLOCK((uint32)src_ptr, (uint8*)data,size);
    data += size;
    size = len - size;
    src_ptr = (uint8 *)(getIINCHIP_RxBASE(s));
    IINCHIP_READ_BLOCK((uint32)src_ptr, (uint8*) data,size);
  } 
  else
  {
    IINCHIP_READ_BLOCK((uint32)src_ptr, (uint8*) data,len);
  }
    
  ptr += len;
  IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8)(ptr & 0x00ff));
}
