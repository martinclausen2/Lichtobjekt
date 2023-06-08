/*
 * fadelight.h
 *
 *  Created on: May 20, 2023
 *      Author: Martin
 */

#ifndef INC_FADELIGHT_H_
#define INC_FADELIGHT_H_

#include "setbrightness.h"
#include "settings.h"

void Init_FadeLight();
void FadeLight();
void ResetFadeLight();

int FadeLight_StepDim(unsigned int i);

#endif /* INC_FADELIGHT_H_ */
