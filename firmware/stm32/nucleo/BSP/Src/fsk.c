/*
 * fsk.c
 *
 *  Created on: Aug 25, 2022
 *      Author: pali
 */

#include "fsk.h"
#include "main.h"

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;

static volatile uint8_t FSK_bit = 0;
static volatile uint8_t EOP = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	//if(htim->Instance==htim6.Instance){
		if(htim4.Instance->PSC==10){
			htim4.Instance->PSC = 5;
		}else{
			htim4.Instance->PSC = 10;
		}
	//}
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
