#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "string.h"

#define BUF_SIZE0 (1024)
#define BUF_SIZE1 (1024)


// font [caracter] [fila][columna]
static const char font[][5][6] = {
// MAYUSCULAS
    // A(0)
    {" *** ",
     "*   *",
     "*****",
     "*   *",
     "*   *"},
    // B(1)
    {"**** ",
     "*   *",
     "**** ",
     "*   *",
     "**** "},
    // C(2)
    {" ****",
     "*    ",
     "*    ",
     "*    ",
     " ****"},
    // D(3)
    {"**** ",
     "*   *",
     "*   *",
     "*   *",
     "**** "},
    // E(4)
    {"*****",
     "*    ",
     "***  ",
     "*    ",
     "*****"},
    // F(5)
    {"*****",
     "*    ",
     "***  ",
     "*    ",
     "*    "},
    // G(6)
    {" ****",
     "*    ",
     "*  **",
     "*   *",
     " ****"},
    // H(7)
    {"*   *",
     "*   *",
     "*****",
     "*   *",
     "*   *"},
    // I(8)
    {" *** ",
     "  *  ",
     "  *  ",
     "  *  ",
     " *** "},
    // J(9)
    {"  ***",
     "    *",
     "    *",
     "*   *",
     " *** "},
    // K(10)
    {"*   *",
     "*  * ",
     "***  ",
     "*  * ",
     "*   *"},
    // L(11)
    {"*    ",
     "*    ",
     "*    ",
     "*    ",
     "*****"},
    // M(12)
    {"*   *",
     "** **",
     "* * *",
     "*   *",
     "*   *"},
    // N(13)
    {"*   *",
     "**  *",
     "* * *",
     "*  **",
     "*   *"},
    // O(14)
    {" *** ",
     "*   *",
     "*   *",
     "*   *",
     " *** "},
    // P(15)
    {"**** ",
     "*   *",
     "**** ",
     "*    ",
     "*    "},
    // Q(16)
    {" *** ",
     "*   *",
     "*   *",
     "*  **",
     " ****"},
    // R(17)
    {"**** ",
     "*   *",
     "**** ",
     "*  * ",
     "*   *"},
    // S(18)
    {" ****",
     "*    ",
     " *** ",
     "    *",
     "**** "},
    // T(19)
    {"*****",
     "  *  ",
     "  *  ",
     "  *  ",
     "  *  "},
    // U(20)
    {"*   *",
     "*   *",
     "*   *",
     "*   *",
     " *** "},
    // V(21)
    {"*   *",
     "*   *",
     "*   *",
     " * * ",
     "  *  "},
    // W(22)
    {"*   *",
     "*   *",
     "* * *",
     "** **",
     "*   *"},
    // X(23)
    {"*   *",
     " * * ",
     "  *  ",
     " * * ",
     "*   *"},
    // Y(24)
    {"*   *",
     " * * ",
     "  *  ",
     "  *  ",
     "  *  "},
    // Z(25)
    {"*****",
     "   * ",
     "  *  ",
     " *   ",
     "*****"},

    // MINISCULAS

    // a(26)
    {"     ",
     " *** ",
     "*  * ",
     "* ** ",
     " ** *"},
    // b(27)
    {"*    ",
     "*    ",
     "**** ",
     "*   *",
     "**** "},
    // c(28)
    {"     ",
     " *** ",
     "*    ",
     "*    ",
     " *** "},
    // d(29)
    {"    *",
     "    *",
     " ****",
     "*   *",
     " ****"},
    // e(30)
    {"     ",
     " *** ",
     "*****",
     "*    ",
     " *** "},
    // f(31)
    {"  ** ",
     " *   ",
     "***  ",
     " *   ",
     " *   "},
    // g(32)
    {"     ",
     " ****",
     "*   *",
     " ****",
     " *** "},
    // h(33)
    {"*    ",
     "*    ",
     "**** ",
     "*   *",
     "*   *"},
    // i(34)
    {"  *  ",
     "     ",
     " **  ",
     "  *  ",
     " *** "},
    // j(35)
    {"   * ",
     "     ",
     "   * ",
     "   * ",
     " **  "},
    // k(36)
    {"*    ",
     "*  * ",
     "***  ",
     "*  * ",
     "*   *"},
    // l(37)
    {" **  ",
     "  *  ",
     "  *  ",
     "  *  ",
     " *** "},
    // m(38)
    {"     ",
     "** * ",
     "* * *",
     "*   *",
     "*   *"},
    // n(39)
    {"     ",
     "**** ",
     "*   *",
     "*   *",
     "*   *"},
    // o(40)
    {"     ",
     " *** ",
     "*   *",
     "*   *",
     " *** "},
    // p(41)
    {"     ",
     "**** ",
     "*   *",
     "**** ",
     "*    "},
    // q(42)
    {"     ",
     " ****",
     "*   *",
     " ****",
     "    *"},
    // r(43)
    {"     ",
     " *** ",
     "*    ",
     "*    ",
     "*    "},
    // s(44)
    {"     ",
     " *** ",
     " **  ",
     "  ** ",
     " *** "},
    // t(45)
    {" *   ",
     "***  ",
     " *   ",
     " *   ",
     "  ** "},
    // u(46)
    {"     ",
     "*   *",
     "*   *",
     "*   *",
     " ****"},
    // v(47)
    {"     ",
     "*   *",
     "*   *",
     " * * ",
     "  *  "},
    // w(48)
    {"     ",
     "*   *",
     "* * *",
     "* * *",
     " * * "},
    // x(49)
    {"     ",
     "*   *",
     " * * ",
     " * * ",
     "*   *"},
    // y(50)
    {"     ",
     "*   *",
     "*   *",
     " ****",
     "    *"},
    // z(51)
    {"     ",
     "*****",
     "  *  ",
     " *   ",
     "*****"},

    //NUMEROS

    // 0(52)
    {" *** ",
     "*  **",
     "* * *",
     "**  *",
     " *** "},
    // 1(53)
    {"  *  ",
     " **  ",
     "  *  ",
     "  *  ",
     " *** "},
    // 2(54)
    {" *** ",
     "*   *",
     "  ** ",
     " *   ",
     "*****"},
    // 3(55)
    {" *** ",
     "    *",
     "  ** ",
     "    *",
     " *** "},
    // 4(56)
    {"*  * ",
     "*  * ",
     "*****",
     "   * ",
     "   * "},
    // 5(57)
    {"*****",
     "*    ",
     "**** ",
     "    *",
     "**** "},
    // 6(58)
    {" *** ",
     "*    ",
     "**** ",
     "*   *",
     " *** "},
    // 7(59)
    {"*****",
     "    *",
     "   * ",
     "  *  ",
     "  *  "},
    // 8(60)
    {" *** ",
     "*   *",
     " *** ",
     "*   *",
     " *** "},
    // 9(61)
    {" *** ",
     "*   *",
     " ****",
     "    *",
     " *** "},

    //CARACTERES ESPECIALES

    // ESpacio (62)
    {"     ",
     "     ",
     "     ",
     "     ",
     "     "},
    // !(63)
    {"  *  ",
     "  *  ",
     "  *  ",
     "     ",
     "  *  "},
    // .(64)
    {"     ",
     "     ",
     "     ",
     "     ",
     "  *  "},
    // +(65)
    {"     ",
     "  *  ",
     "*****",
     "  *  ",
     "     "},
    // -(66)
    {"     ",
     "     ",
     "*****",
     "     ",
     "     "},
};

