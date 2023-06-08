/*
 * mainmenu.c
 *
 *  Created on: Dec 22, 2019
 *      Author: Martin
 */

#include "mainmenu.h"

void MainMenu()
{
	static unsigned char actionCounter = 0xFF;

	PWM_StepDim();			// do next dimming step

	++actionCounter;

	// check keys here since we can have only new input if timer flag was set by the keys interrupt program
	// Select key is pressed, show preview of action
	// need to check each time to generate single events directly from KeyPressDuration counter
	if (1 == KeyState)
	{
		if (1 == KeyPressDuration)
			if (LightOn)
			{
				// Toggle focus
				LEDSetColorTemp(LEDWhite + PreviewToggelFocus());
			}
			else
			{
				// Switch on
				LEDOn();
			}
		else if (KeyPressShort == KeyPressDuration)
			if (LightOn)
			{
				LEDSetupStandby();
			}
			else
			{
				// Switch All On
				LEDOn();
			}
		else if (KeyPressLong == KeyPressDuration)
		{
			LEDCancel();
		}
	}

	switch (actionCounter)
	{
	case 0:
		DecodeRemote();
		break;
	case 1:
		CheckAlarm();
		if (FocusChannel == FadeLightChannel)
		{
			FadeLight();
		}
		else
		{
			ResetFadeLight();
		}
		if (LightOn)
		{
			StoreBrightness();	// store brightness if required
			LEDLimit();
		}
		else
		{
			if (!KeyState)	//protect preview from being overwritten
			{
				LEDStandby();	//dim standby led according to external brightness
			}
			WriteTimer=0;
		}
		break;
	case 2:
		// A Key was released after pressed if OldKeyState = 1 and Keystate = 0
		// OldKeyState = 0 must be set by receiving program after decoding as a flag
		if ((1 == OldKeyState) && (0 == KeyState))
		{
			OldKeyState=0;		//Ack any key anyway
			if (KeyPressShort > KeyPressDuration)
			{
				if (LightOn)
				{
					ToggleFocus();
					LEDOn();
				}
				else
				{
					SendRC5(RC5Addr_com, RC5Cmd_On, 1, ComModeAll, RC5Cmd_Repeats);
					SwAllLightOn();
				}
			}
			else if (KeyPressLong > KeyPressDuration)
			{
				AlarmEnd();
				ResetFadeLight();
				if (LightOn)
				{
					SendRC5(RC5Addr_com, RC5Cmd_Off, 1, ComModeAll, RC5Cmd_Repeats);
					SwAllLightOff();
				}
				else
				{
					if (ComModeConditional<=SenderMode)		//reset to eeprom value in swalllightoff()
					{
						SenderMode=ComModeAll;
					}
					SendRC5(RC5Addr_com, RC5Cmd_On, 1, ComModeAll, RC5Cmd_Repeats);
					SwAllLightOn();
				}
			}
			else
			{
				//Restore status led after preview
				if (LightOn)
					LEDOn();
				else
					LEDSetupStandby();
			}
		}
		break;
	case 3:
		// A Rotation occurred if EncoderSteps!= 0
		// EncoderSteps = 0 must be set by receiving program after decoding
		if (EncoderSteps)
		{
			if (LightOn)
			{
				PWM_SetupDim(FocusChannel, Brightness_steps, EncoderSteps, 0);
				EncoderSteps = 0;								//ack any steps
				LEDSetupLimit();
				WriteTimer=WriteTime;
			  	if ((RC5Addr_first + FocusChannel) < RC5Addr_com)
			  	{
			  		SendRC5(RC5Addr_first+FocusChannel, (Brightness[FocusChannel]>>2) & 0x3F, (Brightness[FocusChannel]>>1) & 0x01, ComModeAll, RC5Value_Repeats);
			  	}
			}
		}
		actionCounter=0xFF;	//last time slot, do reset counter with increment to 0
		break;
	}
}
