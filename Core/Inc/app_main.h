// Declaraciones de funciones (prototipos) para el código proporcionado.
// Estas declaraciones típicamente irían en un archivo de cabecera, por ejemplo, "app_main.h"

#ifndef INC_APP_MAIN_H_
#define INC_APP_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // Puede ser necesario para HAL_UART_HandleTypeDef, etc.

// --- Callbacks de la UART ---
/**
  * @brief  Callback function for UART Receive Complete interrupt.
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains
  * the configuration information for the specified UART.
  * @retval None
  */
void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart);

/**
  * @brief  Callback function for UART Transmit Complete interrupt.
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains
  * the configuration information for the specified UART.
  * @retval None
  */
void HAL_UART_txCptlCallback(UART_HandleTypeDef *huart);

/**
  * @brief  Callback function for UART Error interrupt.
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains
  * the configuration information for the specified UART.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

// --- Funciones auxiliares de UART/RS-485 ---

// Definición del enum (esto también iría en el .h si se usa en otros archivos)
typedef enum {
	RS485_MODE_TRANSMIT,
	RS485_MODE_RECEIVE
} rs485_mode_e;

/**
  * @brief  Sets the operating mode of the RS485 transceiver (Transmit or Receive).
  * @param  mode: Desired RS485 mode (RS485_MODE_TRANSMIT or RS485_MODE_RECEIVE).
  * @retval None
  */
void _rs485_set_mode(rs485_mode_e mode);

/**
  * @brief  Transmits a 32-bit unsigned integer via RS485.
  * @param  Value: The 32-bit unsigned integer to be transmitted.
  * @retval None
  */
void _rs485_write32(uint32_t Value);

/**
  * @brief  Initializes the RS485 communication system.
  * @retval None
  */
void _rs485_init(void);

/**
  * @brief  Sends a null-terminated string via UART.
  * @param  huart: Pointer to a UART_HandleTypeDef structure.
  * @param  str: Pointer to the null-terminated string to send.
  * @retval None
  */
void UART_SendString(UART_HandleTypeDef *huart, const char *str);


// --- Función principal de la aplicación ---
/**
  * @brief  Main application logic.
  * @retval None
  */
void app_main(void);


#ifdef __cplusplus
}
#endif

#endif /* INC_APP_MAIN_H_ */
