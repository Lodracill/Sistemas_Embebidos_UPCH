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

volatile uint32_t uTick = 0;
volatile uint32_t uCurrentTick = 0;

static hw_timer_t *htim = NULL;

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

/* Class ---------------------------------------------------------------------*/
Adafruit_SSD1306 Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);

/* Timer ISR Callback --------------------------------------------------------*/
void ARDUINO_ISR_ATTR TickIncrement(void) 
{
  uTick++;
}

/* Static Functions ----------------------------------------------------------*/
uint32_t TickGet(void)
{
  return uTick;
}

uint16_t adc_convert_bits_to_mvolts( uint16_t raw_value )
{
  return (raw_value * 3300)/(4096 - 1);
}

/* Setup Function -----------------------------------------------------------*/
void setup() 
{
  // Inicializar modulo Serial
  Serial.begin( SERIAL_BAUDRATE );
  vTaskDelay( pdMS_TO_TICKS(500) );
 
  // Inicializa el I2C
  Wire.begin();
  vTaskDelay( pdMS_TO_TICKS( 500 ) );

  // Configurar el watchdog para la tarea actual (loopTask)
  esp_task_wdt_init( &hwdt ); 
  esp_task_wdt_add( NULL );   

  // Configurar el TIMER a 1Mhz
  htim = timerBegin(1000000);
  if(htim == NULL)
  {
    Serial.println("Error to allocate TIMER.");
    while(1);
  }

  // Indica al TIMER cual sera la funcion callback al terminar el conteo
  timerAttachInterrupt(htim, &TickIncrement);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
  timerAlarm(htim, 1000, true, 0);

  // Configuramos el Canal 0 del ADC, el ancho de banda sera de 12 bits
  adc1_config_width( ADC_WIDTH_BIT_12 );
  adc1_config_channel_atten( ADC1_CHANNEL_0, ADC_ATTEN_DB_11 );

  // Configuramos el OLED
  Display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  Display.clearDisplay();
  Display.setTextColor(WHITE);
  Display.display();

  uCurrentTick = TickGet();

}

/* Loop Function ------------------------------------------------------------*/
void loop() 
{
  if( (TickGet() - uCurrentTick) > 5000) // Toma muestras cada 5 segundos
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

    uCurrentTick = TickGet();
  }

  // Feed Firulais!
  esp_task_wdt_reset();
  vTaskDelay( pdMS_TO_TICKS( 1000 ) );
}
