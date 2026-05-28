/* ============================================================
 * SmartGarden v0.0 — Gateway Firmware
 * Hardware  : STM32F103C8T6 "Blue Pill" + ESP8266 + SX1278
 * MCU       : STM32F1xx HAL (STM32CubeIDE 1.16.1)
 * Datum     : 2024/2025
 * Autor     : Oliver Schmoll
 * Status    : Archiviert — abgelöst durch TTGO + TTN (v0.1)
 *
 * Funktion:
 *   1. ESP8266 per AT-Kommandos über USART3 (PB10/PB11) initialisieren
 *   2. WiFi-Verbindung herstellen (Station Mode)
 *   3. TCP-Webserver auf Port 80 starten (lokaler Zugriff)
 *   4. LED blinkt zur Statusanzeige
 *
 * Anschlüsse:
 *   USART3 TX (PB10) → ESP8266 RXD
 *   USART3 RX (PB11) → ESP8266 TXD
 *   ESP8266 CH_PD    → 3.3V (muss HIGH)
 *   ESP8266 VCC      → 3.3V
 *   ESP8266 GND      → GND
 *
 * HINWEIS: WiFi-Credentials wurden entfernt.
 * In der Originalversion standen SSID und Passwort im Klartext.
 * → Lessons Learned: Credentials niemals im Code — immer in
 *   separater Konfigurationsdatei (.gitignore) oder Flash-EEPROM.
 * ============================================================ */

#include "main.h"
#include <string.h>
#include <stdio.h>

/* ── Konfiguration ────────────────────────────────────────── */
/* WiFi-Zugangsdaten — NICHT im Code, in credentials.h ablegen */
#define WIFI_SSID     "YOUR_SSID_HERE"
#define WIFI_PASSWD   "YOUR_PASSWORD_HERE"

/* ── Peripherie ───────────────────────────────────────────── */
UART_HandleTypeDef huart3;   /* USART3: PB10 (TX) / PB11 (RX) → ESP8266 */

/* ── Puffer ───────────────────────────────────────────────── */
char     buffer[60]  = "";
uint8_t  num         = 0;
uint8_t  UART1_rxBuffer[12] = {0};

/* Einmalige WiFi-Initialisierung beim ersten Durchlauf */
static char Init_Wifi = 0x01;

/* ── Deklarationen ────────────────────────────────────────── */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);

/* ============================================================
 * main()
 * ============================================================ */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART3_UART_Init();

    while (1)
    {
        /* ── Einmalige ESP8266-Initialisierung ──────────────── */
        if (Init_Wifi)
        {
            /* AT — Verbindungstest */
            sprintf(buffer, "AT\r\n");
            HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 1000);
            HAL_Delay(1000);

            /* CWMODE=1 — Station Mode (Client, kein AP) */
            sprintf(buffer, "AT+CWMODE=1\r\n");
            HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 1000);
            HAL_Delay(1000);

            /* CWJAP — Mit Home-WiFi verbinden */
            sprintf(buffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWD);
            HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 1000);
            HAL_Delay(5000);   /* Verbindungsaufbau kann mehrere Sekunden dauern */

            /* CIFSR — IP-Adresse abfragen (Debug) */
            sprintf(buffer, "AT+CIFSR\r\n");
            HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 1000);
            HAL_Delay(1000);

            /* CIPMUX=1 — Mehrfachverbindungen erlauben */
            sprintf(buffer, "AT+CIPMUX=1\r\n");
            HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 1000);
            HAL_Delay(1000);

            /* CIPSERVER=1,80 — TCP-Server auf Port 80 starten */
            sprintf(buffer, "AT+CIPSERVER=1,80\r\n");
            HAL_UART_Transmit(&huart3, (uint8_t*)buffer, strlen(buffer), 1000);
            HAL_Delay(1000);

            Init_Wifi = 0x00;   /* Initialisierung einmalig */
        }

        /* ── LED-Heartbeat (Statusanzeige) ─────────────────── */
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  /* LED an */
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);    /* LED aus */
        HAL_Delay(500);

        /* ── TODO (nicht implementiert in v0.0) ─────────────── */
        /* - SX1278 SPI initialisieren                           */
        /* - LoRa-Pakete von Slave-Nodes empfangen               */
        /* - Payload parsen (Temperatur, Feuchte, Luftdruck)     */
        /* - Daten per HTTP/MQTT an Firebase senden              */
    }
}

/* ============================================================
 * System-Konfiguration (automatisch generiert von CubeMX)
 * ============================================================ */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue      = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL         = RCC_PLL_MUL9;  /* 8 MHz × 9 = 72 MHz */
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   /* APB1 max 36 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

static void MX_USART3_UART_Init(void)
{
    /* PB10 = TX, PB11 = RX → ESP8266 RXD/TXD */
    huart3.Instance          = USART3;
    huart3.Init.BaudRate     = 115200;
    huart3.Init.WordLength   = UART_WORDLENGTH_8B;
    huart3.Init.StopBits     = UART_STOPBITS_1;
    huart3.Init.Parity       = UART_PARITY_NONE;
    huart3.Init.Mode         = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart3);
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) { }
}
