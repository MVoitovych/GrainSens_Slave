/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "commands.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define packetSize (8)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t UART_RX_buffer[packetSize];
uint16_t UART_TX_buffer[packetSize];
uint8_t UART_is_received = 0;
uint8_t UART_is_transmitted = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void changeUARTAddress(uint8_t address);
void getCrc(uint8_t Crc_8[4]);
void addCRC();
uint8_t checkCRC(uint8_t Crc_8[4]);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &huart1) {
    UART_is_received = 1;
    uint8_t Crc_8[4];
    getCrc(Crc_8);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
    // Check CRC
    if (checkCRC(Crc_8)) {
      // Correct CRC

      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
      if (UART_RX_buffer[1] == 'X') {
        UART_RX_buffer[1] = 0;
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
        changeUARTAddress(UART_RX_buffer[2]);
      } else {
        // other commands
      }
    } else {
      // Incorrect CRC
      
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
      UART_TX_buffer[0] = ERROR_COMMAND;
      addCRC();
      //      UART_is_transmitted = 0;
      HAL_UART_Transmit_IT(&huart1, (uint8_t *)UART_TX_buffer, packetSize);
    }

    UART_Start_Receive_IT(&huart1, (uint8_t *)UART_RX_buffer, packetSize);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  UART_is_transmitted = 1;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_USART1_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  HAL_MultiProcessor_EnableMuteMode(&huart1);
  HAL_MultiProcessor_EnterMuteMode(&huart1);

  HAL_UART_Receive_IT(&huart1, (uint8_t *)UART_RX_buffer, packetSize);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void changeUARTAddress(uint8_t address) {
  if (HAL_MultiProcessor_Init(&huart1, address,
                              UART_WAKEUPMETHOD_ADDRESSMARK) != HAL_OK) {
    Error_Handler();
  }
}

void getCrc(uint8_t Crc_8[4]) {
  uint32_t Crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)UART_RX_buffer,
                                   (packetSize / 2 - 2));
  Crc_8[0] = (Crc & 0xFF000000) >> 24;
  Crc_8[1] = (Crc & 0x00FF0000) >> 16;
  Crc_8[2] = (Crc & 0x0000FF00) >> 8;
  Crc_8[3] = (Crc & 0x000000FF);
}

void addCRC() {
  uint8_t Crc[4];
  getCrc(Crc);
  UART_TX_buffer[packetSize - 4] = Crc[0];
  UART_TX_buffer[packetSize - 3] = Crc[1];
  UART_TX_buffer[packetSize - 2] = Crc[2];
  UART_TX_buffer[packetSize - 1] = Crc[3];
}

uint8_t checkCRC(uint8_t Crc_8[4]) {
  return (Crc_8[0] == UART_RX_buffer[4] && Crc_8[1] == UART_RX_buffer[5] &&
          Crc_8[2] == UART_RX_buffer[6] && Crc_8[3] == UART_RX_buffer[7]);
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
