/*
 * serialLogger.h
 *
 *  Created on: 05.06.2021
 *      Author: Martin
 */

#ifndef INC_SERIALLOGGER_H_
#define INC_SERIALLOGGER_H_

#include <string.h>
#include "terminal_helper.h"

void log_serial(char *c);
void log_serial_P(const char *c);

#endif /* INC_SERIALLOGGER_H_ */
