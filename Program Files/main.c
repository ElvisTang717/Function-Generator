#include "main.h"
#include "math.h"
#include "keypad.h"
#include "DAC.h"

#define period 178	// set period time
#define samples 1800
#define duty_cycles 9
#define DAC_volt_3 3723
#define DAC_volt_0 0
#define pi 3.1415

uint16_t square_wave [duty_cycles][samples];
uint16_t sin_wave [samples];
uint16_t sawtooth_wave [samples];
uint16_t triangle_wave [samples];
uint8_t keypress;
uint8_t wave_tag = 9;
uint8_t frequency_tag = 1;
uint16_t index_num = 0;
uint8_t duty_tag = 4;

void SystemClock_Config(void);

int main(void)
{

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	DAC_init();
	keypad_init();

	//////////////// Timer & Interrupts Configure ///////////////
	// enable TIM2 clock
	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);

	// enable interrupts on UEV (update interrupt enable)
	// Enable CC1 Interrupt(capture compare 1 interrupt enable)
	TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);

	// set auto reload register continuously counting
	TIM2->ARR = 0xFFFFFFFF;
	// set initial CCR1 interrupt
	TIM2->CCR1 = period - 1;

	// Counter enable(CEN)
	TIM2->CR1 |= (TIM_CR1_CEN);

	// enable TIM2 ISR in NVIC
	NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));

	// enable interrupts globalLy
	__enable_irq();

	//////////////////// Look Up Table //////////////////////////

	/*Square Wave*/
	for(uint8_t i = 0; i < duty_cycles; i++){				// save different duty cycle square wave into one matrix
		for(uint16_t j = 0; j < samples; j++){
			if(j < (samples * (double)(i + 1)*0.1)){		// set duty cycle
				square_wave [i][j] = DAC_volt_3;			// set to high
			}
			else{
				square_wave [i][j] = DAC_volt_0;			// set to low
			}
		}
	}
	/*Sin Wave*/
	double x = 0;
	for(uint16_t i = 0; i < samples; i++){
		double dx = (2*pi)/samples;
		sin_wave[i] = (DAC_volt_3/2)*(1 + sinf(x));
		x += dx;
	}

	/*Sawtooth Wave*/
	for(uint16_t i = 0; i < samples; i++){
		sawtooth_wave[i] = (double) DAC_volt_3/samples * i; //slope = DAC_volt_3/samples
	}

	/*Triangle Wave*/
	for(uint16_t i = 0; i < samples; i++){
		if(i <= (samples/2)){								//first half of triangle
			triangle_wave[i] = (DAC_volt_3/(samples/2)) * i;
		}
		else{												//another half of triangle
			triangle_wave[i] = triangle_wave[samples - i];
		}
	}

	while (1) // do nothing in main after setting up TIM2
	{
		while(keypad_read() != NO_KEY);	    	// wait for button release

		keypress = keypad_read();

		while(keypress == NO_KEY){				// read until button press
			keypress = keypad_read();
		}

		if((keypress > 0) && (keypress < 6)){
			frequency_tag = keypress;			// set different global tag for different frequency and index
			index_num = 0;
		}
		else if((keypress > 5) && (keypress < 10)){
			wave_tag = keypress;				// set different global tag for different wave form
		}
		else if(wave_tag == 9){
			switch(keypress){
			case STAR:
				if(duty_tag<=0){ 				//make sure duty cycle won't less than 10%
					duty_tag = 0;
				}
				else{
					duty_tag--;					// decrease duty cycle
				}

				break;
			case 0:
				duty_tag = 4;					// duty cycle back to 50%
				break;
			case POUND:
				if(duty_tag>=8){				//make sure duty cycle won't greater than 90%
					duty_tag = 8;
				}
				else{
					duty_tag++;					// increase duty cycle
				}

				break;
			}
		}
		while(keypad_read() != NO_KEY);			// wait for button release
	}

}


// ISR name matches NVIC (replace n with Handler)
void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_CC1IF){
		TIM2->CCR1 += period;					// set CCR1 interrupt continuously
		index_num += frequency_tag;				// set index increases continuously with interval of frequency_tag
		index_num %= samples;					// make sure index won't greater than sample numbers
		switch(wave_tag){						// base on different global tag generate different wave form frequency and duty cycle
		case 6:
			DAC_write(sin_wave[index_num]);		// generate sin wave
			break;
		case 7:
			DAC_write(triangle_wave[index_num]);// generate triangle wave
			break;
		case 8:
			DAC_write(sawtooth_wave[index_num]);// generate sawtooth wave
			break;
		case 9:
			DAC_write(square_wave[duty_tag][index_num]);// generate square wave
			break;
		}
		TIM2->SR &= ~(TIM_SR_CC1IF);			// clear interrupt flag
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
