/*
 * usb_desc.h
 */

#ifndef USB_DESC_H_
#define USB_DESC_H_

extern const uint8_t USB_DeviceDescriptor[];
extern uint8_t USB_FsConfigDescriptor[];
extern const uint8_t USB_StringDescriptor[];
extern const uint8_t USB_DeviceQualifier[];
extern const uint8_t USB_BOSDescriptor[];
extern const uint8_t USB_MsOs20DescriptorSet[];

#define USB_MS_VENDOR_CODE				0x20	/* bRequest Windows uses to fetch the MS OS 2.0 set */
#define USB_MS_OS_20_DESCRIPTOR_INDEX	0x07	/* wIndex for MS_OS_20_DESCRIPTOR_INDEX */
#define USB_BOS_DESC_TOTAL_LENGTH		0x0021
#define USB_MS_OS_20_DESC_SET_LENGTH	0x00A2

#endif /* USB_DESC_H_ */
