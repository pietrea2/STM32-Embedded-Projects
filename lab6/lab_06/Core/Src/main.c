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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ov7670.h"
#include <stdbool.h>

#define BASELINE 0
#define UART_WITH_DMA 1
#define BIT_4_PIXELS 2
#define BIT_4_PIXELS_RLE 3
#define VID_COMPR 4

#define IMPL BIT_4_PIXELS_RLE

/* USER CODE BEGIN PV */
#define PREAMBLE "\r\n!START!\r\n"
#define DELTA_PREAMBLE "\r\n!DELTA!\r\n"
#define SUFFIX "!END!\r\n"

uint8_t snapshot_buf[IMG_ROWS * IMG_COLS * 2];

#if IMPL == UART_WITH_DMA
uint8_t cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS];
uint8_t tx_buff[sizeof(PREAMBLE) - 1 + IMG_ROWS * IMG_COLS + sizeof(SUFFIX) - 1];
void print_buf_DMA(void);

#elif IMPL == BIT_4_PIXELS
uint8_t cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS/2];
uint8_t tx_buff[sizeof(PREAMBLE) - 1 + IMG_ROWS * IMG_COLS/2  + sizeof(SUFFIX) - 1];
void print_buf_DMA(void);

#elif IMPL == BIT_4_PIXELS_RLE
uint8_t cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS];
uint8_t tx_buff[sizeof(PREAMBLE) - 1 + IMG_ROWS * IMG_COLS/2  + sizeof(SUFFIX) - 1];
void print_buf_DMA(void);

#elif IMPL == VID_COMPR
uint8_t cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS];

uint8_t prev_cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS/2];
//uint8_t delta_buf[IMG_ROWS * IMG_COLS];
uint8_t tx_buff[sizeof(DELTA_PREAMBLE) - 1 + IMG_ROWS * IMG_COLS  + sizeof(SUFFIX) - 1];

int vid_compress_counter = 0;
#define vid_compress_N (5)

bool first_frame = true;

void print_buf_DMA(void);

#endif

#if IMPL == BASELINE
void print_buf(void);
#endif

/*
#if IMPL == UART_WITH_DMA || IMPL == BIT_4_PIXELS_RLE
uint8_t cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS];
#elif IMPL == BIT_4_PIXELS
uint8_t cmrsd_snapshot_buf[IMG_ROWS * IMG_COLS/2];
#endif

#if IMPL == UART_WITH_DMA
uint8_t tx_buff[sizeof(PREAMBLE) - 1 + IMG_ROWS * IMG_COLS + sizeof(SUFFIX) - 1];
#elif IMPL == BIT_4_PIXELS || IMPL == BIT_4_PIXELS_RLE
uint8_t tx_buff[sizeof(PREAMBLE) - 1 + IMG_ROWS * IMG_COLS/2  + sizeof(SUFFIX) - 1];
#endif


#if IMPL == UART_WITH_DMA || IMPL == BIT_4_PIXELS || IMPL == BIT_4_PIXELS_RLE
void print_buf_DMA(void);
#endif
*/


int do_assert(char* cond, char* msg)
{
	char buf[100];
	snprintf(buf, 100, "%s%s", cond, msg);
	print_msg(buf);
	abort();
}

size_t tx_buff_len = 0;
volatile uint8_t dma_flag = 0;

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
	
	//TESTING sending white frame
  //print_buf();
	
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  
  ov7670_init();
  //HAL_Delay(100);
	
#if IMPL == UART_WITH_DMA || IMPL == BIT_4_PIXELS
	memcpy(tx_buff, PREAMBLE, sizeof(PREAMBLE) - 1);
	memcpy(tx_buff + sizeof(tx_buff) - sizeof(SUFFIX) + 1, SUFFIX, sizeof(SUFFIX) - 1);

#elif IMPL == BIT_4_PIXELS_RLE
	//Don't copy the SUFFIX here yet, do that at right before sending bin_to_uart
	//Bc size of cmrsd_snapshot_buf (and therefore tx_buff
	memcpy(tx_buff, PREAMBLE, sizeof(PREAMBLE) - 1);
#endif
	
	ov7670_capture(snapshot_buf, IMG_ROWS * IMG_COLS * 2);
  
  while (1)
  {
    
		//SNAPSHOT CODE
		/*
    if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
      HAL_Delay(100);  // debounce
		
      print_msg("Snap!\r\n");
    }
		*/
		
		while(dma_flag == 0);
		
		HAL_DCMI_Suspend(&hdcmi);
#if IMPL == BASELINE
		print_buf();
#else
		print_buf_DMA();
#endif
		HAL_DCMI_Resume(&hdcmi);
		
		dma_flag = 0;
  }
}

