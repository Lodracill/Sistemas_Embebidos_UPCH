/* Includes ------------------------------------------------------------------*/
#include "esp_task_wdt.h"
#include "driver/uart.h"

/* Defines -------------------------------------------------------------------*/

#define SERIAL_BAUDRATE 115200
#define WDT_TIMEOUT     (5*1000)

// UART PIN 
#if CONFIG_IDF_TARGET_ESP32S2
#define UART_ECHO_PORT      UART_NUM_1
#define UART_ECHO_TX_PIN    GPIO_NUM_10
#define UART_ECHO_RX_PIN    GPIO_NUM_9
#elif CONFIG_IDF_TARGET_ESP32
#define UART_ECHO_PORT      UART_NUM_2
#define UART_ECHO_TX_PIN    GPIO_NUM_17
#define UART_ECHO_RX_PIN    GPIO_NUM_16
#else
#error "Platform is no supported."
#endif
#define UART_ECHO_BAUDRATE  115200
#define UART_ECHO_BUF_SIZE  1024 // bytes


#define BUFF_SIZE UART_ECHO_BUF_SIZE

/* Static Variables ----------------------------------------------------------*/

static uint8_t data[BUFF_SIZE] = {0};
static uint32_t len = 0;

// Estructura para configurar el WDT
static esp_task_wdt_config_t hwdt = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = 1 << 0,
    .trigger_panic = true
};

// Estructura para configurar el UART
static uart_config_t uart_config = {
    .baud_rate = UART_ECHO_BAUDRATE,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

/* Setup Function -----------------------------------------------------------*/
void setup() 
{ 
    // Init serial module
    Serial.begin( SERIAL_BAUDRATE );
    vTaskDelay( pdMS_TO_TICKS(500) );
 
    // Configurar el watchdog para la tarea actual (loopTask)
    esp_task_wdt_init( &hwdt ); 
    esp_task_wdt_add( NULL );   

    // Habilitamos el UART
    // 1.- Se configura el UART utilizando los parametros de la estructura
    // 2.- Se indica los PINs que se van a utilizar (Depende del modulo que se este utilizando)
    // 3.- Se inicializa el driver del UART
    uart_param_config( UART_ECHO_PORT, &uart_config);
    uart_set_pin( UART_ECHO_PORT, UART_ECHO_TX_PIN, UART_ECHO_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );
    uart_driver_install( UART_ECHO_PORT, (UART_ECHO_BUF_SIZE * 2), 0, 0, NULL, 0 );

}

/* Loop Function ------------------------------------------------------------*/
void loop() 
{
    // Esperamos a recibir datos
    len = uart_read_bytes( UART_ECHO_PORT, data, BUFF_SIZE, pdMS_TO_TICKS(500));
    if (len > 0) 
    {    
        // Opcional: imprimir lo recibido por UART en consola principal
        data[len] = '\0';
        Serial.printf( "[UART ECHO]: Received String: %s", (char *)data );

        // ECHO: Enviamos los datos que recivimos de vuelta
        uart_write_bytes( UART_ECHO_PORT, data, len );
    }

    // 10 mS delay
    vTaskDelay(pdMS_TO_TICKS(50)); 

    // Feed Firulais!
    esp_task_wdt_reset();
}
