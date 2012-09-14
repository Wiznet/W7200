#ifndef __HTTP_CLIENT_H
#define __HTTP_CLIENT_H
#include "Apps\types.h"
//int16 http_client(uint8 s, uint8 *HTTPs_IP, uint8 *url_path, uint8 * data_buf );
int16 http_client( uint8 s, uint8 *HTTPs_IP, uint8 *url_path,uint8 *url_dn, uint8 * data_buf);
#endif
