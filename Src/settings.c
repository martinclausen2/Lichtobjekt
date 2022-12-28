/*
 * settings.c
 *
 *  Created on: 15.04.2022
 *      Author: Martin
 *      Taken from https://m0agx.eu/2018/04/15/using-the-internal-eeprom-of-stm32l/
 */


#include "settings.h"

#define DATA_EEPROM_START_ADDR     0x08080000
#define DATA_EEPROM_SIZE_BYTES     4096 // 8192 only with STM32L15xxC

//Static_assert(sizeof(settings_t) < DATA_EEPROM_SIZE_BYTES, "EEPROM struct too large!");
//Static_assert(sizeof(settings_t) % 4 == 0, "EEPROM struct has to be multiple of 4 bytes");

CRC_HandleTypeDef *hcrc_settings;				//handle to address crc

static settings_t _settings_in_ram;

settings_t *GLOBAL_settings_ptr = &_settings_in_ram;

static void SettingsReset2Defaults(void);

void Settings_Init(CRC_HandleTypeDef *handle_crc)
{
	hcrc_settings = handle_crc;
	Settings_Read();
}

void Settings_Read(void){
    //copy data from EEPROM to RAM
    memcpy(GLOBAL_settings_ptr, (uint32_t*)DATA_EEPROM_START_ADDR, sizeof(settings_t));

    __HAL_RCC_CRC_CLK_ENABLE();

    uint32_t computed_crc = HAL_CRC_Calculate(
    		hcrc_settings,
            (uint32_t *)GLOBAL_settings_ptr,
            (sizeof(settings_t)-sizeof(uint32_t))/sizeof(uint32_t)/*size minus the crc32 at the end, IN WORDS*/
    );

    __HAL_RCC_CRC_CLK_DISABLE();

    if (computed_crc != GLOBAL_settings_ptr->crc32){
        SettingsReset2Defaults();
    }
}

uint32_t Settings_Write(void){
    GLOBAL_settings_ptr->revision++;

    __HAL_RCC_CRC_CLK_ENABLE();

    GLOBAL_settings_ptr->crc32 = HAL_CRC_Calculate( //calculate new CRC
    		hcrc_settings,
			(uint32_t *)GLOBAL_settings_ptr,
            (sizeof(settings_t)-sizeof(uint32_t))/sizeof(uint32_t)/*size minus the crc32 at the end, IN WORDS*/
    );

    __HAL_RCC_CRC_CLK_DISABLE();

    HAL_FLASHEx_DATAEEPROM_Unlock();  //Unprotect the EEPROM to allow writing

    uint32_t *src = (uint32_t*)GLOBAL_settings_ptr;
    uint32_t *dst = (uint32_t*)DATA_EEPROM_START_ADDR;

    //write settings word (uint32_t) at a time
    for (uint32_t i = 0; i < sizeof(settings_t)/sizeof(uint32_t); i++){
        if (*dst != *src){ //write only if value has been modified
        	HAL_StatusTypeDef status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, (uint32_t)dst, *src);
            if (status != HAL_OK){
                return FLASH->SR;
            }
        }
        src++;
        dst++;
    }

    HAL_FLASHEx_DATAEEPROM_Lock();  // Reprotect the EEPROM

    Settings_Read();
    return 0;
}

static void SettingsReset2Defaults(void){
    memset(GLOBAL_settings_ptr, 0, sizeof(settings_t));

    GLOBAL_settings_ptr->revision = 0;
    GLOBAL_settings_ptr->version = 0;
    GLOBAL_settings_ptr->RC5Addr = 0;					//IR remote control address
    GLOBAL_settings_ptr->ReceiverMode = 2;				//Mode for acting on commands from other devices
    GLOBAL_settings_ptr->SenderMode = 2;				//Mode for sending commands to other devices
    GLOBAL_settings_ptr->LCDContrast = 12;				//LCD contrast setting
    GLOBAL_settings_ptr->ExtBrightness_last = 0;		//external brightness during lights off divided by 256
    GLOBAL_settings_ptr->Brightness_start[0] = 0;		//value before lights off
    GLOBAL_settings_ptr->Brightness_start[1] = 1;		//value before lights off
    GLOBAL_settings_ptr->Brightness_start[2] = 2;		//value before lights off
    GLOBAL_settings_ptr->Brightness_start[3] = 3;		//value before lights off
    GLOBAL_settings_ptr->minBrightness[0] = 7;			//minimum brightness after power on and recalculation using measured brightness
    GLOBAL_settings_ptr->minBrightness[1] = 7;			//minimum brightness after power on and recalculation using measured brightness
    GLOBAL_settings_ptr->minBrightness[2] = 7;			//minimum brightness after power on and recalculation using measured brightness
    GLOBAL_settings_ptr->minBrightness[3] = 7;			//minimum brightness after power on and recalculation using measured brightness
    GLOBAL_settings_ptr->AlarmBrightness[0] = 0x80;		//maximum brightness targeted during alarm
	GLOBAL_settings_ptr->AlarmBrightness[1] = 0x80;		//maximum brightness targeted during alarm
	GLOBAL_settings_ptr->AlarmBrightness[2] = 0x80;		//maximum brightness targeted during alarm
    GLOBAL_settings_ptr->AlarmBrightness[3] = 0x80;		//maximum brightness targeted during alarm
    GLOBAL_settings_ptr->PWM_Offset[0] = 0;				//PWM value, where the driver effectively starts to generate an output
    GLOBAL_settings_ptr->PWM_Offset[1] = 0;				//PWM value, where the driver effectively starts to generate an output
    GLOBAL_settings_ptr->PWM_Offset[2] = 0;				//PWM value, where the driver effectively starts to generate an output
    GLOBAL_settings_ptr->PWM_Offset[3] = 0;				//PWM value, where the driver effectively starts to generate an output
    GLOBAL_settings_ptr->LightFading = 16;				//Minutes to fade light in
    GLOBAL_settings_ptr->AlarmTime2Signal = 11;			//Delay after alarm until noise is being generated
    GLOBAL_settings_ptr->AlarmTimeSnooze = 6;			//Snooze Time
    GLOBAL_settings_ptr->BeepVolume = 0x40;				//Volume of the key beep

    alarm_t alarms[7] = {
    		{ 0, 6, 20 },
    		{ 1, 6, 20 },
    		{ 2, 6, 20 },
    		{ 3, 6, 20 },
    		{ 4, 6, 20 },
    		{ 5, 7, 20 },
    		{ 6, 7, 20 }
    };

    memcpy(GLOBAL_settings_ptr->Alarm, alarms, sizeof alarms);
}
