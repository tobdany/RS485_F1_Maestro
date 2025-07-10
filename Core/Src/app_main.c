#include "app_main.h"
#include <stdio.h>  // Para sprintf (aunque no lo usamos directamente en este "Hola Mundo")
#include <string.h> // Para strlen

// Declaración externa del manejador de la UART.
// 'huart1' se inicializa en main.c (generado por STM32CubeMX)
extern UART_HandleTypeDef huart1;

// --- Función auxiliar para enviar cadenas de texto vía UART ---
// (Puedes mover esto a un archivo de utilidades si tu proyecto crece)
void UART_SendString(UART_HandleTypeDef *huart, const char *str) {
    // Transmite la cadena de caracteres
    // strlen(str) obtiene la longitud de la cadena
    // 100 es un timeout en ms para la transmisión bloqueante
    HAL_UART_Transmit(huart, (uint8_t*)str, strlen(str), 100);
}

// --- UART Callbacks (mantenemos las funciones vacías por completitud, no son necesarias para este "Hola Mundo" TX) ---
// Puedes eliminarlas si no vas a usarlas y no te dan warnings/errores
void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart) {
    // No usada en este ejemplo de solo transmisión
}

void HAL_UART_txCptlCallback(UART_HandleTypeDef *huart) {
    // No usada en este ejemplo de solo transmisión
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    // No usada en este ejemplo
}

// --- Función principal de la aplicación ---
void app_main(void)
{
    // Pequeño retardo al inicio para asegurar que todo el sistema esté estable
    HAL_Delay(500);

    // Bucle infinito donde se ejecuta la lógica de la aplicación
    while(1)
    {
        // Envía el mensaje "Hola Mundo!" seguido de un retorno de carro y nueva línea
        UART_SendString(&huart1, "Hola Mundo!\r\n");

        // Espera 1 segundo antes de enviar el siguiente mensaje
        HAL_Delay(1000);
    }
}
