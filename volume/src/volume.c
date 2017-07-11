/*
===============================================================================
 Name        : ad9552pcb.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#include "ambient.h"
#include "cdc.h"
#include "debug.h"
#include "encoder.h"
#include "lcd.h"
#include "led.h"
#include "tlc5928.h"
#include "touch.h"
#include "usb.h"
#include "usbd/usbd.h"
#include <string.h>


/* Setup system clocking */
static void setupBoardClocking(void)
{
	volatile int i;

	Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC);


	/* Powerup main oscillator */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_SYSOSC_PD);



	/* Wait 200us for OSC to be stablized, no status
	   indication, dummy wait. */
	for (i = 0; i < 0x100; i++) {}




   // led_on(LED_RED);
	/* Set system PLL input to main oscillator */
	Chip_Clock_SetSystemPLLSource(SYSCTL_PLLCLKSRC_MAINOSC);
	//led_on(LED_GREEN);
	/* Power down PLL to change the PLL divider ratio */
	Chip_SYSCTL_PowerDown(SYSCTL_POWERDOWN_SYSPLL_PD);


	//led_on(LED_BLUE);
	/* Setup PLL for main oscillator rate (FCLKIN = 12MHz) * 4 = 48MHz
	   MSEL = 3 (this is pre-decremented), PSEL = 1 (for P = 2)
	   FCLKOUT = FCLKIN * (MSEL + 1) = 12MHz * 4 = 48MHz
	   FCCO = FCLKOUT * 2 * P = 48MHz * 2 * 2 = 192MHz (within FCCO range) */
	Chip_Clock_SetupSystemPLL(3, 1);




	/* Powerup system PLL */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_SYSPLL_PD);

	/* Wait for PLL to lock */
	while (!Chip_Clock_IsSystemPLLLocked()) {}

	/* Set system clock divider to 1 */
	Chip_Clock_SetSysClockDiv(1);

	/* Setup FLASH access to 3 clocks */
	Chip_FMC_SetFLASHAccess(FLASHTIM_50MHZ_CPU);

	/* Set main clock source to the system PLL. This will drive 48MHz
	   for the main clock and 48MHz for the system clock */
	Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_PLLOUT);

	/* Set USB PLL input to main oscillator */
	Chip_Clock_SetUSBPLLSource(SYSCTL_PLLCLKSRC_MAINOSC);
	/* Setup USB PLL  (FCLKIN = 12MHz) * 4 = 48MHz
	   MSEL = 3 (this is pre-decremented), PSEL = 1 (for P = 2)
	   FCLKOUT = FCLKIN * (MSEL + 1) = 12MHz * 4 = 48MHz
	   FCCO = FCLKOUT * 2 * P = 48MHz * 2 * 2 = 192MHz (within FCCO range) */
	Chip_Clock_SetupUSBPLL(3, 1);

	/* Powerup USB PLL */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_USBPLL_PD);


	/* Wait for PLL to lock */
	while (!Chip_Clock_IsUSBPLLLocked()) {}
}
/*
static int32_t parseInt32(const char** it) {
	int32_t v = 0;
	int m = 1;

	if (**it == '-') {
		m = -1;
		++*it;
	}
	while (**it >= '0' && **it <= '9') {
		v = v * 10 + (int32_t)(**it - '0');
		++*it;
	}
	return v * m;
}

void handle(const char* str) {
	if (*str == 0) {
		encoder_cdc_demo();
		return;
	} else if (*str == 'l' || *str == 'L') {
		ambient_measure();
		return;
	} else if (*str == 't' || *str == 'T') {
		int i;
		cdc_write_str("Printing raw touch samples:\r\n");
		for (i = 0; i < 5000; i++) {
			int ijk;
			for (ijk = 0; ijk < 10000; ijk++);

			touch_sample();


		}
		return;
	} else if (*str == 'd' || *str == 'D') {
		lcd_test_pattern();
		return;
	} else if (*str == 'r' || *str == 'R') {
		int i;
		cdc_write_str("Producing test patterns... ");

		for (i = 0; i < 128; i++) {
			int ijk;
			for (ijk = 0; ijk < 100000; ijk++);

			// triiger blue
			// analog red
#define pin_hi led_off
#define pin_lo led_on

			if (i == 0) {
				pin_lo(LED_BLUE);
				pin_hi(LED_BLUE);
				pin_lo(LED_RED);
				pin_hi(LED_RED);
				pin_lo(LED_RED);
				pin_hi(LED_RED);
				pin_hi(LED_RED);
			} else if (i == 127) {
				pin_lo(LED_BLUE);
				pin_hi(LED_BLUE);
				pin_lo(LED_RED);
				pin_hi(LED_RED);
				pin_hi(LED_RED);
				pin_lo(LED_RED);
				pin_hi(LED_RED);
			} else {
				pin_lo(LED_BLUE);
				pin_hi(LED_BLUE);
				pin_lo(LED_RED);
				pin_hi(LED_RED);
				pin_hi(LED_RED);
				pin_hi(LED_RED);
				pin_hi(LED_RED);
			}


		}

		cdc_write_str("Done.\r\n");
		return;
	} else if (*str == '&') {
		/ *cdc_write_str("TLC test mode\r\n");
		for (;;) {
			test5928();
		}* /
		return;
	}

	cdc_write_str("Got this: \"");
	cdc_write_str(str);
	cdc_write_str("\"\r\n");

}
*/
volatile int debug = 0;

