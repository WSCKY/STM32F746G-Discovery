/*********************************************************************
*                SEGGER Microcrontroller GmbH & Co. KG               *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2010  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.04 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : MainTask.c
Content     : Brewing Automation Demo - Requires app. 1 MB of memory.
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
*/
#include <stddef.h>
#include "WM.h"
#include "Graph.h"
#include "GUI.h"

#define COLOR_BEER       0x0DC5FD
#define APP_NEXT_FRAME  (WM_USER + 0)
#define APP_DRAW        (WM_USER + 1)
#define APP_HIDE        (WM_USER + 2)
#define APP_FIRST_FRAME (WM_USER + 3)
#define GRID_DRAW_FULL  (WM_USER + 4)
#define GRID_DRAW_START (WM_USER + 5)

/*********************************************************************
*
*       Fonts
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF16B_AA;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF19B_AA;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontF24B_AA;

/*********************************************************************
*
*       Bitmaps
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Bubbles;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_ArrowDown;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_ArrowDown_2;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_ArrowLeft;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_ArrowRight;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Barrel;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Tank_2;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_B;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_SeggerLogo;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Detail_Mashing_00;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Detail_Mashing_01;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Detail_Mashing_02;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Detail_Mashing_03;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Detail_Mashing_04;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Detail_Mashing_05;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Boiling_00;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Boiling_01;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Boiling_02;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Boiling_03;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Boiling_04;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Boiling_05;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Lautering_00;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Lautering_01;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Lautering_02;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Lautering_03;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Lautering_04;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Lautering_05;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Mashing_00;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Mashing_01;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Mashing_02;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Mashing_03;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Mashing_04;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Mashing_05;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_00;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_01;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_02;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_03;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_04;
extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_05;

/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef struct {
  int TextY[5];
  int Cnt;
  int DrawBk;
} BkState;

typedef struct {
  int Cnt;
  int Draw;
} MashingState;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const short _aTemperature[] = {
  50, 51, 52, 52, 53, 54, 55, 55, 55, 55, 55,  //   0 -  10
  55, 55, 55, 55, 55, 55, 55, 55, 55, 55,      //  11 -  20
  55, 55, 55, 55, 55, 56, 57, 58, 58, 59,      //  21 -  30
  60, 61, 62, 62, 63, 64, 64, 64, 64, 64,      //  31 -  40
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,      //  41 -  50
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,      //  51 -  60
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,      //  61 -  70
  64, 64, 64, 64, 64, 65, 66, 67, 68, 68,      //  71 -  80
  69, 70, 71, 72, 72, 72, 72, 72, 72, 72,      //  81 -  90
  72, 72, 72, 72, 72, 72, 72, 72, 72, 72,      //  91 - 100
  73, 74, 74, 75, 76, 76, 77, 77, 78, 78,      // 101 - 110
  78, 78, 78, 78, 78, 78, 78, 78, 78, 78,      // 111 - 120
  78, 78, 78, 78                               // 121 - 124
};

static int _iTemperature;
static int _iWort;
static int _iGraphCnt;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawMovingBitmapClipRect
*/
static void _DrawMovingBitmapClipRect(GUI_CONST_STORAGE GUI_BITMAP * pBitmap,
                                      int ClipRectX0, int ClipRectY0, int ClipRectX1, int ClipRectY1,
                                      int Step,       int StepDist,   int MovDir) { // 0 == x; 1 == y;
  
  GUI_RECT Rect;
  int Stop;
  int x0;
  int y0;
  int dx;
  int dy;
  
  Rect.x0 = ClipRectX0;
  Rect.y0 = ClipRectY0;
  Rect.x1 = ClipRectX1;
  Rect.y1 = ClipRectY1,
  Stop = 0;
  GUI_SetClipRect(&Rect);
  if (MovDir) {
    dx = 0;
    dy = pBitmap->YSize;
    x0 = ClipRectX0;
    y0 = ClipRectY0 - pBitmap->YSize + Step * StepDist;
  } else {
    dx = pBitmap->XSize;
    dy = 0;
    x0 = ClipRectX0 - pBitmap->XSize + Step * StepDist;
    y0 = ClipRectY0;
  }
  do {
    GUI_DrawBitmap(pBitmap, x0, y0);
    x0 += dx;
    y0 += dy;
    if ((x0 > ClipRectX1) || (y0 > ClipRectY1)) {
      Stop = 1;
    }
  } while (Stop == 0);
  GUI_SetClipRect(NULL);
}

