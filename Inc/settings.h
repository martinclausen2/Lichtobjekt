/*
 * settings.h
 *
 *  Created on: 15.04.2022
 *      Author: Martin
 *      Taken from https://m0agx.eu/2018/04/15/using-the-internal-eeprom-of-stm32l/
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

//#include <stdio.h>
#include <stdbool.h>
//#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "stm32l1xx_hal.h"
#include <assert.h>

typedef struct {
	unsigned char weekday;
	unsigned char hour;
	unsigned char minute;
} alarm_t;

typedef struct {
  uint32_t version;
  uint32_t revision;
  unsigned char RC5Addr;				//IR remote control address
  unsigned char ReceiverMode;			//Mode for acting on commands from other devices
  unsigned char SenderMode;				//Mode for sending commands to other devices
  unsigned char LCDContrast;			//LCD contrast setting
  unsigned int ExtBrightness_last;		//external brightness during lights off divided by 256
  unsigned char Brightness_start[4];	//value before lights off
  unsigned char minBrightness[4];		//minimum brightness after power on and recalculation using measured brightness
  unsigned char AlarmBrightness[4];		//maximum brightness targeted during alarm
  unsigned int PWM_Offset[4];			//PWM value, where the driver effectively starts to generate an output
  unsigned char LightFading;			//Minutes to fade light in
  unsigned char AlarmTime2Signal;		//Delay after alarm until noise is being generated
  unsigned char AlarmTimeSnooze;		//Snooze Time
  unsigned char BeepVolume;				//Volume of the key beep
  alarm_t Alarm[7];
  uint32_t crc32;
} settings_t;

extern settings_t *GLOBAL_settings_ptr;

void Settings_Init(CRC_HandleTypeDef *hcrc_settings);
void Settings_Read(void);
uint32_t Settings_Write(void);

#endif /* INC_SETTINGS_H_ */



