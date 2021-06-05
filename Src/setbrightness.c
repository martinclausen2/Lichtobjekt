/*
 * setbrightness.c
 *
 *  Created on: 24.12.2019
 *      Author: Martin
 */

#include "setbrightness.h"

unsigned char Brightness_start[] = {0x3F,0x3F};	//value before lights off
unsigned int PWM_Offset[] = {0,0};			//PWM value, where the driver effectively starts to generate an output
unsigned int ExtBrightness_last = 0x01FFF;			//external brightness during lights off divided by 256

signed int PWM_set[] = {0,0};				//current PWM value
signed int PWM_incr[] = {0,0};			//PWM dimming step size
unsigned int PWM_incr_cnt[] = {0,0};		//no of steps required to reach target PWM value

void PWM_Init(TIM_HandleTypeDef *handle_tim)
{
	htim_PWM = handle_tim;
	htim_PWM->Instance->CCR1 = 0;
	htim_PWM->Instance->CCR2 = 0;
	HAL_TIM_PWM_Start(htim_PWM, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(htim_PWM, TIM_CHANNEL_2);
	Update_PWM_Offset(0);
	Update_PWM_Offset(1);
}

void Update_PWM_Offset(unsigned char i)
{
	PWM_Offset[i]  = 0; // TODO Read_EEPROM(EEAddr_OffsetFrontBrightness+i);
	PWM_Offset[i] *= PWM_Offset[i];
}

void PWM_SetPulseWidth(int channel)
{
	PWM_set[channel] += PWM_incr[channel];
	--PWM_incr_cnt[channel];
	//limit to 16 bit
	if (PWM_set[channel] > maxPWM)
	{
		PWM_set[channel] = maxPWM;
	}
}

void PWM_StepDim()		// perform next dimming step, must frequently called for dimming action
						// contains repeated code to optimize performance
{
	if (PWM_incr_cnt[FrontChannel])
		{
		PWM_SetPulseWidth(FrontChannel);
	    htim_PWM->Instance->CCR1 = PWM_set[FrontChannel];
	    HAL_TIM_PWM_Start(htim_PWM, TIM_CHANNEL_1); //TODO required?
		}

	if (PWM_incr_cnt[BackChannel])
		{
		PWM_SetPulseWidth(BackChannel);
	    htim_PWM->Instance->CCR2 = PWM_set[BackChannel];
	    HAL_TIM_PWM_Start(htim_PWM, TIM_CHANNEL_2);
		}
}

void PWM_SetupDim(unsigned char i, signed int PWM_dimsteps, signed int Steps)
{
	signed int temp;
	limit=0;						//reset limit indicator
	temp = Brightness[i] + Steps;
	if (maxBrightness < temp)		//avoid overflow
		{
		temp = maxBrightness;
		limit = maxLimit;
		}
	else if (0 > temp)
		{
		limit = minLimit;
		temp = 0;
		}
	Brightness[i] = temp;

	temp = temp * (temp + 2) - PWM_set[i];
	if ((temp > PWM_dimsteps) || ((temp<0) && (-temp>PWM_dimsteps)))	// if we have more difference then steps to go
		{
		PWM_incr[i] = temp / PWM_dimsteps;
		PWM_set[i] += (temp - PWM_incr[i]*PWM_dimsteps); 		//calculate remainder, brackets to avoid overflow!?
		PWM_incr_cnt[i] = PWM_dimsteps;
		}
	else
		{
		if (0<temp)		// if we would have a stepsize smaller then one, we better reduce the number of steps
			{
			PWM_incr[i] = 1;
			PWM_incr_cnt[i] = temp;
			}
		else if (0>temp)
			{
			PWM_incr[i] = -1;
			PWM_incr_cnt[i] = -temp;				//count must be a positive number!
			}
		}
}

void PWM_SetupNow(unsigned char i, signed char Steps)
{
	PWM_SetupDim(i, 1, Steps);
	PWM_StepDim();
}

unsigned int sqrt32(unsigned long a)
{
	unsigned int rem=0;
	unsigned int root=0;
	unsigned int divisor=0;
	int i;

	// Iterate 16 times, because the maximum number of bits in the result is 16 bits
	for(i=0;i<16;i++)
	{
	   	root<<=1;
   		rem=(rem << 2)+(a>>30);
    		a<<=2;
    		divisor=(root<<1)+1;
    		if (divisor<=rem)
	    		{
         		rem-=divisor;
         		root+=1;
     		}
   	}
   	return root;
}

void SwLightOn(unsigned char i, unsigned int relBrightness)
{
	unsigned long temp;
	unsigned char minBrightness;					//avoid reduction to very low brightness values by external light

	minBrightness = 0;  //TODO Read_EEPROM(EEAddr_MinimumFrontBrightness+i-1);	//2 is lcd backlight
	temp=Brightness_start[i];
	temp=(temp*relBrightness)>>4;
	if (maxBrightness < temp)						//limit brightness to maximum
		{
		Brightness[i] = maxBrightness;
		}
	else if ((Brightness_start[i]>temp) && (minBrightness>temp))		//limit brightness ..
		{
		if (minBrightness>Brightness_start[i])
			{
			Brightness[i] = Brightness_start[i];		// .. to last value if it is smaller than minimum brightness
			}
		else
			{
			Brightness[i] = minBrightness;			// .. to minimum brightness if the last value was larger than the minimum brightness
			}
		}
	else
		{
		Brightness[i] = temp;					// or just take the calculated value!
		}
	PWM_SetupDim(i, fadetime, 0);
}

void SwLightOff(unsigned char i)
{
	Brightness_start[i]=Brightness[i];
	Brightness[i]=0;
	PWM_SetupDim(i, fadetime, 0);
}

void SwAllLightOn()
{
	unsigned int relBrightness;
	if (LightOn == false)						//remote signal might try to switch a switched on light on again
		{
		FocusBacklight=startupfocus;
		LightOn=true;
		relBrightness=1; //TODO sqrt32(ExtBrightness/ExtBrightness_last);
		SwLightOn(FrontChannel, relBrightness);
		SwLightOn(BackChannel, relBrightness);
		LEDOn();
		}
}

void SwAllLightOff()
{
	if (LightOn == true)						//remote signal might try to switch a switched on light on again
		{
		LightOn=false;
		Alarmflag=0;
		SwLightOff(FrontChannel);
		SwLightOff(BackChannel);
		ExtBrightness_last=1; // TODO(ExtBrightness>>8) & 0xFFFF;
		if (0==ExtBrightness_last)
			{
			ExtBrightness_last=1;
			}
//		SenderMode=0; //TODO Read_EEPROM(EEAddr_SenderMode);			//reset mode
		LEDSetupStandby();
		}
}

void ToggleFocus()
{
	FocusBacklight = !FocusBacklight;
}
