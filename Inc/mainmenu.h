/*
 * mainmenu.h
 *
 *  Created on: Dec 22, 2019
 *      Author: Martin
 */

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "stm32l1xx_hal.h"
#include "settings.h"
#include "keys.h"
#include "encoder.h"
#include "setbrightness.h"
#include "status_LED.h"
#include "RC5.h"
#include "wakeuplight.h"
#include "main.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define KeyPressShort	40
#define KeyPressLong	120
#define KeyPressLonger	KeyPressLong*3

void MainMenu();
void EnterBatStandby();

#endif /* MAINMENU_H_ */
