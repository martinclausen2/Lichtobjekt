/* Function to decode and code RC5 commands for LampenSteuerung
   call decoder state machine at 4499 Hz for four times oversampling of the 889µs demodulated RC5 signal pulses
 */

#include "RC5.h"

void RC5SignalSampling(GPIO_PinState signal)		//int from Timer to read RC5 state
{
	static unsigned char rc5state;
	static bool Rbit;					//Detektiertes Bit von RC5

	rc5state=rc5state<<1;
	rc5state|=!signal;					//Insert inverted input
	rc5state=tblRemote[rc5state];		//get next state
	if (33==rc5state)					//Startsequenz erkannt?
		{
		rCounter=0;						//alles zurücksetzen
		rAddress=0;
		rCommand=0;
		}
	else if ((42==rc5state) || (50==rc5state))	 //Erkanntes Bit einorden
		{
		if (42==rc5state)			//Nutzbit 1 erkannt?
			{
			Rbit=1;
			}
		else if (50==rc5state)		//Nutzbit 0 erkannt?
			{
			Rbit=0;
			}
   			++rCounter;				//Da neues Bit ...
		if (1==rCounter)
			{
			RTbit=Rbit;
			}
		else if (7>rCounter)		//Adressbit
			{
			rAddress=rAddress<<1;
			rAddress|=Rbit;
			}
		else						//Commandbit
			{
  			rCommand=rCommand<<1;
  			rCommand|=Rbit;
  			}
		}
}


//setup brightness values
void SetLightRemote(unsigned char i, signed char steps)
{
	PWM_SetupDim(i, Brightness_steps, steps);
	FocusBacklight=i;
	LightOn=1;
	LEDOn();
	LEDSetupLimit();
}

void SetBrightnessRemote(unsigned char i)
{
	if (ReceiverMode>=ComModeConditional)
		{
		Brightness[i]=((rCommand<<1) & 0x7E) + RTbit;
		SetLightRemote(i,0);
		}
}

void SetBrightnessLevelRemote()
{
	if (rCommand<=5)
		{
		Brightness[0]=(((unsigned int)rCommand*(unsigned int)maxBrightness)/5) & maxBrightness;
		SetLightRemote(0,0);
		}
	else if (rCommand<=10)
		{
		Brightness[1]=(((unsigned int)(rCommand-5)*(unsigned int)maxBrightness)/5) & maxBrightness;
		SetLightRemote(1,0);
		}
}

void DecodeRemote()
{
	bool static RTbitold;				//Togglebit des letzten Befehls von RC5

	if (12==rCounter)
		{
		char serial_Buffer[64];
		sprintf(serial_Buffer,"RC5 Addr %d Cmd %d\r\n", rAddress, rCommand);
		log_serial(serial_Buffer);

		if (RC5Addr==rAddress)
			{
			if (RTbit ^ RTbitold)		//Neue Taste erkannt
				{
				switch (rCommand)
					{
					case 12:			//Standby
						SwAllLightOn();
						break;
					case 13:			//mute
						SwAllLightOff();
						break;
					default:
						SetBrightnessLevelRemote();
						break;
	  				}
				}

	  		switch (rCommand)			//new or same key pressed
	  			{
	  			case 16:				//incr vol
					SetLightRemote(0, RemoteSteps);
					break;
	  			case 17:				//decr vol
					SetLightRemote(0, -RemoteSteps);
					break;
				case 32:				//incr channel
					SetLightRemote(1, RemoteSteps);
					break;
				case 33:				//decr channel
					SetLightRemote(1, -RemoteSteps);
					break;
	  			}
	  		RTbitold=RTbit;				//Togglebit speichern
	  		}
	  	else if (RC5Addr_front==rAddress)
	  		{
	  		SetBrightnessRemote(0);
	  		}
	  	else if (RC5Addr_back==rAddress)
	  		{
	  		SetBrightnessRemote(1);
	  		}
	  	else if (RC5Addr_com==rAddress)
	  		{
	  		switch (rCommand)
	  			{
	  			case RC5Cmd_AlarmStart:
	  				if (ComModeAlarm<=ReceiverMode)
	  					{
	  					// TODO		Alarm();
	  					}
	  				break;
	  			case RC5Cmd_AlarmEnd:
	  				if (ComModeAlarm<=ReceiverMode)
	  					{
	  					// TODO		AlarmEnd();
	  					}
	  				break;
	  			case RC5Cmd_Off:
					if (ComModeConditional<=ReceiverMode)
						{
						SwAllLightOff();
						}
	  				break;
	  			case RC5Cmd_On:
					if (ComModeConditional<=ReceiverMode)
						{
						SwAllLightOn();
						}
	  				break;
	  			}
	  		}
		rCounter=0;					//Nach Erkennung zurücksetzen
		}
}
