/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include <stdio.h>
#include "config.h"

int main(void)
{
  /* Reset of all peripherals. */
	//Hardware Abstraction Layer
	//provides easy access to board's registers and peripherals
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();


	/////////////////////
	//All func below defined in config.c
	/////////////////////
  /* Initialize all configured peripherals */
	//General Purpose Input/Output pins on board
	//use pins to connect to external peripherals
  MX_GPIO_Init();
	//configures UART (or serial) com protocol
	//that lets us print from Hercules
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  
  char message[100];
  sprintf(message, "Printing test");
  print_msg(message);

  /* Infinite loop */
	//need to loop bc the code doesn't use an OS
	//so nowhere for main to return to
  while (1)
  {
		//Blue LED is labeled LD2 on board
		//LED is outside chip
		//so it is connected thru GIPO
		//funct takes 1) port that GIPO is part of 2) GIPO pin that LED is connected to
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); //#define LD2_Pin GPIO_PIN_7 in main.h
    HAL_Delay(500);
		
		char message[100];
		sprintf(message, "Printing test");
		print_msg(message);
		
		
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		HAL_Delay(500);
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		HAL_Delay(500);
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		HAL_Delay(500);
		
  }
}