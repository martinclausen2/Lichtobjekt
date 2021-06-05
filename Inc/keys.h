/*
 * keys.h
 *
 *  Created on: Dec 22, 2019
 *      Author: Martin
 */

#ifndef KEYS_H_
#define KEYS_H_

volatile unsigned char KeyState;		//stores the keys state
volatile unsigned char OldKeyState;
volatile unsigned char KeyPressDuration;

void Keys(unsigned int KeyState);
unsigned char CheckKeyPressed();

#endif /* KEYS_H_ */
