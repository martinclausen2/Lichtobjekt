/*
 * encoder.c
 *
 *  Created on: 23.12.2019
 *      Author: Martin
 */

#include "encoder.h"

volatile int EncoderSteps;

TIM_HandleTypeDef *htim_encoder;						//handle to address timer

void Encoder_Init(TIM_HandleTypeDef *handle_tim)
{
	htim_encoder = handle_tim;
	HAL_TIM_Encoder_Start(htim_encoder, TIM_CHANNEL_ALL);
}

void Encoder()
{
	static unsigned short OldCounter;
	static short Acceleration;
	static unsigned short Decay;
	unsigned short Counter = htim_encoder->Instance->CNT;
	short DeltaCounter;

	DeltaCounter = Counter - OldCounter;
	//handle warp around
	if (DeltaCounter > MaxChange)
	{
		// counter underflow
		DeltaCounter -= MaxCounter;
	}
	else if (DeltaCounter < -MaxChange)
	{
		// counter overflow
		DeltaCounter += MaxCounter;
	}

	//Measure the number of steps, introduce acceleration
	if (DeltaCounter != 0)
		{
		EncoderSteps += DeltaCounter;
		EncoderSteps += DeltaCounter * Acceleration / AccelerationScale;
		++Acceleration;
		}
	else if (Decay > 0)
		{
		--Decay;
		}
	else
		{
		Decay=startDecay;
		if (Acceleration)
			{
			--Acceleration;			//no new step, decrease acceleration
			}
		}

	OldCounter = Counter;
}
