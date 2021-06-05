/*
 * eeprom.h
 *
 *  Created on: 21.05.2021
 *      Author: Martin
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "stm32l1xx_hal.h"
#include "stm32l1xx_hal_flash_ex.h"

#define eeprom_address_offset 0x08080000

uint8_t readEEPROMByte(uint32_t address);

HAL_StatusTypeDef writeEEPROMByte(uint32_t address, uint8_t data);
HAL_StatusTypeDef writeEEPROMHalfWord(uint32_t address, uint16_t data);
HAL_StatusTypeDef writeEEPROMWord(uint32_t address, uint32_t data);

#endif /* EEPROM_H_ */
