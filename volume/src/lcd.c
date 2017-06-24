/*
 * lcd.c
 *
 *  Driver for
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */


#include "chip.h"

#define DSI 2
#define DSCL 20
#define DRS 17

void lcd_init() {
	LPC_GPIO->DIR[0] |= (1 << DSI) | (1 << DSCL) | (1 << DRS);
	LPC_GPIO->SET[0] = (1 << DSI);
}

static inline void out(uint8_t v, uint8_t is_data) {

	tlc5928_broadcast(0b1001101010101010);tlc5928_broadcast(0b1001101010101010);

	//CS = 0;
	__NOP(); __NOP(); __NOP();
	LPC_GPIO->W[0][DRS] = is_data;

	for (int k=0; k<8; k++){
		__NOP(); __NOP(); __NOP();
		LPC_GPIO->CLR[0] = (1 << DSCL);
		__NOP(); __NOP(); __NOP();
		LPC_GPIO->W[0][DSI] = v & 0x80;
		v <<= 1;
		__NOP(); __NOP(); __NOP();
		LPC_GPIO->SET[0] = (1 << DSCL);
	}
	//CS = 1;


    tlc5928_broadcast(0b0010101010101010);
}

#define comm_out(x) out((x), 0)
#define data_out(x) out((x), 1)



