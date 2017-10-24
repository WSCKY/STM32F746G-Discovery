#include "GUI.h"
void MainTask(void)
{
	GUI_MTOUCH_EVENT Event;
	GUI_MTOUCH_INPUT Input;
	unsigned i;
	//GUI_Init();
	//GUI_MTOUCH_Enable(1);
	GUI_SetPenSize(5);
	do
	{
		if (GUI_MTOUCH_GetEvent(&Event) == 0)
		{
			for (i = 0; i < Event.NumPoints; i++)
			{
				GUI_MTOUCH_GetTouchInput(&Event, &Input, i);
				GUI_DrawPoint(Input.x, Input.y);
			}
		}
		GUI_Delay(1);
	} while (1);
}
