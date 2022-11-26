/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 1024
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t sentenceA[RX_BUFFER_SIZE] = {0};
uint8_t sentenceB[RX_BUFFER_SIZE] = {0};
uint8_t* data_to_process = sentenceB;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Log(UART_HandleTypeDef *huart, uint8_t *String, uint16_t Size)
{
	if (HAL_OK != HAL_UART_Transmit(huart, String, Size, 100)){
		Error_Handler();
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_CRC_Init();
	MX_I2C2_Init();
	MX_TIM1_Init();
	MX_USART2_UART_Init();
	MX_UART4_Init();

	/* USER CODE BEGIN 2 */
	char* initmsg = "Tsum getsz, jöhet a riz$a:\r\n";
	Log(&huart2, (uint8_t*) initmsg, strlen(initmsg));

	if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx_buffer, RX_BUFFER_SIZE)){
		Error_Handler();
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
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
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

void ProcessData(uint16_t size){
	Log(&huart2, data_to_process, size);
	Log(&huart2, (uint8_t*)"\r\n", 3);
}

void BuildSentence(uint16_t next_place, char incoming){
	static uint8_t* sentence_to_build = sentenceA;
	static uint16_t length_count = 0;

	// swap buffer on end of sentence
	if(incoming=='\n'){
		sentence_to_build[next_place] = '\0';
		if(data_to_process==sentenceA){
			data_to_process = sentenceB;
			sentence_to_build = sentenceA;
		}else{
			data_to_process = sentenceA;
			sentence_to_build = sentenceB;
		}
		ProcessData(length_count);
		length_count = 0;
	}else{
		sentence_to_build[next_place] = incoming;
		length_count ++;
	}

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t pos)
{
	//static uint16_t old_pos = 0;
	//uint16_t chars_received = 0;

	if(huart->Instance==UART4){

		/*
		if(pos != old_pos){
			if(pos > old_pos){
				chars_received = pos - old_pos;
				for(uint16_t i=0; i<chars_received; i++){
					BuildSentence(i, rx_buffer[old_pos + i]);
				}
			}else{
				chars_received = RX_BUFFER_SIZE - old_pos;
				for(uint16_t i=0; i<chars_received; i++){
					BuildSentence(i, rx_buffer[old_pos + i]);
				}
				for (uint16_t i = 0; i < pos; i++){
					BuildSentence(chars_received + i, rx_buffer[i]);
				}
			}
		}
		old_pos = pos;*/
		Log(&huart2, (uint8_t*)"PING\r\n", 7);

		/*if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx_buffer, RX_BUFFER_SIZE)){
			Error_Handler();
		}*/
	}
}
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
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(500);
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
