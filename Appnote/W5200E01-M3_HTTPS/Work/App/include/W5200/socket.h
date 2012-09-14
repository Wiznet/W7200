/*
*
@file		socket.h
@brief	define function of socket API 
*
*/

#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "Types.h"

#define STATUS_CLOSED		-1
#define STATUS_INIT		0
#define STATUS_LISTEN		1
#define STATUS_SYNSENT		2
#define STATUS_SYNRECV		3
#define STATUS_ESTABLISHED	4
#define STATUS_FIN_WAIT		5
#define STATUS_CLOSING		6
#define STATUS_TIME_WAIT	7
#define STATUS_CLOSE_WAIT	8
#define STATUS_LAST_ACK		9
#define STATUS_UDP		10

#define ERROR_NOT_TCP_SOCKET	-1
#define ERROR_NOT_UDP_SOCKET	-2
#define ERROR_CLOSED		-3
#define ERROR_NOT_ESTABLISHED	-4
#define ERROR_FIN_WAIT		-5
#define ERROR_CLOSE_WAIT	-6
#define ERROR_WINDOW_FULL	-7
#define ERROR_TIME_OUT		-8

#define SUCCESS	1
#define FAIL	0


typedef struct _wiz_NetInfo
{
	uint8 Mac[6];
	uint8 IP[4];
	uint8 Subnet[4];
	uint8 Gateway[4];
	uint8 DNSServerIP[4];
	uint8 DHCPEnable;

} wiz_NetInfo;

extern void wizInit();
extern void wizSWReset();
extern void wizMemInit(uint8 * tx_size, uint8 * rx_size);
extern void SetNetInfo(wiz_NetInfo *netinfo);
extern void GetNetInfo(wiz_NetInfo *netinfo);

extern void SetSocketOption(uint8 option_type, uint16 option_value);
extern int8 GetTCPSocketStatus(SOCKET s);
extern int8 GetUDPSocketStatus(SOCKET s);

extern int8 TCPServerOpen(SOCKET s, uint16 port);
extern int8 TCPClientOpen(SOCKET s, uint16 port, uint8 * destip, uint16 destport);
extern int16 TCPSend(SOCKET s, const uint8 * src, uint16 len);
extern int16 TCPReSend(SOCKET s);
extern int16 TCPRecv(SOCKET s, uint8 * buf, uint16 len);
extern int8 TCPClose(SOCKET s);

extern int8 UDPOpen(SOCKET s, uint16 port);
extern int16 UDPSend(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port);
extern int16 UDPRecv(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16 *port);
extern int8 UDPClose(SOCKET s);

extern uint8 GetInterrupt(SOCKET s);
extern void PutInterrupt(SOCKET s, uint8 vector);
extern uint16 GetSocketTxFreeBufferSize(SOCKET s);
extern uint16 GetSocketRxRecvBufferSize(SOCKET s);


#endif
/* _SOCKET_H_ */
