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

#include "CLCD_I2C.h"
#include "MPU6050.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;


CLCD_I2C_Name LCD1;
uint32_t volatile state = 0, resetCounter = 0;
extern uint32_t volatile delayLED;
extern unsigned int stepCount;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);

	if((GPIOA->IDR & (1u<<8)) == 0) {
		state++;
		
		for(int i = 0; i < 500000; i++);
		HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	}
	
	if((GPIOA->IDR & (1u<<9)) == 0) {
		resetCounter = 1;
		stepCount = 0;
		
		for(int i = 0; i < 500000; i++);
		HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	}
	
}

int main(void)
{
  SysTick->LOAD = SystemCoreClock / 1000;
	SysTick->CTRL = 0x7;

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
	MPU6050_Init();
	CLCD_I2C_Init(&LCD1,&hi2c1,0x4E,16,2);
	char buf[4];
	
  while (1)
  {
		if(resetCounter == 1) {
			resetCounter = 0;
			
			CLCD_I2C_Clear(&LCD1);
			CLCD_I2C_SetCursor(&LCD1,1,0);
		  CLCD_I2C_WriteString(&LCD1, "So buoc chan");
			CLCD_I2C_SetCursor(&LCD1,5,1);
		  CLCD_I2C_WriteString(&LCD1, "0");
		}
		
		if(state % 2 == 0) {
				GPIOC->ODR &= ~(1u<<14);
			if(delayLED >=1000) {
				delayLED = 0;
				GPIOC->ODR ^= (1u<<13);
			}	
			
			CLCD_I2C_SetCursor(&LCD1,1,0);
			CLCD_I2C_WriteString(&LCD1, "So buoc chan");
			sprintf(buf, "%d", MPU6050_Counter());
			CLCD_I2C_SetCursor(&LCD1,5,1);
			CLCD_I2C_WriteString(&LCD1, buf);
			
			HAL_Delay(50);
		} else {
			GPIOC->ODR |= (1u<<13); //Tat LED xanh
			GPIOC->ODR |= (1u<<14); //Bat LED do-
		}
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1; 
  hi2c1.Init.ClockSpeed = 100000; // Tan so SCL 100khz
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; 
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // 7 bit dia chi
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_GPIO_Init(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_Msk; //1u<<4
	GPIOC->CRH |= GPIO_CRH_MODE13_Msk; // bit 20::21 la 11
	
	/*Config GPIO PC15*/
	GPIOC->CRH |= GPIO_CRH_MODE14_Msk; // bit 28:29 la 11
	GPIOC->CRH &= ~GPIO_CRH_CNF14_Msk; // bit 26:27 la 00
	
	/* config GPIO PA8 */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_Msk; //1u<<2
	GPIOA->CRH &= ~GPIO_CRH_CNF8_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF8_1; // bit 10
	GPIOA->ODR |= (1u<<8); // pull up
	
	/* config GPIO PA9 */
	GPIOA->CRH &= ~GPIO_CRH_CNF9_Msk;
	GPIOA->CRH |= GPIO_CRH_CNF9_1; // bit 10
	GPIOA->ODR |= (1u<<9); // pull up
	
	/* EXTI interrupt init*/
  EXTI->IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9; // kich hoat ngat cho A8 va A9
	EXTI->FTSR |= EXTI_FTSR_FT8 | EXTI_FTSR_FT9; // falling
	NVIC_SetPriority(EXTI9_5_IRQn,0); // dat muc do uu tien cho ngat A8 va A9
	NVIC_EnableIRQ(EXTI9_5_IRQn); // kich hoat ham xu ly ngat

}

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
