/*
 * lcd.c
 *
 *  Driver for
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */

#include "lcd.h"
#include "chip.h"
#include "tlc5928.h"

#define DSI 2
#define DSCL 20
#define DRS 17

void lcd_init() {
	LPC_GPIO->DIR[0] |= (1 << DSI) | (1 << DSCL) | (1 << DRS);
	LPC_GPIO->SET[0] = (1 << DSI);
}

static inline void out(uint8_t v, uint8_t is_data) {

	//tlc5928_broadcast(0b1001101010101010);tlc5928_broadcast(0b1001101010101010);

	//CS = 0;

	LPC_GPIO->W[0][DRS] = is_data;

	while (lcd_cs_unsafe());

	for (int k=0; k<8; k++){

		LPC_GPIO->CLR[0] = (1 << DSCL);

		LPC_GPIO->W[0][DSI] = v & 0x80;
		v <<= 1;

		LPC_GPIO->SET[0] = (1 << DSCL);
	}
	//CS = 1;


    //tlc5928_broadcast(0b0010101010101010);
}

#define comm_out(x) out((x), 0)
#define data_out(x) out((x), 1)

/*const uint8_t TEST_IMAGE[4][128] = {
 {15, 1, 1, 1, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 15},
 {0, 0, 0, 0, 0, 0, 255, 255, 96, 96, 96, 96, 96, 96, 96, 96, 255, 255, 0, 0, 192, 240, 176, 152, 152, 152, 152, 176, 240, 192, 0, 0, 48, 48, 24, 24, 152, 152, 152, 248, 240, 0, 0, 0, 192, 240, 56, 24, 24, 24, 48, 255, 255, 0, 0, 248, 248, 48, 24, 24, 24, 56, 240, 192, 0, 0, 255, 255, 48, 24, 24, 24, 56, 240, 224, 0, 0, 192, 240, 48, 24, 24, 24, 24, 48, 240, 192, 0, 0, 248, 248, 48, 24, 24, 24, 56, 240, 224, 0, 0, 192, 240, 176, 152, 152, 152, 152, 176, 240, 192, 0, 0, 224, 240, 152, 152, 152, 24, 24, 48, 32, 0, 0, 0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0, 0, 63, 63, 0, 0, 0, 0, 0, 0, 0, 0, 63, 63, 0, 0, 7, 31, 25, 49, 49, 49, 49, 57, 25, 9, 0, 0, 30, 63, 51, 51, 49, 49, 25, 31, 63, 32, 0, 0, 7, 31, 56, 48, 48, 48, 24, 63, 63, 0, 0, 255, 255, 24, 48, 48, 48, 56, 31, 7, 0, 0, 63, 63, 0, 0, 0, 0, 0, 63, 63, 0, 0, 7, 31, 24, 48, 48, 48, 48, 24, 31, 7, 0, 0, 63, 63, 0, 0, 0, 0, 0, 63, 63, 0, 0, 7, 31, 25, 49, 49, 49, 49, 57, 25, 9, 0, 0, 8, 25, 49, 49, 49, 51, 51, 31, 14, 0, 0, 0, 0, 0, 0, 0},
 {240, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 240}
};*/

//const uint8_t TEST_IMAGE[512] = {
 // 15, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 15, 0, 0, 0, 0, 0, 0, 252, 252, 0, 0, 0, 0, 0, 0, 0, 0, 252, 252, 0, 0, 224, 248, 152, 140, 140, 140, 140, 156, 152, 144, 0, 0, 120, 252, 204, 204, 140, 140, 152, 248, 252, 4, 0, 0, 224, 248, 28, 12, 12, 12, 24, 252, 252, 0, 0, 255, 255, 24, 12, 12, 12, 28, 248, 224, 0, 0, 252, 252, 0, 0, 0, 0, 0, 252, 252, 0, 0, 224, 248, 24, 12, 12, 12, 12, 24, 248, 224, 0, 0, 252, 252, 0, 0, 0, 0, 0, 252, 252, 0, 0, 224, 248, 152, 140, 140, 140, 140, 156, 152, 144, 0, 0, 16, 152, 140, 140, 140, 204, 204, 248, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 6, 6, 6, 6, 6, 6, 6, 6, 255, 255, 0, 0, 3, 15, 13, 25, 25, 25, 25, 13, 15, 3, 0, 0, 12, 12, 24, 24, 25, 25, 25, 31, 15, 0, 0, 0, 3, 15, 28, 24, 24, 24, 12, 255, 255, 0, 0, 31, 31, 12, 24, 24, 24, 28, 15, 3, 0, 0, 255, 255, 12, 24, 24, 24, 28, 15, 7, 0, 0, 3, 15, 12, 24, 24, 24, 24, 12, 15, 3, 0, 0, 31, 31, 12, 24, 24, 24, 28, 15, 7, 0, 0, 3, 15, 13, 25, 25, 25, 25, 13, 15, 3, 0, 0, 7, 15, 25, 25, 25, 24, 24, 12, 4, 0, 0, 0, 0, 0, 0, 0, 240, 128, 128, 128, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 240
