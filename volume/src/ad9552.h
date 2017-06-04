/*
 * ad9552.h
 *
 *  Created on: 27 Dec 2016
 *      Author: Hueburtsonly
 */

#ifndef AD9552_H_
#define AD9552_H_

#include "stdint.h"

void ad9552_reset();
void ad9552_init();
void ad9552_vco_band_auto();
void ad9552_vco_band_manual(uint8_t band);
void ad9552_frequency(uint32_t fOUT);
uint8_t ad9552_is_locked();

#endif /* AD9552_H_ */
