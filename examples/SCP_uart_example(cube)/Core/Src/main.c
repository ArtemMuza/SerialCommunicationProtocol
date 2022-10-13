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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SCP.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;

/* Definitions for Host */
osThreadId_t HostHandle;
const osThreadAttr_t Host_attributes = {
  .name = "Host",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Slave */
osThreadId_t SlaveHandle;
const osThreadAttr_t Slave_attributes = {
  .name = "Slave",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for myTask */
osThreadId_t myTaskHandle;
const osThreadAttr_t myTask_attributes = {
  .name = "myTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART4_UART_Init(void);
static void MX_USART5_UART_Init(void);
void HostTask(void *argument);
void SlaveTask(void *argument);
void StartTask(void *argument);

/* USER CODE BEGIN PFP */
void HostOut(Host _host);
void SlaveOut(Slave _slave);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t hostBuffer[1024];
uint8_t slaveBuffer[1024];
char outputBuffer[1024];
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
  MX_USART2_UART_Init();
  MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  /* USER CODE BEGIN 2 */
  const char* reset = "\r\nRESET\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)reset, 9, 10);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Host */
  HostHandle = osThreadNew(HostTask, NULL, &Host_attributes);

  /* creation of Slave */
  SlaveHandle = osThreadNew(SlaveTask, NULL, &Slave_attributes);

  /* creation of myTask */
  myTaskHandle = osThreadNew(StartTask, NULL, &myTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief USART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART5_UART_Init(void)
{

  /* USER CODE BEGIN USART5_Init 0 */

  /* USER CODE END USART5_Init 0 */

  /* USER CODE BEGIN USART5_Init 1 */

  /* USER CODE END USART5_Init 1 */
  huart5.Instance = USART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART5_Init 2 */

  /* USER CODE END USART5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HostOut(Host _host) {
	taskENTER_CRITICAL();
	uint16_t len = 0;


	strncpy(outputBuffer, "Host: ", 7);
	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 6, 100);
	if(REQ_TYPE(_host)) {
		//read
		strncpy(outputBuffer, "(read)\r\nRegister: ", 19);
		HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 18, 100);


	} else {
		strncpy(outputBuffer, "(write)\r\nRegister: ", 20);
		HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 19, 100);
	}
	len = sprintf(outputBuffer, "%u", _host.header.cmd1);
	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, len, 100);
	strncpy(outputBuffer, "\r\nData: ",9);
	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 8, 100);

	len = _host.header.cmd2;

	int index = 0;
	for (int i=0; i<len; i++)
		index += sprintf(&outputBuffer[index], "%u ", *(GET_DATA_PTR(_host) + i));
	index += sprintf(&outputBuffer[index], "\r\n\n ");

	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, index, 100);
	taskEXIT_CRITICAL();
}

void SlaveOut(Slave _slave) {
	taskENTER_CRITICAL();
	uint16_t len = 0;

	strncpy(outputBuffer, "Slave: ", 8);
	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 7, 100);
	if(REQ_TYPE(_slave)) {
		//read
		strncpy(outputBuffer, "(read)\r\nRegister: ", 19);
		HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 18, 100);


	} else {
		strncpy(outputBuffer, "(write)\r\nRegister: ", 20);
		HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 19, 100);
	}
	len = sprintf(outputBuffer, "%u", _slave.header.cmd1);
	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, len, 100);
	strncpy(outputBuffer, "\r\nData: ", 9);
	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, 8, 100);

	len = _slave.header.cmd2;

	int index = 0;
	for (int i=0; i<len; i++)
		index += sprintf(&outputBuffer[index], "%u ", *(GET_DATA_PTR(_slave) + i));
	index += sprintf(&outputBuffer[index], "\r\n\n ");

	HAL_UART_Transmit(&huart2, (uint8_t*)outputBuffer, index, 100);
	taskEXIT_CRITICAL();
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_HostTask */
/**
  * @brief  Function implementing the Host thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_HostTask */
void HostTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */

  Host pc;
  uint8_t byte = 0;
  uint8_t* data;
  for(;;)
  {
	  CreateHost(&pc, hostBuffer, 1024, addr_linux, 0x18, REQR);
	  HostOut(pc);

	  data = pc.CreateRequest(&pc);

	  for(int i = 0; i < REQUEST_SIZE(pc); i++) {
		  byte = *(data + i);
		  HAL_UART_Transmit(&huart4, &byte, 1, 100);
		  taskYIELD();

	  }
	  while(pc.header.mode != finish) {
		 if (HAL_UART_Receive(&huart4, &byte, 1, 100) != HAL_TIMEOUT)
		  pc.Read(&pc, byte);
		 else
			 taskYIELD();
	  }
	  if(pc.IsValid(&pc) == no_error) {

		  //OUTPUT
	  }
	  osDelay(100);
	  CreateHost(&pc, hostBuffer, 1024, addr_linux, 0x18, REQW);
	  byte = 0x33;
	  pc.WriteData(&pc, &byte, 1);
	  data = pc.CreateRequest(&pc);
	  for(int i = 0; i < REQUEST_SIZE(pc); i++) {
	  		  byte = *(data + i);
	  		  HAL_UART_Transmit(&huart4, &byte, 1, 100);
	  		  taskYIELD();
	  }
	  HostOut(pc);
	  while(pc.header.mode != finish) {
		  if (HAL_UART_Receive(&huart4, &byte, 1, 100) != HAL_TIMEOUT)
		  	pc.Read(&pc, byte);
		  else
		  	taskYIELD();
	  }
	  if(pc.IsValid(&pc) == no_error) {

	  		  //OUTPUT
	  }

	  osDelay(5000);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_SlaveTask */
/**
* @brief Function implementing the Slave thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SlaveTask */
void SlaveTask(void *argument)
{
  /* USER CODE BEGIN SlaveTask */
  /* Infinite loop */

  Slave stm;
  uint8_t byte = 0;
  uint8_t* data;

  CreateSlave(&stm, slaveBuffer, 1024);
  for(;;)
  {

	  while(stm.header.mode != finish) {
		  if (HAL_UART_Receive(&huart5, &byte, 1, 100) != HAL_TIMEOUT)
			  stm.Read(&stm, byte);
		  else
		  	taskYIELD();
	  }
	  if(stm.IsValid(&stm) == no_error) {

		  if(REQ_TYPE(stm)) {
			  //Read

			  //Get register address by GET_REG_ADDR
			  byte = 0x34;
			  stm.WriteData(&stm, &byte, 1);
			  data = stm.CreateResponse(&stm);
			  for(int i =0; i < RESPONSE_SIZE(stm); i++) {
			  	  HAL_UART_Transmit(&huart5, data + i, 1, 10);
			  	  taskYIELD();
			  }
		  } else {
			  //Write
			  //Get register address by GET_REG_ADDR
			  //Get data by GET_DATA_PTR
			  data = stm.CreateResponse(&stm);
			  for(int i =0; i < RESPONSE_SIZE(stm); i++) {
		  	  	  HAL_UART_Transmit(&huart5, data + i, 1, 10);
		  	  	  taskYIELD();
			  }

		  }

	  } else {
		  data = stm.CreateResponse(&stm);
		  for(int i =0; i < RESPONSE_SIZE(stm); i++) {
		  	  HAL_UART_Transmit(&huart5, data + i, 1, 10);
		  	  taskYIELD();
		  }

	  }
	  SlaveOut(stm);
	  CreateSlave(&stm, slaveBuffer, 1024);
  }
  /* USER CODE END SlaveTask */
}

/* USER CODE BEGIN Header_StartTask */
/**
* @brief Function implementing the myTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask */
void StartTask(void *argument)
{
  /* USER CODE BEGIN StartTask */
  /* Infinite loop */

  for(;;)
  {


  }
  /* USER CODE END StartTask */
}

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
