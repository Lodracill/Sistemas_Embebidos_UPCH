/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "config.h"

#include <ArduinoJson.h>

/* Static Variables ----------------------------------------------------------*/

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

/* Constant Variables -------------------------------------------------------*/

// WiFi Credentials
// Modificalo de acuerdo a los datos de tu Router [NOTA: ANTES DE COMPARTIR, ELIMINA LAS CREDENCIALES]
const char SSID[WIFI_CREDENTIALS_MAX_LEN] = "TU_SSID_AQUI";
const char PSWD[WIFI_CREDENTIALS_MAX_LEN] = "TU_PASWORD_AQUI";

void setup() 
{
  // Inicializar modulo Serial
  Serial.begin( SERIAL_BAUDRATE );
  vTaskDelay( pdMS_TO_TICKS(500) );
 
  // Configurar el watchdog para la tarea actual (loopTask)
  esp_task_wdt_init( &hwdt ); 
  esp_task_wdt_add( NULL );   

  // Inicializamos el WiFi y conectamos al router que le indiquemos
  WiFi.begin(SSID, PSWD);
  Serial.print("Conectando al WiFi");

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

  // Esperamos a que se conecte a la red WiFi
  while (WiFi.status() != WL_CONNECTED) 
  {
    vTaskDelay( pdMS_TO_TICKS(500) );
    Serial.print(".");

    //Feed Firulais
    esp_task_wdt_reset();  

    //TODO: Agregar un TimeOut
  }

  Serial.println("\nConexion Exitosa!");
  
  uCurrentTick = TickGet();

}

void loop() 
{

  // La rutina se ejecutara cada 20 segundos
  if( (TickGet() - uCurrentTick) > GET_TIME)
  {
    if (WiFi.status() == WL_CONNECTED) 
    {
      // Creamos el objeto http (HTTPClient)
      HTTPClient http;

      // Usa CoinGecko API (gratuita, sin clave)
      String url = "https://api.coingecko.com/api/v3/simple/price?ids=solana,bitcoin,ethereum&vs_currencies=usd,mxn";

      http.begin(url);
      int httpCode = http.GET();

      if (httpCode == 200) 
      {
        String payload = http.getString();
        const size_t capacity = JSON_OBJECT_SIZE(3) + 3 * JSON_OBJECT_SIZE(2) + 100;
        DynamicJsonDocument doc(capacity);

        DeserializationError error = deserializeJson(doc, payload);
        if (error) 
        {
          Serial.print("Error al parsear JSON: ");
          Serial.println(error.c_str());
        }
        else
        {
          //Extrae los datos del archivo JSON
          float sol_usd = doc["solana"]["usd"];
          float sol_mxn = doc["solana"]["mxn"];
          float btc_usd = doc["bitcoin"]["usd"];
          float btc_mxn = doc["bitcoin"]["mxn"];
          float eth_usd = doc["ethereum"]["usd"];
          float eth_mxn = doc["ethereum"]["mxn"];

          // Imprime por consola los precios
          Serial.println();
          Serial.println("Precios actuales:");
          Serial.printf("Solana:   $%.2f USD | $%.2f MXN\n", sol_usd, sol_mxn);
          Serial.printf("Bitcoin:  $%.2f USD | $%.2f MXN\n", btc_usd, btc_mxn);
          Serial.printf("Ethereum: $%.2f USD | $%.2f MXN\n", eth_usd, eth_mxn);
        }
      } 
      else 
      {
        Serial.print("Error en la petición HTTP: ");
        Serial.println(http.errorToString(httpCode).c_str());
      }

      http.end();
    }

    uCurrentTick = TickGet();
  }

  // 10 mS delay
  vTaskDelay(pdMS_TO_TICKS(50)); 

  // Feed Firulais!
  esp_task_wdt_reset();  
}