/*********************************************************************
*
*       _OverviewStatic
*/
static void _OverviewStatic(void) {
  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();
  GUI_DrawBitmap( &bm320_Overview_Whirlpool_B, 270, 88);
  GUI_DrawBitmap( &bm320_Overview_Tank_2,      7,   125);
  GUI_DrawBitmap( &bm320_Overview_Tank_2,      88,  125);
  GUI_DrawBitmap( &bm320_Overview_Barrel,      184, 184);
  GUI_DrawBitmap( &bm320_Overview_Barrel,      227, 184);
  GUI_DrawBitmap( &bm320_Overview_Barrel,      271, 184);
  //
  // Draw pipes
  //
  GUI_SetColor(GUI_BLACK);
  //
  // 1. Long pipe between whirlpooling and fermenting
  //
  GUI_FillRect(304,  88, 304, 109);
  GUI_FillRect(313,  88, 313, 118);
  GUI_FillRect( 35, 109, 303, 109);
  GUI_FillRect( 44, 118, 312, 118);
  GUI_FillRect( 35, 110,  35, 123);
  GUI_FillRect( 44, 119,  44, 123);
  //
  // 2. Short pipe between fermenting and storage
  //
  GUI_FillRect( 71, 162,  87, 162);
  GUI_FillRect( 71, 171,  87, 171);
  //
  // 3. Long pipe between storage and barrels
  //
  GUI_FillRect(152, 162, 291, 162);
  GUI_FillRect(152, 171, 199, 171);
  GUI_FillRect(204, 171, 242, 171);
  GUI_FillRect(247, 171, 286, 171);
  //
  // 4. Short pipes between storage and barrels
  //
  GUI_FillRect(199, 172, 199, 182);
  GUI_FillRect(204, 172, 204, 182);
  GUI_FillRect(242, 172, 242, 182);
  GUI_FillRect(247, 172, 247, 182);
  GUI_FillRect(286, 172, 286, 182);
  GUI_FillRect(291, 163, 291, 182);
  //
  // 5. Cornered pipe between mashing and lautering
  //
  GUI_FillRect( 60,  88,  60, 103);
  GUI_FillRect( 61, 103,  98, 103);
  GUI_FillRect( 98,  88,  98, 102);
  GUI_FillRect( 70,  88,  70,  94);
  GUI_FillRect( 71,  94,  88,  94);
  GUI_SetColor(0x5A525A);
  GUI_FillRect( 88,  88,  88,  93);
  GUI_SetColor(0x18738B);
  GUI_FillRect( 69,  88,  69,  94);
  GUI_FillRect( 97,  88,  97, 102);
  GUI_SetColor(0x737373);
  GUI_FillRect( 71,  93,  87,  93);
  GUI_SetColor(0x109CC5);
  GUI_FillRect( 89,  88,  89,  94);
  GUI_SetColor(GUI_BLACK);
  //
  // 6. Cornered pipe between lautering and boiling
  //
  GUI_FillRect(141,  88, 141, 103);
  GUI_FillRect(142, 103, 179, 103);
  GUI_FillRect(179,  88, 179, 102);
  GUI_FillRect(151,  88, 151,  94);
  GUI_FillRect(152,  94, 169,  94);
  GUI_SetColor(0x5A525A);
  GUI_FillRect(169,  88, 169,  93);
  GUI_SetColor(0x18738B);
  GUI_FillRect(150,  88, 150,  94);
  GUI_FillRect(178,  88, 178, 102);
  GUI_SetColor(0x737373);
  GUI_FillRect(152,  93, 168,  93);
  GUI_SetColor(0x109CC5);
  GUI_FillRect(170,  88, 170,  94);
  GUI_SetColor(GUI_BLACK);
  //
  // 7. Cornered pipe between boiling and whirlpooling
  //
  GUI_FillRect(222,  88, 222, 103);
  GUI_FillRect(223, 103, 260, 103);
  GUI_FillRect(260,  88, 260, 102);
  GUI_FillRect(232,  88, 232,  94);
  GUI_FillRect(233,  94, 250,  94);
  GUI_SetColor(0x5A525A);
  GUI_FillRect(250,  88, 250,  93);
  GUI_SetColor(0x18738B);
  GUI_FillRect(231,  88, 231,  94);
  GUI_FillRect(259,  88, 259, 102);
  GUI_SetColor(0x737373);
  GUI_FillRect(233,  93, 249,  93);
  GUI_SetColor(0x109CC5);
  GUI_FillRect(251,  88, 251,  94);
  GUI_SetColor(GUI_BLACK);
  //
  // Fill Pipes with color
  //
  GUI_SetColor(COLOR_BEER);
  //
  // Fill 1
  //
  GUI_FillRect(305,  88, 312, 117);
  GUI_FillRect( 36, 110, 304, 117);
  GUI_FillRect( 36, 118,  43, 123);
  //
  // Fill 2
  //
  GUI_FillRect( 71, 163,  87, 170);
  //
  // Fill 3
  //
  GUI_FillRect(152, 163, 290, 170);
  //
  // Fill 4
  //
  GUI_FillRect(200, 171, 203, 182);
  GUI_FillRect(243, 171, 246, 182);
  GUI_FillRect(287, 171, 290, 182);
  //
  // Fill 5
  //
  GUI_FillRect( 61,  88,  68, 102);
  GUI_FillRect( 69,  95,  96, 102);
  GUI_FillRect( 90,  88,  96,  94);
  //
  // Fill 6
  //
  GUI_FillRect(142,  88, 149, 102);
  GUI_FillRect(150,  95, 177, 102);
  GUI_FillRect(171,  88, 177,  94);
  //
  // Fill 7
  //
  GUI_FillRect(223,  88, 230, 102);
  GUI_FillRect(231,  95, 258, 102);
  GUI_FillRect(252,  88, 258,  94);
  //
  // Segger Logo
  //
  GUI_DrawBitmap(&bm320_SeggerLogo, 236, 121);
}

