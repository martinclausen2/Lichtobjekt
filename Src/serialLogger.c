/*
 * serialLogger.c
 *
 *  Created on: 05.06.2021
 *      Author: Martin
 */

#include "serialLogger.h"

char Tx_len;
char Tx_Buffer[64];

void Init_SerialLogger(UART_HandleTypeDef *handle_huart)
{
	huart_logger = handle_huart;
};

void log_serial_P(const char *c)
{
  strcpy(Tx_Buffer, c);
  Tx_len=strlen(Tx_Buffer);
  HAL_UART_Transmit_IT(huart_logger,(uint8_t *)Tx_Buffer,Tx_len);
}

void log_serial(char *c)
{
  strcpy(Tx_Buffer, c);
  Tx_len=strlen(Tx_Buffer);
  HAL_UART_Transmit_IT(huart_logger,(uint8_t *)Tx_Buffer,Tx_len);
}
