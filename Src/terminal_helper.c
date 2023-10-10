/*
 * terminal_helper.c
 *
 * controller specific implementations
 *
 *  Created on: 30.12.2022
 *      Author: Martin
 */

#include "terminal_helper.h"

char dbgbuffer[256];

uint8_t aTXBuffer[TX_BUFFER_SIZE];

uint8_t aRXBuffer[RX_BUFFER_SIZE];

/**
 * @brief Data buffers used to manage received data in interrupt routine
 */

uint8_t aRXBufferA[RX_BUFFER_SIZE];
uint8_t aRXBufferB[RX_BUFFER_SIZE];

__IO bool		rxDataPending;
__IO bool		rxDataBlocked;
__IO bool		restartUART;
__IO uint32_t   uwNbReceivedChars;
uint8_t *pBufferReadyForUser;
uint8_t *pBufferReadyForReception;

UART_HandleTypeDef *huart_terminal;

volatile uint16_t last_Size;

/* call once during start up */
void Init_Terminal(UART_HandleTypeDef *handle_huart)
{
	huart_terminal = handle_huart;
	CLI_Init(TDC_Time);
	restartUART = false;
	TUSART_StartReception();
};

/* needs to be called frequently */
void Execute_Terminal()
{
	if (restartUART)
	{
		HAL_UART_DMAStop(huart_terminal);
		CLI_Printf("\r\nRestart UART.");
		TUSART_StartReceptionCore();
	}
	else
	{
		/* Process received data that has been extracted from Rx User buffer */
		if (rxDataPending)
		{
			TUSART_ProcessInput(pBufferReadyForUser, uwNbReceivedChars);
			rxDataPending = false;
		}
		if (rxDataBlocked)
		{
			UARTEx_RxEventCallbackCore();
		}
	}
}

void _reset_fcn()
{
	NVIC_SystemReset();
}

// TODO add FIFO to aggregate char before sending and avoid waiting for empty buffer.

inline void TUSART_PutChar(char c)
{
	// check if previous transmission is ongoing
	while(huart_terminal->gState != HAL_UART_STATE_READY){}
	// copy content into reserved memory
	memcpy((uint8_t*)&aTXBuffer, (uint8_t *)&c, 1);
	HAL_UART_Transmit_DMA(huart_terminal, (uint8_t*)&aTXBuffer, 1);
}

// TODO add FIFO to aggregate char before sending and avoid waiting for empty buffer.
void TUSART_Print(const char* str)
{
	//determine size and limit to buffer size
	unsigned int length = min(strlen(str), TX_BUFFER_SIZE);
	// check if previous transmission is ongoing
	while(huart_terminal->gState != HAL_UART_STATE_READY){}
	// copy content into reserved memory
	memcpy((uint8_t*)&aTXBuffer, str, length);
	HAL_UART_Transmit_DMA(huart_terminal, (uint8_t*)&aTXBuffer, length);
}

/**
 * @brief  This function initiates RX transfer
 * @retval None
 */
void TUSART_StartReception(void)
{
	//	HAL_StatusTypeDef HAL_UART_RegisterRxEventCallback(UART_HandleTypeDef *huart, pUART_RxEventCallbackTypeDef pCallback)
	pUART_RxEventCallbackTypeDef pRxCallback = *HAL_UARTEx_RxEventCallback;
	HAL_UART_RegisterRxEventCallback(huart_terminal, pRxCallback);

	// Register Error Callback
	pUART_CallbackTypeDef pErrorCallback = *HAL_UART_ErrorCallback;
	HAL_UART_RegisterCallback(huart_terminal, HAL_UART_ERROR_CB_ID, pErrorCallback);

	TUSART_StartReceptionCore();
}

