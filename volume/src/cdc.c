#include "cdc.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "debug.h"
#include "usbd/usbd.h"
#include "usb.h"

#include "string.h"

PRE_PACK struct POST_PACK _LINE_CODING_STRUCTURE
{
  uint32_t dwBaud;  // 0x00C20100
  uint8_t  bCharFormat; // 0
  uint8_t  bParityType; // 0
  uint8_t  bDataBits;   // 8
} lineCoding;

void handleGetLineCoding(uint16_t wValue, uint16_t wIndex, uint16_t wLength) {
	sendToEp0InWithStatusPacket((char*)&lineCoding, 7, wLength);
}

void handleSetLineCoding(uint16_t wValue, uint16_t wIndex, uint16_t wLength) {
	activateEndpoint(EP0IN, 0);
	if (wLength != 0) {
		if (wLength != 7) {
			dprintf("Unexpected wLength: %d instead of 7", wLength);
			return;
		}
		EPLIST[EP0OUT] = ACTIVE_EP(EP0OUT, 64);
		while (EPLIST[EP0OUT] & (1 << 31));
		memcpy(&lineCoding, EPBUFFER(EP0OUT), 7);
		dprintf("Baud rate: %u", (lineCoding.dwBaud));
	} else {
		static uint16_t prevWValue = 0;
		if ((wValue ^ prevWValue) & 0x1) {
			if (wValue & 0x1) {
				dprintf("CONNECTION");
			} else {
				dprintf("DISCONNECTION");
			}
		}
		prevWValue = wValue;
	}
}

void handleNonGenericSetup(
		uint8_t bmRequestType,
		uint8_t bRequest,
		uint16_t wValue,
		uint16_t wIndex,
		uint16_t wLength) {
	switch (bmRequestType) {
	case 0xa1:
		// Get line coding
		dprintf("Get line coding: %d %d %d", wValue, wIndex, wLength);
		handleGetLineCoding(wValue, wIndex, wLength);
		break;
	case 0x21:
		// Set line coding
		dprintf("Set line coding: %d %d %d", wValue, wIndex, wLength);
		handleSetLineCoding(wValue, wIndex, wLength);
		break;
	default:
    	// Unknown bmRequestType
    	dprintf("Unknown request type: bmRequestType = %d", bmRequestType);
	}
}


uint8_t readPos = 0;
void cdc_read(char* data, uint32_t len) {
	while (len) {
		waitForEndpoint(EP1OUT);
		uint16_t nBytes = (EPLIST[EP1OUT] >> 16) & 0x3ff;
		int16_t packetLen = 64 - nBytes;

		while (readPos < packetLen) {
			*(data++) = EPBUFFER(EP1OUT)[readPos++];
			if (--len == 0) break;
		}

		if (readPos >= packetLen) {
			readPos = 0;
			activateEndpoint(EP1OUT, 64);
		}
	}
}

// Reads until either a linefeed is encountered, or maxLen characters are read.
// - Any carriage returns (\r) are deleted/ignored.
// - A line-feed (\n) is converted to a null (\0) so that a null-terminated string is returned.
// - If no line-feed is encountered after maxLen characters, null-termination does NOT occur.
void cdc_read_line(char* data, uint32_t maxLen) {
	static char ignoreChar = '\0';
	while (maxLen) {
		waitForEndpoint(EP1OUT);
		uint16_t nBytes = (EPLIST[EP1OUT] >> 16) & 0x3ff;
		int16_t packetLen = 64 - nBytes;

		while (readPos < packetLen) {
			char newChar = *data = EPBUFFER(EP1OUT)[readPos++];

			if (newChar == '\r' || newChar == '\n') {
				if (newChar == ignoreChar) {
					ignoreChar = '\0';
				} else {
					ignoreChar = ('\r' ^ '\n') ^ newChar;
					// Replace with with NUL, and force function return (while still
					// allowing the endpoint to be reactivated if possible).
					*(data) = '\0';
					maxLen = 0;
					break;
				}
			} else {
				++data;
			}
			if (--maxLen == 0) break;
		}

		if (readPos >= packetLen) {
			readPos = 0;
			activateEndpoint(EP1OUT, 64);
		}
	}
}

void cdc_write(const char* data, uint32_t length) {
	sendToEndpoint(EP1IN, data, length);
}

void cdc_init() {
	usb_init();

	lineCoding.dwBaud = 9600;
	lineCoding.bCharFormat = 0;
	lineCoding.bParityType = 0;
	lineCoding.bDataBits = 8;

	activateEndpoint(EP1OUT, 64);

	// Enable interrupt on EP1OUT (MCU RX)
	//LPC_USB->INTEN |= 1 << 2;

	// Enable interrupt on EP1IN (MCU TX)
    //LPC_USB->INTEN |= 1 << 3;

	// Data can be sent even before a connection is made.
    //cdc_write(EP1IN, "So many datums.\r\n", 49-32);
}
