/*
 * Nombre: Blinking LED using registers
 * Descripcion: Utilizando la NUCLEO-L031K6 encenderemos el LED PB3 utilizando solo los registros
 *
 */

void setup() 
{
  // Habilitar el reloj del GPIOB
  // Es necesario habilitar el reloj del periferico, ya que si no este no funcionara correctamente.
  RCC->IOPENR |= (1 << 1);

  // La siguiente parte seccion sera configurar el puerto PB3
  // como salida digital utilizando los registros del MCU
  // En arduino, esto seria el equivalente a utilizar pinMode(13, OUTPUT)

  // Configurar el PB3 como salida
  // MODER REG: bit 7:0 Y bit 6:1
  GPIOB->MODER |= (1 << 6);
  GPIOB->MODER &= ~(1 << 7);

  // Tipo de salida
  // OTYPER REG: bit 3:1
  GPIOB->OTYPER &= ~(1 << 3);

  // Configurar la velocidad
  // OSPEEDR REG: bit 6:0 Y bit 7:0
  GPIOB->OSPEEDR &= ~(1 << 6);
  GPIOB->OSPEEDR &= ~(1 << 7);

  // No pull down/up
  // PUPDR REG: bit 6:0 Y bit 7:0
  GPIOB->PUPDR &= ~(1 << 6);
  GPIOB->PUPDR &= ~(1 << 7);

  // PB3 en estado bajo (LOW)
  // ODR REG: bit 3:0
  GPIOB->ODR &= ~(1 << 3);

}

void loop() 
{
  // Cambiar el estado del pin 
  GPIOB->ODR ^= (1 << 3);

  // Retraso de 1 segundo (1000 mS)
  delay(1000);
}
