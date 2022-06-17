/*
 * keypad.c
 *
 *  Created on: Oct 30, 2021
 *      Author: Prestige
 */

#include "main.h"
#include "keypad.h"
#include <stdbool.h>

void keypad_init(void){
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN); //Enables GPIO BUS for port A and C


	//Columns == outputs; port C (PC0, PC1, PC2, PC3) will be used for outputs
	COL_PORT->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3); //clear mode bits for the input pins
	COL_PORT->MODER |= (1 << GPIO_MODER_MODE0_Pos | 1 << GPIO_MODER_MODE1_Pos | 1 << GPIO_MODER_MODE2_Pos | 1 << GPIO_MODER_MODE3_Pos);
	COL_PORT->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 ); //set output to push-pull
	COL_PORT->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3); //clear speed register
	COL_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3); //very fast speed
	COL_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3); //no pu/pd resistors
	COL_PORT->BSRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //turn on all pins


	//Rows = inputs; port C(PC5, PC6, PC8, PC9) will be used for inputs
	ROW_PORT->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE8 | GPIO_MODER_MODE9); //clear mode bits for outputs and set to input mode for all the pins
	ROW_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9); //clear pupdr bits
	ROW_PORT->PUPDR |= (GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD8_1 | GPIO_PUPDR_PUPD9_1); //set all input pins to pull-down

	return;
}


int8_t keypad_read(void){
	bool status;
	uint8_t current_col;
	int8_t button;

	status = check_rows();
	if (status == false){
		return NO_KEY;
	}
	else{
		for (current_col = 0; current_col <= 3; current_col++){
			switch (current_col){
				case 0:
					COL_PORT->BSRR = GPIO_PIN_0;
					COL_PORT->BRR = (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
					status = check_rows();
					if (status == true){
						current_col = 0;
						button = calc_button(current_col);
						COL_PORT->BSRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
						return button;
					}
				case 1:
					COL_PORT->BSRR = GPIO_PIN_1;
					COL_PORT->BRR = (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3);
					status = check_rows();
					if (status == true){
						current_col = 1;
						button = calc_button(current_col);
						COL_PORT->BSRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
						return button;
					}
				case 2:
					COL_PORT->BSRR = GPIO_PIN_2;
					COL_PORT->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);
					status = check_rows();
					if (status == true){
						current_col = 2;
						button = calc_button(current_col);
						COL_PORT->BSRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
						return button;
					}
				case 3:
					COL_PORT->BSRR = GPIO_PIN_3;
					COL_PORT->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
					status = check_rows();
					if (status == true){
						current_col = 3;
						button = calc_button(current_col);
						COL_PORT->BSRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
						return button;
					}
				}
		}
		COL_PORT->BSRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
		return NO_KEY;

	}
}

//returns true if one of the input pins go high
uint8_t check_rows(void){
	uint16_t rows = ROW_PORT->IDR;

	if(rows & (1<<5)){
		return true;
	}
	else if(rows & (1<<6)){
		return true;
	}
	else if(rows & (1<<8)){
		return true;
	}
	else if(rows & (1<<9)){
		return true;
	}
	return false;
}

//calculates which button was pressed based on columns and rows
int8_t calc_button(uint8_t current_col){
	uint16_t rows = ROW_PORT->IDR;

	if (rows & (1<<5)){
		switch(current_col){
			case 0:
				return 1;
			case 1:
				return 2;
			case 2:
				return 3;
			case 3:
				return A_KEY;
		}
	}
	else if(rows & (1<<6)){
		switch(current_col){
			case 0:
				return 4;
			case 1:
				return 5;
			case 2:
				return 6;
			case 3:
				return B_KEY;
		}
	}
	else if(rows & (1<<8)){
		switch(current_col){
			case 0:
				return 7;
			case 1:
				return 8;
			case 2:
				return 9;
			case 3:
				return C_KEY;
				}
	}
	else if(rows & (1<<9)){
		switch(current_col){
			case 0:
				return STAR;
			case 1:
				return 0;
			case 2:
				return POUND;
			case 3:
				return D_KEY;
				}
	}
	return false;
}
