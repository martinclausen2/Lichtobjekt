/*
Controls Status RGB LED
*/

#ifndef __StatusLED_H_GUARD
#define __StatusLED_H_GUARD

#include "stm32l1xx_hal.h"

unsigned char LEDFlashCount;		//Number of flashes currently required, should not exceed 0x7F ...
unsigned char LEDFlashSeqCounter;	//Number of flashes * 2 (on & off) to be produced in this sequence
unsigned char LEDLimitFlashTimer;	//Software timer to decouple flashing frequency from calling frequency
unsigned char LEDStandbyTimer;

unsigned char LEDCurrentColor;

#define LEDFlashMaxSeq	8	//max. number of flashes in a squence, should not exceed 0x7F ...
#define LEDFlashMaxSeqSteps LEDFlashMaxSeq << 1
#define LEDmaxFlashTimer 6	//cycles to execute before next flash toggel happens
#define LEDmaxLimitFlashTimer 4	//cycles to execute before LED status is restored
#define LEDmaxStandyTimer 0x3F	//cycles to execute before LED brightness is dimed
#define flashColorMask	0x07

#define maxLimit		2	//upper brightness limit
#define minLimit		1	//lower brightness limit

unsigned char limit;

#define LEDBlank	0
#define LEDWhite	1
#define LEDRed		2
#define LEDBlue		3
#define LEDGreen	4
#define LEDReddim	9

static const unsigned char colorTable[12][3] = { { 0x00, 0x00, 0x00},	//off

					{ 0x7F, 0x7F, 0x7F},	//white

					{ 0x00, 0x00, 0xFF},	//red
					{ 0xFF, 0x00, 0x00},	//blue
					{ 0x00, 0xFF, 0x00},	//green

					{ 0x5F, 0x00, 0xBF},
					{ 0x5F, 0x7F, 0x00},
					{ 0x5F, 0x3F, 0x9F},
					{ 0x5F, 0x8F, 0x4F},

					{ 0x00, 0x00, 0x0F},	//red very dim
					{ 0x00, 0x00, 0x3F},	//red dim
					{ 0x00, 0x00, 0xFF},	//red
			 	        };

static const unsigned char flashColorTable[8] = {LEDBlue, LEDBlank, LEDBlue, LEDBlank, LEDBlue, LEDBlank, LEDRed, LEDBlank};

TIM_HandleTypeDef *htim_StatusPWM;				//handle to address timer

void Status_LED_Init(TIM_HandleTypeDef *htim_StatusPWM);

void LEDOff();

void LEDOn();

void LEDCancel();

void LEDOptions();

void LEDFadeLightOut();

void LEDSetupOptions(unsigned char i);

void LEDValue(unsigned char i);

void LEDSetupStandby();

void LEDStandby();

void LEDSetupLimit();

void LEDLimit();

void LEDSetColor(unsigned char i);

void LEDSetColorTemp(unsigned char i);

#endif