//below is picture.h file information
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
char logo[]={													//Logo
0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0x3f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0xc7,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc7,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0xce,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0xfc,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0xc0,0x00,
0x00,0x00,0x00,0x18,0x00,0x78,0xc0,0x00,0x00,0x00,0x00,0x1c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x01,0xf8,0x7f,0xc0,0x00,0x00,0x00,0x00,0x01,0xf8,0x00,0x00,0x00,
0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x1e,0x00,0x7f,0xc0,0x00,0x00,0x00,0x00,0x18,0x00,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x07,0x08,0x07,0x00,0x00,
0x00,0x00,0x00,0x0f,0x98,0x1e,0x00,0x00,0x00,0x00,0x00,0x1d,0xf8,0x38,0x00,0x00,
0x00,0x00,0x00,0x18,0xf0,0x78,0x00,0x00,0x00,0x00,0x00,0x18,0xe0,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x18,0xc0,0x7f,0xc0,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xf8,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x19,0x98,0x03,0xc0,0x00,0x00,0x00,0x00,0x19,0x98,0x07,0x80,0x00,
0x00,0x00,0x00,0x19,0x98,0x0e,0x00,0x00,0x00,0x00,0x00,0x19,0x98,0x38,0x00,0x00,
0x00,0x00,0x00,0x19,0x98,0x78,0x00,0x00,0x00,0x00,0x00,0x19,0x98,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x1f,0xf8,0x7f,0xc0,0x00,0x00,0x00,0x00,0x1f,0xf8,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x40,0x00,
0x00,0x00,0x00,0x1e,0x00,0x44,0x40,0x00,0x00,0x00,0x00,0x1f,0x80,0x44,0x40,0x00,
0x00,0x00,0x00,0x07,0xe0,0x44,0x40,0x00,0x00,0x00,0x00,0x00,0xf8,0x44,0x40,0x00,
0x00,0x00,0x00,0x00,0x78,0x44,0x40,0x00,0x00,0x00,0x00,0x00,0x78,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x03,0xf0,0x7f,0xc0,0x00,0x00,0x00,0x00,0x0f,0x80,0x40,0x00,0x00,
0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x38,0x1f,0x00,0x00,
0x00,0x00,0x00,0x00,0xf8,0x3f,0x80,0x00,0x00,0x00,0x00,0x03,0xf0,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x0f,0xe0,0x60,0xc0,0x00,0x00,0x00,0x00,0x0c,0x60,0x60,0xc0,0x00,
0x00,0x00,0x00,0x0c,0x60,0x40,0xc0,0x00,0x00,0x00,0x00,0x0f,0xe0,0x40,0xc0,0x00,
0x00,0x00,0x00,0x07,0xe0,0x40,0xc0,0x00,0x00,0x00,0x00,0x00,0xf8,0x7f,0xc0,0x00,
0x00,0x00,0x00,0x00,0x38,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0x00,0x00,
0x00,0x00,0x00,0x00,0x0f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x1f,0x87,0x80,0x00,
0x00,0x00,0x00,0x00,0x7e,0x01,0xc0,0x00,0x00,0x00,0x00,0x01,0xf8,0x01,0xc0,0x00,
0x00,0x00,0x00,0x03,0xe0,0x71,0xc0,0x00,0x00,0x00,0x00,0x0f,0xc1,0xf1,0xc0,0x00,
0x00,0x00,0x00,0x3f,0x03,0xf1,0xc0,0x00,0x00,0x00,0x00,0xfc,0x0f,0xc1,0xc0,0x00,
0x00,0x00,0x01,0xf0,0x3f,0x03,0xc0,0x00,0x00,0x00,0x07,0xe0,0xfe,0x07,0xc0,0x00,
0x00,0x00,0x1f,0x81,0xf8,0x1f,0xc0,0x00,0x00,0x00,0x7e,0x07,0xe0,0x7f,0xc0,0x00,
0x00,0x00,0xf8,0x1f,0xc1,0xfd,0xc0,0x00,0x00,0x03,0xf0,0x7f,0x03,0xf9,0xc0,0x00,
0x00,0x0f,0xc0,0xfc,0x0f,0xf1,0xc0,0x00,0x00,0x3f,0xe3,0xf0,0x3f,0x71,0xc0,0x00,
0x00,0x7f,0xff,0xe0,0xfe,0x71,0xc0,0x00,0x00,0x78,0xff,0x81,0xfe,0x71,0xc0,0x00,
0x00,0xf0,0x3f,0x87,0xfe,0x71,0xc0,0x00,0x00,0xe0,0x0f,0xdf,0x8e,0x71,0xc0,0x00,
0x01,0xc3,0x03,0xfe,0x0e,0x71,0xc0,0x00,0x01,0xc7,0xc0,0xfc,0x0e,0x71,0xc0,0x00,
0x01,0xc3,0xe0,0x7e,0x0e,0x71,0xc0,0x00,0x00,0xe1,0xf8,0x1f,0x8e,0x71,0xc0,0x00,
0x00,0xe0,0x7e,0x07,0xfe,0x71,0xc0,0x00,0x00,0xf8,0x3f,0x83,0xfe,0x71,0xc0,0x00,
0x00,0x7c,0x0f,0xc0,0xfe,0x71,0xc0,0x00,0x00,0x3f,0x03,0xf0,0x3e,0x71,0xc0,0x00,
0x00,0x0f,0xc0,0xfc,0x1e,0x71,0xc0,0x00,0x00,0x03,0xf0,0x7f,0x0e,0x71,0xc0,0x00,
0x00,0x01,0xf8,0x1f,0x8e,0x71,0xc0,0x00,0x00,0x00,0x7e,0x07,0xfe,0x71,0xc0,0x00,
0x00,0x00,0x1f,0x83,0xfe,0x71,0xc0,0x00,0x00,0x00,0x0f,0xe0,0xfe,0x71,0xc0,0x00,
0x00,0x00,0x03,0xf0,0x3e,0x71,0xc0,0x00,0x00,0x00,0x00,0xfc,0x1e,0x71,0xc0,0x00,
0x00,0x00,0x00,0x3f,0x0e,0x71,0xc0,0x00,0x00,0x00,0x00,0x1f,0x8e,0x71,0xc0,0x00,
0x00,0x00,0x00,0x07,0xfe,0x31,0xc0,0x00,0x00,0x00,0x00,0x01,0xfe,0x01,0xc0,0x00,
0x00,0x00,0x00,0x00,0x7e,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,

};