/*********************************************************************
*
*       _OverviewDynamic
*/
static void _OverviewDynamic(BkState * pBkState) {
  GUI_CONST_STORAGE GUI_BITMAP * apbm320_Overview_Mashing[] = {
    &bm320_Overview_Mashing_00,
    &bm320_Overview_Mashing_01,
    &bm320_Overview_Mashing_02,
    &bm320_Overview_Mashing_03,
    &bm320_Overview_Mashing_04,
    &bm320_Overview_Mashing_05,
  };

  GUI_CONST_STORAGE GUI_BITMAP * apbm320_Overview_Lautering[] = {
    &bm320_Overview_Lautering_00,
    &bm320_Overview_Lautering_01,
    &bm320_Overview_Lautering_02,
    &bm320_Overview_Lautering_03,
    &bm320_Overview_Lautering_04,
    &bm320_Overview_Lautering_05,
  };

  GUI_CONST_STORAGE GUI_BITMAP * apbm320_Overview_Boiling[] = {
    &bm320_Overview_Boiling_00,
    &bm320_Overview_Boiling_01,
    &bm320_Overview_Boiling_02,
    &bm320_Overview_Boiling_03,
    &bm320_Overview_Boiling_04,
    &bm320_Overview_Boiling_05,
  };

  GUI_CONST_STORAGE GUI_BITMAP * apbm320_Overview_Whirlpool[] = {
    &bm320_Overview_Whirlpool_00,
    &bm320_Overview_Whirlpool_01,
    &bm320_Overview_Whirlpool_02,
    &bm320_Overview_Whirlpool_03,
    &bm320_Overview_Whirlpool_04,
    &bm320_Overview_Whirlpool_05,
  };

  int         j;
  char      * TextString[5] = { "SEGGER", "emWin", "Brewing", "Automation", "Demo" };
  const int   TextX[5]      = { 167,      171,     167,       157,          175    };
  GUI_RECT    Rect          = { 153, 121, 234, 159 };

  GUI_SetBkColor(GUI_WHITE);
  GUI_DrawBitmap(apbm320_Overview_Mashing[pBkState->Cnt],     7,  5);
  GUI_DrawBitmap(apbm320_Overview_Lautering[pBkState->Cnt],  88,  5);
  GUI_DrawBitmap(apbm320_Overview_Boiling[pBkState->Cnt],   169,  5);
  GUI_DrawBitmap(apbm320_Overview_Whirlpool[pBkState->Cnt], 250, 21);
  //
  // Moving Arrows
  //
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowDown,   306, 88,  311, 111, pBkState->Cnt,  4, 1);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowLeft,   37,  111, 311, 116, pBkState->Cnt, -4, 0);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowRight,  72,  164, 86,  169, pBkState->Cnt,  4, 0);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowRight,  153, 164, 289, 169, pBkState->Cnt,  4, 0);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowDown_2, 200, 171, 203, 182, pBkState->Cnt,  2, 1);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowDown_2, 243, 171, 246, 182, pBkState->Cnt,  2, 1);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowDown_2, 287, 171, 290, 182, pBkState->Cnt,  2, 1);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowRight,  62,  96,  95,  101, pBkState->Cnt,  4, 0);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowRight,  143, 96,  176, 101, pBkState->Cnt,  4, 0);
  _DrawMovingBitmapClipRect(&bm320_Overview_ArrowRight,  224, 96,  257, 101, pBkState->Cnt,  4, 0);
  _DrawMovingBitmapClipRect(&bm320_Bubbles,              48,  136, 64,  163, pBkState->Cnt, -2, 1);
  _DrawMovingBitmapClipRect(&bm320_Bubbles,              45,  177, 65,  195, pBkState->Cnt, -2, 1);
  _DrawMovingBitmapClipRect(&bm320_Bubbles,              47,  208, 63,  232, pBkState->Cnt, -2, 1);
  _DrawMovingBitmapClipRect(&bm320_Bubbles,              128, 136, 144, 163, pBkState->Cnt, -2, 1);
  _DrawMovingBitmapClipRect(&bm320_Bubbles,              125, 177, 145, 195, pBkState->Cnt, -2, 1);
  _DrawMovingBitmapClipRect(&bm320_Bubbles,              127, 208, 143, 232, pBkState->Cnt, -2, 1);
  //
  // Moving Text
  //
  GUI_SetClipRect(&Rect);
  GUI_SetFont(&GUI_FontF16B_AA); //GUI_Font16B_ASCII
  GUI_SetColor(0x9c4a31);
  for (j = 0; j < 5; j++) {
    GUI_GetStringDistX(TextString[j]);
    GUI_DispStringAt(TextString[j], TextX[j], pBkState->TextY[j]);
  }
  GUI_SetClipRect(NULL);
}