#if IMPL == BIT_4_PIXELS
void pixel4_bw_compress(void)
{
	for (int i = 0; i < IMG_ROWS * IMG_COLS; i+=2){
		uint8_t pixel_1 = snapshot_buf[2 * i + 1] & 0xF0; //MSB
		uint8_t pixel_2 = (snapshot_buf[2 * (i+1) + 1] & 0xF0) >> 4;
		cmrsd_snapshot_buf[i/2] = pixel_1 | pixel_2;
	}
}
#endif


#if IMPL == BIT_4_PIXELS_RLE || IMPL == VID_COMPR
void RLE_bw_compress(void)
{
	//counter for snapshot buffer bc it might vary (be less than IMG_ROWS * IMG_COLS)
	int a = 0;
	for(int i = 0; i < IMG_ROWS * IMG_COLS; i++){
		
		uint8_t colour_counter = 1;
		while(i < IMG_ROWS * IMG_COLS - 1 && (snapshot_buf[2 * i + 1] & 0xF0) == (snapshot_buf[2 * (i+1) + 1] & 0xF0) ){
			colour_counter++;
			i++;
		
			//Send byte when counter reaches MAX = 15
			if(colour_counter == 15) {
				cmrsd_snapshot_buf[a] = (snapshot_buf[2 * (i - 1) + 1] & 0xF0) | (colour_counter);
				a++;
				colour_counter = 0;
			}
		}
		
		cmrsd_snapshot_buf[a] = (snapshot_buf[2 * i + 1] & 0xF0) | colour_counter;
		a++;
	} 
	tx_buff_len = a;
}
#endif

inline uint8_t nibble(uint8_t* buf, int i)
{
	return i % 2 == 0 ? buf[i/2] >> 4 : buf[i/2] & 0x0F;
}

#if IMPL == VID_COMPR
void RLE_pixel4_compress(void)
{
	//counter for snapshot buffer bc it might vary (be less than IMG_ROWS * IMG_COLS)
	int a = 0;
	for(int i = 0; i < IMG_ROWS * IMG_COLS && a < IMG_ROWS * IMG_COLS; ++i){
		
		uint8_t colour_counter = 1;
		while(i < IMG_ROWS * IMG_COLS - 1 && nibble(cmrsd_snapshot_buf, i) == nibble(cmrsd_snapshot_buf, i + 1)){
			colour_counter++;
			i++;
		
			//Send byte when counter reaches MAX = 15
			if(colour_counter == 15) {
				tx_buff[sizeof(PREAMBLE) - 1 + a] = (nibble(cmrsd_snapshot_buf, i - 1) << 4) | (colour_counter);
				a++;
				colour_counter = 0;
			}
		}
		
		tx_buff[sizeof(PREAMBLE) - 1 + a] = (nibble(cmrsd_snapshot_buf, i) << 4) | (colour_counter);
		a++;
	} 
	tx_buff_len = a;
}
#endif
	

