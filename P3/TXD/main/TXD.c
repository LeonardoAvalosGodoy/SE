#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "string.h"

#define BUF_SIZE0 (1024)
#define BUF_SIZE1 (1024)

void init_uart0(void){
        /* Configure parameters of an UART driver,
        * communication pins and install the driver */
        uart_config_t uart_config0 = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };

        uart_driver_install(UART_NUM_0, BUF_SIZE0 * 2, 0, 0, NULL, 0);
        uart_param_config(UART_NUM_0, &uart_config0);
        uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void init_uart1(void){
        /* Configure parameters of an UART driver,
        * communication pins and install the driver */
        uart_config_t uart_config1 = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };

        uart_driver_install(UART_NUM_1, BUF_SIZE1 * 2, 0, 0, NULL, 0);
        uart_param_config(UART_NUM_1, &uart_config1);
        // TX1 = GPIO4, RX1 = GPIO5 (RX1 no se usa pero se declara)
        uart_set_pin(UART_NUM_1, 4, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

//Recibe un caracter del puerto UART indicado y lo retorna.
char uart_getchar(uart_port_t uart_num){
    uint8_t data;
    int len = uart_read_bytes(uart_num, &data, 1, portMAX_DELAY);
    if(len > 0){
        return (char)data;
    }
    return 0;
}

//Escribe un caracter en el puerto UART indicado.
void putchar_uart(uart_port_t uart_num, char c){
    uart_write_bytes(uart_num, &c, 1);
}

//Escribe una cadena de caracteres en el puerto UART indicado.
void puts_uart(uart_port_t uart_num, char * str){
    uart_write_bytes(uart_num, str, strlen(str));
}

//Recibir Letras, Numeros,  (! .  +  -)
void uart_getPancarta(uart_port_t uart_num, char *str, uint8_t max_len){
    int i = 0;
    char c;
    while(i < max_len - 1){
        c = uart_getchar(uart_num);

        //terminar con enter
        if(c == '\r' || c == '\n'){
            break;
        }
        
        //Backspace o Delete(suprimir)
        //8 = Backspace y 127 = DEL
        if(c == 8 || c == 127){
            if ( i > 0){ //Si hay algo escrito
                i--; //Regresamos
                puts_uart(uart_num, "\b \b"); //borrar con "\b"
            }
            continue;
        }
        

        //Caracteres permitidos
        if ((c >= 'A' && c <='Z') //Permitir mayusculas
            || (c >= 'a' && c <= 'z') //permitir minusculas
            || (c >= '0' && c <= '9') //permitir numero
            || (c == ' ') //permitir espacio
            || (c == '!')
            || (c == '.')
            || (c == '+')
            || (c == '-'))
            {
                putchar_uart(uart_num, c); //eco visual
                str[i] = c;
                i++;
            }
    }
    //fin de cadena
    str[i] = '\0';
}

void emisor_task(void *arg){
    char buffer[26]; //25 + nulo

    while(1){
        puts_uart(UART_NUM_0, "\nIngrese un texto maximo de 25 caracteres: ");

        //obtener el texto para la pancarta
        uart_getPancarta(UART_NUM_0, buffer, sizeof(buffer));

        //se envia la cadena al ESP 2
        uart_write_bytes(UART_NUM_1, buffer, strlen(buffer));

        puts_uart(UART_NUM_0, "\nEnviando al ESP 2...\n");
    }
}


void app_main(void){
    init_uart0();
    init_uart1();
    xTaskCreate(emisor_task, "emisor_task", 2048, NULL, 10, NULL);
}