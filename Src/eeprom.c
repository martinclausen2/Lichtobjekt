/*
 * eeprom.c
 *
 *  Created on: 21.05.2021
 *      Author: Martin
 */

#include "eeprom.h"

uint8_t readEEPROMByte(uint32_t address)
{
	 uint8_t *abs_address = address + eeprom_address_offset;
	 return *abs_address;
}


HAL_StatusTypeDef writeEEPROMByte(uint32_t address, uint8_t data)
 {
    HAL_StatusTypeDef  status;
    address = address + eeprom_address_offset;
    HAL_FLASHEx_DATAEEPROM_Unlock();  //Unprotect the EEPROM to allow writing
    status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_BYTE, address, data);
    HAL_FLASHEx_DATAEEPROM_Lock();  // Reprotect the EEPROM
    return status;
}

HAL_StatusTypeDef writeEEPROMHalfWord(uint32_t address, uint16_t data)
 {
    HAL_StatusTypeDef  status;
    address = address + eeprom_address_offset;
    HAL_FLASHEx_DATAEEPROM_Unlock();  //Unprotect the EEPROM to allow writing
    status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_HALFWORD, address, data);
    HAL_FLASHEx_DATAEEPROM_Lock();  // Reprotect the EEPROM
    return status;
}

HAL_StatusTypeDef writeEEPROMWord(uint32_t address, uint32_t data)
 {
    HAL_StatusTypeDef  status;
    address = address + eeprom_address_offset;
    HAL_FLASHEx_DATAEEPROM_Unlock();  //Unprotect the EEPROM to allow writing
    status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, address, data);
    HAL_FLASHEx_DATAEEPROM_Lock();  // Reprotect the EEPROM
    return status;
}
