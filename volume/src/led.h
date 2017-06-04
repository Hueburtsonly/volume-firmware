/*
 * led.h
 *
 *  Created on: 27 Dec 2016
 *      Author: Hueburtsonly
 */

#ifndef LED_H_
#define LED_H_

#define LED_RED 7
#define LED_GREEN 4
#define LED_BLUE 5

void led_init();
void led_on(uint8_t led);
void led_off(uint8_t led);

void init5928();
void test5928();

#endif /* LED_H_ */
