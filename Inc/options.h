/*
 * options.h
 *
 *  Created on: 24.12.2019
 *      Author: Martin
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

// define EEPROM mapping
#define EEAddr_RC5Addr 0			//RC5 Address

#define maxComMode 3

unsigned char RC5Addr;
unsigned char ReceiverMode;
unsigned char SenderMode;

#define ComModeOff	0
#define ComModeAlarm	1
#define ComModeConditional	2
#define ComModeAll	3

#endif /* OPTIONS_H_ */
