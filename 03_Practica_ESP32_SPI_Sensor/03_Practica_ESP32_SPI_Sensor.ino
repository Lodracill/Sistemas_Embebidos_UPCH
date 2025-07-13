/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "config.h"

#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/* Defines -------------------------------------------------------------------*/

#define BME_SCK   18  // Se conecta al PIN 18 (Esta configurado por default)
#define BME_MISO  19  // Se conecta al PIN 19 (Esta configurado por default)
#define BME_MOSI  23  // Se conecta al PIN 23 (Esta configurado por default)
#define BME_CS    5   // Se conecta al PIN 5 (Se utiliza cuando se crea el objeto, como la linea 32)

#define SEALEVELPRESSURE_HPA (1013.25)

/* Static Variables ----------------------------------------------------------*/

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

static float Temperature = 0.0;
static float Humidity = 0.0;
static float Preasure = 0.0;

/* Classes .......-----------------------------------------------------------*/
Adafruit_BME280 Sensor(BME_CS); 

/* Setup Function -----------------------------------------------------------*/
void setup() 
{
  // Inicializar modulo Serial
  Serial.begin( SERIAL_BAUDRATE );
  vTaskDelay( pdMS_TO_TICKS(500) );
 
  // Configurar el watchdog para la tarea actual (loopTask)
  esp_task_wdt_init( &hwdt ); 
  esp_task_wdt_add( NULL );   

  // Inicializa el sensor
  if (!Sensor.begin()) 
  {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }
  vTaskDelay( pdMS_TO_TICKS(200) );

}

/* Loop Function ------------------------------------------------------------*/
void loop() 
{
  // Obtener la temperatura
  Temperature = Sensor.readTemperature();
  vTaskDelay( pdMS_TO_TICKS(200) );

  // Obtener la humedad
  Humidity = Sensor.readHumidity();
  vTaskDelay( pdMS_TO_TICKS(200) );

  // Obtener la presion atmosferica
  Preasure = Sensor.readPressure();
  vTaskDelay( pdMS_TO_TICKS(200) );

  Serial.printf("Temperatura: %.2f C\n", Temperature);
  Serial.printf("Humedad: %.2f RH%\n", Humidity);
  Serial.printf("Presion atmosferica: %.2f\n\n", Preasure);

  // 2000 mS delay
  vTaskDelay( pdMS_TO_TICKS(2000) ); 

  // Feed Firulais!
  esp_task_wdt_reset();
}
