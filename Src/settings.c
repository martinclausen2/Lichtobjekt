#include "settings.h"
#include <stm32l1xx.h>
#include <string.h>

#define DATA_EEPROM_START_ADDR     0x08080000
#define DATA_EEPROM_SIZE_BYTES     8192

//_Static_assert(true, "test");
//_Static_assert(sizeof(settings_t) < DATA_EEPROM_SIZE_BYTES, "EEPROM struct too large!");
//_Static_assert(sizeof(settings_t) % 4 == 0, "EEPROM struct has to be multiple of 4 bytes");

CRC_HandleTypeDef *hcrc_settings;						//handle to address CRC

static settings_t _settings_in_ram;

settings_t *GLOBAL_settings_ptr = &_settings_in_ram;

void SettingsInit(CRC_HandleTypeDef *handle_crc)
{
	hcrc_settings = handle_crc;
	SettingsRead();
}

void SettingsRead(void){
	//copy data from EEPROM to RAM
	memcpy(GLOBAL_settings_ptr, (uint32_t*)DATA_EEPROM_START_ADDR, sizeof(settings_t));

	__HAL_RCC_CRC_CLK_ENABLE();

	//calculate new CRC
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

uint32_t SettingsWrite(void){
	GLOBAL_settings_ptr->revision++;

	__HAL_RCC_CRC_CLK_ENABLE();

	//calculate new CRC
	GLOBAL_settings_ptr->crc32 = HAL_CRC_Calculate(
			hcrc_settings,
			(uint32_t *)GLOBAL_settings_ptr,
			(sizeof(settings_t)-sizeof(uint32_t))/sizeof(uint32_t)/*size minus the crc32 at the end, IN WORDS*/
	);

	__HAL_RCC_CRC_CLK_DISABLE();

	HAL_FLASHEx_DATAEEPROM_Unlock();

	uint32_t *src = (uint32_t*)GLOBAL_settings_ptr;
	uint32_t *dst = (uint32_t*)DATA_EEPROM_START_ADDR;

	//write settings word (uint32_t) at a time
	for (uint32_t i = 0; i < sizeof(settings_t)/sizeof(uint32_t); i++){
		if (*dst != *src){ //write only if value has been modified
			HAL_StatusTypeDef s = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, (uint32_t)dst, *src);
			if (s != HAL_OK){
				return FLASH->SR;
			}
		}
		src++;
		dst++;
	}

	HAL_FLASHEx_DATAEEPROM_Lock();

	SettingsRead();
	return 0;
}

void SettingsReset2Defaults(void){
	memset(GLOBAL_settings_ptr, 0, sizeof(settings_t));

	GLOBAL_settings_ptr->revision = 0;
	GLOBAL_settings_ptr->version = 0;
	GLOBAL_settings_ptr->RC5Addr = 0;					//IR remote control address
	GLOBAL_settings_ptr->ReceiverMode = 2;				//Mode for acting on commands from other devices
	GLOBAL_settings_ptr->SenderMode = 2;				//Mode for sending commands to other devices
	GLOBAL_settings_ptr->LCDContrast = 12;				//LCD contrast setting
	GLOBAL_settings_ptr->ExtBrightness_last = 0;		//external brightness during lights off divided by 256
	GLOBAL_settings_ptr->Brightness_start[0] = 0;		//value before lights off
	GLOBAL_settings_ptr->Brightness_start[1] = 0;		//value before lights off
	GLOBAL_settings_ptr->Brightness_start[2] = 0;		//value before lights off
	GLOBAL_settings_ptr->Brightness_start[3] = 0;		//value before lights off
	GLOBAL_settings_ptr->Brightness_start[4] = 0;		//value before lights off
	GLOBAL_settings_ptr->minBrightness[0] = 7;			//minimum brightness after power on and recalculation using measured brightness
	GLOBAL_settings_ptr->minBrightness[1] = 7;			//minimum brightness after power on and recalculation using measured brightness
	GLOBAL_settings_ptr->minBrightness[2] = 7;			//minimum brightness after power on and recalculation using measured brightness
	GLOBAL_settings_ptr->minBrightness[3] = 7;			//minimum brightness after power on and recalculation using measured brightness
	GLOBAL_settings_ptr->maxBrightness[0] = 255;		//maximum brightness
	GLOBAL_settings_ptr->maxBrightness[1] = 255;		//maximum brightness
	GLOBAL_settings_ptr->maxBrightness[2] = 255;		//maximum brightness
	GLOBAL_settings_ptr->maxBrightness[3] = 255;		//maximum brightness
	GLOBAL_settings_ptr->maxBrightness[4] = 255;		//maximum brightness
	GLOBAL_settings_ptr->AlarmBrightness[0] = 128;		//maximum brightness targeted during alarm
	GLOBAL_settings_ptr->AlarmBrightness[1] = 255;		//maximum brightness targeted during alarm
	GLOBAL_settings_ptr->AlarmBrightness[2] =  52;		//maximum brightness targeted during alarm
	GLOBAL_settings_ptr->AlarmBrightness[3] = 180;		//maximum brightness targeted during alarm
	GLOBAL_settings_ptr->PWM_Offset[0] = 0;				//PWM value, where the driver effectively starts to generate an output
	GLOBAL_settings_ptr->PWM_Offset[1] = 0;				//PWM value, where the driver effectively starts to generate an output
	GLOBAL_settings_ptr->PWM_Offset[2] = 0;				//PWM value, where the driver effectively starts to generate an output
	GLOBAL_settings_ptr->PWM_Offset[3] = 0;				//PWM value, where the driver effectively starts to generate an output
	GLOBAL_settings_ptr->LightFading = 16;				//Minutes to fade light in
	GLOBAL_settings_ptr->AlarmTime2Signal = 11;			//Delay after alarm until noise is being generated
	GLOBAL_settings_ptr->AlarmTimeSnooze = 6;			//Snooze Time
	GLOBAL_settings_ptr->FadingTime = 10;				//Seconds to fade from one color setting to next
	GLOBAL_settings_ptr->BeepVolume = 0x40;				//Volume of the key beep

	alarm_t alarms[maxAlarm] = {
			{ 0, 6, 20 },
			{ 0, 6, 20 },
			{ 0, 6, 20 },
			{ 0, 6, 20 },
			{ 0, 6, 20 },
			{ 0, 7, 20 },
			{ 0, 7, 20 }
	};

	memcpy(GLOBAL_settings_ptr->Alarm, alarms, sizeof alarms);
}
