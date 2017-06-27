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

	//tlc5928_broadcast(0b1001101010101010);tlc5928_broadcast(0b1001101010101010);

	//CS = 0;

	LPC_GPIO->W[0][DRS] = is_data;

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

	//while(1){						//loop forever
		dispPic(TEST_IMAGE);					//show image
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
			data_out(TEST_IMAGE[page & 0x3][col]);
		}
	}
}
