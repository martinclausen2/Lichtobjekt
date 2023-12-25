/*
 * encoder.h
 *
 *  Created on: 23.12.2019
 *      Author: Martin
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdbool.h>
#include "stm32l1xx_hal.h"

#define AccelerationScale	0x03

//start value for decrementing acceleration, depends on calling frequency
#define startDecay	0x007

// values to correct warp around of counter
#define MaxChange 0x0FFF
#define MaxCounter 0xFFFF

extern volatile int EncoderSteps;

void Encoder_Init(TIM_HandleTypeDef *htim_encoder);

void Encoder();

bool EncoderSetupValue(unsigned char *Value, unsigned char maxValue, unsigned char minValue);

#endif /* ENCODER_H_ */
