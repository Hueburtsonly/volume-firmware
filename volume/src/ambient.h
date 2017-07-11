/*
 * ambient.h
 *
 *  Created on: 27 Jun 2017
 *      Author: Hueburtsonly
 */

#ifndef AMBIENT_H_
#define AMBIENT_H_

#include <stdint.h>

extern uint16_t ambient_result;

void ambient_init();
void ambient_measure();

#endif /* AMBIENT_H_ */