#if IMPL == UART_WITH_DMA || IMPL == BIT_4_PIXELS || IMPL == BIT_4_PIXELS_RLE || IMPL == VID_COMPR
void print_buf_DMA()
{
	#if IMPL == BIT_4_PIXELS
	pixel4_bw_compress();
	
	#elif IMPL == BIT_4_PIXELS_RLE
	RLE_bw_compress();
		
	#elif IMPL == VID_COMPR
	//Regular 4 bit RLE

	bool is_delta_frame = true;
	
	if(first_frame || vid_compress_counter == vid_compress_N - 1){

		for (int i = 0; i < IMG_ROWS * IMG_COLS; i+=2){
			uint8_t pixel_1 = snapshot_buf[2 * i + 1] & 0xF0; //MSB
			uint8_t pixel_2 = (snapshot_buf[2 * (i+1) + 1] & 0xF0) >> 4;
			prev_cmrsd_snapshot_buf[i/2] = pixel_1 | pixel_2;
		}

		RLE_bw_compress();
		//memcpy(cmrsd_snapshot_buf, prev_cmrsd_snapshot_buf, IMG_ROWS * IMG_COLS/2);
		
		vid_compress_counter = 0;
		is_delta_frame = false;
	}
	
	//Calc Delta Frame
	else{
		for (int i = 0; i < IMG_ROWS * IMG_COLS; i+=2){
			uint8_t pixel_1 = snapshot_buf[2 * i + 1] & 0xF0; //MSB
			uint8_t pixel_2 = (snapshot_buf[2 * (i+1) + 1] & 0xF0) >> 4;
			cmrsd_snapshot_buf[i/2] = pixel_1 | pixel_2;
		}
		
		// compute deltas
		for (int i = 0; i < IMG_ROWS * IMG_COLS / 2; ++i)
		{
			uint8_t value = 0;
			value |= (cmrsd_snapshot_buf[i] & 0x0F) - (prev_cmrsd_snapshot_buf[i] & 0x0F);
			value |= ((cmrsd_snapshot_buf[i] >> 4) - (prev_cmrsd_snapshot_buf[i] >> 4)) << 4;
			cmrsd_snapshot_buf[i] = value;
		}
		
		RLE_pixel4_compress();
		
		is_delta_frame = true;
		vid_compress_counter++;
	}
	

	#elif IMPL == BASELINE
	for (int i = 0; i < IMG_ROWS * IMG_COLS; ++i){
		cmrsd_snapshot_buf[i] = snapshot_buf[2 * i + 1];
	}
	#endif
	
	#if IMPL == BIT_4_PIXELS_RLE
	while(HAL_UART_GetState(&huart3) != HAL_UART_STATE_READY);
	
	//copy sn
	memcpy(tx_buff + sizeof(PREAMBLE) - 1, cmrsd_snapshot_buf, sizeof(uint8_t) * tx_buff_len);
	memcpy(tx_buff + sizeof(PREAMBLE) - 1 + (sizeof(uint8_t) * tx_buff_len), SUFFIX, sizeof(SUFFIX) - 1);
	
	//while(HAL_UART_GetState(&huart3) != HAL_UART_STATE_READY);
	
	int e = uart_send_bin_DMA(tx_buff, sizeof(PREAMBLE) + sizeof(SUFFIX) - 2 + tx_buff_len);
	HAL_Delay(1);
	my_assert(!e, "send frame");
	
	#elif IMPL == VID_COMPR
	while(HAL_UART_GetState(&huart3) != HAL_UART_STATE_READY);
	
	if (!is_delta_frame || first_frame)
	{
		memcpy(tx_buff, PREAMBLE, sizeof(PREAMBLE) - 1);
		memcpy(tx_buff + sizeof(PREAMBLE) - 1, cmrsd_snapshot_buf, tx_buff_len);
	}
	else
	{
		memcpy(tx_buff, DELTA_PREAMBLE, sizeof(DELTA_PREAMBLE) - 1);
		//memcpy(tx_buff + sizeof(DELTA_PREAMBLE) - 1, delta_buf, tx_buff_len);
	}

	if (first_frame)
		first_frame = false;

  memcpy(tx_buff + sizeof(PREAMBLE) - 1 + tx_buff_len, SUFFIX, sizeof(SUFFIX) - 1);
	
	int e = uart_send_bin_DMA(tx_buff, sizeof(PREAMBLE) + sizeof(SUFFIX) - 2 + tx_buff_len);
	HAL_Delay(1);
	my_assert(!e, "send frame");
	
	#else
	while(HAL_UART_GetState(&huart3) != HAL_UART_STATE_READY);
	
	memcpy(tx_buff + sizeof(PREAMBLE) - 1, cmrsd_snapshot_buf, sizeof(cmrsd_snapshot_buf));
	
	int e = uart_send_bin_DMA(tx_buff, sizeof(tx_buff));
	my_assert(!e, "send frame");
	#endif
	
}
#endif


void print_buf() 
{
  // Send image data through serial port.
  // Your code here
	
	//TESTING SERIAL
	
	print_msg(PREAMBLE);
	//HAL_Delay(10);
	
	//uint8_t sequence[IMG_COLS*4];
	//memset(sequence,0xFF,IMG_COLS*4);
	//for(int i=0; i < IMG_ROWS/4; i++){
	//	uart_send_bin(sequence, sizeof(sequence));
	//}
	
	for (int i = 0; i < IMG_ROWS * IMG_COLS; i++){
		snapshot_buf[i] = snapshot_buf[2 * i + 1];
	}
	
	for (int i = 0; i < IMG_ROWS; i += 16)
		uart_send_bin(&snapshot_buf[i * IMG_COLS], 16 * IMG_COLS);
	
	//HAL_Delay(10);
	print_msg(SUFFIX);
	
}