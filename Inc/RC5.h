/*
 * RC5.h
 *
 *  Created on: Apr 05, 2021
 *      Author: Martin
 */

#ifndef RC5_H_
#define RC5_H_

#include "stm32l1xx_hal.h"
#include "globals.h"
#include "serialLogger.h"
#include "setbrightness.h"
#include "options.h"
#include "main.h"
#include <stdbool.h>
#include <stdio.h>


#define MainloopFrequ       10	//40Hz durch 4 Schritte

#define RC5Addr_front		27	//address for front light brightness
#define RC5Addr_back		28	//address for back light brightness, MUST follow front address!
#define RC5Addr_com			29	//address for RC5 communication
#define maxRC5Address		31	//maximum possible RC5 address

#define RC5Cmd_AlarmStart	53	//code for start of alarm
#define RC5Cmd_AlarmEnd		54	//code for end of alarm
#define RC5Cmd_Off			13	//code for light off
#define RC5Cmd_On			12	//code for light on

#define RC5Cmd_Repeats		3	//number of repeats for on, off and AlarmStart commands
#define RC5Value_Repeats	1	//number of repeats for value transmission

#define RemoteSteps			2	//Step size for brightness control via remote control
#define Brightness_steps	20	//number of steps used to execute a brightness change

volatile unsigned char rCommand;   //Bitte erhalten! Wenn Befehl fertig: auswerten
volatile unsigned char rAddress;   //Bitte erhalten! Wenn Befehl fertig: auswerten
volatile unsigned char rCounter;   //Bitte erhalten!

volatile bool RTbit;			//Togglebit von RC5

//State Machine zur Decodierung des RC5-Fernbedieungscodes
static const unsigned char tblRemote[] =	{
		1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0,
		8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14,
		0, 15, 0, 16, 0, 17, 0, 18, 0, 19, 0, 20, 0,
		20, 21, 0, 22, 0, 23, 26, 24, 26, 25, 26, 0,
		27, 0, 28, 0, 29, 31, 30, 31, 0, 31, 0, 32,
		0, 33, 35, 34, 35, 43, 36, 43, 37, 0, 38, 40,
		39, 40, 0, 40, 0, 41, 0, 42, 35, 34, 35, 44,
		0, 45, 48, 46, 48, 47, 48, 0, 49, 0, 50, 0, 34, 43};

void RC5SignalSampling(GPIO_PinState signal);
void SetLightRemote(unsigned char i, signed char steps);
void SetBrightnessRemote(unsigned char i);
void SetBrightnessLevelRemote();
void DecodeRemote();

#endif /* RC5_H_ */
