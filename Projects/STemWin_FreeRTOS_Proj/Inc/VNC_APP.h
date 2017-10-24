#ifndef __VNC_APP_H
#define __VNC_APP_H

#include "main.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "ethernetif.h"
#include "lwip/opt.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "vnc_app.h"
#include "ethernetif.h"
#include "GUI_VNC.h"

#define VNC_IDLE                   (uint8_t) 0
#define VNC_INIT                   (uint8_t) 9
#define VNC_LINK_UP                (uint8_t) 1
#define VNC_WAIT_FOR_ADDRESS       (uint8_t) 2
#define VNC_ADDRESS_ASSIGNED       (uint8_t) 3
#define VNC_ERROR                  (uint8_t) 4
#define VNC_LINK_DOWN              (uint8_t) 5
#define VNC_PROCESS                (uint8_t) 6
#define VNC_START                  (uint8_t) 7
#define VNC_CONN_ESTABLISHED       (uint8_t) 8
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   2
#define IP_ADDR3   105

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   2
#define GW_ADDR3   1 

void VNC_SERVER_Start(void);

#endif /* __VNC_APP_H */
