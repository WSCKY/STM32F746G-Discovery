/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2004  SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

***** emWin - Graphical user interface for embedded applications *****
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : BASIC_HelloWorld.c
Purpose     : Simple demo drawing "Hello world"
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "Sprite.h"

BUTTON_Handle hButton;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
static void _cbBk(WM_MESSAGE * pMsg)
{
	int Id, NCode;
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
		break;
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id)
			{
				case GUI_ID_BUTTON0:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							GUI_SetColor(0x00444444);
							GUI_FillRect(0, 0, 388, 271);
						break;
					}
				break;
				case GUI_ID_BUTTON1:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							// Demo ellipses
							GUI_SetColor(0xff);
							GUI_FillEllipse(195, 136, 50, 70);
							GUI_SetColor(0x0);
							GUI_DrawEllipse(195, 136, 50, 70);
							GUI_SetColor(0x000000);
							GUI_FillEllipse(195, 136, 10, 50);
						break;
					}
				break;
			}
		break;
		default:
			WM_DefaultProc(pMsg);
		break;
	}
}

/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
	GUI_Clear();

	WM_SetCallback(WM_HBKWIN, _cbBk);

	GUI_SetColor(GUI_RED);
	GUI_FillRect(389, 0, 391, 271);
	hButton = BUTTON_CreateAsChild(395, 233, 80, 28, WM_HBKWIN, GUI_ID_BUTTON0, WM_CF_SHOW);
	BUTTON_SetFont(hButton, &GUI_Font20B_ASCII);
	BUTTON_SetText(hButton, "CLEAR");
	BUTTON_SetTextAlign(hButton, GUI_TA_HCENTER | GUI_TA_VCENTER);

	hButton = BUTTON_CreateAsChild(395, 195, 80, 28, WM_HBKWIN, GUI_ID_BUTTON1, WM_CF_SHOW);
	BUTTON_SetFont(hButton, &GUI_Font20B_ASCII);
	BUTTON_SetText(hButton, "DRAW");
	BUTTON_SetTextAlign(hButton, GUI_TA_HCENTER | GUI_TA_VCENTER);

	while(1)
	{
		GUI_Delay(1000);
	}
}

/*************************** End of file ****************************/
