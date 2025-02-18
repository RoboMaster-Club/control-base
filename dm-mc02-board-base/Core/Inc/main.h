/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define POWER_24V_2_Pin GPIO_PIN_13
#define POWER_24V_2_GPIO_Port GPIOC
#define POWER_24V_1_Pin GPIO_PIN_14
#define POWER_24V_1_GPIO_Port GPIOC
#define POWER_5V_Pin GPIO_PIN_15
#define POWER_5V_GPIO_Port GPIOC
#define CS1_ACCEL_Pin GPIO_PIN_0
#define CS1_ACCEL_GPIO_Port GPIOC
#define CS1_GYRO_Pin GPIO_PIN_3
#define CS1_GYRO_GPIO_Port GPIOC
#define DCMI_PWDN_Pin GPIO_PIN_5
#define DCMI_PWDN_GPIO_Port GPIOC
#define INT1_ACCEL_Pin GPIO_PIN_10
#define INT1_ACCEL_GPIO_Port GPIOE
#define INT1_GYRO_Pin GPIO_PIN_12
#define INT1_GYRO_GPIO_Port GPIOE
#define LCD_CS_Pin GPIO_PIN_15
#define LCD_CS_GPIO_Port GPIOE
#define LCD_BLK_Pin GPIO_PIN_10
#define LCD_BLK_GPIO_Port GPIOB
#define LCD_RES_Pin GPIO_PIN_11
#define LCD_RES_GPIO_Port GPIOB
#define DCMI_REST_Pin GPIO_PIN_12
#define DCMI_REST_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_10
#define LCD_DC_GPIO_Port GPIOD
#define Remote_Pin GPIO_PIN_2
#define Remote_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
