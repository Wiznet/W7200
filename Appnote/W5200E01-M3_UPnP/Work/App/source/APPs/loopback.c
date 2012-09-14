
#include "config.h"
#include "W5200\w5200.h"
#include "W5200\socket.h"
#include "util.h"
#include "APPs\loopback.h"
#include <stdio.h>

u8 ch_status[MAX_SOCK_NUM];

u8 data_buf[TX_RX_MAX_BUF_SIZE];

void loopback_tcps(u8 ch, u16 port)
{
	int ret;
	int SendLen, ReSendLen;

	ret = TCPRecv(ch, data_buf, TX_RX_MAX_BUF_SIZE);

	if(ret > 0){				// Received
		SendLen = TCPSend(ch, data_buf, ret);

		while(SendLen < ret){
			ReSendLen = TCPReSend(ch);

			if(ReSendLen > 0){
				SendLen += ReSendLen;

			} else if(ReSendLen == ERROR_WINDOW_FULL){
				printf("Window Full!!\r\n");
				TCPClose(ch);
				printf("TCP Socket Close!!\r\n");
				while(1);

			} else{
				break;
			}
		}

	} else if(ret == ERROR_NOT_TCP_SOCKET){	// Not TCP Socket, It's UDP Socket
		UDPClose(ch);

	} else if(ret == ERROR_CLOSED){		// Socket Closed
		printf("\r\n%d : Loop-Back TCP Started.\r\n",(u16)ch);
		TCPServerOpen(ch, port);

	}

	if(GetTCPSocketStatus(ch) == STATUS_CLOSE_WAIT){	// Close waiting
		TCPClose(ch);
	}
}

void loopback_udp(u8 ch, u16 port)
{
	int ret;
	u32 destip = 0;
	u16 destport;

	ret = UDPRecv(ch, data_buf, TX_RX_MAX_BUF_SIZE, (u8*)&destip, &destport);

	if(ret > 0){				// Received
		ret = UDPSend(ch, data_buf, ret, (u8*)&destip ,destport);

		if(ret == ERROR_TIME_OUT){
			printf("Timeout!!\r\n");
			UDPClose(ch);
			printf("UDP Socket Close!!\r\n");
		}

	} else if(ret == ERROR_NOT_UDP_SOCKET){	// Not UDP Socket, It's TCP Socket
		TCPClose(ch);

	} else if(ret == ERROR_CLOSED){		// Socket Closed
		printf("\r\n%d : Loop-Back UDP Started.\r\n",(u16)ch);
		UDPOpen(ch, port);

	}
}
