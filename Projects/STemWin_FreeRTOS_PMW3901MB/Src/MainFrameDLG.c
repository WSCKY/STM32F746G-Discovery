/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.28                          *
*        Compiled Jan 30 2015, 16:41:06                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "PMW3901MB.h"
#include "cpu_utils.h"
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_GRAPH_0    (GUI_ID_USER + 0x01)
#define ID_TEXT_0    (GUI_ID_USER + 0x02)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
GRAPH_DATA_Handle hData_X, hData_Y;
GRAPH_SCALE_Handle _hScaleH, _hScaleV;
static GUI_COLOR _aColorData[2] = {
  0x50C0FF,
  0xFFC050
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "MainFrame", ID_WINDOW_0, 0, 0, 480, 262, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Waves", ID_GRAPH_0, 0, 32, 480, 200, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Motion Waves", ID_TEXT_0, 165, 0, 150, 32, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'MainFrame'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, 0x00808080);
    //
    // Initialization of 'Waves'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(hItem, 20, 0, 0, 10);
		GRAPH_SetLineStyleH(hItem, GUI_LS_DASH);
		GRAPH_SetLineStyleV(hItem, GUI_LS_DASH);
		GRAPH_SetGridDistX(hItem, 50); GRAPH_SetGridDistY(hItem, 25);
		GRAPH_SetGridOffY(hItem, 95);
		GRAPH_SetGridVis(hItem, 1);
		WM_BringToBottom(hItem);

		hData_X = GRAPH_DATA_YT_Create(_aColorData[0], 460, 0, 0);
		GRAPH_DATA_YT_SetOffY(hData_X, 95);
		hData_Y = GRAPH_DATA_YT_Create(_aColorData[1], 460, 0, 0);
		GRAPH_DATA_YT_SetOffY(hData_Y, 95);
		GRAPH_AttachData(hItem, hData_X);
		GRAPH_AttachData(hItem, hData_Y);

		_hScaleH = GRAPH_SCALE_Create(196, GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, 20);
		_hScaleV = GRAPH_SCALE_Create(10, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL, 20);
		GRAPH_SCALE_SetOff(_hScaleV, 95);
		GRAPH_SCALE_SetFactor(_hScaleV, 0.25f);
		GRAPH_AttachScale(hItem, _hScaleH);
		GRAPH_AttachScale(hItem, _hScaleV);
    //
    // Initialization of 'Motion Waves'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, 0x00FF0000);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  // USER START (Optionally insert additional message handling)
//	case WM_PAINT:
//		GUI_DispStringAt("MCU Load:", 400, 262); GUI_DispDecMin(osGetCPUUsage());
//	break;
	case WM_TIMER:
		GUI_SetFont(&GUI_Font8x8);
		GUI_DispStringAt("Data Rate:", 0, 264); GUI_DispDecMin(GetPMW3901DataRate()); GUI_DispString("Hz  ");
		GUI_DispStringAt("MCU Load:", 384, 264); GUI_DispDecMin(osGetCPUUsage()); GUI_DispString("% ");
		WM_RestartTimer(pMsg->Data.v, 1000);
	break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
  break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateMainFrame
*/
WM_HWIN CreateMainFrame(void);
WM_HWIN CreateMainFrame(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
PMW3901MB_BurstReportDef *p;
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
	WM_HWIN hWin = CreateMainFrame();
	WM_HTIMER hTimer = WM_CreateTimer(hWin, 0, 1000, 0);
	p = ReadDeltaDataRaw();
//	GUI_SetFont(&GUI_Font8x8);
	while(1) {
		GUI_Delay(10);
		if(PMW3901_DataUpdated()) {
			GRAPH_DATA_YT_AddValue(hData_X, 2 * (((int16_t)p->Delta_X_H << 8) | p->Delta_X_L));
			GRAPH_DATA_YT_AddValue(hData_Y, 2 * (((int16_t)p->Delta_Y_H << 8) | p->Delta_Y_L));
		}
//		GUI_DispStringAt("Data Rate:", 0, 264); GUI_DispDecMin(GetPMW3901DataRate()); GUI_DispString("Hz  ");
//		GUI_DispStringAt("MCU Load:", 384, 264); GUI_DispDecMin(osGetCPUUsage()); GUI_DispString("% ");
	}
}
// USER END

/*************************** End of file ****************************/
