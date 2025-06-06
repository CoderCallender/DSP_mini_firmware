/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "codec.h"
#include "IIR_PeakingFilter.h"
#include "fir_filter.h"
#include "iir_filter.h"
#include "distortion.h"
#include "delay_line.h"
#include "reverb.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_i2s2_ext_rx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */




uint16_t adcData[NUM_ADC_CHANNELS];	//buffer for the ADC data from the ADC used to read the potentiometers

static int16_t codecInData[AUDIO_BUFFER_SIZE];	//buffers to deal with the in/out of the codec
static int16_t codecOutData[AUDIO_BUFFER_SIZE];

volatile uint8_t audioDataReadyFlag = 0;
volatile uint8_t audio_update_lockout_flag = 0;


static volatile int16_t *codecInBuff_p;	//pointers to help handle the double buffering (read half and process the other half at the same time) of the CODEC data
static volatile int16_t *codecOutBuff_p;
//structure to hold all the ADC values from the pots
struct potentiometers
{
	uint16_t pot1;
	uint16_t pot2;
	uint16_t pot3;
	uint16_t pot4;
	uint16_t pot5;
	uint16_t pot6;
}pots;

IIR_peakingFilter bass_filter;
IIR_peakingFilter mid_filter;
IIR_peakingFilter high_filter;
fir_filter_t anti_aliasing_filter;
iir_filter_t treble_cut_filter;
iir_filter_t bass_cut_filter;
distortion_t overdrive;

delayline_t delay1;
delayline_t delay2;
delayline_t delay3;
delayline_t delay4;

reverb_delayline_t reverb_one;
reverb_delayline_t reverb_two;
reverb_delayline_t reverb_three;
reverb_delayline_t reverb_four;

reverb_delayline_t reverb_five;
reverb_delayline_t reverb_six;
reverb_delayline_t reverb_seven;
reverb_delayline_t reverb_eight;

#define RVERB_ONE_MS 	20
#define RVERB_TWO_MS 	35
#define RVERB_THREE_MS 	60
#define RVERB_FOUR_MS 	75
#define RVERB_FIVE_MS 	10
#define RVERB_SIX_MS 	2
#define RVERB_SEVEN_MS 	63
#define RVERB_EIGHT_MS 	55

float verb_array_one[3255];
float verb_array_two[3255];
float verb_array_three[3255];
float verb_array_four[3255];
float verb_array_five[3255];
float verb_array_six[3255];
float verb_array_seven[3255];
float verb_array_eight[3255];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void update_filter_settings(uint16_t q_pot, uint16_t boostCut_pot, IIR_peakingFilter *filt, float centre_freq);
float clamp(float in, float min, float max);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//this function is called every time an ADC conversion is complete
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

	//unload the array into our structure for easier handling
	pots.pot1 = adcData[0];
	pots.pot2 = adcData[1];
	pots.pot3 = adcData[4];
	pots.pot4 = adcData[5];
	pots.pot5 = adcData[2];
	pots.pot6 = adcData[3];

	//
	if(!audio_update_lockout_flag)
	{
	//	iir_highpass_set_params(&bass_cut_filter, (((float)pots.pot1 / 6.82f) + 50));	//50hz to 600Hz
	//	iir_lowpass_set_params(&treble_cut_filter, (((4096 - (float)pots.pot2) / 2.048f) + 3000));	//3k to 5k
	//	overdrive.gain = (((float)pots.pot3 / 200) + 0.1f);
	//	overdrive.asym_Q = (((float)pots.pot4 / 2048) + 0.1f) * -1.0f;
	//	overdrive.asym_d = (((float)pots.pot5 / 409.0) + 0.1);

	//	DelayLine_SetLength(&delay, ((float)pots.pot1 / 8) + 10.0f, SAMPLE_RATE_HZ);	//sets between 0 and 512ms

	//	delay.delay_time_pot_value = ((float)pots.pot1 / 8);
		delay1.mix = (float)pots.pot2 / 4096;
		delay1.feedback = ((float)pots.pot3 / 4500); //(never let it go full feedback)

	}
}

void update_filter_settings(uint16_t q_pot, uint16_t boostCut_pot, IIR_peakingFilter *filt, float centre_freq)
{
	//convert pot values to something in range for our filters
	//max ADC = 4096
	//205 (approx) for a 20 bit scaling
	float boostCut;

	//Q = q_pot / 205;
	boostCut = (float)boostCut_pot / 2048.0;

	if(!audio_update_lockout_flag)
	{
		IIR_peakingFilter_setParams(filt, centre_freq, (float)q_pot, boostCut);
	}

}