//};


const uint8_t TEST_IMAGES[4][512] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 192, 224, 120, 24, 56, 124, 192, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 112, 24, 8, 12, 6, 2, 130, 129, 193, 65, 65, 65, 225, 65, 65, 65, 227, 134, 132, 12, 24, 48, 96, 192, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 194, 2, 2, 2, 2, 2, 2, 3, 129, 193, 33, 49, 17, 17, 17, 17, 1, 1, 1, 1, 1, 129, 129, 65, 97, 33, 161, 97, 1, 0, 0, 0, 24, 60, 60, 7, 7, 7, 1, 0, 0, 0, 0, 0, 15, 31, 28, 56, 120, 240, 224, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 15, 1, 0, 0, 0, 12, 14, 15, 11, 26, 31, 147, 153, 137, 143, 137, 11, 10, 15, 10, 11, 15, 14, 8, 0, 0, 1, 7, 60, 192, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 254, 255, 255, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 248, 192, 0, 8, 8, 12, 62, 39, 36, 68, 68, 76, 120, 48, 0, 0, 0, 0, 24, 36, 36, 34, 34, 35, 33, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 15, 60, 240, 192, 0, 0, 0, 0, 0, 0, 7, 28, 96, 128, 0, 0, 0, 0, 224, 32, 32, 32, 225, 3, 6, 8, 0, 0, 112, 144, 16, 16, 48, 224, 0, 0, 0, 0, 0, 240, 31, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 192, 192, 192, 255, 255, 255, 192, 192, 192, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 48, 12, 7, 1, 0, 1, 2, 12, 24, 48, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 30, 28, 112, 0, 0, 0, 0, 0, 0, 3, 14, 24, 48, 32, 99, 67, 66, 67, 193, 128, 128, 128, 129, 130, 130, 130, 131, 195, 65, 97, 32, 48, 24, 12, 6, 3, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 1, 1, 1, 63, 63, 127, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 7, 7, 7, 15, 62, 252, 248, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 255, 255, 3, 3, 3, 3, 0, 240, 252, 254, 31, 7, 7, 3, 3, 3, 3, 7, 7, 31, 254, 252, 240, 0, 0, 0, 0, 254, 255, 255, 3, 3, 3, 3, 0, 60, 127, 255, 231, 195, 195, 195, 195, 195, 199, 135, 142, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 128, 240, 254, 127, 15, 127, 255, 248, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 192, 192, 255, 255, 255, 192, 192, 192, 192, 0, 15, 63, 127, 248, 224, 224, 192, 192, 192, 192, 224, 224, 248, 127, 63, 15, 0, 192, 192, 192, 255, 255, 255, 192, 192, 192, 192, 0, 24, 120, 248, 240, 225, 193, 193, 193, 193, 195, 227, 227, 255, 127, 63, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 224, 252, 255, 31, 3, 0, 0, 0, 3, 31, 255, 254, 240, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 63, 63, 127, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 63, 63, 127, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 7, 7, 7, 15, 62, 252, 248, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 127, 255, 231, 195, 195, 195, 195, 195, 199, 135, 142, 255, 255, 255, 0, 0, 0, 240, 252, 254, 31, 7, 7, 3, 3, 3, 3, 3, 7, 15, 62, 60, 48, 0, 0, 0, 0, 254, 255, 255, 3, 3, 3, 3, 0, 0, 252, 254, 255, 15, 7, 3, 3, 3, 3, 7, 6, 28, 255, 255, 255, 0, 0, 0, 60, 127, 255, 231, 195, 195, 195, 195, 195, 199, 135, 142, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 128, 240, 254, 127, 15, 127, 255, 248, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 120, 248, 240, 225, 193, 193, 193, 193, 195, 227, 227, 255, 127, 63, 0, 0, 0, 15, 63, 127, 248, 224, 224, 192, 192, 192, 192, 192, 224, 240, 120, 56, 24, 0, 192, 192, 192, 255, 255, 255, 192, 192, 192, 192, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 24, 120, 248, 240, 225, 193, 193, 193, 193, 195, 227, 227, 255, 127, 63, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 224, 252, 255, 31, 3, 0, 0, 0, 3, 31, 255, 254, 240, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 63, 63, 127, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 255, 255, 15, 63, 252, 224, 128, 0, 0, 0, 255, 255, 15, 62, 252, 240, 192, 0, 0, 0, 0, 112, 254, 255, 159, 7, 3, 3, 3, 3, 3, 7, 15, 30, 252, 248, 240, 0, 0, 3, 255, 255, 252, 128, 0, 0, 0, 0, 0, 0, 0, 0, 3, 255, 255, 252, 128, 128, 128, 192, 240, 252, 127, 31, 7, 3, 0, 0, 0, 28, 30, 31, 15, 135, 131, 131, 131, 195, 195, 195, 199, 231, 254, 252, 120, 0, 0, 0, 0, 3, 3, 227, 128, 0, 0, 0, 0, 0, 0, 0, 3, 3, 227, 128, 0, 0, 0, 0, 0, 0, 0, 3, 3, 227, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 224, 255, 255, 31, 0, 0, 0, 1, 7, 31, 126, 240, 252, 255, 15, 0, 0, 0, 3, 7, 31, 124, 248, 224, 128, 7, 31, 63, 120, 240, 224, 192, 192, 192, 192, 192, 224, 248, 255, 127, 31, 0, 0, 0, 31, 255, 255, 248, 96, 192, 192, 192, 192, 192, 128, 0, 0, 31, 255, 255, 231, 7, 15, 31, 61, 56, 112, 224, 224, 192, 128, 0, 0, 28, 63, 127, 231, 231, 199, 195, 195, 195, 195, 193, 225, 241, 240, 112, 0, 0, 0, 0, 0, 0, 1, 255, 248, 128, 0, 0, 0, 0, 0, 0, 0, 1, 255, 248, 128, 0, 0, 0, 0, 0, 0, 0, 1, 255, 248, 128, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 7, 255, 255, 248, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 255, 255, 248, 0, 0, 0, 0, 0, 0, 0, 7, 255, 255, 248, 0, 0, 0, 0, 0, 0, 0, 7, 255, 255, 248, 0, 0}
};



