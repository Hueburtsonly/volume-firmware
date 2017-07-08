/*
 * usb.h
 *
 */

#ifndef USB_H_
#define USB_H_

#include "stdint.h"

// Memory map for USB RAM
//   2kiB from 0x20004000 to 0x20004800
// 0x20004000: EPLIST (0x00 - 0x3F)
// 0x20004040: EPLIST (0x40 - 0x4F)
// 0x20004080: EP0OUT buffer
// 0x200040C0: SETUP buffer
// 0x20004100: EP0IN buffer
// 0x20004140: RESERVED
// 0x20004180: EP1OUT buffer0
// 0x200041C0: EP1OUT buffer1
// 0x20004200: EP1IN buffer0
// 0x20004240: EP1IN buffer1
// 0x20004280: EP2OUT buffer0 ^^^^^^^^^^
// 0x200042C0: EP2OUT buffer1
// 0x20004300: EP2IN buffer0
// 0x20004340: EP2IN buffer1
// 0x20004380: EP3OUT buffer0
// 0x200043C0: EP3OUT buffer1
// 0x20004400: EP3IN buffer0
// 0x20004440: EP3IN buffer1
// 0x20004480: EP4OUT buffer0
// 0x200044C0: EP4OUT buffer1
// 0x20004500: EP4IN buffer0
// 0x20004540: EP4IN buffer1
// 0x20004580:   640 (0x0280) spare bytes
// 0x200045C0:
// 0x20004600:
// 0x20004640:
// 0x20004680:
// 0x200046C0:
// 0x20004700:
// 0x20004740:
// 0x20004780:
// 0x200047C0:

#define EP0OUT 0
#define SETUP  1
#define EP0IN  2
#define EP1OUT 4
#define EP1IN  6
#define EP2OUT 8
#define EP2IN  10
#define EP3OUT 12
#define EP3IN  14
#define EP4OUT 16
#define EP4IN  18

#define EPBUFFER(i) ((char*)(0x20004080 + 0x40 * (i)))

#define EPOFFSET(i) (0x102 + (i))

#define ACTIVE_EP(i, n) ((1 << 31) | ((uint32_t)(n) << 16) | EPOFFSET(i))

#define EPLIST ((volatile uint32_t*)0x20004000)



void usb_init();
void sendToEndpoint(uint8_t ep, const void* data, uint16_t length);
void sendToEp0InWithStatusPacket(const void* data, uint16_t length, uint16_t maxLength);

static inline void activateEndpoint(uint8_t ep, uint8_t numBytes) {
	EPLIST[ep] = ACTIVE_EP(ep, numBytes);
}

static inline void activateIsochronousEndpoint(uint8_t ep, uint8_t numBytes) {
	EPLIST[ep] = ACTIVE_EP(ep, numBytes) | (1 << 26);
}

static inline void waitForEndpoint(uint8_t ep) {
	while (EPLIST[ep] & (1 << 31));
}

// This is a callback that is called whenever a SETUP packet arrives
// that usb.c doesn't know how to handle.
void handleNonGenericSetup(uint8_t bmRequestType,
		uint8_t bRequest,
		uint16_t wValue,
		uint16_t wIndex,
		uint16_t wLength);

#endif /* USB_H_ */
