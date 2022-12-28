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
#include <stdbool.h>

#define KeyPressShort	30
#define KeyPressLong	80
#define KeyPressLonger	KeyPressLong*2

void MainMenu();
void Init_MainMenu();

#endif /* MAINMENU_H_ */
