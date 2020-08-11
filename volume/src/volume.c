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


    // Reset displays
    tlc5928_broadcast(0b0100000000000000);

    // Wait for display reset
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0000000000000000);
    tlc5928_broadcast(0b0010000000000000);

    // Display test images
    for (int i = 0; i < 8; i++) {
    	tlc5928_demo(i);
    	lcd_soft_init(i);
    }


    LPC_TIMER32_1->TCR = 0b1;

    __enable_irq();

    led_on(LED_BLUE);
    led_on(LED_GREEN);

    //dprintf("(2/3) CDC/USB...");
    cdc_init();

    //dprintf("(3/3) Done.");
    led_off(LED_BLUE);
    led_off(LED_GREEN);
    led_off(LED_RED);


    activateEndpoint(EP4OUT, 52);

#define DISPLAYBUF 520

#define EX(v, k) ((uint16_t)((displayBuf)[(k)]) | ((uint16_t)((displayBuf)[(k)+1]) << 8))
    for (;;) {

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


