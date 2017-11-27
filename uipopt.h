#ifndef __UIPOPT_H__
#define __UIPOPT_H__

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

#include "uip_conf.h"

#define UIP_FIXEDADDR    0


#ifdef UIP_CONF_PINGADDRCONF
#define UIP_PINGADDRCONF UIP_CONF_PINGADDRCONF
#else /* UIP_CONF_PINGADDRCONF */
#define UIP_PINGADDRCONF 0
#endif /* UIP_CONF_PINGADDRCONF */

#define UIP_FIXEDETHADDR 0
#define UIP_TTL         64
#define UIP_REASSEMBLY 0
#define UIP_REASS_MAXAGE 40

#ifdef UIP_CONF_UDP
#define UIP_UDP UIP_CONF_UDP
#else /* UIP_CONF_UDP */
#define UIP_UDP           0
#endif /* UIP_CONF_UDP */

/**
 * Toggles if UDP checksums should be used or not.
 *
 * \note Support for UDP checksums is currently not included in uIP,
 * so this option has no function.
 *
 * \hideinitializer
 */
#ifdef UIP_CONF_UDP_CHECKSUMS
#define UIP_UDP_CHECKSUMS UIP_CONF_UDP_CHECKSUMS
#else
#define UIP_UDP_CHECKSUMS 0
#endif

/**
 * The maximum amount of concurrent UDP connections.
 *
 * \hideinitializer
 */
#ifdef UIP_CONF_UDP_CONNS
#define UIP_UDP_CONNS UIP_CONF_UDP_CONNS
#else /* UIP_CONF_UDP_CONNS */
#define UIP_UDP_CONNS    10
#endif /* UIP_CONF_UDP_CONNS */

#define UIP_ACTIVE_OPEN 1

/**
 * The maximum number of simultaneously open TCP connections.
 *
 * Since the TCP connections are statically allocated, turning this
 * configuration knob down results in less RAM used. Each TCP
 * connection requires approximatly 30 bytes of memory.
 *
 * \hideinitializer
 */
#ifndef UIP_CONF_MAX_CONNECTIONS
#define UIP_CONNS       10
#else /* UIP_CONF_MAX_CONNECTIONS */
#define UIP_CONNS UIP_CONF_MAX_CONNECTIONS
#endif /* UIP_CONF_MAX_CONNECTIONS */

#ifndef UIP_CONF_MAX_LISTENPORTS
#define UIP_LISTENPORTS 20
#else /* UIP_CONF_MAX_LISTENPORTS */
#define UIP_LISTENPORTS UIP_CONF_MAX_LISTENPORTS
#endif /* UIP_CONF_MAX_LISTENPORTS */


#define UIP_URGDATA      0
#define UIP_RTO         3
#define UIP_MAXRTX      8

/**
 * The maximum number of times a SYN segment should be retransmitted
 * before a connection request should be deemed to have been
 * unsuccessful.
 *
 * This should not need to be changed.
 */
#define UIP_MAXSYNRTX      5

/**
 * The TCP maximum segment size.
 *
 * This is should not be to set to more than
 * UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN.
 */
#ifndef UIP_CONF_TCP_MSS
#define UIP_TCP_MSS     (UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN)
#else
#define UIP_TCP_MSS UIP_CONF_TCP_MSS
#endif

/**
 * The size of the advertised receiver's window.
 *
 * Should be set low (i.e., to the size of the uip_buf buffer) is the
 * application is slow to process incoming data, or high (32768 bytes)
 * if the application processes data quickly.
 *
 * \hideinitializer
 */
#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_RECEIVE_WINDOW UIP_TCP_MSS
#else
#define UIP_RECEIVE_WINDOW UIP_CONF_RECEIVE_WINDOW
#endif

/**
 * How long a connection should stay in the TIME_WAIT state.
 *
 * This configiration option has no real implication, and it should be
 * left untouched.
 */
#define UIP_TIME_WAIT_TIMEOUT 120


/** @} */
/*------------------------------------------------------------------------------*/
/**
 * \name ARP configuration options
 * @{
 */

/**
 * The size of the ARP table.
 *
 * This option should be set to a larger value if this uIP node will
 * have many connections from the local network.
 *
 * \hideinitializer
 */
#ifdef UIP_CONF_ARPTAB_SIZE
#define UIP_ARPTAB_SIZE UIP_CONF_ARPTAB_SIZE
#else
#define UIP_ARPTAB_SIZE 8
#endif

/**
 * The maxium age of ARP table entries measured in 10ths of seconds.
 *
 * An UIP_ARP_MAXAGE of 120 corresponds to 20 minutes (BSD
 * default).
 */
#define UIP_ARP_MAXAGE 120


#ifndef UIP_CONF_BUFFER_SIZE
#define UIP_BUFSIZE     400
#else /* UIP_CONF_BUFFER_SIZE */
#define UIP_BUFSIZE UIP_CONF_BUFFER_SIZE
#endif /* UIP_CONF_BUFFER_SIZE */


/**
 * Determines if statistics support should be compiled in.
 *
 * The statistics is useful for debugging and to show the user.
 *
 * \hideinitializer
 */
#ifndef UIP_CONF_STATISTICS
#define UIP_STATISTICS  0
#else /* UIP_CONF_STATISTICS */
#define UIP_STATISTICS UIP_CONF_STATISTICS
#endif /* UIP_CONF_STATISTICS */

/**
 * Determines if logging of certain events should be compiled in.
 *
 * This is useful mostly for debugging. The function uip_log()
 * must be implemented to suit the architecture of the project, if
 * logging is turned on.
 *
 * \hideinitializer
 */
#ifndef UIP_CONF_LOGGING
#define UIP_LOGGING     0
#else /* UIP_CONF_LOGGING */
#define UIP_LOGGING     UIP_CONF_LOGGING
#endif /* UIP_CONF_LOGGING */

/**
 * Broadcast support.
 *
 * This flag configures IP broadcast support. This is useful only
 * together with UDP.
 *
 * \hideinitializer
 *
 */
#if UIP_UDP && UIP_CONF_BROADCAST
#define UIP_BROADCAST UIP_CONF_BROADCAST
#else /* UIP_CONF_BROADCAST */
#define UIP_BROADCAST 0
#endif /* UIP_CONF_BROADCAST */


void uip_log(char *msg);

#ifdef UIP_CONF_LLH_LEN
#define UIP_LLH_LEN UIP_CONF_LLH_LEN
#else /* UIP_CONF_LLH_LEN */
#define UIP_LLH_LEN     14
#endif /* UIP_CONF_LLH_LEN */


#ifdef UIP_CONF_BYTE_ORDER
#define UIP_BYTE_ORDER     UIP_CONF_BYTE_ORDER
#else /* UIP_CONF_BYTE_ORDER */
#define UIP_BYTE_ORDER     UIP_LITTLE_ENDIAN
#endif /* UIP_CONF_BYTE_ORDER */


#endif /* __UIPOPT_H__ */



