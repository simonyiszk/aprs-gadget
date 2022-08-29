/*
 * gps.c
 *
 *  Created on: Aug 30, 2022
 *      Author: pali
 */

#include <gps.h>
#include <main.h>

uint8_t gps_message[1024];

volatile uint8_t gps_busy = 0;
volatile uint8_t packet_available = 0;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

void GPS_ReceiveNMEA(void){
	if(!gps_busy){
		gps_busy = 1;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, gps_message, 1024);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
	}

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART1){
		packet_available = 1;
		//TODO: implement reading and processing NMEA batch
		gps_busy = 0;
	}
}
