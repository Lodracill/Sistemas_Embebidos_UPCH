/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "config.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "driver/adc.h"

/* Static Variables ----------------------------------------------------------*/

// ADC Values
static uint16_t rawValue = 0;
static uint16_t voltValue = 0;

// WiFi credenciales
const char SSID[64] = "TU_SSID_AQUI";
const char PSWD[64] = "TU_PASWORD_AQUI";

// ThingSpeak WRITE API Key and network
String ServerLink = "https://api.thingspeak.com/update";
String apiKey = "TU_API_KEY_AQUI";

// Timer variables
volatile uint32_t uTick = 0;
volatile uint32_t uCurrentTick = 0;
static hw_timer_t *htim = NULL;

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

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

  // Conectarse al Router
  WiFi.begin(SSID, PSWD);
  Serial.print("Conectando al WiFi");

  // Configuramos el Canal 0 del ADC, el ancho de banda sera de 12 bits
  adc1_config_width( ADC_WIDTH_BIT_12 );
  adc1_config_channel_atten( ADC1_CHANNEL_0, ADC_ATTEN_DB_11 );

  // Esperar a la conexion
  while(WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay( pdMS_TO_TICKS(500) );
    Serial.print(".");
  }

  // Transmitir conexion exitosa
  Serial.println("\nConexion Exitosa!");

  // Get current tick
  uCurrentTick = TickGet();
}

/* Loop Function ------------------------------------------------------------*/
void loop() 
{
  // La rutina se ejecutara cada 20 segundos
  if( (TickGet() - uCurrentTick) > GET_TIME)
  {
    // Obtener el valor del ADC1 CH0
    rawValue = adc1_get_raw( ADC1_CHANNEL_0 );
  
    // Convertir el valor del ADC a mV
    voltValue = adc_convert_bits_to_mvolts( rawValue );

    Serial.println( "[LOG]: rawValue: " + String( rawValue ) + " voltValue: " + String( voltValue ) );

    if(WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;
      String url = ServerLink + "?api_key=" + apiKey + "&field1=" + String( voltValue );

      http.begin(url);
      int httpCode = http.GET();
      if(httpCode > 0)
      {
        Serial.println( "[LOG]: Datos enviados con exito" );
      }
      else
      {
        Serial.println( "[LOG]: Error al enviar" );
      }

      http.end();
    }

    // Get current tick
    uCurrentTick = TickGet();
  }

  // 10 mS delay
  vTaskDelay(pdMS_TO_TICKS(50)); 

  // Feed Firulais!
  esp_task_wdt_reset();
}
