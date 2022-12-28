/*
 * extbrightness.h
 *
 *  Created on: 18.04.2022
 *      Author: Martin
 */

#ifndef EXTBRIGHTNESS_H_
#define EXTBRIGHTNESS_H_

#include "stm32l1xx_hal.h"
#include "main.h"

#define extbrightness_ADC_RANK 1

#define minphotoamp		 100
#define maxphotoamp		3000

//must match bit field below
#define minphotogain	0b00
#define maxphotogain	0b10

// define the bit field
typedef union {
	struct {
		unsigned ALL:8;
	};
	struct {
		unsigned LSB:1;
		unsigned MSB:1;
		unsigned free:6;
	};
} PhotoGain_t;

extern unsigned long ExtBrightness;

/**amplification factors of photoamp */
extern const unsigned int photoampfactor[];

void Init_ExtBrightness(ADC_HandleTypeDef *handle_adc);
void Sample_ExtBrightness();
void AddValue_ExtBrightness(ADC_HandleTypeDef* handle_adc);
unsigned long Get_ExtBrightness();

#endif /* EXTBRIGHTNESS_H_ */
