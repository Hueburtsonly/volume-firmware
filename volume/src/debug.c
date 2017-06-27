#include "debug.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "usb.h"
#include <stdio.h>
#include <stdarg.h>

void dprintf(const char const* format, ...) {
	uint32_t now = 0; //LPC_TIMER32_0->TC;
	char buf[160];

	// 01234567890123456
	// [4294.967295] 0
	buf[0] = '[';
	buf[1] = ' ';
	buf[2] = ' ';
	buf[3] = ' ';
	buf[4] = ' ';
	buf[12] = ']';
	buf[13] = ' ';
	buf[14] = (LPC_USB->DEVCMDSTAT & 0x7f) ? 'a' : ' ';
	buf[15] = ((EPLIST[EP0IN] | EPLIST[EP0OUT]) & 1 << 29) ? 's' : ' ';
	buf[16] = ' ';
	buf[17] = 0;

	uint8_t i = 11;
	while (i >= 4 || now) {
	  buf[i--] = '0' + (now % 10);
	  now /= 10;
	  if (i == 5) {
		  buf[i--] = '.';
	  }
	}
	DEBUGSTR(buf);

	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	DEBUGSTR(buf);
	DEBUGSTR("\r\n");
}

void debug_init() {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CT32B0);
	//LPC_TIMER32_0->TC = 0;
	//LPC_TIMER32_0->PR = 48 - 1;

	//LPC_TIMER32_0->TCR = 0b1;
}


/* Outputs a string on the debug UART */
void DEBUGSTR(char *str)
{
	while (*str != '\0') {
		Chip_UART_SendBlocking(LPC_USART, str++, 1);
	}
}
