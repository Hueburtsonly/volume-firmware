/*
 * touch.h
 *
 *  Created on: 11 May 2017
 *      Author: Hueburtsonly
 */

#ifndef TOUCH_H_
#define TOUCH_H_

#include <stdint.h>

void touch_init();

extern uint16_t touch_result;
void touch_measure();

#endif /* TOUCH_H_ */
