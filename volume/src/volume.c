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

#include "cdc.h"
#include "ad9102.h"
#include "ad9552.h"
#include "debug.h"
#include "led.h"
#include "spi.h"
#include "touch.h"
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

	if (*str == 'a' || *str == 'A') {

		ad9552_vco_band_auto();
		cdc_write_str("Set to auto.\r\n");
		return;
	} else if (*str == 'p' || *str == 'P') {
		const char* it = str + 2;
		int32_t freq = parseInt32(&it);
		++it;
		int32_t repeats = parseInt32(&it);
		if (*it == ' ') ++it;
		int32_t phase = parseInt32(&it); // optional, up to 65535

		int i;
		for (i = 0; i < repeats; i++) {
			ad9102_sine_burst(freq, phase);
		}
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

	} else if ((*str >= '0' && *str <= '9') || *str == '-') {

		const char* it = str;
		int32_t v = parseInt32(&it);

		v *= 800000;

		if (v < 5000000 || v > 1000000000) {
			cdc_write_str("Invalid freq.\r\n");
			return;
		}
		ad9552_frequency(v);
		ad9552_vco_band_auto();
		cdc_write_str("Set to freq: ");
		cdc_write_int(v);
		cdc_write_str("\r\n");
		return;
	} else if (*str == '.') {
		ad9102_dout(1);
		return;
	} else if (*str == ',') {
		ad9102_dout(0);
		return;
	} else if (*str == '&') {
		cdc_write_str("TLC test mode\r\n");
		for (;;) {
			test5928();
		}
	}

	cdc_write_str("Got this: \"");
	cdc_write_str(str);
	cdc_write_str("\"\r\n");
	cdc_write_str("Btw: ");
	cdc_write_uint(ad9102_pull_test());
	cdc_write_str("\r\n");

}


int main(void) {


	init5928();

    //led_on(LED_GREEN);
    //for (;;);
	setupBoardClocking();

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#endif

    // GPIO(6), TIMER(7,8), USART(12), ADC(13), USB(14), IOCON(16), RAM1(26), USBRAM(27)
    LPC_SYSCTL->SYSAHBCLKCTRL |= (1 << 6) | (0xf << 7) | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 16) | (1 << 26) | (1 << 27);
	LPC_IOCON->PIO0[18] = 0x1; // RXD=RXD
	LPC_IOCON->PIO0[19] = 0x1; // TXD=TXD
//383
    Chip_UART_Init(LPC_USART);

    led_init();
    led_on(LED_RED);


    // Generic SPI setup
    spi_init();
    touch_init();

    // USB_CONNECT enabled
    // set PIO0_6.FUNC = 0x1
    LPC_IOCON->PIO0[6] = 0x1;

    // USB_VBUS disable
    // set PIO0_3.FUNC = 0x0
    //LPC_IOCON->PIO0[3] = 0x0;

    DEBUGSTR("\r\n\r\n\r\n\r\n\r\n");
    debug_init();
    dprintf("rs20 AD9552 test starting up...");

    ad9552_reset();
    ad9102_reset();

    //dprintf("(0/3) AD9552...");
    ad9552_init();
    led_on(LED_BLUE);

	//volatile int i;
	//for (i = 0; i < 1000000; i++);
    while (!ad9552_is_locked());
    led_off(LED_RED);

    //dprintf("(1/3) AD9102...");
    ad9102_init();
    led_on(LED_GREEN);

    //dprintf("(2/3) CDC/USB...");
    cdc_init();

    //dprintf("(3/3) Done.");
    led_off(LED_BLUE);

	/*for (;;) {
		int ijk;
		for (ijk = 0; ijk < 10000; ijk++);

		touch_sample();


	}*/

    for (;;) {
    	char str[80];
    	memset(str, 0, 80);
    	cdc_read_line(str, 79);
    	handle(str);
    	//dprintf("> \"%s\"", str);
    }

    return 0 ;
}