/*********************************************************************
*
*       _MashingDetail
*/
static void _MashingDetail(MashingState * pState) {
  GUI_CONST_STORAGE GUI_BITMAP * apbm320_Detail_Mashing[] = {
    &bm320_Detail_Mashing_00,
    &bm320_Detail_Mashing_01,
    &bm320_Detail_Mashing_02,
    &bm320_Detail_Mashing_03,
    &bm320_Detail_Mashing_04,
    &bm320_Detail_Mashing_05,
  };

  GUI_SetBkColor(GUI_WHITE);
  if (pState->Draw) {
    GUI_Clear();
    GUI_DrawBitmap(&bm320_SeggerLogo,   2, 2);
    GUI_DrawBitmap(&bm320_SeggerLogo, 239, 2);
    GUI_SetFont(&GUI_FontF24B_AA);     //GUI_SetFont(&GUI_Font24B_ASCII);
    GUI_SetColor(0x9c4a31);
    GUI_DispStringAt("Mashing",      117, 10);
    GUI_SetFont(&GUI_FontF19B_AA);
    GUI_DispStringAt("Wort:",        139, 54);
    GUI_DispStringAt("Temperature:", 139, 78);
    GUI_SetColor(0x3f3f3f);
    GUI_DrawRoundedFrame(2, 43, 316, 236, 7, 2);
  }
  GUI_DrawBitmap(apbm320_Detail_Mashing[pState->Cnt], 9, 65);
  GUI_SetFont(&GUI_FontF19B_AA);
  GUI_SetColor(0x9c4a31);
  GUI_DispDecAt(_iWort,        261, 54, 2);
  GUI_DispDecAt(_iTemperature, 261, 78, 2);
  GUI_DispStringAt("°P", 285, 54);
  GUI_DispStringAt("°C", 285, 78);
}

/*********************************************************************
*
*       _IncPos
*/
static void _IncPos(int * pStateCnt, int Start, int Step, int MaxCnt) {
  *pStateCnt = (*pStateCnt < MaxCnt) ? *pStateCnt + Step : Start;
}

