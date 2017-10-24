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
File        : 320_Overview_ArrowDown_2.c
Content     : Bitmap 4 * 12
---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif


static GUI_CONST_STORAGE unsigned short ac320_Overview_ArrowDown_2[] = {
  0x071F, 0x071F, 0x071F, 0x071F,
  0x071F, 0x071F, 0x071F, 0x071F,
  0x071F, 0x071F, 0x071F, 0x071F,
  0x071F, 0x071F, 0x071F, 0x071F,
  0x071F, 0x071F, 0x071F, 0x071F,
  0x16DC, 0x0000, 0x10A6, 0x071F,
  0x16DC, 0x0000, 0x10A6, 0x071F,
  0x16DC, 0x0000, 0x10A6, 0x071F,
  0x1D4B, 0x0000, 0x0421, 0x1DD1,
  0x1678, 0x0000, 0x0843, 0x0EFD,
  0x071F, 0x110A, 0x15D2, 0x071F,
  0x071F, 0x1678, 0x1678, 0x071F
};

extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_ArrowDown_2;

GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_ArrowDown_2 = {
  4, /* XSize */
  12, /* YSize */
  8, /* BytesPerLine */
  16, /* BitsPerPixel */
  (unsigned char *)ac320_Overview_ArrowDown_2,  /* Pointer to picture data */
  NULL  /* Pointer to palette */
 ,GUI_DRAW_BMP555
};

/*************************** End of file ****************************/
