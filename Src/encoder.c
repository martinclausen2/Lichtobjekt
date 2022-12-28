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
	static short OldDeltaCounter;

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

		//still same direction?
		if ((DeltaCounter<0) == (OldDeltaCounter<0))
			{
			EncoderSteps += DeltaCounter * Acceleration / AccelerationScale;
			++Acceleration;
			}
		else
			{
			Acceleration = 0;
			}
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
	OldDeltaCounter = DeltaCounter;
}

//check encoder and change value within limits, returns true if value was changed
bool EncoderSetupValue(unsigned char *Value, unsigned char maxValue, unsigned char minValue)
{
	signed int temp;
	bool returnval = false;
	// A Rotation occurred if EncoderSteps!= 0
	// EncoderSteps = 0 must be set by receiving program after decoding
	if (EncoderSteps)
		{
		temp = EncoderSteps;
		EncoderSteps = 0;			//ack received steps
		temp += *Value;
		if (maxValue < temp)
			{
			*Value = maxValue;
			}
		else if (minValue > temp)
			{
			*Value = minValue;
			}
		else
			{
			*Value = temp & 0xFF;
			}
		returnval = true;
		}
	return returnval;
}