char graphic1[]={												//large text
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xe0,0x00,0x00,0x00,
0x00,0x07,0xc0,0x3f,0xf8,0x00,0x00,0x00,0x00,0x3f,0xf8,0xff,0xfe,0x00,0x00,0x00,
0x00,0x7f,0xfc,0xff,0xff,0x00,0x00,0x00,0x00,0x7f,0xfc,0xff,0xff,0x80,0x00,0x00,
0x01,0xff,0xff,0xff,0xff,0xc0,0x00,0x00,0x01,0xfc,0x3f,0xf0,0x3f,0xc0,0x04,0xc0,
0x01,0xf8,0x0f,0xe0,0x0f,0xc0,0x09,0x20,0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x09,0x20,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x06,0x40,0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x06,0xc0,
0x01,0xfc,0x3f,0xe0,0x1f,0xc0,0x09,0x20,0x01,0xff,0xff,0xf0,0x3f,0xc0,0x09,0x20,
0x00,0xff,0xfd,0xff,0xff,0x80,0x09,0x20,0x00,0x7f,0xfc,0xff,0xff,0x80,0x0f,0xe0,
0x00,0x7f,0xfc,0xff,0xff,0x00,0x00,0x00,0x00,0x1f,0xf0,0x3f,0xfe,0x00,0x00,0x00,
0x00,0x07,0xc0,0x1f,0xf8,0x00,0x00,0x20,0x00,0x00,0x00,0x0f,0xe0,0x00,0x00,0x20,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xe0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x1f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x1f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x1f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0f,0xe0,0x00,0x00,0x00,0x00,0x07,0xc0,0x3f,0xf8,0x00,0x00,0x00,
0x00,0x3f,0xf8,0xff,0xfe,0x00,0x00,0x00,0x00,0x7f,0xfc,0xff,0xff,0x00,0x05,0xc0,
0x00,0x7f,0xfc,0xff,0xff,0x80,0x09,0x20,0x01,0xff,0xff,0xff,0xff,0xc0,0x08,0x20,
0x01,0xfc,0x3f,0xf0,0x3f,0xc0,0x08,0x20,0x01,0xf8,0x0f,0xe0,0x0f,0xc0,0x07,0xc0,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x08,0x60,0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x05,0x80,
0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x02,0x00,0x01,0xfc,0x3f,0xe0,0x1f,0xc0,0x01,0x00,
0x01,0xff,0xff,0xf0,0x3f,0xc0,0x0f,0xe0,0x00,0xff,0xfd,0xff,0xff,0x80,0x00,0x00,
0x00,0x7f,0xfc,0xff,0xff,0x80,0x00,0x00,0x00,0x7f,0xfc,0xff,0xff,0x00,0x00,0x00,
0x00,0x1f,0xf0,0x3f,0xfe,0x00,0x00,0x00,0x00,0x07,0xc0,0x1f,0xf8,0x00,0x00,0x00,
0x00,0x00,0x00,0x0f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xe0,0x00,0x00,0x00,
0x00,0x07,0xc0,0x3f,0xf8,0x00,0x00,0x00,0x00,0x3f,0xf8,0xff,0xfe,0x00,0x00,0x00,
0x00,0x7f,0xfc,0xff,0xff,0x00,0x00,0x00,0x00,0x7f,0xfc,0xff,0xff,0x80,0x00,0x00,
0x01,0xff,0xff,0xff,0xff,0xc0,0x00,0x00,0x01,0xfc,0x3f,0xf0,0x3f,0xc0,0x00,0x00,
0x01,0xf8,0x0f,0xe0,0x0f,0xc0,0x00,0x00,0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,
0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x00,0x00,0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x00,0x00,
0x01,0xfc,0x3f,0xe0,0x1f,0xc0,0x00,0x00,0x01,0xff,0xff,0xff,0xff,0xc0,0x00,0x00,
0x00,0xff,0xfd,0xff,0xff,0x80,0x00,0x00,0x00,0x7f,0xfc,0xff,0xff,0x80,0x00,0x00,
0x00,0x3f,0xf8,0x7f,0xfe,0x00,0x00,0x00,0x00,0x1f,0xf0,0x3f,0xfe,0x00,0x00,0x00,
0x00,0x07,0xc0,0x1f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0f,0xe0,0x00,0x00,0x00,0x00,0x07,0xc0,0x3f,0xf8,0x00,0x00,0x00,
0x00,0x3f,0xf8,0xff,0xfe,0x00,0x00,0x00,0x00,0x7f,0xfc,0xff,0xff,0x00,0x00,0x00,
0x00,0x7f,0xfc,0xff,0xff,0x80,0x00,0x00,0x01,0xff,0xff,0xff,0xff,0xc0,0x12,0x40,
0x01,0xfc,0x3f,0xf0,0x3f,0xc0,0x12,0x40,0x01,0xf8,0x0f,0xe0,0x0f,0xc0,0x12,0x40,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x1f,0xc0,0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,
0x01,0xf0,0x0f,0xc0,0x07,0xe0,0x00,0x00,0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x0f,0x80,
0x01,0xf8,0x0f,0xc0,0x07,0xe0,0x10,0x40,0x01,0xfc,0x3f,0xe0,0x1f,0xc0,0x10,0x40,
0x01,0xff,0xff,0xff,0xff,0xc0,0x10,0x40,0x00,0xff,0xfd,0xff,0xff,0x80,0x1f,0xc0,
0x00,0x7f,0xfc,0xff,0xff,0x80,0x00,0x00,0x00,0x3f,0xf8,0x7f,0xfe,0x00,0x00,0x00,
0x00,0x1f,0xf0,0x3f,0xfe,0x00,0x0f,0x80,0x00,0x07,0xc0,0x1f,0xf8,0x00,0x10,0x40,
0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x40,
0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x1f,0xc0,
0x00,0x00,0x00,0x3f,0x00,0x00,0x0e,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x01,0xc0,
0x00,0x00,0x00,0x3f,0x00,0x00,0x0e,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x1f,0xc0,
0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};


