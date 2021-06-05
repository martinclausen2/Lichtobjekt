/*
 * setbrightness.h
 *
 *  Created on: 24.12.2019
 *      Author: Martin
 */

#ifndef SETBRIGHTNESS_H_
#define SETBRIGHTNESS_H_

#include "globals.h"
#include "status_LED.h"
#include <stdbool.h>
#include "stm32l1xx_hal.h"

#define maxBrightness		045	    //avoid overflow with signed numbers, should be filled with 1 from MSB to LSB
#define maxPWM	   			02047	// =  maxBrigntess^2, limited to keep high PWM frequency required for LED driver
#define Brightness_steps	20		//number of steps used to execute a brightness change
#define startupfocus		1		//focus on back=1 or front=0
#define fadetime			150
#define FrontChannel		0
#define BackChannel			1

bool LightOn;

bool FocusBacklight;

unsigned char Brightness[2];// = {0,0};	//current value

TIM_HandleTypeDef *htim_PWM;				//handle to address timer

void PWM_Init(TIM_HandleTypeDef *htim_PWM);
void PWM_StepDim();
void Update_PWM_Offset(unsigned char i);
void SwLightOn(unsigned char i, unsigned int relBrightness);
void SwAllLightOn();
void SwLightOff(unsigned char i);
void SwAllLightOff();
void PWM_SetupDim(unsigned char i, signed int PWM_dimsteps, signed int Steps);
void ToggleFocus();

#endif /* SETBRIGHTNESS_H_ */
