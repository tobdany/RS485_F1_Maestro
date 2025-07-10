#include "app_main.h"
#include <stdio.h> // Necesario para sprintf si lo usas
#include <string.h> // Necesario para strlen

extern UART_HandleTypeDef huart1;
uint32_t rxValue_aux,rxValue,txvalue;
uint8_t flagRx=0, Rs485_Conn=0; //Rs_485_Conn se pone en 1 al recibir datos;
uint32_t ConnTimeOut;
uint32_t Tick1000;



// --- Nueva función auxiliar para enviar cadenas de texto ---
void UART_SendString(UART_HandleTypeDef *huart, const char *str) {
    // Asegurarse de que el modo RS485 esté en TRANSMIT antes de enviar
    _rs485_set_mode(RS485_MODE_TRANSMIT);
    // Enviar la cadena de texto
    HAL_UART_Transmit(huart, (uint8_t*)str, strlen(str), 100); // 100ms timeout
    // Volver al modo RECEIVE después de enviar
    _rs485_set_mode(RS485_MODE_RECEIVE);
}
// -----------------------------------------------------------

//uart callbacks
//se activa cuando ocurre la interrupción
void HAL_UART_RxCptlCallback(UART_HandleTypeDef *huart){
	ConnTimeOut=HAL_GetTick(); //lo usa para saber cuando fue la última recepción
	flagRx=1; //indica que se recibió el dato
	HAL_GPIO_WritePin(LED_RX_GPIO_Port,LED_RX_Pin,GPIO_PIN_SET); //indica que se está recibiendo datos
	rxValue=rxValue_aux;
	HAL_UART_Receive_IT(huart,(uint8_t*)&rxValue_aux,sizeof(uint32_t)); //vuelve a habilitar la recepción por interrupión

}

void HAL_UART_txCptlCallback(UART_HandleTypeDef *huart){

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(huart,(uint8_t*)&rxValue_aux,sizeof(uint32_t));
	//en caso de error vuelve a habilitar la recepción
}

/*RS485*/
void _rs485_set_mode(rs485_mode_e mode){
	//funcion que establece el modo de transmisión
	switch(mode){
	case RS485_MODE_TRANSMIT:
		// Se pone el pin DE en ALTO para transmitir
		HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin,GPIO_PIN_SET);
		break;
	case RS485_MODE_RECEIVE:
		// Se pone el pin DE en BAJO para recibir
		HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin,GPIO_PIN_RESET);
		break;
	}

}

void _rs485_write32(uint32_t Value){
	_rs485_set_mode(RS485_MODE_TRANSMIT);
	// Considera un pequeño delay aquí si tienes problemas de latencia al cambiar DE
	// HAL_Delay(1); // Descomentar si es necesario
	HAL_UART_Transmit(&huart1,(uint8_t*)&Value,sizeof(uint32_t),20);
	_rs485_set_mode(RS485_MODE_RECEIVE);
}

void _rs485_init(){
	txvalue=0;
	_rs485_set_mode(RS485_MODE_RECEIVE);
	HAL_UART_Receive_IT(&huart1,(uint8_t*)&rxValue_aux,sizeof(uint32_t));
}

void app_main(void)
{
	HAL_Delay(200); // Espera inicial
	_rs485_init(); // Inicializa el modo RS485 (se pone en RX) y UART RX IT

    // --- AGREGAR ESTE BLOQUE PARA EL MENSAJE DE CHECK ---
    UART_SendString(&huart1, "Terminal Check OK!\r\n");
    // También puedes enviar el txvalue inicial para depuración
    char buffer[50];
    sprintf(buffer, "Initial txvalue: %lu\r\n", txvalue);
    UART_SendString(&huart1, buffer);
    // ----------------------------------------------------

	Tick1000=HAL_GetTick();
	while(1){
		if((HAL_GetTick()-Tick1000)>1000){
			Tick1000=HAL_GetTick();
			txvalue++;
			_rs485_write32(txvalue);
		}
		if(Rs485_Conn==1 && (HAL_GetTick()-ConnTimeOut)>1500){
			//para detectar si la comunicación cayó
			//los datos se envian cada segundo
			//si desde la ultima recepción ha pasado más de un segundo
			//quiere decir que la comunicación cayó
			Rs485_Conn=0;
		}

		if(flagRx==1){
			flagRx=0;
			HAL_Delay(50); // Pequeño retardo, considera si es necesario

			HAL_GPIO_WritePin(LED_RX_GPIO_Port,LED_RX_Pin,GPIO_PIN_RESET);
			if(Rs485_Conn==0){
				Rs485_Conn=1;
			}
		}
	}
}