void TUSART_StartReceptionCore(void)
{

	/* Initializes Buffer swap mechanism (used in User callback) :
	   - 2 physical buffers aRXBufferA and aRXBufferB (RX_BUFFER_SIZE length)
	 */
	pBufferReadyForReception = aRXBufferA;
	pBufferReadyForUser      = aRXBufferB;
	uwNbReceivedChars        = 0;
	rxDataPending 			 = false;
	restartUART				 = false;

	/* Initializes Rx sequence using Reception To Idle event API.
     As DMA channel associated to UART Rx is configured as Circular,
     reception is endless.
     If reception has to be stopped, call to HAL_UART_AbortReceive() could be used.
     Use of HAL_UARTEx_ReceiveToIdle_DMA service, will generate calls to
     user defined HAL_UARTEx_RxEventCallback callback for each occurrence of
     following events :
     - DMA RX Half Transfer event (HT)
     - DMA RX Transfer Complete event (TC)
     - IDLE event on UART Rx line (indicating a pause is UART reception flow)
     => make sure to enable both DMA and UART interrupts
	 */
	HAL_UARTEx_ReceiveToIdle_DMA(huart_terminal, aRXBuffer, RX_BUFFER_SIZE);
}

/**
 * @brief  This function handles buffer containing received data
 * @note   This routine is executed in Interrupt context.
 * @param  huart UART handle.
 * @param  pData Pointer on received data buffer to be processed
 * @retval Size  Nb of received characters available in buffer
 */
void TUSART_ProcessInput(uint8_t* pData, uint16_t Size)
{
	/*
	 * This function might be called in any of the following interrupt contexts :
	 *  - DMA TC and HT events
	 *  - UART IDLE line event
	 *
	 * pData and Size defines the buffer where received data have been copied, in order to be processed.
	 * During this processing of already received data, reception is still ongoing.
	 *
	 */
	uint8_t* pBuff = pData;
	uint8_t  i;
	for (i = 0; i < Size; i++)
	{
		char c = (char)*pBuff;
		CLI_EnterChar(c);
		if(c == TERM_KEY_ENTER)
			CLI_Execute();
		pBuff++;
	}
	CLI_Execute();
}

/**
 * @brief  User implementation of the Reception Event Callback
 *         (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	last_Size = Size;
	UARTEx_RxEventCallbackCore();
}

/**
 * @brief  UART error callbacks.
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	restartUART = true;
}


void UARTEx_RxEventCallbackCore()
{
	static uint16_t old_pos = 0;
	volatile static bool lock;
	uint8_t *ptemp;
	uint16_t i;

	// function can be called from interrupt or regular main loop to catch up with pending data
	if (!lock)
	{
		lock = true;
		/* Check if number of received data in reception buffer has changed
		 * and if older user buffer is already processed,
		 * if not hope for the aRXBufferUser not to overflow and wait for next interrupt
		 */
		if ((last_Size != old_pos) & !rxDataPending)
		{
			/* Check if position of index in reception buffer has simply be increased */
			/* of if end of buffer has been reached */
			if (last_Size > old_pos)
			{
				/* Current position is higher than previous one */
				uwNbReceivedChars = last_Size - old_pos;
				/* Copy received data in "User" buffer for evacuation */
				for (i = 0; i < uwNbReceivedChars; i++)
				{
					pBufferReadyForReception[i] = aRXBuffer[old_pos + i];
				}
			}
			else
			{
				/* Current position is lower than previous one : end of buffer has been reached */
				/* First copy data from current position till end of buffer */
				uwNbReceivedChars = RX_BUFFER_SIZE - old_pos;
				/* Copy received data in "User" buffer for evacuation */
				for (i = 0; i < uwNbReceivedChars; i++)
				{
					pBufferReadyForReception[i] = aRXBuffer[old_pos + i];
				}
				/* Check and continue with beginning of buffer */
				if (last_Size > 0)
				{
					for (i = 0; i < last_Size; i++)
					{
						pBufferReadyForReception[uwNbReceivedChars + i] = aRXBuffer[i];
					}
					uwNbReceivedChars += last_Size;
				}
			}

			/* Swap buffers for next bytes to be processed */
			ptemp = pBufferReadyForUser;
			pBufferReadyForUser = pBufferReadyForReception;
			pBufferReadyForReception = ptemp;

			rxDataPending = true;

			/* Update old_pos as new reference of position in User Rx buffer that
     	 	 	 indicates position to which data have been processed */
			old_pos = last_Size;
			rxDataBlocked = false;
		}
		else
		{
			// could not execute due to ongoing data processing
			rxDataBlocked = true;
		}
		lock = false;
	}
	else
	{
		// could not execute due to lock
		rxDataBlocked = true;
	}
}
