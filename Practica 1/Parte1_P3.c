    //Leonardo Avalos Godoy
    //Jose Raziel Trujeque Castor
    #include <stdio.h>
    #include <string.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/uart.h"
    #include "driver/gpio.h"
    #include "sdkconfig.h"
    #include "esp_log.h"

    #define ECHO_TEST_TXD (UART_PIN_NO_CHANGE)
    #define ECHO_TEST_RXD (UART_PIN_NO_CHANGE)
    #define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
    #define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

    #define ECHO_UART_PORT_NUM      (UART_NUM_0)
    #define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
    #define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

    #define BUF_SIZE (1024)

    void init_uart(void){
        /* Configure parameters of an UART driver,
        * communication pins and install the driver */
        uart_config_t uart_config = {
            .baud_rate = ECHO_UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };
        int intr_alloc_flags = 0;

    #if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif

        ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
        ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
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

    //Recibe una cadena de caracteres del puerto UART indicado y la almacena en str. 
    //La longitud máxima es max_len incluyendo el fin de cadena.
    void uart_gets(uart_port_t uart_num, char *str, uint8_t max_len){
        int i = 0;
        char c;
        while (i < max_len - 1) {
            c = uart_getchar(uart_num);
            if (c == '\n' || c == '\r') {
                break;
            }
            putchar_uart(uart_num, c);
            str[i] = c;
            i++;
        }
        str[i] = '\0';
    }

    // Recibe una cadena de caracteres validada a solo caracteres numéricos, 
    // del puerto UART indicado y la almacena en str. 
    // La longitud máxima es max_len incluyendo el fin de cadena.
    void uart_getNum(uart_port_t uart_num, char *str, uint8_t max_len){
        int i = 0;
        char c;
        while (i < max_len - 1) {
            c = uart_getchar(uart_num);
            if (c == '\n' || c == '\r') {
                break;
            }
            if (c >= '0' && c <= '9') {
                putchar_uart(uart_num, c);
                str[i] = c;
                i++;
            }
        }
        str[i] = '\0';
    }

    //Recibe una cadena de caracteres validada a solo letras, 
    //del puerto UART indicado y la almacena en str. 
    //La longitud máxima es max_len incluyendo el fin de cadena.
    void uart_getAlpha(uart_port_t uart_num, char *str, uint8_t max_len){
        int i = 0;
        char c;
        while (i < max_len - 1) {
            c = uart_getchar(uart_num);
            if (c == '\n' || c == '\r') {
                break;
            }
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                putchar_uart(uart_num, c);
                str[i] = c;
                i++;
            }
        }
        str[i] = '\0';
    }

    static void echo_task(void *arg)
    {
        char buffer[32];

        while(1){ 
        puts_uart(ECHO_UART_PORT_NUM, "\r\nIngresa texto: ");
        uart_gets(ECHO_UART_PORT_NUM, buffer, sizeof(buffer));
        puts_uart(ECHO_UART_PORT_NUM, "\r\nRecibido: ");
        puts_uart(ECHO_UART_PORT_NUM, buffer);

        puts_uart(ECHO_UART_PORT_NUM, "\r\nIngresa numero: ");
        uart_getNum(ECHO_UART_PORT_NUM, buffer, sizeof(buffer));
        puts_uart(ECHO_UART_PORT_NUM, "\r\nRecibido: ");
        puts_uart(ECHO_UART_PORT_NUM, buffer);

        puts_uart(ECHO_UART_PORT_NUM, "\r\nIngresa letras: ");
        uart_getAlpha(ECHO_UART_PORT_NUM, buffer, sizeof(buffer));
        puts_uart(ECHO_UART_PORT_NUM, "\r\nRecibido: ");
        puts_uart(ECHO_UART_PORT_NUM, buffer);

        puts_uart(ECHO_UART_PORT_NUM, "\r\nIngresa un caracter: ");
        char c = uart_getchar(ECHO_UART_PORT_NUM);
        puts_uart(ECHO_UART_PORT_NUM, "\r\nRecibido: ");
        putchar_uart(ECHO_UART_PORT_NUM, c);

        vTaskDelay(1000 / portTICK_PERIOD_MS); }
    }

    void app_main(void)
    {
        init_uart();
        xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
    }