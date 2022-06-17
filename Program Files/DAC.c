/*
 * DAC.c
 *
 *  Created on: Oct 30, 2021
 *      Author: Prestige
 */

#include "main.h"
#include "DAC.h"


void DAC_GPIO_config()
{
	// PA4 = SPI1_CS, PA5 = SPI1_SCK, PA7 = SPI1_MOSI
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);	// IO port A clock enable

	// Clear MODE bits for PA4-PA7
	DAC_PORT->MODER &= ~(GPIO_MODER_MODE4
			| GPIO_MODER_MODE5
			| GPIO_MODER_MODE7);

	DAC_PORT->MODER |= (GPIO_MODER_MODE4_1
			|GPIO_MODER_MODE5_1
			| GPIO_MODER_MODE7_1);	// alternate function mode


	DAC_PORT->OTYPER &= ~(GPIO_OTYPER_OT4
			| GPIO_OTYPER_OT5
			| GPIO_OTYPER_OT7);	// Push-pull output

	DAC_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD4
			| GPIO_PUPDR_PUPD5
			| GPIO_PUPDR_PUPD7);	// no pu/pd resistor

	DAC_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4
			| GPIO_OSPEEDR_OSPEED5
			| GPIO_OSPEEDR_OSPEED7);	// high speed

	DAC_PORT->AFR[0] |= (5 << GPIO_AFRL_AFSEL4_Pos
			| 5 << GPIO_AFRL_AFSEL5_Pos
			| 5 << GPIO_AFRL_AFSEL7_Pos);
}


void DAC_init(void)
{

	DAC_GPIO_config();

	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN); 	//enable SP1

	SPI1 -> CR1 = (SPI_CR1_MSTR); // enable Master bit

	SPI1 ->CR2 = (SPI_CR2_DS  // enable 16 bit DS mode
			| SPI_CR2_NSSP); // NSSP mode

	SPI1 -> CR1 |= (SPI_CR1_SPE); // Enable SPI
}


void DAC_write(uint16_t data)
{
	while(!(SPI1->SR & SPI_SR_TXE));    // Check to Make Sure Buffer is Empty
	SPI1->DR  = (SHDN | GAIN| data);        // enable HIGH and SHDN bits
	//while(!(SPI1->SR & SPI_SR_RXNE));   // Wait for RXIFG to be Set (RXBUF Empty)

}

// converts milliVolts taken in to be converted into count for DAC
void DAC_volt_conv(uint16_t mVolt)
{
	int DAC_count;
	DAC_count = (mVolt * DAC_RES)/ (VREF * 1000);
	DAC_write(DAC_count);
}

