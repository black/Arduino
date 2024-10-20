#ifndef __HM11__
#define __HM11__


/*If 1, hardware serial will be used*/
#define HARDWARE_SERIAL 1

/*UART parameters*/
#define UART_ID uart0 //UART port
#define BAUD_RATE 115200 //UART baudrate

#define UART_TX_PIN 16 //UART TX pin
#define UART_RX_PIN 17 //UART RX pin

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <Arduino.h>

class HM11{
    public:
    void initialise();
    void send_data(String data);
};

#endif
