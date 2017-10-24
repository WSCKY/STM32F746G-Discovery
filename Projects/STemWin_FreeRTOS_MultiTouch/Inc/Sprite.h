#ifndef __SPRITE_H
#define __SPRITE_H

#include "stm32f7xx_hal.h"
#include "DIALOG.h"
#include "gui.h"

#define USE_SPRITE 0

void Sprite_Run(void const * argument);
void SpriteSingleInit(void const * argument);
void SpriteSingleRun_100ms(void const * argument);

#endif
