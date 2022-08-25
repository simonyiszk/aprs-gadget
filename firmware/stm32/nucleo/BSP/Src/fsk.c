/*
 * fsk.c
 *
 *  Created on: Aug 25, 2022
 *      Author: pali
 */

#include "fsk.h"
#include "main.h"

extern TIM_HandleTypeDef htim4;

static volatile uint8_t FSK_bit = 0;
static volatile uint8_t EOP = 0;

void HAL_TIM_PeriodElapsedCallBack(TIM_HandleTypeDef *htim){

	/*
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	if(htim->Instance==TIM4){

		static uint8_t dds_idx = 0;
		static uint8_t period = 0;

		htim4.Instance->CCR1 = sine[dds_idx];

		if(FSK_bit == 1){
			dds_idx += 6;
		}else{
			dds_idx += 11;
		}

		if(dds_idx > 197){
			dds_idx -= 198;
		}

		period++;
		if(period==33){
			period = 0;
			EOP = 1;
		}
	}*/
}

void FSK_Init(void){
	//HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}

void FSK_SetBit(uint8_t bit){
    while(!EOP);
    FSK_bit = bit;
    EOP = 0;
}
