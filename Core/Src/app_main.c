#include "app_main.h"
#include <stdio.h>
#include <string.h> // Para strlen

extern UART_HandleTypeDef huart1;
uint32_t rxValue_aux, rxValue, txvalue; // txvalue: contador de solicitudes enviadas
uint8_t flagRx = 0, Rs485_Conn = 0;
uint32_t ConnTimeOut;
uint32_t Tick1000;
uint32_t LastRequestTime;
uint8_t currentSlaveID = 1;


void UART_SendString(UART_HandleTypeDef *huart, const char *str) {
    _rs485_set_mode(RS485_MODE_TRANSMIT);
    HAL_UART_Transmit(huart, (uint8_t*)str, strlen(str), 100);
    _rs485_set_mode(RS485_MODE_RECEIVE);
}

//uart callbacks
void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart) {
    ConnTimeOut = HAL_GetTick();
    flagRx = 1; //avisa que recibió
    HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_SET);
    rxValue = rxValue_aux;
    HAL_UART_Receive_IT(huart, (uint8_t*)&rxValue_aux, sizeof(uint32_t));
}

void HAL_UART_txCptlCallback(UART_HandleTypeDef *huart) {

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    HAL_UART_Receive_IT(huart, (uint8_t*)&rxValue_aux, sizeof(uint32_t));
}

/*RS485*/
void _rs485_set_mode(rs485_mode_e mode) {
    switch (mode) {
    case RS485_MODE_TRANSMIT:
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
        break;
    case RS485_MODE_RECEIVE:
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
        break;
    }
}

void _rs485_write32(uint32_t Value) {
    _rs485_set_mode(RS485_MODE_TRANSMIT);
    HAL_Delay(5);
    HAL_UART_Transmit(&huart1, (uint8_t*)&Value, sizeof(uint32_t), 20);
    _rs485_set_mode(RS485_MODE_RECEIVE);
}

void _rs485_init() {
    txvalue = 0;
    _rs485_set_mode(RS485_MODE_RECEIVE);
    HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxValue_aux, sizeof(uint32_t));
}

void app_main(void) {
    HAL_Delay(200);
    _rs485_init(); // Inicializa en modo RECEIVE

    UART_SendString(&huart1, "MAESTRO: Iniciando...\r\n");

    Tick1000 = HAL_GetTick(); // Para el temporizador de la propia app
    LastRequestTime = HAL_GetTick(); // Para controlar cuándo enviamos la última solicitud

    while (1) {
        // MAESTRO: Envía una solicitud cada dos segundos a partir de enviar la solicitud
        if ((HAL_GetTick() - LastRequestTime) > 2000) {

            _rs485_write32(currentSlaveID);
            UART_SendString(&huart1, "MAESTRO: Solicitud enviada a Esclavo ");
            char slaveID_str[10];
            sprintf(slaveID_str, "%u\r\n", currentSlaveID);
            UART_SendString(&huart1, slaveID_str);
            LastRequestTime = HAL_GetTick();
        }

        // MAESTRO: Procesar respuesta del esclavo
        if (flagRx == 1) {
            flagRx = 0;
            HAL_GPIO_WritePin(LED_RX_GPIO_Port, LED_RX_Pin, GPIO_PIN_RESET); // Apaga led de RX

            char rx_str[50];
            sprintf(rx_str, "MAESTRO: Dato recibido: %lu\r\n", rxValue);
            UART_SendString(&huart1, rx_str);

            if (Rs485_Conn == 0) {
                Rs485_Conn = 1;
            }
        }

        // Detectar si la comunicación cayó (timeout de recepción)
        if (Rs485_Conn == 1 && (HAL_GetTick() - ConnTimeOut) > 1500) {
            Rs485_Conn = 0;
            UART_SendString(&huart1, "MAESTRO: Comunicacion con Esclavo caida.\r\n");
        }
    }
}
