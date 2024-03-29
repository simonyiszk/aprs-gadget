/*
 * fsk.c
 *
 *  Created on: Aug 25, 2022
 *      Author: pali
 */

#include <fsk.h>
#include <fifo.h>
#include <tim.h>

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;

static FIFO bitfifo;
static uint8_t fifo_buffer[512];
static uint8_t FSK_BUSY_FLAG = 0;

static void FSK_AlterFrequency(void){
	if(FIFO_pop(&bitfifo)==0){
		htim4.Instance->PSC = 5;
	}else{
		htim4.Instance->PSC = 10;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance==TIM6){

		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		if(FIFO_isempty(&bitfifo)){
			HAL_TIM_Base_Stop_IT(&htim6);
			HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_1);
			FIFO_reset(&bitfifo);
			FSK_BUSY_FLAG = 0;
		}else{
			FSK_AlterFrequency();
		}
	}
}

void FSK_Init(void){
	FIFO_init(&bitfifo, fifo_buffer, 512);
	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_SR_UIF);
}

void FSK_Send(void){
	if(FSK_BUSY_FLAG || FIFO_isempty(&bitfifo)){
		return;
	}

	FSK_BUSY_FLAG = 1;
	FSK_AlterFrequency();

	HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t*) sine, DDS_SAMPLES);
	HAL_TIM_Base_Start_IT(&htim6);
}

void FSK_AddBit(uint8_t bit){
	FIFO_push(&bitfifo, bit);
}