char graphic2[]={													//eagle
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0x77,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xbb,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xc8,0xff,0xff,0xff,0xff,0xff,0xff,0xf9,0xc4,0x7f,0xff,0xff,0xff,0xff,0xff,
0xff,0x20,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x07,0xff,0xff,0xff,0xff,0xff,
0xf9,0xc0,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x01,0x7f,0xff,0xff,0xef,0xff,
0xff,0x80,0x00,0x3f,0xff,0xff,0xe7,0xff,0xff,0xc0,0x00,0x1f,0xff,0xff,0xeb,0xff,
0xff,0xa0,0x00,0x1f,0xff,0xff,0xfa,0xff,0xff,0xc0,0x00,0x07,0xff,0xff,0xfd,0xff,
0xff,0xf0,0x00,0x01,0xff,0xff,0xf5,0x7f,0xff,0xf0,0x00,0x01,0xff,0xff,0xfe,0xff,
0xff,0xf8,0x00,0x40,0xff,0xff,0xf7,0x9f,0xff,0xfc,0x00,0x00,0x3f,0xff,0xff,0x3f,
0xff,0xfc,0x00,0x00,0x1f,0xff,0xfc,0xff,0xff,0xfe,0x00,0x00,0x1f,0xff,0x7f,0xff,
0xff,0xff,0x00,0x04,0x07,0xfe,0xff,0xef,0xff,0xff,0x40,0x00,0x03,0xfe,0x0f,0xef,
0xff,0xff,0x82,0x00,0x41,0xfc,0x63,0xcf,0xff,0xff,0xc0,0x00,0x00,0x7c,0x43,0xef,
0xff,0xff,0xc0,0x81,0x20,0x30,0xc1,0x3f,0xff,0xff,0xf0,0x00,0x00,0x00,0x89,0xef,
0xff,0xff,0xe0,0x00,0x00,0x00,0x11,0xef,0xff,0xff,0xf0,0x00,0x00,0x00,0x18,0x4f,
0xff,0xff,0xfc,0x00,0x01,0x00,0x31,0xff,0xff,0xff,0xfe,0x00,0x00,0x20,0x33,0xff,
0xff,0xff,0xff,0x80,0x00,0xe8,0x27,0xe7,0xff,0xff,0xff,0xc0,0x00,0x60,0x0a,0xef,
0xff,0xff,0xff,0xc0,0x00,0x22,0x0e,0xff,0xff,0xff,0xff,0xc0,0x00,0x20,0x0b,0xff,
0xff,0xff,0xff,0xe0,0x00,0x00,0x0f,0xff,0xff,0xff,0xff,0xf0,0x01,0x00,0x3f,0xff,
0xff,0xff,0xff,0xf8,0x00,0x20,0x3f,0xff,0xff,0xff,0xff,0xf8,0x00,0x80,0x3f,0xff,
0xff,0xff,0xff,0xfc,0x00,0x00,0x3f,0xff,0xff,0xff,0xff,0xfe,0x00,0x20,0x3f,0xff,
0xff,0xff,0xff,0xfd,0xc0,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xc0,0x00,0x7f,0xff,
0xff,0xff,0xff,0xff,0xe0,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xe0,0x00,0x3f,0xff,
0xff,0xff,0xff,0xff,0xf0,0x00,0x7f,0xff,0xff,0xff,0xff,0xff,0xf8,0x00,0x7f,0xff,
0xff,0xff,0xff,0xff,0xf0,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xf8,0x00,0x3f,0xff,
0xff,0xff,0xff,0xfd,0xf8,0x00,0x7f,0xff,0xff,0xff,0xff,0xfd,0xf8,0x00,0x3f,0xff,
0xff,0xff,0xff,0xff,0xf8,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xb8,0x02,0x7f,0xff,
0xff,0xff,0xff,0xff,0x70,0x00,0x3f,0xff,0xff,0xff,0xff,0xf9,0x60,0x01,0x3f,0xff,
0xff,0xff,0xff,0xff,0x70,0x00,0x3f,0xff,0xff,0xff,0xff,0xfd,0xf8,0x00,0x3f,0xff,
0xff,0xff,0xff,0xfe,0x78,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x1f,0xff,
0xff,0xff,0xff,0xff,0xfc,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x3f,0xff,
0xff,0xff,0xff,0xff,0xfe,0x02,0x1f,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x1f,0xff,
0xff,0xff,0xff,0xff,0xfe,0x02,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x1f,0xff,
0xff,0xff,0xff,0xff,0xfe,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x1f,0xff,
0xff,0xff,0xff,0xff,0xff,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x1f,0xff,
0xff,0xff,0xff,0xff,0xfe,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xfe,0x01,0x5f,0xff,
0xff,0xff,0xff,0xff,0xfe,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x0f,0xff,
0xff,0xff,0xff,0xff,0xff,0x80,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0x88,0x0f,0xff,
0xff,0xff,0xff,0xff,0xff,0x80,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x47,0xff,
0xff,0xff,0xff,0xff,0xff,0x80,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0xff,
0xff,0xff,0xff,0xff,0xff,0x80,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x07,0xff,
0xff,0xff,0xff,0xff,0xff,0x80,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x47,0xff,
0xff,0xff,0xff,0xff,0xff,0xc0,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xc4,0x07,0xff,
0xff,0xff,0xff,0xff,0xff,0xc0,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x07,0xff,
0xff,0xff,0xff,0xff,0xff,0xe0,0x23,0xff,0xff,0xff,0xff,0xff,0xff,0xe2,0x07,0xff,
0xff,0xff,0xff,0xff,0xff,0xe0,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x07,0xff,
0xff,0xff,0xff,0xff,0xff,0xf0,0x1b,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x0b,0xff,
0xff,0xff,0xff,0xff,0xff,0xf8,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xfa,0x27,0xff,
0xff,0xff,0xff,0xff,0xff,0xf8,0x27,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x27,0xff,
0xff,0xff,0xff,0xff,0xff,0xfc,0x13,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x5b,0xff,
0xff,0xff,0xff,0xff,0xff,0xfc,0x5b,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x5b,0xff,
0xff,0xff,0xff,0xff,0xff,0xfd,0x5f,0xff,0xff,0xff,0xff,0xff,0xff,0xfd,0x5f,0xff,
0xff,0xff,0xff,0xff,0xff,0xf9,0x6f,0xff,0xff,0xff,0xff,0xff,0xff,0xf9,0x6f,0xff,
0xff,0xff,0xff,0xff,0xff,0xfd,0x6f,0xff,0xff,0xff,0xff,0xff,0xff,0xf5,0x7f,0xff,
0xff,0xff,0xff,0xff,0xff,0xed,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xed,0x7f,0xff,
0xff,0xff,0xff,0xff,0xff,0xfd,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

};


