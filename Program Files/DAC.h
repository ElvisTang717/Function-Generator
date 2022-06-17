/*
 * DAC.h
 *
 *  Created on: Oct 30, 2021
 *      Author: Prestige
 */

#ifndef SRC_DAC_H_
#define SRC_DAC_H_

void DAC_GPIO_config(void);
void DAC_init(void);
void DAC_write(uint16_t);
void DAC_volt_conv(uint16_t);

#define SHDN 0x2000     // Bit 12
#define GAIN 0x1000		 // Bit 13
#define DAC_PORT GPIOA
#define DAC_RES 4095
#define VREF 3.3

#endif /* SRC_DAC_H_ */
