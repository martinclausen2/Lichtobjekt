/*
 * fadelight.c
 *
 *  Created on: May 20, 2023
 *      Author: Martin
 */

#include "fadelight.h"

#define colorCount 4

#define maxBrightnessSetting 40

unsigned int FadeDim_Cnt[maxChannel] = {0};

unsigned int fadeColor = colorCount;
unsigned int oldFadeColor = colorCount-1;

bool fadeLightFlag = false;

// array with meaning full color combinations

unsigned int colors[colorCount][maxChannel]=
{
		{ 40, 00},
		{ 10, 40},
		{ 00, 40},
		{ 10, 40}
};

// limit entry to max brightness
void Init_FadeLight()
{
	for (unsigned int j = 0; j < colorCount; j++)
	{
		for (unsigned int i = 0; i < maxChannel; i++)
		{
			if (colors[j][i] > GLOBAL_settings_ptr->maxBrightness[i])
			{
				colors[j][i] = GLOBAL_settings_ptr->maxBrightness[i];
			}
		}
	}
}

void FadeLight()
{
	bool fading = false;
	if (fadeLightFlag)
	{
		for (unsigned int i = 0; i < maxChannel; i++)
		{
			fading = fading || FadeLight_StepDim(i);
		}
	}
	if (!fadeLightFlag || !fading)
	{
		fadeLightFlag = true;

		oldFadeColor = fadeColor;
		fadeColor++;
		if (fadeColor >= colorCount)
		{
			fadeColor = 0;
		}
	}
}

void ResetFadeLight()
{
	fadeLightFlag = false;
}

//fade light dimming
int FadeLight_StepDim(unsigned int i)
{
	int dimStep = 0;
	int deltaBrightness = ((Brightness[FadeLightChannel]*colors[fadeColor][i]) / maxBrightnessSetting) - Brightness[i];

	if (FadeDim_Cnt[i])
	{
		--FadeDim_Cnt[i];							//count down step
	}
	else if (deltaBrightness)						//dimming step
	{
		int totalDeltaBrightness = (Brightness[FadeLightChannel]*(colors[fadeColor][i]-colors[oldFadeColor][i])) / maxBrightnessSetting;
		if (totalDeltaBrightness)
		{
			FadeDim_Cnt[i]=GLOBAL_settings_ptr->FadingTime*callsinsecond/totalDeltaBrightness;
		}
		else
		{
			FadeDim_Cnt[i]=1;
		}

		if (deltaBrightness > 0)
		{
			dimStep = 1;
		}
		else if (deltaBrightness < 0)
		{
			dimStep = -1;
		}
		else
		{
			dimStep = 0;
		}

		int targetBrightness = Brightness[i] + dimStep;
		int dimsteps = abs(targetBrightness * targetBrightness - Brightness[i] * Brightness[i]);

		if (dimsteps)								//avoid div zero if nothing is to do
		{
			PWM_SetupDim(i, dimsteps, dimStep, FadeDim_Cnt[i]*cases/dimsteps);	//setup brightness
		}
	}
	return deltaBrightness;
}

