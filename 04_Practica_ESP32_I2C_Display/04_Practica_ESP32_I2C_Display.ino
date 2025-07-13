/* Includes ------------------------------------------------------------------*/
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "esp_task_wdt.h"

/* Defines -------------------------------------------------------------------*/
#define SERIAL_BAUDRATE 115200
#define WDT_TIMEOUT     (5*1000)

#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64
#define DISPLAY_RESET   -1

/* Static Variables ----------------------------------------------------------*/

// Contador
static uint32_t counter = 0;

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

/* Class ---------------------------------------------------------------------*/
Adafruit_SSD1306 Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);

/* setup function ------------------------------------------------------------*/
void setup() 
{
  // Inicializa el monitor serial
  Serial.begin( SERIAL_BAUDRATE );
  vTaskDelay( pdMS_TO_TICKS( 500 ) );
 
  // Inicializa el I2C
  Wire.begin();
  vTaskDelay( pdMS_TO_TICKS( 500 ) );

  // Configurar el watchdog para la tarea actual (loopTask)
  esp_task_wdt_init( &hwdt ); 
  esp_task_wdt_add( NULL ); 

  // Inicializamos el driver de la pantalla OLED
  Display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  Display.clearDisplay();
  Display.setTextColor(WHITE);
  Display.display();

}

/* loop function -------------------------------------------------------------*/
void loop() 
{
  // Mostrar el conteo
  Display.clearDisplay();
  Display.setCursor( 0, 0 );
  Display.printf("Contador: %u", counter);

  // Aumentar el contador
  counter++;

  // Feed Firulais!
  esp_task_wdt_reset();
  vTaskDelay( pdMS_TO_TICKS( 1000 ) );

}
