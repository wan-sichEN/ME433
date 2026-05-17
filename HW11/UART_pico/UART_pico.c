#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 0 and 1 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// Global Variables
char buffer[1000] = {0};
int buf_index = 0;
//volatile bool message_ready = false;

void uart_rx_interrupt_handler() {
    while (uart_is_readable(UART_ID)) {
        char ch = uart_getc(UART_ID);
        if (ch == '\r' || ch == '\n') { // message is ready
            buffer[buf_index] = '\0';  // null terminate
            buf_index = 0;
            //message_ready = true;      // signal main loop
            printf("%s\n", buffer);
        } else {
            if (buf_index < 999) {
                buffer[buf_index++] = ch;
                //printf("%s", buffer[buf_index++]);
            }
        }
    }
}

int main()
{
    stdio_init_all();

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Set up RX interrupt
    irq_set_exclusive_handler(UART0_IRQ, uart_rx_interrupt_handler);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false); // RX interrupt on, TX off
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart
    
    while (true) { 
        // Send out a string, with CR/LF conversions
        char message[100];
        fgets(message, 100, stdin);
        uart_puts(UART_ID, message);
        printf("%s", message);
    }
}
