/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "config.h"

#include "driver/adc.h"

#include <Wire.h>
#include <Adafruit_SSD1306.h>

/* Defines -------------------------------------------------------------------*/
#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64
#define DISPLAY_RESET   -1

/* Static Variables ----------------------------------------------------------*/
static uint16_t rawValue = 0;
static uint16_t voltValue = 0;

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

/* Class ---------------------------------------------------------------------*/
Adafruit_SSD1306 Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);

/* Static Functions ----------------------------------------------------------*/
uint16_t adc_convert_bits_to_mvolts( uint16_t raw_value )
{
  return (raw_value * 3300)/(4096 - 1); // 3300 -> 3.3V Voltaje de referencia | 4096 -> Resolucion del ADC
}

/* Setup Function -----------------------------------------------------------*/
void setup() 
{
  // Inicializar modulo Serial
  Serial.begin( SERIAL_BAUDRATE );
  vTaskDelay( pdMS_TO_TICKS(500) );
 
  // Configurar el watchdog para la tarea actual (loopTask)
  esp_task_wdt_init( &hwdt ); 
  esp_task_wdt_add( NULL );   

  // Configuramos el Canal 0 del ADC, el ancho de banda sera de 12 bits
  adc1_config_width( ADC_WIDTH_BIT_12 );
  adc1_config_channel_atten( ADC1_CHANNEL_0, ADC_ATTEN_DB_11 );

  // Configuramos el OLED
  Display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  Display.clearDisplay();
  Display.setTextColor(WHITE);
  Display.display();

}

/* Loop Function ------------------------------------------------------------*/
void loop() 
{
  // Obtener el valor del ADC1 CH0
  rawValue = adc1_get_raw( ADC1_CHANNEL_0 );
  
  // Convertir el valor del ADC a mV
  voltValue = adc_convert_bits_to_mvolts( rawValue );

  Display.clearDisplay();
  Display.setCursor(0,0);
  Display.println("Parameters:");
  Display.printf("rawValue %u\n", rawValue);
  Display.printf("Volt Value %u\n", voltValue);
  Display.display();

  Serial.println("######################################");
  Serial.printf("ADC Value: %u\n", rawValue);
  Serial.printf("Volt Value: %u\n", voltValue);
  Serial.println("######################################");

  // 10 mS delay
  vTaskDelay( pdMS_TO_TICKS(1000) ); 

  // Feed Firulais!
  esp_task_wdt_reset();
}
