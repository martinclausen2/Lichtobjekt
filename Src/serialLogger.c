/*
 * serialLogger.c
 *
 * wrapper to redirect logging commands to terminal
 * logging not (!) yet synchronized to terminal IO
 *
 *  Created on: 05.06.2021
 *      Author: Martin
 */

#include "serialLogger.h"

void log_serial_P(const char *c)
{
  TUSART_Print(c);
}

void log_serial(char *c)
{
  CLI_Printf(c);
}
