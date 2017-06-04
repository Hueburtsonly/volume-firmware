/*
 * ad9102.h
 *
 *  Created on: 27 Dec 2016
 *      Author: Hueburtsonly
 */

#ifndef AD9102_H_
#define AD9102_H_

void ad9102_reset();
void ad9102_init();

void ad9102_sine_burst(int32_t freq, int32_t phase);

void ad9102_dout(uint8_t on);
void ad9102_const(int32_t dc);

uint16_t ad9102_pull_test();

#endif /* AD9102_H_ */
