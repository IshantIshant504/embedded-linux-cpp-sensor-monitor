/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

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

COM_InitTypeDef BspCOMInit;
FDCAN_HandleTypeDef hfdcan1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_FDCAN1_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void STM_putchar(uint8_t c)
{
    HAL_UART_Transmit(&huart2, &c, 1, 1000);
}

void UART_Print(char *message)
{
    while (*message != '\0')
    {
        STM_putchar((uint8_t)*message);
        message++;
    }
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_FDCAN1_Init();
  /* USER CODE BEGIN 2 */
  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
      Error_Handler();
  }
  /* USER CODE BEGIN 2 */

  uint8_t data_reg = 0xF7;
  uint8_t raw_data[8] = {0};
  HAL_StatusTypeDef raw_read_status;
  int counter = 0;

  /* Existing chip ID test — KEEP THIS */




  uint8_t calb[26] = {0};
  uint8_t reg_calb = 0x88;
  HAL_StatusTypeDef calb_read_status =
		  HAL_I2C_Mem_Read(
		  &hi2c1,
		  0x77 << 1,
		  reg_calb,
		  I2C_MEMADD_SIZE_8BIT,
		  calb,
		  26,
		  100
		  );
  uint8_t hum_calb[7] = {0};
  uint8_t hum_calb_reg = 0xE1;

  HAL_StatusTypeDef hum_calb_read_status =
      HAL_I2C_Mem_Read(
          &hi2c1,
          0x77 << 1,
          hum_calb_reg,
          I2C_MEMADD_SIZE_8BIT,
          hum_calb,
          7,
          100
      );


  /* Temperature coefficients */
  uint16_t dig_T1 = (uint16_t)((calb[1] << 8) | calb[0]);
  int16_t dig_T2 = (int16_t)((calb[3] << 8) | calb[2]);
  int16_t dig_T3 = (int16_t)((calb[5] << 8) | calb[4]);

  /* Pressure coefficients */
  uint16_t dig_P1 = (uint16_t)((calb[7] << 8) | calb[6]);
  int16_t dig_P2 = (int16_t)((calb[9] << 8) | calb[8]);
  int16_t dig_P3 = (int16_t)((calb[11] << 8) | calb[10]);
  int16_t dig_P4 = (int16_t)((calb[13] << 8) | calb[12]);
  int16_t dig_P5 = (int16_t)((calb[15] << 8) | calb[14]);
  int16_t dig_P6 = (int16_t)((calb[17] << 8) | calb[16]);
  int16_t dig_P7 = (int16_t)((calb[19] << 8) | calb[18]);
  int16_t dig_P8 = (int16_t)((calb[21] << 8) | calb[20]);
  int16_t dig_P9 = (int16_t)((calb[23] << 8) | calb[22]);

  /* Humidity coefficients */
  uint8_t dig_H1 = calb[25];

  uint16_t h2_u = (uint16_t)(((uint16_t)hum_calb[1] << 8)
                             | hum_calb[0]);
  int16_t dig_H2 = (int16_t)h2_u;

  uint8_t dig_H3 = hum_calb[2];

  uint16_t h4_u = (uint16_t)(((uint16_t)(uint8_t)hum_calb[3] << 4)
                             | (hum_calb[4] & 0x0F));
  int16_t dig_H4 = (h4_u & 0x0800)
                    ? (int16_t)(h4_u | 0xF000)
                    : (int16_t)h4_u;

  uint16_t h5_u = (uint16_t)(((uint16_t)(uint8_t)hum_calb[5] << 4)
                             | (hum_calb[4] >> 4));
  int16_t dig_H5 = (h5_u & 0x0800)
                    ? (int16_t)(h5_u | 0xF000)
                    : (int16_t)h5_u;

  int8_t dig_H6 = (int8_t)hum_calb[6];

  /* Combine the 3 temperature bytes into a 20-bit raw value */


  /* Combine the 3 pressure bytes into a 20-bit raw value */
  char msg5[] = "UART MESSAGE BEFORE LOOP\r\n";

                HAL_UART_Transmit(&huart2,
                                  (uint8_t *)msg5,
                                  sizeof(msg5) - 1,
                                  1000);




  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  /*if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	   raw_read_status =
	  	       HAL_I2C_Mem_Read(
	  	           &hi2c1,
	  	           0x77 << 1,
	  	           data_reg,
	  	           I2C_MEMADD_SIZE_8BIT,
	  	           raw_data,
	  	           8,
	  	           100
	  	       );
	  	  if (raw_read_status != HAL_OK)
	  	  {
	  	      HAL_Delay(1000);
	  	      continue;
	  	  }
	  	   uint32_t raw_temp =
	  	         ((uint32_t)raw_data[3] << 12) |
	  	         ((uint32_t)raw_data[4] << 4)  |
	  	         ((uint32_t)raw_data[5] >> 4);


	  	     int32_t var1, var2, t_fine;
	  	     int32_t temperature_centi;
	  	     var1 = ((((int32_t)(raw_temp >> 3) -
	  	               ((int32_t)dig_T1 << 1))) *
	  	             (int32_t)dig_T2) >> 11;
	  	     var2 = (((((int32_t)(raw_temp >> 4) -
	  	                (int32_t)dig_T1) *
	  	               ((int32_t)(raw_temp >> 4) -
	  	                (int32_t)dig_T1)) >> 12) *
	  	             (int32_t)dig_T3) >> 14;
	  	     t_fine = var1 + var2;
	  	     temperature_centi = (t_fine * 5 + 128) >> 8;
	  	     float temperature_C = (float)temperature_centi / 100.0f;

	  	     uint32_t raw_pressure =
	  	         ((uint32_t)raw_data[0] << 12) |
	  	         ((uint32_t)raw_data[1] << 4)  |
	  	         ((uint32_t)raw_data[2] >> 4);

	  	     int64_t var1_p, var2_p, pressure_comp;
	  	     var1_p = (int64_t)t_fine - 128000;
	  	     var2_p = var1_p * var1_p * (int64_t)dig_P6;
	  	     var2_p = var2_p + ((var1_p * (int64_t)dig_P5) << 17);
	  	     var2_p = var2_p + (((int64_t)dig_P4) << 35);
	  	     var1_p = ((var1_p * var1_p * (int64_t)dig_P3) >> 8)
	  	            + ((var1_p * (int64_t)dig_P2) << 12);
	  	     var1_p = (((((int64_t)1) << 47) + var1_p)
	  	              * (int64_t)dig_P1) >> 33;
	  	     float pressure_hPa = 0.0f;
	  	     if (var1_p != 0)
	  	     {
	  	         pressure_comp = 1048576 - (int64_t)raw_pressure;
	  	         pressure_comp = (((pressure_comp << 31) - var2_p)
	  	                         * 3125) / var1_p;
	  	         var1_p = ((int64_t)dig_P9
	  	                  * (pressure_comp >> 13)
	  	                  * (pressure_comp >> 13)) >> 25;
	  	         var2_p = ((int64_t)dig_P8
	  	                  * pressure_comp) >> 19;
	  	         pressure_comp = ((pressure_comp + var1_p + var2_p) >> 8)
	  	                       + (((int64_t)dig_P7) << 4);
	  	         pressure_hPa = (float)pressure_comp / 25600.0f;
	  	     }

	  	     uint16_t raw_humidity =
	  	         ((uint16_t)raw_data[6] << 8) |
	  	         (uint16_t)raw_data[7];

	  	     int32_t h = t_fine - 76800;
	  	     h = (((((int32_t)raw_humidity << 14)
	  	            - ((int32_t)dig_H4 << 20)
	  	            - ((int32_t)dig_H5 * h)) + 16384) >> 15)
	  	         * (((((((h * (int32_t)dig_H6) >> 10)
	  	                * (((h * (int32_t)dig_H3) >> 11) + 32768)) >> 10)
	  	              + 2097152) * (int32_t)dig_H2 + 8192) >> 14);
	  	     h = h - (((((h >> 15) * (h >> 15)) >> 7)
	  	               * (int32_t)dig_H1) >> 4);
	  	     if (h < 0)
	  	     {
	  	         h = 0;
	  	     }
	  	     else if (h > 419430400)
	  	     {
	  	         h = 419430400;
	  	     }
	  	     float humidity_RH = (float)(h >> 12) / 1024.0f;

	  	   char message[120];

	  	   snprintf(message, sizeof(message),
	  	            "Temperature: %.2f C | Pressure: %.2f hPa | Humidity: %.2f %%RH\r\n",
	  	            temperature_C,
	  	            pressure_hPa,
	  	            humidity_RH);
	  	 if (raw_read_status != HAL_OK)
	  	 {
	  	     HAL_Delay(1000);
	  	     continue;
	  	 }
	  	   UART_Print(message);

	  	   HAL_Delay(1000);
	  	 counter++;
	  	 if (counter >= 5)
	  	 {
	  	     UART_Print("\033[2J\033[H");
	  	     counter = 0;
	  	 }
	  	HAL_Delay(1000);



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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 17;
  hfdcan1.Init.NominalSyncJumpWidth = 4;
  hfdcan1.Init.NominalTimeSeg1 = 15;
  hfdcan1.Init.NominalTimeSeg2 = 4;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

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
  hi2c1.Init.Timing = 0x40B285C2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
#ifdef USE_FULL_ASSERT
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