void lcd_disp(unsigned char *lcd_string)
{
	char is_blank = 1;
	unsigned int i,j;
	unsigned char page = 0xB0;			//Page Address + 0xB0
	comm_out(0xAE);					//Display OFF
	comm_out(0x40);					//Display start address + 0x40
	for(i=0;i<4;i++){				//32pixel display / 8 pixels per page = 4 pages
		comm_out(page);				//send page address
		comm_out(0x10);				//column address upper 4 bits + 0x10
		comm_out(0x00);				//column address lower 4 bits + 0x00
		for(j=0;j<128;j++){			//128 columns wide
			if (*lcd_string) is_blank = 0;
			data_out(*lcd_string);		//send picture data
			lcd_string++;			//point to next picture data
			}
		page++;					//after 128 columns, go to next page
   		}
	if (!is_blank)
		comm_out(0xAF);					//Display ON
}

void lcd_soft_init(int index) {
	comm_out(0xA0); // ADC select normal
	comm_out(0xAE); // LCD display on
	comm_out(0xC0); // COM output scan direction normal
	comm_out(0xA2); // LCD drive bias 1/9
	comm_out(0x2F); // Power supply operating mode 7
	comm_out(0x21); // Internal resistor ratio 1
	comm_out(0x81); // Set Vs output voltage electronic volume register...
	comm_out(0x20); // .. to 0x3F (max).

	//comm_out(0xA5); // ALL POINTS ON
	//comm_out(0xA5); // ALL POINTS ON

	//while(1){						//loop forever
	lcd_disp(TEST_IMAGES[index & 3]);					//show image
		//for (;;);
		//delay(1000);					//wait 1s
		//dispPic(graphic1);
		//delay(1000);
		//dispPic(graphic2);
		//delay(1000);
	//}
}



void lcd_test_pattern() {
	for (int page = 0; page < 4; page++) {
		// Page address
		comm_out(0xB0 | (page & 0x0F));

		// Column address = 0
		comm_out(0x10);
		comm_out(0x00);

		for (int col = 0; col < 128; col++) {
			//data_out(TEST_IMAGE[page & 0x3][col]);
		}
	}
}