/*********************************************************************
*
*       _SetPos
*/
static void _SetPos(int *pStateCnt, int Pos) {
  *pStateCnt = Pos;
}

/*********************************************************************
*
*       _cbMashing
*/
static void _cbMashing(WM_MESSAGE * pMsg) {
  static MashingState State = { 0, 0 };

  switch (pMsg->MsgId) {
  case APP_DRAW:
    State.Draw = 1;
    break;
  case APP_HIDE:
    State.Draw = 0;
    break;
  case APP_NEXT_FRAME:
    _IncPos(&State.Cnt, 0, 1, 5);
    break;
  case APP_FIRST_FRAME:
    _SetPos(&State.Cnt, 0);
    break;
  case WM_PAINT:
    _MashingDetail(&State);
    break;
  }
}

/*********************************************************************
*
*       _BkOnPaint
*/
static void _BkOnPaint(BkState * pBkState) {
  if (pBkState->DrawBk) {
    _OverviewStatic();
  }
  _OverviewDynamic(pBkState);
}

/*********************************************************************
*
*       _IncPosBk
*/
static void _IncPosBk(BkState * pBkState) {
  int j;

  pBkState->Cnt = (pBkState->Cnt < 5) ? pBkState->Cnt + 1 : 0;
  for (j = 0; j < 5; j++) {
    if (pBkState->TextY[j] < 107) {
      pBkState->TextY[j] = 200;
    } else {
      pBkState->TextY[j]--;
    }
  }
}

/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  static BkState BkState = {
    { 161, 178, 195, 212, 229 },
      0,
      1
  };

  switch (pMsg->MsgId) {
  case APP_DRAW:
    BkState.DrawBk = 1;
    break;
  case APP_HIDE:
    BkState.DrawBk = 0;
    break;
  case APP_NEXT_FRAME:
    _IncPosBk(&BkState);
    break;
  case WM_PAINT:
    _BkOnPaint(&BkState);
    break;
  }
}