const uint8_t TEST_IMAGE[4][128] = {
 {15, 1, 1, 1, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 15},
 {0, 0, 0, 0, 0, 0, 255, 255, 96, 96, 96, 96, 96, 96, 96, 96, 255, 255, 0, 0, 192, 240, 176, 152, 152, 152, 152, 176, 240, 192, 0, 0, 48, 48, 24, 24, 152, 152, 152, 248, 240, 0, 0, 0, 192, 240, 56, 24, 24, 24, 48, 255, 255, 0, 0, 248, 248, 48, 24, 24, 24, 56, 240, 192, 0, 0, 255, 255, 48, 24, 24, 24, 56, 240, 224, 0, 0, 192, 240, 48, 24, 24, 24, 24, 48, 240, 192, 0, 0, 248, 248, 48, 24, 24, 24, 56, 240, 224, 0, 0, 192, 240, 176, 152, 152, 152, 152, 176, 240, 192, 0, 0, 224, 240, 152, 152, 152, 24, 24, 48, 32, 0, 0, 0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0, 0, 63, 63, 0, 0, 0, 0, 0, 0, 0, 0, 63, 63, 0, 0, 7, 31, 25, 49, 49, 49, 49, 57, 25, 9, 0, 0, 30, 63, 51, 51, 49, 49, 25, 31, 63, 32, 0, 0, 7, 31, 56, 48, 48, 48, 24, 63, 63, 0, 0, 255, 255, 24, 48, 48, 48, 56, 31, 7, 0, 0, 63, 63, 0, 0, 0, 0, 0, 63, 63, 0, 0, 7, 31, 24, 48, 48, 48, 48, 24, 31, 7, 0, 0, 63, 63, 0, 0, 0, 0, 0, 63, 63, 0, 0, 7, 31, 25, 49, 49, 49, 49, 57, 25, 9, 0, 0, 8, 25, 49, 49, 49, 51, 51, 31, 14, 0, 0, 0, 0, 0, 0, 0},
 {240, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 240}
};


