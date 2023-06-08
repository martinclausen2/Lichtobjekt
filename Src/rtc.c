#include "rtc.h"

RTC_TimeTypeDef timeRtc = {0};
RTC_DateTypeDef dateRtc = {0};
RTC_AlarmTypeDef alarmRtc = {.Alarm = RTC_ALARM_A,
								.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY};

alarmState_t alarmState = {0};

HAL_StatusTypeDef Rtc_Init()
{
	pRTC_CallbackTypeDef pCallback = *Rtc_AlarmAEventCallback;
	HAL_RTC_RegisterCallback(&hrtc, HAL_RTC_ALARM_A_EVENT_CB_ID , pCallback);
	return Rtc_SetAlarm();
}

HAL_StatusTypeDef Rtc_GetDateTime()
{
	return HAL_ERROR & (HAL_RTC_GetTime(&hrtc, &timeRtc, RTC_FORMAT_BIN)
			| HAL_RTC_GetDate(&hrtc, &dateRtc, RTC_FORMAT_BIN));
}

HAL_StatusTypeDef Rtc_SetDate()
{
	return HAL_ERROR & (HAL_RTC_SetDate(&hrtc, &dateRtc, RTC_FORMAT_BIN)
			| Rtc_SetAlarm());
}

HAL_StatusTypeDef Rtc_SetTime()
{
	return HAL_ERROR & (HAL_RTC_SetTime(&hrtc, &timeRtc, RTC_FORMAT_BIN)
			| Rtc_SetAlarm());
}

HAL_StatusTypeDef Rtc_GetAlarm()
{
	return HAL_RTC_GetAlarm(&hrtc, &alarmRtc, RTC_ALARM_A, RTC_FORMAT_BIN);
}

HAL_StatusTypeDef Rtc_SetAlarm()
{
	Find_NextAlarm();
	HAL_StatusTypeDef status = HAL_RTC_DeactivateAlarm(&hrtc, alarmRtc.Alarm);
	if (noPendingAlarm != alarmState.nextAlarmIndex)
	{
		alarmRtc.AlarmDateWeekDay = GLOBAL_settings_ptr->Alarm[alarmState.nextAlarmIndex].weekday;
		alarmRtc.AlarmTime.Hours = GLOBAL_settings_ptr->Alarm[alarmState.nextAlarmIndex].hour;
		alarmRtc.AlarmTime.Minutes = GLOBAL_settings_ptr->Alarm[alarmState.nextAlarmIndex].minute;
		status |= HAL_RTC_SetAlarm_IT(&hrtc, &alarmRtc, RTC_FORMAT_BIN);
	}
	return HAL_ERROR & status;
}

void Rtc_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	alarmState.alarmTrigger = true;
}

//Checks for the next alarm, returns -1 if no alarm was found
//considers skipAlarmCnt, not fast, but memory effective
void Find_NextAlarm()
{
	unsigned int j;
	unsigned int i;
	signed int curdifference;		//time between alarm and curent time
	signed int mindifference=0;		//maximum difference is 7*24*60 Minutes = 10080 Minutes
									//0 is no alarm on yet found
	signed int curmindifference=0;	//minimum for this skipped alarm iteration, goes up with every skip iteration

	int minAlarm = noPendingAlarm;	//number of the alarm with the smallest difference

	Rtc_GetDateTime();

	for(i=0; i<=alarmState.skipAlarmCnt; i++)
		{
		alarmState.maxskipAlarmCnt=0;
		for(j=0; j<maxAlarm+1; j++)		//go through all alarms
			{
			alarm_t curAlarm = GLOBAL_settings_ptr->Alarm[j];
			if (curAlarm.weekday)			//is this alarm on at all?
				{
				//calculate time difference between now and alarm
				if (alldays==curAlarm.weekday)
					{
					curdifference = 0;
					}
				else
					{
					curdifference = (curAlarm.weekday-dateRtc.WeekDay)*minutesinday;
					}
				curdifference += (curAlarm.hour-timeRtc.Hours)*minutesinhour;
				curdifference += curAlarm.minute-timeRtc.Minutes;
				if (0>curdifference)		//warp into next week
					{
					curdifference+=daysinweek*minutesinday;
					}
				alarmState.maxskipAlarmCnt++;

				// set alarm no if alarm is closer or no alarm is set yet
				if (((mindifference>curdifference) || 0==mindifference) && curmindifference<curdifference)
					{
					mindifference=curdifference;
					minAlarm=j;
					}
				}
			}
		curmindifference=mindifference;			//save this iterations smallest time difference
		mindifference=0;						//reset search
		}
	alarmState.nextAlarmIndex = minAlarm;
}
