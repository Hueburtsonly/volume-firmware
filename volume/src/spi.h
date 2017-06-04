/*
 * spi.h
 *
 *  Created on: 27 Dec 2016
 *      Author: Hueburtsonly
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void spi_init();
void spi_write(uint8_t cs, const uint8_t* data, uint8_t len);
void spi_write_read(uint8_t cs, const uint8_t* wdata, uint8_t wlen, uint8_t* rdata, uint8_t rlen);

#endif /* SPI_H_ */
