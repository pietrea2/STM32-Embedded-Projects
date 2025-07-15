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

#include "dfr0151.h"
#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <inttypes.h>


char msg[80];


int main(void)
{
  /* Reset of all peripherals. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();  
	
	
	//sprintf(msg, "hello_before rtc init");
  //print_msg(msg);
	
  rtc_init();

	//sprintf(msg, "after rtc init");
  //print_msg(msg);
	
	// set rtc time
	uint8_t tinit[7];
	eeprom_read_ext(0x00, tinit, 7);
	rtc_set_date(tinit[0], tinit[1], tinit[2], tinit[3]);
	rtc_set_time(tinit[4], tinit[5], tinit[6]);

  // Your start up code here
	uint8_t tcur[7];
	uint8_t talarm[7];
	
	uint16_t led_counter = 0;
	bool alarm_on = false;
	
  while (1)
  {
		rtc_get_time(tcur + 4, tcur + 5, tcur + 6);
		rtc_get_date(tcur, tcur + 1, tcur + 2, tcur + 3);
		
		sprintf(msg, "\nDate: %02u-%02u-%02u-%02u Time: %02u:%02u:%02u", 
			tcur[3], tcur[2], tcur[1], tcur[0], tcur[4], tcur[5], tcur[6]);
    print_msg(msg);
		
		//if(!(while_counter % 15)){
		//	rtc_set_time(2, 2, 50);
		//	rtc_set_date(02, 02, 07, 23);
		//}
		
		eeprom_read_ext(0x07, talarm, 7);
		
		if (alarm_on || memcmp(tcur, talarm, 7 * sizeof(uint8_t)) == 0)
		{
			if (led_counter == 0)
				print_msg("\nALARM");
			
			alarm_on = true;
			led_counter++;
			
			//toggle green LEDs
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); 
			HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); 
			
			if (led_counter == 10)
			{
				alarm_on = false;
				led_counter = 0;
				
				// turn off
				HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET); 
			}			
			HAL_Delay(998);
		}
		else 
			HAL_Delay(999);
  }
}