void dispPic(unsigned char *lcd_string)
{
	unsigned int i,j;
	unsigned char page = 0xB0;			//Page Address + 0xB0
	comm_out(0xAE);					//Display OFF
	comm_out(0x40);					//Display start address + 0x40
	for(i=0;i<4;i++){				//32pixel display / 8 pixels per page = 4 pages
		comm_out(page);				//send page address
		comm_out(0x10);				//column address upper 4 bits + 0x10
		comm_out(0x00);				//column address lower 4 bits + 0x00
		for(j=0;j<128;j++){			//128 columns wide
			data_out(*lcd_string);		//send picture data
			lcd_string++;			//point to next picture data
			}
		page++;					//after 128 columns, go to next page
   		}
	comm_out(0xAF);					//Display ON
}

void lcd_soft_init() {
	comm_out(0xA0); // ADC select normal
	comm_out(0xAE); // LCD display on
	comm_out(0xC0); // COM output scan direction normal
	comm_out(0xA2); // LCD drive bias 1/9
	comm_out(0x2F); // Power supply operating mode 7
	comm_out(0x21); // Internal resistor ratio 1
	comm_out(0x81); // Set Vs output voltage electronic volume register...
	comm_out(0x2F); // .. to 0x3F (max).

	//comm_out(0xA5); // ALL POINTS ON
	//comm_out(0xA5); // ALL POINTS ON

	while(1){						//loop forever
		dispPic(TEST_IMAGE);					//show image
		for (;;);
		//delay(1000);					//wait 1s
		//dispPic(graphic1);
		//delay(1000);
		//dispPic(graphic2);
		//delay(1000);
	}
}



void lcd_test_pattern() {
	for (int page = 0; page < 4; page++) {
		// Page address
		comm_out(0xB0 | (page & 0x0F));

		// Column address = 0
		comm_out(0x10);
		comm_out(0x00);

		for (int col = 0; col < 128; col++) {
			data_out(TEST_IMAGE[page & 0x3][col]);
		}
	}
}