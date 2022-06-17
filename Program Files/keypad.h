/*
 * keypad.h
 *
 *  Created on: Oct 30, 2021
 *      Author: Prestige
 */

#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_

#define TRUE 1
#define FALSE 0

#define COL_PORT GPIOC
#define ROW_PORT GPIOC

//defines for the extra buttons
#define STAR 10
#define POUND 11
#define A_KEY 12
#define B_KEY 13
#define C_KEY 14
#define D_KEY 15
#define NO_KEY 99

void keypad_init(void);
uint8_t check_rows(void);
int8_t keypad_read(void);
int8_t calc_button(uint8_t current_col);

#endif /* SRC_KEYPAD_H_ */
