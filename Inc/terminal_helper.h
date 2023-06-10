/*
 * terminal_helper.h
 *
 * USE_HAL_UART_REGISTER_CALLBACKS must be set to on = 1U
 *
 *  Created on: 30.12.2022
 *      Author: Martin
 */

#ifndef INC_TERMINAL_HELPER_H_
#define INC_TERMINAL_HELPER_H_

#include "../terminal/terminal.h"
#include "../terminal/terminal_config.h"

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Size of Reception buffer */
#define RX_BUFFER_SIZE   256
/* Size of Transmission buffer */
#define TX_BUFFER_SIZE   256

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

void Init_Terminal(UART_HandleTypeDef *handle_huart);

void Execute_Terminal();

void _reset_fcn();

void TUSART_PutChar(char c);

void TUSART_Print(const char* str);

void TUSART_StartReception(void);

void TUSART_ProcessInput(uint8_t* pData, uint16_t Size);

void TUSART_UARTEx_RxEventCallback(uint16_t Size);

#endif /* INC_TERMINAL_HELPER_H_ */
