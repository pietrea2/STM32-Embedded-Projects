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
#define _USE_MATH_DEFINES
#define M_PI           3.14159265358979323846
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "config.h"
#include "fixedpt.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool button_pressed = false;
int sine_square_terms = 1;
bool decrease_square_terms = false;


// h=0.01 rad
#define NSAMPLES (628)
#define NPERIODS (1)
#define MEASURETIME (0)

char message[128];



void part1(void)
{
	volatile int sum = 0; //or uint16_t
	
	int tstart = __HAL_TIM_GET_COUNTER(&htim6);
	for (int i = 0 ; i < 99 ; ++i){
		sum += i;
	}
	int tend = __HAL_TIM_GET_COUNTER(&htim6);

	sprintf(message, "----------- 1: for-loop execution time=%d\n", tend - tstart);
	print_msg(message);
}



void part2_sin_floating(void)
{
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
#if MEASURETIME
	int tstart = __HAL_TIM_GET_COUNTER(&htim6);
#else
	while(1) {
#endif
		for (int i = 0; i < NPERIODS * NSAMPLES; ++i) 
		{
			uint32_t value = (uint32_t)(2047 * (sin(i*2.0*M_PI/NSAMPLES) + 1.0));
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
		}
#if MEASURETIME
	int tend = __HAL_TIM_GET_COUNTER(&htim6);
		
	sprintf(message, "----------- 2.2: sin() floating execution time=%d\n", (tend - tstart));
	print_msg(message);
#else
	}
#endif
	HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
}



void part2_lut_floating(double* lut)
{
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
#if MEASURETIME
	int tstart = __HAL_TIM_GET_COUNTER(&htim6);
#else
	while(1) {
#endif
		for (int i = 0; i < NPERIODS* NSAMPLES; ++i) 
		{
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(2047 * (lut[i] + 1.0)));
		}
#if MEASURETIME
	int tend = __HAL_TIM_GET_COUNTER(&htim6);
		
	sprintf(message, "----------- 2.2: LUT floating execution time=%d\n", (tend - tstart));
	print_msg(message);
#else
	}
#endif
	HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
}



void part3_lut_floating(double* lut)
{
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	
	while(1)
	{
		for (int i = 0; i < NPERIODS*NSAMPLES; ++i)
		{
			double value = 0;
			for (int n = 0; n < sine_square_terms; ++n)
			{
				int a = 2*n + 1;
				value += lut[(i*a)%(NPERIODS*NSAMPLES)] / a;
			}
			value *= 4.0/M_PI;
			
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(1500 * (value + 1.0)));
		}
	}
	HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
}



void part3_lut_fixed(fixedpt* lut)
{
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	
	while(1)
	{
		for (int i = 0; i < NPERIODS*NSAMPLES; ++i)
		{
			fixedpt value = 0;
			for (int n = 0; n < sine_square_terms; ++n)
			{
				int a = 2*n + 1;
				value = FXD_ADD(value, FXD_DIV(lut[(i*a)%(NPERIODS*NSAMPLES)], FXD_FROM_INT(a)));
			}
			value = FXD_MUL(value, FXD_DIV(FXD_FROM_INT(4), FIXEDPT_PI));		
			value = FXD_ADD(value, FXD_FROM_INT(1));
			
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(1500 *(value >> (FXD_TOTAL_BITS - 12))));
		}
	}
	HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
}



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DAC_Init();
  MX_USART3_Init();
  //MX_USB_OTG_FS_PCD_Init();	
	HAL_DAC_Init(&hdac);
	
#if MEASURETIME
  MX_TIM6_Init(); //change prescaler here!!!!!
	HAL_TIM_Base_Start(&htim6);
#endif
  

	////// Part 1: for loop
	//part1();
	

	////////Create LUT for sin for 1 period
	double *lut = (double*)calloc(NPERIODS * NSAMPLES, sizeof(double));
	fixedpt *flut = (fixedpt*)calloc(NPERIODS * NSAMPLES, sizeof(fixedpt));
	
	for (int i = 0; i < NPERIODS * NSAMPLES; ++i) {
		lut[i] = sin(i*2.0*M_PI/NSAMPLES);
		flut[i] = FXD_FROM_FLOAT(lut[i]);
	}
	
	////// Part 2: sin wave
	//part2_sin_floating();
	//part2_lut_floating(lut);
	part3_lut_floating(lut);
	//part3_lut_fixed(flut);
	
	
	
	free(lut);
	free(flut);
	
	//MX_TIM6_Init();
	//HAL_TIM_Base_Start(&htim6);
	//
	//int while_counter = 0;
	//radians = 0;
	//int sin_start_2 = __HAL_TIM_GET_COUNTER(&htim6);
	
	
	
  //while (1)
  //{
	//	
	//	while(while_counter < 630){
	//		///////////////////////////Part 2.1: sin()
	//		if((radians >= 6.3) || (while_counter == 629)){ //6.283185
	//			
	//			int sin_end = __HAL_TIM_GET_COUNTER(&htim6);
	//			
	//			sprintf(message, "\n2.1: Sin() 1 Period Complete\n");
	//			print_msg(message);
	//			sprintf(message, "sin execution time=%d____\n", (sin_end - sin_start_2));
	//			print_msg(message);
	//			sprintf(message, "sin start time=%d\n", sin_start_2);
	//			print_msg(message);
	//			sprintf(message, "sin end time=%d\n", sin_end);
	//			print_msg(message);
	//			while_counter += 1;
	//			break;
	//		}
	//		
	//		//sin_val = sin(radians * 180 / M_PI) + 1.0;
	//		double sin_val = sin(radians) + 1.00;
	//		//sin_val = sin_val && 0xFFF000000000;
	//		
	//		//HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	//		//HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sin_val);
	//		
	//		////////////////printing sin() values
	//		
	//		if(!(while_counter % 15)){
	//			//sprintf(message, "%f\r\n", sin_val);
	//			//sprintf(message, "radians=%f, sin value=%f\r\n", radians, sin_val);
	//			//print_msg(message);
	//		}
	//		
	//		
	//		radians += 0.01;
	//		while_counter += 1;
	//	}
	//	
	//	
	//	
	//	/////////////////Part 3: Generating arbitrary waveforms
	//	
	//	//if(button_pressed){
	//		
	//	 int num_of_sines = sine_square_terms; //now that we pressed once
	//	 
	//	 double sin_square_val = 0;
	//	 for(int n = 0; n <= num_of_sines; n++){
	//		 sin_square_val += (4.0/(M_PI * (2*n+1))) * sin((2*n+1) * radians );
	//	 }	
	//		//sin_val = sin(2*radians) + 1.00;
	//		//sprintf(message, "button pressed now sin(2x)");
	//		//print_msg(message);
	//	//}
	//	
	//	double sin_val = sin(radians) + 1.00;
	//
	//	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	//	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sin_val);
	//	
	//	radians += 0.01;
	//	
	//	
	//	
  //}
}
