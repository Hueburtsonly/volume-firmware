/*
 * tlc5928.h
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */

#ifndef TLC5928_H_
#define TLC5928_H_

#include <stdint.h>

void tlc5928_init();
void tlc5928_broadcast(uint16_t state);
void tlc5928_send();
void tlc5928_demo(int active);

#endif /* TLC5928_H_ */
