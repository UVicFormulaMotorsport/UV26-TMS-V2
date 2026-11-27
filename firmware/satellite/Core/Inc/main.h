/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MUX02_DAQ_Pin GPIO_PIN_1
#define MUX02_DAQ_GPIO_Port GPIOA
#define MUX01_DAQ_Pin GPIO_PIN_2
#define MUX01_DAQ_GPIO_Port GPIOA
#define MUX03_DAQ_Pin GPIO_PIN_3
#define MUX03_DAQ_GPIO_Port GPIOA
#define CS_LTC6820_Pin GPIO_PIN_4
#define CS_LTC6820_GPIO_Port GPIOA
#define MUXE_Pin GPIO_PIN_0
#define MUXE_GPIO_Port GPIOB
#define ADDR_Pin GPIO_PIN_9
#define ADDR_GPIO_Port GPIOA
#define SW_Data_In_Out_Pin GPIO_PIN_13
#define SW_Data_In_Out_GPIO_Port GPIOA
#define Serial_Wire_CLOCK_Pin GPIO_PIN_14
#define Serial_Wire_CLOCK_GPIO_Port GPIOA
#define Serial_Wire_OUT_Pin GPIO_PIN_3
#define Serial_Wire_OUT_GPIO_Port GPIOB
#define MUX1_Addr_Supply_Pin GPIO_PIN_5
#define MUX1_Addr_Supply_GPIO_Port GPIOB
#define MUX2_Addr_Supply_Pin GPIO_PIN_6
#define MUX2_Addr_Supply_GPIO_Port GPIOB
#define MUX3_Addr_Supply_Pin GPIO_PIN_7
#define MUX3_Addr_Supply_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
