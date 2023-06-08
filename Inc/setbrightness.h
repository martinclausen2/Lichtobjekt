/*
 * setbrightness.h
 *
 *  Created on: 24.12.2019
 *      Author: Martin
 */

#ifndef SETBRIGHTNESS_H_
#define SETBRIGHTNESS_H_

#include "status_LED.h"
#include <stdbool.h>
#include "stm32l1xx_hal.h"
#include "extbrightness.h"

#define maxPWM	   			0xFFFF	// =  maxBrigntess^2 = 2^16-1
#define Brightness_steps	20		//number of steps used to execute a brightness change
#define startupfocus		0		//focus channel
#define fadetime			150

#define WriteTime			0xFF	/* time until new brightness value is saved to the eeprom */
extern unsigned char WriteTimer;	/* time until Brightness is saved in calls to StoreBrightness() */

extern bool LightOn;

extern int FocusChannel;

extern unsigned char Brightness[maxVirtualChannel];	//current value

void PWM_Init(TIM_HandleTypeDef *htim_PWM);
void PWM_StepDim();
void SwLightOn(unsigned char i, unsigned int relBrightness);
void SwAllLightOn();
void SwLightOff(unsigned char i);
void SwAllLightOff();
void PWM_SetupDim(unsigned char i, signed int PWM_dimsteps, signed int brightnessStep, unsigned int timeSteps);
void ToggleFocus();
int PreviewToggelFocus();
void SetExtBrightness_last();
void StoreBrightness();

#endif /* SETBRIGHTNESS_H_ */
