#include "HM11.h"

void HM11::initialise(){
    #if HARDWARE_SERIAL
    /*Usew hardware serial*/
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    #else
    /*Use software serial*/
    #endif
}

void HM11::send_data(String data){
    #if HARDWARE_SERIAL
    /*Usew hardware serial*/
    uart_puts(UART_ID,data.c_str());
    #else
    /*Use software serial*/
    #endif
}
