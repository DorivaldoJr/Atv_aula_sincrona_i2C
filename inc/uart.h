#ifndef UART_H
#define UART_H

#include "hardware/uart.h"

// UART defines
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

void uart_init_custom();

#endif // UART_H