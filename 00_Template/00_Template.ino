/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "config.h"

/* Static Variables ----------------------------------------------------------*/

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

/* Setup Function -----------------------------------------------------------*/
void setup() 
{
    // Inicializar modulo Serial
    Serial.begin( SERIAL_BAUDRATE );
    vTaskDelay( pdMS_TO_TICKS(500) );
 
    // Configurar el watchdog para la tarea actual (loopTask)
    esp_task_wdt_init( &hwdt ); 
    esp_task_wdt_add( NULL );   


}

/* Loop Function ------------------------------------------------------------*/
void loop() 
{
  //TODO: Codigo de aplicacion va aqui

  // 10 mS delay
  vTaskDelay(pdMS_TO_TICKS(50)); 

  // Feed Firulais!
  esp_task_wdt_reset();
}
