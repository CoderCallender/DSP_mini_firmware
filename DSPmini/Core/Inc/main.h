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
#include "stm32f4xx_hal.h"

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
#define SWITCH_0_Pin GPIO_PIN_4
#define SWITCH_0_GPIO_Port GPIOC
#define SWITCH_1_Pin GPIO_PIN_5
#define SWITCH_1_GPIO_Port GPIOC
#define CODEC_RST_Pin GPIO_PIN_11
#define CODEC_RST_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_10
#define LED_BLUE_GPIO_Port GPIOC
#define LED_RED_Pin GPIO_PIN_11
#define LED_RED_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define NUM_ADC_CHANNELS 	6
#define AUDIO_BUFFER_SIZE	128

#define SAMPLE_RATE_HZ		32552.0f	//

#define UINT16_TO_FLOAT 0.00001525878f
#define INT16_TO_FLOAT 0.00003051757f
#define FLOAT_TO_INT16 32768.0f

#define BASS_EQ_FREQ	150.0f
#define MID_EQ_FREQ		1000.0f
#define HIGH_EQ_FREQ	6000.0f

#define DELAY_TIME_MS	500.0f
#define DELAY_ALPHA		0.6f
#define DELAY_BETA		0.4f
#define DELAY_FEEDBACK	0.8f
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
