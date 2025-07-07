# Practica 02 - ECHO utilizando UART
## Objetivo

Utilizando el UART del ESP32, se diseñara un ECHO (Devuelve el mismo mensaje que se e le envia).

## Flujo de trabajo

1. Configurar el UART (UART2) con los siguientes parametros.
    1. Baudrate: 115200
    2. Data Length: 8 bits
    3. Parity Bit: None
    4. Stop Bits: One bit
    5. HW Control Flow: Disabled
2. Configurar los GPIO para ser utilizados como TX y RX del UART
3. Inicializar el driver del UART
4. Esperar a recibir datos de la PC.
5. Al recibir datos de la PC, se devolveran exactamente como llegaron.

## Contenido

El repositorio cuenta con el archivo .ino (Arduino) para poder compilar y cargar el binario a la tarjeta de desarrollo.

## Referencias

- [ESP-IDF Programming Guide - UART](https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s2/api-reference/peripherals/uart.html)
- [UART Reference from Analog Devices](https://www.analog.com/en/resources/analog-dialogue/articles/uart-a-hardware-communication-protocol.html)
