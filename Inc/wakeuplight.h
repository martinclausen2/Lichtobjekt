/*
 * wakeuplight.h
 *
 *  Created on: 14.04.2023
 *      Author: Martin
 */

#ifndef INC_WAKEUPLIGHT_H_
#define INC_WAKEUPLIGHT_H_

#include "setbrightness.h"
#include "settings.h"
#include "rtc.h"
#include "RC5.h"

#define timerfrequency 16000000
#define timerprescaler 100
#define timerreload 1600
#define cases 4
#define callsinsecond timerfrequency/timerprescaler/timerreload/cases
#define callsinminute callsinsecond*secondsinminute

#define alarmtimeout 15

#define maxSnooze 30

#define maxTime2Signal 100

#define AlarmBrightnessStep 1

//check if any alarm is set to be executed NOW
void CheckAlarm();

void AlarmSnooze();

void AlarmEnd();

//wake-up light dimming one channel
void Alarm_StepDim(unsigned char i);

//calculate timer IRQ until acoustic signal is triggered
unsigned int GetTime2Singal();

#endif /* INC_WAKEUPLIGHT_H_ */
