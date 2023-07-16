#include <settingscommands.h>

void SettingsCommands_Init()
{
	CLI_AddCmd("bright", BrightnessCmd, 1, TMC_None, "set brightness values - [type] <-c channel_no> <-b brightness_value>");
	CLI_AddCmd("getextbright", GetExtBrightCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "getextbright");
	CLI_AddCmd("remote", RemoteControlCmd, 0, TMC_None, "set infrared remote parameters - <-a address> <-rm receiver mode> <-sm sender mode> <-s send command>");
	CLI_AddCmd("time", SetTimeCmd, 3, TMC_None, "set time of RTC - [hour] [minute] [second]");
	CLI_AddCmd("date", SetDateCmd, 4, TMC_None, "set date of RTC - [2 digit year] [month] [day] [w] with w weekday");
	CLI_AddCmd("timestamp", GetTimestampCmd, 0, TMC_None, "get date and time from RTC");
	CLI_AddCmd("alarmschedule", AlarmScheduleCmd, 1, TMC_None, "set alarm schedule - [alarm no] <-w weekday> <-h hour> <-m minute>");
	CLI_AddCmd("alarmsetting", AlarmSettingsCmd, 0, TMC_None, "set alarm parameters - <-f time to fade-in light>");
	CLI_AddCmd("alarm", AlarmCmd, 0, TMC_None, "trigger, reset, set alarm skip count - <-a 1 | 0> <-s alarm skip count>");
	CLI_AddCmd("power", PowerCmd, 0, TMC_None, "switch light set min external power and battery voltage and calibration- <-l 1 | 0>\r\n<-ub battery voltage> <-mub minimum battery voltage>\r\n<-up external power voltage> <-mup minimum external power voltage difference>");
	CLI_AddCmd("reset", ResetSettingsCmd, 0, TMC_None, "reset settings to factory defaults");
	CLI_AddCmd("statusled", StatusLEDCmd, 1, TMC_None, "flash status led  - [flash count]");
	CLI_AddCmd("fadelight", FadeLightCmd, 0, TMC_None, "mood light - <-f 1 | 0> time - <-t time> <-b brightness> <-mb maximum brightness>");
}

void printValueArray(unsigned char(*values)[])
{
	int i;
	for (i = 0; i < maxChannel;	i++)
	{
		CLI_Printf("\r\nchannel: %d value: %d", i, (int)*(*values+i));
	};
}

void printIntValueArray(unsigned int(*values)[])
{
	int i;
	for (i = 0; i < maxChannel;	i++)
	{
		CLI_Printf("\r\nchannel: %d value: %d", i, (int)*(*values+i));
	};
}


char* printMinuteOff(char* str, unsigned char Value)
{
	if (0==Value)
		{
		strcpy(str, "off");
		}
	else if (1==Value)
		{
		strcpy(str, "instantly");
		}
	else if (2==Value)
		{
		strcpy(str, "1 min.");
		}
	else
		{
		sprintf(str, "%2d min.", Value-1);
		}
	return &(str[0]);
}


// ***************** implementation commands ****************

