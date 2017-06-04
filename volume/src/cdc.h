/*
 * cdc.h
 *
 */

#ifndef CDC_H_
#define CDC_H_

#include "stdint.h"
#include <string.h>

void cdc_read(char* data, uint32_t len);

// Reads until either a linefeed is encountered, or maxLen characters are read.
// - Any carriage returns (\r) are deleted/ignored.
// - A line-feed (\n) is converted to a null (\0) so that a null-terminated string is returned.
// - If no line-feed is encountered after maxLen characters, null-termination does NOT occur.
void cdc_read_line(char* data, uint32_t maxLen);
void cdc_write(const char* data, uint32_t length);

static inline void cdc_write_str(const char* str) {
	cdc_write(str, strlen(str));
}

static inline void cdc_write_uint(uint32_t v) {
	if (v >= 10) {
		cdc_write_uint(v / 10);
	}
	cdc_write("0123456789" + (v % 10), 1);
}

static inline void cdc_write_int(int32_t v) {
	if (v < 0) {
		cdc_write("-", 1);
		v = -v;
	}
	cdc_write_uint(v);
}

void cdc_init();

#endif /* CDC_H_ */