int main(void) {


	//init5928();

    //led_on(LED_GREEN);
    //for (;;);
	setupBoardClocking();

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#endif

    // GPIO(6), TIMER(7,8,9,10), USART(12), ADC(13), USB(14), IOCON(16), RAM1(26), USBRAM(27)
    LPC_SYSCTL->SYSAHBCLKCTRL |= (1 << 6) | (0xf << 7) | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 16) | (1 << 26) | (1 << 27);
	LPC_IOCON->PIO0[18] = 0x1; // RXD=RXD
	LPC_IOCON->PIO0[19] = 0x1; // TXD=TXD
//383
    Chip_UART_Init(LPC_USART);

    led_init();
    led_on(LED_RED);

    lcd_init();
    ambient_init();
    tlc5928_init();
    encoder_init();

    // Generic SPI setup
    touch_init();

    // USB_CONNECT enabled
    // set PIO0_6.FUNC = 0x1
    LPC_IOCON->PIO0[6] = 0x1;

    // USB_VBUS disable
    // set PIO0_3.FUNC = 0x0
    //LPC_IOCON->PIO0[3] = 0x0;

    DEBUGSTR("\r\n\r\n\r\n\r\n\r\n");
    debug_init();
    dprintf("Starting up...");

    led_on(LED_RED);
    led_on(LED_GREEN);
    led_off(LED_BLUE);


    //tlc5928_broadcast(0b1001111111111111);

    //for (;;) {

    // Reset displays
    tlc5928_broadcast(0b0110101010101010);

    // Wait for display reset
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);
    tlc5928_broadcast(0b0010101010101010);

    // CS all displays + LED pattern
//    tlc5928_broadcast(0b1000011010101010);
    for (int i = 0; i < 8; i++) {
    	tlc5928_demo(i & 3);
    	lcd_soft_init(i & 3);
    }


    //lcd_test_pattern();

    //for (int kk = 0; kk < 17000; kk++) {
    //tlc5928_broadcast(0b0010101010101010);}
    //}
    // Display test image
    // TODO


    LPC_TIMER32_1->TC = 0;
    LPC_TIMER32_1->PR = 48 - 1;

    LPC_TIMER32_1->TCR = 0b1;
    LPC_TIMER32_1->MCR = 3; // Reset and interrupt on MR0 match
    LPC_TIMER32_1->MR[0] = 199;

	NVIC_EnableIRQ(TIMER_32_1_IRQn);
	NVIC_SetPriority(TIMER_32_1_IRQn, 0);

    __enable_irq();

    led_on(LED_BLUE);
    led_on(LED_GREEN);

    //for (;;) {
    //	debug = LPC_TIMER32_1->IR;
    //}

    //dprintf("(2/3) CDC/USB...");
    cdc_init();

    //dprintf("(3/3) Done.");
    led_off(LED_BLUE);
    led_off(LED_GREEN);
    led_off(LED_RED);


//for (;;) TIMER32_1_IRQHandler();

    activateEndpoint(EP4OUT, 50);

#define DISPLAYBUF 516

    //for (;;) {
    //	LPC_GPIO->NOT[0] = (1 << 23);
    //}
#define EX(v, k) ((uint16_t)((displayBuf)[(k)]) | ((uint16_t)((displayBuf)[(k)+1]) << 8))
    for (;;) {
    	//char str[80];
    	//memset(str, 0, 80);
    	//cdc_read_line(str, 79);
    	//handle(str);

    	ALIGNED(4) uint8_t displayBuf[DISPLAYBUF];

		activateEndpoint(EP3OUT, 64);
		waitForEndpoint(EP3OUT);
		int16_t packetLen = packetLength(EP3OUT);
		memcpy(displayBuf, EPBUFFER(EP3OUT), packetLen);
		uint16_t bufSize = EX(displayBuf, 0); //((uint16_t*)displayBuf)[0];
		if (bufSize != DISPLAYBUF) {
			continue;
		}
		uint16_t lcdToUpdate = EX(displayBuf, 2); //((uint16_t*)displayBuf)[1];

		setCsLcd(lcdToUpdate);

		int16_t upto = packetLen;

		while (upto != DISPLAYBUF) {
			activateEndpoint(EP3OUT, 64);
			waitForEndpoint(EP3OUT);
			packetLen = packetLength(EP3OUT);
			if (upto + packetLen > DISPLAYBUF) break;
			memcpy(displayBuf + upto, EPBUFFER(EP3OUT), packetLen);
			upto += packetLen;
		}

		lcd_disp(displayBuf + 4);

		led_toggle(LED_BLUE);
    }

    return 0;
}

volatile int countrrz = 0;

void TIMER32_1_IRQHandler (void) {
	LPC_TIMER32_1->IR = 1;
	handle_timer_interrupt();
}