//function is called when I2S data is half complete, so align the pointers to the first half of the buffers
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	codecInBuff_p = &codecInData[0];
	codecOutBuff_p = &codecOutData[0];

	audioDataReadyFlag = 1;
}

//function is called when I2S data is complete, so align the pointers to the second half of the buffers
void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	codecInBuff_p = &codecInData[AUDIO_BUFFER_SIZE/2];
	codecOutBuff_p = &codecOutData[AUDIO_BUFFER_SIZE/2];

	audioDataReadyFlag = 1;
}

void processData(void)
{
	float leftIn, processed_data, current_delay_data[4];
	float audio_channels[4], shuffled_channels[4];
	static float leftOut;

	audio_update_lockout_flag = 1;

	for(uint8_t n = 0; n < (AUDIO_BUFFER_SIZE/2) - 1; n += 2)
	{

		// Check if number is negative (sign bit)
		if (codecInBuff_p[n] & 0x8000) {
			codecInBuff_p[n] |= ~0xFFFF;
		}

		//left channel data, normalise to float -1.0 to +1.0
		leftIn = (float)codecInBuff_p[n] / (float)0x7FFF;


		//modify the data here
		//high pass IIR
	//	outTemp_1 = iir_filter_update(&bass_cut_filter, leftIn);
		//anti alias (low pass)
//		processed_data = process_fir_filter(&anti_aliasing_filter, leftIn);
		//distortion
/*		if(HAL_GPIO_ReadPin(SWITCH_1_GPIO_Port, SWITCH_1_Pin))
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);	//Blue
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1); //RED
			outTemp_3 = distortion(&overdrive, outTemp_2);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);	//Blue
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);		//RED
			//outTemp_3 = asym_distortion(&overdrive, outTemp_2);
			outTemp_3 = DELAY_ALPHA * outTemp_2 + DELAY_BETA * DelayLine_Update(&delay, (outTemp_2 + (0.8 * static_temp)));

		}
*/
		//input into 4 reverb delays
		audio_channels[0] = reverb_delayline_update(&reverb_one, leftIn);
		audio_channels[1] = reverb_delayline_update(&reverb_two, leftIn);
		audio_channels[2] = reverb_delayline_update(&reverb_three, leftIn);
		audio_channels[3] = reverb_delayline_update(&reverb_four, leftIn);

		//output of reverb delays into shuffle
		reverb_shuffle_inverter(audio_channels, shuffled_channels);

		//output of shuffle into hadamard
		reverb_hadamard_matrix(shuffled_channels, audio_channels);

		//REPEAT
/*
		//input into 4 reverb delays
		audio_channels[0] = reverb_delayline_update(&reverb_five, audio_channels[0]);
		audio_channels[1] = reverb_delayline_update(&reverb_six, audio_channels[1]);
		audio_channels[2] = reverb_delayline_update(&reverb_seven, audio_channels[2]);
		audio_channels[3] = reverb_delayline_update(&reverb_eight, audio_channels[3]);

		//output of reverb delays into shuffle
		reverb_shuffle_inverter(audio_channels, shuffled_channels);

		//output of shuffle into hadamard
		reverb_hadamard_matrix(shuffled_channels, audio_channels);
*/
		//get the current delay sample
		current_delay_data[0] = *(delay1.memory_bank_one + delay1.index);
		current_delay_data[1] = *(delay2.memory_bank_one + delay2.index);
		current_delay_data[2] = *(delay3.memory_bank_one + delay3.index);
		current_delay_data[3] = *(delay4.memory_bank_one + delay4.index);

		//put it through a householder matrix to scramble it
		reverb_householder_matrix(audio_channels, shuffled_channels);

		//mix it with input (acts as our feedback line)
		//leftOut = processed_data + (delay.feedback * current_delay_data);
		audio_channels[0] = shuffled_channels[0] + (delay1.feedback * current_delay_data[0]);
		audio_channels[1] = shuffled_channels[1] + (delay1.feedback * current_delay_data[1]);
		audio_channels[2] = shuffled_channels[2] + (delay1.feedback * current_delay_data[2]);
		audio_channels[3] = shuffled_channels[3] + (delay1.feedback * current_delay_data[3]);

		//update the delay line and output data
	//	leftOut = processed_data + (delay.mix * DelayLine_Update(&delay, leftOut));
		audio_channels[0] = audio_channels[0] + (delay1.mix * DelayLine_Update(&delay1, audio_channels[0]));
		audio_channels[1] = audio_channels[1] + (delay1.mix * DelayLine_Update(&delay2, audio_channels[1]));
		audio_channels[2] = audio_channels[2] + (delay1.mix * DelayLine_Update(&delay3, audio_channels[2]));
		audio_channels[3] = audio_channels[3] + (delay1.mix * DelayLine_Update(&delay4, audio_channels[3]));

		//mix into 1 channel

		//mix with input
		processed_data = (audio_channels[0] + audio_channels[1] + audio_channels[2] + audio_channels[3]) / 4;


		//add in the current sample to the output
		//leftOut = (leftOut + leftIn) / 2;

		leftOut = processed_data;//debug
		//leftOut = processed_data + delay.mix * DelayLine_Update(&delay, (processed_data + (delay.feedback * leftOut)));
		//low pass IIR
	//	leftOut = iir_filter_update(&treble_cut_filter, outTemp_3);
	//	static_temp = leftOut; //save
		//volume
		leftOut = leftOut * ((float)pots.pot6 / 4096);

		// Ensure output samples are within [-1.0,+1.0] range
		if (leftOut < -1.0f) {
			leftOut = -1.0f;
		} else if (leftOut > 1.0f) {
			leftOut =  1.0f;
		}

		//convert back to signed int and transfer to DAC (via pointer)
		codecOutBuff_p[n] =  (int16_t)(leftOut * 0x7FFF);

	}

	audio_update_lockout_flag = 0;
	audioDataReadyFlag = 0;
	return;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

 // codec_hardware_reset_pin_set();	//hold CODEC in reset
 // HAL_Delay(1500);	//wait for horrible noises of power up to go away
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adcData, NUM_ADC_CHANNELS); 	//start the ADC and link it with the DMA
  HAL_TIM_Base_Start(&htim2); 											//start timer 2 which triggers the ADC
  HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t *) codecOutData, (uint16_t *) codecInData, AUDIO_BUFFER_SIZE);
  codec_hardware_reset_pin_clear();
  HAL_Delay(50);
  codec_configure(&hi2c1);

  init_fir_filter(&anti_aliasing_filter);
  reverb_delayLine_init(&reverb_one, RVERB_ONE_MS, SAMPLE_RATE_HZ, verb_array_one);
  reverb_delayLine_init(&reverb_two, RVERB_TWO_MS, SAMPLE_RATE_HZ, verb_array_two);
  reverb_delayLine_init(&reverb_three, RVERB_THREE_MS, SAMPLE_RATE_HZ, verb_array_three);
  reverb_delayLine_init(&reverb_four, RVERB_FOUR_MS, SAMPLE_RATE_HZ, verb_array_four);

  reverb_delayLine_init(&reverb_five, RVERB_FIVE_MS, SAMPLE_RATE_HZ, verb_array_five);
  reverb_delayLine_init(&reverb_six, RVERB_SIX_MS, SAMPLE_RATE_HZ, verb_array_six);
  reverb_delayLine_init(&reverb_seven, RVERB_SEVEN_MS, SAMPLE_RATE_HZ, verb_array_seven);
  reverb_delayLine_init(&reverb_eight, RVERB_EIGHT_MS, SAMPLE_RATE_HZ, verb_array_eight);

  DelayLine_Init(&delay1, DELAY_TIME_MS, SAMPLE_RATE_HZ);
  DelayLine_Init(&delay2, DELAY_TIME_MS, SAMPLE_RATE_HZ);
  DelayLine_Init(&delay3, DELAY_TIME_MS, SAMPLE_RATE_HZ);
  DelayLine_Init(&delay4, DELAY_TIME_MS, SAMPLE_RATE_HZ);

/*  for(uint8_t x = 0; x <= BIG_NUMBER; x++)
  {
	  memory_in_ccm_ram[x] = 0;
  }
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(!HAL_GPIO_ReadPin(SWITCH_0_GPIO_Port, SWITCH_0_Pin))
	  {
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);	//Blue
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);		//RED

		  if(audioDataReadyFlag)
		  {
			  processData();
		  }
	  }

	  else
	  {
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);	//Blue
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);	//Red
		  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 6;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 10000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_32K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 31;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 49999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CODEC_RST_GPIO_Port, CODEC_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_BLUE_Pin|LED_RED_Pin|LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SWITCH_0_Pin SWITCH_1_Pin */
  GPIO_InitStruct.Pin = SWITCH_0_Pin|SWITCH_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : CODEC_RST_Pin */
  GPIO_InitStruct.Pin = CODEC_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CODEC_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_BLUE_Pin LED_RED_Pin LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_BLUE_Pin|LED_RED_Pin|LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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