uint8_t BrightnessCmd()
{
	uint32_t channel_no = 0;
	uint32_t brightness_value = 0;

	// be sure arguments
	uint32_t type = CLI_GetArgHex(0);

	// optional arguments
	bool cflag = CLI_GetArgDecByFlag("-c", &channel_no) & (channel_no < maxChannel);
	bool bflag = CLI_GetArgDecByFlag("-b", &brightness_value) & (brightness_value <= maxBrightnessLimit);

	switch (type)
	{
		case 0:
			CLI_Printf("\r\nBrightness last used before lights were switched off.");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->Brightness_start[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->Brightness_start));
			break;
		case 1:
			CLI_Printf("\r\nMinimum brightness when switched on:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->minBrightness[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->minBrightness));
			break;
		case 2:
			CLI_Printf("\r\nMaximum brightness any time:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->maxBrightness[channel_no]=brightness_value;
			}
			Init_FadeLight();
			printValueArray(&(GLOBAL_settings_ptr->maxBrightness));
			break;
		case 3:
			CLI_Printf("\r\nMaximum brightness when an alarm is triggered:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->AlarmBrightness[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->AlarmBrightness));
			break;
		case 4:
			CLI_Printf("\r\nPWM offset for brightness:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->PWM_Offset[channel_no]=brightness_value;
				PWM_SetupDim(channel_no, fadetime, 0, 0);
			}
			printIntValueArray(&(GLOBAL_settings_ptr->PWM_Offset));
			break;
		case 5:
			CLI_Printf("\r\nCurrent brightness:");
			if (cflag & bflag)
			{
				Brightness[channel_no]=brightness_value;
				PWM_SetupDim(channel_no, fadetime, 0, 0);
			}
			printValueArray(&Brightness);
			break;
		default:
			return TE_ArgErr;
			break;
	}

	SettingsWrite();

	return TE_OK;
}

uint8_t GetExtBrightCmd()
{
	CLI_Printf("\r\nExternal brightness: %10d", (int) (extBrightness >> 6));
	return TE_OK;
}

uint8_t RemoteControlCmd()
{
	uint32_t address = 0;
	uint32_t receiverMode = 0;
	uint32_t senderMode = 0;
	uint32_t send = 0;

	// optional arguments
	if (CLI_GetArgDecByFlag("-a", &address) & (address <= maxRC5Addr))
	{
		GLOBAL_settings_ptr->RC5Addr = (uint8_t)address;
	}
	if (CLI_GetArgDecByFlag("-rm", &receiverMode) & (receiverMode <= maxComMode))
	{
		GLOBAL_settings_ptr->ReceiverMode = (uint8_t)receiverMode;
	}
	if (CLI_GetArgDecByFlag("-sm", &senderMode) & (senderMode <= maxComMode))
	{
		GLOBAL_settings_ptr->SenderMode = (uint8_t)senderMode;
	}
	if (CLI_GetArgDecByFlag("-s", &send))
	{
		SendRC5(RC5Addr_com, send, 1, ComModeOff, RC5Cmd_Repeats);
	}

	CLI_Printf("\r\nRC5 address: %d\r\nreceiver mode: %d %s\r\nsender mode: %d %s",
			(int)GLOBAL_settings_ptr->RC5Addr,
			(int)GLOBAL_settings_ptr->ReceiverMode,
			ComModetext[GLOBAL_settings_ptr->ReceiverMode],
			(int)GLOBAL_settings_ptr->SenderMode,
			ComModetext[GLOBAL_settings_ptr->SenderMode]);

	SettingsWrite();

	return TE_OK;
}

uint8_t SetTimeCmd()
{
	RTC_TimeTypeDef sTimeRtcTemp;

	sTimeRtcTemp.Hours = CLI_GetArgDec(0);
	sTimeRtcTemp.Minutes = CLI_GetArgDec(1);
	sTimeRtcTemp.Seconds = CLI_GetArgDec(2);

	if (IS_RTC_HOUR24(sTimeRtcTemp.Hours) &
			IS_RTC_MINUTES(sTimeRtcTemp.Minutes) &
			IS_RTC_SECONDS(sTimeRtcTemp.Seconds))
	{
		//copy over values being assigned
		timeRtc.Hours = sTimeRtcTemp.Hours;
		timeRtc.Minutes = sTimeRtcTemp.Minutes;
		timeRtc.Seconds = sTimeRtcTemp.Seconds;
		Rtc_SetTime();
		GetTimestampCmd();
		return TE_OK;
	}
	return TE_ArgErr;
}

uint8_t SetDateCmd()
{
	RTC_DateTypeDef sDateRtcTemp;

	sDateRtcTemp.Year = CLI_GetArgDec(0);
	sDateRtcTemp.Month = CLI_GetArgDec(1);
	sDateRtcTemp.Date = CLI_GetArgDec(2);
	sDateRtcTemp.WeekDay = CLI_GetArgDec(3);

	if (IS_RTC_YEAR(sDateRtcTemp.Year) &
			IS_RTC_MONTH(sDateRtcTemp.Month) &
			IS_RTC_DATE(sDateRtcTemp.Date) &
			IS_RTC_WEEKDAY(sDateRtcTemp.WeekDay))
	{
		dateRtc = sDateRtcTemp;
		Rtc_SetDate();
		GetTimestampCmd();
		return TE_OK;
	}
	return TE_ArgErr;
}

uint8_t GetTimestampCmd()
{
	Rtc_GetDateTime();
	CLI_Printf("\r\nTimestamp: %02d-%02d-%02d %s %02d:%02d:%02d",
			dateRtc.Year, dateRtc.Month, dateRtc.Date, WeekdayNames[dateRtc.WeekDay],
			timeRtc.Hours, timeRtc.Minutes, timeRtc.Seconds)
	return TE_OK;
}

uint8_t AlarmSettingsCmd()
{
	uint32_t fading = 0;
	char strf[12];

	// optional arguments
	if (CLI_GetArgDecByFlag("-f", &fading))
	{
		GLOBAL_settings_ptr->LightFading = (uint8_t)fading;
	}

	SettingsWrite();

	CLI_Printf("\r\ntime to fade-in light: %s", printMinuteOff(strf, GLOBAL_settings_ptr->LightFading));

	return TE_OK;
}

uint8_t AlarmScheduleCmd()
{
	uint32_t weekday = 0;
	uint32_t hour = 0;
	uint32_t minute = 0;

	// be sure arguments
	uint32_t i = CLI_GetArgDec(0);

	if (i <= maxAlarm)
	{
		// optional arguments
		// weekday 0 = off, weekday 8 = any weekday
		bool wflag = CLI_GetArgDecByFlag("-w", &weekday) & (weekday <= 8);
		bool bflag = CLI_GetArgDecByFlag("-h", &hour) & (hour < 24);
		// in case minutes are not set the full hour is assumed
		CLI_GetArgDecByFlag("-m", &minute);
		bool mflag = minute < 60;

		if (wflag & bflag & mflag)
		{
			GLOBAL_settings_ptr->Alarm[i].weekday = weekday;
			GLOBAL_settings_ptr->Alarm[i].hour = hour;
			GLOBAL_settings_ptr->Alarm[i].minute = minute;
		}

		CLI_Printf("\r\nno\tweekday\t\ttime");

		for (i = 0; i < maxAlarm;	i++)
		{
			CLI_Printf("\r\n%d\t%d %s\t%02d:%02d",
				(int)i,
				(int)GLOBAL_settings_ptr->Alarm[i].weekday,
				WeekdayNames[GLOBAL_settings_ptr->Alarm[i].weekday],
				(int)GLOBAL_settings_ptr->Alarm[i].hour,
				(int)GLOBAL_settings_ptr->Alarm[i].minute);
		}

		SettingsWrite();
		Rtc_SetAlarm();

		return TE_OK;
	}
	return TE_ArgErr;
}

uint8_t AlarmCmd()
{
	uint32_t skipcnt = 0;
	uint32_t set = 0;

	// optional arguments
	if (CLI_GetArgDecByFlag("-s", &skipcnt) & (skipcnt <=alarmState.maxskipAlarmCnt))
	{
		alarmState.skipAlarmCnt = skipcnt;
		Rtc_SetAlarm();
	}

	if (CLI_GetArgDecByFlag("-a", &set) & (set <=1))
	{
		alarmState.alarmTrigger = (set == 1);
	}

	if (noPendingAlarm == alarmState.nextAlarmIndex)
	{
		CLI_Printf("\r\nNo pending alarm.");
	}
	else
	{
		Rtc_GetAlarm();
		CLI_Printf("\r\nNext alarm no %d\r\nweekday: %d %s\r\ntime: %02d:%02d",
				(int)alarmState.nextAlarmIndex,
				(int)alarmRtc.AlarmDateWeekDay,
				WeekdayNames[alarmRtc.AlarmDateWeekDay],
				(int)alarmRtc.AlarmTime.Hours,
				(int)alarmRtc.AlarmTime.Minutes);
		CLI_Printf("\r\nSkipping #%d, current maximum skip count %d", alarmState.skipAlarmCnt, alarmState.maxskipAlarmCnt);
	}
	return TE_OK;
}


uint8_t PowerCmd()
{
	uint32_t light = 0;
	uint32_t ref_uBat = 0;
	uint32_t ref_uPwr = 0;
	uint32_t min_uBat = 0;
	uint32_t min_uPwr = 0;

	if (CLI_GetArgDecByFlag("-l", &light))
	{
		if (light == 0)
		{
			SwAllLightOff();
		}
		else
		{
			SwAllLightOn();
		}
	}

	// user provided value in mV
	// ADC value in bit
	// calibration value in mV / bit

	if (CLI_GetArgDecByFlag("-ub", &ref_uBat))
	{
		GLOBAL_settings_ptr->cal_uBat = (ref_uBat<<calibarationScaleBits)/uBat;
	}

	if (CLI_GetArgDecByFlag("-up", &ref_uPwr))
	{
		GLOBAL_settings_ptr->cal_uPwr = (ref_uPwr<<calibarationScaleBits)/uPwr;
	}

	if (CLI_GetArgDecByFlag("-mub", &min_uBat))
	{
		GLOBAL_settings_ptr->min_uBat = (min_uBat<<calibarationScaleBits)/(GLOBAL_settings_ptr->cal_uBat);
	}

	if (CLI_GetArgDecByFlag("-mup", &min_uPwr))
	{
		GLOBAL_settings_ptr->min_uPwr = (min_uPwr<<calibarationScaleBits)/(GLOBAL_settings_ptr->cal_uPwr);
	}

	if (LightOn)
	{
		CLI_Printf("\r\nLight on.");
	}
	else
	{
		CLI_Printf("\r\nLight off.");
	}

	CLI_Printf("\r\nBattery: %d raw, %dmV min battery: %dmV.", (int)uBat, (int)((uBat*(GLOBAL_settings_ptr->cal_uBat))>>calibarationScaleBits), (int)(((GLOBAL_settings_ptr->min_uBat)*(GLOBAL_settings_ptr->cal_uBat))>>calibarationScaleBits) );
	CLI_Printf("\r\nExternal power: %d raw, %dmV.", (int)uPwr, (int)((uPwr*(GLOBAL_settings_ptr->cal_uPwr))>>calibarationScaleBits));
	CLI_Printf("\r\nMin external power to battery difference: %dmV.", (int)(((GLOBAL_settings_ptr->min_uPwr)*(GLOBAL_settings_ptr->cal_uPwr))>>calibarationScaleBits) );

	if 	(HAL_GPIO_ReadPin(Charging_Status_GPIO_Port, Charging_Status_Pin))
	{
		CLI_Printf("\r\nCharging.");
	}

	SettingsWrite();

	return TE_OK;
}

uint8_t ResetSettingsCmd()
{
	CLI_Printf("\r\nReset settings to factory defaults.");
	SettingsReset2Defaults();

	Rtc_SetAlarm();

	return TE_OK;
}

uint8_t StatusLEDCmd()
{
	// be sure arguments
	uint32_t flashCount = CLI_GetArgDec(0);
	if (flashCount <= LEDFlashMaxSeq)
	{
		LEDSetupOptions(flashCount);
		return TE_OK;
	}
	else
	{
		return TE_ArgErr;
	}
}

uint8_t FadeLightCmd()
{
	uint32_t time = 0;
	uint32_t status = 0;
	uint32_t brightness = 0;

	// optional arguments
	if (CLI_GetArgDecByFlag("-t", &time))
	{
		GLOBAL_settings_ptr->FadingTime = time;
	}

	if (CLI_GetArgDecByFlag("-b", &brightness) & (brightness <= maxBrightnessLimit))
	{
		Brightness[FadeLightChannel] = brightness;
	}

	if (CLI_GetArgDecByFlag("-mb", &brightness) & (brightness <= maxBrightnessLimit))
	{
		GLOBAL_settings_ptr->maxBrightness[FadeLightChannel] = brightness;
	}

	// optional arguments
	if (CLI_GetArgDecByFlag("-f", &status))
	{
		if(status)
		{
			FadeLight();
			FocusChannel = FadeLightChannel;
		}
		else
		{
			ResetFadeLight();
			FocusChannel = 0;
		}
	}

	CLI_Printf("\r\nFading time: %d s", (int)GLOBAL_settings_ptr->FadingTime);
	CLI_Printf("\r\nRelative brightness: %d", (int)Brightness[FadeLightChannel]);
	CLI_Printf("\r\nMax relative brightness: %d", (int)GLOBAL_settings_ptr->maxBrightness[FadeLightChannel]);

	SettingsWrite();

	return TE_OK;
}
