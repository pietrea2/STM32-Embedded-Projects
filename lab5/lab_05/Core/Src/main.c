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

#include "config.h"
#include "ov7670.h"

#include <stdio.h>
#include <string.h>

#include <stdbool.h>


#define VIDEO_STREAM (1)

//uint16_t capture_buf[IMG_ROWS * IMG_COLS]; //RUN OUT OF MEMORY!!!
#if VIDEO_STREAM
uint8_t snapshot_buf[IMG_ROWS * IMG_COLS * 4];
uint8_t grayscale_buf[IMG_ROWS * IMG_COLS ];
#else
uint8_t snapshot_buf[IMG_ROWS * IMG_COLS * 2];
uint8_t grayscale_buf[IMG_ROWS * IMG_COLS ];
#endif

volatile uint8_t dma_flag = 0;


void print_buf(void);


int main(void)
{
  /* Reset of all peripherals */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();

  char msg[100];

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  ov7670_init();

#if VIDEO_STREAM
		ov7670_capture(snapshot_buf, IMG_ROWS * IMG_COLS * 2);
#endif

  while (1)
  {		
#if VIDEO_STREAM
		
		while(dma_flag == 0){
			HAL_Delay(0);
		}
		
		HAL_DCMI_Suspend(&hdcmi);
		print_buf();
		HAL_DCMI_Resume(&hdcmi);
		
		dma_flag = 0;

#else		
		//SNAPSHOT
    if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
      HAL_Delay(200);  // debounce
	
      print_msg("Snap!\r\n");
			ov7670_snapshot(snapshot_buf, IMG_ROWS * IMG_COLS * 2);
						
			while(dma_flag == 0){
				HAL_Delay(0);
			}
				
			HAL_DCMI_Stop(&hdcmi);
			print_buf();
			
			dma_flag = 0;
    }
#endif
  }
}


void print_buf() {
  // Send image data through serial port.
	
	//TESTING
	/*
	int tot_pixels = IMG_ROWS * IMG_COLS;
	uint8_t sequence[IMG_COLS*4];
	memset(sequence,0xFF,IMG_COLS*4);
	for(int i=0; i < IMG_ROWS/4; i++){
		uart_send_bin(sequence, sizeof(sequence));
	}
	*/
	

	//OLD VERSION
	/*
	#if VIDEO_STREAM
		//for (int i = 0; i < 2 * IMG_ROWS * IMG_COLS; ++i)
		for (int i = 0; i < IMG_ROWS * IMG_COLS; ++i)
	#else
		for (int i = 0; i < IMG_ROWS * IMG_COLS; ++i)
	#endif
		{
			//snapshot_buf[i] = snapshot_buf[2 * i + 1];
			grayscale_buf[i] = snapshot_buf[2 * i + 1];
		}
		
		print_msg("\r\nPREAMBLE!\r\n");
		for (int i = 0; i < IMG_ROWS; i+= 16)
			uart_send_bin(&grayscale_buf[i * IMG_COLS], IMG_COLS * 16);
	
	#if VIDEO_STREAM
		print_msg("\r\nPREAMBLE!\r\n");
		//for (int i = IMG_ROWS; i < 2 * IMG_ROWS; i+= 16)
		//	uart_send_bin(&snapshot_buf[i * IMG_COLS], IMG_COLS * 16);
		for (int i = 0; i < IMG_ROWS; i+= 1)
			uart_send_bin(&grayscale_buf[i * IMG_COLS], IMG_COLS * 1);
	#endif
	*/
	
	

	
	#if VIDEO_STREAM
		for (int i = 0; i < 2 * IMG_ROWS * IMG_COLS; ++i)
	#else
		for (int i = 0; i < IMG_ROWS * IMG_COLS; ++i)
	#endif
		{
			snapshot_buf[i] = snapshot_buf[2 * i + 1];
		}
		
		print_msg("\r\nPREAMBLE!\r\n");
		for (int i = 0; i < IMG_ROWS; i+= 16)
			uart_send_bin(&snapshot_buf[i * IMG_COLS], IMG_COLS * 16);
		
	#if VIDEO_STREAM
		print_msg("\r\nPREAMBLE!\r\n");
		for (int i = IMG_ROWS; i < 2 * IMG_ROWS; i+= 16)
			uart_send_bin(&snapshot_buf[i * IMG_COLS], IMG_COLS * 16);
	#endif
	
}
