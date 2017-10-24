/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/app_ethernet.c 
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    22-April-2016 
  * @brief   Ethernet specefic module
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "VNC_APP.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define MAX_DHCP_TRIES  4
struct netif gnetif; /* network interface structure */
__IO uint8_t VNC_State = VNC_IDLE;
osThreadId  VNC_ThreadId = 0;
static GUI_VNC_CONTEXT    _Context;
__IO uint8_t VNC_LockState = 1;
static struct sockaddr_in _Addr;
int _Sock;
static int Connection_accepted = 0;
/* Private function prototypes -----------------------------------------------*/
static void Netif_Config(void);
void VNC_Thread(void const * argument);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Called by the server to send data
  * @param  buf: buffer to be sent.
  * @param  len: length of buf.
  * @param  pConnectionInfo: Connection info  
  * @retval transmit status.
  */
static int _Send(const U8 * buf, int len, void * pConnectionInfo) {
  return ( send((long)pConnectionInfo, (const char *)buf, len, 0));
}

/**
  * @brief  Called by the server when data is received
  * @param  buf: buffer to get the received data.
  * @param  len: length of received data.
  * @param  pConnectionInfo: Connection info  
  * @retval receive status.
  */
static int _Recv(U8 * buf, int len, void * pConnectionInfo) {
  return recv((long)pConnectionInfo, (char *)buf, len, 0);
}

/**
  * @brief  Initializes ...
  * @param  None.
  * @retval None.
  */
void VNC_SERVER_Start(void)
{
	/* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

    /* Initialize the LwIP stack */
    Netif_Config();

    /* Check connection */
    if(netif_is_up(&gnetif))
	{
		/* Update VNC state machine */
		VNC_State = VNC_LINK_UP;
	}
	else
	{
		//Cable is not connected
		/* Update VNC state machine */
		VNC_State = VNC_LINK_DOWN;
	}

	/* Start VNC Task */
	if(VNC_ThreadId == 0)
	{
		osThreadDef(VNC, VNC_Thread, osPriorityAboveNormal, 0, 1024);
		VNC_ThreadId = osThreadCreate (osThread(VNC), NULL);
	}
}

/**
  * @brief  Initializes ...
  * @param  None.
  * @retval Nine.
  */
void VNC_SERVER_Stop(void)
{
	VNC_State = VNC_IDLE;
	if(!Connection_accepted)
		closesocket(_Sock);

	if(VNC_ThreadId != 0)
	{
		osThreadTerminate(VNC_ThreadId);
		VNC_ThreadId = 0;
	}
}

/**
  * @brief  Starts listening at a TCP port.
  * @param  Port: TCP port to listen at 
  * @retval listen status.
  */
static int _ListenAtTcpAddr(U16 Port)
{
	struct sockaddr_in addr;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(Port);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	listen(sock, 1);
	return sock;
}

/**
  * @brief  VNC server thread process
  * @param  argument: not used
  * @retval none
  */
static void VNC_Process(void)
{
	static int s;
	static uint8_t Is_SocketAssigned = 0;
	u32_t AddrLen;
	U16 Port;

	if(Is_SocketAssigned == 0)
	{
		Is_SocketAssigned = 1;
		/* Prepare socket (one time setup) 
		Default port for VNC is is 590x, where x is the 0-based layer index */
		Port = 5900 + _Context.ServerIndex; 

		/* Loop until we get a socket into listening state */
		do {
			s = _ListenAtTcpAddr(Port);
			if (s != -1) break;
			vTaskDelay(100); /* Try again */
			} while(1);
	}
	/* Loop once per client and create a thread for the actual server */
	while (VNC_State == VNC_PROCESS) 
	{
		/* Wait for an incoming connection */
		AddrLen = sizeof(_Addr);
		Connection_accepted = 1;
		if ((_Sock = accept(s, (struct sockaddr*)&_Addr, &AddrLen)) < 1) {
			closesocket(_Sock);
			vTaskDelay(100);
			continue; /* Error */
		}
		Connection_accepted = 0;
		GUI_VNC_Process(&_Context, _Send, _Recv, (void *)_Sock);

		/* Close the connection */
		closesocket(_Sock);
	}
}

/**
  * @brief  VNC Thread
* @param  argument: network interface
  * @retval None
  */
void VNC_Thread(void const * argument)
{
	for(;;)
	{
		switch (VNC_State)
		{
			case VNC_LINK_UP:
			{
				gnetif.ip_addr.addr = 0;
				gnetif.netmask.addr = 0;
				gnetif.gw.addr = 0;
				dhcp_start(&gnetif);
				VNC_State = VNC_WAIT_FOR_ADDRESS;
			}
			break;

			case VNC_WAIT_FOR_ADDRESS:
			{
				if (gnetif.ip_addr.addr!=0)
				{
					dhcp_stop(&gnetif);
					VNC_State = VNC_START;
				}
				else
				{
					/* DHCP timeout */
					if (gnetif.dhcp->tries > MAX_DHCP_TRIES)
					{
						VNC_State = VNC_ERROR;
						dhcp_stop(&gnetif);
					}
				}
			}
			break;

			case VNC_START: 
				/* Init VNC context and attach to layer (so context is updated if the display-layer-contents change */
				GUI_VNC_AttachToLayer(&_Context, 0);
				_Context.ServerIndex = 0;
				GUI_VNC_SetProgName ("CKY VNC Server");
				if(VNC_LockState)
				{
					GUI_VNC_SetPassword((U8 *)"CKY");
				}
				else
				{
					GUI_VNC_SetAuthentication(NULL);
				}
				VNC_State = VNC_PROCESS;
			break;

			case VNC_PROCESS:
				VNC_Process();
			break;

			case VNC_IDLE:
			break;

			default:
			break;
		}
		osDelay(250);
	}
}

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;	

	/* IP address setting */
	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

	/* - netif_add(struct netif *netif, ip_addr_t *ipaddr,
	ip_addr_t *netmask, ip_addr_t *gw,
	void *state, err_t (* init)(struct netif *netif),
	err_t (* input)(struct pbuf *p, struct netif *netif))

	Adds your network interface to the netif_list. Allocate a struct
	netif and pass a pointer to this structure as the first argument.
	Give pointers to cleared ip_addr structures when using DHCP,
	or fill them with sane numbers otherwise. The state pointer may be NULL.

	The init function pointer must point to a initialization function for
	your ethernet netif interface. The following code illustrates it's use.*/

	netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface. */
	netif_set_default(&gnetif);

	if (netif_is_link_up(&gnetif))
	{
		/* When the netif is fully configured this function must be called.*/
		netif_set_up(&gnetif);
	}
	else
	{
		/* When the netif link is down this function must be called */
		netif_set_down(&gnetif);
	}
}
