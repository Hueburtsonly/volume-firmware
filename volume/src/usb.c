#include "usb.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "debug.h"
#include "string.h"
#include "usb_desc.h"

void sendToEndpoint(uint8_t ep, const void* data, uint16_t length) {
	while (length > 64) {
		waitForEndpoint(ep);
		memcpy(EPBUFFER(ep), data, 64);
		activateEndpoint(ep, 64);
		data += 64;
		length -= 64;
	}

	waitForEndpoint(ep);
	memcpy(EPBUFFER(ep), data, length);
	activateEndpoint(ep, length);
}

void sendToEp0InWithStatusPacket(const void* data, uint16_t length, uint16_t maxLength) {
	if (length > maxLength) length = maxLength;
	sendToEndpoint(EP0IN, data, length);
	waitForEndpoint(EP0IN);
    activateEndpoint(EP0OUT, 5);
}

void handleGetDescriptor(uint16_t wValue, uint16_t wIndex, uint16_t wLength) {
	switch (wValue >> 8) {
	case 0x01: // Device
		dprintf("  Device.");
		if (wIndex != 0 || wValue != 0x0100) {
			dprintf("    Unexpected wIndex.");
			return;
		}
		sendToEp0InWithStatusPacket(USB_DeviceDescriptor, USB_DeviceDescriptor[0], wLength);
		break;
	case 0x02: // Configuration
		dprintf("  Configuration, wIndex = %d, wLength = %d.", wIndex, wLength);
		if (wIndex != 0 || wValue != 0x0200) {
			dprintf("    Unexpected wIndex.");
			return;
		}
		sendToEp0InWithStatusPacket(
			USB_FsConfigDescriptor,
			*(uint16_t*)(USB_FsConfigDescriptor+2),
			wLength
		);

		break;
	case 0x03: // String
		{
			uint8_t stringIndex = wValue & 0xff;
			//dprintf("  String %d.", stringIndex);
			//if (stringIndex > 4) {
			//	dprintf("    Unexpected wIndex.");
			//	return;
			//}

			uint8_t currentString = 0;
			const uint8_t* base = USB_StringDescriptor;
			for (;;) {
				const uint8_t stringLength = *base;
				if (currentString == stringIndex) {
					sendToEp0InWithStatusPacket(base, stringLength, wLength);
					break;
				}
				currentString++;
				base += stringLength;
			}
		}
		break;
	case 0x06:
		dprintf("  0x06: Fullspeed attempt");

		//EPLIST[EP0OUT] = 1 << 29; //ACTIVE_EP(EP0OUT, 64);
		EPLIST[EP0IN] = 1 << 29; // Stall.

		while (0 == (LPC_USB->DEVCMDSTAT & (1 << 8)));

		//EPLIST[EP0OUT] = 0 << 29; //ACTIVE_EP(EP0OUT, 64);
		EPLIST[EP0IN] = 0 << 29; // Stall.

		break;

	default:
		dprintf("  Unknown wValue.");
		return;
	}
}


void handleSetAddress(uint16_t wValue, uint16_t wIndex, uint16_t wLength) {
	// Copy new address to DEV_ADDR
	LPC_USB->DEVCMDSTAT = (LPC_USB->DEVCMDSTAT & ~0x7f) | (wValue & 0x7f);

	while (EPLIST[EP0IN] & (1 << 31));
	activateEndpoint(EP0IN, 0);

	while (EPLIST[EP0IN] & (1 << 31));
}

void handleSetConfiguration(uint16_t wValue, uint16_t wIndex, uint16_t wLength) {
	activateEndpoint(EP0IN, 0);
}


void handleSetup() {

    volatile uint8_t bmRequestType = *((volatile uint8_t*)0x200040C0);
    volatile uint8_t bRequest =      *((volatile uint8_t*)0x200040C1);
    volatile uint16_t wValue =      *((volatile uint16_t*)0x200040C2);
    volatile uint16_t wIndex =      *((volatile uint16_t*)0x200040C4);
    volatile uint16_t wLength =     *((volatile uint16_t*)0x200040C6);

    // Ping setup
    LPC_USB->DEVCMDSTAT |= (1 << 8);

    switch (bmRequestType) {
    case 0x80: // Device to Host, Standard, Device
    	switch (bRequest) {
    	case 0x06:
    		dprintf("Get Descriptor.");
    		handleGetDescriptor(wValue, wIndex, wLength);
    		break;
    	default:
    		dprintf("Unknown device->host request.");
    		return;
    	}
    	break;
    case 0x00:
    	switch (bRequest) {
    	case 0x05: // Set address
    		dprintf("Set address to %d.", wValue);
    		handleSetAddress(wValue, wIndex, wLength);
    		break;
    	case 0x09:
    		dprintf("Set configuration %d.", wValue);
    		handleSetConfiguration(wValue, wIndex, wLength);
    		break;
    	default:
    		dprintf("Unknown host->device request, bRequest = %d", bRequest);
    		return;
    	}
    	break;

    default:
    	handleNonGenericSetup(bmRequestType, bRequest, wValue, wIndex, wLength);
    }
}

/* Initialize pin and clocks for USB0/USB1 port */
static void usb_pin_clk_init(void)
{
	/* enable USB main clock */
	Chip_Clock_SetUSBClockSource(SYSCTL_USBCLKSRC_PLLOUT, 1);
	/* Enable AHB clock to the USB block and USB RAM. */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_USB);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_USBRAM);
	/* power UP USB Phy */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_USBPAD_PD);

	NVIC_EnableIRQ(USB0_IRQn);
	NVIC_EnableIRQ(USB0_FIQ_IRQn);
	NVIC_SetPriority(USB0_IRQn, 3);
	NVIC_SetPriority(USB0_FIQ_IRQn, 2);
}

void USB_FIQHandler(void) {
	if (LPC_USB->INTSTAT & (1 << 0)) {
		// EP0OUT
		LPC_USB->INTSTAT = 1 << 0;
		if (0 != (LPC_USB->DEVCMDSTAT & (1 << 8))) {
		   handleSetup();
		}
	}
}

void usb_init() {
	usb_pin_clk_init();

	// Zero out USB SRAM.
	memset((void*)0x20004000, 0, 2048);

	// Configure EPLIST & USB buffers locations
    LPC_USB->EPLISTSTART = (uint32_t)EPLIST;
    LPC_USB->DATABUFSTART = 0x20000000; //(((uint32_t)&DATABUF) >> 11) << 22;

    // Tell USB peripheral where to place SETUP packets.
    EPLIST[1] = EPOFFSET(SETUP);

    // Enable FIQ interrupt for SETUP packets.
    LPC_USB->INTEN |= 1 << 0;
    LPC_USB->INTROUTING = 1 << 0;

    // Enable USB peripheral, enable connection.
    LPC_USB->DEVCMDSTAT =
    		1 << 7 | /* DEV_EN */
    		1 << 16; /* DCON */

    dprintf("USB enabled.");
}
