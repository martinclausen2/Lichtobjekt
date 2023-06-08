/*
 * keys.c
 *
 *  Created on: Dec 22, 2019
 *      Author: Martin
 */

/** Handles keys reading
 */

#include "keys.h"

volatile unsigned char KeyState;		//stores the keys state
volatile unsigned char OldKeyState;
volatile unsigned char KeyPressDuration;

void Keys(unsigned int KeyStateInput)	//call from int at 40Hz to read keys
{
	KeyState = KeyStateInput;
	//decode keystate and measure time, no release debouncing, so do not call to often
	if (1 == KeyState)				//Key pressed?
		{
		if (0 == OldKeyState)		//Is this key new?
			{
			OldKeyState = KeyState;	// then store it
			KeyPressDuration = 0;	//tick zero
			}
		else if (OldKeyState == KeyState)
			{						//same key is pressed
			if (0xFF != KeyPressDuration)	//count ticks while same key is being pressed
				{
				++KeyPressDuration;
				}
			}
		else
			{						//we have a new key without an release of the old one
			OldKeyState = KeyState;	// store new one
			KeyPressDuration=0;		// reset time
			}
		}
}

//Returns if to stay in that menu build with a while loop
unsigned char CheckKeyPressed()
{
	unsigned char returnval=1;
	// A Key was pressed if OldKeyState != 0 and Keystate = 0
	// OldKeyState = 0 must be set by receiving program after decoding as a flag
	if ((0 != OldKeyState) && (0 == KeyState))
		{
		OldKeyState=0;
		returnval=0;
		}
	return returnval;
}
