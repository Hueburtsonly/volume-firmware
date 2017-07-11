/*
 * lcd.h
 *
 *  Created on: 4 Jun 2017
 *      Author: Hueburtsonly
 */

#ifndef LCD_H_
#define LCD_H_

void lcd_init();
void lcd_soft_init(int index);
void lcd_test_pattern();
void lcd_disp(unsigned char *lcd_string);

#endif /* LCD_H_ */
