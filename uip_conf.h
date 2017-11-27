#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <inttypes.h>
#include "uipethernet_conf.h"

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef unsigned short uip_stats_t;

#define UIP_CONF_MAX_LISTENPORTS 4
#define UIP_CONF_BUFFER_SIZE     98
#define UIP_CONF_TCP_MSS 512
#define UIP_CONF_RECEIVE_WINDOW 512
#define UIP_CONF_BYTE_ORDER      LITTLE_ENDIAN
#define UIP_CONF_LOGGING         0
#define UIP_CONF_UDP_CHECKSUMS   1
#define UIP_CONF_STATISTICS      0

typedef void* uip_tcp_appstate_t;

void uipclient_appcall(void);

#define UIP_APPCALL uipclient_appcall

typedef void* uip_udp_appstate_t;

void uipudp_appcall(void);

#define UIP_UDP_APPCALL uipudp_appcall

#define CC_REGISTER_ARG register

#define UIP_ARCH_CHKSUM 1

#endif



