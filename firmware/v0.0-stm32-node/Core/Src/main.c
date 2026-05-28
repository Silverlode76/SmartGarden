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
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "ESP8266_HAL.h"
#include "BME280_STM32.h"
#include "LoRa.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Frequency 868
#define Spreading_Factor SF_7
#define Bandwidth BW_125KHz
#define CRCRate CR_4_5
#define Power POWER_20db
#define Preamble 8
#define OverCurrentProtection 240
#define pc_uart &huart3
#define wifi_uart &huart1
#define MASTER 0
#define SLAVE 1
#define TIMEOUT_MASTER 15000
#define TIMEOUT_SLAVE 1000

#if SLAVE
const char UUID[]= "AAAA02";
#else
char UUID_SLAVE[]="FFFFFF";
char sTemp[]="22.37";
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
 LoRa_Device Dev01;
 extern I2C_HandleTypeDef hi2c1;
 //LoRa myLoRa;
#define BUFFER_SIZE 30
 uint8_t received_data[BUFFER_SIZE];
 uint8_t packet_size = 0;
 uint8_t send_buffer[BUFFER_SIZE];
 uint8_t bmebuffer[BUFFER_SIZE];
 char db_command[138];
 int counter;
 uint8_t packet_size;
 int Led_cmd;
 int Led_stat;

 

 typedef enum {
     OFF,    // Wert 0
     ON    // Wert 1
 } LED_Status;
 LED_Status Green_Led;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float Temperature, Pressure, Humidity;
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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();

  //Wake Up Check
  //checkWakeupReason();
  /* USER CODE BEGIN 2 */
  //ESP_Init("WLAN-706244","53327469682487785325");
#if SLAVE
  BME280_Config(OSRS_16, OSRS_16, OSRS_1, MODE_NORMAL, T_SB_0p5, IIR_16);
#endif
  
  Ringbuf_init();

  Uart_flush(pc_uart);
  Uart_sendstring("Init ESP8266 AP...\r\n",pc_uart);
  Uart_flush(wifi_uart);

#if MASTER
   //while(!(Wait_for("DB_CONNECTED\r\n", wifi_uart)));

   Uart_sendstring("ESP8266_AP connected\r\n",pc_uart);

#endif

  Uart_sendstring("Init_LoRa...",pc_uart);
  //myLoRa = newLoRa();
  Init_Lora();


#if MASTER
  Dev01.state = REQUEST_SLAVE_DATA;
  counter = 0;
#else
  Dev01.state = REC;
  counter = 0;
#endif

  LoRa_startReceiving(&Dev01.myLoRa);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#if MASTER
	  if(Dev01.state == REQUEST_SLAVE_DATA)
	  {

		  sprintf(send_buffer, "REQ\r\n", counter++);
		  LoRa_transmit(&Dev01.myLoRa,send_buffer ,BUFFER_SIZE, 50);
		  Uart_sendstring("\r\n",pc_uart );
		  Uart_sendstring("tx:",pc_uart );
		  Uart_sendstring(send_buffer,pc_uart );

	  }
	  else if(Dev01.state == WRITE_SLAVE_DATA)
	  {
		  sprintf(send_buffer, "CMD;LED=%i\r\n", (counter%2));
		  LoRa_transmit(&Dev01.myLoRa,send_buffer ,BUFFER_SIZE, 50);
		  Uart_sendstring("\r\n",pc_uart );
		  Uart_sendstring("tx:",pc_uart );
		  Uart_sendstring(send_buffer,pc_uart );
		  counter++;

	  }
	  else if(Dev01.state == SEND_DB)
	  {
		  sprintf(db_command, "WRITE;SLAVE=0;UUID=%s;COUNT=%i;LED_STAT=%i;LED_CMD=%s\r\n",UUID_SLAVE, counter, Led_stat, sTemp);
		  Uart_sendstring("\r\n",pc_uart);
		  Uart_sendstring(db_command,pc_uart);

		  Uart_flush(wifi_uart);
		  Uart_sendstring(db_command,wifi_uart);
		  Uart_flush(wifi_uart);
		  //while(!(Wait_for("DB_CONNECTED\r\n", wifi_uart)));

		 Dev01.state = WRITE_SLAVE_DATA;


	  }
#else

	  if(Dev01.state == SEND_UUID)
	  {

		  sprintf(send_buffer, "UUID=%s\r\n", UUID);
		  LoRa_transmit(&Dev01.myLoRa,send_buffer ,BUFFER_SIZE, 50);
		  Uart_sendstring("\r\n",pc_uart );
		  Uart_sendstring(send_buffer,pc_uart );

		  Dev01.state = REC;
	  }
	  else if (Dev01.state == SEND_LED_STAT)
	  {
		  sprintf(send_buffer, "UUID=%s;LED=%i,Temp=%d.%02d\r\n", UUID, Led_stat,(int)Temperature, (int)((Temperature - (int)Temperature) * 100));
		  LoRa_transmit(&Dev01.myLoRa,send_buffer ,BUFFER_SIZE, 50);
		  Uart_sendstring("\r\n",pc_uart );
		  Uart_sendstring(send_buffer,pc_uart );

		  Dev01.state = REC;
		  // gehe in sleep mode
		  HAL_DBGMCU_EnableDBGStopMode();  // Erlaubt Debugging im STOP-Modus
		  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
		  SystemClock_Config();  // Wiederherstellen des Takts nach STOP-Mode
	  }