/*********************************************************************
*
*       _cbGraph
*/
static void _cbGraph(WM_HWIN hWin, int Stage) {
  static int DrawGrid = 1;
  unsigned   OffX;
  unsigned   OffY;
  unsigned   MaxX;
  unsigned   MaxY;
  unsigned   StepX;
  unsigned   StepY;
  unsigned   i;
  unsigned   Value;

  GUI_USE_PARA(hWin);
  OffX  = 148;
  OffY  = 12;
  MaxX  = 171;
  MaxY  = 131;
  StepX = 25;
  StepY = 25;
  switch (Stage) {
  case GRAPH_DRAW_AFTER_BORDER:
    GUI_SetColor(0x9c4a31);
    for (i = OffY; i < MaxY; i += StepY) {
      GUI_DrawHLine(i, 16, 169);
    }
    if (DrawGrid) {
      GUI_DrawVLine(23, 1, 118);
      for (i = OffX; i < MaxX + 125; i += StepX) {
        if (i - _iGraphCnt < 23) {
          GUI_DrawVLine(i - _iGraphCnt + 125, 1, 118);
        } else {
          GUI_DrawVLine(i - _iGraphCnt,       1, 118);
        }
      }
    } else {
      GUI_DrawVLine(148, 2, 118);
    }
    break;
  case GRAPH_DRAW_LAST:
    GUI_SetFont(&GUI_Font6x8);
    Value = 0;
    i     = OffX - 5;
    while (i - _iGraphCnt < 170) {
      if (i - _iGraphCnt > 23) {
        if (Value == 0) {
          GUI_DispDecAt(Value, i - _iGraphCnt + 3, 120, 1);
        } else if (Value > 0 && Value < 100) {
          GUI_DispDecAt(Value, i - _iGraphCnt,     120, 2);
        } else if (Value == 100) {
          GUI_DispDecAt(Value, i - _iGraphCnt - 3, 120, 3);
        }
      }
      i     += StepX;
      Value += 20;
    };
    break;
  case GRID_DRAW_FULL:
    DrawGrid = 1;
    break;
  case GRID_DRAW_START:
    DrawGrid = 0;
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
*       MainTask
*/
void MainTask(void);
void MainTask(void) {
  WM_HWIN            hWinBk;
  WM_HWIN            hWinMashing;
  WM_HWIN            hGraph;
  GRAPH_DATA_Handle  hData;
  GRAPH_SCALE_Handle hScaleVert;
  int                i;

  //
  // Initialize / Create
  //
//  GUI_Init();
  WM_SetDesktopColor(GUI_WHITE);
  hWinBk      = WM_CreateWindow(0,   0,   320, 240, WM_CF_SHOW,  _cbBk,      0);
  hWinMashing = WM_CreateWindow(0,   0,   320, 240, 0,           _cbMashing, 0);
  hGraph      = GRAPH_CreateEx (140, 100, 171, 131, hWinMashing, WM_CF_SHOW | WM_CF_MEMDEV, 0, GUI_ID_GRAPH0);
  hData       = GRAPH_DATA_YT_Create(COLOR_BEER, 126, 0, 0);
  hScaleVert  = GRAPH_SCALE_Create(14, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 25);
  //
  // Configure GRAPH widget and data object
  //
  WIDGET_SetEffect      (hGraph, &WIDGET_Effect_Simple);
  GRAPH_SetColor        (hGraph, GUI_WHITE, GRAPH_CI_BK);
  GRAPH_SetBorder       (hGraph, 23, 0, 0, 18);
  GRAPH_SetUserDraw     (hGraph, _cbGraph);
  GRAPH_DATA_YT_SetAlign(hData,  GRAPH_ALIGN_LEFT);
  //
  // Configure scale object
  //
  GRAPH_SCALE_SetFactor   (hScaleVert, 0.4f);
  GRAPH_SCALE_SetOff      (hScaleVert, -102);
  GRAPH_SCALE_SetTextColor(hScaleVert, 0x9c4a31);
  //
  // Attach data and scale object to the GRAPH widget
  //
  GRAPH_AttachData (hGraph, hData);
  GRAPH_AttachScale(hGraph, hScaleVert);
  GUI_MEMDEV_MoveInWindow(hWinBk, 160, 120, 360, 800);
  WM_SendMessageNoPara   (hWinBk, APP_HIDE);
  while (1) {
    //
    // Draw Overview
    //
    for (i = 0; i < 100; i++) {
      GUI_Delay(60);
      WM_SendMessageNoPara(hWinBk, APP_NEXT_FRAME);
      WM_InvalidateWindow (hWinBk);
    }
    //
    // Draw Mashing in detail
    //
    for (i = 0; i < 126; i++) {
      GRAPH_DATA_YT_AddValue(hData, (50 - 40) * 25 / 10);
    }
    _iTemperature = _aTemperature[0];
    _iWort        = 0;
    WM_SendMessageNoPara   (hWinBk,      APP_DRAW);
    WM_SendMessageNoPara   (hWinMashing, APP_DRAW);
    WM_InvalidateWindow    (hWinMashing);
    GUI_MEMDEV_MoveInWindow(hWinMashing, 10, 10, -180, 500);
    WM_SendMessageNoPara   (hWinBk,      APP_HIDE);
    WM_SendMessageNoPara   (hWinMashing, APP_HIDE);
    _cbGraph               (hWinMashing, GRID_DRAW_FULL);
    for (i = 0; i < 126; i++) {
      GUI_Delay(60);
      _iTemperature = _aTemperature[i];
      _iWort        = i / 10;
      _iGraphCnt    = i;
      GRAPH_DATA_YT_AddValue(hData, (_aTemperature[i] - 40) * 25 / 10);
      WM_SendMessageNoPara  (hWinMashing, APP_NEXT_FRAME);
      WM_InvalidateWindow   (hWinMashing);
      WM_InvalidateWindow   (hGraph);
    }
    GRAPH_DATA_YT_Clear     (hData);
    for (i = 0; i < 126; i++) {
      GRAPH_DATA_YT_AddValue(hData, (50 - 40) * 25 / 10);
    }
    _cbGraph                (hWinMashing, GRID_DRAW_START);
    WM_SendMessageNoPara    (hWinBk,      APP_DRAW);
    WM_SendMessageNoPara    (hWinMashing, APP_DRAW);
    GUI_MEMDEV_MoveOutWindow(hWinMashing, 10, 10, -180, 500);
    WM_InvalidateWindow     (hWinBk);
    WM_SendMessageNoPara    (hWinBk,      APP_HIDE);
  }
}

/*************************** End of file ****************************/