// Retorna el indice en font[] para el caracter dado, o -1 si no es válido.
int id_caracter(char c){
    if(c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if(c >= 'a' && c <= 'z') {
        return 26 + (c - 'a');
    }
    if(c >= '0' && c <= '9') {
        return 52 + (c - '0');
    }
    if(c == ' ') {
        return 62;
    }
    if(c == '!') {
        return 63;
    }
    if(c == '.') {
        return 64;
    }
    if(c == '+') {
        return 65;
    }
    if(c == '-') {
        return 66;
    }
    return -1;
}

void putchar_uart(uart_port_t uart_num, char c){
    uart_write_bytes(uart_num, &c, 1);
}

void puts_uart(uart_port_t uart_num, char *str){
    uart_write_bytes(uart_num, str, strlen(str));
}

void imprimir_pancarta(uart_port_t uart_num, const char *str){
    int len = strlen(str);
    puts_uart(uart_num, "\r\n");
    for(int fila = 0; fila < 5; fila++){
        for(int i = 0; i < len; i++){
            int idx = id_caracter(str[i]);
            if(idx >= 0){
                puts_uart(uart_num, (char *)font[idx][fila]);
                putchar_uart(uart_num, ' ');
            }
        }
        puts_uart(uart_num, "\r\n");
    }
    puts_uart(uart_num, "\r\n");
}

void init_uart0(void){
    uart_config_t cfg = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM_0, BUF_SIZE0 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &cfg);
    uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void init_uart2(void){
    uart_config_t cfg = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM_2, BUF_SIZE1 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &cfg);
    // RX2 = GPIO16, TX2 = GPIO17 (TX2 no se usa pero se declara)
    uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void receptor_task(void *arg){
    char buffer[26];
    int idx;
    uint8_t byte;

    while(1){
        idx = 0;
        memset(buffer, 0, sizeof(buffer));

        // Recibir bytes por UART2 (GPIO16), timeout 100ms = fin de trama
        while(idx < 25){
            int len = uart_read_bytes(UART_NUM_2, &byte, 1, pdMS_TO_TICKS(100));
            if(len <= 0) break;
            buffer[idx++] = (char)byte;
        }
        buffer[idx] = '\0';

        if(idx > 0){
            // Mostrar en monitor serie del receptor (UART0)
            puts_uart(UART_NUM_0, "\r\nPancarta recibida:\r\n");
            imprimir_pancarta(UART_NUM_0, buffer);
        }
    }
}

void app_main(void){
    init_uart0();
    init_uart2();
    xTaskCreate(receptor_task, "receptor_task", 8192, NULL, 10, NULL);
}