#endif

#if MASTER
		HAL_Delay(TIMEOUT_MASTER);
#else
		BME280_Measure();
		/*sprintf(bmebuffer, "Temp=%d.%02d,Pres=%d.%02d,Hum=%d.%02d\r\n",
		        (int)Temperature, (int)((Temperature - (int)Temperature) * 100),
		        (int)Pressure, (int)((Pressure - (int)Pressure) * 100),
		        (int)Humidity, (int)((Humidity - (int)Humidity) * 100));
		Uart_sendstring(bmebuffer,pc_uart );
		*/
		HAL_Delay(TIMEOUT_SLAVE);
#endif
  }//while

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == DIO0_Pin) // Beispiel für PA1
    {
		// Prüfen, ob ein RxDone-Ereignis vorliegt
        uint8_t irqFlags = LoRa_read(&Dev01.myLoRa, 0x12); // IRQ_FLAGS lesen

        if (irqFlags & 0x08) { // TxDone-Flag
        	Uart_sendstring("Senden abgeschlossen\n",pc_uart );

        }
        else if (irqFlags & 0x40) // RX_DONE (Bit 6)
        {
        	packet_size = LoRa_receive(&Dev01.myLoRa, received_data, BUFFER_SIZE);
        	char command[packet_size];


            // Daten über UART ausgeben oder anderweitig verarbeiten
        	Uart_sendstring("rx:",pc_uart );
        	Uart_sendstring(received_data,pc_uart);

#if MASTER
			if( (sscanf((char *)received_data, "UUID=%6s\r\n", &UUID_SLAVE) == 1) && (Dev01.state == REQUEST_SLAVE_DATA) )
			{

				// Prozess für PING-Nachricht
				sprintf(command, "SLAVE_UUID=%s\r\n", UUID_SLAVE);
				Uart_sendstring(command,pc_uart);
				//sprintf(db_command, "WRITE;SLAVE=0;UUID=AAAA02;COUNT=%i;LED_STAT=0;LED_CMD=0\r\n",pong_counter);

				Dev01.state = SEND_DB;


			}
			else if (sscanf((char *)received_data, "UUID=%6s;LED=%i,Temp=%s\r\n", &UUID_SLAVE, &Led_stat, &sTemp) == 3)
			{
				sprintf(command, "UUID=%6s;LED=%i;Temp=%s\r\n", UUID_SLAVE, Led_stat, sTemp);
				Uart_sendstring(command,pc_uart);

				Dev01.state = SEND_DB;
			}
#else

			if (strcmp((char *)received_data, "REQ\r\n") == 0) {
			    // Der empfangene String ist genau "REQ"
				Dev01.state = SEND_UUID;
			}
			else if(sscanf((char *)received_data, "CMD;LED=%i\r\n", &Led_cmd) == 1)
			{
				if(Led_cmd == 0)
				{
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
					Led_stat=0;
				}
				else
				{
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
					Led_stat=1;
				}

				Dev01.state = SEND_LED_STAT;
			}


#endif
			// IRQ_FLAGS zurücksetzen
			LoRa_write(&Dev01.myLoRa, 0x12, 0xFF);

        }
    }
}

void checkWakeupReason() {
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // Standby-Flag löschen
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);  // Wakeup-Flag löschen

        // GPIO und Interrupts NEU initialisieren
        MX_GPIO_Init();
        Init_Lora();
    }
}

void Init_Lora(){
	 Dev01 = newLoRaDevice(Frequency, Spreading_Factor,Bandwidth, CRCRate, Preamble, Power, OverCurrentProtection);
	  Dev01.myLoRa.CS_port		= NSS_GPIO_Port;
	  Dev01.myLoRa.CS_pin      	= NSS_Pin;
	  Dev01.myLoRa.reset_port 	= RST_GPIO_Port;
	  Dev01.myLoRa.reset_pin    = RST_Pin;
	  Dev01.myLoRa.DIO0_port    = DIO0_GPIO_Port;
	  Dev01.myLoRa.DIO0_pin     = DIO0_Pin;
	  Dev01.myLoRa.hSPIx        = &hspi1;

	  if(LORA_OK == LoRa_init(&Dev01.myLoRa)){

		  Uart_sendstring("success\n\r",pc_uart );
	  }
}

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
