/*
 * settings.h
 *
 *  Created on: Dec 27, 2022
 *      Author: Martin
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "stm32l1xx_hal.h"
#include <assert.h>

#define maxChannel 2				//start counting at 1
#define maxVirtualChannel 3			//start counting at 1, including channel from mood light
#define FadeLightChannel 2			//start counting at 0, virutalChannel for mood light
#define maxBrightnessLimit 0xFF
#define maxBeepVolume 0xFF
#define maxRC5Addr 31
#define maxComMode 3
#define maxLightFading 30
#define minLCDContrast 0x05
#define maxLCDContrast 0x0F
#define maxAlarm 7
#define maxWeekday 8

static const char __attribute__((unused)) *WeekdayNames[10] = {"off","Monday","Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "all"};

static const char __attribute__((unused)) *ComModetext[10] = {"Off", "Alarm", "Condional", "All"};

#define ComModeOff	0
#define ComModeAlarm	1
#define ComModeConditional	2
#define ComModeAll	3

typedef struct {
	unsigned char weekday;
	unsigned char hour;
	unsigned char minute;
} alarm_t;

typedef struct {
	uint32_t version;
	uint32_t revision;
	unsigned char RC5Addr;								//IR remote control address
	unsigned char ReceiverMode;							//Mode for acting on commands from other devices
	unsigned char SenderMode;							//Mode for sending commands to other devices
	unsigned char LCDContrast;							//LCD contrast setting
	unsigned int ExtBrightness_last;					//external brightness during lights off divided by 256
	unsigned char Brightness_start[maxVirtualChannel];	//value before lights off
	unsigned char minBrightness[maxChannel];			//minimum brightness after power on and recalculation using measured brightness
	unsigned char maxBrightness[maxVirtualChannel];		//maximum brightness
	unsigned char AlarmBrightness[maxChannel];			//maximum brightness targeted during alarm
	unsigned int PWM_Offset[maxChannel];				//PWM value, where the driver effectively starts to generate an output
	unsigned char LightFading;							//Minutes to fade light in
	unsigned char AlarmTime2Signal;						//Delay after alarm until noise is being generated
	unsigned char AlarmTimeSnooze;						//Snooze Time
	unsigned int FadingTime;							//Seconds to fade from one color setting to next
	unsigned char BeepVolume;							//Volume of the key beep
	alarm_t Alarm[maxAlarm];
	uint32_t crc32;
} settings_t;

extern settings_t *GLOBAL_settings_ptr;

void SettingsInit(CRC_HandleTypeDef *handle_crc);

void SettingsRead(void);
uint32_t SettingsWrite(void);
void SettingsReset2Defaults(void);

#endif /* INC_SETTINGS_H_ */
