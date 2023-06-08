/*
 * extbrightness.c
 *
 *  Created on: 18.04.2022
 *      Author: Martin
 * Function to obtain brightness
 * requires two volatile variables to store gain setting and value for moving average
 * call frequently
 * divide result by number of data points of the moving average
 * maximum filter size = 2^32 / 2^8 / 1000 = 167777 datapoints
 * result is found in the unsigned long ExtBrightness
 *
 * BRIGHT_LOW_Pin and BRIGHT_HIGH_Pin must be set to high via the device configuration
*/

#include "extbrightness.h"

ADC_HandleTypeDef *hadc_extbrightness;

unsigned long extBrightness = 0;	//will be filled up to 24 bit with 64 datapoint moving average

PhotoGain_t PhotoGain;

/**amplification factors of photoamp */
const unsigned int photoampfactor[] = {1, 30, 1000};

void Init_ExtBrightness(ADC_HandleTypeDef *handle_adc)
{
	hadc_extbrightness = handle_adc;

	pADC_CallbackTypeDef pCallback = AddValue_ExtBrightness;
	HAL_ADC_RegisterCallback(hadc_extbrightness, HAL_ADC_CONVERSION_COMPLETE_CB_ID, pCallback);

	//Set both pins to high to the can be fake pull up outputs by changing the output mode.
	HAL_GPIO_WritePin(BRIGHT_LOW_GPIO_Port, BRIGHT_LOW_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BRIGHT_HIGH_GPIO_Port, BRIGHT_HIGH_Pin, GPIO_PIN_SET);
}

void Sample_ExtBrightness()
{
    HAL_ADC_Start_IT(hadc_extbrightness);
}

// set pin to output or analog input to imitate open source output not available on STM32
void SetPinStatus(bool pin_status, GPIO_InitTypeDef *GPIO_InitStruct) {
	if (pin_status) {
		GPIO_InitStruct->Mode = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(GPIOB, &*GPIO_InitStruct);
	} else {
		GPIO_InitStruct->Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOB, &*GPIO_InitStruct);
	}
}

// Adjust gain setting
void AdjustGain(PhotoGain_t PhotoGain) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = BRIGHT_LOW_Pin;
	_Bool pin_status = PhotoGain.LSB;
	SetPinStatus(pin_status, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = BRIGHT_HIGH_Pin;
	pin_status = PhotoGain.MSB;
	SetPinStatus(pin_status, &GPIO_InitStruct);
}

void AddValue_ExtBrightness(ADC_HandleTypeDef *handle_adc)
{
	// check if value was created by sampling the brightness ADC input channel
	if (handle_adc->Instance == hadc_extbrightness->Instance && handle_adc->NbrOfConversionRank == extbrightness_ADC_RANK)
	{
		int ADC_Result = maxADCvalue - HAL_ADC_GetValue(hadc_extbrightness);
		extBrightness -= (extBrightness >> 6) & 0x0FFFFFFF;		//in the meantime: remove a 1/64 so we have a moving average over 64 data points
		extBrightness += ADC_Result * photoampfactor[PhotoGain.ALL];
		if ((maxphotoamp < ADC_Result) && (maxphotogain > PhotoGain.ALL))
			{
			++PhotoGain.ALL;
	    	AdjustGain(PhotoGain);
			}
		else if ((minphotoamp > ADC_Result) && (minphotogain < PhotoGain.ALL))
			{
			--PhotoGain.ALL;
	    	AdjustGain(PhotoGain);
			}
	}
}

