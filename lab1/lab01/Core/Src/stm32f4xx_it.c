/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_it.h"
#include <stdio.h>
#include "config.h"

/* External variables --------------------------------------------------------*/
extern int8_t current_col;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  // Your code here
	
	//COL0
	//handle buttons: 3, 7, B, F
	//check row pins:
	if(HAL_GPIO_ReadPin(ROW0_GPIO_Port, ROW0_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "3\n"); print_msg(message_1);
	}
	else if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "7\n"); print_msg(message_1);
	}
	else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "B\n"); print_msg(message_1);
	}
	else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "F\n"); print_msg(message_1);
	}

	//delay
	for (int i = 0; i < 90000000; i++){}

  HAL_GPIO_EXTI_IRQHandler(COL0_Pin);
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  // Your code here
	
	//check which col pin is set
	if (__HAL_GPIO_EXTI_GET_FLAG(COL1_Pin)) {
		//handle buttons: 2, 6, A, E
		
		if(HAL_GPIO_ReadPin(ROW0_GPIO_Port, ROW0_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "2\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "6\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "A\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "E\n"); print_msg(message_1);
		}
		
	}
	else if(__HAL_GPIO_EXTI_GET_FLAG(COL2_Pin)){
		//handle buttons: 1, 5, 9, D
		if(HAL_GPIO_ReadPin(ROW0_GPIO_Port, ROW0_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "1\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "5\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "9\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "D\n"); print_msg(message_1);
		}

	}
	else if(__HAL_GPIO_EXTI_GET_FLAG(COL3_Pin)){
		//handle buttons: 0, 4, 8, C
		if(HAL_GPIO_ReadPin(ROW0_GPIO_Port, ROW0_Pin) == GPIO_PIN_SET){
		char message_1[100]; sprintf(message_1, "0\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "4\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "8\n"); print_msg(message_1);
		}
		else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET){
			char message_1[100]; sprintf(message_1, "C\n"); print_msg(message_1);
		}

	}
	
	
	//delay
	for (int i = 0; i < 90000000; i++){}
	

  HAL_GPIO_EXTI_IRQHandler(COL1_Pin);
  HAL_GPIO_EXTI_IRQHandler(COL2_Pin);
  HAL_GPIO_EXTI_IRQHandler(COL3_Pin);
}


/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
	HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin); //toggle green LED
	HAL_Delay(500);
}	
*/


/** 
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
	
  if (__HAL_GPIO_EXTI_GET_FLAG(USER_Btn_Pin)) { //check if triggered by push button
    // Your code here
		
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin); //toggle green LED
		
		//print message
		char message_1[100];
		sprintf(message_1, "Pressed\n");
		print_msg(message_1);
		
		//delay
		for (int i = 0; i < 90000000; i++){}
  }

	//HAL_Delay(500);
	HAL_GPIO_EXTI_IRQHandler(USER_Btn_Pin);
	
}
